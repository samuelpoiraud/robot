/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : elements.c
 *	Package : Carte Principale
 *	Description : Traitement des informations sur les éléments de jeu
 *	Auteur : Ronan, Adrien
 *	Version 20110430
 */
#include "elements.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_CANmsgList.h"
#include "act_functions.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_adc.h"
#include "config/config_pin.h"
#include "state_machine_helper.h"
#include "Pathfind.h"
#include "config/config_qs.h"
#include <math.h>
#include "QS/QS_maths.h"

#define LABIUM_TIMEOUT			500
#define verin_order_TIMEOUT		200
#define PUMP_TIMEOUT			100

#define conv_value_to_dist_laser_torch(x)	(Sint16)((x)*0.2615-66.868)

// Pour torche
#define SMALL_FORWARD_WIDTH 83
#define RADIUS_TORCH 80
#define DIM_TRIANGLE 100

#define FRESQUE_2_ADC_VALUE		2048
#define FRESQUE_3_ADC_VALUE		1024
#define FRESQUE_ADC_EPSILON		100

static fruit_verin_state_e fruit_verin_state = UNKNOWN;
static time32_t time_fruit_verin_state = 0;


static enum{
	PUMP_ANSWER_ANYTHING, PUMP_ANSWER_NO, PUMP_ANSWER_YES
}pump_answer = PUMP_ANSWER_ANYTHING;

static GEOMETRY_point_t posTorch[2] = {{1100,900},		// Torche Rouge
									   {1100,2100}};	// Torche Jaune

typedef struct{
	GEOMETRY_point_t drop;
	GEOMETRY_point_t getIn;
	GEOMETRY_point_t getOut;

	//Rectangle d'acceptation
	Uint16 xMin;
	Uint16 xMax;
	Uint16 yMin;
	Uint16 yMax;
}pos_drop_t;

pos_drop_t pos_drop[] = {{{400,200},{600,300},{600,300},500,1600,400,1200},			// RED_CAVERN
						 {{400,2800},{600,2700},{600,2700},500,1600,1800,2600},		// YELLOW_CAVERN
						 {{1300,150},{1300,400},{1300,400},500,1600,400,1200},		// RED_TREE_MAMOU
						 {{1850,700},{1600,700},{1600,700},500,1600,400,1200},		// RED_TREE
						 {{1300,2850},{1300,2600},{1300,2600},500,1600,1800,2600},	// YELLOW_TREE_MAMOU
						 {{1850,2300},{1600,2300},{1600,2300},500,1600,1800,2600}	// YELLOW_TREE
						};

//------------------------------------
// Fonction de lancement / subaction
//------------------------------------


