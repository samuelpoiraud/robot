/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : act_queue_utils.c
 *	Package : Carte Actionneur
 *	Description : Propose des fonctions pour g�rer les actions avec la pile.
 *	Auteur : Alexis
 *	Version 20130420
 */

#include "act_queue_utils.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"
#include "QS/QS_ax12.h"
#include "QS/QS_rx24.h"
#include "QS/QS_DCMotor2.h"

#define LOG_PREFIX "ActUtils: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTQUEUEUTILS
#include "QS/QS_outputlog.h"

typedef enum {
	CAN_TPT_NoParam, //Pas de param�tre
	CAN_TPT_Line,    //Le param�tre est une ligne
	CAN_TPT_Normal   //Le param�tre est un nombre normal sans signification particuli�re pour nous (ce code)
} CAN_result_param_type_t;
static void ACTQ_internal_printResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, CAN_result_param_type_t paramType, Uint16 param, bool_e sent_can_result);


//Met sur la pile une action qui sera g�r�e par act_function_ptr avec en param�tre param. L'action est prot�g�e par semaphore avec act_id
//Cette fonction est appel�e par les fonctions de traitement des messages CAN de chaque actionneur.
void ACTQ_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Uint16 param, bool_e sendResult) {
	queue_id_t queueId = QUEUE_create();
	assert(queueId != QUEUE_CREATE_FAILED);
	if(queueId != QUEUE_CREATE_FAILED) {	//Si on est pas en verbose_mode, l'assert sera ignor� et la suite risque de vraiment planter ...
		QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, act_id);

		// Si nous voulons, nous envoyer des messages nous-m�me sans avertir les autres cartes
		if(sendResult == TRUE)
			QUEUE_add(queueId, act_function_ptr, (QUEUE_arg_t){msg->data.act_msg.order, param, &ACTQ_finish_SendResult}, act_id);
		else
			QUEUE_add(queueId, act_function_ptr, (QUEUE_arg_t){msg->data.act_msg.order, param, &ACTQ_finish_SendNothing}, act_id);

		QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, act_id);
	} else {	//on indique qu'on a pas g�r� la commande
		ACTQ_sendResult(msg->sid, msg->data.act_msg.order, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
	}
}

//Envoie le message CAN de retour � la strat (et affiche des infos de debuggage si activ�)
void ACTQ_sendResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode) {
	CAN_msg_t resultMsg;

	if(global.flags.match_started == TRUE) {
		resultMsg.sid = ACT_RESULT;
		resultMsg.size = SIZE_ACT_RESULT;
		resultMsg.data.act_result.sid = originalSid & 0xFF;
		resultMsg.data.act_result.cmd = originalCommand;
		resultMsg.data.act_result.result = result;
		resultMsg.data.act_result.error_code = errorCode;
		CAN_send(&resultMsg);
	}

	ACTQ_internal_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_NoParam, 0, TRUE);
}

//Comme CAN_sendResult mais ajoute un param�tre au message. Peut servir pour debuggage.
void ACTQ_sendResultWithParam(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint32 param) {
	CAN_msg_t resultMsg;

	resultMsg.sid = ACT_RESULT;
	resultMsg.size = SIZE_ACT_RESULT;
	resultMsg.data.act_result.sid = originalSid & 0xFF;
	resultMsg.data.act_result.cmd = originalCommand;
	resultMsg.data.act_result.result = result;
	resultMsg.data.act_result.error_code = errorCode;
	resultMsg.data.act_result.param = param;
	CAN_send(&resultMsg);

	ACTQ_internal_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_Normal, param, TRUE);
}

//Comme CAN_sendResultWithParam mais le param�tre est consid�r� comme �tant un num�ro de ligne.
void ACTQ_sendResultWitExplicitLine(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 line) {
	CAN_msg_t resultMsg;

	resultMsg.sid = ACT_RESULT;
	resultMsg.size = SIZE_ACT_RESULT;
	resultMsg.data.act_result.sid = originalSid & 0xFF;
	resultMsg.data.act_result.cmd = originalCommand;
	resultMsg.data.act_result.result = result;
	resultMsg.data.act_result.error_code = errorCode;
	resultMsg.data.act_result.param = line;
	CAN_send(&resultMsg);

	ACTQ_internal_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_Line, line, TRUE);
}

