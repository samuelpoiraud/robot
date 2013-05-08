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


/* Ne tient plus compte de la position de l'adversaire quand elle date
 * de plus de detection_timeout ms (detection_timeout doit etre superieur
 * à la base de temps de la carte stratégie)
 */
#define DETECTION_TIMEOUT				600 	// ancienne valeur : 375

#define DETECTION_DIST_MIN_KRUSTY       700  // distance minimale d'évitement sans compter la vitesse du robot, en mm
#define DETECTION_DIST_MIN_TINY         600
#define DETECTION_DIST_SPEED_FACTOR_KRUSTY  700  //temps pour que le robot s'arrete, en ms
#define DETECTION_DIST_SPEED_FACTOR_TINY    800


/***************************** Evitement 2011 **************************/
/* Les valeurs qui suivent sont totalement arbitraires by Mystery */

/* Temps avant un nouveau calcul de vitesse et de sens */
#define SPEED_COMPUTE_TIME	50		// en ms

/* Angle d'ouverture de vision */
#define DETECTION_ANGLE_NARROW		1900	//1900 = 20°
#define DETECTION_ANGLE_WIDE		3217	//3217 = 45°

#define WAIT_TIME_DETECTION			3000	// en ms //TODO: diminuer ce coef (c'etait pour des tests de wait et distance d'evitement)


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
static Uint16 AVOIDANCE_speed_indicator_compute(void);

/* ----------------------------------------------------------------------------- */
/* 				Fonctions de génération de la trajectoire à 3 points             */
/* ----------------------------------------------------------------------------- */

/* Fonction qui exécute une esquive du robot adverse en 3 points 
 * La fonction charge la pile !
 */
static bool_e AVOIDANCE_foe_complex_dodge(way_e move_way, bool_e in_path[NB_FOES]);

/* Fonction qui calcule les 3 points d'esquive de l'adversaire 
 * move_way : sens de déplacement
 * avoidance_way : 1 pour la gauche, -1 pour la droite
 * first_point : premier point calculé
 * second_point : deuxième point calculé
 * third_point : troisième point calculé
 */
static bool_e AVOIDANCE_dodge_triple_points_compute(way_e move_way, Sint16 avoidance_way,
	GEOMETRY_point_t* first_point, GEOMETRY_point_t* second_point, GEOMETRY_point_t* third_point, foe_e foe_id);

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
static bool_e AVOIDANCE_compute_dodge_point(way_e move_way, Sint16 x_distance, 
	Sint16 y_distance, Sint16 computed_way, GEOMETRY_point_t* result_point);

static error_e AVOIDANCE_move_colision();

static error_e AVOIDANCE_watch_asser_stack();
	
/* Fonction qui regarde si l'adversaire a bougé durant l'intervalle de temps précédent 
 * time_for_analyse : temps entre les analyses de position
 *
 * return TRUE : l'adversaire n'a pas bougé pendant time_for_analyse
 * return FALSE : l'adversaire a bougé pendant time_for_analyse
 */
static bool_e AVOIDANCE_foe_not_move(foe_e foe_id);

/* Fonction qui regarde si notre robot est immobile ou non 
 * return TRUE : notre robot se déplace en translation
 * return FALSE : notre robot ne se déplace pas en translation (immobile ou rotation)
 */
static bool_e AVOIDANCE_robot_translation_move();

//Comme goto_pos_with_scan_foe mais avec choix de la fin, je savais pas quoi mettre comme nom ... si quelqu'un a une idée de nom utile, go changer ça :)
static error_e goto_pos_with_avoidance(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, ASSER_end_condition_e end_condition);


#ifdef DEBUG_AVOIDANCE
	#define avoidance_printf(format, ...)	debug_printf("t=%lums " format, global.env.match_time, ##__VA_ARGS__)
#else
	#define avoidance_printf(...)	(void)0
#endif


