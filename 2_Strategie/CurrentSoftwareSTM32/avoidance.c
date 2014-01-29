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


/* Ne tient plus compte de la position de l'adversaire quand elle date
 * de plus de detection_timeout ms (detection_timeout doit etre superieur
 * à la base de temps de la carte stratégie)
 */
#define DETECTION_TIMEOUT				600 	// ancienne valeur : 375

#define DETECTION_DIST_MIN_KRUSTY       700  // distance minimale d'évitement sans compter la vitesse du robot, en mm
#define DETECTION_DIST_MIN_TINY         500
#define DETECTION_DIST_SPEED_FACTOR_KRUSTY  700  //temps pour que le robot s'arrete, en ms
#define DETECTION_DIST_SPEED_FACTOR_TINY    800


/***************************** Evitement 2011 **************************/
/* Les valeurs qui suivent sont totalement arbitraires by Mystery */

/* Temps avant un nouveau calcul de vitesse et de sens */
#define SPEED_COMPUTE_TIME	50		// en ms

/* Angle d'ouverture de vision */
#define DETECTION_ANGLE_NARROW		1900	//1900 = 20°
#define DETECTION_ANGLE_WIDE		3217	//3217 = 45°

#define WAIT_TIME_DETECTION			1000	// en ms


/* Constantes relatives à l'évitement */
#define MAX_AVOIDANCE_DETECTION			20		// nombre de détections maximal
#define TIME_BEFORE_NEW_DETECTION		1000	// en ms
#define MAX_DISTANCE_BEFORE_DESTINATION	400 	// en mm

/* Constantes pour les décalages du triple point */
#define X_AVOIDANCE_FOE_DISTANCE		350		// en mm
#define Y_AVOIDANCE_EXTERN_POINT		280		// en mm
#define Y_AVOIDANCE_CENTRAL_POINT		500		// en mm

/* Distance minimale d'analyse des mouvements du robot adversaire */
#define FOE_MINIMAL_MOVE	200	// en mm
/* Durée entre les analyses des mouvements adverses */
#define FOE_MOVE_ANALYSE_TIME	350	// en ms

/* Distance minimale d'analyse des mouvements de notre robot */
#define US_MINIMAL_MOVE		30	// en mm
/* Durée entre 2 calculs de translation de notre robot */
#define US_TRANSLATION_ANALYSE_TIME	80	// en ms

/* #define pour la gestion de collision */

/* distance de recul si asser erreur */
#define FORWARD_COLISION_MOVE 200

/* distance de recul si asser erreur */
#define BACKWARD_COLISION_MOVE	200

/* temps maximum pour aller à un noeud */
#define GO_TO_NODE_TIMEOUT 	10000 //10s

/* ----------------------------------------------------------------------------- */
/* 				Fonctions de scrutation de la position de l'adversaire           */
/* ----------------------------------------------------------------------------- */

/*
* Fonction qui regarde si le robot est dans notre chemin
*
* in_path = TRUE Quand l'adversaire est sur notre chemin
* in_path = FALSE Quand l'adversaire n'est pas sur le chemin
*/
//static void foe_in_path(bool_e in_path[NB_FOES]);
//Dans le .h

/* Fonction de calcul d'un indicateur de la vitesse et du sens de déplacement du robot
 * move_way = retourne le sens de déplacement
 */
//static Uint16 AVOIDANCE_speed_indicator_compute(void);

/* ----------------------------------------------------------------------------- */
/* 				Fonctions de génération de la trajectoire à 3 points             */
/* ----------------------------------------------------------------------------- */

/* Fonction qui exécute une esquive du robot adverse en 3 points
 * La fonction charge la pile !
 */
//static bool_e AVOIDANCE_foe_complex_dodge(way_e move_way, bool_e in_path[NB_FOES]);

/* Fonction qui calcule les 3 points d'esquive de l'adversaire
 * move_way : sens de déplacement
 * avoidance_way : 1 pour la gauche, -1 pour la droite
 * first_point : premier point calculé
 * second_point : deuxième point calculé
 * third_point : troisième point calculé
 */
//static bool_e AVOIDANCE_dodge_triple_points_compute(way_e move_way, Sint16 avoidance_way,
//	GEOMETRY_point_t* first_point, GEOMETRY_point_t* second_point, GEOMETRY_point_t* third_point, foe_e foe_id);

