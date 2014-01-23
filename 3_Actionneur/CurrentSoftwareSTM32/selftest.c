#define SELFTEST_C
#include "selftest.h"
#include "QS/QS_all.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"
#include "clock.h"
#include "queue.h"

#include "config_debug.h"
#define LOG_PREFIX "Selftest: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SELFTEST
#include "QS/QS_outputlog.h"

#define MAX_NB_ACT 10
#define SELFTEST_TIMEOUT 100 // 10s

typedef struct {
	Uint11 sid;
	SELFTEST_state_e state;
} SELFTEST_data_t;

static SELFTEST_data_t act_test_states[MAX_NB_ACT];
static Uint8 expected_act_num = 0;
static bool_e test_finished = TRUE;

static bool_e SELFTEST_check_end(queue_id_t queueId);
static void SELFTEST_run(queue_id_t queueId, bool_e init);

void SELFTEST_new_selftest(Uint8 nb_actionneurs) {
	Uint8 i;

	if(test_finished != TRUE) {
		error_printf("Selftest déjà en cours alors qu\'un nouveau test avec %d actionneurs a été demandé\n", nb_actionneurs);
		return;
	}

	for(i = 0; i < MAX_NB_ACT; i++) {
		act_test_states[i].state = SELFTEST_UNUSED;
		act_test_states[i].sid = 0;
	}

	expected_act_num = nb_actionneurs;
	test_finished = FALSE;

	debug_printf("Nouveau selftest, test de %d actionneurs\n", nb_actionneurs);

	queue_id_t queueId = QUEUE_create();
	QUEUE_add(queueId, &SELFTEST_run, (QUEUE_arg_t){0, 0}, 0);
}

void SELFTEST_set_state(Uint11 act_sid, SELFTEST_state_e test_status) {
	Uint8 i;
	assert(test_ok == SELFTEST_UNUSED);

	if(test_finished) {
		error_printf("SELFTEST_set_state appelé sans selftest en cours !, sid = 0x%x, test state = %d\n", act_sid, test_status);
		return;
	}

	debug_printf("Test de l\'actionneur sid 0x%x terminé\n", act_sid);

	for(i = 0; i < MAX_NB_ACT; i++) {
		if(act_test_states[i].state == SELFTEST_UNUSED) {
			act_test_states[i].state = test_status;
			act_test_states[i].sid = act_sid;
		}
	}
}

static void SELFTEST_run(queue_id_t queueId, bool_e init) {
	if(!init) {
		if(SELFTEST_check_end(queueId))
			QUEUE_behead(queueId);
	}
}

//Verifie si tous les actionneurs on répondu
static bool_e SELFTEST_check_end(queue_id_t queueId) {
	Uint8 i, nb_act;

	for(nb_act = i = 0; i < MAX_NB_ACT; i++) {
		if(act_test_states[i].state == SELFTEST_OK || act_test_states[i].state == SELFTEST_ERROR)
			nb_act++;
	}

	if(nb_act == expected_act_num || CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + SELFTEST_TIMEOUT) {
		if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + SELFTEST_TIMEOUT) {
			warn_printf("Timeout du selftest\n");
		}

		test_finished = TRUE;
		CAN_msg_t msg;
		msg.sid = STRAT_ACT_SELFTEST_DONE;
		msg.size = 0;

		for(i = 0; i < MAX_NB_ACT; i++) {
			if(act_test_states[i].state == SELFTEST_ERROR || act_test_states[i].state == SELFTEST_IN_PROGRESS) {
				//si on trouve un actionneur qui n'a pas terminé son test alors qu'on a déjà assez d'actionneur qui ont terminé le test, alors le in_progress est en trop
				if(act_test_states[i].state == SELFTEST_IN_PROGRESS) {
					warn_printf("Test non prévu en in progress pour l\'actionneur de sid 0x%x\n", act_test_states[i].sid);
				}
				if(msg.size < 8) {
					switch(act_test_states[i].sid) <%
						case ACT_FRUIT_MOUTH:
							msg.data[msg.size] = SELFTEST_ACT_FRUIT_MOUTH;
							break;

						case ACT_LANCELAUNCHER:
							msg.data[msg.size] = SELFTEST_ACT_LANCELAUNCHER;
							break;

						case ACT_ARM:
							msg.data[msg.size] = SELFTEST_ACT_ARM;
							break;

						default:
							msg.data[msg.size] = SELFTEST_ACT_UNKNOWN;
							break;
					%>

					msg.size++;
				}
			}
		}
#ifdef USE_CAN
		CAN_send(&msg);
#endif
		return TRUE;
	} else if(nb_act > expected_act_num) {
		error_printf("Recu plus de réponse que prévu, nb actioneurs = %d, reçu %d réponses\n", expected_act_num, nb_act);
		return TRUE;
	}

	return FALSE;
}
