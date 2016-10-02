/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Generic_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions generiques pour piles de gestion
 *					des actionneurs
 *	Auteur : Jacen
 *	Version 20090111
 */

#include "QS/QS_all.h"

#ifndef GENERIC_FUNCTIONS_H
	#define GENERIC_FUNCTIONS_H

	Uint8 wait_time(time32_t time, Uint8 in_progress, Uint8 success_state);

	// Fonction à utiliser en entrée et sortie de sub_action pour vérifier le bon placement du robot
	void on_turning_point();

#endif /* ndef GENERIC_FUNCTIONS_H */
