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


#include "actions_pierre.h"
#include "actions_net.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "strat_pierre.h"


/**********************************************************************************************************************************
 *
 *		Robot prototype devra modifier le sens ( BACKWARD / FORWARD ) sur le lanceLauncher, depose de fruit et ramassage de fruit
 *				sur la verison finale
 *
 **********************************************************************************************************************************/

static bool_e pointIsShotter(pathfind_node_id_t n1,pathfind_node_id_t n2);

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test								 */
/* ----------------------------------------------------------------------------- */

#define DECALAGE_LARGEUR 200
#define ELOIGNEMENT_ARBRE 320
#define ELOIGNEMENT_POSE_BAC_FRUIT 500
#define ELOIGNEMENT_SHOOT_BALL 510
#define SPEED_LANCE_LAUNCHER 130
#define POS_MIN_FRESCO 1300
#define POS_MAX_FRESCO 1700


//Pour Activer le mode manual de pose de fresque
#define MODE_MANUAL_FRESCO TRUE
#define POS_Y_MANUAL_FRESCO 1500 // Mettre une valeur entre 1300 et 1700


//Les differente's actions que pierre devra faire lors d'un match
static bool_e action_fresco_filed = FALSE;
static bool_e action_recup_fruit_group_1 = FALSE;
static bool_e action_recup_fruit_group_2 = FALSE;


static bool_e strat_fruit_sucess = FALSE;

//Provisoire pour le moment juste pour test
#define ADVERSARY_DETECTED_HOKUYO FALSE
#define FRESQUE_ENLEVER_APRS_1_COUP TRUE
#define FRESQUE_ENLEVER_APRS_2_COUP FALSE
#define NB_MAX_ADVERSARY_FRESCO_POSITION   2 //Les positions devront etre compris entre 1700 et 1300
volatile Uint16 adversary_fresco_positions[NB_MAX_ADVERSARY_FRESCO_POSITION]={1450,1590};
volatile Uint8 adversary_fresco_index = 2;


