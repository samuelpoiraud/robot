/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : motor_roller_trihole.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur MOTOR_TURN_TRIHOLE
 *  Auteur : Arnaud
 *  Version 2017
 *  Robot : BIG
 */

#include "motor_turn_trihole.h"


// MOTOR_TURN_TRIHOLE d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_MOTOR_TURN_TRIHOLE
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(MOTOR_TURN_TRIHOLE)
// 6) Un #define MOTOR_TURN_TRIHOLE_AX12_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
// 7) Ajout des postions dans QS_types.h dans l'�num ACT_order_e (avec "ACT_" et sans "_POS" � la fin)
// 8) Mise � jour de config/config_debug.h : mettre LOG_PRINT_On
// 9) Include le .h dans act_manager et dans termi_io
// 10) Enlever le #IF 0 qui se trouve quelques lignes

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'�num�ration SELFTEST)

// En strat�gie
// 1) ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// 2) ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'�vitement du robot

// En strat�gie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosit� dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)


// If def � mettre si l'actionneur est seulement pr�sent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotorSpeed.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "motor_turn_trihole_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "MOTOR_TURN_TRIHOLE.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MOTOR_TURN_TRIHOLE
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static DC_MOTOR_SPEED_speed MOTOR_TURN_TRIHOLE_sensorRPM();
static void MOTOR_TURN_TRIHOLE_command_run(queue_id_t queueId);
static void MOTOR_TURN_TRIHOLE_command_init(queue_id_t queueId);

static volatile RPM_SENSOR_id_t id = 0xFF;

// Fonction appell�e au lancement de la carte (via ActManager)
void MOTOR_TURN_TRIHOLE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DC_MOTOR_SPEED_init();

	RPM_SENSOR_init();
	id = RPM_SENSOR_addSensor(MOTOR_TURN_TRIHOLE_PORT_SENSOR, MOTOR_TURN_TRIHOLE_PORT_SENSOR_BIT, MOTOR_TURN_TRIHOLE_SENSOR_EDGE, MOTOR_TURN_TRIHOLE_SENSOR_TICK_PER_REV);

	DC_MOTOR_SPEED_config_t config;
	config.sensorRead = &MOTOR_TURN_TRIHOLE_sensorRPM;

	config.activateRecovery = MOTOR_TURN_TRIHOLE_RECOVERY_MODE;
	config.speedRecovery = MOTOR_TURN_TRIHOLE_RECOVERY_SPEED;
	config.timeRecovery = MOTOR_TURN_TRIHOLE_RECOVERY_TIME;
	config.Kd = MOTOR_TURN_TRIHOLE_KD;
	config.Ki = MOTOR_TURN_TRIHOLE_KI;
	config.Kp = MOTOR_TURN_TRIHOLE_KP;
	config.Kv = MOTOR_TURN_TRIHOLE_KV;
	config.epsilon = MOTOR_TURN_TRIHOLE_EPSILON_SPEED;
	config.max_duty = MOTOR_TURN_TRIHOLE_MAX_PWM;
	config.pwm_number = MOTOR_TURN_TRIHOLE_PWM_NUM;
	config.simulateWay = MOTOR_TURN_TRIHOLE_SIMUATE_WAY;
	config.timeout = MOTOR_TURN_TRIHOLE_TIMEOUT;
	config.way_bit_number = MOTOR_TURN_TRIHOLE_PORT_WAY_BIT;
	config.way_latch = MOTOR_TURN_TRIHOLE_PORT_WAY;

	DC_MOTOR_SPEED_config(MOTOR_TURN_TRIHOLE_ID, &config);
}

static DC_MOTOR_SPEED_speed MOTOR_TURN_TRIHOLE_sensorRPM(){
	return RPM_SENSOR_getSpeed(id);
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void MOTOR_TURN_TRIHOLE_reset_config(){}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
/// Ignor� ici mise en position fait par le process_main
void MOTOR_TURN_TRIHOLE_init_pos(){}

// Fonction appell�e � la fin du match (via ActManager)
void MOTOR_TURN_TRIHOLE_stop(){
	DC_MOTOR_SPEED_stop(MOTOR_TURN_TRIHOLE_ID);
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e MOTOR_TURN_TRIHOLE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ORE_TRIHOLE){
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_ORE_TRIHOLE_STOP :
			case ACT_ORE_TRIHOLE_IDLE :
			case ACT_ORE_TRIHOLE_RUN :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_MOTOR_ORE_TURN_TRIHOLE, &MOTOR_TURN_TRIHOLE_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void MOTOR_TURN_TRIHOLE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_MOTOR_ORE_TURN_TRIHOLE) {    // Gestion des mouvements de ELEVATOR
		if(init)
			MOTOR_TURN_TRIHOLE_command_init(queueId);
		else
			MOTOR_TURN_TRIHOLE_command_run(queueId);
	}
}

//Initialise une commande
static void MOTOR_TURN_TRIHOLE_command_init(queue_id_t queueId) {
	Sint16 dcm_goalSpeed = QUEUE_get_arg(queueId)->param;

	if(dcm_goalSpeed != 0){
		DC_MOTOR_SPEED_setSpeed(MOTOR_TURN_TRIHOLE_ID, dcm_goalSpeed);
		DC_MOTOR_SPEED_restart(MOTOR_TURN_TRIHOLE_ID);
		debug_printf("Mise � jours de la vitesse (%d rpm) du moteur DC\n", dcm_goalSpeed);
	}else{
		DC_MOTOR_SPEED_stop(MOTOR_TURN_TRIHOLE_ID);
		QUEUE_next(queueId, ACT_ORE_TRIHOLE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
		debug_printf("Arr�t de la rotation du moteur DC MOTOR_TURN_TRIHOLE\n");
	}
}

//G�re les �tats pendant le mouvement du moteur DC
static void MOTOR_TURN_TRIHOLE_command_run(queue_id_t queueId) {
	Uint8 result, error_code;
	Uint16 line;

	if(ACTQ_check_status_dcMotorSpeed(MOTOR_TURN_TRIHOLE_ID, &result, &error_code, &line)){
		QUEUE_next(queueId, ACT_ORE_TRIHOLE, result, error_code, line);
	}
}

#endif
