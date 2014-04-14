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
#include "can_utils.h"
#include "maths_home.h"
#include "QS/QS_who_am_i.h"
#include "asser_functions.h"
#include "config_use.h"
#include "QS/QS_outputlog.h"
#include "config/config_pin.h"
#include "config/config_debug.h"
#include "Supervision/Buzzer.h"
#include "Supervision/SD/SD.h"
#include "math.h"

#define SMALL_ROBOT_ACCELERATION_NORMAL	468*2	//Réglage d'accélération de la propulsion : 625 	mm/sec = 64 	[mm/4096/5ms/5ms]
#define BIG_ROBOT_ACCELERATION_NORMAL	937*2	//Réglage d'accélération de la propulsion : 1094 	mm/sec = 112 	[mm/4096/5ms/5ms]
#define SMALL_ROBOT_RESPECT_DIST_MIN	400		//Distance à laquelle on se tient d'un adversaire [mm]
#define BIG_ROBOT_RESPECT_DIST_MIN		500		//Distance à laquelle on se tient d'un adversaire [mm]
#define SMALL_ROBOT_WIDTH				200		//Largeur du petit robot [mm]
#define BIG_ROBOT_WIDTH					300		//Largeur du gros robot [mm]
#define	FOE_SIZE						400		//taille supposée de l'adversaire
#define MARGE_COULOIR_EVITEMENT_STATIC	(300 + 100)
#define DISTANCE_EVITEMENT_STATIC		500

#define WAIT_TIME_DETECTION			1000	//[ms] temps pendant lequel on attend que l'adversaire s'en aille. Ensuite, on abandonne la trajectoire.
#define FOE_IS_LEFT_TIME			250		//[ms] temps depuis lequel l'adversaire doit être parti pour que l'on reprenne notre trajectoire.

static error_e AVOIDANCE_watch_asser_stack();

#ifdef DEBUG_AVOIDANCE
	#define avoidance_printf(format, ...)	debug_printf("t=%lums " format, global.env.match_time, ##__VA_ARGS__)
#else
	#define avoidance_printf(...)	(void)0
#endif

static Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