void strat_inutile(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		CHEMIN,
		LANCE_LAUNCHER,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(440,COLOR_Y(350),POS_DEPART,LANCE_LAUNCHER,ERROR,FAST,FORWARD,NO_AVOIDANCE);
			break;


		case LANCE_LAUNCHER:
			state = check_sub_action_result(strat_lance_launcher(TRUE),LANCE_LAUNCHER,CHEMIN,ERROR);
			break;

		case CHEMIN:
			state = try_going_until_break(500,COLOR_Y(1700),CHEMIN,DONE,ERROR,FAST,ANY_WAY,NO_AVOIDANCE);
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

	displacement_t deplacement[12] = {
		// Triangle 1
		{{800,COLOR_Y(750)},FAST},
		{{820,COLOR_Y(900)},FAST},
		{{750,COLOR_Y(1200)},FAST},

		// Triangle 2
		{{680,COLOR_Y(1650)},FAST},
		{{830,COLOR_Y(2090)},FAST},
		{{850,COLOR_Y(2250)},FAST},
		//{{1000,COLOR_Y(2200},SLOW}

		// Triangle 3
		{{1280,COLOR_Y(2360)},FAST},
		{{1340,COLOR_Y(2100)},FAST},
		{{1415,COLOR_Y(1645)},FAST},
		{{1415,COLOR_Y(1360)},FAST},
		{{1370,COLOR_Y(900)},FAST},
		{{1305,COLOR_Y(720)},FAST},
		//{{1000,COLOR_Y(650)},SLOW}
	};

	static way_e sensRobot;
	static color_e colorIsRed;

	if(TIME_MATCH_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch(state){
			case IDLE:
				state = POS_DEPART;
				break;

			case POS_DEPART:
				state = try_going_until_break(440,COLOR_Y(350),POS_DEPART,LANCE_LAUNCHER,ERROR,FAST,FORWARD,NO_AVOIDANCE);
				break;

				// Triangle
			case TRIANGLE1:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Open);

				state = try_going_multipoint(deplacement,3,TRIANGLE1,TRIANGLE2,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;
			case TRIANGLE2:
				state = try_going_multipoint(&deplacement[3],3,TRIANGLE2,TRIANGLE3,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;
			case TRIANGLE3:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Close);

				state = try_going_until_break(1400,COLOR_Y(2400),TRIANGLE3,TRIANGLE3_AVANCER,ERROR,FAST,sensRobot,NO_AVOIDANCE);
				break;
			case TRIANGLE3_AVANCER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Open);
				state = try_going_until_break(900,COLOR_Y(2400),TRIANGLE3_AVANCER,TRIANGLE4_5,ERROR,FAST,sensRobot,NO_AVOIDANCE);
				break;
			case TRIANGLE4_5:
					state = try_going_multipoint(&deplacement[6],6,TRIANGLE4_5,TRIANGLE6,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
				break;
			case TRIANGLE6:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Close);

				state = try_going_until_break(900,COLOR_Y(600),TRIANGLE6,TRIANGLE6_AVANCER,ERROR,FAST,sensRobot,NO_AVOIDANCE);
				break;
			case TRIANGLE6_AVANCER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Open);

				state = try_going_until_break(1200,COLOR_Y(600),TRIANGLE6_AVANCER,LANCE_LAUNCHER,ERROR,FAST,sensRobot,NO_AVOIDANCE);
				break;


				// Balles et fresques
			case LANCE_LAUNCHER:
				if(entrance)
					ACT_fruit_mouth_goto(ACT_FRUIT_Close);

				state = check_sub_action_result(strat_lance_launcher(FALSE),LANCE_LAUNCHER,POINT_M0,ERROR);
				break;
			case POINT_M0:
				state = PATHFIND_try_going(M0, POINT_M0, DEPOSER_FRESQUE, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_BREAK);
				break;
			case DEPOSER_FRESQUE:
				state = check_sub_action_result(strat_manage_fresco(),DEPOSER_FRESQUE,LANCE_LAUNCHER_ENNEMY,ERROR);
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
		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		AGAIN_RAMASSER_FRUIT_ARBRE1,
		AGAIN_RAMASSER_FRUIT_ARBRE2,
		POINT_DEPOSE_FRUIT,
		DEPOSER_FRUIT,
		POINT_DEPOSE_FRESCO,
		DEPOSE_FRESCO,
		LANCE_LAUNCHER,
		LANCE_LAUNCHER_ENNEMY,
		POS_FIN,
		VERIFY,
		DONE,
		ERROR
	);

	static bool_e presenceFruit = FALSE;

	if(TIME_MATCH_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch(state){
			case IDLE:
				state = POS_DEPART;
				break;
			case POS_DEPART:
				state = try_going_until_break(450,COLOR_Y(350),POS_DEPART,LANCE_LAUNCHER,LANCE_LAUNCHER,FAST,(global.env.color == RED)? BACKWARD:FORWARD,NO_DODGE_AND_NO_WAIT);
				break;


				// Premier shoot sur notre mammouth
			case LANCE_LAUNCHER:
				state = check_sub_action_result(strat_lance_launcher(FALSE),LANCE_LAUNCHER,RAMASSER_FRUIT_ARBRE1,RAMASSER_FRUIT_ARBRE1);
				break;


				// Rammasse notre groupe de fruit
			case RAMASSER_FRUIT_ARBRE1:
				if(global.env.color == RED)
					state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((pointIsShotter(Z1,W3)==TRUE)? TRUE:FALSE),RAMASSER_FRUIT_ARBRE1,POINT_DEPOSE_FRESCO,POINT_DEPOSE_FRESCO);
				else
					state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((pointIsShotter(A1,C3)==TRUE)? TRUE:FALSE),RAMASSER_FRUIT_ARBRE1,POINT_DEPOSE_FRESCO,POINT_DEPOSE_FRESCO);

				if(strat_fruit_sucess == TRUE){
					presenceFruit = TRUE;
					action_recup_fruit_group_1 = TRUE;
				}

				break;


				// Depose fresque
			case POINT_DEPOSE_FRESCO:
				state = PATHFIND_try_going(M0, POINT_DEPOSE_FRESCO, DEPOSE_FRESCO, POINT_DEPOSE_FRESCO, ANY_WAY, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
				break;
			case DEPOSE_FRESCO:
				state = check_sub_action_result(strat_manage_fresco(),DEPOSE_FRESCO,LANCE_LAUNCHER_ENNEMY,ERROR);

				if(state == LANCE_LAUNCHER_ENNEMY)
					action_fresco_filed = TRUE;

				break;


				// Tire seconde balle
			case LANCE_LAUNCHER_ENNEMY:
				state = check_sub_action_result(strat_lance_launcher_ennemy(),LANCE_LAUNCHER_ENNEMY,RAMASSER_FRUIT_ARBRE2,ERROR);
				break;


				// Rammasse second groupe de fruit
			case RAMASSER_FRUIT_ARBRE2:
				if(global.env.color == RED)
					state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((pointIsShotter(Z1,W3)==TRUE)? TRUE:FALSE),RAMASSER_FRUIT_ARBRE2,POINT_DEPOSE_FRUIT,ERROR);
				else
					state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((pointIsShotter(A1,C3)==TRUE)? TRUE:FALSE),RAMASSER_FRUIT_ARBRE2,POINT_DEPOSE_FRUIT,ERROR);

				if(strat_fruit_sucess == TRUE){
					presenceFruit = TRUE;
					action_recup_fruit_group_2 = TRUE;
				}

				break;


				// Depose de fruit
			case POINT_DEPOSE_FRUIT:
				if(presenceFruit == FALSE){
					state = VERIFY;
				}else
					state = PATHFIND_try_going(M0, POINT_DEPOSE_FRUIT, DEPOSER_FRUIT, ERROR, ANY_WAY, FAST, NO_DODGE_AND_NO_WAIT, END_AT_BREAK);
				break;
			case DEPOSER_FRUIT:
				state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,VERIFY,ERROR);

				if(state == VERIFY)
					presenceFruit = FALSE;
				break;

			case VERIFY: {
				static bool_e tryRammasseFruit1 = FALSE;
				static bool_e tryRammasseFruit2 = FALSE;

				if(action_recup_fruit_group_1 == FALSE && tryRammasseFruit1 == FALSE){
					tryRammasseFruit1 = TRUE;
					state = AGAIN_RAMASSER_FRUIT_ARBRE1;
				}else if(action_recup_fruit_group_2 == FALSE && tryRammasseFruit2 == FALSE){
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
					state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((pointIsShotter(A1,C3)==TRUE)? TRUE:FALSE),AGAIN_RAMASSER_FRUIT_ARBRE1,VERIFY,VERIFY);
				else
					state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((pointIsShotter(Z1,W3)==TRUE)? TRUE:FALSE),AGAIN_RAMASSER_FRUIT_ARBRE1,VERIFY,VERIFY);

				if(strat_fruit_sucess == TRUE){
					presenceFruit = TRUE;
					action_recup_fruit_group_1 = TRUE;
				}

				break;

			case AGAIN_RAMASSER_FRUIT_ARBRE2:
				if(global.env.color == RED)
					state = check_sub_action_result(strat_ramasser_fruit_arbre2_double((pointIsShotter(Z1,W3)==TRUE)? TRUE:FALSE),AGAIN_RAMASSER_FRUIT_ARBRE2,VERIFY,VERIFY);
				else
					state = check_sub_action_result(strat_ramasser_fruit_arbre1_double((pointIsShotter(A1,C3)==TRUE)? TRUE:FALSE),AGAIN_RAMASSER_FRUIT_ARBRE2,VERIFY,VERIFY);

				if(strat_fruit_sucess == TRUE){
					presenceFruit = TRUE;
					action_recup_fruit_group_2 = TRUE;
				}

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

