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
#include "state_machine_helper.h"

#define ACT_SENSOR_ANSWER_TIMEOUT		500

typedef enum{
	ACT_SENSOR_WAIT,
	ACT_SENSOR_ABSENT,
	ACT_SENSOR_PRESENT
}act_sensor_e;

typedef struct{
	ACT_MAE_holly_spotix_e order;
	ACT_MAE_holly_spotix_side_e who;
}act_mae_real_time_t;

static volatile act_sensor_e gobelet_right_wood_answer = ACT_SENSOR_WAIT;
static volatile act_sensor_e gobelet_left_wood_answer = ACT_SENSOR_WAIT;
static volatile act_sensor_e gobelet_front_wood_answer = ACT_SENSOR_WAIT;
static volatile act_sensor_e gobelet_holly_answer = ACT_SENSOR_WAIT;

static volatile act_mae_real_time_t act_mae_real_time = {END_OK, FALSE};
static volatile FIFO_t FIFO_MAE_spotix;

static error_e ACT_MAE_holly_spotix(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who);

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

/*bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_TORCH_LOCKER, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_TORCH_LOCKER, ACT_TORCH_LOCKER_STOP);

	debug_printf("Pushing torch locker Run cmd\n");

*******   ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_TORCH_LOCKER, cmd); ***    <----  Prend en compte la position de l'actionneur pour l'évitement

	return ACT_push_operation(ACT_QUEUE_Torch_locker, &args);
}*/
bool_e ACT_pop_collect_left(ACT_pop_collect_left_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_COLLECT_LEFT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_COLLECT_LEFT,  ACT_POP_COLLECT_LEFT_STOP);

	debug_printf("Pushing pop collect left Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_POP_COLLECT_LEFT, cmd);

	return ACT_push_operation(ACT_QUEUE_Pop_collect_left, &args);
}

bool_e ACT_pop_collect_right(ACT_pop_collect_right_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_COLLECT_RIGHT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_COLLECT_RIGHT,  ACT_POP_COLLECT_RIGHT_STOP);

	debug_printf("Pushing pop collect right Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_POP_COLLECT_RIGHT, cmd);

	return ACT_push_operation(ACT_QUEUE_Pop_collect_right, &args);
}

bool_e ACT_pop_drop_left(ACT_pop_drop_left_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_DROP_LEFT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_DROP_LEFT,  ACT_POP_DROP_LEFT_STOP);

	debug_printf("Pushing pop drop left Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_POP_DROP_LEFT, cmd);

	return ACT_push_operation(ACT_QUEUE_Pop_drop_left, &args);
}

bool_e ACT_pop_drop_right(ACT_pop_drop_right_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_DROP_RIGHT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_DROP_RIGHT,  ACT_POP_DROP_RIGHT_STOP);

	debug_printf("Pushing pop drop right Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_POP_DROP_RIGHT, cmd);

	return ACT_push_operation(ACT_QUEUE_Pop_drop_right, &args);
}


bool_e ACT_back_spot_right(ACT_back_spot_right_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_BACK_SPOT_RIGHT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_BACK_SPOT_RIGHT,  ACT_BACK_SPOT_RIGHT_STOP);

	debug_printf("Pushing back spot right Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_BACK_SPOT_RIGHT, cmd);

	return ACT_push_operation(ACT_QUEUE_Back_spot_right, &args);
}

bool_e ACT_back_spot_left(ACT_back_spot_left_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_BACK_SPOT_LEFT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_BACK_SPOT_LEFT,  ACT_BACK_SPOT_LEFT_STOP);

	debug_printf("Pushing back spot left Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_BACK_SPOT_LEFT, cmd);

	return ACT_push_operation(ACT_QUEUE_Back_spot_left, &args);
}

bool_e ACT_spot_pompe_right(ACT_spot_pompe_right_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_SPOT_POMPE_RIGHT, cmd, 100);
	ACT_arg_set_fallbackmsg(&args, ACT_SPOT_POMPE_RIGHT,  ACT_SPOT_POMPE_RIGHT_STOP);

	debug_printf("Run stop pompe right cmd\n");

	return ACT_push_operation(ACT_QUEUE_Spot_pompe_right, &args);
}

bool_e ACT_spot_pompe_left(ACT_spot_pompe_left_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_SPOT_POMPE_LEFT, cmd, 100);
	ACT_arg_set_fallbackmsg(&args, ACT_SPOT_POMPE_LEFT,  ACT_SPOT_POMPE_LEFT_STOP);

	debug_printf("Run stop pompe left cmd\n");

	return ACT_push_operation(ACT_QUEUE_Spot_pompe_left, &args);
}

bool_e ACT_carpet_launcher_right(ACT_carpet_launcher_right_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CARPET_LAUNCHER_RIGHT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CARPET_LAUNCHER_RIGHT,  ACT_CARPET_LAUNCHER_RIGHT_STOP);

	debug_printf("Pushing carpet launcher right Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_Carpet_launcher_right, &args);
}


