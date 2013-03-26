/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Ball_sorter.c
 *	Package : Carte actionneur
 *	Description : Gestion de la detection des balles cerises et de leur envoi une par une.
 *  Auteur : Alexis
 *  Version 20130315
 *  Robot : Krusty
 */

#include "KBall_sorter.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_adc.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"

#define LOG_PREFIX "BS: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_BALLSORTER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

//Etape d'un passage de cerise (demandé par la strat)
typedef enum {
	BALLSORTER_CS_EjectCherry = 0,     //Ejecte la cerise prise
	BALLSORTER_CS_GotoNextCherry = 1,  //Va prendre une nouvelle cerise dans le bac
	BALLSORTER_CS_TakeCherry = 2,      //Met la cerise prise devant le capteur
	BALLSORTER_CS_DetectCherry = 3     //Detecte la cerise pour savoir si elle est blanche ou pas et renvoi le resultat à la strat
} BALLSORTER_command_state_e;

static void BALLSORTER_initAX12();
static void BALLSORTER_run_command(queue_id_t queueId, bool_e init);

void BALLSORTER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	ADC_init();

	BALLSORTER_initAX12();

	COMPONENT_log(LOG_LEVEL_Info, "Séquenceur de cerise initialisé\n");
	//a faire: RAM de cerise, registres de cerise, ALU de cerise, GPIO de cerise, bras de cerise, pieds de cerise, firmware groupama(c)(r)(tm) 3.1.2 de cerise
	//groupama est une marque déposée sur le bord de la route actuellement orpheline, pour des demandes d'adoption, veuillez vous renseigner à: www.jadoptemabankdememoire.com
}