error_e strat_manage_fresco(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
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

	STOP_REQUEST_IF_CHANGE(entrance, &state, 2, (Uint8 []){DONE, ERROR});

	switch(state){

		case IDLE:
			if(action_fresco_filed){
				debug_printf("ERREUR, la fresque a déja été posé\n");
				return END_OK;
			}

			if(MODE_MANUAL_FRESCO){
				posY = POS_Y_MANUAL_FRESCO;
				state = FILE_FRESCO;
			}else if(ADVERSARY_DETECTED_HOKUYO == FALSE){//Pose les fresques au milieu si on a pas vu l'adversaire poser les siennes
				posY = 1500;
				state = FILE_FRESCO;
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
				else{    // Il y a une fresque de chaque coté par rapport au milieu
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
			default: // On ne sait jamais si aucun adversaire a été detecte
				posY = 1500;
				break;
			}

			debug_printf("psoY   %d\n",posY);
			state = FILE_FRESCO;

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

					if(posY > 1500) //Ne prend plus en compte les positions des adversaires eu precedement (Elles sont fausses sinon ne serait pas dans cet état)
						posY = POS_MIN_FRESCO;
					else
						posY = POS_MAX_FRESCO;
				}

				state = check_sub_action_result(strat_file_fresco(posY),LAST_CHANCE_FILE_FRESCO,VERIFICATION_2,ERROR);
			break;

		case VERIFICATION_2:
			if(FRESCO_1 && FRESCO_2)//fresque plus presente sur le support grace au capteur
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
			action_fresco_filed = TRUE;
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

error_e strat_file_fresco(Sint16 posY){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		WALL,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		END,
		END_IMPOSSIBLE,
		DONE,
		ERROR
	);

	static bool_e timeout=FALSE;

	STOP_REQUEST_IF_CHANGE(entrance, &state, 4, (Uint8 []){END, END_IMPOSSIBLE, DONE, ERROR});

	switch(state){
		case IDLE:
			if(posY > POS_MAX_FRESCO || posY < POS_MIN_FRESCO){
				debug_printf("ERREUR Mauvaise position envoyer en fresque\n");
				return NOT_HANDLED;
			}

			timeout=FALSE;

			state = WALL;
			break;

		case WALL:
			state = try_going(240,posY,WALL,PUSH_MOVE,ERROR,SLOW,BACKWARD,NO_DODGE_AND_WAIT);
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
			state = try_going_until_break(250,posY,END,DONE,END_IMPOSSIBLE,FAST,FORWARD,NO_DODGE_AND_NO_WAIT);
			break;

		case END_IMPOSSIBLE:
			if(global.env.pos.x > 200)
				state = ERROR;
			else
				state = END;
			break;

		case DONE:
			state = IDLE;
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

error_e strat_file_fruit(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_BEGINNING,
		POS_END,
		DONE,
		ERROR
	);

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

		state = POS_BEGINNING;

		break;
	case POS_BEGINNING:
		state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,FAST,sensRobot,DODGE_AND_WAIT);
		break;
	case POS_END:
		state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,DONE,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);

		if(entrance){
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
			ASSER_WARNER_arm_y(posOpen);
		}

		if(global.env.asser.reach_y){ // Ouvrir le bac à fruit pour les faire tomber et sortir le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_OPEN);
		}

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
			state = IDLE;
		return NOT_HANDLED;
		break;
	default:
		break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre1_double(bool_e sens){ //Commence côté mammouth si sens == true
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		COURBE,
		TREE_2,
		POS_FIN,
		DONE,
		ERROR
	);


	static displacement_t point[5]; // Pourrait creer une variable tempo

	static const Uint8 NBPOINT = 5;

	static displacement_t courbe[5];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
	case IDLE:
		strat_fruit_sucess = FALSE;

		if(global.env.color == RED){ // Dans ce sens là, fonctionne plutôt bien
			point[0] = (displacement_t){{1000,ELOIGNEMENT_ARBRE-10},FAST};
			point[1] = (displacement_t){{1500,ELOIGNEMENT_ARBRE},FAST};
			point[2] = (displacement_t){{1580,350},FAST};
			point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,480},FAST};
			point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,900},FAST};
		}else{ // Jaune
			point[0] = (displacement_t){{1000,ELOIGNEMENT_ARBRE-10},FAST};
			point[1] = (displacement_t){{1500,ELOIGNEMENT_ARBRE},FAST};
			point[2] = (displacement_t){{1580,350},FAST};
			point[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+10,480},FAST};
			point[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE+10,900},FAST};
		}

		for(i=0;i<NBPOINT;i++){
			if(sens == TRUE)
				courbe[i] = point[i];
			else
				courbe[i] = point[NBPOINT-1-i];
		}

		if(sens == TRUE)  // Modifie le sens
			sensRobot = BACKWARD;
		else
			sensRobot = FORWARD;

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,COURBE,ERROR,FAST,sensRobot,DODGE_AND_WAIT);
		break;
	case COURBE:
		state = try_going_multipoint(&courbe[1],2,COURBE,TREE_2,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);

		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
		break;
	case TREE_2:
		state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,TREE_2,POS_FIN,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
		break;
	case POS_FIN:
		state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
		break;
	case DONE:
		strat_fruit_sucess = TRUE;
		ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
		state = IDLE;
		return END_OK;
		break;
	case ERROR:
		ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
		state = IDLE;
		return NOT_HANDLED;
		break;
	default:
		break;
	}

	return IN_PROGRESS;
}

