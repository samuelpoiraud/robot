/*
 *  Club Robot ESEO 2016
 *
 *  Fichier : QS_mosfet.c
 *  Package : Qualit� Soft
 *  Description : Regroupement de toutes les fonctions utiles � la gestion d'une carte MOSFET
 *  Auteur : Valentin
 *
 */

#include "QS_all.h"

#ifdef USE_MOSFETS

/* Pour utiliser ce module, vous devez d�finir dans le fichier config_use la macro USE_MOSFETS et la
 * macro NB_MOSFETS suivie du nombre de mosfets � piloter (max = 8).
 *		exemple : #define USE_MOSFETS
 *				  #define NB_MOSFETS 3
 * Dans QS_CAN_msgList.h,vous devez d�finir les SID de chacun des mosfets (ex ACT_MOSFET_0) pour un mosfet
 * situ� en actionneur ou  (ex STRAT_MOSFET_0) pour un mosfet situ� en strat�gie. Vous pouvez �galement red�finir
 * ces SID pour leur donner un nom.
 * Dans queue.h de la strat�gie, vous devez d�finir ACT_QUEUE_Mosfet_act_0 pour un mosfet en actionneur
 * ou ACT_QUEUE_Mosfet_strat_0 en strat�gie.
 * Vous devez aussi d�finir si cela n'est pas fait des macros pour chaque mosfet dans config_pin (ex MOSFET_0_PIN)
 */

#include "QS_mosfet.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#ifdef I_AM_CARTE_STRAT
	#include "../act_functions.h"
#endif

#define LOG_PREFIX "mosfet : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MOSFET
#include "../QS/QS_outputlog.h"

#define MOSFET_DELAY                 10  //Delay en milliseconde entre l'allumage de chaque pompe
#define MOSFET_SELFTEST_TIME         500 //Delay en milliseconde pour l'allumage de Selftest pour l'allumage de chaque pompe


#if NB_MOSFETS>8
	#warning "QS_mosfet ne peut pas g�rer plus de 8 mosfets"
#endif

#if defined(MOSFET_0_PIN) //&& NB_MOSFETS>=1
static void MOSFET_0_do_order(Uint8 command);
#endif
#if defined(MOSFET_1_PIN) //&& NB_MOSFETS>=2
static void MOSFET_1_do_order(Uint8 command);
#endif
#if defined(MOSFET_2_PIN) //&& NB_MOSFETS>=3
static void MOSFET_2_do_order(Uint8 command);
#endif
#if defined(MOSFET_3_PIN) //&& NB_MOSFETS>=4
static void MOSFET_3_do_order(Uint8 command);
#endif
#if defined(MOSFET_4_PIN) //&& NB_MOSFETS>=5
static void MOSFET_4_do_order(Uint8 command);
#endif
#if defined(MOSFET_5_PIN) //&& NB_MOSFETS>=6
static void MOSFET_5_do_order(Uint8 command);
#endif
#if defined(MOSFET_6_PIN) //&& NB_MOSFETS>=7
static void MOSFET_6_do_order(Uint8 command);
#endif
#if defined(MOSFET_7_PIN) //&& NB_MOSFETS>=8
static void MOSFET_7_do_order(Uint8 command);
#endif

//Initialisation du module MOSFET
void MOSFET_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
}

void MOSFET_reset_config(){}

//Fonction pour stopper tous les mosfets
void MOSFET_stop() {
#if defined(MOSFET_0_PIN) //&& NB_MOSFETS>=1
	GPIO_ResetBits(MOSFET_0_PIN);
#endif
#if defined(MOSFET_1_PIN) //&& NB_MOSFETS>=2
	GPIO_ResetBits(MOSFET_1_PIN);
#endif
#if defined(MOSFET_2_PIN) //&& NB_MOSFETS>=3
	GPIO_ResetBits(MOSFET_2_PIN);
#endif
#if defined(MOSFET_3_PIN) //&& NB_MOSFETS>=4
	GPIO_ResetBits(MOSFET_3_PIN);
#endif
#if defined(MOSFET_4_PIN) //&& NB_MOSFETS>=5
	GPIO_ResetBits(MOSFET_4_PIN);
#endif
#if defined(MOSFET_5_PIN) //&& NB_MOSFETS>=6
	GPIO_ResetBits(MOSFET_5_PIN);
#endif
#if defined( MOSFET_6_PIN) //&& NB_MOSFETS>=7
	GPIO_ResetBits(MOSFET_6_PIN);
#endif
#if defined(MOSFET_7_PIN) //&& NB_MOSFETS>=8
	GPIO_ResetBits(MOSFET_7_PIN);
#endif
}

void MOSFET_init_pos(){
#if defined(MOSFET_0_PIN) //&& NB_MOSFETS>=1
	GPIO_ResetBits(MOSFET_0_PIN);
#endif
#if defined(MOSFET_1_PIN) //&& NB_MOSFETS>=2
	GPIO_ResetBits(MOSFET_1_PIN);
#endif
#if defined(MOSFET_2_PIN) //&& NB_MOSFETS>=3
	GPIO_ResetBits(MOSFET_2_PIN);
#endif
#if defined(MOSFET_3_PIN) //&& NB_MOSFETS>=4
	GPIO_ResetBits(MOSFET_3_PIN);
#endif
#if defined(MOSFET_4_PIN) //&& NB_MOSFETS>=5
	GPIO_ResetBits(MOSFET_4_PIN);
#endif
#if defined(MOSFET_5_PIN) //&& NB_MOSFETS>=6
	GPIO_ResetBits(MOSFET_5_PIN);
#endif
#if defined(MOSFET_6_PIN) //&& NB_MOSFETS>=7
	GPIO_ResetBits(MOSFET_6_PIN);
#endif
#if defined(MOSFET_7_PIN) //&& NB_MOSFETS>=8
	GPIO_ResetBits(MOSFET_7_PIN);
#endif
}

#ifdef I_AM_CARTE_ACT
//Fonction de gestions des messages CAN pour les mosfets situ�s sur une carte Mosfet command�e par l'actionneur
bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg) {
	CAN_msg_t msg_result;
	msg_result.sid = ACT_RESULT;
	msg_result.size = SIZE_ACT_RESULT;
	msg_result.data.act_result.sid = msg->sid;
	msg_result.data.act_result.cmd = msg->data.act_msg.order;
	msg_result.data.act_result.result = ACT_RESULT_DONE;
	msg_result.data.act_result.error_code = ACT_RESULT_ERROR_OK;

	if(msg->sid == ACT_MOSFET_0 && NB_MOSFETS>=1) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_0_do_order\n");
				MOSFET_0_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#if NB_MOSFETS>=2
	}else if(msg->sid == ACT_MOSFET_1) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_1_do_order\n");
				MOSFET_1_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#if NB_MOSFETS>=3
	}else if(msg->sid == ACT_MOSFET_2) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_2_do_order\n");
				MOSFET_2_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#if NB_MOSFETS>=4
	}else if(msg->sid == ACT_MOSFET_3) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_3_do_order\n");
				MOSFET_3_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#if NB_MOSFETS>=5
	}else if(msg->sid == ACT_MOSFET_4) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_4_do_order\n");
				MOSFET_4_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#if NB_MOSFETS>=6
	}else if(msg->sid == ACT_MOSFET_5) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_5_do_order\n");
				MOSFET_5_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#if NB_MOSFETS>=7
	}else if(msg->sid == ACT_MOSFET_6) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_6_do_order\n");
				MOSFET_6_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#if NB_MOSFETS>=8
	}else if(msg->sid == ACT_MOSFET_7) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("ACT_MOSFET_7_do_order\n");
				MOSFET_7_do_order(msg->data.act_msg.order);
				CAN_send(&msg_result);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
	}else if(msg->sid == ACT_DO_SELFTEST) {

	}

	return FALSE;
}

#elif defined(I_AM_CARTE_STRAT)
//Fonction de gestions des messages CAN pour les mosfets situ�s sur une carte Mosfet command�e par la strat�gie
bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == STRAT_MOSFET_0) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_0_do_order\n");
				MOSFET_0_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_0, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#ifdef MOSFET_1_PIN
	}else if(msg->sid == STRAT_MOSFET_1) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_1_do_order\n");
				MOSFET_1_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_1, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#ifdef MOSFET_2_PIN
	}else if(msg->sid == STRAT_MOSFET_2) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_2_do_order\n");
				MOSFET_2_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_2, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#ifdef MOSFET_3_PIN
	}else if(msg->sid == STRAT_MOSFET_3) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_3_do_order\n");
				MOSFET_3_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_3, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#ifdef MOSFET_4_PIN
	}else if(msg->sid == STRAT_MOSFET_4) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_4_do_order\n");
				MOSFET_4_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_4, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#ifdef MOSFET_5_PIN
	}else if(msg->sid == STRAT_MOSFET_5) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_5_do_order\n");
				MOSFET_5_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_5, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#ifdef MOSFET_6_PIN
	}else if(msg->sid == STRAT_MOSFET_6) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_6_do_order\n");
				MOSFET_6_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_6, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
#ifdef MOSFET_7_PIN
	}else if(msg->sid == STRAT_MOSFET_7) {
		switch(msg->data.act_msg.order) {
			case ACT_MOSFET_NORMAL:
			case ACT_MOSFET_STOP:
				debug_printf("STRAT_MOSFET_7_do_order\n");
				MOSFET_7_do_order(msg->data.act_msg.order);
				ACT_set_result(ACT_QUEUE_Mosfet_strat_7, ACT_RESULT_Ok);
				break;
			default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
#endif
	}else if(msg->sid == ACT_DO_SELFTEST) {

	}
	return FALSE;
}
#endif




#if defined(MOSFET_0_PIN)
static void MOSFET_0_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_0_PIN set\n");
		GPIO_SetBits(MOSFET_0_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_0_PIN reset\n");
		GPIO_ResetBits(MOSFET_0_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_0_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_0_PIN);
		return;
	}
}
#endif

#if defined(MOSFET_1_PIN)
static void MOSFET_1_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_1_PIN set\n");
		GPIO_SetBits(MOSFET_1_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_1_PIN reset\n");
		GPIO_ResetBits(MOSFET_1_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_1_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_1_PIN);
		return;
	}
}
#endif

#if defined( MOSFET_2_PIN)
static void MOSFET_2_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_2_PIN set\n");
		GPIO_SetBits(MOSFET_2_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_2_PIN reset\n");
		GPIO_ResetBits(MOSFET_2_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_2_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_2_PIN);
		return;
	}
}
#endif

#if defined(MOSFET_3_PIN)
static void MOSFET_3_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_3_PIN set\n");
		GPIO_SetBits(MOSFET_3_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_3_PIN reset\n");
		GPIO_ResetBits(MOSFET_3_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_3_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_3_PIN);
		return;
	}
}
#endif

#if defined(MOSFET_4_PIN)
static void MOSFET_4_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_4_PIN set\n");
		GPIO_SetBits(MOSFET_4_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_4_PIN reset\n");
		GPIO_ResetBits(MOSFET_4_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_4_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_4_PIN);
		return;
	}
}
#endif

#if defined(MOSFET_5_PIN)
static void MOSFET_5_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_5_PIN set\n");
		GPIO_SetBits(MOSFET_5_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_5_PIN reset\n");
		GPIO_ResetBits(MOSFET_5_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_5_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_5_PIN);
		return;
	}
}
#endif

#if defined(MOSFET_6_PIN)
static void MOSFET_6_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_6_PIN set\n");
		GPIO_SetBits(MOSFET_6_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_6_PIN reset\n");
		GPIO_ResetBits(MOSFET_6_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_0_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_6_PIN);
		return;
	}
}
#endif

