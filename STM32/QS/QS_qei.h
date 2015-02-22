/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_qei.h
 *	Package : Qualite Software
 *	Description :	Utilise le module QEI pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_QEI_H
#define QS_QEI_H
#include "QS_all.h"

	// Initialisation de l'interface
	void QEI_init();

	#ifdef USE_QUEI1
		// Recupération de la valeur du codeur
		Sint16 QEI1_get_count();

		// Ecriture de la valeur du codeur
		void QEI1_set_count(Sint16 count);
	#endif

	#ifdef USE_QUEI2
		// Recupération de la valeur du codeur
		Sint16 QEI2_get_count();

		// Ecriture de la valeur du codeur
		void QEI2_set_count(Sint16 count);
	#endif

#endif /* ndef QS_QEI_H */
