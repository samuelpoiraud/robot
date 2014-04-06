/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.c
 *	Package : Carte S�/strats2013
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� par .
 *	Version 2013/10/03
 */

#include "actions_pierre.h"
#include "actions_net.h"
#include "actions_Pfruit.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "../high_level_strat.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_can_over_xbee.h"

static void REACH_POINT_C1_send_request();


// Define � activer pour activer le get_in / get_ou des subactions
#define USE_GET_IN_OUT

// Define � activer pour activer les triangles des torches
#define USE_TRIANGLE_TORCHE

// Define � activer pour activer la correction triangle 2
#define USE_CORRECTION_TRIANGLE_2


//Pour Activer le mode manual de pose de fresque
#define MODE_MANUAL_FRESCO TRUE
#define POS_Y_MANUAL_FRESCO 1300 // Mettre une valeur entre 1300 et 1700



//Pour activer si on doit lancer toute les balles sur le premier mammouth ou non
//#define ALL_SHOOT_OUR_MAMMOUTH

//Pour privil�ge les fruits � la fresque dans la sub_action_nominale
//#define FAVOR_FILE_FRUIT_AT_FRESCO

//Fait tomber le triangle du bout si tout va bien ou guy l'aura fait tomber avant
//#define DROP_TRIANGLE_END

/**********************************************************************************************************************************
 *
 *		Robot prototype devra modifier le sens ( BACKWARD / FORWARD ) sur le lanceLauncher, depose de fruit et ramassage de fruit
 *				sur la verison finale
 *
 **********************************************************************************************************************************/

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test								 */
/* ----------------------------------------------------------------------------- */

#define LARGEUR_ROBOT_BACK	124
#define LARGEUR_LABIUM	250

#define DECALAGE_LARGEUR 200
#define ELOIGNEMENT_SHOOT_BALL 510
#define SPEED_LANCE_LAUNCHER 125
#define POS_MIN_FRESCO 1300
#define POS_MAX_FRESCO 1700


//Les differente's actions que pierre devra faire lors d'un match
static bool_e action_fresco_filed = FALSE;
static bool_e launcher_ball_adversary = FALSE;  // Si il doit lancer une balle sur le mammouth ennemis
static tree_sucess_e action_recup_fruit_group_1 = NO_TREE;
static tree_sucess_e action_recup_fruit_group_2 = NO_TREE;

// Lors de la recalibration, sera appeler en extern vers d'autre fichier ( ne pas passer en static )
volatile GEOMETRY_point_t offset_recalage = {0, 0};

//Provisoire pour le moment juste pour test
#define ADVERSARY_DETECTED_HOKUYO FALSE
#define FRESQUE_ENLEVER_APRS_1_COUP TRUE
#define FRESQUE_ENLEVER_APRS_2_COUP TRUE
#define NB_MAX_ADVERSARY_FRESCO_POSITION   2 //Les positions devront etre compris entre 1700 et 1300
volatile Uint16 adversary_fresco_positions[NB_MAX_ADVERSARY_FRESCO_POSITION]={1450,1590};
volatile Uint8 adversary_fresco_index = 2;


void strat_inutile(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		CHEMIN,
		LANCE_LAUNCHER,
		SUB_ACTION,
		PROTECTED_FIRES,
		DEPOSE_FRUIT,
		RAMMASSE_FRUIT,
		LANCER_FILET,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_DEPART,LANCER_FILET,ERROR,FAST,FORWARD,NO_AVOIDANCE);
			break;

		case LANCE_LAUNCHER:
			state = check_sub_action_result(strat_lance_launcher(TRUE),LANCE_LAUNCHER,DONE,ERROR);
			break;

		case SUB_ACTION:
			state = check_sub_action_result(sub_action_initiale(),SUB_ACTION,DONE,ERROR);
			break;

		case PROTECTED_FIRES:
			state = check_sub_action_result(protected_fires(OUR_FIRES),PROTECTED_FIRES,DONE,ERROR);
			break;

		case RAMMASSE_FRUIT:
			state = check_sub_action_result(manage_fruit(TREE_ADVERSARY,CHOICE_ALL_TREE,WAY_CHOICE),RAMMASSE_FRUIT,DONE,ERROR);
			break;

		case DEPOSE_FRUIT:
			state = check_sub_action_result(strat_file_fruit(),DEPOSE_FRUIT,DONE,ERROR);
			break;

		case LANCER_FILET:
			strat_placement_net();
			break;

		case DONE:
			break;
		case ERROR:
			break;
		default:
			break;
	}

}

