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

	//Compatibilité avec les anciennes fonctions, il n'y avait qu'un QEI et un autre simulé par IT externe
	#define QEI_get_count QEI1_get_count
	#define QEI_set_count QEI1_set_count

	#define QEI_ON_IT_init QEI_init
	#define QEI_ON_IT_get_count QEI2_get_count
	#define QEI_ON_IT_set_count QEI2_set_count


	// Recupération de la valeur du codeur
	Sint16 QEI1_get_count();

	// Ecriture de la valeur du codeur
	void QEI1_set_count(Sint16 count);


	// Recupération de la valeur du codeur
	Sint16 QEI2_get_count();

	// Ecriture de la valeur du codeur
	void QEI2_set_count(Sint16 count);

#endif /* ndef QS_QEI_H */
