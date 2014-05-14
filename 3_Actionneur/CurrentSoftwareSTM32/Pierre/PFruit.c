/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : Fruit.c
 *	Package : Carte actionneur
 *	Description : Gestion des fruit_mouths
 *  Auteur : Amaury, Anthony
 *  Version 20130219
 *  Robot : BIG
 */

#include "PFruit.h"
#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_watchdog.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"
#include "PFruit_config.h"

#include "config_debug.h"
#define LOG_PREFIX "PFruit : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_FRUIT
#include "../QS/QS_outputlog.h"

#define TIMEOUT_ASSERVISEMENT_DC	2000

static void FRUIT_initAX12();
static void FRUIT_initDCM();
static void FRUIT_command_pompe_init(queue_id_t queueId);
static void FRUIT_command_pompe_run(queue_id_t queueId);
static void FRUIT_command_labium_init(queue_id_t queueId);
static void FRUIT_command_labium_run(queue_id_t queueId);
static void POMPE_goToPos(Uint8 command);

static bool_e stopActVerin = FALSE;
static bool_e flagEndWatchDog; // Pour la vibration

static enum {
	OPEN,
	CLOSE,
	VIBRATION
}wanted_state = CLOSE;

static bool_e have_send_answer = TRUE;

void FRUIT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	FRUIT_initDCM();
	FRUIT_initAX12();
	info_printf("FRUIT_MOUTH initialisé\n");
}

// Initilastion du moteur, si init ne fait rien
static void FRUIT_initDCM() {
	static bool_e dcm_is_initialized = FALSE;

	if(dcm_is_initialized == FALSE ) {
		PORTS_pwm_init();
		PWM_init();
		//FRUIT_POMPE_PIN = 0;
		wanted_state = CLOSE;
		dcm_is_initialized = TRUE;
		info_printf("VERIN FRUIT initialisé (pompe) \n");
	}
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void FRUIT_initAX12() {
	static bool_e ax12_is_initialized = FALSE;

	if(ax12_is_initialized == FALSE && AX12_is_ready(FRUIT_LABIUM_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(FRUIT_LABIUM_AX12_ID, 136);
		AX12_config_set_lowest_voltage(FRUIT_LABIUM_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(FRUIT_LABIUM_AX12_ID, FRUIT_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(FRUIT_LABIUM_AX12_ID, 300);
		AX12_config_set_minimal_angle(FRUIT_LABIUM_AX12_ID, 0);

		AX12_config_set_error_before_led(FRUIT_LABIUM_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(FRUIT_LABIUM_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.

		debug_printf("FRUIT_LABIUM AX12 initialisé\n");
	}
	debug_printf("Fruit init config %s\n", ax12_is_initialized ? "DONE" : "FAIL");
}

void FRUIT_stop() {
	CAN_msg_t msg;
	PWM_stop(FRUIT_POMPE_PWM_NUM);
	if(have_send_answer == FALSE){
		msg.size = 1;
		msg.sid = STRAT_INFORM_FRUIT_MOUTH;
		if(wanted_state == OPEN)
			msg.data[0] = STRAT_INFORM_FRUIT_MOUTH_OPEN;
		else
			msg.data[0] = STRAT_INFORM_FRUIT_MOUTH_CLOSE;
		CAN_send(&msg);
		have_send_answer = TRUE;
	}
	//FRUIT_POMPE_PIN = 0;

}

void FRUIT_init_pos(){
	FRUIT_initAX12();
	debug_printf("Fruit init pos : \n");
	if(!AX12_set_position(FRUIT_LABIUM_AX12_ID, FRUIT_AX12_LABIUM_INIT_POS))
		debug_printf("    L'AX12 n°%d n'est pas la\n", FRUIT_LABIUM_AX12_ID);
}

bool_e FRUIT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_FRUIT_MOUTH) {
		FRUIT_initAX12();
		switch(msg->data[0]) {


			/******** POMPE ***********/


			case ACT_FRUIT_MOUTH_CANCELED:
				stopActVerin = TRUE;
				break;

				//Pour les 2 cas (open ou close), la différence est faite dans FRUIT_command_init
			case ACT_FRUIT_MOUTH_VIBRATION:
				WATCHDOG_create_flag(FRUIT_POMPE_TIMER_VIBRATION,&flagEndWatchDog);
			case ACT_FRUIT_MOUTH_CLOSE:
			case ACT_FRUIT_MOUTH_OPEN:
				have_send_answer = TRUE;
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_POMPE_Fruit, &FRUIT_run_command, 0);
				break;


			/******** AX12 LABIUM ***********/


			case ACT_FRUIT_LABIUM_CLOSE:
			case ACT_FRUIT_LABIUM_OPEN:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_Fruit, &FRUIT_run_command, 0);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&FRUIT_run_command, 4, (SELFTEST_action_t[]){
								 {ACT_FRUIT_MOUTH_OPEN,   0, QUEUE_ACT_POMPE_Fruit},
								 {ACT_FRUIT_MOUTH_CLOSE,  0, QUEUE_ACT_POMPE_Fruit},
								 {ACT_FRUIT_LABIUM_OPEN,  0, QUEUE_ACT_AX12_Fruit},
								 {ACT_FRUIT_LABIUM_CLOSE, 0, QUEUE_ACT_AX12_Fruit}
							 });
	}

	return FALSE;
}

void FRUIT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_Fruit) {    // Gestion des mouvements du labium
			if(init)
				FRUIT_command_labium_init(queueId);
			else
				FRUIT_command_labium_run(queueId);
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_Fruit) {

		if(init)
			FRUIT_command_pompe_init(queueId);
		else
			FRUIT_command_pompe_run(queueId);
	}
}

