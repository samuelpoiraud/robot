/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : clap_holly.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur clap
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */


#include "clap_holly.h"

#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_adc.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"
#include "../config/config_big/config_pin.h"
#include "../QS/QS_watchdog.h"

#include "clap_holly_config.h"

#define LOG_PREFIX "clap_holly : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CLAP_HOLLY
#include "../QS/QS_outputlog.h"

static void CLAP_HOLLY_command_run(Uint8 command);
static void CLAP_HOLLY_command_init(queue_id_t queueId);
static void CLAP_PWM_stop();
static watchdog_id_t clap_watchdog;
static bool_e mouv = TRUE;

typedef enum{
	CLAP_IN,
	CLAP_OUT
}clap_state_e;

clap_state_e state_clap = CLAP_IN;


#define CLAP_HOLLY_PWM 3

void CLAP_process_it(){
//	if(!GPIO_ReadOutputDataBit(CLAP_GOLLY_DCM_SENS) && !CLAP_FDP_RIGHT && mouv){   // Sortie Bras
//		CLAP_PWM_stop();
//		debug_printf("Stop Right\n");
//		mouv = FALSE;
//	}

//	if(GPIO_ReadOutputDataBit(CLAP_GOLLY_DCM_SENS) && !CLAP_FDP_LEFT && mouv){		// Rentre bras
//		CLAP_PWM_stop();
//		debug_printf("Stop Left \n");
//		mouv = FALSE;
//	}
}


void CLAP_HOLLY_init() {
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	clap_watchdog = 0;

	PWM_init();

}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void CLAP_HOLLY_init_pos(){

}

void CLAP_HOLLY_reset_config(){}

