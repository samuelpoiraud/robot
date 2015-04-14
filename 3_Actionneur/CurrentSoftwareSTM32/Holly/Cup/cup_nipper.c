/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : cup_nipper.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pince à gobelet
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#include "cup_nipper.h"

#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_qei.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "cup_nipper_config.h"

#define LOG_PREFIX "cup_nipper : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CUP_NIPPER
#include "../QS/QS_outputlog.h"

static volatile bool_e CUP_NIPPER_ready = FALSE;
static volatile bool_e encoder_ready = FALSE;
static volatile bool_e ax12_is_initialized = FALSE;
static volatile Sint16 CUP_NIPPER_position = 0;

static void CUP_NIPPER_DCM_command_run(queue_id_t queueId);
static void CUP_NIPPER_DCM_command_init(queue_id_t queueId);
static void CUP_NIPPER_AX12_command_run(queue_id_t queueId);
static void CUP_NIPPER_AX12_command_init(queue_id_t queueId);
static Sint16 CUP_NIPPER_get_position();
static void CUP_NIPPER_initAX12();

// Fonction appellée au lancement de la carte (via ActManager)
void CUP_NIPPER_init() {
	static bool_e initialized = FALSE;
	DCMotor_config_t dcconfig;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	CUP_NIPPER_initAX12();

	PWM_init();
	DCM_init();
	dcconfig.sensor_read = &CUP_NIPPER_get_position;
	dcconfig.double_PID = TRUE;
	dcconfig.Kp = CUP_NIPPER_KP;
	dcconfig.Ki = CUP_NIPPER_KI;
	dcconfig.Kd = CUP_NIPPER_KD;
	dcconfig.Kp2 = CUP_NIPPER_KP2;
	dcconfig.Ki2 = CUP_NIPPER_KI2;
	dcconfig.Kd2 = CUP_NIPPER_KD2;
	dcconfig.pos[0] = 0;
	dcconfig.speed[0] = 0;
	dcconfig.pwm_number = CUP_NIPPER_PWM_NUM;
	dcconfig.way_latch = CUP_NIPPER_PORT_WAY;
	dcconfig.way_bit_number = CUP_NIPPER_PORT_WAY_BIT;
	dcconfig.way0_max_duty = CUP_NIPPER_MAX_PWM_WAY0;
	dcconfig.way1_max_duty = CUP_NIPPER_MAX_PWM_WAY1;
	dcconfig.timeout = CUP_NIPPER_DCM_ASSER_TIMEOUT;
	dcconfig.epsilon = CUP_NIPPER_DCM_ASSER_POS_EPSILON;
	DCM_config(CUP_NIPPER_ID, &dcconfig);
	DCM_stop(CUP_NIPPER_ID);
}

void CUP_NIPPER_state_machine(){
	typedef enum{
		INIT,
		WAIT_FDC,
		INIT_POS,
		WAIT_POS,
		INIT_AX12,
		RUN
	}state_e;
	static state_e state = INIT;
	Uint8 result, error_code;
	Uint16 line;

	switch (state) {
		case INIT:
			CUP_NIPPER_init();
			GPIO_SetBits(CUP_NIPPER_PORT_WAY, CUP_NIPPER_PORT_WAY_BIT);
			PWM_run(15, CUP_NIPPER_PWM_NUM);
			state = WAIT_FDC;
			break;

		case WAIT_FDC:
			if(CUP_NIPPER_FDC){
				PWM_run(0, CUP_NIPPER_PWM_NUM);
				encoder_ready = TRUE;
				QEI2_set_count(0);
				state = INIT_POS;
			}

			break;

		case INIT_POS:
			DCM_setPosValue(CUP_NIPPER_ID, 0, CUP_NIPPER_DCM_INIT_POS, CUP_NIPPER_DCM_SPEED);
			DCM_goToPos(CUP_NIPPER_ID, 0);
			DCM_restart(CUP_NIPPER_ID);
			state = WAIT_POS;
			break;

		case WAIT_POS:
			if(ACTQ_check_status_dcmotor(CUP_NIPPER_ID, FALSE, &result, &error_code, &line)){
				if(result == ACT_RESULT_DONE){
					CUP_NIPPER_ready = TRUE;
					state = INIT_AX12;
				}else{
					DCM_stop(CUP_NIPPER_ID);
					state = INIT;
				}
			}
			break;

		case INIT_AX12:
			debug_printf("Init pos : \n");
			if(!AX12_set_position(CUP_NIPPER_AX12_ID, CUP_NIPPER_AX12_INIT_POS))
				debug_printf("   L'AX12 n°%d n'est pas là\n", CUP_NIPPER_AX12_ID);
			else
				debug_printf("   L'AX12 n°%d a été initialisé en position\n", CUP_NIPPER_AX12_ID);
			break;

		case RUN:
			break;
	}
}

