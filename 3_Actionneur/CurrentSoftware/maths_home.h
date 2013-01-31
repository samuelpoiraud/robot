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

	#include <math.h>
	
	double cos4096(Sint16 angle);
	
	double sin4096(Sint16 angle);

	Sint16 atan4096(double tangent);

	#ifdef FAST_COS_SIN
		#ifdef COS_SIN_16384
			#define PI16384				51472
			#define HALF_PI16384		25736
			#define QUATER_PI16384		12868
			#define THREE_HALF_PI16384	77208
			#define	TWO_PI16384			102944
			#define TWO_PI	 (25736)
			#define PI_22   (13176795)		/*Valeur de PI<<22 */ 
			#define TWO_PI22 (26353589)
			void cos_sin (Sint32 x,Sint16 * cos, Sint16 * sin);
			Sint16 tableau(Sint16 teta);
		#endif
		
		#ifndef COS_SIN_16384	
			#define PI2 	 (3294198)
			#define PI_22   (13176795)		/*Valeur de PI<<22 */ 
			#define TWO_PI22 (26353589)
			#define TWO_PI	 (25736)
			#define HALF_PI  0x1922
		
			void cos_sin (Uint16 x,Sint16 * cos, Sint16 * sin);
			void cosinus(Sint16 x,Sint16*cos);
			void sinus(Sint16 x,Sint16*sin);
		#endif
	#endif

#endif /* ndef MATHS_HOME_H */

