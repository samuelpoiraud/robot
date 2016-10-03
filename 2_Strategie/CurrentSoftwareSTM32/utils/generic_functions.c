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

#include "generic_functions.h"
#include "../propulsion/movement.h"
#include "../QS/QS_outputlog.h"

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

//Un point est-t-il une position permettant les rotations sans tapper dans un élément de jeu.
bool_e is_possible_point_for_rotation(GEOMETRY_point_t * p)
{
	Uint8 widthRobot;
	widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? (BIG_ROBOT_WIDTH/2) : (SMALL_ROBOT_WIDTH/2);
	widthRobot += 100;	//Marge !

	// Spécifique Terrain 2016
	if(
			!is_in_square(50 + (widthRobot), 2000-(widthRobot), 0+(widthRobot), 3000-(widthRobot), *p)	// Hors Terrain - la zone des portes de cabanes...
		|| 	is_in_square(0, 200+(widthRobot), 800-(widthRobot), 940+(widthRobot),*p)			        // Tasseau de la dune côté violet + carré de 8 cubes
		||  is_in_square(0, 200+(widthRobot), 2060-(widthRobot), 2200+(widthRobot),*p)		            // Tasseau de la dune côté vert + carré de 8 cubes
		||	is_in_square(750-(widthRobot), 1350+(widthRobot), 1460-(widthRobot), 1530+(widthRobot),*p)  // Tasseau verticale de la zone centrale
		||	is_in_square(750-(widthRobot), 780+(widthRobot), 900-(widthRobot), 2100+(widthRobot),*p)	// Tasseau horizontale de la zone centrale
		||	is_in_square(1950-(widthRobot), 2000, 910-(widthRobot), 960+(widthRobot),*p)                // Attache filet côté vert
		||	is_in_square(1950-(widthRobot), 2000, 2040-(widthRobot), 2090+(widthRobot),*p)              // Attache filet côté violet
	  )
		return FALSE;

	return  TRUE;
}

//Le point passé en paramètre permet-il une extraction ?
bool_e is_possible_point_for_dodge(GEOMETRY_point_t * p)
{
	Uint8 widthRobot;
	widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? (BIG_ROBOT_WIDTH/2) : (SMALL_ROBOT_WIDTH/2);
	widthRobot += 100;	//Marge !
	GEOMETRY_circle_t zone_depose_adv = {(GEOMETRY_point_t){750, 1500}, 700};

	// Spécifique Terrain 2016
	if(
			(is_in_circle(*p, zone_depose_adv) && is_in_square(750, 1350+(widthRobot), 1500, COLOR_Y(2100+(widthRobot)),*p))  // Zone de dépose adverse
		||  is_in_square(600-(widthRobot), 1100+(widthRobot), COLOR_Y(2700-(widthRobot)), COLOR_Y(3000),*p) // zone de départ adverse
	)
		return FALSE;

	return  TRUE;
}

bool_e i_am_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2){
	return is_in_square(x1, x2, COLOR_Y(y1), COLOR_Y(y2), (GEOMETRY_point_t){global.pos.x, global.pos.y});
}

bool_e is_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, GEOMETRY_point_t current){
	return is_in_square(x1, x2, COLOR_Y(y1), COLOR_Y(y2), current);
}

