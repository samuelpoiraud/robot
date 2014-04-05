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

#if 1

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
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
#include <string.h>
#include <math.h>

#include "config_debug.h"
#define LOG_PREFIX "Arm: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ARM
#include "../QS/QS_outputlog.h"

#define LONGUEUR_AVANT_BRAS_PIERRE		138		//(138)
#define	LONGUEUR_BRAS_PIERRE			126		//(125,5)
#define LONGUEUR_AVANT_BRAS_GUY			84		//(83,6)
#define	LONGUEUR_BRAS_GUY				126		//(125,5)

#define BRAS_POS_X_PIERRE				50
#define BRAS_POS_Y_PIERRE				50
#define BRAS_POS_X_GUY					4.07
#define BRAS_POS_Y_GUY					4.07

#define DISTANCE_MAX_TO_TAKE			20

#define square(x) (x*x)

typedef struct{
	Sint16 x;
	Sint16 y;
	Sint16 z;
	Uint16 rayon;
	Sint16 angle;
	Sint16 real_angle;
}data_pos_triangle_s;

typedef struct{
	Uint16 rayon;
	Sint16 value_ax12;
}rayon_pos_triangle_s;

typedef struct{
	Sint16 angle;
	Sint16 value_rx24;
	Uint16 rayon_min;
}angle_pos_triangle_s;

typedef struct{
	Uint8 i_min_angle;
	Uint8 i_min_rayon;
}data_arm_triangle_s;

#define DELTA_ANGLE			(PI4096/8)
#define RAYON_MIN			10
#define RAYON_MAX			208

static const rayon_pos_triangle_s rayon_pos_triangle[] = {
	{10,		45},
	{20,		45},
	{30,		45},
	{40,		45},
	{50,		45},
	{60,		45},
	{70,		45},
	{80,		45},
	{90,		45},
	{100,		45},
	{110,		45},
	{120,		45}
};

static const Uint8 taille_rayon_pos_triangle = sizeof(rayon_pos_triangle)/sizeof(rayon_pos_triangle_s);

static const angle_pos_triangle_s angle_pos_triangle[] = {
	{PI4096 + DELTA_ANGLE*1,		45,			30},
	{PI4096 + DELTA_ANGLE*2,		45,			30},
	{PI4096 + DELTA_ANGLE*3,		45,			30},
	{PI4096 + DELTA_ANGLE*4,		45,			30},
	{PI4096 + DELTA_ANGLE*5,		45,			30},
	{PI4096 + DELTA_ANGLE*6,		45,			30},
	{PI4096 + DELTA_ANGLE*7,		45,			30},
	{PI4096 + DELTA_ANGLE*8,		45,			30},
	{PI4096 + DELTA_ANGLE*9,		45,			30},
	{PI4096 + DELTA_ANGLE*10,		45,			30},
	{PI4096 + DELTA_ANGLE*11,		45,			30},
	{PI4096 + DELTA_ANGLE*12,		45,			30}
};

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
static bool_e find_state();
static void get_data_pos_triangle(CAN_msg_t* msg);
static bool_e goto_triangle_pos();
static void get_data_pos_triangle(CAN_msg_t* msg);
static Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);


static Sint8 old_state = -1;

void ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	AX12_init();
	ADC_init();

	if(check_state_transitions() == FALSE)
		print_state_transitions(TRUE);

	Uint8 i;
	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCMotor_config_t dcconfig;

			dcconfig.sensor_read = ARM_MOTORS[i].sensorRead;
			dcconfig.Kp = ARM_MOTORS[i].kp;
			dcconfig.Ki = ARM_MOTORS[i].ki;
			dcconfig.Kd = ARM_MOTORS[i].kd;
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
			} else if(ARM_ax12_is_initialized[i] == FALSE) {
				// Au moins un RX24/AX12 non prêt => pas allOk, on affiche pas le message d'init
				allOk = FALSE;
				debug_printf("AX12 %d not here\n", ARM_MOTORS[i].id);
			}
		}
	}

	if(allOk) {
		info_printf("AX12 et RX24 du bras initialisés\n");
		allInitialized = TRUE;
		find_state();
	}
}

