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
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_can_verbose.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_buffer_fifo.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../utils/actionChecker.h"
#include "../utils/generic_functions.h"
#include "act_avoidance.h"

#define ACT_SENSOR_ANSWER_TIMEOUT		500
#define ULU_TIME                        300

typedef enum{
	ACT_SENSOR_WAIT = 0,
	ACT_SENSOR_ABSENT,
	ACT_SENSOR_PRESENT
}act_sensor_e;

const act_link_SID_Queue_s act_link_SID_Queue[] = {
	{0,							    NB_QUEUE,						""},

	// Harry
	//{ACT_FISH_MAGNETIC_ARM,			ACT_QUEUE_Fish_magnetic_arm,			"Fish magnetic arm"},
    //{ACT_BILLIX_TEST,			        ACT_QUEUE_Billix_test,			        "Billix test"},
    {ACT_ORE_GUN,			  		    ACT_QUEUE_Ore_gun,			        	"Ore gun"},
    {ACT_ORE_CATCH,			  		    ACT_QUEUE_Ore_catch,			        "Ore catch"},
    {ACT_ORE_MIXER,			  		    ACT_QUEUE_Ore_mixer,			        "Ore mixer"},
    {ACT_ORE_SHOVEL,			  		ACT_QUEUE_Ore_shovel,			        "Ore shovel"},
    {ACT_ORE_SWEEPER,			  		ACT_QUEUE_Ore_sweeper,			        "Ore sweeper"},
    {ACT_CYLINDER_DISPENSER_LEFT,		ACT_QUEUE_Cylinder_dispenser_left,		"Cylinder dispenser left"},
    {ACT_CYLINDER_DISPENSER_RIGHT,		ACT_QUEUE_Cylinder_dispenser_right,		"Cylinder dispenser right"},
    {ACT_CYLINDER_ELEVATOR_LEFT,		ACT_QUEUE_Cylinder_elevator_left,		"Cylinder elevator left"},
    {ACT_CYLINDER_ELEVATOR_RIGHT,		ACT_QUEUE_Cylinder_elevator_right,		"Cylinder elevator right"},
    {ACT_CYLINDER_HATCH_LEFT,			ACT_QUEUE_Cylinder_hatch_left,			"Cylinder hatch left"},
    {ACT_CYLINDER_HATCH_RIGHT,			ACT_QUEUE_Cylinder_hatch_right,			"Cylinder hatch right"},
    {ACT_CYLINDER_PUSHER_LEFT,			ACT_QUEUE_Cylinder_pusher_left,			"Cylinder pusher left"},
    {ACT_CYLINDER_PUSHER_RIGHT,			ACT_QUEUE_Cylinder_pusher_right,		"Cylinder pusher right"},
    {ACT_CYLINDER_SLIDER_LEFT,			ACT_QUEUE_Cylinder_slider_left,			"Cylinder slider left"},
    {ACT_CYLINDER_SLIDER_RIGHT,			ACT_QUEUE_Cylinder_slider_right,		"Cylinder slider right"},
    {ACT_CYLINDER_SLOPE_LEFT,			ACT_QUEUE_Cylinder_slope_left,			"Cylinder slope left"},
    {ACT_CYLINDER_SLOPE_RIGHT,			ACT_QUEUE_Cylinder_slope_right,			"Cylinder slope right"},
    {ACT_CYLINDER_TURN_LEFT_ARM,		ACT_QUEUE_Cylinder_turn_left_arm,		"Cylinder turn left arm"},
    {ACT_CYLINDER_TURN_RIGHT_ARM,		ACT_QUEUE_Cylinder_turn_right_arm,		"Cylinder turn right arm"},
	{ACT_BEARING_BALL_WHEEL,			ACT_QUEUE_Bearing_ball_wheel,			"Bearing ball wheel"},
	{ACT_BIG_BALL_FRONT_LEFT,			ACT_QUEUE_Big_bearing_front_left,			"Bearing ball front left"},
	{ACT_BIG_BALL_FRONT_RIGHT,			ACT_QUEUE_Big_bearing_front_right,			"Bearing ball front right"},
	{ACT_BIG_BALL_BACK_LEFT,			ACT_QUEUE_Big_bearing_back_left,			"Bearing ball back left"},
	{ACT_BIG_BALL_BACK_RIGHT,			ACT_QUEUE_Big_bearing_back_right,			"Bearing ball back right"},
	{ACT_ORE_WALL,						ACT_QUEUE_Ore_wall,						"Ore wall"},
    {ACT_ORE_ROLLER_ARM,                ACT_QUEUE_Ore_roller_arm,               "Ore roller arm"},

	// Anne
	//{ACT_LEFT_ARM,					ACT_QUEUE_Left_arm,						"Left Arm"},
	{ACT_SMALL_BALL_FRONT_LEFT,			ACT_QUEUE_Small_bearing_front_left,			"Bearing ball front left"},
	{ACT_SMALL_BALL_FRONT_RIGHT,		ACT_QUEUE_Small_bearing_front_right,		"Bearing ball front right"},
	{ACT_SMALL_BALL_BACK_LEFT,			ACT_QUEUE_Small_bearing_back_left,			"Bearing ball back left"},
	{ACT_SMALL_BALL_BACK_RIGHT,			ACT_QUEUE_Small_bearing_back_right,			"Bearing ball back right"},

	// Mosfets actionneurs
	{ACT_POMPE_SLIDER_LEFT,			ACT_QUEUE_Pompe_act_slider_left,		"Mosfet_act_slider_left"},
	{ACT_POMPE_SLIDER_RIGHT,		ACT_QUEUE_Pompe_act_slider_right,		"Mosfet_act_slider_right"},
	{ACT_POMPE_ELEVATOR_LEFT,		ACT_QUEUE_Pompe_act_elevator_left,		"Mosfet_act_elevator_left"},
	{ACT_POMPE_ELEVATOR_RIGHT,		ACT_QUEUE_Pompe_act_elevator_right,		"Mosfet_act_elevator_right"},
	{ACT_MOSFET_5,					ACT_QUEUE_Mosfet_act_5,				     "Mosfet_act_5"},
	{ACT_MOSFET_6,					ACT_QUEUE_Mosfet_act_6,			         "Mosfet_act_6"},
	{ACT_MOSFET_7,					ACT_QUEUE_Mosfet_act_7,			         "Mosfet_act_7"},
	{ACT_MOSFET_8,					ACT_QUEUE_Mosfet_act_8,				     "Mosfet_act_8"},
	{ACT_MOSFET_MULTI,      		ACT_QUEUE_Mosfet_act_multi,		         "Mosfet_act_multi"},

	//Mosfets stratégie
	{STRAT_POMPE_SLIDER_LEFT,		ACT_QUEUE_Pompe_strat_slider_left,		"Mosfet_strat_slider_left"},
	{STRAT_POMPE_SLIDER_RIGHT,		ACT_QUEUE_Pompe_strat_slider_right,		"Mosfet_strat_slider_right"},
	{STRAT_POMPE_ELEVATOR_LEFT,		ACT_QUEUE_Pompe_strat_elevator_left,	"Mosfet_strat_elevator_left"},
	{STRAT_POMPE_ELEVATOR_RIGHT,	ACT_QUEUE_Pompe_strat_elevator_right,	"Mosfet_strat_elevator_right"},
	{STRAT_MOSFET_5,	           	ACT_QUEUE_Mosfet_strat_5,				"Mosfet_strat_5"},
	{STRAT_MOSFET_6,                ACT_QUEUE_Mosfet_strat_6,               "Mosfet_strat_6"},
	{STRAT_MOSFET_7,               	ACT_QUEUE_Mosfet_strat_7,               "Mosfet_strat_7"},
	{STRAT_MOSFET_8,                ACT_QUEUE_Mosfet_strat_8,		        "Mosfet_strat_8"},
	{STRAT_MOSFET_MULTI,	        ACT_QUEUE_Mosfet_strat_multi,			"Mosfet_strat_multi"},


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
//////////////// COMMON ////////////////
////////////////////////////////////////

bool_e ACT_config(Uint16 sid, Uint8 sub_act, act_config_e cmd, Uint16 value){
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

