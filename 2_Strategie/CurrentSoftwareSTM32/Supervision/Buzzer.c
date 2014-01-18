/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : Buzzer.c
 *	Package : Supervision
 *	Description : Gestion du buzzer
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201401
 */

#include "Buzzer.h"

	#include "../QS/QS_outputlog.h"
#include "../QS/QS_pwm.h"
volatile time32_t buzz_duration = 0;
bool_e b_ask_buzz = FALSE;
BUZZER_note_e current_note = DEFAULT_NOTE;

void BUZZER_process_1ms(void)
{
	if(buzz_duration)
		buzz_duration--;
}


//Demander un buzz pour une durée donnée en ms
void BUZZER_play(time32_t duration_ms, BUZZER_note_e note)
{
	b_ask_buzz = TRUE;
	buzz_duration = duration_ms;
	current_note = note;
}

void BUZZER_process_main(void)
{
	typedef enum
	{
		INIT = 0,
		IDLE,
		BUZZ
	}state_e;
	static state_e state = INIT;
	switch(state)
	{
		case INIT:
			PWM_init();	//On s'assure que l'init du module PWM est faite..
			buzz_duration = 0;
			b_ask_buzz = 0;
			state = IDLE;
			break;
		case IDLE:
			if(b_ask_buzz)
				state = BUZZ;
				//On ne descend pas le flag... pour que l'état BUZZ déclenche l'allumage
			break;
		case BUZZ:
			if(b_ask_buzz)	//Cela permet d'autoriser le changement de fréquence pendant un buzz
			{
				//Run buzz(freq);
				b_ask_buzz = FALSE;
			}
			if(!buzz_duration)
			{
				//Stop buzz
				state = IDLE;
			}
			break;
		default:
			break;
	}
}