void ARM_initPos() {
	CAN_msg_t msg = {ACT_ARM, {ACT_ARM_INIT}, 1};

	if(find_state() == FALSE)
		ARM_CAN_process_msg(&msg);
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
	if(msg->sid == ACT_ARM) {
		//Initialise les RX24/AX12 du bras s'ils n'étaient pas alimentés lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		ARM_initAX12();

		switch(msg->data[0]) {
			case ACT_ARM_GOTO:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[1]);
				if(msg->data[1] == ACT_ARM_POS_ON_TRIANGLE)
					get_data_pos_triangle(msg);
				break;

			case ACT_ARM_STOP:
				ARM_stop();
				break;

			case ACT_ARM_INIT: {
				queue_id_t queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					Uint8 i;
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);

					for(i = 0; i < ARM_INIT_NUMBER; i++) {
						QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], ARM_INIT[i], &ACTQ_finish_SendNothing}, QUEUE_ACT_Arm);
					}

					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
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
	} else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_action_t tests[ARM_INIT_NUMBER];
		Uint8 i;

		for(i = 0; i < ARM_INIT_NUMBER; i++) {
			tests[i].canCommand = ACT_ARM_INIT;
			tests[i].optionnal_act = QUEUE_ACT_Arm;
			tests[i].param = ARM_INIT[i];
		}

		SELFTEST_set_actions(&ARM_run_command, ARM_INIT_NUMBER, tests);
	}

	return FALSE;
}

void ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Arm) {    // Gestion des mouvements de rotation de l'assiette
		Sint16 new_state = QUEUE_get_arg(queueId)->param;

		if(init) {
			Uint8 canCommand = QUEUE_get_arg(queueId)->canCommand;

			if(old_state < 0 && canCommand != ACT_ARM_INIT) {
				warn_printf("Etat non initialisé, impossible d\'aller à l\'état %s(%d)\n",
							ARM_STATES_NAME[new_state], new_state);
				QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
				return;
			}

			if(old_state >= 0 && ARM_STATES_TRANSITIONS[old_state][new_state] == 0) {
				//déplacement impossible, le bras doit passer par d'autre positions avant d'atteindre la position demandée
				warn_printf("Déplacement impossible de l\'etat %s(%d) à %s(%d)\n",
							ARM_STATES_NAME[old_state], old_state,
							ARM_STATES_NAME[new_state], new_state);
				QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
				return;
			}

			debug_printf("Going from state %s(%d) to %s(%d)\n",
						 (old_state >= 0) ? ARM_STATES_NAME[old_state] : "non-initialisé", old_state,
						 ARM_STATES_NAME[new_state], new_state);
			gotoState(new_state);

		} else {
			bool_e done = TRUE, return_result = TRUE;
			Uint8 result = ACT_RESULT_DONE, error_code = ACT_RESULT_ERROR_OK;
			Uint16 line = 0;
			Uint8 i;

			for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
				if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
					done = ACTQ_check_status_dcmotor(ARM_MOTORS[i].id, FALSE, &result, &error_code, &line);
				} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
					done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, ARM_get_motor_pos(new_state, i), ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 0, &result, &error_code, &line);
				}

				//Si au moins un moteur n'a pas terminé son mouvement, alors l'action de déplacer le bras n'est pas terminée
				if(!done) {
					return_result = FALSE;
				}

				//Si au moins un moteur n'a pas pu correctement se déplacer, alors on a fail l'action et on retourne à la position précédente
				if(done && result != ACT_RESULT_DONE) {
					return_result = TRUE;
					if(old_state >= 0) {
						gotoState(old_state);
					}
					break;
				}
			}

			if(return_result) {
				if(result == ACT_RESULT_DONE)
					old_state = new_state;
				QUEUE_next(queueId, ACT_ARM, result, error_code, line);
			}
		}
	} else {
		error_printf("Invalid act: %d\n", QUEUE_get_act(queueId));
		QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
	}
}

