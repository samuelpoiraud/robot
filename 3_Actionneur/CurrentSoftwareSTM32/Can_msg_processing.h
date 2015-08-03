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
	#include "QS/QS_CANmsgList.h"

	void CAN_process_init();

	//Traite les messages CAN reçus
	void CAN_process_msg(CAN_msg_t* msg_to_process);
	
#endif /* ndef CAN_MSG_PROCESSING_H */
