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


#define ACT_SENSOR_ANSWER_TIMEOUT		500
#define ULU_TIME                        300
typedef enum{
	ACT_SENSOR_WAIT = 0,
	ACT_SENSOR_ABSENT,
	ACT_SENSOR_PRESENT
}act_sensor_e;

const act_link_SID_Queue_s act_link_SID_Queue[] = {
	{0,							NB_QUEUE,						""},
	// Holly
	{ACT_POP_COLLECT_LEFT,		ACT_QUEUE_Pop_collect_left,		"Pop collect left"},
	{ACT_POP_COLLECT_RIGHT,		ACT_QUEUE_Pop_collect_right,	"Pop collect right"},
	{ACT_POP_DROP_LEFT,			ACT_QUEUE_Pop_drop_left,		"Pop drop left"},
	{ACT_POP_DROP_RIGHT,		ACT_QUEUE_Pop_drop_right,		"Pop drop right"},
	{ACT_ELEVATOR,				ACT_QUEUE_Elevator,				"Elevator"},
	{ACT_PINCEMI_RIGHT,			ACT_QUEUE_PinceMi_right,		"PinceMi right"},
	{ACT_PINCEMI_LEFT,			ACT_QUEUE_PinceMi_left,			"PinceMi left"},
	{ACT_STOCK_RIGHT,			ACT_QUEUE_Stock_right,			"Stock right"},
	{ACT_STOCK_LEFT,			ACT_QUEUE_Stock_left,			"Stock left"},
	{ACT_CUP_NIPPER,			ACT_QUEUE_Cup_Nipper,			"Cup nipper"},
	{ACT_CUP_NIPPER_ELEVATOR,	ACT_QUEUE_Cup_Nipper_Elevator,	"Cup nipper Elevator"},
	{ACT_BACK_SPOT_RIGHT,		ACT_QUEUE_Back_spot_right,		"Back spot right"},
	{ACT_BACK_SPOT_LEFT,		ACT_QUEUE_Back_spot_left,		"Back spot left"},
	{ACT_SPOT_POMPE_LEFT,		ACT_QUEUE_Spot_pompe_left,		"Spot pompe left"},
	{ACT_SPOT_POMPE_RIGHT,		ACT_QUEUE_Spot_pompe_right,		"Spot pompe right"},
	{ACT_CARPET_LAUNCHER_RIGHT,	ACT_QUEUE_Carpet_launcher_right,"Carpet right"},
	{ACT_CARPET_LAUNCHER_LEFT,	ACT_QUEUE_Carpet_launcher_left,	"Carpet left"},
	{ACT_CLAP_HOLLY,			ACT_QUEUE_Clap_Holly,			"Clap"},

	// Wood
	{ACT_PINCE_GAUCHE,			ACT_QUEUE_Pince_Gauche,			"Pince gauche"},
	{ACT_PINCE_DROITE,			ACT_QUEUE_Pince_Droite,			"Pince droite"},
	{ACT_CLAP,					ACT_QUEUE_Clap,					"Clap"},
	{ACT_POP_DROP_LEFT_WOOD,	ACT_QUEUE_Pop_drop_left_Wood,	"Pop drop left"},
	{ACT_POP_DROP_RIGHT_WOOD,	ACT_QUEUE_Pop_drop_right_Wood,	"Pop drop right"},
	{ACT_POMPE_WOOD,			ACT_QUEUE_Pompe_wood,			"Pompe"}
};

static const Uint8 act_link_SID_Queue_size = sizeof(act_link_SID_Queue)/sizeof(act_link_SID_Queue_s);

volatile act_sensor_e sensor_answer[NB_ACT_SENSOR] = {0};

static error_e ACT_MAE_holly_spotix(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who);
static error_e ACT_MAE_holly_cup(ACT_MAE_holly_cup_e order);

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
//////////////// HOLLY /////////////////
////////////////////////////////////////


