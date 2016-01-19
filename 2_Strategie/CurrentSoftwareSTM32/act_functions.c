/*
 *	Club Robot ESEO 2009 - 2010
 *	CHOMP
 *
 *	Fichier : act_functions.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'actionneur
 *	Auteur : Julien et Ronan
 *	Version 20110313
 */

#include "act_functions.h"
#include "act_can.h"

#define LOG_PREFIX "act_f: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"
#include "act_avoidance.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_buffer_fifo.h"
#include "QS/QS_watchdog.h"
#include "state_machine_helper.h"
#include "Generic_functions.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_verbose.h"

#include "pompes_black_2016/pompe_very_left.h"
#include "pompes_black_2016/pompe_left.h"
#include "pompes_black_2016/pompe_middle_left.h"
#include "pompes_black_2016/pompe_middle.h"
#include "pompes_black_2016/pompe_middle_right.h"
#include "pompes_black_2016/pompe_right.h"
#include "pompes_black_2016/pompe_very_right.h"


#define ACT_SENSOR_ANSWER_TIMEOUT		500
#define ULU_TIME                        300
#define ACT_POMPE_DELAY                  10 //Delay en milliseconde entre l'allumage de chaque pompe
#define ACT_POMPE_SELFTEST_TIME         300 //Delay en milliseconde pour l'allumage de Selftest pour l'allumage de chaque pompe
typedef enum{
	ACT_SENSOR_WAIT = 0,
	ACT_SENSOR_ABSENT,
	ACT_SENSOR_PRESENT
}act_sensor_e;

const act_link_SID_Queue_s act_link_SID_Queue[] = {
	{0,							NB_QUEUE,						""},

	// Black
	{ACT_FISH_MAGNETIC_ARM,			ACT_QUEUE_Fish_magnetic_arm,			"Fish magnetic arm"},
	{ACT_FISH_UNSTICK_ARM,		    ACT_QUEUE_Fish_unstick_arm,		        "Fish unstick arm"},
	{ACT_BOTTOM_DUNE_LEFT,			ACT_QUEUE_Bottom_dune_left,             "Bottom dune left"},
	{ACT_BOTTOM_DUNE_RIGHT,			ACT_QUEUE_Bottom_dune_right,            "Bottom dune right"},
	{ACT_BLACK_SAND_CIRCLE,			ACT_QUEUE_Black_sand_circle,            "Sand Circle"},
	{ACT_MIDDLE_DUNE_LEFT,			ACT_QUEUE_Middle_dune_left,             "Middle dune left"},
	{ACT_MIDDLE_DUNE_RIGHT,			ACT_QUEUE_Middle_dune_right,            "Middle dune right"},
	{ACT_CONE_DUNE,					ACT_QUEUE_Cone_dune,					"Cone dune"},
	{ACT_DUNIX_LEFT,			    ACT_QUEUE_Dunix_left,                   "Dunix left"},
	{ACT_DUNIX_RIGHT,			    ACT_QUEUE_Dunix_right,                  "Dunix right"},
	{ACT_SAND_LOCKER_LEFT,			ACT_QUEUE_Sand_locker_left,             "Sand locker left"},
	{ACT_SAND_LOCKER_RIGHT,			ACT_QUEUE_Sand_locker_right,            "Sand locker right"},
	{ACT_SHIFT_CYLINDER,			ACT_QUEUE_Shift_cylinder,				"Shift cylinder"},
	{ACT_PENDULUM,			        ACT_QUEUE_Pendulum,						"Pendulum"},
	{ACT_POMPE_SAND_LOCKER_LEFT,	ACT_QUEUE_Pompe_sand_locker_left,       "Sand locker left"},
	{ACT_POMPE_SAND_LOCKER_RIGHT,	ACT_QUEUE_Pompe_sand_locker_right,      "Sand locker right"},
	{ACT_POMPE_BLACK_FRONT_LEFT,    ACT_QUEUE_Pompe_black__front_left,		"Pompe front left"},
	{ACT_POMPE_BLACK_FRONT_RIGHT,   ACT_QUEUE_Pompe_black_front_right,		"Pompe front right"},
	{ACT_POMPE_PENDULUM,			ACT_QUEUE_Pompe_pendulum,				"Pendulum"},
	{ACT_POMPE_VERY_LEFT,			ACT_QUEUE_Pompe_very_left,				"Pompe very left"},
	{ACT_POMPE_LEFT,			    ACT_QUEUE_Pompe_left,				    "Pompe left"},
	{ACT_POMPE_MIDDLE_LEFT,			ACT_QUEUE_Pompe_middle_left,			"Pompe middle left"},
	{ACT_POMPE_MIDDLE,			    ACT_QUEUE_Pompe_middle,				    "Pompe middle"},
	{ACT_POMPE_MIDDLE_RIGHT,		ACT_QUEUE_Pompe_middle_right,			"Pompe middle right"},
	{ACT_POMPE_RIGHT,			    ACT_QUEUE_Pompe_right,				    "Pompe right"},
	{ACT_POMPE_VERY_RIGHT,			ACT_QUEUE_Pompe_very_right,				"Pompe very right"},
	{ACT_POMPE_ALL,      			ACT_QUEUE_Pompe_all,			        "Pompe all"},
	{ACT_POMPE_VERY_ALL,      	    ACT_QUEUE_Pompe_very_all,			    "Pompe very all"},

	// Pearl
	{ACT_SAND_LEFT_ARM,         ACT_QUEUE_Sand_left_arm,            "Left Arm"},
	{ACT_SAND_RIGHT_ARM,        ACT_QUEUE_Sand_right_arm,			"Right Arm"},
	{ACT_POMPE_BACK_LEFT,       ACT_QUEUE_Pompe_back_left,			"Pompe BL"},
	{ACT_POMPE_BACK_RIGHT,      ACT_QUEUE_Pompe_back_right,			"Pompe BR"},
	{ACT_POMPE_FRONT_LEFT,      ACT_QUEUE_Pompe_front_left,			"Pompe FL"},
	{ACT_POMPE_FRONT_RIGHT,     ACT_QUEUE_Pompe_front_right,		"Pompe FR"},
	{ACT_PEARL_SAND_CIRCLE,		ACT_QUEUE_Pearl_sand_circle,		"Sand Circle"},


};

