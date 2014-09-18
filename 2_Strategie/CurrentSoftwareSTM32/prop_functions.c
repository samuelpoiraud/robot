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
#include "avoidance.h"
#include "config_use.h"
#include "QS/QS_outputlog.h"

/*	Pile conservant les eventuels arguments pour les fonctions des actionneurs
 *	tout en conservant le meme prototype pour tous les actionneurs, reduisant
 *	le code de gestion des actionneurs � chaque boucle
 */
static prop_arg_t prop_args[STACKS_SIZE];

void CAN_send_debug(char*);
static void PROP_goto (stack_id_e stack_id, bool_e init);
static void PROP_goto_until_break (stack_id_e stack_id, bool_e init);
static void PROP_relative_goto (stack_id_e stack_id, bool_e init);
static void PROP_goto_multi_point (stack_id_e stack_id, bool_e init);
static void PROP_goto_multi_point_until_break(stack_id_e stack_id, bool_e init);
static void PROP_relative_goangle_multi_point (stack_id_e stack_id, bool_e init);
static void PROP_goangle (stack_id_e stack_id, bool_e init);
static void PROP_relative_goangle (stack_id_e stack_id, bool_e init);
static void PROP_rush_in_the_wall(stack_id_e stack_id, bool_e init);
static void PROP_stop(stack_id_e stack_id, bool_e init);


// ---------------------------------------------------------------------------- Fonctions de d�placement

void PROP_push_stop ()
{
	STACKS_push (PROP, &PROP_stop, TRUE);
}

/* ajoute une instruction goto sur la pile asser */
void PROP_push_goto (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, avoidance_e avoidance, PROP_end_condition_e end_condition ,bool_e run)
{
	debug_printf("\nD=%d\n",x);
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];
	debug_printf("\nE=%d\n",x);
	pos->x = x;
	pos->y = y;
	pos->speed = speed;
	pos->way = way;
	pos->curve = curve;
	pos->avoidance = avoidance;
	if(end_condition == END_AT_LAST_POINT)
		STACKS_push (PROP, &PROP_goto, run);
	else
		STACKS_push (PROP, &PROP_goto_until_break, run);
}

void PROP_push_goto_multi_point (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, avoidance_e avoidance, Uint8 priority_order, PROP_end_condition_e end_condition ,bool_e run)
{
	prop_arg_t* pos = &prop_args[STACKS_get_top(PROP)+1];

	pos->x = x;
	pos->y = y;
	pos->speed = speed;
	pos->way = way;
	pos->curve = curve;
	pos->priority_order = priority_order;
	pos->avoidance = avoidance;
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
	//speed est utilis� pour indiquer si l'asservissement en rotation doit etre actif.
	pos->speed= prop_rotate;
		pos->angle = angle;
	STACKS_push (PROP, &PROP_rush_in_the_wall, run);
}


// ---------------------------------------------------------------------------- Fonctions de warner


/*
	Fonction permettant d'armer un avertisseur sur la propulsion.
	Un message de BROACAST_POSITION avec raison |= WARNING_REACH_X sera envoy� d�s que le robot atteindra cette ligne virtuelle...
	Ce message d�clenchera la lev�e en environnement strat�gie du flag global.env.prop.reach_x
	@param : 0 permet de demander un d�sarmement de l'avertisseur.
*/
void PROP_WARNER_arm_x(Sint16 x)
{
	CAN_msg_t msg;
	msg.sid = PROP_WARN_X;
	msg.data[0] = HIGHINT(x);
	msg.data[1] = LOWINT(x);
	msg.size = 2;
	CAN_send(&msg);
}

/*
	Fonction permettant d'armer un avertisseur sur la propulsion.
	Un message de BROACAST_POSITION avec raison |= WARNING_REACH_Y sera envoy� d�s que le robot atteindra cette ligne virtuelle...
	Ce message d�clenchera la lev�e en environnement strat�gie du flag global.env.prop.reach_y
	@param : 0 permet de demander un d�sarmement de l'avertisseur.
*/
void PROP_WARNER_arm_y(Sint16 y)
{
	CAN_msg_t msg;
	msg.sid = PROP_WARN_Y;
	msg.data[0] = HIGHINT(y);
	msg.data[1] = LOWINT(y);
	msg.size = 2;
	CAN_send(&msg);
}

