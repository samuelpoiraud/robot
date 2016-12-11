/*
 *	Club Robot ESEO 2013 - 2015
 *	Pierre & Guy, Holly & Wood
 *
 *	Fichier : Buzzer.c
 *	Package : Supervision
 *	Description : Gestion du buzzer
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201401
 */
#include "Buzzer.h"
#include "../QS/QS_pwm.h"

#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_BUZZER
#define LOG_PREFIX "Buzzer : "
#include "../QS/QS_outputlog.h"

volatile time32_t t = 0;
time32_t buzz_duration = 0;
Uint8 nb_buzz_remaining = 0;
BUZZER_note_e current_note = DEFAULT_NOTE;

void BUZZER_processIt(Uint8 ms) {

	if(t) {
		t--;
	}
}

//exemple d'utilisation pour 5 bips de 50ms : BUZZER_play(50, NOTE_DO, 5);

//Demander un buzz pour une durée donnée en ms
void BUZZER_play_fct_dont_use(time32_t duration_ms, BUZZER_note_e note, Uint8 nb_buzz, const char* fonction, Uint16 ligne)
{
	debug_printf("request fct:%s line:%d\n", fonction, ligne);

	buzz_duration = duration_ms;
	nb_buzz_remaining = nb_buzz;
	if(current_note != note)
	{
		current_note = note;
		PWM_set_frequency(current_note);
	}
}

void BUZZER_processMain(void)
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
			PWM_set_frequency(current_note);	//1kHz
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

static BUZZER_note_e BUZZER_convertEnumQS(buzzer_play_note_e val) {
	switch(val){
		case BUZZER_DEFAULT_NOTE :
			return DEFAULT_NOTE;

		case BUZZER_NOTE_DO0:
			return NOTE_DO0;

		case BUZZER_NOTE_RE0:
			return NOTE_RE0;

		case BUZZER_NOTE_MI0:
			return NOTE_MI0;

		case BUZZER_NOTE_FA:
			return NOTE_FA;

		case BUZZER_NOTE_SOL:
			return NOTE_SOL;

		case BUZZER_NOTE_LA:
			return NOTE_LA;

		case BUZZER_NOTE_SI:
			return NOTE_SI;

		case BUZZER_NOTE_DO:
			return NOTE_DO;

		case BUZZER_NOTE_RE:
			return NOTE_RE;

		case BUZZER_NOTE_MI:
			return NOTE_MI;

		default:
			return DEFAULT_NOTE;
	}
}
