/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP
 *
 *	Fichier : avoidance.c
 *	Package : Carte Principale
 *	Description : Actions relatives au déplacement avec evitement
 *	Auteur : Jacen
 *	Version 20110426
 */

#include "movement.h"
#include "prop_functions.h"
#include "../Supervision/Buzzer.h"
#include <math.h>
#include "../utils/actionChecker.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can.h"
#include "../utils/generic_functions.h"

#define LOG_PREFIX "move: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MOVEMENT
#include "../QS/QS_outputlog.h"
#include "../Supervision/SD/SD.h"

#define FOE_IS_LEFT_TIME			250		//[ms] temps depuis lequel l'adversaire doit être parti pour que l'on reprenne notre trajectoire.
#define EXTRACTION_DISTANCE			300

static error_e extraction_of_foe(PROP_speed_e speed);
static error_e goto_extract_with_avoidance(const displacement_t displacements);

//------------------------------------------------------------------- Machines à états de déplacements

//Action qui gere un déplacement et renvoi le state rentré en arg. Ne s'arrète qu'à la fin que si aucun autre déplacement n'est demandé.
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition)
{
	error_e sub_action;
	sub_action = goto_pos_curve_with_avoidance((displacement_t[]){{{x, y},speed}}, NULL, 1, way, avoidance, end_condition, PROP_NO_BORDER_MODE);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;
		case FOE_IN_PATH:
		case NOT_HANDLED:
		case END_WITH_TIMEOUT:
			//releaseZones(list);
			return fail_state;
			break;
		case END_OK:
		default:
			//releaseZones(list);
			return success_state;
			break;
	}
	return in_progress;
}

//Action qui gere un déplacement et renvoi le state rentré en arg. Cette fonction permet le multipoint.
Uint8 try_going_multipoint(const displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition)
{
	error_e sub_action;
	sub_action = goto_pos_curve_with_avoidance(displacements, NULL, nb_displacements, way, avoidance, end_condition, PROP_NO_BORDER_MODE);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;
		case FOE_IN_PATH:
		case NOT_HANDLED:
		case END_WITH_TIMEOUT:
			return fail_state;
			break;
		case END_OK:
		default:
			return success_state;
			break;
	}
	return in_progress;
}

Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, STRAT_endCondition_e endCondition){
	CREATE_MAE(
			GO,
			WAIT
		);

	static time32_t begin_time;
	static Uint8 idTraj;

	Uint8 ret = in_progress;

	switch(state)
	{
		case GO:
			idTraj = PROP_getNextIdTraj();
			begin_time = global.absolute_time;
			PROP_goAngle(angle, PROP_ABSOLUTE, speed, way, FALSE, PROP_END_AT_POINT, idTraj);
			state = WAIT;
			break;

		case WAIT:{
			error_e sub_action = wait_move_and_wait_detection(TRAJECTORY_ROTATION, 1, idTraj, endCondition, begin_time);

			switch(sub_action)
			{
				case END_OK:
					RESET_MAE();
					ret = success_state;
					break;

				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					RESET_MAE();
					ret = fail_state;
					break;

				case IN_PROGRESS:
					break;

				default:
					RESET_MAE();
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					ret = fail_state;
					break;
			}
		}break;
	}
	return ret;
}

