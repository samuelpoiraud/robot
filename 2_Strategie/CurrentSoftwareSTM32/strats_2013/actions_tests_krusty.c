/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_krusty.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#include "actions_tests_krusty.h"
#include "actions_glasses.h"
#include "actions_cherries.h"
#include "../state_machine_helper.h"
#include "../output_log.h"
#include "config_pin.h"

//#define LOG_PREFIX "strat_tests: "
//#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */

/*
	Stratégie d'homologation. Marque quelques points faciles.
	Evite l'adversaire à coup sur !
*/
void TEST_STRAT_K_homologation(void)
{
	/*
		REMARQUE : en homologation, l'idéal est de produire une stratégie semblable à celle visée en match.
		Cependant, si on se prend une remarque, ou qu'un truc foire, il peut être TRES intéressant d'avoir sous le coude
		 une stratégie élémentaire qui permet d'être homologable
			- sortir de la zone, marquer 1 point
			- éviter correctement un adversaire (donc il faut un minimum de déplacement quand même)
	*/
	enum state_e {
		DO_GLASSES,
		DONE
	};
	static enum state_e state = DO_GLASSES;

	switch(state) {
		case DO_GLASSES:
			state = check_sub_action_result(K_STRAT_sub_glasses_alexis(TRUE, TRUE), DO_GLASSES, DONE, DONE);	//Dans tous les cas on fini
			break;

		case DONE:
			break;
	}
}

/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */

void K_Strat_Pour_Les_Nuls(void){
	static enum{
			INIT=0,
			DEPART,
			PAS1,
			PAS2ANGLE1,
			PAS2ANGLE2,
			PAS2ANGLE3,
			PAS2ANGLE4,
			PAS3,
			PAS4,
			PAS5,
			PAS6,
			PAS7,
			PAS8,
			DONE,
		}state = INIT;

		// Multipoints
		displacement_t tab[8]={{820,2650,SLOW},
							   {650,2550,SLOW},
							   {520,2400,SLOW},
							   {430,2240,SLOW},
							   {370,2030,SLOW},
							   {340,1850,SLOW},
							   {320,1680,SLOW},
							   {300,1500,SLOW},
								};

		displacement_t tab6[12]={{620,1420,SLOW},
									{500,1290,SLOW},
									{400,1180,SLOW},
									{320,1060,SLOW},
									{300,940,SLOW},
									{300,820,SLOW},
									{350,700,SLOW},
									{480,580,SLOW},
									{620,450,SLOW},
									{880,370,SLOW},
									{900,310,SLOW},
									{1000,300,SLOW},
									};
		displacement_t rond[17]={{500,1500,SLOW},
									{560,1260,SLOW},
									{680,1100,SLOW},
									{840,1030,SLOW},
									{1000,1000,SLOW},
									{1160,1030,SLOW},
									{1340,1110,SLOW},
									{1450,1260,SLOW},
									{1500,1500,SLOW},
									{1450,1720,SLOW},
									{1340,1870,SLOW},
									{1160,1960,SLOW},
									{1000,2000,SLOW},
									{840,1960,SLOW},
									{680,1870,SLOW},
									{560,1720,SLOW},
									{500,1500,SLOW},
											};

		Uint8 i = 0;
		switch(state){
		case INIT:
			state = DEPART;
			break;
		case DEPART:
			state = try_going(1000,COLOR_Y(500),DEPART,PAS1,PAS1,SLOW,FORWARD,NO_AVOIDANCE);
			break;
		case PAS1:
			state = try_going_multipoint(tab,8,PAS1,PAS2ANGLE1,PAS2ANGLE1,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case PAS2ANGLE1:
			state = try_go_angle(PI4096/2,PAS2ANGLE1,PAS2ANGLE2,PAS2ANGLE2,SLOW);
			break;
		case PAS2ANGLE2:
			state = try_go_angle(PI4096,PAS2ANGLE2,PAS2ANGLE3,PAS2ANGLE3,SLOW);
			break;
		case PAS2ANGLE3:
			state = try_go_angle(-PI4096/2,PAS2ANGLE3,PAS2ANGLE3,PAS2ANGLE3,SLOW);
			break;
		case PAS2ANGLE4:
			state = try_go_angle(0,PAS2ANGLE4,PAS3,PAS3,SLOW);
			break;
		case PAS3:
			state = try_going(850,COLOR_Y(1500),PAS3,PAS4,PAS4,SLOW,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;
		case PAS4:
			state = try_going(650,COLOR_Y(1500),PAS4,PAS5,PAS5,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;
		case PAS5:
			state = try_going(750,COLOR_Y(1500),PAS5,PAS6,PAS6,SLOW,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;
		case PAS6:
			state = try_going_multipoint(tab6,12,PAS6,PAS6,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
			if(state==PAS6) state = try_go_angle(PI4096/2,PAS6,PAS7,PAS7,SLOW);
			break;
		case PAS7:
			state = try_going_multipoint(rond,17,PAS8,DONE,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		case DONE:
			break;
		default :
			break;
		}
}



void Deplacement_Pierre_Strat_Samuel(void){
	static enum{
		INIT,
		DEPLACEMENT_A_PIERRE,
		A_viserMur,
		DEPLACEMENT_B_PIERRE,
		B_viserTriangle,
		DEPLACEMENT_C_PIERRE,
		DEPLACEMENT_D_PIERRE,
		DEPLACEMENT_E_PIERRE,
		E_viserArbre,
		DEPLACEMENT_F_PIERRE,
		F_viserCoin,
		DEPLACEMENT_G_PIERRE,
		G_viserArbre,
		DEPLACEMENT_H_PIERRE,
		DEPLACEMENT_I_PIERRE,
		DONE
	}state = INIT;


	displacement_t INIT_vers_A[]={
						{448,2498,SLOW},
						{505,2445,SLOW},
						{555,2398,SLOW},
						{626,2291,SLOW}};

	displacement_t A_vers_B[]={
						{669,2245,SLOW},
						{708,2188,SLOW},
						{723,2106,SLOW}};


	displacement_t B_vers_C[]={
						{723,2106,SLOW},
						{976,2095,SLOW}};

	displacement_t C_vers_D[]={
						{1000,2000,SLOW},
						{1000,1900,SLOW},
						{1100,1800,SLOW},
						{1200,1900,SLOW},
						{1339,2017,SLOW},
						{1457,2017,SLOW},
						{1589,2038,SLOW} };

	displacement_t D_vers_E[]={
						{1700,2000,SLOW},
						{1773,2284,SLOW} };

	displacement_t E_vers_F[]={
						{1789,2797,SLOW} };

	displacement_t F_vers_G[]={
						{1297,2813,SLOW}};

	displacement_t G_vers_H[]={
						{938,2781,SLOW},
						{740,1589,SLOW},
						{179,1247,SLOW}};

	displacement_t H_vers_I[]={
						{152,1717,SLOW}  };


	switch(state){
	case INIT:
		state = DEPLACEMENT_A_PIERRE;
		break;
	case DEPLACEMENT_A_PIERRE:
		state = try_going_multipoint(INIT_vers_A,4,DEPLACEMENT_A_PIERRE,A_viserMur,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case A_viserMur:
		state = try_go_angle(-3*PI4096/4,A_viserMur,DEPLACEMENT_B_PIERRE,DONE,SLOW);
		break;
	case DEPLACEMENT_B_PIERRE:
		state = try_going_multipoint(A_vers_B,3,DEPLACEMENT_B_PIERRE,B_viserTriangle,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case B_viserTriangle:
		state = try_go_angle(-3*PI4096/4,B_viserTriangle,DEPLACEMENT_C_PIERRE,DONE,SLOW);
		break;
	case DEPLACEMENT_C_PIERRE:
		state = try_going_multipoint(B_vers_C,2,DEPLACEMENT_C_PIERRE,DEPLACEMENT_D_PIERRE,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case DEPLACEMENT_D_PIERRE:
		state = try_going_multipoint(C_vers_D,7,DEPLACEMENT_D_PIERRE,DEPLACEMENT_E_PIERRE,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case DEPLACEMENT_E_PIERRE:
		state = try_going_multipoint(D_vers_E,2,DEPLACEMENT_E_PIERRE,E_viserArbre,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case E_viserArbre:
		state = try_go_angle(0,E_viserArbre,DEPLACEMENT_F_PIERRE,DONE,SLOW);
		break;
	case DEPLACEMENT_F_PIERRE:
		state = try_going_multipoint(E_vers_F,1,DEPLACEMENT_F_PIERRE,F_viserCoin,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case F_viserCoin:
		state = try_go_angle(PI4096/4,F_viserCoin,DEPLACEMENT_G_PIERRE,DONE,SLOW);
		break;
	case DEPLACEMENT_G_PIERRE:
		state = try_going_multipoint(F_vers_G,1,DEPLACEMENT_G_PIERRE,G_viserArbre,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case G_viserArbre:
		state = try_go_angle(PI4096/2,G_viserArbre,DEPLACEMENT_H_PIERRE,DONE,SLOW);
		break;
	case DEPLACEMENT_H_PIERRE:
		state = try_going_multipoint(G_vers_H,3,DEPLACEMENT_H_PIERRE,DEPLACEMENT_I_PIERRE,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case DEPLACEMENT_I_PIERRE:
		state = try_going_multipoint(H_vers_I,1,DEPLACEMENT_I_PIERRE,DONE,DONE,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
		break;
	case DONE:
		break;
	default:
		break;
	}
}



void K_Strat_Coupe(void){
	static enum{
		SORTIR = 0,
		VERRE_1,
		VERRE_2,
		POSER_VERRE,
		ASSIETTE_2,
		ASSIETTE_4,
		ASSIETTE_5,
		DONE,
	}state=SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR :
			sub_action = K_Sortie();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_2;
					break;
				case END_WITH_TIMEOUT:
					state=ASSIETTE_2;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_2;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case VERRE_1 :
			sub_action = K_push_half_row_glasses();
			switch(sub_action)
			{
				case END_OK:
					state=VERRE_2;
					break;
				case END_WITH_TIMEOUT:
					state=VERRE_2;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=VERRE_2;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case VERRE_2 :
			sub_action = K_push_back_row_glasses();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_2;
					break;
				case END_WITH_TIMEOUT:
					state=ASSIETTE_2;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_2;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case ASSIETTE_2 :
			sub_action = TEST_STRAT_assiettes_evitement_2();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_4;
					break;
				case END_WITH_TIMEOUT:
					state=ASSIETTE_4;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_4;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case POSER_VERRE :
			//sub_action = Lacher_verres();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_4;
					break;
				case END_WITH_TIMEOUT:
					state=ASSIETTE_4;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_4;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case ASSIETTE_4 :
			sub_action = Assiete_2_4_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_5;
					break;
				case END_WITH_TIMEOUT:
					state=ASSIETTE_5;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_5;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case ASSIETTE_5 :
			sub_action = Assiete_5_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;
				case END_WITH_TIMEOUT:
					state=DONE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=DONE;
					break;
				case IN_PROGRESS:
					break;
			}
			break;

		case DONE :
			break;
	}


}

error_e Assiete_2_4_lanceur(void){
	static error_e sub_action;
	static enum{
		ASSIETTE_2=0,
		ASSIETTE_4,
		LANCEUR,
				LANCEUR2,
		DONE,
	}state=ASSIETTE_2;

	switch (state)
	{
		case ASSIETTE_2:
			sub_action=TEST_STRAT_assiettes_evitement_2();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_4;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;

				case NOT_HANDLED:
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE_2;
					return NOT_HANDLED;
					break;
			}
			break;

		case ASSIETTE_4:
			sub_action=TEST_STRAT_assiettes_evitement_4();
			switch(sub_action)
			{
				case END_OK:
					state=LANCEUR;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;

				case NOT_HANDLED:
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE_2;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR:
			sub_action=TEST_Launcher_ball_gateau_double();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					return END_OK;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE_2;
					return NOT_HANDLED;
					break;
			}

			break;

			case LANCEUR2:
			sub_action=TEST_Launcher_ball_mid_double();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					return END_OK;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE_2;
					return NOT_HANDLED;
					break;
			}

			break;

		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

error_e Assiete_5_lanceur(void){
	static error_e sub_action;
	static enum{
		ASSIETTE=0,
		LANCEUR,
				LANCEUR2,
		DONE,
	}state=ASSIETTE;

	switch (state)
	{
		case ASSIETTE:
			sub_action=TEST_STRAT_assiettes_evitement_5();
			switch(sub_action)
			{
				case END_OK:
					state=LANCEUR;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;

		case LANCEUR:
			sub_action=TEST_Launcher_ball_gateau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					return END_OK;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;

		case LANCEUR2:
			sub_action=TEST_Launcher_ball_mid();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					return END_OK;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

void TEST_STRAT_ALEXIS() {
	enum state_e {
		EXTRACT,	//Avance devant pour pouvoir tourner après le début du match (on est collé au mur après calibration)
		DO_GLASSES,	//Faire les verres
		DO_PLATES,	//Faire les assiettes & lancer les cerises,
		PROTECT_GLASSES,	//On va dans notre zone de départ pour proteger les verres
		PUT_DOWN_GLASSES,	//Va poser les verres dans la zone de Krusty si on ne l'avait pas fait. Ce n'est fait que a la fin du match et si on ne l'a pas déjà fait
		EXTRACT_FROM_GLASSES,	//S'écarte des verres si on les a posé a la fin du match
		DONE,		//On a fini
		NBSTATE		//Pas un état, utilisé pour savoir le nombre d'état
	};
	static enum state_e state = EXTRACT;
	static enum state_e last_state = EXTRACT;
	error_e sub_action;

	//On a changé d'état, on l'indique sur l'UART pour débugage
	if(state != last_state) {
		static const char* state_str[NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, DO_GLASSES);
			STATE_STR_DECLARE(state_str, DO_PLATES);
			STATE_STR_DECLARE(state_str, PROTECT_GLASSES);
			STATE_STR_DECLARE(state_str, PUT_DOWN_GLASSES);
			STATE_STR_DECLARE(state_str, EXTRACT_FROM_GLASSES);
			STATE_STR_DECLARE(state_str, DONE);
			STATE_STR_INIT_UNDECLARED(state_str, NBSTATE);
			state_str_initialized = TRUE;
		}

//		STATECHANGE_log(LOG_LEVEL_Debug, "TEST_STRAT_ALEXIS: state changed: %s(%d) -> %s(%d)\n",
//			state_str[last_state], last_state,
//			state_str[state], state);
		STATECHANGE_log(SM_ID_KRUSTY_STRAT_ALEXIS, state_str[last_state], last_state, state_str[state], state);
	}

	last_state = state;

	switch(state) {
		//Avance devant pour pouvoir tourner après le début du match (on est collé au mur après calibration)
		case EXTRACT:
			state = try_relative_move(70,  EXTRACT, DO_GLASSES, DO_GLASSES, FAST, FORWARD, END_AT_BREAK);
			break;

		//Faire les verres
		case DO_GLASSES:
			sub_action = K_STRAT_sub_glasses_alexis(FALSE, TRUE);
			state = check_sub_action_result(sub_action, DO_GLASSES, DO_PLATES, DO_PLATES);	//Dans tous les cas on fait la suite ...
			break;

		//Faire les assiettes & lancer les cerises
		case DO_PLATES:
			if(global.env.must_drop_glasses_at_end && global.env.match_time > (MATCH_DURATION - 20000))
				state = PUT_DOWN_GLASSES;
			else {
				sub_action = K_STRAT_sub_cherries_alexis();
				state = check_sub_action_result(sub_action, DO_PLATES, PROTECT_GLASSES, PROTECT_GLASSES);	//Idem

				if(state == PROTECT_GLASSES && global.env.must_drop_glasses_at_end)
					state = PUT_DOWN_GLASSES;
			}
			break;

		//On va dans notre zone de départ pour proteger les verres
		case PROTECT_GLASSES:
		{
			static bool_e timeout_set = FALSE;
			if(timeout_set == FALSE) {
				AVOIDANCE_set_timeout(MATCH_DURATION - global.env.match_time);
				timeout_set = TRUE;
			}
			if(global.env.glasses_x_pos > 600)
				state = try_going(1000, COLOR_Y(400), PROTECT_GLASSES, DONE, DONE, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			else state = try_going(280, COLOR_Y(400), PROTECT_GLASSES, DONE, DONE, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;
		}

		//Va poser les verres dans la zone de Krusty si on ne l'avait pas fait. Ce n'est fait que a la fin du match et si on ne l'a pas déjà fait
		//Si on fail, on tente quand même, il ne reste que 10sec !!!!
		case PUT_DOWN_GLASSES:
			state = try_going_multipoint((displacement_t[]){{{1000, COLOR_Y(430)}, FAST}, {{1000, COLOR_Y(220)}, FAST}}, 2,
					PUT_DOWN_GLASSES, EXTRACT_FROM_GLASSES, PUT_DOWN_GLASSES, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);

			//Si on a fini, on pose les verres
			if(state == EXTRACT_FROM_GLASSES) {
				if(!GLASS_SENSOR_LEFT)  //S'il n'y a pas de verre en bas, on descend l'ascenseur avant
					ACT_lift_translate(ACT_LIFT_Left, ACT_LIFT_TranslateDown);
				ACT_lift_plier(ACT_LIFT_Left, ACT_LIFT_PlierOpen);
				if(!GLASS_SENSOR_RIGHT)  //S'il n'y a pas de verre en bas, on descend l'ascenseur avant
					ACT_lift_translate(ACT_LIFT_Right, ACT_LIFT_TranslateDown);
				ACT_lift_plier(ACT_LIFT_Right, ACT_LIFT_PlierOpen);
			}
			break;

		case EXTRACT_FROM_GLASSES:
			state = try_relative_move(50, EXTRACT_FROM_GLASSES, DONE, DONE, FAST, BACKWARD, END_AT_LAST_POINT);
			break;

		//On a fini
		case DONE:
			//Rien a faire on attend la fin de match
			break;

		//Non utilisé
		case NBSTATE:
			break;
	}
}


//////
//TODO:
//Bizarre fin de match tôt
//Evitement: tourne dans la case, a voir c'est dangereux avec le buffet
//////
void TEST_STRAT_ALEXIS_FINALE() {
	enum state_e {
		INIT,				//Initialise la machine a etat
		EXTRACT,			//Avance devant pour pouvoir tourner après le début du match (on est collé au mur après calibration)
		DO_GLASSES,			//Faire les verres
		GOTO_PLATE,			//Va a une assiette (coté gateau)
		DO_PLATE,			//Prend l'assiette
		PUT_DOWN_GLASSES,	//Pose les verres
		LAUNCH_CHERRY,		//Lance les cerises
		MOVE_TO_DROP,		//Va pour lacher l'assiette
		DROP_PLATE,			//Lache l'assiette
		DO_NEXT_PLATE,		//Fait la suivante (celle du milieu puis celle coté cadeau)
		FAILED,				//gère les erreurs
		DONE,				//On a fini
		NBSTATE				//Pas un état, utilisé pour savoir le nombre d'état
	};
	static enum state_e state = INIT;
	static enum state_e last_state = INIT;
	static enum state_e last_state_for_check_entrance = INIT;

	static Sint16 DROP_ANGLE = 6434; //6434 = 90° //10366;	//10366 rad*4096 == 145°

	static Uint8 current_plate;
	static bool_e glasses_droped;

	bool_e entrance = (state == last_state_for_check_entrance)? FALSE:TRUE;
	last_state_for_check_entrance = state;
	error_e sub_action;

	//On a changé d'état, on l'indique sur l'UART pour débugage
	if(entrance) {
		static const char* state_str[NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, DO_GLASSES);
			STATE_STR_DECLARE(state_str, GOTO_PLATE);
			STATE_STR_DECLARE(state_str, DO_PLATE);
			STATE_STR_DECLARE(state_str, PUT_DOWN_GLASSES);
			STATE_STR_DECLARE(state_str, LAUNCH_CHERRY);
			STATE_STR_DECLARE(state_str, MOVE_TO_DROP);
			STATE_STR_DECLARE(state_str, DROP_PLATE);
			STATE_STR_DECLARE(state_str, DO_NEXT_PLATE);
			STATE_STR_DECLARE(state_str, DONE);
			STATE_STR_INIT_UNDECLARED(state_str, NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(SM_ID_KRUSTY_STRAT_ALEXIS_FINALE, state_str[last_state], last_state, state_str[state], state);
	}

	switch(state) {
		case INIT:
			if(global.env.color == BLUE)
				current_plate = 2;
			else current_plate = 4;
			glasses_droped = FALSE;
			state = EXTRACT;
			global.env.must_drop_glasses_at_end = TRUE;
			break;

		//Avance devant pour pouvoir tourner après le début du match (on est collé au mur après calibration)
		case EXTRACT:
			state = try_relative_move(70,  EXTRACT, DO_GLASSES, FAILED, FAST, FORWARD, END_AT_BREAK);
			break;

		//Faire les verres
		case DO_GLASSES:
			sub_action = K_STRAT_sub_glasses_alexis(FALSE, FALSE);
			state = check_sub_action_result(sub_action, DO_GLASSES, GOTO_PLATE, FAILED);	//Dans tous les cas on fait la suite ...
			break;

		//Va a une assiette (coté gateau)
		case GOTO_PLATE:
			sub_action = K_STRAT_micro_move_to_plate(current_plate, LP_Near, FALSE);
			state = check_sub_action_result(sub_action, GOTO_PLATE, DO_PLATE, FAILED);	//TODO: CAS DERREUR A GERER !!!!!!
			break;

		//Faire les assiettes
		case DO_PLATE: {
			if(current_plate == 3 || current_plate == 4 || current_plate == 1) {
				plate_info_t plate_info = PLATE_get_info(current_plate);	//4ème assiette a partir de 0
				//TODO: #warning "Code moche mais bon il passera pas le concours de beauté de toute façon"
				sub_action = K_STRAT_micro_grab_plate(STRAT_PGA_Y, STRAT_PGA_Y, TRUE, TRUE, plate_info.x, 200);
				state = check_sub_action_result(sub_action, DO_PLATE, PUT_DOWN_GLASSES, FAILED);	//Idem
			} else state = PUT_DOWN_GLASSES;
			break;
		}

		//Va poser les verres dans la zone de Krusty si on ne l'avait pas fait. Ce n'est fait que a la fin du match et si on ne l'a pas déjà fait
		//Si on fail, on tente quand même, il ne reste que 10sec !!!!
		case PUT_DOWN_GLASSES:
			if(glasses_droped == FALSE) {
				sub_action = K_STRAT_micro_put_down_glasses();
				state = check_sub_action_result(sub_action, PUT_DOWN_GLASSES, LAUNCH_CHERRY, FAILED);	//Idem
				if(state == LAUNCH_CHERRY)
					glasses_droped = TRUE;
			} else state = LAUNCH_CHERRY;
			break;

		//Lance les cerises
		case LAUNCH_CHERRY:
			sub_action = K_STRAT_micro_launch_cherries(STRAT_LC_PositionNear, 8, FALSE);
			state = check_sub_action_result(sub_action, LAUNCH_CHERRY, MOVE_TO_DROP, FAILED);	//Idem
			break;

		case MOVE_TO_DROP:
			if(entrance)
				DROP_ANGLE = 6434;
			sub_action = K_STRAT_micro_move_to_plate(current_plate, LP_Near, FALSE);
			state = check_sub_action_result(sub_action, MOVE_TO_DROP, DROP_PLATE, FAILED);	//TODO: CAS DERREUR A GERER !!!!!!
			break;

		//Lache l'assiette
		case DROP_PLATE:
			sub_action = K_STRAT_micro_drop_plate(TRUE, DROP_ANGLE);
			state = check_sub_action_result(sub_action, DROP_PLATE, DO_NEXT_PLATE, FAILED);	//Idem
			break;

		//Fait la suivante (celle du milieu puis celle coté cadeau)
		case DO_NEXT_PLATE:
			state = GOTO_PLATE;
			if(current_plate == 4 || current_plate == 2)
				current_plate = 3;
			else if(current_plate == 3)
				current_plate = 1;
			else
				state = LAUNCH_CHERRY;
			break;

		case FAILED:
			state = DONE;
			switch(last_state) {
				case DO_GLASSES:
					state = GOTO_PLATE;
					break;

				case GOTO_PLATE:
					if(glasses_droped == FALSE) {
						current_plate = 2;
						state = GOTO_PLATE;
					} else state = DO_NEXT_PLATE;
					break;

				case DO_PLATE:
					state = DROP_PLATE;
					break;

				case PUT_DOWN_GLASSES:
					current_plate = 2;
					state = DO_PLATE;
					break;

				case LAUNCH_CHERRY:
					state = DROP_PLATE;
					break;

				case MOVE_TO_DROP:
					DROP_ANGLE = 10366;
					state = DROP_PLATE;
					break;

				case DROP_PLATE:
					state = DO_NEXT_PLATE;
					break;

				default:
					state = DONE;
					break;
			}
			break;

		//On a fini
		case DONE:
			//Rien a faire on attend la fin de match
			break;

		//Non utilisé
		case NBSTATE:
			break;
	}

	last_state = last_state_for_check_entrance; //last_state contient l'état avant de passer dans le switch, pour être utilisable dans les états quand entrance vaut TRUE
}


error_e drink(void){
	static enum{
		DRINK,
		ALCOOLIC,
		DROP
	}state = DRINK;

	static error_e sub_action;
	error_e ret = IN_PROGRESS;

	switch(state){
		case DRINK:
			sub_action = two_first_rows();
			if(sub_action != IN_PROGRESS)
				state = ALCOOLIC;
			break;
		case ALCOOLIC:
			sub_action = try_last_row();
			if(sub_action != IN_PROGRESS)
				state = DROP;
			break;
		case DROP:
			sub_action = Lacher_verres(1);
			if(sub_action != IN_PROGRESS)
				ret = END_OK;
			break;
	}
	return ret;
}


void K_test_strat_unitaire(void){
	static enum{
		SORTIR,
		ACTION,
		DONE
	}state = SORTIR;

	static error_e sub_action;

	/*if(GLASS_SENSOR_LEFT)//gauche
		LED_ERROR = 1;
	else
		LED_ERROR = 0;

	if(GLASS_SENSOR_RIGHT)//droite
		LED_USER2 = 1;
	else
		LED_USER2 = 0;*/
	switch(state){
		case SORTIR:
			sub_action = goto_pos(1000,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = ACTION;
					break;
				case END_OK:
					state = ACTION;
					break;
				case END_WITH_TIMEOUT:
					state = ACTION;
					break;
				default:
					state = ACTION;
					break;
			}
			break;
		case ACTION:
			sub_action = drink(); //Mettez ici le nom de votre micro-strat à tester
			//sub_action = two_first_rows(); //Mettez ici le nom de votre micro-strat à tester
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
	}
}

void K_TEST_STRAT_avoidance(void){
	static enum{
		SORTIR,
		DEPLACEMENT1,
		DEPLACEMENT2,
		DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DEPLACEMENT1;
					break;
				case END_OK:
					state = DEPLACEMENT1;
					break;
				case END_WITH_TIMEOUT:
					state = DEPLACEMENT1;
					break;
				default:
					state = DEPLACEMENT1;
					break;
			}

			break;

		case DEPLACEMENT1:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{500, COLOR_Y(700)},SLOW},
					{{500, COLOR_Y(1000)},SLOW}},2,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				case FOE_IN_PATH:
					state = DONE;
					break;

				default:
					state = DONE;
					break;
			}
			break;

		case DEPLACEMENT2:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(380)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				case FOE_IN_PATH:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;

	}


}

void TEST_STRAT_kdo(void){
	static enum{
		SORTIR = 0,
				KDO1,
				KDO2,
				KDO3,
				KDO4,
				DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO1;
					break;
				case END_OK:
					state = KDO1;
					break;
				case END_WITH_TIMEOUT:
					state = KDO1;
					break;
				default:
					state = KDO1;
					break;
			}
			break;

		case KDO1:
			sub_action = K_CADEAU1();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO2;
					break;
				case END_OK:
					state = KDO2;
					break;
				case END_WITH_TIMEOUT:
					state = KDO2;
					break;
				default:
					state = KDO2;
					break;
			}
			break;

		case KDO2:
			sub_action = K_CADEAU2();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO3;
					break;
				case END_OK:
					state = KDO3;
					break;
				case END_WITH_TIMEOUT:
					state = KDO3;
					break;
				default:
					state = KDO3;
					break;
			}
			break;

		case KDO3:
			sub_action = K_CADEAU3();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO4;
					break;
				case END_OK:
					state = KDO4;
					break;
				case END_WITH_TIMEOUT:
					state = KDO4;
					break;
				default:
					state = KDO4;
					break;
			}
			break;

		case KDO4:
			sub_action = K_CADEAU4();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
	}
}


void TEST_STRAT_assiettes(void){
	static enum{
		SORTIR = 0,
				ASSIETTE1,
				DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = ASSIETTE1;
					break;
				case END_OK:
					state = ASSIETTE1;
					break;
				case END_WITH_TIMEOUT:
					state = ASSIETTE1;
					break;
				default:
					state = ASSIETTE1;
					break;
			}
			break;

		case ASSIETTE1:
			sub_action = K_ASSIETE1();

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
	}

}

void TEST_STRAT_assiettes_lanceur(void){
	static enum{
		SORTIR = 0,
				ASSIETTE1,
						LANCEUR,
						DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = ASSIETTE1;
					break;
				case END_OK:
					state = ASSIETTE1;
					break;
				case END_WITH_TIMEOUT:
					state = ASSIETTE1;
					break;
				default:
					state = ASSIETTE1;
					break;
			}
			break;

		case ASSIETTE1:
			sub_action = K_ASSIETE1();

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = LANCEUR;
					break;
				case END_OK:
					state = LANCEUR;
					break;
				case END_WITH_TIMEOUT:
					state = LANCEUR;
					break;
				default:
					state = LANCEUR;
					break;
			}
			break;
		case DONE:
			break;
			case LANCEUR:
				sub_action = TEST_Launcher_ball_cadeau();
				switch(sub_action)
				{
					case END_OK:
						state=DONE;
						break;

					case END_WITH_TIMEOUT:


						state=DONE;
						break;
					case FOE_IN_PATH:
					case NOT_HANDLED:
						state=DONE;
						break;

					case IN_PROGRESS:
						break;
				}
					break;
		}

}


void TEST_Launcher_ball(void){
	static enum{
				LANCEUR_CADEAU=0,
				LANCEUR_MID,
				LANCEUR_GATEAU,
				STOP_ACT,
				DONE,
	}state=LANCEUR_CADEAU;

	static error_e sub_action;
	static ACT_function_result_e sub_action_act;

	switch(state){
		case LANCEUR_CADEAU:
			sub_action = TEST_Launcher_ball_cadeau();
					switch(sub_action)
			{
				case END_OK:
					state=LANCEUR_MID;
					break;

				case END_WITH_TIMEOUT:
					ACT_ball_launcher_stop();

					state=STOP_ACT;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=LANCEUR_GATEAU;
					break;

				case IN_PROGRESS:
					break;
			}
			break;
		case LANCEUR_MID:
			sub_action = TEST_Launcher_ball_mid();
					switch(sub_action)
			{
				case END_OK:
					state=LANCEUR_GATEAU;
					break;

				case END_WITH_TIMEOUT:
					ACT_ball_launcher_stop();
					state=STOP_ACT;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=LANCEUR_GATEAU;
					break;

				case IN_PROGRESS:
					break;
			}
			break;
		case LANCEUR_GATEAU:
			sub_action = TEST_Launcher_ball_gateau();
					switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					ACT_ball_launcher_stop();
					state=STOP_ACT;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=LANCEUR_CADEAU;
					break;

				case IN_PROGRESS:
					break;
			}
			break;
		case STOP_ACT:
			sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
			switch(sub_action_act)
			{
				case ACT_FUNCTION_InProgress:
					break;

				case ACT_FUNCTION_Done:
					state = DONE;
					break;

				case ACT_FUNCTION_ActDisabled:
					state=DONE;
					break;

				case ACT_FUNCTION_RetryLater:
					state=DONE;
					break;
				default:
					state=DONE;
					break;
			}
			break;
		case DONE:
			break;

	}
}

void TEST_STRAT_verres(void){
	static enum{
		SORTIR = 0,
		GO,
		NEXT,
		DONE
	}state = GO;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(1000,COLOR_Y(350),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = GO;
					break;
				case END_WITH_TIMEOUT:
					state = GO;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state = GO;
					break;
			}
		case GO:
			sub_action = K_push_half_row_glasses();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case END_OK:
				default:
					state = NEXT;
					break;
			}
			break;

		case NEXT:
			sub_action = K_push_back_row_glasses();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case END_OK:
				default:
					state = DONE;
					break;
			}
			break;

		case DONE:
			break;
		default:
			debug_printf("fuyez ca va peter");
			break;
	}
}



/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */



void TEST_STRAT_assiettes_evitement(void){

	static error_e sub_action;
	static ACT_function_result_e sub_action_act;
	static enum {
		POS_MOVE = 0,
				POS_MOVE2,
				POS_MOVE3,
				GRABBER_DOWN ,
				GRABBER_DOWN_ATT,
				GRABBER_OPEN,
				GRABBER_OPEN_ATT,
		PUSH,
				GRABBER_CRUSH,
				GRABBER_CRUSH_ATT,
		GRABBER_UP,
				GRABBER_UP_ATT,
				BACK,
				DONE,
	} state = POS_MOVE;

//    static bool_e timeout = FALSE;

	switch (state) {
		case POS_MOVE:

			//sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(800)}}},1,FORWARD,NORMAL_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=POS_MOVE2;

					break;

				case END_WITH_TIMEOUT:
					state=POS_MOVE2;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=POS_MOVE2;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		 case POS_MOVE2:

			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(800)}}},1,FORWARD,NORMAL_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=POS_MOVE3;
					break;

				case END_WITH_TIMEOUT:
					state=POS_MOVE3;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=POS_MOVE3;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case POS_MOVE3:

			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(600)}}},1,BACKWARD,NORMAL_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=GRABBER_DOWN;
					break;

				case END_WITH_TIMEOUT:
					state=GRABBER_DOWN;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=GRABBER_DOWN;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case GRABBER_DOWN:

			ACT_plate_rotate(ACT_PLATE_RotateDown);
			state =GRABBER_DOWN_ATT;
			break;
		 case GRABBER_DOWN_ATT:
			sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
			switch(sub_action_act)
			{
				case ACT_FUNCTION_InProgress:
					break;

				case ACT_FUNCTION_Done:
					state = GRABBER_OPEN;
					break;

				case ACT_FUNCTION_ActDisabled:
					//state = GRABBER_TIDY_2;

					break;

				case ACT_FUNCTION_RetryLater:
					 //state = GRABBER_TIDY_2;

					break;
			}
			break;



		case GRABBER_OPEN:

			ACT_plate_plier(ACT_PLATE_PlierOpen);
			state =GRABBER_OPEN_ATT;
			break;
		 case GRABBER_OPEN_ATT:
			sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
			switch(sub_action_act)
			{
				case ACT_FUNCTION_InProgress:
					break;

				case ACT_FUNCTION_Done:
					state = PUSH;
					break;

				case ACT_FUNCTION_ActDisabled:
					state = PUSH;

					break;

				case ACT_FUNCTION_RetryLater:
					 state = PUSH;

					break;
			}
			break;

		case PUSH:

			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(400)}}},1,BACKWARD,NORMAL_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=GRABBER_CRUSH;
					break;

				case END_WITH_TIMEOUT:
					state=GRABBER_CRUSH;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=GRABBER_CRUSH;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case GRABBER_CRUSH:
			ACT_plate_plier(ACT_PLATE_PlierClose);
			state =GRABBER_CRUSH_ATT;
			break;
		 case GRABBER_CRUSH_ATT:
			sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
			switch(sub_action_act)
			{
				case ACT_FUNCTION_InProgress:
					break;

				case ACT_FUNCTION_Done:
					state = GRABBER_UP;
					break;

				case ACT_FUNCTION_ActDisabled:
					state = GRABBER_UP;

					break;

				case ACT_FUNCTION_RetryLater:
					 state = GRABBER_UP;

					break;
			}
			break;

		case GRABBER_UP:

			ACT_plate_rotate(ACT_PLATE_RotateUp);
			state =GRABBER_UP_ATT;
			break;
		 case GRABBER_UP_ATT:
			sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
			switch(sub_action_act)
			{
				case ACT_FUNCTION_InProgress:
					break;

				case ACT_FUNCTION_Done:
					state = BACK;
					break;

				case ACT_FUNCTION_ActDisabled:
					//state = GRABBER_TIDY_2;

					break;

				case ACT_FUNCTION_RetryLater:
					 //state = GRABBER_TIDY_2;

					break;
			}
			break;
		case BACK:

			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(700)}}},1,FORWARD,NORMAL_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=DONE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case DONE:
			break;
	}
}

