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
#include "output_log.h"
#include "queue.h"
#include "Stacks.h"

#define LOG_PREFIX "act_f: "

/* Pile contenant les arguments d'une demande d'op�ration
 * Contient les messages CAN � envoyer � la carte actionneur pour ex�cuter l'action.
 * fallbackMsg contient le message CAN lorsque l'op�ration demand� par le message CAN msg ne peut pas �tre compl�t� (bras bloqu�, robot adverse qui bloque l'actionneur par exemple)
 * On utilise une structure diff�rente de CAN_msg_t pour �conomiser la RAM (voir matrice act_args)
 *
 * Pour chaque fonction appel�e par le code strat�gie pour empiler une action, on remplie les messages CAN � enovoyer et on empile l'action.
 * Un timeout est disponible si jamais la carte actionneur ne renvoie pas de message CAN ACT_RESULT (elle le devrait, mais on sait jamais)
 *
 * Le code de ACT_check_result() g�re le renvoi de message lorsque la carte actionneur indique qu'il n'y avait pas assez de ressource ou l'envoi du message fallback si la position demand� par le message CAN msg n'est pas atteignable.
 * Ce le message fallback se solve par un echec aussi, on indique le code de strat�gie que cet actionneur n'est pas dispo maintenant et de r�essayer plus tard (il faut faire une autre strat pendant ce temps, si c'est un robot qui bloque le bras, il faut que l'environnement du jeu bouge)
 * Si le renvoi du message � cause du manque de ressource cause la m�me erreur, on marque l'actionneur comme inutilisable (ce cas est grave, il y a probablement un probl�me dans le code actionneur ou un flood de demande d'op�ration de la carte strat�gie)
 */


//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur g�n�re des resultats et d�tail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) converti et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.

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
	ACT_BEHAVIOR_GoalUnreachable,          //ACT_RESULT_ERROR_TIMEOUT la premi�re fois. Tenter de retourner � la position en mode non-d�ploy�. Si une autre erreur survient,passer � ACT_ERROR_DisableAct Sinon essayer la strat qui l'utilise plus tard, et si l'erreur revient, refaire la proc�dure mais au lieu de ressayer la strat plus tard, desactiver l'actionneur.
										   //Si ACT_RESULT_ERROR_NOT_HERE, envoyer une commande de retour en mode d�ploy� et passer directement � ACT_ERROR_DisableAct. Esperons que l'actionneur re�evra la commande et restera pas d�ploy�.
} act_error_recommended_behavior_e;

typedef struct {
	bool_e disabled;	//TRUE si l'actionneur est d�sactiv� car inutilisable
	act_result_e operationResult;	//Resultat de la derrni�re op�ration faite
	act_error_recommended_behavior_e recommendedBehavior;		//Quoi faire suite � l'op�ration faite
	ACT_function_result_e lastResult;
} act_state_info_t;

#define ACT_STACK_TIMEOUT_MS 3000

#ifdef ACT_NO_ERROR_HANDLING
	#warning "La gestion d'erreur des actionneurs est d�sactiv�e ! (voir act_function.c, constante: ACT_NO_ERROR_HANDLING)"
#endif


static act_state_info_t act_states[NB_QUEUE];  //Info li� a chaque actionneur

// Verifie s'il y a eu un timeout et met � jour l'�tat de la pile en cons�quence.
// Si l'�tat de l'actionneur n'est pas normal ou qu'il y a eu une erreur, cette fonction affiche un message d'erreur
static void ACT_check_result(queue_id_e act_id);

static bool_e ACT_push_operation(queue_id_e act_id, QUEUE_arg_t* arg, bool_e run_now);
static void ACT_run_operation(queue_id_e act_id, bool_e init);

ACT_function_result_e ACT_get_last_action_result2(queue_id_e act_id) {
	assert(act_id < NB_QUEUE);
	return act_states[act_id].lastResult;
}

ACT_function_result_e ACT_get_last_action_result(stack_id_e act_id) {
	return ACT_get_last_action_result2(act_id - 1); //FIXME: GROSSE BIDOUILLE ICI !!!
}


//FONCTIONS D'ACTIONNEURS PUBLIQUES
// <editor-fold desc="Krusty">

bool_e ACT_push_ball_launcher_run(Uint16 speed, bool_e run) {
	QUEUE_arg_t args;

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
	return ACT_push_operation(ACT_QUEUE_BallLauncher, &args, run);
}

bool_e ACT_push_ball_launcher_stop(bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_BALLLAUNCHER;
	args.msg.data[0] = ACT_BALLLAUNCHER_STOP;
	args.msg.size = 1;

	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing BallLauncher Stop cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallLauncher, &args, run);
}


