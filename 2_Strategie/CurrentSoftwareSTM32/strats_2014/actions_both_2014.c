/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_both_2014.c
 *	Package : Carte S²/strats2014
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par .
 *	Version 2013/10/03
 */

#include "actions_both_2014.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../Pathfind.h"
#include "../avoidance.h"
#include "../QS/QS_who_am_i.h"

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#define LARGEUR_ROBOT 240

// Strat Rotation
#define DEFAULT_SPEED	(SLOW)
#define ODOMETRIE_PLAGE_ROTATION 5 //La variation de la plage sur PI4096(12868) pour savoir si on doit modifier l'angle. Théoriquement avec 10, on devrait avoir au maximum de 2mm de décalage sur un 1m
#define USE_CURVE	0
#define NB_TOUR_ODO_ROTATION 5


// Start Translation
#define ODOMETRIE_PLAGE_TRANSLATION 2 // + ou -



/* ----------------------------------------------------------------------------- */
/* 							Fonctions de réglage odométrique		             */
/* ----------------------------------------------------------------------------- */

void strat_reglage_odo_rotation(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_COEF_PROPULSION,
		IDLE,
		CALAGE,
		REINIT,
		ATTENTE,
		PROCESS,
		COMPARE_N_CORRECT,
		REPORT,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		AVANCER,
		TOUR1,
		TOUR2,
		TOUR3,
		DONE,
		ERROR
	);


	/******TEST******/
	//state = COMPARE_N_CORRECT;

	static enum state_e inProcess = IDLE;
	static bool_e timeout=FALSE;
	static Sint16 i=0;
	static Uint32 coefOdoRotation; // Peut etre 0xc581 //Original 0x0000C5A2; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;


	switch(state){
	case IDLE: //Cas d'attente et de réinitialisation

		if(QS_WHO_AM_I_get() == PIERRE)
			coefOdoRotation = 0x0000C581;
		else // GUY
			coefOdoRotation = 0x00010AC0;


		state = CALAGE;
		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'échapement dépend d'ou vient la machine
		//  in the wall
		switch(last_state)
		{
			case IDLE:	// On vient du début de la procédure on doit donc initialiser le robot
				state = REINIT;
				break;
			case PROCESS:	//On vient de finir le process on va donc traiter les valeurs
				state = PUSH_MOVE;
				break;
			case REINIT:	//On à remis le robot à zéro après un process, on relance la procédure
				state = AVANCER; // Ha merde jai oublié davancer
				break;
			case AVANCER:
				state = PROCESS;
				break;
			case WAIT_END_OF_MOVE:
				state = COMPARE_N_CORRECT;
				break;
			default:
				state = ERROR;
				break;
		}
		break;

	case REINIT:	// Envoie le message can pour réinitialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);  le robot doit se trouver a peut pres a set(2000,500,PI4096)
		i=0;
		debug_printf("REINIT\n\n");

		debug_printf("\nGlobale variable de x %d\n	de y %d\n l'angle %d\n",global.env.pos.x,global.env.pos.y,global.env.pos.angle);

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=1000 >> 8;	//Je lui dis qu'il est au milieu du terrain ( si je mets 0, il risque d'aller dans les négatifs)
		msg.data[1]=1000 & 0xFF;
		msg.data[2]=LARGEUR_ROBOT/2 >> 8;
		msg.data[3]=LARGEUR_ROBOT/2 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size=6;
		CAN_send(&msg);

		state = CALAGE;

		break;
	case AVANCER://pour le faire avancer du bord
		state = try_going(1000,500,AVANCER,CALAGE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);

		if(state==CALAGE)
			debug_printf("\nRENIT variable de x %d\n\t\t\tde y %x\n",global.env.pos.x,global.env.pos.y);
		break;
	case PROCESS:	// Tourne 10 fois pour comparer l'angle
		// Tourner 10 fois

		if(i < NB_TOUR_ODO_ROTATION){
			switch(inProcess){
			case IDLE:
				//debug_printf("IDLE\n");
				inProcess=try_go_angle(0,IDLE,TOUR1,ERROR,SLOW);
				break;
			case TOUR1:
				//debug_printf("TOUR1\n");
				inProcess=try_go_angle(3*PI4096/2,TOUR1,TOUR2,ERROR,SLOW);
				break;
			case TOUR2:
				//debug_printf("TOUR1\n");
				inProcess=try_go_angle(PI4096,TOUR2,TOUR3,ERROR,SLOW);
				break;
			case TOUR3:
				//debug_printf("TOUR2\n");
				inProcess=try_go_angle(PI4096/2,TOUR3,IDLE,ERROR,SLOW);
				if(inProcess == IDLE)
					i++; // On incremente le i apres avoir fait un tour complet
				break;
			default:
				state=ERROR;
				break;
			}
		}
		else{
			state=CALAGE;
		}

		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal réglé
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,PI4096/2,TRUE);//Le fait forcer en marche avant pour protéger les pinces à l'arriére
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
		}
		break;
	case COMPARE_N_CORRECT:
		// compare l'angle reçu après calage par rapport au ZERO
		//correction de la nouvelle odométrie
		// Envoi du nouveau coefficient à la propulsion
		//Renouvel le process autant de fois que nécessaire

		//Si il est bien réglé l'angle interne du robot devrait etre de zero, sinon l'angle interne du robot s'est décalé quand le robot a approché le mur

		if(global.env.pos.angle > (PI4096/2-ODOMETRIE_PLAGE_ROTATION) && global.env.pos.angle < (PI4096/2+ODOMETRIE_PLAGE_ROTATION)){//Le coef serait parfait global.env.pos.angle doit être égale à 90°
			debug_printf("Ne pas modifier l'angle globale est de %d \n\n",global.env.pos.angle);
			state=DONE;


		}else{//Recommencer procédure en modifiant les valeurs
			debug_printf("Modifier valeur l'angle globale est de %d \n\n",global.env.pos.angle);

			state = REINIT;

			//Modifier KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT selon l'angle obtenu
			if(global.env.pos.angle > PI4096/2){//Si l'angle est sup à 90°
				if((global.env.pos.angle-PI4096/2) > 500)
					coefOdoRotation+=10;
				else if((global.env.pos.angle-PI4096/2) > 250)
					coefOdoRotation+=5;
				else
					coefOdoRotation++;
				debug_printf("Augmentez le coef \n\n");
			}
			else{//Si l'angle est inf à 90°
				if((global.env.pos.angle-PI4096/2) < 500)
					coefOdoRotation-=10;
				else if((global.env.pos.angle-PI4096/2) < 250)
					coefOdoRotation-=5;
				else
					coefOdoRotation++;
				debug_printf("Diminuez le coef \n\n");
			}

			//Envoie du message CAN
			msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_ROTATION;
			msg.data[0] = coefOdoRotation >> 24;
			msg.data[1] = coefOdoRotation >> 16;
			msg.data[2] = coefOdoRotation >> 8;
			msg.data[3] = coefOdoRotation;
			msg.size=4;
			CAN_send(&msg);

			debug_printf("Nouvelle valeur du coef odométrie rotation %lx\n\n",coefOdoRotation);
		}

		break;

	case REPORT:	// Correspond à la partie IHM du processus: Le robot affiche le coeff calculé sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'écran LCD
		if(FALSE)// Mettre une condition associée à l'IHM: attend que l'utilisateur réponde
			state = IDLE;

		break;
	case DONE:
		break;

	case ERROR:
		break;
	default:
		break;
	}

}