bool_e ACT_carpet_launcher_left(ACT_carpet_launcher_left_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CARPET_LAUNCHER_LEFT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CARPET_LAUNCHER_LEFT,  ACT_CARPET_LAUNCHER_LEFT_STOP);

	debug_printf("Pushing carpet launcher left Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_Carpet_launcher_left, &args);
}

bool_e ACT_cup_nipper(ACT_cup_nipper_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CUP_NIPPER, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CUP_NIPPER,  ACT_cup_nipper_stop);

	debug_printf("Pushing cup nipper Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_CUP_NIPPER, cmd);

	return ACT_push_operation(ACT_QUEUE_Cup_Nipper, &args);
}


bool_e ACT_cup_nipper_elevator(ACT_cup_nipper_elevator_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CUP_NIPPER_ELEVATOR, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CUP_NIPPER_ELEVATOR,  ACT_cup_nipper_elevator_stop);

	debug_printf("Pushing cup nipper elevator Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_Cup_Nipper_Elevator, &args);
}

bool_e ACT_clap_holly(ACT_clap_holly_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CLAP_HOLLY, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CLAP_HOLLY,  ACT_clap_holly_stop);

	debug_printf("Pushing clap Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_CLAP_HOLLY, cmd);

	return ACT_push_operation(ACT_QUEUE_Clap_Holly, &args);
}

bool_e ACT_elevator(ACT_elevator_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_ELEVATOR, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_ELEVATOR,  ACT_elevator_stop);

	debug_printf("Pushing elevator Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_Elevator, &args);
}


bool_e ACT_pincemi_right(ACT_pincemi_right_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PINCEMI_RIGHT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_PINCEMI_RIGHT,  ACT_pincemi_right_stop);

	debug_printf("Pushing pincemi right Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_PinceMi_right, &args);
}

bool_e ACT_pincemi_left(ACT_pincemi_left_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PINCEMI_LEFT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_PINCEMI_LEFT,  ACT_pincemi_left_stop);

	debug_printf("Pushing pincemi left Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_PinceMi_left, &args);
}

bool_e ACT_stock_right(ACT_stock_right_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_STOCK_RIGHT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_STOCK_RIGHT,  ACT_stock_right_stop);

	debug_printf("Pushing stock right Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_Stock_right, &args);
}

bool_e ACT_stock_left(ACT_stock_left_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_STOCK_LEFT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_STOCK_LEFT,  ACT_stock_left_stop);

	debug_printf("Pushing stock left Run cmd\n");

	return ACT_push_operation(ACT_QUEUE_Stock_left, &args);
}


////////////////////////////////////////
//////////////// WOOD //////////////////
////////////////////////////////////////
bool_e ACT_pince_gauche(ACT_pince_gauche_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PINCE_GAUCHE, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_PINCE_GAUCHE,  ACT_PINCE_GAUCHE_STOP);

	return ACT_push_operation(ACT_QUEUE_Pince_Gauche, &args);
}

bool_e ACT_pince_droite(ACT_pince_droite_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PINCE_DROITE, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_PINCE_DROITE,  ACT_PINCE_DROITE_STOP);

	return ACT_push_operation(ACT_QUEUE_Pince_Droite, &args);
}

bool_e ACT_clap(ACT_clap_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CLAP, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CLAP,  ACT_CLAP_STOP);

	return ACT_push_operation(ACT_QUEUE_Clap, &args);
}

bool_e ACT_pop_drop_left_Wood(ACT_pop_drop_left_Wood_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_DROP_LEFT_WOOD, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_DROP_LEFT_WOOD,  ACT_POP_DROP_LEFT_WOOD_STOP);

	return ACT_push_operation(ACT_QUEUE_Pop_drop_left_Wood, &args);
}

bool_e ACT_pop_drop_right_Wood(ACT_pop_drop_right_Wood_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_DROP_RIGHT_WOOD, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_DROP_RIGHT_WOOD,  ACT_POP_DROP_RIGHT_WOOD_STOP);

	return ACT_push_operation(ACT_QUEUE_Pop_drop_right_Wood, &args);
}

////////////////////////////////////////
//////////////// COMMON ////////////////
////////////////////////////////////////