/* Action qui fait une trajectoire courbe */
error_e smooth_goto (Sint16 x, Sint16 y, Sint16 angle, Uint8 precision)
{

	enum state_e {
		COMPUTE_CONTROL_POINTS,
		COMPUTE_NEXT_POINT,
		GOING
	};
	static enum state_e state = COMPUTE_CONTROL_POINTS;

	// Points de contrôles
	static Sint16 Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;

	//relevé de timeout
	static bool_e timeout = FALSE;

	// Variable
	static double a, b, d;

	switch (state) {
		case COMPUTE_CONTROL_POINTS :
			timeout = FALSE;
			
			// On prend pour longueur des segments de contrôle la moitié de
			// la distance entre le point de départ et le point d'arrivée.
			// TODO utiliser plutot la vitesse pour determiner sa taille
			d = sqrt(
				(double)(global.env.pos.x - x)*(global.env.pos.x - x) +
				(double)(global.env.pos.y - y)*(global.env.pos.y - y));

			// départ
			Ax = global.env.pos.x;
			Ay = global.env.pos.y;
			// point de de contrôle départ
			Bx =  Ax + d/2 * cos4096(global.env.pos.angle);
			By =  Ay + d/2 * sin4096(global.env.pos.angle);

			// arrivée
			Dx = x;
			Dy = y;
			// point de contrôle arrivée
			// on ajoute pi car angle est l'angle d'arrivée du robot
			Cx = Dx + d/2 * cos4096(angle + PI4096);
			Cy = Dy + d/2 * sin4096(angle + PI4096);

			// On commence au "deuxième cran" car le robot est déja
			// au point de départ
			a = 1.0 - 1.0 / precision;

			STACKS_flush(ASSER);

			state = COMPUTE_NEXT_POINT;
			break;

		case COMPUTE_NEXT_POINT :

			/* We will not actually draw a curve, but we will divide the curve into small
				points and draw a line between each point. If the points are close enough, it
				will appear as a curved line. 20 points are plenty, and since the variable goes
				from 1.0 to 0.0 we must change it by 1/20 = 0.05 each time */
			if (a >= 0) 
			{
				b = 1.0 - a;
				// Get a point on the curve
				ASSER_push_goto(
					Ax*a*a*a + Bx*3*a*a*b + Cx*3*a*b*b + Dx*b*b*b, // x
					Ay*a*a*a + By*3*a*a*b + Cy*3*a*b*b + Dy*b*b*b, // y
					SLOW, ANY_WAY, 0,END_AT_LAST_POINT,TRUE
				);

				a -= 1.0 / precision;
				state = GOING;
			}
			else
			{
				state = COMPUTE_CONTROL_POINTS;
				return timeout?END_WITH_TIMEOUT:END_OK;
			}
			break;

		case GOING :
			if (STACKS_wait_end_auto_pull (ASSER, &timeout))
			{
				state = COMPUTE_NEXT_POINT ;
			}
			break;
		
		default : 
			break;
	}
	return IN_PROGRESS;
}

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

	/* Action qui va à un noeud */
	error_e goto_node (Uint8 node, ASSER_speed_e speed, way_e way)
	{
		Uint8 i;

		/* Gestion de la machine à états */
		enum state_e {
			RESTART_STATE_MACHINE=0,/* retourner ici quand on renvoie un END */
			COMPUTE_AND_GO ,
			GOING,
			WAIT_FOE,
			STOP_UPDATE_POS_THEN_COMPUTE_AND_GO
		};
		static enum state_e state = RESTART_STATE_MACHINE;
	
		/* Prend en compte l'évitement */
		static bool_e enable_avoidance[NB_FOES];
		
		/* relève les timeout */
		static bool_e timeout = FALSE;
	
		/* Pour gérer les timeouts */
		static time32_t start_time = 0;
		static time32_t avoidance_start_time = 0;
	
		/* Destination courante, qui peut différer de node en cas d'évitement */
		static pathfind_node_id_t current_destination;
	
		for (i=0; i < NB_FOES; i++)
		{
			enable_avoidance[i] = FALSE;
			
			if (global.env.config.evitement &&
				 (global.env.match_time - global.env.foe[i].update_time < (DETECTION_TIMEOUT)))
			{
				enable_avoidance[i] = TRUE; //Prend en compte l'adversaire i dans la recherche du chemin
				if (global.env.foe[i].updated) 
				{
					PATHFIND_updateOpponentPosition(i); //Récupère le noeud le plus proche de l'adversaire
					if (PATHFIND_is_opponent_in_path(i)) //Regarde si le noeud est dans la pile ASSER
					{
						avoidance_printf("[GOTO_NODE]opponent %d is in path\n", i);
						STACKS_flush(ASSER);
						state = STOP_UPDATE_POS_THEN_COMPUTE_AND_GO;
						break;
					}
				}
			}

		}
		
		
		switch (state)
		{
			case RESTART_STATE_MACHINE :
				avoidance_printf("GO TO NODE RESTART_STATE_MACHINE\n");
				start_time = global.env.match_time;
				// no break, compute and go now
			case COMPUTE_AND_GO :
				avoidance_printf("GO TO NODE COMPUTE AND GO\n");
				timeout = FALSE;
	
				STACKS_flush(ASSER);
				
				current_destination = node;
				
				if ( (enable_avoidance[FOE_1] && PATHFIND_opponent_node(FOE_1) == current_destination) 
					|| (enable_avoidance[FOE_2] && PATHFIND_opponent_node(FOE_2) == current_destination) )
				{
					current_destination = PATHFIND_random_neighbor(node, enable_avoidance);
				}
				
				avoidance_printf("[GOTO NODE] Current destination = %d\n", current_destination);
	
				// On calcule le chemin, si l'adversaire est detecté, on le prend en compte dans le calcul
				// Si l'opposant n'est pas dans le chemin, on y va
				Uint16 cost = PATHFIND_compute(global.env.pos.x, global.env.pos.y, current_destination, speed, way, global.env.config.evitement);
				/* le noeud d'arrivée ne doit pas être le plus proche de l'adversaire */
				//#warning "assert(cost != PATHFIND_OPPONENT_COST);" on ne peut pas reboot le code comme ca
				if(cost == PATHFIND_OPPONENT_COST)
				{
					avoidance_printf("[GOTO NODE] cost == PATHFIND_OPPONENT_COST\n");
					STACKS_flush(ASSER);
					state = RESTART_STATE_MACHINE;
					return NOT_HANDLED;
				}
				
				STACKS_pull(ASSER); /* On enlève le wait_forever */
				
				state = GOING;
				avoidance_printf("[GOTO NODE] new state:GOING\n");
				return IN_PROGRESS;
				break;
	
			case GOING :
				avoidance_printf("GO TO NODE GOING\n");
				//avoidance_printf("ANGLE _FOE_1 %d ANGLE _FOE_2 %d\n", global.env.foe[FOE_1].angle, global.env.foe[FOE_2].angle);
				//avoidance_printf("DISTANCE _FOE_1 %d DISTANCE _FOE_2 %d\n", global.env.foe[FOE_1].dist, global.env.foe[FOE_2].dist);
				if(STACKS_wait_end_auto_pull(ASSER, &timeout))
				{
					avoidance_printf("Stacks\n");
					if (current_destination == node)
					{
						avoidance_printf("[GOTO NODE] Restart_Machine\n");
						state = RESTART_STATE_MACHINE;
						return END_OK;
					}
					else
					{
						avoidance_printf("[GOTO NODE] Stop update pos then compute\n");
						//on est à un node a coté pour evitement
						state = STOP_UPDATE_POS_THEN_COMPUTE_AND_GO;
					}
				}
				else if (global.env.match_time - start_time > GO_TO_NODE_TIMEOUT) 
				//Si cela fait plus de 10 s qu'on essaye d'aller au noeud
				{
					state = RESTART_STATE_MACHINE;
					STACKS_flush(ASSER);
					return NOT_HANDLED;
				}
				else if (global.env.config.evitement && 
					((global.env.foe[FOE_1].dist <500 && (global.env.foe[FOE_1].angle > -PI4096/3) && (global.env.foe[FOE_1].angle < PI4096/3)) ||
					(global.env.foe[FOE_2].dist <500 && ((global.env.foe[FOE_2].angle > -PI4096/3) && (global.env.foe[FOE_2].angle < PI4096/3)))))
				{
					avoidance_printf("FOE DETECTED\n");
					STACKS_push(ASSER, &wait_forever, TRUE);
					ASSER_push_stop();
					avoidance_start_time = global.env.match_time;
					state = WAIT_FOE;
				}	
				
				//evitement des adversaires devant le robot
				//ne devrait pas etre déclenché car un adversaire devant le robot
				//devrait provoquer le calcul d'un nouveau chemin.
				
				
				return IN_PROGRESS;
				break;
				
			case WAIT_FOE: 
				avoidance_printf("GO TO NODE WAIT FOE\n");
				if ((global.env.foe[FOE_1].dist >600 || global.env.foe[FOE_1].angle < -PI4096/3 || global.env.foe[FOE_1].angle > PI4096/3) &&
				(global.env.foe[FOE_2].dist >600 || global.env.foe[FOE_2].angle < -PI4096/3 || global.env.foe[FOE_2].angle > PI4096/3))
				{
					STACKS_pull(ASSER);
					avoidance_printf("FOE DONE\n");
					state = GOING;
				}
				else if (global.env.match_time - avoidance_start_time > 3000)
				{
					return FOE_IN_PATH;
				}
				return IN_PROGRESS;
				break;
	
			case STOP_UPDATE_POS_THEN_COMPUTE_AND_GO :
				avoidance_printf("GO TO NODE STOP_UPDATE_POS_THEN_COMPUTE_AND_GO\n");
			
				if (ACTION_asser_stop() == END_OK)
				{
					state = COMPUTE_AND_GO;
				}
				return IN_PROGRESS;
				break;
	
			default : 
				break;
		}
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


//Action qui gere un déplacement et renvoi le state rentré en arg
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance)
{
	error_e sub_action;
	sub_action = goto_pos_with_scan_foe((displacement_t[]){{{x, y},FAST}},1,way,avoidance);
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
}