#ifdef USE_POLYGON

	/* Action qui déplace le robot grâce à l'algorithme des polygones en testant avec tous les elements
	puis seulement avec les notres s'il est impossible de trouver un chemin */
	error_e goto_polygon_default(Sint16 x, Sint16 y, way_e way, ASSER_speed_e speed, Uint8 curve,polygon_elements_type_e element_type)
	{
		enum state_e
		{
			GOTO_POLYGON_WITH_ALL_ELEMENTS = 0,
			GOTO_POLYGON_WITH_OUR_AND_OPPONENT_ELEMENTS,
			GOTO_POLYGON_WITH_OUR_ELEMENTS,
			DONE
		};
		static enum state_e state = GOTO_POLYGON_WITH_ALL_ELEMENTS;

		static error_e sub_action;
		static bool_e timeout = FALSE;
		static bool_e init = FALSE;

		if(!init)
		{
			switch(element_type)
			{
				case ALL_ELEMENTS:
					state = GOTO_POLYGON_WITH_ALL_ELEMENTS;
					break;

				case OUR_AND_OPPONENT_ELEMENTS:
					state = GOTO_POLYGON_WITH_OUR_AND_OPPONENT_ELEMENTS;
					break;

				case OUR_ELEMENTS:
					state = GOTO_POLYGON_WITH_OUR_ELEMENTS;
					break;

				default:
					state = GOTO_POLYGON_WITH_ALL_ELEMENTS;
					break;
			}
			init = TRUE;
		}

		switch (state)
		{
			case GOTO_POLYGON_WITH_ALL_ELEMENTS:
				sub_action = goto_polygon(x, y, way, speed, curve, ALL_ELEMENTS);
				switch(sub_action)
				{
					case END_OK:
						state = DONE;
						break;

					case END_WITH_TIMEOUT:
						timeout = TRUE;
						state = DONE;
						break;

					case NOT_HANDLED:
						/* Aucun chemin possible avec tous les elements */
						avoidance_printf("goto_polygon : impossible avec tous les elements !\r\n");
						state = GOTO_POLYGON_WITH_OUR_AND_OPPONENT_ELEMENTS;
						break;

					case IN_PROGRESS:
						break;

					default:
						state = DONE;
						break;
				}
				break;

			case GOTO_POLYGON_WITH_OUR_AND_OPPONENT_ELEMENTS:
				sub_action = goto_polygon(x, y, way, speed, curve, OUR_AND_OPPONENT_ELEMENTS);
				switch(sub_action)
				{
					case END_OK:
						state = DONE;
						break;

					case END_WITH_TIMEOUT:
						timeout = TRUE;
						state = DONE;
						break;

					case NOT_HANDLED:
						/* Aucun chemin possible avec tous nos elements */
						avoidance_printf("goto_polygon : impossible avec nos elements !\r\n");
						state = GOTO_POLYGON_WITH_OUR_ELEMENTS;
						break;

					case IN_PROGRESS:
						break;

					default:
						state = DONE;
						break;
				}
				break;

			case GOTO_POLYGON_WITH_OUR_ELEMENTS:
				sub_action = goto_polygon(x, y, way, speed, curve, OUR_ELEMENTS);
				switch(sub_action)
				{
					case END_OK:
						state = DONE;
						break;

					case END_WITH_TIMEOUT:
						timeout = TRUE;
						state = DONE;
						break;

					case NOT_HANDLED:
						/* Aucun chemin possible avec tous nos elements */
						avoidance_printf("goto_polygon : impossible avec nos elements !\r\n");
						state = DONE;
						return NOT_HANDLED;
						break;

					case IN_PROGRESS:
						break;

					default:
						state = DONE;
						break;
				}
				break;

			case DONE:
				state = GOTO_POLYGON_WITH_ALL_ELEMENTS;
				init = FALSE;
				return timeout?END_WITH_TIMEOUT:END_OK;
				break;

			default:
				state = GOTO_POLYGON_WITH_ALL_ELEMENTS;
				return NOT_HANDLED;
				break;
		}
		return IN_PROGRESS;
	}

	/* Action qui déplace le robot grâce à l'algorithme des polygones en choisissant le type d'elements */
	error_e goto_polygon(Sint16 x, Sint16 y, way_e way, ASSER_speed_e speed, Uint8 curve, polygon_elements_type_e type_elements)
	{
		/* Gestion de la machine à états */
		enum state_e
		{
			INIT = 0,
			COMPUTE,
			WAIT_GO_POLYGON,
			DONE
		};
		static enum state_e state = INIT;

		Uint16 cost;

		/* relève les timeout */
		static bool_e timeout = FALSE;
		static bool_e use_opponent = FALSE;

		switch (state)
		{
			case INIT :
				use_opponent = FALSE;
				state = COMPUTE;
				break;

			case COMPUTE:
				cost=POLYGON_compute(global.env.pos.x, global.env.pos.y, x, y, way, speed, curve,type_elements,use_opponent);
				if(cost==POLYGON_PATHFIND_OPPONENT_COST)
				{
					avoidance_printf("POLYGON_compute : chemin polygon invalide !\n");
					state = COMPUTE;
					return NOT_HANDLED;
				}
				else
				{
					avoidance_printf("POLYGON_compute : chemin polygon trouve !\n");
					STACKS_pull(ASSER);	// On supprime le wait_forever pour lancer le déplacement
					state = WAIT_GO_POLYGON;
				}
				break;

			case WAIT_GO_POLYGON:
				switch(wait_move_and_scan_foe(NO_DODGE_AND_NO_WAIT))
				{
					case END_OK:
					case END_WITH_TIMEOUT:
						debug_printf("Pas d'adversaire sur polygone\n");
						state = DONE;
						break;

					case NOT_HANDLED:
					case FOE_IN_PATH:
						debug_printf("Adversaire detecte sur polygon!\r\n");
						use_opponent = TRUE;
						state = COMPUTE;
						break;

					case IN_PROGRESS :
					default :
						break;
				}
				break;

			case DONE:
				state = INIT;
				return (timeout?END_WITH_TIMEOUT:END_OK);
				break;

			default :
				state = INIT;
				break;
		}
		return IN_PROGRESS;
	}

	/* Action qui va à un noeud */
	error_e goto_node(Uint8 node,...)
	{
		avoidance_printf("USE_POLYGON activé, fonction indisponible\n");
		return NOT_HANDLED;
	}

#else
	/* Action qui déplace le robot grâce à l'algorithme des polygones en testant avec tous les elements
	puis seulement avec les notres s'il est impossible de trouver un chemin */
	error_e goto_polygon_default(Sint16 x,...)
	{
		avoidance_printf("USE_POYGON désactivé, fonction indisponible\n");
		return NOT_HANDLED;
	}

	/* Action qui déplace le robot grâce à l'algorythme de polygones */
	error_e goto_polygon(Sint16 x,...)
	{
		avoidance_printf("USE_POLYGON désactivé, fonction indisponible\n");
		return NOT_HANDLED;
	}

#endif


/*Equivalent de ASSER_goangle avec gestion de pile*/
error_e goto_angle (Sint16 angle, ASSER_speed_e speed){
	enum state_e {
		EMPILE,
		WAIT,
		DONE,
	};
	static enum state_e state = EMPILE;

	static bool_e timeout;

	switch(state){
		case EMPILE:
			ASSER_push_goangle(angle, speed, TRUE);
			state = WAIT;
			return IN_PROGRESS;
			break;
		case WAIT:
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				state = DONE;
			}
			return IN_PROGRESS;
			break;
		case DONE:
			state = EMPILE;
			return (timeout)? END_WITH_TIMEOUT : END_OK;
			break;
		default:
			state = EMPILE;
			return NOT_HANDLED;
	}
	return NOT_HANDLED;
}


/* Equivalent d'un ASSER_push_goto avec la gestion de la pile */
error_e goto_pos(Sint16 x, Sint16 y, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition)
{
	debug_printf("\nA=%d\n",x);
	enum state_e
	{
		PUSH_MOVE = 0,
		WAIT_END_OF_MOVE,
		DONE
	};
	static enum state_e state = PUSH_MOVE;

	static bool_e timeout=FALSE;

	switch(state)
	{
		case PUSH_MOVE:
			timeout = FALSE;
			ASSER_push_goto(x,y,speed,way, 0, end_condition,TRUE);

			state = WAIT_END_OF_MOVE;
			break;

		case WAIT_END_OF_MOVE:
			if(STACKS_wait_end_auto_pull(ASSER,&timeout))
			{
				state = DONE;
			}
			break;

		case DONE:
			state = PUSH_MOVE;
			return ((timeout)? END_WITH_TIMEOUT : END_OK);
			break;

		default :
			state = PUSH_MOVE;
			return NOT_HANDLED;
			break;
	}
	return IN_PROGRESS;
}

