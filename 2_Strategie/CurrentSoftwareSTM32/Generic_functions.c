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
#include "avoidance.h"
#include "QS/QS_outputlog.h"

Uint8 wait_time(time32_t time, Uint8 in_progress, Uint8 success_state){
	typedef enum
	{
		IDLE,
		WAIT,
		END
	}state_e;
	static state_e state = IDLE;
	static time32_t timeEnd;
	switch(state){

		case IDLE :
			timeEnd = global.match_time + time;
			state = WAIT;
			break;

		case WAIT:
			if(timeEnd <= global.match_time)
				state = END;
			break;

		case END:
			state = IDLE;
			return success_state;
	}
	return in_progress;
}

void on_turning_point(){
	if(!is_possible_point_for_rotation(&((GEOMETRY_point_t){global.pos.x, global.pos.y})))
		debug_printf(" !! NOT ON TURNING POINT !!\n");
}
