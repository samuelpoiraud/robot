/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Asser_functions.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'asser
 *	Auteur : Jacen
 *	Version 20110313
 */

#define ASSER_FUNCTIONS_C

#include "Asser_functions.h"

/*	Pile conservant les eventuels arguments pour les fonctions des actionneurs
 *	tout en conservant le meme prototype pour tous les actionneurs, reduisant
 *	le code de gestion des actionneurs à chaque boucle
 */
static asser_arg_t asser_args[STACKS_SIZE];
	
void CAN_send_debug(char*);

/* Accesseur en lecture sur les arguments de la pile ASSER */ 
asser_arg_t ASSER_get_stack_arg(Uint8 index)
{
	return asser_args[index];
}

/* Accesseur en écriture sur les arguments de la pile ASSER */ 
void ASSER_set_stack_arg(asser_arg_t arg, Uint8 index)
{
	asser_args[index] = arg;
}

/* Va a la position indiquée, se termine à l'arret */
void ASSER_goto (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = ASSER_GO_POSITION;
		order.data[CONFIG]=NOW+NO_MULTIPOINT+ABSOLUTE;
		order.data[XMSB]=HIGHINT(asser_args[STACKS_get_top(stack_id)].x);
		order.data[XLSB]=LOWINT(asser_args[STACKS_get_top(stack_id)].x);
		order.data[YMSB]=HIGHINT(asser_args[STACKS_get_top(stack_id)].y);
		order.data[YLSB]=LOWINT(asser_args[STACKS_get_top(stack_id)].y);
		order.data[VITESSE]=asser_args[STACKS_get_top(stack_id)].speed;		
		order.data[MARCHE]=asser_args[STACKS_get_top(stack_id)].way;
		order.data[RAYONCRB]=asser_args[STACKS_get_top(stack_id)].curve;
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.asser.fini)
		{
			if (ASSER_near_destination())
			{
				asser_fun_printf("\nASSER_goto : fini\n");
				STACKS_pull(ASSER);
			}
			else
			{
				asser_fun_printf("\nASSER_goto : far_from_destination\n");
				CAN_send_debug("CCCCCCC");
				#ifdef ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
				STACKS_pull(ASSER);
				#endif /* def ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION */
			}
		}
		else 
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(ASSER)) >= (GOTO_TIMEOUT_TIME)))
			{
				CAN_send_debug("0000000");
				asser_fun_printf("\nASSER_timeout : GOTO\n");
				STACKS_set_timeout(stack_id,GOTO_TIMEOUT);
			}
		}
	}
}

#ifdef USE_ASSER_MULTI_POINT
void ASSER_goto_multi_point (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	asser_arg_t* args;
	Sint16 distance;
	static time32_t timeout;
	static Uint8 save_stack_top, save_stack_bottom;

	if (init)
	{
		timeout = global.env.match_time + 2000;
		save_stack_top = STACKS_get_top(stack_id);

		/* On envoie tous les multi points à l'asser */
		for( ; STACKS_get_action(ASSER,STACKS_get_top(stack_id)) == &ASSER_goto_multi_point ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{	
			args=&asser_args[STACKS_get_top(stack_id)];			
			order.sid = ASSER_GO_POSITION;
			order.data[CONFIG]=args->priority_order+MULTIPOINT+ABSOLUTE;
			order.data[XMSB]=HIGHINT(args->x);
			order.data[XLSB]=LOWINT(args->x);
			order.data[YMSB]=HIGHINT(args->y);
			order.data[YLSB]=LOWINT(args->y);
			order.data[VITESSE]=args->speed;		
			order.data[MARCHE]=args->way;
			order.data[RAYONCRB]=args->curve;
			order.size=8;
			CAN_send(&order);
			distance=GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){args->x,args->y});
			//timeout += distance * (args->speed == FAST?COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST:COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW);
			timeout += GOTO_MULTI_POINT_TIMEOUT_TIME;
		}
		
			/*
			 * On s'arrêtre quand le haut de la pile n'est plus un ASSER_multi_point_goto,
			 * c'est à dire un cran trop loin.
			 */

		// On sauvegarde l'état bas de la pile
		save_stack_bottom = (STACKS_get_top(stack_id)+1);
		//STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);
		STACKS_set_top(stack_id,save_stack_top);
		asser_fun_printf("STACK ASSER, TOP = %d, BOTTOM = %d\n",save_stack_top, save_stack_bottom);
	}
	else
	{
		if (global.env.asser.fini)
		{
			asser_fun_printf("\nASSER_multi_point : freine dernier point\n");
			// Par sécurité, on met le pointeur de pile à la fin des multi-points
			STACKS_set_top(stack_id,save_stack_bottom);
			STACKS_pull(ASSER);
		}
		else if (global.env.asser.change_point)
		{
			STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1);
			asser_fun_printf("\nASSER_multi_point : new_point STACK TOP = %d\n",STACKS_get_top(ASSER));
		}
		else 
		{
			if (global.env.match_time > timeout)
			{
				asser_fun_printf("\nASSER_timeout : GOTO_MULTI_POINT\n");
				STACKS_set_timeout(stack_id,GOTO_MULTI_POINT_TIMEOUT);
			}
		}
	}
}