Uint8 try_going_slow(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance)
{
	error_e sub_action;
	sub_action = goto_pos_with_scan_foe((displacement_t[]){{{x, y},SLOW}},1,way,avoidance);
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
}

//Action qui gere un déplacement et renvoi le state rentré en arg. Cette fonction permet le multipoint.
Uint8 try_going_multipoint(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance, ASSER_end_condition_e end_condition, Uint8 in_progress, Uint8 success_state, Uint8 fail_state)
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
}

Uint8 try_relative_move(Sint16 distance, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition, Uint8 in_progress, Uint8 success_state, Uint8 fail_state) {
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

/***************************** Evitement 2010 **************************/
/* Gère les colisions */
error_e move_colision()
{
	#ifdef USE_POLYGON
		avoidance_printf("USE_POLYGON activé, move_collision indisponible !\n");
		return NOT_HANDLED;
	#else

	enum state_e {
		WAIT_COLISION = 0,
		MOVE_BACK,
		WAIT_MOVE_BACK,
		COMPUTE,
		GOTO_NODE,
		DONE
	};
	static enum state_e state = WAIT_COLISION;
	
	error_e sub_action;

	static pathfind_node_id_t bestNode;


	switch(state)
	{
		case WAIT_COLISION:
			if(global.env.asser.erreur)
			{
				avoidance_printf("move_colision : asser.erreur\n");
				state = MOVE_BACK;
			}
			else
			{
				return END_OK;
			}
			break;

		case MOVE_BACK:
			sub_action = relative_move(BACKWARD_COLISION_MOVE,FAST,BACKWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case END_OK : 
					state = COMPUTE;
					avoidance_printf("Recolte:En Translation");
					break;

				case END_WITH_TIMEOUT :
				case NOT_HANDLED: 
					state = COMPUTE;
					break;
				
				case IN_PROGRESS :
					break;
				default:
					break;
			}
			break;

		case COMPUTE:
			/* On calcule la distance minimale pour rejoindre tous les noeuds */
			bestNode = PATHFIND_closestNode(global.env.pos.x, global.env.pos.y, TRUE);
			/* Je déplace le robot au noeud choisi qui doit être le plus près */
			/* Dans le haut de la fonction, sinon on va perdre le robot quand on bouge */
			/* C'est moin dur le code à 19h23 du matin */
			state = GOTO_NODE;
			break;

		case GOTO_NODE:
			switch (goto_node(bestNode, VERY_SLOW_TRANSLATION_AND_FAST_ROTATION, ANY_WAY))
			{	
				case END_OK :
					state = DONE;
					break;

				case END_WITH_TIMEOUT :
				case NOT_HANDLED :
					state = COMPUTE; 
					break;

				case FOE_IN_PATH :
					/*si l'ennemi est encore sur ma route (enfoiré ^^) */
					state = COMPUTE;
					break;

				case IN_PROGRESS :
					break;
				default :
					break;
			}
			break;

		case DONE:
			state = WAIT_COLISION;
			return END_WITH_TIMEOUT;
			break;

		default :
			state = WAIT_COLISION;
			break;
	}
	return IN_PROGRESS;
	#endif
}

/***************************** Evitement 2011 **************************/
/* Fonction qui regarde si l'adversaire est devant nous pendant un mouvement, et on l'évite si nécessaire 
 * Elle doit être appelée à la place de STACKS_wait_end_auto_pull (c'est géré dans cette fonction
 * En retour, on a :
 * END_OK : on est arrivé à destination dans les temps
 * END_WITH_TIMEOUT : soit on est arrivé à destination en ayant évité l'adversaire, soit on a eu un timeout de la pile ASSER
 * NOT_HANDLED : Erreur déplacement impossible
 * FOE_IN_PATH : l'évitement est trop long à se faire correctement, on prévient la fonction au dessus qui doit agir en conséquence
 */
error_e wait_move_and_scan_foe(avoidance_type_e avoidance_type)
{
	enum state_e
	{
		INITIALIZATION = 0,
		NO_FOE,
		WAIT_FOE,
		DODGE,
		WAIT_DODGE,
		DONE
	};
	static enum state_e state = INITIALIZATION;

	static bool_e timeout;
	static Uint16 nb_detection;
	static time32_t detection_time;
	static bool_e dodge_wait;
	static bool_e is_in_path[NB_FOES]; //Nous indique si l'adversaire est sur le chemin

	asser_arg_t	asser_args;
	GEOMETRY_point_t current_point, destination_point;
	Sint16 distance_before_destination;


	switch(state)
	{
		case INITIALIZATION:
			// initialisation des variables statiques
			timeout = FALSE;
			nb_detection = 0;
			detection_time = 0;
			dodge_wait = FALSE;

			avoidance_printf("wait_move_and_scan_foe initialized\n");
			state = NO_FOE;
			break;

		case NO_FOE:
			foe_in_path(is_in_path);//Regarde si les adversaires sont sur le chemin
			//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
			if((is_in_path[FOE_1] || is_in_path[FOE_2]) /*&& AVOIDANCE_robot_translation_move()*/)
			{
				//debug_printf("IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
				avoidance_printf("nb_detection=%d\n",nb_detection+1);
				switch(avoidance_type)
				{
					case NORMAL_WAIT:
					case NO_DODGE_AND_WAIT:
						// adversaire détecté ! on s'arrête !
						STACKS_push(ASSER, &wait_forever, FALSE);
						ASSER_push_stop();

						if(nb_detection > MAX_AVOIDANCE_DETECTION)
						{
							// on a fait des évitements trop souvent, il faut faire autre chose
							avoidance_printf("Ennemi détecté trop de fois, point inatteignable : FOE_IN_PATH\n");
							STACKS_flush(ASSER);
							state = INITIALIZATION;
							return FOE_IN_PATH;
						}
						else
						{
							// un adversaire est détecté devant nous
							avoidance_printf("ENNEMI DETECTE DEVANT NOUS\n");
							nb_detection++;
							// on met le timeout à jour pour se souvenir qu'on a détecté l'adversaire et qu'on va l'éviter
							timeout = TRUE;
							// on enregistre le temps de détection
							detection_time = global.env.match_time;
							state = WAIT_FOE;
						}
						break;
					case DODGE_AND_WAIT:
					case DODGE_AND_NO_WAIT:
						if(nb_detection > MAX_AVOIDANCE_DETECTION)
						{
							// on a fait des évitements trop souvent, il faut faire autre chose
							avoidance_printf("Ennemi détecté trop de fois, point inatteignable : FOE_IN_PATH\n");
							STACKS_flush(ASSER);
							state = INITIALIZATION;
							return FOE_IN_PATH;
						}
						else
						{
							// un adversaire est détecté devant nous
							avoidance_printf("ENNEMI DETECTE DEVANT NOUS\n");
							nb_detection++;
							// on met le timeout à jour pour se souvenir qu'on a détecté l'adversaire et qu'on va l'éviter
							timeout = TRUE;
							// on enregistre le temps de détection
							detection_time = global.env.match_time;
							state = DODGE;
						}
						break;
					case NO_DODGE_AND_NO_WAIT:
						avoidance_printf("Aucune esquive, aucune attente !\n");
						ASSER_push_stop();
						STACKS_flush(ASSER);
						state = INITIALIZATION;
						return FOE_IN_PATH;
						break;
					case NO_AVOIDANCE:
						// on ne fait rien...
						if(STACKS_wait_end_auto_pull(ASSER,&timeout))
						{
							state = DONE;
						}
						break;
					default:
						state = INITIALIZATION;
						break;
				}
			}
			else
			{
				// aucun adversaire n'est détecté, on fait notre mouvement normalement
				// on regarde si la pile s'est vidée
				switch(AVOIDANCE_watch_asser_stack())
				{
					case END_OK: 
						avoidance_printf("Pile ASSER vide\n");
						state = DONE;
						break;
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						state = INITIALIZATION;
						return NOT_HANDLED;
						break;
					case IN_PROGRESS:
						break;
					default:
						break;
				}
			}
			break;

		case WAIT_FOE:
			foe_in_path(is_in_path);//Regarde si les adversaires sont sur le chemin
			//debug_printf("Test 2: IN_PATH[FOE1] = %d, IN_PATH[FOE1] = %d, robotmove = %d\n", is_in_path[FOE_1], is_in_path[FOE_2], AVOIDANCE_robot_translation_move());
			// on va attendre que l'ennemi sorte de notre chemin
			/*if((is_in_path[FOE_1] && !AVOIDANCE_foe_not_move(FOE_1)) || 
				(is_in_path[FOE_2] && !AVOIDANCE_foe_not_move(FOE_2)))
			*/
			// Adversaire devant nous !
			if(is_in_path[FOE_1] || is_in_path[FOE_2])
			{
                            
				if(!AVOIDANCE_foe_not_move(FOE_1) || !AVOIDANCE_foe_not_move(FOE_2))
				{
                                        debug_printf(" FOE 1 ou 2 bouge \n");
					// adversaire toujours devant nous, et se déplace, il nous gène donc !
					// On va donc patienter le temps qu'il se barre ou qu'il s'arrête
					if(global.env.match_time - detection_time > WAIT_TIME_DETECTION)
					{
						if(avoidance_type == NO_DODGE_AND_WAIT || avoidance_type == NO_DODGE_AND_NO_WAIT)
						{
							// on va sortir de la fonction en retournant FOE_IN_PATH
							avoidance_printf("Pas d'esquive après attente, on sort\n");
							STACKS_flush(ASSER);
							state = INITIALIZATION;
							return FOE_IN_PATH;
						}
						else
						{
							// on a dépassé le temps d'attente, on va regarder si on fait une esquive
							avoidance_printf("Temps d'attente dépassé !!!\n");
							detection_time = global.env.match_time;
							state = DODGE;
						}
					}
				}
				else
				{
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
					//}else{
                                        //    avoidance_printf("il n'y a pas de chemain posible donc on stop\n");
                                        //    STACKS_flush(ASSER);
                                        //    state = INITIALIZATION;
                                         //   return FOE_IN_PATH;}
					}
				}
                            
			}
			else
			{
				avoidance_printf("On relance notre chemin ! L'adversaire n'est plus en vue !\n");
				// on vire le wait_forever et on lance l'action suivante
				STACKS_pull(ASSER);

				// adversaire n'est plus dans notre chemin, on reprend le mouvement normal
				state = NO_FOE;
			}
			break;

		case DODGE:
			// on va regarder si un mouvement est possible pour esquiver l'adversaire
			// on descend la pile pour virer le wait_forever
			//STACKS_set_top(ASSER,STACKS_get_top(ASSER)-1);
			
			// on regarde d'abord si la destination est accessible
			asser_args = ASSER_get_stack_arg(STACKS_get_top(ASSER));

			current_point.x = global.env.pos.x;
			current_point.y = global.env.pos.y;
			destination_point.x = asser_args.x;
			destination_point.y = asser_args.y;
		
			distance_before_destination = GEOMETRY_distance(current_point,destination_point);
		
			if(distance_before_destination < MAX_DISTANCE_BEFORE_DESTINATION)
			{
				// la destination est trop près, l'adversaire nous gène
				avoidance_printf("Adverse trop près de la destination !\n");
				ASSER_push_stop();
				STACKS_flush(ASSER);
				state = INITIALIZATION;
				return FOE_IN_PATH;
			}
			else
			{
				if(AVOIDANCE_foe_complex_dodge(asser_args.way, is_in_path))
				{
					avoidance_printf("Triple esquive chargée !\n");
					// on relance la machine d'état
					STACKS_push(ASSER,&wait_forever,FALSE);
					STACKS_pull(ASSER);
					state = WAIT_DODGE;
				}
				else
				{
					STACKS_push(ASSER, &wait_forever, FALSE);
					ASSER_push_stop();

					switch(avoidance_type)
					{
						case NORMAL_WAIT:
						case DODGE_AND_NO_WAIT:
							// les esquives sont impossibles !
							avoidance_printf("Trajectoires d'esquives impossibles !\n");
							state = INITIALIZATION;
							STACKS_flush(ASSER);
							return FOE_IN_PATH;
							break;
						case DODGE_AND_WAIT:
							avoidance_printf("DODGE_AND_WAIT, évitement impossible, on attend\n");
							if(!dodge_wait)
							{
								dodge_wait = TRUE;
								state = WAIT_FOE;
							}
							else
							{
								avoidance_printf("Trajectoires d'esquives impossibles après DODGE_AND_WAIT !\n");
								STACKS_flush(ASSER);
								state = INITIALIZATION;
								return FOE_IN_PATH;
							}
							break;
						case NO_DODGE_AND_WAIT:
						case NO_DODGE_AND_NO_WAIT:
							avoidance_printf("Pas d'esquive, mauvais endroit -- Line %d\n",__LINE__);
							STACKS_flush(ASSER);
							ASSER_push_stop();
							state = INITIALIZATION;
							return NOT_HANDLED;
							break;
						case NO_AVOIDANCE:
							state = NO_FOE;
							break;
						default:
							STACKS_flush(ASSER);
							state = INITIALIZATION;
							return NOT_HANDLED;
							break;
					}		
				}
			} // end esquives
			break;

		case WAIT_DODGE:
			if(global.env.match_time - detection_time > TIME_BEFORE_NEW_DETECTION)
			{
				avoidance_printf("Temps d'attente dépassé, on refait une détection\n");
				state = NO_FOE;
			}
			break;

		case DONE:
			// on est arrivé à destination
			state = INITIALIZATION;
			avoidance_printf("State DONE\n");
			// ici on a 2 retours possibles :
			// END_OK : on est arrivé correctement sans éviter l'adversaire
			// END_WITH_TIMEOUT : soit on est arrivé en ayant évité l'adversaire, soit on a eu un timeout sur la pile ASSER
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default:
			debug_printf("PROBLEME, on est dans le default !\n");
			state = INITIALIZATION;
			return NOT_HANDLED;
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
	time32_t current_match_time = global.env.match_time;

	bool_e timeout;


	//Si pas d'évitement, on fait pas d'évitement
	if(avoidance_type == NO_AVOIDANCE) {
		error_e asser_stack_state = AVOIDANCE_watch_asser_stack();
		switch(asser_stack_state)
		{
			case IN_PROGRESS:
				break;

			case END_OK:
			case END_WITH_TIMEOUT:
			case NOT_HANDLED:
				avoidance_printf("wait_move_and_scan_foe: end state = %d\n", asser_stack_state);
				state = INITIALIZATION;
				return asser_stack_state;

			default: //Ne devrait jamais arriver, AVOIDANCE_watch_asser_stack ne doit pas retourner FOE_IN_PATH car elle ne gère pas d'evitement
				avoidance_printf("wait_move_and_scan_foe: DEFAULT asser_stack_state = %d!!\n", asser_stack_state);
				state = INITIALIZATION;
				return NOT_HANDLED;
				break;
		}

		return IN_PROGRESS;
	}

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
							state = INITIALIZATION;
							return asser_stack_state;
	
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
					state = INITIALIZATION;
					return FOE_IN_PATH;
				} 
				else 
				{
					switch(avoidance_type) {
						case NORMAL_WAIT:
						case NO_DODGE_AND_WAIT:
						case DODGE_AND_WAIT:
							state = WAIT_FOE;
							break;

						case DODGE_AND_NO_WAIT:
						case NO_DODGE_AND_NO_WAIT:
							state = INITIALIZATION;
							return FOE_IN_PATH;

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
			if(avoidance_timeout_time >= wait_timeout) {
				//On est déjà arreté
//				STACKS_flush(ASSER);
//				ASSER_push_stop();
				state = INITIALIZATION;

				avoidance_printf("wait_move_and_scan_foe: timeout avec ennemi sur path\n");
				return FOE_IN_PATH;
			}

			if(is_in_path[FOE_1] || is_in_path[FOE_2])
			{
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
//                                            STACKS_flush(ASSER);
//                                            state = INITIALIZATION;
//                                            return FOE_IN_PATH;
//						}
					}
				}
				//*/

			}
			else
			{
				avoidance_printf("wait_move_and_scan_foe: no more foe, continuing\n");
				// on vire le wait_forever et on lance l'action suivante
				STACKS_pull(ASSER);

				// adversaire n'est plus dans notre chemin, on reprend le mouvement normal
				state = NO_FOE;
			}
			break;

		default:
			debug_printf("PROBLEME, on est dans le default !\n");
			state = INITIALIZATION;
			return NOT_HANDLED;
			break;
	}

	last_match_time = current_match_time;

	return IN_PROGRESS;
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
static error_e goto_pos_with_avoidance(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, ASSER_end_condition_e end_condition)
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
	if (!global.env.config.evitement) {
		for (i=0; i<NB_FOES; i++)
			in_path[i] = FALSE;
		static bool_e already_printed_debug_no_evitement = FALSE;
		if(already_printed_debug_no_evitement == FALSE) {
			already_printed_debug_no_evitement = TRUE;
			debug_printf("No evitement");
		}
		return;
	}

	// on regarde notre indicateur de vitesse et notre sens de direction
	//speed_indicator = AVOIDANCE_speed_indicator_compute();
