 /*
 *  Club Robot ESEO 2014
 *
 *  Fichier : pathChecker.h
 *  Package : Propulsion
 *  Description : Analyse la trajectoire que va emprunter le robot par rapport au robot adversaire, afin de savoir si le chemin est réalisable
 *  Auteur : Arnaud
 *  Version 201203
 */

#ifndef _PATHCHECKER_H
	#define _PATHCHECKER_H
	#include "QS/QS_all.h"

	void PATHCHECKER_init();

	bool_e PATHCHECKER_target_safe(Sint32 destx, Sint32 desty);



#endif  //def _PATHCHECKER_H