Uint8 ACT_search_link_SID_Queue(ACT_sid_e sid){
	Uint8 i;
	for(i=0;i<act_link_SID_Queue_size;i++){
		if(act_link_SID_Queue[i].sid == sid)
			return i;
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

	debug_printf("Pushing %s Run cmd (sid : %d   order : %d)\n", act_link_SID_Queue[i].name, sid, order);

	ACT_AVOIDANCE_new_classic_cmd(act_link_SID_Queue[i].queue_id, order);

	return ACT_push_operation(order, &args);
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

static error_e ACT_MAE_holly_spotix(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who){
#ifdef DEBUG_MAE_SPOTIX
	CREATE_MAE_WITH_VERBOSE(SM_ID_HOLLY_MANAGE_MAE_SPOTIX,
#else
	CREATE_MAE(
#endif
		INIT,
		COMPUTE_ORDER,
		FAIL_COMPUTE,

		// Open
		OPEN_ALL,
		WIN_OPEN_ALL,

		// Open
		OPEN_NIPPER,
		WIN_OPEN_NIPPER,

		// Take feet
		TAKE_FEET,
		CHECK_PRESENCE_FEET,
		WIN_TAKE,
		FAIL_TAKE,

		// Take ball
		TAKE_BALL,
		WIN_TAKE_BALL,
		FAIL_TAKE_BALL,

		// Open great
		MOVE_OTHER_NIPPER,
		OPEN_GREAT_FEET,
		WIN_OPEN_GREAT,
		FAIL_OPEN_GREAT,

		// Stock unlock
		UNLOCK_JUST_STOCK,
		WIN_UNLOCK_JUST_STOCK,
		FAIL_UNLOCK_JUST_STOCK,


		// Stock
		INIT_STOCK,
		STOCK_UNLOCK,
		ELEVATOR_UP,
		FAIL_ELEVATOR_UP_1,
		FAIL_ELEVATOR_UP_2,
		STOCK_LOCK,
		ELEVATOR_DOWN,
		NIPPER_OPEN,
		ELEVATOR_MID,
		STOCK_LOCK_FULL,
		WIN_STOCK,
		FAIL_STOCK,

		// Lock stock
		LOCK_STOCK,
		WIN_LOCK_STOCK,
		FAIL_LOCK_STOCK,


		// Go down (and release)
		RELEASE_NIPPER,
		ELEVATOR_GO_DOWN,
		WIN_GO_DOWN,
		FAIL_GO_DOWN,

		// Go up
		ELEVATOR_GO_UP,
		WIN_GO_UP,
		FAIL_GO_UP,

		// Go mid
		ELEVATOR_GO_JUST_MID,
		WIN_GO_JUST_MID,
		FAIL_GO_JUST_MID,

		// Go estrad
		ELEVATOR_GO_ESTRAD,
		WIN_GO_ESTRAD,
		FAIL_GO_ESTRAD,

		// Go mid low
		ELEVATOR_GO_MID_LOW,
		WIN_GO_MID_LOW,
		FAIL_GO_MID_LOW,

		// Go mid ++
		UNLOCK_SPOT_TO_MID,
		ELEVATOR_GO_MID,
		LOCK_SPOT_TO_MID,
		WIN_GO_MID,
		FAIL_GO_MID,

		// Release spot
		UNLOCK_SPOT,
		WIN_RELEASE,
		FAIL_RELEASE,

		//Unlock_Lock_Unlock
		ULU_UNLOCK_1,
		ULU_WAIT_1,
		ULU_LOCK,
		ULU_WAIT_2,
		ULU_UNLOCK_2,
		ULU_WAIT_3,
		ULU_WIN
	);

	static bool_e right_error = FALSE, left_error = FALSE;
	static enum state_e state1, state2, state3, state4;
	static bool_e check_presence = FALSE;
	error_e ret = IN_PROGRESS;

	switch(state){
		case INIT:
			right_error = FALSE;
			left_error = FALSE;
			state = COMPUTE_ORDER;
			break;

		case COMPUTE_ORDER:
			switch(order){

				case ACT_MAE_SPOTIX_OPEN_ALL:
					state = OPEN_ALL;
					break;

				case ACT_MAE_SPOTIX_OPEN_NIPPER:
					state = OPEN_NIPPER;
					break;

				case ACT_MAE_SPOTIX_TAKE_WITH_PRESENCE:
#ifdef DISABLED_PRESENCE_FEET_SENSOR
					check_presence = FALSE;
#else
					check_presence = TRUE;
#endif
					state = TAKE_FEET;
					break;

				case ACT_MAE_SPOTIX_TAKE_WITHOUT_PRESENCE:
					check_presence = FALSE;
					state = TAKE_FEET;
					break;

				case ACT_MAE_SPOTIX_TAKE_BALL:
					state = TAKE_BALL;
					break;

				case ACT_MAE_SPOTIX_OPEN_GREAT:
					if(who != ACT_MAE_SPOTIX_BOTH)
						state = MOVE_OTHER_NIPPER;
					else
						state = FAIL_COMPUTE;
					break;

				case ACT_MAE_SPOTIX_STOCK_AND_GO_DOWN_WITH_PRESENCE:
				case ACT_MAE_SPOTIX_STOCK_AND_STAY_WITH_PRESENCE:
#ifdef DISABLED_PRESENCE_FEET_SENSOR
					check_presence = FALSE;
#else
					check_presence = TRUE;
#endif
					state = INIT_STOCK;
					break;

				case ACT_MAE_SPOTIX_STOCK_AND_GO_DOWN_WITHOUT_PRESENCE:
				case ACT_MAE_SPOTIX_STOCK_AND_STAY_WITHOUT_PRESENCE:
					check_presence = FALSE;
					state = INIT_STOCK;
					break;

				case ACT_MAE_SPOTIX_RELEASE_NIPPER_AND_GO_DOWN:
					state = RELEASE_NIPPER;
					break;

				case ACT_MAE_SPOTIX_GO_DOWN:
					state = ELEVATOR_GO_DOWN;
					break;

				case ACT_MAE_SPOTIX_GO_UP:
					state = ELEVATOR_GO_UP;
					break;

				case ACT_MAE_SPOTIX_GO_MID:
					state = ELEVATOR_GO_JUST_MID;
					break;


				case ACT_MAE_SPOTIX_GO_MID_IN_OPENING_STOCK:
					state = UNLOCK_SPOT_TO_MID;
					break;

				case ACT_MAE_SPOTIX_RELEASE_STOCK:
					state = UNLOCK_SPOT;
					break;

				case ACT_MAE_SPOTIX_GO_DISPOSE:
					state = ELEVATOR_GO_ESTRAD;
					break;

				case ACT_MAE_SPOTIX_GO_MID_LOW:
					state = ELEVATOR_GO_MID_LOW;
				break;

				case ACT_MAE_SPOTIX_UNLOCK_STOCK:
					state = UNLOCK_JUST_STOCK;
					break;

				case ACT_MAE_SPOTIX_LOCK_STOCK:
					state = LOCK_STOCK;
					break;

				case ACT_MAE_SPOTIX_ULU:
					state = ULU_UNLOCK_1;
					break;

				default :
					state = FAIL_COMPUTE;
					break;
			}
			break;

		case FAIL_COMPUTE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;


//--------------------------------------- Open all

		case OPEN_ALL:
			if(entrance){
				state1 = state2 = state3 = state4 = OPEN_ALL;
				if(who != ACT_MAE_SPOTIX_LEFT){
					ACT_push_order(ACT_STOCK_RIGHT,  ACT_STOCK_RIGHT_OPEN);
					ACT_push_order(ACT_PINCEMI_RIGHT,  ACT_PINCEMI_RIGHT_OPEN);
				}
				if(who != ACT_MAE_SPOTIX_RIGHT){
					ACT_push_order(ACT_STOCK_LEFT,  ACT_STOCK_LEFT_OPEN);
					ACT_push_order(ACT_PINCEMI_LEFT,  ACT_PINCEMI_LEFT_OPEN);
				}
			}

			if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
				if(state1 == OPEN_ALL)
					state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_OPEN_ALL, WIN_OPEN_ALL);
				if(state2 == OPEN_ALL)
					state2 = check_act_status(ACT_QUEUE_PinceMi_right, state, WIN_OPEN_ALL, WIN_OPEN_ALL);
			}

			if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
				if(state3 == OPEN_ALL)
					state3 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_OPEN_ALL, WIN_OPEN_ALL);
				if(state4 == OPEN_ALL)
					state4 = check_act_status(ACT_QUEUE_PinceMi_left, state, WIN_OPEN_ALL, WIN_OPEN_ALL);
			}

			if((who == ACT_MAE_SPOTIX_BOTH && (state1 != OPEN_ALL && state2 != OPEN_ALL) && state3 != OPEN_ALL && state4 != OPEN_ALL)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != OPEN_ALL && state2 != OPEN_ALL)
					|| (who == ACT_MAE_SPOTIX_LEFT && state3 != OPEN_ALL && state4 != OPEN_ALL))
				state = WIN_OPEN_ALL;
			break;

		case WIN_OPEN_ALL:
			RESET_MAE();
			ret = END_OK;
			break;

