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

/* Pile contenant les arguments d'une demande d'opération
 * Contient les messages CAN à envoyer à la carte actionneur pour exécuter l'action.
 * fallbackMsg contient le message CAN lorsque l'opération demandé par le message CAN msg ne peut pas être complété (bras bloqué, robot adverse qui bloque l'actionneur par exemple)
 * On utilise une structure différente de CAN_msg_t pour économiser la RAM (voir matrice act_args)
 *
 * Pour chaque fonction appelée par le code stratégie pour empiler une action, on remplie les messages CAN à enovoyer et on empile l'action.
 * Un timeout est disponible si jamais la carte actionneur ne renvoie pas de message CAN ACT_RESULT (elle le devrait, mais on sait jamais)
 *
 * Le code de ACT_check_result() gère le renvoi de message lorsque la carte actionneur indique qu'il n'y avait pas assez de ressource ou l'envoi du message fallback si la position demandé par le message CAN msg n'est pas atteignable.
 * Ce le message fallback se solve par un echec aussi, on indique le code de stratégie que cet actionneur n'est pas dispo maintenant et de réessayer plus tard (il faut faire une autre strat pendant ce temps, si c'est un robot qui bloque le bras, il faut que l'environnement du jeu bouge)
 * Si le renvoi du message à cause du manque de ressource cause la même erreur, on marque l'actionneur comme inutilisable (ce cas est grave, il y a probablement un problème dans le code actionneur ou un flood de demande d'opération de la carte stratégie)
 */

//Version allégé pour nos besoin d'un message CAN, histoire de pas prendre trop de RAM dans la variable act_args ...
typedef struct {
	Uint11 sid;
	Uint8 data[3];
	Uint8 size;
} ACT_can_msg_t;

//Pour timeout: utiliser ACT_ARG_NOTIMEOUT pour ne pas mettre de timeout
typedef struct {
	Uint16 timeout;         //Temps avant timeout de l'action en ms (relatif au démarrage de l'execution de l'action).
	ACT_can_msg_t msg;          //Message à envoyer pour executer l'action
	ACT_can_msg_t fallbackMsg;  //Quand on ne peut pas aller a la position demandé, envoyer ce message. Utilisé pour ACT_BEHAVIOR_GoalUnreachable. Si le SID est ACT_ARG_NOFALLBACK_SID, aucun message de sera envoyé et l'erreur indiqué au code de strat sera de reessayer la la strat qui utilise cette commande plus tard (ACT_FUNCTION_RetryLater).
} act_arg_t;

#define ACT_ARG_NOFALLBACK_SID 0xFFF
#define ACT_ARG_NOTIMEOUT ((time32_t)-1)
#define ACT_ARG_USE_DEFAULT 0
#define ACT_STACK_TIMEOUT_MS 3000

#ifdef ACT_NO_ERROR_HANDLING
	#warning "La gestion d'erreur des actionneurs est désactivée ! (voir act_function.c, constante: ACT_NO_ERROR_HANDLING)"
#endif


static act_arg_t act_args[ACTUATORS_NB][STACKS_SIZE];
static ACT_function_result_e act_states[ACTUATORS_NB];	//resultat de la dernière action exécuté pour chaque pile

// Verifie s'il y a eu un timeout et met à jour l'état de la pile en conséquence.
// Si l'état de l'actionneur n'est pas normal ou qu'il y a eu une erreur, cette fonction affiche un message d'erreur
static void ACT_check_result(stack_id_e act_id);

static bool_e ACT_push_operation(stack_id_e act_id, act_arg_t* arg, bool_e run_now);
static void ACT_run_operation(stack_id_e stack_id, bool_e init);

/* Accesseur en lecture sur les arguments des piles ACT */
static act_arg_t* ACT_get_stack_arg(stack_id_e act_id);
/* Accesseur en écriture sur les arguments des piles ACT */
static void ACT_set_stack_arg(stack_id_e act_id, const act_arg_t* arg);

ACT_function_result_e ACT_get_last_action_result(stack_id_e act_id) {
	assert(act_id < ACTUATORS_NB);
	return act_states[act_id];
}


