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
	#include "Black/Fishs/fish_magnetic_arm.h"
	#include "Black/Fishs/fish_magnetic_arm_config.h"
	#include "Black/Fishs/fish_unstick_arm.h"
	#include "Black/Fishs/fish_unstick_arm_config.h"
	#include "Black/Sand_circle/black_sand_circle.h"
	#include "Black/Sand_circle/black_sand_circle_config.h"
	#include "Black/Bottom_dune/bottom_dune.h"
	#include "Black/Bottom_dune/bottom_dune_config.h"
	#include "Black/Middle_dune/middle_dune.h"
	#include "Black/Middle_dune/middle_dune_config.h"
	#include "Black/Cone_dune/cone_dune.h"
	#include "Black/Cone_dune/cone_dune_config.h"
	#include "Black/Sand_locker/sand_locker_left.h"
	#include "Black/Sand_locker/sand_locker_left_config.h"
	#include "Black/Sand_locker/sand_locker_right.h"
	#include "Black/Sand_locker/sand_locker_right_config.h"
	#include "Black/Shift_cylinder/shift_cylinder.h"
	#include "Black/Shift_cylinder/shift_cylinder_config.h"
	#include "Black/Pendulum/pendulum.h"
	#include "Black/Pendulum/pendulum_config.h"
	#include "Black/Shovel_dune/shovel_dune.h"
	#include "Black/Shovel_dune/shovel_dune_config.h"
	#include "Black/Brush_dune/brush_dune.h"
	#include "Black/Brush_dune/brush_dune_config.h"
	#include "QS/QS_mosfet.h"
#else
	#include "Pearl/Sand/left_arm.h"
	#include "Pearl/Sand/left_arm_config.h"
	#include "Pearl/Sand/right_arm.h"
	#include "Pearl/Sand/right_arm_config.h"
	#include "Pearl/Sand/pearl_sand_circle.h"
	#include "Pearl/Sand/pearl_sand_circle_config.h"
	#include "Pearl/Parasol/parasol.h"
	#include "Pearl/Parasol/parasol_config.h"
	#include "Pearl/Fishs_pearl/fish_pearl.h"
	#include "Pearl/Fishs_pearl/fish_pearl_config.h"
	#include "Pearl/Pompes/pompe_front_left.h"
	#include "Pearl/Pompes/pompe_front_right.h"
	#include "QS/QS_mosfet.h"
#endif


static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

#define ACT_DECLARE(prefix) {&prefix##_init, &prefix##_init_pos, &prefix##_stop, &prefix##_reset_config, &prefix##_CAN_process_msg}

static ACTQ_functions_t actionneurs[] = {
		//QS_mosfets
		ACT_DECLARE(MOSFET),
	#ifdef I_AM_ROBOT_BIG  //Big Robot
		ACT_DECLARE(FISH_MAGNETIC_ARM),
		ACT_DECLARE(FISH_UNSTICK_ARM),
		ACT_DECLARE(BLACK_SAND_CIRCLE),
		ACT_DECLARE(BOTTOM_DUNE),
		ACT_DECLARE(MIDDLE_DUNE),
		ACT_DECLARE(CONE_DUNE),
		ACT_DECLARE(SAND_LOCKER_LEFT),
		ACT_DECLARE(SAND_LOCKER_RIGHT),
		ACT_DECLARE(SHIFT_CYLINDER),
		ACT_DECLARE(PENDULUM)
		//ACT_DECLARE(SHOVEL_DUNE),
		//ACT_DECLARE(BRUSH_DUNE)
	#else  //Small Robot
		ACT_DECLARE(LEFT_ARM),
		ACT_DECLARE(RIGHT_ARM),
		ACT_DECLARE(PEARL_SAND_CIRCLE),
		ACT_DECLARE(PARASOL),
		ACT_DECLARE(POMPE_FRONT_LEFT),
		ACT_DECLARE(POMPE_FRONT_RIGHT),
		ACT_DECLARE(FISH_PEARL)
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


static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	Uint8 i;
	if(init) {
		//Init des actionneurs
	} else {
		bool_e isReady = FALSE, responseReceived = FALSE;
		#ifdef I_AM_ROBOT_BIG
				responseReceived = RX24_async_is_ready(FISH_UNSTICK_ARM_RX24_ID, &isReady);
		#else
				//responseReceived = AX12_async_is_ready(LEFT_ARM_AX12_ID, &isReady);
		#endif

		if((responseReceived && isReady) || global.flags.alim) { // Si il y a le +12/24V (on laisse le AX12_is_ready si on utilise le FDP hors robot sous 12V mais l'initialisation peut ne pas marcher si l'ax12 testé n'est pas présent)
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