/*
	Fonction permettant d'armer un avertisseur sur la propulsion.
	Un message de BROACAST_POSITION avec raison |= WARNING_REACH_TETA sera envoy� d�s que le robot atteindra cette ligne angulaire virtuelle...
	Ce message d�clenchera la lev�e en environnement strat�gie du flag global.env.prop.reach_teta
	@param : 0 permet de demander un d�sarmement de l'avertisseur.
*/
void PROP_WARNER_arm_teta(Sint16 teta)
{
	CAN_msg_t msg;
	msg.sid = PROP_WARN_ANGLE;
	msg.data[0] = HIGHINT(teta);
	msg.data[1] = LOWINT(teta);
	msg.size = 2;
	CAN_send(&msg);
}


// ---------------------------------------------------------------------------- Fonctions autres

/* Accesseur en lecture sur les arguments de la pile PROP */
prop_arg_t PROP_get_stack_arg(Uint8 index)
{
	return prop_args[index];
}

/* Accesseur en �criture sur les arguments de la pile PROP */
void PROP_set_stack_arg(prop_arg_t arg, Uint8 index)
{
	prop_args[index] = arg;
}

void PROP_set_threshold_error_translation(Uint8 value, bool_e reset)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_SET_ERROR_TRESHOLD_TRANSLATION;
	if(reset)
		msg.data[0] = 0;
	else
		msg.data[0] = value;
	msg.size = 1;
	CAN_send(&msg);
}
//Impose une position � la propulsion... (A ne pas faire en match !!!!!)
void PROP_set_position(Sint16 x, Sint16 y, Sint16 teta)
{
	CAN_msg_t msg;
	msg.sid=PROP_SET_POSITION;
	msg.data[0]=HIGHINT(x);
	msg.data[1]=LOWINT(x);
	msg.data[2]=HIGHINT(y);
	msg.data[3]=LOWINT(y);
	msg.data[4]=HIGHINT(teta);
	msg.data[5]=LOWINT(teta);
	msg.size=6;
	CAN_send(&msg);
}

//Acceleration en mm/4096/5ms/5ms..
void PROP_set_acceleration(Uint8 acceleration)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_PROPULSION_SET_ACCELERATION;
	msg.data[0] = 0x00;
	msg.data[1] = acceleration;
	msg.size = 2;
	CAN_send(&msg);
}

void PROP_ask_propulsion_coefs(void)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_PROPULSION_GET_COEFS;
	msg.size = 0;
	CAN_send(&msg);
}

void PROP_set_correctors(bool_e corrector_rotation, bool_e corrector_translation)
{
	CAN_msg_t msg;
	msg.sid = PROP_SET_CORRECTORS;
	msg.data[0] = corrector_rotation;
	msg.data[1] = corrector_translation;
	msg.size = 2;
	CAN_send(&msg);
}

/* fonction retournant si on se situe � moins de 30 cm de la destination.
   (distance Manhattan) */
bool_e PROP_near_destination()
{
	Sint16 x1, x2, y1, y2;

	x1 = (Sint16) global.env.pos.x;
	y1 = (Sint16) global.env.pos.y;
	x2 = (Sint16) prop_args[STACKS_get_top(PROP)].x;
	y2 = (Sint16) prop_args[STACKS_get_top(PROP)].y;
	return ((absolute(y2-y1)+absolute(x2-x1))<100); /* mm */
}