//FONCTIONS D'ACTIONNEURS PUBLIQUES
// <editor-fold desc="Krusty">

bool_e ACT_push_ball_launcher_run(Uint16 speed, bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_BALLLAUNCHER;
	args.msg.data[0] = ACT_BALLLAUNCHER_ACTIVATE;
	args.msg.data[1] = LOWINT(speed);
	args.msg.data[2] = HIGHINT(speed);
	args.msg.size = 3;

	args.fallbackMsg.sid = ACT_BALLLAUNCHER;
	args.fallbackMsg.data[0] = ACT_BALLLAUNCHER_STOP;
	args.fallbackMsg.size = 1;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing BallLauncher Run cmd, speed: %u\n", speed);
	return ACT_push_operation(ACT_STACK_BallLauncher, &args, run);
}

bool_e ACT_push_ball_launcher_stop(bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_BALLLAUNCHER;
	args.msg.data[0] = ACT_BALLLAUNCHER_STOP;
	args.msg.size = 1;

	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing BallLauncher Stop cmd\n");
	return ACT_push_operation(ACT_STACK_BallLauncher, &args, run);
}


bool_e ACT_push_plate_rotate_horizontally(bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_PLATE;
	args.msg.data[0] = ACT_PLATE_ROTATE_HORIZONTALLY;
	args.msg.size = 1;

	//Si on ne peut pas aller en position horizontale, revenir en vertical et la strat pourra passer à autre chose (au lieu de défoncer le décor)
	args.fallbackMsg.sid = ACT_PLATE;
	args.fallbackMsg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
	args.fallbackMsg.size = 1;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Plate rotate vertically cmd\n");
	return ACT_push_operation(ACT_STACK_Plate, &args, run);
}

bool_e ACT_push_plate_rotate_prepare(bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_PLATE;
	args.msg.data[0] = ACT_PLATE_ROTATE_PREPARE;
	args.msg.size = 1;

	//Si on ne peut pas aller en position intermediaire, revenir en vertical et la strat pourra passer à autre chose (au lieu de défoncer le décor)
	args.fallbackMsg.sid = ACT_PLATE;
	args.fallbackMsg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
	args.fallbackMsg.size = 1;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Plate rotate vertically cmd\n");
	return ACT_push_operation(ACT_STACK_Plate, &args, run);
}

bool_e ACT_push_plate_rotate_vertically(bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_PLATE;
	args.msg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
	args.msg.size = 1;

	//Que faire si on ne peut pas se replier ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Plate rotate horizontally Stop cmd\n");
	return ACT_push_operation(ACT_STACK_Plate, &args, run);
}

// </editor-fold>

// <editor-fold desc="Tiny">
bool_e ACT_push_hammer_goto(Uint16 position, bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_HAMMER;
	args.msg.data[0] = ACT_HAMMER_MOVE_TO;
	args.msg.data[1] = LOWINT(position);
	args.msg.data[2] = HIGHINT(position);
	args.msg.size = 3;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer moveto cmd\n");
	return ACT_push_operation(ACT_STACK_Hammer, &args, run);
}

bool_e ACT_push_hammer_stop(bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_HAMMER;
	args.msg.data[0] = ACT_HAMMER_STOP;
	args.msg.size = 1;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer stop asser cmd\n");
	return ACT_push_operation(ACT_STACK_Hammer, &args, run);
}

bool_e ACT_push_ball_inflater_inflate(Uint8 duration_sec, bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_BALLINFLATER;
	args.msg.data[0] = ACT_BALLINFLATER_START;
	args.msg.data[1] = duration_sec;
	args.msg.size = 2;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_BALLINFLATER;
	args.fallbackMsg.data[0] = ACT_BALLINFLATER_STOP;
	args.fallbackMsg.size = 1;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer stop asser cmd\n");
	return ACT_push_operation(ACT_STACK_BallInflater, &args, run);
}

bool_e ACT_push_ball_inflater_stop(bool_e run) {
	act_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_BALLINFLATER;
	args.msg.data[0] = ACT_BALLINFLATER_STOP;
	args.msg.size = 1;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer stop asser cmd\n");
	return ACT_push_operation(ACT_STACK_BallInflater, &args, run);
}

