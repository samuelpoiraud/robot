/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : Can_msg_processing.c
 *	Package : Carte actionneur
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Aurélien
 *  Version 20110225
 */


#define CAN_MSG_PROCESSING_C

#include "Can_msg_processing.h"
#include "QS/QS_DCMotor2.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "queue.h"

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

void CAN_process_msg(CAN_msg_t* msg) {
	if(ACTMGR_process_msg(msg))
		return;  //Le message a déja été géré
	
	// Traitement des autres messages reçus
	switch (msg->sid)
	{
		//Fin de la partie
		case BROADCAST_STOP_ALL :
			global.match_started = FALSE;
			debug_printf("C:BROADCAST_STOP_ALL\r\n");
			//QUEUE_flush_all();
			//DCM_stop_all();
			break;
		
		//Reprise de la partie
		case BROADCAST_START :
			debug_printf("C:BROADCAST_START\r\n");
			global.match_started = TRUE;
			break;

		case BROADCAST_POSITION_ROBOT:
			//Rien, mais pas inclus dans le cas default où l'on peut afficher le sid...
			break;	
		
		default: 
			//debug_printf("SID:%x\r\n",msg->sid);
			break;
	}//End switch
}

//Met sur la pile une action qui sera gérée par act_function_ptr avec en paramètre param. L'action est protégée par semaphore avec act_id
//Cette fonction est appelée par les fonctions de traitement des messages CAN de chaque actionneur.
void CAN_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Uint16 param) {
	queue_id_t queueId = QUEUE_create();
	assert(queueId != QUEUE_CREATE_FAILED);
	if(queueId != QUEUE_CREATE_FAILED) {	//Si on est pas en verbose_mode, l'assert sera ignoré et la suite risque de vraiment planter ...
		QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0}, act_id);
		QUEUE_add(queueId, act_function_ptr, (QUEUE_arg_t){msg->data[0], param}, act_id);
		QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0}, act_id);
	} else {	//on indique qu'on a pas géré la commande
		CAN_msg_t resultMsg = {ACT_RESULT, {msg->sid & 0xFF, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
		CAN_send(&resultMsg);
	}
}
