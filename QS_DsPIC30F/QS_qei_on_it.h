/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_qei_on_it.h
 *	Package : Qualite Software
 *	Description :	Utilise deux IT externes pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_QEI_ON_IT_H
#define QS_QEI_ON_IT_H
#include "QS_all.h"

#ifdef USE_QEI_ON_IT

	// Initialisation de l'interface
	void QEI_ON_IT_init(); 

	// Recupération de la valeur du codeur
	Sint16 QEI_ON_IT_get_count();
	
	// Ecriture de la valeur du codeur
	void QEI_ON_IT_set_count(Sint16 count);

#endif /* def USE_QEI_ON_IT */	
#endif /* ndef QS_QEI_ON_IT_H */
