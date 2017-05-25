/*
 *	Club Robot ESEO 2015 - 2016
 *	Black & Pearl
 *
 *	Fichier : actions_both_2016.c
 *	Package : Carte S²/strats_2016
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Arnaud
 *	Version 2016
 */


#include "actions_both_2017.h"
#include "actions_both_generic.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_CapteurCouleurCW.h"
#include "../propulsion/prop_functions.h"
#include "../propulsion/movement.h"
#include "../propulsion/astar.h"
#include "../utils/generic_functions.h"
#include "../actuator/queue.h"
#include "../actuator/act_functions.h"
#include "../utils/actionChecker.h"

#define BIG_COLOR_LEFT_CH0_PORT 	GPIOC
#define BIG_COLOR_LEFT_CH0_PIN	 	GPIO_Pin_6
#define BIG_COLOR_RIGHT_CH0_PORT 	GPIOC
#define BIG_COLOR_RIGHT_CH0_PIN	 	GPIO_Pin_7

#define BIG_COLOR_LEFT_CH1_PORT 	GPIOC
#define BIG_COLOR_LEFT_CH1_PIN	 	GPIO_Pin_10
#define BIG_COLOR_RIGHT_CH1_PORT 	GPIOC
#define BIG_COLOR_RIGHT_CH1_PIN	 	GPIO_Pin_11

#define BIG_COLOR_LEFT_CH2_PORT 	GPIOD
#define BIG_COLOR_LEFT_CH2_PIN	 	GPIO_Pin_2
#define BIG_COLOR_RIGHT_CH2_PORT 	GPIOD
#define BIG_COLOR_RIGHT_CH2_PIN	 	GPIO_Pin_3


#define SMALL_COLOR_MAGIC_ARM_CH0_PORT 	GPIOC
#define SMALL_COLOR_MAGIC_ARM_CH0_PIN	GPIO_Pin_6

#define SMALL_COLOR_MAGIC_ARM_CH1_PORT 	GPIOC
#define SMALL_COLOR_MAGIC_ARM_CH1_PIN	GPIO_Pin_10

#define SMALL_COLOR_MAGIC_ARM_CH2_PORT 	GPIOD
#define SMALL_COLOR_MAGIC_ARM_CH2_PIN	GPIO_Pin_2

#define SMALL_COLOR_MAGIC_ARM_CH3_PORT 	GPIOD
#define SMALL_COLOR_MAGIC_ARM_CH3_PIN	GPIO_Pin_5

