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
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../act_can.h"

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test								 */
/* ----------------------------------------------------------------------------- */

#define DECALAGE_LARGEUR 200
#define ELOIGNEMENT_ARBRE 360
#define ELOIGNEMENT_POSE_BAC_FRUIT 500
#define POS_MIN_FRESCO 1300
#define POS_MAX_FRESCO 1700


//Pour Activer le mode manual de pose de fresque
#define MODE_MANUAL_FRESCO TRUE
#define POS_Y_MANUAL_FRESCO 1500 // Mettre une valeur entre 1300 et 1700


//Les differente's actions que pierre devra faire lors d'un match
bool_e action_fresco_filed = FALSE;



//Provisoire pour le moment juste pour test
#define ADVERSARY_DETECTED_HOKUYO TRUE
#define FRESQUE_ENLEVER_APRS_1_COUP TRUE
#define FRESQUE_ENLEVER_APRS_2_COUP FALSE
#define NB_MAX_ADVERSARY_FRESCO_POSITION   2 //Les positions devront etre compris entre 1700 et 1300
volatile Uint16 adversary_fresco_positions[NB_MAX_ADVERSARY_FRESCO_POSITION]={1450,1590};
volatile Uint8 adversary_fresco_index = 2;


void strat_inutile(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
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
		state = try_going_until_break(500,400,POS_DEPART,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
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

	CAN_msg_t msg;

	static way_e sensRobot;

	switch(state){
	case IDLE:
		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=COLOR_Y(120) >> 8;
		msg.data[3]=COLOR_Y(120) & 0xFF;

		if(global.env.color == RED){
			msg.data[4]=PI4096/2 >> 8;
			msg.data[5]=PI4096/2 & 0xFF;

		}else{
			msg.data[4]=-PI4096/2 >> 8;
			msg.data[5]=-PI4096/2 & 0xFF;
		}

		msg.size = 6;
		CAN_send(&msg);

		if(global.env.color == RED)
			sensRobot = BACKWARD;
		else
			sensRobot = FORWARD;

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going_until_break(500,COLOR_Y(300),POS_DEPART,TRIANGLE1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE1:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);

		state = try_going_multipoint(deplacement,3,TRIANGLE1,FRESQUE,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case FRESQUE:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Close);

		state = check_sub_action_result(strat_manage_fresco(),FRESQUE,AVANCER_FRESQUE,ERROR);
		break;
	case AVANCER_FRESQUE:
		state = try_going_until_break(600,1500,AVANCER_FRESQUE,TRIANGLE2,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE2:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);

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

		state = try_going_until_break(1200,COLOR_Y(600),TRIANGLE6_AVANCER,DONE,ERROR,FAST,sensRobot,NO_AVOIDANCE);
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
		POINT_W3, // Point arbre2b cote jaune armoire
		POINT_CO, // Pour deposser fruit
		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		RAMASSER_FRUIT_ARBRE1A,
		RAMASSER_FRUIT_ARBRE1B,
		DEPOSER_FRUIT,
		DEPOSER_FRUIT_ROUGE,
		DEPOSER_FRUIT_JAUNE,
		LANCE_LAUNCHER,
		LANCE_LAUNCHER_ENNEMY,
		POS_FIN,
		DONE,
		ERROR
	);


	CAN_msg_t msg;

	switch(state){
	case IDLE:

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=COLOR_Y(120) >> 8;
		msg.data[3]=COLOR_Y(120) & 0xFF;

		if(global.env.color == RED){
			msg.data[4]=PI4096/2 >> 8;
			msg.data[5]=PI4096/2 & 0xFF;

		}else{
			msg.data[4]=-PI4096/2 >> 8;
			msg.data[5]=-PI4096/2 & 0xFF;
		}

		msg.size = 6;
		CAN_send(&msg);
		debug_printf("Envoie Message ACtionneur\n");
		ACT_lance_launcher_run(ACT_Lance_ALL);

		state = DONE;

		break;
	case POS_DEPART:
		state = try_going(500,COLOR_Y(300),POS_DEPART,LANCE_LAUNCHER,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_A1:
		state = try_going(1000,COLOR_Y(1500),POINT_A1,DEPOSER_FRUIT,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case DEPOSER_FRUIT:
		state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,DONE,ERROR);
		break;
	case RAMASSER_FRUIT_ARBRE1:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1_double(TRUE),RAMASSER_FRUIT_ARBRE1,POINT_W3,ERROR);
		break;
	case POINT_W3:
		state = try_going(1670,1900,POINT_W3,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case LANCE_LAUNCHER:
		state = check_sub_action_result(strat_lance_launcher(),LANCE_LAUNCHER,LANCE_LAUNCHER_ENNEMY,ERROR);
		break;
	case LANCE_LAUNCHER_ENNEMY:
		state = check_sub_action_result(strat_lance_launcher_ennemy(),LANCE_LAUNCHER_ENNEMY,DONE,ERROR);
		break;
	case RAMASSER_FRUIT_ARBRE2:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre2_double(TRUE),RAMASSER_FRUIT_ARBRE2,DEPOSER_FRUIT_JAUNE,ERROR);
		break;
	/*case DEPOSER_FRUIT_ROUGE:
		state = check_sub_action_result(strat_test_deposser_fruit_rouge(),DEPOSER_FRUIT_ROUGE,DONE,ERROR);
		break;
	case DEPOSER_FRUIT_JAUNE:
		state = check_sub_action_result(strat_test_deposser_fruit_jaune(),DEPOSER_FRUIT_JAUNE,DONE,ERROR);
		break;*/
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

		state = try_going(200,posY,WALL,PUSH_MOVE,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
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
		state = try_going_until_break(250,posY,END,DONE,END_IMPOSSIBLE,FAST,FORWARD,NO_AVOIDANCE);
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

			sensRobot = FORWARD;
			posOpen = 2500;

		}else if(global.env.color == RED){ // Commence au milieu du terrain en étant Rouge
			dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[0].y = 1800;

			dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[1].y = 2700;

			sensRobot = BACKWARD;
			posOpen = 2000;

		}else if(global.env.pos.y > 750){ // Il est de couleur Jaune commence au milieu
			dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[0].y = 1200;

			dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[1].y = 300;

			sensRobot = FORWARD;
			posOpen = 1000;

		}else{							// Va commencer en bas
			dplt[0].x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[0].y = 300;

			dplt[1].x = ELOIGNEMENT_POSE_BAC_FRUIT;
			dplt[1].y = 1200;

			sensRobot = BACKWARD;
			posOpen = 500;
		}

		state = POS_BEGINNING;

		break;
	case POS_BEGINNING:
		state = try_going(dplt[0].x,dplt[0].y,POS_BEGINNING,POS_END,ERROR,SLOW,sensRobot,NO_AVOIDANCE);
		break;
	case POS_END:
		state = try_going_until_break(dplt[1].x,dplt[1].y,POS_END,DONE,ERROR,SLOW,sensRobot,NO_AVOIDANCE);

		if(entrance){
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
			ASSER_WARNER_arm_y(posOpen);
		}

		if(global.env.asser.reach_y){ // Ouvrir le bac à fruit pour les faire tomber et sortir le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_OPEN);
		}

		break;
	case DONE: // Fermer le bac à fruit et rentrer le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_CLOSE);
		return END_OK;
		break;
	case ERROR: // Fermer le bac à fruit et rentrer le bras
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
			ACT_fruit_mouth_goto(ACT_FRUIT_LABIUM_CLOSE);
		return NOT_HANDLED;
		break;
	default:
		break;
	}

	return IN_PROGRESS;
}

error_e strat_test_ramasser_fruit_arbre1_double(bool_e sens){ //Commence côté mammouth si sens == true
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		COURBE,
		POS_FIN,
		DONE,
		ERROR
	);

	static displacement_t point[] = {
		{{1200,ELOIGNEMENT_ARBRE},SLOW},
		{{1450,ELOIGNEMENT_ARBRE},SLOW},
//		{{1555,395},SLOW},
//		{{1610,450},SLOW},
		{{1580,422},SLOW},

		{{1630,530},SLOW},
		{{1630,900},SLOW},
	};

	static const Uint8 NBPOINT = sizeof(point)/sizeof(displacement_t);

	static displacement_t courbe[sizeof(point)/sizeof(displacement_t)];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
	case IDLE:

		for(i=0;i<NBPOINT;i++){
			if(sens == TRUE)
				courbe[i] = point[i];
			else
				courbe[i] = point[NBPOINT-1-i];
		}

		if(sens == TRUE)
			sensRobot = FORWARD;
		else
			sensRobot = BACKWARD;

		state = POS_DEPART;
		break;
	case POS_DEPART:
			state = try_going(courbe[0].point.x,courbe[0].point.y,POS_DEPART,COURBE,ERROR,SLOW,sensRobot,NO_AVOIDANCE);
		break;
	case COURBE:
			state = try_going_multipoint(&courbe[1],4,COURBE,POS_FIN,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);

		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
		break;
	case POS_FIN:
			state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,SLOW,sensRobot,NO_AVOIDANCE);
		break;
	case DONE:
		ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
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

