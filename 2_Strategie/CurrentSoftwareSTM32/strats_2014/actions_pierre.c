/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.c
 *	Package : Carte S�/strats2013
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� par .
 *	Version 2013/10/03
 */

#include "actions_pierre.h"
#include "actions_net.h"
#include "actions_Pfruit.h"
#include "actions_balls.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_adc.h"
#include "../state_machine_helper.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "../high_level_strat.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_can_over_xbee.h"
#include "../Supervision/Buzzer.h"
#include "../Supervision/SD/SD.h"


static void REACH_POINT_C1_send_request();

// SWITCH_STRAT_1 est utilis� pour le lancer de balles (5 balles nous et une sur ennemis si active sinon 6 balles chez nous)
// SWITCH_STRAT_2 est utilis� pour le choix d�s le d�but aller vers la torche directement apres les balles ou faire les fresques si actif fait torche
// SWITCH_STRAT_3 : activation du ramassage de la torche.


#define MAX_HEIGHT_ARM	120
#define CONVERT_LASERS_VALUE_TO_DIST(x)	(x*2)

//Pour Activer le mode manual de pose de fresque
#define MODE_MANUAL_FRESCO TRUE
#define POS_Y_MANUAL_FRESCO COLOR_Y(1500) // Mettre une valeur entre POS_MIN_FRESCO et POS_MAX_FRESCO
#define NB_TRY_FRESCO 2	//(Choix possible : 1, 2 ou 3 : nombre de tentatives de d�pose de la fresque en cas de d�tection de fresque non pos�e.



/**********************************************************************************************************************************
 *
 *		Robot prototype devra modifier le sens ( BACKWARD / FORWARD ) sur le lanceLauncher, depose de fruit et ramassage de fruit
 *				sur la verison finale
 *
 **********************************************************************************************************************************/

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test								 */
/* ----------------------------------------------------------------------------- */

#define LARGEUR_ROBOT_BACK	124
#define LARGEUR_LABIUM	250

#define DECALAGE_LARGEUR 200
#define POS_MIN_FRESCO 1450		//A cause de l'incapacit� de tourner � moins de 25cm du bac de fruits...
#define POS_MAX_FRESCO 1650		//A cause de l'incapacit� de tourner � moins de 25cm du bac de fruits...


//Les differente's actions que pierre devra faire lors d'un match
static bool_e action_fresco_filed = FALSE;
static bool_e i_must_deal_with_our_torch = FALSE;	//On doit g�rer notre torche.
static bool_e i_do_torch_before_fresco = FALSE;		//On fait la torche en priorit�... ou le contraire.
static bool_e cooperation_enable = FALSE;			//Action de coop�ration

// Lors de la recalibration, sera appeler en extern vers d'autre fichier ( ne pas passer en static )
volatile GEOMETRY_point_t offset_recalage = {0, 0};

//Provisoire pour le moment juste pour test
#define ADVERSARY_DETECTED_HOKUYO FALSE

#define NB_MAX_ADVERSARY_FRESCO_POSITION   2 //Les positions devront etre compris entre 1700 et 1300
volatile Uint16 adversary_fresco_positions[NB_MAX_ADVERSARY_FRESCO_POSITION]={1450,1590};
volatile Uint8 adversary_fresco_index = 2;


