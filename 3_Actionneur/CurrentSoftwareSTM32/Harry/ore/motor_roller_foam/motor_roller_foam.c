/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : motor_roller_foam.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur MOTOR_ROLLER_FOAM
 *  Auteur : Arnaud
 *  Version 2017
 *  Robot : BIG
 */

#include "motor_roller_foam.h"


// MOTOR_ROLLER_FOAM d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_MOTOR_ROLLER_FOAM
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(MOTOR_ROLLER_FOAM)
// 6) Un #define MOTOR_ROLLER_FOAM_AX12_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
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
#include "../QS/QS_rpm_sensor.h"
#include "../QS/QS_DCMotorSpeed.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "motor_roller_foam_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "MOTOR_ROLLER_FOAM.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MOTOR_ROLLER_FOAM
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static DC_MOTOR_SPEED_speed MOTOR_ROLLER_FOAM_sensorRPM();
static void MOTOR_ROLLER_FOAM_command_run(queue_id_t queueId);
static void MOTOR_ROLLER_FOAM_command_init(queue_id_t queueId);

static volatile RPM_SENSOR_id_t id = 0xFF;

// Fonction appell�e au lancement de la carte (via ActManager)
void MOTOR_ROLLER_FOAM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DC_MOTOR_SPEED_init();

	RPM_SENSOR_init();
	id = RPM_SENSOR_addSensor(MOTOR_ROLLER_FOAM_PORT_SENSOR, MOTOR_ROLLER_FOAM_PORT_SENSOR_BIT, MOTOR_ROLLER_FOAM_SENSOR_EDGE, MOTOR_ROLLER_FOAM_SENSOR_TICK_PER_REV);

	DC_MOTOR_SPEED_config_t config;
	config.sensorRead = &MOTOR_ROLLER_FOAM_sensorRPM;

	config.activateRecovery = MOTOR_ROLLER_FOAM_RECOVERY_MODE;
	config.speedRecovery = MOTOR_ROLLER_FOAM_RECOVERY_SPEED;
	config.timeRecovery = MOTOR_ROLLER_FOAM_RECOVERY_TIME;
	config.Kd = MOTOR_ROLLER_FOAM_KD;
	config.Ki = MOTOR_ROLLER_FOAM_KI;
	config.Kp = MOTOR_ROLLER_FOAM_KP;
	config.Kv = MOTOR_ROLLER_FOAM_KV;
	config.epsilon = MOTOR_ROLLER_FOAM_EPSILON_SPEED;
	config.max_duty = MOTOR_ROLLER_FOAM_MAX_PWM;
	config.pwm_number = MOTOR_ROLLER_FOAM_PWM_NUM;
	config.simulateWay = MOTOR_ROLLER_FOAM_SIMUATE_WAY;
	config.timeout = MOTOR_ROLLER_FOAM_TIMEOUT;
	config.way_bit_number = MOTOR_ROLLER_FOAM_PORT_WAY_BIT;
	config.way_latch = MOTOR_ROLLER_FOAM_PORT_WAY;

	DC_MOTOR_SPEED_config(MOTOR_ROLLER_FOAM_ID, &config);
}

static DC_MOTOR_SPEED_speed MOTOR_ROLLER_FOAM_sensorRPM(){
	return RPM_SENSOR_getSpeed(id);
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void MOTOR_ROLLER_FOAM_reset_config(){}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
/// Ignor� ici mise en position fait par le process_main
void MOTOR_ROLLER_FOAM_init_pos(){}

// Fonction appell�e � la fin du match (via ActManager)
void MOTOR_ROLLER_FOAM_stop(){
	DC_MOTOR_SPEED_stop(MOTOR_ROLLER_FOAM_ID);
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e MOTOR_ROLLER_FOAM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ORE_ROLLER_FOAM){
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_ORE_ROLLER_FOAM_STOP :
			case ACT_ORE_ROLLER_FOAM_IDLE :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_MOTOR_ORE_ROLLER_FOAM, &MOTOR_ROLLER_FOAM_run_command, 0, TRUE);
				break;

			case ACT_ORE_ROLLER_FOAM_RUN :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_MOTOR_ORE_ROLLER_FOAM, &MOTOR_ROLLER_FOAM_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void MOTOR_ROLLER_FOAM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_MOTOR_ORE_ROLLER_FOAM) {    // Gestion des mouvements de ELEVATOR
		if(init)
			MOTOR_ROLLER_FOAM_command_init(queueId);
		else
			MOTOR_ROLLER_FOAM_command_run(queueId);
	}
}

//Initialise une commande
static void MOTOR_ROLLER_FOAM_command_init(queue_id_t queueId) {
	Sint16 dcm_goalSpeed = QUEUE_get_arg(queueId)->param;

	if(dcm_goalSpeed != 0){
		DC_MOTOR_SPEED_setSpeed(MOTOR_ROLLER_FOAM_ID, dcm_goalSpeed);
		DC_MOTOR_SPEED_restart(MOTOR_ROLLER_FOAM_ID);
		debug_printf("Mise � jours de la vitesse (%d rpm) du moteur DC\n", dcm_goalSpeed);
	}else{
		DC_MOTOR_SPEED_stop(MOTOR_ROLLER_FOAM_ID);
		QUEUE_next(queueId, ACT_ORE_ROLLER_FOAM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
		debug_printf("Arr�t de la rotation du moteur DC MOTOR_ROLLER_FOAM\n");
	}
}

//G�re les �tats pendant le mouvement du moteur DC
static void MOTOR_ROLLER_FOAM_command_run(queue_id_t queueId) {
	Uint8 result, error_code;
	Uint16 line;

	if(ACTQ_check_status_dcMotorSpeed(MOTOR_ROLLER_FOAM_ID, &result, &error_code, &line)){
		QUEUE_next(queueId, ACT_ORE_ROLLER_FOAM, result, error_code, line);
	}
}

#endif