void Test_STRAT_homolo(void){
	static error_e sub_action;

	static enum{
		SORTIE = 0,
		VERRE_ALLER,
		VERRE_RETOUR,
		DONE,
	}state=SORTIE;

	switch(state){
		case SORTIE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(300)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=VERRE_ALLER;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=DONE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case VERRE_ALLER:
			sub_action=K_push_half_row_glasses_HOMOLO();
			switch(sub_action)
			{
				case END_OK:
					state=VERRE_RETOUR;
					break;

				case END_WITH_TIMEOUT:
					state=VERRE_RETOUR;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state = VERRE_RETOUR;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case VERRE_RETOUR:
			sub_action=K_push_back_row_glasses_HOMOLO();
			switch(sub_action)
			{
				case END_OK:
					state= DONE;
					break;

				case END_WITH_TIMEOUT:
					state= DONE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state = DONE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case DONE:
			break;
	}
}

void Test_STRAT_COUPE(void){
	static error_e sub_action;

	static enum{
		SORTIE = 0,
		VERRE_ALLER,
		VERRE_RETOUR,
		ASSIETTE_GATEAU_BLEUE,
		ASSIETTE_GATEAU_BLANC,
		ASSIETTE_MILIEU,
		ASSIETTE_CADEAU_BLANC,
		ASSIETTE_CADEAU_BLEUE,
		MOVE_FINAL,
		CADEAU,
		DONE,
	}state=SORTIE;

	switch(state){
		case SORTIE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(500)}}},1,FORWARD,NO_AVOIDANCE);
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_CADEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_CADEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=ASSIETTE_CADEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case VERRE_ALLER:
			sub_action=K_push_half_row_glasses();
			switch(sub_action)
			{
				case END_OK:
					state=VERRE_RETOUR;
					break;

				case END_WITH_TIMEOUT:
					state=VERRE_RETOUR;
					break;
				case NOT_HANDLED:
					state = VERRE_RETOUR;
					break;

				case IN_PROGRESS:
					break;

				default:
					state = VERRE_RETOUR;
					break;
			}
			break;
		case VERRE_RETOUR:
			sub_action=K_push_back_row_glasses();
			switch(sub_action)
			{
				case END_OK:
					state= ASSIETTE_CADEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state= ASSIETTE_CADEAU_BLANC;
					break;
				case NOT_HANDLED:
					state = ASSIETTE_CADEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

				default:
					state = ASSIETTE_CADEAU_BLANC;
					break;
			}
			break;

		case ASSIETTE_CADEAU_BLEUE:
			sub_action=Assiete1_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_CADEAU_BLANC:
			sub_action=Assiete2_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_MILIEU:
			sub_action=Assiete3_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_GATEAU_BLANC:
			sub_action=Assiete4_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLEUE;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLEUE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=ASSIETTE_GATEAU_BLEUE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_GATEAU_BLEUE:
			sub_action=Assiete_5_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=VERRE_ALLER;
					break;

				case END_WITH_TIMEOUT:
					state=VERRE_ALLER;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=VERRE_ALLER;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case CADEAU:
			sub_action=Cadeau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=CADEAU;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=CADEAU;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case MOVE_FINAL :
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{740, COLOR_Y(555)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=DONE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case DONE:
			break;
	}
	//state=ASSIETTE_CADEAU_BLEUE;
}

