/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_qei.h
 *	Package : Qualite Software
 *	Description :	Utilise le module QEI pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Version 20091030
 */

#ifndef QS_QEI_H
#define QS_QEI_H
#include "../QS/QS_all.h"

	// Initialisation de l'interface
	void QEI_init(); 

	// Recupération de la valeur du codeur
	Sint16 QEI_get_count();

	#ifdef QS_QEI_C

		#include <qei.h>
			
	#endif /* def QS_QEI_C */
#endif /* ndef QS_QEI_H */