/* Fonction de calcul d'un point d'esquive de l'adversaire
 * move_way = sens de déplacement
 * x_distance = décalage en X par rapport au robot
 * y_distance = décalage en Y par rapport au robot
 * computed_way = facteur multiplicatif (1 indique à gauche, -1 à droite)
 * result_point = point calculé
 *
 * return : TRUE = point valide
 * 			FALSE = point invalide hors du terrain
 */
//static bool_e AVOIDANCE_compute_dodge_point(way_e move_way, Sint16 x_distance,
//	Sint16 y_distance, Sint16 computed_way, GEOMETRY_point_t* result_point);

//static error_e AVOIDANCE_move_colision();

static error_e AVOIDANCE_watch_asser_stack();

/* Fonction qui regarde si l'adversaire a bougé durant l'intervalle de temps précédent
 * time_for_analyse : temps entre les analyses de position
 *
 * return TRUE : l'adversaire n'a pas bougé pendant time_for_analyse
 * return FALSE : l'adversaire a bougé pendant time_for_analyse
 */
//static bool_e AVOIDANCE_foe_not_move(foe_e foe_id);

/* Fonction qui regarde si notre robot est immobile ou non
 * return TRUE : notre robot se déplace en translation
 * return FALSE : notre robot ne se déplace pas en translation (immobile ou rotation)
 */
//static bool_e AVOIDANCE_robot_translation_move();



#ifdef DEBUG_AVOIDANCE
	#define avoidance_printf(format, ...)	debug_printf("t=%lums " format, global.env.match_time, ##__VA_ARGS__)
#else
	#define avoidance_printf(...)	(void)0