void ASSER_relative_goangle_multi_point (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;
	static time32_t timeout;

	if (init)
	{
		timeout = global.env.match_time;
		/* On envoie tous les multi points à l'asser */
		for( ; STACKS_get_action(ASSER,STACKS_get_top(stack_id)) == &ASSER_relative_goangle_multi_point ; STACKS_set_top(stack_id,STACKS_get_top(stack_id)-1))
		{				
			order.sid = ASSER_GO_ANGLE;
			order.data[CONFIG]=END_OF_BUFFER+MULTIPOINT+RELATIVE;
			order.data[XMSB]=HIGHINT(asser_args[STACKS_get_top(stack_id)].angle);
			order.data[XLSB]=LOWINT(asser_args[STACKS_get_top(stack_id)].angle);
			order.data[YLSB]=0x0;
			order.data[YLSB]=0x0;
			order.data[VITESSE]=(asser_args[STACKS_get_top(stack_id)].speed);
			order.data[MARCHE]=0x00;
			order.data[RAYONCRB]=0x00;
			order.size = 8;
			CAN_send (&order);
			timeout += (RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME);
		}
		
			/*
			 * On s'arrêtre quand le haut de la pile n'est plus un ASSER_multi_point_goto,
			 * c'est à dire un cran trop loin.
			 */
		
		STACKS_set_top(stack_id,STACKS_get_top(stack_id)+1);	
	}
	else
	{
		if (global.env.asser.fini || (global.env.asser.freine && ASSER_near_destination_angle()))
		{
			STACKS_pull(ASSER);
		}
		else
		{
			if (global.env.match_time > timeout)
			{
				asser_fun_printf("\nASSER_timeout : RELATIVE_GOANGLE_MULTI_POINT\n");
				STACKS_set_timeout(stack_id,RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT);
			}
		}
	}
}
#endif /* def USE_ASSER_MULTI_POINT */

/* Va a l'angle indiquée, se termine à l'arret */
void ASSER_goangle (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = ASSER_GO_ANGLE;
		order.data[CONFIG]=NOW+NO_MULTIPOINT+ABSOLUTE;
		order.data[XMSB]=HIGHINT(asser_args[STACKS_get_top(stack_id)].angle);
		order.data[XLSB]=LOWINT(asser_args[STACKS_get_top(stack_id)].angle);
		order.data[YLSB]=0x0;
		order.data[YLSB]=0x0;
		order.data[VITESSE]=(asser_args[STACKS_get_top(stack_id)].speed);
		order.data[MARCHE]=0x00;
		order.data[RAYONCRB]=0x00;
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.asser.fini)
		{
			STACKS_pull(ASSER);
		}
		else 
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(ASSER)) >= (GOANGLE_TIMEOUT_TIME)))
			{
				CAN_send_debug("1111111");
				asser_fun_printf("\nASSER_timeout : GOANGLE\n");
				STACKS_set_timeout(stack_id,GOANGLE_TIMEOUT);
			}
		}
	}
}

/* Tourne par rapport à l'angle actuel, se termine à l'arret */
void ASSER_relative_goangle (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = ASSER_GO_ANGLE;
		order.data[CONFIG]=END_OF_BUFFER+NO_MULTIPOINT+RELATIVE;
		order.data[XMSB]=HIGHINT(asser_args[STACKS_get_top(stack_id)].angle);
		order.data[XLSB]=LOWINT(asser_args[STACKS_get_top(stack_id)].angle);
		order.data[YLSB]=0x0;
		order.data[YLSB]=0x0;
		order.data[VITESSE]=(asser_args[STACKS_get_top(stack_id)].speed);
		order.data[MARCHE]=0x00;
		order.data[RAYONCRB]=0x00;
		order.size = 8;
		CAN_send (&order);
	}
	else
	{
		if (global.env.asser.fini)
		{
			STACKS_pull(ASSER);
		}
		else 
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(ASSER)) >= (GOANGLE_TIMEOUT_TIME)))
			{
				CAN_send_debug("3333333");
				asser_fun_printf("\nASSER_timeout : RELATIVE_GOANGLE\n");
				STACKS_set_timeout(stack_id,GOANGLE_TIMEOUT);
			}
		}
	}
}