/* fonction retournant si on se situe � moins de 2 degr�s de la destination. */
bool_e PROP_near_destination_angle()
{
	Sint16 angle1, angle2;

	angle1 = (Sint16) global.env.pos.angle;
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
	char * speed;
	char * way;
	char * priority_order;

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
			case FORWARD : way = "FORWARD"; break;
			case BACKWARD    : way = "BACKWARD"; break;
			default      : way = "undefined"; break;
		}
		switch (args.priority_order) {
			case NOW 			: priority_order = "NOW"; break;
			case END_OF_BUFFER 	: priority_order = "END_OF_BUFFER"; break;
			default				: priority_order = "undefined"; break;
		}

		if (command == &PROP_goto)
			debug_printf("PROP_goto (%d, %d, %s, %s)\n", args.x, args.y, speed, way);
		else if (command == &PROP_goto_multi_point)
			debug_printf("PROP_multi_point (%d, %d, %s, %s, %s)\n", args.x, args.y, speed, way, priority_order);
		else if (command == &PROP_goangle)
			debug_printf("PROP_goangle (%d [%.1f�], %s)\n", args.angle, 180 * ((double)args.angle) / PI4096, speed);
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
/* Va a la position indiqu�e, se termine � l'arret */
static void PROP_goto (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.data[CONFIG]=NOW+NO_MULTIPOINT+ABSOLUTE;
		order.data[XMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].x);
		order.data[XLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].x);
		order.data[YMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].y);
		order.data[YLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].y);
		order.data[VITESSE]=prop_args[STACKS_get_top(stack_id)].speed;
		order.data[MARCHE]=prop_args[STACKS_get_top(stack_id)].way;
		order.data[RAYONCRB]=prop_args[STACKS_get_top(stack_id)].curve | (prop_args[STACKS_get_top(stack_id)].avoidance << 4);
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.prop.ended)
		{
			if (PROP_near_destination())
			{
				prop_fun_printf("\nPROP_goto : fini\n");
				STACKS_pull(PROP);
			}
			else
			{
				prop_fun_printf("\nPROP_goto : far_from_destination\n");
				CAN_send_debug("CCCCCCC");
				#ifdef PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
				STACKS_pull(PROP);
				#endif /* def PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION */
			}
		}
		else
		{
#ifdef USE_PROP_AVOIDANCE
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME) + WAIT_ADD_TIMEOUT_TIME))
#else
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME)))
#endif
			{
				CAN_send_debug("0000000");
				prop_fun_printf("\nPROP_goto : timeout(GOTO)\n");
				STACKS_set_timeout(stack_id,GOTO_TIMEOUT);
			}
		}
	}
}

/* Va a la position indiqu�e, se termine d�s le freinage de la propulsion */
/* Permet d'enchainer manuellement des ordres sans s'arr�ter*/
/* Cette fonction EST TRES SEMBLABLE a PROP_goto()*/
static void PROP_goto_until_break (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.data[CONFIG]=NOW+NO_MULTIPOINT+ABSOLUTE;
		order.data[XMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].x);
		order.data[XLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].x);
		order.data[YMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].y);
		order.data[YLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].y);
		order.data[VITESSE]=prop_args[STACKS_get_top(stack_id)].speed;
		order.data[MARCHE]=prop_args[STACKS_get_top(stack_id)].way;
		order.data[RAYONCRB]=prop_args[STACKS_get_top(stack_id)].curve | (prop_args[STACKS_get_top(stack_id)].avoidance << 4);
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.prop.ended || global.env.prop.freine)
		{
			prop_fun_printf("\nPROP_goto_until_break : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
#ifdef USE_PROP_AVOIDANCE
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME) + WAIT_ADD_TIMEOUT_TIME))
#else
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME)))
#endif
			{
				CAN_send_debug("0000000");
				prop_fun_printf("\nPROP_goto_until_break : timeout(GOTO)\n");
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
		global.env.pos.nb_points_reached = 0;
		timeout = global.env.match_time + 2000;
		save_stack_top = STACKS_get_top(stack_id);

		/* On envoie tous les multi points � l'asser */
		for( ; STACKS_get_action(PROP,STACKS_get_top(stack_id)) == &PROP_goto_multi_point ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{
			args=&prop_args[STACKS_get_top(stack_id)];
			order.sid = PROP_GO_POSITION;
			order.data[CONFIG]=args->priority_order+MULTIPOINT+ABSOLUTE;
			order.data[XMSB]=HIGHINT(args->x);
			order.data[XLSB]=LOWINT(args->x);
			order.data[YMSB]=HIGHINT(args->y);
			order.data[YLSB]=LOWINT(args->y);
			order.data[VITESSE]=args->speed;
			order.data[MARCHE]=args->way;
			order.data[RAYONCRB]=args->curve | (args->avoidance << 4);
			order.size=8;
			CAN_send(&order);
			//distance=GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){args->x,args->y});
			//timeout += distance * (args->speed == FAST?COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST:COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW);
			timeout += GOTO_MULTI_POINT_TIMEOUT_TIME;
		}
		#ifdef USE_PROP_AVOIDANCE
			timeout += WAIT_ADD_TIMEOUT_TIME;
		#endif

			/*
			 * On s'arr�te quand le haut de la pile n'est plus un PROP_multi_point_goto,
			 * c'est � dire un cran trop loin.
			 */

		// On sauvegarde l'�tat bas de la pile
		save_stack_bottom = (STACKS_get_top(stack_id)+1);
		//STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
		STACKS_set_top(stack_id,save_stack_top);
		prop_fun_printf("STACK PROP, TOP = %d, BOTTOM = %d\n",save_stack_top, save_stack_bottom);
	}
	else
	{
		if (global.env.prop.ended)
		{
			prop_fun_printf("\nPROP_multi_point : freine dernier point\n");
			global.env.pos.nb_points_reached = save_stack_top - save_stack_bottom + 1;
			// Par s�curit�, on met le pointeur de pile � la fin des multi-points
			STACKS_set_top(stack_id,save_stack_bottom);
			STACKS_pull(PROP);
		}
		else if (global.env.prop.freine)
		{
			global.env.pos.nb_points_reached++;
			//La r�ception d'un message de freinage nous permet de consid�rer que la propulsion � chang� de point.
			//On d�pile alors l'ordre MULTIPOINT SI il ne s'agit pas du dernier point.
			if(STACKS_get_top(stack_id) != save_stack_bottom)	//S'il ne s'agit pas du dernier point...
			{
				STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1);
				prop_fun_printf("\nPROP_multi_point : new_point STACK TOP = %d\n",STACKS_get_top(PROP));
			}
		}
		else
		{
			if (global.env.match_time > timeout)
			{
				prop_fun_printf("\nPROP_goto_multi_point : timeout(GOTO_MULTI_POINT)\n");
				STACKS_set_timeout(stack_id,GOTO_MULTI_POINT_TIMEOUT);
			}
		}
	}
	//debug_printf("nb_points: %d\n", global.env.pos.nb_points_reached);
}