//--------------------------------------- Open Nipper
		case OPEN_NIPPER:
			if(entrance){
				state1 = state2 = OPEN_NIPPER;
				if(who != ACT_MAE_SPOTIX_LEFT){
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);
				}
				if(who != ACT_MAE_SPOTIX_RIGHT){
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);
				}
			}

			if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
				if(state1 == OPEN_NIPPER)
					state1 = check_act_status(ACT_QUEUE_PinceMi_right, state,WIN_OPEN_NIPPER, WIN_OPEN_NIPPER);
			}

			if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
				if(state2 == OPEN_NIPPER)
					state2 = check_act_status(ACT_QUEUE_PinceMi_left, state, WIN_OPEN_NIPPER, WIN_OPEN_NIPPER);
			}

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != OPEN_NIPPER && state2 != OPEN_NIPPER)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != OPEN_NIPPER )
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != OPEN_NIPPER ))
				state = WIN_OPEN_NIPPER;
			break;

		case WIN_OPEN_NIPPER:
			RESET_MAE();
			ret = END_OK;
			break;

//--------------------------------------- Take feet

		case TAKE_FEET:
			if(entrance){
				state1 = state2 = TAKE_FEET;
				if(who != ACT_MAE_SPOTIX_LEFT)
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_LOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_LOCK);
			}

			if(state1 == TAKE_FEET && who != ACT_MAE_SPOTIX_LEFT)
				state1 = check_act_status(ACT_QUEUE_PinceMi_right, state, CHECK_PRESENCE_FEET, FAIL_TAKE);
			if(state2 == TAKE_FEET && who != ACT_MAE_SPOTIX_RIGHT)
				state2 = check_act_status(ACT_QUEUE_PinceMi_left, state, CHECK_PRESENCE_FEET, FAIL_TAKE);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != TAKE_FEET && state2 != TAKE_FEET)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != TAKE_FEET)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != TAKE_FEET))
				state = CHECK_PRESENCE_FEET;

			break;

		case CHECK_PRESENCE_FEET:
#ifdef ROBOT_VIRTUEL_PARFAIT
			state = WIN_TAKE;
#else
			if(check_presence){
				if(who != ACT_MAE_SPOTIX_RIGHT && !PRESENCE_PIED_PINCE_GAUCHE_HOLLY)
					left_error = TRUE;
				if(who != ACT_MAE_SPOTIX_LEFT && !PRESENCE_PIED_PINCE_DROITE_HOLLY)
					right_error = TRUE;
				debug_printf("pied_gauche=%d   pied_gauche=%d\n", PRESENCE_PIED_PINCE_GAUCHE_HOLLY, PRESENCE_PIED_PINCE_DROITE_HOLLY);
				debug_printf("left_error=%d    right_error=%d\n", left_error, right_error);
				if(left_error || right_error)
					state = FAIL_TAKE;
				else
					state = WIN_TAKE;
			}else{
				state = WIN_TAKE;
			}