//#warning "cette fonction ne peut pas, fonctionner... Suggestion : utiliser plutot le type de trajectoire en cours...?"

	// on identifie une distance par rapport à la distance
	//distance_computed = ((speed_indicator*52) >>2) + 400;		// DISTANCE 2011
	//distance_computed = ((speed_indicator*52) >>2) + 240;
	//distance_computed = 600;

	if(QS_WHO_AM_I_get() == TINY)
		distance_computed_narrow = DETECTION_DIST_MIN_TINY + ((Uint32)DETECTION_DIST_SPEED_FACTOR_TINY)*(abs(global.env.pos.translation_speed))/1000;  //8*125 = 1000, DETECTION_DIST_SPEED_FACTOR_TINY est en milisecondes
	else distance_computed_narrow = DETECTION_DIST_MIN_KRUSTY + ((Uint32)DETECTION_DIST_SPEED_FACTOR_KRUSTY)*(abs(global.env.pos.translation_speed))/1000;
	if(QS_WHO_AM_I_get() == TINY)
		distance_computed_wide = DETECTION_DIST_MIN_TINY;
	else distance_computed_wide = DETECTION_DIST_MIN_KRUSTY;


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
					//avoidance_printf("F_");
					//CAN_send_debug("FFFFFF");
				}//else debug_printf("O_ ");
			}
		
			if(move_way == BACKWARD || move_way == ANY_WAY)
			{
		//		debug_printf("B_%d\n",global.env.foe.angle);
				/* On regarde si l'adversaire est dans un gabarit devant nous */
				if(((global.env.foe[i].dist < distance_computed_narrow) && (global.env.foe[i].angle > (-DETECTION_ANGLE_NARROW) && global.env.foe[i].angle < DETECTION_ANGLE_NARROW)) ||
				   ((global.env.foe[i].dist < distance_computed_wide)   && (global.env.foe[i].angle > (-DETECTION_ANGLE_WIDE)   && global.env.foe[i].angle < DETECTION_ANGLE_WIDE)  )    )
				{
					in_path[i] = TRUE;
					//avoidance_printf("B_");
					//CAN_send_debug("BBBBBB");
				}//else debug_printf("O_ ");
			}
		}
	}
}