void Test_STRAT_COUPE_2(void){
	static error_e sub_action;

	static enum{
		SORTIE = 0,
		VERRE_ALLER,
		VERRE_RETOUR,
		ASSIETTE_GATEAU_BLEUE,
		ASSIETTE_GATEAU_BLANC,
		ASSIETTE_MILIEU,
		ASSIETTE_CADEAU_BLANC,
		ASSIETTE_CADEAU_BLEUE,
		MOVE_FINAL,
		CADEAU,
		DONE,
	}state=VERRE_ALLER;

	switch(state){
		case SORTIE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(500)}}},1,FORWARD,NO_AVOIDANCE);
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_CADEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_CADEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=ASSIETTE_CADEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case VERRE_ALLER:
			sub_action=K_push_half_row_glasses();
			switch(sub_action)
			{
				case END_OK:
					debug_printf("VERRE_ALLEREND OK%d\n",__LINE__);
					state=VERRE_RETOUR;
					break;

				case END_WITH_TIMEOUT:
					debug_printf("VERRE_ALLER END_WITH_TIMEOUT%d\n",__LINE__);
					state=VERRE_RETOUR;
					break;
				case NOT_HANDLED:
					debug_printf("VERRE_ALLER NOT_HANDLED%d\n",__LINE__);
					state = VERRE_RETOUR;
					break;

				case IN_PROGRESS:
					break;

				default:
					debug_printf("VERRE_ALLER DEFAULT%d\n",__LINE__);
					state = VERRE_RETOUR;
					break;
			}
			break;
		case VERRE_RETOUR:
			sub_action=K_push_back_row_glasses();
			switch(sub_action)
			{
				case END_OK:
					state= ASSIETTE_CADEAU_BLANC;
					debug_printf("VERRE_RETOUR END OK%d\n",__LINE__);
					break;

				case END_WITH_TIMEOUT:
					debug_printf("VERRE_RETOUR END_WITH_TIMEOUT%d\n",__LINE__);
					state= ASSIETTE_CADEAU_BLANC;
					break;
				case NOT_HANDLED:
					 debug_printf("VERRE_RETOUR NOT_HANDLED%d\n",__LINE__);
					state = ASSIETTE_CADEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

				default:
					debug_printf("VERRE_RETOUR DEFAULT%d\n",__LINE__);
					state = ASSIETTE_CADEAU_BLANC;
					break;
			}
			break;

		case ASSIETTE_CADEAU_BLEUE:
			sub_action=Assiete1_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_CADEAU_BLANC:
			sub_action=Assiete2_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_MILIEU:
			sub_action=Assiete3_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLANC;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=ASSIETTE_GATEAU_BLANC;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_GATEAU_BLANC:
			sub_action=Assiete4_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=ASSIETTE_GATEAU_BLEUE;
					break;

				case END_WITH_TIMEOUT:
					state=ASSIETTE_GATEAU_BLEUE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=ASSIETTE_GATEAU_BLEUE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case ASSIETTE_GATEAU_BLEUE:
			sub_action=Assiete_5_lanceur();
			switch(sub_action)
			{
				case END_OK:
					state=MOVE_FINAL;
					break;

				case END_WITH_TIMEOUT:
					state=MOVE_FINAL;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=MOVE_FINAL;
					break;

				case IN_PROGRESS:
					break;

			}
			break;

		case CADEAU:
			sub_action=Cadeau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=CADEAU;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state=CADEAU;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case MOVE_FINAL :
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{740, COLOR_Y(555)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					 state=DONE;
					break;

				case IN_PROGRESS:
					break;

			}
			break;
		case DONE:
			break;
	}
	//state=ASSIETTE_CADEAU_BLANC;
}

