 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : calculator.h
 *  Package : Propulsion
 *  Description : prototypes Fonctions de calculs servant à la gestion de trajectoire
 *  Auteur : Val' && Thomas MOESSE alias Ketchom && Arnaud AUMONT alias Koubi && Nirgal 2009
 *  Version 201203
 */

#ifndef _CALCULATOR_H
	#define _CALCULATOR_H
	#include "QS/QS_all.h"


	/* Fonctions de calcul */
	Sint32 CALCULATOR_viewing_algebric_distance(Sint32 depart_x, Sint32 depart_y, Sint32 destination_x, Sint32 destination_y, Sint32 angle_de_vue);
	Sint32 CALCULATOR_viewing_algebric_distance_mm16(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y, Sint32 angle_de_vue);


	Sint16 CALCULATOR_viewing_angle(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y);
	Sint32 CALCULATOR_viewing_angle_22(Sint32 start_x, Sint32 start_y, Sint32 destination_x, Sint32 destination_y);

	Sint16 CALCULATOR_modulo_angle(Sint16 angle);
	Sint32 CALCULATOR_modulo_angle_22(Sint32 angle);

	Uint16 CALCULATOR_distance(Sint16 xfrom, Sint16 yfrom, Sint16 xto, Sint16 yto);
	Uint16 CALCULATOR_manhattan_distance(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

#endif	//def _CALCULATOR_H