static const Uint8 act_link_SID_Queue_size = sizeof(act_link_SID_Queue)/sizeof(act_link_SID_Queue_s);

volatile act_sensor_e sensor_answer[NB_ACT_SENSOR] = {0};


/* Pile contenant les arguments d'une demande d'opération
 * Contient les messages CAN à envoyer à la carte actionneur pour exécuter l'action.
 * fallbackMsg contient le message CAN lorsque l'opération demandée par le message CAN msg ne peut pas être complétée (bras bloqué, robot adverse qui bloque l'actionneur par exemple)
 * On utilise une structure différente de CAN_msg_t pour économiser la RAM (voir matrice act_args)
 *
 * Pour chaque fonction appelée par le code stratégie pour empiler une action, on remplit les messages CAN à envoyer et on empile l'action.
 * Un timeout est disponible si jamais la carte actionneur ne renvoie pas de message CAN ACT_RESULT (elle le devrait, mais on sait jamais)
 *
 * Le code de ACT_check_result() gère le renvoi de message lorsque la carte actionneur indique qu'il n'y avait pas assez de ressource ou l'envoi du message fallback si la position demandé par le message CAN msg n'est pas atteignable.
 * Si le message fallback se solve par un echec aussi, on indique au code de stratégie que cet actionneur n'est pas dispo maintenant et de réessayer plus tard (il faut faire une autre strat pendant ce temps, si c'est un robot qui bloque le bras, il faut que l'environnement du jeu bouge)
 * Si le renvoi du message à cause du manque de ressource cause la même erreur, on marque l'actionneur comme inutilisable (ce cas est grave, il y a probablement un problème dans le code actionneur ou un flood de demande d'opération de la carte stratégie)
 */


//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur génère des resultats et détaille les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) convertit les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) convertit et gère les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour être ensuite utilisé par le reste du code stratégie.


//FONCTIONS D'ACTIONNEURS

////////////////////////////////////////
///////////// BLACK & PEARL ////////////
////////////////////////////////////////


Uint8 ACT_search_link_SID_Queue(ACT_sid_e sid){
	Uint8 i;
	for(i=0;i<act_link_SID_Queue_size;i++){
		if(act_link_SID_Queue[i].sid == sid){
			return i;
		}
	}
	return 0;
}

