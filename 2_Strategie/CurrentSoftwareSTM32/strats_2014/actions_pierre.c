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
#define ELOIGNEMENT_ARBRE 350
#define POS_MIN_FRESCO 1300
#define POS_MAX_FRESCO 1700


//Pour Activer le mode manual de pose de fresque
#define MODE_MANUAL_FRESCO FALSE
#define POS_Y_MANUAL_FRESCO 1300 // Mettre une valeur entre 1300 et 1700


//Les differente's actions que pierre devra faire lors d'un match
bool_e action_fresco_filed = FALSE;



//Provisoire pour le moment juste pour test
#define ADVERSARY_DETECTED_HOKUYO FALSE
#define FRESQUE_ENLEVER_APRS_1_COUP TRUE
#define NB_MAX_ADVERSARY_FRESCO_POSITION   2
volatile Uint16 adversary_fresco_positions[NB_MAX_ADVERSARY_FRESCO_POSITION]={1370,1360};
volatile Uint8 adversary_fresco_index = 1;



void strat_test_triangle_cote_rouge(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		PASSAGE_ARMOIRE,
		TRIANGLE_BOUT_JAUNE,
		TRIANGLE3,
		MILIEU2,
		TRIANGLE4,
		TRIANGLE_BOUT_ROUGE,
		TOUR1J,
		TOUR2J,
		TOUR3J,
		TOUR4J,
		TOUR1R,
		TOUR2R,
		TOUR3R,
		TOUR4R,
		MUR,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		FIN,
		DONE,
		ERROR
	);

	displacement_t tabMamo[8] = {
		{{500,200},SLOW},
		{{800,750},SLOW},
		{{820,900},SLOW},//Triangle 1
		{{680,1370},SLOW},
		{{680,1650},SLOW},
		{{830,2090},SLOW},//Triangle 2
		{{880,2270},SLOW},
		{{1020,2365},SLOW}//Bout triangle Jaune
	};

	displacement_t tabArmoire[7] = {
		{{1210,2360},SLOW},
		{{1340,2100},SLOW},//Triangle 1
		{{1415,1645},SLOW},
		{{1415,1360},SLOW},
		{{1370,900},SLOW},//Triangle 2
		{{1305,720},SLOW},
		{{1100,625},SLOW}//Bout triangle rouge
	};

	CAN_msg_t msg;
	static bool_e timeout=FALSE;

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
		state = try_going_multipoint(tabMamo,8,POS_DEPART,PASSAGE_ARMOIRE,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case PASSAGE_ARMOIRE:
		state = try_going_multipoint(tabArmoire,7,PASSAGE_ARMOIRE,MUR,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
		break;
	case TRIANGLE_BOUT_JAUNE:
		state = try_going(1000,400+DECALAGE_LARGEUR,TRIANGLE_BOUT_JAUNE,TOUR1J,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE_BOUT_ROUGE:
		state = try_going(900,2600-DECALAGE_LARGEUR,TRIANGLE_BOUT_ROUGE,TOUR1R,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TOUR1J:
		state = try_go_angle(3*PI4096/2,TOUR1J,TOUR2J,ERROR,FAST);
		break;
	case TOUR2J:
		state = try_go_angle(0,TOUR2J,TOUR3J,ERROR,FAST);
		break;
	case TOUR3J:
		state = try_go_angle(PI4096/2,TOUR3J,TOUR4J,ERROR,FAST);
		break;
	case TOUR4J:
		state = try_go_angle(PI4096,TOUR4J,TRIANGLE3,ERROR,FAST);
		break;
	case TOUR1R:
		state = try_go_angle(3*PI4096/2,TOUR1R,TOUR2R,ERROR,FAST);
		break;
	case TOUR2R:
		state = try_go_angle(0,TOUR2R,TOUR3R,ERROR,FAST);
		break;
	case TOUR3R:
		state = try_go_angle(PI4096/2,TOUR3R,TOUR4R,ERROR,FAST);
		break;
	case TOUR4R:
		state = try_go_angle(PI4096,TOUR4R,MUR,ERROR,FAST);
		break;
	case MUR:
		state = try_going(400,1500,MUR,PUSH_MOVE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case PUSH_MOVE://Le fait forcer contre le mur pour poser la fresque
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,0,TRUE);//Le fait forcer en marche arriere
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = FIN;
		}
		break;
	case FIN:
		state = try_going(400,1500,FIN,DONE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
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
		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1(),RAMASSER_FRUIT_ARBRE1,DEPOSER_FRUIT_ROUGE,ERROR);
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
		DONE,
		ERROR
	);

	static Sint16 posY = 1500;

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
			if(adversary_fresco_positions[0] > 1500)
				posY = POS_MIN_FRESCO;
			else
				posY = POS_MAX_FRESCO;

			break;
		case 2:
			if(adversary_fresco_positions[0] > 1500 && adversary_fresco_positions[1] > 1500)
				posY = POS_MIN_FRESCO;
			else if(adversary_fresco_positions[0] < 1500 && adversary_fresco_positions[1] < 1500)
				posY = POS_MAX_FRESCO;
			else    //Pas fine
				posY = 1500;

			break;
		default: // On ne sait jamais si aucun adversaire a été detecte
			posY = 1500;
			break;
		}

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
				if(posY > 1500) //Ne prend plus en compte les positions des adversaires eu precedement (Elles sont fausses sinon ne serait pas dans cet état)
					posY = POS_MIN_FRESCO;
				else
					posY = POS_MAX_FRESCO;
			}

			state = check_sub_action_result(strat_file_fresco(posY),LAST_CHANCE_FILE_FRESCO,DONE,ERROR);
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
		state = try_going_until_break(400,posY,END,DONE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
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

error_e strat_test_ramasser_fruit_arbre1(){
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
		{{2000-ELOIGNEMENT_ARBRE,530},SLOW}
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
		state = try_going(2000-ELOIGNEMENT_ARBRE,900,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
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

//resultat dans ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);
