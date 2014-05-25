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
#include "../Supervision/SD/SD.h"
#include "../QS/QS_watchdog.h"


#define LARGEUR_LABIUM	200
#define ELOIGNEMENT_ARBRE (LARGEUR_LABIUM+122)
#define ELOIGNEMENT_POSE_BAC_FRUIT  500
#define PROFONDEUR_BAC_FRUIT		300
#define RAYON_MAX_PIERRE			310	//Avec marge de 9cm... (théorique : 212).. Et il faut bien cette marge...
#define VITESSE_FRUIT				120
#define TIME_CLOSE_MOUTH			1000 // Quand il fini de prendre les friuts correctement, il attend pour refermer le bras

#define TIME_CONSIDERING_FRUITS_FALL_SINCE_OPENING_LABIUM	500	//[ms]

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
		CORRECT_TRAJECTORY_WHEN_ODOMETRY_IS_NOT_GOOD,
		GET_OUT_WITH_ERROR,
		GOBACK_FIRST_POINT,
		GOBACK_LAST_POINT,
		COME_BACK_FOR_DROPING_NEAR_FRESCO,
		DROP_NEAR_FRESCO,
		DROP_FRUIT,
		GET_OUT_NEAR_FRESCO,
		WAIT_FOR_EXIT,
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
	static time32_t labium_opening_time;
	 // Prend le temps quand nous avons ouvert le labium, si le labium a été ouvert plus d'une seconde
	// cela veut dire que les fruits sont forcément tombé de ce dernier
	static Uint16 last_time_Open_labium;
	static bool_e i_am_trying_an_other_trajectory;
	static time32_t local_time;
	bool_e b;
	avoidance_type_e avoidance;
	GEOMETRY_point_t p;
	p = (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y};
	//Astuce permettant d'activer sur une portion seulement des trajectoires l'extraction DODGE.
	if(is_possible_point_for_rotation(&p))
		avoidance = DODGE_AND_WAIT;
	else
		avoidance = NO_DODGE_AND_WAIT;

	switch(state){
		case IDLE:
			if(presenceFruit == FALSE)
			{
				SD_printf("On a pas de fruit, pas la peine de vider !\n");
				state = DONE;
				break;
			}
			last_time_Open_labium = 0;	//Cette ligne est importante...
			i_am_trying_an_other_trajectory = FALSE;

			dplt[0].point.x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[0].point.y = COLOR_Y(1700);
			dplt[0].speed = FAST;

			dplt[1].point.x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[1].point.y = COLOR_Y(2300);
			dplt[1].speed = SLOW;

			dplt[2].point.x = PROFONDEUR_BAC_FRUIT+RAYON_MAX_PIERRE;
			dplt[2].point.y = COLOR_Y(2500);
			dplt[2].speed = SLOW;


			sensRobot = (global.env.color == RED)?BACKWARD:FORWARD;
			if(COLOR_Y(global.env.pos.y) < 1800)
				firstPointway = sensRobot;
			else
				firstPointway = (sensRobot==BACKWARD)?FORWARD:BACKWARD;

			posOpenVerin = COLOR_Y(1800);
			posOpen = COLOR_Y(1900);
			posClose = COLOR_Y(2300);

			labium_state = LABIUM_CLOSED_VERIN_IN;

			//Zone d'acceptation
			if(		est_dans_carre(0, 1000, COLOR_Y(1600), COLOR_Y(3000), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})
				|| 	est_dans_carre(250, 700, COLOR_Y(0), COLOR_Y(1600), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})
				|| 	est_dans_carre(600, 1000, COLOR_Y(0), COLOR_Y(1000), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))	//Le cas de la récup du feu de start avant de venir ici...
				state = GOTO_FIRST_DISPOSE_POINT;
			else
				state = GET_IN;

			break;

		case GET_IN :
			//Le point de départ est : M1...
			state = PATHFIND_try_going(M0,GET_IN, GOTO_FIRST_DISPOSE_POINT, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case GOTO_FIRST_DISPOSE_POINT:
			switch(goto_pos_curve_with_avoidance(&dplt[0], NULL, 1, firstPointway, avoidance, END_AT_LAST_POINT))
			{
				case FOE_IN_PATH:
					state = ERROR; //On ferme les actionneurs et on vérifie qu'on peut rendre la main...
					break;
				case NOT_HANDLED:
					state = CORRECT_TRAJECTORY_WHEN_ODOMETRY_IS_NOT_GOOD;
					break;
				case END_OK:
				case END_WITH_TIMEOUT:
					state = DO_DISPOSE;
					break;
				default:
					break;
			}
			break;
		case DO_DISPOSE:
			if(entrance)
			{
				//Si l'adversaire est très proche devant nous, on file direct vers le DROP_NEAR_FRESCO.
				//Sinon, on tente de déposer......... si jamais on rencontre l'adversaire ensuite lors de la dépose.. on verra en fonction de presenceFruit.
				b = foe_in_zone(FALSE,500,COLOR_Y(2800),FALSE);
				display(b);
				if(b)
				{
					state = DROP_NEAR_FRESCO;
					break;
				}
				ASSER_WARNER_arm_y(posOpenVerin);
				labium_state = LABIUM_CLOSED_VERIN_IN;
			}
			switch(labium_state)
			{
				case LABIUM_CLOSED_VERIN_IN:
					if(global.env.asser.reach_y)
					{
						ASSER_WARNER_arm_y(posOpen);
						ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Vibration);
						labium_state = LABIUM_CLOSED_VERIN_OUT;
					}
					break;
				case LABIUM_CLOSED_VERIN_OUT:
					if(global.env.asser.reach_y)
					{
						ASSER_WARNER_arm_y(posClose);
						ACT_fruit_labium_goto(ACT_FRUIT_Labium_Open);
						last_time_Open_labium = global.env.match_time;
						labium_state = LABIUM_OPENED_VERIN_OUT;

						labium_opening_time = global.env.match_time;
					}
					break;
				case LABIUM_OPENED_VERIN_OUT:
					if(global.env.asser.reach_y)
					{
						ACT_fruit_labium_goto(ACT_FRUIT_Labium_Close);
						ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
						labium_state = LABIUM_CLOSED_VERIN_IN;
					}
					if(presenceFruit == TRUE && (global.env.asser.reach_y || global.env.match_time > labium_opening_time + TIME_CONSIDERING_FRUITS_FALL_SINCE_OPENING_LABIUM))
					{
						//On considère qu'on a plus de fruits SI on a commencé à fermer, ou si on a ouvert le labium depuis 500ms
						//S'il y a évitement maintenant entre 500ms et notre arrivée au reach_y, on ira pas poser ailleurs, considérant que la dépose est faite.
						presenceFruit = FALSE;			//Dès cet instant, on a plus de fruits... (même si on termine pas la trajectoire, on reviendra pas les déposer !)
						set_sub_act_done(SUB_DROP_FRUITS,TRUE);
						set_sub_act_enable(SUB_DROP_FRUITS, FALSE);
					}
					break;
				default:
					break;
			}



			//NO_DODGE : on gère nous même la sortie...
			switch(goto_pos_curve_with_avoidance(&dplt[1], NULL, 2, firstPointway, NO_DODGE_AND_WAIT, END_AT_LAST_POINT))
			{
				case FOE_IN_PATH:
					if(presenceFruit)	//On a toujours les fruits... on tente de reculer pour déposer à coté des fresques
						state = COME_BACK_FOR_DROPING_NEAR_FRESCO;
					else
						state = ERROR; //On ferme les actionneurs et on vérifie qu'on peut rendre la main...
					break;
				case NOT_HANDLED:
					state = CORRECT_TRAJECTORY_WHEN_ODOMETRY_IS_NOT_GOOD;
					break;
				case END_OK:
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					break;
			}
			break;
		case CORRECT_TRAJECTORY_WHEN_ODOMETRY_IS_NOT_GOOD:
			if(entrance)
			{
				if(i_am_trying_an_other_trajectory)
				{
					state = ERROR;	//Ca ne marche pas non plus, on abandonne
					break;
				}
				//Modification de la trajectoire prévue...
				dplt[0].point.x += 30;	//On ajoute 3cm.
				dplt[1].point.x += 30;	//On ajoute 3cm.
				i_am_trying_an_other_trajectory = TRUE;
			}

			//On revient en M0 pour pouvoir retenter une trajectoire plus éloignée.
			state = try_going(500, 1500, CORRECT_TRAJECTORY_WHEN_ODOMETRY_IS_NOT_GOOD, GOTO_FIRST_DISPOSE_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;
		case COME_BACK_FOR_DROPING_NEAR_FRESCO:		//On a échoué la dépose normale, on revient pour tenter la dépose près des fresques
			state = try_going(dplt[0].point.x, dplt[0].point.y, COME_BACK_FOR_DROPING_NEAR_FRESCO, DROP_NEAR_FRESCO, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;
		case DROP_NEAR_FRESCO:
			//NO_DODGE, on gère la sortie.
			state = try_going(200, dplt[0].point.y, DROP_NEAR_FRESCO, DROP_FRUIT, GET_OUT_NEAR_FRESCO, FAST, (global.env.color == RED)? FORWARD:BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case DROP_FRUIT:{
			static bool_e watchdog = FALSE;
			if(entrance){
				WATCHDOG_create_flag(1500,&watchdog);
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Vibration);
				ACT_fruit_labium_goto(ACT_FRUIT_Labium_Open);
			}

			if(watchdog)
			{
				presenceFruit = FALSE;			//Dès cet instant, on a plus de fruits...
				ACT_fruit_labium_goto(ACT_FRUIT_Labium_Close);
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
				set_sub_act_done(SUB_DROP_FRUITS,TRUE);
				set_sub_act_enable(SUB_DROP_FRUITS, FALSE);
				state = GET_OUT_NEAR_FRESCO;
			}

		}break;

		case GET_OUT_NEAR_FRESCO:
			state = try_going(dplt[0].point.x, dplt[0].point.y, GET_OUT_NEAR_FRESCO, (last_state == DROP_FRUIT)?DONE:ERROR, WAIT_FOR_EXIT, FAST, (global.env.color == RED)? BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			break;
		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de réessayer périodiquement.
			if(entrance)
			{
				local_time = global.env.match_time;
			}
			if(global.env.match_time - local_time > 2000)
				state = GET_OUT_NEAR_FRESCO;
			break;

		case ERROR: // Fermer le bac à fruit et rentrer le bras
			ACT_fruit_labium_goto(ACT_FRUIT_Labium_Close);
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);

			// Si le labium a été ouvert les fruits sont forcément tombée sur le sol ou dans le bac (ce qui sera le cas ^^, j'espére)
			if(last_time_Open_labium != 0 && global.env.match_time > last_time_Open_labium + 1000){
					set_sub_act_done(SUB_DROP_FRUITS,TRUE);
					set_sub_act_enable(SUB_DROP_FRUITS, FALSE);
					presenceFruit = FALSE;
			}

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

			//Inutile en principe.. mais par sécurité...
			ACT_fruit_labium_goto(ACT_FRUIT_Labium_Close);
			ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			#ifdef MODE_SIMULATION
				//Sécurité inutile en principe.... mais sur le robot virtuel, les messages ne vont pas assez vite... donc le warner n'est pas levé à temps.
				//Ceci permet d'éviter de refaire la dépose...
				set_sub_act_done(SUB_DROP_FRUITS,TRUE);
				set_sub_act_enable(SUB_DROP_FRUITS, FALSE);
				presenceFruit = FALSE;
			#endif
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
		ORIENTATION,
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
	static GEOMETRY_point_t escape_point[2];

	displacement_t point[5];
	static displacement_t courbe[5];
	Uint8 i;
	static way_e sensRobot;
	static pathfind_node_id_t point_pathfind;
	switch(state){
		case IDLE:
			strat_fruit_sucess = NO_TREE;

			point[0] = (displacement_t){{1000+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		VITESSE_FRUIT};
			point[1] = (displacement_t){{1500+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		VITESSE_FRUIT};
			point[2] = (displacement_t){{1580+offset_recalage.x,					350+offset_recalage.y},						VITESSE_FRUIT};
			point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	480+offset_recalage.y},						VITESSE_FRUIT};
			point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	1100+offset_recalage.y},						VITESSE_FRUIT};

			for(i=0;i<NBPOINT;i++){
				if(sens == TRIGO)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {1250, 750};
			escape_point[1] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};


			if(sens == TRIGO)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;

			if(est_dans_carre(400, 2000, 0, 1300, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = ORIENTATION;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
				else
					state = POS_DEPART;				//On se rend à la première position directement
			}
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			if(entrance)
			{
				point_pathfind = (sens == HORAIRE)?C3:A1;
			}
			state = PATHFIND_try_going(point_pathfind,GET_IN, POS_DEPART, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x, courbe[0].point.y, POS_DEPART, ORIENTATION, RETURN_NOT_HANDLED, FAST, sensRobot, DODGE_AND_WAIT);
			break;

		case ORIENTATION:
			state = try_go_angle((sens == TRIGO)?0:PI4096/2,ORIENTATION,OPEN_FRUIT_VERIN,RETURN_NOT_HANDLED,FAST);
			break;

		case OPEN_FRUIT_VERIN :
			state = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, RECUP_TREE_1, RECUP_TREE_1);
			break;

		case RECUP_TREE_1:
			//NO_DODGE : on gère nous même la sortie
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
			WATCHDOG_create(TIME_CLOSE_MOUTH, ACT_fruit_mouth_goto_close,FALSE);
			//ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
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
		ORIENTATION,
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

	const Uint8 NBPOINT = 5;

	static pathfind_node_id_t point_pathfind;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[2];

	Uint8 i;
	displacement_t point[5];
	static displacement_t courbe[5];
	static way_e sensRobot;

	switch(state){
		case IDLE:
			strat_fruit_sucess = NO_TREE;

				point[0] = (displacement_t){{1000+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	VITESSE_FRUIT};
				point[1] = (displacement_t){{1500+offset_recalage.x,					3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	VITESSE_FRUIT};
				point[2] = (displacement_t){{1620+offset_recalage.x,					2625+offset_recalage.y},					VITESSE_FRUIT};
				point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2500+offset_recalage.y},					VITESSE_FRUIT};
				point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	1800+offset_recalage.y},					VITESSE_FRUIT};

			for(i=0;i<NBPOINT;i++){
				if(sens == HORAIRE)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
			}

			escape_point[0] = (GEOMETRY_point_t) {1250, 2250};
			escape_point[1] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};


			if(sens == HORAIRE)  // Modifie le sens
				sensRobot = BACKWARD;
			else
				sensRobot = FORWARD;

			if(est_dans_carre(400, 2000, 1750, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = ORIENTATION;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
				else
					state = POS_DEPART;				//On se rend à la première position directement
			}
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			if(entrance){
				if(sens==HORAIRE)
					point_pathfind = Z1;
				else
					point_pathfind = W3;
			}
			state = PATHFIND_try_going(point_pathfind, GET_IN, POS_DEPART, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_DEPART:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,ORIENTATION,RETURN_NOT_HANDLED,FAST,sensRobot,DODGE_AND_WAIT);
			break;

		case ORIENTATION:
			state = try_go_angle((sens == TRIGO)?PI4096/2:PI4096,ORIENTATION,OPEN_FRUIT_VERIN,RETURN_NOT_HANDLED,FAST);
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
			WATCHDOG_create(TIME_CLOSE_MOUTH, ACT_fruit_mouth_goto_close,FALSE);
			//ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
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
		ORIENTATION,
		OPEN_FRUIT_VERIN,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static const Uint8 NBPOINT = 2;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[2];

	displacement_t point[2];
	static displacement_t courbe[2];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
		case IDLE:
			strat_fruit_sucess = NO_TREE;

			if( tree == CHOICE_TREE_1){
				point[0] = (displacement_t){{850+offset_recalage.x,						ELOIGNEMENT_ARBRE+offset_recalage.y},		VITESSE_FRUIT};
				point[1] = (displacement_t){{1600+offset_recalage.x,					ELOIGNEMENT_ARBRE+offset_recalage.y},		VITESSE_FRUIT};
			}else{
				point[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	350+offset_recalage.y},						VITESSE_FRUIT};
				point[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	1000+offset_recalage.y},						VITESSE_FRUIT};
			}


			for(i=0;i<NBPOINT;i++){
				if(sens == TRIGO)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
				courbe[i].speed = FAST;
			}

			escape_point[0] = (GEOMETRY_point_t) {1250, 750};
			escape_point[1] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};

			if(sens == TRIGO)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;




			if(est_dans_carre(400, 2000, 0, 1300, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = ORIENTATION;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
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

		case POS_DEPART:{
			// Petit gain de temps de choisir le sens de la zone depart
			static way_e sens_Start;

			if(entrance){
				if(tree == CHOICE_TREE_1 && global.env.color == RED && global.env.pos.x>900)
					sens_Start = BACKWARD;
				else
					sens_Start = sensRobot;
			}

			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,ORIENTATION,RETURN_NOT_HANDLED,courbe[0].speed,sens_Start,NO_DODGE_AND_WAIT);
			}break;

		case ORIENTATION:
			state = try_go_angle((tree == CHOICE_TREE_1)?0:PI4096/2,ORIENTATION,OPEN_FRUIT_VERIN,RETURN_NOT_HANDLED,FAST);
			break;

		case OPEN_FRUIT_VERIN :{
			static enum state_e state1, state2 = OPEN_FRUIT_VERIN;

			if(entrance){
				state1 = OPEN_FRUIT_VERIN;

				if(tree == CHOICE_TREE_2 && global.env.color == RED && !(global.env.guy_took_fire[FIRE_ID_START]) && state2 == OPEN_FRUIT_VERIN)// Pour effectuer seulement une fois cette action
					state2 = OPEN_FRUIT_VERIN;
				else
					state2 = POS_FIN;
			}
			if(state1 == OPEN_FRUIT_VERIN)
				state1 = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, POS_FIN, POS_FIN);
			if(state2 == OPEN_FRUIT_VERIN) // Effectue quand la ramasse des fruits si le bras n'a pas pu se mettre correctement en position
				state2 = ACT_arm_move(ACT_ARM_POS_TAKE_ON_ROAD, 0, 0, OPEN_FRUIT_VERIN,POS_FIN, POS_FIN);

			if(state1 == ERROR || state2 == ERROR)
				state = ERROR;
			else if(state1 == POS_FIN && state2 == POS_FIN)
				state = POS_FIN;

		}break;

		case POS_FIN:
			state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,courbe[NBPOINT-1].speed,sensRobot,NO_DODGE_AND_WAIT);
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

			WATCHDOG_create(TIME_CLOSE_MOUTH, ACT_fruit_mouth_goto_close,FALSE);
			//ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
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
		ORIENTATION,
		OPEN_FRUIT_VERIN,
		POS_FIN,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static const Uint8 NBPOINT = 2;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[2];

	displacement_t point[2];
	static displacement_t courbe[2];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
		case IDLE:
			strat_fruit_sucess = NO_TREE;

			if( tree == CHOICE_TREE_1){
				point[0] = (displacement_t){{950+offset_recalage.x,		3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	VITESSE_FRUIT};
				point[1] = (displacement_t){{1550+offset_recalage.x,	3000-ELOIGNEMENT_ARBRE+offset_recalage.y},	VITESSE_FRUIT};
			}else{
				point[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	2600+offset_recalage.y},					VITESSE_FRUIT};
				point[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+offset_recalage.x,	1900+offset_recalage.y},					VITESSE_FRUIT};
			}


			for(i=0;i<NBPOINT;i++){
				if(sens == HORAIRE)
					courbe[i] = point[i];
				else
					courbe[i] = point[NBPOINT-1-i];
				courbe[i].speed = FAST;
			}

			escape_point[0] = (GEOMETRY_point_t) {1250, 2250};
			escape_point[1] = (GEOMETRY_point_t) {courbe[0].point.x, courbe[0].point.y};


			if(sens == TRIGO)  // Modifie le sens
				sensRobot = FORWARD;
			else
				sensRobot = BACKWARD;

			if(est_dans_carre(400, 2000, 1800, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			{
				if(est_dans_carre(courbe[0].point.x-50, courbe[0].point.x+50, courbe[0].point.y-50, courbe[0].point.y+50, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = ORIENTATION;		//On est déjà sur la position de départ ou juste à coté... (- de 5cm)
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

		case POS_DEPART:{
			// Petit gain de temps de choisir le sens de la zone depart
			static way_e sens_Start;

			if(entrance){
				if(tree == CHOICE_TREE_2 && global.env.color != RED && global.env.pos.y > 1950)
					sens_Start = BACKWARD;
				else
					sens_Start = sensRobot;
			}

			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,ORIENTATION,RETURN_NOT_HANDLED,courbe[0].speed,sens_Start,DODGE_AND_WAIT);
			}break;

		case ORIENTATION:
			state = try_go_angle((tree == CHOICE_TREE_1)?PI4096:PI4096/2,ORIENTATION,OPEN_FRUIT_VERIN,RETURN_NOT_HANDLED,FAST);
			break;

		case OPEN_FRUIT_VERIN :{
				static enum state_e state1, state2 = OPEN_FRUIT_VERIN;

				if(entrance){
					state1 = OPEN_FRUIT_VERIN;

					if(tree == CHOICE_TREE_1 && global.env.color != RED && !(global.env.guy_took_fire[FIRE_ID_START]) && state2 == OPEN_FRUIT_VERIN)//Pour effectuer seulement un efois cette action
						state2 = OPEN_FRUIT_VERIN;
					else
						state2 = POS_FIN;
				}
				if(state1 == OPEN_FRUIT_VERIN)
					state1 = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, OPEN_FRUIT_VERIN, POS_FIN, POS_FIN);
				if(state2 == OPEN_FRUIT_VERIN) // Idem, effectue quand même l'action de prendre les fruits si bras pas sortie
					state2 = ACT_arm_move(ACT_ARM_POS_TAKE_ON_ROAD, 0, 0, OPEN_FRUIT_VERIN,POS_FIN, POS_FIN);

				if(state1 == ERROR ||  state2 == ERROR)
					state = ERROR;
				else if(state1 == POS_FIN && state2 == POS_FIN)
					state = POS_FIN;

			}break;

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

			WATCHDOG_create(TIME_CLOSE_MOUTH, ACT_fruit_mouth_goto_close,FALSE);
			//ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
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