#endif
		break;

		case WIN_TAKE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_TAKE:
			if(left_error)
				ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);

			if(right_error)
				ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);

			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Take ball

		case TAKE_BALL:
				if(entrance){
					state1 = state2 = TAKE_BALL;
					if(who != ACT_MAE_SPOTIX_LEFT)
						ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_LOCK_BALL);
					if(who != ACT_MAE_SPOTIX_RIGHT)
						ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_LOCK_BALL);
				}

				if(state1 == TAKE_BALL && who != ACT_MAE_SPOTIX_LEFT)
					state1 = check_act_status(ACT_QUEUE_PinceMi_right, state, WIN_TAKE_BALL, FAIL_TAKE_BALL);
				if(state2 == TAKE_BALL && who != ACT_MAE_SPOTIX_RIGHT)
					state2 = check_act_status(ACT_QUEUE_PinceMi_left, state, WIN_TAKE_BALL, FAIL_TAKE_BALL);

				if((who == ACT_MAE_SPOTIX_BOTH && state1 != TAKE_BALL && state2 != TAKE_BALL)
						|| (who == ACT_MAE_SPOTIX_LEFT && state2 != TAKE_BALL)
						|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != TAKE_BALL))
					state = WIN_TAKE_BALL;

				break;

			case WIN_TAKE_BALL:
				RESET_MAE();
				ret = END_OK;
				break;

			case FAIL_TAKE_BALL:
				if(left_error)
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);

				if(right_error)
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);

				RESET_MAE();
				ret = NOT_HANDLED;
				break;


//--------------------------------------- Take great feet

		case MOVE_OTHER_NIPPER:
			if(entrance){
				if(who == ACT_MAE_SPOTIX_LEFT){
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_CLOSE_INNER);
				}if(who == ACT_MAE_SPOTIX_RIGHT){
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_CLOSE_INNER);
				}
			}

			if(who == ACT_MAE_SPOTIX_LEFT){
				state = check_act_status(ACT_QUEUE_PinceMi_right, state, OPEN_GREAT_FEET, FAIL_OPEN_GREAT);
			}
			if(who == ACT_MAE_SPOTIX_RIGHT){
				state = check_act_status(ACT_QUEUE_PinceMi_left, state, OPEN_GREAT_FEET, FAIL_OPEN_GREAT);
			}
			break;

		case OPEN_GREAT_FEET:
			if(entrance){
				if(who == ACT_MAE_SPOTIX_LEFT){
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN_GREAT);
				}if(who == ACT_MAE_SPOTIX_RIGHT){
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN_GREAT);
				}
			}

			if(who == ACT_MAE_SPOTIX_LEFT){
				state = check_act_status(ACT_QUEUE_PinceMi_right, state, WIN_OPEN_GREAT, FAIL_OPEN_GREAT);
			}
			if(who == ACT_MAE_SPOTIX_RIGHT){
				state = check_act_status(ACT_QUEUE_PinceMi_left, state, WIN_OPEN_GREAT, FAIL_OPEN_GREAT);
			}
			break;

		case WIN_OPEN_GREAT:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_OPEN_GREAT:
			if(entrance){
				ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);
				ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);
			}
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Stock unlock
		case UNLOCK_JUST_STOCK:
			if(entrance){
				state1 = state2 = UNLOCK_JUST_STOCK;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_UNLOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_UNLOCK);
			}
			if(state1 == UNLOCK_JUST_STOCK && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_UNLOCK_JUST_STOCK, FAIL_UNLOCK_JUST_STOCK);
			if(state2 == UNLOCK_JUST_STOCK && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state2 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_UNLOCK_JUST_STOCK, FAIL_UNLOCK_JUST_STOCK);

			if((state1 != UNLOCK_JUST_STOCK && state2 != UNLOCK_JUST_STOCK)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != UNLOCK_JUST_STOCK)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != UNLOCK_JUST_STOCK))
				state = WIN_UNLOCK_JUST_STOCK;
			break;

		case WIN_UNLOCK_JUST_STOCK:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_UNLOCK_JUST_STOCK:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Lock stock
		case LOCK_STOCK:
			if(entrance){
				state1 = state2 = LOCK_STOCK;
				if(who != ACT_MAE_SPOTIX_LEFT)
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_LOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_LOCK);
			}

			if(state1 == LOCK_STOCK && who != ACT_MAE_SPOTIX_LEFT)
				state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_LOCK_STOCK, FAIL_LOCK_STOCK);
			if(state2 == LOCK_STOCK && who != ACT_MAE_SPOTIX_RIGHT)
				state2 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_LOCK_STOCK, FAIL_LOCK_STOCK);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != LOCK_STOCK && state2 != LOCK_STOCK)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != LOCK_STOCK)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != LOCK_STOCK))
				state = WIN_LOCK_STOCK;

			break;

		case WIN_LOCK_STOCK:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_LOCK_STOCK:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;


//--------------------------------------- Stock

		case INIT_STOCK:
#ifndef ROBOT_VIRTUEL_PARFAIT
			if(who != ACT_MAE_SPOTIX_RIGHT && (!PRESENCE_PIED_PINCE_GAUCHE_HOLLY && check_presence))
				left_error = TRUE;
			if(who != ACT_MAE_SPOTIX_LEFT && (!PRESENCE_PIED_PINCE_DROITE_HOLLY && check_presence))
				right_error = TRUE;
