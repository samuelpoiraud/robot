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

#include "avoidance.h"
#include "prop_functions.h"
#include "Supervision/Buzzer.h"
#include "math.h"
#include "state_machine_helper.h"
#include "QS/QS_maths.h"
#include "QS/QS_IHM.h"
#include "QS/QS_maths.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_watchdog.h"


#define LOG_PREFIX "avoid: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_AVOIDANCE
#include "QS/QS_outputlog.h"
#include "Supervision/SD/SD.h"

#define WAIT_TIME_DETECTION			1000	//[ms] temps pendant lequel on attend que l'adversaire s'en aille. Ensuite, on abandonne la trajectoire.
#define FOE_IS_LEFT_TIME			250		//[ms] temps depuis lequel l'adversaire doit être parti pour que l'on reprenne notre trajectoire.
#define EXTRACTION_DISTANCE			300

static error_e AVOIDANCE_watch_prop_stack();
static error_e extraction_of_foe(PROP_speed_e speed);
static error_e goto_extract_with_avoidance(const displacement_t displacements);
static error_e wait_move_and_wait_foe();


static Uint16 wait_timeout = WAIT_TIME_DETECTION;
static bool_e prop_detected_foe = FALSE;

//------------------------------------------------------------------- Machines à états de déplacements

//Action qui gere un déplacement et renvoi le state rentré en arg. Ne s'arrète qu'à la fin que si aucun autre déplacement n'est demandé.
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition, zones_list_t list)
{
	/*if(checkZones(list)==FALSE)
		return fail_state;*/
	error_e sub_action;
	//sub_action = goto_pos_with_scan_foe((displacement_t[]){{{x, y},FAST}},1,way,avoidance);
	if(end_condition == END_AT_LAST_POINT || end_condition == END_AT_BREAK)
		sub_action = goto_pos_curve_with_avoidance((displacement_t[]){{{x, y},speed}}, NULL, 1, way, avoidance, end_condition, PROP_NO_BORDER_MODE);
	else
		sub_action = goto_pos_curve_with_avoidance_and_break((displacement_t[]){{{x, y},speed}}, NULL, 1, way, avoidance, end_condition, PROP_NO_BORDER_MODE);
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
Uint8 try_going_multipoint(const displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition)
{
	error_e sub_action;
	if(end_condition == END_AT_LAST_POINT || end_condition == END_AT_BREAK)
		sub_action = goto_pos_curve_with_avoidance(displacements, NULL, nb_displacements, way, avoidance, end_condition, PROP_NO_BORDER_MODE);
	else
		sub_action = goto_pos_curve_with_avoidance_and_break(displacements, NULL, nb_displacements, way, avoidance, end_condition, PROP_NO_BORDER_MODE);

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

//Action qui gere un changement d'angle et renvoi le state rentré en arg.
Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed)
{
	CREATE_MAE(
			EMPILE,
			WAIT,
			DONE,
			ERROR
		);

	Uint8 ret;
	static bool_e timeout;
	ret = in_progress;
	switch(state)
	{
		case EMPILE:
			PROP_push_goangle(angle, speed, TRUE);
			state = WAIT;
			break;
		case WAIT:
			if(STACKS_wait_end_auto_pull(PROP, &timeout)){
				state = DONE;
			}
			else if (global.prop.erreur)
			{
				STACKS_flush(PROP);
				state = ERROR;
			}
			break;
		case DONE:
			state = EMPILE;
			ret = (timeout)? fail_state : success_state;
			break;
		case ERROR:
			state = EMPILE;
			ret = NOT_HANDLED;
			break;
		default:
			state = EMPILE;
			ret = fail_state;
			break;
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
					angle -= (way == BACKWARD)? PI4096 : 0;
				else
					angle += (way == BACKWARD)? PI4096 : 0;
			}
			state = try_go_angle(angle, FAST_ROTATE, INIT_COEF, ERROR, FAST);
			break;

		case INIT_COEF:
			PROP_set_threshold_error_translation(50,FALSE);
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
			PROP_set_threshold_error_translation(0,TRUE);
			RESET_MAE();
			return fail_state;

		case DONE :
			PROP_set_threshold_error_translation(0,TRUE);
			RESET_MAE();
			return success_state;
	}
	return in_progress;
}

