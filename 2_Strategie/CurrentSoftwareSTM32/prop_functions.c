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

#define PROP_FUNCTIONS_C

#include "prop_functions.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_maths.h"
#include "avoidance.h"

#define LOG_PREFIX "prop_fun: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_PROP_FUNCTION
#include "QS/QS_outputlog.h"

// Timeout en ms
#define GOTO_TIMEOUT_TIME							5000	// On n'attend que 3 secondes sur les PROP_push_goto cette année
															// car on ne fait pas de gros déplacements sur le terrain
#define STOP_TIMEOUT_TIME							2000

#define GOTO_MULTI_POINT_TIMEOUT_TIME				4000	//Nombre de secondes de timeout PAR POINT en mode multipoint.
#define RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME	3000

#define RUSH_TIMEOUT_TIME							25000
// suffisant pour un demi tour en vitesse lente
#define GOANGLE_TIMEOUT_TIME						6000

#define WAIT_ADD_TIMEOUT_TIME						3000

// Coefficient multiplicatif de la distance pour obtenir le temps avant le timeout
#define COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST			4
#define COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW			8

//Valeur par défault pour la variable distance_to_reach
#define DEFAULT_DISTANCE_TO_REACH 100

/*	Pile conservant les eventuels arguments pour les fonctions des actionneurs
 *	tout en conservant le meme prototype pour tous les actionneurs, reduisant
 *	le code de gestion des actionneurs à chaque boucle
 */
static prop_arg_t prop_args[STACKS_SIZE];

static void PROP_goto (stack_id_e stack_id, bool_e init);
static void PROP_goto_until_break (stack_id_e stack_id, bool_e init);
static void PROP_goto_until_distance_reached (stack_id_e stack_id, bool_e init);
static void PROP_relative_goto (stack_id_e stack_id, bool_e init);
static void PROP_goto_multi_point (stack_id_e stack_id, bool_e init);
static void PROP_goto_multi_point_until_break(stack_id_e stack_id, bool_e init);
static void PROP_relative_goangle_multi_point (stack_id_e stack_id, bool_e init);
static void PROP_goangle (stack_id_e stack_id, bool_e init);
static void PROP_relative_goangle (stack_id_e stack_id, bool_e init);
static void PROP_rush_in_the_wall(stack_id_e stack_id, bool_e init);
static void PROP_stop(stack_id_e stack_id, bool_e init);

/* Variable pour la fonction PROP_goto_until_distance_reached
 * Il s'agit de la distance entre le robot et le point de destination avant le changement de point.
 * Cette variable peut aussi être vue comme le rayon du cercle de centre le point de destination. Lorsque le robot entre dans ce cercle,
 * on lui envoie le nouveau point de destination
 */
static Uint16 distance_to_reach = DEFAULT_DISTANCE_TO_REACH;

// ---------------------------------------------------------------------------- Fonctions de déplacement

void PROP_push_stop ()
{
	STACKS_push (PROP, &PROP_stop, TRUE);
}

/* ajoute une instruction goto sur la pile asser */
void PROP_push_goto (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, avoidance_type_e avoidance, PROP_end_condition_e end_condition, prop_border_mode_e border_mode, bool_e run)
{
	debug_printf("\nD=%d\n",x);
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];
	debug_printf("\nE=%d\n",x);
	pos->x = x;
	pos->y = y;
	pos->speed = speed;
	pos->way = way;
	pos->curve = curve;
	pos->border_mode = border_mode;
	if(avoidance == NORMAL_WAIT || avoidance == NO_DODGE_AND_WAIT || avoidance == DODGE_AND_WAIT)
		pos->avoidance = AVOID_ENABLED_AND_WAIT;
	else if(avoidance == NO_DODGE_AND_NO_WAIT || avoidance == DODGE_AND_NO_WAIT)
		pos->avoidance = AVOID_ENABLED;
	else
		pos->avoidance = AVOID_DISABLED;
	if(end_condition == END_AT_LAST_POINT)
		STACKS_push (PROP, &PROP_goto, run);
	else if(end_condition == END_AT_BREAK)
		STACKS_push (PROP, &PROP_goto_until_break, run);
	else //(end_condition == END_AT_DISTANCE)
		STACKS_push (PROP, &PROP_goto_until_distance_reached, run);
}

