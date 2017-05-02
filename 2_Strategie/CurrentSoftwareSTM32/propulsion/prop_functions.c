/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : prop_functions.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'asser
 *	Auteur : Jacen
 *	Version 20110313
 */

#include "prop_functions.h"
#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_maths.h"
#include "movement.h"

#define LOG_PREFIX "prop_fun: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_PROP_FUNCTION
#include "../QS/QS_outputlog.h"

// Timeout en ms
#define GOTO_TIMEOUT_TIME							7000	// On n'attend que 3 secondes sur les PROP_push_goto cette année car on ne fait pas de gros déplacements sur le terrain
#define STOP_TIMEOUT_TIME							2000
#define GOTO_MULTI_POINT_TIMEOUT_TIME				4000	//Nombre de secondes de timeout PAR POINT en mode multipoint.
#define RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME	3000
#define RUSH_TIMEOUT_TIME							25000
#define GOANGLE_TIMEOUT_TIME						6000

#define WAIT_ADD_TIMEOUT_TIME						3000

//Valeur par défault pour la variable distance_to_reach
#define DEFAULT_DISTANCE_TO_REACH 100

/* Variable pour la fonction PROP_goto_until_distance_reached
 * Il s'agit de la distance entre le robot et le point de destination avant le changement de point.
 * Cette variable peut aussi être vue comme le rayon du cercle de centre le point de destination. Lorsque le robot entre dans ce cercle,
 * on lui envoie le nouveau point de destination
 */
static Uint16 distance_to_reach = DEFAULT_DISTANCE_TO_REACH;
static Uint8 idTrajActual = 0;

// ---------------------------------------------------------------------------- Fonctions de déplacement

void PROP_stop(){
	CAN_send_sid(PROP_STOP);
}

void PROP_goTo(Sint16 x, Sint16 y, prop_referential_e referential, PROP_speed_e speed, way_e way, bool_e multipoint, propEndCondition_e propEndCondition, prop_curve_e curve, avoidance_type_e avoidance, prop_border_mode_e border_mode, Uint8 idTraj){
	CAN_msg_t order;
	order.sid = PROP_GO_POSITION;
	order.size = SIZE_PROP_GO_POSITION;
	order.data.prop_go_position.buffer_mode = (multipoint)?PROP_END_OF_BUFFER:PROP_NOW;
	order.data.prop_go_position.propEndCondition = propEndCondition;
	order.data.prop_go_position.border_mode = border_mode;
	order.data.prop_go_position.referential = referential;
	order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
	order.data.prop_go_position.x = x;
	order.data.prop_go_position.y = y;
	order.data.prop_go_position.speed = speed;
	order.data.prop_go_position.way = way;
	order.data.prop_go_position.curve = curve;
	order.data.prop_go_position.idTraj = idTraj;

	if(avoidance == NO_DODGE_AND_WAIT || avoidance == DODGE_AND_WAIT)
		order.data.prop_go_position.avoidance = AVOID_ENABLED_AND_WAIT;
	else if(avoidance == NO_DODGE_AND_NO_WAIT || avoidance == DODGE_AND_NO_WAIT)
		order.data.prop_go_position.avoidance = AVOID_ENABLED;
	else
		order.data.prop_go_position.avoidance = AVOID_DISABLED;

	CAN_send(&order);
}

void PROP_goAngle(Sint16 angle, prop_referential_e referential, PROP_speed_e speed, way_e way, bool_e multipoint, propEndCondition_e propEndCondition, Uint8 idTraj){
	CAN_msg_t order;
	order.sid = PROP_GO_ANGLE;
	order.size = SIZE_PROP_GO_ANGLE;
	order.data.prop_go_angle.buffer_mode = (multipoint)?PROP_END_OF_BUFFER:PROP_NOW;
	order.data.prop_go_angle.propEndCondition = propEndCondition;
	order.data.prop_go_angle.referential = referential;
	order.data.prop_go_angle.acknowledge = PROP_ACKNOWLEDGE;
	order.data.prop_go_angle.way = way;
	order.data.prop_go_angle.speed = speed;
	order.data.prop_go_angle.teta = angle;
	order.data.prop_go_angle.idTraj = idTraj;
	CAN_send(&order);
}

