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
	Sint32 CALCULATOR_viewing_algebric_distance(Sint16 depart_x, Sint16 depart_y, Sint16 destination_x, Sint16 destination_y, Sint16 angle_de_vue);
	Sint16 CALCULATOR_viewing_angle(Sint16 start_x, Sint16 start_y, Sint16 destination_x, Sint16 destination_y);

	Sint16 CALCULATOR_modulo_angle(Sint16 angle);

	
	Sint32 CALCULATOR_modulo_angle_22(Sint32 angle);
	
#endif	//def _CALCULATOR_H
