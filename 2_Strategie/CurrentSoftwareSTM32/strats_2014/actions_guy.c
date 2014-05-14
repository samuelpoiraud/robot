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
#include <math.h>

static void REACH_POINT_GET_OUT_INIT_send_request();
error_e goto_adversary_zone(void);

//TODO Si absence de Pierre, on doit être capable de compiler un code de GUY qui met un max de points (tout nos feux... !)


//SWITCH_STRAT2 et SWITCH_STRAT1 permettent de choisir le chemin à emprunter pour se rendre chez l'adversaire
//Voir le case INIT de la subaction initiale
//ATTENTION, la route coté mammouths suggère fortement que Pierre commence par la torche et non par la fresque :: sinon conflit !


#define DROP_TRIANGLE_UNDER_TREE    // Va deposer l'un des deux triangles sous les arbres
#define DETECTION_TRIANGLE_MIDDLE	// Pour savoir si nous avons besoins de faire un dection des triangles du milieu ou non pour la strat triangles_between_tree
#define DIM_TRIANGLE 100

//#define GUY_FALL_FIRST_FIRE // Si on souhaite faire tomber le premier feux dés le début

// Fonctionne que pour les chemins MAMMOUTH_SIDE et HEART_SIDE
bool_e rush_to_torch = FALSE;  // Si FALSE va faire tomber un ou des triangle(s) avant
bool_e fall_fire_wall_adv = TRUE;  // Va aller faire tomber le feu si on sait que l'ennemis ne le fais pas tomber des le debut


//IL FAUT définir une zone de dépose pour la torche adverse. (si on a réussi à la prendre)
	//#define DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH
	#define DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH
	//#define DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH

//IL FAUT définir une zone de dépose en cas d'échec de la première tentative pour la torche adverse. (si on a réussi à la prendre)
	//#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_OUR_HEARTH
	#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_ADVERSARY_HEARTH
	//#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_ON_CENTRAL_HEARTH
	//#define IF_FAIL_DISPOSE_ADVERSARY_TORCH_NO_DISPOSE

#define DIM_START_TRAVEL_TORCH 200

typedef enum{
	NORTH_MAMMOUTH = 0,
	NORTH_HEART,
	SOUTH_HEART,
	SOUTH_TREES
}initial_path_e;	//Chemin initial choisi pour se rendre du coté adverse



pos_scan_t pos_scan[2] = {{{1200,1800},{1200,1900},600,1600,1800,2300,-PI4096,-PI4096/2}, // FOYER CENTRALE
						  {{1650,2600},{1600,2550},800,1700,2000,2700,0,PI4096/2}		// FOYER ADVERSE
						 };