#endif
			if(left_error && right_error)
				state = FAIL_STOCK;
			else
				state = STOCK_UNLOCK;
			break;

		case STOCK_UNLOCK:
			if(entrance){
				state1 = state2 = STOCK_UNLOCK;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_UNLOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_UNLOCK);
			}
			if(state1 == STOCK_UNLOCK && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state1 = check_act_status(ACT_QUEUE_Stock_right, state, ELEVATOR_UP, FAIL_TAKE);
			if(state2 == STOCK_UNLOCK && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state2 = check_act_status(ACT_QUEUE_Stock_left, state, ELEVATOR_UP, FAIL_TAKE);

			if((who == ACT_MAE_SPOTIX_BOTH && (state1 != STOCK_UNLOCK || right_error) && (state2 != STOCK_UNLOCK || left_error))
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != STOCK_UNLOCK)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != STOCK_UNLOCK)){
				if( (ELEMENTS_get_holly_left_spot_level() >= 3 && who != ACT_MAE_SPOTIX_RIGHT) || (ELEMENTS_get_holly_right_spot_level() >= 3 && who != ACT_MAE_SPOTIX_LEFT ))
					state = ELEVATOR_MID;
				else
					state = ELEVATOR_UP;
			}

			break;

		// Question gestion d'erreur sur le fait de fail l'unlock ?

		case ELEVATOR_UP:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_TOP);
			state = check_act_status(ACT_QUEUE_Elevator, state, STOCK_LOCK, FAIL_STOCK);
			break;

		case FAIL_ELEVATOR_UP_1:
			if(entrance){
				state1 = state2 = FAIL_ELEVATOR_UP_1;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_LOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_LOCK);
			}
			if(state1 == FAIL_ELEVATOR_UP_1 && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state1 = check_act_status(ACT_QUEUE_Stock_left, state, FAIL_ELEVATOR_UP_2, FAIL_ELEVATOR_UP_2);
			if(state2 == FAIL_ELEVATOR_UP_1 && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state2 = check_act_status(ACT_QUEUE_Stock_right, state, FAIL_ELEVATOR_UP_2, FAIL_ELEVATOR_UP_2);

			if((who == ACT_MAE_SPOTIX_BOTH && (state1 != FAIL_ELEVATOR_UP_1 || left_error) && (state2 != FAIL_ELEVATOR_UP_1 || right_error))
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != FAIL_ELEVATOR_UP_1)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state2 != FAIL_ELEVATOR_UP_1))
				state = FAIL_ELEVATOR_UP_2;
			break;

		case FAIL_ELEVATOR_UP_2:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_BOT);
			state = check_act_status(ACT_QUEUE_Elevator, state, FAIL_STOCK, FAIL_STOCK);
			break;

		case STOCK_LOCK:
			if(entrance){
				state1 = state2 = STOCK_LOCK;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_LOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_LOCK);
			}
			if(state1 == STOCK_LOCK && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state1 = check_act_status(ACT_QUEUE_Stock_left, state, NIPPER_OPEN, FAIL_TAKE);
			if(state2 == STOCK_LOCK && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state2 = check_act_status(ACT_QUEUE_Stock_right, state, NIPPER_OPEN, FAIL_TAKE);

			if((who == ACT_MAE_SPOTIX_BOTH && (state1 != STOCK_LOCK || left_error) && (state2 != STOCK_LOCK || right_error))
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != STOCK_LOCK)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state2 != STOCK_LOCK)){

				if(order == ACT_MAE_SPOTIX_STOCK_AND_STAY_WITH_PRESENCE || order == ACT_MAE_SPOTIX_STOCK_AND_STAY_WITHOUT_PRESENCE)
					state = WIN_STOCK;
				else
					state = NIPPER_OPEN;
			}
			break;

		// Question gestion d'erreur sur le fait de fail le lock ?

		case NIPPER_OPEN:
			if(entrance){
				state1 = state2 = NIPPER_OPEN;
				if(who != ACT_MAE_SPOTIX_LEFT)
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);
			}
			if(state1 == NIPPER_OPEN && who != ACT_MAE_SPOTIX_LEFT)
				state1 = check_act_status(ACT_QUEUE_PinceMi_right, state, ELEVATOR_DOWN, ELEVATOR_DOWN);
			if(state2 == NIPPER_OPEN && who != ACT_MAE_SPOTIX_RIGHT)
				state2 = check_act_status(ACT_QUEUE_PinceMi_left, state, ELEVATOR_DOWN, ELEVATOR_DOWN);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != NIPPER_OPEN && state2 != NIPPER_OPEN)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != NIPPER_OPEN)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != NIPPER_OPEN))
				state = ELEVATOR_DOWN;
			break;

		case ELEVATOR_DOWN:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_BOT);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_STOCK, FAIL_STOCK);
			break;

		case ELEVATOR_MID:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_MID_LOW);
			state = check_act_status(ACT_QUEUE_Elevator, state, STOCK_LOCK_FULL, FAIL_STOCK);
			break;

		case STOCK_LOCK_FULL:
			if(entrance){
				state1 = state2 = STOCK_LOCK_FULL;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_LOCK);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_LOCK);
			}
			if(state1 == STOCK_LOCK_FULL && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state1 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_STOCK, FAIL_TAKE);
			if(state2 == STOCK_LOCK_FULL && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state2 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_STOCK, FAIL_TAKE);

			if((who == ACT_MAE_SPOTIX_BOTH && (state1 != STOCK_LOCK_FULL || left_error) && (state2 != STOCK_LOCK_FULL || right_error))
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != STOCK_LOCK_FULL)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state2 != STOCK_LOCK_FULL)){
				state = WIN_STOCK;
			}
			break;

		case WIN_STOCK:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_STOCK:
			if(left_error)
				ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);

			if(right_error)
				ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);

			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go down and release
		case RELEASE_NIPPER:
			if(entrance){
				state1 = state2 = RELEASE_NIPPER;
				if(who != ACT_MAE_SPOTIX_LEFT)
					ACT_push_order(ACT_PINCEMI_RIGHT, ACT_PINCEMI_RIGHT_OPEN);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_push_order(ACT_PINCEMI_LEFT, ACT_PINCEMI_LEFT_OPEN);
			}
			if(state1 == RELEASE_NIPPER && who != ACT_MAE_SPOTIX_LEFT)
				state1 = check_act_status(ACT_QUEUE_PinceMi_right, state, ELEVATOR_GO_DOWN, ELEVATOR_GO_DOWN);
			if(state2 == RELEASE_NIPPER && who != ACT_MAE_SPOTIX_RIGHT)
				state2 = check_act_status(ACT_QUEUE_PinceMi_left, state, ELEVATOR_GO_DOWN, ELEVATOR_GO_DOWN);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != RELEASE_NIPPER && state2 != RELEASE_NIPPER)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != RELEASE_NIPPER)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != RELEASE_NIPPER))
				state = ELEVATOR_GO_DOWN;
			break;

		case ELEVATOR_GO_DOWN:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_BOT);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_GO_DOWN, FAIL_GO_DOWN);
			break;

		case WIN_GO_DOWN:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_GO_DOWN:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go up

		case ELEVATOR_GO_UP:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_TOP);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_GO_UP, FAIL_GO_UP);
			break;

		case WIN_GO_UP:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_GO_UP:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go mid low

		case ELEVATOR_GO_MID_LOW:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_MID_LOW);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_GO_MID_LOW, FAIL_GO_MID_LOW);
			break;

		case WIN_GO_MID_LOW:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_GO_MID_LOW:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go estrad

		case ELEVATOR_GO_ESTRAD:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_ESTRAD);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_GO_ESTRAD, FAIL_GO_ESTRAD);
			break;

		case WIN_GO_ESTRAD:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_GO_ESTRAD:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go mid

		case ELEVATOR_GO_JUST_MID:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_MID);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_GO_JUST_MID, FAIL_GO_JUST_MID);
			break;

		case WIN_GO_JUST_MID:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_GO_JUST_MID:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go middle

		case UNLOCK_SPOT_TO_MID:
			if(order == ACT_MAE_SPOTIX_GO_MID){
				if(entrance){
					state1 = state2 = UNLOCK_SPOT_TO_MID;
					if(who != ACT_MAE_SPOTIX_LEFT){
						ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_UNLOCK);
					}
					if(who != ACT_MAE_SPOTIX_RIGHT){
						ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_UNLOCK);
					}
				}
				if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
					if(state1 == UNLOCK_SPOT_TO_MID)
						state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_GO_MID, FAIL_GO_MID);
				}

				if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
					if(state2 == UNLOCK_SPOT_TO_MID)
						state2 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_GO_MID, FAIL_GO_MID);
				}

				if((who == ACT_MAE_SPOTIX_BOTH && state1 != UNLOCK_SPOT_TO_MID && state2 != UNLOCK_SPOT_TO_MID)
						|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != UNLOCK_SPOT_TO_MID)
						|| (who == ACT_MAE_SPOTIX_LEFT && state2 != UNLOCK_SPOT_TO_MID))
					state = ELEVATOR_GO_MID;
			}else{
				state = ELEVATOR_GO_MID;
			}
			break;

		case ELEVATOR_GO_MID:
			if(entrance)
				ACT_push_order(ACT_ELEVATOR, ACT_ELEVATOR_MID);
			state = check_act_status(ACT_QUEUE_Elevator, state,(order == ACT_MAE_SPOTIX_GO_MID)? WIN_GO_MID:LOCK_SPOT_TO_MID, FAIL_GO_MID);
			break;

		case LOCK_SPOT_TO_MID:
			if(entrance){
					state1 = state2 = LOCK_SPOT_TO_MID;
					if(who != ACT_MAE_SPOTIX_LEFT){
						ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_LOCK);
					}
					if(who != ACT_MAE_SPOTIX_RIGHT){
						ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_LOCK);
					}
				}
				if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
					if(state1 == LOCK_SPOT_TO_MID)
						state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_GO_MID, FAIL_GO_MID);
				}

				if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
					if(state2 == LOCK_SPOT_TO_MID)
						state2 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_GO_MID, FAIL_GO_MID);
				}

				if((who == ACT_MAE_SPOTIX_BOTH && state1 != LOCK_SPOT_TO_MID && state2 != LOCK_SPOT_TO_MID)
						|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != LOCK_SPOT_TO_MID)
						|| (who == ACT_MAE_SPOTIX_LEFT && state2 != LOCK_SPOT_TO_MID))
					state = WIN_GO_MID;
			break;

		case WIN_GO_MID:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_GO_MID:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;


