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

#define LOG_PREFIX "act_f: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "../QS/QS_outputlog.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_can_verbose.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_buffer_fifo.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../utils/actionChecker.h"
#include "../utils/generic_functions.h"
#include "act_avoidance.h"

#define ACT_CONFIG_ANSWER_TIMEOUT		500
#define ACT_SENSOR_ANSWER_TIMEOUT		500
#define ULU_TIME                        300

#define ACT_VACUOSTAT_TIME_VALID		500
#define ACT_VACUOSTAT_TIMEOUT			500
#define ACT_NB_VACUOSTAT				8

#define ACT_TURBINE_GET_SPEED_TIMEOUT	500

typedef struct{
	Uint16 speed;
	time32_t lastRefresh;
}act_turbine_s;

typedef struct{
	MOSFET_BOARD_CURRENT_MEASURE_state_e state;
	time32_t lastRefresh;
}act_vacuostat_s;

typedef enum{
	ACT_SENSOR_WAIT = 0,
	ACT_SENSOR_ABSENT,
	ACT_SENSOR_PRESENT
}act_sensor_e;

const act_link_SID_Queue_s act_link_SID_Queue[] = {
	{0,							    NB_QUEUE,						""},

	// Harry
	{ACT_BEARING_BALL_WHEEL,			ACT_QUEUE_Bearing_ball_wheel,			"Bearing ball wheel"},
    {ACT_BIG_BALL_FRONT_LEFT,			ACT_QUEUE_Big_bearing_front_left,		"Bearing ball front left"},
    {ACT_BIG_BALL_FRONT_RIGHT,			ACT_QUEUE_Big_bearing_front_right,		"Bearing ball front right"},
    {ACT_BIG_BALL_BACK_LEFT,			ACT_QUEUE_Big_bearing_back_left,		"Bearing ball back left"},
    {ACT_BIG_BALL_BACK_RIGHT,			ACT_QUEUE_Big_bearing_back_right,		"Bearing ball back right"},

    {ACT_CYLINDER_BALANCER_LEFT,		ACT_QUEUE_Cylinder_balancer_left,		"Cylinder dispenser left"},
    {ACT_CYLINDER_BALANCER_RIGHT,		ACT_QUEUE_Cylinder_balancer_right,		"Cylinder dispenser right"},
    {ACT_CYLINDER_ELEVATOR_LEFT,		ACT_QUEUE_Cylinder_elevator_left,		"Cylinder elevator left"},
    {ACT_CYLINDER_ELEVATOR_RIGHT,		ACT_QUEUE_Cylinder_elevator_right,		"Cylinder elevator right"},
    {ACT_CYLINDER_PUSHER_LEFT,			ACT_QUEUE_Cylinder_pusher_left,			"Cylinder pusher left"},
    {ACT_CYLINDER_PUSHER_RIGHT,			ACT_QUEUE_Cylinder_pusher_right,		"Cylinder pusher right"},
    {ACT_CYLINDER_SLIDER_LEFT,			ACT_QUEUE_Cylinder_slider_left,			"Cylinder slider left"},
    {ACT_CYLINDER_SLIDER_RIGHT,			ACT_QUEUE_Cylinder_slider_right,		"Cylinder slider right"},
    {ACT_CYLINDER_SLOPE_LEFT,			ACT_QUEUE_Cylinder_slope_left,			"Cylinder slope left"},
    {ACT_CYLINDER_SLOPE_RIGHT,			ACT_QUEUE_Cylinder_slope_right,			"Cylinder slope right"},
    {ACT_CYLINDER_ARM_LEFT,				ACT_QUEUE_Cylinder_arm_left,		"Cylinder turn left arm"},
    {ACT_CYLINDER_ARM_RIGHT,			ACT_QUEUE_Cylinder_arm_right,		"Cylinder turn right arm"},
	{ACT_CYLINDER_COLOR_LEFT,			ACT_QUEUE_Cylinder_color_left,			"Cylinder color left"},
	{ACT_CYLINDER_COLOR_RIGHT,			ACT_QUEUE_Cylinder_color_right,			"Cylinder color right"},
	{ACT_CYLINDER_DISPOSE_LEFT,			ACT_QUEUE_Cylinder_dispose_left,		"Cylinder dispose left"},
	{ACT_CYLINDER_DISPOSE_RIGHT,		ACT_QUEUE_Cylinder_dispose_right,		"Cylinder dispose right"},

	{ACT_ORE_GUN,			  		    ACT_QUEUE_Ore_gun,			        	"Ore gun"},
	{ACT_ORE_WALL,						ACT_QUEUE_Ore_wall,						"Ore wall"},
    {ACT_ORE_ROLLER_ARM,                ACT_QUEUE_Ore_roller_arm,               "Ore roller arm"},
    {ACT_ORE_ROLLER_FOAM,               ACT_QUEUE_Ore_roller_foam,              "Ore roller foam"},
	{ACT_ORE_TRIHOLE,               	ACT_QUEUE_Ore_trihole,              	"Ore trihole"},


    {ACT_ROCKET,                        ACT_QUEUE_Rocket,                       "Rocket"},


	// Anne
	{ACT_SMALL_BALL_FRONT_LEFT,			ACT_QUEUE_Small_bearing_front_left,			"Bearing ball front left"},
	{ACT_SMALL_BALL_FRONT_RIGHT,		ACT_QUEUE_Small_bearing_front_right,		"Bearing ball front right"},
	{ACT_SMALL_BALL_BACK_LEFT,			ACT_QUEUE_Small_bearing_back_left,			"Bearing ball back left"},
	{ACT_SMALL_BALL_BACK_RIGHT,			ACT_QUEUE_Small_bearing_back_right,			"Bearing ball back right"},

	// Mosfets actionneurs
	{ACT_POMPE_SLIDER_LEFT,			ACT_QUEUE_Pompe_act_slider_left,		"Mosfet_act_slider_left"},
	{ACT_POMPE_SLIDER_RIGHT,		ACT_QUEUE_Pompe_act_slider_right,		"Mosfet_act_slider_right"},
	{ACT_POMPE_ELEVATOR_LEFT,		ACT_QUEUE_Pompe_act_elevator_left,		"Mosfet_act_elevator_left"},
	{ACT_POMPE_ELEVATOR_RIGHT,		ACT_QUEUE_Pompe_act_elevator_right,		"Mosfet_act_elevator_right"},
	{ACT_POMPE_DISPOSE_LEFT,		ACT_QUEUE_Pompe_act_dispose_left,		"Mosfet_act_dispose_left"},
	{ACT_POMPE_DISPOSE_RIGHT,		ACT_QUEUE_Pompe_act_dispose_right,		"Mosfet_act_dispose_right"},
	{ACT_MOSFET_7,					ACT_QUEUE_Mosfet_act_7,			        "Mosfet_act_7"},
	{ACT_MOSFET_8,					ACT_QUEUE_Mosfet_act_8,				    "Mosfet_act_8"},
	{ACT_MOSFET_MULTI,      		ACT_QUEUE_Mosfet_act_multi,		        "Mosfet_act_multi"},

	//Mosfets stratégie
	{STRAT_MOSFET_1,				ACT_QUEUE_Mosfet_strat_1,				"Mosfet_strat_0"},
	{STRAT_MOSFET_2,				ACT_QUEUE_Mosfet_strat_2,				"Mosfet_strat_1"},
	{STRAT_MOSFET_3,				ACT_QUEUE_Mosfet_strat_3,				"Mosfet_strat_2"},
	{STRAT_MOSFET_4,				ACT_QUEUE_Mosfet_strat_4,				"Mosfet_strat_3"},
	{STRAT_MOSFET_5,	           	ACT_QUEUE_Mosfet_strat_5,				"Mosfet_strat_5"},
	{STRAT_MOSFET_6,                ACT_QUEUE_Mosfet_strat_6,               "Mosfet_strat_6"},
	{STRAT_MOSFET_7,               	ACT_QUEUE_Mosfet_strat_7,               "Mosfet_strat_7"},
	{STRAT_MOSFET_8,                ACT_QUEUE_Mosfet_strat_8,		        "Mosfet_strat_8"},
	{STRAT_MOSFET_MULTI,	        ACT_QUEUE_Mosfet_strat_multi,			"Mosfet_strat_multi"},

};

