/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Gestion du bras
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#include "Arm.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_who_am_i.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"
#ifdef I_AM_ROBOT_BIG
	#include "../Pierre/Arm_config.h"
#else
	#include "../Guy/Arm_config.h"
#endif
#include "Arm_data.h"
#include "../maths_home.h"
#include <string.h>
#include <math.h>

#include "config_debug.h"
#define LOG_PREFIX "Arm: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ARM
#include "../QS/QS_outputlog.h"

#define LONGUEUR_AVANT_BRAS_PIERRE		138
#define	LONGUEUR_BRAS_PIERRE			126
#define LONGUEUR_AVANT_BRAS_GUY			140
#define	LONGUEUR_BRAS_GUY				84

#define BRAS_POS_X_PIERRE				50
#define BRAS_POS_Y_PIERRE				50
#define BRAS_POS_X_GUY					4.07
#define BRAS_POS_Y_GUY					4.07

#define RAYON_MIN_PIERRE			10
#define RAYON_MAX_PIERRE			208
#define RAYON_MIN_GUY				110
#define RAYON_MAX_GUY				230

#define DISTANCE_MAX_TO_TAKE		50

#ifdef I_AM_ROBOT_SMALL
	#define TIME_TO_INC_RUSH			20		//ms
	#define INC_RUSH					5
	#define TIME_RUSH_IN_FLOOR			5000
	#define DIFF_POS_FICT_RUSH			45
	#define EPSILON_POS_RUSH_FLOOR		20
#else
	#define TIME_TO_INC_RUSH			20		//ms
	#define INC_RUSH					-5
	#define TIME_RUSH_IN_FLOOR			5000
	#define DIFF_POS_FICT_RUSH			25
	#define EPSILON_POS_RUSH_FLOOR		20
#endif


#define square(x)				((Sint32)(x)*(x))
#define SIGNE(x)				(((x) >= 0)?1:-1)
#define PROTECTION_SIN_COS(x)	(((x) > 1)?1:(((x) <-1)?-1:x))

#ifdef I_AM_ROBOT_SMALL
	#define conv_dist_to_potar_updown(x) ((Sint16)(-3.1822*(x)+29.086))
	#define conv_potar_updown_to_dist(x) ((Sint16)(-(x)/3.1822+29.086/3.1822))
#else
	#define conv_dist_to_potar_updown(x) ((Sint16)(2.662*(x)-462.4))
	#define conv_potar_updown_to_dist(x) ((Sint16)((x)*0.374+159.9))
#endif


typedef struct{
	Sint16 x;
	Sint16 y;
	Sint16 z;
	Uint16 rayon;
	Sint32 angle;
	Sint32 real_angle;
}data_pos_triangle_s;

typedef struct{
	Uint16 rayon;
	Sint16 value_ax12_right;
	Sint16 value_ax12_left;
}rayon_pos_triangle_s;

typedef struct{
	Sint16 angle;
	Sint16 value_rx24;
	Uint16 rayon_min_right;
	Uint16 rayon_min_left;
}angle_pos_triangle_s;

typedef struct{
	Uint8 i_min_angle;
	Uint8 i_min_rayon;
	enum{RIGHT, LEFT} arm_way;
}data_arm_triangle_s;

#ifdef I_AM_ROBOT_BIG
	static const rayon_pos_triangle_s rayon_pos_triangle[] = {
		{110,	266,	-1},
		{120,	258,	-1},
		{130,	250,	-1},
		{140,	242,	41},
		{150,	236,	49},
		{160,	224,	60},
		{170,	218,	75},
		{180,	210,	80},
		{190,	200,	91},
		{200,	176,	107},
		{210,	142,	123},
		{220,	148,    148}
	};

	static const angle_pos_triangle_s angle_pos_triangle[] = {
		{0,				110,	170,	0},
		{10*PI4096/180,	120,	170,	0},
		{20*PI4096/180,	130,	160,	0},
		{30*PI4096/180,	140,	140,	0},
		{40*PI4096/180,	150,	0,		0},
		{50*PI4096/180,	160,	0,		150},
		{60*PI4096/180,	170,	0,		170},
		{70*PI4096/180,	180,	0,		170},
		{80*PI4096/180,	185,	0,		170}
	};
#else
	static const rayon_pos_triangle_s rayon_pos_triangle[] = {
		{110,	267,	-1},
		{115,	263,	-1},
		{120,	261,	-1},
		{125,	257,	-1},
		{130,	254,	-1},
		{135,	249,	-1},
		{140,	246,	-1},
		{145,	240,	-1},
		{150,	237,	39},
		{155,	234,	42},
		{160,	231,	44},
		{165,	228,	47},
		{170,	226,	52},
		{175,	220,	56},
		{180,	216,	60},
		{185,	212,	64},
		{190,	208,	68},
		{195,	202,	62},
		{200,	197,	78},
		{205,	190,	82},
		{210,	185,	87},
		{215,	180,	93},
		{220,	177,    103},
		{225,	170,    124},
		{230,	163,    147}
	};

	static const angle_pos_triangle_s angle_pos_triangle[] = {
		{0,				110,	170,	0},
		{10*PI4096/180,	120,	170,	0},
		{15*PI4096/180,	125,	170,	0},
		{20*PI4096/180,	130,	160,	0},
		{25*PI4096/180,	135,	160,	0},
		{30*PI4096/180,	140,	140,	0},
		{35*PI4096/180,	145,	140,	0},
		{40*PI4096/180,	150,	0,		0},
		{45*PI4096/180,	155,	0,		0},
		{50*PI4096/180,	160,	0,		150},
		{55*PI4096/180,	165,	0,		150},
		{60*PI4096/180,	170,	0,		170},
		{65*PI4096/180,	175,	0,		170},
		{70*PI4096/180,	180,	0,		170},
		{75*PI4096/180,	185,	0,		170},
		{80*PI4096/180,	185,	0,		170}
	};
#endif

