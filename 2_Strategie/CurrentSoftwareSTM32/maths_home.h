/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : maths_home.h
 *	Package : Carte Principale
 *	Description : Fonctions de maths maison
 *	Auteur : Djoman
 *	Version 20090506
 */

#include "QS/QS_all.h"

#ifndef MATHS_HOME_H
#define MATHS_HOME_H


	#ifdef FAST_COS_SIN
		void COS_SIN_4096_get(Sint16 teta,Sint16 * cos, Sint16 * sin);
	#endif

	#define PI16384				51472
	#define HALF_PI16384		25736
	#define QUATER_PI16384		12868
	#define THREE_HALF_PI16384	77208
	#define	TWO_PI16384			102944
	#define PI_22				(13176795)		/*Valeur de PI<<22 */
	#define TWO_PI22			(26353589)

	#define TWO_PI4096	 (25736)
	#define HALF_PI4096 (6434)
	#define THREE_HALF_PI4096 (19302)

	#define SQUARE(x)	((x)*(x))

	Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

#endif /* ndef MATHS_HOME_H */

