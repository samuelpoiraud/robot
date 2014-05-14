/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Selftest
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#define SELFTEST_C
#include "selftest.h"
#include "QS/QS_all.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"
#include "clock.h"
#include "act_queue_utils.h"

#include "config_debug.h"
#define LOG_PREFIX "Selftest: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SELFTEST
#include "QS/QS_outputlog.h"

#define MAX_NB_ACT 8
#define SELFTEST_TIMEOUT 150 // unité: 0.1s, 150 = 15s, doit être inférieur au timeout en strat

static SELFTEST_error_code_e failed_act_tests[MAX_NB_ACT];
static Uint8 expected_act_num = 0;
static Uint8 act_test_done_num = 0;
static bool_e test_finished = TRUE;

static void SELFTEST_done_test(Uint11 act_sid, bool_e test_ok);
static bool_e SELFTEST_check_end(queue_id_t queueId);
static void SELFTEST_run(queue_id_t queueId, bool_e init);

bool_e SELFTEST_new_selftest(Uint8 nb_actionneurs) {
	Uint8 i;

	if(test_finished != TRUE) {
		error_printf("Selftest déjà en cours alors qu\'un nouveau test avec %d actionneurs a été demandé\n", nb_actionneurs);
		return FALSE;
	}

	for(i = 0; i < MAX_NB_ACT; i++) {
		failed_act_tests[i] = SELFTEST_NOT_DONE;
	}

	act_test_done_num = 0;
	expected_act_num = nb_actionneurs;
	test_finished = FALSE;

	debug_printf("Nouveau selftest, test de %d actionneurs\n", nb_actionneurs);

	queue_id_t queueId = QUEUE_create();
	QUEUE_add(queueId, &SELFTEST_run, (QUEUE_arg_t){0, 0}, 0);

	return TRUE;
}

void SELFTEST_set_actions(action_t action, Uint8 action_num, const SELFTEST_action_t actions[]) {
	Uint8 i, j;
	queue_id_t queueId = QUEUE_create();

	//contient la liste des actionneurs dont on a déjà fait QUEUE_take_sem
	Uint8 act_locked_count = 0;
	QUEUE_act_e act_locked[action_num];

	if(action_num == 0)
		return;

	if(test_finished) {
		error_printf("SELFTEST_set_actions appelé sans selftest en cours !, action function = 0x%x with %d test actions\n", (int)action, action_num);
		return;
	}

	if(queueId != QUEUE_CREATE_FAILED) {
		//On cherche les actionneurs à lock, on doit lock un actionneur qu'une seule fois
		for(i = 0; i < action_num; i++) {
			bool_e already_locked = FALSE;

			//On verifie s'il n'est pas déjà locké
			for(j = 0; j < act_locked_count; j++) {
				if(act_locked[j] == actions[i].optionnal_act) {
					already_locked = TRUE;
					break;
				}
			}

			//On l'ajoute à la liste et le lock s'il n'était pas déjà locké
			if(already_locked == FALSE) {
				act_locked[act_locked_count] = actions[i].optionnal_act;
				act_locked_count++;
				QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, actions[i].optionnal_act);
			}
		}

		//On ajoute la liste d'action à faire
		for(i = 0; i < action_num; i++) {
			QUEUE_add(queueId, action, (QUEUE_arg_t){actions[i].canCommand,  actions[i].param, &SELFTEST_finish}, actions[i].optionnal_act);
		}

		//Et on libère les actionneurs (dans le sens inverse)
		for(j = 0; j < act_locked_count; j++) {
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, act_locked[act_locked_count - j - 1]);
		}
	} else {
		QUEUE_flush(queueId);
		ACTQ_printResult(ACT_DO_SELFTEST, 0, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES, 0);
	}
}

bool_e SELFTEST_finish(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	ACTQ_printResult(act_sid, QUEUE_get_arg(queue_id)->canCommand, result, error_code, param);

	if(result == ACT_RESULT_DONE) {
		// Si reste que cette action (qui vient de finir) dans la file, les tests pour cet actionneur sont terminés.
		// Si result n'est pas ACT_RESULT_DONE, alors on termine tout de suite le test et on enregistre l'erreur
		// 2 car il y a nous et le give_sem
		action_t next_action;
		QUEUE_next_action_info(queue_id, &next_action, NULL, NULL);
		if(next_action == &QUEUE_give_sem) {
			SELFTEST_done_test(act_sid, TRUE);
		}

		return TRUE;
	} else {
		SELFTEST_done_test(act_sid, FALSE);
	}

	return FALSE;
}