void PROP_push_goto_multi_point (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, avoidance_type_e avoidance, Uint8 priority_order, PROP_end_condition_e end_condition, prop_border_mode_e border_mode, bool_e run)
{
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];

	pos->x = x;
	pos->y = y;
	pos->speed = speed;
	pos->way = way;
	pos->curve = curve;
	pos->priority_order = priority_order;
	pos->border_mode = border_mode;
	if(avoidance == NORMAL_WAIT || avoidance == NO_DODGE_AND_WAIT || avoidance == DODGE_AND_WAIT)
		pos->avoidance = AVOID_ENABLED_AND_WAIT;
	else if(avoidance == NO_DODGE_AND_NO_WAIT || avoidance == DODGE_AND_NO_WAIT)
		pos->avoidance = AVOID_ENABLED;
	else
		pos->avoidance = AVOID_DISABLED;
	if(end_condition == END_AT_LAST_POINT)
		STACKS_push (PROP, &PROP_goto_multi_point, run);
	else
		STACKS_push (PROP, &PROP_goto_multi_point_until_break, run);
}

void PROP_push_relative_goangle_multi_point (Sint16 angle, PROP_speed_e speed, bool_e run)
{
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];

	pos->angle = angle;
	pos->speed = speed;
	STACKS_push (PROP, &PROP_relative_goangle_multi_point, run);
}

/* ajoute une instruction goangle sur la pile asser */
void PROP_push_goangle (Sint16 angle, PROP_speed_e speed, bool_e run)
{
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];

	pos->angle = angle;
	pos->speed = speed;
	STACKS_push (PROP, &PROP_goangle, run);
}

/* ajoute une instruction relative_goangle sur la pile asser */
void PROP_push_relative_goangle (Sint16 angle, PROP_speed_e speed, bool_e run)
{
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];

	pos->angle = angle;
	pos->speed = speed;
	STACKS_push (PROP, &PROP_relative_goangle, run);
}

/* ajoute une instruction rush_in_the_wall sur la pile asser */
void PROP_push_rush_in_the_wall (way_e way, bool_e prop_rotate,Sint16 angle, bool_e run)
{
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];

	pos->way = way;
	//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
	pos->speed= prop_rotate;
		pos->angle = angle;
	STACKS_push (PROP, &PROP_rush_in_the_wall, run);
}


// ---------------------------------------------------------------------------- Fonctions de warner


/*
	Fonction permettant d'armer un avertisseur sur la propulsion.
	Un message de BROACAST_POSITION avec raison |= WARNING_REACH_X sera envoyé dès que le robot atteindra cette ligne virtuelle...
	Ce message déclenchera la levée en environnement stratégie du flag global.prop.reach_x
	@param : 0 permet de demander un désarmement de l'avertisseur.
*/
void PROP_WARNER_arm_x(Sint16 x)
{
	CAN_msg_t msg;
	msg.sid = PROP_WARN_X;
	msg.size = SIZE_PROP_WARN_X;
	msg.data.prop_warn_x.x = x;
	CAN_send(&msg);
}

/*
	Fonction permettant d'armer un avertisseur sur la propulsion.
	Un message de BROACAST_POSITION avec raison |= WARNING_REACH_Y sera envoyé dès que le robot atteindra cette ligne virtuelle...
	Ce message déclenchera la levée en environnement stratégie du flag global.prop.reach_y
	@param : 0 permet de demander un désarmement de l'avertisseur.
*/
void PROP_WARNER_arm_y(Sint16 y)
{
	CAN_msg_t msg;
	msg.sid = PROP_WARN_Y;
	msg.size = SIZE_PROP_WARN_Y;
	msg.data.prop_warn_y.y = y;
	CAN_send(&msg);
}

/*
	Fonction permettant d'armer un avertisseur sur la propulsion.
	Un message de BROACAST_POSITION avec raison |= WARNING_REACH_TETA sera envoyé dès que le robot atteindra cette ligne angulaire virtuelle...
	Ce message déclenchera la levée en environnement stratégie du flag global.prop.reach_teta
	@param : 0 permet de demander un désarmement de l'avertisseur.
*/
void PROP_WARNER_arm_teta(Sint16 teta)
{
	CAN_msg_t msg;
	msg.sid = PROP_WARN_ANGLE;
	msg.size = SIZE_PROP_WARN_ANGLE;
	msg.data.prop_warn_angle.angle = teta;
	CAN_send(&msg);
}


// ---------------------------------------------------------------------------- Fonctions autres

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

/* Accesseur en lecture sur les arguments de la pile PROP */
prop_arg_t PROP_get_stack_arg(Uint8 index)
{
	return prop_args[index];
}

/* Accesseur en écriture sur les arguments de la pile PROP */
void PROP_set_stack_arg(prop_arg_t arg, Uint8 index)
{
	prop_args[index] = arg;
}