//Execute une courbe multipoint et fini quand on freine sur le dernier point.
//Permet de passer d'une courbe � une autre sans s'arreter.
static void PROP_goto_multi_point_until_break(stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	prop_arg_t* args;
	//Sint16 distance;
	static time32_t timeout;
	static Uint8 save_stack_top, save_stack_bottom;

	if (init)
	{
		global.env.pos.nb_points_reached = 0;
		timeout = global.env.match_time + 2000;
		save_stack_top = STACKS_get_top(stack_id);

		/* On envoie tous les multi points � l'asser */
		for( ; STACKS_get_action(PROP,STACKS_get_top(stack_id)) == &PROP_goto_multi_point_until_break ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{
			args=&prop_args[STACKS_get_top(stack_id)];
			order.sid = PROP_GO_POSITION;
			order.data[CONFIG]=args->priority_order+MULTIPOINT+ABSOLUTE;
			order.data[XMSB]=HIGHINT(args->x);
			order.data[XLSB]=LOWINT(args->x);
			order.data[YMSB]=HIGHINT(args->y);
			order.data[YLSB]=LOWINT(args->y);
			order.data[VITESSE]=args->speed;
			order.data[MARCHE]=args->way;
			order.data[RAYONCRB]=args->curve | (args->avoidance << 4);
			order.size=8;
			CAN_send(&order);
			//distance=GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){args->x,args->y});
			//timeout += distance * (args->speed == FAST?COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST:COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW);
			timeout += GOTO_MULTI_POINT_TIMEOUT_TIME;
		}
		#ifdef USE_PROP_AVOIDANCE
			timeout += WAIT_ADD_TIMEOUT_TIME;
		#endif

			/*
			 * On s'arr�te quand le haut de la pile n'est plus un PROP_multi_point_goto,
			 * c'est � dire un cran trop loin.
			 */

		// On sauvegarde l'�tat bas de la pile
		save_stack_bottom = (STACKS_get_top(stack_id)+1);
		//STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
		STACKS_set_top(stack_id,save_stack_top);
		prop_fun_printf("STACK PROP, TOP = %d, BOTTOM = %d\n",save_stack_top, save_stack_bottom);
	}
	else
	{
		//Si on � fini avant de freiner ... (on est d�j� arriv� � destination avant de d�marrer ?)
		if (global.env.prop.ended)
		{
			prop_fun_printf("\nPROP_multi_point : fini dernier point\n");
			global.env.pos.nb_points_reached = save_stack_top - save_stack_bottom + 1;
			// Par s�curit�, on met le pointeur de pile � la fin des multi-points
			STACKS_set_top(stack_id,save_stack_bottom);
			STACKS_pull(PROP);
		}
		else if (global.env.prop.freine)
		{
			global.env.pos.nb_points_reached++;
			//La r�ception d'un message de freinage nous permet de consid�rer que la propulsion � chang� de point.
			//On d�pile alors l'ordre MULTIPOINT.
			if(STACKS_get_top(stack_id) != save_stack_bottom)	//S'il ne s'agit pas du dernier point, on d�pile sans initialiser la point suivant (ils ont d�j� tous �t� envoy�s)
			{
				STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1);
				prop_fun_printf("\nPROP_multi_point : new_point STACK TOP = %d\n",STACKS_get_top(PROP));
			} else {	//Dernier point et on freine => on � fini
				prop_fun_printf("\nPROP_multi_point : freine dernier point\n");
				global.env.pos.nb_points_reached = save_stack_top - save_stack_bottom + 1;
				STACKS_pull(PROP);
			}
		}
		else
		{
			if (global.env.match_time > timeout)
			{
				prop_fun_printf("\nPROP_goto_multi_point_until_break : timeout(GOTO_MULTI_POINT)\n");
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
		timeout = global.env.match_time;
		/* On envoie tous les multi points � l'asser */
		for( ; STACKS_get_action(PROP,STACKS_get_top(stack_id)) == &PROP_relative_goangle_multi_point ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{
			order.sid = PROP_GO_ANGLE;
			order.data[CONFIG]=END_OF_BUFFER+MULTIPOINT+RELATIVE;
			order.data[XMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].angle);
			order.data[XLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].angle);
			order.data[YLSB]=0x0;
			order.data[YLSB]=0x0;
			order.data[VITESSE]=(prop_args[STACKS_get_top(stack_id)].speed);
			order.data[MARCHE]=0x00;
			order.data[RAYONCRB]=0x00;
			order.size = 8;
			CAN_send (&order);
			timeout += (RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME);
		}

			/*
			 * On s'arr�tre quand le haut de la pile n'est plus un PROP_multi_point_goto,
			 * c'est � dire un cran trop loin.
			 */

		STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
	}
	else
	{
		if (global.env.prop.ended/* || (global.env.prop.freine && PROP_near_destination_angle())*/)
		{
			prop_fun_printf("\nPROP_relative_goangle_multi_point : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if (global.env.match_time > timeout)
			{
				prop_fun_printf("\nPROP_relative_goangle_multi_point : timeout(RELATIVE_GOANGLE_MULTI_POINT)\n");
				STACKS_set_timeout(stack_id,RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT);
			}
		}
	}
}

/* Va a l'angle indiqu�e, se termine � l'arret */
static void PROP_goangle (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_ANGLE;
		order.data[CONFIG]=NOW+NO_MULTIPOINT+ABSOLUTE;
		order.data[XMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].angle);
		order.data[XLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].angle);
		order.data[YLSB]=0x0;
		order.data[YLSB]=0x0;
		order.data[VITESSE]=(prop_args[STACKS_get_top(stack_id)].speed);
		order.data[MARCHE]=0x00;
		order.data[RAYONCRB]=0x00;
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.prop.ended)
		{
			prop_fun_printf("\nPROP_goangle : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOANGLE_TIMEOUT_TIME)))
			{
				CAN_send_debug("1111111");
				prop_fun_printf("\nPROP_goangle : timeout(GOANGLE)\n");
				STACKS_set_timeout(stack_id,GOANGLE_TIMEOUT);
			}
		}
	}
}

