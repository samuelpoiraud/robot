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
#include "../Pathfind.h"
#include "../Supervision/Buzzer.h"

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#define LARGEUR_ROBOT 246

// Strat Rotation
#define DEFAULT_SPEED	(SLOW)
#define ODOMETRIE_PLAGE_ROTATION 2 //La variation de la plage sur PI4096(12868) pour savoir si on doit modifier l'angle. Théoriquement avec 10, on devrait avoir au maximum de 2mm de décalage sur un 1m
#define USE_CURVE	0
#define NB_TOUR_ODO_ROTATION 2




// Start Translation
#define ODOMETRIE_PLAGE_TRANSLATION 2 // + ou -


/* ----------------------------------------------------------------------------- */
/* 							Fonctions de réglage odométrique		             */
/* ----------------------------------------------------------------------------- */


void strat_tourne_en_rond(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_TOURNE_EN_ROND,
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
	Uint16 nbTour = 1;

	switch(state){
	case IDLE:
//		msg.sid=ASSER_SET_POSITION;
//		msg.data[0]=500 >> 8;
//		msg.data[1]=500 & 0xFF;
//		msg.data[2]=120 >> 8;
//		msg.data[3]=120 & 0xFF;
//		msg.data[4]=PI4096/2 >> 8;
//		msg.data[5]=PI4096/2 & 0xFF;
//		msg.size = 6;
//		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going_until_break(global.env.pos.x,200,POS_DEPART,POS_POINT,ERROR,SLOW,(QS_WHO_AM_I_get_name() == BIG_ROBOT)?FORWARD:BACKWARD,NO_AVOIDANCE);
		break;
	case POS_POINT:
		state = try_going_until_break(650,600,POS_POINT,TOUR,ERROR,SLOW,(QS_WHO_AM_I_get_name() == BIG_ROBOT)?FORWARD:BACKWARD,NO_AVOIDANCE);
		break;
	case TOUR:
		state = try_going_multipoint(tour,6,TOUR,FIN,ERROR,(QS_WHO_AM_I_get_name() == BIG_ROBOT)?FORWARD:BACKWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
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
			coefOdoRotation = 0x0000C5AD;
		else // GUY
			coefOdoRotation = 0x00010AC0;

		// Enlever le commentaire si vous voulez imposer le coefficient des le début de la stratégie
		/*msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_ROTATION;
		msg.data[0] = coefOdoRotation >> 24;
		msg.data[1] = coefOdoRotation >> 16;
		msg.data[2] = coefOdoRotation >> 8;
		msg.data[3] = coefOdoRotation;
		msg.size=4;
		CAN_send(&msg);*/
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
		ASSER_set_position(1000, LARGEUR_ROBOT/2, PI4096/2);
		//Je lui dis qu'il est au milieu du terrain ( si je mets 0, il risque d'aller dans les négatifs)
		state = CALAGE;

		break;
	case AVANCER://pour le faire avancer du bord
		state = try_going(1000,100,AVANCER,CALAGE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);

		if(state==CALAGE)
			debug_printf("\nRENIT variable de x %d\n\t\t\tde y %x\n",global.env.pos.x,global.env.pos.y);
		break;
	case PROCESS:
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

		if(absolute(global.env.pos.angle-PI4096/2) <= ODOMETRIE_PLAGE_ROTATION){//Le coef serait parfait global.env.pos.angle doit être égale à 90°
			debug_printf("Ne pas modifier l'angle globale est de %d \n\n",global.env.pos.angle);
			state=DONE;


		}else{//Recommencer procédure en modifiant les valeurs
			debug_printf("Modifier valeur l'angle globale est de %d \n\n",global.env.pos.angle);

			state = REINIT;

			//Modifier KRUSTY_ODOMETRY_COEF_ROTATION_DEFAULT selon l'angle obtenu
			if(global.env.pos.angle > PI4096/2){//Si l'angle est sup à 90°
				if(absolute(global.env.pos.angle-PI4096/2) > 500)
					coefOdoRotation+=10;
				else if(absolute(global.env.pos.angle-PI4096/2) > 250)
					coefOdoRotation+=5;
				else
					coefOdoRotation++;
				debug_printf("Augmentez le coef \n\n");
			}
			else{//Si l'angle est inf à 90°
				if(absolute(global.env.pos.angle-PI4096/2) > 500)
					coefOdoRotation-=10;
				else if(absolute(global.env.pos.angle-PI4096/2) > 250)
					coefOdoRotation-=5;
				else
					coefOdoRotation--;
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
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_ODO_TRANSLATION,
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
			coefOdoTranslation = 0x0C3C;
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
		ASSER_set_position(1000, LARGEUR_ROBOT/2, PI4096/2);
		//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les négatifs)
		state = CALAGE;

		break;
	case AVANCER://pour le faire avancer du bord
		state = try_going(1000,2600,AVANCER,CALAGE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DEMI_TOUR1:
		state = try_go_angle(PI4096,DEMI_TOUR1,DEMI_TOUR2,ERROR,SLOW);
		break;
	case DEMI_TOUR2:
		state = try_go_angle(3*PI4096/2,DEMI_TOUR2,CALAGE,ERROR,SLOW);
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

		if(3000-(global.env.pos.y+LARGEUR_ROBOT/2) > ODOMETRIE_PLAGE_TRANSLATION){
			debug_printf("Il n'est pas allé assez loin\n");
			coefOdoTranslation--;
			state = REINIT;
		}else if(3000-(global.env.pos.y+LARGEUR_ROBOT/2) < -ODOMETRIE_PLAGE_TRANSLATION){
			debug_printf("Il est trop loin\n");
			coefOdoTranslation++;
			state = REINIT;
		}else{ // Si bien réglé s'arrête
			debug_printf("Odométrie en translation bien calibrée\n");
			state = DONE;
		}


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
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_ODO_SYMETRIE,
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
//	static Uint16 coefOdoSymetrie = 0; //Mofifier la valeur KRUSTY_ODOMETRY_COEF_SYM_DEFAULT dans _Propulsion_config.h
//	CAN_msg_t msg;

	switch(state){
	case IDLE: //Cas d'attente et de réinitialisation
		state = CALAGE;

		/*if(QS_WHO_AM_I_get() == PIERRE)
			coefOdoSymetrie = 0;
		else // GUY
			coefOdoSymetrie = 0;*/

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

		ASSER_set_position(500, LARGEUR_ROBOT/2, PI4096/2);
		//Je lui dis qu'il est au milieu du terrain ( si je mest 0, il risque d'aller dans les négatifs)
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


typedef enum
{
	KPT_COMPUTE = 0,
	KDT_COMPUTE,
	KPR_COMPUTE,
	KDR_COMPUTE,
	COEF_NB
}coefs_e;
//COEFS par défaut pour les robots...
const Uint32 default_coefs_small_robot[COEF_NB] =	{		34,		12,		130,		288	};
const Uint32 default_coefs_big_robot[COEF_NB] =		{		32,		0x36,	0xA0,		0x800	};
static coefs_e current_coef;

void send_coef(coefs_e coef, Uint32 value)
{
	CAN_msg_t msg;
	debug_printf("send coef : ");
	switch(coef)
	{
		case KPT_COMPUTE:	msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_KP_TRANSLATION;		debug_printf("Kp translation");	break;
		case KDT_COMPUTE:	msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_KD_TRANSLATION;		debug_printf("Kd translation");	break;
		case KPR_COMPUTE:	msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_KP_ROTATION;		debug_printf("Kp rotation");	break;
		case KDR_COMPUTE:	msg.sid = DEBUG_PROPULSION_REGLAGE_COEF_KD_ROTATION;		debug_printf("Kd rotation");	break;
		default:				break;
	}
	msg.data[0] = HIGHINT(value);
	msg.data[1] = LOWINT(value);
	msg.size = 2;
	debug_printf(" = %ld\n",value);//TODO
	CAN_send(&msg);
}

void send_default_coefs(void)
{
	Uint8 i;
	const Uint32 * default_coefs;
	debug_printf("SEND DEFAULT COEFS...\n");
	if(QS_WHO_AM_I_get() == SMALL_ROBOT)
		default_coefs = default_coefs_small_robot;
	else
		default_coefs = default_coefs_big_robot;
	for(i=0;i<COEF_NB;i++)		//On charge les coefs par défaut.
		send_coef(i, default_coefs[i]);
}

#define PRECISION	16	//Puissance de 2... plus c'est élevé, plus on cherchera un coef précis...

//Précondition : la première trajectoire (coefs par défaut) doit fonctionner sans FAIL !!
bool_e strat_reglage_asser_compute_coefs(time32_t duration)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_REGLAGE_COMPUTE_COEFS,
				INIT,
				FIRST_MEASURE_LAUNCH,
				FIRST_MEASURE_SAVE_DURATION,
				LOOKING_FOR_MAXIMUM,
				LOOKING_FOR_MAXIMUM_COMPARE_DURATION,
				LOOKING_FOR_MINIMUM,
				LOOKING_FOR_MINIMUM_COMPARE_DURATION,
				LOOKING_FOR_BEST,
				LOOKING_FOR_BEST_ADD_OFFSET,
				LOOKING_FOR_BEST_SUB_OFFSET,
				LOOKING_FOR_BEST_ANALYSE,
				NEXT_COEF
				);
	static Uint32 best_coefs[COEF_NB];
	static Uint32 multiply_value, divide_value, offset, current_value;
	static time32_t best_duration;
	Uint8 i;
	bool_e update_coef_and_return_true = FALSE;

	debug_printf("# %ld %ld %ld %ld : %ld\n",	((current_coef==0)?current_value:best_coefs[0]),
											((current_coef==1)?current_value:best_coefs[1]),
											((current_coef==2)?current_value:best_coefs[2]),
											((current_coef==3)?current_value:best_coefs[3]),
											duration);

	switch(state)
	{
		case INIT:
			for(i=0;i<COEF_NB;i++)		//On charge les coefs par défaut.
			{
				if(QS_WHO_AM_I_get() == SMALL_ROBOT)
					best_coefs[i] = default_coefs_small_robot[i];
				else
					best_coefs[i] = default_coefs_big_robot[i];
			}
			current_coef = KPT_COMPUTE;	//Premier coef à éprouver
			state = FIRST_MEASURE_LAUNCH;
			break;
		case FIRST_MEASURE_LAUNCH:
			current_value = best_coefs[current_coef];
			multiply_value = 1;
			divide_value = 1;
			update_coef_and_return_true = TRUE;
			state = FIRST_MEASURE_SAVE_DURATION;
			break;
		case FIRST_MEASURE_SAVE_DURATION:
			best_duration = duration;
			state = LOOKING_FOR_MAXIMUM;
			break;
		case LOOKING_FOR_MAXIMUM:
			multiply_value *=2;
			current_value = best_coefs[current_coef] * multiply_value;
			update_coef_and_return_true = TRUE;
			if(multiply_value < 4096)
				state = LOOKING_FOR_MAXIMUM_COMPARE_DURATION;
			else
				state = LOOKING_FOR_MINIMUM;
			break;
		case LOOKING_FOR_MAXIMUM_COMPARE_DURATION:
			if(duration < best_duration)	//On obtient mieux, on continue de multiplier
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
				state = LOOKING_FOR_MAXIMUM;
			}
			else							//On a trouvé le maximum..
				state = LOOKING_FOR_MINIMUM;
			break;
		case LOOKING_FOR_MINIMUM:
			divide_value *=2;
			current_value = best_coefs[current_coef] / divide_value;
			update_coef_and_return_true = TRUE;
			if(current_value != 0)
				state = LOOKING_FOR_MINIMUM_COMPARE_DURATION;
			else
				state = LOOKING_FOR_BEST;
			break;
		case LOOKING_FOR_MINIMUM_COMPARE_DURATION:
			if(duration < best_duration)	//On obtient mieux, on continue de diviser
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
				state = LOOKING_FOR_MINIMUM;
			}
			else							//On a trouvé le maximum..
				state = LOOKING_FOR_BEST;
			break;
		case LOOKING_FOR_BEST:
			//On a une "meilleure valeur", à x2 ou /2 prêt... go pour la dichotomie...
			multiply_value = PRECISION;
			state = LOOKING_FOR_BEST_ADD_OFFSET;

			break;
		case LOOKING_FOR_BEST_ADD_OFFSET:
			multiply_value /= 2;
			if(multiply_value)
			{
				offset = (best_coefs[current_coef] * multiply_value)/PRECISION;
				current_value = best_coefs[current_coef] + offset;	//On essaye d'ajouter l'offset
				update_coef_and_return_true = TRUE;
				state = LOOKING_FOR_BEST_SUB_OFFSET;
			}
			else
				state = NEXT_COEF;
			break;
		case LOOKING_FOR_BEST_SUB_OFFSET:
			if(duration < best_duration)	//C'était mieux en ajoutant l'offset
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
				state = LOOKING_FOR_BEST_ADD_OFFSET;
			}
			else
			{
				current_value = best_coefs[current_coef] - offset;	//On essaye d'enlever l'offset
				update_coef_and_return_true = TRUE;
				state = LOOKING_FOR_BEST_ANALYSE;
			}
			break;
		case LOOKING_FOR_BEST_ANALYSE:
			if(duration < best_duration)	//C'était mieux en retirant l'offset
			{
				best_coefs[current_coef] = current_value;
				best_duration = duration;
			}
			state = LOOKING_FOR_BEST_ADD_OFFSET;
			break;
		case NEXT_COEF:
			debug_printf("_____________________\n\tBest coef for ");
			switch(current_coef)
			{
				case KPT_COMPUTE:	debug_printf("Kp translation");	break;
				case KDT_COMPUTE:	debug_printf("Kd translation");	break;
				case KPR_COMPUTE:	debug_printf("Kp rotation");	break;
				case KDR_COMPUTE:	debug_printf("Kd rotation");	break;
				default:											break;
			}
			debug_printf("is %ld",best_coefs[current_coef]);
			current_coef = (current_coef < COEF_NB - 1)?(current_coef+1):0;
			state = FIRST_MEASURE_LAUNCH;
			break;
		default:
			break;
	}
	if(update_coef_and_return_true)
	{
		send_coef(current_coef, current_value);
		return TRUE;
	}
	return FALSE;
}


void strat_reglage_asser(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_COEF_ASSER,
			INIT,
			COMPUTE_COEFS,
			WAIT_TRAJECTORY,
			TRAJECTORY_TRANSLATION,
			TRAJECTORY_CURVE,
			ROTATION,
			FAILED,
			ERROR_STATE,
			PRINT_RESULT,
			ROTATION_FAILED,
			DONE);

	static time32_t duration = 0;

	switch(state)
	{
		case INIT:
			debug_printf("Lancement de la stratégie de réglage des coefs.\n);");
			debug_printf("Assurez vous que le robot est à 50cm de la bordure derrière lui, qu'il a 2m devant lui et 80cm de chaque coté...\n");
			//Send mode BEST_EFFORT
			CAN_send_sid(DEBUG_ENABLE_MODE_BEST_EFFORT);
			ASSER_set_position(1000,500,PI4096/2);
			send_default_coefs();
			state = COMPUTE_COEFS;
			break;
		case COMPUTE_COEFS:
			if(strat_reglage_asser_compute_coefs(duration))
				state = TRAJECTORY_TRANSLATION;
			break;
		case TRAJECTORY_TRANSLATION:
			CAN_send_sid(DEBUG_DO_TRAJECTORY_FOR_TEST_COEFS);
			state = WAIT_TRAJECTORY;
			//state = try_going_multipoint(displacements, 1, TRAJECTORY_TRANSLATION, TRAJECTORY_CURVE, FAILED, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case TRAJECTORY_CURVE:
			//state = try_going_multipoint(&displacements[1], DISPLACEMENTS_NB-1, TRAJECTORY_CURVE, ROTATION, FAILED, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case ROTATION:
			//state = try_go_angle(PI4096/2, ROTATION, PRINT_RESULT, FAILED, FAST);
			break;
		case WAIT_TRAJECTORY:
			if(global.env.duration_trajectory_for_test_coefs)
			{
				duration = global.env.duration_trajectory_for_test_coefs;
				state = PRINT_RESULT;
			}
			if(global.env.asser.erreur)
				state = FAILED;
			//TODO gestion d'un timeout sur cette action...
			break;
		case PRINT_RESULT:
			debug_printf("END : t=%ldms\n", duration);
			state = COMPUTE_COEFS;
			break;
		case FAILED:
			if(entrance)
			{
				debug_printf("FAILED : come back home...\n");
				duration = 100000;	//fausse durée de fail : 100 secondes.
				send_coef(current_coef, ((QS_WHO_AM_I_get()==SMALL_ROBOT)?default_coefs_small_robot[current_coef]:default_coefs_big_robot[current_coef]));
			}
			state = try_going(1000,500, FAILED, ROTATION_FAILED, ERROR_STATE, FAST, ANY_WAY, NO_AVOIDANCE);
			break;
		case ROTATION_FAILED:
			state = try_go_angle(PI4096/2, ROTATION_FAILED, COMPUTE_COEFS, FAILED, FAST);
			break;
		case ERROR_STATE:
			if(entrance)
				debug_printf("Fin de la stratégie, échec du déplacement\n");
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
			state = PATHFIND_try_going(Z2, PATH_1, PATH_2, PATH_2, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATH_2:
			state = PATHFIND_try_going(A1, PATH_2, PATH_3, PATH_3, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATH_3:
			state = PATHFIND_try_going(W0, PATH_3, PATH_4, PATH_4, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;
		case PATH_4:
			state = PATHFIND_try_going(C3, PATH_4, PATH_5, PATH_5, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATH_5:
			state = PATHFIND_try_going(C0, PATH_5, PATH_1, PATH_1, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ROBOT_VIRTUEL_AVOIDANCE,
		INIT,
		EXTRACT,
		GOTO,
		BACK,
		DONE
	);

	switch(state){
		case INIT:
			state = EXTRACT;
			break;
		case EXTRACT:
			state = try_going(588, COLOR_Y(500), EXTRACT, GOTO, GOTO, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;
		case GOTO:
			state = try_going(1000, COLOR_Y(2500),GOTO,BACK,GOTO,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case BACK:
			state = try_going(1000, COLOR_Y(500),BACK,GOTO,BACK,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case DONE:
			break;
		default:
			break;
	}
}

#define WAIT_TIME	1000		//[ms] temps d'attente sur le point visé.
/*
 * Cette sub_action se rend au point (x,y) OU au noeud le plus proche si le point(x,y) est à moins de 5cm manhattan d'un noeud.
 * Ensuite, l'action marque une attente d'une seconde avant de rendre la main.
 * Si elle est appelée ou rappelée, alors qu'on est sur ce point, elle marque simplement une attente d'une seconde.
 *
 * Cas d'exemple : sub_wait(1000,COLOR_Y(2000));								//Va passer une seconde en 1000, COLOR_Y(2000)
 * Autre exemple intéressant : sub_wait(global.env.pos.x, global.env.pos.y);	//Marque une attente où que tu sois
 */
error_e sub_wait(Sint16 x, Sint16 y)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_BOTH_WAIT,
			INIT,
			GET_IN,
			GOTO_POINT,
			WAIT,
			ERROR,
			DONE
		);
	static enum state_e success_state;
	static pathfind_node_id_t node;
	static time32_t local_time;

	switch(state)
	{
		case INIT:
			node = PATHFIND_closestNode(x,y,(Uint32)(NULL));
			if(est_dans_carre(x-50, x+50, y-50, y+50,(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))	//Je suis sur le point (5cm près)
				state = WAIT;
			else if(PATHFIND_manhattan_dist(PATHFIND_get_node_x(node),PATHFIND_get_node_y(node),global.env.pos.x,global.env.pos.y) < 50)
				state = GOTO_POINT;
			else
				state = GET_IN;
			break;
		case GET_IN:
			if(entrance)
			{
				if(PATHFIND_manhattan_dist(PATHFIND_get_node_x(node),PATHFIND_get_node_y(node),x,y) < 50)
					success_state = WAIT;	//Le point demandé est très près d'un noeud... on se contente d'aller au noeud.
				else
					success_state = GOTO_POINT; 	//Le point demandé n'est pas sur un noeud...
			}
			state = PATHFIND_try_going(node,GET_IN,success_state,ERROR,ANY_WAY,FAST,DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;
		case GOTO_POINT:
			state = try_going(x,y,GOTO_POINT,WAIT,ERROR,FAST,DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;
		case WAIT:
			if(entrance)
			{
				local_time = global.env.match_time;
			}
			if(global.env.match_time > local_time + WAIT_TIME)
				state = DONE;
			break;
		case ERROR:
			state = INIT;
			return NOT_HANDLED;
			break;
		case DONE:
			state = INIT;
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

error_e protected_fires(protected_fires_e fires){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PROTECTED_FIRES,
		IDLE,
	//	GET_IN,
		POINT_0,
		POINT_1,
		POINT_2,
		POINT_3,
		NOT_REACHED,
		DONE,
		ERROR
	);
	static enum state_e next_point_protected = IDLE;
	static protected_fires_e current_protected_fires = OUR_FIRES;

	static pathfind_node_id_t points[4];
	if(current_protected_fires != fires)
	{
		state = IDLE;		//On force l'état IDLE si les feux à protéger ne sont plus les mêmes...
	}

	switch(state){
		case IDLE:

			if((global.env.color == RED && fires == OUR_FIRES) || (global.env.color != RED && fires == ADVERSARY_FIRES)){
				points[0] = A2;   // A2
				points[1] = C3;	// B3
				points[2] = C2;  // C2
				points[3] = B1;    // B1
			}else{
				points[0] = Z2;  // Z2
				points[1] = W3;	// Y3
				points[2] = W2;  // W2
				points[3] = Y1;   // Y1
			}
			current_protected_fires = fires;
			next_point_protected = POINT_0;

			//if((((global.env.color == RED && fires == OUR_FIRES) && (global.env.color != RED && fires == ADVERSARY_FIRES)) && est_dans_carre(400, 2000, 0, 1500, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
			//		|| (((global.env.color == RED && fires == ADVERSARY_FIRES) && (global.env.color != RED && fires == OUR_FIRES)) && est_dans_carre(400, 2000, 1500, 3000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})))
				state = next_point_protected;
			//else
			//	state = GET_IN;
		   break;

	   //case GET_IN:
		//   state = PATHFIND_try_going(PATHFIND_closestNode(points[next_point_protected-POINT_0].x,points[next_point_protected-POINT_0].y, 0x00),
		//		   GET_IN, next_point_protected, ERROR, ANY_WAY, FAST,DODGE_AND_WAIT, END_AT_BREAK);
		//   break;

		case POINT_0:	//no break;
		case POINT_1:	//no break;
		case POINT_2:	//no break;
		case POINT_3:

			state = PATHFIND_try_going(points[next_point_protected-POINT_0], state, DONE,ERROR,ANY_WAY,FAST, DODGE_AND_WAIT, END_AT_LAST_POINT); //No dodge volontaire

			if(state != next_point_protected)
			{
				BUZZER_play(200, DEFAULT_NOTE, 2);
				next_point_protected = (next_point_protected>=POINT_3)?POINT_0:next_point_protected+1;
			}
			break;

		case DONE:
			state = next_point_protected;	//ATTENTION, on ne retourne pas vers IDLE, mais vers le prochain point, pour le prochain appel !
			return END_OK;
			break;

		case ERROR:
			state = next_point_protected;	//ATTENTION, on ne retourne pas vers IDLE, mais vers le prochain point, pour le prochain appel !
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}



error_e action_recalage_x(way_e sens, Sint16 wanted_x, Sint16 diff_x){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_RECALAGE_X,
		IDLE,
		RUSH_WALL,
		WAIT,
		GET_OUT,
		GET_OUT_WITH_ERROR,
		WAIT_FOR_EXIT,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static bool_e timeout;
	static time32_t local_time;

	static GEOMETRY_point_t escape_point[1];

	switch(state){
		case IDLE :
			timeout = FALSE;
			if(global.env.pos.x > 1000)
				escape_point[0] = (GEOMETRY_point_t){wanted_x - 200, global.env.pos.y};
			else
				escape_point[0] = (GEOMETRY_point_t){wanted_x + 200, global.env.pos.y};
			state = RUSH_WALL;
			break;

		case RUSH_WALL :
			if(global.env.pos.x > 1000)
				ASSER_push_rush_in_the_wall(sens, TRUE, 0, TRUE);
			else
				ASSER_push_rush_in_the_wall(sens, TRUE, PI4096, TRUE);
			state = WAIT;
			break;

		case WAIT :
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				global.env.recalage_x.offset = global.env.pos.x - wanted_x;
				global.env.recalage_x.last_time = global.env.match_time;
				display(global.env.recalage_x.offset);
				state = GET_OUT;
			}
			break;

		case GET_OUT :
			if(global.env.pos.x > 1000)
				state = try_going(wanted_x - 200, global.env.pos.y, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			else
				state = try_going(wanted_x + 200, global.env.pos.y, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[0].x,escape_point[0].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,WAIT_FOR_EXIT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de réessayer périodiquement.
			if(entrance)
			{
				local_time = global.env.match_time;
			}
			if(global.env.match_time - local_time > 2000)
				state = GET_OUT_WITH_ERROR;
			break;
		case DONE :
			state = IDLE;
			return END_OK;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_RECALAGE_Y,
		IDLE,
		RUSH_WALL,
		WAIT,
		GET_OUT,
		GET_OUT_WITH_ERROR,
		WAIT_FOR_EXIT,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static bool_e timeout;
	static time32_t local_time;
	static GEOMETRY_point_t escape_point[1];

	switch(state){
		case IDLE :
			timeout = FALSE;
			if(global.env.pos.y > 1500)
				escape_point[0] = (GEOMETRY_point_t){global.env.pos.x, wanted_y - 200};
			else
				escape_point[0] = (GEOMETRY_point_t){global.env.pos.x, wanted_y + 200};
			state = RUSH_WALL;
			break;

		case RUSH_WALL :
			ASSER_push_rush_in_the_wall(sens, TRUE, angle, TRUE);
			state = WAIT;
			break;

		case WAIT :
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				global.env.recalage_y.offset = global.env.pos.y - wanted_y;
				global.env.recalage_y.last_time = global.env.match_time;
				display(global.env.recalage_y.offset);
				state = GET_OUT;
			}
			break;

		case GET_OUT :
			if(global.env.pos.y > 1500)
				state = try_going(global.env.pos.x, wanted_y - 80, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			else
				state = try_going(global.env.pos.x, wanted_y + 80, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[0].x,escape_point[0].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,WAIT_FOR_EXIT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de réessayer périodiquement.
			if(entrance)
			{
				local_time = global.env.match_time;
			}
			if(global.env.match_time - local_time > 2000)
				state = GET_OUT_WITH_ERROR;
			break;
		case DONE :
			state = IDLE;
			return END_OK;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}
