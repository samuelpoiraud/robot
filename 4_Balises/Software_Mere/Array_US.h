/*
 *	Club Robot ESEO 2012 - 2013
 *
 *	Fichier : Array_US.h
 *	Package : Balises
 *	Description : tableaux et fonctions permettant le calcul de la position
 *   				du robot adverse selon les distances mesurées
 *	Auteur : Nirgal
 *	Version 201208
 */

 
 
 #ifndef ARRAY_US_H
	#define ARRAY_US_H
	#include "QS\QS_all.h"

	#define BAD (16383)	//Valeurs non cohérentes...

	bool_e find_x_y_from_d0_d1(Sint16 * x, Sint16 * y, Uint8 d0, Uint8 d1);
	bool_e find_two_points_from_d0_d2(Sint16 * xa, Sint16 * ya, Sint16 * xb, Sint16 * yb, Uint8 d0, Uint8 d2);
	bool_e find_two_points_from_d1_d2(Sint16 * xa, Sint16 * ya, Sint16 * xb, Sint16 * yb, Uint8 d1, Uint8 d2);
	bool_e find_one_point_from_d0_d1_d2(Sint16 * x, Sint16 * y, Uint8 d0, Uint8 d1, Uint8 d2);
	void ARRAY_US_tests(void);
	
#endif /* def ARRAY_US_H */