void PROP_set_threshold_error_translation(Uint8 value, bool_e reset)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_SET_ERROR_TRESHOLD_TRANSLATION;
	msg.size = SIZE_DEBUG_SET_ERROR_TRESHOLD_TRANSLATION;
	if(reset)
		msg.data.debug_set_error_treshold_translation.error_treshold_trans = 0;
	else
		msg.data.debug_set_error_treshold_translation.error_treshold_trans = value;
	CAN_send(&msg);
}
//Impose une position à la propulsion... (A ne pas faire en match !!!!!)
void PROP_set_position(Sint16 x, Sint16 y, Sint16 teta)
{
	CAN_msg_t msg;
	msg.sid = PROP_SET_POSITION;
	msg.size = SIZE_PROP_SET_POSITION;
	msg.data.prop_set_position.x = x;
	msg.data.prop_set_position.y = y;
	msg.data.prop_set_position.teta = teta;
	CAN_send(&msg);
}

//Acceleration en mm/4096/5ms/5ms..
void PROP_set_acceleration(Uint16 acceleration)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_PROPULSION_SET_ACCELERATION;
	msg.size = SIZE_DEBUG_PROPULSION_SET_ACCELERATION;
	msg.data.debug_propulsion_set_acceleration.acceleration_coef = acceleration;
	CAN_send(&msg);
}

void PROP_ask_propulsion_coefs(void)
{
	CAN_send_sid(DEBUG_PROPULSION_GET_COEFS);
}

void PROP_set_correctors(bool_e corrector_rotation, bool_e corrector_translation)
{
	CAN_msg_t msg;
	msg.sid = PROP_SET_CORRECTORS;
	msg.size = SIZE_PROP_SET_CORRECTORS;
	msg.data.prop_set_correctors.rot_corrector = corrector_rotation;
	msg.data.prop_set_correctors.trans_corrector = corrector_translation;
	CAN_send(&msg);
}

/* fonction retournant si on se situe à moins de 30 cm de la destination.
   (distance Manhattan) */
bool_e PROP_near_destination_manhattan()
{
	Sint16 x1, x2, y1, y2;

	x1 = (Sint16) global.pos.x;
	y1 = (Sint16) global.pos.y;
	x2 = (Sint16) prop_args[STACKS_get_top(PROP)].x;
	y2 = (Sint16) prop_args[STACKS_get_top(PROP)].y;
	return ((absolute(y2-y1)+absolute(x2-x1))<100); /* mm */
}

/* fonction retournant si on se situe à moins de 2 degrés de la destination. */
bool_e PROP_near_destination_angle()
{
	Sint16 angle1, angle2;

	angle1 = (Sint16) global.pos.angle;
	angle2 = (Sint16) prop_args[STACKS_get_top(PROP)].angle;
	return (absolute(angle1-angle2)<PI4096/90);
}

