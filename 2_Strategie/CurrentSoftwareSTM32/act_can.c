#include "act_can.h"
#include "avoidance.h"
#include "QS/QS_CANmsgList.h"
#include "config_use.h"

#include "config_debug.h"
#define LOG_PREFIX "act_c: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"

//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur génère des resultats et détail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) converti et gère les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour être ensuite utilisé par le reste du code stratégie.

#ifdef ACT_NO_ERROR_HANDLING
	#warning "La gestion d'erreur des actionneurs est désactivée ! (voir act_function.c/h, constante: ACT_NO_ERROR_HANDLING)"
#endif

#define ACT_STACK_TIMEOUT_MS 3000

typedef enum {
	ACT_RESULT_Idle,	//Etat au démarage, par la suite ce sera le resultat de la dernière opération effectuée
	ACT_RESULT_Working,	//L'opération n'est pas terminée
	ACT_RESULT_Ok,		//L'opération s'est terminée correctement
	ACT_RESULT_Failed	//Une erreur est survenue, voir details dans act_error_recommended_behavior_e
} act_result_e;

typedef enum {
	ACT_BEHAVIOR_Ok,                       //Pas d'erreur (ACT_RESULT_ERROR_OK)
	ACT_BEHAVIOR_DisableAct,               //L'actionneur est inutilisable
	ACT_BEHAVIOR_RetryLater,               //Ressayer plus tard les commandes, si arrive plusieurs fois de suite, passer à ACT_ERROR_DisableAct (ACT_RESULT_ERROR_NO_RESOURCES)
	ACT_BEHAVIOR_GoalUnreachable           //ACT_RESULT_ERROR_TIMEOUT la première fois. Tenter de retourner à la position en mode non-déployé. Si une autre erreur survient,passer à ACT_ERROR_DisableAct Sinon essayer la strat qui l'utilise plus tard, et si l'erreur revient, refaire la procédure mais au lieu de ressayer la strat plus tard, desactiver l'actionneur.
										   //Si ACT_RESULT_ERROR_NOT_HERE, envoyer une commande de retour en mode déployé et passer directement à ACT_ERROR_DisableAct. Esperons que l'actionneur reçevra la commande et restera pas déployé.
} act_error_recommended_behavior_e;

typedef struct {
	bool_e disabled;	//TRUE si l'actionneur est désactivé car inutilisable
	act_result_e operationResult;	//Resultat de la derrnière opération faite
	act_error_recommended_behavior_e recommendedBehavior;		//Quoi faire suite à l'opération faite
	ACT_function_result_e lastResult;
} act_state_info_t;

static act_state_info_t act_states[NB_QUEUE];  //Info lié a chaque actionneur

// Verifie s'il y a eu un timeout et met à jour l'état de la pile en conséquence.
// Si l'état de l'actionneur n'est pas normal ou qu'il y a eu une erreur, cette fonction affiche un message d'erreur
static void ACT_check_result(queue_id_e act_id);

static void ACT_run_operation(queue_id_e act_id, bool_e init);


ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id) {
	assert(act_id < NB_QUEUE);
#ifdef MODE_SIMULATION
	return ACT_FUNCTION_Done;	//En mode simulation, l'action demandée se termine immédiatement.
#endif
	return act_states[act_id].lastResult;
}

void ACT_arg_init(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd) {
	arg->msg.sid = sid;
	arg->msg.data[0] = cmd;
	arg->msg.size = 1;
	arg->fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;
	arg->timeout = ACT_ARG_USE_DEFAULT;
}

void ACT_arg_init_with_param(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param)  {
	arg->msg.sid = sid;
	arg->msg.data[0] = cmd;
	arg->msg.data[1] = LOWINT(param);
	arg->msg.data[2] = HIGHINT(param);
	arg->msg.size = 3;
	arg->fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;
	arg->timeout = ACT_ARG_USE_DEFAULT;
}

void ACT_arg_set_timeout(QUEUE_arg_t* arg, Uint16 timeout) {
	arg->timeout = timeout;
}