void PROP_rushInTheWall(way_e way, bool_e prop_rotate,Sint16 angle){
	CAN_msg_t order;
	order.sid = PROP_RUSH_IN_THE_WALL;
	order.size = SIZE_PROP_RUSH_IN_THE_WALL;
	order.data.prop_rush_in_the_wall.teta = angle;
	order.data.prop_rush_in_the_wall.way = way;
	order.data.prop_rush_in_the_wall.asser_rot = prop_rotate;
	CAN_send (&order);
}


error_e wait_move_and_wait_detection(trajectory_e trajectory_type, Uint8 nb_trajectory, Uint8 idLastTraj, STRAT_endCondition_e end_condition, time32_t begin_time){

	time32_t timeout_traj;
	error_e ret = IN_PROGRESS;

	// todo temps d'évitement

	switch(trajectory_type){

		case TRAJECTORY_AUTOMATIC_CURVE:
		case TRAJECTORY_TRANSLATION:

			if(nb_trajectory > 1)
				timeout_traj = (GOTO_MULTI_POINT_TIMEOUT_TIME) * nb_trajectory;
			else
				timeout_traj = GOTO_TIMEOUT_TIME;

			if(global.prop.idTrajActual == idLastTraj
					&& (global.prop.ended
						|| (global.prop.brake && end_condition == END_AT_BRAKE)
						)
					){
				ret = END_OK;

			}else if(global.absolute_time - begin_time >= timeout_traj)
				ret = END_WITH_TIMEOUT;
			else if(global.prop.error)
				ret = NOT_HANDLED;

			if(global.prop.detected_foe){
				ret = FOE_IN_PATH;
			}
			break;

		case TRAJECTORY_ROTATION:

			if(global.prop.idTrajActual == idLastTraj
					&& (global.prop.ended
						|| (global.prop.brake && end_condition == END_AT_BRAKE)
						)
					){
				ret = END_OK;

			}else if(global.absolute_time - begin_time >= GOANGLE_TIMEOUT_TIME)
				ret = END_WITH_TIMEOUT;
			else if(global.prop.error)
				ret = NOT_HANDLED;
			break;

		case TRAJECTORY_STOP:
			if(global.prop.ended)
				ret = END_OK;
			else if(global.absolute_time - begin_time >= STOP_TIMEOUT_TIME)
				ret = END_WITH_TIMEOUT;
			break;

		case TRAJECTORY_NONE:
		case WAIT_FOREVER:
			ret = NOT_HANDLED;
			break;
	}

	return ret;
}

// ---------------------------------------------------------------------------- Fonctions de warner

void PROP_WARNER_arm_x(Sint16 x){
	CAN_msg_t msg;
	msg.sid = PROP_WARN_X;
	msg.size = SIZE_PROP_WARN_X;
	msg.data.prop_warn_x.x = x;
	CAN_send(&msg);
}

void PROP_WARNER_arm_y(Sint16 y){
	CAN_msg_t msg;
	msg.sid = PROP_WARN_Y;
	msg.size = SIZE_PROP_WARN_Y;
	msg.data.prop_warn_y.y = y;
	CAN_send(&msg);
}

void PROP_WARNER_arm_teta(Sint16 teta){
	CAN_msg_t msg;
	msg.sid = PROP_WARN_ANGLE;
	msg.size = SIZE_PROP_WARN_ANGLE;
	msg.data.prop_warn_angle.angle = teta;
	CAN_send(&msg);
}

void PROP_WARNER_arm_distance(Uint16 distance, Sint16 x, Sint16 y){
	CAN_msg_t msg;
	msg.sid = PROP_WARN_DISTANCE;
	msg.size = SIZE_PROP_WARN_DISTANCE;
	msg.data.prop_warn_distance.distance = distance;
	msg.data.prop_warn_distance.x = x;
	msg.data.prop_warn_distance.y = y;
	CAN_send(&msg);
	debug_printf("Warner send at pos x=%d  y=%d  distance=%d\n", x, y, distance);
}

// ---------------------------------------------------------------------------- Fonctions autres

Uint8 PROP_getNextIdTraj(){
	if(idTrajActual >= 63){
		idTrajActual = 1;
	}else{
		idTrajActual++;
	}
	return idTrajActual;
}