bool_e ACT_config(Uint16 sid, Uint8 sub_act, Uint8 cmd, Uint16 value){
	QUEUE_arg_t args;
	queue_id_e queue_id;
	ACT_can_msg_t msg;

	msg.sid = sid;
	msg.data[0]=ACT_CONFIG;
	msg.data[1]=sub_act;
	msg.data[2]=cmd;
	msg.data[3]=LOWINT(value);
	msg.data[4]=HIGHINT(value);
	msg.size = 5;

	ACT_arg_init_with_msg(&args, msg);
	ACT_arg_set_timeout(&args, 0);

	switch(sid){
		/*case ACT_TORCH_LOCKER :
			queue_id = ACT_QUEUE_Torch_locker;
			debug_printf("Config : ACT_TORCH_LOCKER\n");
			break;*/

		case ACT_POP_COLLECT_LEFT :
			queue_id = ACT_QUEUE_Pop_collect_left;
			debug_printf("Config : ACT_POP_COLLECT_LEFT \n");
			break;

		case ACT_POP_COLLECT_RIGHT :
			queue_id = ACT_QUEUE_Pop_collect_right;
			debug_printf("Config : ACT_POP_COLLECT_RIGHT \n");
			break;

		case ACT_POP_DROP_LEFT :
			queue_id = ACT_QUEUE_Pop_drop_left;
			debug_printf("Config : ACT_POP_DROP_LEFT \n");
			break;

		case ACT_POP_DROP_RIGHT :
			queue_id = ACT_QUEUE_Pop_drop_right;
			debug_printf("Config : ACT_POP_DROP_RIGHT \n");
			break;

		case ACT_ELEVATOR :
			queue_id = ACT_QUEUE_Elevator;
			debug_printf("Config : ACT_ELEVATOR \n");
			break;

		case ACT_PINCEMI_LEFT :
			queue_id = ACT_QUEUE_PinceMi_left;
			debug_printf("Config : ACT_PINCEMI_LEFT\n");
			break;

		case ACT_PINCEMI_RIGHT :
			queue_id = ACT_QUEUE_PinceMi_right;
			debug_printf("Config : ACT_PINCEMI_RIGHT \n");
			break;

		case ACT_STOCK_RIGHT :
			queue_id = ACT_QUEUE_Stock_right;
			debug_printf("Config : ACT_STOCK_RIGHT \n");
			break;

		case ACT_STOCK_LEFT :
			queue_id = ACT_QUEUE_Stock_left;
			debug_printf("Config : ACT_STOCK_LEFT \n");
			break;

		case ACT_CUP_NIPPER :
			queue_id = ACT_QUEUE_Cup_Nipper;
			debug_printf("Config : ACT_CUP_NIPPER \n");
			break;

		case ACT_CUP_NIPPER_ELEVATOR :
			queue_id = ACT_QUEUE_Cup_Nipper_Elevator;
			debug_printf("Config : ACT_CUP_NIPPER_ELEVATOR \n");
			break;

		case ACT_BACK_SPOT_RIGHT :
			queue_id = ACT_QUEUE_Back_spot_right;
			debug_printf("Config : ACT_BACK_SPOT_RIGHT \n");
			break;

		case ACT_BACK_SPOT_LEFT :
			queue_id = ACT_QUEUE_Back_spot_left;
			debug_printf("Config : ACT_BACK_SPOT_LEFT \n");
			break;

		case ACT_SPOT_POMPE_LEFT :
			queue_id = ACT_QUEUE_Spot_pompe_left;
			debug_printf("Config : ACT_SPOT_POMPE_LEFT \n");
			break;

		case ACT_SPOT_POMPE_RIGHT :
			queue_id = ACT_QUEUE_Spot_pompe_right;
			debug_printf("Config : ACT_SPOT_POMPE_RIGHT \n");
			break;

		case ACT_CARPET_LAUNCHER_RIGHT :
			queue_id = ACT_QUEUE_Carpet_launcher_right;
			debug_printf("Config : ACT_CARPET_LAUNCHER_RIGHT \n");
			break;

		case ACT_CARPET_LAUNCHER_LEFT :
			queue_id = ACT_QUEUE_Carpet_launcher_left;
			debug_printf("Config : ACT_CARPET_LAUNCHER_LEFT \n");
			break;

		case ACT_CLAP_HOLLY :
			queue_id = ACT_QUEUE_Clap_Holly;
			debug_printf("Config : ACT_CLAP_HOLLY \n");
			break;

		case ACT_PINCE_GAUCHE :
			queue_id = ACT_QUEUE_Pince_Gauche;
			debug_printf("Config : ACT_PINCE_GAUCHE \n");
			break;

		case ACT_PINCE_DROITE :
			queue_id = ACT_QUEUE_Pince_Droite;
			debug_printf("Config : ACT_PINCE_DROITE \n");
			break;

		case ACT_CLAP :
			queue_id = ACT_QUEUE_Clap;
			debug_printf("Config : ACT_CLAP \n");
			break;

		case ACT_POP_DROP_LEFT_WOOD:
			queue_id = ACT_QUEUE_Pop_drop_left_Wood;
			debug_printf("Config : ACT_POP_DROP_LEFT_WOOD \n");
			break;

		case ACT_POP_DROP_RIGHT_WOOD:
			queue_id = ACT_QUEUE_Pop_drop_right_Wood;
			debug_printf("Config : ACT_POP_DROP_RIGHT_WOOD \n");
			break;

		default :
			warn_printf("ACT_CONFIG : sid de l'actionneur introuvable\n");
			return FALSE;
	}
	debug_printf("    cmd : %d\n", cmd);
	debug_printf("    value : %d\n", value);

	return ACT_push_operation(queue_id, &args);
}

////////////////////////////////////////
//////////////// MAE  //////////////////
////////////////////////////////////////

static error_e ACT_MAE_holly_spotix(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who){
	CREATE_MAE_WITH_VERBOSE(SM_ID_HOLLY_MAE_SPOTIX,
		INIT,
		COMPUTE_ORDER,
		FAIL_COMPUTE,

		// Open
		OPEN_ALL,
		WIN_OPEN_ALL,

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

		// Stock
		INIT_STOCK,
		ELEVATOR_PRE_UP,
		FAIL_ELEVATOR_PRE_UP,
		STOCK_UNLOCK,
		ELEVATOR_UP,
		FAIL_ELEVATOR_UP_1,
		FAIL_ELEVATOR_UP_2,
		STOCK_LOCK,
		ELEVATOR_DOWN,
		NIPPER_OPEN,
		WIN_STOCK,
		FAIL_STOCK,

		// Go down (and release)
		RELEASE_NIPPER,
		ELEVATOR_GO_DOWN,
		WIN_GO_DOWN,
		FAIL_GO_DOWN,

		// Go up
		ELEVATOR_GO_UP,
		WIN_GO_UP,
		FAIL_GO_UP,

		// Release spot
		UNLOCK_SPOT,
		RELEASE_SPOT,
		WIN_RELEASE,
		FAIL_RELEASE
	);

	static bool_e right_error = FALSE, left_error = FALSE;
	static enum state_e state1, state2, state3, state4;
	error_e ret = IN_PROGRESS;

	switch(state){
		case INIT:
			right_error = FALSE;
			left_error = FALSE;
			state = COMPUTE_ORDER;
			break;

		case COMPUTE_ORDER:
			switch(order){

				case ACT_MAE_SPOTIX_OPEN:
					state = OPEN_ALL;
					break;

				case ACT_MAE_SPOTIX_TAKE:
					state = TAKE_FEET;
					break;

				case ACT_MAE_SPOTIX_TAKE_BALL:
					state = TAKE_BALL;
					break;

				case ACT_MAE_SPOTIX_OPEN_GREAT:
					if(who != ACT_MAE_SPOTIX_BOTH)
						state = OPEN_GREAT_FEET;
					else
						state = FAIL_COMPUTE;
					break;

				case ACT_MAE_SPOTIX_STOCK_AND_GO_DOWN:
				case ACT_MAE_SPOTIX_STOCK_AND_STAY:
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

				case ACT_MAE_SPOTIX_RELEASE_STOCK:
					state = UNLOCK_SPOT;
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
					ACT_stock_right(ACT_stock_right_open);
					ACT_pincemi_right(ACT_pincemi_right_open);
				}
				if(who != ACT_MAE_SPOTIX_RIGHT){
					ACT_stock_left(ACT_stock_left_open);
					ACT_pincemi_left(ACT_pincemi_left_open);
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

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != OPEN_ALL && state2 != OPEN_ALL && state3 != OPEN_ALL && state4 != OPEN_ALL)
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != OPEN_ALL && state2 != OPEN_ALL)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state3 != OPEN_ALL && state4 != OPEN_ALL))
				state = WIN_OPEN_ALL;
			break;

		case WIN_OPEN_ALL:
			RESET_MAE();
			ret = END_OK;
			break;

