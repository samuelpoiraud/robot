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

#define BUZZER_RUN()			GPIO_SetBits(BUZZER)
#define BUZZER_STOP()			GPIO_ResetBits(BUZZER)

volatile time32_t t = 0;
time32_t buzz_duration = 0;
Uint8 nb_buzz_remaining = 0;

void BUZZER_processIt(Uint8 ms) {

	if(t) {
		t--;
	}
}

void BUZZER_play(time32_t duration, Uint8 nbBuzz) {
	buzz_duration = duration;
	nb_buzz_remaining = nbBuzz;
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
				BUZZER_RUN();
				t = buzz_duration;
				nb_buzz_remaining--;
			}
			if(!t)
			{
				//Stop buzz
				BUZZER_STOP();
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
