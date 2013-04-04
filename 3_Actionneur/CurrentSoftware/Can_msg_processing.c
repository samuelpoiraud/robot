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
#include "output_log.h"

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

#define LOG_PREFIX "CANProcess: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_CANPROCESSMSG, log_level, LOG_PREFIX format, ## __VA_ARGS__)

typedef enum {
	CAN_TPT_NoParam, //Pas de paramètre
	CAN_TPT_Line,    //Le paramètre est une ligne
	CAN_TPT_Normal   //Le paramètre est un nombre normal sans signification particulière pour nous (ce code)
} CAN_result_param_type_t;
static void CAN_printResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, CAN_result_param_type_t paramType, Uint16 param);

void CAN_process_msg(CAN_msg_t* msg) {
	if(ACTMGR_process_msg(msg)) {
		COMPONENT_log(LOG_LEVEL_Debug, "Act Msg SID: 0x%x, cmd: 0x%x(%u)\n", msg->sid, msg->data[0], msg->data[0]);
		return;  //Le message a déja été géré
	}

	
	// Traitement des autres messages reçus
	switch (msg->sid)
	{
		//Fin de la partie
		case BROADCAST_STOP_ALL :
			global.match_started = FALSE;
			COMPONENT_log(LOG_LEVEL_Info, "C:BROADCAST_STOP_ALL\n");
			QUEUE_flush_all();
			DCM_stop_all();
			break;
		
		//Reprise de la partie
		case BROADCAST_START :
			COMPONENT_log(LOG_LEVEL_Info, "C:BROADCAST_START\n");
			global.match_started = TRUE;
			break;

		case BROADCAST_POSITION_ROBOT:
			//Rien, mais pas inclus dans le cas default où l'on peut afficher le sid...
			break;
		
		default: 
			//debug_printf("SID:%x\r\n",msg->sid);
			COMPONENT_log(LOG_LEVEL_Trace, "Msg SID: 0x%x(%u)\n", msg->sid, msg->sid);
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
		//CAN_msg_t resultMsg = {ACT_RESULT, {msg->sid & 0xFF, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
		//CAN_send(&resultMsg);
		CAN_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
	}
}

//Envoie le message CAN de retour à la strat (et affiche des infos de debuggage si activé)
void CAN_sendResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode) {
	CAN_msg_t resultMsg;

	resultMsg.sid = ACT_RESULT;
	resultMsg.data[0] = originalSid & 0xFF;
	resultMsg.data[1] = originalCommand;
	resultMsg.data[2] = result;
	resultMsg.data[3] = errorCode;
	resultMsg.size = 4;

	CAN_send(&resultMsg);

	CAN_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_NoParam, 0);
}

//Comme CAN_sendResult mais ajoute un paramètre au message. Peut servir pour debuggage.
void CAN_sendResultWithParam(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 param) {
	CAN_msg_t resultMsg;

	resultMsg.sid = ACT_RESULT;
	resultMsg.data[0] = originalSid & 0xFF;
	resultMsg.data[1] = originalCommand;
	resultMsg.data[2] = result;
	resultMsg.data[3] = errorCode;
	resultMsg.data[4] = LOWINT(param);
	resultMsg.data[5] = HIGHINT(param);
	resultMsg.size = 6;

	CAN_send(&resultMsg);

	CAN_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_Normal, param);
}

//Comme CAN_sendResultWithParam mais le paramètre est considéré comme étant un numéro de ligne.
void CAN_sendResultWitExplicitLine(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 line) {
	CAN_msg_t resultMsg;

	resultMsg.sid = ACT_RESULT;
	resultMsg.data[0] = originalSid & 0xFF;
	resultMsg.data[1] = originalCommand;
	resultMsg.data[2] = result;
	resultMsg.data[3] = errorCode;
	resultMsg.data[4] = LOWINT(line);
	resultMsg.data[5] = HIGHINT(line);
	resultMsg.size = 6;

	CAN_send(&resultMsg);

	CAN_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_Line, line);
}

