/*
 *	Club Robot ESEO 2009 - 2010
 *	CHOMP
 *
 *	Fichier : act_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'actionneur
 *	Auteur : Julien et Ronan
 *	Version 20110313
 */
 
 

#include "QS/QS_all.h"

#ifndef ACT_FUNCTIONS_H
	#define ACT_FUNCTIONS_H
	
	//Fonction générique permettant de positionner les actionneurs
	//void ACT_push_set_pos(ACT_id_e act, ACT_position_e position, bool_e run);
	
	//void ACT_set_pos(stack_id_e stack_id, bool_e init);

	// Gère les messages de retour de la carte actionneur lorsque une action s'est terminé ou à échouée
	void ACT_process_result(Uint8 can_act_id, Uint8 can_result);
	
	//simple envoi du message 
	void ACT_hammer_up(void);
	void ACT_hammer_down(void);
	void ACT_hammer_tidy(void);

	void ACT_ball_grabber_up(void);
	void ACT_ball_grabber_down(void);
	void ACT_ball_grabber_tidy(void);

	void ACT_push_ball_launcher_stop(bool_e run);
	void ACT_push_ball_launcher_run(Uint16 speed, bool_e run);
	/* Fonctions empilables */
	/* ATTENTION  : pas de variables statiques pour les fonctions qui sont éxécutées dans deux piles */
	
	// Renvoie l'actionneur demandé
	//Uint8 ACT_id_data(stack_id_e stack_id);
	
	// Renvoie la position demandée
	//Uint16 ACT_position_data(stack_id_e stack_id);

#endif /* ndef ACT_FUNCTIONS_H */