static bool_e gotoState(ARM_state_e state) {
	bool_e ok = TRUE;
	Uint8 i;

	if(state < 0 || state >= ARM_ST_NUMBER)
		return FALSE;

	if(state == ACT_ARM_POS_ON_TRIANGLE)
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
			if(ARM_STATES_TRANSITIONS[i][j] != ARM_STATES_TRANSITIONS[j][i]) {
				warn_printf("Déplacement non reversible: %s(%d) %s %s(%d)\n",
							ARM_STATES_NAME[i], i,
							ARM_STATES_TRANSITIONS[i][j] ? "=>" : "<=",
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
				val = MAX(ARM_STATES_TRANSITIONS[i][j], ARM_STATES_TRANSITIONS[j][i]);  //avec correction
			else
				val = ARM_STATES_TRANSITIONS[i][j]; //sans correction

			OUTPUTLOG_printf(LOG_LEVEL_Info, "%s%-*d", isFirstVal ? "" : ",", columnSize, val);
			isFirstVal = FALSE;
		}
		OUTPUTLOG_printf(LOG_LEVEL_Info, "},  //%s\n", ARM_STATES_NAME[i]);
	}
	OUTPUTLOG_printf(LOG_LEVEL_Info, "};  //\n");
}

static bool_e find_state() {
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
			return TRUE;
		}
	}


	debug_printf("Etat initial non détecté\n");
	return FALSE;
}