void ColorSensor_init(){
	CW_config_t config_left, config_right, config_magic_arm;
	Uint8 i;

	// Init the driver (set all ports and pins as unused)
	CW_init();

	// Init all configuration to default values (very important to avoid hardfault)
	for(i = 0; i < CW_PP_MAXPORTNUM; i++){
		config_left.digital_ports[i].port = CW_UNUSED_PORT;
		config_left.digital_ports[i].pin = 0;
		config_left.digital_ports[i].is_inverted_logic = 0;

		config_right.digital_ports[i].port = CW_UNUSED_PORT;
		config_right.digital_ports[i].pin = 0;
		config_right.digital_ports[i].is_inverted_logic = 0;

		config_magic_arm.digital_ports[i].port = CW_UNUSED_PORT;
		config_magic_arm.digital_ports[i].pin = 0;
		config_magic_arm.digital_ports[i].is_inverted_logic = 0;
	}

	config_left.analog_X = 0;
	config_left.analog_Y = 0;
	config_left.analog_Z = 0;

	config_right.analog_X = 0;
	config_right.analog_Y = 0;
	config_right.analog_Z = 0;

	config_magic_arm.analog_X = 0;
	config_magic_arm.analog_Y = 0;
	config_magic_arm.analog_Z = 0;

	// Init channels
	if(I_AM_BIG()){
		// Left side
		config_left.digital_ports[CW_PP_Channel0].port = BIG_COLOR_LEFT_CH0_PORT;
		config_left.digital_ports[CW_PP_Channel0].pin = BIG_COLOR_LEFT_CH0_PIN;
		config_left.digital_ports[CW_PP_Channel1].port = BIG_COLOR_LEFT_CH1_PORT;
		config_left.digital_ports[CW_PP_Channel1].pin = BIG_COLOR_LEFT_CH1_PIN;
		config_left.digital_ports[CW_PP_Channel2].port = BIG_COLOR_LEFT_CH2_PORT;
		config_left.digital_ports[CW_PP_Channel2].pin = BIG_COLOR_LEFT_CH2_PIN;

		// Right side
		config_right.digital_ports[CW_PP_Channel0].port = BIG_COLOR_RIGHT_CH0_PORT;
		config_right.digital_ports[CW_PP_Channel0].pin = BIG_COLOR_RIGHT_CH0_PIN;
		config_right.digital_ports[CW_PP_Channel1].port = BIG_COLOR_RIGHT_CH1_PORT;
		config_right.digital_ports[CW_PP_Channel1].pin = BIG_COLOR_RIGHT_CH1_PIN;
		config_right.digital_ports[CW_PP_Channel2].port = BIG_COLOR_RIGHT_CH2_PORT;
		config_right.digital_ports[CW_PP_Channel2].pin = BIG_COLOR_RIGHT_CH2_PIN;

		CW_config_sensor(CW_SENSOR_LEFT, &config_left);
		CW_config_sensor(CW_SENSOR_RIGHT, &config_right);
	}else{
		// Magic arm
		config_magic_arm.digital_ports[CW_PP_Channel0].port = SMALL_COLOR_MAGIC_ARM_CH0_PORT;
		config_magic_arm.digital_ports[CW_PP_Channel0].pin = SMALL_COLOR_MAGIC_ARM_CH0_PIN;
		config_magic_arm.digital_ports[CW_PP_Channel1].port = SMALL_COLOR_MAGIC_ARM_CH1_PORT;
		config_magic_arm.digital_ports[CW_PP_Channel1].pin = SMALL_COLOR_MAGIC_ARM_CH1_PIN;
		config_magic_arm.digital_ports[CW_PP_Channel2].port = SMALL_COLOR_MAGIC_ARM_CH2_PORT;
		config_magic_arm.digital_ports[CW_PP_Channel2].pin = SMALL_COLOR_MAGIC_ARM_CH2_PIN;

		CW_config_sensor(CW_SENSOR_SMALL, &config_magic_arm);
	}
}