error_e Assiete4_lanceur(void){
	static error_e sub_action;
	static enum{
		ASSIETTE=0,
		LANCEUR_1,
		LANCEUR_2,
		DONE,
	}state=ASSIETTE;

	switch (state)
	{
		case ASSIETTE:
			sub_action=TEST_STRAT_assiettes_evitement_4();
			switch(sub_action)
			{
				case END_OK:
					state=LANCEUR_1;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;

		case LANCEUR_1:
			sub_action=TEST_Launcher_ball_gateau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					return END_OK;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_2:
			sub_action=TEST_Launcher_ball_mid();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					return END_OK;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_1;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}

			break;
		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

error_e Assiete3_lanceur(void){
	static error_e sub_action;
	static enum{
		ASSIETTE=0,
		LANCEUR_1,
		LANCEUR_2,
		DONE,
	}state=ASSIETTE;

	switch (state)
	{
		case ASSIETTE:
			sub_action=TEST_STRAT_assiettes_evitement_3();
			switch(sub_action)
			{
				case END_OK:
					state=LANCEUR_1;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_1:
			sub_action=TEST_Launcher_ball_mid();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_2:
			sub_action=TEST_Launcher_ball_gateau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_1;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}

			break;
		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

error_e Assiete2_lanceur(void){
	static error_e sub_action;
	static enum{
		ASSIETTE=0,
		LANCEUR_1,
		LANCEUR_2,
		DONE,
	}state=ASSIETTE;

	switch (state)
	{
		case ASSIETTE:
			sub_action=TEST_STRAT_assiettes_evitement_2();
			switch(sub_action)
			{
				case END_OK:
					state=LANCEUR_1;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					//state=ASSIETTE;
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_1:
			sub_action=TEST_Launcher_ball_mid();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_2:
			sub_action=TEST_Launcher_ball_cadeau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_1;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}

			break;
		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}

error_e Assiete1_lanceur(void){
	static error_e sub_action;
	static enum{
		ASSIETTE=0,
		LANCEUR_1,
		LANCEUR_2,
		DONE,
	}state=ASSIETTE;

	switch (state)
	{
		case ASSIETTE:
			sub_action=TEST_STRAT_assiettes_evitement_1();
			switch(sub_action)
			{
				case END_OK:
					state=LANCEUR_1;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_1;

					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_1:
			sub_action=TEST_Launcher_ball_cadeau();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
					break;
				case NOT_HANDLED:
					state=LANCEUR_2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}
			break;
		case LANCEUR_2:
			sub_action=TEST_Launcher_ball_mid();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
				case NOT_HANDLED:
					state=LANCEUR_1;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=ASSIETTE;
					return NOT_HANDLED;
					break;
			}

			break;
		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}


error_e Cadeau(void){
	static error_e sub_action;
	static enum{
		CADEAU_1=0,
				CADEAU_2,
				CADEAU_3,
				CADEAU_4,
				DONE,
	}state=CADEAU_1;
	switch(state)
	{
		case CADEAU_1:
			sub_action=K_CADEAU1();
			switch(sub_action)
			{
				case END_OK:
					state=CADEAU_2;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
				case NOT_HANDLED:
					state=CADEAU_2;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=CADEAU_1;
					return NOT_HANDLED;
					break;
			}
			break;

		case CADEAU_2:
			sub_action=K_CADEAU2();
			switch(sub_action)
			{
				case END_OK:
					state=CADEAU_3;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
				case NOT_HANDLED:
					state=CADEAU_3;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=CADEAU_1;
					return NOT_HANDLED;
					break;
			}
			break;

		case CADEAU_3:
			sub_action=K_CADEAU3();
			switch(sub_action)
			{
				case END_OK:
					state=CADEAU_4;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
				case NOT_HANDLED:
					state=CADEAU_4;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=CADEAU_1;
					return NOT_HANDLED;
					break;
			}
			break;

		case CADEAU_4:
			sub_action=K_CADEAU4();
			switch(sub_action)
			{
				case END_OK:
					state=DONE;
					break;

				case END_WITH_TIMEOUT:
					state=DONE;
					return END_WITH_TIMEOUT;
				case NOT_HANDLED:
					state=CADEAU_1;
					break;

				case IN_PROGRESS:
					return IN_PROGRESS;
					break;

				default:
					state=CADEAU_1;
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			return END_OK;
			break;
		default:
			state=CADEAU_1;
			return NOT_HANDLED;
			break;


	}
	return IN_PROGRESS;
}
