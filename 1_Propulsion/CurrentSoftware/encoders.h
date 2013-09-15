 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : encoders.h
 *  Package : Propulsion
 *  Description : Récupération des informations des encodeurs optiques.
 *  Auteur : Nirgal (2009)
 *  Version 201203
 */

#ifndef _ENCODERS_H
	#define _ENCODERS_H
		#include "QS/QS_all.h"
		void ENCODERS_init(void);

	void ENCODERS_get(Sint32 * left, Sint32 * right);

#endif	//def _ENCODERS_H