static const Uint8 taille_rayon_pos_triangle = sizeof(rayon_pos_triangle)/sizeof(rayon_pos_triangle_s);
static const Uint8 taille_angle_pos_triangle = sizeof(angle_pos_triangle)/sizeof(angle_pos_triangle_s);

static data_pos_triangle_s data_pos_triangle;
static data_arm_triangle_s data_arm_triangle;

#define XX(val) #val,
const char* ARM_STATES_NAME[] = {
	ARM_STATE_ENUMVALS(XX)
};
#undef XX

static void ARM_initAX12();
static bool_e gotoState(ARM_state_e state);
static bool_e check_state_transitions();
static void print_state_transitions(bool_e correct);
static Sint8 find_state();
static void get_data_pos_triangle(CAN_msg_t* msg);
static bool_e goto_triangle_pos();
static void get_data_pos_triangle(CAN_msg_t* msg);
static Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);
static bool_e move_updown_to(Sint16 pos);
static bool_e check_state_and_rush_in_floor(Uint8 dcmotor_id, bool_e timeout_is_ok, Uint8* result, Uint8* error_code, Uint16* line, bool_e init, Sint16 order);
static bool_e find_state_path(Sint8 begin_state, Sint8 end_state);
static bool_e est_dans_liste(Sint8 liste[], Uint8 taille, Sint8 search);
static Sint8 ajout_liste(Sint8 liste[], Uint8 taille, Sint8 val);
static Sint8 del_liste(Sint8 liste[], Uint8 taille, Sint8 val);
static Sint16 modulo_angle(Sint32 angle);

static Sint8 old_state = -1;
static Sint8 switch_state[ARM_ST_NUMBER];
static Uint8 taille_path = 0;

void ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	PORTS_pwm_init();
	PWM_init();
	DCM_init();
	AX12_init();
	ADC_init();

	init_perm_transitions_table();
	if(check_all_state_perm_transitions_initialized() == FALSE)
		debug_printf("Attention toutes les transitions d'état du bras n'ont pas été paramètrées !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	//check_state_transitions();
//	if(check_state_transitions() == FALSE)
//		print_state_transitions(TRUE);

	Uint8 i;
	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCMotor_config_t dcconfig;

			dcconfig.sensor_read = ARM_MOTORS[i].sensorRead;
			dcconfig.double_PID = ARM_MOTORS[i].double_PID;
			dcconfig.Kp = ARM_MOTORS[i].kp;
			dcconfig.Ki = ARM_MOTORS[i].ki;
			dcconfig.Kd = ARM_MOTORS[i].kd;
			dcconfig.Kp2 = ARM_MOTORS[i].kp2;
			dcconfig.Ki2 = ARM_MOTORS[i].ki2;
			dcconfig.Kd2 = ARM_MOTORS[i].kd2;
			dcconfig.pos[0] = 0;
			dcconfig.pwm_number = ARM_MOTORS[i].pwmNum;
			dcconfig.way_latch = &ARM_MOTORS[i].pwmWayPort->ODR;
			dcconfig.way_bit_number = ARM_MOTORS[i].pwmWayBit;
			dcconfig.way0_max_duty = ARM_MOTORS[i].maxPowerWay0;
			dcconfig.way1_max_duty = ARM_MOTORS[i].maxPowerWay1;
			dcconfig.timeout = ARM_MOTORS[i].timeout;
			dcconfig.epsilon = ARM_MOTORS[i].epsilon;
			DCM_config(ARM_MOTORS[i].id, &dcconfig);
			DCM_stop(ARM_MOTORS[i].id);
		} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			//pas initialisé ici mais dans ARM_initAX12 en dessous
		}
	}

	info_printf("Bras initialisé (DCMotor)\n");



	ARM_initAX12();
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void ARM_initAX12(){
	static bool_e allInitialized = FALSE;
	Uint8 i;
	Uint16 A, B, C, D;
	bool_e allOk = TRUE;

	if(allInitialized)
		return;

	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			if(ARM_ax12_is_initialized[i] == FALSE && AX12_is_ready(ARM_MOTORS[i].id) == TRUE) {
				ARM_ax12_is_initialized[i] = TRUE;

				AX12_config_set_highest_voltage(ARM_MOTORS[i].id, 150);
				AX12_config_set_lowest_voltage(ARM_MOTORS[i].id, 60);
				AX12_config_set_maximum_torque_percentage(ARM_MOTORS[i].id, ARM_MOTORS[i].maxPowerWay0);
				AX12_set_torque_limit(ARM_MOTORS[i].id, ARM_MOTORS[i].maxPowerWay0);
				AX12_config_set_maximal_angle(ARM_MOTORS[i].id, ARM_MOTORS[i].max_value);
				AX12_config_set_minimal_angle(ARM_MOTORS[i].id, ARM_MOTORS[i].min_value);

				AX12_config_set_error_before_led(ARM_MOTORS[i].id, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
				AX12_config_set_error_before_shutdown(ARM_MOTORS[i].id, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.
#ifdef I_AM_ROBOT_SMALL
				if(ARM_MOTORS[i].type == ARM_RX24){
					AX12_set_punch_torque_percentage(ARM_MOTORS[i].id, 60);
					AX12_set_torque_response(ARM_MOTORS[i].id, 2, 1, 0, 2);
				}else{
					AX12_set_punch_torque_percentage(ARM_MOTORS[i].id, 20);
					AX12_set_torque_response(ARM_MOTORS[i].id, 4, 1, 0, 4);
				}
#endif
				AX12_get_torque_response(ARM_MOTORS[i].id, &A, &B, &C, &D);
				display(ARM_MOTORS[i].id);
				display(AX12_get_punch_torque_percentage(ARM_MOTORS[i].id));
				display(A);
				display(B);
				display(C);
				display(D);

			} else if(ARM_ax12_is_initialized[i] == FALSE) {
				// Au moins un RX24/AX12 non prêt => pas allOk, on affiche pas le message d'init
				debug_printf("AX12 %d absent\n", ARM_MOTORS[i].id);
				allOk = FALSE;
			}
		}
	}

	if(allOk) {
		info_printf("AX12 et RX24 du bras initialisés\n");
		allInitialized = TRUE;
	}

	debug_printf("Arm init config %s\n", allInitialized ? "DONE" : "FAIL");
}

