/*  Club Robot ESEO 2012 - 2013
 *
 *	Fichier : ActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs
 *  Auteur : Alexis, remake Arnaud
 *  Version 20130227
 */
#include "ActManager.h"

#include "QS/QS_CANmsgList.h"
#include "QS/QS_ax12.h"
#include "QS/QS_rx24.h"
#include "QS/QS_outputlog.h"
#include "act_queue_utils.h"
#include "selftest.h"

#ifdef I_AM_ROBOT_BIG
	#include "QS/QS_mosfet.h"
	#include "Harry/ore/ore_gun/ore_gun.h"
    #include "Harry/big_bearing_ball/big_ball_back_left/big_ball_back_left.h"
    #include "Harry/big_bearing_ball/big_ball_back_right/big_ball_back_right.h"
    #include "Harry/big_bearing_ball/big_ball_front_left/big_ball_front_left.h"
    #include "Harry/big_bearing_ball/big_ball_front_right/big_ball_front_right.h"
	#include "Harry/cylinder/cylinder_hatch/cylinder_hatch_left.h"
	#include "Harry/cylinder/cylinder_hatch/cylinder_hatch_right.h"
	#include "Harry/cylinder/cylinder_slope/cylinder_slope_left.h"
	#include "Harry/cylinder/cylinder_slope/cylinder_slope_right.h"
	#include "Harry/cylinder/cylinder_dispenser/cylinder_dispenser_left.h"
	#include "Harry/cylinder/cylinder_dispenser/cylinder_dispenser_right.h"
    #include "Harry/big_bearing_ball/bearing_ball_wheel/bearing_ball_wheel.h"
    #include "Harry/cylinder/cylinder_pusher/cylinder_pusher_left.h"
    #include "Harry/cylinder/cylinder_pusher/cylinder_pusher_right.h"
    #include "Harry/cylinder/cylinder_elevator/cylinder_elevator_left.h"
    #include "Harry/cylinder/cylinder_elevator/cylinder_elevator_right.h"
    #include "Harry/cylinder/cylinder_slider/cylinder_slider_left.h"
    #include "Harry/cylinder/cylinder_slider/cylinder_slider_right.h"
    #include "Harry/cylinder/cylinder_turn_arm/cylinder_turn_left_arm.h"
    #include "Harry/cylinder/cylinder_turn_arm/cylinder_turn_right_arm.h"
	#include "Harry/ore/ore_wall/ore_wall.h"
    #include "Harry/ore/ore_roller_arm/ore_roller_arm.h"
    #include "Harry/rocket/rocket.h"





#else
	#include "QS/QS_mosfet.h"
	#include "Anne/small_bearing_ball/small_ball_back_left/small_ball_back_left.h"
	#include "Anne/small_bearing_ball/small_ball_back_right/small_ball_back_right.h"
	#include "Anne/small_bearing_ball/small_ball_front_left/small_ball_front_left.h"
	#include "Anne/small_bearing_ball/small_ball_front_right/small_ball_front_right.h"
	#include "Anne/motor_test/motor_test.h"
#endif


static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);
static Uint8 index_AX12;
static Uint8 index_RX24;
static bool_e detection_error = FALSE;
static time32_t time_error;
static time32_t check_act_error_time;
static bool_e init_control_act=TRUE;

#define ACT_DECLARE(prefix) {&prefix##_init, &prefix##_init_pos, &prefix##_stop, &prefix##_reset_config, &prefix##_CAN_process_msg}