error_e strat_test_ramasser_fruit_arbre2_double(bool_e sens){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		COURBE,
		POS_FIN,
		DONE,
		ERROR
	);

	displacement_t point[6] = {
		{{1200,3000-ELOIGNEMENT_ARBRE},SLOW},
		{{1450,3000-ELOIGNEMENT_ARBRE},SLOW},
		{{1555,2605},SLOW},
		{{1610,2550},SLOW},
		{{2000-ELOIGNEMENT_ARBRE,2450},FAST},
		{{2000-ELOIGNEMENT_ARBRE,2100},SLOW},
	};

	static displacement_t courbe[6];
	Uint8 i;
	static way_e sensRobot;

	switch(state){
	case IDLE:

		for(i=0;i<6;i++){
			if(sens==TRUE)
				courbe[i] = point[i];
			else
				courbe[i] = point[5-i];
		}

		if(sens==TRUE)
			sensRobot = FORWARD;
		else
			sensRobot = BACKWARD;

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(2000-ELOIGNEMENT_ARBRE,2100,POS_DEPART,COURBE,ERROR,FAST,sensRobot,NO_AVOIDANCE);
		break;
	case COURBE:
		state = try_going_multipoint(&courbe[1],4,COURBE,POS_FIN,ERROR,sensRobot,NO_AVOIDANCE, END_AT_LAST_POINT);

		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_OPEN);
		break;
	case POS_FIN:
		state = try_going(1050,3000-ELOIGNEMENT_ARBRE,POS_FIN,DONE,ERROR,SLOW,sensRobot,NO_AVOIDANCE);
		break;
	case DONE:
		ACT_fruit_mouth_goto(ACT_FRUIT_MOUTH_CLOSE);
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

