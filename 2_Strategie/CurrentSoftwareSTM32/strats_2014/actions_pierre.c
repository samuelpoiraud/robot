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

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */

#define DECALAGE_LARGEUR 200
#define ELOIGNEMENT_ARBRE 360
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
		TOUR,
		FIN,
		DONE,
		ERROR
	);

	displacement_t tour[6] = {
		{{630,600},SLOW},
		{{630,2250},SLOW},
		{{960,2600},SLOW},
		{{1500,2300},SLOW},
		{{1500,750},SLOW},
		{{1200,450},SLOW}
	};

	CAN_msg_t msg;

	static Uint16 i = 0;

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
		state = try_going_until_break(500,200,POS_DEPART,TOUR,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case TOUR:
		state = try_going_multipoint(tour,6,TOUR,FIN,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case FIN:
		i++;

		if(i >= 1)
			state = DONE;
		else
			state = TOUR;

		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}

}

void strat_lannion_rouge(void){
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

	displacement_t triangle1[3] = {
		{{800,750},FAST},
		{{820,900},FAST},
		{{750,1200},FAST}
	};


	displacement_t triangle2[3] = {
		{{680,1650},FAST},
		{{830,2090},FAST},
		{{850,2250},FAST},
		//{{1000,2200},SLOW}
	};

	displacement_t tabArmoire[6] = {
		{{1280,2360},FAST},
		{{1340,2100},FAST},
		{{1415,1645},FAST},
		{{1415,1360},FAST},
		{{1370,900},FAST},
		{{1305,720},FAST},
		//{{1000,650},SLOW}
	};

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
		state = try_going_until_break(500,300,POS_DEPART,TRIANGLE1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE1:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);

		state = try_going_multipoint(triangle1,3,TRIANGLE1,FRESQUE,ERROR,BACKWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
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

		state = try_going_multipoint(triangle2,3,TRIANGLE2,TRIANGLE3,ERROR,BACKWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case TRIANGLE3:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Close);

		state = try_going_until_break(1400,2400,TRIANGLE3,TRIANGLE3_AVANCER,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE3_AVANCER:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);
		state = try_going_until_break(900,2400,TRIANGLE3_AVANCER,TRIANGLE4_5,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE4_5:
		state = try_going_multipoint(tabArmoire,6,TRIANGLE4_5,TRIANGLE6,ERROR,BACKWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case TRIANGLE6:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Close);

		state = try_going_until_break(900,600,TRIANGLE6,TRIANGLE6_AVANCER,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE6_AVANCER:
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);

		state = try_going_until_break(1200,600,TRIANGLE6_AVANCER,DONE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}

}

void strat_JPO(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		POINT_A1, // Point arbre1A cote couloir
		POINT_B3, // Point arbre1B rouge cote armoire
		POINT_W3, // Point arbre2b cote jaune armoire
		POINT_CO, // Pour deposser fruit
		POINT_M0,
		DEPOSER_FRESQUE,
		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		RAMASSER_FRUIT_ARBRE1A,
		RAMASSER_FRUIT_ARBRE1B,
		DEPOSER_FRUIT_ROUGE,
		DEPOSER_FRUIT_JAUNE,
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
		msg.data[2]=120 >> 8;
		msg.data[3]=120 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(500,300,POS_DEPART,POINT_A1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_A1:
		state = try_going(740,400,POINT_A1,RAMASSER_FRUIT_ARBRE1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case RAMASSER_FRUIT_ARBRE1:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1(),RAMASSER_FRUIT_ARBRE1,POINT_W3,ERROR);
		break;
	case POINT_W3:
		state = try_going(1670,1900,POINT_W3,RAMASSER_FRUIT_ARBRE2,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case RAMASSER_FRUIT_ARBRE2:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre2(),RAMASSER_FRUIT_ARBRE2,DEPOSER_FRUIT_JAUNE,ERROR);
		break;
	/*case RAMASSER_FRUIT_ARBRE1A:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1A(),RAMASSER_FRUIT_ARBRE1A,RAMASSER_FRUIT_ARBRE1B,ERROR);
		break;
	case POINT_B3:
		state = try_going(1600,650,POINT_B3,RAMASSER_FRUIT_ARBRE1B,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case RAMASSER_FRUIT_ARBRE1B:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1B(),RAMASSER_FRUIT_ARBRE1B,POINT_CO,ERROR);
		break;
	case POINT_CO:
		state = try_going(500,1100,POINT_CO,DEPOSER_FRUIT,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;*/
	case DEPOSER_FRUIT_ROUGE:
		state = check_sub_action_result(strat_test_deposser_fruit_rouge(),DEPOSER_FRUIT_ROUGE,DONE,ERROR);
		break;
	case DEPOSER_FRUIT_JAUNE:
		state = check_sub_action_result(strat_test_deposser_fruit_jaune(),DEPOSER_FRUIT_JAUNE,POINT_M0,ERROR);
		break;
	case POINT_M0:
		state = try_going(400,1500,POINT_M0,DEPOSER_FRESQUE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DEPOSER_FRESQUE:
		state = check_sub_action_result(strat_file_fresco(1500),DEPOSER_FRESQUE,DONE,ERROR);
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
		DEPOSER_FRUIT_ROUGE,
		DEPOSER_FRUIT_JAUNE,
		LANCE_LAUNCHER,
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
		msg.data[2]=120 >> 8;
		msg.data[3]=120 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(500,300,POS_DEPART,LANCE_LAUNCHER,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_A1:
		state = try_going(740,400,POINT_A1,RAMASSER_FRUIT_ARBRE1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case RAMASSER_FRUIT_ARBRE1:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1_double(TRUE),RAMASSER_FRUIT_ARBRE1,POINT_W3,ERROR);
		break;
	case POINT_W3:
		state = try_going(1670,1900,POINT_W3,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case LANCE_LAUNCHER:
		state = check_sub_action_result(strat_lance_launcher(),LANCE_LAUNCHER,DONE,ERROR);
		break;
	case RAMASSER_FRUIT_ARBRE2:
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre2(),RAMASSER_FRUIT_ARBRE2,DEPOSER_FRUIT_JAUNE,ERROR);
		break;
	case DEPOSER_FRUIT_ROUGE:
		state = check_sub_action_result(strat_test_deposser_fruit_rouge(),DEPOSER_FRUIT_ROUGE,DONE,ERROR);
		break;
	case DEPOSER_FRUIT_JAUNE:
		state = check_sub_action_result(strat_test_deposser_fruit_jaune(),DEPOSER_FRUIT_JAUNE,DONE,ERROR);
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

error_e strat_test_deposser_fruit_rouge(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		AVANCER,
		DEBUT,
		TRAVERSE_BAC,
		FIN,
		DONE,
		ERROR
	);

	switch(state){
	case IDLE:
		state = AVANCER;
		break;
	case AVANCER:
		state = try_going(470,1250,AVANCER,DEBUT,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case DEBUT:
		state = try_going_until_break(470,1050,DEBUT,TRAVERSE_BAC,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case TRAVERSE_BAC:
		state = try_going_until_break(470,300,TRAVERSE_BAC,FIN,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);
		break;
	case FIN:
		state = try_going_until_break(750,400,FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Close);

		//state = check_act_status(ACT_QUEUE_Fruit,FERMER_BRAS,DONE,ERROR);
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

error_e strat_test_deposser_fruit_jaune(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		AVANCER,
		DEBUT,
		TRAVERSE_BAC,
		FIN,
		DONE,
		ERROR
	);

	switch(state){
	case IDLE:
		state = AVANCER;
		break;
	case AVANCER:
		state = try_going(470,2750,AVANCER,DEBUT,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case DEBUT:
		state = try_going_until_break(470,2550,DEBUT,TRAVERSE_BAC,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case TRAVERSE_BAC:
		state = try_going_until_break(470,1950,TRAVERSE_BAC,FIN,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Open);
		break;
	case FIN:
		state = try_going_until_break(750,1800,FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Close);

		//state = check_act_status(ACT_QUEUE_Fruit,FERMER_BRAS,DONE,ERROR);
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

error_e strat_test_ramasser_fruit_arbre1(){ //Commence côté mammouth
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		COURBE,
		POS_FIN,
		DONE,
		ERROR
	);

	displacement_t courbe[4] = {
		{{1450,ELOIGNEMENT_ARBRE},FAST},
		{{1550,380},SLOW},
		{{1600,440},SLOW},
		{{1630,530},SLOW}
	};


	switch(state){
	case IDLE:
		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(1200,ELOIGNEMENT_ARBRE,POS_DEPART,COURBE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case COURBE:
		state = try_going_multipoint(courbe,4,COURBE,POS_FIN,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);

		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
		break;
	case POS_FIN:
		state = try_going(1630,900,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case DONE:
		ACT_fruit_mouth_goto(ACT_FRUIT_Close);
		return END_OK;
		break;
	case ERROR:
		//ACT_fruit_mouth_goto(ACT_FRUIT_Close);
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
			ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
		break;
	case POS_FIN:
			state = try_going(courbe[NBPOINT-1].point.x,courbe[NBPOINT-1].point.y,POS_FIN,DONE,ERROR,SLOW,sensRobot,NO_AVOIDANCE);
		break;
	case DONE:
		ACT_fruit_mouth_goto(ACT_FRUIT_Close);
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

error_e strat_test_ramasser_fruit_arbre2(){


	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		COURBE,
		POS_FIN,
		DONE,
		ERROR
	);

	displacement_t courbe[3] = {
		{{2000-ELOIGNEMENT_ARBRE,2450},FAST},
		//{{1600,2500},SLOW},
		{{1550,2600},SLOW},
		{{1300,3000-ELOIGNEMENT_ARBRE},SLOW},
	};


	switch(state){
	case IDLE:
		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(2000-ELOIGNEMENT_ARBRE,2100,POS_DEPART,COURBE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case COURBE:
		state = try_going_multipoint(courbe,3,COURBE,POS_FIN,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);

		if(entrance)
			ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
		break;
	case POS_FIN:
		state = try_going(1050,3000-ELOIGNEMENT_ARBRE,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case DONE:
		ACT_fruit_mouth_goto(ACT_FRUIT_Close);
		return END_OK;
		break;
	case ERROR:
		//ACT_fruit_mouth_goto(ACT_FRUIT_Close);
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
			ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
		break;
	case POS_FIN:
		state = try_going(1050,3000-ELOIGNEMENT_ARBRE,POS_FIN,DONE,ERROR,SLOW,sensRobot,NO_AVOIDANCE);
		break;
	case DONE:
		ACT_fruit_mouth_goto(ACT_FRUIT_Close);
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
		POS_DEPART,
		FACE_MUR,
		LANCE_LAUNCHER,
		DONE,
		ERROR
	);

	CAN_msg_t msg;

	switch(state){
	case IDLE:
		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going(500,650,POS_DEPART,FACE_MUR,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case FACE_MUR:
		state = try_go_angle(PI4096/2,FACE_MUR,LANCE_LAUNCHER,ERROR,SLOW);
		break;
	case LANCE_LAUNCHER:

		msg.size = 1;
		msg.sid = ACT_LANCELAUNCHER;
		msg.data[0] = ACT_LANCELAUNCHER_RUN;
		CAN_send(&msg);
		debug_printf("Main: ACT_LANCELAUNCHER_RUN_1\n");

		state = DONE;
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