/* Fonction de calcul d'un indicateur de la vitesse et du sens de déplacement du robot */
static Uint16 AVOIDANCE_speed_indicator_compute(void)
{
	// Surveiller les initialisations
	static time32_t last_time_compute = 0;
	static GEOMETRY_point_t old_pos = {0,0};

	// On met une première valeur super élevée pour éviter d'éviter au début (si on veut éviter, ce qui n'est pas encore le cas)
	static Uint16 speed_computed = 100;
	GEOMETRY_point_t new_pos;
	
	if((global.env.asser.last_time_pos_updated - last_time_compute) > SPEED_COMPUTE_TIME)
	{
		// mise à jour de notre nouvelle position
		new_pos.x = global.env.pos.x;
		new_pos.y = global.env.pos.y;

		// calcul de la vitesse
		speed_computed = (GEOMETRY_distance(old_pos, new_pos));

		// calcul du sens de déplacement
	//	old_move_way = ASSER_get_stack_arg(STACKS_get_top(ASSER)).way;

		// on met à jour nos valeurs pour la prochaine mise à jour
		last_time_compute = global.env.asser.last_time_pos_updated;
		old_pos.x = new_pos.x;
		old_pos.y = new_pos.y;
		//debug_printf("NS=%d\n",speed_computed);
	}

	// on retourne la valeur de la vitesse
	return speed_computed;
}