bool_e ACT_push_order(ACT_sid_e sid,  ACT_order_e order){
	QUEUE_arg_t args;
	Uint8 i;

	i = ACT_search_link_SID_Queue(sid);

	if(i == act_link_SID_Queue_size){
		error_printf("Link SID non trouvé dans ACT_push_order !\n");
		return FALSE;
	}

	ACT_arg_init(&args, sid, order);
	ACT_arg_set_fallbackmsg(&args, sid,  ACT_DEFAULT_STOP);

	debug_printf("Pushing %s Run cmd (sid : 0x%x   order : %d)\n", act_link_SID_Queue[i].name, sid, order);

	ACT_AVOIDANCE_new_classic_cmd(act_link_SID_Queue[i].queue_id, order);

	return ACT_push_operation(act_link_SID_Queue[i].queue_id, &args);
}

bool_e ACT_push_order_with_param(ACT_sid_e sid,  ACT_order_e order, Uint16 param){
	QUEUE_arg_t args;
	Uint8 i;

	i = ACT_search_link_SID_Queue(sid);

	if(i == act_link_SID_Queue_size){
		error_printf("Link SID non trouvé dans ACT_push_order !\n");
		return FALSE;
	}

	ACT_arg_init_with_param(&args, sid, order, param);
	ACT_arg_set_fallbackmsg(&args, sid,  ACT_DEFAULT_STOP);

	debug_printf("Pushing %s Run cmd (sid : %d   order : %d)\n", act_link_SID_Queue[i].name, sid, order);

	ACT_AVOIDANCE_new_classic_cmd(act_link_SID_Queue[i].queue_id, order);

	return ACT_push_operation(order, &args);
}

////////////////////////////////////////
//////////////// COMMON ////////////////
////////////////////////////////////////

bool_e ACT_config(Uint16 sid, Uint8 sub_act, Uint8 cmd, Uint16 value){
	QUEUE_arg_t args;
	CAN_msg_t msg;

	msg.sid = sid;
	msg.size = SIZE_ACT_CONFIG;
	msg.data.act_msg.order = ACT_CONFIG;
	msg.data.act_msg.act_data.act_config.sub_act_id = sub_act;
	msg.data.act_msg.act_data.act_config.config = cmd;
	msg.data.act_msg.act_data.act_config.data_config.raw_data = value;

	ACT_arg_init_with_msg(&args, msg);
	ACT_arg_set_timeout(&args, 0);

	debug_printf("Config : %s\n", act_link_SID_Queue[ACT_search_link_SID_Queue(sid)].name);
	debug_printf("    cmd : %d\n", cmd);
	debug_printf("    value : %d\n", value);

	return ACT_push_operation(act_link_SID_Queue[ACT_search_link_SID_Queue(sid)].queue_id, &args);
}

////////////////////////////////////////
//////////////// MAE  //////////////////
////////////////////////////////////////
void ACT_init_all_pompes(){
	ACT_push_order(ACT_POMPE_VERY_ALL, ACT_POMPE_VERY_ALL_STOP);
}