/* Va se caler contre le mur */
void ASSER_rush_in_the_wall (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = ASSER_RUSH_IN_THE_WALL;

                //way
                order.data[0]= asser_args[STACKS_get_top(stack_id)].way;
                //speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
                order.data[1]=asser_args[STACKS_get_top(stack_id)].speed;
		order.data[2]= HIGHINT(asser_args[STACKS_get_top(stack_id)].angle) ;
                order.data[3]= LOWINT(asser_args[STACKS_get_top(stack_id)].angle);

		//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
//		order.data[1]=asser_args[STACKS_get_top(stack_id)].speed;
		order.size = 4;
		CAN_send (&order);
	}
	else
	{
		if (global.env.asser.fini || global.env.asser.erreur)
		{
			STACKS_pull(ASSER);
		}
		else
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(stack_id)) >= (RUSH_TIMEOUT_TIME)))
			{
				CAN_send_debug("2222222");
				asser_fun_printf("\nASSER_timeout : RUSH\n");
				STACKS_set_timeout(stack_id,RUSH_TIMEOUT);
			}
		}
	}
}
//fonction dégeulasse coder a 4 heure du mat le dernier jour de la coupe , ne jamais réutiliser cette version du code !!!
void ASSER_rush_in_the_totem_north (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = ASSER_RUSH_IN_THE_WALL;
	//	order.data[0]= asser_args[STACKS_get_top(stack_id)].way;
		order.data[0]=2;
		order.data[1]= 1;
		order.data[2]= HIGHINT(0) ;
		order.data[3]= LOWINT(0);

		//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
//		order.data[1]=asser_args[STACKS_get_top(stack_id)].speed;
		order.size = 4;
		CAN_send (&order);
	}
	else
	{
		if (global.env.asser.fini || global.env.asser.erreur)
		{
			STACKS_pull(ASSER);
		}
		else 
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(stack_id)) >= (RUSH_TIMEOUT_TIME)))
			{
				CAN_send_debug("2222222");
				asser_fun_printf("\nASSER_timeout : RUSH\n");
				STACKS_set_timeout(stack_id,RUSH_TIMEOUT);
			}
		}			
	}
}


void ASSER_rush_in_the_totem_south (stack_id_e stack_id, bool_e init)
{
	CAN_msg_t order;

	if (init)
	{
		order.sid = ASSER_RUSH_IN_THE_WALL;
	//	order.data[0]= asser_args[STACKS_get_top(stack_id)].way;
		order.data[0]= 2;
		order.data[1]= 1;
		order.data[2]= HIGHINT(PI4096) ;
		order.data[3]= LOWINT(PI4096);
		//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
//		order.data[1]=asser_args[STACKS_get_top(stack_id)].speed;
		order.size = 4;
		CAN_send (&order);
	}
	else
	{
		if (global.env.asser.fini || global.env.asser.erreur)
		{
			STACKS_pull(ASSER);
		}
		else 
		{
			if ((global.env.match_time - STACKS_get_action_initial_time(stack_id,STACKS_get_top(stack_id)) >= (RUSH_TIMEOUT_TIME)))
			{
				CAN_send_debug("2222222");
				asser_fun_printf("\nASSER_timeout : RUSH\n");
				STACKS_set_timeout(stack_id,RUSH_TIMEOUT);
			}
		}			
	}
}



void ASSER_stop ()
{
	CAN_msg_t msg;
	msg.sid = ASSER_STOP;
	msg.size = 0;
	CAN_send(&msg);
}

/* ajoute une instruction goto sur la pile asser */
void ASSER_push_goto (Sint16 x, Sint16 y, ASSER_speed_e speed, ASSER_way_e way, Uint8 curve, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->x = x;
	pos->y = y;
	pos->speed = speed;
	pos->way = way;
	pos->curve = curve;
	STACKS_push (ASSER, &ASSER_goto, run);
}

#ifdef USE_ASSER_MULTI_POINT
void ASSER_push_goto_multi_point (Sint16 x, Sint16 y, ASSER_speed_e speed, ASSER_way_e way, Uint8 curve, Uint8 priority_order, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->x = x;
	pos->y = y;
	pos->speed = speed;
	pos->way = way;
	pos->curve = curve;
	pos->priority_order = priority_order;
	STACKS_push (ASSER, &ASSER_goto_multi_point, run);
}

void ASSER_push_relative_goangle_multi_point (Sint16 angle, ASSER_speed_e speed, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->angle = angle;
	pos->speed = speed;
	STACKS_push (ASSER, &ASSER_relative_goangle_multi_point, run);
}
#endif /* def USE_ASSER_MULTI_POINT */