static void CAN_printResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, CAN_result_param_type_t paramType, Uint16 param) {
#ifdef OUTPUT_LOG
	const char* originalSidStr;
	const char* resultStr;
	const char* errorCodeStr;

	switch(originalSid) {
		case ACT_BALLINFLATER:       originalSidStr = "BallInflater"; break;
		case ACT_BALLLAUNCHER:       originalSidStr = "BallLauncher"; break;
		case ACT_BALLSORTER:         originalSidStr = "BallSorter";   break;
		case ACT_CANDLECOLOR:        originalSidStr = "CandleColor";  break;
		case ACT_HAMMER:             originalSidStr = "Hammer";       break;
		case ACT_LIFT_LEFT:          originalSidStr = "LiftLeft";     break;
		case ACT_LIFT_RIGHT:         originalSidStr = "LiftRight";    break;
		case ACT_PLATE:              originalSidStr = "Plate";        break;
		default:                     originalSidStr = "Unknown";      break;
	}
	switch(result) {
		case ACT_RESULT_DONE:        resultStr = "Done";       break;
		case ACT_RESULT_FAILED:      resultStr = "Failed";     break;
		case ACT_RESULT_NOT_HANDLED: resultStr = "NotHandled"; break;
		default:                     resultStr = "Unknown";    break;
	}
	switch(errorCode) {
		case ACT_RESULT_ERROR_LOGIC:        errorCodeStr = "Logic";         break;
		case ACT_RESULT_ERROR_NOT_HERE:     errorCodeStr = "NotHere";       break;
		case ACT_RESULT_ERROR_NO_RESOURCES: errorCodeStr = "NoResources";   break;
		case ACT_RESULT_ERROR_OK:           errorCodeStr = "Ok";            break;
		case ACT_RESULT_ERROR_OTHER:        errorCodeStr = "Other";         break;
		case ACT_RESULT_ERROR_TIMEOUT:      errorCodeStr = "Timeout";       break;
		case ACT_RESULT_ERROR_UNKNOWN:      errorCodeStr = "Unknown";       break;
		default:                            errorCodeStr = "Unknown error"; break;
	}
	log_level_e level = LOG_LEVEL_Debug;
	if(result != ACT_RESULT_DONE)
		level = LOG_LEVEL_Error;
	if(paramType == CAN_TPT_Normal) {
		COMPONENT_log(level, "Result msg: Act: %s(0x%x), cmd: 0x%x(%u), result: %s(%u), error: %s(%u), param: 0x%x(%u)\n",
			originalSidStr, originalSid & 0xFF,
			originalCommand, originalCommand,
			resultStr, result,
			errorCodeStr, errorCode,
			param, param);
	} else if(paramType == CAN_TPT_Line) {
		COMPONENT_log(level, "Result msg: Act: %s(0x%x), cmd: 0x%x(%u), result: %s(%u), error: %s(%u), line: %u\n",
			originalSidStr, originalSid & 0xFF,
			originalCommand, originalCommand,
			resultStr, result,
			errorCodeStr, errorCode,
			param);
	} else {
		COMPONENT_log(level, "Result msg: Act: %s(0x%x), cmd: 0x%x(%u), result: %s(%u), error: %s(%u)\n",
			originalSidStr, originalSid & 0xFF,
			originalCommand, originalCommand,
			resultStr, result,
			errorCodeStr, errorCode);
	}
#endif
}

//CAN is not enabled, declare functions so compilation won't fail
//FIXME: should be in QS_can.c ...
#ifndef USE_CAN
void CAN_send(CAN_msg_t* msg) {}

CAN_msg_t CAN_get_next_msg() {
	return ((CAN_msg_t){0});
}

bool_e CAN_data_ready() {
	return FALSE;
}

void CAN_set_direct_treatment_function(direct_treatment_function_pt func) {}
#endif
