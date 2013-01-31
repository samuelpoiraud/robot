/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_can.h
 *	Package : QSx86
 *	Description : Fonctions CAN
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */
 

#ifndef QS_CAN_H
	#define QS_CAN_H

	#include "QS_all.h"
	
	/* Type de pointeur sur fonction qui vérifie si le message CAN doit être traité directement */
	typedef bool_e (*direct_treatment_function_pt)(CAN_msg_t* msg); 
	
	/* Prototype */	
	void CAN_init(void);
	
	#ifdef USE_CAN
		/* Permet d'envoyer un message CAN */
		void CAN_send(CAN_msg_t* can_msg);
		/* Permet de récupérer le prochain message */
		CAN_msg_t CAN_get_next_msg();
		/* Permet de savoir s'il reste des messages dans la file */
		bool_e CAN_data_ready();
		/* Permet de recevoir des messages CAN */
		void CAN_receive(CAN_msg_t* can_msg);
		/* Permet le traitement direct des messages CAN */
		void CAN_set_direct_treatment_function(direct_treatment_function_pt fct);
	#endif /* def USE_CAN */

	#ifdef USE_CAN2
		/* Permet d'envoyer un message sur CAN2 */
		void CAN2_send(CAN_msg_t* can_msg);
		/* Permet de récupérer le prochain message dans la file */
		CAN_msg_t CAN2_get_next_msg();
		/* Permet de savoir s'il reste des messages dans la file */
		bool_e CAN2_data_ready();
		/* Permet de recevoir des messages CAN2 */
		void CAN2_receive(CAN_msg_t* can_msg);
		/* Permet le traitement direct des messages CAN */
		void CAN2_set_direct_treatment_function(direct_treatment_function_pt fct);
	#endif /* def USE_CAN2 */
#endif
