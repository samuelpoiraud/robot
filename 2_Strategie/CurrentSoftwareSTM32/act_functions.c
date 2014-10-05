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
#include "config/config_pin.h"

#include "config_debug.h"

#ifndef OUTPUT_LOG_COMPONENT_ACTFUNCTION
#  define OUTPUT_LOG_COMPONENT_ACTFUNCTION LOG_PRINT_Off
#  warning "OUTPUT_LOG_COMPONENT_ACTFUNCTION is not defined, defaulting to Off"
#endif

#define LOG_PREFIX "act_f: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"
#include "act_avoidance.h"
#include "QS/QS_who_am_i.h"


/* Pile contenant les arguments d'une demande d'op�ration
 * Contient les messages CAN � envoyer � la carte actionneur pour ex�cuter l'action.
 * fallbackMsg contient le message CAN lorsque l'op�ration demand�e par le message CAN msg ne peut pas �tre compl�t�e (bras bloqu�, robot adverse qui bloque l'actionneur par exemple)
 * On utilise une structure diff�rente de CAN_msg_t pour �conomiser la RAM (voir matrice act_args)
 *
 * Pour chaque fonction appel�e par le code strat�gie pour empiler une action, on remplit les messages CAN � envoyer et on empile l'action.
 * Un timeout est disponible si jamais la carte actionneur ne renvoie pas de message CAN ACT_RESULT (elle le devrait, mais on sait jamais)
 *
 * Le code de ACT_check_result() g�re le renvoi de message lorsque la carte actionneur indique qu'il n'y avait pas assez de ressource ou l'envoi du message fallback si la position demand� par le message CAN msg n'est pas atteignable.
 * Si le message fallback se solve par un echec aussi, on indique au code de strat�gie que cet actionneur n'est pas dispo maintenant et de r�essayer plus tard (il faut faire une autre strat pendant ce temps, si c'est un robot qui bloque le bras, il faut que l'environnement du jeu bouge)
 * Si le renvoi du message � cause du manque de ressource cause la m�me erreur, on marque l'actionneur comme inutilisable (ce cas est grave, il y a probablement un probl�me dans le code actionneur ou un flood de demande d'op�ration de la carte strat�gie)
 */


//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur g�n�re des resultats et d�taille les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) convertit les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) convertit et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.


#define ARM_TIMEOUT 2500

//FONCTIONS D'ACTIONNEURS


// HOLLY

bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_TORCH_LOCKER, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_TORCH_LOCKER, ACT_TORCH_LOCKER_STOP);

	debug_printf("Pushing torch locker Run cmd\n");

	ACT_AVOIDANCE_reset_actionneur(ACT_AVOID_TORCH_LOCKER);

	switch(cmd){
		case ACT_TORCH_Locker_Inside:
			ACT_AVOIDANCE_new_action(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Inside, TRUE);
			break;

		case ACT_TORCH_Locker_Lock:
			ACT_AVOIDANCE_new_action(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Lock, TRUE);
			break;

		case ACT_TORCH_Locker_Unlock:
			ACT_AVOIDANCE_new_action(ACT_AVOID_TORCH_LOCKER, ACT_AVOID_TORCH_Locker_Unlock, TRUE);
			break;
	}

	return ACT_push_operation(ACT_QUEUE_Torch_locker, &args);
}

bool_e ACT_fruit_mouth_goto(ACT_fruit_mouth_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_FRUIT_MOUTH, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_FRUIT_MOUTH, ACT_FRUIT_MOUTH_STOP);

	ACT_AVOIDANCE_reset_actionneur(ACT_AVOID_FRUIT_MOUTH);

	switch(cmd){
		case ACT_FRUIT_Verrin_Open:
			ACT_AVOIDANCE_new_action(ACT_AVOID_FRUIT_MOUTH, ACT_AVOID_FRUIT_MOUTH_Open, TRUE);
			break;

		case ACT_FRUIT_Verrin_Close:
			ACT_AVOIDANCE_new_action(ACT_AVOID_FRUIT_MOUTH, ACT_AVOID_FRUIT_MOUTH_Close, TRUE);
			break;
	}

	debug_printf("Pushing Fruit Run cmd\n");
	return ACT_push_operation(ACT_QUEUE_Fruit, &args);
}

bool_e ACT_fruit_labium_goto(ACT_fruit_labium_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_FRUIT_MOUTH, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_FRUIT_MOUTH, ACT_FRUIT_MOUTH_STOP);

	debug_printf("Pushing Fruit labium Run cmd\n");
	return ACT_push_operation(ACT_QUEUE_Fruit_labium, &args);
}

bool_e ACT_lance_launcher_run(ACT_lance_launcher_cmd_e cmd,Uint16 param){
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_LANCELAUNCHER, cmd, param);
	ACT_arg_set_fallbackmsg(&args, ACT_LANCELAUNCHER, ACT_LANCELAUNCHER_STOP);

	debug_printf("Pushing launcher Run %d cmd\n", cmd);
	return ACT_push_operation(ACT_QUEUE_launcher, &args);
}

bool_e ACT_filet_launch(ACT_filet_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_FILET, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_FILET, ACT_FILET_STOP);

	debug_printf("Pushing Filet run %d cmd\n", cmd);
	return ACT_push_operation(ACT_QUEUE_Filet, &args);
}