//Fonction de déplacement qui renvoie un état de stratégie suivant l'avancement du déplacement. Il s'arrète à la fin du déplacement
Uint8 try_going_until_break(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, avoidance_type_e avoidance)
{
	error_e sub_action;
	//sub_action = goto_pos_with_scan_foe_until_break((displacement_t[]){{{x, y},FAST}},1,way,avoidance);
	sub_action = goto_pos_with_avoidance((displacement_t[]){{{x, y},speed}},1,way,avoidance, END_AT_BREAK);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;
		case FOE_IN_PATH:
		case NOT_HANDLED:
			return fail_state;
			break;
		case END_OK:
		case END_WITH_TIMEOUT:
		default:
			return success_state;
			break;
	}
	return in_progress;
}

//Action qui gere un déplacement et renvoi le state rentré en arg. Ne s'arrète qu'à la fin que si aucun autre déplacement n'est demandé.
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, avoidance_type_e avoidance)
{
	error_e sub_action;
	//sub_action = goto_pos_with_scan_foe((displacement_t[]){{{x, y},FAST}},1,way,avoidance);
	sub_action = goto_pos_with_avoidance((displacement_t[]){{{x, y},speed}}, 1, way, avoidance, END_AT_LAST_POINT);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;
		case FOE_IN_PATH:
		case NOT_HANDLED:
			return fail_state;
			break;
		case END_OK:
		case END_WITH_TIMEOUT:
		default:
			return success_state;
			break;
	}
	return in_progress;
}


//Action qui gere un déplacement et renvoi le state rentré en arg. Cette fonction permet le multipoint.
Uint8 try_going_multipoint(displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, ASSER_end_condition_e end_condition)
{
	error_e sub_action;
	sub_action = goto_pos_with_avoidance(displacements, nb_displacements, way, avoidance, end_condition);
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

Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed)
{
	error_e sub_action;
	sub_action = goto_angle(angle, speed);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
		break;
		case NOT_HANDLED:
			return fail_state;
		break;
		case END_OK:
		case END_WITH_TIMEOUT:
		default:
			return success_state;
		break;
	}
	return in_progress;
}

Uint8 try_relative_move(Sint16 distance, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition)
{
	error_e sub_action;
	sub_action = relative_move(distance, speed, way, end_condition);
	switch(sub_action){
		case IN_PROGRESS:
			return in_progress;
			break;

		case FOE_IN_PATH:
		case NOT_HANDLED:
			return fail_state;
			break;

		case END_OK:
		case END_WITH_TIMEOUT:
		default:
			return success_state;
			break;
	}
	return in_progress;
}

/* Action va à une position relative */
error_e relative_move (Sint16 d, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition)
{
	/* Gestion de la machine à états */
	enum state_e {
		COMPUTE_AND_GO,
		GOING
	};
	static enum state_e state = COMPUTE_AND_GO;

	static bool_e timeout=FALSE;
	Sint32 x,y;
	Sint16 cosinus, sinus;

	switch (state)
	{
		case COMPUTE_AND_GO :
			//STACKS_flush(ASSER);
			/* Si la distance fournie est négative, on inverse la direction */
			if (d < 0) {
				if (way == BACKWARD) way = FORWARD;
				if (way == FORWARD) way = BACKWARD;
			}
			/* Si l'utilisateur demande d'aller en arrière, on inverse la direction */
			else if (way == BACKWARD) {
				d = -d;
			}
			COS_SIN_4096_get(global.env.pos.angle, &cosinus, &sinus);
			x = global.env.pos.x + ((d * (Sint32)cosinus)>>12);
			y = global.env.pos.y + ((d * (Sint32)sinus)>>12);

			//debug_printf("relative_move::current_pos x=%d y=%d\n", global.env.pos.x, global.env.pos.y);
			//debug_printf("relative_move::x=%f y=%f\n", x, y);
			if (x >= 0 && y >= 0) {
#ifdef USE_ASSER_MULTI_POINT
				ASSER_push_goto_multi_point((Sint16)x, (Sint16)y, speed, way, 0, END_OF_BUFFER, end_condition, TRUE);
#else
				ASSER_push_goto((Sint16)x, (Sint16)y, speed, way, 0, end_condition, TRUE);
#endif
				state = GOING;
				return IN_PROGRESS;
			}
			else {
				/* On ne gère pas les x ou y négatifs */
				state = COMPUTE_AND_GO;
				return NOT_HANDLED;
			}
			break;
		case GOING :
			if (STACKS_wait_end_auto_pull (ASSER, &timeout))
			{
				state = COMPUTE_AND_GO;
				return (timeout?END_WITH_TIMEOUT:END_OK);
			}
			break;
	}
	return IN_PROGRESS;
}