static const Uint8 act_link_SID_Queue_size = sizeof(act_link_SID_Queue)/sizeof(act_link_SID_Queue_s);

// Config actionneurs
static act_config_s act_config[sizeof(act_link_SID_Queue)/sizeof(act_link_SID_Queue_s)] = {0};

// Warner actionneurs
static bool_e act_warner[sizeof(act_link_SID_Queue)/sizeof(act_link_SID_Queue_s)] = {0};

// Capteurs
static volatile act_sensor_e sensor_answer[NB_ACT_SENSOR] = {0};

static volatile act_vacuostat_s vacuostat[ACT_NB_VACUOSTAT] = {0};

static volatile act_turbine_s turbine;


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
////////// ORDRES ACTIONNEURS //////////
////////////////////////////////////////

//Recherche l'indice de la case du tableau act_link_SID_Queue ayant pour sid le sid passé en paramètre
//Attention pour avoir l'indice de la Queue il faut faire -1 sur la résultat.
//ex: ACT_FIRST_ACT=301  ACT_QUEUE_First_act=0
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

	i = ACT_search_link_SID_Queue(sid) ;

	if(i >= act_link_SID_Queue_size || i == 0){
		error_printf("Link SID non trouvé dans ACT_push_order !\n");
		return FALSE;
	}

	ACT_arg_init(&args, sid, order);
	ACT_arg_set_fallbackmsg(&args, sid,  ACT_DEFAULT_STOP);

	debug_printf("Pushing %s Run cmd (sid : 0x%x   order : %d)\n", act_link_SID_Queue[i].name, sid, order);

	ACT_AVOIDANCE_new_classic_cmd(act_link_SID_Queue[i].queue_id, order);

	return ACT_push_operation(act_link_SID_Queue[i].queue_id, &args);
}