/* ajoute une instruction goangle sur la pile asser */
void ASSER_push_goangle (Sint16 angle, ASSER_speed_e speed, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->angle = angle;
	pos->speed = speed;
	STACKS_push (ASSER, &ASSER_goangle, run);
}

/* ajoute une instruction relative_goangle sur la pile asser */
void ASSER_push_relative_goangle (Sint16 angle, ASSER_speed_e speed, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->angle = angle;
	pos->speed = speed;
	STACKS_push (ASSER, &ASSER_relative_goangle, run);
}

/* ajoute une instruction rush_in_the_wall sur la pile asser */
void ASSER_push_rush_in_the_wall (ASSER_way_e way, bool_e asser_rotate,Sint16 angle, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->way = way;
	//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
	pos->speed= asser_rotate;
        pos->angle = angle;
	STACKS_push (ASSER, &ASSER_rush_in_the_wall, run);
}



void ASSER_push_rush_in_the_totem_south (ASSER_way_e way, bool_e asser_rotate, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->way = way;
	//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
	pos->speed= asser_rotate;
	STACKS_push (ASSER, &ASSER_rush_in_the_totem_south, run);
}

void ASSER_push_rush_in_the_totem_north (ASSER_way_e way, bool_e asser_rotate, bool_e run)
{
	asser_arg_t* pos = &asser_args[STACKS_get_top(ASSER)+1];

	pos->way = way;
	//speed est utilisé pour indiquer si l'asservissement en rotation doit etre actif.
	pos->speed= asser_rotate;
	STACKS_push (ASSER, &ASSER_rush_in_the_totem_north, run);
}


/* fonction retournant si on se situe à moins de 30 cm de la destination.
   (distance Manhattan) */
bool_e ASSER_near_destination()
{
	Sint16 x1, x2, y1, y2;
	
	x1 = (Sint16) global.env.pos.x;
	y1 = (Sint16) global.env.pos.y;
	x2 = (Sint16) asser_args[STACKS_get_top(ASSER)].x;
	y2 = (Sint16) asser_args[STACKS_get_top(ASSER)].y;
	return ((abs(y2-y1)+abs(x2-x1))<100); /* mm */
}

/* fonction retournant si on se situe à moins de 2 degrés cm de la destination. */
bool_e ASSER_near_destination_angle()
{
	Sint16 angle1, angle2;
	
	angle1 = (Sint16) global.env.pos.angle;
	angle2 = (Sint16) asser_args[STACKS_get_top(ASSER)].angle;
	return (abs(angle1-angle2)<PI4096/90); 
}


bool_e ASSER_has_goto(Sint16 x, Sint16 y)
{
	int i;
	
	for (i = STACKS_get_top(ASSER); i >= 0; i--) {
		
		if ((STACKS_get_action(ASSER,i) == &ASSER_goto) &&
			(asser_args[i].x == x) &&
			(asser_args[i].y == y)
			) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

void ASSER_dump_statck ()
{
	int i;
	generic_fun_t command;
	asser_arg_t args;
	char * speed;
	char * way;
	char * priority_order;

	asser_fun_printf("\n----- ASSER Stack dump -----\n");
	for (i = STACKS_get_top(ASSER); i >= 0; i--) {

		command = STACKS_get_action(ASSER,i);
		args = asser_args[i];

		switch (args.speed) {
			case FAST : speed = "FAST"; break;
			case SLOW : speed = "SLOW"; break;
			default   : speed = "undefined"; break;
		}
		switch (args.way) {
			case FORWARD : way = "FORWARD"; break;
			case REAR    : way = "REAR"; break;
			default      : way = "undefined"; break;
		}
		switch (args.priority_order) {
			case NOW 			: priority_order = "NOW"; break;
			case END_OF_BUFFER 	: priority_order = "END_OF_BUFFER"; break;
			default				: priority_order = "undefined"; break;
		}


		if (command == &ASSER_goto)
		{
			asser_fun_printf("ASSER_goto (%d, %d, %s, %s)\n", args.x, args.y, speed, way);
		}
#ifdef USE_ASSER_MULTI_POINT
		else if (command == &ASSER_goto_multi_point)
		{
			asser_fun_printf("ASSER_multi_point (%d, %d, %s, %s, %s)\n", args.x, args.y, speed, way, priority_order);
		}
#endif /* def #ifdef USE_ASSER_MULTI_POINT */
		else if (command == &ASSER_goangle) {
			asser_fun_printf("ASSER_goangle (%d [%.1f°], %s)\n", args.angle, 180 * ((double)args.angle) / PI4096, speed);
		}
		else if (command == &wait_forever) {
			asser_fun_printf("wait_forever\n");
		}
		else {
			asser_fun_printf("undefined function\n");
		}
	}

	asser_fun_printf("\n----- End of dump -----\n");
}

