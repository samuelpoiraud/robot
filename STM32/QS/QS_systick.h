/*
 *	Club Robot ESEO 2014 - 2015
 *	Holly & Wood
 *
 *	Fichier : QS_systick.c
 *	Package : Qualite Soft
 *	Description : Configuration de systick pour gérer une l'horloge du projet
 *	Auteur : Arnaud
 *	Version 20100421
 */


/** ----------------  Defines possibles  --------------------
 *	Aucun
 */


#ifndef QS_SYSTICK_H
	#define QS_SYSTICK_H

	#include "QS_all.h"

	void SYSTICK_init(time32_t* systick_counter);

#endif /* ndef QS_SYSTICK_H */