void ACT_arg_set_fallbackmsg(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd)  {
#ifndef ACT_DONT_TRY_FALLBACK
	arg->fallbackMsg.sid = sid;
	arg->fallbackMsg.data[0] = cmd;
	arg->fallbackMsg.size = 1;
#endif
}


void ACT_arg_set_fallbackmsg_with_param (QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param)  {
#ifndef ACT_DONT_TRY_FALLBACK
	arg->fallbackMsg.sid = sid;
	arg->fallbackMsg.data[0] = cmd;
	arg->fallbackMsg.data[1] = LOWINT(param);
	arg->fallbackMsg.data[2] = HIGHINT(param);
	arg->fallbackMsg.size = 3;
#endif
}


//Prépare une action correctement et l'envoie sur la pile
bool_e ACT_push_operation(queue_id_e act_id, QUEUE_arg_t* args) {
#ifndef ACT_NEVER_DISABLE
	if(act_states[act_id].disabled == TRUE) {
		COMPONENT_log(LOG_LEVEL_Info, "Ignoring push_operation, act %d is disabled\n", act_id);
		return FALSE;
	}
#endif

	if(args->timeout == ACT_ARG_USE_DEFAULT)
		args->timeout = ACT_STACK_TIMEOUT_MS;

	QUEUE_add(act_id, &ACT_run_operation, *args);

	return TRUE;
}

