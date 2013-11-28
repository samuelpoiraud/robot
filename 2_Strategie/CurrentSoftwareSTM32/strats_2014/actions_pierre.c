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

#define DECALAGE_LARGEUR 140

void strat_test_triangle_cote_rouge(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		AVANCER,
		POS_DEPART,
		TRIANGLE1,
		TRIANGLE2,
		TRIANGLE_BOUT_JAUNE,
		TRIANGLE3,
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

	CAN_msg_t msg;
	static bool_e timeout=FALSE;

	switch(state){
	case IDLE:
		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=1000 >> 8;
		msg.data[1]=1000 & 0xFF;
		msg.data[2]=2880 >> 8;
		msg.data[3]=2880 & 0xFF;
		msg.data[4]=-PI4096/2 >> 8;
		msg.data[5]=-PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = AVANCER;
		break;
	case AVANCER:
		//debug_printf("\nGlobale variable de x %d\n	de y %d\n",global.env.pos.x,global.env.pos.y);
	state = try_going(1000,2700,AVANCER,POS_DEPART,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POS_DEPART:
		state = try_going(1500,2600,POS_DEPART,TRIANGLE1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE1:
		state = try_going(1400-DECALAGE_LARGEUR,2200,TRIANGLE1,TRIANGLE2,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE2:
		state = try_going(1400-DECALAGE_LARGEUR,800,TRIANGLE2,TRIANGLE_BOUT_JAUNE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE_BOUT_JAUNE:
		state = try_going(1000,500+DECALAGE_LARGEUR,TRIANGLE_BOUT_JAUNE,TOUR1J,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE3:
		state = try_going(550+DECALAGE_LARGEUR,800,TRIANGLE3,TRIANGLE4,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE4:
		state = try_going(550+DECALAGE_LARGEUR,2200,TRIANGLE4,TRIANGLE_BOUT_ROUGE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case TRIANGLE_BOUT_ROUGE:
		state = try_going(900,2500-DECALAGE_LARGEUR,TRIANGLE_BOUT_ROUGE,TOUR1R,ERROR,FAST,FORWARD,NO_AVOIDANCE);
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
		state = try_going(1600,1500,MUR,PUSH_MOVE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal réglé
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,PI4096,TRUE);//Le fait forcer en marche avant pour protéger les pinces à l'arriére
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = FIN;
		}
		break;
	case FIN:
		state = try_going(1600,1500,FIN,DONE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}

}