//--------------------------------------- Take feet

		case TAKE_FEET:
			if(entrance){
				state1 = state2 = TAKE_FEET;
				if(who != ACT_MAE_SPOTIX_LEFT)
					ACT_pincemi_right(ACT_pincemi_right_lock);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_pincemi_left(ACT_pincemi_left_lock);
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
			if(who != ACT_MAE_SPOTIX_RIGHT && !PRESENCE_PIED_PINCE_GAUCHE_HOLLY)
				left_error = TRUE;
			if(who != ACT_MAE_SPOTIX_LEFT && !PRESENCE_PIED_PINCE_DROITE_HOLLY)
				right_error = TRUE;

			if(left_error || right_error)
				state = FAIL_TAKE;
			else
				state = WIN_TAKE;
#endif
		break;

		case WIN_TAKE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_TAKE:
			if(left_error)
				ACT_pincemi_left(ACT_pincemi_left_open);

			if(right_error)
				ACT_pincemi_right(ACT_pincemi_right_open);

			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Take ball

		case TAKE_BALL:
				if(entrance){
					state1 = state2 = TAKE_BALL;
					if(who != ACT_MAE_SPOTIX_LEFT)
						ACT_pincemi_right(ACT_pincemi_right_lock_ball);
					if(who != ACT_MAE_SPOTIX_RIGHT)
						ACT_pincemi_left(ACT_pincemi_left_lock_ball);
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
					ACT_pincemi_left(ACT_pincemi_left_open);

				if(right_error)
					ACT_pincemi_right(ACT_pincemi_right_open);

				RESET_MAE();
				ret = NOT_HANDLED;
				break;


//--------------------------------------- Take great feet

		case MOVE_OTHER_NIPPER:
			if(entrance){
				if(who == ACT_MAE_SPOTIX_LEFT){
					ACT_pincemi_right(ACT_pincemi_right_close_inner);
				}if(who == ACT_MAE_SPOTIX_RIGHT){
					ACT_pincemi_left(ACT_pincemi_left_close_inner);
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
					ACT_pincemi_left(ACT_pincemi_left_open_great);
				}if(who == ACT_MAE_SPOTIX_RIGHT){
					ACT_pincemi_right(ACT_pincemi_right_open_great);
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
				ACT_pincemi_right(ACT_pincemi_right_open);
				ACT_pincemi_left(ACT_pincemi_left_open);
			}
			RESET_MAE();
			ret = NOT_HANDLED;
			break;


//--------------------------------------- Stock

		case INIT_STOCK:
#ifndef ROBOT_VIRTUEL_PARFAIT
			if(who != ACT_MAE_SPOTIX_RIGHT && !PRESENCE_PIED_PINCE_GAUCHE_HOLLY)
				left_error = TRUE;
			if(who != ACT_MAE_SPOTIX_LEFT && !PRESENCE_PIED_PINCE_DROITE_HOLLY)
				right_error = TRUE;
#endif
			if(left_error && right_error)
				state = FAIL_STOCK;
			else
				state = ELEVATOR_PRE_UP;
			break;

		case ELEVATOR_PRE_UP:
			if(entrance)
				ACT_elevator(ACT_elevator_pre_top);
			state = check_act_status(ACT_QUEUE_Elevator, state, STOCK_UNLOCK, FAIL_STOCK);
			break;

		case FAIL_ELEVATOR_PRE_UP:
			if(entrance)
				ACT_elevator(ACT_elevator_bot);
			state = check_act_status(ACT_QUEUE_Elevator, state, FAIL_STOCK, FAIL_STOCK);
			break;

		case STOCK_UNLOCK:
			if(entrance){
				state1 = state2 = STOCK_UNLOCK;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_stock_right(ACT_stock_right_unlock);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_stock_left(ACT_stock_left_unlock);
			}
			if(state1 == STOCK_UNLOCK && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state1 = check_act_status(ACT_QUEUE_Stock_right, state, ELEVATOR_UP, FAIL_TAKE);
			if(state2 == STOCK_UNLOCK && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state2 = check_act_status(ACT_QUEUE_Stock_left, state, ELEVATOR_UP, FAIL_TAKE);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != STOCK_UNLOCK && state2 != STOCK_UNLOCK)
					|| (who == ACT_MAE_SPOTIX_LEFT && state2 != STOCK_UNLOCK)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state1 != STOCK_UNLOCK))
				state = ELEVATOR_UP;
			break;

		// Question gestion d'erreur sur le fait de fail l'unlock ?

		case ELEVATOR_UP:
			if(entrance)
				ACT_elevator(ACT_elevator_top);
			state = check_act_status(ACT_QUEUE_Elevator, state, STOCK_LOCK, FAIL_STOCK);
			break;

		case FAIL_ELEVATOR_UP_1:
			if(entrance){
				state1 = state2 = FAIL_ELEVATOR_UP_1;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_stock_right(ACT_stock_right_lock);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_stock_left(ACT_stock_left_lock);
			}
			if(state1 == FAIL_ELEVATOR_UP_1 && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state1 = check_act_status(ACT_QUEUE_Stock_left, state, FAIL_ELEVATOR_UP_2, FAIL_ELEVATOR_UP_2);
			if(state2 == FAIL_ELEVATOR_UP_1 && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state2 = check_act_status(ACT_QUEUE_Stock_right, state, FAIL_ELEVATOR_UP_2, FAIL_ELEVATOR_UP_2);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != FAIL_ELEVATOR_UP_1 && state2 != FAIL_ELEVATOR_UP_1)
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != FAIL_ELEVATOR_UP_1)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state2 != FAIL_ELEVATOR_UP_1))
				state = FAIL_ELEVATOR_UP_2;
			break;

		case FAIL_ELEVATOR_UP_2:
			if(entrance)
				ACT_elevator(ACT_elevator_bot);
			state = check_act_status(ACT_QUEUE_Elevator, state, FAIL_STOCK, FAIL_STOCK);
			break;

		case STOCK_LOCK:
			if(entrance){
				state1 = state2 = STOCK_LOCK;
				if(who != ACT_MAE_SPOTIX_LEFT && !right_error)
					ACT_stock_right(ACT_stock_right_lock);
				if(who != ACT_MAE_SPOTIX_RIGHT && !left_error)
					ACT_stock_left(ACT_stock_left_lock);
			}
			if(state1 == STOCK_LOCK && who != ACT_MAE_SPOTIX_RIGHT && !left_error)
				state1 = check_act_status(ACT_QUEUE_Stock_left, state, NIPPER_OPEN, FAIL_TAKE);
			if(state2 == STOCK_LOCK && who != ACT_MAE_SPOTIX_LEFT && !right_error)
				state2 = check_act_status(ACT_QUEUE_Stock_right, state, NIPPER_OPEN, FAIL_TAKE);

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != STOCK_LOCK && state2 != STOCK_LOCK)
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != STOCK_LOCK)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state2 != STOCK_LOCK)){
				if(order == ACT_MAE_SPOTIX_STOCK_AND_STAY)
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
					ACT_pincemi_right(ACT_pincemi_right_open);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_pincemi_left(ACT_pincemi_left_open);
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
				ACT_elevator(ACT_elevator_bot);
			state = check_act_status(ACT_QUEUE_Elevator, state, WIN_STOCK, FAIL_STOCK);
			break;

		case WIN_STOCK:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_STOCK:
			if(left_error)
				ACT_pincemi_left(ACT_pincemi_left_open);

			if(right_error)
				ACT_pincemi_right(ACT_pincemi_right_open);

			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//--------------------------------------- Go down and release
		case RELEASE_NIPPER:
			if(entrance){
				state1 = state2 = RELEASE_NIPPER;
				if(who != ACT_MAE_SPOTIX_LEFT)
					ACT_pincemi_right(ACT_pincemi_right_open);
				if(who != ACT_MAE_SPOTIX_RIGHT)
					ACT_pincemi_left(ACT_pincemi_left_open);
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
				ACT_elevator(ACT_elevator_bot);
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
				ACT_elevator(ACT_elevator_top);
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


//--------------------------------------- Release

		case UNLOCK_SPOT:
			if(entrance){
				state1 = state2 = state3 = state4 = UNLOCK_SPOT;
				if(who != ACT_MAE_SPOTIX_LEFT){
					ACT_stock_right(ACT_stock_right_unlock);
					ACT_pincemi_right(ACT_pincemi_right_unlock);
				}
				if(who != ACT_MAE_SPOTIX_RIGHT){
					ACT_stock_left(ACT_stock_left_unlock);
					ACT_pincemi_left(ACT_pincemi_left_unlock);
				}
			}
			if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
				if(state1 == UNLOCK_SPOT)
					state1 = check_act_status(ACT_QUEUE_Stock_right, state, RELEASE_SPOT, FAIL_RELEASE);
				if(state2 == UNLOCK_SPOT)
					state2 = check_act_status(ACT_QUEUE_PinceMi_right, state, RELEASE_SPOT, FAIL_RELEASE);
			}

			if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
				if(state3 == UNLOCK_SPOT)
					state3 = check_act_status(ACT_QUEUE_Stock_left, state, RELEASE_SPOT, FAIL_RELEASE);
				if(state4 == UNLOCK_SPOT)
					state4 = check_act_status(ACT_QUEUE_PinceMi_left, state, RELEASE_SPOT, FAIL_RELEASE);
			}

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != UNLOCK_SPOT && state2 != UNLOCK_SPOT && state3 != UNLOCK_SPOT && state4 != UNLOCK_SPOT)
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != UNLOCK_SPOT && state2 != UNLOCK_SPOT)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state3 != UNLOCK_SPOT && state4 != UNLOCK_SPOT))
				state = RELEASE_SPOT;
			break;


		case RELEASE_SPOT:
			if(entrance){
				state1 = state2 = state3 = state4 = RELEASE_SPOT;
				if(who != ACT_MAE_SPOTIX_LEFT){
					ACT_stock_right(ACT_stock_right_open);
					ACT_pincemi_right(ACT_pincemi_right_open);
				}
				if(who != ACT_MAE_SPOTIX_RIGHT){
					ACT_stock_left(ACT_stock_left_open);
					ACT_pincemi_left(ACT_pincemi_left_open);
				}
			}
			if(who != ACT_MAE_SPOTIX_LEFT){ // Gestion Droite
				if(state1 == RELEASE_SPOT)
					state1 = check_act_status(ACT_QUEUE_Stock_right, state, WIN_RELEASE, FAIL_RELEASE);
				if(state2 == RELEASE_SPOT)
					state2 = check_act_status(ACT_QUEUE_PinceMi_right, state, WIN_RELEASE, FAIL_RELEASE);
			}

			if(who != ACT_MAE_SPOTIX_RIGHT){ // Gestion Gauche
				if(state3 == RELEASE_SPOT)
					state3 = check_act_status(ACT_QUEUE_Stock_left, state, WIN_RELEASE, FAIL_RELEASE);
				if(state4 == RELEASE_SPOT)
					state4 = check_act_status(ACT_QUEUE_PinceMi_left, state, WIN_RELEASE, FAIL_RELEASE);
			}

			if((who == ACT_MAE_SPOTIX_BOTH && state1 != RELEASE_SPOT && state2 != RELEASE_SPOT && state3 != RELEASE_SPOT && state4 != RELEASE_SPOT)
					|| (who == ACT_MAE_SPOTIX_LEFT && state1 != RELEASE_SPOT && state2 != RELEASE_SPOT)
					|| (who == ACT_MAE_SPOTIX_RIGHT && state3 != RELEASE_SPOT && state4 != RELEASE_SPOT))
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
	}


	return ret;
}