void ARM_init_pos() {
	CAN_msg_t msg = {ACT_ARM, {ACT_ARM_INIT}, 1};
	debug_printf("Arm init pos :\n");
#ifdef I_AM_ROBOT_SMALL
	find_state();
	ARM_CAN_process_msg(&msg);
#else
	if(find_state() == -1 || find_state() == ACT_ARM_POS_PRE_PARKED_1){
		old_state = -1;
		ARM_CAN_process_msg(&msg);
	}else{
		msg.sid = ACT_ARM;
		msg.data[0] = ACT_ARM_GOTO;
		msg.data[1] = ACT_ARM_POS_PRE_PARKED_1;
		msg.size = 2;
		ARM_CAN_process_msg(&msg);
	}
#endif

}

void ARM_stop() {
	Uint8 i;
	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCM_stop(ARM_MOTORS[i].id);
		} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			AX12_set_torque_enabled(ARM_MOTORS[i].id, FALSE);
		}
	}
}

bool_e ARM_CAN_process_msg(CAN_msg_t* msg) {
	bool_e stateOk;
	Uint8 i;
	if(msg->sid == ACT_ARM) {
		//Initialise les RX24/AX12 du bras s'ils n'étaient pas alimentés lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		ARM_initAX12();

		switch(msg->data[0]) {
			case ACT_ARM_GOTO:
				if(msg->data[1] == ACT_ARM_POS_ON_TRIANGLE || msg->data[1] == ACT_ARM_POS_ON_TORCHE)
					get_data_pos_triangle(msg);
				if(old_state == msg->data[1]){}
				else if(arm_states_transitions[old_state][msg->data[1]] == 1)
					ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[1],TRUE);
				else if(find_state_path(old_state, msg->data[1])){
					queue_id_t queueId = QUEUE_create();
					assert(queueId != QUEUE_CREATE_FAILED);
					if(queueId != QUEUE_CREATE_FAILED){
						Uint8 i;
						QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);

						for(i = 0; i < taille_path-1; i++) {
							QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], switch_state[i], &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_Arm);
							debug_printf("-> %s", ARM_STATES_NAME[switch_state[i]]);
						}
						QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], switch_state[i], &ACTQ_finish_SendResult}, QUEUE_ACT_Arm);
						debug_printf("-> %s", ARM_STATES_NAME[switch_state[i]]);
						debug_printf("\n");

						QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);
					}else	//on indique qu'on a pas géré la commande
						ACTQ_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}else
					ACTQ_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG);
				break;

			case ACT_ARM_UPDOWN_GOTO :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[1],TRUE);
				break;

			case ACT_ARM_UPDOWN_RUSH_IN_FLOOR :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[1],TRUE);
				break;

			case ACT_ARM_STOP:
				ARM_stop();
				break;

			case ACT_ARM_INIT:{
				queue_id_t queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				Sint8 temp_old_state = old_state;
				if(queueId != QUEUE_CREATE_FAILED){
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);
#ifdef I_AM_ROBOT_SMALL
					if(temp_old_state < 0){
						QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], ACT_ARM_POS_OPEN, &ACTQ_finish_SendNothing}, QUEUE_ACT_Arm);
						temp_old_state = ACT_ARM_POS_OPEN;
					}else if(temp_old_state == POS_INIT_ARM || arm_states_transitions[temp_old_state][POS_INIT_ARM] == 1){
						if(temp_old_state == POS_INIT_ARM)
							old_state = -1;
						QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], POS_INIT_ARM, &ACTQ_finish_SendNothing}, QUEUE_ACT_Arm);
					}else if(find_state_path(temp_old_state, POS_INIT_ARM)){
						Uint8 i;
						for(i = 0; i < taille_path; i++){
							QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], switch_state[i], &ACTQ_finish_SendNothing}, QUEUE_ACT_Arm);
							debug_printf("-> %s", ARM_STATES_NAME[switch_state[i]]);
						}
						debug_printf("\n");
					}else
						ACTQ_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG);
#else
					stateOk = TRUE;

					for(i = 0; i < ARM_MOTORS_NUMBER; i++){
						if(ARM_MOTORS[i].type == ARM_DCMOTOR){
							if(absolute(ARM_MOTORS[i].sensorRead() - ARM_get_motor_pos(ACT_ARM_POS_PRE_PARKED_1, i)) >= 40){
								stateOk = FALSE;
								break;
							}
						}else if(ARM_MOTORS[i].id == ARM_ACT_RX24_ID){
							if(absolute(AX12_get_position(ARM_MOTORS[i].id) - ARM_get_motor_pos(ACT_ARM_POS_PRE_PARKED_1, i)) >= 25){
								stateOk = FALSE;
								break;
							}
						}else if(ARM_MOTORS[i].id == ARM_ACT_AX12_MID_ID){
							if(absolute(AX12_get_position(ARM_MOTORS[i].id) - ARM_get_motor_pos(ACT_ARM_POS_PRE_PARKED_1, i)) >= 25){
								stateOk = FALSE;
								break;
							}
						}else if(ARM_MOTORS[i].id == ARM_ACT_AX12_TRIANGLE_ID){
							if(absolute(AX12_get_position(ARM_MOTORS[i].id) - ARM_get_motor_pos(ACT_ARM_POS_PRE_PARKED_1, i)) >= 40){
								stateOk = FALSE;
								break;
							}
						}
					}

					if(stateOk){
						QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], ACT_ARM_POS_PRE_PARKED_1, &ACTQ_finish_SendNothing}, QUEUE_ACT_Arm);
					}else{
						debug_printf("Echec d'initialisation du bras !\n");
					}

