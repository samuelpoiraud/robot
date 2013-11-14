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

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)


#define DEFAULT_SPEED	(SLOW)
#define ODOMETRIE_PLAGE 10 //La variation de la plage sur PI4096(12868) pour savoir si on doit modifier l'angle. Th�oriquement avec 10, on devrait avoir au maximum de 2mm de d�calage sur un 1m
#define USE_CURVE	0
#define NB_TOUR_ODO_ROTATION 1



/* ----------------------------------------------------------------------------- */
/* 							Fonctions de r�glage odom�trique		             */
/* ----------------------------------------------------------------------------- */

void strat_reglage_odo_rotation(void){
	typedef enum{
		IDLE = 0,
		SORTIR,
		CALAGE,
		REINIT,
		PROCESS,
		COMPARE_N_CORRECT,
		REPORT,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		AVANCER,
		TOUR1,
		TOUR2,
		ERROR = -1
	}state_e;

	static state_e state = IDLE;

	/******TEST******/
	//state = COMPARE_N_CORRECT;

	static state_e from = IDLE;
	static state_e inProcess = IDLE;
	static bool_e timeout=FALSE;
	static Sint16 i=0;
	static Uint16 coefOdoRotation = 0x0000C5A2; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de r�initialisation
		from = IDLE;
		state = AVANCER;
		break;

	case SORTIR: // Sert � s'�carter du mur au d�but et au cours de la proc�dure, revient dans tous les cas (sauf erreur) au calage
		state = try_going(200,200,SORTIR,CALAGE,ERROR,SLOW,ANY_WAY,NO_AVOIDANCE);
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

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=2000 >> 8;
		msg.data[1]=2000 & 0xFF;
		msg.data[2]=500 >> 8;
		msg.data[3]=500 & 0xFF;
		msg.data[4]=PI4096 >> 8;
		msg.data[5]=PI4096 & 0xFF;
		CAN_send(&msg);
		break;


	case AVANCER://pour le faire avancer du bord
		state = try_going(1000,COLOR_Y(500),AVANCER,CALAGE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		from = AVANCER;
		break;
	case PROCESS:	// Tourne 10 fois pour comparer l'angle
		// Tourner 10 fois

		if(i < NB_TOUR_ODO_ROTATION){
			switch(inProcess){
			case IDLE:
				inProcess=try_go_angle(0,IDLE,TOUR1,ERROR,SLOW);
				break;
			case TOUR1:
				inProcess=try_go_angle(4*PI4096/3,TOUR1,TOUR2,ERROR,SLOW);
				break;
			case TOUR2:
				inProcess=try_go_angle(2*PI4096/3,TOUR2,IDLE,ERROR,SLOW);
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
		ASSER_push_rush_in_the_wall(BACKWARD,TRUE,PI4096,TRUE);//Le fait forcer en marche arri�re pour prot�ger les cartes � l'avant
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

		debug_printf("L'ancgle globale est de %d\n",global.env.pos.angle);

		if(global.env.pos.angle > (PI4096-ODOMETRIE_PLAGE) || global.env.pos.angle < (-PI4096-ODOMETRIE_PLAGE)){//Robot bien regle, on fait avec PI4096 car le robot recule au lieu d'avancer vers le mur
			debug_printf("Ne pas modifier\n");

		}else{//Recommencer proc�dure en modifiant les valeurs
			debug_printf("Modifier valeur\n");
			i=0;
			state = REINIT;

			//Modifier KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT selon l'angle obtenu
			if(global.env.pos.angle > 0)//Si l'angle est entre 0 et PI4096
				coefOdoRotation++;						//A voir peut modifier -- ici et ++ dans l'autre
			else//Si l'angle est compris entre 0 et -PI4096
				coefOdoRotation--;

			//Envoie du message CAN
			msg.sid=DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_ROTATION;
			msg.data[0] = coefOdoRotation;
			CAN_send(&msg);

			debug_printf("Nouvelle valeur du coef odom�trie rotation %x\n",coefOdoRotation);
		}

		break;

	case REPORT:	// Correspond � la partie IHM du processus: Le robot affiche le coeff calcul� sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'�cran LCD
		if(FALSE)// Mettre une condition associ�e � l'IHM: attend que l'utilisateur r�ponde
			state = IDLE;

		break;

	case ERROR:
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
