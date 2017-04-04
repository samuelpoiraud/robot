/*
 *  Club Robot ESEO 2016
 *
 *  Fichier : QS_mosfet.c
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions utiles à la gestion d'une carte MOSFET
 *  Auteur : Valentin
 *
 */

#include "QS_all.h"
#include "QS_mosfet.h"

#ifdef USE_MOSFETS_MODULE

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../QS/QS_types.h"
#ifdef I_AM_CARTE_STRAT
	#include "../actuator/act_functions.h"
#endif /* I_AM_CARTE_STRAT */

#define LOG_PREFIX "mosfet : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MOSFET
#include "../QS/QS_outputlog.h"

#define MOSFET_DELAY                 10  //Delay en milliseconde entre l'allumage de chaque pompe
#define MOSFET_SELFTEST_TIME         500 //Delay en milliseconde pour l'allumage de Selftest pour l'allumage de chaque pompe


static void MOSFET_do_order(ACT_sid_e sid, Uint8 command);
static bool_e MOSFET_activate_this_mosfet(bool_e entrance, ACT_sid_e sid, Uint8 command, code_id_e code);

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
#ifdef USE_MOSFET_1
	GPIO_ResetBits(MOSFET_1_PIN);
#endif
#ifdef USE_MOSFET_2
	GPIO_ResetBits(MOSFET_2_PIN);
#endif
#ifdef USE_MOSFET_3
	GPIO_ResetBits(MOSFET_3_PIN);
#endif
#ifdef USE_MOSFET_4
	GPIO_ResetBits(MOSFET_4_PIN);
#endif
#ifdef USE_MOSFET_5
	GPIO_ResetBits(MOSFET_5_PIN);
#endif
#ifdef USE_MOSFET_6
	GPIO_ResetBits(MOSFET_6_PIN);
#endif
#ifdef USE_MOSFET_7
	GPIO_ResetBits(MOSFET_7_PIN);
#endif
#ifdef USE_MOSFET_8
	GPIO_ResetBits(MOSFET_8_PIN);
#endif
}

void MOSFET_init_pos(){
	MOSFET_stop();  //Arrêt de tous les mosfets au début du match
}






