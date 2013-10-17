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
#include "../act_queue_utils.h"
#include "config_pin.h"
#include "KBall_sorter_config.h"
#include "KBall_launcher.h"

#include "config_debug.h"
#define LOG_PREFIX "BS: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_BALLSORTER
#include "../QS/QS_outputlog.h"

static Uint16 desired_ball_launcher_speed = 0;
static bool_e cherry_taken = FALSE;
static bool_e launch_cherry = TRUE;	//Si TRUE on lance les cerises blanches sinon on ne les lance pas (on garde la cerise). Les pourie sont toujours lanc�es

static void BALLSORTER_initAX12();

void BALLSORTER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	ADC_init();

	BALLSORTER_initAX12();

	info_printf("S�quenceur de cerise initialis�\n");
	//a faire: RAM de cerise, registres de cerise, ALU de cerise, GPIO de cerise, bras de cerise, pieds de cerise, firmware groupama(c)(r)(tm) 3.1.2 de cerise
	//groupama est une marque d�pos�e sur le bord de la route actuellement orpheline, pour des demandes d'adoption, veuillez vous renseigner �: www.jadoptemabankdememoire.com
}

//Initialise l'AX12 s'il n'�tait pas alliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
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

		info_printf("AX12 initialis�\n");
	}
}

void BALLSORTER_stop() {
}

bool_e BALLSORTER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;

	if(msg->sid == ACT_BALLSORTER) {
		//Initialise l'AX12 s'il n'�tait pas alliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
		BALLSORTER_initAX12();

		switch(msg->data[0]) {
			case ACT_BALLSORTER_TAKE_NEXT_CHERRY:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					desired_ball_launcher_speed = U16FROMU8(msg->data[2], msg->data[1]) & 0x7FFF;
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_GotoNextCherry    , &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_TakeCherry        , &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_DetectCherry      , &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_CheckLauncherSpeed, &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_BallSorter);
					if((msg->data[2] & 0x80) == 0)	//R�trocompatibilit� avec ancien msg
						launch_cherry = TRUE;		//On choisi si on ne doit pas garder les cerises blanches ...
					else launch_cherry = FALSE;		//... On lance toujours les cerises mauvaises
					QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){msg->data[0], BALLSORTER_CS_EjectCherry       , &ACTQ_finish_SendResult}      , QUEUE_ACT_BallSorter);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
				} else {	//on indique qu'on a pas g�r� la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			default:
				warn_printf("invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void BALLSORTER_run_command(queue_id_t queueId, bool_e init) {
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
				error_printf("invalid translation command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}

			debug_printf("state: %d, ax12 pos: %d\n", state, AX12_get_position(BALLSORTER_AX12_ID));

			//Si on a d�j� pris une cerise, on en reprendre pas une autre
			if(cherry_taken == TRUE && (state == BALLSORTER_CS_GotoNextCherry || state == BALLSORTER_CS_TakeCherry)) {
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
				return;
			}

			switch(state) {
				case BALLSORTER_CS_CheckLauncherSpeed:
					return; //La suite c'est les commandes AX12, pas d'init pour cet �tat, on v�rifie juste si le lanceur de balle a atteint sa vitesse avant de passer a l'�tat suivant

				case BALLSORTER_CS_EjectCherry:
					if(launch_cherry == TRUE) {
						AX12_set_move_to_position_speed(BALLSORTER_AX12_ID, 250);
						wantedPosition = BALLSORTER_AX12_EJECT_CHERRY_POS;
					} else {		//Si on ne doit pas lancer la cerise, on la lance pas
						QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
						return;
					}
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
					error_printf("Invalid command: %u, code is broken !\n", command);
					QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
					return;
				}
			}

			if(wantedPosition == 0xFFFF) {
				error_printf("invalid AX12 position: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}

			debug_printf("AX12 moveto %d\n", wantedPosition);
			AX12_reset_last_error(BALLSORTER_AX12_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
			if(!AX12_set_position(BALLSORTER_AX12_ID, wantedPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que l'AX12 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
				error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(BALLSORTER_AX12_ID).error);
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
				return;
			}
		} else if(state == BALLSORTER_CS_CheckLauncherSpeed) {
			DCM_working_state_e ball_launcher_state = DCM_get_state(BALLLAUNCHER_DCMOTOR_ID);
			//Tant que on a pas atteint la bonne vitesse du lanceur de balle
			//On ne passe pas a l'�tat suivant qui envoie la cerise dans le lanceur pour �tre �ject�e dans le gateau
			if(ball_launcher_state == DCM_IDLE) {
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			} else if(ball_launcher_state == DCM_TIMEOUT) {
				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_TIMEOUT, __LINE__);
			}
		} else if(state != BALLSORTER_CS_DetectCherry) {
			Uint16 line;
			Uint8 result, errorCode;

			if(state == BALLSORTER_CS_EjectCherry) {
				cherry_taken = FALSE;
			}

			if(ACTQ_check_status_ax12(queueId, BALLSORTER_AX12_ID, wantedPosition, BALLSORTER_AX12_ASSER_POS_EPSILON, BALLSORTER_AX12_ASSER_TIMEOUT, BALLSORTER_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
				QUEUE_next(queueId, ACT_BALLSORTER, result, errorCode, line);

		} else { //BALLSORTER_CS_DetectCherry
			if(CLOCK_get_time() > detection_end_time) {   //Envoyer le message du resultat de la detection qu'apr�s la fin du temps BALLSORTER_DETECT_TIME
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

//				//FIXME on lance TOUTES les cerises
//				cherry_color = ACT_BALLSORTER_WHITE_CHERRY;

				CAN_msg_t detectionResultMsg = {ACT_BALLSORTER_RESULT, {cherry_color}, 1};

				if(desired_ball_launcher_speed >= 0) {
					if(cherry_color == ACT_BALLSORTER_WHITE_CHERRY)
						BALLLAUNCHER_set_speed(desired_ball_launcher_speed);
					else {
						launch_cherry = TRUE;	//Dans tous les cas on lance la cerise prise si elle n'est pas bonne
						BALLLAUNCHER_set_speed(2000);
					}
				}

				if(cherry_color != ACT_BALLSORTER_NO_CHERRY)
					cherry_taken = TRUE;

				CAN_send(&detectionResultMsg);

				QUEUE_next(queueId, ACT_BALLSORTER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, cherry_color); //param vaut la couleur de la cerise s'il n'y a pas eu d'erreur avant
			}
		}
	}
}

#endif  /* I_AM_ROBOT_KRUSTY */