bool_e PROP_has_goto(Sint16 x, Sint16 y)
{
	int i;

	for (i = STACKS_get_top(PROP); i >= 0; i--) {

		if ((STACKS_get_action(PROP,i) == &PROP_goto) &&
			(prop_args[i].x == x) &&
			(prop_args[i].y == y)
			)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void PROP_dump_stack ()
{
	int i;
	generic_fun_t command;
	prop_arg_t args;
	char * speed = NULL;
	char * way = NULL;
	char * priority_order = NULL;

	// Dispositif anti-warning
	UNUSED_VAR(priority_order);
	UNUSED_VAR(way);
	UNUSED_VAR(speed);

	debug_printf("\n----- PROP Stack dump -----\n");
	debug_printf("stack top : %d/%d",STACKS_get_top(PROP), STACKS_SIZE);
	for (i = STACKS_SIZE/*STACKS_get_top(PROP)*/; i >= 0; i--) {

		command = STACKS_get_action(PROP,i);
		args = prop_args[i];

		switch (args.speed) {
			case FAST : speed = "FAST"; break;
			case SLOW : speed = "SLOW"; break;
			default   : speed = "undefined"; break;
		}
		switch (args.way) {
			case FORWARD	: way = "FORWARD"; break;
			case BACKWARD	: way = "BACKWARD"; break;
			default			: way = "undefined"; break;
		}
		switch (args.priority_order) {
			case PROP_NOW 			: priority_order = "NOW"; break;
			case PROP_END_OF_BUFFER 	: priority_order = "END_OF_BUFFER"; break;
			default				: priority_order = "undefined"; break;
		}

		if (command == &PROP_goto)
			debug_printf("PROP_goto (%d, %d, %s, %s)\n", args.x, args.y, speed, way);
		else if (command == &PROP_goto_multi_point)
			debug_printf("PROP_multi_point (%d, %d, %s, %s, %s)\n", args.x, args.y, speed, way, priority_order);
		else if (command == &PROP_goangle)
			debug_printf("PROP_goangle (%d [%.1f°], %s)\n", args.angle, 180 * ((double)args.angle) / PI4096, speed);
		else if (command == &wait_forever)
			debug_printf("wait_forever\n");
		else if (command == &PROP_stop)
			debug_printf("prop_stop\n");
		else
			debug_printf("undefined function %lX\n", (Uint32)(command));
	}

	debug_printf("\n----- End of dump -----\n");
}

// ---------------------------------------------------------------------------- Fonctions interne
/* Va a la position indiquée, se termine à l'arret */
static void PROP_goto (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.size = SIZE_PROP_GO_POSITION;
		order.data.prop_go_position.buffer_mode = PROP_NOW;
		order.data.prop_go_position.multipoint = PROP_NO_MULTIPOINT;
		order.data.prop_go_position.border_mode = prop_args[STACKS_get_top(stack_id)].border_mode;
		order.data.prop_go_position.referential = PROP_ABSOLUTE;
		order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
		order.data.prop_go_position.x = prop_args[STACKS_get_top(stack_id)].x;
		order.data.prop_go_position.y = prop_args[STACKS_get_top(stack_id)].y;
		order.data.prop_go_position.speed = prop_args[STACKS_get_top(stack_id)].speed;
		order.data.prop_go_position.way = prop_args[STACKS_get_top(stack_id)].way;
		order.data.prop_go_position.curve = prop_args[STACKS_get_top(stack_id)].curve;
		order.data.prop_go_position.avoidance = prop_args[STACKS_get_top(stack_id)].avoidance;
		CAN_send(&order);
	}
	else
	{
		if (global.prop.ended)
		{
			if (PROP_near_destination_manhattan())
			{
				debug_printf("\nPROP_goto : fini\n");
				STACKS_pull(PROP);
			}
			else
			{
				debug_printf("\nPROP_goto : far_from_destination\n");
				#ifdef PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
				STACKS_pull(PROP);
				#endif /* def PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION */
			}
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME) +
				 (prop_args[STACKS_get_top(stack_id)].avoidance == AVOID_ENABLED_AND_WAIT) ? WAIT_ADD_TIMEOUT_TIME : 0))
			{
				debug_printf("\nPROP_goto : timeout(GOTO)\n");
				STACKS_set_timeout(stack_id,GOTO_TIMEOUT);
			}
		}
	}
}

/* Va a la position indiquée, se termine dès le freinage de la propulsion */
/* Permet d'enchainer manuellement des ordres sans s'arrêter*/
/* Cette fonction EST TRES SEMBLABLE a PROP_goto()*/
static void PROP_goto_until_break (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.size = SIZE_PROP_GO_POSITION;
		order.data.prop_go_position.buffer_mode = PROP_NOW;
		order.data.prop_go_position.multipoint = PROP_NO_MULTIPOINT;
		order.data.prop_go_position.border_mode = prop_args[STACKS_get_top(stack_id)].border_mode;
		order.data.prop_go_position.referential = PROP_ABSOLUTE;
		order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
		order.data.prop_go_position.x = prop_args[STACKS_get_top(stack_id)].x;
		order.data.prop_go_position.y = prop_args[STACKS_get_top(stack_id)].y;
		order.data.prop_go_position.speed = prop_args[STACKS_get_top(stack_id)].speed;
		order.data.prop_go_position.way = prop_args[STACKS_get_top(stack_id)].way;
		order.data.prop_go_position.curve = prop_args[STACKS_get_top(stack_id)].curve;
		order.data.prop_go_position.avoidance = prop_args[STACKS_get_top(stack_id)].avoidance;
		CAN_send(&order);
	}
	else
	{
		if (global.prop.ended || global.prop.freine)
		{
			debug_printf("\nPROP_goto_until_break : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME) +
				 (prop_args[STACKS_get_top(stack_id)].avoidance == AVOID_ENABLED_AND_WAIT) ? WAIT_ADD_TIMEOUT_TIME : 0))
			{
				debug_printf("\nPROP_goto_until_break : timeout(GOTO)\n");
				STACKS_set_timeout(stack_id,GOTO_TIMEOUT);
			}
		}
	}
}


