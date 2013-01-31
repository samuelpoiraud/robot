/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_qei.h
 *	Package : Qualite Software
 *	Description :	Utilise le module QEI pour d�coder les
					signaux d'un codeur incr�mental
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_QEI_H
#define QS_QEI_H
#include "QS_all.h"

	// Initialisation de l'interface
	void QEI_init(); 

	// Recup�ration de la valeur du codeur
	Sint16 QEI_get_count();

	// Ecriture de la valeur du codeur
	void QEI_set_count(Sint16 count);

#endif /* ndef QS_QEI_H */