/* ----------------------------------------------------------------------------- */
/* 				Fonctions de génération de la trajectoire à 3 points             */
/* ----------------------------------------------------------------------------- */

/* Fonction qui exécute une esquive du robot adverse en 3 points 
 * La fonction charge la pile !
 */
static bool_e AVOIDANCE_foe_complex_dodge(way_e move_way, bool_e in_path[NB_FOES])
{
	GEOMETRY_point_t first_point, second_point, third_point;
	bool_e result_compute;
	foe_e avoidance_foe; //On fait l'évitement sur l'adversaire le plus proche
	
	if ((in_path[FOE_1] && !in_path[FOE_2]) 
		|| (in_path[FOE_1] && in_path[FOE_2] && global.env.foe[FOE_1].dist <= global.env.foe[FOE_2].dist))
	{
		avoidance_foe = FOE_1;
	}
	else
	{
		avoidance_foe = FOE_2;
	}

	result_compute = AVOIDANCE_dodge_triple_points_compute(move_way, -1, &first_point, &second_point, &third_point, avoidance_foe);
	if(!result_compute)
	{
		avoidance_printf("Points à gauche invalides\n");
		result_compute = AVOIDANCE_dodge_triple_points_compute(move_way, 1, &first_point, &second_point, &third_point, avoidance_foe);
		if(!result_compute)
		{
			avoidance_printf("Points à droite invalides\n");
			return FALSE;
		}
	}
	
	avoidance_printf("Points valides trouvés, on charge la pile\n");
	#ifdef USE_ASSER_MULTI_POINT
		ASSER_push_goto_multi_point(third_point.x, third_point.y, FAST, move_way, ASSER_CURVES, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);
		ASSER_push_goto_multi_point(second_point.x, second_point.y, FAST, move_way, ASSER_CURVES, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);
		ASSER_push_goto_multi_point(first_point.x, first_point.y, FAST, move_way, ASSER_CURVES, NOW, END_AT_LAST_POINT, FALSE);
	#else
		ASSER_push_goto(third_point.x, third_point.y, FAST, move_way, ASSER_CURVES, END_AT_LAST_POINT,FALSE);
		ASSER_push_goto(second_point.x, second_point.y, FAST, move_way, ASSER_CURVES,END_AT_LAST_POINT, FALSE);
		ASSER_push_goto(first_point.x, first_point.y, FAST, move_way, ASSER_CURVES,END_AT_LAST_POINT, FALSE);
	#endif

	// on a correctement chargé la pile, et lancé l'action !
	return TRUE;	
}

