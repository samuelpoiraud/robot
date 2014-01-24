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
#define SELFTEST_TIMEOUT 100 // 10s

static SELFTEST_error_code_e failed_act_tests[MAX_NB_ACT];
static Uint8 expected_act_num = 0;
static Uint8 act_test_done_num = 0;
static bool_e test_finished = TRUE;

static void SELFTEST_done_test(Uint11 act_sid, bool_e test_ok);
static bool_e SELFTEST_check_end(queue_id_t queueId);
static void SELFTEST_run(queue_id_t queueId, bool_e init);

void SELFTEST_new_selftest(Uint8 nb_actionneurs) {
	Uint8 i;

	if(test_finished != TRUE) {
		error_printf("Selftest déjà en cours alors qu\'un nouveau test avec %d actionneurs a été demandé\n", nb_actionneurs);
		return;
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
}

bool_e SELFTEST_finish(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	if(result == ACT_RESULT_DONE) {
		SELFTEST_done_test(act_sid, TRUE);
	}

	SELFTEST_done_test(act_sid, FALSE);

	return ACTQ_finish_SendNothing(queue_id, act_sid, result, error_code, param);
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

					default:
						warn_printf("L\'actionneur sid 0x%x n\'a pas d\'enum SELFTEST_ACT_xxx associé, utilisation de SELFTEST_ACT_UNKNOWN_ACT\n", act_sid);
						failed_act_tests[i] = SELFTEST_ACT_UNKNOWN_ACT;
						break;
				}
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
			warn_printf("Timeout du selftest\n");
		}

		test_finished = TRUE;
		CAN_msg_t msg;
		msg.sid = STRAT_ACT_SELFTEST_DONE;
		msg.size = 0;

		if(act_test_done_num != expected_act_num) {
			msg.data[msg.size] = SELFTEST_ACT_MISSING_TEST;
			msg.size++;
		}

		for(i = 0; i < MAX_NB_ACT && i < msg.size; i++) {
			msg.data[msg.size] = failed_act_tests[i];
			msg.size++;
		}
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