bool_e ACTQ_check_status_ax12(queue_id_t queueId, Uint8 ax12Id, Uint16 wantedPosition, Uint16 pos_epsilon, Uint16 timeout_ms, Uint16 large_epsilon, Uint8* result, Uint8* error_code, Uint16* line) {
	AX12_reset_last_error(ax12Id);

	Uint16 current_pos = AX12_get_position(ax12Id);
	Uint8 error = AX12_get_last_error(ax12Id).error;
	Uint16 dummy;

	//Avec �a, on ne se soucie pas du contenu de result et error_code par la suite, on sait qu'ils ne sont pas NULL
	if(!result) result = (Uint8*)&dummy;
	if(!error_code) error_code = (Uint8*)&dummy;
	if(!line) line = &dummy;

	if(absolute((Sint16)current_pos - (Sint16)(wantedPosition)) <= pos_epsilon) {
		*result = ACT_RESULT_DONE;
		*error_code = ACT_RESULT_ERROR_OK;
		*line = 0x0100;
	} else if((error & AX12_ERROR_TIMEOUT) && (error & AX12_ERROR_RANGE)) {
		//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoy�es)
		AX12_set_torque_enabled(ax12Id, FALSE);
		*result = ACT_RESULT_NOT_HANDLED;
		*error_code = ACT_RESULT_ERROR_LOGIC;
		*line = 0x0200;
	} else if(error & AX12_ERROR_TIMEOUT) {
		//AX12_set_torque_enabled(ax12Id, FALSE);
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_NOT_HERE;
		*line = 0x0300;
	} else if(ACTQ_check_timeout(queueId, timeout_ms)) {
		//Timeout, l'ax12 n'a pas bouger � la bonne position a temps
		if(absolute((Sint16)current_pos - (Sint16)(wantedPosition)) <= large_epsilon) {
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0400;
		} else {
#warning ATTENTION MISE EN COMMENTAIRE DE LA PROTECTION DES AX12 SUR TIMEOUT (cot� act)
			//AX12_set_torque_enabled(ax12Id, FALSE);
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0500;
		}
	} else if(error & AX12_ERROR_OVERHEATING) {
		//autres erreurs fiable, les autres on les teste pas car si elle arrive, c'est plus probablement un probl�me de transmission ou code ...
		AX12_set_torque_enabled(ax12Id, FALSE);
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_UNKNOWN;
		*line = error; //0x00xx avec xx = error
	} else if(error) {
		component_printf(LOG_LEVEL_Error, "Error AX12 %d\n", error);
		return FALSE;
	} else return FALSE;

	return TRUE;
}

bool_e ACTQ_check_status_rx24(queue_id_t queueId, Uint8 rx24Id, Uint16 wantedPosition, Uint16 pos_epsilon, Uint16 timeout_ms, Uint16 large_epsilon, Uint8* result, Uint8* error_code, Uint16* line) {
	RX24_reset_last_error(rx24Id);

	Uint16 current_pos = RX24_get_position(rx24Id);
	Uint8 error = RX24_get_last_error(rx24Id).error;
	Uint16 dummy;

	//Avec �a, on ne se soucie pas du contenu de result et error_code par la suite, on sait qu'ils ne sont pas NULL
	if(!result) result = (Uint8*)&dummy;
	if(!error_code) error_code = (Uint8*)&dummy;
	if(!line) line = &dummy;

	if(absolute((Sint16)current_pos - (Sint16)(wantedPosition)) <= pos_epsilon) {
		*result = ACT_RESULT_DONE;
		*error_code = ACT_RESULT_ERROR_OK;
		*line = 0x0100;
	} else if((error & RX24_ERROR_TIMEOUT) && (error & RX24_ERROR_RANGE)) {
		//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoy�es)
		RX24_set_torque_enabled(rx24Id, FALSE);
		*result = ACT_RESULT_NOT_HANDLED;
		*error_code = ACT_RESULT_ERROR_LOGIC;
		*line = 0x0200;
	} else if(error & RX24_ERROR_TIMEOUT) {
		//RX24_set_torque_enabled(rx24Id, FALSE);
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_NOT_HERE;
		*line = 0x0300;
	} else if(ACTQ_check_timeout(queueId, timeout_ms)) {
		//Timeout, le RX24 n'a pas bouger � la bonne position a temps
		if(absolute((Sint16)current_pos - (Sint16)(wantedPosition)) <= large_epsilon) {
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0400;
		} else {
#warning ATTENTION MISE EN COMMENTAIRE DE LA PROTECTION DES RX24 SUR TIMEOUT (cot� act)
			//RX24_set_torque_enabled(rx24Id, FALSE);
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0500;
		}
	} else if(error & RX24_ERROR_OVERHEATING) {
		//autres erreurs fiable, les autres on les teste pas car si elle arrive, c'est plus probablement un probl�me de transmission ou code ...
		RX24_set_torque_enabled(rx24Id, FALSE);
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_UNKNOWN;
		*line = error; //0x00xx avec xx = error
	} else if(error) {
		component_printf(LOG_LEVEL_Error, "Error RX24 %d\n", error);
		return FALSE;
	} else return FALSE;

	return TRUE;
}

