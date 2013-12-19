/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_both_2014.c
 *	Package : Carte S�/strats2014
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� par .
 *	Version 2013/10/03
 */

#include "actions_both_2014.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#define LARGEUR_ROBOT 240

// Strat Rotation
#define DEFAULT_SPEED	(SLOW)
#define ODOMETRIE_PLAGE_ROTATION 5 //La variation de la plage sur PI4096(12868) pour savoir si on doit modifier l'angle. Th�oriquement avec 10, on devrait avoir au maximum de 2mm de d�calage sur un 1m
#define USE_CURVE	0
#define NB_TOUR_ODO_ROTATION 5


// Start Translation
#define ODOMETRIE_PLAGE_TRANSLATION 2 // + ou -



/* ----------------------------------------------------------------------------- */
/* 							Fonctions de r�glage odom�trique		             */
/* ----------------------------------------------------------------------------- */

void strat_reglage_odo_rotation(void){
	CREATE_MAE_WITH_VERBOSE(0,
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

	static enum state_e from = IDLE;
	static enum state_e inProcess = IDLE;
	static bool_e timeout=FALSE;
	static Sint16 i=0;
	static Uint16 coefOdoRotation = 0x0000C581; // Peut etre 0xc581 //Original 0x0000C5A2; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de r�initialisation
		from = IDLE;
		state = CALAGE;
		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'�chapement d�pend d'ou vient la machine
		//  in the wall
		switch(from){
		case IDLE:	// On vient du d�but de la proc�dure on doit donc initialiser le robot
			state = REINIT;
			break;
		case PROCESS:	//On vient de finir le process on va donc traiter les valeurs
			state = PUSH_MOVE;
			break;
		case REINIT:	//On � remis le robot � z�ro apr�s un process, on relance la proc�dure
			state = AVANCER; // Ha merde jai oubli� davancer
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

	case REINIT:	// Envoie le message can pour r�initialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);  le robot doit se trouver a peut pres a set(2000,500,PI4096)

		from = REINIT;

		i=0;
		debug_printf("REINIT\n\n");

		debug_printf("\nGlobale variable de x %d\n	de y %d\n l'angle %d\n",global.env.pos.x,global.env.pos.y,global.env.pos.angle);

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=1000 >> 8;	//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les n�gatifs)
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

		from = AVANCER;
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

		from = PROCESS;
		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal r�gl�
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,PI4096/2,TRUE);//Le fait forcer en marche avant pour prot�ger les pinces � l'arri�re
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
			from = WAIT_END_OF_MOVE;
		}
		break;
	case COMPARE_N_CORRECT:
		// compare l'angle re�u apr�s calage par rapport au ZERO
		//correction de la nouvelle odom�trie
		// Envoi du nouveau coefficient � la propulsion
		//Renouvel le process autant de fois que n�cessaire

		//Si il est bien r�gl� l'angle interne du robot devrait etre de zero, sinon l'angle interne du robot s'est d�cal� quand le robot a approch� le mur

		if(global.env.pos.angle > (PI4096/2-ODOMETRIE_PLAGE_ROTATION) && global.env.pos.angle < (PI4096/2+ODOMETRIE_PLAGE_ROTATION)){//Le coef serait parfait global.env.pos.angle doit �tre �gale � 90�
			debug_printf("Ne pas modifier l'angle globale est de %d \n\n",global.env.pos.angle);
			state=DONE;


		}else{//Recommencer proc�dure en modifiant les valeurs
			debug_printf("Modifier valeur l'angle globale est de %d \n\n",global.env.pos.angle);

			state = REINIT;

			//Modifier KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT selon l'angle obtenu
			if(global.env.pos.angle > PI4096/2){//Si l'angle est sup � 90�
				if((global.env.pos.angle-PI4096/2) > 500)
					coefOdoRotation+=10;
				else if((global.env.pos.angle-PI4096/2) > 250)
					coefOdoRotation+=5;
				else
					coefOdoRotation++;
				debug_printf("Augmentez le coef \n\n");
			}
			else{//Si l'angle est inf � 90�
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

			debug_printf("Nouvelle valeur du coef odom�trie rotation %x\n\n",coefOdoRotation);
		}

		break;

	case REPORT:	// Correspond � la partie IHM du processus: Le robot affiche le coeff calcul� sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'�cran LCD
		if(FALSE)// Mettre une condition associ�e � l'IHM: attend que l'utilisateur r�ponde
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

	static enum state_e from = IDLE;
	static bool_e timeout=FALSE;
	static Uint16 coefOdoTranslation = 0x0C43; //Original 0x0C47; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_TRANSLATION_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de r�initialisation
		from = IDLE;
		state = CALAGE;
		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'�chapement d�pend d'ou vient la machine
		//  in the wall
		switch(from){
		case IDLE:	// On vient du d�but de la proc�dure on doit donc initialiser le robot
			state = REINIT;
			break;
		case REINIT:	//On � remis le robot � z�ro apr�s un process, on relance la proc�dure
			state = AVANCER; // Ha merde jai oubli� davancer
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

	case REINIT:	// Envoie le message can pour r�initialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);  le robot doit se trouver a peut pres a set(2000,500,PI4096)

		from = REINIT;

		debug_printf("REINIT\n\n");

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=1000 >> 8;	//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les n�gatifs)
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
		from = AVANCER;
		break;
	case DEMI_TOUR1:
		state = try_go_angle(PI4096,DEMI_TOUR1,DEMI_TOUR2,ERROR,FAST);
		break;
	case DEMI_TOUR2:
		state = try_go_angle(3*PI4096/2,DEMI_TOUR2,CALAGE,ERROR,FAST);
		from = DEMI_TOUR2;
		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal r�gl�
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,3*PI4096/2,TRUE);//Le fait forcer en marche avant pour prot�ger les pinces � l'arri�re
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
			from = WAIT_END_OF_MOVE;
		}
		break;
	case COMPARE_N_CORRECT:
		debug_printf("\nVAriable de x %d\n\tde y %d\nplage %d\nvaleur coef actu %x\n",global.env.pos.x,global.env.pos.y,3000-(global.env.pos.y+LARGEUR_ROBOT/2),coefOdoTranslation);

		if(3000-(global.env.pos.y+LARGEUR_ROBOT/2) > ODOMETRIE_PLAGE_TRANSLATION){ // Robot n'est pas all� assez loin
			debug_printf("Il est trop Loin\n");

			coefOdoTranslation++;
			state = REINIT;
		}else if(3000-(global.env.pos.y+LARGEUR_ROBOT/2) < -ODOMETRIE_PLAGE_TRANSLATION){ // Trop loin
			debug_printf("Il est pas assez loin\n");

			coefOdoTranslation--;
			state = REINIT;
		}else // Si bien r�gl� s'arr�te
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

	case REPORT:	// Correspond � la partie IHM du processus: Le robot affiche le coeff calcul� sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'�cran LCD
		if(FALSE)// Mettre une condition associ�e � l'IHM: attend que l'utilisateur r�ponde
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
 *	Placer le robot dans le coin c�t� couloir ordinateur
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

	static enum state_e from = IDLE;
	static bool_e timeout=FALSE;
	static Uint16 coefOdoSymetrie = 0; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_SYM_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de r�initialisation
		from = IDLE;
		state = CALAGE;
		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'�chapement d�pend d'ou vient la machine
		//  in the wall
		switch(from){
		case IDLE:	// On vient du d�but de la proc�dure on doit donc initialiser le robot
			state = REINIT;
			break;
		case REINIT:	//On � remis le robot � z�ro apr�s un process, on relance la proc�dure
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

	case REINIT:	// Envoie le message can pour r�initialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);  le robot doit se trouver a peut pres a set(2000,500,PI4096)

		from = REINIT;

		debug_printf("REINIT\n\n");

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;	//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les n�gatifs)
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
		from = ALLIGNER_ANGLE1;
		break;
	case PUSH_MOVE://Le fait forcer contre le mur si mal r�gl�
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,PI4096/2,TRUE);//Le fait forcer en marche avant pour prot�ger les pinces � l'arri�re
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
			from = WAIT_END_OF_MOVE;
		}
		break;
	case COMPARE_N_CORRECT:

		break;

	case REPORT:	// Correspond � la partie IHM du processus: Le robot affiche le coeff calcul� sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'�cran LCD
		if(FALSE)// Mettre une condition associ�e � l'IHM: attend que l'utilisateur r�ponde
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