#endif
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);
				}else
					ACTQ_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);

				break;
			}

			case ACT_ARM_PRINT_POS: {
				Uint8 i;
				bool_e firstValue = TRUE;

				info_printf("Positions:\n");
				OUTPUTLOG_printf(LOG_LEVEL_Info, "    {");
				for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
					if(!firstValue)
						OUTPUTLOG_printf(LOG_LEVEL_Info, ",");
					if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
						OUTPUTLOG_printf(LOG_LEVEL_Info, " %5d", ARM_MOTORS[i].sensorRead());
					} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
						OUTPUTLOG_printf(LOG_LEVEL_Info, " %5d", AX12_get_position(ARM_MOTORS[i].id));
					}
					firstValue = FALSE;
				}

				OUTPUTLOG_printf(LOG_LEVEL_Info, "}, //%s\n", old_state >= 0 ? ARM_STATES_NAME[old_state] : "");
				break;
			}

			case ACT_ARM_PRINT_STATE_TRANSITIONS:
				print_state_transitions(FALSE);
				break;

			default:
				warn_printf("invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	} else if(msg->sid == ACT_DO_SELFTEST)
		SELFTEST_set_actions(&ARM_run_command, ARM_SELFTEST_NUMBER, arm_selftest_action);

	return FALSE;
}

void ARM_run_command(queue_id_t queueId, bool_e init) {
	static Sint16 old_DCM_post;

	bool_e done = TRUE, return_result = TRUE;
	Uint8 result = ACT_RESULT_DONE, error_code = ACT_RESULT_ERROR_OK;
	Uint16 line = 0;

	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Arm) {    // Gestion des mouvements du bras
		Sint16 new_state = QUEUE_get_arg(queueId)->param;

		if(init) {
			Uint8 canCommand = QUEUE_get_arg(queueId)->canCommand;

			if(canCommand == ACT_ARM_UPDOWN_GOTO){
				old_DCM_post = ARM_readDCMPos();
				if(!move_updown_to(new_state))
					QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
			}else if(canCommand == ACT_ARM_UPDOWN_RUSH_IN_FLOOR){
				old_DCM_post = ARM_readDCMPos();
				if(check_state_and_rush_in_floor(ARM_ACT_UPDOWN_ID, FALSE, &result, &error_code, &line, init, new_state))
					QUEUE_next(queueId, ACT_ARM, result, error_code, __LINE__);
			}else{
				if(old_state < 0 && canCommand != ACT_ARM_INIT) {
					warn_printf("Etat non initialisé, impossible d\'aller à l\'état %s(%d)\n",
								ARM_STATES_NAME[new_state], new_state);
					QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, new_state);
					return;
				}

				if(old_state == new_state){
					QUEUE_next(queueId, ACT_ARM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, new_state);
					return;
				}

				if(old_state >= 0 && arm_states_transitions[old_state][new_state] == 0){
					warn_printf("Déplacement impossible de l\'etat %s(%d) à %s(%d)\n",
								ARM_STATES_NAME[old_state], old_state,
								ARM_STATES_NAME[new_state], new_state);
					QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, new_state);
					return;
				}

				debug_printf("Going from state %s(%d) to %s(%d)\n",
							 (old_state >= 0) ? ARM_STATES_NAME[old_state] : "non-initialisé", old_state,
							 ARM_STATES_NAME[new_state], new_state);
				if(!gotoState(new_state))
					QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_UNKNOWN, new_state);
			}
		} else {//------------------------------------------------------------------------------------------------------------------------------------
			Uint8 canCommand = QUEUE_get_arg(queueId)->canCommand;
			Uint8 i;

			if(canCommand == ACT_ARM_UPDOWN_GOTO){
				done = ACTQ_check_status_dcmotor(ARM_ACT_UPDOWN_ID, FALSE, &result, &error_code, &line);
				if(done && result != ACT_RESULT_DONE){
					move_updown_to(old_DCM_post); // On remet le moteur  à la position antérieur
				}
				if(done)
					QUEUE_next(queueId, ACT_ARM, result, error_code, line);
			}else if(canCommand == ACT_ARM_UPDOWN_RUSH_IN_FLOOR){
				done = check_state_and_rush_in_floor(ARM_ACT_UPDOWN_ID, FALSE, &result, &error_code, &line, init, new_state);
				if(done)
					QUEUE_next(queueId, ACT_ARM, result, error_code, line);
			}else{
				for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
					if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
							done = ACTQ_check_status_dcmotor(ARM_MOTORS[i].id, FALSE, &result, &error_code, &line);
					}else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24){
						//------------------------------------------------------------------------------------------------------------
						if(new_state == ACT_ARM_POS_ON_TRIANGLE || new_state == ACT_ARM_POS_ON_TORCHE){
							if(ARM_MOTORS[i].id == ARM_ACT_RX24_ID)
								done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, angle_pos_triangle[data_arm_triangle.i_min_angle].value_rx24, ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 0, &result, &error_code, &line);
							else if(ARM_MOTORS[i].id == ARM_ACT_AX12_MID && data_arm_triangle.arm_way == RIGHT)
								done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, rayon_pos_triangle[data_arm_triangle.i_min_rayon].value_ax12_right, ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 0, &result, &error_code, &line);
							else if(ARM_MOTORS[i].id == ARM_ACT_AX12_MID && data_arm_triangle.arm_way == LEFT)
								done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, rayon_pos_triangle[data_arm_triangle.i_min_rayon].value_ax12_left, ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 0, &result, &error_code, &line);
						}else//------------------------------------------------------------------------------------------------------------
							done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, ARM_get_motor_pos(new_state, i), ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 0, &result, &error_code, &line);
					}

					//Si au moins un moteur n'a pas terminé son mouvement, alors l'action de déplacer le bras n'est pas terminée
					if(!done) {
						return_result = FALSE;
					}

					//Si au moins un moteur n'a pas pu correctement se déplacer, alors on a fail l'action et on retourne à la position précédente
					if(done && result != ACT_RESULT_DONE) {
						return_result = TRUE;
						if(ARM_MOTORS[i].type == ARM_DCMOTOR)
							debug_printf("Le moteur %d n'a pas atteint la position voulue\n", ARM_MOTORS[i].id);
						else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24)
							debug_printf("L'AX12/RX24 %d n'a pas atteint la position voulue\n",  ARM_MOTORS[i].id);
						if(old_state >= 0) {
							gotoState(old_state);
						}
						break;
					}
				}
			}

			if(return_result && (canCommand == ACT_ARM_GOTO || canCommand == ACT_ARM_INIT)) {
				if(result == ACT_RESULT_DONE){
					debug_printf("Mise à jours de l'état du bras : %s\n", ARM_STATES_NAME[new_state]);
					old_state = new_state;
				}
				QUEUE_next(queueId, ACT_ARM, result, error_code, new_state);
			}
		}
	} else {
		error_printf("Invalid act: %d\n", QUEUE_get_act(queueId));
		QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
	}
}