error_e strat_lance_launcher(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_BEGINNING,
		POS_END,
		DONE,
		ERROR
	);

	switch(state){
	case IDLE:
		state = POS_BEGINNING;
		break;
	case POS_BEGINNING:
		state = try_going(500,COLOR_Y(400),POS_BEGINNING,POS_END,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POS_END:
		state = try_going(500,COLOR_Y(1100),POS_END,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);

		if(entrance)
			ASSER_WARNER_arm_y(COLOR_Y(600-100)); //-100 largeur du robot

		if(global.env.asser.reach_y)
			ACT_lance_launcher_run(ACT_Lance_5_BALL);

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

	switch(state){
	case IDLE:
		state = POS_BEGINNING;
		break;
	case POS_BEGINNING:
		state = try_going(500,COLOR_Y(1600),POS_BEGINNING,POS_END,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POS_END:
		state = try_going(500,COLOR_Y(2300),POS_END,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);

		if(entrance)
			ASSER_WARNER_arm_y(COLOR_Y(2400-120)); //-100 largeur du robot

		if(global.env.asser.reach_y)
			ACT_lance_launcher_run(ACT_Lance_ALL);

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

error_e strat_launch_net(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		DONE,
		ERROR
	);

	switch(state){
	case IDLE:

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

//resultat dans ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);




void strat_test_filet(){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		LAUNCH,
		DONE
	);

	switch(state){
		case INIT :
			state = LAUNCH;
			break;

		case LAUNCH :
			ACT_filet_launch(ACT_FILET_LAUNCHED);
			debug_printf("Lancement du filet");
			state = DONE;
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
