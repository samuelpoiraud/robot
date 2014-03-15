/*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : debug.c
 *  Package : Propulsion
 *  Description : Fonctions de débogage, ou de test du code. L'idée de ce fichier est de polluer le moins possible le reste du code avec des routines de débog.
 *  Auteur : Nirgal
 *  Version 201203
 */

#ifndef _DEBUG_H
	#define _DEBUG_H

	#include "QS/QS_all.h"

	void DEBUG_process_main(void);
	void DEBUG_init(void);
	void DEBUG_process_it(void);
	void DEBUG_envoi_point_fictif_alteration_coordonnees_reelles(void);
	bool_e DEBUG_get_we_touch_border(void);
	
#endif //ndef _DEBUG_H
