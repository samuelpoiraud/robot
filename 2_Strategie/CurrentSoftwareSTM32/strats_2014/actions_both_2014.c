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

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)


#define DEFAULT_SPEED	(SLOW)
#define ODOMETRIE_PLAGE 10 //La variation de la plage sur PI4096(12868) pour savoir si on doit modifier l'angle. Théoriquement avec 10, on devrait avoir au maximum de 2mm de décalage sur un 1m
#define USE_CURVE	0
#define NB_TOUR_ODO_ROTATION 1



/* ----------------------------------------------------------------------------- */
/* 							Fonctions de réglage odométrique		             */
/* ----------------------------------------------------------------------------- */

void strat_reglage_odo_rotation(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		CALAGE,
		REINIT,
		ATTENTE,
		PROCESS,
		IN_THE_WALK,
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
	static Uint16 coefOdoRotation = 0x0000C5A2; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT dans _Propulsion_config.h
	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de réinitialisation
		from = IDLE;
		state = CALAGE;
		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'échapement dépend d'ou vient la machine
		//  in the wall
		switch(from){
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

		from = REINIT;

		i=0;
		debug_printf("REINIT\n\n");

		debug_printf("\nGlobale variable de x %d\n	de y %d\n",global.env.pos.x,global.env.pos.y);

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=0;
		msg.data[1]=0;
		msg.data[2]=0;
		msg.data[3]=0;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size=6;
		CAN_send(&msg);

		state = CALAGE;

		break;
	case IN_THE_WALK://Pour le mettre bien en position
		state = try_going(2000,500,IN_THE_WALK,CALAGE,ERROR,SLOW,ANY_WAY,NO_AVOIDANCE);
		break;
	case AVANCER://pour le faire avancer du bord
		debug_printf("J'AVANCE\n");


		state = try_going(0,500,AVANCER,CALAGE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);

		if(state==CALAGE)
			debug_printf("\nRENIT variable de x %d\n\t\t\tde y %d\n",global.env.pos.x,global.env.pos.y);

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
				if(inProcess == IDLE){
					i++; // On incremente le i apres avoir fait un tour complet
					debug_printf("incrementation de i\n");
				}
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
	case PUSH_MOVE://Le fait forcer contre le mur si mal réglé
		ASSER_push_rush_in_the_wall(FORWARD,TRUE,0,TRUE);//Le fait forcer en marche avant pour protéger les pinces à l'arriére
		state = WAIT_END_OF_MOVE;
		break;
	case WAIT_END_OF_MOVE:
		if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
			state = CALAGE;
			from = WAIT_END_OF_MOVE;

			debug_printf("Le robot a forcer dans le mur\n");
		}
		break;
	case COMPARE_N_CORRECT:
		// compare l'angle reçu après calage par rapport au ZERO
		//correction de la nouvelle odométrie
		// Envoi du nouveau coefficient à la propulsion
		//Renouvel le process autant de fois que nécessaire

		//Si il est bien réglé l'angle interne du robot devrait etre de zero, sinon l'angle interne du robot s'est décalé quand le robot a approché le mur

		debug_printf("L'ancgle globale est de %d\n",global.env.pos.angle);

		if(global.env.pos.angle > (PI4096-ODOMETRIE_PLAGE) || global.env.pos.angle < (-PI4096+ODOMETRIE_PLAGE)){//Robot bien regle, on fait avec PI4096 car le robot recule au lieu d'avancer vers le mur
			debug_printf("Ne pas modifier\n");
			state=DONE;


		}else{//Recommencer procédure en modifiant les valeurs
			debug_printf("Modifier valeur\n");

			state = REINIT;

			//Modifier KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT selon l'angle obtenu
			if(global.env.pos.angle > 0)//Si l'angle est entre 0 et PI4096
				coefOdoRotation++;						//A voir peut modifier -- ici et ++ dans l'autre
			else//Si l'angle est compris entre 0 et -PI4096
				coefOdoRotation--;

			//Envoie du message CAN
			msg.sid=DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_ROTATION;
			msg.data[0] = coefOdoRotation;
			msg.size=1;
			CAN_send(&msg);

			debug_printf("Nouvelle valeur du coef odométrie rotation %x\n",coefOdoRotation);
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

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */
