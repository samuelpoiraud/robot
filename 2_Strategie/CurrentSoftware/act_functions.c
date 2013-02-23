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
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "Stacks.h"
#include "output_log.h"

#define LOG_PREFIX "act_f: "

//Timeout en ms
#define ACT_SET_POS_TIMEOUT_TIME  	10000

// Timeout en ms

//Utilisez OUTPUT_printf plutot
/*#ifdef DEBUG_ACT_FUN
	#define act_fun_printf(...)	debug_printf("Act: "__VA_ARGS__)
#else
	#define act_fun_printf(...)	(void)0
#endif
*/

/*	Piles conservant les eventuels arguments pour les fonctions des actionneurs
 *	tout en conservant le meme prototype pour tous les actionneurs, reduisant
 *	le code de gestion des actionneurs à chaque boucle
 */

//Pour timeout: utiliser ACT_ARG_NOTIMEOUT pour ne pas mettre de timeout
typedef struct {
	Uint32 param;
	time32_t timeout;
} act_arg_t;

#define ACT_ARG_NOTIMEOUT ((time32_t)-1)
#define ACT_ARG_USE_DEFAULT 0
#define ACT_STACK_TIMEOUT_MS 3000

//static act_arg_t act_args[NB_ACT][STACKS_SIZE];
static act_arg_t act_args[ACTUATORS_NB][STACKS_SIZE];

// Verifie s'il y a eu un timeout et met à jour l'état de la pile en conséquence.
// Si l'état de l'actionneur n'est pas normal ou qu'il y a eu une erreur, cette fonction affiche un message d'erreur
static bool_e ACT_check_result(stack_id_e act_id, const char* command);

/* Accesseur en lecture sur les arguments des piles ACT */
static act_arg_t* ACT_get_stack_arg(stack_id_e act_id);
/* Accesseur en écriture sur les arguments des piles ACT */
static void ACT_set_stack_arg(stack_id_e act_id, const act_arg_t* arg);

static void ACT_run_ball_launcher_run(stack_id_e stack_id, bool_e init);
static void ACT_run_ball_launcher_stop(stack_id_e stack_id, bool_e init);

void ACT_hammer_up(void){

    CAN_msg_t order;
    order.sid = ACT_AX12;
    order.size=1;
    order.data[0]=ACT_HAMMER_GO_UP;
    CAN_send(&order);
}
void ACT_hammer_down(void){

    CAN_msg_t order;
    order.sid = ACT_AX12;
    order.size=1;
    order.data[0]=ACT_HAMMER_GO_DOWN;
    CAN_send(&order);
}
void ACT_hammer_tidy(void){

    CAN_msg_t order;
    order.sid = ACT_AX12;
    order.size=1;
    order.data[0]=ACT_HAMMER_GO_TIDY;
    CAN_send(&order);
}

void ACT_ball_grabber_up(void){

    CAN_msg_t order;
    order.sid = ACT_AX12;
    order.size=1;
    order.data[0]= ACT_BALL_GRABBER_GO_UP;
    CAN_send(&order);
}
void ACT_ball_grabber_down(void){

    CAN_msg_t order;
    order.sid = ACT_AX12;
    order.size=1;
    order.data[0]= ACT_BALL_GRABBER_GO_DOWN;
    CAN_send(&order);
}
void ACT_ball_grabber_tidy(void){

    CAN_msg_t order;
    order.sid = ACT_AX12;
    order.size=1;
    order.data[0]= ACT_BALL_GRABBER_GO_TIDY;
    CAN_send(&order);
}

void ACT_push_ball_launcher_run(Uint16 speed, bool_e run) {
	act_arg_t args;
	args.param = speed;
	args.timeout = ACT_ARG_USE_DEFAULT;
	ACT_set_stack_arg(ACT_STACK_BallLauncher, &args);
	global.env.act[ACT_STACK_BallLauncher].lastOperationResult = ACT_RESULT_Idle;
	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing BallLauncher Run cmd, speed: %u", speed);
	STACKS_push(ACT_STACK_BallLauncher, &ACT_run_ball_launcher_run, run);
}