bool_e ACT_push_order_with_timeout(ACT_sid_e sid,  ACT_order_e order, Uint16 timeout){
	QUEUE_arg_t args;
	Uint8 i;

	i = ACT_search_link_SID_Queue(sid) ;

	if(i >= act_link_SID_Queue_size || i == 0){
		error_printf("Link SID non trouvé dans ACT_push_order !\n");
		return FALSE;
	}

	ACT_arg_init(&args, sid, order);
	ACT_arg_set_timeout(&args, timeout);
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

	return ACT_push_operation(act_link_SID_Queue[i].queue_id, &args);
}

////////////////////////////////////////
//////////////// CONFIG ////////////////
////////////////////////////////////////

bool_e ACT_set_config(Uint16 sid, Uint8 sub_act, act_config_e cmd, Uint16 value){
	QUEUE_arg_t args;
	CAN_msg_t msg;

	msg.sid = sid;
	msg.size = SIZE_ACT_SET_CONFIG;
	msg.data.act_msg.order = ACT_SET_CONFIG;
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

bool_e ACT_get_config_request(Uint16 sid, act_config_e config){
	QUEUE_arg_t args;
	CAN_msg_t msg;

	msg.sid = sid;
	msg.size = SIZE_ACT_GET_CONFIG;
	msg.data.act_msg.order = ACT_GET_CONFIG;
	msg.data.act_msg.act_data.config = config;

	ACT_arg_init_with_msg(&args, msg);
	ACT_arg_set_timeout(&args, 0);

	return ACT_push_operation(act_link_SID_Queue[ACT_search_link_SID_Queue(sid)].queue_id, &args);
}

void ACT_get_config_answer(CAN_msg_t* msg){
	assert(msg->sid == ACT_GET_CONFIG_ANSWER);
	Uint8 i = ACT_search_link_SID_Queue((Uint16)(ACT_FILTER | msg->data.act_get_config_answer.sid));

	switch(msg->data.act_get_config_answer.config){
		case POSITION_CONFIG:
			act_config[i].pos = msg->data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.order;
			break;
		case TORQUE_CONFIG:
			act_config[i].torque = msg->data.act_get_config_answer.act_get_config_data.torque;
			break;
		case TEMPERATURE_CONFIG:
			act_config[i].temperature = msg->data.act_get_config_answer.act_get_config_data.temperature;
			break;
		case LOAD_CONFIG:
			act_config[i].load = msg->data.act_get_config_answer.act_get_config_data.load;
			break;
		default:
			error_printf("Error config recieved but not requested !! file %s line %d\n", __FILE__, __LINE__);
	}

	act_config[i].info_received = TRUE; // On vient de recevoir une réponse
}

error_e ACT_check_position_config(Uint16 sid, ACT_order_e order){
	CREATE_MAE(SEND,
				WAIT);

	static time32_t begin_time;
	static Uint8 i = 0;
	error_e ret = IN_PROGRESS;

	switch(state){
		case SEND:
			i = ACT_search_link_SID_Queue(sid);
			ACT_get_config_request(sid, POSITION_CONFIG);
			act_config[i].info_received = FALSE;
			begin_time = global.absolute_time;
			state = WAIT;
			break;

		case WAIT:
			if(global.absolute_time - begin_time > ACT_CONFIG_ANSWER_TIMEOUT){
				RESET_MAE();
				ret = END_WITH_TIMEOUT;
			}else if(act_config[i].info_received && act_config[i].pos == order){ // On a recu une réponse
				RESET_MAE();
				ret = END_OK;
			}else if(act_config[i].info_received){
				RESET_MAE();
				ret = NOT_HANDLED;
			}
			break;
	}
	return ret;
}

////////////////////////////////////////
//////////////// WARNER ////////////////
////////////////////////////////////////

bool_e ACT_set_warner(Uint16 sid, ACT_order_e pos){
	QUEUE_arg_t args;
	CAN_msg_t msg;
	Uint8 i = ACT_search_link_SID_Queue(sid);

	msg.sid = sid;
	msg.size = SIZE_ACT_WARNER;
	msg.data.act_msg.order = ACT_WARNER;
	msg.data.act_msg.act_data.warner_pos = pos;

	ACT_arg_init_with_msg(&args, msg);
	ACT_arg_set_timeout(&args, 0);

	act_warner[i] = FALSE; // On baisse le flag

	return ACT_push_operation(act_link_SID_Queue[i].queue_id, &args);
}

void ACT_warner_answer(CAN_msg_t* msg){
	assert(msg->sid == ACT_WARNER_ANSWER);
	Uint8 i = ACT_search_link_SID_Queue((Uint16)(ACT_FILTER | msg->data.act_warner_answer.sid));

	act_warner[i] = TRUE; // On lève le flag
}

bool_e ACT_get_warner(Uint16 sid){
	return act_warner[ACT_search_link_SID_Queue(sid)];
}

void ACT_reset_all_warner(){
	Uint8 i;
	for(i = 0; i < sizeof(act_link_SID_Queue)/sizeof(act_link_SID_Queue_s); i++){
		act_warner[i] = FALSE;
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


Uint8 ACT_wait_state_vacuostat(Uint8 idVacuostat, MOSFET_BOARD_CURRENT_MEASURE_state_e stateToWait, Uint8 in_progress, Uint8 sucess, Uint8 fail){
	CREATE_MAE(INIT,
				GET_STATE,
				WAIT_STATE);

	static time32_t timeBegin;

	switch(state){
		case INIT:
			if(idVacuostat >= ACT_NB_VACUOSTAT){
				error_printf("waitVacuostat id hors tableau\n");
				RESET_MAE();
				return fail;
			}
			timeBegin = global.absolute_time;
			state = GET_STATE;
			break;

		case GET_STATE:{
			CAN_msg_t msg;
			msg.sid = ACT_GET_MOSFET_CURRENT_STATE;
			msg.size = SIZE_ACT_GET_MOSFET_CURRENT_STATE;
			msg.data.act_get_mosfet_state.id = idVacuostat;
			CAN_send(&msg);

			state = WAIT_STATE;
			}break;

		case WAIT_STATE:
			if(global.absolute_time - vacuostat[idVacuostat].lastRefresh < ACT_VACUOSTAT_TIME_VALID && vacuostat[idVacuostat].state == stateToWait){
				RESET_MAE();
				return sucess;
			}else if(global.absolute_time - timeBegin > ACT_VACUOSTAT_TIMEOUT){
				info_printf("vacuostat %d timeout\n", idVacuostat);
				RESET_MAE();
				return fail;
			}

			break;

		default:
			error_printf("waitVacuostat case default\n");
			RESET_MAE();
			return fail;
	}

	return in_progress;
}

void ACT_receive_vacuostat_msg(CAN_msg_t *msg){
	if(msg->sid != ACT_TELL_MOSFET_CURRENT_STATE)
		return;

	if(msg->data.act_tell_mosfet_state.id >= ACT_NB_VACUOSTAT)
		return;

	vacuostat[msg->data.act_tell_mosfet_state.id].state = msg->data.act_tell_mosfet_state.state;
	vacuostat[msg->data.act_tell_mosfet_state.id].lastRefresh = global.absolute_time;
}

Uint8 ACT_get_turbine_speed(Uint16 * speed,  Uint8 in_progress, Uint8 sucess, Uint8 fail){
	CREATE_MAE(INIT,
				GET_SPEED,
				WAIT_SPEED);

	static time32_t timeBegin;

	switch(state){
		case INIT:
			if(speed == NULL){
				error_printf("ERROR ACT_get_turbine_speed speed == NULL\n");
				RESET_MAE();
				return fail;
			}
			timeBegin = global.absolute_time;
			state = GET_SPEED;
			break;

		case GET_SPEED:{
			CAN_msg_t msg;
			msg.sid = ACT_GET_TURBINE_SPEED;
			msg.size = SIZE_ACT_GET_TURBINE_SPEED;
			CAN_send(&msg);

			state = WAIT_SPEED;
			}break;

		case WAIT_SPEED:
			if(global.absolute_time - turbine.lastRefresh < ACT_VACUOSTAT_TIME_VALID){
				*speed = turbine.speed;
				RESET_MAE();
				return sucess;
			}else if(global.absolute_time - timeBegin > ACT_TURBINE_GET_SPEED_TIMEOUT){
				info_printf("ACT_get_turbine_speed timeout\n");
				RESET_MAE();
				return fail;
			}

			break;

		default:
			error_printf("ACT_get_turbine_speed case default\n");
			RESET_MAE();
			return fail;
	}

	return in_progress;
}

void ACT_receive_turbine_msg(CAN_msg_t * msg){
	if(msg->sid != ACT_TELL_TURBINE_SPEED)
		return;

	turbine.speed = msg->data.act_tell_turbine_speed.speed;
	turbine.lastRefresh = global.absolute_time;
}

void ACT_set_turbine_speed(Uint16 speed){
	CAN_msg_t msg;
	msg.sid = ACT_SET_TURBINE_SPEED;
	msg.size = SIZE_ACT_SET_TURBINE_SPEED;
	msg.data.act_set_turbine_speed.speed = speed;
	CAN_send(&msg);
}