bool_e ACT_push_plate_rotate_horizontally(bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_PLATE;
	args.msg.data[0] = ACT_PLATE_ROTATE_HORIZONTALLY;
	args.msg.size = 1;

	//Si on ne peut pas aller en position horizontale, revenir en vertical et la strat pourra passer � autre chose (au lieu de d�foncer le d�cor)
	args.fallbackMsg.sid = ACT_PLATE;
	args.fallbackMsg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
	args.fallbackMsg.size = 1;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Plate rotate horizontally cmd\n");
	return ACT_push_operation(ACT_QUEUE_Plate, &args, run);
}

bool_e ACT_push_plate_rotate_prepare(bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_PLATE;
	args.msg.data[0] = ACT_PLATE_ROTATE_PREPARE;
	args.msg.size = 1;

	//Si on ne peut pas aller en position intermediaire, revenir en vertical et la strat pourra passer � autre chose (au lieu de d�foncer le d�cor)
	args.fallbackMsg.sid = ACT_PLATE;
	args.fallbackMsg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
	args.fallbackMsg.size = 1;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Plate rotate prepare cmd\n");
	return ACT_push_operation(ACT_QUEUE_Plate, &args, run);
}

bool_e ACT_push_plate_rotate_vertically(bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_PLATE;
	args.msg.data[0] = ACT_PLATE_ROTATE_VERTICALLY;
	args.msg.size = 1;

	//Que faire si on ne peut pas se replier ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Plate rotate vertically cmd\n");
	return ACT_push_operation(ACT_QUEUE_Plate, &args, run);
}

// </editor-fold>

// <editor-fold desc="Tiny">
bool_e ACT_push_hammer_goto(Uint16 position, bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_HAMMER;
	args.msg.data[0] = ACT_HAMMER_MOVE_TO;
	args.msg.data[1] = LOWINT(position);
	args.msg.data[2] = HIGHINT(position);
	args.msg.size = 3;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer moveto cmd\n");
	return ACT_push_operation(ACT_QUEUE_Hammer, &args, run);
}

bool_e ACT_push_hammer_stop(bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_HAMMER;
	args.msg.data[0] = ACT_HAMMER_STOP;
	args.msg.size = 1;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer stop asser cmd\n");
	return ACT_push_operation(ACT_QUEUE_Hammer, &args, run);
}

bool_e ACT_push_ball_inflater_inflate(Uint8 duration_sec, bool_e run) {
	QUEUE_arg_t args;

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
	return ACT_push_operation(ACT_QUEUE_BallInflater, &args, run);
}

bool_e ACT_push_ball_inflater_stop(bool_e run) {
	QUEUE_arg_t args;

	args.timeout = ACT_ARG_USE_DEFAULT;

	args.msg.sid = ACT_BALLINFLATER;
	args.msg.data[0] = ACT_BALLINFLATER_STOP;
	args.msg.size = 1;

	//Que faire si on ne peut pas bouger le bras ... (rien ici)
	args.fallbackMsg.sid = ACT_ARG_NOFALLBACK_SID;

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Pushing Hammer stop asser cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallInflater, &args, run);
}

// </editor-fold>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gestion des actions et de leur r�sultat (priv�)
// <editor-fold defaultstate="collapsed">

