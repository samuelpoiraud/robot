/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Plate.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'assiette contenant les cerises
 *  Auteur : Alexis
 *  Version 20130219
 *  Robot : Krusty
 */

#include "KPlate.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../output_log.h"

#define LOG_PREFIX "PL: "

#define PLATE_NUM_POS           4
	#define PLATE_PARKED_POS_ID     0
	#define PLATE_HORIZONTAL_POS_ID 1
	#define PLATE_PREPARE_POS_ID    2
	#define PLATE_VERTICAL_POS_ID   3

#define PLATE_PLIER_AX12_NUM_POS       3
	#define PLATE_PLIER_AX12_PARKED_POS_ID 0
	#define PLATE_PLIER_AX12_OPEN_POS_ID   1
	#define PLATE_PLIER_AX12_CLOSED_POS_ID 2

static DCMotor_config_t plate_rotation_config;

static Sint16 PLATE_getRotationAngle();

void PLATE_init() {
	plate_rotation_config.sensor_read = &PLATE_getRotationAngle;
	plate_rotation_config.Kp = PLATE_ASSER_KP;
	plate_rotation_config.Ki = PLATE_ASSER_KI;
	plate_rotation_config.Kd = PLATE_ASSER_KD;
	plate_rotation_config.pos[PLATE_PARKED_POS_ID] = PLATE_PARKED_POS;
	plate_rotation_config.pos[PLATE_HORIZONTAL_POS_ID] = PLATE_HORIZONTAL_POS;
	plate_rotation_config.pos[PLATE_PREPARE_POS_ID] = PLATE_PREPARE_POS;
	plate_rotation_config.pos[PLATE_VERTICAL_POS_ID] = PLATE_VERTICAL_POS;
	plate_rotation_config.pwm_number = PLATE_DCMOTOR_PWM_NUM;
	plate_rotation_config.way_latch = (Uint16*)&PLATE_DCMOTOR_PORT_WAY;
	plate_rotation_config.way_bit_number = PLATE_DCMOTOR_PORT_WAY_BIT;
	plate_rotation_config.way0_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY0;
	plate_rotation_config.way1_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY1;
	plate_rotation_config.timeout = 2000;
	plate_rotation_config.epsilon = 50;	//TODO: à ajuster
	DCM_config(PLATE_DCMOTOR_ID, &plate_rotation_config);

	OUTPUTLOG_printf(LOG_LEVEL_Info, LOG_PREFIX"actionneur assiette initialisé\n");
}

//For queues, optionnal_arg is the wanted position id
void PLATE_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_PLATE) {
		switch(msg->data[0]) {
			case ACT_PLATE_PLIER_CLOSE:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {	//Si on est pas en verbose_mode, l'assert sera ignoré et la suite risque de vraiment planter ...
					QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &PLATE_run_command, ACT_PLATE_PLIER_CLOSE, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, ACT_PLATE_PLIER_CLOSE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			case ACT_PLATE_PLIER_OPEN:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &PLATE_run_command, ACT_PLATE_PLIER_OPEN, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, ACT_PLATE_PLIER_OPEN, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			case ACT_PLATE_ROTATE_HORIZONTALLY:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &PLATE_run_command, ACT_PLATE_ROTATE_HORIZONTALLY, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_Rotation);
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, ACT_PLATE_ROTATE_HORIZONTALLY, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			case ACT_PLATE_ROTATE_VERTICALLY:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &PLATE_run_command, ACT_PLATE_ROTATE_VERTICALLY, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_Rotation);
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, ACT_PLATE_ROTATE_VERTICALLY, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			//Cas d'urgence, inutile donc normalement :)
			case ACT_PLATE_PLIER_STOP:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					//QUEUE_flush()
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, ACT_PLATE_PLIER_STOP, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			case ACT_PLATE_ROTATE_STOP:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					//QUEUE_flush()
				} else {	//on indique qu'on a pas géré la commande
					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, ACT_PLATE_ROTATE_STOP, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
					CAN_send(&resultMsg);
				}
				break;

			default:
				OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"invalid CAN msg data[0]=%hhu !\n", msg->data[0]);
		}
	}
}

