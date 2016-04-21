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
	#include "Black/Fishs/fish_magnetic_arm.h"
	#include "Black/Fishs/fish_unstick_arm.h"
	#include "Black/Sand_circle/black_sand_circle.h"
	#include "Black/Bottom_dune/bottom_dune.h"
	#include "Black/Middle_dune/middle_dune.h"
	#include "Black/Cone_dune/cone_dune.h"
	#include "Black/Dunix/dunix_left.h"
	#include "Black/Dunix/dunix_right.h"
	#include "Black/Sand_locker/sand_locker_left.h"
	#include "Black/Sand_locker/sand_locker_right.h"
	#include "Black/Shift_cylinder/shift_cylinder.h"
	#include "Black/Pendulum/pendulum.h"
#endif

#ifdef I_AM_ROBOT_SMALL
	#include "Pearl/Sand/left_arm.h"
	#include "Pearl/Sand/right_arm.h"
	#include "Pearl/Pompes/pompe_front_left.h"
	#include "Pearl/Pompes/pompe_front_right.h"
	#include "Pearl/Sand/pearl_sand_circle.h"
	#include "Pearl/Parasol/parasol.h"
#endif

#define LOG_PREFIX "Selftest: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SELFTEST
#include "QS/QS_outputlog.h"

#define MAX_NB_ACT 64
#define SELFTEST_TIMEOUT 30000 // en ms (30sec)  //Duree du timeout du selftest complet
#define ACT_TIMEOUT  5000 //en ms (5sec)  //Duree du timeout par actionneur