/* Tourne par rapport � l'angle actuel, se termine � l'arret */
static void PROP_relative_goangle (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_ANGLE;
		order.data[CONFIG]=END_OF_BUFFER+NO_MULTIPOINT+RELATIVE;
		order.data[XMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].angle);
		order.data[XLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].angle);
		order.data[YLSB]=0x0;
		order.data[YLSB]=0x0;
		order.data[VITESSE]=(prop_args[STACKS_get_top(stack_id)].speed);
		order.data[MARCHE]=0x00;
		order.data[RAYONCRB]=0x00;
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.prop.ended)
		{
			prop_fun_printf("\nPROP_relative_goangle : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOANGLE_TIMEOUT_TIME)))
			{
				CAN_send_debug("3333333");
				prop_fun_printf("\nPROP_relative_goangle : timeout(GOANGLE)\n");
				STACKS_set_timeout(stack_id,GOANGLE_TIMEOUT);
			}
		}
	}
}

/* Va a la position relative indiqu�e, se termine � l'arret */
static void PROP_relative_goto (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = PROP_GO_POSITION;
		order.data[CONFIG]=NOW+NO_MULTIPOINT+RELATIVE;
		order.data[XMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].x);
		order.data[XLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].x);
		order.data[YMSB]=HIGHINT(prop_args[STACKS_get_top(stack_id)].y);
		order.data[YLSB]=LOWINT(prop_args[STACKS_get_top(stack_id)].y);
		order.data[VITESSE]=prop_args[STACKS_get_top(stack_id)].speed;
		order.data[MARCHE]=prop_args[STACKS_get_top(stack_id)].way;
		order.data[RAYONCRB]=prop_args[STACKS_get_top(stack_id)].curve | (prop_args[STACKS_get_top(stack_id)].avoidance << 4);
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.prop.ended)
		{
			if (PROP_near_destination())
			{
				prop_fun_printf("\nPROP_relative_goto : fini\n");
				STACKS_pull(PROP);
			}
			else
			{
				prop_fun_printf("\nPROP_relative_goto : far_from_destination\n");
				CAN_send_debug("CCCCCCC");
				#ifdef PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
				STACKS_pull(PROP);
				#endif /* def PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION */
			}
		}
		else
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (GOTO_TIMEOUT_TIME)))
			{
				CAN_send_debug("0000000");
				prop_fun_printf("\nPROP_relative_goto : timeout(GOTO)\n");
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

				//way
				order.data[0]= prop_args[STACKS_get_top(stack_id)].way;
				//speed est utilis� pour indiquer si l'asservissement en rotation doit etre actif.
				order.data[1]=prop_args[STACKS_get_top(stack_id)].speed;
		order.data[2]= HIGHINT(prop_args[STACKS_get_top(stack_id)].angle) ;
				order.data[3]= LOWINT(prop_args[STACKS_get_top(stack_id)].angle);

		//speed est utilis� pour indiquer si l'asservissement en rotation doit etre actif.
//		order.data[1]=prop_args[STACKS_get_top(stack_id)].speed;
		order.size = 4;
		CAN_send (&order);
	}
	else
	{
		if (global.env.prop.ended || global.env.prop.erreur)
		{
			prop_fun_printf("\nPROP_rush_in_the_wal : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(stack_id)) >= (RUSH_TIMEOUT_TIME)))
			{
				CAN_send_debug("2222222");
				prop_fun_printf("\nPROP_rush_in_the_wal : timeout(RUSH)\n");
				STACKS_set_timeout(stack_id,RUSH_TIMEOUT);
			}
		}
	}
}

/* Arrete le robot. L'op�ration se termine lorsque le robot est arret� */
static void PROP_stop(stack_id_e stack_id, bool_e init) {
	if(init) {
		CAN_msg_t msg;

		msg.sid = PROP_STOP;
		msg.size = 0;
		CAN_send(&msg);
	} else {
		if (global.env.prop.ended)
		{
			prop_fun_printf("\nPROP_stop : fini\n");
			STACKS_pull(PROP);
		}
		else
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(PROP)) >= (STOP_TIMEOUT_TIME)))
			{
				prop_fun_printf("\nPROP_stop : timeout(STOP)\n");
				STACKS_set_timeout(stack_id,STOP_TIMEOUT);
			}
		}
	}
}
