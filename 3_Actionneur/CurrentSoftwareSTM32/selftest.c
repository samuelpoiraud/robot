/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Selftest
 *  Auteur : Alexis
 *  Robot : Pierre & Guy, Holly & Wood, Black & Pearl
 */

#define SELFTEST_C
#include "selftest.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"
#include "clock.h"
#include "act_queue_utils.h"
#include "can_msg_processing.h"

#ifdef I_AM_ROBOT_BIG

#endif

#ifdef I_AM_ROBOT_SMALL

#endif

#define LOG_PREFIX "Selftest: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SELFTEST
#include "QS/QS_outputlog.h"

#define MAX_NB_ACT 64
#define SELFTEST_TIMEOUT 30000 // en ms (30sec)  //Duree du timeout du selftest complet
#define ACT_TIMEOUT  5000 //en ms (5sec)  //Duree du timeout par actionneur

typedef enum{
	DONE
}SELFTEST_state_machine_e;

static SELFTEST_error_code_e failed_act_tests[MAX_NB_ACT];
static SELFTEST_state_e state_act_tests[MAX_NB_ACT];
static 	Uint8 state = 0, last_state = 0; //état de la machine à états
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
		state_act_tests[i] = SELFTEST_STATE_IN_PROGRESS;
	}

	// TODO 2017 Mettre ici l'état initial du selftest pour chaque robot
	/*if(I_AM_BIG())
		//state = SELFTEST_FISHS;
	else
		//state = SELFTEST_LEFT_ARM;
	*/

	last_state = 255;
	act_test_done_num = 0;
	expected_act_num = nb_actionneurs - 1; //Spécificité 2016, il faut enlever ACT_MOSFET dont le selftest est fait depuis la strat
	test_finished = FALSE;

	debug_printf("Nouveau selftest, test de %d actionneurs\n", expected_act_num);

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

		/*if(delay){  //Possibilité d'ajouter un delay (delay est un multiple de 100ms)
			QUEUE_add(queueId, &QUEUE_wait_time, (QUEUE_arg_t){delay, 0, NULL}, 0);
		}*/

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

