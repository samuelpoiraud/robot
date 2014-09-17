/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : elements.h
 *	Package : Carte Principale
 *	Description : Traitement des informations sur les éléments de jeu
 *	Auteur : Ronan, Adrien
 *	Version 20110430
 */

#include "QS/QS_all.h"
#ifndef ELEMENTS_H
	#define ELEMENTS_H

	#include "QS/QS_maths.h"
	#include "prop_types.h"
	#include "avoidance.h"

	// Enumération
	typedef enum{
		FRUIT_VERIN_OPEN,
		FRUIT_VERIN_CLOSE,
		UNKNOWN
	}fruit_verin_state_e;

	typedef enum{
		ADVERSARY_TORCH,
		OUR_TORCH
	}torch_choice_e;

	typedef enum{
		NO_DISPOSE = 0,			//l'action do_torch se termine dès qu'on a la torche (sans la dépiler)
		HEARTH_OUR,			//Dépilage de la torche sur notre foyer
		HEARTH_ADVERSARY,	//Dépilage sur foyer adverse
		HEARTH_CENTRAL,		//Dépilage sur foyer central
		FILED_FRESCO,		//Dépilage devant les fresques
		ANYWHERE			//Dépilage n'importe où (la fonction choisit elle même le plus pertinent)
	}torch_dispose_zone_e;

	typedef enum{
		RED_CAVERN,
		YELLOW_CAVERN,
		RED_TREE_MAMOU,
		RED_TREE,
		YELLOW_TREE_MAMOU,
		YELLOW_TREE
	}pos_drop_e;


	// Va poser un triangle à l'endriot demandé
	error_e ELEMENT_go_and_drop(pos_drop_e choice);

	// Fonction de réception de message CAN
	void ELEMENT_update_fruit_verin_state(CAN_msg_t* msg);	// Mets à jours l'état du labium
	void ELEMENT_answer_scan_anything(CAN_msg_t* msg);	// Récupère le résultat du scan
	void ELEMENT_answer_pump(CAN_msg_t *msg);			// Récupère la réponse de la pompe

	// Fonction utilisateur

	// Fonction de lancement / subaction

	// subaction qui donne l'ordre puis attend que le labium soit dans la position 'labium_order'
	Uint8 ELEMENT_do_and_wait_end_fruit_verin_order(fruit_verin_state_e labium_order, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	// Nouvelle position de la torche apres une poussée
	void TORCH_new_position(torch_choice_e choice);

	// Renvois les position de la torche
	GEOMETRY_point_t TORCH_get_position(torch_choice_e choice);

	// Gere le message can de la nouvelle position de la torche
	void TORCH_CAN_process_msg(CAN_msg_t *msg);

	// Envois la nouvelle position de la torche par XBEE
	void TORCH_XBee_send_msg(torch_choice_e choice, GEOMETRY_point_t pos);

	// Attends que la pompe renvoie si elle détecte la une prise d'objet
	Uint8 ELEMENT_wait_pump_capture_object(Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	// Renvoie l'état du capteur de fresque numéro NB (1 présence, 0 absence)
	// NB allant de 1 à 3
	bool_e get_fresco(Uint8 nb);


	Sint16 get_dist_torch_laser();

#endif /* ndef ELEMENTS_H */
