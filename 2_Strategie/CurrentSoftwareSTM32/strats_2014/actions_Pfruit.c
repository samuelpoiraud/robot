

#include "actions_Pfruit.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../avoidance.h"
#include "../act_can.h"
#include "../Pathfind.h"

#define ELOIGNEMENT_ARBRE (LARGEUR_LABIUM+117)
#define ELOIGNEMENT_POSE_BAC_FRUIT 500


static GEOMETRY_point_t offset_recalage = {0, 0};

error_e strat_file_fruit(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_BEGINNING,
		POS_END,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[2];

	static GEOMETRY_point_t dplt[2]; // Deplacement
	static way_e sensRobot;
	static Uint16 posOpen; // Position à laquelle, on va ouvrir le bac à fruit

	switch(state){
		case IDLE:
			if(global.env.pos.y > 2225 && global.env.color == RED){ // Va commencer en haut du bac rouge
				dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[0].y = 2700;

				dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[1].y = 1800;

				sensRobot = BACKWARD;
				posOpen = 2600;

			}else if(global.env.color == RED){ // Commence au milieu du terrain en étant Rouge
				dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[0].y = 1500;

				dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[1].y = 2700;

				sensRobot = FORWARD;
				posOpen = 1900;

			}else if(global.env.pos.y > 750){ // Il est de couleur Jaune commence au milieu
				dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[0].y = 1200;

				dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[1].y = 300;

				sensRobot = BACKWARD;
				posOpen = 1100;

			}else{							// Va commencer en bas
				dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[0].y = 300;

				dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
				dplt[1].y = 1200;

				sensRobot = FORWARD;
				posOpen = 400;
			}

			escape_point[0] = dplt[1];
			escape_point[1] = dplt[0];
			#ifdef USE_GET_IN_OUT
				state = GET_IN;
			#else
				state = POS_BEGINNING;
			#endif
			break;

		case GET_IN :
			state = PATHFIND_try_going(PATHFIND_closestNode(dplt[0].x,dplt[0].y, 0x00),
					GET_IN, POS_BEGINNING, ERROR, sensRobot, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case POS_BEGINNING:
			state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,SLOW,sensRobot,DODGE_AND_WAIT);
			break;

		case POS_END:
			if(entrance){
				ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
				ASSER_WARNER_arm_y(posOpen);
			}

			state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,DONE,ERROR,SLOW,sensRobot,NO_DODGE_AND_NO_WAIT);

			if(global.env.asser.reach_y){ // Ouvrir le bac à fruit pour les faire tomber et sortir le bras
				ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_OPEN);
			}
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE: // Fermer le bac à fruit et rentrer le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_CLOSE);
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR: // Fermer le bac à fruit et rentrer le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_CLOSE);
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			#ifdef USE_GET_IN_OUT
				state = GET_OUT_WITH_ERROR;
			#else
				state = IDLE;
				return NOT_HANDLED;
			#endif
			break;

		case ERROR_WITH_GET_OUT :
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_CLOSE);
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre1_double(tree_way_e sens){ //Commence côté mammouth si sens == TRIGO
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_LABIUM,
		COURBE,
		TREE_2,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static const Uint8 NBPOINT = 5;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[3];

	displacement_t point[5];
	static displacement_t courbe[5];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
		case IDLE:
			strat_fruit_sucess = FALSE;

			point[0] = (displacement_t){{1000+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		SLOW};
			point[1] = (displacement_t){{1500+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		SLOW};
			point[2] = (displacement_t){{1580+offset_recalage.x,					350+offset_recalage.y},						SLOW};
			point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	480+offset_recalage.y},						SLOW};
			point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	900+offset_recalage.y},						SLOW};
			/*if(global.env.color == RED){ // Dans ce sens là, fonctionne plutôt bien
				point[0] = (displacement_t){{1000,ELOIGNEMENT_ARBRE-10},SLOW};
				point[1] = (displacement_t){{1500,ELOIGNEMENT_ARBRE},SLOW};
				point[2] = (displacement_t){{1580,350},FAST};
				point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,480},SLOW};
				point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,900},SLOW};
			}else{ // Jaune
				point[0] = (displacement_t){{1000,ELOIGNEMENT_ARBRE-10},SLOW};
				point[1] = (displacement_t){{1500,ELOIGNEMENT_ARBRE},SLOW};
				point[2] = (displacement_t){{1580,350},FAST};
				point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,480},SLOW};
				point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,900},SLOW};
			}*/

			for(i=0;i<NBPOINT;i++){
				if(sens == TRIGO)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};
			escape_point[1] = (GEOMETRY_point_t) {courbe[4].point.x, courbe[4].point.y};
			escape_point[2] = (GEOMETRY_point_t) {1600, 400};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = BACKWARD;
			else
				sensRobot = FORWARD;

			#ifdef USE_GET_IN_OUT
				state = GET_IN;
			#else
				state = POS_DEPART;
			#endif
			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(courbe[0].point.x,courbe[0].point.y, 0x00),
					GET_IN, POS_DEPART, ERROR, sensRobot, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,OPEN_LABIUM,ERROR,FAST,sensRobot,DODGE_AND_WAIT);
			break;

		case OPEN_LABIUM :
			if(entrance)
				ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
			#ifdef USE_WAIT_LABIUM
				state = wait_end_labium_order(LABIUM_OPEN, OPEN_LABIUM, COURBE, COURBE);
			#else
				state = COURBE;
			#endif
			break;

		case COURBE:
			state = try_going_multipoint(&courbe[1],2,COURBE,TREE_2,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case TREE_2:
			state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,TREE_2,POS_FIN,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
			break;

		case POS_FIN:
			state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			strat_fruit_sucess = TRUE;
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			#ifdef USE_GET_IN_OUT
				state = GET_OUT_WITH_ERROR;
			#else
				state = IDLE;
				return NOT_HANDLED;
			#endif
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre2_double(tree_way_e sens){ //Commence côté mammouth si sens == HORAIRE
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_LABIUM,
		COURBE,
		TREE_2,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static const Uint8 NBPOINT = 5;

	pathfind_node_id_t point_pathfind;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[3];

	Uint8 i;
	displacement_t point[5];
	static displacement_t courbe[5];
	static way_e sensRobot;

	switch(state){
		case IDLE:

			strat_fruit_sucess = FALSE;

				point[0] = (displacement_t){{1000+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	SLOW};
				point[1] = (displacement_t){{1500+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	SLOW};
				point[2] = (displacement_t){{1620+offset_recalage.x,					2625+offset_recalage.y},					SLOW};
				point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2450+offset_recalage.y},					SLOW};
				point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	1800+offset_recalage.y},					SLOW};

			/*if(sens==HORAIRE){ // GROS BIDOUILLAGE pour avoir les points a la recup
				if(global.env.color == RED){
					courbe[0] = (displacement_t){{1000,3000-ELOIGNEMENT_ARBRE},SLOW};
					courbe[1] = (displacement_t){{1500,3000-ELOIGNEMENT_ARBRE},SLOW};
					courbe[2] = (displacement_t){{1620,2625},FAST};
					courbe[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-20,2450},SLOW};
					courbe[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-20,1800},SLOW};
				}else{
					courbe[0] = (displacement_t){{1000,3000-ELOIGNEMENT_ARBRE},SLOW};
					courbe[1] = (displacement_t){{1500,3000-ELOIGNEMENT_ARBRE},SLOW};
					courbe[2] = (displacement_t){{1620,2625},FAST};
					courbe[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-10,2450},SLOW};
					courbe[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-10,1800},SLOW};
				}
			}else{
				courbe[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,1800},SLOW};
				courbe[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,2450},SLOW};
				courbe[2] = (displacement_t){{1620,2575},FAST};
				courbe[3] = (displacement_t){{1500,3000-ELOIGNEMENT_ARBRE},SLOW};
				courbe[4] = (displacement_t){{1000,3000-ELOIGNEMENT_ARBRE},SLOW};
			}*/

			for(i=0;i<NBPOINT;i++){
				if(sens == HORAIRE)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};
			escape_point[1] = (GEOMETRY_point_t) {courbe[4].point.x, courbe[4].point.y};
			escape_point[2] = (GEOMETRY_point_t) {1600, 2600};

			if(sens == HORAIRE)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;

			#ifdef USE_GET_IN_OUT
				state = GET_IN;
			#else
				state = POS_DEPART;
			#endif
			break;

		case GET_IN:
			if(entrance){
				if(global.env.color == RED && sens==HORAIRE)
					point_pathfind = Z1;
				else if(global.env.color == RED && sens==TRIGO)
					point_pathfind = W3;
				else if(sens==TRIGO)
					point_pathfind = A1;
				else
					point_pathfind = C3;
			}
			state = PATHFIND_try_going(point_pathfind, GET_IN, POS_DEPART, ERROR, sensRobot, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,OPEN_LABIUM,ERROR,FAST,sensRobot,DODGE_AND_WAIT);
			break;

		case OPEN_LABIUM :
			if(entrance)
				ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
			#ifdef USE_WAIT_LABIUM
				state = wait_end_labium_order(LABIUM_OPEN, OPEN_LABIUM, COURBE, COURBE);
			#else
				state = COURBE;
			#endif
			break;

		case COURBE:
			state = try_going_multipoint(&courbe[1],2,COURBE,TREE_2,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case TREE_2:
			state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,TREE_2,POS_FIN,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
			break;

		case POS_FIN:
			state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			strat_fruit_sucess = TRUE;
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			#ifdef USE_GET_IN_OUT
				state = GET_OUT_WITH_ERROR;
			#else
				state = IDLE;
				return NOT_HANDLED;
			#endif
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}