//Action qui gere un déplacement en avance ou arrière et renvoi le state rentré en arg.
Uint8 try_advance(GEOMETRY_point_t *point, bool_e compute, Uint16 dist, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition)
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
			return_state = try_going(compute_point.x, compute_point.y, in_progress, success_state, fail_state, speed, way, avoidance, end_condition, NO_ZONE);
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
		STOP,
		WAIT_AND_CHECK
	);

	error_e subaction;

	switch(state){
		case STOP :
			PROP_push_stop();
			state = WAIT_AND_CHECK;
			break;

		case WAIT_AND_CHECK :
			subaction = AVOIDANCE_watch_prop_stack();
			switch (subaction) {
				case END_OK:
					debug_printf("PROP_STOP effectué\n");
					state = STOP;
					return success_state;
					break;

				case IN_PROGRESS:
					break;

				case END_WITH_TIMEOUT:
					debug_printf("PROP_STOP effectué avec TIMEOUT\n");
					state = STOP;
					return fail_state;
					break;

				case NOT_HANDLED:
					debug_printf("PROP_STOP erreur\n");
					state = STOP;
					return fail_state;
					break;

				default:
					debug_printf("PROP_STOP erreur\n");
					state = STOP;
					return fail_state;
					break;
			}
			break;
	}
	return in_progress;
}


//------------------------------------------------------------------- Fonctions relatives à l'évitement

//Version simplifiée de wait_move_and_scan_foe. on esquive pas la cible ici. (pas de dodge)
static error_e wait_move_and_wait_foe() {

	error_e ret = IN_PROGRESS;

	// check fin de trajectoire
	error_e prop_stack_state = AVOIDANCE_watch_prop_stack();
	switch(prop_stack_state)
	{
		case IN_PROGRESS:
			break;

		case END_OK:
		case END_WITH_TIMEOUT:
		case NOT_HANDLED:
			debug_printf("wait_move_and_wait_foe: end state = %d\n", prop_stack_state);
			ret = prop_stack_state;
			break;

		default: //Ne devrait jamais arriver, AVOIDANCE_watch_prop_stack ne doit pas retourner FOE_IN_PATH car elle ne gère pas d'evitement
			debug_printf("wait_move_and_wait_foe: DEFAULT prop_stack_state = %d!!\n", prop_stack_state);
			ret = NOT_HANDLED;
			break;
	}

	// check adversaire détecté
	if(prop_detected_foe){
		STACKS_flush(PROP);
		ret = FOE_IN_PATH;
	}

	return ret;
}


void AVOIDANCE_forced_foe_dected(){
	CAN_msg_t msg;
	msg.sid = PROP_DEBUG_FORCED_FOE;
	msg.size = 0;
	CAN_send(&msg);
}