//Pr�pare une action correctement et l'envoie sur la pile
static bool_e ACT_push_operation(queue_id_e act_id, QUEUE_arg_t* args, bool_e run_now) {
#ifndef ACT_NEVER_DISABLE
	if(act_states[act_id].disabled == TRUE) {
		OUTPUTLOG_printf(LOG_LEVEL_Info, LOG_PREFIX"Ignoring push_operation, act %d is disabled\n", act_id);
		return FALSE;
	}
#endif

	if(args->timeout == ACT_ARG_USE_DEFAULT)
		args->timeout = ACT_STACK_TIMEOUT_MS;

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

		OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Sending operation, act_id: %d, sid: 0x%x, size: %d, data[0]: 0x%x, data[1]: 0x%x, data[2]: 0x%x\n", act_id, command->sid, command->size, command->data[0], command->data[1], command->data[2]);

		//Copie des �lements de notre structure � celle accept� par CAN_send, la notre est plus petite pour �conomiser la RAM
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

//G�re l'�tat d'une op�ration.
//Pour les renvois de messages on ne reinitialise pas le timeout histoire d'eviter les boucles si il y en a
static void ACT_check_result(queue_id_e act_id) {
	QUEUE_arg_t* argument = QUEUE_get_arg(act_id);

	//L'�tat � d�ja �t� g�r�, mais l'erreur n'a pas encore �t� g�r� par le reste du code strat�gie
	//Si c'est ACT_FUNCTION_Done on ne passe pas ici (a moins d'un gros probl�me de conception / amn�sie
	if(act_states[act_id].lastResult != ACT_FUNCTION_InProgress) {
		if(act_states[act_id].lastResult == ACT_FUNCTION_Done)
			OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Begin check but already in Done state, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
		return;
	}

	if(global.env.match_time >= argument->timeout + QUEUE_get_initial_time(act_id)) {
		OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation timeout (by strat) act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
		act_states[act_id].disabled = TRUE;
		act_states[act_id].lastResult = ACT_FUNCTION_ActDisabled;
		QUEUE_set_error(act_id, TRUE);
		QUEUE_next(act_id);
	} else {
#ifdef ACT_NO_ERROR_HANDLING
		act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
		act_states[act_id].lastResult = ACT_FUNCTION_Done;
		QUEUE_next(act_id);
#else
		switch(act_states[act_id].operationResult) {
			case ACT_RESULT_Failed:
				switch(act_states[act_id].recommendedBehavior) {

					case ACT_BEHAVIOR_GoalUnreachable:	//Envoyer le message fallback, s'il redonne une erreur, ACT_process_result s'occupera de d�sactiver l'actionneur
						if(argument->fallbackMsg.sid != ACT_ARG_NOFALLBACK_SID) {
							CAN_msg_t msg;

							act_states[act_id].operationResult = ACT_RESULT_Working;
							OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"GoalUnreachable, sending fallback message, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);

							msg.sid     = argument->fallbackMsg.sid;
							msg.data[0] = argument->fallbackMsg.data[0];
							msg.data[1] = argument->fallbackMsg.data[1];
							msg.data[2] = argument->fallbackMsg.data[2];
							msg.size    = argument->fallbackMsg.size;
							CAN_send(&msg);
							//On ne change pas act_states[act_id] car on n'a pas encore termin� avec l'op�ration
						} else {	//Si on n'a pas de message fallback, (car par exemple, �a aurait �t� le m�me message) on d�clare une erreur indiquant d'essayer plus tard
							OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"GoalUnreachable, no fallback message to send, disabling act, act id: %u\n", act_id);
							//act_states[act_id].disabled = TRUE;
							act_states[act_id].lastResult = ACT_FUNCTION_RetryLater;
							QUEUE_set_error(act_id, TRUE);
							QUEUE_next(act_id);
						}
						break;

					case ACT_BEHAVIOR_RetryLater: {
							static time32_t waitMsCount = 0;
							if(waitMsCount == 0) {
								OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"RetryLater, will retry 10ms later, act id: %u\n", act_id);
								waitMsCount = global.env.match_time + 10;	//Attendre 3ms avant de ressayer
							}else if(global.env.match_time >= waitMsCount) {  //Apr�s un certain temps d�fini juste avant, renvoyer la commande en esp�rant que �a passe cette fois
								CAN_msg_t msg;

								waitMsCount = 0;
								act_states[act_id].operationResult = ACT_RESULT_Working;
								OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"RetryLater, time to resend message, act id: %u\n", act_id);

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
						OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Operation failed but behavior is Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
						//pas de break ici, si la raison n'est pas connue de ce code, considerer l'erreur comme grave et desactiver l'actionneur
					case ACT_BEHAVIOR_DisableAct:
						OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Bad act behavior, act disabled, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
						act_states[act_id].disabled = TRUE;
						act_states[act_id].lastResult = ACT_FUNCTION_ActDisabled;
						QUEUE_set_error(act_id, TRUE);
						QUEUE_next(act_id);
						break;
				}
				break;

			case ACT_RESULT_Ok:
				if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//Dans ce cas, le result_ok  est en fait celui de la commande de d�placement a la position fallback, donc indiquer qu'il y a eu un probl�me pour atteindre la position et de reessayer plus tard
					OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Fallback position Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->fallbackMsg.sid, argument->fallbackMsg.data[0]);
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id].lastResult = ACT_FUNCTION_RetryLater;
					QUEUE_set_error(act_id, TRUE);
					QUEUE_next(act_id);
				} else {	//Sinon c'est bien la commande voulue qui s'est bien termin�e
					OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Operation Ok, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_id, argument->msg.sid, argument->msg.data[0]);
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
					act_states[act_id].lastResult = ACT_FUNCTION_Done;
					QUEUE_next(act_id);
				}
				break;

			case ACT_RESULT_Working:	//Pas encore fini l'op�ration, on a pas re�u de message de la carte actionneur
				break;

			case ACT_RESULT_Idle:		//Ne devrait jamais arriver, sinon erreur dans le code
			default:
				OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Warning: act should be in working/finished mode but wasn't, mode: %d, act id: %u, sid: 0x%x, cmd: 0x%x\n", act_states[act_id].operationResult, act_id, argument->msg.sid, argument->msg.data[0]);
				QUEUE_set_error(act_id, TRUE);
				QUEUE_next(act_id);
				break;
		}