error_e ACT_MAE_holly_cup(ACT_MAE_holly_cup_e order){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		COMPUTE_ORDER,
		FAIL_COMPUTE,

		// Init pos
		INIT_POS,
		WIN_INIT,
		FAIL_INIT,

		// Cup take
		TAKE_CUP,
		CHECK_PRESENCE_CUP,
		ELEVATOR_UP,
		WIN_TAKE,
		FAIL_TAKE,

		// Down and release
		ELEVATOR_DOWN,
		RELEASE_CUP,
		WIN_RELEASE,
		FAIL_RELEASE,

		// Close
		ELEVATOR_DOWN_FOR_CLOSE,
		CLOSE,
		WIN_CLOSE,
		FAIL_CLOSE
	);

	static bool_e init = FALSE;
	error_e ret = IN_PROGRESS;

	switch(state){
		case INIT:
			state = COMPUTE_ORDER;
			break;

		case COMPUTE_ORDER:
			switch(order){

				case ACT_MAE_CUP_TAKE:
				case ACT_MAE_CUP_TAKE_AND_UP:
					state = TAKE_CUP;
					break;

				case ACT_MAE_CUP_DOWN_AND_RELEASE:
					state = ELEVATOR_DOWN;
					break;

				case ACT_MAE_CUP_CLOSE:
					state = ELEVATOR_DOWN_FOR_CLOSE;
					break;

				default :
					state = FAIL_COMPUTE;
					break;
			}
			if(init == FALSE)
				state = INIT_POS;
			break;

		case FAIL_COMPUTE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//----------------------------------------- Init
		case INIT_POS:
			if(entrance)
				ACT_cup_nipper(ACT_cup_nipper_open);
			state = check_act_status(ACT_QUEUE_Cup_Nipper, state, WIN_INIT, FAIL_INIT);
			break;

		case WIN_INIT:
			init = TRUE;
			state = COMPUTE_ORDER;
			break;

		case FAIL_INIT:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;


//----------------------------------------- Take cup
		case TAKE_CUP:
			if(entrance)
				ACT_cup_nipper(ACT_cup_nipper_lock);
			state = check_act_status(ACT_QUEUE_Cup_Nipper, state, CHECK_PRESENCE_CUP, FAIL_TAKE);
			break;

		case CHECK_PRESENCE_CUP:{
#ifdef ROBOT_VIRTUEL_PARFAIT
			state = ELEVATOR_UP;
#else
			static enum state_e sucess_state;
			if(entrance){
				if(order == ACT_MAE_CUP_TAKE)
					sucess_state = WIN_TAKE;
				else
					sucess_state = FAIL_TAKE;
			}
			state = check_sub_action_result(ACT_sensor_gobelet_holly(), state, sucess_state, FAIL_TAKE);
#endif
		}break;

		case ELEVATOR_UP:
			if(entrance)
				ACT_cup_nipper_elevator(ACT_cup_nipper_elevator_up);
			state = check_act_status(ACT_QUEUE_Cup_Nipper_Elevator, state, WIN_TAKE, FAIL_TAKE);
			break;

		case WIN_TAKE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_TAKE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//----------------------------------------- Down and release
		case ELEVATOR_DOWN:
			if(entrance)
				ACT_cup_nipper_elevator(ACT_cup_nipper_elevator_idle);
			state = check_act_status(ACT_QUEUE_Cup_Nipper_Elevator, state, RELEASE_CUP, FAIL_RELEASE);
			break;

		case RELEASE_CUP:
			if(entrance)
				ACT_cup_nipper(ACT_cup_nipper_open);
			state = check_act_status(ACT_QUEUE_Cup_Nipper, state, WIN_RELEASE, FAIL_RELEASE);
			break;

		case WIN_RELEASE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_RELEASE:
			RESET_MAE();
			ret = NOT_HANDLED;
			break;

//------------------------------------------- Close
		case ELEVATOR_DOWN_FOR_CLOSE:
			if(entrance)
				ACT_cup_nipper_elevator(ACT_cup_nipper_elevator_idle);
			state = check_act_status(ACT_QUEUE_Cup_Nipper_Elevator, state, CLOSE, FAIL_CLOSE);
			break;

		case CLOSE:
			if(entrance)
				ACT_cup_nipper(ACT_cup_nipper_close);
			state = check_act_status(ACT_QUEUE_Cup_Nipper, state, WIN_CLOSE, FAIL_CLOSE);
			if(ON_LEAVING(CLOSE))
				init = FALSE;
			break;

		case WIN_CLOSE:
			RESET_MAE();
			ret = END_OK;
			break;

		case FAIL_CLOSE:
			RESET_MAE();
			ret = NOT_HANDLED;
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
volatile static spotix_order_id_t last_order_id = -1;

error_e ACT_MAE_holly_spotix_bloquing(ACT_MAE_holly_spotix_e order, ACT_MAE_holly_spotix_side_e who){
	static bool_e entrance = TRUE;
	static spotix_order_id_t spotix_order_id;

	if(entrance)
		spotix_order_id = ACT_MAE_holly_spotix_do_order(order, who);

	if(spotix_order_id == -1){
		entrance = TRUE;
		return NOT_HANDLED;
	}

	if(ACT_MAE_holly_wait_end_order(spotix_order_id)){
		entrance = TRUE;
		return ACT_holly_spotix_get_last_error();
	}

	entrance = FALSE;
	return IN_PROGRESS;
}

bool_e ACT_MAE_holly_wait_end_order(spotix_order_id_t id){
	if(last_order_id >= id)
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
	CREATE_MAE_WITH_VERBOSE(SM_ID_HOLLY_MANAGE_MAE_SPOTIX,
		INIT,
		WAIT_ORDER,
		DO_ORDER
	);

	static state_e state;
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
				last_order_id = spotix_order_queue[spotix_read].order_id;
				spotix_read = (spotix_read + 1) % 10;
			}
			break;

	}
}

