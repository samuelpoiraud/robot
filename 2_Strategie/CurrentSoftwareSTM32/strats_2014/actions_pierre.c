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

void strat_test_ramasser_fruit(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		SOUS_ARBRE,
		FIN,
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
		state = try_going(500,300,POS_DEPART,SOUS_ARBRE,ERROR,FAST,FORWARD,NO_AVOIDANCE);

		if(entrance)
			fruit_bac_mid();
		break;
	case SOUS_ARBRE:
		state = try_going(1150,310,SOUS_ARBRE,DONE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}
}

void fruit_bac_open(){
	ACT_fruit_mouth_goto(ACT_FRUIT_Open);
}

void fruit_bac_mid(){
	ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
}

void fruit_bac_close(){
	ACT_fruit_mouth_goto(ACT_FRUIT_Close);
}

//resultat dans ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);
