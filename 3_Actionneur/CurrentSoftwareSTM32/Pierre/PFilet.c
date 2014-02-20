/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : PFilet.c
 *	Package : Carte actionneur
 *	Description : Gestion du Filet
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */
#include "PFilet.h"
#ifdef I_AM_ROBOT_BIG

//#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"
#include "PFilet_config.h"
#include "../QS/QS_watchdog.h"


#include "config_debug.h"
#define LOG_PREFIX "FR: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_FILET
#include "../QS/QS_outputlog.h"


#define TIME_FILET_IT					5
#define TIME_BEFORE_REARM				500
#define TIME_BEFORE_FREE_STRING			50
#define TIME_AFTER_FREE_STRING			20

static void FILET_lacher_ficelles();
static void FILET_liberer_gache();


void FILET_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	FILET_initAX12();
	//info_printf("FILET initialisé\n");
}


//Initialise l'AX12 du filet s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void FILET_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(FILET_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(FILET_AX12_ID, 136);
		AX12_config_set_lowest_voltage(FILET_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(FILET_AX12_ID, FILET_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(FILET_AX12_ID, 300);
		AX12_config_set_minimal_angle(FILET_AX12_ID, 0);

		AX12_config_set_error_before_led(FILET_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(FILET_AX12_ID, AX12_ERROR_OVERHEATING);

		// Pas d'initialisation sinon réarmement impossible
		//AX12_set_position(FILET_AX12_ID, FILET_AX12_INIT_POS);
		//info_printf("FILET AX12 initialisé\n");
	}
}


bool_e FILET_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId1;
	FILET_initAX12();
	if(msg->sid == ACT_FILET) {
		switch(msg->data[0]) {
			case ACT_FILET_IDLE :
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
					QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){msg->data[0], FILET_CS_IdleAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Filet);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
				}else{					//on indique qu'on n'a pas géré la commande
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_FILET_LAUNCHED :
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
					QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){msg->data[0], FILET_CS_LaunchedAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Filet);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
				}else{					//on indique qu'on n'a pas géré la commande
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_FILET_STOP :
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
					QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){msg->data[0], FILET_CS_StopAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Filet);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
				}else{					//on indique qu'on n'a pas géré la commande
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	} else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&FILET_run_command, 2, (SELFTEST_action_t[]){
								 {ACT_FILET_IDLE,  FILET_CS_IdleAX12,  QUEUE_ACT_AX12_Filet}
								 // Vérifier présence filet
							 });
	}

	return FALSE;
}


void FILET_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_Filet) {    // Gestion des mouvements du FILET
		if(init)
			FILET_command_init(queueId);
		else
			FILET_command_run(queueId);
	}
}


//Initialise une commande
static void FILET_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_FILET_IDLE :  *ax12_goalPosition = FILET_AX12_IDLE_POS; break;
		case ACT_FILET_LAUNCHED : *ax12_goalPosition = FILET_AX12_LAUNCHED_POS; break;
		case ACT_FILET_STOP :
			AX12_set_torque_enabled(FILET_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère le filet
			QUEUE_next(queueId, ACT_FILET, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				error_printf("Invalid plier command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_FILET, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid plier position: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_FILET, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move filet ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(FILET_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(FILET_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(FILET_AX12_ID).error);
		QUEUE_next(queueId, ACT_FILET, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	if(command == ACT_FILET_LAUNCHED)
		WATCHDOG_create(TIME_BEFORE_FREE_STRING, FILET_lacher_ficelles, FALSE);
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de l'AX12 déclenchant le filet
static void FILET_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	//En cas de timeout on passe à l'instruction suivante
	if(ACTQ_check_status_ax12(queueId, FILET_AX12_ID, QUEUE_get_arg(queueId)->param, FILET_AX12_ASSER_POS_EPSILON, FILET_AX12_ASSER_TIMEOUT, 360, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_FILET, result, errorCode, line);
}

static void FILET_lacher_ficelles(){
	GACHE_FILET = 1;
	WATCHDOG_create(TIME_AFTER_FREE_STRING, FILET_liberer_gache, FALSE);
}

static void FILET_liberer_gache(){
	GACHE_FILET = 0;
}
#endif  /* I_AM_ROBOT_BIG */