error_e sub_cross_rocker(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_CROSS_ROCKER,
			INIT,
			PREPARE_TO_PASS,
			CROSS,
			CROSS_END,
			MOVE_BACK,
			CORRECT_ODOMETRY_MATH,
			CORRECT_ODOMETRY_MEASURE,
			AVANCE_TO_NEXT_POSITION,
			RECALAGE_X,
			GET_OUT,
			GO_BACK,
			ERROR,
			DONE
		);

	switch (state) {
		case INIT:
			if(i_am_in_square_color(0, 350, 0, 400)){
				state = CROSS;
			}else{
				state = ERROR;
			}
			break;
/*
		case PREPARE_TO_PASS:
			state = try_advance(NULL, entrance, 90, state, CROSS, ERROR, FAST, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			if(ON_LEAVE()){
				ACT_push_order(ACT_QUEUE_Small_bearing_back, ACT_SMALL_BALL_BACK_UP);
				ACT_push_order(ACT_QUEUE_Small_bearing_front_left, ACT_SMALL_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_QUEUE_Small_bearing_front_right, ACT_SMALL_BALL_FRONT_RIGHT_UP);
			}
			break;
*/
		case CROSS:
			if(entrance){
			//	PROP_WARNER_arm_y(COLOR_Y(530));
				PROP_set_threshold_error_translation(1214400, FALSE);
				ACT_push_order(ACT_SMALL_BALL_BACK, ACT_SMALL_BALL_BACK_UP);
				ACT_push_order(ACT_SMALL_BALL_FRONT_LEFT, ACT_SMALL_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_SMALL_BALL_FRONT_RIGHT, ACT_SMALL_BALL_FRONT_RIGHT_UP);
			}
			state = try_going(180, COLOR_Y(600), state, CROSS_END, MOVE_BACK, 22, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			//if(global.prop.reach_y){
//					ACT_push_order(ACT_SMALL_BALL_FRONT_LEFT,ACT_SMALL_BALL_FRONT_LEFT_DOWN);
	//				ACT_push_order(ACT_SMALL_BALL_FRONT_RIGHT,ACT_SMALL_BALL_FRONT_RIGHT_DOWN);
			//}
			break;

		case CROSS_END:
			state = try_going(180, COLOR_Y(850), state, CORRECT_ODOMETRY_MATH, ERROR, 16, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case MOVE_BACK:
			state = try_going(180, COLOR_Y(200), state, CROSS, CROSS, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case CORRECT_ODOMETRY_MATH:{
			if(I_AM_SMALL()){
				PROP_set_threshold_error_translation(614400,TRUE);
				ACT_push_order(ACT_SMALL_BALL_BACK, ACT_SMALL_BALL_BACK_DOWN);
				ACT_push_order(ACT_SMALL_BALL_FRONT_LEFT, ACT_SMALL_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_SMALL_BALL_FRONT_RIGHT, ACT_SMALL_BALL_FRONT_RIGHT_DOWN);
			}else{
				ACT_push_order(ACT_BIG_BALL_BACK_LEFT, ACT_BIG_BALL_BACK_LEFT_DOWN);
				ACT_push_order(ACT_BIG_BALL_BACK_RIGHT, ACT_BIG_BALL_BACK_RIGHT_DOWN);
				ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			}
			Sint16 diffY = COLOR_EXP(-10, 10);
#warning 'c\'est plus que bancal cette histoire'
			PROP_set_position(global.pos.x, global.pos.y + diffY, global.pos.angle);

			state = CORRECT_ODOMETRY_MEASURE;
			}break;

		case CORRECT_ODOMETRY_MEASURE:{
			Sint16 diffy = 0;

			// COCO ON T'ATTEND !
			//J'arrive avec mes gros sabots et mes méthodes dégueulasses !
			state = check_sub_action_result(action_recalage_y(FORWARD, COLOR_ANGLE(-PI4096/2), COLOR_Y(710+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE),COLOR_ANGLE(-PI4096/2), FALSE, &diffy, TRUE, TRUE), state, AVANCE_TO_NEXT_POSITION, AVANCE_TO_NEXT_POSITION);
		}break;

		case AVANCE_TO_NEXT_POSITION:
			state = try_advance(NULL, entrance, 100, state, RECALAGE_X, CORRECT_ODOMETRY_MEASURE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case RECALAGE_X:{
			Sint16 diffx = 0;

			state = check_sub_action_result(action_recalage_x(FORWARD, -PI4096, SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE, -PI4096, FALSE, &diffx, TRUE, TRUE), state, GET_OUT, CORRECT_ODOMETRY_MEASURE);
		}break;

		case GET_OUT:
			state = try_advance(NULL, entrance, 300, state, DONE, RECALAGE_X, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);

			//state = try_going(250, COLOR_Y(1000), state, DONE, GO_BACK, SLOW, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GO_BACK:
			state = try_going(175, COLOR_Y(900), state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;

		default:
			if(entrance)
			{
				error_printf("default case in sub_cross_rocker\n");
			}
			state = ERROR;
			break;
	}

	return IN_PROGRESS;
}


error_e sub_wait_1_sec(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_WAIT_ONE_SEC,
							INIT,
							DONE,
							ERROR
							);


	switch (state) {
		case INIT:
			if(entrance){
				on_turning_point();
			}
			state = wait_time(1000, INIT, DONE);
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_wait_1_sec\n");
			}
			break;
	}
	return IN_PROGRESS;
}

#define DEFAULT_PROTECT_ZONE		ZONE_TO_PROTECT_OUR_FULL_SIDE

//Cette fonction de protection se rend à un point à protéger et rend la main au bout d'une seconde...
//Elle peut être appelée en boucle si on a rien d'autre à faire... ^^
error_e sub_protect(zone_to_protect_e zone)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PROTECT_ZONE,
								INIT,
								GET_IN_ASTAR,
								WAIT_1SEC,
								DONE,
								ERROR
								);
	error_e ret;
	ret = IN_PROGRESS;
	static GEOMETRY_point_t p;

	switch (state) {
			case INIT:
				if(zone == ZONE_TO_PROTECT_SMART_CHOICE)
					zone = DEFAULT_PROTECT_ZONE;	//On fait confiance au codeur qui a placé son espoir dans le define.

				switch(zone)
				{
					case ZONE_TO_PROTECT_ADV_CENTER_MOONBASE:	p = (GEOMETRY_point_t){1300,COLOR_Y(2300)};			break;
					case ZONE_TO_PROTECT_ADV_SIDE_MOONBASE:		p = (GEOMETRY_point_t){900,COLOR_Y(2700)};			break;
					case ZONE_TO_PROTECT_MIDDLE_MOONBASE:		p = (GEOMETRY_point_t){950,1500};					break;
					case ZONE_TO_PROTECT_OUR_CENTER_MOONBASE:	p = (GEOMETRY_point_t){1300,COLOR_Y(700)};			break;
					case ZONE_TO_PROTECT_OUR_SIDE_MOONBASE:		p = (GEOMETRY_point_t){900,COLOR_Y(300)};			break;
					case ZONE_TO_PROTECT_OUR_FULL_SIDE:					//no break
					case ZONE_TO_PROTECT_SMART_CHOICE:					//no break
					default:									p = (GEOMETRY_point_t){800,COLOR_Y(1300)};			break;
				}
				if(i_am_in_square(p.x-50, p.x+50, p.y-50, p.y+50))
					state = WAIT_1SEC;	//Nous sommes déjà sur le point à protéger (à 50mm près)
				else
					state = GET_IN_ASTAR;
				error_printf("Protection : i will try to protect %d ; %d\n", p.x, p.y);
				break;
			case GET_IN_ASTAR:
				state = ASTAR_try_going(p.x, p.y, state, WAIT_1SEC, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case WAIT_1SEC:
				state = wait_time(1000, state, DONE);
				break;
			case DONE:
				RESET_MAE();
				on_turning_point();
				ret = END_OK;
				break;

			case ERROR:
				RESET_MAE();
				on_turning_point();
				ret = NOT_HANDLED;
				break;

			default:
				if(entrance)
				{
					error_printf("default case in sub_wait_1_sec\n");
				}
				break;
		}

		return ret;
}

// Subaction de test de l'évitement (A ne pas jouer en match)
error_e strat_test_avoidance(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_TEST_AVOIDANCE,
			INIT,
			ACTION,
			GO_POINT_1,
			GO_POINT_2,
			ERROR,
			DONE
		);

	switch(state){

		case INIT:
			state = try_going(global.pos.x + 700, global.pos.y, state, GO_POINT_2, GO_POINT_2, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_POINT_1:
			state = try_going(900, COLOR_Y(500), state, GO_POINT_2, GO_POINT_2, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_POINT_2:
			state = try_going(900, COLOR_Y(1800), state, GO_POINT_1, GO_POINT_1, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		default:
			if(entrance)
			{
				error_printf("default case in strat_test_avoidance\n");
			}
			break;
	}
	return IN_PROGRESS;
}

//Code permettant de calculer le deplacement du robot pendant la prise
void compute_take_point_rocket(GEOMETRY_point_t *take_point, Sint16 *take_angle, GEOMETRY_point_t store_point, Sint16 angle_robot, Uint16 dist){
	Sint16 angle = GEOMETRY_modulo_angle(angle_robot);
	Sint16 angle_theorical =  angle;
	GEOMETRY_point_t p = store_point;
	if(angle > -PI4096/4 && angle <= PI4096/4){	// approximativement angle == 0
		p.x = store_point.x + dist;
		angle_theorical = 0;
	}else if(angle > PI4096/4 && angle <= 3*PI4096/4){ // approximativement angle == PI4096/2
		p.y = store_point.y + dist;
		angle_theorical = PI4096/2;
	}else if(angle > 3*PI4096/4 || angle <= -3*PI4096/4){ // approximativement angle == PI4096
		p.x = store_point.x - dist;
		angle_theorical = PI4096;
	}else if(angle > -3*PI4096/4 && angle <= -PI4096/4){ // approximativement angle == -PI4096/2
		p.y = store_point.y - dist;
		angle_theorical = -PI4096/2;
	}else{
		error_printf("ERROR : couldn't compute point for rocket\n");
	}

	if(take_point != NULL){
		*take_point = p;
	}

	if(take_angle != NULL){
		*take_angle = angle_theorical;
	}
}

