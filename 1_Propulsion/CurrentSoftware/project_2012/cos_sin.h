 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : cos_sin.h
 *  Package : Propulsion
 *  Description : Tableau de cosinus sinus - accès direct.
 *  Auteur : Asser 2006 - modif by Val' - peu de modifs en 2009 - ajout du 16384 en 2010
 *  Version 201203
 */
 
#ifndef _COS_SIN_H
	#define _COS_SIN_H
	
	#include "QS/QS_all.h"
	
	

	void COS_SIN_4096_get(Sint16 teta,Sint16 * cos, Sint16 * sin);
	void COS_SIN_16384_get(Sint32 x,Sint16 * cos, Sint16 * sin);

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

#endif