// </editor-fold>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gestion des actions et de leur résultat (privé)
// <editor-fold defaultstate="collapsed">

//Prépare une action correctement et l'envoie sur la pile
static bool_e ACT_push_operation(stack_id_e act_id, act_arg_t* args, bool_e run_now) {
#ifndef ACT_NEVER_DISABLE
	if(global.env.act[act_id].disabled == TRUE) {
		OUTPUTLOG_printf(LOG_LEVEL_Info, LOG_PREFIX"Ignoring push_operation, act %d is disabled\n", act_id);
		return FALSE;
	}
#endif

	ACT_set_stack_arg(act_id, args);
	STACKS_push(act_id, ACT_run_operation, run_now);

	return TRUE;
}

//Gère le déroulement d'une action coté strat: initialisation = envoi du message CAN de l'action, suite = vérification du résultat si reçu (par msg CAN) ou si timeout (géré ici en strat, si jamais quelque chose ne va pas en actionneur)
//Cette fonction est appelée par le module STACKS, STACKS_run en particulié quand init vaut FALSE
static void ACT_run_operation(stack_id_e act_id, bool_e init) {
	if(init) {
		ACT_can_msg_t* command = &(ACT_get_stack_arg(act_id)->msg);
		CAN_msg_t msg;

		global.env.act[ACT_STACK_BallLauncher].operationResult = ACT_RESULT_Working;
		act_states[act_id] = ACT_FUNCTION_InProgress;

		OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Sending operation, act_id: %d, sid: 0x%x, size: %d, data[0]: 0x%x, data[1]: 0x%x, data[2]: 0x%x\n", act_id, command->sid, command->size, command->data[0], command->data[1], command->data[2]);

		//Copie des élements de notre structure à celle accepté par CAN_send, la notre est plus petite pour économiser la RAM
		msg.sid     = command->sid;
		msg.data[0] = command->data[0];
		msg.data[1] = command->data[1];
		msg.data[2] = command->data[2];
		msg.size    = command->size;
		CAN_send(&msg);

	} else {
		ACT_check_result(act_id);
	}
}

