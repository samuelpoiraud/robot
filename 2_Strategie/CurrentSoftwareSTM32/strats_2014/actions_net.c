/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.c
 *	Package : Carte S²/strats2013
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par .
 *	Version 2013/10/03
 */


#include "actions_net.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../avoidance.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "../Supervision/Buzzer.h"
#include <math.h>


#define DIST_LAUNCH_NET		300
#define Y_MAMMOUTH_RED		750
#define Y_MAMMOUTH_YELLOW	2350
#define NB_NODE				6

void strat_placement_net(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PLACEMENT_NET,
		INIT,
		STOP_ROBOT,
		INIT_PATH,
		FOUND_PATH,
		GET_IN,
		PLACEMENT,
		PLACEMENT_TETA,
		STOP_ALL,
		NO_PATH,
		DONE
	);


	typedef struct{
		struct{Sint16 x; Sint16 y;}	node[NB_NODE];
		Uint16 dist_with_node[NB_NODE];
		bool_e tryed_node[NB_NODE];
		enum{RED_MAMMOTH, YELLOW_MAMMOTH} focused_mammoth[NB_NODE];
		Uint8 selected_node;
		bool_e all_node_try;
	}Position;

	static Position pos = {
		{{600, 450},	{700, 750},		{600, 1050},	{600, 1950},	{600, 2250},	{600, 2550}},
		{0,				0,				0,				0,				0,				0},
		{FALSE,			FALSE,			FALSE,			FALSE,			FALSE,			FALSE},
		{RED_MAMMOTH,	RED_MAMMOTH,	RED_MAMMOTH,	YELLOW_MAMMOTH,	YELLOW_MAMMOTH,	YELLOW_MAMMOTH},
		0,
		FALSE
	};

	Uint8 i;
	Sint16 forced_angle;

	if(global.env.match_time >= TIME_MATCH_TO_NET_ROTATE && state != DONE && state != PLACEMENT_TETA && state != STOP_ALL)
		state = STOP_ALL;

	switch(state){
		case INIT :
			STACKS_flush_all();
			QUEUE_reset_all();
			BUZZER_play(1000, DEFAULT_NOTE, 1);
			state = STOP_ROBOT;
			break;

		case STOP_ROBOT :
			if(entrance){} // Retrait des actionneurs
			state = try_stop(STOP_ROBOT, INIT_PATH, INIT_PATH);
			break;

		case INIT_PATH :
			for(i=0;i<NB_NODE;i++)
				pos.dist_with_node[i] = PATHFIND_manhattan_dist(global.env.pos.x, global.env.pos.y,
																pos.node[i].x,
																pos.node[i].y);
			state = FOUND_PATH;
			break;

		case FOUND_PATH :
			pos.all_node_try = TRUE;
			for(i=0;i<NB_NODE;i++)
				if(pos.tryed_node[i] == FALSE){
					pos.selected_node = i;
					pos.all_node_try = FALSE;
				}

			if(pos.all_node_try == FALSE){
				for(i=0;i<NB_NODE;i++)
					if((pos.dist_with_node[i] < pos.dist_with_node[pos.selected_node])
							&& pos.tryed_node[i] == FALSE)
						pos.selected_node = i;
				if((est_dans_carre(250, 2000, 100, 1350, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) && pos.focused_mammoth[pos.selected_node] == RED_MAMMOTH)
								  ||
				   (est_dans_carre(250, 2000, 2900, 1750, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) && pos.focused_mammoth[pos.selected_node] == YELLOW_MAMMOTH))
					state = PLACEMENT;
				else
					state = GET_IN;
			}else
				state = NO_PATH;

			break;

		case GET_IN :
			if(pos.focused_mammoth[pos.selected_node] == RED_MAMMOTH)
				state = PATHFIND_try_going(B1, GET_IN, PLACEMENT, INIT_PATH, ANY_WAY, FAST, NO_DODGE_AND_WAIT, END_AT_BREAK);
			else
				state = PATHFIND_try_going(Y1, GET_IN, PLACEMENT, INIT_PATH, ANY_WAY, FAST, NO_DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case PLACEMENT :
			if(entrance){
				if(foe_in_zone(FALSE, pos.node[pos.selected_node].x, pos.node[pos.selected_node].y, TRUE))
					state = INIT_PATH;
			}
			if(state == PLACEMENT)
				state = try_going(pos.node[pos.selected_node].x, pos.node[pos.selected_node].y, PLACEMENT, PLACEMENT_TETA, INIT_PATH, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			if(ON_LEAVING(PLACEMENT))
				pos.tryed_node[pos.selected_node] = TRUE;
			break;

		case STOP_ALL :
			if(entrance){
				BUZZER_play(1000, DEFAULT_NOTE, 1);
				STACKS_flush_all();
				QUEUE_reset_all();
				// Retrait des actionneurs
			}
			state = try_stop(STOP_ALL, PLACEMENT_TETA, PLACEMENT_TETA);
			break;

		case PLACEMENT_TETA :
			if(entrance){
				if(global.env.pos.y >= 1500)
					forced_angle = PI4096/2-(atan2((2350-global.env.pos.y),global.env.pos.x)*4096.);
				else
					forced_angle =  PI4096/2-(atan2((750-global.env.pos.y),global.env.pos.x)*4096.);
			}
			state = try_go_angle(forced_angle, PLACEMENT_TETA, DONE, DONE, FAST);
		break;

		case NO_PATH :
			if(entrance)
				debug_printf("PLACEMENT IMPOSSIBLE\n");
			state = PLACEMENT_TETA;
			break;

		case DONE :
			//Etat puy.
			break;
	}
}


// Ancienne fonction avec les nodes
/*
void strat_placement_net(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PLACEMENT_NET,
		INIT,
		STOP_ROBOT,
		INIT_PATH,
		FOUND_PATH,
		PLACEMENT,
		PLACEMENT_TETA,
		STOP_ALL,
		NO_PATH,
		DONE
	);

	typedef struct{
		pathfind_node_id_t	node[NB_NODE];
		Uint16 dist_with_node[NB_NODE];
		bool_e tryed_node[NB_NODE];
		Uint8 selected_node;
		bool_e all_node_try;
	}Position;

	static Position pos = {
		{A1,		B1,			C1,			Z1,			Y1,			W1},
		{0,			0,			0,			0,			0,			0},
		{FALSE,		FALSE,		FALSE,		FALSE,		FALSE,		FALSE},
		0,
		FALSE
	};

	Uint8 i;
	Sint16 forced_angle;

	if(global.env.match_time >= TIME_MATCH_TO_NET_ROTATE && state != DONE && state != PLACEMENT_TETA && state != STOP_ALL)
		state = STOP_ALL;

	switch(state){
		case INIT :
			STACKS_flush_all();
			QUEUE_reset_all();
			BUZZER_play(1000, DEFAULT_NOTE, 1);
			state = STOP_ROBOT;
			break;

		case STOP_ROBOT :
			if(entrance){} // Retrait des actionneurs
			state = try_stop(STOP_ROBOT, INIT_PATH, INIT_PATH);
			break;

		case INIT_PATH :
			for(i=0;i<NB_NODE;i++)
				pos.dist_with_node[i] = PATHFIND_manhattan_dist(global.env.pos.x, global.env.pos.y,
															PATHFIND_get_node_x(pos.node[i]),
															PATHFIND_get_node_y(pos.node[i]));
			state = FOUND_PATH;
			break;

		case FOUND_PATH :
			pos.all_node_try = TRUE;
			for(i=0;i<NB_NODE;i++)
				if(pos.tryed_node[i] == FALSE){
					pos.selected_node = i;
					pos.all_node_try = FALSE;
				}

			if(pos.all_node_try == FALSE){
				for(i=0;i<NB_NODE;i++)
					if((pos.dist_with_node[i] < pos.dist_with_node[pos.selected_node])
							&& pos.tryed_node[i] == FALSE)
						pos.selected_node = i;
				state = PLACEMENT;
			}else
				state = NO_PATH;

			break;

		case PLACEMENT :
			state = PATHFIND_try_going(pos.node[pos.selected_node], PLACEMENT, PLACEMENT_TETA, FOUND_PATH, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVING(PLACEMENT))
				pos.tryed_node[pos.selected_node] = TRUE;
			break;

		case STOP_ALL :
			if(entrance){
				BUZZER_play(1000, DEFAULT_NOTE, 1);
				STACKS_flush_all();
				QUEUE_reset_all();
				// Retrait des actionneurs
			}
			state = try_stop(STOP_ALL, PLACEMENT_TETA, PLACEMENT_TETA);
			break;

		case PLACEMENT_TETA :
			if(entrance){
				if(global.env.pos.y >= 1500)
					forced_angle = PI4096/2-(atan2((2350-global.env.pos.y),global.env.pos.x)*4096.);
				else
					forced_angle =  PI4096/2-(atan2((750-global.env.pos.y),global.env.pos.x)*4096.);
			}
			state = try_go_angle(forced_angle, PLACEMENT_TETA, DONE, DONE, FAST);
		break;

		case NO_PATH :
			if(entrance)
				debug_printf("PLACEMENT IMPOSSIBLE\n");
			state = PLACEMENT_TETA;
			break;

		case DONE :
			break;
	}
}*/
