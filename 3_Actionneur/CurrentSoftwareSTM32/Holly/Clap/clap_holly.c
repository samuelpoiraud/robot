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

#include "clap_holly_config.h"

#define LOG_PREFIX "clap_holly : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CLAP_HOLLY
#include "../QS/QS_outputlog.h"

static void CLAP_HOLLY_command_run(queue_id_t queueId);
static void CLAP_HOLLY_command_init(queue_id_t queueId);
static Sint16 CLAP_HOLLY_get_position();

// Fonction appell�e au lancement de la carte (via ActManager)
void CLAP_HOLLY_init() {
	static bool_e initialized = FALSE;
	DCMotor_config_t dcconfig;

	if(initialized)
		return;
	initialized = TRUE;

	PWM_init();
	DCM_init();
	dcconfig.sensor_read = &CLAP_HOLLY_get_position;
	dcconfig.double_PID = FALSE;
	dcconfig.Kp = CLAP_HOLLY_KP;
	dcconfig.Ki = CLAP_HOLLY_KI;
	dcconfig.Kd = CLAP_HOLLY_KD;
	dcconfig.pos[0] = 0;
	dcconfig.pwm_number = CLAP_HOLLY_PWM_NUM;
	dcconfig.way_latch = CLAP_HOLLY_PORT_WAY;
	dcconfig.way_bit_number = CLAP_HOLLY_PORT_WAY_BIT;
	dcconfig.way0_max_duty = CLAP_HOLLY_MAX_PWM_WAY0;
	dcconfig.way1_max_duty = CLAP_HOLLY_MAX_PWM_WAY1;
	dcconfig.timeout = CLAP_HOLLY_ASSER_TIMEOUT;
	dcconfig.epsilon = CLAP_HOLLY_ASSER_POS_EPSILON;
	DCM_config(CLAP_HOLLY_ID, &dcconfig);
	DCM_stop(CLAP_HOLLY_ID);
}

static Sint16 CLAP_HOLLY_get_position(){
	return ADC_getValue(CLAP_HOLLY_ADC_SENSOR);
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des serv os
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void CLAP_HOLLY_reset_config(){}

// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void CLAP_HOLLY_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void CLAP_HOLLY_init_pos(){
	DCM_setPosValue(CLAP_HOLLY_ID, 0, ACT_CLAP_HOLLY_INIT_POS);
	DCM_goToPos(CLAP_HOLLY_ID, 0);
	DCM_restart(CLAP_HOLLY_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void CLAP_HOLLY_stop(){
	DCM_stop(CLAP_HOLLY_ID);
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e CLAP_HOLLY_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CLAP_HOLLY){
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_CLAP_HOLLY_IDLE :
			case ACT_CLAP_HOLLY_LEFT :
			case ACT_CLAP_HOLLY_RIGHT :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_CLAP_HOLLY, &CLAP_HOLLY_run_command, 0, TRUE);
				break;

			case ACT_CONFIG :
				CLAP_HOLLY_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
		SELFTEST_set_actions(&CLAP_HOLLY_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_CLAP_HOLLY_LEFT,		0,  QUEUE_ACT_CLAP_HOLLY},
								 {ACT_CLAP_HOLLY_RIGHT,		0,  QUEUE_ACT_CLAP_HOLLY},
								 {ACT_CLAP_HOLLY_IDLE,		0,  QUEUE_ACT_CLAP_HOLLY}
							 });
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void CLAP_HOLLY_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_CLAP_HOLLY) {    // Gestion des mouvements de CLAP_HOLLY
		if(init)
			CLAP_HOLLY_command_init(queueId);
		else
			CLAP_HOLLY_command_run(queueId);
	}
}

//Initialise une commande
static void CLAP_HOLLY_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* dcm_goalPosition = &QUEUE_get_arg(queueId)->param;

	*dcm_goalPosition = 0x8000;

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_CLAP_HOLLY_IDLE :		*dcm_goalPosition = ACT_CLAP_HOLLY_IDLE_POS; break;
		case ACT_CLAP_HOLLY_LEFT :		*dcm_goalPosition = ACT_CLAP_HOLLY_LEFT_POS; break;
		case ACT_CLAP_HOLLY_RIGHT :		*dcm_goalPosition = ACT_CLAP_HOLLY_RIGHT_POS; break;

		case ACT_CLAP_HOLLY_STOP :
			DCM_stop(CLAP_HOLLY_ID);
			QUEUE_next(queueId, ACT_CLAP_HOLLY, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_CLAP_HOLLY, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(*dcm_goalPosition == 0x8000) {
		error_printf("Invalid dcm position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_CLAP_HOLLY, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	DCM_setPosValue(CLAP_HOLLY_ID, 0, *dcm_goalPosition);
	DCM_goToPos(CLAP_HOLLY_ID, 0);
	DCM_restart(CLAP_HOLLY_ID);
	debug_printf("Placement en position %d du moteur DC lanc�\n", *dcm_goalPosition);
}

//G�re les �tats pendant le mouvement du moteur DC
static void CLAP_HOLLY_command_run(queue_id_t queueId) {
	Uint8 result, error_code;
	Uint16 line;

	if(ACTQ_check_status_dcmotor(CLAP_HOLLY_ID, FALSE, &result, &error_code, &line))
		QUEUE_next(queueId, ACT_ELEVATOR, result, error_code, line);
}

#endif
