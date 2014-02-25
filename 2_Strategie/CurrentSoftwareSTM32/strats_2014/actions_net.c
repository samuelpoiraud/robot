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
#include "../act_can.h"
#include "../Pathfind.h"


#define DIST_LAUNCH_NET		300
#define Y_MAMMOUTH_RED		750
#define Y_MAMMOUTH_YELLOW	2350
#define NB_NODE				6

void strat_placement_net(){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		FOUND_PATH,
		PLACEMENT,
		PLACEMENT_TETA,
		NO_PATH,
		DONE
	);

	typedef struct{
		pathfind_node_id_t	node[NB_NODE];
		Uint16 dist_with_node[NB_NODE];
		Sint16 teta_for_node[NB_NODE];
		bool_e tryed_node[NB_NODE];
		Uint8 selected_node;
		bool_e all_node_try;
	}Position;

	static Position pos = {
		{A1,		B1,			C1,			Z1,			Y1,			W1},
		{0,			0,			0,			0,			0,			0},
		{11829,		12388,		-11268,		-11550,		12627,		10874},
		{FALSE,		FALSE,		FALSE,		FALSE,		FALSE,		FALSE},
		0,
		FALSE
	};

	Uint8 i;
	CAN_msg_t msg;


	switch(state){
		case INIT :
			ASSER_push_stop();
			// Retrait des actionneurs

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
			state = PATHFIND_try_going(pos.node[pos.selected_node], PLACEMENT, PLACEMENT_TETA, FOUND_PATH, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_LAST_POINT);
			pos.tryed_node[pos.selected_node] = TRUE;
			break;

		case PLACEMENT_TETA :
			state = try_go_angle(pos.teta_for_node[pos.selected_node], PLACEMENT_TETA, DONE, FOUND_PATH, SLOW);
			break;

		case NO_PATH :
			if(entrance)
				debug_printf("PLACEMENT IMPOSSIBLE\n");
			break;

		case DONE :
			break;
	}
}