#endif /* not def ACT_NO_ERROR_HANDLING */
	}
}

// Gere les messages CAN ACT_RESULT contenant des infos sur le d�roulement d'une action
void ACT_process_result(const CAN_msg_t* msg) {
	queue_id_e act_id = NB_QUEUE;

	assert(msg->sid == ACT_RESULT);

	OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Received result: act: %u, cmd: 0x%x, result: %u, reason: %u\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3]);

	switch(msg->data[0]) {
		//Krusty
		case ACT_BALLLAUNCHER & 0xFF:
			act_id = ACT_QUEUE_BallLauncher;
			break;

		case ACT_PLATE & 0xFF:
			act_id = ACT_QUEUE_Plate;
			break;

		//Tiny
		case ACT_HAMMER & 0xFF:
			act_id = ACT_QUEUE_Hammer;
			break;

		case ACT_BALLINFLATER & 0xFF:
			act_id = ACT_QUEUE_BallInflater;
			break;
	}

	if(act_id >= NB_QUEUE) {
		OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Unknown act result, can_act_id: 0x%x, can_result: %u\n", msg->data[0], msg->data[2]);
		return;
	}

	//Erreur de codage, �a ne devrait jamais arriver
	if(act_states[act_id].operationResult != ACT_RESULT_Working) {
		OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"act is not in working mode but received result, act: 0x%x, cmd: 0x%x, result: %u, reason: %u, mode: %d\n", msg->data[0], msg->data[1], msg->data[2], msg->data[3], act_states[act_id].operationResult);
	}

#ifdef ACT_NO_ERROR_HANDLING
	act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_Ok;
	act_states[act_id].operationResult = ACT_RESULT_Ok;
#else
	switch(msg->data[2]) {
		case ACT_RESULT_DONE:
			//On n'affecte pas act_states[act_id].recommendedBehavior pour garder une trace des erreurs pr�c�dentes (dans le cas ou on a renvoy� une commande par exemple, permet de savoir l'erreur d'origine)
			//act_states[act_id].recommendedBehavior est affect� � ACT_BEHAVIOR_Ok dans
			act_states[act_id].operationResult = ACT_RESULT_Ok;
			OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"Reason Ok, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
			break;

		default:	//ACT_RESULT_NOT_HANDLED et ACT_RESULT_FAILED (et les autres si ajout�)
			act_states[act_id].operationResult = ACT_RESULT_Failed;
			switch(msg->data[3]) {
				case ACT_RESULT_ERROR_OK:
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Reason Ok with result Failed, act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);
					break;

				case ACT_RESULT_ERROR_OTHER: //Keep last error, do nothing
					break;

				case ACT_RESULT_ERROR_TIMEOUT:
				case ACT_RESULT_ERROR_NOT_HERE:
					if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_GoalUnreachable) {	//On a d�j� re�u cette erreur, aucun d�placement possible de l'actionneur -> d�sactivation
						act_states[act_id].disabled = TRUE;
						act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_GoalUnreachable;
					OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"Goal unreachable ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);
					break;

				case ACT_RESULT_ERROR_NO_RESOURCES:
					if(act_states[act_id].recommendedBehavior == ACT_BEHAVIOR_RetryLater) {	//Bug surement dans la carte actionneur, il n'y a pas assez de ressources pour g�rer la commande
						act_states[act_id].disabled = TRUE;
						act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					} else act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_RetryLater;
					OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"NoResource error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Notifier le cas, il faudra par la suite augmenter les resources dispo ...
					break;

				case ACT_RESULT_ERROR_LOGIC:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Logic error ! act_id: 0x%x, cmd: 0x%x\n", msg->data[0], msg->data[1]);	//Ceci est un moment WTF. A r�soudre le plus rapidement possible.
					break;

				default:
					act_states[act_id].disabled = TRUE;
					act_states[act_id].recommendedBehavior = ACT_BEHAVIOR_DisableAct;
					OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"Unknown error ! act_id: 0x%x, cmd: 0x%x, reason: %u\n", msg->data[0], msg->data[1], msg->data[3]);	//Ceci est un moment WTF. A r�soudre le plus rapidement possible.
			}
			break;
	}
#endif /* not def ACT_NO_ERROR_HANDLING */
}

// </editor-fold>