/* Va a la position indiquée, se termine dès que le robot est arrivé à une certaine distance du point de destination */
/* Permet d'enchainer manuellement des ordres sans s'arrêter et indépendemment de la distance de freinage*/
/* Cette fonction EST TRES SEMBLABLE a PROP_goto()*/
static void PROP_goto_until_distance_reached (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	static GEOMETRY_point_t current;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.size = SIZE_PROP_GO_POSITION;
		order.data.prop_go_position.buffer_mode = PROP_NOW;
		order.data.prop_go_position.multipoint = PROP_MULTIPOINT;
		order.data.prop_go_position.border_mode = prop_args[STACKS_get_top(stack_id)].border_mode;
		order.data.prop_go_position.referential = PROP_ABSOLUTE;
		order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
		order.data.prop_go_position.x = prop_args[STACKS_get_top(stack_id)].x;
		order.data.prop_go_position.y = prop_args[STACKS_get_top(stack_id)].y;
		order.data.prop_go_position.speed = prop_args[STACKS_get_top(stack_id)].speed;
		order.data.prop_go_position.way = prop_args[STACKS_get_top(stack_id)].way;
		order.data.prop_go_position.curve = prop_args[STACKS_get_top(stack_id)].curve;
		order.data.prop_go_position.avoidance = prop_args[STACKS_get_top(stack_id)].avoidance;
		CAN_send(&order);

		current.x = prop_args[STACKS_get_top(stack_id)].x ;
		current.y = prop_args[STACKS_get_top(stack_id)].y;
	}
	else
	{
		if (global.prop.ended || PROP_near_destination_euclidienne(distance_to_reach, current) /*global.prop.reach_distance*/)
		{
			debug_printf("\nPROP_goto_until_distance_reached : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME) +
				 (prop_args[STACKS_get_top(stack_id)].avoidance == AVOID_ENABLED_AND_WAIT) ? WAIT_ADD_TIMEOUT_TIME : 0))
			{
				debug_printf("\nPROP_goto : timeout(GOTO)\n");
				STACKS_set_timeout(stack_id,GOTO_TIMEOUT);
			}
		}
	}
}

static void PROP_goto_multi_point (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	prop_arg_t* args;
	//Sint16 distance;
	static time32_t timeout;
	static Uint8 save_stack_top, save_stack_bottom;

	if (init)
	{
		global.pos.nb_points_reached = 0;
		timeout = global.match_time + 2000;
		save_stack_top = STACKS_get_top(stack_id);

		/* On envoie tous les multi points à l'asser */
		for( ; STACKS_get_action(PROP,STACKS_get_top(stack_id)) == &PROP_goto_multi_point ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{
			args=&prop_args[STACKS_get_top(stack_id)];

			order.sid = PROP_GO_POSITION;
			order.size = SIZE_PROP_GO_POSITION;
			order.data.prop_go_position.buffer_mode = args->priority_order;
			order.data.prop_go_position.multipoint = PROP_MULTIPOINT;
			order.data.prop_go_position.border_mode = args->border_mode;
			order.data.prop_go_position.referential = PROP_ABSOLUTE;
			order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
			order.data.prop_go_position.x = args->x;
			order.data.prop_go_position.y = args->y;
			order.data.prop_go_position.speed = args->speed;
			order.data.prop_go_position.way = args->way;
			order.data.prop_go_position.curve = args->curve;
			order.data.prop_go_position.avoidance = args->avoidance;
			CAN_send(&order);
			//distance=GEOMETRY_distance((GEOMETRY_point_t){global.pos.x,global.pos.y},(GEOMETRY_point_t){args->x,args->y});
			//timeout += distance * (args->speed == FAST?COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST:COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW);
			timeout += GOTO_MULTI_POINT_TIMEOUT_TIME;
		}
		if(prop_args[STACKS_get_top(stack_id)].avoidance == AVOID_ENABLED_AND_WAIT)
			timeout += WAIT_ADD_TIMEOUT_TIME;

			/*
			 * On s'arrête quand le haut de la pile n'est plus un PROP_multi_point_goto,
			 * c'est à dire un cran trop loin.
			 */

		// On sauvegarde l'état bas de la pile
		save_stack_bottom = (STACKS_get_top(stack_id)+1);
		//STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
		STACKS_set_top(stack_id,save_stack_top);
		debug_printf("STACK PROP, TOP = %d, BOTTOM = %d\n",save_stack_top, save_stack_bottom);
	}
	else
	{
		if (global.prop.ended)
		{
			debug_printf("PROP_multi_point : freine dernier point\n");
			global.pos.nb_points_reached = save_stack_top - save_stack_bottom + 1;
			// Par sécurité, on met le pointeur de pile à la fin des multi-points
			STACKS_set_top(stack_id,save_stack_bottom);
			STACKS_pull(PROP);
		}
		else if (global.prop.freine)
		{
			global.pos.nb_points_reached++;
			//La réception d'un message de freinage nous permet de considérer que la propulsion à changé de point.
			//On dépile alors l'ordre MULTIPOINT SI il ne s'agit pas du dernier point.
			if(STACKS_get_top(stack_id) != save_stack_bottom)	//S'il ne s'agit pas du dernier point...
			{
				STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1);
				debug_printf("PROP_multi_point : new_point STACK TOP = %d\n",STACKS_get_top(PROP));
			}
		}
		else
		{
			if (global.match_time > timeout)
			{
				debug_printf("\nPROP_goto_multi_point : timeout(GOTO_MULTI_POINT)\n");
				STACKS_set_timeout(stack_id,GOTO_MULTI_POINT_TIMEOUT);
			}
		}
	}
	//debug_printf("nb_points: %d\n", global.pos.nb_points_reached);
}