void strat_reglage_odo_translation(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		CALAGE,
		REINIT,
		ATTENTE,
		DEMI_TOUR1,
		DEMI_TOUR2,
		COMPARE_N_CORRECT,
		REPORT,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		AVANCER,
		DONE,
		ERROR
	);


	/******TEST******/
	//state = COMPARE_N_CORRECT;

	static bool_e timeout=FALSE;
	static Uint16 coefOdoTranslation; //Original 0x0C47; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_TRANSLATION_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de réinitialisation
		state = CALAGE;

		if(QS_WHO_AM_I_get() == PIERRE)
			coefOdoTranslation = 0x0C43;
		else // GUY
			coefOdoTranslation = 0x0C10;

		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'échapement dépend d'ou vient la machine
		//  in the wall
		switch(last_state){
		case IDLE:	// On vient du début de la procédure on doit donc initialiser le robot
			state = REINIT;
			break;
		case REINIT:	//On à remis le robot à zéro après un process, on relance la procédure
			state = AVANCER; // Ha merde jai oublié davancer
			break;
		case AVANCER:
			state = DEMI_TOUR1;
			break;
		case DEMI_TOUR2:
			state = PUSH_MOVE;
			break;
		case WAIT_END_OF_MOVE:
			state = COMPARE_N_CORRECT;
			break;
		default:
			state = ERROR;
			break;
		}
		break;

	case REINIT:	// Envoie le message can pour réinitialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);  le robot doit se trouver a peut pres a set(2000,500,PI4096)

		debug_printf("REINIT\n\n");

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=1000 >> 8;	//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les négatifs)
		msg.data[1]=1000 & 0xFF;
		msg.data[2]=LARGEUR_ROBOT/2 >> 8;
		msg.data[3]=LARGEUR_ROBOT/2 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size=6;
		CAN_send(&msg);

		state = CALAGE;

		break;
	case AVANCER://pour le faire avancer du bord
		state = try_going(1000,2600,AVANCER,CALAGE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DEMI_TOUR1:
		state = try_go_angle(PI4096,DEMI_TOUR1,DEMI_TOUR2,ERROR,FAST);
		break;
	case DEMI_TOUR2:
		state = try_go_angle(3*PI4096/2,DEMI_TOUR2,CALAGE,ERROR,FAST);
		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal réglé
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,3*PI4096/2,TRUE);//Le fait forcer en marche avant pour protéger les pinces à l'arriére
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
		}
		break;
	case COMPARE_N_CORRECT:
		debug_printf("\nVAriable de x %d\n\tde y %d\nplage %d\nvaleur coef actu %x\n",global.env.pos.x,global.env.pos.y,3000-(global.env.pos.y+LARGEUR_ROBOT/2),coefOdoTranslation);

		if(3000-(global.env.pos.y+LARGEUR_ROBOT/2) > ODOMETRIE_PLAGE_TRANSLATION){ // Robot n'est pas allé assez loin
			debug_printf("Il est trop Loin\n");

			coefOdoTranslation++;
			state = REINIT;
		}else if(3000-(global.env.pos.y+LARGEUR_ROBOT/2) < -ODOMETRIE_PLAGE_TRANSLATION){ // Trop loin
			debug_printf("Il est pas assez loin\n");

			coefOdoTranslation--;
			state = REINIT;
		}else // Si bien réglé s'arrête
			state = DONE;


		if(state == REINIT){
			debug_printf("Nouveau coefficient  %x\n",coefOdoTranslation);

			msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_TRANSLATION;
			msg.data[0] = coefOdoTranslation >> 8;
			msg.data[1] = coefOdoTranslation;
			msg.size=2;
			CAN_send(&msg);
		}

		break;

	case REPORT:	// Correspond à la partie IHM du processus: Le robot affiche le coeff calculé sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'écran LCD
		if(FALSE)// Mettre une condition associée à l'IHM: attend que l'utilisateur réponde
			state = IDLE;

		break;
	case DONE:
		break;

	case ERROR:
		break;
	default:
		break;
	}
}

