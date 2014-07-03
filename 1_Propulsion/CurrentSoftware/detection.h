/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : detection.h
 *	Package : Carte Principale
 *	Description : Traitement des informations pour détection
 *	Auteur : Jacen (Modifié par Ronan)
 *	Version 20110417
 */

#ifndef DETECTION_H
	#define DETECTION_H

#include "QS/QS_all.h"
#include "hokuyo.h"
typedef struct
{
	bool_e enable;					//cet objet est activé.
	bool_e updated;
	Uint32 update_time;
	Sint16 angle;
	Sint16 dist;
	Sint16 x;
	Sint16 y;
	Uint8 fiability_error;
}adversary_t;

void DETECTION_init(void);

/*	mise à jour de l'information de détection avec le contenu
	courant de l'environnement */
void DETECTION_process_main(void);


// Doit être appelé en FIN d'IT... (les flags updated y sont baissés)
void DETECTION_process_it(void);

/*
 * Déclaration des détections adverses :
 * 	- Via un message CAN (sinon, msg = NULL)
 * 	- Via une structure HOKUYO_adversary_position et adv_nb (sinon adv = NULL)
 */
void DETECTION_new_adversary_position(CAN_msg_t * msg, HOKUYO_adversary_position * adv, Uint8 adv_nb);
	

//Retourne le tableau des adversaires détectés...
//@post : size contient la taille du tableau
adversary_t * DETECTION_get_adversaries(Uint8 * size);
	
	
	
	

#endif /* ndef DETECTION_H */