//Execute une courbe multipoint et fini quand on freine sur le dernier point.
//Permet de passer d'une courbe à une autre sans s'arreter.
static void PROP_goto_multi_point_until_break(stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	prop_arg_t* args;
	//Sint16 distance;
	static time32_t timeout;
	static Uint8 save_stack_top, save_stack_bottom;

	if (init)
	{
		global.pos.nb_points_reached = 0;
		timeout = global.match_time + 2000;
		save_stack_top = STACKS_get_top(stack_id);

		/* On envoie tous les multi points à l'asser */
		for( ; STACKS_get_action(PROP,STACKS_get_top(stack_id)) == &PROP_goto_multi_point_until_break ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{
			args=&prop_args[STACKS_get_top(stack_id)];

			order.sid = PROP_GO_POSITION;
			order.size = SIZE_PROP_GO_POSITION;
			order.data.prop_go_position.buffer_mode = args->priority_order;
			order.data.prop_go_position.multipoint = PROP_MULTIPOINT;
			order.data.prop_go_position.border_mode = args->border_mode;
			order.data.prop_go_position.referential = PROP_ABSOLUTE;
			order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
			order.data.prop_go_position.x = args->x;
			order.data.prop_go_position.y = args->y;
			order.data.prop_go_position.speed = args->speed;
			order.data.prop_go_position.way = args->way;
			order.data.prop_go_position.curve = args->curve;
			order.data.prop_go_position.avoidance = args->avoidance;
			CAN_send(&order);

			//distance=GEOMETRY_distance((GEOMETRY_point_t){global.pos.x,global.pos.y},(GEOMETRY_point_t){args->x,args->y});
			//timeout += distance * (args->speed == FAST?COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST:COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW);
			timeout += GOTO_MULTI_POINT_TIMEOUT_TIME;
		}
		if(prop_args[STACKS_get_top(stack_id)].avoidance == AVOID_ENABLED_AND_WAIT)
			timeout += WAIT_ADD_TIMEOUT_TIME;

			/*
			 * On s'arrête quand le haut de la pile n'est plus un PROP_multi_point_goto,
			 * c'est à dire un cran trop loin.
			 */

		// On sauvegarde l'état bas de la pile
		save_stack_bottom = (STACKS_get_top(stack_id)+1);
		//STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
		STACKS_set_top(stack_id,save_stack_top);
		debug_printf("STACK PROP, TOP = %d, BOTTOM = %d\n",save_stack_top, save_stack_bottom);
	}
	else
	{
		//Si on à fini avant de freiner ... (on est déjà arrivé à destination avant de démarrer ?)
		if (global.prop.ended)
		{
			debug_printf("PROP_multi_point : fini dernier point\n");
			global.pos.nb_points_reached = save_stack_top - save_stack_bottom + 1;
			// Par sécurité, on met le pointeur de pile à la fin des multi-points
			STACKS_set_top(stack_id,save_stack_bottom);
			STACKS_pull(PROP);
		}
		else if (global.prop.freine)
		{
			global.pos.nb_points_reached++;
			//La réception d'un message de freinage nous permet de considérer que la propulsion à changé de point.
			//On dépile alors l'ordre MULTIPOINT.
			if(STACKS_get_top(stack_id) != save_stack_bottom)	//S'il ne s'agit pas du dernier point, on dépile sans initialiser la point suivant (ils ont déjà tous été envoyés)
			{
				STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1);
				debug_printf("PROP_multi_point : new_point STACK TOP = %d\n",STACKS_get_top(PROP));
			} else {	//Dernier point et on freine => on à fini
				debug_printf("PROP_multi_point : freine dernier point\n");
				global.pos.nb_points_reached = save_stack_top - save_stack_bottom + 1;
				STACKS_pull(PROP);
			}
		}
		else
		{
			if (global.match_time > timeout)
			{
				debug_printf("\nPROP_goto_multi_point_until_break : timeout(GOTO_MULTI_POINT)\n");
				STACKS_set_timeout(stack_id,GOTO_MULTI_POINT_TIMEOUT);
			}
		}
	}
}

