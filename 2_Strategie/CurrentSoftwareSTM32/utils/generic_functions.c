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

	// Spécifique Terrain 2017
	if(
			!is_in_square(0 + (widthRobot), 2000-(widthRobot), 0+(widthRobot), 3000-(widthRobot), *p)	// Hors Terrain
		|| 	is_in_square(0, 382+(widthRobot), 0, 710+(widthRobot),*p)						// Tasseau zone de départ bleu + bascule
		|| 	is_in_square(0, 382+(widthRobot), 2290-(widthRobot), 3000,*p)					// Tasseau zone de départ jaune + bascule
		|| 	is_in_square(0, 80+(widthRobot), 1110-(widthRobot), 1190+(widthRobot),*p)		// Fusée bleu
		|| 	is_in_square(0, 80+(widthRobot), 1110-(widthRobot), 1190+(widthRobot),*p)		// Fusée jaune
		|| 	is_in_square(678-(widthRobot), 1400+(widthRobot), 0, 102+(widthRobot),*p)		// Zone de dépose modules côté bleu + fusée
		|| 	is_in_square(678-(widthRobot), 1400+(widthRobot), 1898-(widthRobot), 3000,*p)   // Zone de dépose modules côté jaune + fusée
		|| 	is_in_square(540-85-(widthRobot), 540+85+(widthRobot), 650-85-(widthRobot), 650+85+(widthRobot),*p)   // Petit cratère nord côté bleu
		|| 	is_in_square(540-85-(widthRobot), 540+85+(widthRobot), 2350-85-(widthRobot), 2350+85+(widthRobot),*p) // Petit cratère nord côté jaune
		|| 	is_in_square(1400-(widthRobot), 2000, 910-(widthRobot), 2090+(widthRobot),*p)   // Zone du milieu (les 2 petits cratères + les 2 zones de dépose modules inclinées)
		|| 	is_in_square(1200-(widthRobot), 2000, 1430-(widthRobot), 1570+(widthRobot),*p)	// Zone du de dépose modules du milieu
		|| 	(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){2000, 0}, 510+(widthRobot)}) && is_in_square(1490-(widthRobot), 2000, 0, 510+(widthRobot),*p))			// Gros cratère côté bleu
		|| 	(is_in_circle(*p, (GEOMETRY_circle_t){(GEOMETRY_point_t){2000, 3000}, 510+(widthRobot)}) && is_in_square(1490-(widthRobot), 2000, 2490-(widthRobot), 3000,*p))  // Gros cratère côté jaune

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

	// Spécifique Terrain 2017
	if(
			is_in_square(0, 350+(widthRobot), COLOR_Y(1900-(widthRobot)), COLOR_Y(2300),*p) // zone de départ adverse (n'empêche pas de tourner mais on ne doit pas s'extraire dedans)
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

