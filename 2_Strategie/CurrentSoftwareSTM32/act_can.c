#include "act_can.h"
#include "QS/QS_CANmsgList.h"

#define LOG_PREFIX "act_c: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"

//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur g�n�re des resultats et d�tail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) converti et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.

//Temps de timeout d'une action par d�faut
#define ACT_ARG_DEFAULT_TIMEOUT_MS 3000
#define ACT_ARG_NOFALLBACK_SID 0xFFF

#ifdef ACT_NO_ERROR_HANDLING
	#warning "La gestion d'erreur des actionneurs est d�sactiv�e ! (voir act_function.c/h, constante: ACT_NO_ERROR_HANDLING)"
#endif

typedef enum {
	ACT_RESULT_Idle,	//Etat au d�marage, par la suite ce sera le resultat de la derni�re op�ration effectu�e
	ACT_RESULT_Working,	//L'op�ration n'est pas termin�e
	ACT_RESULT_Ok,		//L'op�ration s'est termin�e correctement
	ACT_RESULT_Failed	//Une erreur est survenue, voir details dans act_error_recommended_behavior_e
} act_result_e;

typedef enum {
	ACT_BEHAVIOR_Ok,                       //Pas d'erreur (ACT_RESULT_ERROR_OK)
	ACT_BEHAVIOR_DisableAct,               //L'actionneur est inutilisable
	ACT_BEHAVIOR_RetryLater,               //Ressayer plus tard les commandes, si arrive plusieurs fois de suite, passer � ACT_ERROR_DisableAct (ACT_RESULT_ERROR_NO_RESOURCES)
	ACT_BEHAVIOR_GoalUnreachable           //ACT_RESULT_ERROR_TIMEOUT la premi�re fois. Tenter de retourner � la position en mode non-d�ploy�. Si une autre erreur survient,passer � ACT_ERROR_DisableAct Sinon essayer la strat qui l'utilise plus tard, et si l'erreur revient, refaire la proc�dure mais au lieu de ressayer la strat plus tard, desactiver l'actionneur.
										   //Si ACT_RESULT_ERROR_NOT_HERE, envoyer une commande de retour en mode d�ploy� et passer directement � ACT_ERROR_DisableAct. Esperons que l'actionneur re�evra la commande et restera pas d�ploy�.
} act_error_recommended_behavior_e;

typedef struct {
	bool_e disabled;	//TRUE si l'actionneur est d�sactiv� car inutilisable
	act_result_e operationResult;	//Resultat de la derrni�re op�ration faite
	Uint8 paramResult[4];
	act_error_recommended_behavior_e recommendedBehavior;		//Quoi faire suite � l'op�ration faite
	ACT_function_result_e lastResult;
	Uint8 lastParamResult[4];
} act_state_info_t;

static act_state_info_t act_states[NB_QUEUE];  //Info li� a chaque actionneur

// Verifie s'il y a eu un timeout et met � jour l'�tat de la pile en cons�quence.
// Si l'�tat de l'actionneur n'est pas normal ou qu'il y a eu une erreur, cette fonction affiche un message d'erreur
static void ACT_check_result(queue_id_e act_id);

static void ACT_run_operation(queue_id_e act_id, bool_e init);


ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id) {
	assert(act_id < NB_QUEUE);
#ifdef ROBOT_VIRTUEL_PARFAIT
	return ACT_FUNCTION_Done;	//En mode simulation, l'action demand�e se termine imm�diatement.
#endif
	return act_states[act_id].lastResult;
}

Uint8 ACT_get_result_param(queue_id_e act_id, Uint8 i_param){
	return act_states[act_id].lastParamResult[i_param];
}

void ACT_arg_init(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd) {
	arg->msg.sid = sid;
	arg->msg.data[0] = cmd;
	arg->msg.size = 1;
	arg->fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;
	arg->timeout = ACT_ARG_DEFAULT_TIMEOUT_MS;
}

void ACT_arg_init_with_param(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param)  {
	arg->msg.sid = sid;
	arg->msg.data[0] = cmd;
	arg->msg.data[1] = LOWINT(param);
	arg->msg.data[2] = HIGHINT(param);
	arg->msg.size = 3;
	arg->fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;
	arg->timeout = ACT_ARG_DEFAULT_TIMEOUT_MS;
}