void strat_tourne_en_rond(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		POS_POINT,
		TOUR,
		FIN,
		DONE,
		ERROR
	);

	displacement_t tour[6] = {
		{{650,600},SLOW},
		{{700,2250},SLOW},
		{{960,2600},SLOW},
		{{1500,2300},SLOW},
		{{1500,750},SLOW},
		{{1200,450},SLOW}
	};

	CAN_msg_t msg;

	static Uint16 i = 0;
	Uint16 nbTour = 2;

	switch(state){
	case IDLE:
		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=120 >> 8;
		msg.data[3]=120 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going_until_break(500,200,POS_DEPART,POS_POINT,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POS_POINT:
		state = try_going_until_break(650,600,POS_POINT,TOUR,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case TOUR:
		state = try_going_multipoint(tour,6,TOUR,FIN,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case FIN:
		i++;

		if(i >= nbTour)
			state = DONE;
		else
			state = POS_POINT;

		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}

}

void strat_lannion(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		TRIANGLE1,
		FRESQUE,
		AVANCER_FRESQUE,
		TRIANGLE2,
		TRIANGLE3,
		TRIANGLE3_AVANCER,
		TRIANGLE4_5,
		TRIANGLE6,
		TRIANGLE6_AVANCER,
		MUR,
		DEGAGEMENT,
		RECALAGE,

		POINT_W0,
		POINT_C3,
		POINT_CO, // Pour deposser fruit
		POINT_M0, // Pour deposer la fresque
		POINT_M0_BIS,
		POINT_Z1,
		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		DEPOSER_FRUIT,
		DEPOSER_FRESQUE,
		LANCE_LAUNCHER,
		LANCE_LAUNCHER_ENNEMY,
		POS_FIN,

		FIN,
		DONE,
		ERROR
	);


	displacement_t deplacement[17] = {
		// Triangle 1 x = 600 / yr = 900 / yj = 2100
		{{600+LARGEUR_LABIUM	,COLOR_Y(600)},FAST},
		{{600+LARGEUR_LABIUM	,COLOR_Y(920)},FAST},
		{{700					,COLOR_Y(1500)},FAST},

		// Triangle 2 x = 600 / yr = 2100 / yj = 900
		{{700					,COLOR_Y(1500)},FAST},
		{{600+LARGEUR_LABIUM	,COLOR_Y(2100)},FAST},
		{{600+LARGEUR_LABIUM	,COLOR_Y(2600-LARGEUR_LABIUM)},FAST},

		// Triangle 3 x = 1100 / yr = 2600 / yj = 400
		{{1400					,COLOR_Y(2600-LARGEUR_LABIUM)},FAST}, // Labium ferm�
		{{900					,COLOR_Y(2600-LARGEUR_LABIUM)},FAST}, // Labium ouvert

		// Triangle 4 x = 1600 / yr = 2100 / yj = 900
		{{1280					,COLOR_Y(2360)},FAST},
		{{1340					,COLOR_Y(2100)},FAST},
		{{1415					,COLOR_Y(1645)},FAST},

		// Triangle 5 x = 1600 / yr = 900 / yj = 2100
		{{1415					,COLOR_Y(1360)},FAST},
		{{1370					,COLOR_Y(900)},FAST},
		{{1305					,COLOR_Y(400+LARGEUR_LABIUM)},FAST},

		// Triangle 6 x = 1100 / yr = 400 / yj = 2600
		{{750				,COLOR_Y(400+LARGEUR_LABIUM)},FAST}, // Labium ferm�
		{{1400				,COLOR_Y(400+LARGEUR_LABIUM)},FAST}, // Labium ouvert

		// D�gagement
		{{750				,COLOR_Y(500)}} // Labium ferm�
	};

	static way_e sensRobot;
	static color_e colorIsRed;

	if(TIME_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch(state){
			case IDLE:
				state = POS_DEPART;
				if(global.env.color == RED)
					sensRobot = FORWARD;
				else
					sensRobot = BACKWARD;
				break;

			case POS_DEPART:
				state = try_going_until_break(440,COLOR_Y(350),POS_DEPART,TRIANGLE1,ERROR,FAST,sensRobot,NO_AVOIDANCE);
				break;

			case TRIANGLE1:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);

				state = try_going_multipoint(deplacement,3,TRIANGLE1,DEPOSER_FRESQUE,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;

			case DEPOSER_FRESQUE:
				state = check_sub_action_result(strat_manage_fresco(),DEPOSER_FRESQUE,TRIANGLE2,ERROR);
				break;

			case TRIANGLE2:
				state = try_going_multipoint(&deplacement[3],3,TRIANGLE2,TRIANGLE3,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;

			case TRIANGLE3:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);

				state = try_going_until_break(deplacement[6].point.x,deplacement[6].point.y,TRIANGLE3,TRIANGLE3_AVANCER,ERROR,deplacement[6].speed,sensRobot,NO_AVOIDANCE);
				break;

			case TRIANGLE3_AVANCER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);
				state = try_going_until_break(deplacement[7].point.x,deplacement[7].point.y,TRIANGLE3_AVANCER,TRIANGLE4_5,ERROR,deplacement[7].speed,sensRobot,NO_AVOIDANCE);
				break;

			case TRIANGLE4_5:
					state = try_going_multipoint(&deplacement[8],6,TRIANGLE4_5,TRIANGLE6,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;

			case TRIANGLE6:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);

				state = try_going_until_break(deplacement[14].point.x,deplacement[14].point.y,TRIANGLE6,TRIANGLE6_AVANCER,ERROR,deplacement[14].speed,sensRobot,NO_AVOIDANCE);
				break;

			case TRIANGLE6_AVANCER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);

				state = try_going_until_break(deplacement[15].point.x,deplacement[15].point.y,TRIANGLE6_AVANCER,DEGAGEMENT,ERROR,deplacement[15].speed,sensRobot,NO_AVOIDANCE);
				break;

			case DEGAGEMENT:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
				state = try_going_until_break(deplacement[16].point.x,deplacement[16].point.y,DEGAGEMENT,RECALAGE,ERROR,deplacement[16].speed,sensRobot,NO_AVOIDANCE);
				break;

			case RECALAGE :
				if(global.env.color == RED)
					state = check_sub_action_result(recalage_begin_zone(RED), RECALAGE, LANCE_LAUNCHER, ERROR);
				else
					state = check_sub_action_result(recalage_begin_zone(BLUE), RECALAGE, LANCE_LAUNCHER, ERROR);
				break;

				// Balles et fresques
			case LANCE_LAUNCHER:
				state = check_sub_action_result(strat_lance_launcher(FALSE),LANCE_LAUNCHER,POINT_M0,ERROR);
				break;

			case POINT_M0:
				state = PATHFIND_try_going(M0, POINT_M0, LANCE_LAUNCHER_ENNEMY, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_BREAK);
				break;

			case LANCE_LAUNCHER_ENNEMY:
				state = check_sub_action_result(strat_lance_launcher_ennemy(),LANCE_LAUNCHER_ENNEMY,POINT_Z1,ERROR);
				break;

				// Fruit
			case POINT_Z1:
				state = PATHFIND_try_going(Z1, POINT_Z1, RAMASSER_FRUIT_ARBRE2, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_BREAK);
				break;

			case RAMASSER_FRUIT_ARBRE2:
				state = check_sub_action_result(strat_ramasser_fruit_arbre2_double(colorIsRed),RAMASSER_FRUIT_ARBRE2,POINT_C3,ERROR);
				break;

			case POINT_C3:
				state = PATHFIND_try_going(C3, POINT_C3, RAMASSER_FRUIT_ARBRE1, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_BREAK);
				break;

			case RAMASSER_FRUIT_ARBRE1:
				state = check_sub_action_result(strat_ramasser_fruit_arbre1_double(!colorIsRed),RAMASSER_FRUIT_ARBRE1,POINT_M0_BIS,ERROR);
				break;

			case POINT_M0_BIS:
				state = PATHFIND_try_going(M0, POINT_M0_BIS, DEPOSER_FRUIT, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_BREAK);
				break;

			case DEPOSER_FRUIT:
				state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,DONE,ERROR);
				break;


//			case POS_FIN:
//				state = try_going(500,300,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
//				break;

			case DONE:
				break;

			case ERROR:
				break;

			default:
				break;
		}
}

void strat_test_point2(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		TRIANGLE1,
		DEPOSE_FRESCO,
		ESCAPE_FRESCO,
		TRIANGLE2,
		TRIANGLE3,
		TRIANGLE3_AVANCER,
		TRIANGLE4_5,
		TRIANGLE6,
		TRIANGLE6_AVANCER,
		MUR,
		DEGAGEMENT,
		RECALAGE1,

#ifdef USE_TRIANGLE_TORCHE
		PLACEMENT_TORCHE1,
		END_TORCHE1,
		END2_TORCHE1,
		PLACEMENT_TORCHE2,
		END_TORCHE2,
#endif

		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		AGAIN_RAMASSER_FRUIT_ARBRE1,
		AGAIN_RAMASSER_FRUIT_ARBRE2,
		POINT_DEPOSE_FRUIT,
		DEPOSER_FRUIT,
		POINT_DEPOSE_FRESCO,
		LANCE_LAUNCHER,
		LANCE_LAUNCHER_ENNEMY,
		POS_FIN,
		VERIFY,
		DONE,
		ERROR
	);

	static bool_e presenceFruit = FALSE;
	static way_e sensRobot;

	displacement_t deplacement[17] = {
		// Triangle 1 x = 600 / yr = 900 / yj = 2100
		{{600+LARGEUR_LABIUM	,COLOR_Y(600)},SLOW},
		{{600+LARGEUR_LABIUM	,COLOR_Y(920)},FAST},
		{{700					,COLOR_Y(1500)},FAST},

		// Triangle 2 x = 600 / yr = 2100 / yj = 900
		{{700					,COLOR_Y(1800)},FAST},
		{{600+15+LARGEUR_LABIUM	,COLOR_Y(2100)},FAST},
		{{600+15+LARGEUR_LABIUM	,COLOR_Y(2600-LARGEUR_LABIUM)},FAST},

		// Triangle 3 x = 1100 / yr = 2600 / yj = 400
		{{1400					,COLOR_Y(2600-LARGEUR_LABIUM)},SLOW}, // Labium ferm�
		{{900					,COLOR_Y(2600-LARGEUR_LABIUM)},SLOW}, // Labium ouvert

		// Triangle 4 x = 1600 / yr = 2100 / yj = 900
		{{1600-LARGEUR_LABIUM		,COLOR_Y(2320)},FAST},
		{{1600-LARGEUR_LABIUM		,COLOR_Y(2100)},FAST},
		{{1600-LARGEUR_LABIUM+100	,COLOR_Y(1645)},FAST},

		// Triangle 5 x = 1600 / yr = 900 / yj = 2100
		{{1415					,COLOR_Y(1360)},FAST},
		{{1370					,COLOR_Y(900)},FAST},
		{{1305					,COLOR_Y(400+LARGEUR_LABIUM)},FAST},

		// Triangle 6 x = 1100 / yr = 400 / yj = 2600
		{{750				,COLOR_Y(400+LARGEUR_LABIUM)},SLOW}, // Labium ferm�
		{{1400				,COLOR_Y(400+LARGEUR_LABIUM)},SLOW}, // Labium ouvert

		// D�gagement
		{{750				,COLOR_Y(500)},FAST} // Labium ferm�
	};


	if(TIME_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch(state){
			case IDLE:
				state = POS_DEPART;
				if(global.env.color == RED)
					sensRobot = FORWARD;
				else
					sensRobot = BACKWARD;
				break;

			case POS_DEPART:
				state = try_going_until_break(450,COLOR_Y(350),POS_DEPART,LANCE_LAUNCHER,LANCE_LAUNCHER,FAST,ANY_WAY,NO_AVOIDANCE);
				break;

			case LANCE_LAUNCHER:
				state = check_sub_action_result(strat_lance_launcher(TRUE),LANCE_LAUNCHER,DEPOSE_FRESCO,DEPOSE_FRESCO);
				break;

//			case TRIANGLE1:
//				if(entrance)
//					ACT_fruit_mouth_goto(ACT_FRUIT_Open);

//				state = try_going_multipoint(deplacement,3,TRIANGLE1,DEPOSE_FRESCO,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
//				break;


			case DEPOSE_FRESCO:
//				state = ESCAPE_FRESCO;
//				break;
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
				state = check_sub_action_result(strat_manage_fresco(),DEPOSE_FRESCO,ESCAPE_FRESCO,ERROR);
				break;

			case ESCAPE_FRESCO:
				#ifdef USE_CORRECTION_TRIANGLE_2
				state = try_going(725, COLOR_Y(1700),ESCAPE_FRESCO,TRIANGLE2,TRIANGLE2,FAST,ANY_WAY,NO_AVOIDANCE);
				#else
					state = try_going(500, 1500,ESCAPE_FRESCO,TRIANGLE2,TRIANGLE2,FAST,ANY_WAY,NO_AVOIDANCE);
				#endif
				break;

			case TRIANGLE2:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);
				state = try_going_multipoint(&deplacement[3],3,TRIANGLE2,TRIANGLE3,TRIANGLE3,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;

			case TRIANGLE3:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);

				state = try_going_until_break(deplacement[6].point.x,deplacement[6].point.y,TRIANGLE3,TRIANGLE3_AVANCER,TRIANGLE3_AVANCER,deplacement[6].speed,sensRobot,NO_AVOIDANCE);
				break;

			case TRIANGLE3_AVANCER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);
				state = try_going_until_break(deplacement[7].point.x,deplacement[7].point.y,TRIANGLE3_AVANCER,TRIANGLE4_5,TRIANGLE4_5,deplacement[7].speed,(sensRobot == FORWARD)?BACKWARD:FORWARD,NO_AVOIDANCE);
				break;

			case TRIANGLE4_5:
					state = try_going_multipoint(&deplacement[8],6,TRIANGLE4_5,TRIANGLE6,TRIANGLE6,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;

			case TRIANGLE6:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);

				state = try_going_until_break(deplacement[14].point.x,deplacement[14].point.y,TRIANGLE6,TRIANGLE6_AVANCER,TRIANGLE6_AVANCER,deplacement[14].speed,sensRobot,NO_AVOIDANCE);
				break;

			case TRIANGLE6_AVANCER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);

				state = try_going_until_break(deplacement[15].point.x,deplacement[15].point.y,TRIANGLE6_AVANCER,DEGAGEMENT,DEGAGEMENT,deplacement[15].speed,(sensRobot == FORWARD)?BACKWARD:FORWARD,NO_AVOIDANCE);
				break;

			case DEGAGEMENT:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
				state = try_going_until_break(450,COLOR_Y(350),DEGAGEMENT,RECALAGE1,RECALAGE1,FAST,ANY_WAY,NO_AVOIDANCE);
				break;

			case RECALAGE1 :