//--------------------------------------- Release

		case UNLOCK_SPOT:
			if(entrance){
				state1 = state2 = UNLOCK_SPOT;
				if(who != ACT_MAE_SPOTIX_LEFT){
					ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_OPEN);
				}
				if(who != ACT_MAE_SPOTIX_RIGHT){
					ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_OPEN);
				}
			}
			if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
				if(state1 == UNLOCK_SPOT)
					state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_RELEASE, FAIL_RELEASE);
			}

			if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
				if(state2 == UNLOCK_SPOT)
					state2 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_RELEASE, FAIL_RELEASE);
			}

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != UNLOCK_SPOT && state2 != UNLOCK_SPOT)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != UNLOCK_SPOT)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != UNLOCK_SPOT))
				state = WIN_RELEASE;
			break;

		case WIN_RELEASE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_RELEASE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//---------------------------UNLOCK_LOCK_UNLOCK------------------------------------------------------------
		case ULU_UNLOCK_1:
			if(who != ACT_MAE_SPOTIX_LEFT){
				ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_UNLOCK);
			}
			if(who != ACT_MAE_SPOTIX_RIGHT){
				ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_UNLOCK);
			}
			state=ULU_WAIT_1;
			break;

		case ULU_WAIT_1:
			state = wait_time(ULU_TIME,ULU_WAIT_1,ULU_LOCK);
			break;

		case ULU_LOCK:
			if(who != ACT_MAE_SPOTIX_LEFT){
				ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_LOCK);
			}
			if(who != ACT_MAE_SPOTIX_RIGHT){
				ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_LOCK);
			}
			state=ULU_WAIT_2;
			break;

		case ULU_WAIT_2:
			state = wait_time(ULU_TIME,ULU_WAIT_2,ULU_UNLOCK_2);
			break;

		case ULU_UNLOCK_2:
			if(who != ACT_MAE_SPOTIX_LEFT){
				ACT_push_order(ACT_STOCK_RIGHT, ACT_STOCK_RIGHT_UNLOCK);
			}
			if(who != ACT_MAE_SPOTIX_RIGHT){
				ACT_push_order(ACT_STOCK_LEFT, ACT_STOCK_LEFT_UNLOCK);
			}
			state=ULU_WAIT_3;
			break;
		case ULU_WAIT_3:
			state = wait_time(ULU_TIME,ULU_WAIT_3,ULU_WIN);
			break;

		case ULU_WIN:
			RESET_MAE();
			ret = END_OK;
			break;
	}


	return ret;
}