void CUP_NIPPER_process_it(){
	if(encoder_ready){
		static Sint16 count_prec = 0;
		Sint16 count = QEI2_get_count();
		Sint32 delta = count - count_prec;

		if(delta > 512)
			delta -= 65536;
		else if(delta < -512)
			delta += 65536;

		count_prec = count;

		CUP_NIPPER_position += delta * CUP_NIPPER_QEI_COEF;
	}
}

static Sint16 CUP_NIPPER_get_position(){
	return CUP_NIPPER_position;
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void CUP_NIPPER_reset_config(){
	ax12_is_initialized = FALSE;
	CUP_NIPPER_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void CUP_NIPPER_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(CUP_NIPPER_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(CUP_NIPPER_AX12_ID, 136);
		AX12_config_set_lowest_voltage(CUP_NIPPER_AX12_ID, 70);
		AX12_set_torque_limit(CUP_NIPPER_AX12_ID, CUP_NIPPER_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(CUP_NIPPER_AX12_ID, CUP_NIPPER_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(BACK_SPOT_LEFT_AX12_ID, CUP_NIPPER_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(CUP_NIPPER_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(CUP_NIPPER_AX12_ID, AX12_ERROR_OVERHEATING);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}

// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void CUP_NIPPER_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0 :
			ACTMGR_config_AX12(CUP_NIPPER_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void CUP_NIPPER_init_pos(){
	CUP_NIPPER_initAX12();

	if(ax12_is_initialized == FALSE)
		return;


}

// Fonction appellée à la fin du match (via ActManager)
void CUP_NIPPER_stop(){
	AX12_set_torque_enabled(CUP_NIPPER_AX12_ID, FALSE);
	DCM_stop(CUP_NIPPER_ID);
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e CUP_NIPPER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CUP_NIPPER){
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_CUP_NIPPER_OPEN :
			case ACT_CUP_NIPPER_CLOSE :
			case ACT_CUP_NIPPER_LOCK :
			case ACT_CUP_NIPPER_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_CUP_NIPPER, &CUP_NIPPER_run_command, 0, TRUE);
				break;

			case ACT_CONFIG :
				CUP_NIPPER_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_CUP_NIPPER_ELEVATOR){
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_CUP_NIPPER_ELEVATOR_IDLE :
			case ACT_CUP_NIPPER_ELEVATOR_UP :
			case ACT_CUP_NIPPER_ELEVATOR_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_DCM_CUP_NIPPER, &CUP_NIPPER_run_command, 0, TRUE);
				break;

			case ACT_CONFIG :
				warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		SELFTEST_set_actions(&CUP_NIPPER_run_command, 15, 3, (SELFTEST_action_t[]){
								 {ACT_CUP_NIPPER_OPEN,				0,  QUEUE_ACT_AX12_CUP_NIPPER},
								 {ACT_CUP_NIPPER_CLOSE,				0,  QUEUE_ACT_AX12_CUP_NIPPER},
								 {ACT_CUP_NIPPER_OPEN,				0,  QUEUE_ACT_AX12_CUP_NIPPER},
							 });

		SELFTEST_set_actions(&CUP_NIPPER_run_command, 15, 3, (SELFTEST_action_t[]){
								 {ACT_CUP_NIPPER_ELEVATOR_IDLE,		0,  QUEUE_ACT_DCM_CUP_NIPPER},
								 {ACT_CUP_NIPPER_ELEVATOR_UP,		0,  QUEUE_ACT_DCM_CUP_NIPPER},
								 {ACT_CUP_NIPPER_ELEVATOR_IDLE,		0,  QUEUE_ACT_DCM_CUP_NIPPER}
							 });
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void CUP_NIPPER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_CUP_NIPPER) {    // Gestion des mouvements de l'AX12 CUP_NIPPER
		if(init)
			CUP_NIPPER_AX12_command_init(queueId);
		else
			CUP_NIPPER_AX12_command_run(queueId);
	}else if(QUEUE_get_act(queueId) == QUEUE_ACT_DCM_CUP_NIPPER) {    // Gestion des mouvements du moteur CUP_NIPPER
		if(init)
			CUP_NIPPER_DCM_command_init(queueId);
		else
			CUP_NIPPER_DCM_command_run(queueId);
	}
}

//Initialise une commande
static void CUP_NIPPER_DCM_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* dcm_goalPosition = &QUEUE_get_arg(queueId)->param;

	*dcm_goalPosition = 0x8000;

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_CUP_NIPPER_ELEVATOR_IDLE :		*dcm_goalPosition = CUP_NIPPER_DCM_IDLE_POS; break;
		case ACT_CUP_NIPPER_ELEVATOR_UP :		*dcm_goalPosition = CUP_NIPPER_DCM_UP_POS; break;

		case ACT_CUP_NIPPER_STOP :
			DCM_stop(CUP_NIPPER_ID);
			QUEUE_next(queueId, ACT_CUP_NIPPER_ELEVATOR, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_CUP_NIPPER_ELEVATOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(CUP_NIPPER_ready == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_CUP_NIPPER_ELEVATOR, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*dcm_goalPosition == 0x8000) {
		error_printf("Invalid dcm position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_CUP_NIPPER_ELEVATOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	DCM_setPosValue(CUP_NIPPER_ID, 0, *dcm_goalPosition, CUP_NIPPER_DCM_SPEED);
	DCM_goToPos(CUP_NIPPER_ID, 0);
	DCM_restart(CUP_NIPPER_ID);
	debug_printf("Placement en position %d du moteur DC lancé\n", *dcm_goalPosition);
}

//Gère les états pendant le mouvement du moteur DC
static void CUP_NIPPER_DCM_command_run(queue_id_t queueId) {
	Uint8 result, error_code;
	Uint16 line;

	if(ACTQ_check_status_dcmotor(CUP_NIPPER_ID, FALSE, &result, &error_code, &line))
		QUEUE_next(queueId, ACT_CUP_NIPPER_ELEVATOR, result, error_code, line);
}

//Initialise une commande
static void CUP_NIPPER_AX12_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	CUP_NIPPER_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_CUP_NIPPER_OPEN : *ax12_goalPosition = CUP_NIPPER_AX12_OPEN_POS; break;
		case ACT_CUP_NIPPER_CLOSE : *ax12_goalPosition = CUP_NIPPER_AX12_CLOSE_POS; break;
		case ACT_CUP_NIPPER_LOCK : *ax12_goalPosition = CUP_NIPPER_AX12_LOCK_POS; break;

		case ACT_CUP_NIPPER_STOP :
			AX12_set_torque_enabled(CUP_NIPPER_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_CUP_NIPPER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_CUP_NIPPER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_CUP_NIPPER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_CUP_NIPPER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(CUP_NIPPER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(CUP_NIPPER_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(CUP_NIPPER_AX12_ID).error);
		QUEUE_next(queueId, ACT_CUP_NIPPER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
	debug_printf("Move cup_nipper ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void CUP_NIPPER_AX12_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, CUP_NIPPER_AX12_ID, QUEUE_get_arg(queueId)->param, CUP_NIPPER_AX12_ASSER_POS_EPSILON, CUP_NIPPER_AX12_ASSER_TIMEOUT, CUP_NIPPER_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_CUP_NIPPER, result, errorCode, line);
}

#endif
