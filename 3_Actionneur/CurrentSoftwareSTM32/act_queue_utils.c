/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : act_queue_utils.c
 *	Package : Carte Actionneur
 *	Description : Propose des fonctions pour gérer les actions avec la pile.
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
	CAN_TPT_NoParam, //Pas de paramètre
	CAN_TPT_Line,    //Le paramètre est une ligne
	CAN_TPT_Normal   //Le paramètre est un nombre normal sans signification particulière pour nous (ce code)
} CAN_result_param_type_t;
static void ACTQ_internal_printResult(Uint11 originalSid, Uint8 originalCommand, Uint8 result, Uint8 errorCode, CAN_result_param_type_t paramType, Uint16 param, bool_e sent_can_result);


//Met sur la pile une action qui sera gérée par act_function_ptr avec en paramètre param. L'action est protégée par semaphore avec act_id
//Cette fonction est appelée par les fonctions de traitement des messages CAN de chaque actionneur.
void ACTQ_push_operation_from_msg(CAN_msg_t* msg, QUEUE_act_e act_id, action_t act_function_ptr, Uint16 param, bool_e sendResult) {
	queue_id_t queueId = QUEUE_create();
	assert(queueId != QUEUE_CREATE_FAILED);
	if(queueId != QUEUE_CREATE_FAILED) {	//Si on est pas en verbose_mode, l'assert sera ignoré et la suite risque de vraiment planter ...
		QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, act_id);

		// Si nous voulons, nous envoyer des messages nous-même sans avertir les autres cartes
		if(sendResult == TRUE)
			QUEUE_add(queueId, act_function_ptr, (QUEUE_arg_t){msg->data.act_msg.order, param, &ACTQ_finish_SendResult}, act_id);
		else
			QUEUE_add(queueId, act_function_ptr, (QUEUE_arg_t){msg->data.act_msg.order, param, &ACTQ_finish_SendNothing}, act_id);

		QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, act_id);
	} else {	//on indique qu'on a pas géré la commande
		ACTQ_sendResult(msg->sid, msg->data.act_msg.order, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
	}
}

//Envoie le message CAN de retour à la strat (et affiche des infos de debuggage si activé)
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

//Comme CAN_sendResult mais ajoute un paramètre au message. Peut servir pour debuggage.
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

//Comme CAN_sendResultWithParam mais le paramètre est considéré comme étant un numéro de ligne.
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


void ACTQ_sendErrorAct(Uint8 id_act, Uint8 errorCode) {
	CAN_msg_t errorMsg;

	errorMsg.sid = ACT_ERROR;
	errorMsg.size = SIZE_ACT_ERROR_MSG;
	errorMsg.data.act_error.error_code = errorCode;
	errorMsg.data.act_error.idAct = id_act;
	CAN_send(&errorMsg);
}


bool_e ACTQ_check_status_ax12(queue_id_t queueId, Uint8 ax12Id, Uint16 wantedGoal, Uint16 currentGoal, Uint16 epsilon, Uint16 timeout_ms, Uint16 large_epsilon, Uint8* result, Uint8* error_code, Uint16* line) {
	AX12_reset_last_error(ax12Id);

	Uint8 error = AX12_get_last_error(ax12Id).error;
	Uint16 dummy;

	//Avec ça, on ne se soucie pas du contenu de result et error_code par la suite, on sait qu'ils ne sont pas NULL
	if(!result) result = (Uint8*)&dummy;
	if(!error_code) error_code = (Uint8*)&dummy;
	if(!line) line = &dummy;

	if(absolute((Sint16)currentGoal - (Sint16)(wantedGoal)) <= epsilon) {
		*result = ACT_RESULT_DONE;
		*error_code = ACT_RESULT_ERROR_OK;
		*line = 0x0100;
	} else if((error & AX12_ERROR_TIMEOUT) && (error & AX12_ERROR_RANGE)) {
		//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
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
		//Timeout, l'ax12 n'a pas bouger à la bonne position a temps
		if(absolute((Sint16)currentGoal - (Sint16)(wantedGoal)) <= large_epsilon) {
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0400;
		} else {
#warning ATTENTION MISE EN COMMENTAIRE DE LA PROTECTION DES AX12 SUR TIMEOUT (coté act)
			//AX12_set_torque_enabled(ax12Id, FALSE);
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0500;
		}
	} else if(error & AX12_ERROR_OVERHEATING) {
		//autres erreurs fiable, les autres on les teste pas car si elle arrive, c'est plus probablement un problème de transmission ou code ...
		AX12_set_torque_enabled(ax12Id, FALSE);
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_UNKNOWN;
		*line = error; //0x00xx avec xx = error
		ACTQ_sendErrorAct(ax12Id, ACT_ERROR_OVERHEATING);
	} else if(error) {
		component_printf(LOG_LEVEL_Error, "Error AX12 %d\n", error);
		return FALSE;
	} else return FALSE;

	return TRUE;
}

