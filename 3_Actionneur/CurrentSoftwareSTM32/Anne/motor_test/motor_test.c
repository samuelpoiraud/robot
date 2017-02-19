/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : MOTOR_TEST_ax12.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 MOTOR_TEST
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */

#include "MOTOR_TEST.h"


// MOTOR_TEST d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_MOTOR_TEST
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(MOTOR_TEST)
// 6) Un #define MOTOR_TEST_AX12_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
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
#ifdef I_AM_ROBOT_SMALL

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rpm_sensor.h"
#include "../QS/QS_DCMotorSpeed.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "motor_test_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "MOTOR_TEST.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MOTOR_TEST
#include "../QS/QS_outputlog.h"

#define NB_TICK_PER_REV		2

// Les fonctions internes au fonctionnement de l'actionneur
static void MOTOR_TEST_config(CAN_msg_t* msg);
static DC_MOTOR_SPEED_speed MOTOR_TEST_sensorRPM();

static volatile RPM_SENSOR_id_t id = 0xFF;

// Fonction appell�e au lancement de la carte (via ActManager)
void MOTOR_TEST_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DC_MOTOR_SPEED_init();

	RPM_SENSOR_init();
	id = RPM_SENSOR_addSensor(EXTIT_GpioB, 0, EXTIT_Edge_Rising, NB_TICK_PER_REV);

	DC_MOTOR_SPEED_config_t config;
	config.activateRecovery = TRUE;
	config.speedRecovery = - 25;
	config.timeRecovery = 1500;
	config.Kd = 0;
	config.Ki = 0;
	config.Kp = 50;
	config.Kv = 256;
	config.epsilon = 8;
	config.max_duty = 100;
	config.pwm_number = 1;
	config.sensorRead = &MOTOR_TEST_sensorRPM;
	config.simulateWay = TRUE;
	config.timeout = 500;
	config.way_bit_number = 8;
	config.way_latch = GPIOA;

	DC_MOTOR_SPEED_config(0, &config);
}

static DC_MOTOR_SPEED_speed MOTOR_TEST_sensorRPM(){
	return RPM_SENSOR_getSpeed(id);
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void MOTOR_TEST_reset_config(){
	DC_MOTOR_SPEED_setSpeed(0, 120);
}

// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void MOTOR_TEST_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur

			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void MOTOR_TEST_init_pos(){
	DC_MOTOR_SPEED_setSpeed(0, 120);

}

// Fonction appell�e � la fin du match (via ActManager)
void MOTOR_TEST_stop(){

}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e MOTOR_TEST_CAN_process_msg(CAN_msg_t* msg) {

	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void MOTOR_TEST_run_command(queue_id_t queueId, bool_e init) {

}

void MOTOR_TEST_test(){
	display(RPM_SENSOR_getSpeed(id));
}
#endif
