/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : detection.h
 *	Package : Carte Principale
 *	Description : Traitement des informations pour d�tection
 *	Auteur : Jacen (Modifi� par Ronan)
 *	Version 20110417
 */

#ifndef DETECTION_H
	#define DETECTION_H

#include "QS/QS_all.h"
#include "hokuyo.h"
typedef struct
{
	bool_e enable;					//cet objet est activ�.
	bool_e updated;
	Uint32 update_time;
	Sint16 angle;
	Sint16 dist;
	Sint16 x;
	Sint16 y;
	Uint8 fiability_error;
}adversary_t;

void DETECTION_init(void);

/*	mise � jour de l'information de d�tection avec le contenu
	courant de l'environnement */
void DETECTION_process_main(void);


// Doit �tre appel� en FIN d'IT... (les flags updated y sont baiss�s)
void DETECTION_process_it(void);

/*
 * D�claration des d�tections adverses :
 * 	- Via un message CAN (sinon, msg = NULL)
 * 	- Via une structure HOKUYO_adversary_position et adv_nb (sinon adv = NULL)
 */
void DETECTION_new_adversary_position(CAN_msg_t * msg, HOKUYO_adversary_position * adv, Uint8 adv_nb);
	

//Retourne le tableau des adversaires d�tect�s...
//@post : size contient la taille du tableau
adversary_t * DETECTION_get_adversaries(Uint8 * size);
	
	
	
	

#endif /* ndef DETECTION_H */