void PLATE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_Rotation) {
		if(init && !QUEUE_has_error(queueId)) {
			Sint16 command = QUEUE_get_arg(queueId);
			Uint8 wantedPosition;
			switch(command) {
				case ACT_PLATE_ROTATE_HORIZONTALLY: wantedPosition = PLATE_HORIZONTAL_POS_ID; break;
				case ACT_PLATE_ROTATE_VERTICALLY:   wantedPosition = PLATE_VERTICAL_POS_ID;   break;
				default: {
						CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
						CAN_send(&resultMsg);
						OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid rotation command: %hhu, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}
			DCM_goToPos(PLATE_DCMOTOR_ID, wantedPosition);
		} else {
			DCM_working_state_e asserState = DCM_get_state(PLATE_DCMOTOR_ID);
			CAN_msg_t resultMsg;

			if(QUEUE_has_error(queueId)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
			} else if(asserState == DCM_TIMEOUT) {
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_TIMEOUT;
				QUEUE_set_error(queueId);
			} else if(asserState == DCM_IDLE) {
				resultMsg.data[2] = ACT_RESULT_DONE;
				resultMsg.data[3] = ACT_RESULT_ERROR_OK;
			} else return;	//Operation is not finished, do nothing

			resultMsg.sid = ACT_RESULT;
			resultMsg.data[0] = ACT_PLATE & 0xFF;
			resultMsg.data[1] = QUEUE_get_arg(queueId);
			resultMsg.size = 4;

			CAN_send(&resultMsg);
			QUEUE_behead(queueId);	//gestion terminée
		}
	} else if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_AX12_Plier) {
		static Uint16 goalPosition;
		if(init && !QUEUE_has_error(queueId)) {
			Sint16 command = QUEUE_get_arg(queueId);
			bool_e cmdOk;

			AX12_reset_last_error(PLATE_PLIER_AX12_ID);
			switch(command) {
				case ACT_PLATE_PLIER_OPEN:  goalPosition = PLATE_PLIER_AX12_OPEN_POS; break;
				case ACT_PLATE_PLIER_CLOSE: goalPosition = PLATE_PLIER_AX12_CLOSED_POS; break;
				default: {
						CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
						CAN_send(&resultMsg);
						OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid plier command: %hhu, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}
			cmdOk = AX12_set_position(PLATE_PLIER_AX12_ID, goalPosition);
			if(!cmdOk) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
				CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE}, 4};
				CAN_send(&resultMsg);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
			//La commande a été envoyée et l'AX12 l'a bien reçu
		} else {
			Uint16 asserState;
			Uint16 ax12Pos;
			CAN_msg_t resultMsg;

			AX12_reset_last_error(PLATE_PLIER_AX12_ID);
			ax12Pos = AX12_get_position(PLATE_PLIER_AX12_ID);
			asserState = AX12_get_last_error(PLATE_PLIER_AX12_ID).error;

			if(QUEUE_has_error(queueId)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
			} else if((asserState & AX12_ERROR_TIMEOUT) && (asserState & AX12_ERROR_RANGE)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_LOGIC;	//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
				QUEUE_set_error(queueId);
			} else if(asserState & AX12_ERROR_TIMEOUT) {	//L'ax12 n'a pas répondu à la commande
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_NOT_HERE;

				//FIXME: A faire en strat ou ici ?
				//AX12_set_position(PLATE_PLIER_AX12_ID, PLATE_PLIER_AX12_OPEN_POS);	//dans le doute on ouvre la pince, histoire de pas prendre l'assiette en déplacement, si jamais l'AX12 reçoi bien les instructions
			} else if(asserState) {							//autres erreurs
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_UNKNOWN;
			} else if(abs((Sint16)ax12Pos - (Sint16)goalPosition) <= PLATE_PLIER_AX12_POS_EPSILON) {	//Fin du mouvement
				resultMsg.data[2] = ACT_RESULT_DONE;
				resultMsg.data[3] = ACT_RESULT_ERROR_OK;
			} else return;	//Operation is not finished, do nothing

			resultMsg.sid = ACT_RESULT;
			resultMsg.data[0] = ACT_PLATE & 0xFF;
			resultMsg.data[1] = QUEUE_get_arg(queueId);
			resultMsg.size = 4;

			CAN_send(&resultMsg);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

static Sint16 PLATE_getRotationAngle() {
	return 0;
}

#endif  /* I_AM_ROBOT_KRUSTY */