void ACT_push_ball_launcher_stop(bool_e run) {
	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing BallLauncher Stop cmd");
	STACKS_push(ACT_STACK_BallLauncher, &ACT_run_ball_launcher_stop, run);
}

void ACT_process_result(Uint8 can_act_id, Uint8 can_result) {
	stack_id_e act_id = ACTUATORS_NB;

	switch(can_act_id) {
		case ACT_BALLLAUNCHER & 0xFF:
			act_id = ACT_STACK_BallLauncher;
			break;

		case ACT_PLATE & 0xFF:
			act_id = ACT_STACK_Plate;
			break;
	}

	if(act_id < ACTUATORS_NB) {
		if(can_result == ACT_RESULT_DONE)
			global.env.act[act_id].lastOperationResult = ACT_RESULT_Ok;
		else if(can_result == ACT_RESULT_FAILED)
			global.env.act[act_id].lastOperationResult = ACT_RESULT_Fail;
	} else {
		OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Unknown act result, can_act_id: %hhu, can_result: %hhu", can_act_id, can_result);
	}
}

static void ACT_run_ball_launcher_run(stack_id_e stack_id, bool_e init) {
	if(stack_id == ACT_STACK_BallLauncher) {
		if(init) {
			CAN_msg_t order;
			order.sid = ACT_BALLLAUNCHER;
			order.data[0] = ACT_BALLLAUNCHER_ACTIVATE;
			order.data[1] = LOWINT(ACT_get_stack_arg(stack_id)->param);
			order.data[2] = HIGHINT(ACT_get_stack_arg(stack_id)->param);
			order.size = 3;
			OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Sending BallLauncher Run cmd, speed: %u", ACT_get_stack_arg(stack_id)->param);
			CAN_send(&order);
			global.env.act[ACT_STACK_BallLauncher].lastOperationResult = ACT_RESULT_Working;
		} else {
			if(ACT_check_result(stack_id, "balllauncher_run"))
				STACKS_pull(stack_id);
		}
	}
}

static void ACT_run_ball_launcher_stop(stack_id_e stack_id, bool_e init) {
	if(stack_id == ACT_STACK_BallLauncher) {
		if(init) {
			CAN_msg_t order;
			order.sid = ACT_BALLLAUNCHER;
			order.data[0] = ACT_BALLLAUNCHER_STOP;
			order.size = 1;
			OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Sending BallLauncher Stop cmd");
			CAN_send(&order);
			STACKS_pull(stack_id);
		}
	}
}

//Retourne TRUE si l'opération s'est terminée correctement
static bool_e ACT_check_result(stack_id_e act_id, const char* command) {
	if(global.env.match_time >= ACT_get_stack_arg(act_id)->timeout) {
		if(command)
			OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation timeout act id: %u, cmd: %s\n", act_id, command);
		else OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation timeout act id: %u\n", act_id);
		STACKS_set_timeout(act_id, TRUE);
		return FALSE;
	} else {
		switch(global.env.act[act_id].lastOperationResult) {
			case ACT_RESULT_Fail:
				if(command)
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation failed act id: %u, cmd: %s\n", act_id, command);
				else OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation failed act id: %u\n", act_id);
				STACKS_set_timeout(act_id, TRUE);
				return FALSE;

			case ACT_RESULT_Ok:
				return TRUE;

			case ACT_RESULT_Working:
				return FALSE;

			case ACT_RESULT_Idle:
				if(command)
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Warning: act should be in working/finished mode but was in Idle mode, act id: %u, cmd: %s\n", act_id, command);
				else OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Warning: act should be in working/finished mode but was in Idle mode, act id: %u\n", act_id);
				STACKS_set_timeout(act_id, TRUE);
				return FALSE;
		}
	}

	return FALSE;
}

/* Accesseur sur les arguments des piles ACT */
act_arg_t* ACT_get_stack_arg(stack_id_e act_id)
{
	return &act_args[act_id][STACKS_get_top(act_id)];
}

// Accesseur en écriture sur les arguments des piles ACT
// arg->timeout doit contenir le timeout relatif, 0 pour pas de timeout géré par la carte stratégie
void ACT_set_stack_arg(stack_id_e act_id, const act_arg_t* arg) {
	act_args[act_id][STACKS_get_top(act_id)+1] = *arg;
	if(arg->timeout)
		act_args[act_id][STACKS_get_top(act_id)+1].timeout = global.env.match_time + arg->timeout;
	else act_args[act_id][STACKS_get_top(act_id)+1].timeout = global.env.match_time + ACT_STACK_TIMEOUT_MS;
}

