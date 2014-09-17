/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : elements.h
 *	Package : Carte Principale
 *	Description : Traitement des informations sur les �l�ments de jeu
 *	Auteur : Ronan, Adrien
 *	Version 20110430
 */

#include "QS/QS_all.h"
#ifndef ELEMENTS_H
	#define ELEMENTS_H

	#include "QS/QS_maths.h"
	#include "prop_types.h"
	#include "avoidance.h"

	// Enum�ration
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
		NO_DISPOSE = 0,			//l'action do_torch se termine d�s qu'on a la torche (sans la d�piler)
		HEARTH_OUR,			//D�pilage de la torche sur notre foyer
		HEARTH_ADVERSARY,	//D�pilage sur foyer adverse
		HEARTH_CENTRAL,		//D�pilage sur foyer central
		FILED_FRESCO,		//D�pilage devant les fresques
		ANYWHERE			//D�pilage n'importe o� (la fonction choisit elle m�me le plus pertinent)
	}torch_dispose_zone_e;

	typedef enum{
		RED_CAVERN,
		YELLOW_CAVERN,
		RED_TREE_MAMOU,
		RED_TREE,
		YELLOW_TREE_MAMOU,
		YELLOW_TREE
	}pos_drop_e;


	// Va poser un triangle � l'endriot demand�
	error_e ELEMENT_go_and_drop(pos_drop_e choice);

	// Fonction de r�ception de message CAN
	void ELEMENT_update_fruit_verin_state(CAN_msg_t* msg);	// Mets � jours l'�tat du labium
	void ELEMENT_answer_scan_anything(CAN_msg_t* msg);	// R�cup�re le r�sultat du scan
	void ELEMENT_answer_pump(CAN_msg_t *msg);			// R�cup�re la r�ponse de la pompe

	// Fonction utilisateur

	// Fonction de lancement / subaction

	// subaction qui donne l'ordre puis attend que le labium soit dans la position 'labium_order'
	Uint8 ELEMENT_do_and_wait_end_fruit_verin_order(fruit_verin_state_e labium_order, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	// Nouvelle position de la torche apres une pouss�e
	void TORCH_new_position(torch_choice_e choice);

	// Renvois les position de la torche
	GEOMETRY_point_t TORCH_get_position(torch_choice_e choice);

	// Gere le message can de la nouvelle position de la torche
	void TORCH_CAN_process_msg(CAN_msg_t *msg);

	// Envois la nouvelle position de la torche par XBEE
	void TORCH_XBee_send_msg(torch_choice_e choice, GEOMETRY_point_t pos);

	// Attends que la pompe renvoie si elle d�tecte la une prise d'objet
	Uint8 ELEMENT_wait_pump_capture_object(Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	// Renvoie l'�tat du capteur de fresque num�ro NB (1 pr�sence, 0 absence)
	// NB allant de 1 � 3
	bool_e get_fresco(Uint8 nb);


	Sint16 get_dist_torch_laser();

#endif /* ndef ELEMENTS_H */
