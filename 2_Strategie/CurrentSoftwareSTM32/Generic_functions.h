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

	typedef action_fun_t generic_fun_t;

	void wait_forever (stack_id_e stack_id, bool_e init);

	Uint8 wait_time(time32_t time, Uint8 in_progress, Uint8 success_state);


#endif /* ndef GENERIC_FUNCTIONS_H */
