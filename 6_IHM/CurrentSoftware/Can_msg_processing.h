/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : Can_msg_processing.h
 *	Package : Carte IHM
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Anthony
 *  Version 20110225
 */

#ifndef CAN_MSG_PROCESSING_H
	#define CAN_MSG_PROCESSING_H
	#include "QS/QS_all.h"

	//Traite les messages CAN reçus
	void CAN_process_msg(CAN_msg_t* msg_to_process);

#endif /* ndef CAN_MSG_PROCESSING_H */
