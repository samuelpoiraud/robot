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
#include "../act_queue_utils.h"
#include "KBall_sorter_config.h"
#include "KBall_launcher.h"

#define LOG_PREFIX "BS: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_BALLSORTER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

//Etape d'un passage de cerise (demandé par la strat)
typedef enum {
	BALLSORTER_CS_CheckLauncherSpeed = 0, //Verifie que le lanceur de balle à atteint sa vitesse (avant d'envoyer la cerise)
	BALLSORTER_CS_EjectCherry = 1,        //Ejecte la cerise prise
	BALLSORTER_CS_GotoNextCherry = 2,     //Va prendre une nouvelle cerise dans le bac
	BALLSORTER_CS_TakeCherry = 3,         //Met la cerise prise devant le capteur
	BALLSORTER_CS_DetectCherry = 4        //Detecte la cerise pour savoir si elle est blanche ou pas et renvoi le resultat à la strat
} BALLSORTER_command_state_e;

static Uint16 desired_ball_launcher_speed = 0;

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

		//AX12_set_torque_enabled(BALLSORTER_AX12_ID, TRUE);
		//AX12_set_position(BALLSORTER_AX12_ID, BALLSORTER_AX12_EJECT_CHERRY_POS);
		COMPONENT_log(LOG_LEVEL_Info, "AX12 initialisé\n");
	}
}

void BALLSORTER_stop() {
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
					if(msg->size >= 3) //Compatibilité avec l'ancien message CAN qui ne gérait pas le lanceur de balle
						desired_ball_launcher_speed = U16FROMU8(msg->data[2], msg->data[1]);
					else desired_ball_launcher_speed = 0;
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_CheckLauncherSpeed, &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_EjectCherry       , &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_GotoNextCherry    , &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_TakeCherry        , &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_DetectCherry      , &ACTQ_finish_SendResult}      , QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
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
	static time_t detection_end_time; //pour attendre un certain temps sur la position detect de l'ax12
	BALLSORTER_command_state_e state = QUEUE_get_arg(queueId)->param;

	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallSorter) {    // Gestion des mouvements de rotation de l'AX12 et de la detection des cerises
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;

			wantedPosition = 0xFFFF;

			if(command != ACT_BALLSORTER_TAKE_NEXT_CHERRY) {
				COMPONENT_log(LOG_LEVEL_Error, "invalid translation command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}

			COMPONENT_log(LOG_LEVEL_Debug, "state: %d, ax12 pos: %d\n", state, AX12_get_position(BALLSORTER_AX12_ID));

			switch(state) {
				case BALLSORTER_CS_CheckLauncherSpeed:
					return; //La suite c'est les commandes AX12, pas d'init pour cet état, on vérifie juste si le lanceur de balle a atteint sa vitesse avant de passer a l'état suivant

				case BALLSORTER_CS_EjectCherry:
					AX12_set_move_to_position_speed(BALLSORTER_AX12_ID, 500);
					wantedPosition = BALLSORTER_AX12_EJECT_CHERRY_POS;
					break;

				case BALLSORTER_CS_GotoNextCherry:
					AX12_set_move_to_position_speed(BALLSORTER_AX12_ID, 500);
					wantedPosition = BALLSORTER_AX12_NEXT_CHERRY_POS;
					break;

				case BALLSORTER_CS_TakeCherry:
					AX12_set_move_to_position_speed(BALLSORTER_AX12_ID, 150);
					wantedPosition = BALLSORTER_AX12_DETECT_CHERRY_POS;
					break;

				case BALLSORTER_CS_DetectCherry:
					detection_end_time = CLOCK_get_time() + BALLSORTER_DETECT_TIME;
					return; //La suite c'est les commandes AX12

				default: {
					COMPONENT_log(LOG_LEVEL_Error, "Invalid command: %u, code is broken !\n", command);
					QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
					return;
				}
			}

			if(wantedPosition == 0xFFFF) {
				COMPONENT_log(LOG_LEVEL_Error, "invalid AX12 position: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}

			debug_printf("AX12 moveto %d\n", wantedPosition);
			AX12_reset_last_error(BALLSORTER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
			if(!AX12_set_position(BALLSORTER_AX12_ID, wantedPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
				COMPONENT_log(LOG_LEVEL_Error, "AX12_set_position error: 0x%x\n", AX12_get_last_error(BALLSORTER_AX12_ID).error);
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
				return;
			}
		} else if(state == BALLSORTER_CS_CheckLauncherSpeed) {
			DCM_working_state_e ball_launcher_state = DCM_get_state(BALLLAUNCHER_DCMOTOR_ID);
			//Tant que on a pas atteint la bonne vitesse du lanceur de balle
			//On ne passe pas a l'état suivant qui envoie la cerise dans le lanceur pour être éjectée dans le gateau
			if(ball_launcher_state == DCM_IDLE) {
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			} else if(ball_launcher_state == DCM_TIMEOUT) {
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_TIMEOUT, __LINE__);
			}
		} else if(state != BALLSORTER_CS_DetectCherry) {
			Uint16 line;
			Uint8 result, errorCode;

			//On envoie le message CAN de retour que si l'opération a fail

			if(ACTQ_check_status_ax12(queueId, BALLSORTER_AX12_ID, wantedPosition, BALLSORTER_AX12_ASSER_POS_EPSILON, BALLSORTER_AX12_ASSER_TIMEOUT, BALLSORTER_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
				QUEUE_next(queueId, ACT_BALLSORTER, result, errorCode, line);

		} else { //BALLSORTER_CS_DetectCherry
			if(CLOCK_get_time() > detection_end_time) {   //Envoyer le message du resultat de la detection qu'après la fin du temps BALLSORTER_DETECT_TIME
				Uint8 cherry_color;
#ifndef BALLSORTER_USE_SICK_SENSOR
				if(BALLSORTER_SENSOR_PIN == BALLSORTER_SENSOR_DETECTED_LEVEL)
					cherry_color = ACT_BALLSORTER_WHITE_CHERRY;
				else cherry_color = ACT_BALLSORTER_NO_CHERRY;
#else

				if(BALLSORTER_SENSOR_PIN_WHITE_CHERRY == BALLSORTER_SENSOR_DETECTED_LEVEL)
					cherry_color = ACT_BALLSORTER_WHITE_CHERRY;
				else if(BALLSORTER_SENSOR_PIN_RED_CHERRY == BALLSORTER_SENSOR_DETECTED_LEVEL || BALLSORTER_SENSOR_PIN_BLUE_CHERRY == BALLSORTER_SENSOR_DETECTED_LEVEL)
					cherry_color = ACT_BALLSORTER_BAD_CHERRY;
				else cherry_color = ACT_BALLSORTER_NO_CHERRY;
#endif

				CAN_msg_t detectionResultMsg = {ACT_BALLSORTER_RESULT, {cherry_color}, 1};

				if(desired_ball_launcher_speed >= 0) {
					if(cherry_color == ACT_BALLSORTER_WHITE_CHERRY)
						BALLLAUNCHER_set_speed(desired_ball_launcher_speed);
					else BALLLAUNCHER_set_speed(desired_ball_launcher_speed >> 1);
				}

				CAN_send(&detectionResultMsg);

				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, cherry_color); //param vaut la couleur de la cerise s'il n'y a pas eu d'erreur avant
			}
		}
	}
}

#endif  /* I_AM_ROBOT_KRUSTY */