static ACTQ_functions_t actionneurs[] = {
		ACT_DECLARE(MOSFET), //QS_mosfets
	#ifdef I_AM_ROBOT_BIG  //Big Robot
		ACT_DECLARE(ORE_GUN),
        ACT_DECLARE(BIG_BALL_BACK_LEFT),
        ACT_DECLARE(BIG_BALL_BACK_RIGHT),
        ACT_DECLARE(BIG_BALL_FRONT_LEFT),
        ACT_DECLARE(BIG_BALL_FRONT_RIGHT),
		ACT_DECLARE(CYLINDER_HATCH_LEFT),
		ACT_DECLARE(CYLINDER_HATCH_RIGHT),
		ACT_DECLARE(CYLINDER_SLOPE_LEFT),
		ACT_DECLARE(CYLINDER_SLOPE_RIGHT),
		ACT_DECLARE(CYLINDER_DISPENSER_LEFT),
		ACT_DECLARE(CYLINDER_DISPENSER_RIGHT),
        ACT_DECLARE(BEARING_BALL_WHEEL),
        ACT_DECLARE(CYLINDER_PUSHER_LEFT),
        ACT_DECLARE(CYLINDER_PUSHER_RIGHT),
        ACT_DECLARE(CYLINDER_ELEVATOR_LEFT),
        ACT_DECLARE(CYLINDER_ELEVATOR_RIGHT),
        ACT_DECLARE(CYLINDER_SLIDER_LEFT),
        ACT_DECLARE(CYLINDER_SLIDER_RIGHT),
        ACT_DECLARE(CYLINDER_TURN_LEFT_ARM),
        ACT_DECLARE(CYLINDER_TURN_RIGHT_ARM),
		ACT_DECLARE(ORE_WALL),
        ACT_DECLARE(ORE_ROLLER_ARM),
        ACT_DECLARE(ROCKET),

	#else  //Small Robot
		//ACT_DECLARE(LEFT_ARM),
		/*ACT_DECLARE(SMALL_BALL_BACK_LEFT),
		ACT_DECLARE(SMALL_BALL_BACK_RIGTH),
		ACT_DECLARE(SMALL_BALL_FRONT_LEFT),
		ACT_DECLARE(SMALL_BALL_FRONT_RIGHT),*/
		ACT_DECLARE(MOTOR_TEST),
	#endif
};

static const Uint8 NB_ACTIONNEURS = sizeof(actionneurs) / sizeof(ACTQ_functions_t);

void ACTMGR_init() {
	Uint8 i;
	debug_printf("Init de %d actionneurs\n", NB_ACTIONNEURS);
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		if(actionneurs[i].onInit != NULL)
			actionneurs[i].onInit();
	}

	ACTMGR_reset_act();
}

void ACTMGR_reset_config() {
	Uint8 i;
	debug_printf("Re-init de %d actionneurs\n", NB_ACTIONNEURS);
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		if(actionneurs[i].onResetConfig != NULL)
			actionneurs[i].onResetConfig();
	}
}

//Met les actionneurs en position de départ
void ACTMGR_reset_act() {
	queue_id_t queueId;

	queueId = QUEUE_create();
	QUEUE_add(queueId, &ACTMGR_run_reset_act, (QUEUE_arg_t){0, 0, NULL}, 0);
}

//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg) {
	//Traitement des messages, si la fonction de traitement d'un actionneur retourne TRUE, c'est que le message a été traité donc arrête le passage du message dans chaque fonction de traitement. (les fonctions de traitement des actionneurs sont responsable de retourner FALSE lorsque le message n'est pas destiné qu'a un actionneur)

	Uint8 i;

	if(msg->sid == ACT_DO_SELFTEST) {
		if(SELFTEST_new_selftest(NB_ACTIONNEURS) == FALSE)
			return TRUE;  //si selftest déjà en cours, ne pas le refaire
	}

	for(i = 0; i < NB_ACTIONNEURS; i++) {
		//Dans le cas du selftest, on fait le test pour tous les actionneurs, qu'ils gèrent ou non le message
		if(actionneurs[i].onCanMsg != NULL)
			if(actionneurs[i].onCanMsg(msg) && msg->sid != ACT_DO_SELFTEST)
				return TRUE;
	}

	return FALSE;
}

void ACTMGR_stop() {
	Uint8 i;
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		if(actionneurs[i].onStop != NULL)
			actionneurs[i].onStop();
	}
}