void SELFTEST_state_machine(void){
	//Attention à bien mettre : state_act_tests[ACT_EXEMPLE & 0xFF] != SELFTEST_IN_PROGRESS
	//Et pas : state_act_tests[QUEUE_ACT_EXEMPLE] != SELFTEST_IN_PROGRESS
	//Car les indices de la Queue et le l'enum ACT sont peut-être dans un ordre différent
	static time32_t time_for_timeout;
	UNUSED_VAR(time_for_timeout);
	bool_e entrance = (last_state == state)? FALSE:TRUE;
	last_state = state;
	if(entrance)
		time_for_timeout = global.absolute_time;

		switch(state){

// Harry
#ifdef I_AM_ROBOT_BIG
			/*case SELFTEST_FISHS:
				if(entrance && state_act_tests[ACT_FISH_MAGNETIC_ARM & 0xFF] == SELFTEST_STATE_IN_PROGRESS && state_act_tests[ACT_FISH_UNSTICK_ARM & 0xFF] == SELFTEST_STATE_IN_PROGRESS){
					SELFTEST_set_actions(&FISH_MAGNETIC_ARM_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_FISH_MAGNETIC_ARM_IDLE,	0,  QUEUE_ACT_RX24_FISH_MAGNETIC_ARM},
											 {ACT_FISH_MAGNETIC_ARM_OPEN,   0,  QUEUE_ACT_RX24_FISH_MAGNETIC_ARM},
											 {ACT_FISH_MAGNETIC_ARM_IDLE,   0,  QUEUE_ACT_RX24_FISH_MAGNETIC_ARM}
										 });
					SELFTEST_set_actions(&FISH_UNSTICK_ARM_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_FISH_UNSTICK_ARM_IDLE,	0,  QUEUE_ACT_RX24_FISH_UNSTICK_ARM},
											 {ACT_FISH_UNSTICK_ARM_OPEN,    0,  QUEUE_ACT_RX24_FISH_UNSTICK_ARM},
											 {ACT_FISH_UNSTICK_ARM_IDLE,	0,  QUEUE_ACT_RX24_FISH_UNSTICK_ARM}
										 });
					debug_printf("SELFTEST Launch Fishs\n");
				}
				if(state_act_tests[ACT_FISH_MAGNETIC_ARM & 0xFF] != SELFTEST_STATE_IN_PROGRESS && state_act_tests[ACT_FISH_UNSTICK_ARM & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_CONE_DUNE_OPEN;
					debug_printf("SELFTEST of ACT_FISH_MAGNETIC_ARM finished\n");
					debug_printf("SELFTEST of ACT_FISH_UNSTICK_ARM finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_FISH_MAGNETIC_ARM & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state_act_tests[ACT_FISH_UNSTICK_ARM & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT fishs\n");
					state = SELFTEST_CONE_DUNE_OPEN;
				}
				break;*/

#endif

// Anne
#ifdef I_AM_ROBOT_SMALL

#endif

			case DONE:
				//debug_printf("I am in NB_ACT\n");
				//On attend
				break;
			default:
				debug_printf("SELFTEST_state_machine: state not cased\n");
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

	state_act_tests[act_sid & 0xFF] = (test_ok)? SELFTEST_STATE_OK:SELFTEST_STATE_ERROR; //Enregistrement de l'info pour la machine à états

	if(test_ok == FALSE) {
		for(i = 0; i < MAX_NB_ACT; i++) {
			if(failed_act_tests[i] == SELFTEST_NOT_DONE) {
				switch(act_sid){
#if 0
					case ACT_EXEMPLE:
						failed_act_tests[i] = SELFTEST_ACT_EXEMPLE;
						break;
#endif

// Harry
                    case ACT_BEARING_BALL_WHEEL:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_BEARING_BALL_WHEEL;
                        break;

                    case ACT_BIG_BALL_BACK_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_BIG_BALL_BACK_LEFT;
                        break;

                    case ACT_BIG_BALL_BACK_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_BIG_BALL_BACK_RIGHT;
                        break;

                    case ACT_BIG_BALL_FRONT_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_BIG_BALL_FRONT_LEFT;
                        break;

                    case ACT_BIG_BALL_FRONT_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_BIG_BALL_FRONT_RIGHT;
                        break;

                    case ACT_CYLINDER_BALANCER_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT;
                        break;

                    case ACT_CYLINDER_BALANCER_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT;
                        break;

                    case ACT_CYLINDER_ELEVATOR_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT;
                        break;

                    case ACT_CYLINDER_ELEVATOR_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT;
                        break;

                    case ACT_CYLINDER_PUSHER_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT;
                        break;

                    case ACT_CYLINDER_PUSHER_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_CYLINDER_PUSHER_RIGHT;
                        break;

                    case ACT_CYLINDER_SLIDER_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT;
                        break;

                    case ACT_CYLINDER_SLIDER_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT;
                        break;

                    case ACT_CYLINDER_SLOPE_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT;
                        break;

                    case ACT_CYLINDER_SLOPE_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT;
                        break;

                    case ACT_CYLINDER_ARM_LEFT:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT;
                        break;

                    case ACT_CYLINDER_ARM_RIGHT:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT;
                        break;

                    case ACT_ORE_GUN:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_ORE_GUN;
                        break;

                    case ACT_ORE_ROLLER_ARM:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_ORE_ROLLER_ARM;
                        break;

                    case ACT_ORE_WALL:
                        failed_act_tests[i] = SELFTEST_ACT_RX24_ORE_WALL;
                        break;

                    case ACT_ROCKET:
                        failed_act_tests[i] = SELFTEST_ACT_AX12_ROCKET;
                        break;



// Anne



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
	SELFTEST_state_machine();
	if(!init){
		if(SELFTEST_check_end(queueId))
			QUEUE_behead(queueId);
	}
}

//Verifie si tous les actionneurs on répondu
static bool_e SELFTEST_check_end(queue_id_t queueId) {
	Uint8 i;

	if(act_test_done_num == expected_act_num || global.absolute_time >= QUEUE_get_initial_time(queueId) + SELFTEST_TIMEOUT) {
		if(global.absolute_time >= QUEUE_get_initial_time(queueId) + SELFTEST_TIMEOUT) {
			warn_printf("Timeout du selftest, reçu %d resultat vs %d attendus\n", act_test_done_num, expected_act_num);
		}

		test_finished = TRUE;
		CAN_msg_t msg;
		msg.sid = STRAT_ACT_SELFTEST_DONE;
		msg.size = 0;

		if(act_test_done_num != expected_act_num) {
			debug_printf("act_test_done_num=%d  expected_act_num=%d\n",act_test_done_num, expected_act_num);
			msg.data.strat_act_selftest_done.error_code[msg.size] = SELFTEST_ACT_MISSING_TEST;
			OUTPUTLOG_printf(LOG_LEVEL_Debug, "%3d ", msg.data.strat_act_selftest_done.error_code[msg.size]);
			msg.size++;
		}

		for(i = 0; i < MAX_NB_ACT && msg.size < 8; i++) {
			if(failed_act_tests[i] != SELFTEST_NOT_DONE) {
				msg.data.strat_act_selftest_done.error_code[msg.size] = failed_act_tests[i];
				msg.size++;
			}
		}

		debug_printf("Resultat: %d/%d réponses, erreurs: ", act_test_done_num, expected_act_num);
		for(i = 0; i < msg.size; i++)
			OUTPUTLOG_printf(LOG_LEVEL_Debug, "%3d ", msg.data.strat_act_selftest_done.error_code[i]);
		OUTPUTLOG_printf(LOG_LEVEL_Debug, "\n");
		CAN_send(&msg);
		return TRUE;
	} else if(act_test_done_num > expected_act_num) {
		error_printf("Recu plus de réponse que prévu, nb actioneurs = %d, reçu %d réponses\n", expected_act_num, act_test_done_num);
		return TRUE;
	}

	return FALSE;
}