volatile initial_path_e initial_path;
static bool_e pierre_reach_point_C1 = FALSE;

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
		GOTO_ADVERSARY_ZONE,
		GOTO_TREE_INIT,
		FALL_FIRE_WALL_TREE,  // Fait tomber les deux feux contre le mur milieu
		WAIT_GOTO_MAMMOUTH_INIT,
		GOTO_MAMMOUTH_INIT,
		FALL_MOBILE_MM_ADV,
		GOTO_HEART_INIT,
		FALL_FIRE_MOBILE_TREE_ADV,
		GOTO_TORCH_ADVERSARY,
		DO_TORCH,
		FALL_FIRE_MOBILE_MM_ADV,
		FALL_FIRE_WALL_ADV,
		DONE,
		ERROR
	);

	//static displacement_t points[6];
	//static Uint8	nb_points = 0;
	static bool_e we_prevented_pierre_to_get_out = FALSE;
	static Sint16 y_to_prevent_pierre_to_get_out;
	static time32_t t;
	static torch_dispose_zone_e dispose_zone_for_adversary_torch = HEARTH_ADVERSARY;
	static torch_dispose_zone_e if_fail_dispose_zone_for_adversary_torch = NO_DISPOSE;
	if(global.env.reach_point_C1)
		pierre_reach_point_C1 = TRUE;

	switch(state)
	{
		case INIT:
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
				ASSER_set_acceleration(80);	//Acceleration de guy au démarrage...
			if(global.env.asser.calibrated)
				state  = try_going_until_break(700,COLOR_Y(300),GET_OUT_POS_START,GOTO_ADVERSARY_ZONE, GOTO_ADVERSARY_ZONE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			else
				state  = try_going_until_break(635,COLOR_Y(300),GET_OUT_POS_START,GOTO_ADVERSARY_ZONE, GOTO_ADVERSARY_ZONE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		/*case PREVENT_PIERRE_WE_ARE_GOT_OUT:
			if(entrance)
				REACH_POINT_GET_OUT_INIT_send_request();
			state = GOTO_ADVERSARY_ZONE;
			break;
		*/
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
			state = check_sub_action_result(goto_adversary_zone(),GOTO_ADVERSARY_ZONE,DO_TORCH,ERROR);
			if(state != GOTO_ADVERSARY_ZONE)
				ASSER_set_acceleration(64);

			//ERROR n'est pas censé se produire... la sub_action étant censée trouver une solution pour se rendre en zone adverse !
			break;

		case DO_TORCH:
			if(dispose_zone_for_adversary_torch == NO_DISPOSE)
			{
				SD_printf("Récupération de la torche adverse : désactivée\n");
				state = DONE;	//On ne fait pas la torche -> on rend la main à la high_level_strat
			}
			else
			{
				switch(do_torch(ADVERSARY_TORCH, dispose_zone_for_adversary_torch, if_fail_dispose_zone_for_adversary_torch))
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
 * Sub action qui permet à guy de se rendre dans la zone adverse.
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
		case SB2:
			if(entrance)
			{
				if(initial_path == SOUTH_HEART)
					success_state = SC2;
				else
					success_state = SC3;
			}
			//TODO sur cette trajectoire, bouger le bras ou adapter la trajectoire pour gérer le feu mobile central !
			state = try_going_until_break(1350,COLOR_Y(800),SB2,success_state,SC12,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SC0:
			state = try_going_until_break(500,COLOR_Y(1200),SC0,SW0,SC1,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
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
			state = try_going_until_break(750,COLOR_Y(1200),SC1,success_state,fail_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
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
			state = try_going_until_break(1400,COLOR_Y(1200),SC2,success_state,fail_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case SC3:
			state = try_going_until_break(1750,COLOR_Y(1200),SC3,SW3,SC2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
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
 * - se rend à coté de la torche SI NECESSAIRE (si on y est pas déjà.. mais c'est mieux si on y est !)
 * - Fonction de récupération : pousse la torche dans la bonne direction (en fonction du foyer de dépose)
 * - Se rend au foyer de dépose
 * 	- en cas d'échec... tente une seconde position de dépose (passée en paramètre)
 * 	- en cas d'échec... abandonne le dépilement de la torche
 * - dépile la torche
 *  - en cas d'échec, abandonne
 * - s'extrait de la zone pour pouvoir rendre la main
 * - enregistre le fait de la dépose (et la zone déposée)
 */

error_e do_torch(torch_choice_e torch_choice, torch_dispose_zone_e dispose_zone, torch_dispose_zone_e if_fail_dispose_zone)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DO_TORCH,
		IDLE,
		COMPUTE,
		PRE_POSITIONNING,
		POS_START_TORCH,
		POS_INTERMEDIATE,
		MOVE_TORCH,
		END_TORCH,
		REMOTENESS,
		DEPLOY_TORCH,
		ERROR,
		DONE
	);
	static torch_dispose_zone_e current_dispose_zone = NO_DISPOSE;
	static bool_e fail_at_first_dispose_try = FALSE;	//
	static bool_e i_have_the_torch = FALSE;	//
	static GEOMETRY_point_t posStart,posEnd,posIntermediate,posPre;
	static bool_e posIntermediate_enable = FALSE;
	static bool_e posPrepositionning_enable = FALSE;
	// S'éloigne à la fin de la poussée mais ralenti aussi avant la fin de la poussée
	static GEOMETRY_point_t eloignement;

	static GEOMETRY_point_t torch;
	Uint16 norm;

	switch(state){
		case IDLE :
			fail_at_first_dispose_try = FALSE;
			current_dispose_zone = dispose_zone;
			state = COMPUTE;
			break;
		case COMPUTE:
			torch = TORCH_get_position(torch_choice);
			posIntermediate_enable = FALSE;

			/*	La trajectoire de poussé de torche est construite ainsi :
			 * 1- posPre : SSSI posPrepositionning_enable !
			 * 2- posStart
			 * 3- posIntermediate : SSSI posIntermediate_enable
			 * 4- posEnd
			 * 5- eloignement
			 */
			switch(current_dispose_zone)
			{
				case FILED_FRESCO:
					posEnd.x = 300;
					posEnd.y = 1500;
					break;
				case HEARTH_OUR:
					posEnd.x = 1700;		//TODO positions à régler. (il faudra sans doute 2 positions en fonction de notre couleur !)
					posEnd.y = COLOR_Y(300);
					if(torch_choice == ADVERSARY_TORCH)
					{
						posIntermediate.x = 1450;
						posIntermediate.y = 1500;
						posIntermediate_enable = TRUE;
					}
					break;
				case HEARTH_ADVERSARY:
					posEnd.x = 1700;			//TODO positions à régler.
					posEnd.y = COLOR_Y(2700);
					if(torch_choice == OUR_TORCH)
					{
						posIntermediate.x = 1450;
						posIntermediate.y = 1500;
						posIntermediate_enable = TRUE;
					}
					break;
				case ANYWHERE:
						//No break;
				case HEARTH_CENTRAL:
						//No break;
				default:
					posEnd.x = 1400;
					posEnd.y = 1500;
					break;
			}

			float coefx, coefy;
			if(posIntermediate_enable)
			{
				norm = GEOMETRY_distance(torch,posIntermediate);
				coefx = (torch.x - posIntermediate.x)/(norm*1.);
				coefy = (torch.y - posIntermediate.y)/(norm*1.);
			}
			else
			{
				norm = GEOMETRY_distance(torch,posEnd);
				coefx = (torch.x - posEnd.x)/(norm*1.);
				coefy = (torch.y - posEnd.y)/(norm*1.);
			}

			posStart.x = torch.x + DIM_START_TRAVEL_TORCH*coefx;
			posStart.y = torch.y + DIM_START_TRAVEL_TORCH*coefy;

			if(posIntermediate_enable)
			{
				norm = GEOMETRY_distance(torch,posEnd);
				coefx = (torch.x - posEnd.x)/(norm*1.);
				coefy = (torch.y - posEnd.y)/(norm*1.);
			}
			//Sinon, l'angle calculé plus haut est le bon... pas besoin de le recalculer
			eloignement.x = posEnd.x + DIM_START_TRAVEL_TORCH*coefx;
			eloignement.y = posEnd.y + DIM_START_TRAVEL_TORCH*coefy;

			//On connait le start, la position de la torche et notre position actuelle... Dans certains cas de figure, il faut une position de prépositionnement.
			//TODO calculer si besoin le prepositionning point...

			//On doit passer par un point de prépositionnement SSSI nous sommes du mauvais coté de la torche par rapport au posStart.
			//LE CALCUL GENERIQUE DE TOUT LES CAS EST UN ALGO DIFFICILE A TESTER, TRES EXIGEANT... et inutile compte tenu du peu de cas d'application en match.
			//Les points de prépositionnement sont donc ajoutés à la main, pour certains cas de figure identifiés seulement !

			if(torch_choice == ADVERSARY_TORCH)
			{
				switch(current_dispose_zone)
				{
					case FILED_FRESCO:

						break;
					case HEARTH_ADVERSARY:
						if(global.env.pos.x > posStart.x && COLOR_Y(global.env.pos.y) > COLOR_Y(posStart.y))
						{
							posPrepositionning_enable = TRUE;
							posPre.x = posStart.x;
							posPre.y = posStart.y;
							if(global.env.pos.x - posStart.x < absolute(global.env.pos.y - posStart.y))
								posPre.y = global.env.pos.y;	//Je suis plus prêt des x, je rejoins le bon x en gardant mon y
							else
								posPre.x = global.env.pos.x;	//Je suis plus prêt des y, je rejoins le bon y en gardant mon x
						}
						break;
					case HEARTH_OUR:		//no break
					case ANYWHERE:			//no break;
					case HEARTH_CENTRAL:	//no break;
					default:
						if(global.env.pos.x > posStart.x && COLOR_Y(global.env.pos.y) < COLOR_Y(posStart.y))
						{
							posPre.x = 1350;
							posPre.y = COLOR_Y(2300);
							posPrepositionning_enable = TRUE;
						}
						break;
				}
			}
			else	//OUR TORCH...
			{
				//Pas de point de prépositionnement.. On assume la précondition que pour prendre notre torche il faut être bien placé !!!
			}

			if(last_state == ERROR)
				state = POS_INTERMEDIATE;	//On a échoué lors d'une précédente tentative de pose... on va directement sur le MOVE_TORCH pour rejoindre le point eloignement de la nouvelle zone de dépsose !
			else if(GEOMETRY_distance((GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}, posStart) < 50)
				state = MOVE_TORCH;
			else if(posPrepositionning_enable)
				state = PRE_POSITIONNING;
			else
				state = POS_START_TORCH;

			SD_printf("Torch: start=[%d;%d] | torch=[%d;%d] | end=[%d;%d] | remotness=[%d;%d]\n",posStart.x, posStart.y, torch.x, torch.y, posEnd.x, posEnd.y, eloignement.x, eloignement.y);
			break;
		case PRE_POSITIONNING:	//Il faut parfois se mettre dans un prépositionnement qui permet de pas recouvrir la torche avant de la pousser comme souhaité
			state = try_going(posPre.x, posPre.y, PRE_POSITIONNING, POS_START_TORCH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;
		case POS_START_TORCH:
			state = try_going(posStart.x, posStart.y, POS_START_TORCH, POS_INTERMEDIATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;
		case POS_INTERMEDIATE:
			if(entrance)
				i_have_the_torch = TRUE;
			if(posIntermediate_enable)	//On se rend d'abord à une position intermédiaire.
				state = try_going_until_break(posIntermediate.x, posIntermediate.y, POS_INTERMEDIATE, MOVE_TORCH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			else
				state = MOVE_TORCH;	//Pas de position intermédiaire.. on file directement au MOVE_TORCH
			break;
		case MOVE_TORCH :
			state = try_going_until_break(eloignement.x, eloignement.y, MOVE_TORCH, END_TORCH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case END_TORCH:
			state = try_going_until_break(posEnd.x, posEnd.y, END_TORCH, REMOTENESS, ERROR, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case REMOTENESS:  // eloignement
			if(entrance)
			{
				i_have_the_torch = FALSE;
				TORCH_new_position(torch_choice);
			}

			state = try_going(eloignement.x, eloignement.y, REMOTENESS, DEPLOY_TORCH, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case ERROR:
			if(fail_at_first_dispose_try || if_fail_dispose_zone == NO_DISPOSE || i_have_the_torch == FALSE)
			{
				if(i_have_the_torch)
					TORCH_new_position(torch_choice);
				//TODO : libérer la torche ! (il faudrait d'ailleurs s'en extraire...)
				state = IDLE;
				return NOT_HANDLED;
			}
			else
			{
				fail_at_first_dispose_try = TRUE;
				current_dispose_zone = if_fail_dispose_zone;
				state = COMPUTE;
			}
			break;


		case DEPLOY_TORCH:	//On déploie la torche sur le foyer
			state = check_sub_action_result(ACT_arm_deploy_torche(torch_choice,current_dispose_zone),DEPLOY_TORCH,DONE,ERROR);
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
			state = try_going(pos_scan[scan].scan.x, COLOR_Y(pos_scan[scan].scan.y), POS_START, SCAN_TRIANGLE, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case SCAN_TRIANGLE:
			state = ELEMENT_try_going_and_rotate_scan(COLOR_ANGLE(pos_scan[scan].tetaMin), COLOR_ANGLE(pos_scan[scan].tetaMax), 90,
													  pos_scan[scan].scan.x, COLOR_Y(pos_scan[scan].scan.y), SCAN_TRIANGLE, ANALYZE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			//state = ANALYZE;
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
					size = 170; // Rayon du milieu
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

			state = try_going(point.x,point.y, POS_TAKE, TAKE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);

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
			state = try_going(pos_scan[scan].scan.x,pos_scan[scan].scan.y, MOVE_DROP_TRIANGLE, DROP_TRIANGLE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
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
			state = try_going(pos_scan[scan].get_out.x,pos_scan[scan].get_out.y, GET_OUT, (last_state != ERROR)? DONE:ERROR, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			if(est_dans_carre(500, 1650, COLOR_Y(1850), COLOR_Y(2650), (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // Est sur le pathfind
				state = RETURN_NOT_HANDLED;
			else
				state = GET_OUT;

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
					dpl_dect[0] = (GEOMETRY_point_t){1600,1900};
					dpl_dect[1] = (GEOMETRY_point_t){1600,1100};
				}else{
					dpl_dect[1] = (GEOMETRY_point_t){1600,1900};
					dpl_dect[0] = (GEOMETRY_point_t){1600,1100};
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

error_e ACT_arm_deploy_torche(torch_choice_e choiceTorch, torch_dispose_zone_e dispose_zone){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DEPLOY_TORCH,
		IDLE,
		TORCHE,
		DOWN_ARM,
		UP_ARM,
		FILED_TRIANGLE,
		WAIT_TRIANGLE_BREAK,
		BACK,
		RETURN,
		INVERSE_PUMP,
		PREPARE_RETURN,
		SMALL_ARM_DEPLOYED,
		SMALL_ARM_PARKED,
		TAKE_RETURN,
		GRAP_TRIANGLE,
		OPEN,
		ADVANCE,
		PARKED_NOT_HANDLED,
		ERROR,
		DONE
	);

	static GEOMETRY_point_t torch;

	static GEOMETRY_point_t point[3];
	static Uint8 i = 0;	//TODO renommer ce i.... pour un nom plus explicite !

	if(dispose_zone == HEARTH_OUR){
		point[0] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};
		point[1] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};
		point[2] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};



		/*point[0] = (GEOMETRY_point_t){1800,COLOR_Y(200)};
		point[1] = (GEOMETRY_point_t){1900,COLOR_Y(250)};
		point[2] = (GEOMETRY_point_t){1900,COLOR_Y(250)};*/
	}

	switch(state){
		case IDLE :
			//torch = TORCH_get_position(choiceTorch);

			torch.x = global.env.pos.x;
			torch.y = global.env.pos.y+150;

			state = TORCHE;
			i = 0;
			break;

		case TORCHE :
			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,torch.x, torch.y, TORCHE, DOWN_ARM, PARKED_NOT_HANDLED);
			break;

		case DOWN_ARM: // Commentaire à enlever quand on aura le moteur DC sur le bras
			if(entrance){
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);
			}
			state = ACT_elevator_arm_rush_in_the_floor(120-i*30, DOWN_ARM, UP_ARM, UP_ARM);
			break;

		case UP_ARM: // Commentaire à enlever quand on aura le moteur DC sur le bras
			if((i == 1 && choiceTorch == OUR_TORCH) || ((i == 0 || i == 2) && choiceTorch == ADVERSARY_TORCH)) // Va retourne le deuxieme triangle
				state = ACT_elevator_arm_move(120, UP_ARM, BACK, PARKED_NOT_HANDLED);
			else
				state = ACT_elevator_arm_move(120, UP_ARM, FILED_TRIANGLE, PARKED_NOT_HANDLED);
			break;

		case FILED_TRIANGLE :
			if(entrance)
				i++;

			state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE,point[i-1].x, point[i-1].y, FILED_TRIANGLE, WAIT_TRIANGLE_BREAK, PARKED_NOT_HANDLED);
			break;

		case WAIT_TRIANGLE_BREAK : // Attendre que le triangle soit relacher avant de faire autre chose
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(500, WAIT_TRIANGLE_BREAK, (i>2)? DONE:TORCHE);

			if(ON_LEAVING(WAIT_TRIANGLE_BREAK))
				ACT_pompe_order(ACT_POMPE_STOP, 0);
			break;

		case BACK:
			state = try_going(global.env.pos.x-300, global.env.pos.y, BACK, RETURN, PARKED_NOT_HANDLED, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_RETURN,0, 0, RETURN, INVERSE_PUMP, PARKED_NOT_HANDLED);
			break;

		case INVERSE_PUMP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_pump_capture_object(INVERSE_PUMP, PREPARE_RETURN, PREPARE_RETURN);
			break;

		case PREPARE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, PREPARE_RETURN, SMALL_ARM_DEPLOYED, PARKED_NOT_HANDLED);
			break;

		case SMALL_ARM_DEPLOYED:
			state = ACT_small_arm_move(ACT_SMALL_ARM_DEPLOYED, SMALL_ARM_DEPLOYED, SMALL_ARM_PARKED, PARKED_NOT_HANDLED);
			break;

		case SMALL_ARM_PARKED:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, SMALL_ARM_PARKED, TAKE_RETURN, PARKED_NOT_HANDLED);

			if(ON_LEAVING(SMALL_ARM_PARKED))
				ACT_pompe_order(ACT_POMPE_STOP, 0);
			break;

		case TAKE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_TAKE_RETURN,0, 0, TAKE_RETURN, GRAP_TRIANGLE, PARKED_NOT_HANDLED);
			break;

		case GRAP_TRIANGLE: // prise du triangle au sol
			if(entrance){
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);
			}
			state = ELEMENT_wait_pump_capture_object(GRAP_TRIANGLE, OPEN, OPEN);
			break;

		case OPEN:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN, ADVANCE, PARKED_NOT_HANDLED);
			break;

		case ADVANCE:
			state = try_going(global.env.pos.x+300, global.env.pos.y, ADVANCE, FILED_TRIANGLE, PARKED_NOT_HANDLED, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case PARKED_NOT_HANDLED:{
			static enum state_e state1, state2;

			if(entrance){
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

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			state = IDLE;
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
			state = check_sub_action_result(do_torch(OUR_TORCH,HEARTH_CENTRAL,NO_DISPOSE),DEPILE,DONE,ERROR);
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
			//state = check_sub_action_result(do_torch(OUR_TORCH,HEARTH_CENTRAL),DEPILE,DONE,ERROR);
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
		RAMEMENER_TORCH,
		DEPLOY_TORCH,
		DO_TORCH,
		DO_TRIANGLE,
		POINT_1,
		POINT_2,
		POINT_3,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_DEPART,DO_TRIANGLE,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		//case RAMEMENER_TORCH:
		//	state = check_sub_action_result(travel_torch_line(OUR_TORCH,FILED_FRESCO,1750,250),RAMEMENER_TORCH,DONE,ERROR);
		//	break;

		case DO_TORCH:
			state = check_sub_action_result(do_torch(OUR_TORCH,HEARTH_OUR,HEARTH_CENTRAL),DO_TORCH,DONE,ERROR);
			break;

		case DEPLOY_TORCH:
			state = check_sub_action_result(ACT_arm_deploy_torche(OUR_TORCH,HEARTH_OUR),DEPLOY_TORCH,DONE,ERROR);
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
			ASSER_set_position(500, 500, 0);
			global.env.pos.x = 500;
			global.env.pos.y = 500;
			global.env.pos.angle = 0;

			state = OPEN1;
			break;


		case OPEN1 :
			state = check_sub_action_result(ACT_arm_deploy_torche(OUR_TORCH, HEARTH_OUR), OPEN1, DONE, DONE);
			break;

		case DONE :
			break;
		default:
			break;
	}
}


