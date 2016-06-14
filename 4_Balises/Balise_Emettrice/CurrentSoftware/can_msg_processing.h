/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : it.c
 *  Package : Balise émettrice
 *  Description : Fonctions de traitement des messages CAN
 *  Auteur : Arnaud
 *  Version 200904
 */


#ifndef CAN_MSG_PROCESSING_H
	#define CAN_MSG_PROCESSING_H
	#include "QS/QS_all.h"
	#include "QS/QS_CANmsgList.h"

	//Traite les messages CAN reçus
	void CAN_process_msg(CAN_msg_t* msg_to_process);

#endif /* ndef CAN_MSG_PROCESSING_H */