void ACT_transmit_order_to_pompe(CAN_msg_t* msg){
	static queue_id_e act_id = NB_QUEUE;
	static Uint11 current_sid;
	static Uint11 state;
	static Uint11 last_state = NB_QUEUE + 1;
	static bool_e entrance;
	static time32_t state_time;

	//initialisation en cas de l'envoi d'une nouvelle commande
	if(msg != NULL){
		assert((msg->sid&0x300) == ACT_FILTER);
		act_id = act_link_SID_Queue[ACT_search_link_SID_Queue((Uint16)(ACT_FILTER | msg->data.act_result.sid))].queue_id; //id de la pompe
		state = act_id;
		current_sid = msg->sid;
		//verbose de l'action
		if(IHM_switchs_get(SWITCH_VERBOSE))
			QS_CAN_VERBOSE_can_msg_print(msg, VERB_LOG_MSG);
	}

	//Affectation des variables courantes
	entrance = (last_state != state);
	last_state = state;
	if(entrance){
		state_time = global.absolute_time;
	}

	switch(state){
		case ACT_QUEUE_Pompe_very_all:
		case ACT_QUEUE_Pompe_very_left:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_VERY_LEFT_command(ACT_POMPE_VERY_LEFT_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_VERY_LEFT_command(ACT_POMPE_VERY_LEFT_STOP);
					state = ACT_QUEUE_Pompe_very_right;
				}
			}else{
				if(entrance){
					if(current_sid == ACT_POMPE_VERY_ALL)
						msg->sid = ACT_POMPE_VERY_LEFT;
					POMPE_VERY_LEFT_CAN_process_msg(msg);
				}else if(current_sid  == ACT_POMPE_VERY_ALL && global.absolute_time >= state_time + ACT_POMPE_DELAY){
					state = ACT_QUEUE_Pompe_very_right;
				}else if(current_sid != ACT_POMPE_VERY_ALL){
					state = NB_QUEUE; //finish
				}
			}
			break;

		case ACT_QUEUE_Pompe_very_right:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_VERY_RIGHT_command(ACT_POMPE_VERY_RIGHT_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_VERY_RIGHT_command(ACT_POMPE_VERY_RIGHT_STOP);
					state = ACT_QUEUE_Pompe_left;
				}
			}else{
				if(entrance){
					if(current_sid == ACT_POMPE_VERY_ALL)
						msg->sid = ACT_POMPE_VERY_RIGHT;
					POMPE_VERY_RIGHT_CAN_process_msg(msg);
				}else if(current_sid  == ACT_POMPE_VERY_ALL && global.absolute_time >= state_time + ACT_POMPE_DELAY){
					state = ACT_QUEUE_Pompe_left;
				}else if(current_sid != ACT_POMPE_VERY_ALL){
					state = NB_QUEUE; //finish
				}
			}
			break;

		case ACT_QUEUE_Pompe_all:
		case ACT_QUEUE_Pompe_left:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_LEFT_command(ACT_POMPE_LEFT_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_LEFT_command(ACT_POMPE_LEFT_STOP);
					state = ACT_QUEUE_Pompe_middle_left;
				}
			}else{
				if(entrance){
					if(msg->sid == ACT_POMPE_VERY_ALL || msg->sid == ACT_POMPE_ALL)
						msg->sid = ACT_POMPE_LEFT;
					POMPE_LEFT_CAN_process_msg(msg);
				}else if((current_sid == ACT_POMPE_VERY_ALL || current_sid == ACT_POMPE_ALL) && global.absolute_time >= state_time + ACT_POMPE_DELAY){
					state = ACT_QUEUE_Pompe_middle_left;
				}else if(current_sid != ACT_POMPE_VERY_ALL && current_sid != ACT_POMPE_ALL){
					state = NB_QUEUE; //finish
				}
			}
			break;

		case ACT_QUEUE_Pompe_middle_left:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_MIDDLE_LEFT_command(ACT_POMPE_MIDDLE_LEFT_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_MIDDLE_LEFT_command(ACT_POMPE_MIDDLE_LEFT_STOP);
					state = ACT_QUEUE_Pompe_middle;
				}
			}else{
				if(entrance){
					if(msg->sid == ACT_POMPE_VERY_ALL || msg->sid == ACT_POMPE_ALL)
						msg->sid = ACT_POMPE_MIDDLE_LEFT;
					POMPE_MIDDLE_LEFT_CAN_process_msg(msg);
				}else if((current_sid == ACT_POMPE_VERY_ALL || current_sid == ACT_POMPE_ALL) && global.absolute_time >= state_time + ACT_POMPE_DELAY){
					state = ACT_QUEUE_Pompe_middle;
				}else if(current_sid != ACT_POMPE_VERY_ALL && current_sid != ACT_POMPE_ALL){
					state = NB_QUEUE; //finish
				}
			}
			break;

		case ACT_QUEUE_Pompe_middle:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_MIDDLE_command(ACT_POMPE_MIDDLE_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_MIDDLE_command(ACT_POMPE_MIDDLE_STOP);
					state = ACT_QUEUE_Pompe_middle_right;
				}
			}else{
				if(entrance){
					if(msg->sid == ACT_POMPE_VERY_ALL || msg->sid == ACT_POMPE_ALL)
						msg->sid = ACT_POMPE_MIDDLE;
					POMPE_MIDDLE_CAN_process_msg(msg);
				}else if((current_sid == ACT_POMPE_VERY_ALL || current_sid == ACT_POMPE_ALL) && global.absolute_time >= state_time + ACT_POMPE_DELAY){
					state = ACT_QUEUE_Pompe_middle_right;
				}else if(current_sid != ACT_POMPE_VERY_ALL && current_sid != ACT_POMPE_ALL){
					state = NB_QUEUE; //finish
				}
			}
			break;

		case ACT_QUEUE_Pompe_middle_right:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_MIDDLE_RIGHT_command(ACT_POMPE_MIDDLE_RIGHT_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_MIDDLE_RIGHT_command(ACT_POMPE_MIDDLE_RIGHT_STOP);
					state = ACT_QUEUE_Pompe_right;
				}
			}else{
				if(entrance){
					if(msg->sid == ACT_POMPE_VERY_ALL || msg->sid == ACT_POMPE_ALL)
						msg->sid = ACT_POMPE_MIDDLE_RIGHT;
					POMPE_MIDDLE_RIGHT_CAN_process_msg(msg);
				}else if((current_sid == ACT_POMPE_VERY_ALL || current_sid == ACT_POMPE_ALL) && global.absolute_time >= state_time + ACT_POMPE_DELAY){
					state = ACT_QUEUE_Pompe_right;
				}else if(current_sid != ACT_POMPE_VERY_ALL && current_sid != ACT_POMPE_ALL){
					state = NB_QUEUE; //finish
				}
			}
			break;

		case ACT_QUEUE_Pompe_right:
			if(current_sid == ACT_DO_SELFTEST)
			{
				if(entrance){
					POMPE_RIGHT_command(ACT_POMPE_RIGHT_NORMAL);
				}else if(global.absolute_time >= state_time + ACT_POMPE_SELFTEST_TIME){
					POMPE_RIGHT_command(ACT_POMPE_RIGHT_STOP);
					state = NB_QUEUE;
				}
			}else{
				if(entrance){
					if(msg->sid == ACT_POMPE_VERY_ALL || msg->sid == ACT_POMPE_ALL)
						msg->sid = ACT_POMPE_RIGHT;
					POMPE_RIGHT_CAN_process_msg(msg);
				}
			}
			state = NB_QUEUE;
			break;

		case NB_QUEUE:
			if(entrance)
				ACT_set_result(act_id, ACT_RESULT_Ok);
			//On ne fait rien. On attend le prochain ordre.
			break;

		default:
			debug_printf("Default state in ACT_transmit_order_to_pompe: bad sid\n");
	}
}