error_e ACT_holly_spotix_get_last_error(){
	return last_spotix_error;
}


////////////////////////////////////////
//////////////// SENSOR ////////////////
////////////////////////////////////////

error_e ACT_sensor_gobelet_right_wood(){
	CREATE_MAE(SEND,
				WAIT);

	static time32_t begin_time;

	switch(state){
		case SEND:
			gobelet_right_wood_answer = ACT_SENSOR_WAIT;
			begin_time = global.env.absolute_time;
			CAN_direct_send(ACT_ASK_SENSOR, 1, (Uint8 []){PINCE_GOBELET_DROITE});
			state = WAIT;
			break;

		case WAIT:
			if(global.env.absolute_time - begin_time > ACT_SENSOR_ANSWER_TIMEOUT){
				RESET_MAE();
				return END_WITH_TIMEOUT;
			}else if(gobelet_right_wood_answer == ACT_SENSOR_PRESENT){
				RESET_MAE();
				return END_OK;
			}else if(gobelet_right_wood_answer == ACT_SENSOR_ABSENT){
				RESET_MAE();
				return NOT_HANDLED;
			}
			break;
	}
	return IN_PROGRESS;
}

error_e ACT_sensor_gobelet_left_wood(){
	CREATE_MAE(SEND,
				WAIT);

	static time32_t begin_time;

	switch(state){
		case SEND:
			gobelet_left_wood_answer = ACT_SENSOR_WAIT;
			begin_time = global.env.absolute_time;
			CAN_direct_send(ACT_ASK_SENSOR, 1, (Uint8 []){PINCE_GOBELET_GAUCHE});
			state = WAIT;
			break;

		case WAIT:
			if(global.env.absolute_time - begin_time > ACT_SENSOR_ANSWER_TIMEOUT){
				RESET_MAE();
				return END_WITH_TIMEOUT;
			}else if(gobelet_left_wood_answer == ACT_SENSOR_PRESENT){
				RESET_MAE();
				return END_OK;
			}else if(gobelet_left_wood_answer == ACT_SENSOR_ABSENT){
				RESET_MAE();
				return NOT_HANDLED;
			}
			break;
	}
	return IN_PROGRESS;
}