Uint8 PROP_custom_speed_convertor(Uint16 speed){ // en mm/s

	float factor = 0.16; // (4096*5)/(128*1000) conversion mm/s -> custom
	Uint16 ret = (Uint16)((float)(speed)*factor + 8);

	if(ret > 255)
		ret = 255;

	return ret;
}

void PROP_debug_move_position(Sint16 x, Sint16 y, Sint16 teta){
	CAN_msg_t msg;
	msg.sid = DEBUG_PROP_MOVE_POSITION;
	msg.size = SIZE_DEBUG_PROP_MOVE_POSITION;
	msg.data.debug_prop_move_position.xOffset = x;
	msg.data.debug_prop_move_position.yOffset = y;
	msg.data.debug_prop_move_position.tetaOffset = teta;
	msg.data.prop_warn_angle.angle = teta;
	CAN_send(&msg);
}

void PROP_set_threshold_error_translation(Uint32 value, bool_e reset){
	CAN_msg_t msg;
	msg.sid = DEBUG_SET_ERROR_TRESHOLD_TRANSLATION;
	msg.size = SIZE_DEBUG_SET_ERROR_TRESHOLD_TRANSLATION;
	if(reset)
		msg.data.debug_set_error_treshold_translation.error_treshold_trans = 0;
	else
		msg.data.debug_set_error_treshold_translation.error_treshold_trans = value;
	CAN_send(&msg);
}

void PROP_set_position(Sint16 x, Sint16 y, Sint16 teta){
	CAN_msg_t msg;
	msg.sid = PROP_SET_POSITION;
	msg.size = SIZE_PROP_SET_POSITION;
	msg.data.prop_set_position.x = x;
	msg.data.prop_set_position.y = y;
	msg.data.prop_set_position.teta = teta;
	CAN_send(&msg);
}

//Acceleration en mm/4096/5ms/5ms..
void PROP_set_acceleration(Uint32 acceleration){
	CAN_msg_t msg;
	msg.sid = DEBUG_PROPULSION_SET_ACCELERATION;
	msg.size = SIZE_DEBUG_PROPULSION_SET_ACCELERATION;
	msg.data.debug_propulsion_set_acceleration.acceleration_coef = acceleration;
	CAN_send(&msg);
}

void PROP_ask_propulsion_coefs(void){
	CAN_send_sid(DEBUG_PROPULSION_GET_COEFS);
}

void PROP_set_correctors(bool_e corrector_rotation, bool_e corrector_translation){
	CAN_msg_t msg;
	msg.sid = PROP_SET_CORRECTORS;
	msg.size = SIZE_PROP_SET_CORRECTORS;
	msg.data.prop_set_correctors.rot_corrector = corrector_rotation;
	msg.data.prop_set_correctors.trans_corrector = corrector_translation;
	CAN_send(&msg);
}

/* Accesseur en écriture de la variable globale distance_to_reach
 * pour la focntion PROP_goto_until_distance_reached
 * pour le try_going avec END_AT_DISTANCE pour la end_condition
 */
void PROP_set_distance_to_reach(Uint16 dist){
	distance_to_reach = dist;
}

/* Accesseur en lecture de la variable globale distance_to_reach
 * pour la focntion PROP_goto_until_distance_reached
 * pour le try_going avec END_AT_DISTANCE pour la end_condition
 */
Uint16 PROP_get_distance_to_reach(void){
	return distance_to_reach;
}

void PROP_set_detected_foe(CAN_msg_t *msg){

	if(msg->sid == STRAT_PROP_FOE_DETECTED){
		if(msg->data.strat_prop_foe_detected.in_wait == FALSE){
			global.prop.detected_foe = TRUE;
		}
	}
}



void PROP_set_coef(PROPULSION_coef_e coef, Sint32 value){
	CAN_msg_t msg;
	msg.sid = DEBUG_PROPULSION_SET_COEF;
	msg.size = SIZE_DEBUG_PROPULSION_SET_COEF;
	msg.data.debug_propulsion_set_coef.id = coef;
	msg.data.debug_propulsion_set_coef.value = value;
	CAN_send(&msg);
}

void PROP_reset_coef(PROPULSION_coef_e coef){
	CAN_msg_t msg;
	msg.sid = DEBUG_PROPULSION_RESET_COEF;
	msg.size = SIZE_DEBUG_PROPULSION_RESET_COEF;
	msg.data.debug_propulsion_reset_coef.id = coef;
	CAN_send(&msg);
}
