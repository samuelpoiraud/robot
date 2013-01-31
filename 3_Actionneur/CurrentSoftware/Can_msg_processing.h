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
	
	void CAN_process_msg(CAN_msg_t* msg_to_process);	
	
	#ifdef CAN_MSG_PROCESSING_C
		#include "QS/QS_CANmsgList.h"
		#include "queue.h"
                #include "Ball_grabber.h"
                #include "Hammer.h"
	#ifdef TEST_MODE
			#include <stdio.h>
			#include <string.h>
	#endif /* def TEST_MODE */

	#endif /* def CAN_MSG_PROCESSING_C */
	
#endif /* ndef CAN_MSG_PROCESSING_H */