void ACT_arg_init_with_msg(QUEUE_arg_t* arg, ACT_can_msg_t msg){
	arg->msg = msg;
	arg->fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;
	arg->timeout = ACT_ARG_DEFAULT_TIMEOUT_MS;
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


//Pr�pare une action correctement et l'envoie sur la pile
bool_e ACT_push_operation(queue_id_e act_id, QUEUE_arg_t* args) {
#ifndef ACT_NEVER_DISABLE
	if(act_states[act_id].disabled == TRUE) {
		COMPONENT_log(LOG_LEVEL_Info, "Ignoring push_operation, act %d is disabled\n", act_id);
		return FALSE;
	}
#endif

	QUEUE_add(act_id, &ACT_run_operation, *args);

	return TRUE;
}

//G�re le d�roulement d'une action cot� strat: initialisation = envoi du message CAN de l'action, suite = v�rification du r�sultat si re�u (par msg CAN) ou si timeout (g�r� ici en strat, si jamais quelque chose ne va pas en actionneur)
//Cette fonction est appel�e par le module QUEUE, QUEUE_run en particuli� quand init vaut FALSE
static void ACT_run_operation(queue_id_e act_id, bool_e init) {
	if(init) {
		ACT_can_msg_t* command = &(QUEUE_get_arg(act_id)->msg);
		CAN_msg_t msg;

		act_states[act_id].operationResult = ACT_RESULT_Working;
		act_states[act_id].lastResult = ACT_FUNCTION_InProgress;

		debug_printf("Sending operation, act_id: %d, sid: 0x%x, size: %d, data[0]: 0x%x, data[1]: 0x%x, data[2]: 0x%x\n", act_id, command->sid, command->size, command->data[0], command->data[1], command->data[2]);

		//Copie des �lements de notre structure � celle accept� par CAN_send, la notre est plus petite pour �conomiser la RAM
		msg.sid     = command->sid;
		msg.data[0] = command->data[0];
		msg.data[1] = command->data[1];
		msg.data[2] = command->data[2];
		msg.data[3] = command->data[3];
		msg.data[4] = command->data[4];
		msg.data[5] = command->data[5];
		msg.data[6] = command->data[6];
		msg.data[7] = command->data[7];
		msg.size    = command->size;
		CAN_send(&msg);

	} else {
		ACT_check_result(act_id);
	}
}

//G�re l'�tat d'une op�ration.
//Pour les renvois de messages on ne reinitialise pas le timeout histoire d'eviter les boucles si il y en a
static void ACT_check_result(queue_id_e act_id) {
	QUEUE_arg_t* argument = QUEUE_get_arg(act_id);

	//L'op�ration est termin�e mais on passe encore ici, ya t-il un bug ?
	//Si ce bug arrive, il est probablement li� � l'init de l'action (qui d�finie lastResult � ACT_FUNCTION_InProgress)
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

					case ACT_BEHAVIOR_GoalUnreachable:	//Envoyer le message fallback, s'il redonne une erreur, ACT_process_result s'occupera de d�sactiver l'actionneur
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
							//On ne change pas act_states[act_id] car on n'a pas encore termin� avec l'op�ration
						} else {	//Si on n'a pas de message fallback, (car par exemple, �a aurait �t� le m�me message) on d�clare une erreur indiquant d'essayer plus tard
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
							}else if(global.env.match_time >= waitMsCount) {  //Apr�s un certain temps d�fini juste avant, renvoyer la commande en esp�rant que �a passe cette fois
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
							}	//On ne change pas act_states[act_id] car on n'a pas encore termin� avec l'op�ration
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
				act_states[act_id].lastParamResult[0] = act_states[act_id].paramResult[0];
				act_states[act_id].lastParamResult[1] = act_states[act_id].paramResult[1];
				act_states[act_id].lastParamResult[2] = act_states[act_id].paramResult[2];
				act_states[act_id].lastParamResult[3] = act_states[act_id].paramResult[3];
				break;

			case ACT_RESULT_Ok:
				if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//Dans ce cas, le result_ok  est en fait celui de la commande de d�placement a la position fallback, donc indiquer qu'il y a eu un probl�me pour atteindre la position et de reessayer plus tard
					debug_printf("Fallback position Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id].lastResult = ACT_FUNCTION_RetryLater;
					QUEUE_set_error(act_id, TRUE);
					QUEUE_next(act_id);
				} else {	//Sinon c'est bien la commande voulue qui s'est bien termin�e
					debug_printf("Operation Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id].lastResult = ACT_FUNCTION_Done;
					QUEUE_next(act_id);
				}
				act_states[act_id].lastParamResult[0] = act_states[act_id].paramResult[0];
				act_states[act_id].lastParamResult[1] = act_states[act_id].paramResult[1];
				act_states[act_id].lastParamResult[2] = act_states[act_id].paramResult[2];
				act_states[act_id].lastParamResult[3] = act_states[act_id].paramResult[3];
				break;

			case ACT_RESULT_Working:	//Pas encore fini l'op�ration, on a pas re�u de message de la carte actionneur
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

// Gere les messages CAN ACT_RESULT contenant des infos sur le d�roulement d'une action
void ACT_process_result(const CAN_msg_t* msg) {
	queue_id_e act_id = NB_QUEUE;

	assert(msg->sid == ACT_RESULT);

	debug_printf("Received result: act: %u, cmd: 0x%x, result: %u, reason: %u\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3]);

#ifdef ACT_NO_ERROR_HANDLING
	act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
	act_states[act_id].operationResult = ACT_RESULT_Ok;
#else

	switch(msg->data[0]) {

		// Holly
		/*case ACT_TORCH_LOCKER & 0xFF:
			act_id = ACT_QUEUE_Torch_locker;
			break;*/
		case ACT_POP_COLLECT_LEFT & 0xFF:
					act_id = ACT_QUEUE_Pop_collect_left;
					break;
		case ACT_POP_COLLECT_RIGHT & 0xFF:
					act_id = ACT_QUEUE_Pop_collect_right;
					break;
		case ACT_POP_DROP_LEFT & 0xFF:
					act_id = ACT_QUEUE_Pop_drop_left;
					break;
		case ACT_POP_DROP_RIGHT & 0xFF:
					act_id = ACT_QUEUE_Pop_drop_right;
					break;

		// Wood
		case ACT_PINCE_GAUCHE & 0xFF:
					act_id = ACT_QUEUE_Pince_Gauche;
					break;
		case ACT_PINCE_DROITE & 0xFF:
					act_id = ACT_QUEUE_Pince_Droite;
					break;
		case ACT_CLAP & 0xFF:
					act_id = ACT_QUEUE_Clap;
					break;
		case ACT_POP_DROP_LEFT_WOOD & 0xFF:
					act_id = ACT_QUEUE_Pop_drop_left_Wood;
					break;
		case ACT_POP_DROP_RIGHT_WOOD & 0xFF:
					act_id = ACT_QUEUE_Pop_drop_right_Wood;
					break;
		// Common


	}

	if(act_id >= NB_QUEUE) {
		error_printf("Unknown act result, can_act_id: 0x%x, can_result: %u. Pour g�rer cet act, il faut ajouter un case dans le switch juste au dessus de ce printf\n", msg->data[0], msg->data[2]);
		return;
	}

	//Erreur de codage, �a ne devrait jamais arriver sauf si la commande en question a �t� lanc� par quelqu'un d'autre que la strategie (par exemple via un bouton pour debug)
	if(act_states[act_id].operationResult != ACT_RESULT_Working) {
		warn_printf("act is not in working mode but received result, act: 0x%x, cmd: 0x%x, result: %u, reason: %u, mode: %d\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3], act_states[act_id].operationResult);
		return;
	}

	switch(msg->data[2]) {
		case ACT_RESULT_DONE:
			//On n'affecte pas act_states[act_id].recommendedBehavior pour garder une trace des erreurs pr�c�dentes (dans le cas ou on a renvoy� une commande par exemple, permet de savoir l'erreur d'origine)
			//act_states[act_id].recommendedBehavior est affect� � ACT_BEHAVIOR_Ok dans
			act_states[act_id].operationResult = ACT_RESULT_Ok;
			debug_printf("Reason Ok, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
			break;

		default:	//ACT_RESULT_NOT_HANDLED et ACT_RESULT_FAILED (et les autres si ajout�)
			act_states[act_id].operationResult = ACT_RESULT_Failed;
			switch(msg->data[3]) {
				case ACT_RESULT_ERROR_OK:
					error_printf("Reason Ok with result Failed, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
					break;

				case ACT_RESULT_ERROR_OTHER: //Keep last error, do nothing
					break;

				case ACT_RESULT_ERROR_TIMEOUT:
				case ACT_RESULT_ERROR_NOT_HERE:
					if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//On a d�j� re�u cette erreur, aucun d�placement possible de l'actionneur -> d�sactivation
						act_states[act_id].disabled = TRUE;
						act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_GoalUnreachable;
					warn_printf("Goal unreachable ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);
					break;

				case ACT_RESULT_ERROR_NO_RESOURCES:
					if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_RetryLater) {	//Bug surement dans la carte actionneur, il n'y a pas assez de ressources pour g�rer la commande
						act_states[act_id].disabled = TRUE;
						act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_RetryLater;
					warn_printf("NoResource error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Notifier le cas, il faudra par la suite augmenter les resources dispo ...
					break;

				case ACT_RESULT_ERROR_CANCELED:
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_RetryLater;
					warn_printf("Operation canceled, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//L'op�ration � �t� volontairement annul�e
					break;

				case ACT_RESULT_ERROR_LOGIC:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					error_printf("Logic error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Ceci est un moment WTF. A r�soudre le plus rapidement possible.
					break;

				case ACT_RESULT_ERROR_INVALID_ARG:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					error_printf("Invalid argument ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Argument invalide utilis�, bug cot� strat
					break;

				case ACT_RESULT_ERROR_UNKNOWN:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					error_printf("Unknown error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);  //Erreur inconnu envoy� comme tel par la carte actionneur
					break;
					//Autres erreurs utilis�es mais non g�r�es dans ce switch, il faut ajouter un case ACT_RESULT_ERROR_????? pour la g�rer
				default:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					error_printf("Unknown error value %u for act_id: 0x%x, cmd: 0x%x, Pour g�rer l'erreur, il faut ajouter un case dans le switch qui contient ce printf\n", msg->data[3], msg->data[0], msg->data[1]);	//Ceci est un moment WTF. A r�soudre le plus rapidement possible.
			}
			break;
	}
	act_states[act_id].paramResult[0] = msg->data[4];
	act_states[act_id].paramResult[1] = msg->data[5];
	act_states[act_id].paramResult[2] = msg->data[6];
	act_states[act_id].paramResult[3] = msg->data[7];

#endif /* not def ACT_NO_ERROR_HANDLING */
}