typedef struct{
	ACT_MAE_holly_spotix_e order;
	ACT_MAE_holly_spotix_side_e who;
	Uint16 order_id;
}spotix_order_queue_t;

volatile static spotix_order_queue_t spotix_order_queue[10];
volatile static error_e last_spotix_error;

volatile static Uint8 spotix_read;
volatile static Uint8 spotix_write;

volatile static spotix_order_id_t spotix_order_counter = 0;
volatile static spotix_order_id_t last_spotix_order_id = -1;

error_e ACT_MAE_holly_spotix_bloquing(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who){
	static bool_e entrance = TRUE;
	static spotix_order_id_t spotix_order_id;

	if(entrance)
		spotix_order_id = ACT_MAE_holly_spotix_do_order(order, who);

	if(spotix_order_id == -1){
		entrance = TRUE;
		return NOT_HANDLED;
	}

	if(ACT_MAE_holly_spotix_wait_end_order(spotix_order_id)){
		entrance = TRUE;
		return ACT_holly_spotix_get_last_error();
	}

	entrance = FALSE;
	return IN_PROGRESS;
}

bool_e ACT_MAE_holly_spotix_wait_end_order(spotix_order_id_t id){
	if(last_spotix_order_id >= id)
		return TRUE;
	else
		return FALSE;
}

spotix_order_id_t ACT_MAE_holly_spotix_do_order(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who){

	if(((spotix_write + 1) % 10) == spotix_read){ // Buffer plein
		error_printf("ACT_MAE_holly_spotix_do_order : buffer full\n");
		return -1;
	}

	spotix_order_queue[spotix_write].order = order;
	spotix_order_queue[spotix_write].who = who;
	spotix_order_queue[spotix_write].order_id = spotix_order_counter;

	spotix_write = (spotix_write + 1) % 10;
	return spotix_order_counter++;
}