error_e ACT_sensor_gobelet_front_wood(){
	CREATE_MAE(SEND,
				WAIT);

	static time32_t begin_time;

	switch(state){
		case SEND:
			gobelet_front_wood_answer = ACT_SENSOR_WAIT;
			begin_time = global.env.absolute_time;
			CAN_direct_send(ACT_ASK_SENSOR, 1, (Uint8 []){GOBELET_DEVANT_WOOD});
			state = WAIT;
			break;

		case WAIT:
			if(global.env.absolute_time - begin_time > ACT_SENSOR_ANSWER_TIMEOUT){
				RESET_MAE();
				return END_WITH_TIMEOUT;
			}else if(gobelet_front_wood_answer== ACT_SENSOR_PRESENT){
				RESET_MAE();
				return END_OK;
			}else if(gobelet_front_wood_answer == ACT_SENSOR_ABSENT){
				RESET_MAE();
				return NOT_HANDLED;
			}
			break;
	}
	return IN_PROGRESS;
}

error_e ACT_sensor_gobelet_holly(){
	CREATE_MAE(SEND,
				WAIT);

	static time32_t begin_time;

	switch(state){
		case SEND:
			gobelet_holly_answer = ACT_SENSOR_WAIT;
			begin_time = global.env.absolute_time;
			CAN_direct_send(ACT_ASK_SENSOR, 1, (Uint8 []){GOBELET_HOLLY});
			state = WAIT;
			break;

		case WAIT:
			if(global.env.absolute_time - begin_time > ACT_SENSOR_ANSWER_TIMEOUT){
				RESET_MAE();
				return END_WITH_TIMEOUT;
			}else if(gobelet_holly_answer== ACT_SENSOR_PRESENT){
				RESET_MAE();
				return END_OK;
			}else if(gobelet_holly_answer == ACT_SENSOR_ABSENT){
				RESET_MAE();
				return NOT_HANDLED;
			}
			break;
	}
	return IN_PROGRESS;
}


void ACT_sensor_answer(CAN_msg_t* msg){
	volatile act_sensor_e *act_answer;

	switch(msg->data[0]){
		case PINCE_GOBELET_DROITE:
			act_answer = &gobelet_right_wood_answer;
			break;

		case PINCE_GOBELET_GAUCHE:
			act_answer = &gobelet_left_wood_answer;
			break;

		case GOBELET_DEVANT_WOOD:
			act_answer = &gobelet_front_wood_answer;
			break;

		case GOBELET_HOLLY:
			act_answer = &gobelet_holly_answer;
			break;
	}

	if(!act_answer)
		return;

	if(msg->data[1] == STRAT_INFORM_CAPTEUR_PRESENT)
		*act_answer = ACT_SENSOR_PRESENT;
	else
		*act_answer = ACT_SENSOR_ABSENT;

}