static bool_e check_state_and_rush_in_floor(Uint8 dcmotor_id, bool_e timeout_is_ok, Uint8* result, Uint8* error_code, Uint16* line, bool_e init, Sint16 order){
	static Uint32 begin_time, last_time;
	static Sint16 actual_pos;

	bool_e actualised = FALSE;
	bool_e done;

	if(init){
		DCM_setPwmWay(ARM_ACT_UPDOWN_ID, ARM_ACT_UPDOWN_RUSH_IN_FLOOR_PWM, ARM_ACT_UPDOWN_RUSH_IN_FLOOR_PWM);
		begin_time = CLOCK_get_time_10()*10;
		last_time = CLOCK_get_time_10()*10;
		actual_pos = ARM_readDCMPos() + INC_RUSH;
		actualised = TRUE;
	}

	if(CLOCK_get_time_10()*10 - last_time >= TIME_TO_INC_RUSH){
		last_time = CLOCK_get_time_10()*10;
		actual_pos = actual_pos + INC_RUSH;
		actualised = TRUE;

	}

	if(actualised){
		display(actual_pos);
		bool_e res = move_updown_to(conv_potar_updown_to_dist(actual_pos));
		if(!res && init){
			*result = ACT_RESULT_NOT_HANDLED;
			*error_code = ACT_RESULT_ERROR_INVALID_ARG;
			*line = __LINE__;
			DCM_stop(ARM_ACT_UPDOWN_ID);
			DCM_setPwmWay(ARM_ACT_UPDOWN_ID, ARM_ACT_UPDOWN_MAX_PWM_WAY0, ARM_ACT_UPDOWN_MAX_PWM_WAY1);
			return TRUE;
		}else if(!res){
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_INVALID_ARG;
			*line = __LINE__;
			DCM_stop(ARM_ACT_UPDOWN_ID);
			DCM_setPwmWay(ARM_ACT_UPDOWN_ID, ARM_ACT_UPDOWN_MAX_PWM_WAY0, ARM_ACT_UPDOWN_MAX_PWM_WAY1);
			return TRUE;
		}
	}

	if(CLOCK_get_time_10()*10 - begin_time >= TIME_RUSH_IN_FLOOR){
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_TIMEOUT;
		*line = __LINE__;
		DCM_stop(ARM_ACT_UPDOWN_ID);
		DCM_setPwmWay(ARM_ACT_UPDOWN_ID, ARM_ACT_UPDOWN_MAX_PWM_WAY0, ARM_ACT_UPDOWN_MAX_PWM_WAY1);
		return TRUE;
	}

	done = ACTQ_check_status_dcmotor(dcmotor_id, timeout_is_ok, result, error_code, line);

#ifdef I_AM_ROBOT_SMALL
	if(conv_dist_to_potar_updown(order) - ARM_readDCMPos() < -EPSILON_POS_RUSH_FLOOR){
#else
	if(conv_dist_to_potar_updown(order) - ARM_readDCMPos() > EPSILON_POS_RUSH_FLOOR){
#endif
		*result = ACT_RESULT_FAILED;
		*error_code = ACT_RESULT_ERROR_UNKNOWN;
		*line = __LINE__;
		DCM_stop(ARM_ACT_UPDOWN_ID);
		display(conv_potar_updown_to_dist(actual_pos));
		DCM_setPwmWay(ARM_ACT_UPDOWN_ID, ARM_ACT_UPDOWN_MAX_PWM_WAY0, ARM_ACT_UPDOWN_MAX_PWM_WAY1);
		return TRUE;
	}

	if(absolute(actual_pos - ARM_readDCMPos()) > DIFF_POS_FICT_RUSH){
		if(absolute(ARM_readDCMPos() - conv_dist_to_potar_updown(order)) > EPSILON_POS_RUSH_FLOOR){
			*result = ACT_RESULT_FAILED;
			*error_code = ACT_RESULT_ERROR_UNKNOWN;
			*line = __LINE__;
		}else{
			*result = ACT_RESULT_DONE;
			*error_code = ACT_RESULT_ERROR_OK;
			*line = __LINE__;
		}
		DCM_stop(ARM_ACT_UPDOWN_ID);
		display(conv_potar_updown_to_dist(actual_pos));
		DCM_setPwmWay(ARM_ACT_UPDOWN_ID, ARM_ACT_UPDOWN_MAX_PWM_WAY0, ARM_ACT_UPDOWN_MAX_PWM_WAY1);
		return TRUE;
	}

	return FALSE;
}

static bool_e gotoState(ARM_state_e state) {
	bool_e ok = TRUE;
	Uint8 i;

	if(state < 0 || state >= ARM_ST_NUMBER)
		return FALSE;

	if(state == ACT_ARM_POS_ON_TRIANGLE || state == ACT_ARM_POS_ON_TORCHE)
		return goto_triangle_pos();

	for(i = 0; ok && i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCM_setPosValue(ARM_MOTORS[i].id, 0, ARM_get_motor_pos(state, i));
			DCM_goToPos(ARM_MOTORS[i].id, 0);
			DCM_restart(ARM_MOTORS[i].id);
		} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			if(!AX12_set_position(ARM_MOTORS[i].id, ARM_get_motor_pos(state, i)))
				ok = FALSE;
		}
	}

	return ok;
}