/*void ACT_set_pos(stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if(init)
	{
		order.sid = ACT_position_data(stack_id);
		order.size=1;
		order.data[0]=ACT_id_data(stack_id);
		CAN_send(&order);
	}
	else
	{
		// ACT_position_data(stack_id) incorrecte
		if (global.env.act[ACT_id_data(stack_id)].opened && ACT_position_data(stack_id)==ACT_OPEN)
		{
			act_fun_printf("ACT_set_pos ACT OPENED\n");
			STACKS_pull(stack_id);
		}
		else if (global.env.act[ACT_id_data(stack_id)].closed && ACT_position_data(stack_id)==ACT_CLOSE)
		{
			act_fun_printf("ACT_set_pos ACT CLOSED\n");
			STACKS_pull(stack_id);
		}
		else if (global.env.act[ACT_id_data(stack_id)].ready && ACT_position_data(stack_id)==ACT_PREPARE)
		{
			act_fun_printf("ACT_set_pos ACT PREPARED\n");
			STACKS_pull(stack_id);
		}
		else if(global.env.act[ACT_id_data(stack_id)].failure)
		{
			act_fun_printf("ACT_set_pos ACT FAILURE\n");
			STACKS_set_timeout(stack_id, ACT_SET_POS_TIMEOUT_TIME);
		}
		else if (global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(stack_id))
				 > (ACT_SET_POS_TIMEOUT_TIME))
		{
			act_fun_printf("ACT_set_pos ACT TIMEOUT\n");
			STACKS_set_timeout(stack_id,ACT_SET_POS_TIMEOUT_TIME);
		}
	}
}*/


//Fonction générique permettant de positionner les actionneurs
/*void ACT_push_set_pos(ACT_id_e act, ACT_position_e position, bool_e run)
{
	stack_id_e stack_id;
	act_arg_t* arg;

	if(act == BROOM_LEFT)
	{
			stack_id = ACT_STACK_BROOM_LEFT;
	}
	else if(act == BROOM_RIGHT)
	{
			stack_id = ACT_STACK_BROOM_RIGHT;
	}
	else
	{
			stack_id = ACT_STACK_F;
	}

	arg = &act_args[act][STACKS_get_top(stack_id)+1];
	arg->act_id = act,
	arg->position = position;
	STACKS_push(stack_id, &ACT_set_pos, run);
}*/


/* Fonctions empilables */
/* ATTENTION  : pas de variables statiques pour les fonctions qui sont éxécutées dans deux piles */

// Renvoie la position demandée
/*Uint16 ACT_position_data(stack_id_e stack_id)
{
	Uint16 data = 0xFFFF;
	act_arg_t* act_arg = NULL;
	ACT_id_e act=NULL;

	switch (stack_id)
	{
		case ACT_STACK_BROOM_LEFT:
			act=BROOM_LEFT;
			break;
		case ACT_STACK_BROOM_RIGHT:
			act=BROOM_RIGHT;
			break;
		case ACT_STACK_F:
			act = F;
			break;
		default : break;
	}

	act_arg = &act_args[act][STACKS_get_top(stack_id)];
	switch(act_arg->position)
	{
		case PREPARE:
//			data = ACT_PREPARE;
			break;
		case OPEN:
//			data = ACT_OPEN;
			break;
		case CLOSE:
//			data = ACT_CLOSE;
			break;
		default:
			break;
	}
	return data;
}*/

// Renvoie l'actionneur demandé
/*Uint8 ACT_id_data(stack_id_e stack_id)
{
	Uint8 data = 0xFF;

	switch (stack_id)
	{
		case ACT_STACK_BROOM_LEFT:
		//	data=ACT_BROOM_L;
			break;
		case ACT_STACK_BROOM_RIGHT:
//			data=ACT_BROOM_R;
			break;
		case ACT_STACK_F:
//			data = ACT_F;
			break;
		default : break;
	}
	return data;
}*/