// Fait avancer le robot vers un points jusqu'a ce qu'il rencontre un enemie ou un obstacle
Uint8 try_rush(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, bool_e force_rotate){
	CREATE_MAE(//_WITH_VERBOSE(SM_ID_TRY_RUSH,
			IDLE,
			FAST_ROTATE,
			INIT_COEF,
			GO,
			ERROR,
			DONE
		);

	error_e sub_action;
	static Sint16 angle;

	switch(state){
		case IDLE :
			if(force_rotate)
				state = FAST_ROTATE;
			else
				state = INIT_COEF;
			break;

		case FAST_ROTATE:
			if(entrance){
				angle = atan2(y-global.pos.y, x-global.pos.x)*4096;
				if(angle > 0)
					angle -= ((way == BACKWARD)? PI4096 : 0);
				else
					angle += ((way == BACKWARD)? PI4096 : 0);
			}
			state = try_go_angle(angle, FAST_ROTATE, INIT_COEF, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			break;

		case INIT_COEF:
			PROP_set_threshold_error_translation(50, FALSE);
			state = GO;
			break;

		case GO :
			sub_action = goto_pos_curve_with_avoidance((displacement_t[]){{{x, y},30}}, NULL, 1, way, avoidance, END_AT_LAST_POINT, PROP_BORDER_MODE);
			switch(sub_action){
				case IN_PROGRESS:
					state = GO;
					break;
				case FOE_IN_PATH:
					state = ERROR;
					break;
				case NOT_HANDLED:
				case END_OK:
				case END_WITH_TIMEOUT:
				default:
					state = DONE;
					break;
			}
			break;

		case ERROR :
			PROP_set_threshold_error_translation(0, TRUE);
			RESET_MAE();
			return fail_state;

		case DONE :
			PROP_set_threshold_error_translation(0, TRUE);
			RESET_MAE();
			return success_state;
	}
	return in_progress;
}

//Action qui gere un déplacement en avance ou arrière et renvoi le state rentré en arg.
Uint8 try_advance(GEOMETRY_point_t *point, bool_e compute, Uint16 dist, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition)
{
	static GEOMETRY_point_t compute_point;

	CREATE_MAE(
			COMPUTE,
			GO
		);

	Sint16 cos,sin;
	Uint8 return_state = in_progress;

	switch(state){
		case COMPUTE :
			COS_SIN_4096_get((way == FORWARD)? global.pos.angle:GEOMETRY_modulo_angle(global.pos.angle+PI4096), &cos, &sin);

			if(compute){

				compute_point.x = (Sint32)cos*dist/4096 + global.pos.x;
				compute_point.y = (Sint32)sin*dist/4096 + global.pos.y;

				if(point != NULL){
					point->x = compute_point.x;
					point->y = compute_point.y;
				}

			}else if(point != NULL){

				compute_point.x = point->x;
				compute_point.y = point->y;

			}else{			/// !!!!!!!!!!!!! Ne dois jamais ce produire !!!!!!!!!!!!!!!

				BUZZER_play(200, DEFAULT_NOTE, 3);
				error_printf("Attention compute du try_advance sans lui demander et sans lui donner de point !!! \n");
				compute_point.x = (Sint32)cos*dist/4096 + global.pos.x;
				compute_point.y = (Sint32)sin*dist/4096 + global.pos.y;

			}

			state = GO;
			break;

		case GO :
			return_state = try_going(compute_point.x, compute_point.y, in_progress, success_state, fail_state, speed, way, avoidance, end_condition);
			if(return_state != in_progress)
				state = COMPUTE;
			break;
	}
	return return_state;
}

//Action qui gere l'arret du robot et renvoi le state rentré en arg.
Uint8 try_stop(Uint8 in_progress, Uint8 success_state, Uint8 fail_state)
{
	CREATE_MAE(
		GO,
		WAIT
	);

	static time32_t begin_time;

	switch(state){
		case GO :
			begin_time = global.absolute_time;
			PROP_stop();
			state = WAIT;
			break;

		case WAIT :{
			error_e subaction = wait_move_and_wait_detection(TRAJECTORY_STOP, 1, 0, END_AT_LAST_POINT, begin_time);
			switch (subaction) {
				case END_OK:
					debug_printf("PROP_STOP effectué\n");
					RESET_MAE();
					return success_state;
					break;

				case IN_PROGRESS:
					break;

				case END_WITH_TIMEOUT:
					debug_printf("PROP_STOP effectué avec TIMEOUT\n");
					RESET_MAE();
					return fail_state;
					break;

				case NOT_HANDLED:
					debug_printf("PROP_STOP erreur\n");
					RESET_MAE();
					return fail_state;
					break;

				default:
					debug_printf("PROP_STOP erreur\n");
					RESET_MAE();
					return fail_state;
					break;
			}
			}break;
	}
	return in_progress;
}

Uint8 try_rushInTheWall(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, bool_e asser_rotate, Uint8 thresholdError, Uint8 acceleration){
	CREATE_MAE(
			INIT,
			PUSH_ORDER,
			WAIT,
			DONE,
			FAIL
		);

	static bool_e timeout;
	static time32_t begin_time;
	static Uint8 idTraj;

	Uint8 ret = in_progress;

	switch(state)
	{
		case INIT:
			if(thresholdError)
				PROP_set_threshold_error_translation(thresholdError, FALSE);

			if(acceleration)
				PROP_set_acceleration(acceleration);

			idTraj = PROP_getNextIdTraj();
			begin_time = global.absolute_time;
			timeout = FALSE;
			state = PUSH_ORDER;
			break;


		case PUSH_ORDER:
			PROP_rushInTheWall(way, asser_rotate, angle, 25, idTraj);
			state = WAIT;
			break;

		case WAIT:{
			error_e subaction = wait_move_and_wait_detection(TRAJECTORY_TRANSLATION, 1, idTraj, END_AT_LAST_POINT, begin_time);
			switch (subaction) {
				case END_OK:
				case NOT_HANDLED:
					debug_printf("try_rush_in_the_wall effectué\n");
					state = DONE;
					break;

				case IN_PROGRESS:
					break;

				case END_WITH_TIMEOUT:
					error_printf("try_rush_in_the_wall effectué avec TIMEOUT\n");
					timeout = TRUE;
					state = DONE;
					break;

				case FOE_IN_PATH:
					error_printf("try_rush_in_the_wall erreur adversaire dans le path (impossible)\n");
					state = FAIL;
					break;

				default:
					debug_printf("try_rush_in_the_wall erreur inconnue\n");
					state = FAIL;
					break;
			}
			}break;

		case DONE:
			if(thresholdError)
				PROP_set_threshold_error_translation(0, TRUE);

			if(acceleration)
				PROP_set_acceleration(0);

			RESET_MAE();
			ret = (timeout)? fail_state : success_state;
			break;

		case FAIL:
			if(thresholdError)
				PROP_set_threshold_error_translation(0, TRUE);

			if(acceleration)
				PROP_set_acceleration(0);

			RESET_MAE();
			ret = fail_state;
			break;

		default:
			RESET_MAE();
			ret = fail_state;
			break;
	}

	return ret;
}


//------------------------------------------------------------------- Fonctions relatives à l'évitement


/*
 * Fonction qui réalise un PROP_push_goto tout simple avec la gestion de l'évitement (en courbe)
 *
 * pre : Etre sur le terrain
 * post : Robot aux coordonnées voulues
 *
 * param displacements : deplacements de la trajectoire
 * param nb_displacement : nombre de deplacements de la trajectoire
 * param way : sens de déplacement
 * end_condition : doit on finir quand on freine sur le dernier point ou quand on y est ?
 *
 * return IN_PROGRESS : En cours
 * return END_OK : Terminé
 * return NOT_HANDLED : Action impossible
 * return END_WITH_TIMEOUT : Timeout
 * return FOE_IN_PATH : un adversaire nous bloque
 */
error_e goto_pos_curve_with_avoidance(const displacement_t displacements[], const displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, STRAT_endCondition_e end_condition, prop_border_mode_e border_mode)
{
	CREATE_MAE(
			CHECK,
			LOAD_MOVE,
			WAIT_MOVE_AND_SCAN_FOE,
			EXTRACT
		);

	static time32_t beginTime;
	static Uint8 idTraj;
	static error_e sub_action;

	switch(state)
	{
		case CHECK:
			if(nb_displacements == 0){
				RESET_MAE();
				return END_OK;
			}

			if(displacements == NULL && displacements_curve == NULL){
				RESET_MAE();
				return NOT_HANDLED;
			}
			state = LOAD_MOVE;
			break;

		case LOAD_MOVE:{
			global.prop.detected_foe = FALSE;

			if(displacements)
				global.destination = displacements[nb_displacements-1].point;
			else
				global.destination = displacements_curve[nb_displacements-1].point;

			Uint8 i;
			for(i=0; i<nb_displacements-1; i++)
			{
				if(displacements)
					PROP_goTo(displacements[i].point.x, displacements[i].point.y, PROP_ABSOLUTE, displacements[i].speed, way, TRUE, PROP_END_AT_BRAKE, PROP_CURVE, avoidance_type, border_mode, 0);
				else if(displacements_curve && displacements_curve[i].curve)
					PROP_goTo(displacements_curve[i].point.x, displacements_curve[i].point.y, PROP_ABSOLUTE, displacements_curve[i].speed, way, TRUE, PROP_END_AT_BRAKE, PROP_CURVE, avoidance_type, border_mode, 0);
				else
					PROP_goTo(displacements_curve[i].point.x, displacements_curve[i].point.y, PROP_ABSOLUTE, displacements_curve[i].speed, way, TRUE, PROP_END_AT_POINT, PROP_NO_CURVE, avoidance_type, border_mode, 0);
			}

			idTraj = PROP_getNextIdTraj();
			beginTime = global.absolute_time;

			if(displacements)
				PROP_goTo(displacements[nb_displacements-1].point.x, displacements[nb_displacements-1].point.y, PROP_ABSOLUTE, displacements[nb_displacements-1].speed, way, TRUE, PROP_END_AT_BRAKE, PROP_CURVE, avoidance_type, border_mode, idTraj);
			else if(displacements_curve && displacements_curve[nb_displacements-1].curve)
				PROP_goTo(displacements_curve[nb_displacements-1].point.x, displacements_curve[nb_displacements-1].point.y, PROP_ABSOLUTE, displacements_curve[nb_displacements-1].speed, way, TRUE, PROP_END_AT_BRAKE, PROP_CURVE, avoidance_type, border_mode, idTraj);
			else
				PROP_goTo(displacements_curve[nb_displacements-1].point.x, displacements_curve[nb_displacements-1].point.y, PROP_ABSOLUTE, displacements_curve[nb_displacements-1].speed, way, TRUE, PROP_END_AT_POINT, PROP_NO_CURVE, avoidance_type, border_mode, idTraj);

			debug_printf("goto_pos_with_scan_foe : load_move\n");

			state = WAIT_MOVE_AND_SCAN_FOE;
			}break;

		case WAIT_MOVE_AND_SCAN_FOE:
			sub_action = wait_move_and_wait_detection(TRAJECTORY_TRANSLATION, nb_displacements, idTraj, end_condition, beginTime);
			switch(sub_action)
			{
				case END_OK:
					RESET_MAE();
					debug_printf("wait_move_and_scan_foe -- fini\n");
					return END_OK;

				case END_WITH_TIMEOUT:
					RESET_MAE();
					debug_printf("wait_move_and_scan_foe -- timeout\n");
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return END_WITH_TIMEOUT;

				case NOT_HANDLED:
					RESET_MAE();
					info_printf("wait_move_and_scan_foe -- error\n");
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;

				case FOE_IN_PATH:
					info_printf("wait_move_and_scan_foe -- foe\n");
					if(avoidance_type == DODGE_AND_WAIT || avoidance_type == DODGE_AND_NO_WAIT)
						state = EXTRACT;
					else
					{
						RESET_MAE();
						global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
						return FOE_IN_PATH;
					}
					break;

				case IN_PROGRESS:
					break;

				default:
					RESET_MAE();
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
			}
			break;

		case EXTRACT:
			sub_action = extraction_of_foe(FAST);
			switch(sub_action)
			{
				case END_OK:
					RESET_MAE();
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return FOE_IN_PATH;

				case IN_PROGRESS:
					break;

				case NOT_HANDLED:
				default:
					RESET_MAE();
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
			}
			break;

		default:
			RESET_MAE();
			global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

/* ----------------------------------------------------------------------------- */
/* 		Fonctions de scrutation de la position de l'adversaire					 */
/* ----------------------------------------------------------------------------- */

/*	Trouve une extraction lorsqu'un ou plusieurs ennemi(s) qui nous pose(nt) problème */
static error_e extraction_of_foe(PROP_speed_e speed){
	CREATE_MAE_WITH_VERBOSE(SM_ID_EXTRACTION_OF_FOE,
		IDLE,
		COMPUTE,
		GO_POINT,
		TURN_TRIGO,
		TURN_HORAIRE,
		WAIT,
		DONE,
		ERROR,
		ERROR_FOE
	);
	static Uint8 remaining_try;
	static Uint8 sens = TURN_TRIGO;							//Si il arrive pas à trouver un point au bout de 3 coups tourne sur lui-même pour permettre à l'hokuyo de voir partout
	static Uint8 bestPoint;									//Indice du meilleur point dans le tableau
	Uint32 bestPoint_distance2_with_nearest_foe;		    //Distance entre le meilleur point et l'adversaire qui en est le plus proche
	Uint32 adversary_to_close_distance;						//Distance minimale exigée entre un adversaire et un point candidat
	static GEOMETRY_point_t pointEx[12];					//Incroyable mais VRAI, idée de GRAVOUILLE d'utliser un tableau au lieu de calcul d'angle
	Uint8 i,foe;
	Sint16 cos,sin;
	Uint32 distance2_between_point_and_foe;					//Distance au carré entre le point courant et l'adversaire courant
	Uint32 distance2_between_point_and_foe_min;				//Distance au carré entre le point courant l'adversaire le plus proche trouvé
	bool_e can_i_go_to_point = FALSE;
	bool_e i_can_turn_in_my_position = FALSE;

	switch(state){
		case IDLE:
			remaining_try = 3;
			state = COMPUTE;
			break;
		case COMPUTE:

			// Algo de recherche du meilleur point....
			// Parmi les 12 points candidats (tout les 30°, à EXTRACTION_DISTANCE [mm] de notre robot)
			//	On recherche le point le plus éloigné des adversaires.
			//	C'est à dire, le point ayant son adversaire le plus proche...... le plus loin de lui.
			//	Ainsi, si un seul adversaire est le plus proche de tout les points candidats (plutot probable)... Le meilleur point est celui diamétralement opposé à l'adversaire en question.
			//	Si par contre certains points sont plus proches d'un autre adversaire... ils peuvent ne pas être choisis pour cette raison
			//  Si aucun point n'est accessible (car trop proche bordure, élément fixe de jeu, ou adversaire)... L'algo échoue et patiente 1 seconde avant de retenter.
			// 	On tente 3 fois avant d'abandonner...
			remaining_try--;

			adversary_to_close_distance = (QS_WHO_AM_I_get() == BIG_ROBOT)? 500 : 400;
			i_can_turn_in_my_position = is_possible_point_for_rotation(&((GEOMETRY_point_t){global.pos.x,global.pos.y}));
			debug_printf("i_can_turn_in_my_position= %d\n", i_can_turn_in_my_position);
			debug_printf("Robot x=%d  y=%d teta=%d\n", global.pos.x, global.pos.y, global.pos.angle);
			debug_printf("foe x=%d  y=%d  en=%d\n", global.foe[0].x, global.foe[0].y, global.foe[0].enable);

			bestPoint = 0xFF;
			bestPoint_distance2_with_nearest_foe = 0;

			for(i = 0; i < 12; i++)	//Pour chaque point parmi les 12...
			{
				//Si je peux tourner là où je suis, ou que les points que je vais calculer sont pile devant ou pile derrière... à 30° près... alors je calcule le point i
				if(i_can_turn_in_my_position || i <= 1 || (i >=5 && i<=7) || i>=11)
				{
					//Calcul des coordonnées du point.
					COS_SIN_4096_get(((PI4096*30*i)/180) + global.pos.angle,&cos,&sin);
					pointEx[i].x = ((Sint32)(cos)*EXTRACTION_DISTANCE)/4096 + global.pos.x;
					pointEx[i].y = ((Sint32)(sin)*EXTRACTION_DISTANCE)/4096 + global.pos.y;
					debug_printf("Point i=%d x=%d, y=%d\n", i, pointEx[i].x, pointEx[i].y);
					if(is_possible_point_for_rotation(&pointEx[i]) && is_possible_point_for_dodge(&pointEx[i]))	//Si le point est "acceptable" (loin d'un élément fixe ou d'une bordure...)
					{
						distance2_between_point_and_foe_min = 0xFFFFFFFF;
						//On recherche la distance minimale entre le point 'i' et l'adversaire le plus proche.
						for(foe = 0; foe < MAX_NB_FOES; foe++)		//Pour tout les adversaires obsersés
						{
							if(global.foe[foe].enable){
								distance2_between_point_and_foe = (pointEx[i].x-global.foe[foe].x)*(pointEx[i].x-global.foe[foe].x) + (pointEx[i].y-global.foe[foe].y)*(pointEx[i].y-global.foe[foe].y);
								if(distance2_between_point_and_foe < distance2_between_point_and_foe_min){	//Si l'adversaire en cours est plus proche du point que les autres, on le prend en compte.
									distance2_between_point_and_foe_min = distance2_between_point_and_foe;
								}
							}
						}

						//On recherche maintenant le point ayant "la distance avec son adversaire le plus proche", la plus GRANDE possible...
						if(distance2_between_point_and_foe_min > bestPoint_distance2_with_nearest_foe)
						{	//Si le point 'i' est plus loin des adversaires que les autres points calculés... il est le meilleur point candidat
							bestPoint_distance2_with_nearest_foe = distance2_between_point_and_foe_min;
							bestPoint = i;
						}
					}
				}
			}


			if(bestPoint != 0xFF)
			{
				can_i_go_to_point = TRUE; // Si le point faux, on le mettra a faux

				//Pour gerer un cas d'erreur, si nous somme trop prêt d'un adversaire, un point derrière peut être supprimé alors que l'adversaire est devant nous
				for(foe = 0; foe < MAX_NB_FOES; foe++)
				{// Si nous sommes encercle par deux ennemis, on peut pas se permettre de comparer seulement avec l'ennemis le plus proche du point
					if(global.foe[foe].enable)
					{
						distance2_between_point_and_foe = (pointEx[bestPoint].x-global.foe[foe].x)*(pointEx[bestPoint].x-global.foe[foe].x) + (pointEx[bestPoint].y-global.foe[foe].y)*(pointEx[bestPoint].y-global.foe[foe].y);

						if(distance2_between_point_and_foe < adversary_to_close_distance*adversary_to_close_distance)
						{ // Si le point est pres de l'adveraire, on regarde où il se situe par rapport à nous et l'adversaire
							//Calcul du point sur le bord du robot en direction de l'adversaire de façon à offrir un point de plus pour la sortie si il est encerclée
							Uint16 norm = GEOMETRY_distance((GEOMETRY_point_t){global.pos.x,global.pos.y},(GEOMETRY_point_t){global.foe[foe].x,global.foe[foe].y});

							float coefx = (global.foe[foe].x-global.pos.x)/(norm*1.);
							float coefy = (global.foe[foe].y-global.pos.y)/(norm*1.);

							GEOMETRY_point_t center; // Le centre d'où nous allons faire le produit scalaire
							Uint8 widthRobot;
							widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? (BIG_ROBOT_WIDTH/2) : (SMALL_ROBOT_WIDTH/2);

							center.x = global.pos.x + widthRobot*coefx;
							center.y = global.pos.y + widthRobot*coefy;

							Sint32 vecAdX = global.foe[foe].x-center.x;
							Sint32 vecAdY = global.foe[foe].y-center.y;
							Sint32 vecPointX = pointEx[bestPoint].x-center.x;
							Sint32 vecPointY = pointEx[bestPoint].y-center.y;

							if(vecAdX*vecPointX + vecAdY*vecPointY > 0)
							{ // Si le produit scalaire des deux vecteurs est positif nous pouvons pas aller au point car le point se situe entre nous et l'adversaire
								can_i_go_to_point = FALSE;
								break;
							}
						}
					}
				}
			}



			//Si on a trouvé un point et qu'il est suffisamment loin des adversaires.... Champomy !!!
			if(can_i_go_to_point)
				state = GO_POINT;
			else
				state = WAIT;

			break;

		case GO_POINT:
			switch(goto_extract_with_avoidance((displacement_t){(GEOMETRY_point_t){pointEx[bestPoint].x,pointEx[bestPoint].y}, speed}))
			{
					case END_OK:
						state = DONE;
						break;
					case FOE_IN_PATH:
						if(is_possible_point_for_rotation(&((GEOMETRY_point_t){global.pos.x,global.pos.y})))
							state = ERROR_FOE;
						else
							state = WAIT;	//On est pas sur un turning point, on ne peut pas rendre la main.
						break;
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						if(is_possible_point_for_rotation(&((GEOMETRY_point_t){global.pos.x,global.pos.y})))
							state = ERROR;
						else
							state = WAIT;	//On est pas sur un turning point, on ne peut pas rendre la main.
						break;
					default:
						break;
				}
			break;

		case WAIT:{ // Etat d'attente si il n'a pas trouvé de chemin dans l'immédiat
			static time32_t begin_time;

			if(entrance)
			{
				//BUZZER_play(500, NOTE_LA, 10);
				begin_time = global.match_time;
			}

			if(global.match_time > begin_time + 1000)
			{
				if(remaining_try)
					state = COMPUTE;
				else if(is_possible_point_for_rotation(&((GEOMETRY_point_t){global.pos.x,global.pos.y}))){
					state = sens;
				}else
					state = ERROR;   //ajout 2016 pour éviter les boucles infinies
			}

			}break;

		case TURN_TRIGO:
			remaining_try = 3;
			state = try_go_angle(global.pos.angle + PI4096/2, state, COMPUTE, TURN_HORAIRE, FAST, ANY_WAY, END_AT_LAST_POINT);

			if(state == TURN_HORAIRE)
				sens = TURN_HORAIRE;
			break;

		case TURN_HORAIRE:
			remaining_try = 3;
			state = try_go_angle(global.pos.angle - PI4096/2, TURN_HORAIRE, COMPUTE, TURN_TRIGO, FAST, ANY_WAY, END_AT_LAST_POINT);

			if(state == TURN_TRIGO)
				sens = TURN_TRIGO;
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = IDLE;
			return NOT_HANDLED;
			break;
		case ERROR_FOE:
			state = IDLE;
			return FOE_IN_PATH;
			break;

		default:
			state = IDLE;
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}

/* Fonction qui réalise un PROP_push_goto spécifique à l'extration du robot avec la gestion de l'évitement */

static error_e goto_extract_with_avoidance(const displacement_t displacements)
{
	CREATE_MAE(
			LOAD_MOVE,
			WAIT_MOVE_AND_SCAN_FOE,
			DONE
		);

	static time32_t beginTime;
	static Uint8 idTraj;

	switch(state)
	{

		case LOAD_MOVE:
			global.prop.detected_foe = FALSE;
			global.destination = displacements.point;

			idTraj = PROP_getNextIdTraj();
			beginTime = global.absolute_time;
			PROP_goTo(displacements.point.x, displacements.point.y, PROP_ABSOLUTE, FALSE, PROP_END_AT_POINT, displacements.speed, ANY_WAY, PROP_NO_CURVE, AVOID_ENABLED, PROP_NO_BORDER_MODE, idTraj);
			debug_printf("goto_extract_with_avoidance : load_move\n");
			state = WAIT_MOVE_AND_SCAN_FOE;
			break;

		case WAIT_MOVE_AND_SCAN_FOE:{
			error_e subaction = wait_move_and_wait_detection(TRAJECTORY_TRANSLATION, 1, idTraj, END_AT_LAST_POINT, beginTime);
			switch(subaction)
			{
				case END_OK:
					RESET_MAE();
					debug_printf("goto_extract_with_avoidance -- fini\n");
					return END_OK;

				case END_WITH_TIMEOUT:
					RESET_MAE();
					info_printf("TIMEOUT on goto_extract_with_avoidance");
					return END_OK;

				case NOT_HANDLED:
					info_printf("ERROR on goto_extract_with_avoidance");
					RESET_MAE();
					return NOT_HANDLED;

				case IN_PROGRESS:
					break;

				default:
					RESET_MAE();
					return NOT_HANDLED;
			}

			// check adversaire détecté
			if(global.prop.detected_foe){
				RESET_MAE();
				return FOE_IN_PATH;
			}
			}break;

		default:
			RESET_MAE();
			debug_printf("Cas Default state, panique !!!\n");
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

//------------------------------------------------------------------- Fonctions autress

/* Action qui update la position */
error_e ACTION_update_position()
{
	CREATE_MAE(
			SEND_CAN_MSG,
			WAIT_RECEPTION
		);

	switch (state)
	{
		case SEND_CAN_MSG :
			if (!global.pos.updated)
			{
				CAN_send_sid(PROP_TELL_POSITION);
				state = WAIT_RECEPTION;
				return IN_PROGRESS;
			}
			else
			{
				return END_OK;
			}
			break;

		case WAIT_RECEPTION :
			if (global.pos.updated)
			{
				state = SEND_CAN_MSG;
				return END_OK;
			}
			break;
	}
	return IN_PROGRESS;
}

