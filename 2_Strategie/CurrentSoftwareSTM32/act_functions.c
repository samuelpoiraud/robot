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
#include "state_machine_helper.h"

#define ACT_SENSOR_ANSWER_TIMEOUT		500

typedef enum{
	ACT_SENSOR_WAIT,
	ACT_SENSOR_ABSENT,
	ACT_SENSOR_PRESENT
}act_sensor_e;

static volatile act_sensor_e gobelet_right_wood_answer = ACT_SENSOR_WAIT;
static volatile act_sensor_e gobelet_left_wood_answer = ACT_SENSOR_WAIT;
static volatile act_sensor_e gobelet_front_wood_answer = ACT_SENSOR_WAIT;
static volatile act_sensor_e gobelet_holly_answer = ACT_SENSOR_WAIT;


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

bool_e ACT_pop_drop_left_wood(ACT_pop_drop_left_Wood_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_DROP_LEFT_WOOD, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_DROP_LEFT_WOOD,  ACT_POP_DROP_LEFT_WOOD_STOP);

	return ACT_push_operation(ACT_QUEUE_Pop_drop_left_Wood, &args);
}

bool_e ACT_pop_drop_right_wood(ACT_pop_drop_right_Wood_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_POP_DROP_RIGHT_WOOD, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POP_DROP_RIGHT_WOOD,  ACT_POP_DROP_RIGHT_WOOD_STOP);

	return ACT_push_operation(ACT_QUEUE_Pop_drop_right_Wood, &args);
}

////////////////////////////////////////
//////////////// COMMON ////////////////
////////////////////////////////////////

bool_e ACT_config(Uint16 sid, Uint8 cmd, Uint16 value){
	QUEUE_arg_t args;
	queue_id_e queue_id;
	ACT_can_msg_t msg;

	msg.sid = sid;
	msg.data[0]=ACT_CONFIG;
	msg.data[1]=cmd;
	msg.data[2]=LOWINT(value);
	msg.data[3]=HIGHINT(value);
	msg.size = 4;

	ACT_arg_init_with_msg(&args, msg);
	ACT_arg_set_timeout(&args, 0);

	switch(sid){
		/*case ACT_TORCH_LOCKER :
			queue_id = ACT_QUEUE_Torch_locker;
			debug_printf("Config : ACT_TORCH_LOCKER\n");
			break;*/

		default :
			warn_printf("ACT_CONFIG : sid de l'actionneur introuvable\n");
			return FALSE;
	}
	debug_printf("    cmd : %d\n", cmd);
	debug_printf("    value : %d\n", value);

	return ACT_push_operation(queue_id, &args);
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
