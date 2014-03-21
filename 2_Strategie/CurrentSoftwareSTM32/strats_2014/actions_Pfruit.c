/*
 *	Club Robot ESEO 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_Pfruit.h
 *	Package : Carte S²/strats2014
 *	Description : Tests des actions pour rammasser les fruits
 *	Auteur : Anthony, modifié par .
 *	Version 2014/16/03
 */

#include "actions_Pfruit.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../avoidance.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "../Geometry.h"


// Define à activer pour activer les attentes de l'ouverture des verrins
#define USE_WAIT_LABIUM


#define LARGEUR_LABIUM	250
#define ELOIGNEMENT_ARBRE (LARGEUR_LABIUM+117)
#define ELOIGNEMENT_POSE_BAC_FRUIT 500

extern GEOMETRY_point_t offset_recalage;




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

			if((global.env.color == RED && est_dans_carre((GEOMETRY_point_t){0, 1500}, (GEOMETRY_point_t){1000,3000}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					|| (global.env.color == YELLOW && est_dans_carre((GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){1000, 1500}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = GET_IN;
			else
				state = POS_BEGINNING;

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
			state = GET_OUT_WITH_ERROR;
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


error_e manage_fruit(tree_group_e group, tree_way_e sens){ //Commence côté mammouth si sens == TRIGO
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		RECUP_TREE_1,
		RECUP_TREE_1_SIMPLE,
		RECUP_TREE_2,
		RECUP_TREE_2_SIMPLE,
		DONE,
		ERROR
	);

	static tree_choice_e tree;
	static tree_sucess_e fruit_group_our = NO_ONE;
	static tree_sucess_e fruit_group_adversary = NO_ONE;

	switch(state){
		case IDLE:

			if(global.env.color == RED){
				if(group == TREE_OUR){
					if(fruit_group_our == NO_ONE)
						state = RECUP_TREE_1;
					else if(fruit_group_our == TREE_1){
						tree = CHOICE_TREE_2;
						state = RECUP_TREE_1_SIMPLE;
					}else if(fruit_group_our == TREE_2){
						tree = CHOICE_TREE_1;
						state = RECUP_TREE_1_SIMPLE;
					}else
						state = ERROR;
				}else{ // Arbre adeverse, ici ceux côté jaune
					if(fruit_group_adversary == NO_ONE)
						state = RECUP_TREE_2;
					else if(fruit_group_adversary == TREE_1){
						tree = CHOICE_TREE_2;
						state = RECUP_TREE_2_SIMPLE;
					}else if(fruit_group_adversary == TREE_2){
						tree = CHOICE_TREE_1;
						state = RECUP_TREE_2_SIMPLE;
					}else
						state = ERROR;
				}
			}else{
				if(group == TREE_OUR){
					if(fruit_group_our == NO_ONE)
						state = RECUP_TREE_2;
					else if(fruit_group_our == TREE_1){
						tree = CHOICE_TREE_2;
						state = RECUP_TREE_2_SIMPLE;
					}else if(fruit_group_our == TREE_2){
						tree = CHOICE_TREE_1;
						state = RECUP_TREE_2_SIMPLE;
					}else
						state = ERROR;
				}else{ // Arbre adeverse, ici ceux côté rouge
					if(fruit_group_adversary == NO_ONE)
						state = RECUP_TREE_1;
					else if(fruit_group_adversary == TREE_1){
						tree = CHOICE_TREE_2;
						state = RECUP_TREE_1_SIMPLE;
					}else if(fruit_group_adversary == TREE_2){
						tree = CHOICE_TREE_1;
						state = RECUP_TREE_1_SIMPLE;
					}else
						state = ERROR;
				}
			}

			if(sens == CHOICE){ // On laisse la strat choisir le sens
				if(state == RECUP_TREE_1)
					sens = ((min_node_dist(A1,C3) == A1)?TRIGO : HORAIRE);

				else if(state == RECUP_TREE_1_SIMPLE && tree == CHOICE_TREE_1)
					sens = ((min_node_dist(A1,A2) == A1)?TRIGO : HORAIRE);

				else if(state == RECUP_TREE_1_SIMPLE && tree == CHOICE_TREE_2)
					sens = ((min_node_dist(B3,C3) == B3)?TRIGO : HORAIRE);

				else if(state == RECUP_TREE_2)
					sens = ((min_node_dist(W3,Z1) == W3)?TRIGO : HORAIRE);

				else if(state == RECUP_TREE_2_SIMPLE && tree == CHOICE_TREE_1)
					sens = ((min_node_dist(W3,Y3) == W3)?TRIGO : HORAIRE);

				else if(state == RECUP_TREE_2_SIMPLE && tree == CHOICE_TREE_2)
					sens = ((min_node_dist(Z2,Z1) == Z1)?TRIGO : HORAIRE);

				else
					sens = TRIGO; // Impose un sens par defaut si n'a pas trouver de choix
			}

			break;

		case RECUP_TREE_1:
			state = check_sub_action_result(strat_ramasser_fruit_arbre1_double(sens),RECUP_TREE_1,DONE,ERROR);

			if(global.env.color == RED)
				fruit_group_our = strat_fruit_sucess;
			else
				fruit_group_adversary = strat_fruit_sucess;

			break;

		case RECUP_TREE_1_SIMPLE:
			state = check_sub_action_result(strat_ramasser_fruit_arbre1_simple(tree,sens),RECUP_TREE_1_SIMPLE,DONE,ERROR);

			if(global.env.color == RED)
				fruit_group_our = strat_fruit_sucess;
			else
				fruit_group_adversary = strat_fruit_sucess;

			break;

		case RECUP_TREE_2:
			state = check_sub_action_result(strat_ramasser_fruit_arbre2_double(sens),RECUP_TREE_2,DONE,ERROR);

			if(global.env.color == RED)
				fruit_group_adversary = strat_fruit_sucess;
			else
				fruit_group_our = strat_fruit_sucess;

			break;

		case RECUP_TREE_2_SIMPLE:
			state = check_sub_action_result(strat_ramasser_fruit_arbre2_simple(tree,sens),RECUP_TREE_2_SIMPLE,DONE,ERROR);

			if(global.env.color == RED)
				fruit_group_adversary = strat_fruit_sucess;
			else
				fruit_group_our = strat_fruit_sucess;

			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
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
		RECUP_TREE_1,
		COURBE,
		RECUP_TREE_2,
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
			strat_fruit_sucess = NO_ONE;

			point[0] = (displacement_t){{1000+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		SLOW};
			point[1] = (displacement_t){{1500+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		SLOW};
			point[2] = (displacement_t){{1580+offset_recalage.x,					350+offset_recalage.y},						SLOW};
			point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	480+offset_recalage.y},						SLOW};
			point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	900+offset_recalage.y},						SLOW};

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

			if((global.env.color == RED && est_dans_carre((GEOMETRY_point_t){600, 0}, (GEOMETRY_point_t){2000, 1300}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					|| (global.env.color == YELLOW && est_dans_carre((GEOMETRY_point_t){600, 1800}, (GEOMETRY_point_t){2000, 3000}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = GET_IN;
			else
				state = POS_DEPART;
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
				state = wait_end_labium_order(LABIUM_OPEN, OPEN_LABIUM, RECUP_TREE_1, RECUP_TREE_1);
			#else
				state = RECUP_TREE_1;
			#endif
			break;

		case RECUP_TREE_1:
			state = try_going_until_break(courbe[1].point.x,courbe[1].point.y,RECUP_TREE_1,COURBE,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COURBE:
			if(entrance){
				if(sens == TRIGO)
					strat_fruit_sucess = TREE_1;
				else
					strat_fruit_sucess = TREE_2;
			}

				// multipoint si rajoutes des points pour la courbes plus simples
			state = try_going_multipoint(&courbe[2],1,COURBE,RECUP_TREE_2,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case RECUP_TREE_2:
			state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,RECUP_TREE_2,POS_FIN,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
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
			strat_fruit_sucess = ALL_TREE;
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = GET_OUT_WITH_ERROR;
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
		RECUP_TREE_1,
		COURBE,
		RECUP_TREE_2,
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

			strat_fruit_sucess = NO_ONE;

				point[0] = (displacement_t){{1000+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	SLOW};
				point[1] = (displacement_t){{1500+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	SLOW};
				point[2] = (displacement_t){{1620+offset_recalage.x,					2625+offset_recalage.y},					SLOW};
				point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2450+offset_recalage.y},					SLOW};
				point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	1800+offset_recalage.y},					SLOW};

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

			if((global.env.color == RED && est_dans_carre((GEOMETRY_point_t){600, 1800}, (GEOMETRY_point_t){2000, 3000}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					|| (global.env.color == YELLOW && est_dans_carre((GEOMETRY_point_t){600, 0}, (GEOMETRY_point_t){2000, 1300}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = GET_IN;
			else
				state = POS_DEPART;
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
				state = wait_end_labium_order(LABIUM_OPEN, OPEN_LABIUM, RECUP_TREE_1, RECUP_TREE_1);
			#else
				state = RECUP_TREE_1;
			#endif
			break;

		case RECUP_TREE_1:
			state = try_going_until_break(courbe[1].point.x,courbe[1].point.y,RECUP_TREE_1,COURBE,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COURBE:
			if(entrance){
				if(sens == TRIGO)
					strat_fruit_sucess = TREE_2;
				else
					strat_fruit_sucess = TREE_1;
			}

			state = try_going_multipoint(&courbe[2],1,COURBE,RECUP_TREE_2,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case RECUP_TREE_2:
			state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,RECUP_TREE_2,POS_FIN,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
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
			strat_fruit_sucess = ALL_TREE;
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = GET_OUT_WITH_ERROR;
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

error_e strat_ramasser_fruit_arbre1_simple(tree_choice_e tree, tree_way_e sens){ //Commence côté mammouth si sens == TRIGO
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_LABIUM,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static const Uint8 NBPOINT = 2;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[3];

	displacement_t point[2];
	static displacement_t courbe[2];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
		case IDLE:
			strat_fruit_sucess = NO_ONE;

			if( tree == CHOICE_TREE_1){
				point[0] = (displacement_t){{1000+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		SLOW};
				point[1] = (displacement_t){{1500+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		SLOW};
			}else{
				point[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	450+offset_recalage.y},						SLOW};
				point[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	900+offset_recalage.y},						SLOW};
			}


			for(i=0;i<NBPOINT;i++){
				if(sens == TRIGO)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};
			escape_point[1] = (GEOMETRY_point_t) {courbe[NBPOINT-1].point.x, courbe[NBPOINT-1].point.y};
			escape_point[2] = (GEOMETRY_point_t) {1600, 400};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = BACKWARD;
			else
				sensRobot = FORWARD;

			if((global.env.color == RED && est_dans_carre((GEOMETRY_point_t){600, 0}, (GEOMETRY_point_t){2000, 1300}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					|| (global.env.color == YELLOW && est_dans_carre((GEOMETRY_point_t){600, 1800}, (GEOMETRY_point_t){2000, 3000}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = GET_IN;
			else
				state = POS_DEPART;
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
				state = wait_end_labium_order(LABIUM_OPEN, OPEN_LABIUM, POS_FIN, POS_FIN);
			#else
				state = POS_FIN;
			#endif
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
			if( tree == CHOICE_TREE_1)
				strat_fruit_sucess = TREE_1;
			else
				strat_fruit_sucess = TREE_2;

			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = GET_OUT_WITH_ERROR;
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

error_e strat_ramasser_fruit_arbre2_simple(tree_choice_e tree, tree_way_e sens){ //Commence côté mammouth si sens == TRIGO
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_LABIUM,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static const Uint8 NBPOINT = 2;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[3];

	displacement_t point[2];
	static displacement_t courbe[2];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
		case IDLE:
			strat_fruit_sucess = NO_ONE;

			if( tree == CHOICE_TREE_1){
				point[0] = (displacement_t){{1000+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	SLOW};
				point[1] = (displacement_t){{1500+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	SLOW};
			}else{
				point[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2450+offset_recalage.y},					SLOW};
				point[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2000+offset_recalage.y},					SLOW};
			}


			for(i=0;i<NBPOINT;i++){
				if(sens == HORAIRE)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};
			escape_point[1] = (GEOMETRY_point_t) {courbe[NBPOINT-1].point.x, courbe[NBPOINT-1].point.y};
			escape_point[2] = (GEOMETRY_point_t) {1600, 2600};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = BACKWARD;
			else
				sensRobot = FORWARD;

			if((global.env.color == RED && est_dans_carre((GEOMETRY_point_t){600, 1800}, (GEOMETRY_point_t){2000, 3000}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					|| (global.env.color == YELLOW && est_dans_carre((GEOMETRY_point_t){600, 0}, (GEOMETRY_point_t){2000, 1300}, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = GET_IN;
			else
				state = POS_DEPART;
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
				state = wait_end_labium_order(LABIUM_OPEN, OPEN_LABIUM, POS_FIN, POS_FIN);
			#else
				state = POS_FIN;
			#endif
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
			if( tree == CHOICE_TREE_1)
				strat_fruit_sucess = TREE_1;
			else
				strat_fruit_sucess = TREE_2;

			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			state = GET_OUT_WITH_ERROR;
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