error_e strat_ramasser_fruit_arbre2_double(bool_e sens){ //Commence côté mammouth si sens == true
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		COURBE,
		TREE_2,
		POS_FIN,
		DONE,
		ERROR
	);

	static const Uint8 NBPOINT = 5;

	static displacement_t courbe[5];
	static way_e sensRobot;

	switch(state){
	case IDLE:

		strat_fruit_sucess = FALSE;

		if(sens==TRUE){ // GROS BIDOUILLAGE pour avoir les points a la recup
			if(global.env.color == RED){
				courbe[0] = (displacement_t){{1000,3000-ELOIGNEMENT_ARBRE},FAST};
				courbe[1] = (displacement_t){{1500,3000-ELOIGNEMENT_ARBRE},FAST};
				courbe[2] = (displacement_t){{1620,2625},FAST};
				courbe[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-20,2450},FAST};
				courbe[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-20,1800},FAST};
			}else{
				courbe[0] = (displacement_t){{1000,3000-ELOIGNEMENT_ARBRE},FAST};
				courbe[1] = (displacement_t){{1500,3000-ELOIGNEMENT_ARBRE},FAST};
				courbe[2] = (displacement_t){{1620,2625},FAST};
				courbe[3] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-10,2450},FAST};
				courbe[4] = (displacement_t){{2000-ELOIGNEMENT_ARBRE-10,1800},FAST};
			}
		}else{
			courbe[0] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,1800},FAST};
			courbe[1] = (displacement_t){{2000-ELOIGNEMENT_ARBRE,2450},FAST};
			courbe[2] = (displacement_t){{1620,2575},FAST};
			courbe[3] = (displacement_t){{1500,3000-ELOIGNEMENT_ARBRE},FAST};
			courbe[4] = (displacement_t){{1000,3000-ELOIGNEMENT_ARBRE},FAST};
		}


		if(sens == TRUE)  // Modifie le sens
			sensRobot = FORWARD;
		else
			sensRobot = BACKWARD;

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,COURBE,ERROR,FAST,sensRobot,DODGE_AND_WAIT);
		break;
	case COURBE:
		state = try_going_multipoint(&courbe[1],2,COURBE,TREE_2,ERROR,sensRobot,NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);

		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
		break;
	case TREE_2:
		state = try_going(courbe[NBPOINT-2].point.x,courbe[NBPOINT-2].point.y,TREE_2,POS_FIN,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
		break;
	case POS_FIN:
		state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,FAST,sensRobot,NO_DODGE_AND_NO_WAIT);
		break;
	case DONE:
		strat_fruit_sucess = TRUE;
		ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
		state = IDLE;
		return END_OK;
		break;
	case ERROR:
		state = IDLE;
		ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
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
		POS_BEGINNING,
		LANCE_LAUNCHER,
		POS_END,
		DONE,
		ERROR
	);

	static GEOMETRY_point_t dplt[2]; // Deplacement
	static way_e sensRobot;
	static Uint16 posShoot; // Position à laquelle, on va tirer les balles
	static Uint16 sensShoot; // Vrai si il va de rouge vers jaune sinon faux

	switch(state){
	case IDLE:
		 if(global.env.pos.y > 750  && global.env.color == RED){ // Rouge milieu
			dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[0].y = 1200;

			dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[1].y = 300;

			sensRobot = FORWARD;
			sensShoot = FALSE;
			posShoot = 800;

		}else if(global.env.color == RED){	// Rouge et case depart
			dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[0].y = 500;

			dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[1].y = 1200;

			sensRobot = BACKWARD;
			sensShoot = TRUE;
			posShoot = 660;
		}else if(global.env.pos.y > 2225){ // Jaune, case depart
			dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[0].y = 2500;

			dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[1].y = 1800;

			sensRobot = FORWARD;
			sensShoot = FALSE;
			posShoot = 2340;

		}else{ // Jaune milieu
			dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[0].y = 1800;

			dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
			dplt[1].y = 2700;

			sensRobot = BACKWARD;
			sensShoot = TRUE;
			posShoot = 2100;

		}

		state = POS_BEGINNING;
		break;
	case POS_BEGINNING:
		state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,FAST,sensRobot,DODGE_AND_WAIT);

		break;
	case POS_END:
		state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,DONE,ERROR,SPEED_LANCE_LAUNCHER,sensRobot,NO_DODGE_AND_NO_WAIT);

		if(entrance)
			ASSER_WARNER_arm_y(posShoot);

		if(global.env.asser.reach_y){
			if(lanceAll == TRUE)
				ACT_lance_launcher_run(ACT_Lance_ALL,sensShoot);
			else
				ACT_lance_launcher_run(ACT_Lance_5_BALL,sensShoot);
		}

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