////////////////////////////////////////
//////////////// SENSOR ////////////////
////////////////////////////////////////

error_e ACT_get_sensor(act_sensor_id_e act_sensor_id){
	CREATE_MAE(SEND,
				WAIT);

	static time32_t begin_time;

	switch(state){
		case SEND:{
			sensor_answer[act_sensor_id] = ACT_SENSOR_WAIT;
			begin_time = global.absolute_time;
			CAN_msg_t msg;
			msg.sid = ACT_ASK_SENSOR;
			msg.size = SIZE_ACT_ASK_SENSOR;
			msg.data.act_ask_sensor.act_sensor_id = act_sensor_id;
			CAN_send(&msg);
			state = WAIT;
			}break;

		case WAIT:
			if(global.absolute_time - begin_time > ACT_SENSOR_ANSWER_TIMEOUT){
				RESET_MAE();
				return END_WITH_TIMEOUT;
			}else if(sensor_answer[act_sensor_id] == ACT_SENSOR_PRESENT){
				RESET_MAE();
				return END_OK;
			}else if(sensor_answer[act_sensor_id] == ACT_SENSOR_ABSENT){
				RESET_MAE();
				return NOT_HANDLED;
			}
			break;
	}
	return IN_PROGRESS;
}

void ACT_sensor_answer(CAN_msg_t* msg){
	assert(msg->data.strat_inform_capteur.sensor_id < NB_ACT_SENSOR);
	if(msg->data.strat_inform_capteur.present)
		sensor_answer[msg->data.strat_inform_capteur.sensor_id] = ACT_SENSOR_PRESENT;
	else
		sensor_answer[msg->data.strat_inform_capteur.sensor_id] = ACT_SENSOR_ABSENT;
}