void AVOIDANCE_set_timeout(Uint16 msec) {
	wait_timeout = msec;
}

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
error_e goto_pos_curve_with_avoidance(const displacement_t displacements[], const displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, PROP_end_condition_e end_condition, prop_border_mode_e border_mode)
{
	CREATE_MAE(
			LOAD_MOVE,
			WAIT_MOVE_AND_SCAN_FOE,
			EXTRACT,
			DONE
		);

	static bool_e timeout = FALSE;
	static error_e sub_action;

	Uint8 i;

	//Si nouveau déplacement et qu'aucun point n'est donné, on a rien a faire
	if(state == LOAD_MOVE && nb_displacements == 0)
		return END_OK;

	switch(state)
	{
		case LOAD_MOVE:
			timeout = FALSE;
			clear_prop_detected_foe();

			if(displacements)
				global.destination = displacements[nb_displacements-1].point;
			else
				global.destination = displacements_curve[nb_displacements-1].point;

			for(i=nb_displacements-1;i>=1;i--)
			{
				if(displacements)
					PROP_push_goto_multi_point(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, PROP_CURVE, avoidance_type, PROP_END_OF_BUFFER, end_condition, border_mode, FALSE);
				else if(displacements_curve && displacements_curve[i].curve)
					PROP_push_goto_multi_point(displacements_curve[i].point.x, displacements_curve[i].point.y, displacements_curve[i].speed, way, PROP_CURVE, avoidance_type, PROP_END_OF_BUFFER, end_condition, border_mode, FALSE);
				else
					PROP_push_goto(displacements_curve[i].point.x, displacements_curve[i].point.y, displacements_curve[i].speed, way, PROP_NO_CURVE, avoidance_type, end_condition, border_mode, FALSE);
			}
			if(displacements)
				PROP_push_goto_multi_point(displacements[0].point.x, displacements[0].point.y, displacements[0].speed, way, PROP_CURVE, avoidance_type, PROP_END_OF_BUFFER, end_condition, border_mode, TRUE);
			else if(displacements_curve && displacements_curve[0].curve)
				PROP_push_goto_multi_point(displacements_curve[0].point.x, displacements_curve[0].point.y, displacements_curve[0].speed, way, PROP_CURVE, avoidance_type, PROP_END_OF_BUFFER, end_condition, border_mode, TRUE);
			else
				PROP_push_goto(displacements_curve[0].point.x, displacements_curve[0].point.y, displacements_curve[0].speed, way, PROP_NO_CURVE, avoidance_type, end_condition, border_mode, TRUE);

			debug_printf("goto_pos_with_scan_foe : load_move\n");
			if(displacements || displacements_curve)
				state = WAIT_MOVE_AND_SCAN_FOE;
			else
			{
				state = LOAD_MOVE;
				return NOT_HANDLED;
			}
			break;

		case WAIT_MOVE_AND_SCAN_FOE:

			sub_action = wait_move_and_wait_foe();

			switch(sub_action)
			{
				case END_OK:
					debug_printf("wait_move_and_scan_foe -- fini\n");
					state = DONE;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					debug_printf("wait_move_and_scan_foe -- timeout\n");
					SD_printf("TIMEOUT on WAIT_MOVE_AND_SCAN_FOE\n");
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					state = DONE;
					break;

				case NOT_HANDLED:
					debug_printf("wait_move_and_scan_foe -- probleme\n");
					SD_printf("ERROR on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					state = LOAD_MOVE;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
					break;

				case FOE_IN_PATH:
					debug_printf("wait_move_and_scan_foe -- foe in path\n");
					SD_printf("FOE_IN_PATH on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					if(avoidance_type == DODGE_AND_WAIT || avoidance_type == DODGE_AND_NO_WAIT)
						state = EXTRACT;
					else
					{
						state = LOAD_MOVE;
						global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
						return FOE_IN_PATH;			//Pas d'extraction demandée... on retourne tel quel FOE_IN_PATH !
					}
					break;

				case IN_PROGRESS:
					break;

				default:
					state = LOAD_MOVE;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
					break;
			}
			break;

		case EXTRACT:
			sub_action = extraction_of_foe(FAST);//SLOW_TRANSLATION_AND_FAST_ROTATION);
			switch(sub_action)
			{
				case END_OK:
					state = LOAD_MOVE;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return FOE_IN_PATH;
					break;

				case IN_PROGRESS:
					break;

				case NOT_HANDLED:
				default:
					state = LOAD_MOVE;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			wait_timeout = WAIT_TIME_DETECTION;
			state = LOAD_MOVE;
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("Cas Default state, panique !!!\n");
			state = LOAD_MOVE;
			global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}


/*
 * Fonction qui réalise un PROP_push_goto tout simple avec la gestion de l'évitement (en courbe)
 * Fonction très semblable à goto_pos_curve_with_avoidance mais le changement de point de destination se fait lorsque le robot
 * est arrivé à une certaine distance du point de destination précédent.
 *
 * pre : Etre sur le terrain
 * post : Robot aux coordonnées voulues
 *
 * param displacements : deplacements de la trajectoire
 * param nb_displacement : nombre de deplacements de la trajectoire
 * param way : sens de déplacement
 * end_condition : END_AT_DISTANCE
 *
 * return IN_PROGRESS : En cours
 * return END_OK : Terminé
 * return NOT_HANDLED : Action impossible
 * return END_WITH_TIMEOUT : Timeout
 * return FOE_IN_PATH : un adversaire nous bloque
 */
error_e goto_pos_curve_with_avoidance_and_break(const displacement_t displacements[], const displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, PROP_end_condition_e end_condition, prop_border_mode_e border_mode)
{
	CREATE_MAE(
			INIT,
			NEXT_MOVE,
			LOAD_MOVE,
			WAIT_MOVE_AND_SCAN_FOE,
			EXTRACT,
			DONE
		);

	static bool_e timeout = FALSE;
	static error_e sub_action;

	static Sint16 i;

	//Si nouveau déplacement et qu'aucun point n'est donné, on a rien a faire
	if(state == INIT && nb_displacements == 0)
		return END_OK;

	switch(state)
	{
		case INIT:
			timeout = FALSE;
			clear_prop_detected_foe();

			if(displacements)
				global.destination = displacements[nb_displacements-1].point;
			else
				global.destination = displacements_curve[nb_displacements-1].point;

			i = -1;
			state = NEXT_MOVE;
			break;

		case NEXT_MOVE:
			i++;
			debug_printf("Next move i=%d\n", i);
			state = LOAD_MOVE;
			break;

		case LOAD_MOVE:
			if(i >= nb_displacements){ //Tous les mouvements ont été éffectués
				state = DONE;
				debug_printf("Tous les déplacements ont ete effectues avec succes\n");
			}else if(!displacements && !displacements_curve){ //Si aucun mouvement n'a été défini, on renvoie une erreur
				state = LOAD_MOVE;
				debug_printf("Aucun mouvement défini, return NOT HANDLED\n");
				return NOT_HANDLED;
			}else{ //Si il reste des mouvements à éffectuer
					PROP_push_goto(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, PROP_CURVE, avoidance_type, end_condition, border_mode, TRUE);
					debug_printf("Move goto to x=%d  y=%d push NOW\n", displacements[i].point.x, displacements[i].point.y);
					state = WAIT_MOVE_AND_SCAN_FOE;
			}
			debug_printf("goto_pos_with_scan_foe : load_move\n");
			break;

		case WAIT_MOVE_AND_SCAN_FOE:

			sub_action = wait_move_and_wait_foe();

			switch(sub_action)
			{
				case END_OK:
					debug_printf("wait_move_and_scan_foe -- fini\n");
					state = NEXT_MOVE;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					debug_printf("wait_move_and_scan_foe -- timeout\n");
					SD_printf("TIMEOUT on WAIT_MOVE_AND_SCAN_FOE\n");
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					state = DONE;
					break;

				case NOT_HANDLED:
					debug_printf("wait_move_and_scan_foe -- probleme\n");
					SD_printf("ERROR on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					state = INIT;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
					break;

				case FOE_IN_PATH:
					debug_printf("wait_move_and_scan_foe -- foe in path\n");
					SD_printf("FOE_IN_PATH on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					if(avoidance_type == DODGE_AND_WAIT || avoidance_type == DODGE_AND_NO_WAIT)
						state = EXTRACT;
					else
					{
						state = INIT;
						global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
						return FOE_IN_PATH;			//Pas d'extraction demandée... on retourne tel quel FOE_IN_PATH !
					}
					break;

				case IN_PROGRESS:
					break;

				default:
					state = INIT;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					return NOT_HANDLED;
					break;
			}
			break;

		case EXTRACT:
			sub_action = extraction_of_foe(FAST);//SLOW_TRANSLATION_AND_FAST_ROTATION);
			switch(sub_action)
			{
				case END_OK:
					state = INIT;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					debug_printf("Extraction success\n");
					return FOE_IN_PATH;
					break;

				case IN_PROGRESS:
					break;

				case NOT_HANDLED:
				default:
					state = INIT;
					global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					debug_printf("Extraction fail\n");
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			wait_timeout = WAIT_TIME_DETECTION;
			state = INIT;
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("Cas Default state, panique !!!\n");
			state = INIT;
			global.destination = (GEOMETRY_point_t){global.pos.x, global.pos.y};
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}


/* ----------------------------------------------------------------------------- */
/* 		Fonctions de scrutation de la position de l'adversaire					 */
/* ----------------------------------------------------------------------------- */

//Retourne si un adversaire est dans le carré dont deux coins 1 et 2 sont passés en paramètres
bool_e foe_in_square(bool_e verbose, Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2)
{
	Uint8 i;
	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.foe[i].enable)
		{
			if(is_in_square(x1,x2,y1,y2,(GEOMETRY_point_t){global.foe[i].x,global.foe[i].y}))
			{
				if(verbose)
					SD_printf("FOE %d[%d;%d] found in zone x[%d->%d] y[%d->%d]\n",i,global.foe[i].x,global.foe[i].y,x1,x2,y1,y2);
				return TRUE;
			}
		}
	}
	SD_printf("NO FOE found in zone x[%d->%d] y[%d->%d]\n",x1,x2,y1,y2);
	return FALSE;
}

//Un point est-t-il une position permettant les rotations sans tapper dans un élément de jeu.
bool_e is_possible_point_for_rotation(GEOMETRY_point_t * p)
{
	Uint8 widthRobot;
	widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? BIG_ROBOT_WIDTH/2 : SMALL_ROBOT_WIDTH/2;
	widthRobot += 100;	//Marge !

	// Spécifique Terrain 2016
	if(
			!is_in_square(50 + (widthRobot), 2000-(widthRobot), 0+(widthRobot), 3000-(widthRobot), *p)	// Hors Terrain - la zone des portes de cabanes...
		|| 	is_in_square(0, 200+(widthRobot), 800-(widthRobot), 940+(widthRobot),*p)			        // Tasseau de la dune côté violet + carré de 8 cubes
		||  is_in_square(0, 200+(widthRobot), 2060-(widthRobot), 2200+(widthRobot),*p)		            // Tasseau de la dune côté vert + carré de 8 cubes
		||	is_in_square(750-(widthRobot), 1350+(widthRobot), 1460-(widthRobot), 1530+(widthRobot),*p)  // Tasseau verticale de la zone centrale
		||	is_in_square(750-(widthRobot), 780+(widthRobot), 900-(widthRobot), 2100+(widthRobot),*p)	// Tasseau horizontale de la zone centrale
		||	is_in_square(1950-(widthRobot), 2000, 910-(widthRobot), 960+(widthRobot),*p)                // Attache filet côté vert
		||	is_in_square(1950-(widthRobot), 2000, 2040-(widthRobot), 2090+(widthRobot),*p)              // Attache filet côté violet
	  )
		return FALSE;

	return  TRUE;
}

//Le point passé en paramètre permet-il une extraction ?
bool_e is_possible_point_for_dodge(GEOMETRY_point_t * p)
{
	Uint8 widthRobot;
	widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? BIG_ROBOT_WIDTH/2 : SMALL_ROBOT_WIDTH/2;
	widthRobot += 100;	//Marge !
	GEOMETRY_circle_t zone_depose_adv = {(GEOMETRY_point_t){750, 1500}, 700};

	// Spécifique Terrain 2016
	if(
			(is_in_circle(*p, zone_depose_adv) && is_in_square(750, 1350+(widthRobot), 1500, COLOR_Y(2100+(widthRobot)),*p))  // Zone de dépose adverse
		||  is_in_square(600-(widthRobot), 1100+(widthRobot), COLOR_Y(2700-(widthRobot)), COLOR_Y(3000),*p) // zone de départ adverse
	)
		return FALSE;

	return  TRUE;
}

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
		ERROR
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
							widthRobot =  (QS_WHO_AM_I_get() == BIG_ROBOT)? BIG_ROBOT_WIDTH/2 : SMALL_ROBOT_WIDTH/2;

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
			state = check_sub_action_result(goto_extract_with_avoidance((displacement_t){(GEOMETRY_point_t){pointEx[bestPoint].x,pointEx[bestPoint].y}, speed}), GO_POINT, DONE, WAIT);
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
			state = try_go_angle(global.pos.angle + PI4096/2,TURN_TRIGO,COMPUTE,TURN_HORAIRE,FAST);

			if(state == TURN_HORAIRE)
				sens = TURN_HORAIRE;
			break;

		case TURN_HORAIRE:
			remaining_try = 3;
			state = try_go_angle(global.pos.angle - PI4096/2,TURN_HORAIRE,COMPUTE,TURN_TRIGO,FAST);

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

	static bool_e timeout = FALSE;
	error_e prop_stack_state;
	static volatile bool_e flag_buzzer = FALSE;

	switch(state)
	{

		case LOAD_MOVE:
			clear_prop_detected_foe();
			global.destination = displacements.point;
			PROP_push_goto_multi_point(displacements.point.x, displacements.point.y, displacements.speed, ANY_WAY, PROP_CURVE, AVOID_ENABLED, PROP_END_OF_BUFFER, END_AT_LAST_POINT, PROP_NO_BORDER_MODE, TRUE);
			debug_printf("goto_extract_with_avoidance : load_move\n");
			WATCHDOG_create_flag(300, &flag_buzzer);
			BUZZER_play(200, DEFAULT_NOTE, 1);
			state = WAIT_MOVE_AND_SCAN_FOE;
			break;

		case WAIT_MOVE_AND_SCAN_FOE:
			prop_stack_state = AVOIDANCE_watch_prop_stack();
			switch(prop_stack_state)
			{
				case END_OK:
					debug_printf("goto_extract_with_avoidance -- fini\n");
					state = DONE;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					debug_printf("goto_extract_with_avoidance -- timeout\n");
					SD_printf("TIMEOUT on goto_extract_with_avoidance");
					state = DONE;
					break;

				case NOT_HANDLED:
					debug_printf("goto_extract_with_avoidance -- probleme\n");
					SD_printf("ERROR on goto_extract_with_avoidance");
					state = LOAD_MOVE;
					return NOT_HANDLED;
					break;

				case IN_PROGRESS:
					if (flag_buzzer) {
						BUZZER_play(200, DEFAULT_NOTE, 1);
						WATCHDOG_create_flag(300, &flag_buzzer);
					}
					break;

				default:
					state = LOAD_MOVE;
					return NOT_HANDLED;
					break;
			}

			// check adversaire détecté
			if(prop_detected_foe){
				STACKS_flush(PROP);
				state = LOAD_MOVE;
				return FOE_IN_PATH;
			}
			break;

		case DONE:
			state = LOAD_MOVE;
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("Cas Default state, panique !!!\n");
			state = LOAD_MOVE;
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}

// Vérifie adversaire dans NORTH_US, NORTH_FOE, SOUTH_US, SOUTH_FOE
foe_pos_e AVOIDANCE_where_is_foe(Uint8 foe_id)
{
	assert(foe_id < MAX_NB_FOES);

	if(global.foe[foe_id].x > 1000)
	{
		// Partie SUD
		if(COLOR_Y(global.foe[foe_id].y) > 1500)
		{
			return SOUTH_FOE;
		}
		else
		{
			return SOUTH_US;
		}
	}
	else
	{
		if(COLOR_Y(global.foe[foe_id].y) > 1500)
		{
			return NORTH_FOE;
		}
		else
		{
			return NORTH_US;
		}
	}
}

void debug_foe_reason(foe_origin_e origin, Sint16 angle, Sint16 distance){
	CAN_msg_t msg_to_send;
	msg_to_send.sid = DEBUG_FOE_REASON;
	msg_to_send.size = SIZE_DEBUG_FOE_REASON;
	msg_to_send.data.debug_foe_reason.foe_origine = origin;
	msg_to_send.data.debug_foe_reason.angle = angle;
	msg_to_send.data.debug_foe_reason.dist = distance;
	CAN_send(&msg_to_send);
}


void clear_prop_detected_foe(){
	prop_detected_foe = FALSE;
}

void set_prop_detected_foe(CAN_msg_t *msg){

	if(msg->sid == STRAT_PROP_FOE_DETECTED){
		if(msg->data.strat_prop_foe_detected.in_wait == FALSE){
			prop_detected_foe = TRUE;
		}
	}
}

//------------------------------------------------------------------- Fonctions autress

/*
 * Surveille l'execution de la pile PROP. Renvoie vrai si toutes les fonctions sont finies
 * Rattrappe le robot quand il part en erreur.
 * Inscrit dans le booléen got_timeout si un timeout a été levé
 */
static error_e AVOIDANCE_watch_prop_stack ()
{
	bool_e timeout = FALSE, action_end;

	action_end = STACKS_wait_end(PROP,&timeout);

	if(timeout){ // S'il y a timeout que la l'ensemble des trajectoires soient finit ou pas, on vide la stack puis on déclare le timeout
		STACKS_flush(PROP);
		return END_WITH_TIMEOUT;
	}else if(action_end) // Si l'ensemble des trajectoires sont finit, on le déclare
		return END_OK;
	else if (global.prop.erreur){
		STACKS_flush(PROP);
		return NOT_HANDLED;
	}

	return IN_PROGRESS;
}

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

/* Action qui arrête le robot, met la position à jour */
error_e ACTION_prop_stop()
{
	CREATE_MAE(
			SEND_CAN_MSG,
			WAIT_RECEPTION
		);

	static time32_t initial_time;

	switch (state)
	{
		case SEND_CAN_MSG :
			initial_time = global.match_time;
			STACKS_flush(PROP);
			CAN_send_sid(PROP_STOP);
			state = WAIT_RECEPTION;
			break;

		case WAIT_RECEPTION :
			if (global.prop.ended || (global.match_time-initial_time > (1000/*ms*/)))
			{
				state = SEND_CAN_MSG;
				return END_OK;
			}
			break;
	}
	return IN_PROGRESS;
}

bool_e i_am_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2){
	return is_in_square(x1, x2, COLOR_Y(y1), COLOR_Y(y2), (GEOMETRY_point_t){global.pos.x, global.pos.y});
}

bool_e is_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, GEOMETRY_point_t current){
	return is_in_square(x1, x2, COLOR_Y(y1), COLOR_Y(y2), current);
}
