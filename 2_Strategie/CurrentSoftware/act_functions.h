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

	typedef struct
	{	
		ACT_id_e act_id;
		ACT_position_e position;
	}act_arg_t;
	
	
	/* Accesseur en lecture sur les arguments des piles ACT */ 
	act_arg_t ACT_get_stack_arg(ACT_id_e act_id, Uint8 index);
	
	/* Accesseur en écriture sur les arguments des piles ACT */ 
	void ACT_set_stack_arg(ACT_id_e act_id, act_arg_t arg, Uint8 index);
	
	//Fonction générique permettant de positionner les actionneurs
	void ACT_push_set_pos(ACT_id_e act, ACT_position_e position, bool_e run);
	
	void ACT_set_pos(stack_id_e stack_id, bool_e init);
	
	//simple envoi du message 
	void ACT_hammer_up(void);
        void ACT_hammer_down(void);
	void ACT_hammer_tidy(void);

        void ACT_ball_grabber_up(void);
        void ACT_ball_grabber_down(void);
        void ACT_ball_grabber_tidy(void);
	/* Fonctions empilables */
	/* ATTENTION  : pas de variables statiques pour les fonctions qui sont éxécutées dans deux piles */
	
	// Renvoie l'actionneur demandé
	Uint8 ACT_id_data(stack_id_e stack_id);
	
	// Renvoie la position demandée
	Uint16 ACT_position_data(stack_id_e stack_id);
		
	#ifdef ACT_FUNCTIONS_C
	
		#include "QS/QS_can.h"
		#include "QS/QS_CANmsgList.h"
		
		//Timeout en ms
		#define ACT_SET_POS_TIMEOUT_TIME  	10000
		
		// Timeout en ms
		
		#ifdef DEBUG_ACT_FUN
			#define act_fun_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define act_fun_printf(...)	(void)0
		#endif
		
	#endif /* def ACT_FUNCTIONS_C */

#endif /* ndef ACT_FUNCTIONS_H */