static bool_e goto_triangle_pos(){
	Uint8 i_min_rayon, min_rayon, i_min_angle, min_angle;
	Uint8 i;
	Sint16 x_bras, y_bras;
	Sint16 off_set_angle;

	// Calcul de la position de l'origine du bras
	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		x_bras = sqrt(square(BRAS_POS_X_PIERRE) + square(BRAS_POS_Y_PIERRE)) * sin(PI4096 - (global.pos.angle + atan2(BRAS_POS_Y_PIERRE, BRAS_POS_X_PIERRE)));
		y_bras = sqrt(square(BRAS_POS_X_PIERRE) + square(BRAS_POS_Y_PIERRE)) * cos(PI4096 - (global.pos.angle + atan2(BRAS_POS_Y_PIERRE, BRAS_POS_X_PIERRE)));
	}else{
		x_bras = sqrt(square(BRAS_POS_X_GUY) + square(BRAS_POS_Y_GUY)) * sin(PI4096 - (global.pos.angle + atan2(BRAS_POS_Y_GUY, BRAS_POS_X_GUY)));
		y_bras = sqrt(square(BRAS_POS_X_GUY) + square(BRAS_POS_Y_GUY)) * cos(PI4096 - (global.pos.angle + atan2(BRAS_POS_Y_GUY, BRAS_POS_X_GUY)));
	}

	// Calcul du rayon
	data_pos_triangle.rayon = dist_point_to_point(data_pos_triangle.x, data_pos_triangle.y,global.pos.x +  x_bras, global.pos.y + y_bras);

	// Gestion erreur rayon
	if(data_pos_triangle.rayon < RAYON_MIN || data_pos_triangle.rayon > RAYON_MAX){
		debug_printf("Le bras ne peut pas aller chercher le triangle car rayon hors d'atteinte %d < %d < %d\n", RAYON_MIN, data_pos_triangle.rayon, RAYON_MAX);
		return FALSE;
	}

	// Calcul de l'offset de l'angle du à l'ax12 du rayon
	if(QS_WHO_AM_I_get() == BIG_ROBOT)
		off_set_angle = acos((square(LONGUEUR_BRAS_PIERRE) + square(data_pos_triangle.rayon) - square(LONGUEUR_AVANT_BRAS_PIERRE))/(2*LONGUEUR_BRAS_PIERRE*LONGUEUR_AVANT_BRAS_PIERRE));
	else
		off_set_angle = acos((square(LONGUEUR_BRAS_PIERRE) + square(data_pos_triangle.rayon) - square(LONGUEUR_AVANT_BRAS_PIERRE))/(2*LONGUEUR_BRAS_PIERRE*LONGUEUR_AVANT_BRAS_PIERRE));

	// Calcul de l'angle
	data_pos_triangle.angle = cos((data_pos_triangle.x - x_bras)/data_pos_triangle.rayon);

	// Calcul de l'angle réel pour l'rx24
	data_pos_triangle.real_angle = data_pos_triangle.angle - off_set_angle;

	// Choix du rayon le plus proche du rayon voulu
	i_min_rayon = 0;
	min_rayon = rayon_pos_triangle[0].rayon;
	for(i=1;i<taille_rayon_pos_triangle;i++){
		if(absolute(min_rayon - data_pos_triangle.rayon) < absolute(rayon_pos_triangle[i].rayon - data_pos_triangle.rayon)){
			i_min_rayon = i;
			min_rayon = rayon_pos_triangle[i].rayon;
		}
	}

	// Choix de l'angle le plus proche de l'angle voulu
	i_min_angle = 0;
	min_angle = angle_pos_triangle[0].angle;
	for(i=1;i<taille_angle_pos_triangle;i++){
		if(absolute(min_angle - data_pos_triangle.real_angle) < absolute(angle_pos_triangle[i].angle - data_pos_triangle.real_angle)
				&& min_rayon >= angle_pos_triangle[i].rayon_min){
			i_min_angle = i;
			min_angle = angle_pos_triangle[i].angle;
		}
	}

	// Check si le rayon et l'angle trouvé est suffisant pour la prise ou est impossible
	if(DISTANCE_MAX_TO_TAKE > square(data_pos_triangle.rayon)+square(min_rayon+off_set_angle) - 2*min_rayon*data_pos_triangle.rayon*cos(min_angle+off_set_angle - data_pos_triangle.angle)){
		debug_printf("Le bras ne peut pas aller chercher le triangle écart entre triangle et position trouvé > à %d\n", DISTANCE_MAX_TO_TAKE);
		return FALSE;
	}

	// Placement du bras dans les états voulus
	if(!AX12_set_position(ARM_ACT_RX24, angle_pos_triangle[i_min_angle].value_rx24)){
		debug_printf("Placement du bras (servo RX24) impossible\n");
		return FALSE;
	}
	if(!AX12_set_position(ARM_ACT_AX12_MID, rayon_pos_triangle[i_min_rayon].value_ax12)){
		debug_printf("Placement du bras (servo AX12 MID) impossible\n");
		return FALSE;
	}

	data_arm_triangle.i_min_angle = i_min_angle;
	data_arm_triangle.i_min_rayon = i_min_rayon;

	return TRUE;

}

static Uint8 check_pos_triangle(){

}

static void get_data_pos_triangle(CAN_msg_t* msg){
	assert(msg->data[1] == ACT_ARM_POS_ON_TRIANGLE);
	data_pos_triangle.x = (((Sint16)(msg->data[2]) << 8) & 0xFF00) | ((Sint16)(msg->data[3]) & 0x00FF);
	data_pos_triangle.y = (((Sint16)(msg->data[4]) << 8) & 0xFF00) | ((Sint16)(msg->data[5]) & 0x00FF);
	data_pos_triangle.z = (((Sint16)(msg->data[6]) << 8) & 0xFF00) | ((Sint16)(msg->data[7]) & 0x00FF);
}

static Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2){
	return sqrt((Sint32)(y1 - y2)*(y1 - y2) + (Sint32)(x1 - x2)*(x1 - x2));
}

#endif
