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
	// RAZ du Buffer : Attention, pensez a paramétrer On ou Off
	
	void BUFFER_enable(bool_e state);
	// TRUE : Active le buffer au nombre max de cases
	// FALSE : Désactive le buffer : une seule trajectoire possible est mémorisée !
	// Dans ce mode, toute trajectoire ajoutée écrasera les trajectoires en mémoire !!!
	
	
	void BUFFER_add(order_t * order);
	// Fonction qui ajoute un ordre dans le buffer 
	
	void BUFFER_add_begin(order_t * order);
	// Fonction qui ajoute un ordre au début du buffer
	
	bool_e BUFFER_is_full(void);
	// TRUE : buffer plein, FALSE : buffer non plein
	
	bool_e BUFFER_is_empty(void);
	// TRUE : buffer vide, FALSE : buffer non vide
	

	// Fonction de récupération du prochain ordre dans le buffer
	void BUFFER_get_next(order_t * order);


	
#endif	//def _BUFFER_H
