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

volatile time32_t t = 0;
time32_t buzz_duration = 0;
Uint8 nb_buzz_remaining = 0;
BUZZER_note_e current_note = DEFAULT_NOTE;

void BUZZER_process_1ms(void)
{
	if(t)
		t--;
}

//exemple d'utilisation pour 5 bips de 50ms : BUZZER_play(50, NOTE_DO, 5);

//Demander un buzz pour une durée donnée en ms
void BUZZER_play(time32_t duration_ms, BUZZER_note_e note, Uint8 nb_buzz)
{
	buzz_duration = duration_ms;
	nb_buzz_remaining = nb_buzz;
	if(current_note != note)
	{
		current_note = note;
//		PWM_set_frequency(current_note);
	}
}

void BUZZER_process_main(void)
{
	typedef enum
	{
		INIT = 0,
		IDLE,
		BUZZ,
		WAIT
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance = (state!=previous_state)?TRUE:FALSE;
	previous_state = state;
	switch(state)
	{
		case INIT:
			PWM_init();	//On s'assure que l'init du module PWM est faite..
//			PWM_set_frequency(current_note);	//1kHz
			t = 0;
			nb_buzz_remaining = 0;
			state = IDLE;
			break;
		case IDLE:
			if(nb_buzz_remaining)
				state = BUZZ;
				//On ne descend pas le flag... pour que l'état BUZZ déclenche l'allumage
			break;
		case BUZZ:
			if(entrance)
			{
				PWM_run(50,4);
				t = buzz_duration;
				nb_buzz_remaining--;
			}
			if(!t)
			{
				//Stop buzz
				PWM_stop(4);
				if(nb_buzz_remaining)
					state = WAIT;
				else
					state = IDLE;
			}
			break;
		case WAIT:
			if(entrance)
				t = buzz_duration;
			if(!t)
				state = BUZZ;
			break;
		default:
			break;
	}
}