/* Fonction qui calcule les 3 points d'esquive de l'adversaire */
static bool_e AVOIDANCE_dodge_triple_points_compute(way_e move_way, Sint16 avoidance_way, GEOMETRY_point_t* first_point, GEOMETRY_point_t* second_point, GEOMETRY_point_t* third_point, foe_e foe_id)
{
	GEOMETRY_point_t first_compute, second_compute, third_compute;
	bool_e result_compute;

	result_compute = AVOIDANCE_compute_dodge_point(move_way, global.env.foe[foe_id].dist-X_AVOIDANCE_FOE_DISTANCE, Y_AVOIDANCE_EXTERN_POINT, avoidance_way, &first_compute);
	if(!result_compute)
	{
		avoidance_printf("Premier point pourri !\n");
		return FALSE;
	}

	// calcul du deuxième point
	result_compute = AVOIDANCE_compute_dodge_point(move_way, global.env.foe[foe_id].dist, Y_AVOIDANCE_CENTRAL_POINT, avoidance_way, &second_compute);
	if(!result_compute)
	{
		avoidance_printf("Deuxième point pourri !\n");
		return FALSE;
	}

	// calcul du troisème point
	result_compute = AVOIDANCE_compute_dodge_point(move_way, global.env.foe[foe_id].dist+X_AVOIDANCE_FOE_DISTANCE, Y_AVOIDANCE_EXTERN_POINT, avoidance_way, &third_compute);
	if(!result_compute)
	{
		avoidance_printf("Troisième point pourri !\n");
		return FALSE;
	}

	avoidance_printf("Tous les points sont valides !\n");
	*first_point = first_compute;
	*second_point = second_compute;
	*third_point = third_compute;
	return TRUE;
}

/* Fonction de calcul d'un point d'esquive de l'adversaire */
static bool_e AVOIDANCE_compute_dodge_point(way_e move_way, Sint16 x_distance, Sint16 y_distance, Sint16 computed_way, GEOMETRY_point_t* result_point)
{
	Sint16 x_new_destination, y_new_destination;
	GEOMETRY_point_t result;

	if(move_way == FORWARD)
	{
		// robot en avant, formule normale
		x_new_destination = global.env.pos.x + global.env.pos.cosAngle*x_distance - (computed_way*(global.env.pos.sinAngle*y_distance));
		y_new_destination = global.env.pos.y + global.env.pos.sinAngle*x_distance + (computed_way*(global.env.pos.cosAngle*y_distance));
	}
	else if(move_way == BACKWARD)
	{
		// robot en arrière, on inverse les cos et les sin
		x_new_destination = global.env.pos.x - global.env.pos.cosAngle*x_distance + (computed_way*(global.env.pos.sinAngle*y_distance)/10);
		y_new_destination = global.env.pos.y - global.env.pos.sinAngle*x_distance - (computed_way*(global.env.pos.cosAngle*y_distance)/10);
	}
	else
	{
		// sens inconnu, faire un mouvement est dangereux dans ce cas, on met des valeurs pourries
		avoidance_printf("move_way == ANY_WAY !!!\n");
		x_new_destination = 0;
		y_new_destination = 0;
	}

	// on regarde si les points calculés sont dans le terrain
	if(ENV_game_zone_filter(x_new_destination, y_new_destination, 240) == TRUE)
	{
		// le point est OK
		result.x = x_new_destination;
		result.y = y_new_destination;
		*result_point = result;

		avoidance_printf("Point OK, X = %d, Y = %d\n",x_new_destination,y_new_destination);
		return TRUE;
	}
	else avoidance_printf("Point NOT OK, X = %d, Y = %d\n",x_new_destination,y_new_destination);
	return FALSE;
}

/* ----------------------------------------------------------------------------- */
/* 		Fonctions évoluées de traitement des activités de l'adversaire           */
/* ----------------------------------------------------------------------------- */

/* Fonction qui regarde si l'adversaire a bougé durant l'intervalle de temps précédent */
static bool_e AVOIDANCE_foe_not_move(foe_e foe_id)
{
	static time32_t last_time_compute[NB_FOES] = {0,0};
	static GEOMETRY_point_t old_foe[NB_FOES] = {{0,0},{0,0}};
	static bool_e result = FALSE;

	if((global.env.match_time - last_time_compute[foe_id]) > FOE_MOVE_ANALYSE_TIME)
	{
		if(abs(global.env.foe[foe_id].x - old_foe[foe_id].x) < FOE_MINIMAL_MOVE)
		{
			if(abs(global.env.foe[foe_id].y - old_foe[foe_id].y) < FOE_MINIMAL_MOVE)
			{
				avoidance_printf("L'adversaire n'a pas bougé\n");
				result = TRUE;
			}
			else
			{
				avoidance_printf("L'adversaire a bougé\n");
				result = FALSE;
			}
		}
		else
		{
			avoidance_printf("L'adversaire a bougé\n");
			result = FALSE;
		}

		// on met à jour nos valeurs pour la prochaine mise à jour
		last_time_compute[foe_id] = global.env.match_time;
		old_foe[foe_id].x = global.env.foe[foe_id].x;
		old_foe[foe_id].y = global.env.foe[foe_id].y;

		if ((global.env.match_time - global.env.foe[foe_id].update_time)>(DETECTION_TIMEOUT))
		{
			avoidance_printf("Calcul position adversaire impossible\n");
			result = FALSE;
		}
	}
	return result;
}