/* Action qui update la position */
error_e ACTION_update_position()
{
	enum state_e {
		SEND_CAN_MSG,
		WAIT_RECEPTION
	};
	static enum state_e state;

	switch (state)
	{
		case SEND_CAN_MSG :
			if (!global.env.pos.updated)
			{
				CAN_send_sid(ASSER_TELL_POSITION);
				state = WAIT_RECEPTION;
				return IN_PROGRESS;
			}
			else
			{
				return END_OK;
			}
			break;

		case WAIT_RECEPTION :
			if (global.env.pos.updated)
			{
				state = SEND_CAN_MSG;
				return END_OK;
			}
			break;
	}
	return IN_PROGRESS;
}

/* Action qui arrête le robot, met la position à jour */
error_e ACTION_asser_stop()
{
	enum state_e {
		SEND_CAN_MSG,
		WAIT_RECEPTION
	};
	static enum state_e state;

	static time32_t initial_time;

	switch (state)
	{
		case SEND_CAN_MSG :
			initial_time = global.env.match_time;
			STACKS_flush(ASSER);
			CAN_send_sid(ASSER_STOP);
			state = WAIT_RECEPTION;
			break;

		case WAIT_RECEPTION :
			if (global.env.asser.fini || (global.env.match_time-initial_time > (1000/*ms*/)))
			{
				state = SEND_CAN_MSG;
				return END_OK;
			}
			break;
	}
	return IN_PROGRESS;
}


static Uint16 wait_timeout = WAIT_TIME_DETECTION;

//Version simplifiée de wait_move_and_scan_foe. on esquive pas la cible ici. (pas de dodge)
error_e wait_move_and_scan_foe2(avoidance_type_e avoidance_type) {
	enum state_e
	{
		INITIALIZATION = 0,
		NO_FOE,
		WAIT_STOP,
		WAIT_FOE
	};
	static enum state_e state = INITIALIZATION;

	static time32_t avoidance_timeout_time = 0;
	static time32_t last_match_time;
	static bool_e debug_foe_forced = FALSE;
	static time32_t no_foe_count;
	time32_t current_match_time = global.env.match_time;

	bool_e timeout;
	error_e ret = IN_PROGRESS;

	//Si pas d'évitement, on fait pas d'évitement
	if(avoidance_type == NO_AVOIDANCE)
	{
		error_e asser_stack_state = AVOIDANCE_watch_asser_stack();
		switch(asser_stack_state)
		{
			case IN_PROGRESS:
				break;

			case END_OK:
			case END_WITH_TIMEOUT:
			case NOT_HANDLED:
				avoidance_printf("wait_move_and_scan_foe: end state = %d\n", asser_stack_state);
				ret = asser_stack_state;
				break;

			default: //Ne devrait jamais arriver, AVOIDANCE_watch_asser_stack ne doit pas retourner FOE_IN_PATH car elle ne gère pas d'evitement
				avoidance_printf("wait_move_and_scan_foe: DEFAULT asser_stack_state = %d!!\n", asser_stack_state);
				ret = NOT_HANDLED;
				break;
		}
	}
	else
	{
		switch(state)
		{
			case INITIALIZATION:
				// initialisation des variables statiques
				avoidance_timeout_time = 0;
				debug_foe_forced = FALSE;

				avoidance_printf("wait_move_and_scan_foe: initialized\n");
				state = NO_FOE;
				break;

			case NO_FOE:

				if(global.env.debug_force_foe)	//Evitement manuel forcé !
				{
					STACKS_flush(ASSER);
					debug_foe_reason(FORCED_BY_USER, 0, 0);
					ASSER_push_stop();
					state = WAIT_STOP;
					global.env.debug_force_foe = FALSE;
					debug_foe_forced = TRUE;	//Nous allons juste attendre le stop.. et puis on retournera un FOE_IN_PATH.
				}
				else
				{
					//Si on effectue un translation, c'est qu'on est en direction du point voulu (si le point était sur notre gauche, on aura fait une rotation au préalable)
					//Necessaire pour que l'angle de detection de l'adversaire soit valide (car sinon on ne pointe pas forcément vers notre point d'arrivé ...)
					//On considère ici que si la prop faire une translation, le robot pointe vers le point d'arrivée
		//			if((global.env.asser.current_trajectory != TRAJECTORY_TRANSLATION && global.env.asser.current_trajectory != TRAJECTORY_AUTOMATIC_CURVE) &&
		//				(is_in_path[FOE_1] || is_in_path[FOE_2]))
		//				avoidance_printf("Not in translation but foe in path\n");

					if(global.env.asser.is_in_translation && foe_in_path(TRUE))	//Si un adversaire est sur le chemin
					{	//On ne peut pas inclure le test du type de trajectoire dans le foe_in_path car ce foe_in_path sert également à l'arrêt, une fois qu'on a vu l'adversaire.
						//debug_foe_reason(foe, global.env.foe[foe].angle, global.env.foe[foe].dist);
						//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
						BUZZER_play(20, DEFAULT_NOTE, 3);
						switch(avoidance_type)
						{
							case NORMAL_WAIT:
							case NO_DODGE_AND_WAIT:
							case DODGE_AND_WAIT:
								avoidance_printf("wait_move_and_scan_foe: foe detected, waiting\n");

								// adversaire détecté ! on s'arrête !
								STACKS_push(ASSER, &wait_forever, FALSE);
								ASSER_push_stop();
								// un adversaire est détecté devant nous, on attend de s'arreter avant de voir s'il est toujours la
								state = WAIT_STOP;
								break;
							case DODGE_AND_NO_WAIT:
							case NO_DODGE_AND_NO_WAIT:
								avoidance_printf("wait_move_and_scan_foe: foe detected\n");
								STACKS_flush(ASSER);
								ASSER_push_stop();
								state = WAIT_STOP;
								break;

							case NO_AVOIDANCE: //Géré au début de la fonction
							default:
								state = INITIALIZATION;
								break;
						}
					}
					else
					{
						// aucun adversaire n'est détecté, on fait notre mouvement normalement
						// on regarde si la pile s'est vidée
						error_e asser_stack_state = AVOIDANCE_watch_asser_stack();
						switch(asser_stack_state)
						{
							case END_OK:
							case END_WITH_TIMEOUT:
							case NOT_HANDLED:
								avoidance_printf("wait_move_and_scan_foe: end no foe state = %d\n", asser_stack_state);
								ret = asser_stack_state;
								break;
							case IN_PROGRESS:
								break;
							default:
								break;
						}
					}
				}
				break;

			case WAIT_STOP:
				//Quand on s'est arreté, on regarde si l'adversaire est toujours devant nous avant de redémarrer
				if(STACKS_wait_end_auto_pull(ASSER, &timeout))
				{
					if(debug_foe_forced)
					{			//L'evitement a été forcé pour debuggage, on sort direct
						avoidance_printf("wait_move_and_scan_foe: forced foe detection, returning FOE_IN_PATH\n");
						debug_foe_forced = FALSE;
						ret = FOE_IN_PATH;
					}
					else
					{
						switch(avoidance_type) {
							case NORMAL_WAIT:
							case NO_DODGE_AND_WAIT:
							case DODGE_AND_WAIT:
								no_foe_count = 0;
								state = WAIT_FOE;
								break;

							case DODGE_AND_NO_WAIT:
							case NO_DODGE_AND_NO_WAIT:
								ret = FOE_IN_PATH;
								break;
							case NO_AVOIDANCE: //Géré au début de la fonction
							default:
								state = INITIALIZATION;
								break;
						}
					}
				}
				break;

			case WAIT_FOE:
				//debug_printf("Test 2: IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
				// on va attendre que l'ennemi sorte de notre chemin
				//On ne regarde pas ici si le robot pointe vers le point d'arrivée car il a été arreté en pleine translation vers ce point
				avoidance_timeout_time += current_match_time - last_match_time;
				if(avoidance_timeout_time >= wait_timeout)
				{
					avoidance_printf("wait_move_and_scan_foe: timeout avec ennemi sur path\n");
					ret = FOE_IN_PATH;
				}
				else if(foe_in_path(FALSE))	//Si on vient de recevoir un update de sa position et qu'il est toujours devant nous...
				{
					BUZZER_play(20, DEFAULT_NOTE, 1);
					no_foe_count = 0;	//On reset le compteur de no_foe.
				}
				else
				{
					no_foe_count += current_match_time - last_match_time;
					if(no_foe_count >= FOE_IS_LEFT_TIME) 	//L'adversaire est parti depuis FOE_IS_LEFT_TIME ms
					{
						avoidance_printf("wait_move_and_scan_foe: no more foe, continuing\n");
						STACKS_pull(ASSER);	// on vire le wait_forever et on lance l'action suivante
						state = NO_FOE;	// adversaire n'est plus dans notre chemin, on reprend le mouvement normal
					}
				}
				break;
			default:
				debug_printf("PROBLEME, on est dans le default !\n");
				ret = NOT_HANDLED;
				break;
		}
	}
	last_match_time = current_match_time;
	if(ret != IN_PROGRESS)
	{
		STACKS_flush(ASSER);	//TIMEOUT -> ON VIDE LE BUFFER PROPREMENT.
		state = INITIALIZATION;
	}
	return ret;
}

