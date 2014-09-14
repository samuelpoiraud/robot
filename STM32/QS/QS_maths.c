/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : QS_maths.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions mathématiques utiles
 *  Auteur : Arnaud
 *  Version 20130518
 */


#include "QS_maths.h"

Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2){
	return sqrt((Sint32)(y1 - y2)*(y1 - y2) + (Sint32)(x1 - x2)*(x1 - x2));
}