//Gère l'état d'une opération.
//Pour les renvois de messages on ne reinitialise pas le timeout histoire d'eviter les boucles si il y en a
static void ACT_check_result(stack_id_e act_id) {
	act_arg_t* argument = ACT_get_stack_arg(act_id);

	//L'état à déja été géré, mais l'erreur n'a pas encore été géré par le reste du code stratégie
	//Si c'est ACT_FUNCTION_Done on ne passe pas ici (a moins d'un gros problème de conception / amnésie
	if(act_states[act_id] != ACT_FUNCTION_InProgress) {
		if(act_states[act_id] == ACT_FUNCTION_Done)
			OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Begin check but already in Done state, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
		return;
	}

	if(global.env.match_time >= argument->timeout) {
		OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation timeout (by strat) act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
		global.env.act[act_id].disabled = TRUE;
		act_states[act_id] = ACT_FUNCTION_ActDisabled;
		STACKS_set_timeout(act_id, TRUE);
	} else {
#ifdef ACT_NO_ERROR_HANDLING
		global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
		act_states[act_id] = ACT_FUNCTION_Done;
		STACKS_pull(act_id);
#else
		switch(global.env.act[act_id].operationResult) {
			case ACT_RESULT_Failed:
				switch(global.env.act[act_id].recommendedBehavior) {

					case ACT_BEHAVIOR_GoalUnreachable:	//Envoyer le message fallback, s'il redonne une erreur, ACT_process_result s'occupera de désactiver l'actionneur
						if(argument->fallbackMsg.sid != ACT_ARG_NOFALLBACK_SID) {
							CAN_msg_t msg;

							global.env.act[act_id].operationResult = ACT_RESULT_Working;
							OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"GoalUnreachable, sending fallback message, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);

							msg.sid     = argument->fallbackMsg.sid;
							msg.data[0] = argument->fallbackMsg.data[0];
							msg.data[1] = argument->fallbackMsg.data[1];
							msg.data[2] = argument->fallbackMsg.data[2];
							msg.size    = argument->fallbackMsg.size;
							CAN_send(&msg);
							//On ne change pas act_states[act_id] car on n'a pas encore terminé avec l'opération
						} else {	//Si on n'a pas de message fallback, (car par exemple, ça aurait été le même message) on déclare une erreur indiquant d'essayer plus tard
							OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"GoalUnreachable, no fallback message to send, disabling act, act id: %u\n", act_id);
							//global.env.act[act_id].disabled = TRUE;
							act_states[act_id] = ACT_FUNCTION_RetryLater;
							STACKS_set_timeout(act_id, TRUE);
						}
						break;

					case ACT_BEHAVIOR_RetryLater: {
							static time32_t waitMsCount = 0;
							if(waitMsCount == 0) {
								OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"RetryLater, will retry 10ms later, act id: %u\n", act_id);
								waitMsCount = global.env.match_time + 10;	//Attendre 3ms avant de ressayer
							}else if(global.env.match_time >= waitMsCount) {  //Après un certain temps défini juste avant, renvoyer la commande en espérant que ça passe cette fois
								CAN_msg_t msg;

								waitMsCount = 0;
								global.env.act[act_id].operationResult = ACT_RESULT_Working;
								OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"RetryLater, time to resend message, act id: %u\n", act_id);

								msg.sid     = argument->msg.sid;
								msg.data[0] = argument->msg.data[0];
								msg.data[1] = argument->msg.data[1];
								msg.data[2] = argument->msg.data[2];
								msg.size    = argument->msg.size;
								CAN_send(&msg);
							}	//On ne change pas act_states[act_id] car on n'a pas encore terminé avec l'opération
						}
						break;

					default:	//Cas inexistant
						OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation failed but behavior is Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
						//pas de break ici, si la raison n'est pas connue de ce code, considerer l'erreur comme grave et desactiver l'actionneur
					case ACT_BEHAVIOR_DisableAct:
						OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Bad act behavior, act disabled, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
						global.env.act[act_id].disabled = TRUE;
						act_states[act_id] = ACT_FUNCTION_ActDisabled;
						STACKS_set_timeout(act_id, TRUE);
						break;
				}
				break;

			case ACT_RESULT_Ok:
				if(global.env.act[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//Dans ce cas, le result_ok  est en fait celui de la commande de déplacement a la position fallback, donc indiquer qu'il y a eu un problème pour atteindre la position et de reessayer plus tard
					OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Fallback position Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);
					global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id] = ACT_FUNCTION_RetryLater;
					STACKS_set_timeout(act_id, TRUE);
				} else {	//Sinon c'est bien la commande voulue qui s'est bien terminée
					OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Operation Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
					global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id] = ACT_FUNCTION_Done;
					STACKS_pull(act_id);
				}
				break;

			case ACT_RESULT_Working:	//Pas encore fini l'opération, on a pas reçu de message de la carte actionneur
				break;

			case ACT_RESULT_Idle:		//Ne devrait jamais arriver, sinon erreur dans le code
			default:
				OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Warning: act should be in working/finished mode but wasn't, mode: %d, act id: %u, sid: 0x%x, cmd: 0x%x\n", global.env.act[act_id].operationResult, act_id, argument->msg.sid, argument->msg.data[0]);
				STACKS_set_timeout(act_id, TRUE);
				break;
		}
#endif /* not def ACT_NO_ERROR_HANDLING */
	}
}

/* Accesseur sur les arguments des piles ACT */
static act_arg_t* ACT_get_stack_arg(stack_id_e act_id) {
	return &act_args[act_id][STACKS_get_top(act_id)];
}

// Accesseur en écriture sur les arguments des piles ACT
// arg->timeout doit contenir le timeout relatif, ACT_ARG_USE_DEFAULT pour le temps par défaut (ACT_STACK_TIMEOUT_MS)
static void ACT_set_stack_arg(stack_id_e act_id, const act_arg_t* arg) {
	Uint8 stack_top_pos = STACKS_get_top(act_id)+1;

	act_args[act_id][stack_top_pos] = *arg;
	if(arg->timeout != ACT_ARG_USE_DEFAULT)
		act_args[act_id][stack_top_pos].timeout = global.env.match_time + arg->timeout;
	else act_args[act_id][stack_top_pos].timeout = global.env.match_time + ACT_STACK_TIMEOUT_MS;
}