error_e goto_pos_with_scan_foe(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type) {
	return goto_pos_with_avoidance(displacements, nb_displacements, way, avoidance_type, END_AT_LAST_POINT);
}

error_e goto_pos_with_scan_foe_until_break(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type) {
	return goto_pos_with_avoidance(displacements, nb_displacements, way, avoidance_type, END_AT_BREAK);
}

void AVOIDANCE_set_timeout(Uint16 msec) {
	wait_timeout = msec;
}

/* Fonction qui réalise un ASSER_push_goto tout simple avec la gestion de l'évitement */
error_e goto_pos_with_avoidance(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, ASSER_end_condition_e end_condition)
{
	return goto_pos_curve_with_avoidance(displacements,NULL,nb_displacements,way,avoidance_type,end_condition);
}

/* Fonction qui réalise un ASSER_push_goto avec la possibilité de courbe ou non, avec la gestion de l'évitement */
error_e goto_pos_curve_with_avoidance(displacement_t displacements[], displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, ASSER_end_condition_e end_condition)
{
	enum state_e
	{
		CHECK_SCAN_FOE = 0,
		LOAD_MOVE,
		WAIT_MOVE_AND_SCAN_FOE,
		DONE
	};
	static enum state_e state = CHECK_SCAN_FOE;

	static bool_e timeout = FALSE;
	static error_e sub_action;

	Uint8 i;

	//Si nouveau déplacement et qu'aucun point n'est donné, on a rien a faire
	if(state == CHECK_SCAN_FOE && nb_displacements == 0)
		return END_OK;

	switch(state)
	{
		case CHECK_SCAN_FOE :
//			debug_printf("CHECK_SCAN_FOE\n");
//			if(foe_in_zone(TRUE, displacements[0].point.x, displacements[0].point.y)){
//				state = CHECK_SCAN_FOE;
//				return NOT_HANDLED;
//			}else
				state = LOAD_MOVE;
			break;


		case LOAD_MOVE:
			timeout = FALSE;
			for(i=nb_displacements-1;i>=1;i--)
			{
				if(displacements){
					#ifdef USE_ASSER_MULTI_POINT
						ASSER_push_goto_multi_point
							(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, ASSER_CURVES, END_OF_BUFFER, end_condition, FALSE);
					#else
						ASSER_push_goto
							(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, 0,END_AT_BREAK, FALSE);
					#endif
				}else{
					#ifdef USE_ASSER_MULTI_POINT
						ASSER_push_goto_multi_point
							(displacements_curve[i].point.x, displacements_curve[i].point.y, displacements_curve[i].speed, way, displacements_curve[i].curve?ASSER_CURVES:0, END_OF_BUFFER, end_condition, FALSE);
					#else
						ASSER_push_goto
							(displacements_curve[i].point.x, displacements_curve[i].point.y, displacements_curve[i].speed, way, 0,END_AT_BREAK, FALSE);
					#endif
				}
			}
			if(displacements){
				#ifdef USE_ASSER_MULTI_POINT
					ASSER_push_goto_multi_point
						(displacements[0].point.x, displacements[0].point.y, displacements[0].speed, way, ASSER_CURVES, END_OF_BUFFER, end_condition, TRUE);
				#else
					ASSER_push_goto
						(displacements[0].point.x, displacements[0].point.y, displacements[0].speed, way, 0,END_AT_BREAK, TRUE);
				#endif
			}else{
				#ifdef USE_ASSER_MULTI_POINT
					ASSER_push_goto_multi_point
						(displacements_curve[0].point.x, displacements_curve[0].point.y, displacements_curve[0].speed, way, displacements_curve[0].curve?ASSER_CURVES:0, END_OF_BUFFER, end_condition, TRUE);
				#else
					ASSER_push_goto
						(displacements_curve[0].point.x, displacements_curve[0].point.y, displacements_curve[0].speed, way, 0,END_AT_BREAK, TRUE);
				#endif
			}
						avoidance_printf("goto_pos_with_scan_foe : load_move\n");
			state = WAIT_MOVE_AND_SCAN_FOE;
			break;

		case WAIT_MOVE_AND_SCAN_FOE:
			sub_action = wait_move_and_scan_foe2(avoidance_type);
			switch(sub_action)
			{
				case END_OK:
					avoidance_printf("wait_move_and_scan_foe -- fini\n");
					state = DONE;
					break;

				case END_WITH_TIMEOUT:
					timeout = TRUE;
					avoidance_printf("wait_move_and_scan_foe -- timeout\n");
					SD_printf("TIMEOUT on WAIT_MOVE_AND_SCAN_FOE\n");
					state = DONE;
					break;

				case NOT_HANDLED:
					avoidance_printf("wait_move_and_scan_foe -- probleme\n");
					SD_printf("ERROR on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					state = CHECK_SCAN_FOE;
					return NOT_HANDLED;
					break;

				case FOE_IN_PATH:
					avoidance_printf("wait_move_and_scan_foe -- foe in path\n");
					SD_printf("FOE_IN_PATH on WAIT_MOVE_AND_SCAN_FOE\n");
					wait_timeout = WAIT_TIME_DETECTION;
					state = CHECK_SCAN_FOE;
					return FOE_IN_PATH;
					break;

				case IN_PROGRESS:
					break;

				default:
					state = CHECK_SCAN_FOE;
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			wait_timeout = WAIT_TIME_DETECTION;
			state = CHECK_SCAN_FOE;
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("Cas Default state, panique !!!\n");
			state = CHECK_SCAN_FOE;
			return NOT_HANDLED;
	}
	return IN_PROGRESS;
}




/* ----------------------------------------------------------------------------- */
/* 		Fonctions de scrutation de la position de l'adversaire           */
/* ----------------------------------------------------------------------------- */

/* Fonction qui regarde si le robot est dans notre chemin */
//Pas en static pour tests
/*static*/
bool_e foe_in_path(bool_e verbose)
{
	bool_e in_path;
	Sint16 cosinus, sinus;
	Sint32 relative_foe_x;
	Sint32 relative_foe_y;
	Uint8 i;
	way_e move_way;
	Uint32 breaking_acceleration;
	Uint32 current_speed;
	Uint32 break_distance;
	Uint32 respect_distance;
	Sint32 avoidance_rectangle_min_x;
	Sint32 avoidance_rectangle_max_x;
	Sint32 avoidance_rectangle_width_y;
	bool_e there_is_at_least_one_updated;

	move_way = global.env.asser.current_way;	//TODO cracra.. a nettoyer ultérieurement.

	in_path = FALSE;	//On suppose que pas d'adversaire dans le chemin

	if (!SWITCH_EVIT)
	{	//évitement désactivé par l'interrupteur
		static bool_e already_printed_debug_no_evitement = FALSE;
		if(already_printed_debug_no_evitement == FALSE) {
			already_printed_debug_no_evitement = TRUE;
			debug_printf("\n\nEVIT disabled by the switch. This message is printed only once.\n");
		}
		return FALSE;
	}
	there_is_at_least_one_updated = FALSE;
	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.env.foe[i].updated)
			there_is_at_least_one_updated = TRUE;
	}
	//if(!there_is_at_least_one_updated)
		//return FALSE;	//On économise la suite des calculs qui sont inutiles.


	/*	On définit un "rectangle d'évitement" comme la somme :
	 * 		- du rectangle que le robot va recouvrir s'il décide de freiner maintenant.
	 *  	- du rectangle de "respect" qui nous sépare de l'adversaire lorsqu'on se sera arreté
	 *  On calcule la position relative des robots adverses pour savoir s'ils se trouvent dans ce rectangle
	 *
	 */

	breaking_acceleration = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_ACCELERATION_NORMAL:BIG_ROBOT_ACCELERATION_NORMAL;
	current_speed = (Uint32)(absolute(global.env.pos.translation_speed));
	break_distance = current_speed*current_speed/(2*breaking_acceleration);	//distance que l'on va parcourir si l'on décide de freiner maintenant.
	respect_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_RESPECT_DIST_MIN:BIG_ROBOT_RESPECT_DIST_MIN;	//Distance à laquelle on souhaite s'arrêter

	if(move_way == FORWARD || move_way == ANY_WAY)	//On avance
		avoidance_rectangle_max_x = break_distance + respect_distance;
	else
		avoidance_rectangle_max_x = 0;

	if(move_way == BACKWARD || move_way == ANY_WAY)	//On recule
		avoidance_rectangle_min_x = -(break_distance + respect_distance);
	else
		avoidance_rectangle_min_x = 0;

	avoidance_rectangle_width_y = FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH);
	//avoidance_printf("\n%d[%ld>%ld][%ld>%ld]\n", current_speed,avoidance_rectangle_min_x,avoidance_rectangle_max_x,-avoidance_rectangle_width_y/2,avoidance_rectangle_width_y/2);

	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.env.foe[i].updated)
		{
			COS_SIN_4096_get(global.env.foe[i].angle, &cosinus, &sinus);
			relative_foe_x = (((Sint32)(cosinus)) * global.env.foe[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]
			relative_foe_y = (((Sint32)(sinus))   * global.env.foe[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]

			if(		relative_foe_y > -avoidance_rectangle_width_y/2 	&& 	relative_foe_y < avoidance_rectangle_width_y/2
				&& 	relative_foe_x > avoidance_rectangle_min_x 		&& 	relative_foe_x < avoidance_rectangle_max_x)
				{
					in_path = TRUE;	//On est dans le rectangle d'évitement !!!
					if(verbose)
						SD_printf("FOE[%d]_IN_PATH|d:%d|a:%d|rel_x%ld|rel_y%ld   RECT:[%ld>%ld][%ld>%ld]\n", i, global.env.foe[i].dist, global.env.foe[i].angle, relative_foe_x, relative_foe_y,avoidance_rectangle_min_x,avoidance_rectangle_max_x,-avoidance_rectangle_width_y/2,avoidance_rectangle_width_y/2);
				}
		}
	}
	return in_path;
}