#endif


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
	double x,y;

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
			x = ((double)global.env.pos.x) + ((double)d) * cos4096(global.env.pos.angle);
			y = ((double)global.env.pos.y) + ((double)d) * sin4096(global.env.pos.angle);

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

	static bool_e is_in_path[NB_FOES]; //Nous indique si l'adversaire est sur le chemin
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
					foe_in_path(is_in_path);//Regarde si les adversaires sont sur le chemin
					//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
					//Si on effectue un translation, c'est qu'on est en direction du point voulu (si le point était sur notre gauche, on aura fait une rotation au préalable)
					//Necessaire pour que l'angle de detection de l'adversaire soit valide (car sinon on ne pointe pas forcément vers notre point d'arrivé ...)
					//On considère ici que si la prop faire une translation, le robot pointe vers le point d'arrivée
		//			if((global.env.asser.current_trajectory != TRAJECTORY_TRANSLATION && global.env.asser.current_trajectory != TRAJECTORY_AUTOMATIC_CURVE) &&
		//				(is_in_path[FOE_1] || is_in_path[FOE_2]))
		//				avoidance_printf("Not in translation but foe in path\n");

					if((is_in_path[FOE_1] || is_in_path[FOE_2]) && global.env.asser.is_in_translation)
					{
						foe_origin_e foe = (is_in_path[FOE_1])? FOE1 : FOE2;
						debug_foe_reason(foe, global.env.foe[foe].angle, global.env.foe[foe].dist);
						//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
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
				foe_in_path(is_in_path);//Regarde si les adversaires sont sur le chemin
				//debug_printf("Test 2: IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
				// on va attendre que l'ennemi sorte de notre chemin
				//On ne regarde pas ici si le robot pointe vers le point d'arrivée car il a été arreté en pleine translation vers ce point

				avoidance_timeout_time += current_match_time - last_match_time;
				if(avoidance_timeout_time >= wait_timeout)
				{
					avoidance_printf("wait_move_and_scan_foe: timeout avec ennemi sur path\n");
					ret = FOE_IN_PATH;
				}
				else if(is_in_path[FOE_1] || is_in_path[FOE_2])
				{
					no_foe_count = 0;
				// Adversaire devant nous !
	/*
					if(AVOIDANCE_foe_not_move(FOE_1) && AVOIDANCE_foe_not_move(FOE_2))
					{
						asser_arg_t	asser_args;
						GEOMETRY_point_t destination_point;

						// L'adversaire ne bouge plus... on va regarder si le point d'arrivée est atteignable sans toucher l'adversaire
						// Donc en gros, le point est entre nous et l'adversaire (qui ne bouge pas !) et on a la place de s'y mettre
						debug_printf(" FOE 1 ou 2 bouge plus \n");
						GEOMETRY_point_t foe_point;
						foe_e current_foe;
						if(is_in_path[FOE_1])
						{
													debug_printf("iiiiii\n");
							foe_point.x = global.env.foe[FOE_1].x;
							foe_point.y = global.env.foe[FOE_1].y;
							current_foe = FOE_1;
						}
						else
						{
													debug_printf("uuuuu\n");
							foe_point.x = global.env.foe[FOE_2].x;
							foe_point.y = global.env.foe[FOE_2].y;
							current_foe = FOE_2;
						}
						// Calcul distance adversaire/destination
						asser_args = ASSER_get_stack_arg(STACKS_get_top(ASSER));
						destination_point.x = asser_args.x;
						destination_point.y = asser_args.y;

						// Si la destination est assez loin de l'adversaire (350 = taille des deux robots + marge)
						if(GEOMETRY_distance(destination_point,foe_point) > 350)
						{
							// La distance est supérieure ! On peut regarder si le point est entre nous et l'adversaire
							GEOMETRY_point_t us_point;
							us_point.x = global.env.pos.x;
							us_point.y = global.env.pos.y;
							if(GEOMETRY_distance(destination_point, us_point) < global.env.foe[current_foe].dist)
							{
								// On a une distance de nous à la destination inférieure à la distance entre nous et l'adversaire
								// Donc en gros, la destination est entre nous et l'adversaire
								// Donc, on y va !

								avoidance_printf("On relance notre chemin ! La dest est entre nous et l'adversaire %d !\n",current_foe);
								// on vire le wait_forever et on lance l'action suivante
								STACKS_pull(ASSER);

								// adversaire n'est plus dans notre chemin, on reprend le mouvement normal
								state = NO_FOE;
							}
	//						else {
	//                                            avoidance_printf("il n'y a pas de chemain posible donc on stop\n");
	//                                            ret = FOE_IN_PATH;
	//						}
						}
					}
					//*/

				}
				else
				{
					no_foe_count += current_match_time - last_match_time;
					if(no_foe_count >= 1000) {
						avoidance_printf("wait_move_and_scan_foe: no more foe, continuing\n");
						// on vire le wait_forever et on lance l'action suivante
						STACKS_pull(ASSER);

						// adversaire n'est plus dans notre chemin, on reprend le mouvement normal
						state = NO_FOE;
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
	enum state_e
	{
		LOAD_MOVE = 0,
		WAIT_MOVE_AND_SCAN_FOE,
		DONE
	};
	static enum state_e state = LOAD_MOVE;

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
			for(i=nb_displacements-1;i>=1;i--)
			{
				#ifdef USE_ASSER_MULTI_POINT
					ASSER_push_goto_multi_point
						(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, ASSER_CURVES, END_OF_BUFFER, end_condition, FALSE);
				#else
					ASSER_push_goto
						(displacements[i].point.x, displacements[i].point.y, displacements[i].speed, way, 0,END_AT_BREAK, FALSE);
				#endif
			}
			#ifdef USE_ASSER_MULTI_POINT
				ASSER_push_goto_multi_point
					(displacements[0].point.x, displacements[0].point.y, displacements[0].speed, way, ASSER_CURVES, END_OF_BUFFER, end_condition, TRUE);
			#else
				ASSER_push_goto
					(displacements[0].point.x, displacements[0].point.y, displacements[0].speed, way, 0, end_condition, TRUE);
			#endif
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
					state = DONE;
					break;

				case NOT_HANDLED:
					avoidance_printf("wait_move_and_scan_foe -- probleme\n");
					wait_timeout = WAIT_TIME_DETECTION;
					state = LOAD_MOVE;
					return NOT_HANDLED;
					break;

				case FOE_IN_PATH:
					avoidance_printf("wait_move_and_scan_foe -- foe in path\n");
					wait_timeout = WAIT_TIME_DETECTION;
					state = LOAD_MOVE;
					return FOE_IN_PATH;
					break;

				case IN_PROGRESS:
					break;

				default:
					state = LOAD_MOVE;
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
void foe_in_path(bool_e *in_path)
{
	// variables
	//Uint16 speed_indicator;
	Uint16 distance_computed_narrow;
	Uint16 distance_computed_wide;
	Uint8 i;
	way_e move_way;
	move_way = global.env.asser.current_way;	//TODO cracra.. a nettoyer ultérieurement.

	/* Si on n'a pas d'évitement, l'adversaire n'est jamais devant nous */
	if (!SWITCH_EVIT) {
		for (i=0; i<NB_FOES; i++)
			in_path[i] = FALSE;
		static bool_e already_printed_debug_no_evitement = FALSE;
		if(already_printed_debug_no_evitement == FALSE) {
			already_printed_debug_no_evitement = TRUE;
			debug_printf("No evitement");
		}
		return;
	}


	//on identifie une distance par rapport à la vitesse
	//distance_computed = ((speed_indicator*52) >>2) + 400;		// DISTANCE 2011
	//distance_computed = ((speed_indicator*52) >>2) + 240;
	//distance_computed = 600;

	if(QS_WHO_AM_I_get() == TINY)
	{
		distance_computed_narrow = DETECTION_DIST_MIN_TINY + ((Uint32)DETECTION_DIST_SPEED_FACTOR_TINY)*(absolute(global.env.pos.translation_speed))/1000;  //8*125 = 1000, DETECTION_DIST_SPEED_FACTOR_TINY est en milisecondes
		distance_computed_wide = DETECTION_DIST_MIN_TINY;
	}
	else
	{
		distance_computed_narrow = DETECTION_DIST_MIN_KRUSTY + ((Uint32)DETECTION_DIST_SPEED_FACTOR_KRUSTY)*(absolute(global.env.pos.translation_speed))/1000;
		distance_computed_wide = DETECTION_DIST_MIN_KRUSTY;
	}

//	avoidance_printf("D=%d , DF0=%d, DF1=%d ",distance_computed,global.env.foe[0].dist,global.env.foe[1].dist);
	//debug_printf("la vitesse %d",((speed_indicator*52) >>2) + 240);
	for (i=0; i<NB_FOES; i++)
	{
		//TODO: a enlever
		static time32_t last_printf = 0;
		if(global.env.match_time > last_printf + 1000) {
			last_printf = global.env.match_time;
			if(global.env.foe[i].dist < 5000)
				avoidance_printf("FOE[%d] dist = %d mm (limit: %d mm), angle: %d, way: %d%s%s\n", i, global.env.foe[i].dist, distance_computed_narrow, global.env.foe[i].angle, move_way, (global.env.asser.is_in_translation)? ", in_translation" : "", (global.env.asser.is_in_translation)? ", in_rotation" : "");
		}
		in_path[i] = FALSE; //On initialise à faux
		if ((global.env.match_time - global.env.foe[i].update_time)<(DETECTION_TIMEOUT))
		{
			// on regarde en fonction de notre sens de déplacement
			//Si on a un ANY_WAY, c'est que la prop ne fait pas de translation => pas de detection d'ennemi dans ce cas
			if(move_way == FORWARD || move_way == ANY_WAY)
			{
				//debug_printf("F_%d\nG_%d\n",global.env.foe[0].angle,global.env.foe[1].angle);
				/* On regarde si l'adversaire est dans un gabarit devant nous */
				if(((global.env.foe[i].dist < distance_computed_narrow) && (global.env.foe[i].angle > (-DETECTION_ANGLE_NARROW) && global.env.foe[i].angle < DETECTION_ANGLE_NARROW)) ||
				   ((global.env.foe[i].dist < distance_computed_wide)   && (global.env.foe[i].angle > (-DETECTION_ANGLE_WIDE)   && global.env.foe[i].angle < DETECTION_ANGLE_WIDE)  )    )
				{
					in_path[i] = TRUE;
				}
			}

			if(move_way == BACKWARD || move_way == ANY_WAY)
			{
		//		debug_printf("B_%d\n",global.env.foe.angle);
				/* On regarde si l'adversaire est dans un gabarit derrière nous */
				if(((global.env.foe[i].dist < distance_computed_narrow) && (global.env.foe[i].angle < (-PI4096+DETECTION_ANGLE_NARROW) || global.env.foe[i].angle > PI4096-DETECTION_ANGLE_NARROW)) ||
				   ((global.env.foe[i].dist < distance_computed_wide)   && (global.env.foe[i].angle < (-PI4096+DETECTION_ANGLE_WIDE)   || global.env.foe[i].angle > PI4096-DETECTION_ANGLE_WIDE)  )    )
				{
					in_path[i] = TRUE;
				}
			}
		}
	}
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
foe_pos_e AVOIDANCE_where_is_foe(foe_e foe_id)
{
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