bool_e ACT_small_arm_goto(ACT_small_arm_cmd_e cmd){
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_SMALL_ARM, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_SMALL_ARM, ACT_SMALL_ARM_STOP);

	if(QS_WHO_AM_I_get() == SMALL_ROBOT){
		ACT_AVOIDANCE_reset_actionneur(ACT_AVOID_SMALL_ARM);

		switch(cmd){
			case ACT_Small_arm_Idle:
				ACT_AVOIDANCE_new_action(ACT_AVOID_SMALL_ARM, ACT_AVOID_SMALL_ARM_Idle, TRUE);
				break;

			case ACT_Small_arm_Mid:
				ACT_AVOIDANCE_new_action(ACT_AVOID_SMALL_ARM, ACT_AVOID_SMALL_ARM_Mid, TRUE);
				break;

			case ACT_Small_arm_Deployed:
				ACT_AVOIDANCE_new_action(ACT_AVOID_SMALL_ARM, ACT_AVOID_SMALL_ARM_Deployed, TRUE);
				break;
		}
	}

	debug_printf("Pushing Small Arm run %d cmd\n", cmd);
	return ACT_push_operation(ACT_QUEUE_Small_arm, &args);
}

bool_e ACT_pompe_order(ACT_pompe_cmd_e cmd, Uint8 param){
	QUEUE_arg_t args;

	if(cmd == ACT_Pompe_Normal || cmd == ACT_Pompe_Reverse)
		ACT_arg_init_with_param(&args, ACT_POMPE, cmd, param);
	else
		ACT_arg_init(&args, ACT_POMPE, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_POMPE, ACT_POMPE_STOP);

	debug_printf("Pushing Pompe run %d cmd\n", cmd);
	return ACT_push_operation(ACT_QUEUE_Pompe, &args);
}

bool_e ACT_arm_goto(ARM_state_e position) {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_ARM, ACT_ARM_GOTO, position);

	debug_printf("Pushing ARM goto %d\n", position);
	return ACT_push_operation(ACT_QUEUE_Arm, &args);
}

bool_e ACT_arm_goto_XY(ARM_state_e position, Sint16 x, Sint16 y){
	QUEUE_arg_t args;
	ACT_can_msg_t msg;
	msg.sid = ACT_ARM;
	msg.data[0]=ACT_ARM_GOTO;
	msg.data[1]=LOWINT(position);
	msg.data[2]=HIGHINT(position);
	msg.data[3]=HIGHINT(x);
	msg.data[4]=LOWINT(x);
	msg.data[5]=HIGHINT(y);
	msg.data[6]=LOWINT(y);
	msg.size = 7;

	ACT_arg_init_with_msg(&args, msg);

	debug_printf("Pushing ARM gotoXY %d / x : %d    y : %d\n", position, x, y);
	return ACT_push_operation(ACT_QUEUE_Arm, &args);
}

bool_e ACT_arm_updown_goto(Sint16 height){
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_ARM, ACT_ARM_UPDOWN_GOTO, height);

	debug_printf("Pushing Arm updown to z : %d mm \n", height);
	return ACT_push_operation(ACT_QUEUE_Arm, &args);
}

bool_e ACT_arm_updown_rush_in_the_floor(Sint16 height){
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_ARM, ACT_ARM_UPDOWN_RUSH_IN_FLOOR, height);

	debug_printf("Pushing Arm updown rush in the floor to z : %d mm \n", height);
	return ACT_push_operation(ACT_QUEUE_Arm, &args);
}

error_e ACT_arm_move(ARM_state_e state_arm, Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;

		if(state_arm == ACT_ARM_POS_ON_TORCHE || state_arm == ACT_ARM_POS_ON_TRIANGLE)
			ACT_arm_goto_XY(state_arm, x, y);
		else
			ACT_arm_goto(state_arm);


		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

error_e ACT_small_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;
		ACT_small_arm_goto(state_arm);
		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

error_e ACT_elevator_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;
		ACT_arm_updown_goto(state_arm);
		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

error_e ACT_elevator_arm_rush_in_the_floor(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;
		ACT_arm_updown_rush_in_the_floor(state_arm);
		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

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
		case ACT_FRUIT_MOUTH :
			queue_id = ACT_QUEUE_Fruit;
			//queue_id = ACT_QUEUE_Fruit_labium;
			debug_printf("Config : ACT_FRUIT_MOUTH\n");
			break;

		case ACT_LANCELAUNCHER :
			queue_id = ACT_QUEUE_launcher;
			debug_printf("Config : ACT_LANCELAUNCHER\n");
			break;

		case ACT_TORCH_LOCKER :
			queue_id = ACT_QUEUE_Torch_locker;
			debug_printf("Config : ACT_TORCH_LOCKER\n");
			break;

		case ACT_FILET :
			queue_id = ACT_QUEUE_Filet;
			debug_printf("Config : ACT_FILET\n");
			break;

		case ACT_SMALL_ARM :
			queue_id = ACT_QUEUE_Small_arm;
			debug_printf("Config : ACT_SMALL_ARM\n");
			break;

		case ACT_POMPE :
			queue_id = ACT_QUEUE_Pompe;
			debug_printf("Config : ACT_POMPE\n");
			break;

		case ACT_ARM :
			queue_id = ACT_QUEUE_Arm;
			debug_printf("Config : ACT_ARM\n");
			break;

		default :
			warn_printf("ACT_CONFIG : sid de l'actionneur introuvable\n");
			return FALSE;
	}
	debug_printf("    cmd : %d\n", cmd);
	debug_printf("    value : %d\n", value);

	return ACT_push_operation(queue_id, &args);
}