bool_e foe_in_zone(bool_e verbose, Sint16 x, Sint16 y)
{
	bool_e inZone;
	Uint8 i;
	Sint32 a, b, c; // avec a, b et c les coefficients de la droite entre nous et la cible

	a = y - global.env.pos.y;
	b = x - global.env.pos.x;
	c = -global.env.pos.x*y + global.env.pos.y*x;

	if(global.env.pos.x == x && global.env.pos.y == y)
		return FALSE;


	inZone = FALSE;	//On suppose que pas d'adversaire dans le chemin

	for (i=0; i<MAX_NB_FOES; i++)
	{
		if (global.env.foe[i].updated){
			// d(D, A) < L
			// D : droite que le robot empreinte pour aller au point
			// A : Point adversaire
			// L : Largeur du robot maximum * 2

			debug_printf("Nous x:%d y:%d / ad x:%d y:%d \n ", global.env.pos.x, global.env.pos.y, global.env.foe[i].x, global.env.foe[i].y);

			if((Uint32)(absolute(a*global.env.foe[i].x + b*global.env.foe[i].y + c) / (float)sqrt(a*a + b*b)) < MARGE_COULOIR_EVITEMENT_STATIC){
				// NC.NA ¤ [0,NC*d]
				// NC : Vecteur entre nous et le point cible
				// NA : Vecteur entre nous et l'adversaire
				// d : distance d'évitement de l'adversaire (longueur couloir)

				if((global.env.pos.x*global.env.foe[i].y + global.env.pos.y*global.env.foe[i].x) > 0
						&& (global.env.pos.x*global.env.foe[i].y + global.env.pos.y*global.env.foe[i].x)
							 < dist_point_to_point(global.env.pos.x, global.env.pos.y, x, y)*DISTANCE_EVITEMENT_STATIC){
					inZone = TRUE;
					debug_printf("DETECTED\n");
				}
				else
					debug_printf("NO\n");
			}
			else
				debug_printf("NO\n");
		}
	}
	return inZone;
}