//				state = RAMASSER_FRUIT_ARBRE1;
//				break;
				if(global.env.color == RED)
					state = check_sub_action_result(recalage_begin_zone(RED), RECALAGE1, RAMASSER_FRUIT_ARBRE1, RAMASSER_FRUIT_ARBRE1);
				else
					state = check_sub_action_result(recalage_begin_zone(BLUE), RECALAGE1, RAMASSER_FRUIT_ARBRE1, RAMASSER_FRUIT_ARBRE1);
				break;

//				// Premier shoot sur notre mammouth
//			case LANCE_LAUNCHER:
//				state = check_sub_action_result(strat_lance_launcher(FALSE),LANCE_LAUNCHER,RAMASSER_FRUIT_ARBRE1,RAMASSER_FRUIT_ARBRE1);
//				break;

				// Rammasse notre groupe de fruit
			case RAMASSER_FRUIT_ARBRE1:
				#ifdef USE_TRIANGLE_TORCHE
					if(global.env.color == RED)
						state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((min_node_dist(A1,C3) == A1)? TRIGO:HORAIRE),RAMASSER_FRUIT_ARBRE1,PLACEMENT_TORCHE1,PLACEMENT_TORCHE1);
					else
						state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((min_node_dist(Z1,W3) == Z1)? HORAIRE:TRIGO),RAMASSER_FRUIT_ARBRE1,PLACEMENT_TORCHE1,PLACEMENT_TORCHE1);
				#else
					if(global.env.color == RED)
						state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((min_node_dist(A1,C3) == A1)? TREE_TRIGO:TREE_HORAIRE),RAMASSER_FRUIT_ARBRE1,POINT_DEPOSE_FRESCO,POINT_DEPOSE_FRESCO);
					else
						state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((min_node_dist(Z1,W3) == Z1)? TREE_HORAIRE:TREE_TRIGO),RAMASSER_FRUIT_ARBRE1,POINT_DEPOSE_FRESCO,POINT_DEPOSE_FRESCO);
				#endif
				/*if(strat_fruit_sucess != NO_TREE){
					presenceFruit = TRUE;
					action_recup_fruit_group_1 = strat_fruit_sucess;
				}*/

				break;

#ifdef USE_TRIANGLE_TORCHE
			case PLACEMENT_TORCHE1 :
				state = try_going(1650, COLOR_Y(1150), PLACEMENT_TORCHE1, END_TORCHE1, POINT_DEPOSE_FRESCO, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
				break;

			case END_TORCHE1 :
				state = try_going_until_break(650, COLOR_Y(1150), END_TORCHE1, END2_TORCHE1, POINT_DEPOSE_FRESCO, FAST,(global.env.color == RED)?FORWARD:BACKWARD, NO_DODGE_AND_NO_WAIT);
				break;

			case END2_TORCHE1 :
				state = try_going(500, 1500, END2_TORCHE1, RAMASSER_FRUIT_ARBRE2, RAMASSER_FRUIT_ARBRE2, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT);
				break;
#endif

				// Depose fresque
			case POINT_DEPOSE_FRESCO:
				state = PATHFIND_try_going(M0, POINT_DEPOSE_FRESCO, RAMASSER_FRUIT_ARBRE2, POINT_DEPOSE_FRESCO, (global.env.color == RED)? FORWARD:BACKWARD, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
				break;

//				// Tire seconde balle
//			case LANCE_LAUNCHER_ENNEMY:
//				state = check_sub_action_result(strat_lance_launcher_ennemy(),LANCE_LAUNCHER_ENNEMY,RAMASSER_FRUIT_ARBRE2,ERROR);
//				break;

				// Rammasse second groupe de fruit

			case RAMASSER_FRUIT_ARBRE2:
				#ifdef USE_TRIANGLE_TORCHE
					if(global.env.color == RED)
						state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((min_node_dist(Z1,W3) == Z1)? HORAIRE:TRIGO),RAMASSER_FRUIT_ARBRE2,PLACEMENT_TORCHE2,PLACEMENT_TORCHE2);
					else
						state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((min_node_dist(A1,C3) == A1)? TRIGO:HORAIRE),RAMASSER_FRUIT_ARBRE2,PLACEMENT_TORCHE2,PLACEMENT_TORCHE2);
				#else
					if(global.env.color == RED)
						state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((min_node_dist(Z1,W3) == Z1)? TREE_HORAIRE:TREE_TRIGO),RAMASSER_FRUIT_ARBRE2,POINT_DEPOSE_FRUIT,ERROR);
					else
						state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((min_node_dist(A1,C3) == A1)? TREE_TRIGO:TREE_HORAIRE),RAMASSER_FRUIT_ARBRE2,POINT_DEPOSE_FRUIT,ERROR);
			   #endif
				/*if(strat_fruit_sucess != NO_TREE){
					presenceFruit = TRUE;
					action_recup_fruit_group_2 = strat_fruit_sucess;
				}*/

				break;

