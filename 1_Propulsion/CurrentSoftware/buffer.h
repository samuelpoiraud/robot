 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : cpld.h
 *  Package : Propulsion
 *  Description : prototypes des fonctions de buffer de trajectoire
 *  Auteur : Val' && Thomas MOESSE alias Ketchom && Arnaud AUMONT alias Koubi && Nirgal 2009
 *  Version 201203
 */
#include "QS/QS_all.h"

#ifndef _BUFFER_H
	#define _BUFFER_H

		#include "roadmap.h"
	/* Fonctions de gestion du buffer */

	void BUFFER_init(void);
	// RAZ du Buffer : Attention, pensez a param�trer On ou Off

	void BUFFER_enable(bool_e state);
	// TRUE : Active le buffer au nombre max de cases
	// FALSE : D�sactive le buffer : une seule trajectoire possible est m�moris�e !
	// Dans ce mode, toute trajectoire ajout�e �crasera les trajectoires en m�moire !!!


	void BUFFER_add(order_t * order);
	// Fonction qui ajoute un ordre dans le buffer

	void BUFFER_add_begin(order_t * order);
	// Fonction qui ajoute un ordre au d�but du buffer

	bool_e BUFFER_is_full(void);
	// TRUE : buffer plein, FALSE : buffer non plein

	bool_e BUFFER_is_empty(void);
	// TRUE : buffer vide, FALSE : buffer non vide


	// Fonction de r�cup�ration du prochain ordre dans le buffer
	void BUFFER_get_next(order_t * order);

	// Fonction qui permet de vider le buffer
	void BUFFER_flush();



#endif	//def _BUFFER_H