/* Fonction qui regarde si notre robot est immobile ou non */
//static bool_e AVOIDANCE_robot_translation_move()
//{
//	static bool_e result = FALSE;
//	static GEOMETRY_point_t old_pos = {0,0};
//	static time32_t last_time_update = 0;
//
//	if((global.env.match_time - last_time_update) > US_TRANSLATION_ANALYSE_TIME)
//	{
//		// on recalcule si on a fait une translation
//		if(absolute(global.env.pos.x - old_pos.x) < US_MINIMAL_MOVE)
//		{
//			if(absolute(global.env.pos.y - old_pos.y) < US_MINIMAL_MOVE)
//			{
//				result = FALSE;
//			}
//			else
//			{
//				result = TRUE;
//			}
//		}
//		else
//		{
//			result = TRUE;
//		}
//
//		// mise à jour des données sauvegardées
//		last_time_update = global.env.match_time;
//		old_pos.x = global.env.pos.x;
//		old_pos.y = global.env.pos.y;
//	}
//	return result;
//}


/*
 * Surveille l'execution de la pile ASSER. Renvoie vrai si toutes les fonctions sont finies
 * Rattrappe le robot quand il part en erreur.
 * Inscrit dans le booléen got_timeout si un timeout a été levé
 */
static error_e AVOIDANCE_watch_asser_stack ()
{
	bool_e timeout = FALSE;

	if (STACKS_wait_end_auto_pull(ASSER,&timeout))
	{
		return timeout?END_WITH_TIMEOUT:END_OK;
	}
	else if (global.env.asser.erreur)
	{
		STACKS_flush(ASSER);
		return NOT_HANDLED;
	}

	return IN_PROGRESS;
}