bool_e ACTQ_check_status_rx24(queue_id_t queueId, Uint8 rx24Id, Uint16 wantedGoal, Uint16 currentGoal, Uint16 epsilon, Uint16 timeout_ms, Uint16 large_epsilon, Uint8* result, Uint8* error_code, Uint16* line) {
	RX24_reset_last_error(rx24Id);

	Uint8 error = RX24_get_last_error(rx24Id).error;
	Uint16 dummy;

	//Avec ça, on ne se soucie pas du contenu de result et error_code par la suite, on sait qu'ils ne sont pas NULL
	if(!result) result = (Uint8*)&dummy;
	if(!error_code) error_code = (Uint8*)&dummy;
	if(!line) line = &dummy;

	if(absolute((Sint16)currentGoal - (Sint16)(wantedGoal)) <= epsilon) {
		*result = ACT_RESULT_DONE;
		*error_code = ACT_RESULT_ERROR_OK;
		*line = 0x0100;
	} else if((error & RX24_ERROR_TIMEOUT) && (error & RX24_ERROR_RANGE)) {
		//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
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
		//Timeout, le RX24 n'a pas bouger à la bonne position a temps
		if(absolute((Sint16)currentGoal - (Sint16)(wantedGoal)) <= large_epsilon) {
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0400;
		} else {
#warning ATTENTION MISE EN COMMENTAIRE DE LA PROTECTION DES RX24 SUR TIMEOUT (coté act)
			//RX24_set_torque_enabled(rx24Id, FALSE);
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_TIMEOUT;
			*line = 0x0500;
		}
	} else if(error & RX24_ERROR_OVERHEATING) {
		//autres erreurs fiable, les autres on les teste pas car si elle arrive, c'est plus probablement un problème de transmission ou code ...
		RX24_set_torque_enabled(rx24Id, FALSE);
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_UNKNOWN;
		*line = error; //0x00xx avec xx = error
		ACTQ_sendErrorAct(rx24Id,ACT_ERROR_OVERHEATING);
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

#ifdef USE_DC_MOTOR_SPEED
	bool_e ACTQ_check_status_dcMotorSpeed(DC_MOTOR_SPEED_id id, Uint8* result, Uint8* error_code, Uint16* line){
		DC_MOTOR_SPEED_state_e motorState = DC_MOTOR_SPEED_get_state(id);
		Uint16 dummy;

		if(!result) result = (Uint8*)&dummy;
		if(!error_code) error_code = (Uint8*)&dummy;
		if(!line) line = &dummy;

		switch(motorState){
			case DC_MOTOR_SPEED_IDLE:
				*result =    ACT_RESULT_NOT_HANDLED;
				*error_code = ACT_RESULT_ERROR_CANCELED;
				*line = __LINE__;
				break;

			case DC_MOTOR_SPEED_RUN:
				*result =    ACT_RESULT_DONE;
				*error_code = ACT_RESULT_ERROR_OK;
				*line = __LINE__;
				break;

			case DC_MOTOR_SPEED_ERROR:
				*result =    ACT_RESULT_FAILED;
				*error_code = ACT_RESULT_ERROR_TIMEOUT;
				*line = __LINE__;
				break;

			default:
			case DC_MOTOR_SPEED_INIT_RECOVERY:
			case DC_MOTOR_SPEED_INIT_LAUNCH:
			case DC_MOTOR_SPEED_LAUNCH_RECOVERY:
			case DC_MOTOR_SPEED_LAUNCH:
			case DC_MOTOR_SPEED_RUN_RECOVERY:
				// Waiting
				return FALSE;
		}
		return TRUE;
	}
#endif

bool_e ACTQ_check_timeout(queue_id_t queueId, time32_t timeout_ms) {
	if(global.absolute_time >= QUEUE_get_initial_time(queueId) + timeout_ms)
		return TRUE;
	return FALSE;
}

//Renvoie un retour à la strat dans tous les cas
bool_e ACTQ_finish_SendResult(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	ACTQ_sendResultWithParam(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, param);

	if(result != ACT_RESULT_DONE)
		return FALSE;

	return TRUE;
}

//Retour à la strat seulement si l'opération à fail
bool_e ACTQ_finish_SendResultIfFail(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	if(result != ACT_RESULT_DONE && error_code != ACT_RESULT_ERROR_OTHER) {
		ACTQ_sendResultWithParam(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, param);
		return FALSE;
	} else ACTQ_internal_printResult(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, CAN_TPT_Line, param, FALSE);

	return TRUE;
}

//Retour à la strat seulement si l'opération à reussi
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
           case ACT_BIG_BALL_BACK_LEFT & 0xFF:      	originalSidStr = "BigBallBackLeft";     	break;
           case ACT_BIG_BALL_BACK_RIGHT & 0xFF:     	originalSidStr = "BigBallBackRight";    	break;
           case ACT_BIG_BALL_FRONT_LEFT & 0xFF:     	originalSidStr = "BigBallFrontLeft";    	break;
           case ACT_BIG_BALL_FRONT_RIGHT & 0xFF:    	originalSidStr = "BigBallFrontRight";		break;
           case ACT_BEARING_BALL_WHEEL & 0xFF:      	originalSidStr = "BearingBallWheel";		break;

           case ACT_CYLINDER_SLOPE_LEFT & 0xFF:			originalSidStr = "CylinderSlopeLeft";		break;
		   case ACT_CYLINDER_SLOPE_RIGHT & 0xFF:		originalSidStr = "CylinderSlopeRight";		break;
		   case ACT_CYLINDER_BALANCER_LEFT & 0xFF:		originalSidStr = "CylinderBalancerLeft";	break;
		   case ACT_CYLINDER_BALANCER_RIGHT & 0xFF:		originalSidStr = "CylinderBalancerRight";	break;
           case ACT_CYLINDER_PUSHER_LEFT & 0xFF:		originalSidStr = "CylinderPusherLeft";		break;
           case ACT_CYLINDER_PUSHER_RIGHT & 0xFF:   	originalSidStr = "CylinderPusherRight";		break;
           case ACT_CYLINDER_ELEVATOR_LEFT & 0xFF:		originalSidStr = "CylinderElevatorLeft";	break;
           case ACT_CYLINDER_ELEVATOR_RIGHT & 0xFF: 	originalSidStr = "CylinderElevatorRight";	break;
           case ACT_CYLINDER_SLIDER_LEFT & 0xFF:		originalSidStr = "CylinderSliderLeft";		break;
           case ACT_CYLINDER_SLIDER_RIGHT & 0xFF:   	originalSidStr = "CylinderSliderRight";		break;
           case ACT_CYLINDER_ARM_LEFT & 0xFF:			originalSidStr = "CylinderArmLeft";			break;
           case ACT_CYLINDER_ARM_RIGHT & 0xFF: 			originalSidStr = "CylinderArmRight";		break;
           case ACT_CYLINDER_COLOR_LEFT & 0xFF:			originalSidStr = "CylinderColorLeft";		break;
		   case ACT_CYLINDER_COLOR_RIGHT & 0xFF:		originalSidStr = "CylinderColorRight";		break;
		   case ACT_CYLINDER_DISPOSE_LEFT & 0xFF:		originalSidStr = "CylinderDispenserLeft";	break;
		   case ACT_CYLINDER_DISPOSE_RIGHT & 0xFF:		originalSidStr = "CylinderDispenserRight";	break;

           case ACT_ORE_GUN & 0xFF:						originalSidStr = "OreGun";					break;
           case ACT_ORE_WALL & 0xFF:					originalSidStr = "OreWall";					break;
           case ACT_ORE_ROLLER_ARM & 0xFF:				originalSidStr = "OreRollerArm";       	 	break;

           case ACT_ROCKET & 0xFF:                  	originalSidStr = "Rocket";             	 	break;


           // Anne
          case ACT_SMALL_BALL_BACK & 0xFF:				originalSidStr = "SmallBallBack";		break;
          case ACT_SMALL_BALL_FRONT_LEFT & 0xFF:		originalSidStr = "SmallBallFrontLeft";		break;
          case ACT_SMALL_BALL_FRONT_RIGHT & 0xFF:		originalSidStr = "SmallBallFrontRight";		break;
          case ACT_SMALL_CYLINDER_ARM & 0xFF:			originalSidStr = "SmallCylinderArm";		break;
          case ACT_SMALL_CYLINDER_BALANCER & 0xFF:		originalSidStr = "SmallCylinderBalancer";	break;
          case ACT_SMALL_CYLINDER_COLOR & 0xFF:			originalSidStr = "SmallCylinderColor";		break;
          case ACT_SMALL_CYLINDER_DISPOSE & 0xFF:		originalSidStr = "SmallCylinderDispose";	break;
          case ACT_SMALL_CYLINDER_ELEVATOR & 0xFF:		originalSidStr = "SmallCylinderElevator";	break;
          case ACT_SMALL_CYLINDER_SLIDER & 0xFF:		originalSidStr = "SmallCylinderSlider";		break;
          case ACT_SMALL_CYLINDER_SLOPE & 0xFF:			originalSidStr = "SmallCylinderSlope";		break;
          case ACT_SMALL_CYLINDER_MULTIFONCTION & 0xFF:	originalSidStr = "SmallCylinderMultifonction";	break;
          case ACT_SMALL_MAGIC_ARM & 0xFF:				originalSidStr = "SmallMagicArm";			break;
          case ACT_SMALL_MAGIC_COLOR & 0xFF:			originalSidStr = "SmallMagicColor";			break;
          case ACT_SMALL_ORE & 0xFF:					originalSidStr = "SmallOre";				break;
          case ACT_SMALL_POMPE_DISPOSE & 0xFF:			originalSidStr = "SmallPompeDispose";		break;
          case ACT_SMALL_POMPE_PRISE & 0xFF:			originalSidStr = "SmallPompePrise";			break;



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
