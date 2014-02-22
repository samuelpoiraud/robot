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

#include "PFilet_config.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../config/config_pin.h"
#include "../Can_msg_processing.h"


#include "config_debug.h"
#define LOG_PREFIX "PFilet.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_FILET
#include "../QS/QS_outputlog.h"


/* define & variable relatifs au lanceur de filet */
#define QUANTUM_IT						100  //[ms], unité de temps de clock.c
#define TIME_FILET_IT					(1*QUANTUM_IT)
#define TIME_BEFORE_REARM				500  //[ms]
#define TIME_BEFORE_DETECT				TIME_BEFORE_REARM
#define TIME_BEFORE_FREE_STRING			50  //[ms]
#define TIME_AFTER_FREE_STRING			20  //[ms]
#define EPSILON_POS_FILET				4
Uint8 time_filet = 0;
bool_e rearm_auto_active = TRUE;

static void FILET_lacher_ficelles();
static void FILET_liberer_gache();
static void FILET_initAX12();
static void FILET_command_init(queue_id_t queueId);
static void FILET_command_run(queue_id_t queueId);
static void FILET_detection();
static void FILET_rearm();


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
	FILET_initAX12();
	if(msg->sid == ACT_FILET) {
		switch(msg->data[0]) {

			//Même chose pour les 2 actions
			case ACT_FILET_IDLE :
			case ACT_FILET_LAUNCHED :
			case ACT_FILET_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_Filet, &FILET_run_command, 0);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	} else if(msg->sid == ACT_DO_SELFTEST) {
		// Pas de selftest pour cette actionneur
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
				error_printf("Invalid command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_FILET, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid position for command: %u, code is broken !\n", command);
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
	if(ACTQ_check_status_ax12(queueId, FILET_AX12_ID, QUEUE_get_arg(queueId)->param, FILET_AX12_ASSER_POS_EPSILON, FILET_AX12_ASSER_TIMEOUT, 0, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_FILET, result, errorCode, line);
}

static void FILET_lacher_ficelles(){
	GACHE_FILET = 1;
	WATCHDOG_create(TIME_AFTER_FREE_STRING, FILET_liberer_gache, FALSE);
}

static void FILET_liberer_gache(){
	GACHE_FILET = 0;
}

//************************************** Gestion du réarmement et information /**************************************/

void FILET_process_100ms(void){
	if(time_filet >= 100)
		time_filet -= 100;
	else
		time_filet = 0;
}

void FILET_process_main(void){
	if(time_filet)
		return;

	FILET_detection(); // informe la stratégie de la présence ou non du filet

	if(rearm_auto_active)
		FILET_rearm();	// Si réarmement automatique activé lancer la fonction de gestion du réarmement

	time_filet = TIME_FILET_IT;
}

void FILET_BOUTON_process(void){
	rearm_auto_active = FALSE;

	queue_id_t queueId1;
	FILET_initAX12();

	if(!AX12_is_ready(FILET_AX12_ID) || AX12_is_moving(FILET_AX12_ID) || !global.alim)
		return;

	typedef enum{
		ARMED,
		FREE
	}state_e;

	static state_e state = ARMED;

	if(state == ARMED){
		debug_printf("Libération du bras du filet\n");
		queueId1 = QUEUE_create();
		if(queueId1 != QUEUE_CREATE_FAILED) {
			QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
			QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){ACT_FILET_LAUNCHED, 0,  NULL}, QUEUE_ACT_AX12_Filet);
			QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
		}
		state = FREE;
	}else{
		debug_printf("Armement du bras du filet\n");
		queueId1 = QUEUE_create();
		if(queueId1 != QUEUE_CREATE_FAILED) {
			QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
			QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){ACT_FILET_IDLE, 0,  NULL}, QUEUE_ACT_AX12_Filet);
			QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
		}
		state = ARMED;
	}
}

static void FILET_detection() {
	typedef enum{
		EDGE_DETECTOR,
		WAIT_UP,
		WAIT_DOWN
	}state_e;

	CAN_msg_t msg;
	static state_e state = EDGE_DETECTOR;
	static Uint16 time;
	static bool_e last_port_state = FALSE;

	switch(state){
		case EDGE_DETECTOR:
			time = 0;
			if(!last_port_state && PRESENCE_FILET)
				state = WAIT_UP;
			else if(last_port_state && !PRESENCE_FILET)
				state = WAIT_DOWN;
			break;

		case WAIT_UP:
			if(time >= TIME_BEFORE_DETECT){
				msg.size = 1;
				msg.sid = STRAT_INFORM_FILET;
				msg.data[0] = STRAT_INFORM_FILET_PRESENT;
				CAN_send(&msg);
				state = EDGE_DETECTOR;
			}else
				time += TIME_FILET_IT;
			break;

		case WAIT_DOWN:
			if(time >= TIME_BEFORE_DETECT){
				msg.size = 1;
				msg.sid = STRAT_INFORM_FILET;
				msg.data[0] = STRAT_INFORM_FILET_ABSENT;
				CAN_send(&msg);
				state = EDGE_DETECTOR;
			}else
				time += TIME_FILET_IT;
			break;
	}
	last_port_state = PRESENCE_FILET;
}

static void FILET_rearm(){
	queue_id_t queueId1;
	FILET_initAX12();

	if(!AX12_is_ready(FILET_AX12_ID) || AX12_is_moving(FILET_AX12_ID) || !global.alim)
		return;

	typedef enum{
		RISING_EDGE,
		WAIT_UP,
		REARM,
		FREE
	}state_e;

	static state_e state = RISING_EDGE;
	static bool_e last_port_state = TRUE;
	static Uint16 time;
	Uint16 position_ax12 = AX12_get_position(FILET_AX12_ID);

	switch(state){
		case RISING_EDGE :
			//debug_printf("%d %s\n", position_ax12, PRESENCE_FILET?"présent":"non présent");
			if(PRESENCE_FILET && !last_port_state){
				debug_printf("Tentative de réarmement du bras du filet !\n");
				time = 0;
				state = WAIT_UP;
			}else if(!PRESENCE_FILET
					 && !((position_ax12 <= FILET_AX12_LAUNCHED_POS + EPSILON_POS_FILET)
					 && (position_ax12 >= FILET_AX12_LAUNCHED_POS - EPSILON_POS_FILET)))
				state = FREE;
			break;

		case WAIT_UP :
			if(PRESENCE_FILET){
				if(time >= TIME_BEFORE_REARM)
					state = REARM;
				else{
					time += TIME_FILET_IT;
				}
			}else{
				debug_printf("Réarmement annulé\n");
				state = RISING_EDGE;
			}
			break;

		case REARM :
			debug_printf("Réarmement du bras du filet\n");
			queueId1 = QUEUE_create();
			if(queueId1 != QUEUE_CREATE_FAILED) {
				QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
				QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){ACT_FILET_IDLE, 0,  NULL}, QUEUE_ACT_AX12_Filet);
				QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
			}
			state = RISING_EDGE;
			break;

		case FREE :
			debug_printf("Libération du bras du filet\n");
			queueId1 = QUEUE_create();
			if(queueId1 != QUEUE_CREATE_FAILED) {
				QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t) {0, 0, NULL}, QUEUE_ACT_AX12_Filet);
				QUEUE_add(queueId1, &FILET_run_command, (QUEUE_arg_t){ACT_FILET_LAUNCHED, 0,  NULL}, QUEUE_ACT_AX12_Filet);
				QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Filet);
			}
			state = RISING_EDGE;
			break;
	}
	last_port_state = PRESENCE_FILET;
}

#endif  /* I_AM_ROBOT_BIG */