Uint8 try_stop(Uint8 in_progress, Uint8 success_state, Uint8 fail_state)
{
	typedef enum{
		STOP,
		WAIT_AND_CHECK
	}state_e;
	static state_e state = STOP;

	error_e subaction;

	switch(state){
		case STOP :
			ASSER_push_stop();
			state = WAIT_AND_CHECK;
			break;

		case WAIT_AND_CHECK :
			subaction = AVOIDANCE_watch_asser_stack();
			switch (subaction) {
				case END_OK:
					debug_printf("ASSER_STOP effectué\n");
					state = STOP;
					return success_state;
					break;

				case IN_PROGRESS:
					break;

				case END_WITH_TIMEOUT:
					debug_printf("ASSER_STOP effectué avec TIMEOUT\n");
					state = STOP;
					return fail_state;
					break;

				case NOT_HANDLED:
					debug_printf("ASSER_STOP erreur\n");
					state = STOP;
					return fail_state;
					break;

				default:
					debug_printf("ASSER_STOP erreur\n");
					state = STOP;
					return fail_state;
					break;
			}
			break;
	}
	return in_progress;
}


/*error_e AVOIDANCE_homologation(Sint16 x, Sint16 y, ASSER_speed_e speed, way_e way, Uint8 curve, bool_e run)
{
	static enum
	{
		INIT,
		GO,
		WAIT,
		DONE
	} state = INIT;

	switch (state)
	{
		case INIT:
			ASSER_push_goto(x, y,speed,way,curve,END_AT_LAST_POINT,run);
			state = GO;
			break;
		case GO:
			if((((global.env.match_time - global.env.foe[1].udapte_time) <= 1000) && global && global.env.foe[1].dist <= 400) ||
				(((global.env.match_time - global.env.foe[2].udapte_time) <= 1000) && global && global.env.foe[2].dist <= 400))
			{
				STACKS_push(ASSER, &wait_forever, TRUE);
				state = WAIT;
			}
			else if(STACKS_wait_end_auto_pull(ASSER, &timeout))
			{
				state=DONE;
			}
			break;

		case WAIT:
			if((((global.env.match_time - global.env.foe[1].udapte_time) >= 1000) || global && global.env.foe[1].dist <= 400) &&
				(((global.env.match_time - global.env.foe[2].udapte_time) >= 1000) || global && global.env.foe[2].dist <= 400))
			{
				STACKS_pull(ASSER);
				state = GO;
			}
			break;
		case DONE:
			break;
	}
	return IN_PROGRESS;
}*/

// Vérifie adversaire dans NORTH_US, NORTH_FOE, SOUTH_US, SOUTH_FOE
foe_pos_e AVOIDANCE_where_is_foe(Uint8 foe_id)
{
	assert(foe_id < MAX_NB_FOES);

	if(global.env.foe[foe_id].x > 1000)
	{
		// Partie SUD
		if(COLOR_Y(global.env.foe[foe_id].y) > 1500)
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
		if(COLOR_Y(global.env.foe[foe_id].y) > 1500)
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
	msg_to_send.size = 5;
	msg_to_send.data[0] = origin;
	msg_to_send.data[1] = angle >> 8;
	msg_to_send.data[2] = angle & 0xFF;
	msg_to_send.data[3] = distance >> 8;
	msg_to_send.data[4] = distance & 0xFF;
	CAN_send(&msg_to_send);
}

static Sint32 dist_point_to_point(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2){
	return sqrt((Sint32)(y1 - y2)*(y1 - y2) + (Sint32)(x1 - x2)*(x1 - x2));
}