// Fonction appell�e � la fin du match (via ActManager)
void CLAP_HOLLY_stop(){
	CLAP_PWM_stop();
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e CLAP_HOLLY_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CLAP_HOLLY){
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_CLAP_HOLLY_IDLE :
			case ACT_CLAP_HOLLY_RIGHT :
				CLAP_HOLLY_command_run(msg->data[0]);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest

	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void CLAP_HOLLY_run_command(queue_id_t queueId, bool_e init) {

}

//Initialise une commande
static void CLAP_HOLLY_command_init(queue_id_t queueId) {

}

//G�re les �tats pendant le mouvement du moteur
static void CLAP_HOLLY_command_run(Uint8 command) {
	if(clap_watchdog != 0)
		WATCHDOG_stop(clap_watchdog);

	display(state_clap);

	switch(command){
		case ACT_CLAP_HOLLY_IDLE:
			if(state_clap == CLAP_IN)
				break;

			GPIO_ResetBits(CLAP_GOLLY_DCM_SENS);
			clap_watchdog = WATCHDOG_create(ACT_CLAP_HOLLY_TIMEOUT, &CLAP_PWM_stop, FALSE);
			PWM_run(ACT_CLAP_HOLLY_SPEED, CLAP_HOLLY_PWM);
			state_clap = CLAP_IN;
			mouv = TRUE;
			break;
		case ACT_CLAP_HOLLY_RIGHT:
			if(state_clap == CLAP_OUT)
				break;

			clap_watchdog = WATCHDOG_create(ACT_CLAP_HOLLY_TIMEOUT, &CLAP_PWM_stop, FALSE);
			PWM_run(ACT_CLAP_HOLLY_SPEED, CLAP_HOLLY_PWM);
			state_clap = CLAP_OUT;
			GPIO_SetBits(CLAP_GOLLY_DCM_SENS);
			mouv = TRUE;
			break;
		case ACT_CLAP_HOLLY_LEFT:
			break;
		default:
			debug_printf("commande envoy�e � CLAP_GOLLY_DCM_SENS inconnue -> %d	%x\n", command, command);
			PWM_stop(CLAP_HOLLY_PWM);
			break;
	}
}


static void CLAP_PWM_stop(){
	PWM_stop(CLAP_HOLLY_PWM);
	debug_printf("PWM Stop\n");
	mouv = FALSE;
}

#endif


// Fonction appell�e au lancement de la carte (via ActManager)
//void CLAP_HOLLY_init() {
//	static bool_e initialized = FALSE;
//	DCMotor_config_t dcconfig;

//	if(initialized)
//		return;
//	initialized = TRUE;

//	PWM_init();
//}

//static Sint16 CLAP_HOLLY_get_position(){
//	return ADC_getValue(CLAP_HOLLY_ADC_SENSOR);
//}

//// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des serv os
//// Pour �viter les probl�mes d'utilisation de servo non initialis�
//void CLAP_HOLLY_reset_config(){}

//// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
//// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
//void CLAP_HOLLY_config(CAN_msg_t* msg){
//	switch(msg->data[1]){
//		default :
//			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
//	}
//}

//// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
//void CLAP_HOLLY_init_pos(){
//	DCM_setPosValue(CLAP_HOLLY_ID, 0, ACT_CLAP_HOLLY_INIT_POS, ACT_CLAP_HOLLY_SPEED);
//	DCM_goToPos(CLAP_HOLLY_ID, 0);
//	DCM_restart(CLAP_HOLLY_ID);
//}

//// Fonction appell�e � la fin du match (via ActManager)
//void CLAP_HOLLY_stop(){
//	DCM_stop(CLAP_HOLLY_ID);
//}

//// fonction appell�e � la r�ception d'un message CAN (via ActManager)
//bool_e CLAP_HOLLY_CAN_process_msg(CAN_msg_t* msg) {
//	if(msg->sid == ACT_CLAP_HOLLY){
//		switch(msg->data[0]) {
//			// Listing de toutes les positions de l'actionneur possible
//			case ACT_CLAP_HOLLY_IDLE :
//			case ACT_CLAP_HOLLY_LEFT :
//			case ACT_CLAP_HOLLY_RIGHT :
//				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_CLAP_HOLLY, &CLAP_HOLLY_run_command, 0, TRUE);
//				break;

//			case ACT_CONFIG :
//				CLAP_HOLLY_config(msg);
//				break;


//			default:
//				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
//		}
//		return TRUE;
//	}else if(msg->sid == ACT_DO_SELFTEST){
//		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
//		SELFTEST_set_actions(&CLAP_HOLLY_run_command, 6, 3, (SELFTEST_action_t[]){
//								 {ACT_CLAP_HOLLY_LEFT,		0,  QUEUE_ACT_CLAP_HOLLY},
//								 {ACT_CLAP_HOLLY_RIGHT,		0,  QUEUE_ACT_CLAP_HOLLY},
//								 {ACT_CLAP_HOLLY_IDLE,		0,  QUEUE_ACT_CLAP_HOLLY}
//							 });
//	}
//	return FALSE;
//}

//// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
//void CLAP_HOLLY_run_command(queue_id_t queueId, bool_e init) {
//	if(QUEUE_has_error(queueId)) {
//		QUEUE_behead(queueId);
//		return;
//	}

//	if(QUEUE_get_act(queueId) == QUEUE_ACT_CLAP_HOLLY) {    // Gestion des mouvements de CLAP_HOLLY
//		if(init)
//			CLAP_HOLLY_command_init(queueId);
//		else
//			CLAP_HOLLY_command_run(queueId);
//	}
//}

////Initialise une commande
//static void CLAP_HOLLY_command_init(queue_id_t queueId) {
//	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
//	Sint16* dcm_goalPosition = &QUEUE_get_arg(queueId)->param;

//	*dcm_goalPosition = 0x8000;

//	switch(command) {
//		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
//		case ACT_CLAP_HOLLY_IDLE :		*dcm_goalPosition = ACT_CLAP_HOLLY_IDLE_POS; break;
//		case ACT_CLAP_HOLLY_LEFT :		*dcm_goalPosition = ACT_CLAP_HOLLY_LEFT_POS; break;
//		case ACT_CLAP_HOLLY_RIGHT :		*dcm_goalPosition = ACT_CLAP_HOLLY_RIGHT_POS; break;

//		case ACT_CLAP_HOLLY_STOP :
//			DCM_stop(CLAP_HOLLY_ID);
//			QUEUE_next(queueId, ACT_CLAP_HOLLY, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
//			return;

//		default: {
//			error_printf("Invalid exemple command: %u, code is broken !\n", command);
//			QUEUE_next(queueId, ACT_CLAP_HOLLY, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
//			return;
//		}
//	}

//	if(*dcm_goalPosition == 0x8000) {
//		error_printf("Invalid dcm position for command: %u, code is broken !\n", command);
//		QUEUE_next(queueId, ACT_CLAP_HOLLY, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
//		return;
//	}

//	DCM_setPosValue(CLAP_HOLLY_ID, 0, *dcm_goalPosition, ACT_CLAP_HOLLY_SPEED);
//	DCM_goToPos(CLAP_HOLLY_ID, 0);
//	DCM_restart(CLAP_HOLLY_ID);
//	debug_printf("Placement en position %d du moteur DC lanc�\n", *dcm_goalPosition);
//}

////G�re les �tats pendant le mouvement du moteur DC
//static void CLAP_HOLLY_command_run(queue_id_t queueId) {
//	Uint8 result, error_code;
//	Uint16 line;

//	if(ACTQ_check_status_dcmotor(CLAP_HOLLY_ID, FALSE, &result, &error_code, &line))
//		QUEUE_next(queueId, ACT_CLAP_HOLLY, result, error_code, line);
//}

//#endif
