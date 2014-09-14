/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : QS_maths.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions mathématiques utiles
 *  Auteur : Arnaud
 *  Version 20130518
 */

#ifndef QS_MATHS_H
	#define QS_MATHS_H

	#include "QS_types.h"
	#include "math.h"

	#define SQUARE(x)	((x)*(x))

	Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

#endif /* ndef QS_MATHS_H */