error_e sub_action_initiale(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_INITIALE,
		INIT,
		WAIT_TELL_GUY,
		GET_OUT,
		LANCE_LAUNCHER,
		GOTO_PUSH_TRIANGLE,
		PUSH_TRIANGLE,
		DO_TREE_1,
		DO_TREE_2,
		DO_FIRE_HOME,
		GOTO_TORCH_FIRST_POINT,
		GOTO_TORCH_SECOND_POINT,
		GOTO_TORCH_THIRD_POINT,
		GOTO_HEARTH,
		FAIL_FIRST_POINT,
		FAIL_SECOND_POINT,
		FAIL_THIRD_POINT,
		COME_BACK_FOR_GOING_FRESCO,
		DEPLOY_TORCH,
		GOTO_FRESCO,
		DO_FRESCO,
		FILE_FRUIT,
		LANCE_LAUNCHER_ADVERSARY,
		DONE,
		ERROR
	);


	static enum state_e fail_state, success_state;

	// Mettre a false si pas le cas
	static bool_e guy_get_out_init = FALSE;
	static displacement_t point[6];

	if(global.env.guy_took_fire[FIRE_ID_TORCH_OUR])		//Lorsque Guy d�cide de se diriger vers notre torche (et avant que Pierre ait pu finir son lanc�, Guy envoie un message levant ce flag...)
		i_must_deal_with_our_torch = FALSE;	//Si Guy a pris notre torche, on abandonne son �ventuelle r�cup�ration pr�vue.

	if(global.env.reach_point_get_out_init)
			guy_get_out_init = TRUE;

	switch(state)
	{

		case INIT:
			if(global.env.asser.calibrated)
				state = WAIT_TELL_GUY;
			else	//On est en train de jouer un match de test sans l'avoir calibr�... donc Guy n'est pas l� !
				state = LANCE_LAUNCHER;

			// Si on active le switch de fa�on � envoyer toute les balles sur le premier mammouth, nous devons d�sactive
			//le lancer de balles sur le mammouth adverse dans la high_level_strat
			if(!SWITCH_STRAT_1)
			{
				set_sub_act_done(SUB_LANCE_ADV,TRUE);
				set_sub_act_enable(SUB_LANCE_ADV,FALSE);
				cooperation_enable = FALSE;
			}
			else
				cooperation_enable = TRUE;

			if(SWITCH_STRAT_3)
				i_must_deal_with_our_torch = TRUE;
			else
				i_must_deal_with_our_torch = FALSE;

			if(SWITCH_STRAT_2)
				i_do_torch_before_fresco = TRUE;
			else
				i_do_torch_before_fresco = FALSE;

			if(global.env.color == RED)
			{
				point[0] =  (displacement_t){{750,1100},	FAST}; // Point pos bonne prise de torche
				point[1] =  (displacement_t){{1000,1000},	FAST};
				point[2] = 	(displacement_t){{1150,850},	FAST};
				point[3] = 	(displacement_t){{750,400},		FAST}; // Pos se mettre devant l'arbre
				point[4] = 	(displacement_t){{1000,700},	FAST}; // Pos si triangle pas tomber par guy
				point[5] = 	(displacement_t){{1200,600},	FAST};
			}
			else
			{
				point[0] =  (displacement_t){{620,1670},	FAST}; // Point pos bonne prise de torche
				point[1] =  (displacement_t){{1000,2000},	FAST};
				point[2] = 	(displacement_t){{1150,2150},	FAST};
				point[3] = 	(displacement_t){{1650,1900},	FAST}; // Pos se mettre devant l'arbre
				point[4] = 	(displacement_t){{1350,1870},	FAST}; // Pos si triangle pas tomber par guy
				point[5] = 	(displacement_t){{1400,2250},	FAST};
			}


			break;

		case WAIT_TELL_GUY:{
			static Uint16 last_time;
			if(entrance)
				last_time = global.env.match_time;

			if(guy_get_out_init || global.env.match_time > last_time + 3000)
				state = GET_OUT;
			}break;
		case GET_OUT:	//Sort de la zone de d�part SANS ROTATION, pour rejoindre un point int�rieur au rectangle d'acceptation de la subaction de lanc� des balles
			state = try_going_until_break(520, COLOR_Y(290),GET_OUT,LANCE_LAUNCHER,LANCE_LAUNCHER,FAST,ANY_WAY,NO_AVOIDANCE);		//On force le passage si Guy est devant nous..........
			break;

		case LANCE_LAUNCHER:
			if(entrance)
			{
				if(i_must_deal_with_our_torch && !global.env.guy_took_fire[FIRE_ID_TORCH_OUR])
				{
					//En cas d'�chec (rencontre adverse d�s le lanc�...) on file vers la torche... m�me s'il �tait pr�vu qu'on commence par la fresque... (chemin fresque innaccessible, il y a un adversaire)
					fail_state = GOTO_TORCH_FIRST_POINT;
					if(i_do_torch_before_fresco)
						success_state =  GOTO_TORCH_FIRST_POINT;
					else
						success_state = GOTO_FRESCO;
				}
				else
				{
					fail_state = DONE;
					if(i_do_torch_before_fresco)
						success_state =  DONE;	//On a plus rien � faire... La priorit� des actions dans la high-level d�cidera de la prochaine action...
					else
						success_state = GOTO_FRESCO;
				}
			}
			if(success_state == GOTO_FRESCO && global.env.guy_is_bloqued_in_north && i_must_deal_with_our_torch && !global.env.guy_took_fire[FIRE_ID_TORCH_OUR])
			{	//Si Guy est bloqu� au nord et que l'on pr�voyait d'aller � la fresque, on va plut�t aller faire la torche...
				success_state = GOTO_TORCH_FIRST_POINT;
			}
			//Il y a aussi le risque que cet �vittement soit du � Guy...
			state = check_sub_action_result(sub_lance_launcher((cooperation_enable)?FALSE:TRUE, global.env.color),LANCE_LAUNCHER,success_state,fail_state);
			break;

		case GOTO_TORCH_FIRST_POINT:
			if(entrance) // Ouvre les pinces
				ACT_torch_locker(ACT_TORCH_Locker_Unlock);
			//NO_DODGE : on g�re nous m�me l'�chec.
			state = try_going_until_break(point[0].point.x, point[0].point.y,GOTO_TORCH_FIRST_POINT,GOTO_TORCH_SECOND_POINT,FAIL_FIRST_POINT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GOTO_TORCH_SECOND_POINT:
//			if(entrance) // Ouvre les pinces
//				ACT_torch_locker(ACT_TORCH_Locker_Unlock);
			//NO_DODGE : on g�re nous m�me l'�chec.
			state = try_going_until_break(point[1].point.x, point[1].point.y,GOTO_TORCH_SECOND_POINT,GOTO_TORCH_THIRD_POINT,FAIL_SECOND_POINT,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GOTO_TORCH_THIRD_POINT:
			if(entrance) // Serre l'objet les pinces
				ACT_torch_locker(ACT_TORCH_Locker_Lock);
//				ASSER_WARNER_arm_x(800);
//			if(global.env.asser.reach_x)
//				;//REACH_POINT_C1_send_request();
			//NO_DODGE : on g�re nous m�me l'�chec.
			state = try_going(point[2].point.x, point[2].point.y,GOTO_TORCH_THIRD_POINT,(global.env.guy_took_fire[FIRE_ID_MOBILE_CENTRAL]==TRUE)? DO_TREE_1:GOTO_PUSH_TRIANGLE,FAIL_THIRD_POINT,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GOTO_PUSH_TRIANGLE:
			state = try_going(point[4].point.x, point[4].point.y,GOTO_PUSH_TRIANGLE,PUSH_TRIANGLE,FAIL_THIRD_POINT,FAST,(global.env.color == RED) ? FORWARD:BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case PUSH_TRIANGLE:
			if(entrance)
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Open);
			//NO_DODGE : on g�re nous m�me l'�chec.
			state = try_going(point[5].point.x, point[5].point.y,PUSH_TRIANGLE,DO_TREE_1,FAIL_THIRD_POINT,FAST,FORWARD,NO_DODGE_AND_WAIT);

			break;

		case FAIL_FIRST_POINT:
			if(entrance)
				ACT_torch_locker(ACT_TORCH_Locker_Inside);
			//NO_DODGE : on g�re nous m�me l'�chec.
			state = try_going_until_break(750, COLOR_Y(400),FAIL_FIRST_POINT,DEPLOY_TORCH,GOTO_TORCH_FIRST_POINT,FAST,(global.env.color == RED)? FORWARD : BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case FAIL_SECOND_POINT:
			if(entrance){
				ACT_torch_locker(ACT_TORCH_Locker_Inside);
				ACT_fruit_mouth_goto(ACT_FRUIT_Verrin_Close);
			}

			state = COME_BACK_FOR_GOING_FRESCO;
			break;

		case FAIL_THIRD_POINT:
			state = COME_BACK_FOR_GOING_FRESCO;
			break;
		case COME_BACK_FOR_GOING_FRESCO:	//On a pas r�ussi � rejoindre la torche... on file vers la fresque
			if(action_fresco_filed)	//Fresque d�j� faite
				state = ERROR;		//On rend la main � la high_level
			else
				state = try_going_until_break(520, COLOR_Y(1100), COME_BACK_FOR_GOING_FRESCO, GOTO_FRESCO, GOTO_TORCH_FIRST_POINT, FAST, ANY_WAY, DODGE_AND_WAIT);
			break;

		case GOTO_FRESCO:
			if(entrance)
			{
				if(!i_do_torch_before_fresco && i_must_deal_with_our_torch)
					fail_state = GOTO_TORCH_FIRST_POINT;
				else
					fail_state = DONE;	//Il n'y a plus rien � faire
			}
			//Si on doit aller faire la torche en cas d'�chec, pas de DODGE, c'est notre �chapatoire.
			//Si on doit rendre la main ensuite, on fait un DODGE
			state = try_going_until_break(400,COLOR_Y(1500),GOTO_FRESCO,DO_FRESCO,fail_state,FAST,ANY_WAY,(fail_state==DONE)?DODGE_AND_WAIT:NO_DODGE_AND_NO_WAIT);
			break;

		case DO_TREE_1:
			if(entrance)
			{
				if(i_must_deal_with_our_torch)	//TODO : � remplacer par l'information si(j'ai une torche !)
					success_state = GOTO_HEARTH;
				else
					success_state = DO_TREE_2;
			}
			state = check_sub_action_result(manage_fruit(TREE_OUR,(global.env.color == RED)?CHOICE_TREE_1:CHOICE_TREE_2,TRIGO),DO_TREE_1,success_state,ERROR);
			break;

		case GOTO_HEARTH:	//Va vers le foyer pour d�ployer la torche
			//On tente un DODGE... et on rend la main � la high_level
			//TODO : en cas d'�chec, trouver une alternative pour aller l�cher la torche ou la d�piler ailleurs !
			state = try_going_until_break(1650, COLOR_Y(350), GOTO_HEARTH, DEPLOY_TORCH, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT);
			break;

		case DO_FRESCO:
			if(entrance)
			{
				if(get_presenceFruit())
					success_state = FILE_FRUIT;	//J'ai des fruits, alors je vais les poser maintenant que je suis pr�s du bac
				else
				{
					if(i_do_torch_before_fresco)	//J'ai d�j� fait la torche, je n'ai plus rien � faire
						success_state = DONE;
					else
						success_state = GOTO_TORCH_FIRST_POINT;	//Je dois aller faire la torche
				}
				if(i_do_torch_before_fresco && i_must_deal_with_our_torch)
					fail_state = GOTO_TORCH_FIRST_POINT;		//Je dois aller faire la torche
				else
					fail_state = ERROR;							//Je n'ai plus rien � faire ici.. (le FILE_FRUIT sera fait en high_level)
			}
			state = check_sub_action_result(strat_manage_fresco(),DO_FRESCO,success_state,fail_state);
			break;

		case DEPLOY_TORCH:{
			static Uint16 last_time;

			if(entrance){ // Ouvre les pinces
				last_time = global.env.match_time;
				ACT_torch_locker(ACT_TORCH_Locker_Unlock);
			}

			if(guy_get_out_init || global.env.match_time > last_time + 2000){
				state = DO_TREE_2;	//TODO !
			}

			}break;



		case DO_TREE_2:
			state = check_sub_action_result(manage_fruit(TREE_OUR,(global.env.color == RED)?CHOICE_TREE_2:CHOICE_TREE_1,TRIGO),DO_TREE_2,DO_FIRE_HOME,ERROR);
			break;

		case DO_FIRE_HOME:
			if(entrance)
			{
				ACT_torch_locker(ACT_TORCH_Locker_Inside);

				if(i_do_torch_before_fresco)
					success_state = GOTO_FRESCO;
				else
					success_state = FILE_FRUIT;
			}
			if(global.env.guy_took_fire[FIRE_ID_START])
			{
				SD_printf("Guy a fait le feu de la zone de d�part\n");
				state = success_state;
			}
			else
				state = try_going(800,COLOR_Y(400),DO_FIRE_HOME,success_state,ERROR,FAST,ANY_WAY,DODGE_AND_WAIT);
			break;

		case FILE_FRUIT:
			state = check_sub_action_result(strat_file_fruit(),FILE_FRUIT,(cooperation_enable)?LANCE_LAUNCHER_ADVERSARY:DONE,ERROR);
			break;

		// Si on lance une balle sur le mammouth ennemis
		case LANCE_LAUNCHER_ADVERSARY:
			state = check_sub_action_result(sub_lance_launcher(FALSE, (global.env.color == RED)?YELLOW:RED),LANCE_LAUNCHER_ADVERSARY,DONE, ERROR);
			break;


		case DONE:
			// On indique a high_strat_level que la sub_action_initiale a deja �t� faite pour pas la refaire une prochaine fois
			set_sub_act_done(SUB_ACTION_INIT,TRUE);
			return END_OK;
			break;

		case ERROR:
			// On indique a high_strat_level que la sub_action_initiale a deja �t� faite pour pas la refaire une prochaine fois
			set_sub_act_done(SUB_ACTION_INIT,TRUE);
			return NOT_HANDLED;
			break;
		//default:	//Pour �tre avertis s'il manque un cas !
		//	break;
	}

	return IN_PROGRESS;
}

void strat_homologation(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_HOMOLOGATION,
		INIT,
		WAIT_TELL_GUY,
		GET_OUT,
		LANCE_LAUNCHER,
		PROTECTED_FIRE,
		DO_NET
	);

	// Mettre a false si pas le cas
	static bool_e guy_get_out_init = FALSE;

	if(global.env.reach_point_get_out_init)
		guy_get_out_init = TRUE;


	switch(state){

		case INIT:
			if(global.env.asser.calibrated)
				state = WAIT_TELL_GUY;
			else	//On est en train de jouer un match de test sans l'avoir calibr�... donc Guy n'est pas l� !
				state = LANCE_LAUNCHER;

			break;

		case WAIT_TELL_GUY:{
			static Uint16 last_time;
			if(entrance)
				last_time = global.env.match_time;

			if(guy_get_out_init || global.env.match_time > last_time + 3000)
				state = GET_OUT;
			}break;
		case GET_OUT:	//Sort de la zone de d�part SANS ROTATION, pour rejoindre un point int�rieur au rectangle d'acceptation de la subaction de lanc� des balles
			state = try_going_until_break(520, COLOR_Y(290),GET_OUT,LANCE_LAUNCHER,LANCE_LAUNCHER,FAST,ANY_WAY,NO_AVOIDANCE);		//On force le passage si Guy est devant nous..........
			break;

		case LANCE_LAUNCHER:
			//Il y a aussi le risque que cet �vittement soit du � Guy...
			state = check_sub_action_result(sub_lance_launcher(TRUE, global.env.color),LANCE_LAUNCHER,PROTECTED_FIRE,PROTECTED_FIRE);
			break;

		case PROTECTED_FIRE:
			state = check_sub_action_result(protected_fires(OUR_FIRES),PROTECTED_FIRE,(TIME_TO_NET > global.env.match_time)?PROTECTED_FIRE:DO_NET,(TIME_TO_NET > global.env.match_time)?PROTECTED_FIRE:DO_NET);
			break;

		case DO_NET:
			strat_placement_net();
			break;

		default:
			break;
	}
}



void strat_inutile(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_INUTILE,
		IDLE,
		POS_DEPART,
		CHEMIN,
		GO_1,
		GO_2,
		GO_3,
		GO_4,
		LANCE_LAUNCHER,
		SUB_ACTION,
		PROTECTED_FIRES,
		DEPOSE_FRUIT,
		GET_OUT_CALIBRE,
		FRUIT_RED,
		FRUIT_YELLOW,
		RAMMASSE_FRUIT,
		LANCER_FILET,
		DROP_FRUIT,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_DEPART,DROP_FRUIT,POS_DEPART,250,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_CALIBRE:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),GET_OUT_CALIBRE,FRUIT_RED,ERROR,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GO_1:
			state = try_going(1400,COLOR_Y(450),GO_1,GO_2,GO_1,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GO_2:
			state = try_going(1400,COLOR_Y(3000-500),GO_2,GO_3,GO_2,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GO_3:
			state = try_going(600,COLOR_Y(3000-500),GO_3,GO_4,GO_3,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case GO_4:
			state = try_going(600,COLOR_Y(450),GO_4,GO_1,GO_4,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case LANCE_LAUNCHER:
			state = check_sub_action_result(sub_lance_launcher(TRUE,global.env.color),LANCE_LAUNCHER,DONE,ERROR);
			break;

		case SUB_ACTION:
			state = check_sub_action_result(sub_action_initiale(),SUB_ACTION,DONE,ERROR);
			break;

		case FRUIT_RED:
			state = check_sub_action_result(strat_ramasser_fruit_arbre1_double(TRIGO),FRUIT_RED,FRUIT_YELLOW,ERROR);
			break;

		case DROP_FRUIT:
			state = check_sub_action_result(strat_file_fruit(),DROP_FRUIT,DONE,ERROR);
			break;

		case FRUIT_YELLOW:
			state = check_sub_action_result(strat_ramasser_fruit_arbre2_double(TRIGO),FRUIT_YELLOW,FRUIT_RED,ERROR);
			break;

		case PROTECTED_FIRES:
			state = check_sub_action_result(protected_fires(OUR_FIRES),PROTECTED_FIRES,DONE,ERROR);
			break;

		case RAMMASSE_FRUIT:
			state = check_sub_action_result(manage_fruit(TREE_ADVERSARY,CHOICE_ALL_TREE,WAY_CHOICE),RAMMASSE_FRUIT,DONE,ERROR);
			break;

		case DEPOSE_FRUIT:
			state = check_sub_action_result(strat_file_fruit(),DEPOSE_FRUIT,DONE,ERROR);
			break;

		case LANCER_FILET:
			strat_placement_net();
			break;

		case DONE:
			break;
		case ERROR:
			break;
		default:
			break;
	}

}

void strat_tourne_en_rond(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_TOURNE_EN_ROND,
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
	Uint16 nbTour = 2;

	switch(state){
	case IDLE:
		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=120 >> 8;
		msg.data[3]=120 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going_until_break(500,200,POS_DEPART,POS_POINT,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case POS_POINT:
		state = try_going_until_break(650,600,POS_POINT,TOUR,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;
	case TOUR:
		state = try_going_multipoint(tour,6,TOUR,FIN,ERROR,FORWARD,NO_AVOIDANCE, END_AT_LAST_POINT);
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

void strat_lannion(void){
}
#if 0
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_TEST_POINT,
		IDLE,
		POS_DEPART,
		POINT_A1, // Point arbre1A cote couloir
		POINT_B3, // Point arbre1B rouge cote armoire
		POINT_W0,
		POINT_C3,
		POINT_W3,
		POINT_CO, // Pour deposser fruit
		POINT_M0, // Pour deposer la fresque
		POINT_M0_BIS,
		POINT_Z1,
		RAMASSER_FRUIT_ARBRE1,
		RAMASSER_FRUIT_ARBRE2,
		RAMASSER_FRUIT_ARBRE1A,
		RAMASSER_FRUIT_ARBRE1B,
		DEPOSER_FRUIT,
		DEPOSER_FRESQUE,
		DEPOSER_FRUIT_ROUGE,
		DEPOSER_FRUIT_JAUNE,
		LANCE_LAUNCHER,
		LANCE_LAUNCHER_ENNEMY,
		POS_FIN,
		DONE,
		ERROR
	);


	CAN_msg_t msg;
	static color_e colorIsRed;

	switch(state){
	case IDLE:

		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=COLOR_Y(120) >> 8;
		msg.data[3]=COLOR_Y(120) & 0xFF;

		if(global.env.color == RED){
			colorIsRed = TRUE;
			msg.data[4]=PI4096/2 >> 8;
			msg.data[5]=PI4096/2 & 0xFF;

		}else{
			colorIsRed = FALSE;
			msg.data[4]=-PI4096/2 >> 8;
			msg.data[5]=-PI4096/2 & 0xFF;
		}

		msg.size = 6;
		CAN_send(&msg);


	//		debug_printf("Envoie Message ACtionneur\n");
	//		ACT_lance_launcher_run(ACT_Lance_ALL);

		state = POS_DEPART;

		break;
	case POS_DEPART:
		state = try_going(500,COLOR_Y(250),POS_DEPART,LANCE_LAUNCHER,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;


	case LANCE_LAUNCHER:
		state = check_sub_action_result(sub_lance_launcher(FALSE,global.env.color),LANCE_LAUNCHER,POINT_M0,ERROR);
		break;


	case POINT_M0:
		state = PATHFIND_try_going(M0, POINT_M0, DEPOSER_FRESQUE, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case DEPOSER_FRESQUE:
		state = check_sub_action_result(strat_manage_fresco(),DEPOSER_FRESQUE,LANCE_LAUNCHER_ENNEMY,ERROR);
		break;
	case LANCE_LAUNCHER_ENNEMY:
		state = check_sub_action_result(sub_lance_launcher(FALSE, (global.env.color==RED)?YELLOW:RED),LANCE_LAUNCHER_ENNEMY,POINT_Z1,ERROR);
		break;


		//Fruit sens rouge vers jaune
	//	case POINT_A1:
	//		state = PATHFIND_try_going(A1, POINT_A1, RAMASSER_FRUIT_ARBRE1, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
	//		break;
	//	case RAMASSER_FRUIT_ARBRE1:
	//		state = check_sub_action_result(strat_test_ramasser_fruit_arbre1_double(TRUE),RAMASSER_FRUIT_ARBRE1,RAMASSER_FRUIT_ARBRE2,ERROR);
	//		break;
	//	case POINT_W3:
	//		state = PATHFIND_try_going(W3, POINT_W3, RAMASSER_FRUIT_ARBRE2, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
	//		break;
	//	case RAMASSER_FRUIT_ARBRE2:
	//		state = check_sub_action_result(strat_test_ramasser_fruit_arbre2_double(FALSE),RAMASSER_FRUIT_ARBRE2,POINT_M0_BIS,ERROR);
	//		break;
	//	case POINT_M0_BIS:
	//		state = PATHFIND_try_going(M0, POINT_M0_BIS, DEPOSER_FRUIT, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
	//		break;
	//	case DEPOSER_FRUIT:
	//		state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,DONE,ERROR);
	//		break;
	//	case POS_FIN:
	//		state = try_going(500,300,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
	//		break;



		//Fruit jaune vers rouge
	case POINT_Z1:
		state = PATHFIND_try_going(Z1, POINT_Z1, RAMASSER_FRUIT_ARBRE2, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case RAMASSER_FRUIT_ARBRE2:
		state = check_sub_action_result(strat_ramasser_fruit_arbre2_double(colorIsRed),RAMASSER_FRUIT_ARBRE2,POINT_C3,ERROR);
		break;
	case POINT_C3:
		state = PATHFIND_try_going(C3, POINT_C3, RAMASSER_FRUIT_ARBRE1, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case RAMASSER_FRUIT_ARBRE1:
		state = check_sub_action_result(strat_ramasser_fruit_arbre1_double(colorIsRed),RAMASSER_FRUIT_ARBRE1,POINT_M0_BIS,ERROR);
		break;
	case POINT_M0_BIS:
		state = PATHFIND_try_going(M0, POINT_M0_BIS, DEPOSER_FRUIT, ERROR, ANY_WAY, SLOW, NO_AVOIDANCE, END_AT_BREAK);
		break;
	case DEPOSER_FRUIT:
		state = check_sub_action_result(strat_file_fruit(),DEPOSER_FRUIT,DONE,ERROR);
		break;
	case POS_FIN:
		state = try_going(500,300,POS_FIN,DONE,ERROR,SLOW,FORWARD,NO_AVOIDANCE);
		break;


	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}
}
#endif


void strat_test_fresque(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_TEST_FRESQUE,
		IDLE,
		POS_DEPART,
		POINT_C1,
		POINT_M0,
		DEPOSER_FRESQUE,
		DONE,
		ERROR
	);


	CAN_msg_t msg;

	switch(state){
	case IDLE:
		msg.sid=ASSER_SET_POSITION;
		msg.data[0]=500 >> 8;
		msg.data[1]=500 & 0xFF;
		msg.data[2]=120 >> 8;
		msg.data[3]=120 & 0xFF;
		msg.data[4]=PI4096/2 >> 8;
		msg.data[5]=PI4096/2 & 0xFF;
		msg.size = 6;
		CAN_send(&msg);

		state = POS_DEPART;
		break;
	case POS_DEPART:
		state = try_going_until_break(500,250,POS_DEPART,POINT_C1,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_C1:
		state = try_going_until_break(800,1000,POINT_C1,POINT_M0,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case POINT_M0:
		state = try_going(500,1500,POINT_M0,DEPOSER_FRESQUE,ERROR,FAST,FORWARD,NO_AVOIDANCE);
		break;
	case DEPOSER_FRESQUE:
		state = check_sub_action_result(strat_manage_fresco(),DEPOSER_FRESQUE,DONE,ERROR);
		break;
	case DONE:
		break;
	case ERROR:
		break;
	default:
		break;
	}
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
		state = IDLE;		//On force l'�tat IDLE si les feux � prot�ger ne sont plus les m�mes...
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


error_e protect_north_way(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PROTECT_NORTH_WAY,
		IDLE,
		GET_IN,
		POINT_0,
		POINT_1,
		NODE_0,
		NODE_1,
		NOT_REACHED,
		DONE,
		ERROR
	);
	static enum state_e next_point_protected = IDLE;
	static way_e way;
	static GEOMETRY_point_t points[2];
	static pathfind_node_id_t nodes[2];
	switch(state){
		case IDLE:

			if(global.env.color == RED)
			{
				nodes[0] = C1;   // A2
				nodes[1] = W1;	// B3
				points[0] = (GEOMETRY_point_t){700,1200};
				points[1] = (GEOMETRY_point_t){700,1800};
			}
			else
			{
				nodes[0] = W1;  // Z2
				nodes[1] = C1;	// Y3
				points[0] = (GEOMETRY_point_t){700,1800};
				points[1] = (GEOMETRY_point_t){700,1200};
			}
			next_point_protected = NODE_0;
			state = GET_IN;

		   break;

	   case GET_IN:

		   if(est_dans_carre(points[0].x - 200, points[0].x + 200, points[0].y - 150, points[0].y + 150,(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))	//Je suis proche du POINT_0
			   state = POINT_1;
		   else if (est_dans_carre(points[1].x - 200, points[1].x + 200, points[1].y - 150, points[1].y + 150,(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))//Proche du POINT_1
			   state = POINT_0;
		   else	//Je suis loin -> PATHFIND
		   {
			   if(next_point_protected == NODE_0)
				   state = NODE_0;
			   else
				   state = NODE_1;
		   }
		   break;

	   case NODE_0:
		   state = PATHFIND_try_going(nodes[0], NODE_0, DONE,ERROR,ANY_WAY,FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
		   break;
	   case NODE_1:
		   state = PATHFIND_try_going(nodes[1], NODE_1, DONE,ERROR,ANY_WAY,FAST, DODGE_AND_WAIT, END_AT_LAST_POINT);
		   break;
	   case POINT_0:
		   if(entrance)
			   way = (points[0].y < points[1].y)?BACKWARD:FORWARD; //Si le point 0 est du cot� rouge, on s'y rend en reculant (filet ET angle mort du cot� des fresques)
		   state = try_going(points[0].x, points[0].y,state,DONE,ERROR,FAST,way,NO_DODGE_AND_WAIT);
		   break;
	   case POINT_1:
		   if(entrance)
			   way = (points[1].y < points[0].y)?BACKWARD:FORWARD; //Si le point 1 est du cot� rouge, on s'y rend en reculant (filet ET angle mort du cot� des fresques)
		   state = try_going(points[1].x, points[1].y,state,DONE,ERROR,FAST,way,NO_DODGE_AND_WAIT);
		   break;
	   case DONE:
		   state = GET_IN;	//ATTENTION, on ne retourne pas vers IDLE, mais vers le prochain point, pour le prochain appel !
		   return END_OK;
		   break;
	   case ERROR:
		   state = GET_IN;	//ATTENTION, on ne retourne pas vers IDLE, mais vers le prochain point, pour le prochain appel !
		   next_point_protected = (next_point_protected == NODE_0)?NODE_1:NODE_0;	//On vise l'autre point...
		   return NOT_HANDLED;
		   break;
	   default:
		   break;
	}

	return IN_PROGRESS;
}


error_e strat_manage_fresco(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_MANAGE_FRESCO,
		IDLE,
		GET_IN,
		CHOICE_GET_IN,
		NEAR_FRESCO,
		FILE_FRESCO,
		VERIFICATION,
		LAST_CHANCE_FILE_FRESCO,
		VERIFICATION_2,
		LAST_LAST_CHANCE_FILE_FRESCO,
		DONE,
		ERROR
	);

	static Sint16 posY = 1500;
	static Sint16 oldPosY = 1500; // Si les deux premiere pose ne fonctionne pas nous aurons besoins de lui
	static Uint8 statePosFresco = 0;

	switch(state){

		case IDLE:
			if(action_fresco_filed){
				debug_printf("ERREUR, la fresque a d�ja �t� pos�\n");
				return END_OK;
			}

			if(statePosFresco == LAST_CHANCE_FILE_FRESCO){ // Si il a deja tenter une fois la fresque
				oldPosY = posY;

				if(posY > 1500) //Ne prend plus en compte les positions des adversaires eu precedement (Elles sont fausses sinon ne serait pas dans cet �tat)
					posY = POS_MIN_FRESCO;
				else
					posY = POS_MAX_FRESCO;
			}else if(statePosFresco == LAST_LAST_CHANCE_FILE_FRESCO){ // Si il a deja tent� 2 fois la fresque
				if(posY == POS_MAX_FRESCO && oldPosY != POS_MIN_FRESCO)
					posY = POS_MIN_FRESCO;
				else if(posY == POS_MIN_FRESCO && oldPosY != POS_MAX_FRESCO)
					posY = POS_MAX_FRESCO;
				else
					posY = 1500;
			}else if(MODE_MANUAL_FRESCO) // A partir n as jamais poser la fresque
				posY = POS_Y_MANUAL_FRESCO;
			else if(ADVERSARY_DETECTED_HOKUYO == FALSE)//Pose les fresques au milieu si on a pas vu l'adversaire poser les siennes
				posY = 1500;
			else	//ADVERSARY_DETECTED...
			{
				switch(adversary_fresco_index)
				{
					case 1:
						if(adversary_fresco_positions[0] > POS_MAX_FRESCO && adversary_fresco_positions[0] < POS_MIN_FRESCO)// La valeur n est pas comprise
							posY = 1500;
						else if(adversary_fresco_positions[0] > 1500)
							posY = POS_MIN_FRESCO;
						else
							posY = POS_MAX_FRESCO;
						break;
					case 2:
						if((adversary_fresco_positions[0] > POS_MAX_FRESCO && adversary_fresco_positions[0] < POS_MIN_FRESCO) || (adversary_fresco_positions[1] > POS_MAX_FRESCO && adversary_fresco_positions[1] < POS_MIN_FRESCO))
							posY = 1500;
						else if(adversary_fresco_positions[0] > 1500 && adversary_fresco_positions[1] > 1500)// Les 2 poses ennemis sont sup au milieu de la fresque
							posY = POS_MIN_FRESCO;
						else if(adversary_fresco_positions[0] < 1500 && adversary_fresco_positions[1] < 1500) // sont inf au milieu
							posY = POS_MAX_FRESCO;
						else{    // Il y a une fresque de chaque cot� par rapport au milieu
							Uint16 fresco_inf,fresco_sup; //Correspond a la plus grande et plus petite variable ou l'adversaire aurait poser ses fresques

							if(adversary_fresco_positions[0] < adversary_fresco_positions[1]){
								fresco_inf = adversary_fresco_positions[0];
								fresco_sup = adversary_fresco_positions[1];
							}else{
								fresco_inf = adversary_fresco_positions[1];
								fresco_sup = adversary_fresco_positions[0];
							}

							if(POS_MAX_FRESCO-fresco_sup > fresco_inf-POS_MIN_FRESCO){// La zone la plus grande est entre le POS_MAX_FRECO et le point sup de la pose
								if(POS_MAX_FRESCO-fresco_sup >= fresco_sup-fresco_inf)// Testons si la zone entre les deux fresques n'est pas plus grande
									posY = POS_MAX_FRESCO;
								else
									posY = (fresco_inf+fresco_sup)/2;

							}else{ //Si la zone entre la POS_MIN_FRESCO est plus grande que celle avec POS_MAX_FRESCO
								if(fresco_inf-POS_MIN_FRESCO >= fresco_sup-fresco_inf)
									posY = POS_MIN_FRESCO;
								else
									posY = (fresco_inf+fresco_sup)/2;
							}
						}

						break;
					default: // On ne sait jamais si aucun adversaire a �t� detecte
						posY = 1500;
						break;
				}
			}

			state = CHOICE_GET_IN;
			break;
		case CHOICE_GET_IN:
			if(est_dans_carre(0, 700, 1150, 1850, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = FILE_FRESCO;
			else if(est_dans_carre(500 , 700, 900, 2100, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = NEAR_FRESCO;
			else
				state = GET_IN;
			break;

		case GET_IN :
			state = PATHFIND_try_going(M1, GET_IN, FILE_FRESCO, ERROR, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case NEAR_FRESCO :
			//DODGE acceptable, car plutot int�ressant ici.. mais il y a un risque si l'odom�trie n'est pas bonne de taper un coin dans le bac (si le point d'extraction est malchanceux)
			state = try_going(500,1500,NEAR_FRESCO,FILE_FRESCO,ERROR,FAST,ANY_WAY,DODGE_AND_WAIT);
			break;

		case FILE_FRESCO:
			state = check_sub_action_result(strat_file_fresco(posY),FILE_FRESCO,VERIFICATION,ERROR);
			break;

		case VERIFICATION: // Si fresque pr�sente renvoie 0 donc try again
			if((get_fresco(1) && get_fresco(2) && get_fresco(3)) || NB_TRY_FRESCO < 2)//fresque plus presente sur le support grace au capteur
				state = DONE;
			else
				state = LAST_CHANCE_FILE_FRESCO;

			break;

		case LAST_CHANCE_FILE_FRESCO:
				if(entrance){
					oldPosY = posY;

					if(posY > 1500) //Ne prend plus en compte les positions des adversaires eu precedement (Elles sont fausses sinon ne serait pas dans cet �tat)
						posY = POS_MIN_FRESCO;
					else if (posY < 1500)
						posY = POS_MAX_FRESCO;
					else	// == 1500	//Si la premi�re tentative �tait en 1500, la seconde est du cot� de notre zone de d�part.
						posY = (global.env.color == RED)?POS_MIN_FRESCO:POS_MAX_FRESCO;
				}

				state = check_sub_action_result(strat_file_fresco(posY),LAST_CHANCE_FILE_FRESCO,VERIFICATION_2,ERROR);
			break;

		case VERIFICATION_2:
			if((get_fresco(1) && get_fresco(2) && get_fresco(3)) || NB_TRY_FRESCO < 3)//fresque plus presente sur le support grace au capteur
				state = DONE;
			else
				state = LAST_LAST_CHANCE_FILE_FRESCO;

			break;

		case LAST_LAST_CHANCE_FILE_FRESCO:
				if(entrance){
					if(posY == POS_MAX_FRESCO && oldPosY != POS_MIN_FRESCO)
						posY = POS_MIN_FRESCO;
					else if(posY == POS_MIN_FRESCO && oldPosY != POS_MAX_FRESCO)
						posY = POS_MAX_FRESCO;
					else
						posY = 1500;
				}

				state = check_sub_action_result(strat_file_fresco(posY),LAST_LAST_CHANCE_FILE_FRESCO,DONE,ERROR);
			break;

		case DONE:
			state = IDLE;
			action_fresco_filed = TRUE;
			set_sub_act_done(SUB_FRESCO,TRUE);
			return END_OK;
			break;

		case ERROR:
			if(last_state == LAST_CHANCE_FILE_FRESCO) // Refait au prochain tour si il y a une erreur
				statePosFresco = LAST_LAST_CHANCE_FILE_FRESCO;
			else
				statePosFresco = LAST_CHANCE_FILE_FRESCO;

			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e strat_file_fresco(Sint16 posY){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_FILE_FRESCO,
		IDLE,
		WALL,
		BEFORE_ROTATION,
		ROTATION,
		BEFORE_WALL,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		END,
		END_IMPOSSIBLE,
		END_ERROR,
		END_IMPOSSIBLE_ERROR,
		GET_OUT_WITH_ERROR,
		GET_OUT_WITH_ERROR_RED_NORTH,
		GET_OUT_WITH_ERROR_RED_MIDDLE,
		GET_OUT_WITH_ERROR_RED_SOUTH,
		GET_OUT_WITH_ERROR_YELLOW_NORTH,
		GET_OUT_WITH_ERROR_YELLOW_MIDDLE,
		GET_OUT_WITH_ERROR_YELLOW_SOUTH,
		DONE,
		ERROR,
		DONE_BUT_NOT_HANDLED
	);
	static enum state_e from;
	static bool_e timeout=FALSE;

	switch(state){
		case IDLE:
			if(posY > POS_MAX_FRESCO || posY < POS_MIN_FRESCO){
				debug_printf("ERREUR Mauvaise position envoyer en fresque\n");
				return NOT_HANDLED;
			}

			timeout=FALSE;

			state = BEFORE_WALL;
			break;

//		case BEFORE_ROTATION:
//			state = try_going(300,posY,BEFORE_ROTATION,ROTATION,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
//			break;

//		case ROTATION:
//			state = try_go_angle(PI4096,ROTATION,WALL,END_ERROR,FAST);
//			break;

		case BEFORE_WALL:
			state = try_going(270,posY,BEFORE_WALL,WALL,END_ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case WALL:
			state = try_going(220,posY,WALL,PUSH_MOVE,END_ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT); // Si en erreur on va a END seule veut dire qu un triangle a g�n�, la strat_manage fresco va donc choisir une autre position
			break;

		case PUSH_MOVE://Le fait forcer contre le mur pour poser la fresque
			ASSER_push_rush_in_the_wall(BACKWARD,TRUE,0,TRUE);//Le fait forcer en marche arriere
			state = WAIT_END_OF_MOVE;
			break;

		case WAIT_END_OF_MOVE:
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				state = END;
			}
			break;

		case END:
			state = try_going_until_break(400,posY,END,DONE,END_IMPOSSIBLE,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case END_IMPOSSIBLE:
			if(global.env.pos.x > 200)
				state = ERROR;
			else
				state = END;
			break;

		case END_ERROR: //Pour le faire forcer � partir au droit
			state = try_going_until_break(400,posY,END_ERROR,ERROR,END_IMPOSSIBLE_ERROR,FAST,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case END_IMPOSSIBLE_ERROR:
			if(global.env.pos.x > 200)
				state = ERROR;
			else
				state = END_ERROR;
			break;

		case GET_OUT_WITH_ERROR :
			//On recherche le point de sortie le plus proche.
			if(global.env.pos.y < 1500)
			{
				if(global.env.pos.x < 300)
				{
					state = GET_OUT_WITH_ERROR_RED_MIDDLE;
					from  = GET_OUT_WITH_ERROR_RED_NORTH;
				}
				else if(global.env.pos.x < 600)
				{
					state = GET_OUT_WITH_ERROR_RED_SOUTH;
					from  = GET_OUT_WITH_ERROR_RED_MIDDLE;
				}
				else
				{
					state = DONE_BUT_NOT_HANDLED;	//Je rend la main
				}
			}
			else
			{
				if(global.env.pos.x < 300)
				{
					state = GET_OUT_WITH_ERROR_YELLOW_MIDDLE;
					from  = GET_OUT_WITH_ERROR_YELLOW_NORTH;
				}
				else if(global.env.pos.x < 600)
				{
					state = GET_OUT_WITH_ERROR_YELLOW_SOUTH;
					from  = GET_OUT_WITH_ERROR_YELLOW_MIDDLE;
				}
				else
				{
					state = DONE_BUT_NOT_HANDLED;	//Je rend la main
				}
			}
			break;


			case GET_OUT_WITH_ERROR_RED_SOUTH:		//Cette position permet de sortir et de rendre la main !
				state = try_going(700,1350,GET_OUT_WITH_ERROR_RED_SOUTH,DONE_BUT_NOT_HANDLED,(from == GET_OUT_WITH_ERROR_RED_MIDDLE)? GET_OUT_WITH_ERROR_RED_MIDDLE:GET_OUT_WITH_ERROR_YELLOW_SOUTH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
				if(state != GET_OUT_WITH_ERROR_RED_SOUTH)
					from = GET_OUT_WITH_ERROR_RED_SOUTH;
				break;
			case GET_OUT_WITH_ERROR_RED_MIDDLE:
				state = try_going(500,1350,GET_OUT_WITH_ERROR_RED_MIDDLE,(from == GET_OUT_WITH_ERROR_RED_SOUTH)?GET_OUT_WITH_ERROR_RED_NORTH:GET_OUT_WITH_ERROR_RED_SOUTH, (from == GET_OUT_WITH_ERROR_RED_SOUTH)? GET_OUT_WITH_ERROR_RED_SOUTH:GET_OUT_WITH_ERROR_RED_NORTH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
				if(state != GET_OUT_WITH_ERROR_RED_MIDDLE)
					from = GET_OUT_WITH_ERROR_RED_MIDDLE;
				break;
			case GET_OUT_WITH_ERROR_RED_NORTH:
				state = try_going(250,1350,GET_OUT_WITH_ERROR_RED_NORTH,(from == GET_OUT_WITH_ERROR_RED_MIDDLE)?GET_OUT_WITH_ERROR_YELLOW_NORTH:GET_OUT_WITH_ERROR_RED_MIDDLE,(from == GET_OUT_WITH_ERROR_RED_MIDDLE)? GET_OUT_WITH_ERROR_RED_MIDDLE:GET_OUT_WITH_ERROR_YELLOW_NORTH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
				if(state != GET_OUT_WITH_ERROR_RED_NORTH)
					from = GET_OUT_WITH_ERROR_RED_NORTH;
				break;
			case GET_OUT_WITH_ERROR_YELLOW_NORTH:
				state = try_going(250,1650,GET_OUT_WITH_ERROR_YELLOW_NORTH,(from == GET_OUT_WITH_ERROR_YELLOW_MIDDLE)?GET_OUT_WITH_ERROR_RED_NORTH:GET_OUT_WITH_ERROR_YELLOW_MIDDLE,(from == GET_OUT_WITH_ERROR_YELLOW_MIDDLE)? GET_OUT_WITH_ERROR_YELLOW_MIDDLE:GET_OUT_WITH_ERROR_RED_NORTH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
				if(state != GET_OUT_WITH_ERROR_YELLOW_NORTH)
					from = GET_OUT_WITH_ERROR_YELLOW_NORTH;
				break;
			case GET_OUT_WITH_ERROR_YELLOW_MIDDLE:
				state = try_going(500,1650,GET_OUT_WITH_ERROR_YELLOW_MIDDLE,(from == GET_OUT_WITH_ERROR_YELLOW_SOUTH)?GET_OUT_WITH_ERROR_YELLOW_NORTH:GET_OUT_WITH_ERROR_YELLOW_SOUTH, (from == GET_OUT_WITH_ERROR_YELLOW_SOUTH)? GET_OUT_WITH_ERROR_YELLOW_SOUTH:GET_OUT_WITH_ERROR_YELLOW_NORTH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
				if(state != GET_OUT_WITH_ERROR_YELLOW_MIDDLE)
					from = GET_OUT_WITH_ERROR_YELLOW_MIDDLE;
				break;
			case GET_OUT_WITH_ERROR_YELLOW_SOUTH:		//Cette position permet de sortir et de rendre la main !
				state = try_going(700,1650,GET_OUT_WITH_ERROR_YELLOW_SOUTH,DONE_BUT_NOT_HANDLED,(from == GET_OUT_WITH_ERROR_YELLOW_MIDDLE)? GET_OUT_WITH_ERROR_YELLOW_MIDDLE:GET_OUT_WITH_ERROR_RED_SOUTH,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
				if(state != GET_OUT_WITH_ERROR_YELLOW_SOUTH)
					from = GET_OUT_WITH_ERROR_YELLOW_SOUTH;
				break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case DONE_BUT_NOT_HANDLED :
			state = IDLE;
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}




error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x){
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
			ASSER_push_rush_in_the_wall(sens, TRUE, angle, TRUE);
			state = WAIT;
			break;

		case WAIT :
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){

				offset_recalage.x = global.env.pos.x - wanted_x;
				debug_printf("offset_recalage.x = %d\n", offset_recalage.x);
				state = GET_OUT;
			}
			break;

		case GET_OUT :
			if(global.env.pos.x > 1000)
				state = try_going(wanted_x - 50, global.env.pos.y, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			else
				state = try_going(wanted_x + 200, global.env.pos.y, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[0].x,escape_point[0].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,WAIT_FOR_EXIT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de r�essayer p�riodiquement.
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
				offset_recalage.y = global.env.pos.y - wanted_y;
				debug_printf("offset_recalage.y = %d\n", offset_recalage.y);
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
		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de r�essayer p�riodiquement.
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

#define RELACAGE_Y 520
error_e recalage_begin_zone(color_e begin_zone_color){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_RECALAGE_BEGIN_ZONE,
		IDLE,
		GET_IN,
		PLACEMENT_RECALAGE_X,
		RECALAGE_X,
		PLACEMENT_RECALAGE_Y,
		RECALAGE_Y,
		GET_OUT,
		WAIT_FOR_EXIT,
		GET_OUT_WITH_ERROR,
		DONE,
		ERROR,
		ERROR_WITH_GET_OUT
	);

	static GEOMETRY_point_t escape_point[1];
	static time32_t local_time;

	switch(state){
		case IDLE :
			if(begin_zone_color == RED)
				escape_point[0] = (GEOMETRY_point_t){200, 200};
			else
				escape_point[0] = (GEOMETRY_point_t){200, 1800};
			state = GET_IN;
			break;

		case GET_IN :
			if(begin_zone_color == RED)
				state = PATHFIND_try_going(A1, GET_IN, PLACEMENT_RECALAGE_Y, ERROR_WITH_GET_OUT, ANY_WAY, FAST , DODGE_AND_WAIT, END_AT_BREAK);
			else
				state = PATHFIND_try_going(Z1, GET_IN, PLACEMENT_RECALAGE_Y, ERROR_WITH_GET_OUT, ANY_WAY, FAST , DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case PLACEMENT_RECALAGE_Y :
			if(begin_zone_color == RED)
				state = try_going(RELACAGE_Y, 200, PLACEMENT_RECALAGE_Y, RECALAGE_Y, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			else
				state = try_going(RELACAGE_Y, 2800, PLACEMENT_RECALAGE_Y, RECALAGE_Y, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case RECALAGE_Y :
			if(begin_zone_color == RED)
				state = check_sub_action_result(action_recalage_y(BACKWARD, PI4096/2, LARGEUR_ROBOT_BACK), RECALAGE_Y, PLACEMENT_RECALAGE_X, ERROR);
			else
				state = check_sub_action_result(action_recalage_y(BACKWARD, -PI4096/2, 3000-LARGEUR_ROBOT_BACK), RECALAGE_Y, PLACEMENT_RECALAGE_X, ERROR);
			break;

		case PLACEMENT_RECALAGE_X :
			if(begin_zone_color == RED)
				state = try_going(200, 225, PLACEMENT_RECALAGE_X, RECALAGE_X, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			else
				state = try_going(200, 2775, PLACEMENT_RECALAGE_X, RECALAGE_X, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case RECALAGE_X :
			if(begin_zone_color == RED)
				state = check_sub_action_result(action_recalage_x(BACKWARD, 0, LARGEUR_ROBOT_BACK), RECALAGE_X, GET_OUT, ERROR);
			else
				state = check_sub_action_result(action_recalage_x(BACKWARD, 0, LARGEUR_ROBOT_BACK), RECALAGE_X, GET_OUT, ERROR);
			break;

		case GET_OUT :
			if(begin_zone_color == RED)
				state = try_going(600, 300, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			else
				state = try_going(600, 2700, GET_OUT, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case GET_OUT_WITH_ERROR :
			state = try_going_until_break(escape_point[0].x,escape_point[0].y,GET_OUT_WITH_ERROR,ERROR_WITH_GET_OUT,WAIT_FOR_EXIT,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case WAIT_FOR_EXIT:		//On a pas d'autre choix que d'attendre et de r�essayer p�riodiquement.
			if(entrance)
			{
				local_time = global.env.match_time;
			}
			if(global.env.match_time - local_time > 2000)
				state = GET_OUT_WITH_ERROR;
			break;
		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = GET_OUT_WITH_ERROR;
			break;

		case ERROR_WITH_GET_OUT :
			state = IDLE;
			return NOT_HANDLED;
			break;
	}
	return IN_PROGRESS;
}

void strat_test_vide(){

	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_TEST_VIDE,
		INIT,
		AVANCER,
		DONE
	);

	if(TIME_TO_NET < global.env.match_time)
		strat_placement_net();
	else
		switch (state){
			case INIT :
				state = AVANCER;
				break;

			case AVANCER :
				state = try_going(603, 2500, AVANCER, DONE, DONE, SLOW, ANY_WAY, NO_AVOIDANCE);
				break;

			case DONE :
				break;
		}
}

void strat_test_small_arm(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_PIERRE_TEST_SMALL_ARM,
		INIT,
		IDLE,
		MID,
		DEPLOYED,
		MID_END,
		IDLE_END,
		DONE
	);

	switch(state){
		case INIT :
			state = IDLE;
			break;

		case IDLE :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_IDLE);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = MID;
			break;

		case MID :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_MID);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = DEPLOYED;
			break;

		case DEPLOYED :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_DEPLOYED);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = MID_END;
			break;

		case MID_END :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_MID);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = IDLE_END;
			break;

		case IDLE_END :
			if(entrance)
				ACT_small_arm_goto(ACT_SMALL_ARM_IDLE);
			if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done)
				state = DONE;
			break;

		case DONE :
			break;
	}
}


static void REACH_POINT_C1_send_request() {
	CAN_msg_t msg;

	msg.sid = XBEE_REACH_POINT_C1;
	msg.size = 0;

	CANMsgToXbee(&msg,FALSE);
}

void strat_test_evitement(){
	CREATE_MAE_WITH_VERBOSE(0,
							INIT,
							GO_1,
							GO_2,
							//GO_3,
							DONE
						);
	switch(state){
		case INIT :
			ASSER_set_position(500, 500, 0);
			state = GO_1;
			break;

		case GO_1 :
			state = try_going(2500, 500, GO_1, GO_2, GO_1, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case GO_2 :
			state = try_going(500, 500, GO_2, GO_1, GO_2, SLOW, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		case DONE :
			break;
	}
}

error_e ACT_arm_deploy_torche_pierre(torch_choice_e choiceTorch, torch_dispose_zone_e dispose_zone){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_PIERRE_DEPLOY_TORCH,
		IDLE,
		SCAN,
		TORCHE,
		DOWN_ARM,
		UP_ARM,
		DROP_TRIANGLE,
		WAIT_TRIANGLE_BREAK,
		PREPARE_SMALL_ARM,
		DOWN_SMALL_ARM,
		UP_SMALL_ARM,
		PREPARE_RETURN,
		RETURN,
		WAIT_RETURN,
		REPLACE_SMALL_ARM,
		OPEN_ARM_FAIL,
		FAIL,
		PARKED_NOT_HANDLED,
		ERROR,
		DONE
	);

	static GEOMETRY_point_t torch;

	static GEOMETRY_point_t drop_pos[3];
	static Uint8 niveau;
	Sint16 value_adc;

	if(dispose_zone == HEARTH_OUR){
		drop_pos[0] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};
		drop_pos[1] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};
		drop_pos[2] = (GEOMETRY_point_t){global.env.pos.x+130,global.env.pos.y+0};



		/*point[0] = (GEOMETRY_point_t){1800,COLOR_Y(200)};
		point[1] = (GEOMETRY_point_t){1900,COLOR_Y(250)};
		point[2] = (GEOMETRY_point_t){1900,COLOR_Y(250)};*/
	}

	switch(state){
		case IDLE :

			torch = TORCH_get_position(choiceTorch);

			// Impos� pour les test
			torch.x = global.env.pos.x;
			torch.y = global.env.pos.y+150;

			state = TORCHE;
			niveau = 0;
			break;

		case SCAN :
			value_adc = CONVERT_LASERS_VALUE_TO_DIST(ADC_getValue(ADC_14));

			if(value_adc < 135 && value_adc > 120)
				niveau = 2;
			else if(value_adc < 105 && value_adc > 90)
				niveau = 1;
			else if(value_adc < 75 && value_adc > 60)
				niveau = 0;
			else
				state = DONE;

			if((niveau == 1 && choiceTorch == OUR_TORCH) || ((niveau == 0 || niveau == 2) && choiceTorch == ADVERSARY_TORCH)) // Va retourne le deuxieme triangle
				state = PREPARE_SMALL_ARM;
			else
				state = TORCHE;

			break;

		case TORCHE :
			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,torch.x, torch.y, TORCHE, DOWN_ARM, OPEN_ARM_FAIL);
			break;

		case DOWN_ARM:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_elevator_arm_rush_in_the_floor(120-niveau*30, DOWN_ARM, UP_ARM, OPEN_ARM_FAIL);
			break;

		case UP_ARM:
			state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM, DROP_TRIANGLE, OPEN_ARM_FAIL);
			break;

		case DROP_TRIANGLE :
			state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE,drop_pos[niveau].x, drop_pos[niveau].y, DROP_TRIANGLE, WAIT_TRIANGLE_BREAK, OPEN_ARM_FAIL);
			break;

		case WAIT_TRIANGLE_BREAK : // Attendre que le triangle soit relacher avant de faire autre chose
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(500, WAIT_TRIANGLE_BREAK, (niveau>=2)? DONE:TORCHE);

			if(ON_LEAVING(WAIT_TRIANGLE_BREAK)){
				ACT_pompe_order(ACT_POMPE_STOP, 0);
			}
			break;


		case PREPARE_SMALL_ARM:
			state = ACT_arm_move(ACT_ARM_POS_TO_TAKE_RETURN,0, 0, PREPARE_SMALL_ARM, DOWN_SMALL_ARM, OPEN_ARM_FAIL);
			break;

		case DOWN_SMALL_ARM:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_elevator_arm_rush_in_the_floor(120-niveau*30, DOWN_SMALL_ARM, UP_SMALL_ARM, OPEN_ARM_FAIL);
			break;

		case UP_SMALL_ARM:
			state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_SMALL_ARM, PREPARE_RETURN, OPEN_ARM_FAIL);
			break;

		case PREPARE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, PREPARE_RETURN, RETURN, OPEN_ARM_FAIL);
			break;

		case RETURN:
			state = ACT_small_arm_move(ACT_SMALL_ARM_DEPLOYED, RETURN, WAIT_RETURN, OPEN_ARM_FAIL);
			break;

		case WAIT_RETURN:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(500, WAIT_RETURN, REPLACE_SMALL_ARM);
			break;

		case REPLACE_SMALL_ARM:
			state = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, REPLACE_SMALL_ARM, DROP_TRIANGLE, OPEN_ARM_FAIL);
			break;

		case OPEN_ARM_FAIL:{
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

		case FAIL:

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