static void PROP_relative_goangle_multi_point (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	static time32_t timeout;

	if (init)
	{
		timeout = global.match_time;
		/* On envoie tous les multi points à l'asser */
		for( ; STACKS_get_action(PROP,STACKS_get_top(stack_id)) == &PROP_relative_goangle_multi_point ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{
			order.sid = PROP_GO_ANGLE;
			order.size = SIZE_PROP_GO_ANGLE;
			order.data.prop_go_angle.buffer_mode = PROP_END_OF_BUFFER;
			order.data.prop_go_angle.multipoint = PROP_NO_MULTIPOINT;
			order.data.prop_go_angle.referential = PROP_RELATIVE;
			order.data.prop_go_angle.acknowledge = PROP_ACKNOWLEDGE;
			order.data.prop_go_angle.speed = prop_args[STACKS_get_top(stack_id)].speed;
			order.data.prop_go_angle.teta = prop_args[STACKS_get_top(stack_id)].angle;
			CAN_send(&order);

			timeout += (RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME);
		}

			/*
			 * On s'arrêtre quand le haut de la pile n'est plus un PROP_multi_point_goto,
			 * c'est à dire un cran trop loin.
			 */

		STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
	}
	else
	{
		if (global.prop.ended/* || (global.prop.freine && PROP_near_destination_angle())*/)
		{
			debug_printf("\nPROP_relative_goangle_multi_point : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if (global.match_time > timeout)
			{
				debug_printf("\nPROP_relative_goangle_multi_point : timeout(RELATIVE_GOANGLE_MULTI_POINT)\n");
				STACKS_set_timeout(stack_id,RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT);
			}
		}
	}
}

/* Va a l'angle indiquée, se termine à l'arret */
static void PROP_goangle (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_ANGLE;
		order.size = SIZE_PROP_GO_ANGLE;
		order.data.prop_go_angle.buffer_mode = PROP_NOW;
		order.data.prop_go_angle.multipoint = PROP_NO_MULTIPOINT;
		order.data.prop_go_angle.referential = PROP_ABSOLUTE;
		order.data.prop_go_angle.acknowledge = PROP_ACKNOWLEDGE;
		order.data.prop_go_angle.speed = prop_args[STACKS_get_top(stack_id)].speed;
		order.data.prop_go_angle.teta = prop_args[STACKS_get_top(stack_id)].angle;
		CAN_send(&order);
	}
	else
	{
		if (global.prop.ended)
		{
			debug_printf("\nPROP_goangle : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOANGLE_TIMEOUT_TIME)))
			{
				debug_printf("\nPROP_goangle : timeout(GOANGLE)\n");
				STACKS_set_timeout(stack_id,GOANGLE_TIMEOUT);
			}
		}
	}
}

/* Tourne par rapport à l'angle actuel, se termine à l'arret */
static void PROP_relative_goangle (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_ANGLE;
		order.size = SIZE_PROP_GO_ANGLE;
		order.data.prop_go_angle.buffer_mode = PROP_END_OF_BUFFER;
		order.data.prop_go_angle.multipoint = PROP_NO_MULTIPOINT;
		order.data.prop_go_angle.referential = PROP_RELATIVE;
		order.data.prop_go_angle.acknowledge = PROP_ACKNOWLEDGE;
		order.data.prop_go_angle.speed = prop_args[STACKS_get_top(stack_id)].speed;
		order.data.prop_go_angle.teta = prop_args[STACKS_get_top(stack_id)].angle;
		CAN_send(&order);
	}
	else
	{
		if (global.prop.ended)
		{
			debug_printf("\nPROP_relative_goangle : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOANGLE_TIMEOUT_TIME)))
			{
				debug_printf("\nPROP_relative_goangle : timeout(GOANGLE)\n");
				STACKS_set_timeout(stack_id,GOANGLE_TIMEOUT);
			}
		}
	}
}

/* Va a la position relative indiquée, se termine à l'arret */
static void PROP_relative_goto (stack_id_e stack_id, bool_e init)
{

	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.size = SIZE_PROP_GO_POSITION;
		order.data.prop_go_position.buffer_mode = PROP_NOW;
		order.data.prop_go_position.multipoint = PROP_NO_MULTIPOINT;
		order.data.prop_go_position.border_mode = prop_args[STACKS_get_top(stack_id)].border_mode;
		order.data.prop_go_position.referential = PROP_RELATIVE;
		order.data.prop_go_position.acknowledge = PROP_ACKNOWLEDGE;
		order.data.prop_go_position.x = prop_args[STACKS_get_top(stack_id)].x;
		order.data.prop_go_position.y = prop_args[STACKS_get_top(stack_id)].y;
		order.data.prop_go_position.speed = prop_args[STACKS_get_top(stack_id)].speed;
		order.data.prop_go_position.way = prop_args[STACKS_get_top(stack_id)].way;
		order.data.prop_go_position.curve = prop_args[STACKS_get_top(stack_id)].curve;
		order.data.prop_go_position.avoidance = prop_args[STACKS_get_top(stack_id)].avoidance;
		CAN_send(&order);
	}
	else
	{
		if (global.prop.ended)
		{
			if (PROP_near_destination_manhattan())
			{
				debug_printf("\nPROP_relative_goto : fini\n");
				STACKS_pull(PROP);
			}
			else
			{
				debug_printf("\nPROP_relative_goto : far_from_destination\n");
				#ifdef PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
					STACKS_pull(PROP);
				#endif /* def PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION */
			}
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME) +
				 (prop_args[STACKS_get_top(stack_id)].avoidance == AVOID_ENABLED_AND_WAIT) ? WAIT_ADD_TIMEOUT_TIME : 0))
			{
				debug_printf("\nPROP_relative_goto : timeout(GOTO)\n");
				STACKS_set_timeout(stack_id,GOTO_TIMEOUT);
			}
		}
	}
}