static void SELFTEST_done_test(Uint11 act_sid, bool_e test_ok) {
	Uint8 i;

	if(test_finished) {
		error_printf("SELFTEST_done_test appelé sans selftest en cours !, sid = 0x%x, test state = %d\n", act_sid, test_ok);
		return;
	}

	info_printf("Test de l\'actionneur sid 0x%x terminé avec l\'état %s\n", act_sid, test_ok ? "SELFTEST_STATE_OK" : "SELFTEST_STATE_ERROR");

	if(test_ok == FALSE) {
		for(i = 0; i < MAX_NB_ACT; i++) {
			if(failed_act_tests[i] == SELFTEST_NOT_DONE) {
				switch(act_sid) {
					case ACT_FRUIT_MOUTH:
						failed_act_tests[i] = SELFTEST_ACT_FRUIT_MOUTH;
						break;

					case ACT_LANCELAUNCHER:
						failed_act_tests[i] = SELFTEST_ACT_LANCELAUNCHER;
						break;

					case ACT_ARM:
						failed_act_tests[i] = SELFTEST_ACT_ARM;
						break;

					case ACT_SMALL_ARM:
						failed_act_tests[i] = SELFTEST_ACT_SMALL_ARM;
						break;

					case ACT_FILET:
						failed_act_tests[i] = SELFTEST_ACT_FILET;
						break;

					case ACT_GACHE:
						failed_act_tests[i] = SELFTEST_ACT_GACHE;
						break;

					case ACT_POMPE:
						failed_act_tests[i] = SELFTEST_POMPE;
						break;

					case ACT_TORCH_LOCKER:
						failed_act_tests[i] = SELFTEST_ACT_TORCH_LOCKER;
						break;

					default:
						warn_printf("L\'actionneur sid 0x%x n\'a pas d\'enum SELFTEST_ACT_xxx associé, utilisation de SELFTEST_ACT_UNKNOWN_ACT\n", act_sid);
						failed_act_tests[i] = SELFTEST_ACT_UNKNOWN_ACT;
						break;
				}
				break;
			}
		}
	}
	act_test_done_num++;
}

static void SELFTEST_run(queue_id_t queueId, bool_e init) {
	if(!init) {
		if(SELFTEST_check_end(queueId))
			QUEUE_behead(queueId);
	}
}

//Verifie si tous les actionneurs on répondu
static bool_e SELFTEST_check_end(queue_id_t queueId) {
	Uint8 i;

	if(act_test_done_num == expected_act_num || CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + SELFTEST_TIMEOUT) {
		if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + SELFTEST_TIMEOUT) {
			warn_printf("Timeout du selftest, reçu %d resultat vs %d attendus\n", act_test_done_num, expected_act_num);
		}

		test_finished = TRUE;
		CAN_msg_t msg;
		msg.sid = STRAT_ACT_SELFTEST_DONE;
		msg.size = 0;

		if(act_test_done_num != expected_act_num) {
			msg.data[msg.size] = SELFTEST_ACT_MISSING_TEST;
			OUTPUTLOG_printf(LOG_LEVEL_Debug, "%3d ", msg.data[msg.size]);
			msg.size++;
		}

		for(i = 0; i < MAX_NB_ACT && msg.size < 8; i++) {
			if(failed_act_tests[i] != SELFTEST_NOT_DONE) {
				msg.data[msg.size] = failed_act_tests[i];
				msg.size++;
			}
		}

		debug_printf("Resultat: %d/%d réponses, erreurs: ", act_test_done_num, expected_act_num);
		for(i = 0; i < msg.size; i++)
			OUTPUTLOG_printf(LOG_LEVEL_Debug, "%3d ", msg.data[i]);
		OUTPUTLOG_printf(LOG_LEVEL_Debug, "\n");
#ifdef USE_CAN
		CAN_send(&msg);
#endif
		return TRUE;
	} else if(act_test_done_num > expected_act_num) {
		error_printf("Recu plus de réponse que prévu, nb actioneurs = %d, reçu %d réponses\n", expected_act_num, act_test_done_num);
		return TRUE;
	}

	return FALSE;
}
