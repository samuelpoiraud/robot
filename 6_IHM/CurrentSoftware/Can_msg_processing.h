/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : Can_msg_processing.h
 *	Package : Carte actionneur
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Aurélien
 *  Version 20110225
 */

#ifndef CAN_MSG_PROCESSING_H
	#define CAN_MSG_PROCESSING_H
	#include "QS/QS_all.h"

	//Traite les messages CAN reçus
	void CAN_process_msg(CAN_msg_t* msg_to_process);

	//CAN is not enabled, declare functions so compilation won't fail
	#ifndef USE_CAN
	#include "QS/QS_can.h"
		void CAN_send(CAN_msg_t* can_msg);
		CAN_msg_t CAN_get_next_msg();
		bool_e CAN_data_ready();
		/* Permet le traitement direct des messages CAN */
//		typedef bool_e (*direct_treatment_function_pt)(CAN_msg_t* msg);
		void CAN_set_direct_treatment_function(direct_treatment_function_pt fct);
	#endif
	
#endif /* ndef CAN_MSG_PROCESSING_H */