/* Va se caler contre le mur */
static void PROP_rush_in_the_wall (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_RUSH_IN_THE_WALL;
		order.size = SIZE_PROP_RUSH_IN_THE_WALL;
		order.data.prop_rush_in_the_wall.teta = prop_args[STACKS_get_top(stack_id)].angle;
		order.data.prop_rush_in_the_wall.way = prop_args[STACKS_get_top(stack_id)].way;
		order.data.prop_rush_in_the_wall.asser_rot = prop_args[STACKS_get_top(stack_id)].speed;
		CAN_send (&order);
	}
	else
	{
		if (global.prop.ended || global.prop.erreur)
		{
			debug_printf("\nPROP_rush_in_the_wal : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(stack_id)) >= (RUSH_TIMEOUT_TIME)))
			{
				debug_printf("\nPROP_rush_in_the_wal : timeout(RUSH)\n");
				STACKS_set_timeout(stack_id,RUSH_TIMEOUT);
			}
		}
	}
}

/* Arrete le robot. L'opération se termine lorsque le robot est arreté */
static void PROP_stop(stack_id_e stack_id, bool_e init) {
	if(init) {
		CAN_send_sid(PROP_STOP);
	} else {
		if (global.prop.ended)
		{
			debug_printf("\nPROP_stop : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (STOP_TIMEOUT_TIME)))
			{
				debug_printf("\nPROP_stop : timeout(STOP)\n");
				STACKS_set_timeout(stack_id,STOP_TIMEOUT);
			}
		}
	}
}

/* fonction retournant si on se situe à moins d'e 30 cm de'une certaine distance de la destination.
   (distance Manhattan) */
bool_e PROP_near_destination_euclidienne(Uint16 distance, GEOMETRY_point_t point)
{
	GEOMETRY_point_t pos_robot;

	pos_robot.x = (Sint16) global.pos.x;
	pos_robot.y = (Sint16) global.pos.y;
	//debug_printf("distance to destination is %d   enable=%d\n",GEOMETRY_squared_distance(pos_robot, point), (GEOMETRY_squared_distance(pos_robot, point) < distance*distance) );
	return (GEOMETRY_squared_distance(pos_robot, point) < distance*distance); /* mm */
}


/* Accesseur en écriture de la variable globale distance_to_reach
 * pour la focntion PROP_goto_until_distance_reached
 * pour le try_going avec END_AT_DISTANCE pour la end_condition
 */
void PROP_set_distance_to_reach(Uint16 dist)
{
	distance_to_reach = dist;
}

/* Accesseur en lecture de la variable globale distance_to_reach
 * pour la focntion PROP_goto_until_distance_reached
 * pour le try_going avec END_AT_DISTANCE pour la end_condition
 */
Uint16 PROP_get_distance_to_reach(void)
{
	return distance_to_reach;
}
