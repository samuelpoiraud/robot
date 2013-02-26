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
	#include "queue.h"   //pour action_t

	//Met sur la pile une action qui sera gérée par act_function_ptr avec en paramètre param. L'action est protégée par semaphore avec act_id
	void CAN_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Sint16 param);
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