// Gere les messages CAN ACT_RESULT contenant des infos sur le déroulement d'une action
void ACT_process_result(const CAN_msg_t* msg) {
	stack_id_e act_id = ACTUATORS_NB;

	assert(msg->sid == ACT_RESULT);

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Received result: act: %u, cmd: 0x%x, result: %u, reason: %u\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3]);

	switch(msg->data[0]) {
		//Krusty
		case ACT_BALLLAUNCHER & 0xFF:
			act_id = ACT_STACK_BallLauncher;
			break;

		case ACT_PLATE & 0xFF:
			act_id = ACT_STACK_Plate;
			break;

		//Tiny
		case ACT_HAMMER & 0xFF:
			act_id = ACT_STACK_Hammer;
			break;

		case ACT_BALLINFLATER & 0xFF:
			act_id = ACT_STACK_BallInflater;
			break;
	}

	if(act_id >= ACTUATORS_NB) {
		OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Unknown act result, can_act_id: 0x%x, can_result: %u\n", msg->data[0], msg->data[2]);
		return;
	}

	//Erreur de codage, ça ne devrait jamais arriver
	if(global.env.act[act_id].operationResult != ACT_RESULT_Working) {
		OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"act is not in working mode but received result, act: 0x%x, cmd: 0x%x, result: %u, reason: %u, mode: %d\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3], global.env.act[act_id].operationResult);
	}

#ifdef ACT_NO_ERROR_HANDLING
	global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
	global.env.act[act_id].operationResult = ACT_RESULT_Ok;
#else
	switch(msg->data[2]) {
		case ACT_RESULT_DONE:
			//On n'affecte pas global.env.act[act_id].recommendedBehavior pour garder une trace des erreurs précédentes (dans le cas ou on a renvoyé une commande par exemple, permet de savoir l'erreur d'origine)
			//global.env.act[act_id].recommendedBehavior est affecté à ACT_BEHAVIOR_Ok dans
			global.env.act[act_id].operationResult = ACT_RESULT_Ok;
			break;

		default:	//ACT_RESULT_NOT_HANDLED et ACT_RESULT_FAILED (et les autres si ajouté)
			global.env.act[act_id].operationResult = ACT_RESULT_Failed;
			switch(msg->data[3]) {
				case ACT_RESULT_ERROR_OK:
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Reason Ok with result Failed, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
					break;

				case ACT_RESULT_ERROR_OTHER: //Keep last error, do nothing
					break;

				case ACT_RESULT_ERROR_TIMEOUT:
				case ACT_RESULT_ERROR_NOT_HERE:
					if(global.env.act[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//On a déjà reçu cette erreur, aucun déplacement possible de l'actionneur -> désactivation
						global.env.act[act_id].disabled = TRUE;
						global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_GoalUnreachable;
					OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Goal unreachable ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);
					break;

				case ACT_RESULT_ERROR_NO_RESOURCES:
					if(global.env.act[act_id].recommendedBehavior == ACT_BEHAVIOR_RetryLater) {	//Bug surement dans la carte actionneur, il n'y a pas assez de ressources pour gérer la commande
						global.env.act[act_id].disabled = TRUE;
						global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_RetryLater;
					OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"NoResource error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Notifier le cas, il faudra par la suite augmenter les resources dispo ...
					break;

				case ACT_RESULT_ERROR_LOGIC:
					global.env.act[act_id].disabled = TRUE;
					global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Logic error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Ceci est un moment WTF. A résoudre le plus rapidement possible.
					break;

				default:
					global.env.act[act_id].disabled = TRUE;
					global.env.act[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Unknown error ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);	//Ceci est un moment WTF. A résoudre le plus rapidement possible.
			}
			break;
	}
#endif /* not def ACT_NO_ERROR_HANDLING */
}

// </editor-fold>
