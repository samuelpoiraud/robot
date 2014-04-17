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


// PIERRE

bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_TORCH_LOCKER, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_TORCH_LOCKER, ACT_TORCH_LOCKER_STOP);

	debug_printf("Pushing torch locker Run cmd\n");
	return ACT_push_operation(ACT_QUEUE_Torch_locker, &args);
}

bool_e ACT_fruit_mouth_goto(ACT_fruit_mouth_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_FRUIT_MOUTH, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_FRUIT_MOUTH, ACT_FRUIT_MOUTH_STOP);

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