typedef enum{
	SELFTEST_FISHS,
	SELFTEST_CONE_DUNE_OPEN,
	SELFTEST_BOTTOM_DUNE_OPEN,
	SELFTEST_MIDDLE_DUNE_OPEN,
	SELFTEST_MIDDLE_DUNE_CLOSE,
	SELFTEST_BOTTOM_DUNE_CLOSE,
	SELFTEST_CONE_DUNE_CLOSE,
	SELFTEST_BLACK_SAND_CIRCLE,
	SELFTEST_DUNIX_LEFT,
	SELFTEST_DUNIX_RIGHT,
	SELFTEST_SAND_LOCKER_LEFT,
	SELFTEST_SAND_LOCKER_RIGHT,
	SELFTEST_SHIFT_CYLINDER,
	SELFTEST_PENDULUM,
	SELFTEST_LEFT_ARM,
	SELFTEST_RIGHT_ARM,
	SELFTEST_PEARL_SAND_CIRCLE,
	SELFTEST_PARASOL,
	SELFTEST_POMPE_FRONT_LEFT,
	SELFTEST_POMPE_FRONT_RIGHT,
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

	if(I_AM_BIG())
		state = SELFTEST_FISHS;
	else
		state = SELFTEST_LEFT_ARM;
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
	bool_e entrance = (last_state == state)? FALSE:TRUE;
	last_state = state;
	if(entrance)
		time_for_timeout = global.absolute_time;

		switch(state){

//Black
#ifdef I_AM_ROBOT_BIG
			case SELFTEST_FISHS:
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
				break;

			case SELFTEST_CONE_DUNE_OPEN:
				if(entrance){
					SELFTEST_set_actions(&CONE_DUNE_run_command, 2, (SELFTEST_action_t[]){
											 {ACT_CONE_DUNE_IDLE,		0,  QUEUE_ACT_RX24_CONE_DUNE},
											 {ACT_CONE_DUNE_LOCK,       0,  QUEUE_ACT_RX24_CONE_DUNE}
										 });
					debug_printf("SELFTEST Launch Cone\n");
				}
				if(state_act_tests[ACT_CONE_DUNE & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_BOTTOM_DUNE_OPEN;
					debug_printf("SELFTEST of ACT_CONE_DUNE finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_CONE_DUNE & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT cone\n");
					state = SELFTEST_BOTTOM_DUNE_OPEN;
				}
				break;


			case SELFTEST_BOTTOM_DUNE_OPEN:
				if(entrance){
					SELFTEST_set_actions(&BOTTOM_DUNE_run_command, 2, (SELFTEST_action_t[]){
											 {ACT_BOTTOM_DUNE_IDLE,		0,  QUEUE_ACT_RX24_BOTTOM_DUNE},
											 {ACT_BOTTOM_DUNE_LOCK,		0,  QUEUE_ACT_RX24_BOTTOM_DUNE}
										 });
					debug_printf("SELFTEST Launch Bottom\n");
				}
				if(state_act_tests[ACT_BOTTOM_DUNE & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_MIDDLE_DUNE_OPEN;
					debug_printf("SELFTEST of ACT_BOTTOM_DUNE finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_BOTTOM_DUNE & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Bottom dune\n");
					state = SELFTEST_MIDDLE_DUNE_OPEN;
				}
				break;

			case SELFTEST_MIDDLE_DUNE_OPEN:
				if(entrance){
					debug_printf("SELFTEST Launch Middle\n");
					SELFTEST_set_actions(&MIDDLE_DUNE_run_command, 2, (SELFTEST_action_t[]){
											 {ACT_MIDDLE_DUNE_IDLE,		0,  QUEUE_ACT_RX24_MIDDLE_DUNE},
											 {ACT_MIDDLE_DUNE_LOCK,		0,  QUEUE_ACT_RX24_MIDDLE_DUNE}
										 });
				}
				if(state_act_tests[ACT_MIDDLE_DUNE & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_MIDDLE_DUNE_CLOSE;
					debug_printf("SELFTEST of ACT_MIDDLE_DUNE finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_MIDDLE_DUNE & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Middle dune\n");
					state = SELFTEST_MIDDLE_DUNE_CLOSE;
				}
				break;

			case SELFTEST_MIDDLE_DUNE_CLOSE:
				if(entrance){
					CAN_msg_t msg;
					msg.sid = ACT_MIDDLE_DUNE;
					msg.size = SIZE_ACT_MSG;
					msg.data.act_msg.order = ACT_MIDDLE_DUNE_IDLE;
					CAN_process_msg(&msg);
					debug_printf("SELFTEST Launch Middle\n");
				}
				if(global.absolute_time >= time_for_timeout + 500){
					state = SELFTEST_BOTTOM_DUNE_CLOSE;
				}
				break;


			case SELFTEST_BOTTOM_DUNE_CLOSE:
				if(entrance){
					CAN_msg_t msg;
					msg.sid = ACT_BOTTOM_DUNE;
					msg.size = SIZE_ACT_MSG;
					msg.data.act_msg.order = ACT_BOTTOM_DUNE_IDLE;
					CAN_process_msg(&msg);
					debug_printf("SELFTEST Launch Bottom\n");
				}
				if(global.absolute_time >= time_for_timeout + 500){
					state = SELFTEST_CONE_DUNE_CLOSE;
				}
				break;

			case SELFTEST_CONE_DUNE_CLOSE:
				if(entrance){
					CAN_msg_t msg;
					msg.sid = ACT_CONE_DUNE;
					msg.size = SIZE_ACT_MSG;
					msg.data.act_msg.order = ACT_CONE_DUNE_IDLE;
					CAN_process_msg(&msg);
					debug_printf("SELFTEST Launch Cone\n");
				}
				if(global.absolute_time >= time_for_timeout + 500){
					state = SELFTEST_DUNIX_LEFT;
				}
				break;

			case SELFTEST_DUNIX_LEFT:
				if(entrance){
					SELFTEST_set_actions(&DUNIX_LEFT_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_DUNIX_LEFT_IDLE,		0,  QUEUE_ACT_RX24_DUNIX_LEFT},
											 {ACT_DUNIX_LEFT_OPEN,      0,  QUEUE_ACT_RX24_DUNIX_LEFT},
											 {ACT_DUNIX_LEFT_IDLE,		0,  QUEUE_ACT_RX24_DUNIX_LEFT}
										 });
					debug_printf("SELFTEST Launch Dunix left\n");
				}
				if(state_act_tests[ACT_DUNIX_LEFT & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_DUNIX_RIGHT;
					debug_printf("SELFTEST of ACT_DUNIX_LEFT finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_DUNIX_LEFT & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Dunix left\n");
					state = SELFTEST_DUNIX_RIGHT;
				}
				break;

			case SELFTEST_DUNIX_RIGHT:
				if(entrance){
					SELFTEST_set_actions(&DUNIX_RIGHT_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_DUNIX_RIGHT_IDLE,		0,  QUEUE_ACT_RX24_DUNIX_RIGHT},
											 {ACT_DUNIX_RIGHT_OPEN,     0,  QUEUE_ACT_RX24_DUNIX_RIGHT},
											 {ACT_DUNIX_RIGHT_IDLE,		0,  QUEUE_ACT_RX24_DUNIX_RIGHT}
										 });
					debug_printf("SELFTEST Launch Dunix right\n");
				}
				if(state_act_tests[ACT_DUNIX_RIGHT & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_BLACK_SAND_CIRCLE;
					debug_printf("SELFTEST of ACT_DUNIX_RIGHT finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_DUNIX_RIGHT & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Dunix right\n");
					state = SELFTEST_BLACK_SAND_CIRCLE;
				}
				break;

			case SELFTEST_BLACK_SAND_CIRCLE:
				if(entrance){
					SELFTEST_set_actions(&BLACK_SAND_CIRCLE_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_BLACK_SAND_CIRCLE_IDLE,		0,  QUEUE_ACT_RX24_BLACK_SAND_CIRCLE},
											 {ACT_BLACK_SAND_CIRCLE_LOCK,       0,  QUEUE_ACT_RX24_BLACK_SAND_CIRCLE},
											 {ACT_BLACK_SAND_CIRCLE_IDLE,		0,  QUEUE_ACT_RX24_BLACK_SAND_CIRCLE}
										 });
					debug_printf("SELFTEST Launch Sand circle\n");
				}
				if(state_act_tests[ACT_BLACK_SAND_CIRCLE & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_SAND_LOCKER_LEFT;
					debug_printf("SELFTEST of ACT_BLACK_SAND_CIRCLE finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_BLACK_SAND_CIRCLE & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Black Sand circle\n");
					state = SELFTEST_SAND_LOCKER_LEFT;
				}
				break;

			case SELFTEST_SAND_LOCKER_LEFT:
				if(entrance){
					SELFTEST_set_actions(&SAND_LOCKER_LEFT_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_SAND_LOCKER_LEFT_IDLE,	0,  QUEUE_ACT_RX24_SAND_LOCKER_LEFT},
											 {ACT_SAND_LOCKER_LEFT_LOCK,    0,  QUEUE_ACT_RX24_SAND_LOCKER_LEFT},
											 {ACT_SAND_LOCKER_LEFT_IDLE,	0,  QUEUE_ACT_RX24_SAND_LOCKER_LEFT}
										 });
					debug_printf("SELFTEST Launch Sand locker left\n");
				}
				if(state_act_tests[ACT_SAND_LOCKER_LEFT & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_SAND_LOCKER_RIGHT;
					debug_printf("SELFTEST of ACT_SAND_LOCKER_LEFT finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_SAND_LOCKER_LEFT & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Sand locker left\n");
					state = SELFTEST_SAND_LOCKER_RIGHT;
				}
				break;

			case SELFTEST_SAND_LOCKER_RIGHT:
				if(entrance){
					SELFTEST_set_actions(&SAND_LOCKER_RIGHT_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_SAND_LOCKER_RIGHT_IDLE,	0,  QUEUE_ACT_RX24_SAND_LOCKER_RIGHT},
											 {ACT_SAND_LOCKER_RIGHT_LOCK,   0,  QUEUE_ACT_RX24_SAND_LOCKER_RIGHT},
											 {ACT_SAND_LOCKER_RIGHT_IDLE,	0,  QUEUE_ACT_RX24_SAND_LOCKER_RIGHT}
										 });
					debug_printf("SELFTEST Launch Sand locker right\n");
				}
				if(state_act_tests[ACT_SAND_LOCKER_RIGHT & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_SHIFT_CYLINDER;
					debug_printf("SELFTEST of ACT_SAND_LOCKER_RIGHT finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_SAND_LOCKER_RIGHT & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("Timeout Sand locker right");
					state = SELFTEST_SHIFT_CYLINDER;
				}
				break;

			case SELFTEST_SHIFT_CYLINDER:
				if(entrance){
					SELFTEST_set_actions(&SHIFT_CYLINDER_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_SHIFT_CYLINDER_IDLE,	 0,  QUEUE_ACT_RX24_SHIFT_CYLINDER},
											 {ACT_SHIFT_CYLINDER_OPEN,   0,  QUEUE_ACT_RX24_SHIFT_CYLINDER},
											 {ACT_SHIFT_CYLINDER_IDLE,   0,  QUEUE_ACT_RX24_SHIFT_CYLINDER}
										 });
					debug_printf("SELFTEST Launch Shift cylinder\n");
				}
				if(state_act_tests[ACT_SHIFT_CYLINDER & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_PENDULUM;
					debug_printf("SELFTEST of ACT_SHIFT_CYLINDER finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_SHIFT_CYLINDER & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Shift cylinder\n");
					state = SELFTEST_PENDULUM;
				}
				break;

			case SELFTEST_PENDULUM:
				if(entrance){
					SELFTEST_set_actions(&PENDULUM_run_command, 3, (SELFTEST_action_t[]){
											 {ACT_PENDULUM_IDLE,   0,  QUEUE_ACT_RX24_PENDULUM},
											 {ACT_PENDULUM_OPEN,   0,  QUEUE_ACT_RX24_PENDULUM},
											 {ACT_PENDULUM_IDLE,   0,  QUEUE_ACT_RX24_PENDULUM}
										 });
					debug_printf("SELFTEST Launch Pendulum\n");
				}
				if(state_act_tests[ACT_PENDULUM & 0xFF] != SELFTEST_STATE_IN_PROGRESS ){
					state = DONE;
					debug_printf("SELFTEST of ACT_PENDULUM finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_PENDULUM & 0xFF] = SELFTEST_STATE_TIMEOUT;
					debug_printf("TIMEOUT Pendulum\n");
					state = DONE;
				}
				break;

#endif

//Pearl
#ifdef I_AM_ROBOT_SMALL
			case SELFTEST_LEFT_ARM:
				if(entrance){
					SELFTEST_set_actions(&LEFT_ARM_run_command, 4, (SELFTEST_action_t[]){
													 {ACT_LEFT_ARM_IDLE,		0,  QUEUE_ACT_AX12_LEFT_ARM},
													 {ACT_LEFT_ARM_UNLOCK,      0,  QUEUE_ACT_AX12_LEFT_ARM},
													 {ACT_LEFT_ARM_LOCK,        0,  QUEUE_ACT_AX12_LEFT_ARM},
													 {ACT_LEFT_ARM_IDLE,		0,  QUEUE_ACT_AX12_LEFT_ARM}
												 });
				}
				if(state_act_tests[ACT_LEFT_ARM & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_RIGHT_ARM;
					debug_printf("SELFTEST of ACT_LEFT_ARM finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_LEFT_ARM & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state = SELFTEST_RIGHT_ARM;
				}
				break;

			case  SELFTEST_RIGHT_ARM:
				if(entrance){
					SELFTEST_set_actions(&RIGHT_ARM_run_command, 4, (SELFTEST_action_t[]){
													 {ACT_RIGHT_ARM_IDLE,		0,  QUEUE_ACT_AX12_RIGHT_ARM},
													 {ACT_RIGHT_ARM_UNLOCK,     0,  QUEUE_ACT_AX12_RIGHT_ARM},
													 {ACT_RIGHT_ARM_LOCK,       0,  QUEUE_ACT_AX12_RIGHT_ARM},
													 {ACT_RIGHT_ARM_IDLE,		0,  QUEUE_ACT_AX12_RIGHT_ARM}
												 });
				}
				if(state_act_tests[ACT_RIGHT_ARM & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state =  SELFTEST_PEARL_SAND_CIRCLE;
					debug_printf("SELFTEST of ACT_RIGHT_ARM finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_RIGHT_ARM & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state =  SELFTEST_PEARL_SAND_CIRCLE;
				}
				break;

			case  SELFTEST_PEARL_SAND_CIRCLE:
				if(entrance){
					SELFTEST_set_actions(&PEARL_SAND_CIRCLE_run_command, 3, (SELFTEST_action_t[]){
													 {ACT_PEARL_SAND_CIRCLE_IDLE,		0,  QUEUE_ACT_AX12_PEARL_SAND_CIRCLE},
													 {ACT_PEARL_SAND_CIRCLE_LOCK,       0,  QUEUE_ACT_AX12_PEARL_SAND_CIRCLE},
													 {ACT_PEARL_SAND_CIRCLE_IDLE,		0,  QUEUE_ACT_AX12_PEARL_SAND_CIRCLE}
												 });
				}
				if(state_act_tests[ACT_PEARL_SAND_CIRCLE & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_PARASOL;
					debug_printf("SELFTEST of ACT_PEARL_SAND_CIRCLE finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_PEARL_SAND_CIRCLE & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state = SELFTEST_PARASOL;
				}
				break;

			case  SELFTEST_PARASOL:
				if(entrance){
					SELFTEST_set_actions(&PARASOL_run_command, 1, (SELFTEST_action_t[]){
													 {ACT_PARASOL_OPEN,      0,  QUEUE_ACT_AX12_PARASOL}
												 });
				}
				if(state_act_tests[ACT_PARASOL & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_POMPE_FRONT_LEFT;
					debug_printf("SELFTEST of ACT_PARASOL finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_PARASOL & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state = SELFTEST_POMPE_FRONT_LEFT;
				}
				break;

			case  SELFTEST_POMPE_FRONT_LEFT:
				if(entrance){
					SELFTEST_set_actions(&POMPE_FRONT_LEFT_run_command, 3, (SELFTEST_action_t[]){
													 {ACT_POMPE_NORMAL,	  100,  QUEUE_ACT_POMPE_FRONT_LEFT},
													 {ACT_POMPE_REVERSE,   100,  QUEUE_ACT_POMPE_FRONT_LEFT},
													 {ACT_POMPE_STOP,		0,  QUEUE_ACT_POMPE_FRONT_LEFT}
												 });
				}
				if(state_act_tests[ACT_POMPE_FRONT_LEFT & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = SELFTEST_POMPE_FRONT_RIGHT;
					debug_printf("SELFTEST of ACT_POMPE_FRONT_LEFT finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_POMPE_FRONT_LEFT & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state = SELFTEST_POMPE_FRONT_RIGHT;
				}
				break;

			case  SELFTEST_POMPE_FRONT_RIGHT:
				if(entrance){
					SELFTEST_set_actions(&POMPE_FRONT_RIGHT_run_command, 3, (SELFTEST_action_t[]){
													 {ACT_POMPE_NORMAL,	  100,  QUEUE_ACT_POMPE_FRONT_RIGHT},
													 {ACT_POMPE_REVERSE,  100,  QUEUE_ACT_POMPE_FRONT_RIGHT},
													 {ACT_POMPE_STOP,		0,  QUEUE_ACT_POMPE_FRONT_RIGHT}
												 });
				}
				if(state_act_tests[ACT_POMPE_FRONT_RIGHT & 0xFF] != SELFTEST_STATE_IN_PROGRESS){
					state = DONE;
					debug_printf("SELFTEST of ACT_POMPE_FRONT_RIGHT finished\n");
				}else if(global.absolute_time >= time_for_timeout + ACT_TIMEOUT){
					state_act_tests[ACT_POMPE_FRONT_RIGHT & 0xFF] = SELFTEST_STATE_TIMEOUT;
					state = DONE;
				}
				break;
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

// Black
					case ACT_FISH_MAGNETIC_ARM:
						failed_act_tests[i] = SELFTEST_ACT_RX24_FISH_MAGNETIC_ARM;
						break;
					case ACT_FISH_UNSTICK_ARM:
						failed_act_tests[i] = SELFTEST_ACT_RX24_FISH_UNSTICK_ARM;
						break;
					case ACT_BLACK_SAND_CIRCLE:
						failed_act_tests[i] = SELFTEST_ACT_RX24_BLACK_SAND_CIRCLE;
						break;
					case ACT_BOTTOM_DUNE:
						failed_act_tests[i] = SELFTEST_ACT_RX24_BOTTOM_DUNE;
						break;
					case ACT_MIDDLE_DUNE:
						failed_act_tests[i] = SELFTEST_ACT_RX24_MIDDLE_DUNE;
						break;
					case ACT_CONE_DUNE:
						failed_act_tests[i] = SELFTEST_ACT_RX24_CONE_DUNE;
						break;
					case ACT_SAND_LOCKER_LEFT:
						failed_act_tests[i] = SELFTEST_ACT_RX24_SAND_LOCKER_LEFT;
						break;
					case ACT_SAND_LOCKER_RIGHT:
						failed_act_tests[i] = SELFTEST_ACT_RX24_SAND_LOCKER_RIGHT;
						break;
					case ACT_DUNIX_LEFT:
						failed_act_tests[i] = SELFTEST_ACT_RX24_DUNIX_LEFT;
						break;
					case ACT_DUNIX_RIGHT:
						failed_act_tests[i] = SELFTEST_ACT_RX24_DUNIX_RIGHT;
						break;
					case ACT_SHIFT_CYLINDER:
						failed_act_tests[i] = SELFTEST_ACT_RX24_SHIFT_CYLINDER;
						break;
					case ACT_PENDULUM:
						failed_act_tests[i] = SELFTEST_ACT_RX24_PENDULUM;
						break;


// Pearl
					case ACT_LEFT_ARM:
						failed_act_tests[i] = SELFTEST_ACT_AX12_LEFT_ARM;
						break;
					case ACT_RIGHT_ARM:
						failed_act_tests[i] = SELFTEST_ACT_AX12_RIGHT_ARM;
						break;
					case ACT_PEARL_SAND_CIRCLE:
						failed_act_tests[i] = SELFTEST_ACT_AX12_SAND_CIRCLE;
						break;
					case ACT_PARASOL:
						failed_act_tests[i] = SELFTEST_ACT_AX12_PARASOL;
						break;
					case ACT_POMPE_FRONT_LEFT:
						failed_act_tests[i] = SELFTEST_ACT_POMPE_FRONT_LEFT;
						break;
					case ACT_POMPE_FRONT_RIGHT:
						failed_act_tests[i] = SELFTEST_ACT_POMPE_FRONT_RIGHT;
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