#if defined(MOSFET_7_PIN)
static void MOSFET_7_do_order(Uint8 command){
	if(command == ACT_MOSFET_NORMAL){
		debug_printf("MOSFET_7_PIN set\n");
		GPIO_SetBits(MOSFET_7_PIN);
	}else if(command == ACT_MOSFET_STOP){
		debug_printf("MOSFET_7_PIN reset\n");
		GPIO_ResetBits(MOSFET_7_PIN);
	}else{
		debug_printf("commande envoy�e � MOSFET_7_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(MOSFET_7_PIN);
		return;
	}
}
#endif


#ifdef I_AM_CARTE_STRAT
//Machine � �tat pour activer en d�cal� les mosfets command�s par l'actionneur
void MOSFET_state_machine(CAN_msg_t* msg){
	static Uint11 state = NB_QUEUE;
	static Uint11 last_state = NB_QUEUE + 1;
	static bool_e entrance;
	static time32_t state_time;
	static CAN_msg_t current_msg, msg_act;

	//initialisation en cas de l'envoi d'une nouvelle commande
	if(msg != NULL){
		assert((msg->sid & 0x300) == ACT_FILTER);
		state = ACT_search_link_SID_Queue(msg->sid)-1;
		current_msg = *msg;
		msg_act = *msg;
	}

	//Affectation des variables courantes
	entrance = (last_state != state);
	last_state = state;
	if(entrance){
		state_time = global.absolute_time;
	}

	switch(state){
		case ACT_QUEUE_Mosfet_act_all:
		case ACT_QUEUE_Mosfet_act_0:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_0;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x80) >> 7;
					debug_printf("Modification mosfet 0 done to %d\n", (current_msg.data.act_msg.order & 0x80) >> 7);
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid  == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state = ACT_QUEUE_Mosfet_act_1;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_1:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_1;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x10) >> 4;
					debug_printf("Modification mosfet 1 done to %d\n", (current_msg.data.act_msg.order & 0x10) >> 4);
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state = ACT_QUEUE_Mosfet_act_2;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_2:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_2;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x02) >> 1;
					debug_printf("Modification mosfet 2 done to %d\n", (current_msg.data.act_msg.order & 0x02) >> 1);
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state = ACT_QUEUE_Mosfet_act_3;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_3:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_3;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x01);
					debug_printf("Modification mosfet 3 done to %d\n", (current_msg.data.act_msg.order & 0x01));
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state = ACT_QUEUE_Mosfet_act_4;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_4:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_4;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x04) >> 2;
					debug_printf("Modification mosfet 4 done to %d\n", (current_msg.data.act_msg.order & 0x04) >> 2);
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state =  ACT_QUEUE_Mosfet_act_5;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_5:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_5;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x08) >> 3;
					debug_printf("Modification mosfet 5 done to %d\n", (current_msg.data.act_msg.order & 0x08) >> 3);
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state = ACT_QUEUE_Mosfet_act_6;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_6:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_6;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x20) >> 5;
					debug_printf("Modification mosfet 6 done to %d\n", (current_msg.data.act_msg.order & 0x20) >> 5);
				}
				CAN_send(&msg_act);
			}else if(current_msg.sid == ACT_MOSFETS_ALL && global.absolute_time >= state_time + MOSFET_DELAY){
				state = ACT_QUEUE_Mosfet_act_7;
			}else if(current_msg.sid != ACT_MOSFETS_ALL){
				state = NB_QUEUE; //finish
			}
			break;

		case ACT_QUEUE_Mosfet_act_7:
			if(entrance){
				if(current_msg.sid == ACT_MOSFETS_ALL){
					msg_act.sid = ACT_MOSFET_7;
					msg_act.data.act_msg.order = (current_msg.data.act_msg.order & 0x40) >> 6;
					debug_printf("Modification mosfet 7 done to %d\n", (current_msg.data.act_msg.order & 0x40) >> 6);
				}
				CAN_send(&msg_act);
			}
			state = NB_QUEUE;
			break;

		case NB_QUEUE:
			if(entrance){
				debug_printf("Mosfet_state_machine finish\n");
				ACT_set_result(ACT_QUEUE_Mosfet_act_all, ACT_RESULT_Ok);
			}
			break;

		default:
			debug_printf("Default state in MOSFET_state_machine: bad sid\n");
	}
}

// Fonction de Selftest des mosfets command�s par l'actionneur
// @param : nb_mosfets le nombre de mosfets � tester
bool_e MOSFET_selftest_act(Uint8 nb_mosfets){
	static Uint11 state = ACT_QUEUE_Mosfet_act_0;
	static Uint11 last_state = NB_QUEUE + 1;
	static bool_e entrance;
	static time32_t state_time;
	CAN_msg_t msg;

	//Affectation des variables courantes
	entrance = (last_state != state);
	last_state = state;
	if(entrance){
		state_time = global.absolute_time;
	}

	switch(state){
		case ACT_QUEUE_Mosfet_act_0:
			if(entrance){
				msg.sid = ACT_MOSFET_0;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_0;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=2)
					state = ACT_QUEUE_Mosfet_act_1;
				else
					state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_1:
			if(entrance){
				msg.sid = ACT_MOSFET_1;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_1;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=3)
					state = ACT_QUEUE_Mosfet_act_2;
				else
					state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_2:
			if(entrance){
				msg.sid = ACT_MOSFET_2;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_2;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=4)
					state = ACT_QUEUE_Mosfet_act_3;
				else
					state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_3:
			if(entrance){
				msg.sid = ACT_MOSFET_3;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_3;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=5)
					state = ACT_QUEUE_Mosfet_act_4;
				else
					state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_4:
			if(entrance){
				msg.sid = ACT_MOSFET_4;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_4;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=6)
					state = ACT_QUEUE_Mosfet_act_5;
			   else
					state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_5:
			if(entrance){
				msg.sid = ACT_MOSFET_5;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_5;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=7)
					state = ACT_QUEUE_Mosfet_act_6;
				else
					state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_6:
			if(entrance){
				msg.sid = ACT_MOSFET_6;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_6;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				if(nb_mosfets>=8)
					  state = ACT_QUEUE_Mosfet_act_7;
				else
					  state = NB_QUEUE;
			}
			break;

		case ACT_QUEUE_Mosfet_act_7:
			if(entrance){
				msg.sid = ACT_MOSFET_7;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_NORMAL;
				CAN_send(&msg);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				msg.sid = ACT_MOSFET_7;
				msg.size = SIZE_ACT_MSG;
				msg.data.act_msg.order = ACT_MOSFET_STOP;
				CAN_send(&msg);
				state = NB_QUEUE;
			}
			break;

		case NB_QUEUE:
			state = ACT_QUEUE_Mosfet_act_0;
			return TRUE;
			break;

		default:
			debug_printf("Default state in MOSFET_selftest_act: bad sid\n");
	}
	return FALSE;
}