#ifdef USE_TRIANGLE_TORCHE
			case PLACEMENT_TORCHE2 :
				state = try_going(1650, COLOR_Y(1850), PLACEMENT_TORCHE2, END_TORCHE2, POINT_DEPOSE_FRUIT, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
				break;

			case END_TORCHE2 :
				state = try_going(600, COLOR_Y(1850), END_TORCHE2, DEPOSER_FRUIT, POINT_DEPOSE_FRUIT, FAST, (global.env.color == RED)?FORWARD:BACKWARD, NO_DODGE_AND_NO_WAIT);
				break;
#endif

				// Depose de fruit
			case POINT_DEPOSE_FRUIT:
				if(presenceFruit == FALSE){
					state = VERIFY;
				}else
					state = PATHFIND_try_going(M0, POINT_DEPOSE_FRUIT, DEPOSER_FRUIT, DEPOSER_FRUIT, ANY_WAY, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
				break;

			case DEPOSER_FRUIT:
				state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,VERIFY,ERROR);

				if(state == VERIFY)
					presenceFruit = FALSE;
				break;

			case VERIFY: {
				static bool_e tryRammasseFruit1 = FALSE;
				static bool_e tryRammasseFruit2 = FALSE;

				if(action_recup_fruit_group_1 != ALL_TREE && tryRammasseFruit1 == FALSE){
					tryRammasseFruit1 = TRUE;
					state = AGAIN_RAMASSER_FRUIT_ARBRE1;
				}else if(action_recup_fruit_group_2 != ALL_TREE && tryRammasseFruit2 == FALSE){
					tryRammasseFruit2 = TRUE;
					state = AGAIN_RAMASSER_FRUIT_ARBRE2;
				}else if(presenceFruit == TRUE)
					state = POINT_DEPOSE_FRUIT;
				else
					state = DONE;
				break;
			}

			case AGAIN_RAMASSER_FRUIT_ARBRE1:
				if(global.env.color == RED)
					state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((min_node_dist(A1,C3) == A1)? TRIGO:HORAIRE),AGAIN_RAMASSER_FRUIT_ARBRE1,VERIFY,VERIFY);
				else
					state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((min_node_dist(Z1,W3) == Z1)? HORAIRE:TRIGO),AGAIN_RAMASSER_FRUIT_ARBRE1,VERIFY,VERIFY);

				/*if(strat_fruit_sucess != NO_TREE){
					presenceFruit = TRUE;
					action_recup_fruit_group_1 = strat_fruit_sucess;
				}*/

				break;

			case AGAIN_RAMASSER_FRUIT_ARBRE2:
				if(global.env.color == RED)
					state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((min_node_dist(Z1,W3) == Z1)? HORAIRE:TRIGO),AGAIN_RAMASSER_FRUIT_ARBRE2,VERIFY,VERIFY);
				else
					state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((min_node_dist(A1,C3) == A1)? TRIGO:HORAIRE),AGAIN_RAMASSER_FRUIT_ARBRE2,VERIFY,VERIFY);

				/*if(strat_fruit_sucess != NO_TREE){
					presenceFruit = TRUE;
					action_recup_fruit_group_2 = strat_fruit_sucess;
				}*/

				break;

			case DONE:
				break;
			case ERROR:
				break;
			default:
				break;
		}
}

