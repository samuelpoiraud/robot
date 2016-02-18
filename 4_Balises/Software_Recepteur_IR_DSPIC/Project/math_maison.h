/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier :math_maison.h
 *  Package : Asser
 *  Description : prototypes Fonctions math_maison
 *  Auteur : Asser 2006 - modif by Val' - peu de modifs en 2009
 *  Version 20070209
 */
 
#ifndef _MATH_MAISON_H
	#define _MATH_MAISON_H
	
	#include "QS/QS_all.h"
	
	#define PI2 	 3294198
	#define PI_22   13176795		/*Valeur de PI<<22 */ 
	#define TWO_PI22 26353589
	#define PI4096	 12868
	#define TWO_PI	 25736
	#define HALF_PI  0x1922

	void cos_sin (Uint16 x,Sint16 * cos, Sint16 * sin);
	void cosinus(Sint16 x,Sint16*cos);
	void sinus(Sint16 x,Sint16*sin);
#endif