//Fonction de Selftest des mosfets command�s par la strat�gie
bool_e MOSFET_selftest_strat(){
	static Uint11 state = ACT_QUEUE_Mosfet_strat_0;
	static Uint11 last_state = NB_QUEUE + 1;
	static bool_e entrance;
	static time32_t state_time;

	//Affectation des variables courantes
	entrance = (last_state != state);
	last_state = state;
	if(entrance){
		state_time = global.absolute_time;
	}

	switch(state){
		case ACT_QUEUE_Mosfet_strat_0:
			if(entrance){
				MOSFET_0_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_0_do_order(ACT_MOSFET_STOP);
			   #if NB_MOSFETS>=2
					state = ACT_QUEUE_Mosfet_strat_1;
				#else
					state = NB_QUEUE;
				#endif
			}
			break;
#if NB_MOSFETS>=2
		case ACT_QUEUE_Mosfet_strat_1:
			if(entrance){
				MOSFET_1_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_1_do_order(ACT_MOSFET_STOP);
				#if NB_MOSFETS>=3
					state = ACT_QUEUE_Mosfet_strat_2;
				#else
					state = NB_QUEUE;
				#endif
			}
			break;
#endif
#if NB_MOSFETS>=3
		case ACT_QUEUE_Mosfet_strat_2:
			if(entrance){
				MOSFET_2_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_2_do_order(ACT_MOSFET_STOP);
				#if NB_MOSFETS>=4
					state = ACT_QUEUE_Mosfet_strat_3;
				#else
					state = NB_QUEUE;
				#endif
			}
			break;
#endif
#if NB_MOSFETS>=4
		case ACT_QUEUE_Mosfet_strat_3:
			if(entrance){
				MOSFET_3_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_3_do_order(ACT_MOSFET_STOP);
				#if NB_MOSFETS>=5
					state = ACT_QUEUE_Mosfet_strat_4;
				#else
					state = NB_QUEUE;
				#endif
			}
			break;
#endif
#if NB_MOSFETS>=5
		case ACT_QUEUE_Mosfet_strat_4:
			if(entrance){
				MOSFET_4_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_4_do_order(ACT_MOSFET_STOP);
				#if NB_MOSFETS>=6
					state = ACT_QUEUE_Mosfet_strat_5;
				#else
					state = NB_QUEUE;
				#endif
			}
			break;
#endif
#if NB_MOSFETS>=6
		case ACT_QUEUE_Mosfet_strat_5:
			if(entrance){
				MOSFET_5_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_5_do_order(ACT_MOSFET_STOP);
				#if NB_MOSFETS>=7
					state = ACT_QUEUE_Mosfet_strat_6;
				 #else
					state = NB_QUEUE;
				#endif
			}
			break;
#endif
#if NB_MOSFETS>=7
		case ACT_QUEUE_Mosfet_strat_6:
			if(entrance){
				MOSFET_6_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_6_do_order(ACT_MOSFET_STOP);
				#if NB_MOSFETS>=8
					 state = ACT_QUEUE_Mosfet_strat_7;
				#else
					 state = NB_QUEUE;
				#endif
			}
			break;
#endif
#if NB_MOSFETS>=8
		case ACT_QUEUE_Mosfet_strat_7:
			if(entrance){
				MOSFET_7_do_order(ACT_MOSFET_NORMAL);
			}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
				MOSFET_7_do_order(ACT_MOSFET_STOP);
				state = NB_QUEUE;
			}
			break;
#endif
		case NB_QUEUE:
			state = ACT_QUEUE_Mosfet_strat_0;
			return TRUE;
			break;

		default:
			debug_printf("Default state in MOSFET_selftest_strat: bad sid\n");
	}
	return FALSE;
}
#endif

#endif  //USE_MOSFET