//Gère le déroulement d'une action coté strat: initialisation = envoi du message CAN de l'action, suite = vérification du résultat si reçu (par msg CAN) ou si timeout (géré ici en strat, si jamais quelque chose ne va pas en actionneur)
//Cette fonction est appelée par le module QUEUE, QUEUE_run en particulié quand init vaut FALSE
static void ACT_run_operation(queue_id_e act_id, bool_e init) {
	if(init) {
		ACT_can_msg_t* command = &(QUEUE_get_arg(act_id)->msg);
		CAN_msg_t msg;

		act_states[act_id].operationResult = ACT_RESULT_Working;
		act_states[act_id].lastResult = ACT_FUNCTION_InProgress;

		debug_printf("Sending operation, act_id: %d, sid: 0x%x, size: %d, data[0]: 0x%x, data[1]: 0x%x, data[2]: 0x%x\n", act_id, command->sid, command->size, command->data[0], command->data[1], command->data[2]);

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
static void ACT_check_result(queue_id_e act_id) {
	QUEUE_arg_t* argument = QUEUE_get_arg(act_id);

	//L'opération est terminée mais on passe encore ici, ya t-il un bug ?
	//Si ce bug arrive, il est probablement lié à l'init de l'action (qui définie lastResult à ACT_FUNCTION_InProgress)
	if(act_states[act_id].lastResult != ACT_FUNCTION_InProgress) {
		fatal_printf("Begin check but not in ACT_FUNCTION_InProgress state, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
		QUEUE_next(act_id);
		return;
	}

#ifdef ACT_NO_ERROR_HANDLING
		act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
		act_states[act_id].lastResult = ACT_FUNCTION_Done;
		QUEUE_next(act_id);
#else
	if(global.env.match_time >= argument->timeout + QUEUE_get_initial_time(act_id)) {
		error_printf("Operation timeout (by strat) act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
		act_states[act_id].disabled = TRUE;
		act_states[act_id].lastResult = ACT_FUNCTION_ActDisabled;
		QUEUE_set_error(act_id, TRUE);
		QUEUE_next(act_id);
	} else {
		switch(act_states[act_id].operationResult) {
			case ACT_RESULT_Failed:
				switch(act_states[act_id].recommendedBehavior) {

					case ACT_BEHAVIOR_GoalUnreachable:	//Envoyer le message fallback, s'il redonne une erreur, ACT_process_result s'occupera de désactiver l'actionneur
						if(argument->fallbackMsg.sid != ACT_ARG_NOFALLBACK_SID) {
							CAN_msg_t msg;

							act_states[act_id].operationResult = ACT_RESULT_Working;
							debug_printf("GoalUnreachable, sending fallback message, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);

							msg.sid     = argument->fallbackMsg.sid;
							msg.data[0] = argument->fallbackMsg.data[0];
							msg.data[1] = argument->fallbackMsg.data[1];
							msg.data[2] = argument->fallbackMsg.data[2];
							msg.size    = argument->fallbackMsg.size;
							CAN_send(&msg);
							//On ne change pas act_states[act_id] car on n'a pas encore terminé avec l'opération
						} else {	//Si on n'a pas de message fallback, (car par exemple, ça aurait été le même message) on déclare une erreur indiquant d'essayer plus tard
							debug_printf("GoalUnreachable, no fallback message to send, disabling act, act id: %u\n", act_id);
							//act_states[act_id].disabled = TRUE;
							act_states[act_id].lastResult = ACT_FUNCTION_RetryLater;
							QUEUE_set_error(act_id, TRUE);
							QUEUE_next(act_id);
						}
						break;

					case ACT_BEHAVIOR_RetryLater: {
							static time32_t waitMsCount = 0;
							if(waitMsCount == 0) {
								debug_printf("RetryLater, will retry 10ms later, act id: %u\n", act_id);
								waitMsCount = global.env.match_time + 10;	//Attendre 3ms avant de ressayer
							}else if(global.env.match_time >= waitMsCount) {  //Après un certain temps défini juste avant, renvoyer la commande en espérant que ça passe cette fois
								CAN_msg_t msg;

								waitMsCount = 0;
								act_states[act_id].operationResult = ACT_RESULT_Working;
								debug_printf("RetryLater, time to resend message, act id: %u\n", act_id);

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
						error_printf("Operation failed but behavior is Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
						//pas de break ici, si la raison n'est pas connue de ce code, considerer l'erreur comme grave et desactiver l'actionneur
						//no break
					case ACT_BEHAVIOR_DisableAct:
						warn_printf("Bad act behavior, act disabled, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
						act_states[act_id].disabled = TRUE;
						act_states[act_id].lastResult = ACT_FUNCTION_ActDisabled;
						QUEUE_set_error(act_id, TRUE);
						QUEUE_next(act_id);
						break;
				}
				break;

			case ACT_RESULT_Ok:
				if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//Dans ce cas, le result_ok  est en fait celui de la commande de déplacement a la position fallback, donc indiquer qu'il y a eu un problème pour atteindre la position et de reessayer plus tard
					debug_printf("Fallback position Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id].lastResult = ACT_FUNCTION_RetryLater;
					QUEUE_set_error(act_id, TRUE);
					QUEUE_next(act_id);
				} else {	//Sinon c'est bien la commande voulue qui s'est bien terminée
					debug_printf("Operation Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id].lastResult = ACT_FUNCTION_Done;
					QUEUE_next(act_id);
				}
				break;

			case ACT_RESULT_Working:	//Pas encore fini l'opération, on a pas reçu de message de la carte actionneur
				break;

			case ACT_RESULT_Idle:		//Ne devrait jamais arriver, sinon erreur dans le code
			default:
				error_printf("Warning: act should be in working/finished mode but wasn't, mode: %d, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_states[act_id].operationResult, act_id, argument->msg.sid, argument->msg.data[0]);
				QUEUE_set_error(act_id, TRUE);
				QUEUE_next(act_id);
				break;
		}
	}
#endif /* not def ACT_NO_ERROR_HANDLING */
}

// Gere les messages CAN ACT_RESULT contenant des infos sur le déroulement d'une action
void ACT_process_result(const CAN_msg_t* msg) {
	queue_id_e act_id = NB_QUEUE;

	assert(msg->sid == ACT_RESULT);

	debug_printf("Received result: act: %u, cmd: 0x%x, result: %u, reason: %u\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3]);

#ifdef ACT_NO_ERROR_HANDLING
	act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
	act_states[act_id].operationResult = ACT_RESULT_Ok;
#else

	switch(msg->data[0]) {
		case ACT_FILET & 0xFF:
			act_id = ACT_QUEUE_Filet;
			break;

		//Pierre
		case ACT_FRUIT_MOUTH & 0xFF:
			act_id = ACT_QUEUE_Fruit;
			break;

		//Krusty
		case ACT_BALLLAUNCHER & 0xFF:
			act_id = ACT_QUEUE_BallLauncher;
			break;

		case ACT_BALLSORTER & 0xFF:
			act_id = ACT_QUEUE_BallSorter;
			break;

		case ACT_PLATE & 0xFF:
			act_id = ACT_QUEUE_Plate;
			break;

		case ACT_LIFT_LEFT & 0xFF:
			act_id = ACT_QUEUE_LiftLeft;
			break;

		case ACT_LIFT_RIGHT & 0xFF:
			act_id = ACT_QUEUE_LiftRight;
			break;

		//Tiny
		case ACT_HAMMER & 0xFF:
			act_id = ACT_QUEUE_Hammer;
			break;

		case ACT_BALLINFLATER & 0xFF:
			act_id = ACT_QUEUE_BallInflater;
			break;

		case ACT_CANDLECOLOR & 0xFF:
			act_id = ACT_QUEUE_CandleColor;
			break;

		case ACT_PLIER & 0xFF:
			act_id = ACT_QUEUE_Plier;
			break;
	}

	if(act_id >= NB_QUEUE) {
		warn_printf("Unknown act result, can_act_id: 0x%x, can_result: %u\n", msg->data[0], msg->data[2]);
		return;
	}

	//Erreur de codage, ça ne devrait jamais arriver
	if(act_states[act_id].operationResult != ACT_RESULT_Working) {
		error_printf("act is not in working mode but received result, act: 0x%x, cmd: 0x%x, result: %u, reason: %u, mode: %d\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3], act_states[act_id].operationResult);
	}

	switch(msg->data[2]) {
		case ACT_RESULT_DONE:
			//On n'affecte pas act_states[act_id].recommendedBehavior pour garder une trace des erreurs précédentes (dans le cas ou on a renvoyé une commande par exemple, permet de savoir l'erreur d'origine)
			//act_states[act_id].recommendedBehavior est affecté à ACT_BEHAVIOR_Ok dans
			act_states[act_id].operationResult = ACT_RESULT_Ok;
			debug_printf("Reason Ok, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
			break;

		default:	//ACT_RESULT_NOT_HANDLED et ACT_RESULT_FAILED (et les autres si ajouté)
			act_states[act_id].operationResult = ACT_RESULT_Failed;
			switch(msg->data[3]) {
				case ACT_RESULT_ERROR_OK:
					error_printf("Reason Ok with result Failed, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
					break;

				case ACT_RESULT_ERROR_OTHER: //Keep last error, do nothing
					break;

				case ACT_RESULT_ERROR_TIMEOUT:
				case ACT_RESULT_ERROR_NOT_HERE:
					if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//On a déjà reçu cette erreur, aucun déplacement possible de l'actionneur -> désactivation
						act_states[act_id].disabled = TRUE;
						act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_GoalUnreachable;
					warn_printf("Goal unreachable ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);
					break;

				case ACT_RESULT_ERROR_NO_RESOURCES:
					if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_RetryLater) {	//Bug surement dans la carte actionneur, il n'y a pas assez de ressources pour gérer la commande
						act_states[act_id].disabled = TRUE;
						act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_RetryLater;
					warn_printf("NoResource error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Notifier le cas, il faudra par la suite augmenter les resources dispo ...
					break;

				case ACT_RESULT_ERROR_LOGIC:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					error_printf("Logic error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Ceci est un moment WTF. A résoudre le plus rapidement possible.
					break;

				default:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					error_printf("Unknown error ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);	//Ceci est un moment WTF. A résoudre le plus rapidement possible.
			}
			break;
	}
#endif /* not def ACT_NO_ERROR_HANDLING */
}

