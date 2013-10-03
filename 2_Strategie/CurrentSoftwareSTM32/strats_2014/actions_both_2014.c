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
#include "../output_log.h"

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions de réglage odométrique		             */
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
		ERROR = -1
	}state_e;

	static state_e state = IDLE;
	static state_e from = IDLE;

	switch(state){
	case IDLE: //Cas d'attente et de réinitialisation
		from = IDLE;
		state = SORTIR;
		break;

	case SORTIR: // Sert à s'écarter du mur au début et au cours de la procédure, revient dans tous les cas (sauf erreur) au calage
		state = try_going(200,200,SORTIR,CALAGE,ERROR,SLOW,ANY_WAY,NO_AVOIDANCE);
		break;

	case CALAGE: // Cale le robot pour regler le zero et comparer les valeurs: L'échapement dépend d'ou vient la machine
		// Rush in the wall
		switch(from){
		case IDLE:	// On vient du début de la procédure on doit donc initialiser le robot
			state = REINIT;
			break;
		case PROCESS:	//On vient de finir le process on va donc traiter les valeurs
			state = COMPARE_N_CORRECT;
			break;
		case REINIT:	//On à remis le robot à zéro après un process, on relance la procédure
			state = PROCESS; // Ha merde jai oublié davancer
		default:
			state = ERROR;
			break;
		}
		break;

	case REINIT:	// Envoie le message can pour réinitialiser la position ou a minima l'angle du robot
		//send message can set(0,0,0);
		from = REINIT;
		break;

	case PROCESS:	// Tourne 10 fois pour comparer l'angle
		// Tourner 10 fois
		if(TRUE)//Fini?
			state = CALAGE;
		from = PROCESS;
		break;

	case COMPARE_N_CORRECT:
		// compare l'angle reçu après calage par rapport au ZERO
		//correction de la nouvelle odométrie
		// Envoi du nouveau coefficient à la propulsion
		//Renouvel le process autant de fois que nécessaire
		break;

	case REPORT:	// Correspond à la partie IHM du processus: Le robot affiche le coeff calculé sur le LCD pour modifications en programme

		// Ecrire le rapport sur l'écran LCD
		if(FALSE)// Mettre une condition associée à l'IHM: attend que l'utilisateur réponde
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