//Initialise une commande
static void FRUIT_command_pompe_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	switch(command) {
		case ACT_FRUIT_MOUTH_OPEN:
			POMPE_goToPos(ACT_FRUIT_MOUTH_OPEN);
			break;
		case ACT_FRUIT_MOUTH_CLOSE:
			POMPE_goToPos(ACT_FRUIT_MOUTH_CLOSE);
			break;

		case ACT_FRUIT_MOUTH_VIBRATION:
			POMPE_goToPos(ACT_FRUIT_MOUTH_VIBRATION);
			break;

		case ACT_FRUIT_MOUTH_STOP:
			//FRUIT_POMPE_PIN = 0;
			PWM_stop(FRUIT_POMPE_PWM_NUM);
			return;

		default: {
			error_printf("Invalid pompe command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

// La pompe en déplacement vérification de son état
static void FRUIT_command_pompe_run(queue_id_t queueId){
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	// le verin atteint la postion du capteur de sortie ou la commande a été annulée, on arrete
	if((FRUIT_POMPE_TOR_OPEN == 0 && command == ACT_FRUIT_MOUTH_OPEN) ||
	   (FRUIT_POMPE_TOR_CLOSE == 0 && command == ACT_FRUIT_MOUTH_CLOSE) ||
	   (command == ACT_FRUIT_MOUTH_VIBRATION) ||	//Passe directement a la queue mais est toujours asservi de façon à bouger pendant qu'aucune action est demandée
	   stopActVerin == TRUE)
	{
		PWM_stop(FRUIT_POMPE_PWM_NUM);
		//FRUIT_POMPE_PIN = 0;

		if(stopActVerin == TRUE)
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_FAILED, ACT_RESULT_ERROR_CANCELED, __LINE__);
		else
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);

		stopActVerin = FALSE;
	} else if(ACTQ_check_timeout(queueId, FRUIT_POMPE_ASSER_TIMEOUT)) { //Si timeout, on arrete
		//FRUIT_POMPE_PIN = 0;
		PWM_stop(FRUIT_POMPE_PWM_NUM);
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_FAILED, ACT_RESULT_ERROR_TIMEOUT, __LINE__);
	}
}

static void POMPE_goToPos(Uint8 command){
	if(command == ACT_FRUIT_MOUTH_OPEN){
		wanted_state = OPEN;
		FRUIT_POMPE_SENS = 0;
	}else if(command == ACT_FRUIT_MOUTH_CLOSE){
		wanted_state = CLOSE;
		FRUIT_POMPE_SENS = 1;
	}else if(command == ACT_FRUIT_MOUTH_VIBRATION){
		if(FRUIT_POMPE_TOR_OPEN == 0)
			FRUIT_POMPE_SENS = 1;
		else
			FRUIT_POMPE_SENS = 0;

		wanted_state = VIBRATION;
	}
	//FRUIT_POMPE_PIN = 1;
	PWM_run(FRUIT_POMPE_MAX_PWM_WAY, FRUIT_POMPE_PWM_NUM);

}

static void FRUIT_command_labium_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;
	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_FRUIT_LABIUM_OPEN: *ax12_goalPosition = FRUIT_AX12_LABIUM_OPEN_POS; break;
		case ACT_FRUIT_LABIUM_CLOSE: *ax12_goalPosition = FRUIT_AX12_LABIUM_CLOSE_POS; break;

		case ACT_FRUIT_LABIUM_STOP:
			AX12_set_torque_enabled(FRUIT_LABIUM_AX12_ID, FALSE);
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid labium command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(*ax12_goalPosition == 0xFFFF){
		error_printf("Invalid labium position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move fruit ax12 to %d\n", *ax12_goalPosition);

	AX12_reset_last_error(FRUIT_LABIUM_AX12_ID);
	if(!AX12_set_position(FRUIT_LABIUM_AX12_ID, *ax12_goalPosition)){
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(FRUIT_LABIUM_AX12_ID).error);
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de la pince.
static void FRUIT_command_labium_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, FRUIT_LABIUM_AX12_ID, QUEUE_get_arg(queueId)->param, FRUIT_AX12_ASSER_POS_EPSILON, FRUIT_AX12_ASSER_TIMEOUT, 0, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, result, errorCode, line);
}

// Asservissement verrin
void FRUIT_process_main(){
	FRUIT_initDCM();
	typedef enum{
		NO_ORDER,
		IN_OPENING,
		IN_CLOSING
	}verrin_order_e;
	static verrin_order_e verrin_order = NO_ORDER;
	static bool_e flagWatchDog = TRUE;
	static clock_time_t begin_time_order;

	switch(wanted_state){
		case OPEN :
			if(FRUIT_POMPE_TOR_OPEN == 1 && verrin_order != IN_OPENING){
				POMPE_goToPos(ACT_FRUIT_MOUTH_OPEN);
				verrin_order = IN_OPENING;
				begin_time_order = CLOCK_get_time_10() * 10;
			}else if(FRUIT_POMPE_TOR_OPEN == 0 && verrin_order != NO_ORDER){
				PWM_stop(FRUIT_POMPE_PWM_NUM);
				verrin_order = NO_ORDER;
			}else if(CLOCK_get_time_10() * 10 - begin_time_order > TIMEOUT_ASSERVISEMENT_DC)
				PWM_stop(FRUIT_POMPE_PWM_NUM);
			break;

		case CLOSE :
			if(FRUIT_POMPE_TOR_CLOSE == 1 && verrin_order != IN_CLOSING){
				POMPE_goToPos(ACT_FRUIT_MOUTH_CLOSE);
				verrin_order = IN_CLOSING;
				begin_time_order = CLOCK_get_time_10() * 10;
			}else if(FRUIT_POMPE_TOR_CLOSE == 0 && verrin_order != NO_ORDER){
				PWM_stop(FRUIT_POMPE_PWM_NUM);
				verrin_order = NO_ORDER;
			}else if(CLOCK_get_time_10() * 10 - begin_time_order > TIMEOUT_ASSERVISEMENT_DC)
				PWM_stop(FRUIT_POMPE_PWM_NUM);
			break;

		case VIBRATION :
			if(verrin_order != IN_CLOSING && flagWatchDog){
				POMPE_goToPos(ACT_FRUIT_MOUTH_VIBRATION);
				verrin_order = IN_CLOSING;
				WATCHDOG_create_flag(FRUIT_POMPE_PULSE_VIBRATION,&flagWatchDog);
			}else if(verrin_order != IN_OPENING && flagWatchDog){
				POMPE_goToPos(ACT_FRUIT_MOUTH_VIBRATION);
				verrin_order = IN_OPENING;
				WATCHDOG_create_flag(FRUIT_POMPE_PULSE_VIBRATION,&flagWatchDog);
			}

			if(flagEndWatchDog)
				wanted_state = OPEN;

			break;

		default:
			break;
	}
}

#endif  /* I_AM_ROBOT_BIG */