error_e strat_lance_launcher_ennemy(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_BEGINNING,
		POS_END,
		DONE,
		ERROR
	);

	static GEOMETRY_point_t dplt[2]; // Deplacement
	static way_e sensRobot;
	static Uint16 sensShoot; // Vrai si il va de rouge vers jaune sinon faux
	static Uint16 posShoot; // Position à laquelle, on va tirer les balles

	switch(state){
	case IDLE:
		if(global.env.pos.y > 2225 && global.env.color == RED){ // Va tirer sur le Jaune, case depart jaune
		   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[0].y = 2700;

		   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[1].y = 1800;

		   sensRobot = FORWARD;
		   sensShoot = FALSE;
		   posShoot = 2500;

		}else if(global.env.color == RED){ // Tire sur le Jaune milieu
		   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[0].y = 1800;

		   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[1].y = 2700;

		   sensRobot = BACKWARD;
		   sensShoot = TRUE;
		   posShoot = 2200;

	   }else if(global.env.pos.y > 750){ // tire sur le Rouge milieu
		   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[0].y = 1200;

		   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[1].y = 300;

		   sensRobot = FORWARD;
		   sensShoot = FALSE;
		   posShoot = 700;

	   }else{	// Tire Rouge et case depart
		   dplt[0].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[0].y = 300;

		   dplt[1].x = ELOIGNEMENT_SHOOT_BALL;
		   dplt[1].y = 1200;

		   sensRobot = BACKWARD;
		   sensShoot = TRUE;
		   posShoot = 700;
	   }

		state = POS_BEGINNING;
		break;
	case POS_BEGINNING:
		state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,FAST,sensRobot,DODGE_AND_WAIT);

		break;
	case POS_END:
		state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,DONE,ERROR,SPEED_LANCE_LAUNCHER,sensRobot,NO_DODGE_AND_NO_WAIT);

		if(entrance)
			ASSER_WARNER_arm_y(posShoot);

		if(global.env.asser.reach_y)
			ACT_lance_launcher_run(ACT_Lance_1_BALL,sensShoot);

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

void strat_test_vide(){

	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		AVANCER,
		DONE
	);

	if(TIME_MATCH_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch (state){
			case INIT :
				state = AVANCER;
				break;

			case AVANCER :
				state = try_going(1000, 2500, AVANCER, DONE, DONE, SLOW, ANY_WAY, NO_AVOIDANCE);
				break;

			case DONE :
				break;
		}
}

//resultat dans ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);

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


// Compare deux nodes et retourne TRUE si il est le plus près
static bool_e pointIsShotter(pathfind_node_id_t n1,pathfind_node_id_t n2){
	if(PATHFIND_manhattan_dist(PATHFIND_get_node_x(n1),PATHFIND_get_node_y(n1),global.env.pos.x,global.env.pos.y) < PATHFIND_manhattan_dist(PATHFIND_get_node_x(n2),PATHFIND_get_node_y(n2),global.env.pos.x,global.env.pos.y))
		return TRUE;

	return FALSE;
}
