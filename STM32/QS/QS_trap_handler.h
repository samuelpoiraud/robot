/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  Fichier : QS_trap_handler.h
 *  Package : Qualité Soft
 *  Description : Gestion des trap en cas d'erreur fatales
 *  Auteur : amurzeau
 *  Version 20130603
 */


/** ----------------  Defines possibles  --------------------
 *	Aucun
 */

#ifndef QS_TRAP_HANDLER_H
	#define QS_TRAP_HANDLER_H

	#include "QS_all.h"

	void dump_trap_info(Uint32 stack_ptr[], Uint32 lr);

#endif /* ndef QS_TRAP_HANDLER_H */