//Vérifie que les transitions d'états sont reversibles
static bool_e check_state_transitions() {
	Uint8 i, j;
	bool_e ok = TRUE;

	for(i = 0; i < ARM_ST_NUMBER; i++) {
		for(j = 0; j < ARM_ST_NUMBER; j++) {
			if(arm_states_transitions[i][j] != arm_states_transitions[j][i]) {
				warn_printf("Déplacement non reversible: %s(%d) %s %s(%d)\n",
							ARM_STATES_NAME[i], i,
							arm_states_transitions[i][j] ? "=>" : "<=",
							ARM_STATES_NAME[j], j);
				ok = FALSE;
			}
		}
	}

	return ok;
}

static void print_state_transitions(bool_e correct) {
	Uint8 i, j;
	Uint8 columnSize = 0;
	bool_e isFirstVal;
	info_printf("Proposition de transitions d\'états corrigées:\n");

	OUTPUTLOG_printf(LOG_LEVEL_Info,
					 "const bool_e ARM_STATES_TRANSITIONS[ARM_ST_NUMBER][ARM_ST_NUMBER] = {  //\n"
					 "//   ");
	for(j = 0; j < ARM_ST_NUMBER; j++) {
		Uint8 current_column_size = strlen(ARM_STATES_NAME[j]);
		if(columnSize < current_column_size)
			columnSize = current_column_size;

	}
	for(j = 0; j < ARM_ST_NUMBER; j++) {
		OUTPUTLOG_printf(LOG_LEVEL_Info, "%-*s ", columnSize, ARM_STATES_NAME[j]);
	}
	OUTPUTLOG_printf(LOG_LEVEL_Info, "\n");
	for(i = 0; i < ARM_ST_NUMBER; i++) {
		OUTPUTLOG_printf(LOG_LEVEL_Info, "    {");
		for(isFirstVal = TRUE, j = 0; j < ARM_ST_NUMBER; j++) {
			bool_e val;

			if(correct)
				val = MAX(arm_states_transitions[i][j], arm_states_transitions[j][i]);  //avec correction
			else
				val = arm_states_transitions[i][j]; //sans correction

			OUTPUTLOG_printf(LOG_LEVEL_Info, "%s%-*d", isFirstVal ? "" : ",", columnSize, val);
			isFirstVal = FALSE;
		}
		OUTPUTLOG_printf(LOG_LEVEL_Info, "},  //%s\n", ARM_STATES_NAME[i]);
	}
	OUTPUTLOG_printf(LOG_LEVEL_Info, "};  //\n");
}

static Sint8 find_state() {
	Uint8 state, i;
	bool_e stateOk;

	for(state = 0; state < ARM_ST_NUMBER; state++) {
		stateOk = TRUE;

		for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
			if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
				if(absolute(ARM_MOTORS[i].sensorRead() - ARM_get_motor_pos(state, i)) >= ARM_MOTORS[i].large_epsilon) {
					stateOk = FALSE;
					break;
				}
			} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
				if(absolute(AX12_get_position(ARM_MOTORS[i].id) - ARM_get_motor_pos(state, i)) >= ARM_MOTORS[i].large_epsilon) {
					stateOk = FALSE;
					break;
				}
			}

		}

		if(stateOk == TRUE) {
			old_state = state;
			debug_printf("Etat initial auto détecté: %s(%d)\n",
						ARM_STATES_NAME[old_state], old_state);
			return old_state;
		}
	}


	debug_printf("Etat initial non détecté\n");
	return -1;
}