void ACTMNG_control_act() {
	AX12_status_t status_AX12;
	RX24_status_t status_RX24;
	bool_e state_act;

	UNUSED_VAR(state_act);

	if(init_control_act){
		check_act_error_time=global.absolute_time;
		time_error=global.absolute_time;
		init_control_act=FALSE;
	}
	//Délai de 200 ms entre chaque vérification de servo
	if(global.absolute_time - check_act_error_time > 200){

		if(!detection_error){
			//Met à jour l'état du servo
			state_act = AX12_is_ready(index_AX12);
			//Check la dernière erreur rencontrée
			status_AX12 = AX12_get_last_error(index_AX12);

			if(status_AX12.error & AX12_ERROR_OVERLOAD){
				ACTQ_sendErrorAct(index_AX12, ACT_ERROR_TORQUE_TOO_HIGH);
				detection_error=TRUE;
				time_error = global.absolute_time;
			}
			else if(status_AX12.error & AX12_ERROR_OVERHEATING){
				ACTQ_sendErrorAct(index_AX12, ACT_ERROR_OVERHEATING);
				detection_error=TRUE;
				time_error = global.absolute_time;
			}
			index_AX12=(index_AX12+1)%AX12_NUMBER;

			state_act = RX24_is_ready(index_RX24);
			status_RX24 = RX24_get_last_error(index_RX24);
			if(status_RX24.error & RX24_ERROR_OVERLOAD){
				ACTQ_sendErrorAct(index_RX24, ACT_ERROR_TORQUE_TOO_HIGH);
				detection_error=TRUE;
				time_error = global.absolute_time;
			}
			else if(status_RX24.error & RX24_ERROR_OVERHEATING){
				ACTQ_sendErrorAct(index_RX24, ACT_ERROR_OVERHEATING);
				detection_error=TRUE;
				time_error = global.absolute_time;
			}
			index_RX24=(index_RX24+1)%RX24_NUMBER;

		//pour éviter de flood le CAN avec le même message d'erreur
		}else if(detection_error == TRUE && global.absolute_time - time_error > 2000){
			detection_error = FALSE;
		}
		check_act_error_time = global.absolute_time;
	}
}

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	Uint8 i;
	if(init) {
		//Init des actionneurs
	} else {
		bool_e isReady = FALSE, responseReceived = FALSE;
		#ifdef I_AM_ROBOT_BIG
				//responseReceived = RX24_async_is_ready(FISH_UNSTICK_ARM_RX24_ID, &isReady); //TODO 2017 Remplacer par les appels adéquats
		#else
				//responseReceived = AX12_async_is_ready(LEFT_ARM_AX12_ID, &isReady);		//TODO 2017 Remplacer par les appels adéquats
		#endif

		if((responseReceived && isReady) || global.flags.power) { // Si il y a le +12/24V (on laisse le AX12_is_ready si on utilise le FDP hors robot sous 12V mais l'initialisation peut ne pas marcher si l'ax12 testé n'est pas présent)
			debug_printf("Init pos\n");
			for(i = 0; i < NB_ACTIONNEURS; i++) {
				if(actionneurs[i].onInitPos != NULL)
					actionneurs[i].onInitPos();
			}
			QUEUE_behead(queueId);
		} else if(global.flags.match_started == TRUE) {
			//Le match a démarré, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}

void ACTMGR_config_AX12(Uint8 id_servo, CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.config){
		case SPEED_CONFIG : // Configuration de la vitesse
			if(AX12_is_wheel_mode_enabled(id_servo)){
				AX12_set_speed_percentage(id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_wheel);
				debug_printf("Configuration de la vitesse (wheel mode) de l'AX12 %d avec une valeur de %d\n", id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_wheel);
			}else{
				AX12_set_move_to_position_speed(id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_position);
				debug_printf("Configuration de la vitesse (position mode) de l'AX12 %d avec une valeur de %d\n", id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_position);
			}

			break;

		case TORQUE_CONFIG : // Configuration du couple
			AX12_set_torque_limit(id_servo, msg->data.act_msg.act_data.act_config.data_config.torque);
			debug_printf("Configuration du couple de l'AX12 %d avec une valeur de %d\n", id_servo, msg->data.act_msg.act_data.act_config.data_config.torque);
			break;

		default :
			warn_printf("invalid CAN msg data[2]=%u (configuration impossible)!\n", msg->data.act_msg.act_data.act_config.config);
	}
}

void ACTMGR_config_RX24(Uint8 id_servo, CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.config){
		case SPEED_CONFIG : // Configuration de la vitesse
			if(RX24_is_wheel_mode_enabled(id_servo)){
				RX24_set_speed_percentage(id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_wheel);
				debug_printf("Configuration de la vitesse (wheel mode) du RX24 %d avec une valeur de %d\n", id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_wheel);
			}else{
				RX24_set_move_to_position_speed(id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_position);
				debug_printf("Configuration de la vitesse (position mode) du RX24 %d avec une valeur de %d\n", id_servo, msg->data.act_msg.act_data.act_config.data_config.speed_position);
			}

			break;

		case TORQUE_CONFIG : // Configuration du couple
			RX24_set_torque_limit(id_servo, msg->data.act_msg.act_data.act_config.data_config.torque);
			debug_printf("Configuration du couple du RX24 %d avec une valeur de %d\n", id_servo, msg->data.act_msg.act_data.act_config.data_config.torque);
			break;

		default :
			warn_printf("invalid CAN msg data[2]=%u (configuration impossible)!\n", msg->data.act_msg.act_data.act_config.config);
	}
}