static void MOSFET_do_order(ACT_sid_e sid, Uint8 command){
	switch(sid){
#ifdef USE_MOSFET_1
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_1:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_1:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_1_PIN, TRUE);
				debug_printf("MOSFET_1 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_1_PIN, FALSE);
				debug_printf("MOSFET_1 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_1_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_2
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_2:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_2:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_2_PIN, TRUE);
				debug_printf("MOSFET_2 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_2_PIN, FALSE);
				debug_printf("MOSFET_2 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_2_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_3
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_3:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_3:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_3_PIN, TRUE);
				debug_printf("MOSFET_3 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_3_PIN, FALSE);
				debug_printf("MOSFET_3 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_3_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_4
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_4:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_4:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_4_PIN, TRUE);
				debug_printf("MOSFET_4 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_4_PIN, FALSE);
				debug_printf("MOSFET_4 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_4_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_5
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_5:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_5:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_5_PIN, TRUE);
				debug_printf("MOSFET_5 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				debug_printf("MOSFET_5 désactivé\n");
				GPIO_WriteBit(MOSFET_5_PIN, FALSE);
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_5_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_6
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_6:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_6:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_6_PIN, TRUE);
				debug_printf("MOSFET_6 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_6_PIN, FALSE);
				debug_printf("MOSFET_6 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_6_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_7
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_7:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_7:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_7_PIN, TRUE);
				debug_printf("MOSFET_7 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_7_PIN, FALSE);
				debug_printf("MOSFET_7 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_7_PIN);
			}
			break;
#endif
#ifdef USE_MOSFET_8
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_8:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_8:
#endif
			if(command == ACT_MOSFET_NORMAL){
				GPIO_WriteBit(MOSFET_8_PIN, TRUE);
				debug_printf("MOSFET_8 activé\n");
			}else if(command == ACT_MOSFET_STOP){
				GPIO_WriteBit(MOSFET_8_PIN, FALSE);
				debug_printf("MOSFET_8 désactivé\n");
			}else{
				debug_printf("commande envoyée à MOSFET_do_order inconnue -> %d	0x%x\n", command, command);
				GPIO_ResetBits(MOSFET_8_PIN);
			}
			break;
#endif
		default:
			debug_printf("Le mosfet demandé n'existe pas. Vérifiez les sid.\n");
	}
}

//Machine à état pour activer en décalé les mosfets
bool_e MOSFET_do_order_multi(CAN_msg_t* new_msg){
	typedef enum{
		SM_INIT,
#ifdef USE_MOSFET_1
		SM_MOSFET_1,
#endif
#ifdef USE_MOSFET_2
		SM_MOSFET_2,
#endif
#ifdef USE_MOSFET_3
		SM_MOSFET_3,
#endif
#ifdef USE_MOSFET_4
		SM_MOSFET_4,
#endif
#ifdef USE_MOSFET_5
		SM_MOSFET_5,
#endif
#ifdef USE_MOSFET_6
		SM_MOSFET_6,
#endif
#ifdef USE_MOSFET_7
		SM_MOSFET_7,
#endif
#ifdef USE_MOSFET_8
		SM_MOSFET_8,
#endif
		SM_DONE
	}SM_mosfet_e;
	static SM_mosfet_e state = SM_INIT;
	static SM_mosfet_e last_state = SM_INIT;
	static bool_e entrance;
	static CAN_msg_t *msg;
	Uint8 command;

	UNUSED_VAR(command);

	if(new_msg != NULL){
		msg = new_msg;
		assert(msg->sid == ACT_MOSFET_MULTI || msg->sid == STRAT_MOSFET_MULTI);
		state = SM_INIT;
	}

	//Affectation des variables courantes
	entrance = (last_state != state);
	last_state = state;

	switch(state){
		case SM_INIT:
			if(new_msg != NULL)
				state++;
			break;
#ifdef USE_MOSFET_1
		case SM_MOSFET_1:
			if(msg->data.act_msg.order & 0x01){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_1, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_1, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_2
		case SM_MOSFET_2:
			if(msg->data.act_msg.order & 0x02){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_2, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_2, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_3
		case SM_MOSFET_3:
			if(msg->data.act_msg.order & 0x04){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_3, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_3, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_4
		case SM_MOSFET_4:
			if(msg->data.act_msg.order & 0x08){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_4, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_4, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_5
		case SM_MOSFET_5:
			if(msg->data.act_msg.order & 0x10){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_5, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_5, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_6
		case SM_MOSFET_6:
			if(msg->data.act_msg.order & 0x20){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_6, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_6, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_7
		case SM_MOSFET_7:
			if(msg->data.act_msg.order & 0x40){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_7, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_7, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
#ifdef USE_MOSFET_8
		case SM_MOSFET_8:
			if(msg->data.act_msg.order & 0x80){
				command = ACT_MOSFET_NORMAL;
			}else{
				command = ACT_MOSFET_STOP;
			}
			#if defined(I_AM_CARTE_ACT)
				 if(MOSFET_activate_this_mosfet(entrance, ACT_MOSFET_8, command, CODE_ACT))
					 state++;
			#elif defined(I_AM_CARTE_STRAT)
				 if(MOSFET_activate_this_mosfet(entrance, STRAT_MOSFET_8, command, CODE_STRAT))
					 state++;
			#else
				debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
				state = SM_DONE;
			#endif
			break;
#endif
		case SM_DONE:
			if(entrance){
#if defined(I_AM_CARTE_ACT)
			CAN_msg_t msg_result;
			msg_result.sid = ACT_RESULT;
			msg_result.size = SIZE_ACT_RESULT;
			msg_result.data.act_result.sid = msg->sid;
			msg_result.data.act_result.cmd = msg->data.act_msg.order;
			msg_result.data.act_result.result = ACT_RESULT_DONE;
			msg_result.data.act_result.error_code = ACT_RESULT_ERROR_OK;
			CAN_send(&msg_result);
#elif defined(I_AM_CARTE_STRAT)
			ACT_set_result(ACT_QUEUE_Mosfet_strat_multi, ACT_RESULT_Ok);
#endif
			state = SM_INIT;
			return TRUE;
			}
			break;

		default:
			debug_printf("Default state in MOSFET_do_order_multi: Le mosfet demandé n'est pas configuré (voir config_use)\n");
	}
	return FALSE;
}


static bool_e MOSFET_activate_this_mosfet(bool_e entrance, ACT_sid_e sid, Uint8 command, code_id_e code){
	static time32_t state_time;
	bool_e finish = FALSE;
	if(entrance){
			MOSFET_do_order(sid, command);
	}else if(global.absolute_time >= state_time + MOSFET_DELAY){
		finish = TRUE;
	}
	return finish;
}


bool_e MOSFET_selftest_this_mosfet(bool_e entrance, ACT_sid_e sid, code_id_e code){
	static time32_t state_time;
	CAN_msg_t msg;
	bool_e finish = FALSE;
	if(entrance){
		state_time = global.absolute_time;
		if(code == CODE_ACT){
			msg.sid = sid;
			msg.size = SIZE_ACT_MSG;
			msg.data.act_msg.order = ACT_MOSFET_NORMAL;
			CAN_send(&msg);
		}else if(code == CODE_STRAT){
			MOSFET_do_order(sid, ACT_MOSFET_NORMAL);
		}else{
			debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
		}
	}else if(global.absolute_time >= state_time + MOSFET_SELFTEST_TIME){
		if(code == CODE_ACT){
			msg.sid = sid;
			msg.size = SIZE_ACT_MSG;
			msg.data.act_msg.order = ACT_MOSFET_STOP;
			CAN_send(&msg);
		}else if(code == CODE_STRAT){
			MOSFET_do_order(sid, ACT_MOSFET_STOP);
		}else{
			debug_printf("Les mosfets ne sont pilotables que en STRAT et en ACT.\n");
		}
		finish = TRUE;
	}
	return finish;
}
#endif  //USE_MOSFETS_MODULE

///******************************* Fonctions définies sans USE_MOSFETS_MODULE ********************************/
#if defined(I_AM_CARTE_STRAT)
bool_e MOSFET_isStratMosfetSid(queue_id_e act_id){
#if defined(I_AM_CARTE_STRAT) && defined(USE_MOSFETS_MODULE)
	if(
	#ifdef USE_MOSFET_1
		   act_id == (queue_id_e)ACT_QUEUE_Mosfet_strat_1
	#endif
	#ifdef USE_MOSFET_2
		|| act_id == ACT_QUEUE_Mosfet_strat_2
	#endif
	#ifdef USE_MOSFET_3
		|| act_id == ACT_QUEUE_Mosfet_strat_3
	#endif
	#ifdef USE_MOSFET_4
		|| act_id == ACT_QUEUE_Mosfet_strat_4
	#endif
	#ifdef USE_MOSFET_5
		|| act_id == ACT_QUEUE_Mosfet_strat_5
	#endif
	#ifdef USE_MOSFET_6
		|| act_id == ACT_QUEUE_Mosfet_strat_6
	#endif
	#ifdef USE_MOSFET_7
		|| act_id == ACT_QUEUE_Mosfet_strat_7
	#endif
	#ifdef USE_MOSFET_8
		|| act_id == ACT_QUEUE_Mosfet_strat_8
	#endif
	#ifdef USE_MOSFET_MULTI
		|| act_id == ACT_QUEUE_Mosfet_strat_multi
	#endif
	){
		return TRUE;
	}else{
		return FALSE;
	}
#else
	return FALSE;
#endif
}
#endif //I_AM_CARTE_STRAT

//Fonction de gestions des messages CAN pour les mosfets situés sur une carte Mosfet commandée par la stratégie
bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg) {
	bool_e result;
#if defined(I_AM_CARTE_ACT)
	CAN_msg_t msg_result;
	msg_result.sid = ACT_RESULT;
	msg_result.size = SIZE_ACT_RESULT;
	msg_result.data.act_result.sid = msg->sid;
	msg_result.data.act_result.cmd = msg->data.act_msg.order;
	msg_result.data.act_result.result = ACT_RESULT_DONE;
	msg_result.data.act_result.error_code = ACT_RESULT_ERROR_OK;
	UNUSED_VAR(msg_result);
#endif

	switch(msg->sid){
#ifdef USE_MOSFET_1
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_1:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_1:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_1, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_2
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_2:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_2:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_2, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_3
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_3:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_3:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_3, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_4
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_4:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_4:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_4, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_5
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_5:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_5:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_5, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_6
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_6:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_6:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_6, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_7
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_7:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_7:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_7, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_8
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_8:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_8:
#endif
		{
			switch(msg->data.act_msg.order) {
				case ACT_MOSFET_NORMAL:
				case ACT_MOSFET_STOP:
					MOSFET_do_order(msg->sid, msg->data.act_msg.order);
					#if defined(I_AM_CARTE_ACT)
						CAN_send(&msg_result);
					#elif defined(I_AM_CARTE_STRAT)
						ACT_set_result(ACT_QUEUE_Mosfet_strat_8, ACT_RESULT_Ok);
					#endif
					break;
				default: component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
			}
			result = TRUE;
		}break;
#endif
#ifdef USE_MOSFET_MULTI
#if defined(I_AM_CARTE_ACT)
		case ACT_MOSFET_MULTI:
#elif defined(I_AM_CARTE_STRAT)
		case STRAT_MOSFET_MULTI:
#endif
			MOSFET_do_order_multi(msg);
			result = TRUE;
		break;
#endif
		default :
			result = FALSE;
	}
	return result;
}