void strat_test_point(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		POINT_A1, // Point arbre1A cote couloir
		POINT_B3, // Point arbre1B rouge cote armoire
		POINT_W0,
		POINT_C3,
		POINT_W3,
		POINT_CO, // Pour deposser fruit
		POINT_M0, // Pour deposer la fresque
		POINT_M0_BIS,
		POINT_Z1,
		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		RAMASSER_FRUIT_ARBRE1A,
		RAMASSER_FRUIT_ARBRE1B,
		DEPOSER_FRUIT,
		DEPOSER_FRESQUE,
		DEPOSER_FRUIT_ROUGE,
		DEPOSER_FRUIT_JAUNE,
		LANCE_LAUNCHER,
		LANCE_LAUNCHER_ENNEMY,
		POS_FIN,
		DONE,
		ERROR
	);


	CAN_msg_t msg;
	static color_e colorIsRed;

	switch(state){
	case IDLE:

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=COLOR_Y(120) >> 8;
		msg.data[3]=COLOR_Y(120) & 0xFF;

		if(global.env.color == RED){
			colorIsRed = TRUE;
			msg.data[4]=PI4096/2 >> 8;
			msg.data[5]=PI4096/2 & 0xFF;

		}else{
			colorIsRed = FALSE;
			msg.data[4]=-PI4096/2 >> 8;
			msg.data[5]=-PI4096/2 & 0xFF;
		}

		msg.size = 6;
		CAN_send(&msg);


//		debug_printf("Envoie Message ACtionneur\n");
//		ACT_lance_launcher_run(ACT_Lance_ALL);

		state = POS_DEPART;

		break;
	case POS_DEPART:
		state = try_going(500,COLOR_Y(250),POS_DEPART,LANCE_LAUNCHER,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;


	case LANCE_LAUNCHER:
		state = check_sub_action_result(strat_lance_launcher(FALSE),LANCE_LAUNCHER,POINT_M0,ERROR);
		break;


	case POINT_M0:
		state = PATHFIND_try_going(M0, POINT_M0, DEPOSER_FRESQUE, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case DEPOSER_FRESQUE:
		state = check_sub_action_result(strat_manage_fresco(),DEPOSER_FRESQUE,LANCE_LAUNCHER_ENNEMY,ERROR);
		break;
	case LANCE_LAUNCHER_ENNEMY:
		state = check_sub_action_result(strat_lance_launcher_ennemy(),LANCE_LAUNCHER_ENNEMY,POINT_Z1,ERROR);
		break;


		//Fruit sens rouge vers jaune
//	case POINT_A1:
//		state = PATHFIND_try_going(A1, POINT_A1, RAMASSER_FRUIT_ARBRE1, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
//		break;
//	case RAMASSER_FRUIT_ARBRE1:
//		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1_double(TRUE),RAMASSER_FRUIT_ARBRE1,RAMASSER_FRUIT_ARBRE2,ERROR);
//		break;
//	case POINT_W3:
//		state = PATHFIND_try_going(W3, POINT_W3, RAMASSER_FRUIT_ARBRE2, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
//		break;
//	case RAMASSER_FRUIT_ARBRE2:
//		state = check_sub_action_result(strat_test_ramasser_fruit_arbre2_double(FALSE),RAMASSER_FRUIT_ARBRE2,POINT_M0_BIS,ERROR);
//		break;
//	case POINT_M0_BIS:
//		state = PATHFIND_try_going(M0, POINT_M0_BIS, DEPOSER_FRUIT, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
//		break;
//	case DEPOSER_FRUIT:
//		state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,DONE,ERROR);
//		break;
//	case POS_FIN:
//		state = try_going(500,300,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
//		break;



		//Fruit jaune vers rouge
	case POINT_Z1:
		state = PATHFIND_try_going(Z1, POINT_Z1, RAMASSER_FRUIT_ARBRE2, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case RAMASSER_FRUIT_ARBRE2:
		state = check_sub_action_result(strat_ramasser_fruit_arbre2_double(colorIsRed),RAMASSER_FRUIT_ARBRE2,POINT_C3,ERROR);
		break;
	case POINT_C3:
		state = PATHFIND_try_going(C3, POINT_C3, RAMASSER_FRUIT_ARBRE1, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case RAMASSER_FRUIT_ARBRE1:
		state = check_sub_action_result(strat_ramasser_fruit_arbre1_double(colorIsRed),RAMASSER_FRUIT_ARBRE1,POINT_M0_BIS,ERROR);
		break;
	case POINT_M0_BIS:
		state = PATHFIND_try_going(M0, POINT_M0_BIS, DEPOSER_FRUIT, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case DEPOSER_FRUIT:
		state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,DONE,ERROR);
		break;
	case POS_FIN:
		state = try_going(500,300,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;


	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}
}

void strat_test_fresque(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		POINT_C1,
		POINT_M0,
		DEPOSER_FRESQUE,
		DONE,
		ERROR
	);


	CAN_msg_t msg;

	switch(state){
	case IDLE:
		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=120 >> 8;
		msg.data[3]=120 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going_until_break(500,250,POS_DEPART,POINT_C1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_C1:
		state = try_going_until_break(800,1000,POINT_C1,POINT_M0,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_M0:
		state = try_going(500,1500,POINT_M0,DEPOSER_FRESQUE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DEPOSER_FRESQUE:
		state = check_sub_action_result(strat_manage_fresco(),DEPOSER_FRESQUE,DONE,ERROR);
		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}
}

error_e sub_action_initiale(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		WAIT_TELL_GUY,
		LANCE_LAUNCHER,
		GOTO_TREE_2,
		DO_TREE,
#ifdef DROP_TRIANGLE_END
		GO_DROP_TRIANGLE_END,
		DO_DROP_TRIANGLE_END,
		DO_TREE_1,
#endif
		GOTO_TORCH,
		GOTO_FRESCO,
		GOTO_FRESCO_2,
		DO_FRESCO,
		FILE_FRUIT,
		TAKE_DECISION,
		TAKE_DECISION_ERROR,
		DECISION_FRUIT_FRESCO,
#ifndef ALL_SHOOT_OUR_MAMMOUTH
		LANCE_LAUNCHER_ADVERSARY,
#endif
		GOTO_FRESCO_BY_TORCH_ROAD,
		TRY_AGAIN_FRUIT,
		SUCESS,
		DONE,
		ERROR
	);

	displacement_t point[] = {{{550,COLOR_Y(1144)},	FAST},
							  {{1050,COLOR_Y(1160)},FAST},
							  {{1650,COLOR_Y(1100)},FAST}
							 };


	// Mettre a false si pas le cas
	static bool_e guy_get_out_init = FALSE;

	if(global.env.reach_point_get_out_init)
		guy_get_out_init = TRUE;


	switch(state){

		case IDLE:
			if(global.env.asser.calibrated)
				state = WAIT_TELL_GUY;
			else	//On est en train de jouer un match de test sans l'avoir calibr�... donc Guy n'est pas l� !
				state = LANCE_LAUNCHER;
			break;

		case WAIT_TELL_GUY:{
			static Uint16 last_time;
			if(entrance)
				last_time = global.env.match_time;

			if(guy_get_out_init || global.env.match_time > last_time + 2000)
				state = LANCE_LAUNCHER;
			}break;

		case LANCE_LAUNCHER:
#ifdef ALL_SHOOT_OUR_MAMMOUTH
			state = check_sub_action_result(strat_lance_launcher(TRUE),LANCE_LAUNCHER,GOTO_TORCH,GOTO_TORCH);
#else
			state = check_sub_action_result(strat_lance_launcher(FALSE),LANCE_LAUNCHER,GOTO_TORCH,GOTO_TORCH);
#endif

			break;

		case GOTO_TORCH:

			if(entrance)
				ASSER_WARNER_arm_x(800);

			state = try_going_multipoint(&point[0],3,GOTO_TORCH,GOTO_TREE_2,GOTO_FRESCO,(global.env.color == RED)? FORWARD : BACKWARD,NO_DODGE_AND_WAIT, END_AT_BREAK);

			if(global.env.asser.reach_x)
				REACH_POINT_C1_send_request();

			break;


		case GOTO_TREE_2:{
			static Uint8 s1 = IN_PROGRESS ,s2 = IN_PROGRESS;

			if(s1 == IN_PROGRESS)
				s1 = try_going(1633,COLOR_Y(1200),IN_PROGRESS,SUCESS,ERROR,FAST,ANY_WAY,DODGE_AND_NO_WAIT);

			if(s2 == IN_PROGRESS)
				s2 = ELEMENT_do_and_wait_end_fruit_verin_order(FRUIT_VERIN_OPEN, IN_PROGRESS, SUCESS, ERROR);


			if(s1 == ERROR){
				state = GOTO_FRESCO;
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);

			}else if(s1 == SUCESS && s2 != IN_PROGRESS) // Si le bras est en sucess ou pars en erreur( timeout, probl�me capteur)
				state = DO_TREE;

			else if(s1 != ERROR && s2 != ERROR)
				state = GOTO_TREE_2;
			}break;

		case DO_TREE:
			#ifdef DROP_TRIANGLE_END
			state = check_sub_action_result(manage_fruit(TREE_OUR,CHOICE_TREE_2,(global.env.color == RED)? HORAIRE : TRIGO),DO_TREE,GO_DROP_TRIANGLE_END,TAKE_DECISION_ERROR);
			#else
			state = check_sub_action_result(manage_fruit(TREE_OUR,CHOICE_ALL_TREE,(global.env.color == RED)? HORAIRE : TRIGO),DO_TREE,GOTO_FRESCO,TAKE_DECISION_ERROR);
			#endif
			break;

		#ifdef DROP_TRIANGLE_END
		case GO_DROP_TRIANGLE_END:
			state = try_going(850,COLOR_Y(650),GO_DROP_TRIANGLE_END,DO_DROP_TRIANGLE_END,TAKE_DECISION_ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case DO_DROP_TRIANGLE_END:
			if(entrance)
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);

			state = try_going(1250,COLOR_Y(650),DO_DROP_TRIANGLE_END,DO_TREE_1,TAKE_DECISION_ERROR,FAST,FORWARD,NO_DODGE_AND_WAIT);

			if(state != DO_DROP_TRIANGLE_END)
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			break;

		case DO_TREE_1:
			state = check_sub_action_result(manage_fruit(TREE_OUR,CHOICE_ALL_TREE,(global.env.color == RED)? HORAIRE : TRIGO),DO_TREE_1,GOTO_FRESCO,TAKE_DECISION_ERROR);
			break;
		#endif

		case GOTO_FRESCO:
			state = try_going(400,COLOR_Y(1500),GOTO_FRESCO,DECISION_FRUIT_FRESCO,GOTO_FRESCO_BY_TORCH_ROAD,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		case DECISION_FRUIT_FRESCO:
			#ifdef FAVOR_FILE_FRUIT_AT_FRESCO
				state = FILE_FRUIT;
			#else
				state = DO_FRESCO;
			#endif
			break;

		case TAKE_DECISION:

			#ifndef ALL_SHOOT_OUR_MAMMOUTH
			if(launcher_ball_adversary == FALSE && last_state == FILE_FRUIT){
				state = LANCE_LAUNCHER_ADVERSARY;
				break;
			}
			#endif

			if(action_fresco_filed == FALSE && last_state != DO_FRESCO){
				state = DO_FRESCO;
			}else if(get_presenceFruit() == TRUE && last_state != FILE_FRUIT){
				state = FILE_FRUIT;
			}
			#ifndef ALL_SHOOT_OUR_MAMMOUTH
			else if(launcher_ball_adversary == FALSE && last_state != LANCE_LAUNCHER_ADVERSARY){
				state = LANCE_LAUNCHER_ADVERSARY;
			}
			#endif
			else if(FRUIT_sucess_tree(TREE_OUR) != ALL_TREE){
				state = TRY_AGAIN_FRUIT;
			}else
				state = DONE;


			break;

		case TAKE_DECISION_ERROR:

			if(action_fresco_filed == FALSE && last_state != DO_FRESCO){
				state = DO_FRESCO;
			}else if(get_presenceFruit() == TRUE && last_state != FILE_FRUIT && last_state != LANCE_LAUNCHER_ADVERSARY){
				state = FILE_FRUIT;
			}
			#ifndef ALL_SHOOT_OUR_MAMMOUTH
			else if(launcher_ball_adversary == FALSE && last_state != LANCE_LAUNCHER_ADVERSARY && last_state != FILE_FRUIT){
				state = LANCE_LAUNCHER_ADVERSARY;
			}
			#endif
			else if(FRUIT_sucess_tree(TREE_OUR) != ALL_TREE && last_state != TRY_AGAIN_FRUIT){
				state = TRY_AGAIN_FRUIT;
			}else
				state = ERROR;

			break;

		case DO_FRESCO:
			state = check_sub_action_result(strat_manage_fresco(),DO_FRESCO,TAKE_DECISION,TAKE_DECISION_ERROR);
			break;

		case FILE_FRUIT:
			state = check_sub_action_result(strat_file_fruit(),FILE_FRUIT,TAKE_DECISION,TAKE_DECISION_ERROR);
			break;

		case GOTO_FRESCO_2:
			state = try_going(400,COLOR_Y(1500),GOTO_FRESCO_2,DO_FRESCO,GOTO_FRESCO_BY_TORCH_ROAD,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		#ifndef ALL_SHOOT_OUR_MAMMOUTH
		// Si on lance une balle sur le mammouth ennemis
		case LANCE_LAUNCHER_ADVERSARY:
			state = check_sub_action_result(strat_lance_launcher_ennemy(),LANCE_LAUNCHER_ADVERSARY,TAKE_DECISION, TAKE_DECISION_ERROR);
			break;
		#endif

		// Cas d'erreur � DO_TREE_2 et GOTO_FRESCO
		case GOTO_FRESCO_BY_TORCH_ROAD:
			state = try_going(1400,COLOR_Y(2200),GOTO_FRESCO_BY_TORCH_ROAD,DECISION_FRUIT_FRESCO,TAKE_DECISION_ERROR,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		// Cas erreur fruit non ramass� au d�but
		case TRY_AGAIN_FRUIT:
			state = check_sub_action_result(manage_fruit(TREE_OUR,ALL_TREE,WAY_CHOICE),TRY_AGAIN_FRUIT,TAKE_DECISION,TAKE_DECISION_ERROR);
			break;

		case DONE:
			return END_OK;
			break;

		case ERROR:
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e protected_fires(protected_fires_e fires){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POINT_0,
		POINT_1,
		POINT_2,
		POINT_3,
		SUCESS,
		NOT_REACHED,
		DONE,
		ERROR,
	);

	static GEOMETRY_point_t points[4];

	switch(state){
		case IDLE:

			if((global.env.color == RED && fires == OUR_FIRES) || (global.env.color != RED && fires == ADVERSARY_FIRES)){
				points[0] = (GEOMETRY_point_t){1350,400};   // A2
				points[1] = (GEOMETRY_point_t){1600,650};	// B3
				points[2] = (GEOMETRY_point_t){1250,1100};  // C2
				points[3] = (GEOMETRY_point_t){850,650};    // B1
			}else{
				points[0] = (GEOMETRY_point_t){1350,2600};  // Z2
				points[1] = (GEOMETRY_point_t){1600,2300};	// Y3
				points[2] = (GEOMETRY_point_t){1250,1900};  // W2
				points[3] = (GEOMETRY_point_t){850,2300};   // Y1
			}

			state = POINT_0;

			if((((global.env.color == RED && fires == OUR_FIRES) && (global.env.color != RED && fires == ADVERSARY_FIRES)) && est_dans_carre(400, 2000, 0, 1500, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					|| (((global.env.color == RED && fires == ADVERSARY_FIRES) && (global.env.color != RED && fires == OUR_FIRES)) && est_dans_carre(400, 2000, 1500, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = POINT_0;
			else
				state = GET_IN;
		   break;

	   case GET_IN:
		   state = PATHFIND_try_going(PATHFIND_closestNode(points[0].x,points[0].y, 0x00),
				   GET_IN, POINT_0, ERROR, ANY_WAY, FAST,DODGE_AND_NO_WAIT, END_AT_BREAK);
		   break;

		case POINT_0:
			state = try_going(points[0].x,points[0].y,POINT_0,POINT_3,POINT_1,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		case POINT_1:
			state = try_going(points[1].x,points[1].y,POINT_1,POINT_2,POINT_0,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		case POINT_2:
			state = try_going(points[2].x,points[2].y,POINT_2,DONE,ERROR,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		case POINT_3:
			state = try_going(points[3].x,points[3].y,POINT_3,POINT_1,POINT_2,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
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


error_e strat_manage_fresco(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		ADVERSARY_DETECTED,
		FILE_FRESCO,
		VERIFICATION,
		LAST_CHANCE_FILE_FRESCO,
		VERIFICATION_2,
		LAST_LAST_CHANCE_FILE_FRESCO,
		DONE,
		ERROR
	);

	static Sint16 posY = 1500;
	static Sint16 oldPosY = 1500; // Si les deux premiere pose ne fonctionne pas nous aurons besoins de lui

	switch(state){

		case IDLE:
			if(action_fresco_filed){
				debug_printf("ERREUR, la fresque a d�ja �t� pos�\n");
				return END_OK;
			}

			if(MODE_MANUAL_FRESCO){
				posY = POS_Y_MANUAL_FRESCO;
				if(est_dans_carre(0, 1100, 900, 2000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = FILE_FRESCO;
				else
					state = GET_IN;
			}else if(ADVERSARY_DETECTED_HOKUYO == FALSE){//Pose les fresques au milieu si on a pas vu l'adversaire poser les siennes
				posY = 1500;
				if(est_dans_carre(0, 1100, 900, 2000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					state = FILE_FRESCO;
				else
					state = GET_IN;
			}else
				state = ADVERSARY_DETECTED;
			break;

		case ADVERSARY_DETECTED:
			switch(adversary_fresco_index){
				case 1:
					if(adversary_fresco_positions[0] > POS_MAX_FRESCO && adversary_fresco_positions[0] < POS_MIN_FRESCO)// La valeur n est pas comprise
						posY = 1500;

					if(adversary_fresco_positions[0] > 1500)
						posY = POS_MIN_FRESCO;
					else
						posY = POS_MAX_FRESCO;

					break;
				case 2:
					if((adversary_fresco_positions[0] > POS_MAX_FRESCO && adversary_fresco_positions[0] < POS_MIN_FRESCO) || (adversary_fresco_positions[1] > POS_MAX_FRESCO && adversary_fresco_positions[1] < POS_MIN_FRESCO))
						posY = 1500;

					if(adversary_fresco_positions[0] > 1500 && adversary_fresco_positions[1] > 1500)// Les 2 poses ennemis sont sup au milieu de la fresque
						posY = POS_MIN_FRESCO;
					else if(adversary_fresco_positions[0] < 1500 && adversary_fresco_positions[1] < 1500) // sont inf au milieu
						posY = POS_MAX_FRESCO;
					else{    // Il y a une fresque de chaque cot� par rapport au milieu
						Uint16 fresco_inf,fresco_sup; //Correspond a la plus grande et plus petite variable ou l'adversaire aurait poser ses fresques

						if(adversary_fresco_positions[0] < adversary_fresco_positions[1]){
							fresco_inf = adversary_fresco_positions[0];
							fresco_sup = adversary_fresco_positions[1];
						}else{
							fresco_inf = adversary_fresco_positions[1];
							fresco_sup = adversary_fresco_positions[0];
						}

						if(POS_MAX_FRESCO-fresco_sup > fresco_inf-POS_MIN_FRESCO){// La zone la plus grande est entre le POS_MAX_FRECO et le point sup de la pose
							if(POS_MAX_FRESCO-fresco_sup >= fresco_sup-fresco_inf)// Testons si la zone entre les deux fresques n'est pas plus grande
								posY = POS_MAX_FRESCO;
							else
								posY = (fresco_inf+fresco_sup)/2;

						}else{ //Si la zone entre la POS_MIN_FRESCO est plus grande que celle avec POS_MAX_FRESCO
							if(fresco_inf-POS_MIN_FRESCO >= fresco_sup-fresco_inf)
								posY = POS_MIN_FRESCO;
							else
								posY = (fresco_inf+fresco_sup)/2;
						}
					}

					break;
				default: // On ne sait jamais si aucun adversaire a �t� detecte
					posY = 1500;
					break;
			}

			if(est_dans_carre(0, 1100, 900, 2000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = FILE_FRESCO;
			else
				state = GET_IN;
			break;

		case GET_IN :
			state = PATHFIND_try_going(M0, GET_IN, FILE_FRESCO, ERROR, ANY_WAY, FAST, DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case FILE_FRESCO:
			state = check_sub_action_result(strat_file_fresco(posY),FILE_FRESCO,VERIFICATION,ERROR);
			break;

		case VERIFICATION:
			if(FRESQUE_ENLEVER_APRS_1_COUP)//fresque plus presente sur le support grace au capteur
				state = DONE;
			else
				state = LAST_CHANCE_FILE_FRESCO;

			break;

		case LAST_CHANCE_FILE_FRESCO:
				if(entrance){
					oldPosY = posY;

					if(posY > 1500) //Ne prend plus en compte les positions des adversaires eu precedement (Elles sont fausses sinon ne serait pas dans cet �tat)
						posY = POS_MIN_FRESCO;
					else
						posY = POS_MAX_FRESCO;
				}

				state = check_sub_action_result(strat_file_fresco(posY),LAST_CHANCE_FILE_FRESCO,VERIFICATION_2,ERROR);
			break;

		case VERIFICATION_2:
			if(FRESQUE_ENLEVER_APRS_2_COUP)//FRESCO_1 && FRESCO_2)//fresque plus presente sur le support grace au capteur
				state = DONE;
			else
				state = LAST_LAST_CHANCE_FILE_FRESCO;

			break;

		case LAST_LAST_CHANCE_FILE_FRESCO:
				if(entrance){
					if(posY == POS_MAX_FRESCO && oldPosY != POS_MIN_FRESCO)
						posY = POS_MIN_FRESCO;
					else if(posY == POS_MIN_FRESCO && oldPosY != POS_MAX_FRESCO)
						posY = POS_MAX_FRESCO;
					else
						posY = 1500;
				}

				state = check_sub_action_result(strat_file_fresco(posY),LAST_LAST_CHANCE_FILE_FRESCO,DONE,ERROR);
			break;

		case DONE:
			state = IDLE;
			action_fresco_filed = TRUE;
			set_sub_act_done(SUB_FRESCO,TRUE);
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

error_e strat_file_fresco(Sint16 posY){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		WALL,
		BEFORE_WALL,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		END,
		END_IMPOSSIBLE,
		END_ERROR,
		END_IMPOSSIBLE_ERROR,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static bool_e timeout=FALSE;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[3];

	switch(state){
		case IDLE:
			if(posY > POS_MAX_FRESCO || posY < POS_MIN_FRESCO){
				debug_printf("ERREUR Mauvaise position envoyer en fresque\n");
				return NOT_HANDLED;
			}

			timeout=FALSE;

			escape_point[0] = (GEOMETRY_point_t){300, 1500};
			escape_point[1] = (GEOMETRY_point_t){300, 1350};
			escape_point[2] = (GEOMETRY_point_t){300, 1650};

			state = BEFORE_WALL;
			break;

		case BEFORE_WALL:
			state = try_going(270,posY,BEFORE_WALL,WALL,END_ERROR,FAST,BACKWARD,NO_DODGE_AND_NO_WAIT);
			break;

		case WALL:
			state = try_going(170,posY,WALL,PUSH_MOVE,END_ERROR,FAST,BACKWARD,NO_DODGE_AND_NO_WAIT); // Si en erreur on va a END seule veut dire qu un triangle a g�n�, la strat_manage fresco va donc choisir une autre position
			break;

		case PUSH_MOVE://Le fait forcer contre le mur pour poser la fresque
			ASSER_push_rush_in_the_wall(BACKWARD,TRUE,0,TRUE);//Le fait forcer en marche arriere
			state = WAIT_END_OF_MOVE;
			break;

		case WAIT_END_OF_MOVE:
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				state = END;
			}
			break;

		case END:
			state = try_going_until_break(400,posY,END,DONE,END_IMPOSSIBLE,FAST,FORWARD,NO_DODGE_AND_NO_WAIT);
			break;

		case END_IMPOSSIBLE:
			if(global.env.pos.x > 200)
				state = ERROR;
			else
				state = END;
			break;

		case END_ERROR: //Pour le faire forcer � partir au droit
			state = try_going_until_break(400,posY,END_ERROR,ERROR,END_IMPOSSIBLE_ERROR,FAST,FORWARD,NO_DODGE_AND_NO_WAIT);
			break;

		case END_IMPOSSIBLE_ERROR:
			if(global.env.pos.x > 200)
				state = ERROR;
			else
				state = END_ERROR;
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_lance_launcher(bool_e lanceAll){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_BEGINNING,
		LANCE_LAUNCHER,
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
	static Uint16 posShoot; // Position � laquelle, on va tirer les balles
	static Uint16 sensShoot; // Vrai si il va de rouge vers jaune sinon faux

	switch(state){
		case IDLE:
			 if(global.env.pos.y > 750  && global.env.color == RED){ // Rouge milieu
				dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[0].y = 1200;

				dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[1].y = 300;

				sensRobot = BACKWARD;
				sensShoot = FALSE;
				posShoot = 800;

			}else if(global.env.color == RED){	// Rouge et case depart
				dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[0].y = 300;

				dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[1].y = 1100;

				sensRobot = FORWARD;
				sensShoot = TRUE;
				posShoot = 640;
			}else if(global.env.pos.y > 2225){ // Jaune, case depart
				dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[0].y = 2700;

				dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[1].y = 1900;

				sensRobot = BACKWARD;
				sensShoot = FALSE;
				posShoot = 2340;

			}else{ // Jaune milieu
				dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[0].y = 1800;

				dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
				dplt[1].y = 2700;

				sensRobot = FORWARD;
				sensShoot = TRUE;
				posShoot = 2100;

			}

			 escape_point[0] = dplt[0];
			 escape_point[1] = dplt[1];

			 if((global.env.color == RED && (est_dans_carre(200, 2000, 100, 1350, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) || est_dans_carre(250, 750, 100, 1800, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
					 || (global.env.color == YELLOW && (est_dans_carre(200, 2000, 1650, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) || est_dans_carre(250, 750, 1200, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))))
				 state = POS_BEGINNING;
			 else
				 state = GET_IN;
			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(dplt[0].x,dplt[0].y, 0x00),
					GET_IN, POS_BEGINNING, ERROR, sensRobot, FAST,DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case POS_BEGINNING:
			state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,FAST,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case POS_END:
			if(entrance)
				ASSER_WARNER_arm_y(posShoot);

			state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,DONE,ERROR,SPEED_LANCE_LAUNCHER,sensRobot,NO_DODGE_AND_WAIT);

			if(global.env.asser.reach_y){
				if(lanceAll == TRUE)
					ACT_lance_launcher_run(ACT_Lance_ALL,sensShoot);
				else
					ACT_lance_launcher_run(ACT_Lance_5_BALL,sensShoot);
			}
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			state = IDLE;
			set_sub_act_done(SUB_LANCE,TRUE);
			return END_OK;
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_lance_launcher_ennemy(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		POS_BEGINNING,
		POS_END,
		POS_FINISH,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[2];

	static bool_e do_pos_finish = FALSE;
	static GEOMETRY_point_t dplt[2]; // Deplacement
	static way_e sensRobot;
	static Uint16 sensShoot; // Vrai si il va de rouge vers jaune sinon faux
	static Uint16 posShoot; // Position � laquelle, on va tirer les balles

	switch(state){
		case IDLE:

			if(global.env.pos.y > 2100 && global.env.color == RED){ // Va tirer sur le Jaune, case depart jaune
			   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;

			   if(est_dans_carre(ELOIGNEMENT_SHOOT_BALL-30,700,2350,2700,(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				   dplt[0].y = global.env.pos.y;
			   else
				  dplt[0].y = 2500;

			   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			   dplt[1].y = 1800;

			   sensRobot = BACKWARD;
			   sensShoot = FALSE;
			   posShoot = 2300;

			}else if(global.env.color == RED){ // Tire sur le Jaune milieu
			   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
			   dplt[0].y = 1800;

			   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			   dplt[1].y = 2300;

			   sensRobot = FORWARD;
			   sensShoot = TRUE;
			   posShoot = 2200;
			   do_pos_finish = TRUE;

		   }else if(global.env.pos.y > 850){ // tire sur le Rouge milieu
			   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
			   dplt[0].y = 1200;

			   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			   dplt[1].y = 800;

			   sensRobot = BACKWARD;
			   sensShoot = FALSE;
			   posShoot = 700;
			   do_pos_finish = TRUE;

		   }else{	// Tire Rouge et case depart
			   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;

				if(est_dans_carre(ELOIGNEMENT_SHOOT_BALL-30,700,400,690,(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
					dplt[0].y = global.env.pos.y;
				else
				   dplt[0].y = 300;

			   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			   dplt[1].y = 1200;

			   sensRobot = FORWARD;
			   sensShoot = TRUE;
			   posShoot = 700;
		   }

			escape_point[0] = dplt[0];
			escape_point[1] = dplt[1];


			if((global.env.color == RED && (est_dans_carre(300, 2000, 1650, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) || est_dans_carre(250, 850, 1200, 2500, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
					|| (global.env.color == YELLOW && (est_dans_carre(300, 2000, 100, 1350, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) || est_dans_carre(250, 850,500, 1800, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))){
				state = POS_BEGINNING;

			}else
				state = GET_IN;
			break;

		case GET_IN:
			state = PATHFIND_try_going(PATHFIND_closestNode(dplt[0].x,dplt[0].y, 0x00),
					GET_IN, POS_BEGINNING, ERROR, sensRobot, FAST, DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case POS_BEGINNING:
			state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,FAST,sensRobot,NO_DODGE_AND_WAIT);
			break;

		case POS_END:
			if(entrance)
				ASSER_WARNER_arm_y(posShoot);

			state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,(do_pos_finish == TRUE)? POS_FINISH:DONE,(do_pos_finish == TRUE)? POS_FINISH:ERROR,SPEED_LANCE_LAUNCHER,sensRobot,NO_DODGE_AND_WAIT);

			if(global.env.asser.reach_y)
				ACT_lance_launcher_run(ACT_Lance_1_BALL,sensShoot);
			break;

		case POS_FINISH:
			state = try_going_until_break(ELOIGNEMENT_SHOOT_BALL,COLOR_Y(1700),POS_FINISH,DONE,ERROR,SPEED_LANCE_LAUNCHER,(global.env.color == RED)? FORWARD:BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			state = IDLE;
			set_sub_act_done(SUB_LANCE_ADV,TRUE);
			launcher_ball_adversary = TRUE;
			return END_OK;
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		RUSH_WALL,
		WAIT,
		GET_OUT,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static bool_e timeout;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[1];

	switch(state){
		case IDLE :
			timeout = FALSE;
			if(global.env.pos.x > 1000)
				escape_point[0] = (GEOMETRY_point_t){wanted_x - 200, global.env.pos.y};
			else
				escape_point[0] = (GEOMETRY_point_t){wanted_x + 200, global.env.pos.y};
			state = RUSH_WALL;
			break;

		case RUSH_WALL :
			ASSER_push_rush_in_the_wall(sens, TRUE, angle, TRUE);
			state = WAIT;
			break;

		case WAIT :
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){

				offset_recalage.x = global.env.pos.x - wanted_x;
				debug_printf("offset_recalage.x = %d\n", offset_recalage.x);
				state = GET_OUT;
			}
			break;

		case GET_OUT :
			if(global.env.pos.x > 1000)
				state = try_going(wanted_x - 50, global.env.pos.y, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			else
				state = try_going(wanted_x + 200, global.env.pos.y, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE :
			state = IDLE;
			return END_OK;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		RUSH_WALL,
		WAIT,
		GET_OUT,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static bool_e timeout;

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[1];

	switch(state){
		case IDLE :
			timeout = FALSE;
			if(global.env.pos.y > 1500)
				escape_point[0] = (GEOMETRY_point_t){global.env.pos.x, wanted_y - 200};
			else
				escape_point[0] = (GEOMETRY_point_t){global.env.pos.x, wanted_y + 200};
			state = RUSH_WALL;
			break;

		case RUSH_WALL :
			ASSER_push_rush_in_the_wall(sens, TRUE, angle, TRUE);
			state = WAIT;
			break;

		case WAIT :
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				offset_recalage.y = global.env.pos.y - wanted_y;
				debug_printf("offset_recalage.y = %d\n", offset_recalage.y);
				state = GET_OUT;
			}
			break;

		case GET_OUT :
			if(global.env.pos.y > 1500)
				state = try_going(global.env.pos.x, wanted_y - 80, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			else
				state = try_going(global.env.pos.x, wanted_y + 80, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE :
			state = IDLE;
			return END_OK;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

error_e recalage_begin_zone(color_e begin_zone_color){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		GET_IN,
		PLACEMENT_RECALAGE_X,
		RECALAGE_X,
		PLACEMENT_RECALAGE_Y,
		RECALAGE_Y,
		GET_OUT,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static Uint8 get_out_try = 0;
	static GEOMETRY_point_t escape_point[1];

	switch(state){
		case IDLE :
			if(begin_zone_color == RED)
				escape_point[0] = (GEOMETRY_point_t){200, 200};
			else
				escape_point[0] = (GEOMETRY_point_t){200, 1800};
			#ifdef USE_GET_IN_OUT
				state = GET_IN;
			#else
				state = PLACEMENT_RECALAGE_Y;
			#endif
			break;

		case GET_IN :
			if(begin_zone_color == RED)
				state = PATHFIND_try_going(A1, GET_IN, PLACEMENT_RECALAGE_Y, ERROR_WITH_GET_OUT, ANY_WAY, FAST , NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
			else
				state = PATHFIND_try_going(Z1, GET_IN, PLACEMENT_RECALAGE_Y, ERROR_WITH_GET_OUT, ANY_WAY, FAST , NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case PLACEMENT_RECALAGE_Y :
			if(begin_zone_color == RED)
				state = try_going(ELOIGNEMENT_SHOOT_BALL, 200, PLACEMENT_RECALAGE_Y, RECALAGE_Y, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			else
				state = try_going(ELOIGNEMENT_SHOOT_BALL, 2800, PLACEMENT_RECALAGE_Y, RECALAGE_Y, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case RECALAGE_Y :
			if(begin_zone_color == RED)
				state = check_sub_action_result(action_recalage_y(BACKWARD, PI4096/2, LARGEUR_ROBOT_BACK), RECALAGE_Y, PLACEMENT_RECALAGE_X, ERROR);
			else
				state = check_sub_action_result(action_recalage_y(BACKWARD, -PI4096/2, 3000-LARGEUR_ROBOT_BACK), RECALAGE_Y, PLACEMENT_RECALAGE_X, ERROR);
			break;

		case PLACEMENT_RECALAGE_X :
			if(begin_zone_color == RED)
				state = try_going(200, 225, PLACEMENT_RECALAGE_X, RECALAGE_X, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			else
				state = try_going(200, 2775, PLACEMENT_RECALAGE_X, RECALAGE_X, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case RECALAGE_X :
			if(begin_zone_color == RED)
				state = check_sub_action_result(action_recalage_x(BACKWARD, 0, LARGEUR_ROBOT_BACK), RECALAGE_X, GET_OUT, ERROR);
			else
				state = check_sub_action_result(action_recalage_x(BACKWARD, 0, LARGEUR_ROBOT_BACK), RECALAGE_X, GET_OUT, ERROR);
			break;

		case GET_OUT :
			if(begin_zone_color == RED)
				state = try_going(600, 300, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			else
				state = try_going(600, 2700, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[get_out_try].x,escape_point[get_out_try].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state != GET_OUT_WITH_ERROR)
				get_out_try = (get_out_try == sizeof(escape_point)/sizeof(GEOMETRY_point_t)-1)?0:get_out_try+1;
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
			break;
	}
	return IN_PROGRESS;
}

void strat_test_vide(){

	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		AVANCER,
		DONE
	);

	if(TIME_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch (state){
			case INIT :
				state = AVANCER;
				break;

			case AVANCER :
				state = try_going(603, 2500, AVANCER, DONE, DONE, SLOW, ANY_WAY, NO_AVOIDANCE);
				break;

			case DONE :
				break;
		}
}

void strat_test_small_arm(){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		IDLE,
		MID,
		DEPLOYED,
		MID_END,
		IDLE_END,
		DONE
	);

	switch(state){
		case INIT :
			state = IDLE;
			break;

		case IDLE :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_IDLE);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = MID;
			break;

		case MID :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_MID);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = DEPLOYED;
			break;

		case DEPLOYED :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_DEPLOYED);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = MID_END;
			break;

		case MID_END :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_MID);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = IDLE_END;
			break;

		case IDLE_END :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_IDLE);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = DONE;
			break;

		case DONE :
			break;
	}
}


static void REACH_POINT_C1_send_request() {
	CAN_msg_t msg;

	msg.sid = XBEE_REACH_POINT_C1;
	msg.size = 0;

	CANMsgToXbee(&msg,FALSE);
}