error_e ELEMENT_go_and_drop(pos_drop_e choice){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DO_TRIANGLE_START_ADVERSARY,
		IDLE,
		GET_IN,
		POS_START,
		POS_DROP,
		DROP,
		GET_OUT,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static pathfind_node_id_t pathfind;
	static GEOMETRY_point_t point;

	switch(state){
		case IDLE:

			if(is_in_square(pos_drop[choice].xMin, pos_drop[choice].xMax, pos_drop[choice].yMin, pos_drop[choice].yMax, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = POS_START;
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			if(entrance)
				pathfind = PATHFIND_closestNode(pos_drop[choice].getIn.x, pos_drop[choice].getIn.y, 0);

			state = PATHFIND_try_going(pathfind,GET_IN, POS_START, ERROR, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_START:
			state = try_going(pos_drop[choice].getIn.x,pos_drop[choice].getIn.y, POS_START, POS_DROP, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POS_DROP:{
			if(entrance){ // On veut déposer le triangle à pos_drop.drop et non placé le robot là
				Uint16 norm = GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){pos_drop[choice].drop.x,pos_drop[choice].drop.y});

				float coefx = (global.env.pos.x - pos_drop[choice].drop.x)/(norm*1.);
				float coefy = (global.env.pos.y - pos_drop[choice].drop.y)/(norm*1.);


				point.x = pos_drop[choice].drop.x + DIM_TRIANGLE*coefx;
				point.y = pos_drop[choice].drop.y + DIM_TRIANGLE*coefy;
			}

			state = try_going(point.x,point.y, POS_DROP, DROP, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}break;

		case DROP:
			// Depose le triangle ici
			state = GET_OUT;
			break;

		case GET_OUT: // Ne dipose que d'un point de sorti car se trouve trop prêt du bord ou bien pas de sorti possible autrement pour les cavernes
			state = try_going(pos_drop[choice].getOut.x,pos_drop[choice].getOut.x, GET_OUT, (last_state == DROP)? DONE:RETURN_NOT_HANDLED, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			if(	is_in_square(500, 1650, 400, 1200, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) || // Est sur le pathfind
				is_in_square(500, 1650, 1800, 2600, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) ||
				is_in_square(500, 700, 400, 2600, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) ||
				is_in_square(1400, 1650, 400, 2600, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))

				state = RETURN_NOT_HANDLED;
			else
				state = GET_OUT;

			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

void TORCH_new_position(torch_choice_e choice){
	GEOMETRY_point_t torch;

	Sint16 cos,sin;
	COS_SIN_4096_get(global.env.pos.angle, &cos, &sin);

	torch.x = (Sint32)cos*(SMALL_FORWARD_WIDTH+RADIUS_TORCH - 23)/4096 + global.env.pos.x; // -23 creux du robot
	torch.y = (Sint32)sin*(SMALL_FORWARD_WIDTH+RADIUS_TORCH - 23)/4096 + global.env.pos.y;

	if((global.env.color == RED && choice == OUR_TORCH) || (global.env.color != RED && choice == ADVERSARY_TORCH))
		posTorch[0] = torch;
	else
		posTorch[1] = torch;

	display(posTorch[0].x);
	display(posTorch[0].y);
	display(posTorch[1].x);
	display(posTorch[1].y);

	TORCH_XBee_send_msg(choice, torch);
}

GEOMETRY_point_t TORCH_get_position(torch_choice_e choice){
	if((global.env.color == RED && choice == OUR_TORCH) || (global.env.color != RED && choice == ADVERSARY_TORCH))
		return posTorch[0];

	return posTorch[1];
}

void TORCH_CAN_process_msg(CAN_msg_t *msg){
	if(msg->sid != XBEE_TORCH_NEW_POS)
		return;

	if((global.env.color == RED && msg->data[0] == OUR_TORCH) || (global.env.color != RED && msg->data[0] == ADVERSARY_TORCH)){
		posTorch[0].x = U16FROMU8(msg->data[1],msg->data[2]);
		posTorch[0].y = U16FROMU8(msg->data[3],msg->data[4]);
	}else{
		posTorch[1].x = U16FROMU8(msg->data[1],msg->data[2]);
		posTorch[1].y = U16FROMU8(msg->data[3],msg->data[4]);
	}

}

void TORCH_XBee_send_msg(torch_choice_e choice, GEOMETRY_point_t pos){
	CAN_msg_t msg;

	msg.sid = XBEE_TORCH_NEW_POS;
	msg.data[0] = choice;
	msg.data[1] = HIGHINT(pos.x);
	msg.data[2] = LOWINT(pos.x);
	msg.data[3] = HIGHINT(pos.y);
	msg.data[4] = LOWINT(pos.y);
	msg.size = 5;

	CANMsgToXbee(&msg,FALSE);
}

Uint8 ELEMENT_wait_pump_capture_object(Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	typedef enum
	{
		ASK,
		WAIT,
		DONE
	}state_e;
	static state_e state = ASK;
	static time32_t timeLaunch;
	CAN_msg_t msg;
	switch(state){
		case ASK:
			pump_answer = PUMP_ANSWER_ANYTHING;
			msg.sid = ACT_ASK_POMPE_IN_PRESSURE;
			msg.size = 0;
			CAN_send(&msg);
			timeLaunch = global.env.match_time;
			state = WAIT;
			break;

		case WAIT:
			if(pump_answer != PUMP_ANSWER_ANYTHING || (global.env.match_time - timeLaunch > PUMP_TIMEOUT))
				state = DONE;
			break;

		case DONE:
			state = ASK;
			if(pump_answer == PUMP_ANSWER_ANYTHING || pump_answer == PUMP_ANSWER_NO)
				return fail_state;
			else
				return success_state;
			break;
	}
	return in_progress;
}

Uint8 ELEMENT_do_and_wait_end_fruit_verin_order(fruit_verin_state_e verin_order, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	typedef enum
	{
		IDLE,
		WAIT,
		END_OK,
		ERROR
	}state_e;
	static state_e state = IDLE;
	static time32_t timeLaunch = 0;
	switch(state){

		case IDLE :
			if(verin_order == FRUIT_VERIN_OPEN)
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);
			else if(verin_order == FRUIT_VERIN_CLOSE)
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			timeLaunch = global.env.match_time;
			if(global.env.match_time - time_fruit_verin_state >= verin_order_TIMEOUT)
				fruit_verin_state = UNKNOWN;
			state = WAIT;
			break;

		case WAIT:
			if(verin_order == FRUIT_VERIN_OPEN && fruit_verin_state == FRUIT_VERIN_OPEN)
				state = END_OK;
			else if(verin_order == FRUIT_VERIN_CLOSE && fruit_verin_state == FRUIT_VERIN_CLOSE)
				state = END_OK;
			else if(global.env.match_time - timeLaunch >= LABIUM_TIMEOUT)
				state = ERROR;
			break;

		case END_OK:
			state = IDLE;
			return success_state;

		case ERROR:
			state = IDLE;
			fruit_verin_state = UNKNOWN;
			return fail_state;
	}
	return in_progress;
}

//------------------------------------
// Fonction utilisateur
//------------------------------------

//------------------------------------
// Fonction de reception message CAN
//------------------------------------


void ELEMENT_update_fruit_verin_state(CAN_msg_t* msg){
	time_fruit_verin_state = global.env.match_time;
	if(msg->data[0] == STRAT_INFORM_FRUIT_MOUTH_OPEN)
		fruit_verin_state = OPEN;
	else if(msg->data[0] == STRAT_INFORM_FRUIT_MOUTH_CLOSE)
		fruit_verin_state = CLOSE;
	else
		fruit_verin_state = UNKNOWN;
}

void ELEMENT_answer_pump(CAN_msg_t *msg){
	pump_answer = (msg->data[0] == STRAT_ANSWER_POMPE_NO) ? PUMP_ANSWER_NO : PUMP_ANSWER_YES;
}

Sint16 get_dist_torch_laser(){
	return conv_value_to_dist_laser_torch(ADC_getValue(ADC_14));
}


//------------------------------------
// Fonction interne
//------------------------------------