#ifdef USE_DCMOTOR2
bool_e ACTQ_check_status_dcmotor(Uint8 dcmotor_id, bool_e timeout_is_ok, Uint8* result, Uint8* error_code, Uint16* line) {
	DCM_working_state_e asserState = DCM_get_state(dcmotor_id);
	Uint16 dummy;

	if(!result) result = (Uint8*)&dummy;
	if(!error_code) error_code = (Uint8*)&dummy;
	if(!line) line = &dummy;

	if(asserState == DCM_IDLE) {
		*result =    ACT_RESULT_DONE;
		*error_code = ACT_RESULT_ERROR_OK;
		*line = __LINE__;
	} else if(asserState == DCM_TIMEOUT) {
		if(timeout_is_ok) {
			*result =    ACT_RESULT_DONE;
			*error_code = ACT_RESULT_ERROR_OK;
			*line = __LINE__;
		} else {
			*result =    ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = __LINE__;
		}
	} else return FALSE;

	return TRUE;
}
#endif

bool_e ACTQ_check_timeout(queue_id_t queueId, time32_t timeout_ms) {
	if(global.absolute_time >= QUEUE_get_initial_time(queueId) + timeout_ms)
		return TRUE;
	return FALSE;
}

//Renvoie un retour � la strat dans tous les cas
bool_e ACTQ_finish_SendResult(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	ACTQ_sendResultWithParam(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, param);

	if(result != ACT_RESULT_DONE)
		return FALSE;

	return TRUE;
}

//Retour � la strat seulement si l'op�ration � fail
bool_e ACTQ_finish_SendResultIfFail(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	if(result != ACT_RESULT_DONE && error_code != ACT_RESULT_ERROR_OTHER) {
		ACTQ_sendResultWithParam(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, param);
		return FALSE;
	} else ACTQ_internal_printResult(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, CAN_TPT_Line, param, FALSE);

	return TRUE;
}

//Retour � la strat seulement si l'op�ration � reussi
bool_e ACTQ_finish_SendResultIfSuccess(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	if(result == ACT_RESULT_DONE) {
		ACTQ_sendResultWithParam(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, param);
		return TRUE;
	} else ACTQ_internal_printResult(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, CAN_TPT_Line, param, FALSE);

	return FALSE;
}

//Ne fait aucun retour
bool_e ACTQ_finish_SendNothing(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	ACTQ_internal_printResult(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, CAN_TPT_Line, param, FALSE);
	if(result == ACT_RESULT_DONE)
		return TRUE;

	return FALSE;
}

void ACTQ_printResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, Uint16 param) {
	ACTQ_internal_printResult(originalSid, originalCommand, result, errorCode, CAN_TPT_Normal, param, FALSE);
}

