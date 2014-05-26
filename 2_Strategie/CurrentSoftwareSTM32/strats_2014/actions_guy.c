/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_guy.c
 *	Package : Carte S²/strats2013
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par Anthony.
 *	Version 2013/10/03
 */

#include "actions_guy.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../high_level_strat.h"
#include "../elements.h"
#include "../Geometry.h"
#include "../Pathfind.h"
#include "../zone_mutex.h"
#include "../QS/QS_can_over_xbee.h"
#include "../act_functions.h"
#include "../config/config_pin.h"
#include "../maths_home.h"
#include "../Supervision/SD/SD.h"
#include "../Supervision/Buzzer.h"
#include "actions_both_2014.h"
#include <math.h>

static void REACH_POINT_GET_OUT_INIT_send_request();
error_e goto_adversary_zone(void);
static void send_message_to_pierre(Uint11 sid, Uint8 data0);
//TODO Si absence de Pierre, on doit être capable de compiler un code de GUY qui met un max de points (tout nos feux... !)


//SWITCH_STRAT2 et SWITCH_STRAT1 permettent de choisir le chemin à emprunter pour se rendre chez l'adversaire
//Voir le case INIT de la subaction initiale
//ATTENTION, la route coté mammouths suggère fortement que Pierre commence par la torche et non par la fresque :: sinon conflit !

#define MAX_HEIGHT_ARM	143
#define DIST_RETURN_RETURN_TRIANGLE		250

#define DROP_TRIANGLE_UNDER_TREE    // Va deposer l'un des deux triangles sous les arbres
#define DETECTION_TRIANGLE_MIDDLE	// Pour savoir si nous avons besoins de faire un dection des triangles du milieu ou non pour la strat triangles_between_tree
#define DIM_TRIANGLE 100

#define DISTANCE_BETWEEN_GUY_AND_HEARTH	80	//Entre le centre de guy, et le foyer contre lequel il se place
#define CENTRAL_HEARTH_RADIUS			150
//#define GUY_FALL_FIRST_FIRE // Si on souhaite faire tomber le premier feux dés le début

// Fonctionne que pour les chemins MAMMOUTH_SIDE et HEART_SIDE
bool_e rush_to_torch = FALSE;  // Si FALSE va faire tomber un ou des triangle(s) avant
bool_e fall_fire_wall_adv = TRUE;  // Va aller faire tomber le feu si on sait que l'ennemis ne le fais pas tomber des le debut

#define	DISPOSE_POINT_X		1400//Point où l'on se rend avec NOTRE torche
#define	DISPOSE_POINT_Y		1500//Point où l'on se rend avec NOTRE torche

//IL FAUT définir une zone de dépose pour la torche adverse. (si on a réussi à la prendre)
	#define DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH
	//#define DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH
	//#define DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH

//IL FAUT définir une zone de dépose en cas d'échec de la première tentative pour la torche adverse. (si on a réussi à la prendre)
	//#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH
	//#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH
	//#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH
	#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_NO_DISPOSE

#define DIM_START_TRAVEL_TORCH 200
#define RADIUS_TORCH 80

typedef enum{
	NORTH_MAMMOUTH = 0,
	NORTH_HEART,
	TORCH_ROAD,
	SOUTH_HEART,
	SOUTH_TREES
}initial_path_e;	//Chemin initial choisi pour se rendre du coté adverse



pos_scan_t pos_scan[2] = {{{1200,1800},{1200,1900},600,1600,1800,2300,-PI4096/2,0}, // FOYER CENTRALE
						  {{1650,2600},{1600,2550},800,1700,2000,2700,PI4096/2,PI4096}		// FOYER ADVERSE
						 };

volatile initial_path_e initial_path;
static bool_e pierre_reach_point_C1 = FALSE;
static bool_e we_have_a_torch = FALSE;
/*
 * Le but de la strat_initiale de guy est :
 * 	1- se rendre dans la zone adverse, par un chemin choisi (+ gestion des cas d'erreurs) => sous traité à ############
 *  2- si activé -> voler la torche adverse + la déposer à l'endroit choisi -> puis rendre la main à high_level_strat
 *  3- sinon -> rendre la main à high_level_strat
 */