/* Fonction qui regarde si notre robot est immobile ou non */
static bool_e AVOIDANCE_robot_translation_move()
{
	static bool_e result = FALSE;
	static GEOMETRY_point_t old_pos = {0,0};
	static time32_t last_time_update = 0;

	if((global.env.match_time - last_time_update) > US_TRANSLATION_ANALYSE_TIME)
	{
		// on recalcule si on a fait une translation
		if(abs(global.env.pos.x - old_pos.x) < US_MINIMAL_MOVE)
		{
			if(abs(global.env.pos.y - old_pos.y) < US_MINIMAL_MOVE)
			{
				result = FALSE;
			}
			else
			{
				result = TRUE;
			}
		}
		else
		{
			result = TRUE;
		}
		
		// mise à jour des données sauvegardées
		last_time_update = global.env.match_time;
		old_pos.x = global.env.pos.x;
		old_pos.y = global.env.pos.y;
	}
	return result;
}

/* Gère les colisions */
static error_e AVOIDANCE_move_colision()
{
	enum state_e
	{
		INITIALIZATION = 0,
		WAIT,
		SWITCH_CURRENT_MOVE,
		MOVE_FORWARD,
		MOVE_BACKWARD,
		WAIT_END_ROTATION,
		DONE,
		ROTATE_THE_OTHER_WAY
	};
	static enum state_e state = INITIALIZATION;
	
	error_e sub_action;
	static bool_e timeout;

	static time32_t erreur_time;
	
	static bool_e first_attempt;
	static bool_e move_forward_tested, move_rear_tested;
	static bool_e direct_rotation_tested, indirect_rotation_tested;
	
	switch(state)
	{	
		case INITIALIZATION:
			first_attempt = TRUE;
			move_forward_tested = FALSE;
			move_rear_tested = FALSE;
			direct_rotation_tested = FALSE;
			indirect_rotation_tested = FALSE,
			erreur_time = global.env.match_time;
			state = WAIT;
			break;
		
		case WAIT:
			if(global.env.match_time - erreur_time > 1000)
			{
				state = SWITCH_CURRENT_MOVE;
			}
			break;
			
		case SWITCH_CURRENT_MOVE:
			debug_printf("[MOVE COLISION] init : ");
			timeout = FALSE;
			
			//on regarde les vitesses translation et rotation
			if ((global.env.asser.vitesse_translation_erreur > 10 || !first_attempt)
				&& !move_rear_tested)
			{
				debug_printf("MOVE_BACKWARD\r\n");
				move_rear_tested = TRUE;
				state = MOVE_BACKWARD;
			}
			else if ((global.env.asser.vitesse_translation_erreur < -10  || !first_attempt)
				&& !move_forward_tested)
			{
				debug_printf("MOVE_FORWARD\r\n");
				state = MOVE_FORWARD;				
			}
			else if ((global.env.asser.vitesse_rotation_erreur > 10  || !first_attempt)
				&& !indirect_rotation_tested)
			{
				debug_printf("INDIRECT_ROTATION\r\n");
				// je dois tourner en sens horaire
				ASSER_push_relative_goangle(-PI4096/4, FAST, FALSE); 
				state = WAIT_END_ROTATION;
			}
			else if ((global.env.asser.vitesse_rotation_erreur < -10  || !first_attempt)
				&& !direct_rotation_tested)
			{
				// je dois tourner en sens direct
				debug_printf("DIRECT_ROTATION\r\n");
				ASSER_push_relative_goangle(PI4096/4, FAST, FALSE); 
				state = WAIT_END_ROTATION;
			}
			else
			{
				debug_printf("Nothing\n");
				state= INITIALIZATION;
				return END_OK; //on ne gère pas l'erreur
			}
			break;
			
		case MOVE_FORWARD:
			sub_action = relative_move(FORWARD_COLISION_MOVE,FAST,FORWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case END_OK : 
					state = DONE;
					break;

				case IN_PROGRESS :
					break;

				case END_WITH_TIMEOUT :
				case NOT_HANDLED :
				default:
					state = DONE;
					break;
			}
			
 			if (global.env.asser.erreur)
 			{
	 			debug_printf("Error MOVE_FORWARD\r\n");
	 			STACKS_pull(ASSER);
	 			erreur_time = global.env.match_time;
	 			state = WAIT;
	 		}
	 		break;
		
		case MOVE_BACKWARD:
			sub_action = relative_move(BACKWARD_COLISION_MOVE,FAST,BACKWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case END_OK : 
					state = DONE;
					break;

				case IN_PROGRESS :
					break;

				case END_WITH_TIMEOUT :
				case NOT_HANDLED :
				default:
					state = DONE;
					break;
			}
			
 			if (global.env.asser.erreur)
 			{
	 			debug_printf("Error MOVE_BACKWARD\r\n");
	 			STACKS_pull(ASSER);
	 			erreur_time = global.env.match_time;
	 			state = WAIT;
	 		}
	 		break;
				
		case WAIT_END_ROTATION:
			if(STACKS_wait_end_auto_pull(ASSER,&timeout))
				state = DONE;
				
			if (global.env.asser.erreur)
 			{
	 			debug_printf("Error MOVE_ROTATION\r\n");
	 			STACKS_pull(ASSER);
	 			erreur_time = global.env.match_time;
	 			state = WAIT;
	 		}
			break;
			
		case DONE:
			state = INITIALIZATION;
			return timeout?END_WITH_TIMEOUT:END_OK;
			break;

		default :
			state = INITIALIZATION;
			break;
	}
	return IN_PROGRESS;
}

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