/**
 * @brief strat_reglage_odo_symetrie
 *	Placer le robot dans le coin côté couloir ordinateur
 *		Il va faire un rectangle sens anti-trigo
 */
void strat_reglage_odo_symetrie(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		CALAGE,
		REINIT,
		ATTENTE,
		POINT1,
		POINT2,
		POINT3,
		POINT4,
		ALLIGNER_ANGLE1,
		COMPARE_N_CORRECT,
		REPORT,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		AVANCER,
		DONE,
		ERROR
	);


	/******TEST******/
	//state = COMPARE_N_CORRECT;
	static bool_e timeout=FALSE;
	static Uint16 coefOdoSymetrie = 0; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_SYM_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de réinitialisation
		state = CALAGE;

		if(QS_WHO_AM_I_get() == PIERRE)
			coefOdoSymetrie = 0;
		else // GUY
			coefOdoSymetrie = 0;

		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'échapement dépend d'ou vient la machine
		//  in the wall
		switch(last_state){
		case IDLE:	// On vient du début de la procédure on doit donc initialiser le robot
			state = REINIT;
			break;
		case REINIT:	//On à remis le robot à zéro après un process, on relance la procédure
			state = POINT1; //Fait un tour rectangle
			break;
		case AVANCER:
			state = DONE;
			break;
		case ALLIGNER_ANGLE1:
			state = PUSH_MOVE;
			break;
		case WAIT_END_OF_MOVE:
			state = COMPARE_N_CORRECT;
			break;
		default:
			state = ERROR;
			break;
		}
		break;

	case REINIT:	// Envoie le message can pour réinitialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);  le robot doit se trouver a peut pres a set(2000,500,PI4096)

		debug_printf("REINIT\n\n");

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;	//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les négatifs)
		msg.data[1]=500 & 0xFF;
		msg.data[2]=LARGEUR_ROBOT/2 >> 8;
		msg.data[3]=LARGEUR_ROBOT/2 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size=6;
		CAN_send(&msg);

		state = CALAGE;

		break;
	case AVANCER://pour le faire avancer du bord
		state = try_going(1000,2600,AVANCER,CALAGE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT1:
		state = try_going(500,2500,POINT1,POINT2,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POINT2:
		state = try_going(1500,2500,POINT2,POINT3,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POINT3:
		state = try_going(1500,500,POINT3,POINT4,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POINT4:
		state = try_going(500,500,POINT4,ALLIGNER_ANGLE1,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case ALLIGNER_ANGLE1:
		state = try_go_angle(PI4096/2,ALLIGNER_ANGLE1,CALAGE,ERROR,SLOW);
		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal réglé
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,PI4096/2,TRUE);//Le fait forcer en marche avant pour protéger les pinces à l'arriére
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
		}
		break;
	case COMPARE_N_CORRECT:

		break;

	case REPORT:	// Correspond à la partie IHM du processus: Le robot affiche le coeff calculé sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'écran LCD
		if(FALSE)// Mettre une condition associée à l'IHM: attend que l'utilisateur réponde
			state = IDLE;

		break;
	case DONE:
		break;

	case ERROR:
		break;
	default:
		break;
	}
}



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */


void TEST_pathfind(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACTIONS_BOTH_2014_TEST_PATHFIND,
			INIT,
			GO_FIRST_POINT,
			PATH_1,
			PATH_2,
			PATH_3,
			PATH_4,
			PATH_5,
			PATH_6,
			PATH_7,
			PATH_8,
			DONE,
			ERROR
		);
	switch(state)
	{
		case INIT:
			state = PATH_1;
			break;
		case GO_FIRST_POINT:
			break;
		case PATH_1:
			if(entrance)
			{
				/*
				global.env.pos.x = 1350;
				global.env.pos.y = 400;
				global.env.foe[0].x = 1400;
				global.env.foe[0].y = 1500;
				global.env.foe[1].x = 100;
				global.env.foe[1].y = 100;
				global.env.foe[0].dist = 1100;
				global.env.foe[1].dist = 1285;
				*/
				debug_printf("ETAT 1\n");
			}
			state = PATHFIND_try_going(Z2, PATH_1, PATH_2, PATH_2, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATH_2:
			if(entrance)
			{
				debug_printf("ETAT 2\n");
				/*
				global.env.pos.x = 1600;
				global.env.pos.y = 2300;
				*/
			}
			state = PATHFIND_try_going(A1, PATH_2, PATH_3, PATH_3, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATH_3:
			if(entrance)
			{
				debug_printf("ETAT 3\n");
				/*
				global.env.foe[0].x = 500;
				global.env.foe[0].y = 1500;
				global.env.pos.x = 250;
				global.env.pos.y = 1250;
				global.env.foe[0].dist = 350;
				*/
			}
			state = PATHFIND_try_going(W0, PATH_3, PATH_4, PATH_4, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;
		case PATH_4:
			if(entrance)
			{
				/*
				global.env.foe[0].x = 700;
				global.env.foe[0].y = 700;
				global.env.foe[1].x = 1000;
				global.env.foe[1].y = 400;
				global.env.pos.x = 850;
				global.env.pos.y = 1100;
				global.env.foe[0].dist = 335;
				global.env.foe[1].dist = 150;
				*/
				debug_printf("ETAT 4\n");
			}
			state = PATHFIND_try_going(A1, PATH_4, PATH_5, PATH_5, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATH_5:
			if(entrance)
			{
				debug_printf("ETAT 5\n");
				/*
				global.env.pos.x = 700;
				global.env.pos.y = 2800;
				global.env.foe[1].x = 1250;
				global.env.foe[1].y = 2300;
				*/
			}
			state = PATHFIND_try_going(W3, PATH_5, PATH_1, DONE, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case ERROR:
			break;
		default:
			break;
	}

}



void test_strat_robot_virtuel(void){
	static enum{
		DO1 = 0,
		DO2,
		DO3,
		DO4,
		DONE,
		ERROR
	}state = DO1;

	switch(state){
		case DO1:
			state = try_going(1000, 1500, DO1, DO2, ERROR, FAST ,ANY_WAY, NO_AVOIDANCE);
			break;
		case DO2:
			state = try_going(1500, 2000, DO2, DO3, ERROR, FAST, ANY_WAY, NO_AVOIDANCE);
			break;
		case DO3:
			state = try_going(500, 2000, DO3, DO4, ERROR, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;
		case DO4:
			state = try_going(500, 500, DO4, DONE, ERROR, FAST, ANY_WAY, NO_AVOIDANCE);
			break;
		case DONE:
			break;
		case ERROR:
		default:
			GPIOD->ODR10 = 1;
			break;
	}

}


void test_Pathfind(void){
	static enum{
			DO1 = 0,
			DONE,
			ERROR
	}state = DO1;

	switch(state){
		case DO1:
			state = PATHFIND_try_going(Z2, DO1, DONE, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case DONE:
			debug_printf("fini");
			GPIOD->ODR10 = 1;
			break;
		case ERROR:
		default:
			GPIOD->ODR10 = 1;
			break;
	}

}


void test_strat_robot_virtuel_with_avoidance(void){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		GOTO,
		BACK,
		DONE
	);

	switch(state){
		case INIT:
			state = GOTO;
			break;
		case GOTO:
			state = try_going(1000, COLOR_Y(1000),GOTO,BACK,BACK,SLOW,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case BACK:
			state = try_going(1000, COLOR_Y(300),BACK,GOTO,GOTO,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case DONE:
			break;
		default:
			break;
	}
}