static bool_e goto_triangle_pos(){
	Uint8 i_min_rayon, i_min_angle, i;
	Sint16 min_rayon, min_angle;
	Sint16 x_bras, y_bras;
	Sint16 off_set_angle;

	// Calcul de la position de l'origine du bras
	#ifdef I_AM_ROBOT_BIG
		x_bras = sqrt(square(BRAS_POS_X_PIERRE) + square(BRAS_POS_Y_PIERRE)) * sin4096(PI4096/2 - (global.pos.angle + atan2(BRAS_POS_Y_PIERRE, BRAS_POS_X_PIERRE)*4096.));
		y_bras = sqrt(square(BRAS_POS_X_PIERRE) + square(BRAS_POS_Y_PIERRE)) * cos4096(PI4096/2 - (global.pos.angle + atan2(BRAS_POS_Y_PIERRE, BRAS_POS_X_PIERRE)*4096.));
	#else
		x_bras = sqrt(square(BRAS_POS_X_GUY) + square(BRAS_POS_Y_GUY)) * sin4096(PI4096/2 - (global.pos.angle + atan2(BRAS_POS_Y_GUY, BRAS_POS_X_GUY)*4096.));
		y_bras = sqrt(square(BRAS_POS_X_GUY) + square(BRAS_POS_Y_GUY)) * cos4096(PI4096/2 - (global.pos.angle + atan2(BRAS_POS_Y_GUY, BRAS_POS_X_GUY)*4096.));
	#endif

	// Calcul du rayon
	data_pos_triangle.rayon = dist_point_to_point(data_pos_triangle.x, data_pos_triangle.y,global.pos.x +  x_bras, global.pos.y + y_bras);

	// Gestion erreur rayon
	#ifdef I_AM_ROBOT_BIG
		if(data_pos_triangle.rayon < RAYON_MIN_PIERRE || data_pos_triangle.rayon > RAYON_MAX_PIERRE){
			debug_printf("Le bras ne peut pas aller chercher le triangle car rayon hors d'atteinte %d < %d < %d\n", RAYON_MIN_PIERRE, data_pos_triangle.rayon, RAYON_MAX_PIERRE);
			return FALSE;
		}
	#else
		if(data_pos_triangle.rayon < RAYON_MIN_GUY || data_pos_triangle.rayon > RAYON_MAX_GUY){
			debug_printf("Le bras ne peut pas aller chercher le triangle car rayon hors d'atteinte %d < %d < %d\n", RAYON_MIN_GUY, data_pos_triangle.rayon, RAYON_MAX_GUY);
			return FALSE;
		}
	#endif

	// Calcul de l'offset de l'angle du à l'ax12 du rayon
	#ifdef I_AM_ROBOT_BIG
		off_set_angle = acos((square(LONGUEUR_BRAS_PIERRE) + square(data_pos_triangle.rayon) - square(LONGUEUR_AVANT_BRAS_PIERRE))/(float)(2*LONGUEUR_BRAS_PIERRE*data_pos_triangle.rayon))*4096.;
	#else
		off_set_angle = acos((square(LONGUEUR_BRAS_GUY) + square(data_pos_triangle.rayon) - square(LONGUEUR_AVANT_BRAS_GUY))/(float)(2*LONGUEUR_BRAS_GUY*data_pos_triangle.rayon))*4096.;
	#endif

	// Calcul de l'angle
		//data_pos_triangle.angle = PI4096/2 - absolute(global.pos.angle) + (Sint32)(acos((data_pos_triangle.y - global.pos.y)/(float)data_pos_triangle.rayon)*4096.);
	//data_pos_triangle.angle = PI4096/2 - ((Sint32)(acos((data_pos_triangle.y - global.pos.y)/(float)data_pos_triangle.rayon)*4096.) + global.pos.angle);
	data_pos_triangle.angle = (Sint32)(SIGNE(data_pos_triangle.y - global.pos.y)*acos(PROTECTION_SIN_COS((data_pos_triangle.x - global.pos.x)/(float)data_pos_triangle.rayon))*4096.) - global.pos.angle;

	//------------------------------------------------------------------------------------------------------------------------ Avant bras à droite du bras

	// Calcul de l'angle réel pour l'rx24
	data_pos_triangle.real_angle = modulo_angle(data_pos_triangle.angle + off_set_angle);

	// Choix du rayon le plus proche du rayon voulu
	i_min_rayon = 0;
	min_rayon = rayon_pos_triangle[0].rayon;
	for(i=1;i<taille_rayon_pos_triangle;i++){
		if(absolute(min_rayon - data_pos_triangle.rayon) > absolute(rayon_pos_triangle[i].rayon - data_pos_triangle.rayon)
				&& rayon_pos_triangle[i].value_ax12_right != -1){
			i_min_rayon = i;
			min_rayon = rayon_pos_triangle[i].rayon;
		}
	}

	// Choix de l'angle le plus proche de l'angle voulu
	i_min_angle = 0;
	min_angle = angle_pos_triangle[0].angle;
	for(i=1;i<taille_angle_pos_triangle;i++){
		if(absolute(min_angle - data_pos_triangle.real_angle) > absolute(angle_pos_triangle[i].angle - data_pos_triangle.real_angle)
				&& min_rayon >= angle_pos_triangle[i].rayon_min_right){
			i_min_angle = i;
			min_angle = angle_pos_triangle[i].angle;
		}
	}

	// Check si le rayon et l'angle trouvé est suffisant pour la prise ou est impossible
	if(DISTANCE_MAX_TO_TAKE < (int)sqrt(square(data_pos_triangle.rayon)+square(min_rayon) - 2.*min_rayon*data_pos_triangle.rayon*cos4096(modulo_angle(data_pos_triangle.angle - (min_angle-off_set_angle))))
			|| angle_pos_triangle[i_min_angle].value_rx24 == -1
			|| rayon_pos_triangle[i_min_rayon].value_ax12_right == -1){

		//----------------------------------------------------------------------------- Nous ne pouvons pas aller chercher le triangle avec cette configuration...
		//------------------------------------------------------------------------------------------------------------------------ Avant bras à gauche du bras

		// Calcul de l'angle réel pour l'rx24
		data_pos_triangle.real_angle = modulo_angle(data_pos_triangle.angle - off_set_angle);

		// Choix du rayon le plus proche du rayon voulu
		i_min_rayon = 0;
		min_rayon = rayon_pos_triangle[0].rayon;
		for(i=1;i<taille_rayon_pos_triangle;i++){
			if(absolute(min_rayon - data_pos_triangle.rayon) > absolute(rayon_pos_triangle[i].rayon - data_pos_triangle.rayon)
					&& rayon_pos_triangle[i].value_ax12_left != -1){
				i_min_rayon = i;
				min_rayon = rayon_pos_triangle[i].rayon;
			}
		}

		// Choix de l'angle le plus proche de l'angle voulu
		i_min_angle = 0;
		min_angle = angle_pos_triangle[0].angle;
		for(i=1;i<taille_angle_pos_triangle;i++){
			if(absolute(min_angle - data_pos_triangle.real_angle) > absolute(angle_pos_triangle[i].angle - data_pos_triangle.real_angle)
					&& min_rayon >= angle_pos_triangle[i].rayon_min_left){
				i_min_angle = i;
				min_angle = angle_pos_triangle[i].angle;
			}
		}

		if(DISTANCE_MAX_TO_TAKE < (int)sqrt(square(data_pos_triangle.rayon)+square(min_rayon) - 2.*min_rayon*data_pos_triangle.rayon*cos4096(modulo_angle((data_pos_triangle.angle - (min_angle+off_set_angle)))))
				|| angle_pos_triangle[i_min_angle].value_rx24 == -1
				|| rayon_pos_triangle[i_min_rayon].value_ax12_left == -1){
			debug_printf("Le bras ne peut pas aller chercher le triangle écart entre triangle et position trouvé > à %d\n", DISTANCE_MAX_TO_TAKE);
			return FALSE;
		}else
			data_arm_triangle.arm_way = LEFT;
	}else
		data_arm_triangle.arm_way = RIGHT;


	// Placement du bras dans les états voulus
	if(!AX12_set_position(ARM_ACT_RX24_ID, angle_pos_triangle[i_min_angle].value_rx24)){
		debug_printf("Placement du bras (servo RX24) impossible position %d\n", angle_pos_triangle[i_min_angle].value_rx24);
		return FALSE;
	}
	if(data_arm_triangle.arm_way == RIGHT){
		if(!AX12_set_position(ARM_ACT_AX12_MID_ID, rayon_pos_triangle[i_min_rayon].value_ax12_right)){
			debug_printf("Placement du bras (servo AX12 MID) impossible position %d\n", rayon_pos_triangle[i_min_rayon].value_ax12_right);
			return FALSE;
		}
	}else{
		if(!AX12_set_position(ARM_ACT_AX12_MID_ID, rayon_pos_triangle[i_min_rayon].value_ax12_left)){
			debug_printf("Placement du bras (servo AX12 MID) impossible position %d\n", rayon_pos_triangle[i_min_rayon].value_ax12_left);
			return FALSE;
		}
	}
	data_arm_triangle.i_min_angle = i_min_angle;
	data_arm_triangle.i_min_rayon = i_min_rayon;

	display(angle_pos_triangle[i_min_angle].value_rx24);
	if(data_arm_triangle.arm_way == RIGHT)
		display(rayon_pos_triangle[i_min_rayon].value_ax12_right);
	else
		display(rayon_pos_triangle[i_min_rayon].value_ax12_left);


	return TRUE;

}

