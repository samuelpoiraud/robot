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


//FONCTIONS D'ACTIONNEURS

////////////////////////////////////////
//////////////// HOLLY /////////////////
////////////////////////////////////////

/*bool_e ACT_torch_locker(ACT_torch_locker_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_TORCH_LOCKER, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_TORCH_LOCKER, ACT_TORCH_LOCKER_STOP);

	debug_printf("Pushing torch locker Run cmd\n");

	ACT_AVOIDANCE_new_classic_cmd(ACT_AVOID_TORCH_LOCKER, cmd);

	return ACT_push_operation(ACT_QUEUE_Torch_locker, &args);
}*/

////////////////////////////////////////
//////////////// WOOD //////////////////
////////////////////////////////////////
bool_e ACT_pince_gauche(ACT_pince_gauche_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PINCE_GAUCHE, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_PINCE_GAUCHE,  ACT_PINCE_GAUCHE_STOP);

	return ACT_push_operation(ACT_QUEUE_Pince_Gauche, &args);
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
