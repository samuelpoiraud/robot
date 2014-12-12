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


//FONCTIONS D'ACTIONNEURS

////////////////////////////////////////
//////////////// HOLLY /////////////////
////////////////////////////////////////

/*bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_TORCH_LOCKER, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_TORCH_LOCKER, ACT_TORCH_LOCKER_STOP);

	debug_printf("Pushing torch locker Run cmd\n");

*******   ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_TORCH_LOCKER, cmd); ***    <----  Prend en compte la position de l'actionneur pour l'�vitement

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

bool_e ACT_pince_devant(ACT_pince_devant_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PINCE_DEVANT, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_PINCE_DEVANT,  ACT_PINCE_DEVANT_STOP);

	return ACT_push_operation(ACT_QUEUE_Pince_Devant, &args);
}

bool_e ACT_clap(ACT_clap_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CLAP, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_CLAP,  ACT_CLAP_STOP);

	return ACT_push_operation(ACT_QUEUE_Clap, &args);
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