error_e sub_action_initiale_guy(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_INITIALE,
		INIT,
		SUCESS,
		GET_OUT_POS_START,
		FALL_FIRST_FIRE,
		ROTATION_IF_NOT_CALIBRATED,
		GOTO_ADVERSARY_ZONE,
		GOTO_TREE_INIT,
		FALL_FIRE_WALL_TREE,  // Fait tomber les deux feux contre le mur milieu
		WAIT_GOTO_MAMMOUTH_INIT,
		GOTO_MAMMOUTH_INIT,
		FALL_MOBILE_MM_ADV,
		GOTO_HEART_INIT,
		FALL_FIRE_MOBILE_TREE_ADV,
		GOTO_TORCH_ADVERSARY,
		DO_OUR_TORCH,
		DO_ADV_TORCH,
		FALL_FIRE_MOBILE_MM_ADV,
		FALL_FIRE_WALL_ADV,
		DONE,
		ERROR
	);

	//static displacement_t points[6];
	//static Uint8	nb_points = 0;
	static enum state_e success_state;
	static bool_e we_prevented_pierre_to_get_out = FALSE;
	static Sint16 y_to_prevent_pierre_to_get_out;
	static time32_t t;
	static torch_dispose_zone_e dispose_zone_for_adversary_torch = HEARTH_ADVERSARY;
	static torch_dispose_zone_e dispose_zone_for_our_torch = HEARTH_CENTRAL;
	static torch_dispose_zone_e if_fail_dispose_zone_for_adversary_torch = NO_DISPOSE;
	if(global.env.reach_point_C1)
		pierre_reach_point_C1 = TRUE;

	switch(state)
	{
		case INIT:
			we_have_a_torch = FALSE;
			we_prevented_pierre_to_get_out = FALSE;
			if(SWITCH_STRAT_2)	//NORTH ou SOUTH
			{
				if(SWITCH_STRAT_3)	//intérieur ou extérieur
					initial_path = SOUTH_TREES;		//Sud extérieur : proche des arbres
				else
					initial_path = SOUTH_HEART;		//Sud intérieur : proche du foyer central
			}
			else
			{
				if(SWITCH_STRAT_3)	//intérieur ou extérieur
					initial_path = NORTH_MAMMOUTH;	//Nord extérieur : proche des mammouths
				else
					initial_path = NORTH_HEART;		//Nord intérieur : proche du foyer central
			}


			if(SWITCH_SAVE)
			{
				initial_path = TORCH_ROAD;	//En fait, on prend la TORCH_ROAD !
				send_message_to_pierre(XBEE_GUY_HAVE_DONE_FIRE,FIRE_ID_TORCH_OUR);
				dispose_zone_for_our_torch = HEARTH_CENTRAL;	//On s'occupe de notre torche... des define pourront enrichir avec d'autres choix de zone de dépose.
			}
			else
				dispose_zone_for_our_torch = NO_DISPOSE;	//On ne s'occupe pas de notre torche

			if(SWITCH_STRAT_1)
			{
				#if !defined(DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH) && !defined(DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH) && !defined (DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH)
				#error "Vous devez définir l'un de ces 3 defines : DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH, DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH, DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH"
				#endif
				#ifdef DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH
					dispose_zone_for_adversary_torch = HEARTH_OUR;
				#endif
				#ifdef DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH
					dispose_zone_for_adversary_torch = HEARTH_ADVERSARY;
				#endif
				#ifdef DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH
					dispose_zone_for_adversary_torch = HEARTH_CENTRAL;
				#endif

				#if !defined(IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH) && !defined(IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH) && !defined (IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH) && !defined (IF_FAIL_DISPOSE_ADVERSARY_TORCH_NO_DISPOSE)
				#error "Vous devez définir l'un de ces 4 defines : DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH, DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH, DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH"
				#endif
				#ifdef IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH
					if_fail_dispose_zone_for_adversary_torch = HEARTH_OUR;
				#endif
				#ifdef IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH
					if_fail_dispose_zone_for_adversary_torch = HEARTH_ADVERSARY;
				#endif
				#ifdef IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH
					if_fail_dispose_zone_for_adversary_torch = HEARTH_CENTRAL;
				#endif
				#ifdef IF_FAIL_DISPOSE_ADVERSARY_TORCH_NO_DISPOSE
					if_fail_dispose_zone_for_adversary_torch = NO_DISPOSE;
				#endif
			}
			else
				dispose_zone_for_adversary_torch = NO_DISPOSE;

			#ifdef GUY_FALL_FIRST_FIRE
				if(global.env.asser.calibrated)
					state = FALL_FIRST_FIRE;
				else
			#else
					state = GET_OUT_POS_START;
			#endif

			break;

		case FALL_FIRST_FIRE:
			if(entrance)
			{
				t = global.env.match_time;
				//ATTENTION : le timeout supplémentaire ajouté ici doit être très court... car si le bras ne fonctionne pas, Pierre va vouloir partir et nous foncer dedans !
				//On réactive l'asservissement de la propulsion pour que le bras ne nous fasse pas tourner sur nous-même
				ASSER_set_correctors(TRUE,TRUE);
			}

			//TODO modifier si besoin (c'est probable la position du bras)
			//Attention, cette action est conditionnée par la calibration... !
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0,0,FALL_FIRST_FIRE,GET_OUT_POS_START,GET_OUT_POS_START);
			if(t > 1000)	//C'est grand temps de partir si on veut pas se faire *** par Pierre...
				state = GET_OUT_POS_START;

			if(state != FALL_FIRST_FIRE)	//Si on a fini (échec ou réussite...)
				ACT_arm_goto(ACT_ARM_POS_PARKED);	//On range le bras, sans attendre l'acquittement


			break;

		case GET_OUT_POS_START:
			if(entrance)
			{
				ASSER_set_acceleration(80);	//Acceleration de guy au démarrage...
				success_state = (global.env.asser.calibrated)?GOTO_ADVERSARY_ZONE:ROTATION_IF_NOT_CALIBRATED;
			}
			if(global.env.asser.calibrated)
				state  = try_going_until_break(700,COLOR_Y(300),GET_OUT_POS_START,success_state, success_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			else
				state  = try_going_until_break(635,COLOR_Y(300),GET_OUT_POS_START,success_state, success_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		/*case PREVENT_PIERRE_WE_ARE_GOT_OUT:
			if(entrance)
				REACH_POINT_GET_OUT_INIT_send_request();
			state = GOTO_ADVERSARY_ZONE;
			break;
		*/
		case ROTATION_IF_NOT_CALIBRATED:
			state = try_go_angle((global.env.color == RED)?PI4096/4:-PI4096/4,state,GOTO_ADVERSARY_ZONE,GOTO_ADVERSARY_ZONE,FAST);
			break;
		case GOTO_ADVERSARY_ZONE:
			if(entrance)
			{
				if(initial_path == NORTH_MAMMOUTH)
					y_to_prevent_pierre_to_get_out = 1000;	//Pas de COLOR_Y() ici !
				else
					y_to_prevent_pierre_to_get_out = 400;
			}
			if(!we_prevented_pierre_to_get_out)
			{
				//ATTENTION, on utilise pas les warners qui peuvent ainsi être utilisés pour le bras !
				if(COLOR_Y(global.env.pos.y) > y_to_prevent_pierre_to_get_out)
				{
					we_prevented_pierre_to_get_out = TRUE;
					REACH_POINT_GET_OUT_INIT_send_request();
					BUZZER_play(100,DEFAULT_NOTE,1);
				}
			}
			state = check_sub_action_result(goto_adversary_zone(),GOTO_ADVERSARY_ZONE,DO_OUR_TORCH,ERROR);
			if(state != GOTO_ADVERSARY_ZONE)
				ASSER_set_acceleration(64);

			//ERROR n'est pas censé se produire... la sub_action étant censée trouver une solution pour se rendre en zone adverse !
			break;
		case DO_OUR_TORCH:
			if(we_have_a_torch)
			{
				switch(do_torch(OUR_TORCH,TRUE,dispose_zone_for_our_torch,NO_DISPOSE))
				{
					case IN_PROGRESS:
						break;
					case END_OK:		//no break
					case NOT_HANDLED:	//no break
					default:
						state = DO_ADV_TORCH;
				}
			}
			else
				state = DO_ADV_TORCH;
			break;
		case DO_ADV_TORCH:
			if(dispose_zone_for_adversary_torch == NO_DISPOSE)
			{
				SD_printf("Récupération de la torche adverse : désactivée\n");
				state = DONE;	//On ne fait pas la torche -> on rend la main à la high_level_strat
			}
			else
			{
				switch(do_torch(ADVERSARY_TORCH, FALSE, dispose_zone_for_adversary_torch, if_fail_dispose_zone_for_adversary_torch))
				{
					case IN_PROGRESS:
						break;
					case END_OK:
						state = (fall_fire_wall_adv == TRUE)? FALL_FIRE_WALL_ADV : DONE;
						break;
					case NOT_HANDLED:	//no break
					default:
						state = (fall_fire_wall_adv == TRUE)? FALL_FIRE_WALL_ADV: ERROR;
				}
			}

			break;
		case FALL_FIRE_WALL_ADV:
			state = DONE;
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


void strat_homologation_guy(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_HOMOLOGATION,
		INIT,
		FALL_FIRST_FIRE,
		GET_OUT_POS_START,
		GOTO_ADVERSARY_ZONE,
		DO_OUR_TORCH,
		MOVE_ADV_TORCH,
		DO_ADV_TORCH,
		TRIANGLE_1,
		BACK,
		MOVE,
		TRIANGLE_3,
		PROTECTED_FIRE,
		DONE,
		ERROR
	);

	static bool_e we_prevented_pierre_to_get_out = FALSE;

	switch(state)
	{
		case INIT:
			we_have_a_torch = FALSE;
			we_prevented_pierre_to_get_out = FALSE;

			initial_path = TORCH_ROAD;	//En fait, on prend la TORCH_ROAD !

			state = GET_OUT_POS_START;
			break;

		case GET_OUT_POS_START:
			if(entrance)
			{
				ASSER_set_acceleration(80);	//Acceleration de guy au démarrage...
			}

			state  = try_going_until_break(700,COLOR_Y(300),GET_OUT_POS_START,DO_OUR_TORCH, DO_OUR_TORCH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);

			break;
		case DO_OUR_TORCH:
			switch(do_torch(OUR_TORCH,FALSE,HEARTH_OUR,NO_DISPOSE))
			{
				case IN_PROGRESS:
					break;
				case END_OK:		//no break
				case NOT_HANDLED:	//no break
				default:
					//state = (global.env.color == RED)?DO_ADV_TORCH:MOVE_ADV_TORCH;
					state = TRIANGLE_1;
			}
			break;

		case TRIANGLE_3:
			state = check_sub_action_result(ACT_take_triangle_on_edge(V_TRIANGLE_3),TRIANGLE_3,BACK,BACK);
			break;

		 case BACK:
			state = try_advance(200,BACK,DO_ADV_TORCH,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);

		case MOVE_ADV_TORCH:
			state  = try_going_until_break(1600,1500,MOVE_ADV_TORCH,DO_ADV_TORCH, ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case DO_ADV_TORCH:
			switch(do_torch(ADVERSARY_TORCH, FALSE, HEARTH_ADVERSARY, NO_DISPOSE))
			{
				case IN_PROGRESS:
					break;
				case END_OK:
				case NOT_HANDLED:	//no break
				default:
					state = DONE;
			}

			break;

		case PROTECTED_FIRE:
			state = check_sub_action_result(protected_fires(ADVERSARY_FIRES),PROTECTED_FIRE,PROTECTED_FIRE,PROTECTED_FIRE);
			break;

		case DONE:
			break;

		default:
			break;
	}
}


// Gestion du bras pendant le déplacement vers la zone adverse
void goto_adversary_zone_arm_management(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GOTO_ADVERSARY_ZONE_ARM_MGT,
		INIT,
		WAIT_FOR_PIERRE,
		WAIT_FOR_TREE_FIRE,
		EXIT_ARM_FOR_TREE_FIRE,
		ARM_AT_HOME,
		DONE
		);

	switch(state)
	{
		case INIT:
			switch(initial_path)
			{
				case SOUTH_TREES:
					state = WAIT_FOR_TREE_FIRE;
					break;
				case SOUTH_HEART:
					state = WAIT_FOR_TREE_FIRE;
					break;
				case TORCH_ROAD:
					state = DONE;
					break;
				case NORTH_HEART:
					state = DONE;
					break;
				case NORTH_MAMMOUTH:
					state = DONE;
					break;
			}
			break;
			case WAIT_FOR_TREE_FIRE:
				if(entrance)
					ASSER_WARNER_arm_y(COLOR_Y(600));
				if(global.env.asser.reach_y)
					state = EXIT_ARM_FOR_TREE_FIRE;
				break;
			case EXIT_ARM_FOR_TREE_FIRE:
				if(entrance)
				{
					ASSER_WARNER_arm_y(COLOR_Y(1000));
					//TODO demander la sortie du bras pour tacler le feu
				}
				if(global.env.asser.reach_y)
					state = ARM_AT_HOME;
				break;
			case ARM_AT_HOME:
				if(entrance)
				{
					//TODO demander le rangement du bras

				}
				break;
				//TODO faire de même pour le feu adverse coté arbres... la position du bras dépend du chemin emprunté
			case DONE:
				//ETAT PUIS, plus rien à faire !
				break;
			default:
				break;
	}

}
/*
 * Sub action qui permet à guy de se rendre dans la zone adverse ou au point de dépose avec notre torche
 * Cette sub action gère les cas d'erreur et ne rendra la main que lorsque Guy est dans la zone adverse !
 *
 * Actions activables (par macros) et intégrées à cette sub-action :
 * 	- une grande partie des feux mobiles debout		TODO !!
 * 	- les feux fixes du sud				TODO !!
 * Attention, ces feux ne seront traités que lorsque le chemin choisi le permet et que les macros correspondantes sont actives !
 */
error_e goto_adversary_zone(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GOTO_ADVERSARY_ZONE,
		INIT,
		//WAIT_FOR_PIERRE,
		ST2,
		SB2,
		SC0,
		SC1,
		SC12,	//Point intermédiaire en cas d'échec
		SC2,
		SC3,
		SW0,
		SW1,
		SW2,
		SW3,
		DISPOSE_POINT_TORCH,
		FAIL_DISPOSE_POINT,
		DONE
	);
	static enum state_e fail_state = INIT;
	static enum state_e success_state = INIT;

	error_e ret;
	ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			switch(initial_path)
			{
				case SOUTH_TREES:
					SD_printf("CHEMIN : Sud, arbres (SOUTH Ext)\n");
					state = SB2;
					break;
				case SOUTH_HEART:
					SD_printf("CHEMIN : Sud, proche foyer (SOUTH Int)\n");
					state = SB2;
					break;
				case TORCH_ROAD:
					SD_printf("CHEMIN : Torche\n");
					state = ST2;
					break;
				case NORTH_HEART:
					SD_printf("CHEMIN : Nord, proche foyer (NORTH Int)\n");
					state = SC1;
					break;
				case NORTH_MAMMOUTH:
					SD_printf("CHEMIN : Nord, proche mammouths (NORTH Ext)\n");
					state = SC0;	//WAIT_FOR_PIERRE;
					break;
			}
			break;
		/*case WAIT_FOR_PIERRE:
			{  // Attend le passage de pierre pour pouvoir passer à son tour
				static time32_t last_time;
				if(entrance)
					last_time = global.env.match_time;

				if(pierre_reach_point_C1 || global.env.match_time > last_time + 5000)
					state = SC0;
			}
			break;
		*/
		case ST2:	//On veut récupérer la torche
			//Si on échoue, on abandonne la torche, on file en SC1...
			state = try_going_until_break(1000,COLOR_Y(750),ST2,DISPOSE_POINT_TORCH,SC1,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SB2:
			if(entrance)
			{
				if(initial_path == SOUTH_HEART)
					success_state = SC2;
				else
					success_state = SC3;
			}
			//TODO sur cette trajectoire, bouger le bras ou adapter la trajectoire pour gérer le feu mobile central !
			state = try_going_until_break(1350,COLOR_Y(800),SB2,SC2,SC12,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SC0:
			//NO WAIT : on file direct vers SC1 pour pas géner Pierre...
			state = try_going_until_break(500,COLOR_Y(1200),SC0,SW0,SC1,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state == SC1)	//En cas d'échec, il faut absolument prévenir Pierre qu'on est arrêté !
			{
				BUZZER_play(200,DEFAULT_NOTE,1);
				send_message_to_pierre(XBEE_GUY_IS_BLOQUED_IN_NORTH,0);
			}
			break;
		case SC1:
			if(entrance)
			{
				if(last_state == SW1)	//On revient en échec du prochain point -> changement de chemin
				{
					success_state = SC12;
					fail_state = SW1;
				}
				else
				{
					success_state = SW1;
					fail_state = SC12;
				}
			}
			state = try_going_until_break(750,COLOR_Y(1200),SC1,success_state,fail_state,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			if(state == fail_state && last_state == INIT)
			{
				BUZZER_play(200,DEFAULT_NOTE,1);
				send_message_to_pierre(XBEE_GUY_IS_BLOQUED_IN_NORTH,0);
			}
			break;
		case SC12://Point intermédiaire en cas d'échec
			if(entrance)
			{
				if(last_state == SC1)
				{
					success_state = SC2;
					fail_state = SC1;
				}
				else 	//SC2 ou SB2 !!
				{
					success_state = SC1;
					fail_state = SC2;
				}
			}
			state = try_going_until_break(1050,COLOR_Y(1200),SC12,success_state,fail_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SC2:
			if(entrance)
			{
				if(last_state == SW2)	//On revient en échec du prochain point -> changement de chemin
				{
					success_state = SC12;
					fail_state = SW2;
				}
				else
				{
					success_state = SW2;
					fail_state = SC12;
				}
			}
			state = try_going_until_break(1400,COLOR_Y(1200),SC2,success_state,fail_state,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;
		case SC3:
			state = try_going_until_break(1750,COLOR_Y(1200),SC3,SW3,SC2,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;
		case SW0:
			state = try_going_until_break(500,COLOR_Y(1800),SW0,DONE,SC1,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SW1:
			state = try_going_until_break(750,COLOR_Y(1800),SW1,DONE,SC1,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SW2:
			state = try_going_until_break(1400,COLOR_Y(1800),SW2,DONE,SC2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SW3:
			state = try_going_until_break(1750,COLOR_Y(1800),SW3,DONE,SC2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
			//TODO poursuivre le déplacement vers d'autres points clés pour chaque chemin... afin de mettre les feux mobiles (selon deines)
		case DISPOSE_POINT_TORCH:
			state = try_going(DISPOSE_POINT_X,DISPOSE_POINT_Y,DISPOSE_POINT_TORCH,DONE,FAIL_DISPOSE_POINT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state == DONE)
				we_have_a_torch = TRUE;
			break;
		case FAIL_DISPOSE_POINT:
			state = INIT;
			ret = END_OK;
			break;
		case DONE:
			state = INIT;
			ret = END_OK;
			break;
	}
	return ret;
}

/*
 * PRECONDITIONS ! ATTENTION c'est IMPORTANT
 * La fonction do_torch calcule un moyen de pousser la torche vers le foyer de destination... MAIS n'INTEGRE PAS DE PATHFIND !
 * MAIS, elle ne contourne par la torche pour se rendre au premier point de la poussée...
 * Il FAUT donc appeler cette fonction au bon point, ou éventuellement un peu derrière ce point...
 *
 * Attention, la position if_fail_dispose_zone doit être choisie intelligemment, il n'y a pas de protection anti-recouvrement d'élément fixe !
 *
 * Faire une torche signifie
 * SI on a pas déjà une torche :
 * {
 * 		- se rend à coté de la torche SI NECESSAIRE (si on y est pas déjà.. mais c'est mieux si on y est !)
 * 		- Fonction de récupération : pousse la torche dans la bonne direction (en fonction du foyer de dépose)
 * 		- Se rend au foyer de dépose
 * 		- en cas d'échec... tente une seconde position de dépose (passée en paramètre)
 * 		- en cas d'échec... abandonne le dépilement de la torche
 * 	}
 * - dépile la torche
 *  - en cas d'échec, abandonne
 * - s'extrait de la zone pour pouvoir rendre la main
 * - enregistre le fait de la dépose (et la zone déposée)
 */

error_e do_torch(torch_choice_e torch_choice, bool_e we_are_already_in_pos_end, torch_dispose_zone_e dispose_zone, torch_dispose_zone_e if_fail_dispose_zone)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DO_TORCH,
		IDLE,
		COMPUTE,
		POS_START_TORCH,
		WAY_ADVERSARY,
		MOVE_TORCH,
		WAIT,
		BACK,
		RED_CENTRAL,
		CENTER_TO_HEARTH,
		MOVE_TO_HEARTH,
		DEPLOY_TORCH,
		EXTRACT_HEARTH,
		EXTRACT_FROM_HEART,
		EXTRACT_FROM_HEART_WITH_ERROR,
		WAIT_FOR_EXIT,
		RETURN_NOT_HANDLED,
		ERROR,
		DONE
	);
	static torch_dispose_zone_e current_dispose_zone = NO_DISPOSE;
	static bool_e fail_at_first_dispose_try = FALSE;	//
	static bool_e i_have_the_torch = FALSE;

	// PosEndTorch : Position finale de la torche
	// PosStart : Position de départ du robot
	// PosEnd : Position finale du robot quand il déplace la torche
	// PosCenter : Postion pour correctement s'alligner en face d'un foyer
	// PosHearth : Position au prés du foyer
	static GEOMETRY_point_t posStart,posEndTorch,get_out,posEnd,posCenter,posHearth;
	static bool_e get_out_enable = FALSE;


	static GEOMETRY_point_t torch;
	static displacement_t dpl_adv[3]; // Si nous souhaitons poser sur notre torche sur le foyer adverse
	Uint16 norm;

	switch(state){
		case IDLE :
			fail_at_first_dispose_try = FALSE;
			get_out_enable = FALSE;
			current_dispose_zone = dispose_zone;
			state = COMPUTE;
			break;
		case COMPUTE:
			torch = TORCH_get_position(torch_choice);

			switch(current_dispose_zone)
			{
				case FILED_FRESCO:
					posEndTorch.x = 300;
					posEndTorch.y = 1500;
					break;

				case HEARTH_OUR:
				case HEARTH_ADVERSARY: // Foyer ROUGE
					if((global.env.color == RED && current_dispose_zone == HEARTH_OUR)||(global.env.color != RED && current_dispose_zone == HEARTH_ADVERSARY)){
						posEndTorch.x = 2000 - RADIUS_TORCH;
						posEndTorch.y = 250 + RADIUS_TORCH;

						posCenter.x = 1680;
						posCenter.y = 300;

						posHearth.x = 1760;
						posHearth.y = 220;

					}else{ // Foyer Jaune
						posEndTorch.x = 2000 - 250 - RADIUS_TORCH;
						posEndTorch.y = 3000 - RADIUS_TORCH;

						posCenter.x = 1720;
						posCenter.y = 2660;

						posHearth.x = 1815;
						posHearth.y = 2760;
					}

					get_out.x = 1500;
					get_out.y = (current_dispose_zone == HEARTH_OUR)?(COLOR_Y(600)):(COLOR_Y(2400));
					get_out_enable = TRUE;
					break;

				case ANYWHERE:
						//No break;
				case HEARTH_CENTRAL:
						//No break;
				default:
					posEndTorch.x = 1050 + 150 + RADIUS_TORCH + 20;
					posEndTorch.y = ((global.env.color == RED && torch_choice == OUR_TORCH) || (global.env.color != RED && torch_choice != OUR_TORCH))?1400:1500; // La pousse trop loin sinon pas de bordure pour arrêter la torche

					posCenter.x = 1440;
					posCenter.y = 1770;

					posHearth.x = 1220;
					posHearth.y = 1620;

					get_out.x = 1500;
					get_out.y = COLOR_Y(1600);
					get_out_enable = TRUE;
					break;
			}

			float coefx, coefy;
			norm = GEOMETRY_distance(torch,posEndTorch);
			coefx = (torch.x - posEndTorch.x)/(norm*1.);
			coefy = (torch.y - posEndTorch.y)/(norm*1.);

			posStart.x = torch.x + DIM_START_TRAVEL_TORCH*coefx;
			posStart.y = torch.y + DIM_START_TRAVEL_TORCH*coefy;

			posEnd.x = posEndTorch.x + RADIUS_TORCH*coefx;
			posEnd.y = posEndTorch.y + RADIUS_TORCH*coefy;


			if(we_are_already_in_pos_end || GEOMETRY_distance((GEOMETRY_point_t){torch.x, torch.y}, (GEOMETRY_point_t){posEndTorch.x, posEndTorch.y}) < 20)	//Le cas particulier où on a déjà la torche en position correcte en arrivant dans cette fonction ! AUCUNE VERIFICATION, le développeur est responsable !
				state = CENTER_TO_HEARTH;
			else if(GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}, posStart) < 50)
				state = (dispose_zone == HEARTH_ADVERSARY && torch_choice == OUR_TORCH)? WAY_ADVERSARY:MOVE_TORCH;
			else
				state = POS_START_TORCH;

			SD_printf("Torch: start=[%d;%d] | torch=[%d;%d] | end Torch=[%d;%d] | remotness robot=[%d;%d]\n",posStart.x, posStart.y, torch.x, torch.y, posEndTorch.x, posEndTorch.y, posEnd.x, posEnd.y);
			break;

		case POS_START_TORCH:
			state = try_going(posStart.x, posStart.y, POS_START_TORCH, (dispose_zone == HEARTH_ADVERSARY && torch_choice == OUR_TORCH)? WAY_ADVERSARY:MOVE_TORCH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case MOVE_TORCH :
			if(entrance)
				i_have_the_torch = TRUE;

			state = try_going(posEnd.x, posEnd.y, MOVE_TORCH, WAIT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);

			if(state == ERROR && ((global.env.pos.x > 1700 && ((global.env.color == RED && current_dispose_zone == HEARTH_OUR) || (global.env.color != RED && current_dispose_zone == HEARTH_ADVERSARY))) ||
			   (global.env.pos.y > 2700 && ((global.env.color != RED && current_dispose_zone == HEARTH_OUR) || (global.env.color == RED && current_dispose_zone == HEARTH_ADVERSARY)))))
				state = WAIT;

			break;


		// Si nous voulons déposer notre torche sur le foyer adverse
		case WAY_ADVERSARY:
			if(entrance){
				i_have_the_torch = TRUE;

				dpl_adv[0].point.x = 1520;
				dpl_adv[0].point.y = COLOR_Y(1380);
				dpl_adv[0].speed = SLOW;

				dpl_adv[1].point.x = 1460;
				dpl_adv[1].point.y = COLOR_Y(2220);
				dpl_adv[1].speed = SLOW;

				dpl_adv[2].point.x = posEnd.x;
				dpl_adv[2].point.y = posEnd.y;
				dpl_adv[2].speed = SLOW;
			}

			state = try_going_multipoint(dpl_adv,3, WAY_ADVERSARY, WAIT, ERROR, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

		case WAIT:
			state = ELEMENT_wait_time(1000,WAIT,BACK);
			break;

		// S'éloigne de la torche
		case BACK:
			if(entrance){
				i_have_the_torch = FALSE;
				TORCH_new_position(torch_choice);
			}

			state = try_advance(200, BACK, CENTER_TO_HEARTH, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);

			if(ON_LEAVING(BACK)){ // Petit décalage afin de ne pas toucher la torche car guy est gaucher
				if(current_dispose_zone == HEARTH_CENTRAL && ((global.env.color == RED && torch_choice == OUR_TORCH) || (global.env.color != RED && torch_choice != OUR_TORCH)))
					state = RED_CENTRAL;
			}
			break;

		case RED_CENTRAL:
			state = try_going(1650, 1450, RED_CENTRAL, CENTER_TO_HEARTH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case CENTER_TO_HEARTH:
			state = try_going(posCenter.x, posCenter.y, CENTER_TO_HEARTH, MOVE_TO_HEARTH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);

			break;

		case MOVE_TO_HEARTH:
			state = try_going(posHearth.x, posHearth.y, MOVE_TO_HEARTH, DEPLOY_TORCH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);

			if(state == ERROR && (((global.env.pos.x > 1650 && ((global.env.color == RED && current_dispose_zone == HEARTH_OUR) || (global.env.color != RED && current_dispose_zone == HEARTH_ADVERSARY))) || // Foyer rouge
			   (global.env.pos.y > 2650 && ((global.env.color != RED && current_dispose_zone == HEARTH_OUR) || (global.env.color == RED && current_dispose_zone == HEARTH_ADVERSARY)))) ||					 // Foyer jaune
				(GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}, (GEOMETRY_point_t){1050, 1500}) < 150+120 && current_dispose_zone == HEARTH_CENTRAL)))								 // Foyer centrale
				state = DEPLOY_TORCH;

			break;

		case ERROR:
			if(fail_at_first_dispose_try || if_fail_dispose_zone == NO_DISPOSE || i_have_the_torch == FALSE)
			{
				if(i_have_the_torch)
					TORCH_new_position(torch_choice);
				//TODO : libérer la torche ! (il faudrait d'ailleurs s'en extraire...)

				// Sinon nous sommes sur le pathfind, pas de get_out
				if(est_dans_carre(500,1700,400,1300,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}) ||
				   est_dans_carre(500,1700,1700,2600,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}) ||
				   est_dans_carre(500, 700,400,2600,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}) ||
				   est_dans_carre(1500,1700,400,2600,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}))
					state = RETURN_NOT_HANDLED;
				else
					state = EXTRACT_FROM_HEART_WITH_ERROR;

			}
			else
			{
				fail_at_first_dispose_try = TRUE;
				current_dispose_zone = if_fail_dispose_zone;
				state = COMPUTE;
			}
			break;

		case DEPLOY_TORCH:	//On déploie la torche sur le foyer
			state = check_sub_action_result(ACT_arm_deploy_torche_guy(torch_choice,current_dispose_zone),DEPLOY_TORCH,EXTRACT_HEARTH,EXTRACT_HEARTH);
			break;

		case EXTRACT_HEARTH:
			state = try_advance(200, EXTRACT_HEARTH, EXTRACT_FROM_HEART, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case EXTRACT_FROM_HEART:
			if(get_out_enable)
				state = try_going(get_out.x, get_out.y, state, DONE, WAIT_FOR_EXIT, FAST, ANY_WAY, DODGE_AND_WAIT);
			else
			{
				state = DONE;
				SD_printf("get_out disabled");
			}
			break;
		case EXTRACT_FROM_HEART_WITH_ERROR:
			// Sinon nous sommes sur le pathfind, pas de get_out
			if(est_dans_carre(500,1700,400,1300,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}) ||
			   est_dans_carre(500,1700,1700,2600,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}) ||
			   est_dans_carre(500, 700,400,2600,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}) ||
			   est_dans_carre(1500,1700,400,2600,(GEOMETRY_point_t){global.env.pos.x,global.env.pos.y}))
				state = RETURN_NOT_HANDLED;
			else if(get_out_enable)
				state = try_going(get_out.x, get_out.y, state, RETURN_NOT_HANDLED, WAIT_FOR_EXIT, FAST, ANY_WAY, DODGE_AND_WAIT);
			else
			{
				state = RETURN_NOT_HANDLED;
				SD_printf("get_out disabled");
			}
			break;
		case WAIT_FOR_EXIT:{
			static time32_t local_time;
			if(entrance)
			{
				local_time = global.env.match_time;
			}
			if(global.env.match_time > local_time + 1000)
				state = EXTRACT_FROM_HEART_WITH_ERROR;
			}break;
		case RETURN_NOT_HANDLED:
			state = IDLE;
			return NOT_HANDLED;
			break;
		case DONE:
			state = IDLE;
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}

/*
 * Scanne une zone donné et retourne
 * les triangles qui sont dans cette zone
 */
error_e scan_and_back(pos_scan_e scan){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_SCAN_AND_BACK,
		IDLE,
		GET_IN,
		POS_START,
		SCAN_TRIANGLE,
		ANALYZE,
		POS_TAKE,
		TAKE,
		RETURN_TRIANGLE,
		MOVE_DROP_TRIANGLE,
		DROP_TRIANGLE,
		NEXT_TRIANGLE,
		GET_OUT,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static objet_t objet;
	static GEOMETRY_point_t point;
	static Uint8 i, level;
	static pathfind_node_id_t pathfind;
	static objet_t tabObjet[3][20];
	static Uint8 nb_obj[3];


	switch(state){
		case IDLE:{
			i = 0;
			level = 0;

			if(est_dans_carre(pos_scan[scan].xMin, pos_scan[scan].xMax, COLOR_Y(pos_scan[scan].yMin), COLOR_Y(pos_scan[scan].yMax), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = POS_START;
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
		}break;

		case GET_IN:
			if(entrance)
				pathfind = PATHFIND_closestNode(pos_scan[scan].scan.x, COLOR_Y(pos_scan[scan].scan.y), 0);

			state = PATHFIND_try_going(pathfind,GET_IN, POS_START, ERROR, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_START:
			state = try_going(pos_scan[scan].scan.x, COLOR_Y(pos_scan[scan].scan.y), POS_START, SCAN_TRIANGLE, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT);
			break;

		case SCAN_TRIANGLE:
			state = ELEMENT_try_going_and_rotate_scan(COLOR_ANGLE(pos_scan[scan].tetaMin), COLOR_ANGLE(pos_scan[scan].tetaMax), 90,
													  pos_scan[scan].scan.x, COLOR_Y(pos_scan[scan].scan.y), SCAN_TRIANGLE, ANALYZE, ERROR, FAST, FORWARD, DODGE_AND_WAIT);
			break;

		case ANALYZE:{
			ELEMENT_get_nb_object(nb_obj);
			ELEMENT_get_object(tabObjet);

			if(nb_obj[0] > 0){
				level = 0;
				objet = tabObjet[level][i];
				state = POS_TAKE;
			}else if(nb_obj[1] > 0){
				level = 1;
				objet = tabObjet[level][i];
				state = POS_TAKE;
			}else if(nb_obj[2] > 0){
				level = 2;
				objet = tabObjet[level][i];
				state = POS_TAKE;
			}else{ // Il y a rien, là on nous avons scanné
				state = GET_OUT;
			}
		}break;

		case POS_TAKE:{
			if(entrance){
				GEOMETRY_point_t dir;
				Uint16 size;

				// Pour éviter de frapper dans le foyer
				if(scan == SCAN_CENTRAL_HEARTH && level != 0){
					dir.x = 1050;
					dir.y = 1500;
					size = CENTRAL_HEARTH_RADIUS + DISTANCE_BETWEEN_GUY_AND_HEARTH; // Rayon du milieu
				}else if(scan == SCAN_ADV_HEARTH && level != 0){
					dir.x = 2000;
					dir.y = COLOR_Y(3000);
					size = 270; // Rayon d'un foyer des coins
				}else{
					dir.x = objet.x;
					dir.y = objet.y;
					size = DIM_TRIANGLE;
				}

				Uint16 norm = GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){dir.x,dir.y});

				float coefx = (global.env.pos.x - dir.x)/(norm*1.);
				float coefy = (global.env.pos.y - dir.y)/(norm*1.);


				point.x = dir.x + size*coefx;
				point.y = dir.y + size*coefy;
			}
			//On considère l'erreur comme une réussite (un peu comme un RUSH...)
			state = try_going(point.x,point.y, POS_TAKE, TAKE, TAKE, SLOW, FORWARD, NO_DODGE_AND_WAIT);

			}break;

		case TAKE:
			//Prend le triangle a la position objet

			state = RETURN_TRIANGLE;
			break;

		case RETURN_TRIANGLE:
			// Retourne le triangle

			// Sinon, nous somme en train d'analyser un foyer et qu'un triangle se trouve devant nous le prenons et l'emmenons
			// afin de pouvoir faciliter l'accès au foyer
			if(level == 0 && (scan == SCAN_CENTRAL_HEARTH || scan == SCAN_ADV_HEARTH))
				state = MOVE_DROP_TRIANGLE;
			else
				state = DROP_TRIANGLE;
			break;

		case MOVE_DROP_TRIANGLE:
			state = try_going(pos_scan[scan].scan.x,COLOR_Y(pos_scan[scan].scan.y), MOVE_DROP_TRIANGLE, DROP_TRIANGLE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);

			break;

		case DROP_TRIANGLE:
			// Depose triangle, à l'endroit où nous l'avons trouvé

			state = NEXT_TRIANGLE;
			break;

		case NEXT_TRIANGLE:
			i++;
			state = POS_TAKE;

			if(nb_obj[level] > i){
				objet = tabObjet[level][i];
			}else if(level < 2){
				level++;
				i = 0;

				if(nb_obj[level] > i){
					objet = tabObjet[level][i];
				}else if(level < 2){
					level++;

					if(nb_obj[level] > i){
						objet = tabObjet[level][i];
					}else
						state = GET_OUT;
				}else
					state = GET_OUT;
			}else
				state = GET_OUT;

			break;

		case GET_OUT:
			state = try_going(pos_scan[scan].get_out.x,COLOR_Y(pos_scan[scan].get_out.y), GET_OUT, DONE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT);
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = RETURN_NOT_HANDLED;
			break;

		case RETURN_NOT_HANDLED :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}


/*
 * Regarde si le triangle adverse
 * est ici, et le retourne de notre couleur si c'est le cas
 */
error_e do_triangle_start_adversary(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DO_TRIANGLE_START_ADVERSARY,
		IDLE,
		GET_IN,
		POS_START,
		SCAN_TRIANGLE,
		ANALYZE,
		POS_TAKE,
		TAKE,
		MOVE_DROP_TRIANGLE,
		DROP_TRIANGLE,
		DROP_ANYWHERE,
		GET_OUT_CAVERN,
		GET_OUT_TREE,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static objet_t objet;
	static GEOMETRY_point_t point;
	static bool_e i_have_triangle = FALSE;

	switch(state){
		case IDLE:

			if((est_dans_carre(400, 1200, 2000, 2600, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) && global.env.color == RED) ||
				(est_dans_carre(400, 1200, 400, 1000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) && global.env.color != RED))
				state = POS_START;
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			state = PATHFIND_try_going((global.env.color == RED)?Z1:A1,GET_IN, POS_START, ERROR, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_START:
			state = try_going(900,(global.env.color == RED)? 2600:400, POS_START, SCAN_TRIANGLE, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case SCAN_TRIANGLE:
			state = ELEMENT_try_going_and_rotate_scan((global.env.color == RED)?3*PI4096/4:-PI4096/4, (global.env.color == RED)?PI4096/4:-3*PI4096/4, 90,
							 900, (global.env.color == RED)? 2600:400, SCAN_TRIANGLE, ANALYZE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case ANALYZE:{
			objet_t tabObjet[3][20];
			ELEMENT_get_object(tabObjet);

			if(tabObjet[0][0].x != 0 && tabObjet[0][0].y != 0){
				objet = tabObjet[0][0];
				state = POS_TAKE;
			}else{
				state = ERROR;
				set_sub_act_done(SUB_ACTION_TRIANGLE_START_ADVERSARY, TRUE);
			}

		}break;

		case POS_TAKE:{
			if(entrance){
				Uint16 norm = GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x,global.env.pos.y},(GEOMETRY_point_t){objet.x,objet.y});

				float coefx = (global.env.pos.x - objet.x)/(norm*1.);
				float coefy = (global.env.pos.y - objet.y)/(norm*1.);


				point.x = objet.x + DIM_TRIANGLE*coefx;
				point.y = objet.y + DIM_TRIANGLE*coefy;
			}

			state = try_going(point.x,point.y, POS_TAKE, TAKE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);

			}break;

		case TAKE:
			//Prend le triangle a la position objet

			state = MOVE_DROP_TRIANGLE;
			i_have_triangle = TRUE;
			break;

		case MOVE_DROP_TRIANGLE:
#ifdef DROP_TRIANGLE_UNDER_TREE
			state = check_sub_action_result(ELEMENT_go_and_drop((global.env.color == RED)?YELLOW_TREE_MAMOU:RED_TREE_MAMOU),MOVE_DROP_TRIANGLE,DONE, ERROR);
#else
			state = check_sub_action_result(ELEMENT_go_and_drop((global.env.color == RED)?YELLOW_CAVERN:RED_CAVERN),MOVE_DROP_TRIANGLE,DONE, ERROR);
#endif
			break;

		case DROP_ANYWHERE:
			state = GET_OUT_CAVERN;
			i_have_triangle = FALSE;
			set_sub_act_done(SUB_ACTION_TRIANGLE_START_ADVERSARY, TRUE);
			break;

		case GET_OUT_CAVERN:
			state = try_going(750,(global.env.color == RED)?2600:400,GET_OUT_CAVERN,RETURN_NOT_HANDLED,GET_OUT_TREE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_TREE:
			state = try_going(1350,(global.env.color == RED)?2600:400,GET_OUT_TREE,RETURN_NOT_HANDLED,GET_OUT_CAVERN,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			if((est_dans_carre(400, 1600, 1900, 2600, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) && global.env.color == RED) ||
				(est_dans_carre(400, 1600, 400, 1100, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}) && global.env.color != RED)  ) // Est sur le pathfind
				state = RETURN_NOT_HANDLED;
			else if(global.env.pos.x > 1050)
				state = GET_OUT_TREE;
			else
				state = GET_OUT_CAVERN;

			break;

		case RETURN_NOT_HANDLED :
			if(i_have_triangle)
				state = DROP_ANYWHERE;
			else{
				state = IDLE;
				return NOT_HANDLED;
			}
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}


/*
 * Regarde les deux triangles entre les arbres
 * Fait tomber l'un des triangles et prends
 * le deuxième pour l'emmené sur un foyer ou sous un arbre
 */
error_e do_triangles_between_trees(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DO_TRIANGLES_BETWEEN_TREES,
		IDLE,
		GET_IN,
		CHOICE_DETECTION,
		POS_START_DETECTION,
		DETECTION_TRIANGLE_1,
		DETECTION_TRIANGLE_2,
		CHOICE,
		POS_START,
		ORIENTATION,
		GET_OUT_ARM,
		TURN,
		MOVE,
		TAKE_TRIANGLE,
		MOVE_DROP_TRIANGLE,
		DROP_TRIANGLE,
		MOVE_FORCED_DROP_HEARTH,
		FORCED_DROP_HEARTH,
		DROP_TRIANGLE_ANYWHERE,
		GET_OUT_SIDE_RED,
		GET_OUT_SIDE_MIDDLE,
		GET_OUT_SIDE_YELLOW,
		GET_OUT_M2,
		GET_OUT_W2,
		GET_OUT_C2,
		GET_OUT_Y3,
		GET_OUT_B3,
		GET_OUT_C3,
		GET_OUT_W3,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static bool_e i_have_triangle = FALSE;
	static bool_e try_drop_triangle_center = FALSE;
	static GEOMETRY_point_t dpl_dect[2]; // Les deplacement pour la detection

	static bool_e presence_triangle_2 = TRUE;
	static bool_e presence_triangle_3 = TRUE;

	switch(state){
		case IDLE:

			if(est_dans_carre(1500, 2000, 1000, 2000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = CHOICE_DETECTION;
			else
				state = GET_IN;						//On se rend à la première position par le pathfind
			break;

		case GET_IN:
			state = PATHFIND_try_going(M3,GET_IN, CHOICE_DETECTION, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case CHOICE_DETECTION:
#ifdef DETECTION_TRIANGLE_MIDDLE
				if(global.env.pos.y > 1500){
					dpl_dect[0] = (GEOMETRY_point_t){1800,1900};
					dpl_dect[1] = (GEOMETRY_point_t){1800,1100};
				}else{
					dpl_dect[1] = (GEOMETRY_point_t){1800,1900};
					dpl_dect[0] = (GEOMETRY_point_t){1800,1100};
				}

				state = POS_START_DETECTION;
#else
				state = POS_START;				//On se rend à la première position directement
#endif
			break;

		case POS_START_DETECTION:
			state = try_going(dpl_dect[0].x, dpl_dect[0].y, POS_START_DETECTION, DETECTION_TRIANGLE_1, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case DETECTION_TRIANGLE_1:
			if(entrance)
				ELEMENT_launch_triangle_warner((global.env.pos.y > 1500)? 3:2);

			state = try_going(1600, 1500, DETECTION_TRIANGLE_1, DETECTION_TRIANGLE_2, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case DETECTION_TRIANGLE_2:
			if(entrance){
				//Desactive pour test, faute de pas avoir de capteur
//				if(!ELEMENT_triangle_present()){ // Par default, les triangles sont présent
//					if(dpl_dect[0].y > dpl_dect[1].y) // On regarde le triangle 3 en premier
//						presence_triangle_3 = FALSE;
//					else
//						presence_triangle_2 = FALSE;
//				}

				ELEMENT_launch_triangle_warner((dpl_dect[0].y > dpl_dect[1].y)? 2:3); // Ne peut pas faire confiance à la comparaison global.env.pos.y trop proche de 1500
			}

			state = try_going(dpl_dect[1].x, dpl_dect[1].y, DETECTION_TRIANGLE_2, CHOICE, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);

			break;

		case CHOICE:
//			if(!ELEMENT_triangle_present()){ // Par default, les triangles sont présent
//				if(dpl_dect[0].y > dpl_dect[1].y) // On regarde le triangle 3 en premier
//					presence_triangle_2 = FALSE;
//				else
//					presence_triangle_3 = FALSE;
//			}

			if((global.env.color == RED && presence_triangle_3) || (global.env.color != RED && presence_triangle_2)) // Regarde, si le premier triangle est present
				state = POS_START;
			else if((global.env.color == RED && presence_triangle_2) || (global.env.color != RED && presence_triangle_3)) // Si le premier n'est pas présent, on regarde le 2éme
				state = MOVE;
			else
				state = DONE;

			break;

		case POS_START:
			//en cas d'échec, on peut rendre la main où l'on se trouve.
			state = try_going(1800, 1500, POS_START, ORIENTATION, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case ORIENTATION:
			state = try_go_angle(0,ORIENTATION,GET_OUT_ARM,ERROR,FAST);
			break;

		case GET_OUT_ARM:

			// A remplir

			state = TURN;
			break;

		// Tourne pour faire tomber le premier trinagle
		case TURN:
			state = try_go_angle((global.env.color == RED)?PI4096/2:-PI4096/2,TURN,((global.env.color == RED && presence_triangle_2) || (global.env.color != RED && presence_triangle_3))? MOVE:DONE,ERROR,FAST); // On regarde si le deuxiéme triangle est present
			break;

		case MOVE: // Bouge vers le second triangle
			state = try_going(1800,(global.env.color == RED)?1300:1700,MOVE,TAKE_TRIANGLE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case TAKE_TRIANGLE:
			state = MOVE_DROP_TRIANGLE;
			i_have_triangle = TRUE;

			set_sub_act_done(SUB_ACTION_TRIANGLE_BETWEEN_TREE,TRUE);// On indique que l'action a été faite, pour ne l'a faire qu'une seule fois, si echec derrière
			break;

		case MOVE_DROP_TRIANGLE:
#ifdef DROP_TRIANGLE_UNDER_TREE
			state = check_sub_action_result(ELEMENT_go_and_drop((global.env.color == RED)?YELLOW_TREE:RED_TREE),MOVE_DROP_TRIANGLE,DONE, ERROR);
			i_have_triangle = FALSE;
#else
			state = try_going(1350,1500,MOVE_DROP_TRIANGLE,DROP_TRIANGLE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			try_drop_triangle_center = TRUE;
#endif
			break;

		case DROP_TRIANGLE:
			// A partir d'ici les get_out partent dans tous les sens
			//Pose le triangle à l'endroit voulu
			state = GET_OUT_M2;
			i_have_triangle = FALSE;
			break;

		case DROP_TRIANGLE_ANYWHERE:
			//Pose le triangle la où il se trouve

			state = RETURN_NOT_HANDLED;
			i_have_triangle = FALSE;
			break;


		// Si l'action de poser le triangle sous l'arbre a echoué, on réssaye sur le foyer
		case MOVE_FORCED_DROP_HEARTH:
			state = try_going(1350,1500,MOVE_FORCED_DROP_HEARTH,FORCED_DROP_HEARTH,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			try_drop_triangle_center = TRUE;
			break;
		case FORCED_DROP_HEARTH:
			state = GET_OUT_M2;
			i_have_triangle = FALSE;
			break;

		// Sorti du bord
		case GET_OUT_SIDE_RED: // C3
			state = try_going(1650,1100,GET_OUT_SIDE_RED,RETURN_NOT_HANDLED,GET_OUT_SIDE_MIDDLE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_SIDE_MIDDLE: // M3
			state = try_going(1700,1500,GET_OUT_SIDE_MIDDLE,RETURN_NOT_HANDLED,(global.env.color == RED)?GET_OUT_SIDE_RED:GET_OUT_SIDE_YELLOW,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_SIDE_YELLOW: // W3
			state = try_going(1650,1900,GET_OUT_SIDE_YELLOW,RETURN_NOT_HANDLED,GET_OUT_SIDE_MIDDLE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		// Sorti du milieu va chercher a rejoindre soit le point M2 ou un point situé chez l'adversaire pour ne pas déranger pierre
		case GET_OUT_M2:
			state = try_going(1450,1500,GET_OUT_M2,RETURN_NOT_HANDLED,(global.env.color == RED)?GET_OUT_W2:GET_OUT_C2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_W2:
			state = try_going(1250,1900,GET_OUT_W2,RETURN_NOT_HANDLED,GET_OUT_M2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_C2:
			state = try_going(1250,1100,GET_OUT_C2,RETURN_NOT_HANDLED,GET_OUT_M2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		// Sorti de sous l'arbre jaune
		case GET_OUT_Y3:
			state = try_going(1600,2250,GET_OUT_Y3,(last_state == DROP_TRIANGLE)? DONE:RETURN_NOT_HANDLED,GET_OUT_W3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_W3:
			state = try_going(1650,1900,GET_OUT_W3,RETURN_NOT_HANDLED,GET_OUT_Y3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		// Sorti de sous l'arbre rouge
		case GET_OUT_B3:
			state = try_going(1600,750,GET_OUT_B3,(last_state == DROP_TRIANGLE)? DONE:RETURN_NOT_HANDLED,GET_OUT_C3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_C3:
			state = try_going(1650,1100,GET_OUT_C3,RETURN_NOT_HANDLED,GET_OUT_B3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;


		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			if(est_dans_carre(1400, 1750, 600, 2400, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // Est sur le pathfind
				state = RETURN_NOT_HANDLED;
			else if(est_dans_carre(1750, 2000, 1300, 1700, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // Est sur le bord
				state = GET_OUT_SIDE_MIDDLE;
			else if(est_dans_carre(1750, 2000, 500, 1000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // sous l'arbre rouge, ne peut normalement ps se retrouvé là sécurité
				state = GET_OUT_B3;
			else if(est_dans_carre(1750, 2000, 2000, 2500, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // sous l'arbre jaune, ne peut normalement ps se retrouvé là sécurité
				state = GET_OUT_Y3;
			else
				state = GET_OUT_M2;

			break;

		case RETURN_NOT_HANDLED :
			if(try_drop_triangle_center == FALSE && i_have_triangle == TRUE){ // Si nous avons essayer de poser le triangle sous l'arbre et l'action est echoue
				try_drop_triangle_center = TRUE;
				state = MOVE_FORCED_DROP_HEARTH;
			}else if(i_have_triangle == TRUE){ // Si nous avons toujours un triangle, on le pose ou nous sommes
				state = DROP_TRIANGLE_ANYWHERE;
			}else{
				state = IDLE;
				return NOT_HANDLED;
			}


			break;

		default:
			break;
	}

	return IN_PROGRESS;
}



static void REACH_POINT_GET_OUT_INIT_send_request() {
	CAN_msg_t msg;

	msg.sid = XBEE_REACH_POINT_GET_OUT_INIT;
	msg.size = 0;

	CANMsgToXbee(&msg,FALSE);
}

error_e ACT_take_triangle_on_edge(vertical_triangle_e vertical_triangle){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		PLACEMENT_INIT,
		ROTATION,
		PLACEMENT_BRAS,
		BACK,
		RUSH_IN_THE_FLOOR,
		WAIT,
		UP_ARM,
		PLACEMENT_FOYER,
		DROP,
		PARKED_NOT_HANDLED,
		PARKED,
		ERROR,
		DONE
	);

	const GEOMETRY_point_t triangle_pos_begin[4] = {
		{800,	180}, // Triangle 1
		{1820,	1300},
		{1820,	1700},
		{800,	2820}
	};

	const GEOMETRY_point_t triangle_pos_end[4] = {
		{805,	355},
		{1740,	1300},
		{1685,	1700},
		{800,	2720}
	};

	switch (state) {
		case IDLE:
			state = PLACEMENT_INIT;
			break;

		case PLACEMENT_INIT:
			state = try_going(triangle_pos_begin[vertical_triangle].x, triangle_pos_begin[vertical_triangle].y, PLACEMENT_INIT, ROTATION, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT);
			break;

		case ROTATION:
			if(vertical_triangle == V_TRIANGLE_1)
				state = try_go_angle(-PI4096/2, ROTATION, PLACEMENT_BRAS, ERROR, FAST);
			if(vertical_triangle == V_TRIANGLE_2 || vertical_triangle == V_TRIANGLE_3)
				state = try_go_angle(0, ROTATION, PLACEMENT_BRAS, ERROR, FAST);
			if(vertical_triangle == V_TRIANGLE_4)
				state = try_go_angle(PI4096/2, ROTATION, PLACEMENT_BRAS, ERROR, FAST);
			break;

		case PLACEMENT_BRAS:
			state = ACT_arm_move(ACT_ARM_POS_TAKE_ON_EDGE, 0, 0, PLACEMENT_BRAS, BACK, PARKED_NOT_HANDLED);
			break;

		case BACK:
			state = try_going(triangle_pos_end[vertical_triangle].x, triangle_pos_end[vertical_triangle].y, BACK, RUSH_IN_THE_FLOOR, PARKED_NOT_HANDLED, FAST, ANY_WAY, DODGE_AND_WAIT);
			break;

		case RUSH_IN_THE_FLOOR:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_elevator_arm_rush_in_the_floor(33, RUSH_IN_THE_FLOOR, WAIT, PARKED_NOT_HANDLED);
			break;

		case WAIT:
			state = ELEMENT_wait_time(300,WAIT,UP_ARM);
			break;

		case UP_ARM:
			state = ACT_arm_move(ACT_ARM_POS_DISPOSED_SIMPLE,0, 0, UP_ARM, PLACEMENT_FOYER, PARKED_NOT_HANDLED);
			break;

		case PLACEMENT_FOYER:{
			static GEOMETRY_point_t goal_pos;
			Sint16 cos, sin, l, teta;
			if(entrance){
				teta = atan2(1500-global.env.pos.y, 1050-global.env.pos.x)*4096.;
				COS_SIN_4096_get(teta, &cos, &sin);
				display(teta);
				l = dist_point_to_point(global.env.pos.x, global.env.pos.y, 1050, 1500);
				display(l);
				goal_pos.x = cos*(l-(CENTRAL_HEARTH_RADIUS+83))/4096. + global.env.pos.x;
				goal_pos.y = sin*(l-(CENTRAL_HEARTH_RADIUS+83))/4096. + global.env.pos.y;
				display(goal_pos.x);
				display(goal_pos.y);

			}
			state = try_going(goal_pos.x, goal_pos.y, PLACEMENT_FOYER, DROP, PARKED_NOT_HANDLED, FAST, FORWARD, DODGE_AND_WAIT);

		}break;

		case DROP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(500, DROP, PARKED);
			break;

		case PARKED_NOT_HANDLED:{
			static enum state_e state1, state2;

			if(entrance){
				ACT_pompe_order(ACT_POMPE_STOP, 0);
				state1 = PARKED_NOT_HANDLED;
				state2 = PARKED_NOT_HANDLED;
			}
			if(state1 == PARKED_NOT_HANDLED)
				state1 = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARKED_NOT_HANDLED, ERROR, ERROR);
			if(state2 == PARKED_NOT_HANDLED)
				state2 = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, PARKED_NOT_HANDLED, ERROR, ERROR);

			if((state1 == ERROR && state2 != PARKED_NOT_HANDLED) || (state1 != PARKED_NOT_HANDLED && state2 == ERROR))
				state = ERROR;
			else if(state1 != PARKED_NOT_HANDLED && state2 != PARKED_NOT_HANDLED)
				state = ERROR;
		}break;

		case PARKED:
			state = ACT_arm_move(ACT_ARM_POS_PARKED, 0, 0, PARKED, DONE, ERROR);
			break;

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return NOT_HANDLED;
			break;
	}
	return IN_PROGRESS;
}

error_e ACT_return_triangle_on_edge(vertical_triangle_e vertical_triangle){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		PLACEMENT_INIT,
		PLACEMENT_BRAS,
		ADVANCE,
		WAIT,
		UP_ARM,
		BACK,
		PLACEMENT_BRAS_2,
		PLACEMENT_FOYER,
		DROP,
		PARKED_NOT_HANDLED,
		PARKED,
		ERROR,
		DONE
	);

	const GEOMETRY_point_t triangle_pos_begin[4] = {
		{800,	400},
		{1600,	1300},
		{1600,	1700},
		{800,	2600}
	};

	const GEOMETRY_point_t triangle_pos_end[4] = {
		{800,	200},
		{1800,	1300},
		{1800,	1700},
		{800,	280}
	};

	switch (state) {
		case IDLE:
			state = PLACEMENT_INIT;
			break;

		case PLACEMENT_INIT:
			state = try_going(triangle_pos_begin[vertical_triangle].x, triangle_pos_begin[vertical_triangle].y, PLACEMENT_INIT, PLACEMENT_BRAS, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT);
			break;

		case PLACEMENT_BRAS:
			state = ACT_arm_move(ACT_ARM_POS_TAKE_ON_EDGE, 0, 0, PLACEMENT_BRAS, ADVANCE, PARKED_NOT_HANDLED);
			break;

		case ADVANCE:
			state = try_going(triangle_pos_end[vertical_triangle].x, triangle_pos_end[vertical_triangle].y, ADVANCE, WAIT, PARKED_NOT_HANDLED, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case WAIT:
			state = ELEMENT_wait_time(300,WAIT,UP_ARM);
			break;

		case UP_ARM:
			state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM, BACK, PARKED_NOT_HANDLED);
			break;

		case BACK:
			state = try_going(triangle_pos_begin[vertical_triangle].x, triangle_pos_begin[vertical_triangle].y, BACK, PLACEMENT_BRAS_2, PARKED_NOT_HANDLED, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case PLACEMENT_BRAS_2:
			state = ACT_arm_move(ACT_ARM_POS_DISPOSED_SIMPLE,0, 0, PLACEMENT_BRAS_2, PLACEMENT_FOYER, PARKED_NOT_HANDLED);
			break;

		case PLACEMENT_FOYER:{
			static GEOMETRY_point_t goal_pos;
			Sint16 cos, sin, l, teta;
			if(entrance){
				teta = atan2(1500-global.env.pos.y, 1050-global.env.pos.x)*4096.;
				COS_SIN_4096_get(teta, &cos, &sin);
				l = dist_point_to_point(global.env.pos.x, global.env.pos.y, 1050, 1500);
				goal_pos.x = cos*(l-(CENTRAL_HEARTH_RADIUS+83))/4096. + global.env.pos.x;
				goal_pos.y = sin*(l-(CENTRAL_HEARTH_RADIUS+83))/4096. + global.env.pos.y;

			}
			state = try_going(goal_pos.x, goal_pos.y, PLACEMENT_FOYER, DROP, PARKED_NOT_HANDLED, FAST, FORWARD, DODGE_AND_WAIT);

		}break;

		case DROP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(300, DROP, PARKED);
			break;

		case PARKED_NOT_HANDLED:{
			static enum state_e state1, state2;

			if(entrance){
				ACT_pompe_order(ACT_POMPE_STOP, 0);
				state1 = PARKED_NOT_HANDLED;
				state2 = PARKED_NOT_HANDLED;
			}
			if(state1 == PARKED_NOT_HANDLED)
				state1 = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARKED_NOT_HANDLED, ERROR, ERROR);
			if(state2 == PARKED_NOT_HANDLED)
				state2 = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, PARKED_NOT_HANDLED, ERROR, ERROR);

			if((state1 == ERROR && state2 != PARKED_NOT_HANDLED) || (state1 != PARKED_NOT_HANDLED && state2 == ERROR))
				state = ERROR;
			else if(state1 != PARKED_NOT_HANDLED && state2 != PARKED_NOT_HANDLED)
				state = ERROR;
		}break;

		case PARKED:
			state = ACT_arm_move(ACT_ARM_POS_PARKED, 0, 0, PARKED, DONE, ERROR);
			break;

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return NOT_HANDLED;
			break;
	}
	return IN_PROGRESS;
}

error_e ACT_arm_deploy_torche_guy(torch_choice_e choiceTorch, torch_dispose_zone_e dispose_zone){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DEPLOY_TORCH,
		IDLE,
		TORCHE,
		WAIT_STABILISATION_1,
		DOWN_ARM,
		UP_ARM,
		UP_ARM_ERROR,
		DROP_TRIANGLE,
		WAIT_STABILISATION_2,
		WAIT_TRIANGLE_BREAK,
		BACK,
		BACK_FAIL,
		WAIT,
		DROP_ADV_TRIANGLE,
		RETURN,
		INVERSE_PUMP,
		PREPARE_RETURN,
		SMALL_ARM_DEPLOYED,
		SMALL_ARM_PARKED,
		TAKE_RETURN,
		RUSH_TAKE_RETURN,
		WAIT_2,
		GRAP_TRIANGLE,
		OPEN,
		ADVANCE,
		PARKED_NOT_HANDLED,
		PARKED,
		ERROR,
		DONE
	);

	static GEOMETRY_point_t torch;

	static GEOMETRY_point_t drop_pos[3];
	static GEOMETRY_point_t work_point;
	static GEOMETRY_point_t return_point;
	static GEOMETRY_point_t drop_adv_pos;
	static bool_e droped_triangle[3];
	static bool_e fail_return;
	static Uint8 niveau;
	static Uint8 nb_try_back;

	//if(dispose_zone == HEARTH_OUR){
		/*drop_pos[0] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};
		drop_pos[1] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};
		drop_pos[2] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};*/
	//}

	switch(state){
		case IDLE :{
			Sint16 cos, sin;

			torch = TORCH_get_position(choiceTorch);

			display(torch.x);
			display(torch.y);

			// Imposé pour les test
			//COS_SIN_4096_get(global.env.pos.angle + PI4096/2, &cos, &sin);
			//torch = (GEOMETRY_point_t){global.env.pos.x+(Sint32)cos*180/4096, global.env.pos.y+(Sint32)sin*180/4096};

			drop_adv_pos.x = global.env.pos.x;
			drop_adv_pos.y = global.env.pos.y;// A définir

			COS_SIN_4096_get(global.env.pos.angle + PI4096, &cos, &sin);

			return_point.x = global.env.pos.x + (Sint32)cos*DIST_RETURN_RETURN_TRIANGLE/4096;
			return_point.y = global.env.pos.y + (Sint32)sin*DIST_RETURN_RETURN_TRIANGLE/4096;

			work_point.x = global.env.pos.x;
			work_point.y = global.env.pos.y;

			COS_SIN_4096_get(global.env.pos.angle, &cos, &sin);

			drop_pos[0] = (GEOMETRY_point_t){2000-160, 80};
			drop_pos[1] = (GEOMETRY_point_t){2000-80, 160};
			drop_pos[2] = (GEOMETRY_point_t){2000-80, 80};
			/*drop_pos[0] = (GEOMETRY_point_t){(Sint32)cos*150/4096 + global.env.pos.x, (Sint32)sin*150/4096 + global.env.pos.y};
			drop_pos[1] = (GEOMETRY_point_t){(Sint32)cos*150/4096 + global.env.pos.x, (Sint32)sin*150/4096 + global.env.pos.y};
			drop_pos[2] = (GEOMETRY_point_t){(Sint32)cos*150/4096 + global.env.pos.x, (Sint32)sin*150/4096 + global.env.pos.y};*/


			state = TORCHE;
			niveau = 0;
			fail_return = FALSE;
			nb_try_back = 0;
			droped_triangle[0] = FALSE;
			droped_triangle[1] = FALSE;
			droped_triangle[2] = FALSE;
		}break;

		case TORCHE :
			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,torch.x, torch.y, TORCHE, WAIT_STABILISATION_1, PARKED_NOT_HANDLED);
			break;

		case WAIT_STABILISATION_1 :
			state = ELEMENT_wait_time(300, WAIT_STABILISATION_1, DOWN_ARM);
			break;

		case DOWN_ARM:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_elevator_arm_rush_in_the_floor(125-niveau*30, DOWN_ARM, UP_ARM, UP_ARM_ERROR);
			break;

		case UP_ARM: // Commentaire à enlever quand on aura le moteur DC sur le bras
			if(entrance)
				nb_try_back = 0;

			if((niveau == 1 && choiceTorch == OUR_TORCH) || ((niveau == 0 || niveau == 2) && choiceTorch == ADVERSARY_TORCH)) // Va retourne le deuxieme triangle
				state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM, BACK, PARKED_NOT_HANDLED);
			else
				state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM, DROP_TRIANGLE, PARKED_NOT_HANDLED);
			break;

		case UP_ARM_ERROR:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_STOP, 0);

			state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM_ERROR, TORCHE, PARKED_NOT_HANDLED);
			break;

		case DROP_TRIANGLE :

			if(fail_return && ((niveau == 1 && choiceTorch == OUR_TORCH) || ((niveau == 0 || niveau == 2) && choiceTorch == ADVERSARY_TORCH)))
				state = WAIT_STABILISATION_2;
			else
				state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE,drop_pos[niveau].x, drop_pos[niveau].y, DROP_TRIANGLE, WAIT_STABILISATION_2, PARKED_NOT_HANDLED);

			if(ON_LEAVING(DROP_TRIANGLE))
				droped_triangle[niveau] = TRUE;
			break;

		case WAIT_STABILISATION_2 :
			state = ELEMENT_wait_time(300, WAIT_STABILISATION_2, WAIT_TRIANGLE_BREAK);
			break;

		case WAIT_TRIANGLE_BREAK : // Attendre que le triangle soit relacher avant de faire autre chose
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(500, WAIT_TRIANGLE_BREAK, (niveau>=2)? PARKED:TORCHE);

			if(ON_LEAVING(WAIT_TRIANGLE_BREAK)){
				if(niveau == 0 && choiceTorch == ADVERSARY_TORCH && fail_return)
					state = PARKED;
				niveau++;
				ACT_pompe_order(ACT_POMPE_STOP, 0);
			}
			break;

		case BACK:
			state = try_going(return_point.x, return_point.y, BACK, RETURN, BACK_FAIL, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case BACK_FAIL:
			state = try_going(work_point.x, work_point.y, BACK_FAIL, WAIT, PARKED_NOT_HANDLED, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case WAIT:
			if(entrance)
				nb_try_back++;

			if(nb_try_back <= 2)
				state = ELEMENT_wait_time(1500, WAIT, BACK);
			else
				state = DROP_ADV_TRIANGLE;
			break;

		case DROP_ADV_TRIANGLE:
			if(niveau == 0) // Si c'est le premier triangle on le pose hors terrain
				state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE, drop_adv_pos.x, drop_adv_pos.y, DROP_ADV_TRIANGLE, WAIT_TRIANGLE_BREAK, PARKED_NOT_HANDLED);
			else if((niveau == 1 || niveau == 2) && droped_triangle[0]) // Sinon si on a déjà posé un triangle on le pose dessus
				state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE, drop_pos[0].x, drop_pos[0].y, DROP_ADV_TRIANGLE, WAIT_TRIANGLE_BREAK, PARKED_NOT_HANDLED);
			else if(niveau == 2 && droped_triangle[1]) // Sinon si on a pas posé le premier triangle mais le deuxième
				state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE, drop_pos[1].x, drop_pos[1].y, DROP_ADV_TRIANGLE, WAIT_TRIANGLE_BREAK, PARKED_NOT_HANDLED);
			else if(niveau == 1 || niveau == 2)	// Sinon pose le triangle hors terrain
				state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE, drop_adv_pos.x, drop_adv_pos.y, DROP_ADV_TRIANGLE, WAIT_TRIANGLE_BREAK, PARKED_NOT_HANDLED);
			else	// sinon c'est qu'il ya un soucis
				state = PARKED_NOT_HANDLED;
			break;

		case RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_RETURN,0, 0, RETURN, INVERSE_PUMP, PARKED_NOT_HANDLED);
			break;

		case INVERSE_PUMP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(300, INVERSE_PUMP, PREPARE_RETURN);
			break;

		case PREPARE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_WAIT_RETURN,0, 0, PREPARE_RETURN, SMALL_ARM_DEPLOYED, PARKED_NOT_HANDLED);
			break;

		case SMALL_ARM_DEPLOYED:
			state = ACT_small_arm_move(ACT_SMALL_ARM_MID, SMALL_ARM_DEPLOYED, SMALL_ARM_PARKED, PARKED_NOT_HANDLED);
			break;

		case SMALL_ARM_PARKED:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, SMALL_ARM_PARKED, TAKE_RETURN, PARKED_NOT_HANDLED);

			if(ON_LEAVING(SMALL_ARM_PARKED)){
				ACT_pompe_order(ACT_POMPE_STOP, 0);
				if(state == PARKED_NOT_HANDLED){
					fail_return = TRUE;
					state = OPEN;
				}
			}
			break;

		case TAKE_RETURN:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_arm_move(ACT_ARM_POS_TO_TAKE_RETURN,0, 0, TAKE_RETURN, RUSH_TAKE_RETURN, PARKED_NOT_HANDLED);
			break;

		case RUSH_TAKE_RETURN:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_elevator_arm_rush_in_the_floor(33, RUSH_TAKE_RETURN, GRAP_TRIANGLE, PARKED_NOT_HANDLED);
			break;

		case WAIT_2:
			state = ELEMENT_wait_time(200, WAIT_2, GRAP_TRIANGLE);
			break;

		case GRAP_TRIANGLE: // prise du triangle au sol
			state = ELEMENT_wait_time(300, GRAP_TRIANGLE, OPEN);
			break;

		case OPEN:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN, ADVANCE, PARKED_NOT_HANDLED);
			break;

		case ADVANCE:
			state = try_going(work_point.x, work_point.y, ADVANCE, DROP_TRIANGLE, PARKED_NOT_HANDLED, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case PARKED_NOT_HANDLED:{
			static enum state_e state1, state2;

			if(entrance){
				ACT_pompe_order(ACT_POMPE_STOP, 0);
				state1 = PARKED_NOT_HANDLED;
				state2 = PARKED_NOT_HANDLED;
			}
			if(state1 == PARKED_NOT_HANDLED)
				state1 = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARKED_NOT_HANDLED, ERROR, ERROR);
			if(state2 == PARKED_NOT_HANDLED)
				state2 = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, PARKED_NOT_HANDLED, ERROR, ERROR);

			if((state1 == ERROR && state2 != PARKED_NOT_HANDLED) || (state1 != PARKED_NOT_HANDLED && state2 == ERROR))
				state = ERROR;
			else if(state1 != PARKED_NOT_HANDLED && state2 != PARKED_NOT_HANDLED)
				state = ERROR;
		}break;

		case PARKED:{
			static enum state_e state1, state2;

			if(entrance){
				state1 = PARKED;
				state2 = PARKED;
			}
			if(state1 == PARKED)
				state1 = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARKED, DONE, ERROR);
			if(state2 == PARKED)
				state2 = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, PARKED, DONE, ERROR);

			if((state1 == DONE && state2 == ERROR) || (state1 == ERROR && state2 == DONE))
				state = ERROR;
			else if(state1 == DONE && state2 == DONE)
				state = DONE;
		}break;

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}

/* ----------------------------------------------------------------------------- */
/* 							Fonctions Belgique					                 */
/* ----------------------------------------------------------------------------- */

Uint8 putTorchInMidle(Uint8 currentState, Uint8 successState, Uint8 failState);
Uint8 putTorchInAdversary(Uint8 currentState, Uint8 successState, Uint8 failState);

void strat_belgique_guy(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BELGIUM_STRAT,
		IDLE,
		EXTRACT_FROM_ZONE,
		WAIT,
		AVANCE,
		TOURNE,
		TOURNE2,
		OPEN_ARM,
		DEPLOY_ARM,
		CLOSE_ARM,
		PARK_ARM,
		RANGER,
		OPEN_END,
		CLOSE_END,
		TRIANGLE_1,
		TRIANGLE_1SHOOT,
		TRIANGLE_2,
		TRIANGLE_2SHOOT,
		TRIANGLE_3,
		TRIANGLE_3SHOOT,
		GO_FRESCO,
		BEFORE_WALL,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		EXTRACTION,
		PASSAGE,
		AVANCE2,
		TOURNE3,
		TOURNE4,
		OPEN_ARM2,
		DEPLOY_ARM2,
		CLOSE_ARM2,
		PARK_ARM3,
		GO_TORCHE,
		TOURNE5,
		OPEN_ARM3,
		DEPLOY_ARM3,
		TOURNE6,
		CLOSE_ARM3,
		PARK_ARM2,
		/*DO_FIRST_TRIANGLE,
		CLOSE_ARM,
		PARK_ARM,
		POS_MID,
		TORCH,*/
		DONE,
		ERROR
	);
	static time32_t time;
	static bool_e timeout=FALSE;

	switch(state){
		case IDLE:
			state = EXTRACT_FROM_ZONE;
			break;

		case EXTRACT_FROM_ZONE:
			state = try_going(997,COLOR_Y(150),EXTRACT_FROM_ZONE,WAIT,EXTRACT_FROM_ZONE,
							  FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT); // Pas d'évitement on sort de la zone de départ, il ne faut pas bloquer Pierre
			break;

		case WAIT :
			if(entrance)
				time = global.env.match_time;
			if(global.env.match_time - time > 5000)
				state = AVANCE;
			break;

		case AVANCE :
			state = try_going(610,COLOR_Y(150),AVANCE,TOURNE,RANGER,
							  FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;

		case TOURNE :
			if(global.env.color == RED)
				state = try_go_angle(-PI4096/2, TOURNE, OPEN_ARM, TRIANGLE_1, FAST);
			else
				state = try_go_angle(PI4096/2, TOURNE, OPEN_ARM, TRIANGLE_1, FAST);
			break;

		case OPEN_ARM:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_ARM, DEPLOY_ARM, ERROR);
			break;

		case DEPLOY_ARM:
			if(global.env.color == RED)
				state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, DEPLOY_ARM, CLOSE_ARM, CLOSE_ARM);
			else
				state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, DEPLOY_ARM, TOURNE2, CLOSE_ARM);
			break;

		case TOURNE2:
			state = try_go_angle(0, TOURNE2, CLOSE_ARM, CLOSE_ARM, FAST);
			break;

		case CLOSE_ARM:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, CLOSE_ARM, PARK_ARM, PARK_ARM);
			break;

		case PARK_ARM:
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARK_ARM, TRIANGLE_1, TRIANGLE_1);
			break;

		case RANGER :
			state = try_going(150, COLOR_Y(150), RANGER, OPEN_END, OPEN_END, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			break;

		case OPEN_END :
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_END, CLOSE_END, CLOSE_END);
			break;

		case CLOSE_END :
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, CLOSE_END, DONE, DONE);
			break;



		/*case DO_FIRST_TRIANGLE:
			state = try_going(1444, COLOR_Y(298), DO_FIRST_TRIANGLE, CLOSE_ARM, CLOSE_ARM, FAST, FORWARD, DODGE_AND_NO_WAIT);
			break;

		case CLOSE_ARM:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, CLOSE_ARM, PARK_ARM, PARK_ARM);
			break;

		case PARK_ARM:
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARK_ARM, TORCH, TORCH);
			break;

		case TORCH:
			state = putTorchInAdversary(TORCH,POS_MID,ERROR);
			break;

		case POS_MID:
			state = try_going(1500, 1500, POS_MID, DONE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT);*/

		case TRIANGLE_1:
			state = try_going(825, COLOR_Y(389), TRIANGLE_1, TRIANGLE_1SHOOT, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_1SHOOT:
			state = try_going(1109, COLOR_Y(380), TRIANGLE_1SHOOT, PASSAGE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_2:
			state = try_going(596, COLOR_Y(500), TRIANGLE_2, TRIANGLE_2SHOOT, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_2SHOOT:
			state = try_going(596, COLOR_Y(858), TRIANGLE_2SHOOT, GO_FRESCO, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case GO_FRESCO:
			state = try_going(500, COLOR_Y(1300), GO_FRESCO, BEFORE_WALL, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case BEFORE_WALL:
			state = try_going(300, COLOR_Y(1300), BEFORE_WALL, PUSH_MOVE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case PUSH_MOVE://Le fait forcer contre le mur pour poser la fresque
			ASSER_push_rush_in_the_wall(FORWARD,TRUE,PI4096,TRUE);//Le fait forcer en marche arriere
			state = WAIT_END_OF_MOVE;
			break;

		case WAIT_END_OF_MOVE:
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				state = EXTRACTION;
			}
			break;

		case EXTRACTION:
			state = try_going(500, COLOR_Y(1300), EXTRACTION, PASSAGE, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case PASSAGE:
			state = try_going(1350, COLOR_Y(1000), PASSAGE, TRIANGLE_3, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_3:
			state = try_going(1600, COLOR_Y(1100), TRIANGLE_3, TRIANGLE_3SHOOT, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_3SHOOT:
			state = try_going(1600, COLOR_Y(750), TRIANGLE_3SHOOT, AVANCE2, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case AVANCE2 :
			state = try_going(2000-140,COLOR_Y(1500),AVANCE2,TOURNE3,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;

		case TOURNE3 :
			state = try_go_angle(0, TOURNE3, OPEN_ARM2, ERROR, FAST);
			break;

		case OPEN_ARM2:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_ARM2, DEPLOY_ARM2, ERROR);
			break;

		case DEPLOY_ARM2:
			state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE,global.env.pos.x + 200, global.env.pos.y, DEPLOY_ARM2, TOURNE4, TOURNE4);
			break;

		case TOURNE4 :
			if(global.env.color == RED)
				state = try_go_angle(PI4096/2, TOURNE4, CLOSE_ARM2, ERROR, FAST);
			else
				state = try_go_angle(-PI4096/2, TOURNE4, CLOSE_ARM2, ERROR, FAST);
			break;

		case CLOSE_ARM2:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, CLOSE_ARM2, PARK_ARM3, DONE);
			break;

		case PARK_ARM3:
			state = ACT_arm_move(ACT_ARM_POS_PARKED, 0, 0, PARK_ARM3, DONE, ERROR);
			break;


		case DONE:
			break;

		case ERROR:
			break;

		default:
			break;
	}
}


Uint8 putTorchInMidle(Uint8 currentState, Uint8 successState, Uint8 failState){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BELGIUM_STRAT,
		IDLE,
		TORCHE_MOI_LA_FLAMME,
		CARRY_TORCH,
		EXTRACT_FROM_TORCH,
		BYPASS_TORCH,
		TORCHE_LA,
		DEPILE,
		DONE,
		ERROR
	);


	const displacement_t points_torcheMoiLaFlamme[] = {(displacement_t){{1000,COLOR_Y(600)},FAST},
													   (displacement_t){{1320,COLOR_Y(1200)},SLOW},
													   (displacement_t){{1380, COLOR_Y(1320)},SLOW}
													  };
	const displacement_t points_bypassTorch[] = {(displacement_t){{1570,COLOR_Y(1310)},FAST},
												 (displacement_t){{1510,COLOR_Y(1750)},FAST}};
	switch (state){
		case IDLE:
			state = TORCHE_MOI_LA_FLAMME;
			break;
		case TORCHE_MOI_LA_FLAMME:
			state = try_going_multipoint(points_torcheMoiLaFlamme,2,TORCHE_MOI_LA_FLAMME,CARRY_TORCH,ERROR,FORWARD,NO_DODGE_AND_WAIT,END_AT_BREAK);
			break;
		case CARRY_TORCH:
			state = try_going(1380,COLOR_Y(1320),CARRY_TORCH,EXTRACT_FROM_TORCH,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case EXTRACT_FROM_TORCH:
			state = try_going(1390,COLOR_Y(1260),EXTRACT_FROM_TORCH,BYPASS_TORCH,ERROR,BACKWARD,FAST,NO_DODGE_AND_WAIT);
			break;
		case BYPASS_TORCH:
			state = try_going_multipoint(points_bypassTorch,2,BYPASS_TORCH,TORCHE_LA,ERROR,BACKWARD,NO_DODGE_AND_WAIT,END_AT_BREAK);
			break;
		case TORCHE_LA:
			state = try_going(1380,COLOR_Y(1650),TORCHE_LA,DEPILE,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case DEPILE:
			state = check_sub_action_result(do_torch(OUR_TORCH,FALSE, HEARTH_CENTRAL,NO_DISPOSE),DEPILE,DONE,ERROR);
			state = DONE;
			break;
		case DONE:
			return successState;
			break;
		case ERROR:
			return failState;
			break;
		default:
			break;
	}
	return currentState;
}

Uint8 putTorchInAdversary(Uint8 currentState, Uint8 successState, Uint8 failState){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BELGIUM_STRAT,
		IDLE,
		TORCHE_MOI_LA_FLAMME,
		CARRY_TORCH,
		EXTRACT_FROM_TORCH,
		BYPASS_TORCH,
		TORCHE_LA,
		DEPILE,
		DONE,
		ERROR
	);


	const displacement_t points_torcheMoiLaFlamme[] = {(displacement_t){{900,COLOR_Y(600)},FAST},
													   (displacement_t){{1150,COLOR_Y(930)},FAST},
													   (displacement_t){{1600, COLOR_Y(1470)},FAST},
													   (displacement_t){{1730, COLOR_Y(1520)},FAST},
													   (displacement_t){{1640, COLOR_Y(2590)},FAST}
													  };
	const displacement_t points_bypassTorch[] = {(displacement_t){{1570,COLOR_Y(1310)},FAST},
												 (displacement_t){{1510,COLOR_Y(1750)},FAST}};
	switch (state){
		case IDLE:
			state = TORCHE_MOI_LA_FLAMME;
			break;
		case TORCHE_MOI_LA_FLAMME:
			state = try_going_multipoint(points_torcheMoiLaFlamme,5,TORCHE_MOI_LA_FLAMME,EXTRACT_FROM_TORCH,ERROR,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
			break;
		case CARRY_TORCH:
			state = try_going(1600,COLOR_Y(2420),CARRY_TORCH,EXTRACT_FROM_TORCH,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case EXTRACT_FROM_TORCH:
			state = try_going(1600,COLOR_Y(2420),EXTRACT_FROM_TORCH,DONE,ERROR,BACKWARD,FAST,NO_DODGE_AND_WAIT);
			break;
		case BYPASS_TORCH:
			state = try_going_multipoint(points_bypassTorch,2,BYPASS_TORCH,TORCHE_LA,ERROR,BACKWARD,NO_DODGE_AND_WAIT,END_AT_BREAK);
			break;
		case TORCHE_LA:
			state = try_going(1380,COLOR_Y(1650),TORCHE_LA,DEPILE,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case DEPILE:
			//state = check_sub_action_result(do_torch(OUR_TORCH,FALSE,HEARTH_CENTRAL),DEPILE,DONE,ERROR);
			state = DONE;
			break;
		case DONE:
			return successState;
			break;
		case ERROR:
			return failState;
			break;
		default:
			break;
	}
	return currentState;
}


/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test			         			 */
/* ----------------------------------------------------------------------------- */


void strat_inutile_guy(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_INUTILE,
		IDLE,
		POS_DEPART,
		BACK,
		ADVANCE,
		RAMEMENER_TORCH,
		DEPLOY_TORCH,
		DO_TORCH,
		DO_TRIANGLE,
		POINT_1,
		POINT_2,
		POINT_3,
		ON,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(400),POS_DEPART,ON,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		//case RAMEMENER_TORCH:
		//	state = check_sub_action_result(travel_torch_line(OUR_TORCH,FILED_FRESCO,1750,250),RAMEMENER_TORCH,DONE,ERROR);
		//	break;

		case ON:
			state = check_sub_action_result(ACT_take_triangle_on_edge(V_TRIANGLE_2), ON, DONE, ERROR);
			break;

		case BACK:
			state = try_advance(200, BACK, ADVANCE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case ADVANCE:
			state = try_going_until_break(600,COLOR_Y(2000),ADVANCE,DO_TORCH,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case DO_TORCH:
			state = check_sub_action_result(do_torch(OUR_TORCH,FALSE,(SWITCH_STRAT_1)? HEARTH_OUR: HEARTH_ADVERSARY,NO_DISPOSE),DO_TORCH,DONE,ERROR);
			break;

		case DEPLOY_TORCH:
			state = check_sub_action_result(ACT_arm_deploy_torche_guy(OUR_TORCH,HEARTH_OUR),DEPLOY_TORCH,DONE,ERROR);
			break;

		case DO_TRIANGLE:
			state = check_sub_action_result(do_triangles_between_trees(),DO_TRIANGLE,DONE,ERROR);
			break;

		case POINT_1:
			state = try_going(1350,400,POINT_1,POINT_2,POINT_1,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case POINT_2:
			state = try_going(1650,1100,POINT_2,POINT_3,POINT_2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case POINT_3:
			state = try_going(520,1100,POINT_3,POINT_1,POINT_3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case DONE:
			break;
		case ERROR:
			break;
		default:
			break;
	}

}

void strat_xbee_guy(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_XBEE,
		IDLE,
		POS_DEPART,
		XBEE,
		POS_END,
		RELACHE_ZONE,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(250),POS_DEPART,XBEE,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
			break;

		case XBEE:
			state = try_lock_zone(MZ_FRUIT_TRAY, 1000, XBEE, POS_END, ERROR, ERROR);
			break;

		case POS_END:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_END,RELACHE_ZONE,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
			break;

		case RELACHE_ZONE:
			if(entrance)
				ZONE_unlock(MZ_FRUIT_TRAY);

			state = DONE;
			break;

		case DONE:
			break;
		case ERROR:
			break;
		default:
			break;
	}

}


// Stratégie de test des déctections de triangle
void Strat_Detection_Triangle(void){
	// x = 1000
	// y = 120 led verte / 2880 led rouge

	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_DETECTION_TRIANGLE,
		INIT,
		AVANCER,
		SCAN_FOYER_GAUCHE,
		PLACEMENT_FOYER_MILIEU,
		SCAN_FOYER_MILIEU,
		LIBERE_MILIEU,
		PLACEMENT_FOYER_DROIT,
		SCAN_FOYER_DROIT,
		DONE,
		ERROR
	);


	switch (state){
		case INIT :
			state = AVANCER;
			break;

		case AVANCER :
			state = try_going(1000, 2500, AVANCER, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_GAUCHE :
			state = ELEMENT_try_going_and_rotate_scan(0, -PI4096/2, 90,
							 1600, 2600, SCAN_FOYER_GAUCHE, PLACEMENT_FOYER_MILIEU, PLACEMENT_FOYER_MILIEU, FAST, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_MILIEU :
			state = ELEMENT_try_going_and_rotate_scan(3*PI4096/4, -3*PI4096/4, 90,
							  1000, 1900, PLACEMENT_FOYER_MILIEU, LIBERE_MILIEU, LIBERE_MILIEU, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case LIBERE_MILIEU :
			state = try_going(1500, 1900, LIBERE_MILIEU, PLACEMENT_FOYER_DROIT, PLACEMENT_FOYER_DROIT, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_DROIT :
			state =  ELEMENT_try_going_and_rotate_scan(-PI4096/2, -PI4096, 90,
							   1600, 400, PLACEMENT_FOYER_DROIT, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case DONE :
			break;

		default :
			break;
	}
}


void strat_test_warner_triangle(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_TEST_WARNER_TRIANGLE,
		INIT,
		AVANCER,
		PLACEMENT,
		DETECTION_TRIANGLE_3,
		DISPLAY_ANSWER,
		DONE
	);
	switch(state){
		case INIT :
			state = AVANCER;
			break;

		case AVANCER :
			state = try_going(1000, 2500, AVANCER, PLACEMENT, PLACEMENT, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case PLACEMENT :
			state = try_going(1600, 2300, PLACEMENT, DETECTION_TRIANGLE_3, DETECTION_TRIANGLE_3, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case DETECTION_TRIANGLE_3 :
			if(entrance)
				ELEMENT_launch_triangle_warner(3);
			state = try_going(1600, 650, DETECTION_TRIANGLE_3, DISPLAY_ANSWER, DISPLAY_ANSWER, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case DISPLAY_ANSWER :
			if(ELEMENT_triangle_present())
				debug_printf("triangle n°3 présent\n");
			else
				debug_printf("triangle n°3 non présent\n");
			state = DONE;
			break;

		case DONE :
			break;

	}

}


void strat_test_arm(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_TEST_ARM,
		IDLE,
		TRIANGLE,
		OPEN1,
		TORCHE,
		OPEN2,
//		RETURN,
//		OPEN3,
//		PARKED,
		WAIT1,
//		WAIT2,
		DONE
	);

	//static time32_t time;

	switch(state){
		case IDLE :
			ASSER_set_position(500, 500, -PI4096/4);
			global.env.pos.x = 500;
			global.env.pos.y = 500;
			global.env.pos.angle = -PI4096/4;

			state = OPEN1;
			break;


		case OPEN1 :
			state = check_sub_action_result(ACT_arm_deploy_torche_guy(OUR_TORCH, HEARTH_OUR), OPEN1, DONE, DONE);
			break;

		case DONE :
			break;
		default:
			break;
	}
}


static void send_message_to_pierre(Uint11 sid, Uint8 data0)
{
	CAN_msg_t msg;

	msg.sid = sid;
	switch(sid)
	{
		case XBEE_GUY_HAVE_DONE_FIRE:
			msg.size = 1;
			msg.data[0] = data0;
			break;
		default:
			msg.size = 0;
			break;
	}

	CANMsgToXbee(&msg,FALSE);
}



