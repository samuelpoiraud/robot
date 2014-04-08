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
#include "../high_level_strat.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../avoidance.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "../Geometry.h"

#define LARGEUR_LABIUM	250
#define ELOIGNEMENT_ARBRE (LARGEUR_LABIUM+117)
#define ELOIGNEMENT_POSE_BAC_FRUIT 480
#define PROFONDEUR_BAC_FRUIT		300
#define RAYON_MAX_PIERRE			300	//Avec marge de 9cm... (théorique : 212).. Et il faut bien cette marge...

extern GEOMETRY_point_t offset_recalage;

static tree_sucess_e strat_fruit_sucess = NO_TREE;
static tree_sucess_e fruit_group_our = NO_TREE;
static tree_sucess_e fruit_group_adversary = NO_TREE;

// Des qu'un arbre est bien reussi s'est fort probable que nous ayons des fruits dans notre Labium
bool_e presenceFruit = FALSE;

error_e strat_file_fruit(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_FILE_FRUITS,
		IDLE,
		GET_IN,
		GOTO_FIRST_DISPOSE_POINT,
		DO_DISPOSE,
		GET_OUT_WITH_ERROR,
		GOBACK_FIRST_POINT,
		GOBACK_LAST_POINT,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	typedef enum
	{
		LABIUM_CLOSED_VERIN_IN = 0,
		LABIUM_CLOSED_VERIN_OUT,
		LABIUM_OPENED_VERIN_OUT
	}labium_state_e;
	static labium_state_e labium_state = LABIUM_CLOSED_VERIN_IN;


	static displacement_t dplt[3]; // Deplacement
	static way_e sensRobot;
	static way_e firstPointway;
	static Uint16 posOpen; // Position à laquelle, on va ouvrir le bac à fruit
	static Uint16 posClose; // Position à laquelle, on va fermer le bac à fruit
	static Uint16 posOpenVerin;	//Position à laquelle on sort le vérin

	switch(state){
		case IDLE:
			dplt[0].point.x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[0].point.y = COLOR_Y(1800);
			dplt[0].speed = FAST;

			dplt[1].point.x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[1].point.y = COLOR_Y(2200);
			dplt[1].speed = FAST;

			dplt[2].point.x = PROFONDEUR_BAC_FRUIT+RAYON_MAX_PIERRE;
			dplt[2].point.y = COLOR_Y(2400);
			dplt[2].speed = FAST;


			sensRobot = (global.env.color == RED)?BACKWARD:FORWARD;
			if(COLOR_Y(global.env.pos.y) < 1800)
				firstPointway = sensRobot;
			else
				firstPointway = (sensRobot==BACKWARD)?FORWARD:BACKWARD;

			posOpenVerin = COLOR_Y(1850);
			posOpen = COLOR_Y(1900);
			posClose = COLOR_Y(2300);

			labium_state = LABIUM_CLOSED_VERIN_IN;

			//Zone d'acceptation
			if(		est_dans_carre(0, 1000, COLOR_Y(1600), COLOR_Y(3000), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})
				|| 	est_dans_carre(250, 700, COLOR_Y(0), COLOR_Y(1700), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = GOTO_FIRST_DISPOSE_POINT;
			else
				state = GET_IN;

			break;

		case GET_IN :
			//Le point de départ est : M1...
			state = PATHFIND_try_going(M1,GET_IN, GOTO_FIRST_DISPOSE_POINT, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case GOTO_FIRST_DISPOSE_POINT:
			state = try_going_until_break(dplt[0].point.x, dplt[0].point.y, GOTO_FIRST_DISPOSE_POINT, DO_DISPOSE, ERROR, FAST, firstPointway, NO_DODGE_AND_WAIT);
			break;
		case DO_DISPOSE:
			if(entrance)
			{
				ASSER_WARNER_arm_y(posOpenVerin);
			}
			switch(labium_state)
			{
				case LABIUM_CLOSED_VERIN_IN:
					if(global.env.asser.reach_y)
					{
						ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);
						ASSER_WARNER_arm_y(posOpen);
						labium_state = LABIUM_CLOSED_VERIN_OUT;
					}
					break;
				case LABIUM_CLOSED_VERIN_OUT:
					if(global.env.asser.reach_y)
					{
						ACT_fruit_mouth_goto(ACT_FRUIT_Labium_Open);
						ASSER_WARNER_arm_y(posClose);
						labium_state = LABIUM_OPENED_VERIN_OUT;
					}
					break;
				case LABIUM_OPENED_VERIN_OUT:
					if(global.env.asser.reach_y)
					{
						presenceFruit = FALSE;			//Dès cet instant, on a plus de fruits... (même si on termine pas la trajectoire, on reviendra pas les déposer !)
						ACT_fruit_mouth_goto(ACT_FRUIT_Labium_Close);
						ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
						labium_state = LABIUM_CLOSED_VERIN_IN;
					}
					break;
				default:
					break;
			}
			state = try_going_multipoint(&dplt[1], 2, DO_DISPOSE, DONE , ERROR, sensRobot, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR: // Fermer le bac à fruit et rentrer le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_Labium_Close);
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = GET_OUT_WITH_ERROR;
			break;


		case GET_OUT_WITH_ERROR :
			//On recherche le point de sortie le plus proche.
			if(global.env.pos.x > 530)	//Le point actuel est acceptable comme sortie
				state = RETURN_NOT_HANDLED;
			else
			{
				if(absolute(global.env.pos.y - 1500) < 300)	//Je suis a moins de 300 de l'axe de symétrie du terrain
					state = RETURN_NOT_HANDLED;		//Je peux rendre la main
				else
					state = GOBACK_FIRST_POINT;		//Je suis trop proche du bac pour pouvoir rendre la main
			}
		break;
		case GOBACK_FIRST_POINT:
			state = try_going(dplt[0].point.x,dplt[0].point.y,GOBACK_FIRST_POINT,RETURN_NOT_HANDLED,GOBACK_LAST_POINT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GOBACK_LAST_POINT:
			state = try_going(dplt[2].point.x,dplt[2].point.y,GOBACK_LAST_POINT,RETURN_NOT_HANDLED,GOBACK_FIRST_POINT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case DONE:
			state = IDLE;
			set_sub_act_done(SUB_DROP_FRUITS,TRUE);
			set_sub_act_enable(SUB_DROP_FRUITS, FALSE);
			return END_OK;
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


error_e manage_fruit(tree_group_e group, tree_choice_e choiceTree,tree_way_e parmSens){ //Commence côté mammouth si sens == TREE_TRIGO
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_MANAGE_FRUIT,
		IDLE,
		RECUP_TREE_1,
		RECUP_TREE_1_SIMPLE,
		RECUP_TREE_2,
		RECUP_TREE_2_SIMPLE,
		DONE,
		ERROR
	);

	static tree_choice_e tree;
	static tree_way_e sens;

	switch(state){

		case IDLE:

			//----------------------------------------------------- Rouge
			if(global.env.color == RED){
				if(choiceTree == CHOICE_TREE_1){
					tree = CHOICE_TREE_1;
					if(group == TREE_OUR)
						state = RECUP_TREE_1_SIMPLE;
					else
						state = RECUP_TREE_2_SIMPLE;
				}else if(choiceTree == CHOICE_TREE_2){
					tree = CHOICE_TREE_2;
					if(group == TREE_OUR)
						state = RECUP_TREE_1_SIMPLE;
					else
						state = RECUP_TREE_2_SIMPLE;
				}else if(group == TREE_OUR){
					if(fruit_group_our == NO_TREE)
						state = RECUP_TREE_1;
					else if(fruit_group_our == TREE_1){
						tree = CHOICE_TREE_2;
						state = RECUP_TREE_1_SIMPLE;
					}else if(fruit_group_our == TREE_2){
						tree = CHOICE_TREE_1;
						state = RECUP_TREE_1_SIMPLE;
					}else
						state = ERROR;
				}else{
					 if(fruit_group_adversary == NO_TREE)
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
			//----------------------------------------------------- Jaune
			}else{
				if(choiceTree == CHOICE_TREE_1){
					tree = CHOICE_TREE_1;
					if(group == TREE_OUR)
						state = RECUP_TREE_2_SIMPLE;
					else
						state = RECUP_TREE_1_SIMPLE;
				}else if(choiceTree == CHOICE_TREE_2){
					tree = CHOICE_TREE_2;
					if(group == TREE_OUR)
						state = RECUP_TREE_2_SIMPLE;
					else
						state = RECUP_TREE_1_SIMPLE;
				}else if(group == TREE_OUR){
					if(fruit_group_our == NO_TREE)
						state = RECUP_TREE_2;
					else if(fruit_group_our == TREE_1){
						tree = CHOICE_TREE_2;
						state = RECUP_TREE_2_SIMPLE;
					}else if(fruit_group_our == TREE_2){
						tree = CHOICE_TREE_1;
						state = RECUP_TREE_2_SIMPLE;
					}else
						state = ERROR;
				}else{
					if(fruit_group_adversary == NO_TREE)
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

			if(parmSens == WAY_CHOICE){ // On laisse la strat choisir le sens
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
			}else
				sens = parmSens;

			break;

		case RECUP_TREE_1:
			state = check_sub_action_result(strat_ramasser_fruit_arbre1_double(sens),RECUP_TREE_1,DONE,ERROR);
			break;

		case RECUP_TREE_1_SIMPLE:
			state = check_sub_action_result(strat_ramasser_fruit_arbre1_simple(tree,sens),RECUP_TREE_1_SIMPLE,DONE,ERROR);
			break;

		case RECUP_TREE_2:
			state = check_sub_action_result(strat_ramasser_fruit_arbre2_double(sens),RECUP_TREE_2,DONE,ERROR);
			break;

		case RECUP_TREE_2_SIMPLE:
			state = check_sub_action_result(strat_ramasser_fruit_arbre2_simple(tree,sens),RECUP_TREE_2_SIMPLE,DONE,ERROR);
			break;

		case DONE:
			if(group == TREE_OUR){
				if((fruit_group_our == TREE_1  && strat_fruit_sucess == TREE_2) || (fruit_group_our == TREE_2 && strat_fruit_sucess == TREE_1) || strat_fruit_sucess == ALL_TREE){
					fruit_group_our = ALL_TREE;
					set_sub_act_done(SUB_FRUITS,TRUE);
				}else
					fruit_group_our = strat_fruit_sucess;

			}else{
				if((fruit_group_adversary == TREE_1  && strat_fruit_sucess == TREE_2) || (fruit_group_adversary == TREE_2 && strat_fruit_sucess == TREE_1) || strat_fruit_sucess == ALL_TREE){
					fruit_group_adversary = ALL_TREE;
					set_sub_act_done(SUB_FRUITS_ADV,TRUE);
				}else
					fruit_group_adversary = strat_fruit_sucess;
			}

			if(get_presenceFruit() == TRUE){
				set_sub_act_done(SUB_DROP_FRUITS,FALSE);
				set_sub_act_enable(SUB_DROP_FRUITS, TRUE);
			}

			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			if(get_presenceFruit() == TRUE){  // Meme si une strat ramasse de fruit a ete mise en erreur, cette derniere a peut etre ramasser des fruits
				set_sub_act_done(SUB_DROP_FRUITS,FALSE);
				set_sub_act_enable(SUB_DROP_FRUITS, TRUE);
			}

			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre1_double(tree_way_e sens){ //Commence côté mammouth si sens == TREE_TRIGO
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_TAKE_TREE1_DOUBLE,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_FRUIT_VERIN,
		RECUP_TREE_1,
		COURBE,
		RECUP_TREE_2,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
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
			strat_fruit_sucess = NO_TREE;

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
			escape_point[2] = (GEOMETRY_point_t) {1250, 750};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;

			if(est_dans_carre(400, 2000, 1700, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = OPEN_FRUIT_VERIN;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
				else
					state = POS_DEPART;				//On se rend à la première position directement
			}
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(courbe[0].point.x,courbe[0].point.y, 0x00),
					GET_IN, POS_DEPART, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x, courbe[0].point.y, POS_DEPART, OPEN_FRUIT_VERIN, RETURN_NOT_HANDLED, FAST, sensRobot, NO_DODGE_AND_WAIT);
			break;

		case OPEN_FRUIT_VERIN :
			state = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, RECUP_TREE_1, RECUP_TREE_1);
			break;

		case RECUP_TREE_1:
			state = try_going_until_break(courbe[1].point.x,courbe[1].point.y,RECUP_TREE_1,COURBE,ERROR,courbe[1].speed, sensRobot,NO_DODGE_AND_WAIT);
			break;

		case COURBE:
			if(entrance){
				presenceFruit = TRUE;

				if(sens == TRIGO)
					strat_fruit_sucess = TREE_1;
				else
					strat_fruit_sucess = TREE_2;
				if(main_strategie_used == TRUE){
					set_sub_act_done(SUB_DROP_FRUITS, FALSE);
					set_sub_act_enable(SUB_DROP_FRUITS, TRUE);
				}
			}

				// multipoint si rajoutes des points pour la courbes plus simples
			state = try_going_multipoint(&courbe[2],1,COURBE,RECUP_TREE_2,ERROR,sensRobot,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case RECUP_TREE_2:
			state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,RECUP_TREE_2,POS_FIN,ERROR,courbe[NBPOINT-2].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case POS_FIN:
			state = try_going_until_break(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,courbe[NBPOINT-1].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,RETURN_NOT_HANDLED,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			presenceFruit = TRUE;
			strat_fruit_sucess = ALL_TREE;
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = GET_OUT_WITH_ERROR;
			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre2_double(tree_way_e sens){ //Commence côté mammouth si sens == TREE_HORAIRE
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_TAKE_TREE2_DOUBLE,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_FRUIT_VERIN,
		RECUP_TREE_1,
		COURBE,
		RECUP_TREE_2,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
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
			strat_fruit_sucess = NO_TREE;

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
			escape_point[2] = (GEOMETRY_point_t) {1250, 2250};

			if(sens == HORAIRE)  // Modifie le sens
				sensRobot = BACKWARD;
			else
				sensRobot = FORWARD;

			if(est_dans_carre(400, 2000, 1750, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = OPEN_FRUIT_VERIN;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
				else
					state = POS_DEPART;				//On se rend à la première position directement
			}
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
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
			state = PATHFIND_try_going(point_pathfind, GET_IN, POS_DEPART, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,OPEN_FRUIT_VERIN,ERROR,FAST,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case OPEN_FRUIT_VERIN :
			state = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, RECUP_TREE_1, RECUP_TREE_1);
			break;

		case RECUP_TREE_1:
			state = try_going_until_break(courbe[1].point.x,courbe[1].point.y,RECUP_TREE_1,COURBE,ERROR,courbe[1].speed, sensRobot,NO_DODGE_AND_WAIT);
			break;

		case COURBE:
			if(entrance){
				presenceFruit = TRUE;

				if(sens == TRIGO)
					strat_fruit_sucess = TREE_2;
				else
					strat_fruit_sucess = TREE_1;
				if(main_strategie_used == TRUE){
					set_sub_act_done(SUB_DROP_FRUITS, FALSE);
					set_sub_act_enable(SUB_DROP_FRUITS, TRUE);
				}
			}

			state = try_going_multipoint(&courbe[2],1,COURBE,RECUP_TREE_2,ERROR,sensRobot,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case RECUP_TREE_2:
			state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,RECUP_TREE_2,POS_FIN,ERROR,courbe[NBPOINT-2].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case POS_FIN:
			state = try_going_until_break(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,courbe[NBPOINT-1].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,RETURN_NOT_HANDLED,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			presenceFruit = TRUE;
			strat_fruit_sucess = ALL_TREE;
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = GET_OUT_WITH_ERROR;
			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre1_simple(tree_choice_e tree, tree_way_e sens){ //Commence côté mammouth si sens == TREE_TRIGO
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_TAKE_TREE1_SIMPLE,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_FRUIT_VERIN,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
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
			strat_fruit_sucess = NO_TREE;

			if( tree == CHOICE_TREE_1){
				point[0] = (displacement_t){{1000+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		FAST};
				point[1] = (displacement_t){{1500+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		FAST};
			}else{
				point[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	450+offset_recalage.y},						FAST};
				point[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	900+offset_recalage.y},						FAST};
			}


			for(i=0;i<NBPOINT;i++){
				if(sens == TRIGO)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};
			escape_point[1] = (GEOMETRY_point_t) {courbe[NBPOINT-1].point.x, courbe[NBPOINT-1].point.y};
			escape_point[2] = (GEOMETRY_point_t) {1250, 750};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;




			if(est_dans_carre(400, 2000, 0, 1300, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = OPEN_FRUIT_VERIN;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
				else
					state = POS_DEPART;				//On se rend à la première position directement
			}
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(courbe[0].point.x,courbe[0].point.y, 0x00),
					GET_IN, POS_DEPART, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,OPEN_FRUIT_VERIN,RETURN_NOT_HANDLED,courbe[0].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case OPEN_FRUIT_VERIN :
			state = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, POS_FIN, POS_FIN);
			break;

		case POS_FIN:
			state = try_going_until_break(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,courbe[NBPOINT-1].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,RETURN_NOT_HANDLED,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			presenceFruit = TRUE;

			if(tree == CHOICE_TREE_1)
				strat_fruit_sucess = TREE_1;
			else
				strat_fruit_sucess = TREE_2;

			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = GET_OUT_WITH_ERROR;
			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre2_simple(tree_choice_e tree, tree_way_e sens){ //Commence côté mammouth si sens == TREE_TRIGO
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_TAKE_TREE2_SIMPLE,
		IDLE,
		GET_IN,
		POS_DEPART,
		OPEN_FRUIT_VERIN,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
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
			strat_fruit_sucess = NO_TREE;

			if( tree == CHOICE_TREE_1){
				point[0] = (displacement_t){{1000+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	FAST};
				point[1] = (displacement_t){{1500+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	FAST};
			}else{
				point[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2450+offset_recalage.y},					FAST};
				point[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2000+offset_recalage.y},					FAST};
			}


			for(i=0;i<NBPOINT;i++){
				if(sens == HORAIRE)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};
			escape_point[1] = (GEOMETRY_point_t) {courbe[NBPOINT-1].point.x, courbe[NBPOINT-1].point.y};
			escape_point[2] = (GEOMETRY_point_t) {1250, 2250};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;

			if(est_dans_carre(400, 2000, 1800, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = OPEN_FRUIT_VERIN;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
				else
					state = POS_DEPART;				//On se rend à la première position directement
			}
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(courbe[0].point.x,courbe[0].point.y, 0x00),
					GET_IN, POS_DEPART, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,OPEN_FRUIT_VERIN,RETURN_NOT_HANDLED,courbe[0].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case OPEN_FRUIT_VERIN :
			state = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, POS_FIN, POS_FIN);
			break;

		case POS_FIN:
			state = try_going_until_break(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,courbe[NBPOINT-1].speed,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,RETURN_NOT_HANDLED,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			presenceFruit = TRUE;

			if(tree == CHOICE_TREE_1)
				strat_fruit_sucess = TREE_1;
			else
				strat_fruit_sucess = TREE_2;

			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			state = GET_OUT_WITH_ERROR;
			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

bool_e get_presenceFruit(){
	return presenceFruit;
}

tree_sucess_e FRUIT_sucess_tree(tree_group_e group){
	if(group == TREE_OUR)
		return fruit_group_our;

	return fruit_group_adversary;
}