static void get_data_pos_triangle(CAN_msg_t* msg){
	data_pos_triangle.x = U16FROMU8(msg->data[3], msg->data[4]);
	data_pos_triangle.y = U16FROMU8(msg->data[5], msg->data[6]);
	display(data_pos_triangle.x);
	display(data_pos_triangle.y);
}

static Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2){
	return sqrt((Sint32)(y1 - y2)*(y1 - y2) + (Sint32)(x1 - x2)*(x1 - x2));
}

static bool_e move_updown_to(Sint16 pos){
	Sint16 value = conv_dist_to_potar_updown(pos);

	display(pos);

	if(value < ARM_ACT_UPDOWN_MIN_VALUE || value > ARM_ACT_UPDOWN_MAX_VALUE)
		return FALSE;

	DCM_setPosValue(ARM_ACT_UPDOWN_ID, 0, value);
	DCM_goToPos(ARM_ACT_UPDOWN_ID, 0);
	DCM_restart(ARM_ACT_UPDOWN_ID);
	return TRUE;
}

static bool_e find_state_path(Sint8 begin_state, Sint8 end_state){
	Sint8 i;
	Sint8 open_liste[ARM_ST_NUMBER], close_liste[ARM_ST_NUMBER];
	Sint8 path[ARM_ST_NUMBER];
	Sint8 open_liste_i = -1;

	debug_printf("Recherche d'un chemin du bras pour aller de %s à %s\n", ARM_STATES_NAME[begin_state], ARM_STATES_NAME[end_state]);

	if(begin_state == -1)
		return FALSE;

	// Initialisation des tableaux
	for(i=0;i<ARM_ST_NUMBER;i++){
		open_liste[i] = -1;
		close_liste[i] = -1;
		path[i] = -1;
		switch_state[i] = -1;
	}
	// Ajout de l'état de début dans l'open liste
	open_liste_i = ajout_liste(open_liste, ARM_ST_NUMBER, begin_state);

	// Tant que l'open liste n'est pas vide et que l'état final n'est pas dans l'open liste
	while(open_liste_i != -1 && !est_dans_liste(open_liste, ARM_ST_NUMBER, end_state)){
		for(i=0;i<ARM_ST_NUMBER;i++){
			// S'il y a un chemin possible entre ces deux états, et que l'état n'est pas déjà dans l'open liste ni dans la close liste
			if(arm_states_transitions[open_liste[0]][i] == 1 && !est_dans_liste(open_liste, ARM_ST_NUMBER, i)
															 && !est_dans_liste(close_liste, ARM_ST_NUMBER, i)){

				// On note se chemin dans le path
				path[i] = open_liste[0];

				// On ajout l'état du chemin dans l'open liste
				open_liste_i = ajout_liste(open_liste, ARM_ST_NUMBER, i);
			}
		}
		// Une fois tout les chemins de cette état testé on met cet état dans la close liste et on l'enlève de l'open liste
		ajout_liste(close_liste, ARM_ST_NUMBER, open_liste[0]);
		open_liste_i = del_liste(open_liste, ARM_ST_NUMBER, open_liste[0]);
	}

	// Si l'état final n'est pas dans l'open liste cela veut dire qu'on n'a pas trouvé de chemin
	if(!est_dans_liste(open_liste, ARM_ST_NUMBER, end_state))
		return FALSE;

	Sint8 st = end_state;
	// On calcule la taille du chemin
	taille_path = 0;
	while(st != begin_state){
		st = path[st];
		taille_path++;
	}

	// On stocke le chemin trouvé dans switch_state
	st = end_state;
	for(i=taille_path-1;i>=0;i--){
		switch_state[i] = st;
		st = path[st];
	}

	return TRUE;
}

static bool_e est_dans_liste(Sint8 liste[], Uint8 taille, Sint8 search){
	Uint8 i;
	for(i=0;i<taille;i++){
		if(liste[i] == search)
			return TRUE;
	}
	return FALSE;
}

static Sint8 ajout_liste(Sint8 liste[], Uint8 taille, Sint8 val){
	Uint8 i;
	for(i=0;i<taille;i++){
		if(liste[i] == -1){
			liste[i] = val;
			return i;
		}
	}
	return taille-1;
}

static Sint8 del_liste(Sint8 liste[], Uint8 taille, Sint8 val){
	Uint8 i, j;
	for(i=0;i<taille;i++){
		if(liste[i] == val){
			for(j=i;j<taille-1;j++)
				liste[j] = liste[j+1];
			return i;
		}
	}
	return taille-1;
}

static Sint16 modulo_angle(Sint32 angle){
	while(angle > PI4096)
		angle -= 2*PI4096;

	while(angle < -PI4096)
		angle += 2*PI4096;

	return angle;
}