void ACT_MAE_holly_spotix_process_main(){
#ifdef DEBUG_MAE_SPOTIX
	CREATE_MAE_WITH_VERBOSE(SM_ID_HOLLY_MANAGE_MAE_SPOTIX,
#else
	CREATE_MAE(
#endif
		INIT,
		WAIT_ORDER,
		DO_ORDER
	);

	error_e result_sub;

	switch(state){
		case INIT:
			spotix_read = 0;
			spotix_write = 0;
			state = WAIT_ORDER;
			break;

		case WAIT_ORDER:
			if(spotix_read != spotix_write)
				state = DO_ORDER;
			break;

		case DO_ORDER:
			result_sub = ACT_MAE_holly_spotix(spotix_order_queue[spotix_read].order, spotix_order_queue[spotix_read].who);

			if(result_sub != IN_PROGRESS){
				state = WAIT_ORDER;
				last_spotix_error = result_sub;
				last_spotix_order_id = spotix_order_queue[spotix_read].order_id;
				spotix_read = (spotix_read + 1) % 10;
			}
			break;

	}
}

error_e ACT_holly_spotix_get_last_error(){
	return last_spotix_error;
}


static error_e ACT_MAE_holly_cup(ACT_MAE_holly_cup_e order){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		COMPUTE_ORDER,
		FAIL_COMPUTE,

		// Cup take
		TAKE_CUP,
		WIN_TAKE,
		FAIL_TAKE,

		// Cup release
		RELEASE_CUP,
		WIN_RELEASE,
		FAIL_RELEASE,

		// Cup up
		UP_CUP,
		WIN_UP,
		FAIL_UP,

		// Cup mid
		MID_CUP,
		WIN_MID,
		FAIL_MID,

		// Cup down
		DOWN_CUP,
		WIN_DOWN,
		FAIL_DOWN
	);

	error_e ret = IN_PROGRESS;

	switch(state){
		case INIT:
			state = COMPUTE_ORDER;
			break;

		case COMPUTE_ORDER:
			switch(order){

				case ACT_MAE_CUP_TAKE:
					state = TAKE_CUP;
					break;

				case ACT_MAE_CUP_RELEASE:
					state = RELEASE_CUP;
					break;

				case ACT_MAE_CUP_UP:
					state = UP_CUP;
					break;

				case ACT_MAE_CUP_DOWN:
					state = DOWN_CUP;
					break;


				default :
					state = FAIL_COMPUTE;
					break;
			}
			break;

		case FAIL_COMPUTE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//----------------------------------------- Take cup
		case TAKE_CUP:
			if(entrance)
				ACT_push_order_with_param(ACT_SPOT_POMPE_RIGHT, ACT_SPOT_POMPE_RIGHT_REVERSE, 100);
			state = check_act_status(ACT_QUEUE_Spot_pompe_right, state, WIN_TAKE, FAIL_TAKE);
			break;

		case WIN_TAKE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_TAKE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;


//----------------------------------------- Release cup
		case RELEASE_CUP:
			if(entrance)
				ACT_push_order(ACT_SPOT_POMPE_RIGHT, ACT_SPOT_POMPE_RIGHT_STOP);
			state = check_act_status(ACT_QUEUE_Spot_pompe_right, state, WIN_RELEASE, FAIL_RELEASE);
			break;

		case WIN_RELEASE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_RELEASE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//----------------------------------------- Up
		case UP_CUP:
			if(entrance)
				ACT_push_order(ACT_CUP_NIPPER_ELEVATOR, ACT_CUP_NIPPER_ELEVATOR_UP);
			state = check_act_status(ACT_QUEUE_Cup_Nipper_Elevator, state, WIN_UP, FAIL_UP);
		break;

		case WIN_UP:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_UP:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//----------------------------------------- Mid
		case MID_CUP:
			if(entrance)
				ACT_push_order(ACT_CUP_NIPPER_ELEVATOR, ACT_CUP_NIPPER_ELEVATOR_MID);
			state = check_act_status(ACT_QUEUE_Cup_Nipper_Elevator, state, WIN_MID, FAIL_MID);
		break;

		case WIN_MID:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_MID:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//----------------------------------------- Down
		case DOWN_CUP:
			if(entrance)
				ACT_push_order(ACT_CUP_NIPPER_ELEVATOR, ACT_CUP_NIPPER_ELEVATOR_IDLE);
			state = check_act_status(ACT_QUEUE_Cup_Nipper_Elevator, state, WIN_DOWN, FAIL_DOWN);
		break;

		case WIN_DOWN:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_DOWN:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;
	}
	return ret;
}


typedef struct{
	ACT_MAE_holly_cup_e order;
	Uint16 order_id;
}cup_order_queue_t;

volatile static cup_order_queue_t cup_order_queue[10];
volatile static error_e last_cup_error;

volatile static Uint8 cup_read;
volatile static Uint8 cup_write;

volatile static spotix_order_id_t cup_order_counter = 0;
volatile static spotix_order_id_t last_order_cup_id = -1;

error_e ACT_MAE_holly_cup_bloquing(ACT_MAE_holly_cup_e order){
	static bool_e entrance = TRUE;
	static cup_order_id_t cup_order_id;

	if(entrance)
		cup_order_id = ACT_MAE_holly_cup_do_order(order);

	if(cup_order_id == -1){
		entrance = TRUE;
		return NOT_HANDLED;
	}

	if(ACT_MAE_holly_cup_wait_end_order(cup_order_id)){
		entrance = TRUE;
		return ACT_holly_cup_get_last_error();
	}

	entrance = FALSE;
	return IN_PROGRESS;
}

bool_e ACT_MAE_holly_cup_wait_end_order(cup_order_id_t id){
	if(last_order_cup_id >= id)
		return TRUE;
	else
		return FALSE;
}

cup_order_id_t ACT_MAE_holly_cup_do_order(ACT_MAE_holly_cup_e order){

	if(((cup_write + 1) % 10) == cup_read){ // Buffer plein
		error_printf("ACT_MAE_holly_cup_do_order : buffer full\n");
		return -1;
	}

	cup_order_queue[cup_write].order = order;
	cup_order_queue[cup_write].order_id = cup_order_counter;

	cup_write = (cup_write + 1) % 10;
	return cup_order_counter++;
}


void ACT_MAE_holly_cup_process_main(){
#ifdef DEBUG_MAE_SPOTIX
	CREATE_MAE_WITH_VERBOSE(SM_ID_HOLLY_MANAGE_MAE_CUP,
#else
	CREATE_MAE(
#endif
		INIT,
		WAIT_ORDER,
		DO_ORDER
	);

	error_e result_sub;

	switch(state){
		case INIT:
			cup_read = 0;
			cup_write = 0;
			state = WAIT_ORDER;
			break;

		case WAIT_ORDER:
			if(cup_read != cup_write)
				state = DO_ORDER;
			break;

		case DO_ORDER:
			result_sub = ACT_MAE_holly_cup(cup_order_queue[cup_read].order);

			if(result_sub != IN_PROGRESS){
				state = WAIT_ORDER;
				last_cup_error = result_sub;
				last_order_cup_id = cup_order_queue[cup_read].order_id;
				cup_read = (cup_read + 1) % 10;
			}
			break;

	}
}

error_e ACT_holly_cup_get_last_error(){
	return last_cup_error;
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