static void ACTQ_internal_printResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, CAN_result_param_type_t paramType, Uint16 param, bool_e sent_can_result) {
#ifdef VERBOSE_MODE
	const char* originalSidStr = "Unknown";
	const char* resultStr = "Unknown";
	const char* errorCodeStr = "Unknown error";
	const char* sendedStr = (sent_can_result == TRUE)? "sent" : "not sent";

	originalSid = originalSid & 0xFF;
	switch(originalSid) {
		// Harry
		   //case ACT_FISH_MAGNETIC_ARM & 0xFF:		originalSidStr = "FishMagnetic";		break;
		   case ACT_ORE_GUN & 0xFF:					originalSidStr = "OreGun";				break;
		   case ACT_ORE_SHOVEL & 0xFF:				originalSidStr = "OreShovel";			break;
		   case ACT_ORE_CATCH & 0xFF:				originalSidStr = "OreCatch";			break;
		   case ACT_ORE_MIXER & 0xFF:				originalSidStr = "OreMixer";			break;
		   case ACT_ORE_SWEEPER & 0xFF:				originalSidStr = "OreSweeper";			break;
           case ACT_BIG_BALL_BACK_LEFT & 0xFF:      originalSidStr = "BigBallBackLeft";     break;
           case ACT_BIG_BALL_BACK_RIGHT & 0xFF:     originalSidStr = "BigBallBackRight";    break;
           case ACT_BIG_BALL_FRONT_LEFT & 0xFF:     originalSidStr = "BigBallFrontLeft";    break;
           case ACT_BIG_BALL_FRONT_RIGHT & 0xFF:    originalSidStr = "BigBallFrontRight";	break;
		   case ACT_CYLINDER_HATCH_LEFT & 0xFF:		originalSidStr = "CylinderHatchLeft";	break;
		   case ACT_CYLINDER_HATCH_RIGHT & 0xFF:	originalSidStr = "CylinderHatchRight";	break;
		   case ACT_CYLINDER_SLOPE_LEFT & 0xFF:		originalSidStr = "CylinderSlopeLeft";	break;
		   case ACT_CYLINDER_SLOPE_RIGHT & 0xFF:	originalSidStr = "CylinderSlopeRight";	break;
		   case ACT_CYLINDER_DISPENSER_LEFT & 0xFF:	originalSidStr = "CylinderDispenserLeft";	break;
		   case ACT_CYLINDER_DISPENSER_RIGHT & 0xFF:originalSidStr = "CylinderDispenserRight";	break;
           case ACT_BEARING_BALL_WHEEL & 0xFF:      originalSidStr = "BearingBallWheel";	break;
           case ACT_CYLINDER_PUSHER_LEFT & 0xFF:	originalSidStr = "CylinderPusherLeft";	break;
           case ACT_CYLINDER_PUSHER_RIGHT & 0xFF:   originalSidStr = "CylinderPusherRight";	break;
           case ACT_CYLINDER_ELEVATOR_LEFT & 0xFF:	originalSidStr = "CylinderElevatorLeft";	break;
           case ACT_CYLINDER_ELEVATOR_RIGHT & 0xFF: originalSidStr = "CylinderElevatorRight";	break;
           case ACT_CYLINDER_SLIDER_LEFT & 0xFF:	originalSidStr = "CylinderSliderLeft";	break;
           case ACT_CYLINDER_SLIDER_RIGHT & 0xFF:   originalSidStr = "CylinderSliderRight";	break;
           case ACT_CYLINDER_TURN_LEFT_ARM & 0xFF:	originalSidStr = "CylinderTurnLeftArm";	break;
           case ACT_CYLINDER_TURN_RIGHT_ARM & 0xFF: originalSidStr = "CylinderTurnRightArm";break;

		// Anne
		//case ACT_VENTILATOR_PEARL & 0xFF:        originalSidStr = "VentilatorPearl";     break;
		default:								    originalSidStr = "Unknown";				break;
	}
	switch(result) {
		case ACT_RESULT_DONE:        resultStr = "Done";       break;
		case ACT_RESULT_FAILED:      resultStr = "Failed";     break;
		case ACT_RESULT_NOT_HANDLED: resultStr = "NotHandled"; break;
		default:                     resultStr = "Unknown";    break;
	}
	switch(errorCode) {
		case ACT_RESULT_ERROR_LOGIC:        errorCodeStr = "Logic";				break;
		case ACT_RESULT_ERROR_NOT_HERE:     errorCodeStr = "NotHere";			break;
		case ACT_RESULT_ERROR_NO_RESOURCES: errorCodeStr = "NoResources";		break;
		case ACT_RESULT_ERROR_OK:           errorCodeStr = "Ok";				break;
		case ACT_RESULT_ERROR_OTHER:        errorCodeStr = "Other";				break;
		case ACT_RESULT_ERROR_TIMEOUT:      errorCodeStr = "Timeout";			break;
		case ACT_RESULT_ERROR_INVALID_ARG:  errorCodeStr = "Invalid argument";	break;
		case ACT_RESULT_ERROR_UNKNOWN:      errorCodeStr = "Unknown";			break;
		case ACT_RESULT_ERROR_CANCELED:		errorCodeStr = "Cancelled";			break;
		default:                            errorCodeStr = "Unknown error";		break;
	}
	log_level_e level = LOG_LEVEL_Debug;
	if(result != ACT_RESULT_DONE)
		level = LOG_LEVEL_Error;
	if(paramType == CAN_TPT_Normal) {
		component_printf(level, "Result msg: Act: %s(0x%x), cmd: 0x%x(%u), result: %s(%u), error: %s(%u), param: 0x%x(%u) (%s)\n\n",
			originalSidStr, originalSid & 0xFF,
			originalCommand, originalCommand,
			resultStr, result,
			errorCodeStr, errorCode,
			param, param,
			sendedStr);
	} else if(paramType == CAN_TPT_Line) {
		component_printf(level, "Result msg: Act: %s(0x%x), cmd: 0x%x(%u), result: %s(%u), error: %s(%u), line: %u (%s)\n\n",
			originalSidStr, originalSid & 0xFF,
			originalCommand, originalCommand,
			resultStr, result,
			errorCodeStr, errorCode,
			param,
			sendedStr);
	} else {
		component_printf(level, "Result msg: Act: %s(0x%x), cmd: 0x%x(%u), result: %s(%u), error: %s(%u) (%s)\n\n",
			originalSidStr, originalSid & 0xFF,
			originalCommand, originalCommand,
			resultStr, result,
			errorCodeStr, errorCode,
			sendedStr);
	}
#endif
}