//Initialise l'AX12 s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void BALLSORTER_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(BALLSORTER_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(BALLSORTER_AX12_ID, 136);
		AX12_config_set_lowest_voltage(BALLSORTER_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(BALLSORTER_AX12_ID, BALLSORTER_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(BALLSORTER_AX12_ID, 300);
		AX12_config_set_minimal_angle(BALLSORTER_AX12_ID, 0);

		AX12_config_set_error_before_led(BALLSORTER_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(BALLSORTER_AX12_ID, AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD);
		COMPONENT_log(LOG_LEVEL_Info, "AX12 initialisé\n");
	}
}

bool_e BALLSORTER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;

	if(msg->sid == ACT_BALLSORTER) {
		//Initialise l'AX12 s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		BALLSORTER_initAX12();

		switch(msg->data[0]) {
			case ACT_BALLSORTER_TAKE_NEXT_CHERRY:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_EjectCherry}   , QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_GotoNextCherry}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_TakeCherry}    , QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_DetectCherry}  , QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0}, QUEUE_ACT_BallSorter);
				} else {	//on indique qu'on a pas géré la commande
//					CAN_msg_t resultMsg = {ACT_RESULT, {msg->sid & 0xFF, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES}, 4};
//					CAN_send(&resultMsg);
					CAN_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static void BALLSORTER_run_command(queue_id_t queueId, bool_e init) {
	static Uint16 wantedPosition;

	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallSorter) {    // Gestion des mouvements de rotation de l'AX12 et de la detection des cerises
		if(init && !QUEUE_has_error(queueId)) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			BALLSORTER_command_state_e state = QUEUE_get_arg(queueId)->param;
			bool_e cmdOk;

			wantedPosition = 0xFFFF;

			if(command != ACT_BALLSORTER_TAKE_NEXT_CHERRY) {
//				CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLSORTER & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
//				CAN_send(&resultMsg);
				CAN_sendResultWithLine(ACT_BALLSORTER, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				COMPONENT_log(LOG_LEVEL_Error, "invalid translation command: %u, code is broken !\n", command);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}

			COMPONENT_log(LOG_LEVEL_Debug, "state: %d, ax12 pos: %d\n", state, AX12_get_position(BALLSORTER_AX12_ID));

			switch(state) {
				case BALLSORTER_CS_EjectCherry:
					wantedPosition = BALLSORTER_AX12_EJECT_CHERRY_POS;
					break;

				case BALLSORTER_CS_GotoNextCherry:
					wantedPosition = BALLSORTER_AX12_NEXT_CHERRY_POS;
					break;

				case BALLSORTER_CS_TakeCherry:
					wantedPosition = BALLSORTER_AX12_DETECT_CHERRY_POS;
					break;

				case BALLSORTER_CS_DetectCherry:
				{   //Envoyer le message du resultat de la detection puis le message de resultat de l'opération demandé par la strat
					CAN_msg_t detectionResultMsg = {ACT_BALLSORTER_RESULT, {(BALLSORTER_SENSOR_PIN == BALLSORTER_SENSOR_DETECTED_LEVEL)? ACT_BALLSORTER_WHITE_CHERRY : ACT_BALLSORTER_NO_CHERRY}, 1};
					CAN_send(&detectionResultMsg);
					CAN_sendResultWithLine(ACT_BALLSORTER, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK);

//					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLSORTER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};
//					CAN_send(&resultMsg);

					QUEUE_behead(queueId);
					return; //La suite c'est les commandes AX12
				}
				default: {
//					CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLSORTER & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
//					CAN_send(&resultMsg);
					CAN_sendResultWithLine(ACT_BALLSORTER, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC);
					COMPONENT_log(LOG_LEVEL_Error, "Invalid command: %u, code is broken !\n", command);
					QUEUE_set_error(queueId);
					QUEUE_behead(queueId);
					return;
				}
			}

			if(wantedPosition == 0xFFFF) {
				CAN_sendResultWithLine(ACT_BALLSORTER, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC);
				COMPONENT_log(LOG_LEVEL_Error, "invalid AX12 position: %u, code is broken !\n", command);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}

			AX12_reset_last_error(BALLSORTER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
			cmdOk = AX12_set_position(BALLSORTER_AX12_ID, wantedPosition);
			if(!cmdOk) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
//				CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLSORTER & 0xFF, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE}, 4};
//				CAN_send(&resultMsg);
				CAN_sendResultWithLine(ACT_BALLSORTER, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE);
				COMPONENT_log(LOG_LEVEL_Error, "AX12_set_position error: 0x%x\n", AX12_get_last_error(BALLSORTER_AX12_ID).error);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
		} else {
			Uint8 error;
			Uint16 ax12Pos;
			//CAN_msg_t resultMsg;
			Uint8 result, errorCode;

			AX12_reset_last_error(BALLSORTER_AX12_ID);
			ax12Pos = AX12_get_position(BALLSORTER_AX12_ID); //même si non utilisé, permet de faire un ping en même temps. S'il n'est plus là (parce que kingkong l'a kidnappé par exemple) il ne répondra plus.
			error = AX12_get_last_error(BALLSORTER_AX12_ID).error;

			if(QUEUE_has_error(queueId)) {
				result =    ACT_RESULT_NOT_HANDLED;
				errorCode = ACT_RESULT_ERROR_OTHER;
			} else if(abs((Sint16)ax12Pos - (Sint16)wantedPosition) <= BALLSORTER_AX12_ASSER_POS_EPSILON) {	//Fin du mouvement
			//if(AX12_is_moving(BALLSORTER_AX12_ID) == FALSE) {  //Fin du mouvement
				result =    ACT_RESULT_DONE;
				errorCode = ACT_RESULT_ERROR_OK;
			} else if((error & AX12_ERROR_TIMEOUT) && (error & AX12_ERROR_RANGE)) {
				result =    ACT_RESULT_NOT_HANDLED;
				errorCode = ACT_RESULT_ERROR_LOGIC;	//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
			} else if(error & AX12_ERROR_TIMEOUT) {	//L'ax12 n'a pas répondu à la commande
				result =    ACT_RESULT_FAILED;
				errorCode = ACT_RESULT_ERROR_NOT_HERE;
			} else if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + BALLSORTER_AX12_ASSER_TIMEOUT) {    //Timeout, l'ax12 n'a pas bouger à la bonne position a temps
				result =    ACT_RESULT_FAILED;
				errorCode = ACT_RESULT_ERROR_UNKNOWN;
			} else if(error) {							//autres erreurs
				result =    ACT_RESULT_FAILED;
				errorCode = ACT_RESULT_ERROR_UNKNOWN;
			} else return;	//Operation is not finished, do nothing

			//On envoie le message CAN de retour que si l'opération a fail et que ce n'est pas a cause d'une opération antérieure (ACT_RESULT_ERROR_OTHER)
			if(result != ACT_RESULT_DONE && errorCode != ACT_RESULT_ERROR_OTHER) {
				QUEUE_set_error(queueId);

//				resultMsg.sid = ACT_RESULT;
//				resultMsg.data[0] = ACT_BALLSORTER & 0xFF;
//				resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
//				resultMsg.size = 4;
//
//				CAN_send(&resultMsg);
				CAN_sendResultWithLine(ACT_BALLSORTER, QUEUE_get_arg(queueId)->canCommand, result, errorCode);
			}
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

#endif  /* I_AM_ROBOT_KRUSTY */
