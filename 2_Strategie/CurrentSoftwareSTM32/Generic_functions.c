/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Generic_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonction generique pour piles de gestion
 *					des actionneurs
 *	Auteur : Jacen
 *	Version 20090111
 */

#define GENERIC_FUNCTIONS_C

#include "Generic_functions.h"

void wait_forever (stack_id_e stack_id, bool_e init)
{
	/* fonction destinée à attendre sans sortir de la pile */
	return;
}

Uint8 wait_time(time32_t time, Uint8 in_progress, Uint8 success_state){
	typedef enum
	{
		IDLE,
		WAIT,
		END_OK
	}state_e;
	static state_e state = IDLE;
	static time32_t timeEnd;
	switch(state){

		case IDLE :
			timeEnd = global.env.match_time + time;
			state = WAIT;
			break;

		case WAIT:
			if(timeEnd <= global.env.match_time)
				state = END_OK;
			break;

		case END_OK:
			state = IDLE;
			return success_state;
	}
	return in_progress;
}
