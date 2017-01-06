/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier :  copilot.c
 *  Package : Asser
 *  Description : Fonctions qui gèrent les trajectoires
 *  Auteur : Inspiré par Val' 2007 - Nirgal 2009
 *  Version 200904
 */


#include "copilot.h"
#include "roadmap.h"
#include "buffer.h"
#include "supervisor.h"
#include "pilot.h"
#include "motors.h"
#include "calculator.h"
#include "odometry.h"
#include "QS/QS_maths.h"
#include "debug.h"
#include "avoidance.h"
#include "secretary.h"
#include "QS/QS_timer.h"
#include "roadmap.h"
#include "QS/QS_watchdog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"

/**
 *	Types
 */
typedef enum{
	NOT_ARRIVED = 0,
	ARRIVED
}arrived_e;


/**
 *	Variables locales
 */
static volatile order_t current_order;
static volatile order_t buffer_order;

static volatile arrived_e arrived;
static volatile arrived_e arrived_rotation;
static volatile arrived_e arrived_translation;

static volatile braking_e already_braking_translation;	//a commencé a freiner depuis le début de la trajectoire, ou non.
static volatile braking_e already_braking_rotation;		//a commencé a freiner depuis le début de la trajectoire, ou non.
static volatile braking_e braking_translation;
static volatile braking_e braking_rotation;

static volatile Uint8 rush_count_traj = 0;
static volatile Sint16 rush_first_traj_acc = 300;
static volatile Sint16 rush_second_traj_acc = 245;
static volatile Sint16 rush_brake_acc = 180;
static volatile Sint16 rush_acc_rot_trans = 10;
static volatile bool_e in_rush = FALSE;
static volatile bool_e avoid_in_rush = FALSE;

static volatile Uint8 actualTrajID = 0;

/**
 *	Prototypes
 */
static bool_e COPILOT_decision_change_order(bool_e * change_order_in_multipoint_without_reaching_destination);
static trajectory_e COPILOT_decision_rotation_before_translation(Sint16 destination_x, Sint16 destination_y, Sint16 viewing_angle);
static void COPILOT_try_order(order_t * order, bool_e change_order_in_multipoint_without_reaching_destination);
static void COPILOT_do_order(order_t * order);
static void COPILOT_update_destination_rotation(void);
static void COPILOT_update_destination_translation(void);
static void COPILOT_update_arrived(void);
static arrived_e Decision_robot_arrive_bordure(void);
static arrived_e Decision_robot_arrive_arret(void);
static arrived_e Decision_robot_arrive_rotation(void);
static arrived_e Decision_robot_arrive_translation(void);
static void COPILOT_update_brake_state(void);
static braking_e COPILOT_update_brake_state_rotation(void);
static braking_e COPILOT_update_brake_state_translation(void);
static void COPILOT_destination_angle(Sint16 x, Sint16 y, Sint16 * angle_a_parcourir, Sint16 * teta_destination, way_e way, way_e * chosen_way);


/**
 *	Fonctions
 */

void COPILOT_init(void)
{
	arrived = ARRIVED;
	COPILOT_reset_absolute_destination();
	current_order = (order_t){TRAJECTORY_NONE, 0,0,0, PROP_ABSOLUTE, PROP_END_AT_POINT, NOT_BORDER_MODE, FORWARD, FAST, ACKNOWLEDGED, 0};
	braking_translation = NOT_BRAKING;
	braking_rotation = NOT_BRAKING;
	already_braking_rotation = NOT_BRAKING;
	already_braking_translation = NOT_BRAKING;
}

void COPILOT_process_it(void)
{
	order_t next_order;
	bool_e  change_order_in_multipoint_without_reaching_destination = FALSE;
	static watchdog_id_t id_end_enslavement = 0xFF;
	static bool_e flag_end_enslavement = FALSE;

	if(current_order.trajectory == TRAJECTORY_STOP || current_order.trajectory == WAIT_FOREVER)
		COPILOT_reset_absolute_destination();

	// Mise à jours de la vitesses maximum en rotation en cas d'adversaire proche et de vitesse de translation quasiment nulle (rotation sur place)
	if(current_order.avoidance != AVOID_DISABLED && AVOIDANCE_foe_near() && absolute(global.vitesse_translation) < 100)
		PILOT_set_speed(SLOW);
	else
		PILOT_set_speed(current_order.speed);

	// Mise à jours du mode bordure afin que les moteurs se limite en PWM pour ne pas faire partir en sécurité la batterie
	if(current_order.border_mode == BORDER_MODE)
		MOTORS_set_border_mode(TRUE);
	else
		MOTORS_set_border_mode(FALSE);


	if(arrived !=ARRIVED)
	{
		if(global.debug.mode_best_effort_enable)
		{
			//Ce mode est utilisé pour les réglages de coefs... c'est un mode de débogage.
			//Dans ce mode, le robot ne suit pas un point qui a toujours la même trajectoire...
			//le point suivi est en fait recalculé EN FONCTION de la position REELLE du robot
			//Donc si le robot dévie de sa trajectoire (perturbation...), celle ci est recalculée en permanence !
			//Cela provoque des trajectoires aléatoires, surtout si les coefs sont mal réglés
			//Ce qui veut dire que c'est pratique pour régler les coefs !
			global.position_rotation = global.ecart_rotation_prec;
			global.position_translation = global.ecart_translation_prec;
		}
		else
		{		//MODE NORMAL !
			//On retranche de la position du référentiel IT l'avancement réel du robot (avec un robot parfait, on retomberait à 0)
			//Si le robot n'a pas parfaitement avancé de ce que l'on espérait, cette position sera non nulle...
			//	et prise en compte par l'asservissement, qui devra ajouter/retirer un poil de jus sur les moteurs pour compenser cette erreur...
			global.position_rotation -= global.real_position_rotation;
			global.position_translation -= global.real_position_translation;
		}
		//on remet a jour le point destination dans le référentiel IT
		COPILOT_update_destination_translation();
		COPILOT_update_destination_rotation();
	}

	COPILOT_update_arrived();
	COPILOT_update_brake_state();

	if(current_order.trajectory == TRAJECTORY_STOP && arrived == ARRIVED && global.flags.match_over == TRUE && id_end_enslavement == 0xFF)
		id_end_enslavement = WATCHDOG_create_flag(1500, &flag_end_enslavement);

	if(flag_end_enslavement)
		CORRECTOR_PD_enable(CORRECTOR_DISABLE);

	if(COPILOT_decision_change_order(&change_order_in_multipoint_without_reaching_destination))
	{
		if(ROADMAP_get_next(&next_order))
		{
			COPILOT_try_order(&next_order, change_order_in_multipoint_without_reaching_destination);
		}
	}
}

///--------------------------------------------------------------------------------------------------------------
///-------------------------------------------Choix des trajectoires---------------------------------------------
///--------------------------------------------------------------------------------------------------------------

static bool_e COPILOT_decision_change_order(bool_e * change_order_in_multipoint_without_reaching_destination)
{
	bool_e buffer_vide;
	buffer_vide = BUFFER_is_empty();

	if(ROADMAP_exists_prioritary_order())
		return TRUE;

	if(current_order.trajectory == WAIT_FOREVER)
		return FALSE;

	if(arrived == ARRIVED){
		SUPERVISOR_state_machine(EVENT_ARRIVED, 0);

		if(!buffer_vide)		//Il y a un ordre à traiter.
			return TRUE;
		else					//Il n'y a rien à faire.
			SUPERVISOR_state_machine(EVENT_NOTHING_TO_DO, 0);
	}else{

		if(		(current_order.trajectory == TRAJECTORY_ROTATION && braking_rotation == BRAKING)
			 ||
				(
					(current_order.trajectory == TRAJECTORY_TRANSLATION || current_order.trajectory == TRAJECTORY_AUTOMATIC_CURVE)
					&& braking_translation == BRAKING
					&& (arrived_rotation == ARRIVED || braking_rotation == BRAKING)
				)
		  )
		{
			SUPERVISOR_state_machine(EVENT_BRAKING, 0);

			if(current_order.propEndCondition == PROP_END_AT_BRAKE && !buffer_vide){
				order_t checkNextOrder;
				ROADMAP_check_next(&checkNextOrder);

				if(		(checkNextOrder.trajectory == TRAJECTORY_ROTATION && current_order.trajectory == TRAJECTORY_ROTATION)
					 ||
						( (checkNextOrder.trajectory == TRAJECTORY_TRANSLATION || checkNextOrder.trajectory == TRAJECTORY_AUTOMATIC_CURVE)
						 &&
						  (current_order.trajectory == TRAJECTORY_TRANSLATION || checkNextOrder.trajectory == TRAJECTORY_AUTOMATIC_CURVE)
						)
				   ){
					*change_order_in_multipoint_without_reaching_destination = TRUE;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

/* Fonction test avant le depart
 * Fonction appelée si la trajectoire demandée est une translation...
 * TRANSLATION suppose qu'on veuille rejoindre le point en LIGNE DROITE...
 * On s'engage donc à ne pas faire de 'courbe' trop marquée...
 */
static trajectory_e COPILOT_decision_rotation_before_translation(Sint16 destination_x, Sint16 destination_y, Sint16 angle_a_parcourir)
{
	//Informations :
				//on ne fait une rotation préalable qu'après ARRET du robot (vitesses nulles...)
				//il faut donc lancer un type de trajectoire ARRET avant de relancer la rotation
				//donc, on réécrit la traj actuelle, et on ne fait la rotation que SI nos vitesses sont nulles
				//si ma vitesse en translation est trop grande, je marque un arret...
	Sint32 d;
	d = absolute((Sint32)(CALCULATOR_viewing_algebric_distance (global.position.x, global.position.y, destination_x, destination_y, angle_a_parcourir))<<12);


	if (global.vitesse_rotation > PRECISION_ARRIVE_SPEED_ROTATION )
	{
	//	debug_printf("arret préalable\n");
		return TRAJECTORY_STOP;	//Si notre vitesse en rotation est non nulle, il est difficile de savoir si on réussiera à parcourir la courbe
	}

	//Si on est déjà sur le point demandé >>> aucune trajectoire..... on fait donc la trajectoire "AUCUNE", qui arrivera directement.
	if(d < PRECISION_ARRIVE_POSITION_TRANSLATION && absolute(global.vitesse_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION)
	{
	//	debug_printf("Point trop proche\n");
		return TRAJECTORY_NONE;
	}
	if(absolute(angle_a_parcourir) > ANGLE_MAXI_LANCEMENT_TRANSLATION)
	{
		//larotation préalable doit démarrer par un arrêt si la vitesse est non nulle !
		//sinon, on voudra tourner sur le point actuel, alors qu'on est encore en train d'avancer !
		if(absolute(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
			return TRAJECTORY_STOP;
		else	//cool, c'est parti pour une rotation préalable !
			return TRAJECTORY_ROTATION;
	}

	//Je n'autorise la translation que si l'angle à parcourir est très faible...
	return TRAJECTORY_TRANSLATION;
}

//lance la trajectoire suivante du buffer
static void COPILOT_try_order(order_t * order, bool_e change_order_in_multipoint_without_reaching_destination)
{
	Sint32 d;
	static Sint16 angle_a_parcourir;

	CORRECTOR_PD_enable(order->corrector);

	if(order->trajectory == WAIT_FOREVER){
		COPILOT_do_order(order);
		return;
	}


	if(order->relative == PROP_RELATIVE) //Si l'ordre est relatif, c'est maintenant qu'il doit devenir absolu !
	{
			//REMARQUE : si on est en multipoint, et qu'on vient de changer d'ordre à l'instant du freinage, sans avoir atteint la position visée,
			//La relativité doit alors être calculée sur la destination visée, et non pas sur la position actuelle !!!!!!
			if(change_order_in_multipoint_without_reaching_destination)
			{
				order->x += current_order.x;
				order->y += current_order.y;
			}
			else
			{
				order->x += global.position.x;
				order->y += global.position.y;
			}
			order->teta += global.position.teta;
			order->teta = CALCULATOR_modulo_angle(order->teta);

			order->relative = PROP_ABSOLUTE; 	//Ce n'est PLUS relatif !!!!!!!!
					//Note : si la trajectoire est remise en buffer pour une rotation préalable par exemple, les nouvelles coordonnées non relatives seront prises en compte !
	}

	switch(order->trajectory)
	{
		case TRAJECTORY_STOP:
			order->x = 0;
			order->y = 0;
		break;

		case TRAJECTORY_ROTATION:
			//	si rotation pure -> x et y demandé = position actuelle

			//si la vitesse en translation est élevée, on commence par s'arreter. Ceci évite l'effet suivant :
			//on chercherait à tourner autour du point actuel... or si on continue d'avancer, il faut rejoindre ce fameux point...
			if( absolute(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
			{
				//On réécrit le reste de la trajectoire pour la suite...
				BUFFER_add_begin(order);	//On remet l'ordre en l'état dans le buffer...
				order->trajectory = TRAJECTORY_STOP;
				order->acknowledge = NO_ACKNOWLEDGE;
				order->x = 0;
				order->y = 0;
			}
			else
			{
				order->x = global.position.x;
				order->y = global.position.y;
			}
		break;

		case TRAJECTORY_TRANSLATION:

			//Faut-il scinder la trajectoire en deux ou en trois ?

			//on commence par calculer l'angle de la direction entre le point actuel et la destination
			COPILOT_destination_angle(order->x, order->y, &angle_a_parcourir, &(order->teta), order->way, &(order->way));

			//Quelle trajectoire dois-je commencer par faire ? ( = faut il s'arreter, tourner vers le point, où y aller directement...?)
			switch(COPILOT_decision_rotation_before_translation(order->x, order->y, angle_a_parcourir))
			{
				case TRAJECTORY_ROTATION:
					//On réécrit le reste de la trajectoire pour la suite...
					BUFFER_add_begin(order);

					//Les rotations préalables pourraient se faire en mode multipoints.
					order->propEndCondition = PROP_END_AT_POINT;
					order->way = ANY_WAY;

					//on ne prévient pas la carte P sur rotation préalable ajoutée par nos soins.
					order->acknowledge = NO_ACKNOWLEDGE;
					order->x = global.position.x;
					order->y = global.position.y;
					order->trajectory = TRAJECTORY_ROTATION;
					//on ne fait qu'une rotation préalable pour le moment
				break;
				case TRAJECTORY_STOP:
					BUFFER_add_begin(order);
					//on ne prévient pas la carte P sur arrêt préalable ajoutée par nos soins.
					order->propEndCondition = PROP_END_AT_POINT;
					order->acknowledge = NO_ACKNOWLEDGE;
					order->trajectory = TRAJECTORY_STOP;
										order->x = 0;
										order->y = 0;
				break;
				case TRAJECTORY_NONE:
					//je suis déjà arrivé au bon point... donc je marque un arrêt histoire de bien répondre que je suis arrivé...
					order->trajectory = TRAJECTORY_STOP;
										order->x = 0;
										order->y = 0;
				break;
				case TRAJECTORY_TRANSLATION:

				break;
				default:

				break;
			}
		break;
		case TRAJECTORY_AUTOMATIC_CURVE:
			//on commence par calculer l'angle de la direction entre le point actuel et la destination
			COPILOT_destination_angle(order->x, order->y, &angle_a_parcourir, &(order->teta), order->way, &(order->way));
			d = absolute((Sint32)(CALCULATOR_viewing_algebric_distance (global.position.x, global.position.y, order->x, order->y, angle_a_parcourir))<<12);

			if(d < PRECISION_ARRIVE_POSITION_TRANSLATION && absolute(global.vitesse_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION)
			{
				order->trajectory = TRAJECTORY_STOP;
								order->x = 0;
								order->y = 0;
							//Nous y sommes déjà !
			}
			else
			{
				if(angle_a_parcourir<=-(HALF_PI4096*17)/20 || angle_a_parcourir >= (HALF_PI4096*17)/20)	//Si l'angle à parcourir est plus grand que PI/2 ou un petit peu moins -> rotation préalable.
				{
					//debug_printf("rotation préalable courbe\r\n");
					//On réécrit le reste de la trajectoire pour la suite...
					BUFFER_add_begin(order);

					//Les rotations préalables pourraient se faire en mode multipoints.
					order->propEndCondition = PROP_END_AT_POINT;
					order->acknowledge = NO_ACKNOWLEDGE;
					//on ne prévient pas la carte stratégie sur rotation préalable ajoutée par nos soins.

					//on ne fait qu'une rotation préalable pour le moment (ou un arrêt si nécessaire...)
					order->x = global.position.x;
					order->y = global.position.y;
					if(absolute(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
						order->trajectory = TRAJECTORY_STOP;		//Besoin de faire un arrêt préalable à la rotation préalable
					else{
						order->way = ANY_WAY;
						order->trajectory = TRAJECTORY_ROTATION;	//Nous sommes bien arretés : nous pouvons faire une rotation préalable !
					}
				}
				else
				{
					order->trajectory = TRAJECTORY_TRANSLATION;
				}
			}
			break;
		default:
		break;
	}
		//Et c'est parti !
		COPILOT_do_order(order);
}

//PRECONDITION STRICTE : ce qu'on me demande est possible !!!
static void COPILOT_do_order(order_t * order)
{
	if((order->trajectory == TRAJECTORY_AUTOMATIC_CURVE || order->trajectory == TRAJECTORY_TRANSLATION) &&
			order->avoidance != AVOID_DISABLED &&
			AVOIDANCE_target_safe(order->way, FALSE)){ // Fonction différente à faire pour une trajectoire en courbe automatique

		if(order->avoidance == AVOID_ENABLED){ // adversaire sur la trajectoire, évitement sans wait donc annulation de la trajectoire

			BUFFER_init();

			order_t supp;
				supp.x = 0;
				supp.y = 0;
				supp.teta = 0;
				supp.relative = PROP_ABSOLUTE;
				supp.way = ANY_WAY;
				supp.border_mode = NOT_BORDER_MODE;
				supp.propEndCondition = PROP_END_AT_POINT;
				supp.speed = FAST;
				supp.acknowledge = NO_ACKNOWLEDGE;
				supp.corrector = CORRECTOR_ENABLE;
				supp.avoidance = AVOID_DISABLED;
				supp.total_wait_time = 0;
				supp.trajectory = TRAJECTORY_STOP;
				supp.idTraj = 0;
			current_order = supp;

			AVOIDANCE_said_foe_detected(FALSE, FALSE);

		}else if(order->avoidance == AVOID_ENABLED_AND_WAIT){
			buffer_order = *order;
			buffer_order.wait_time_begin = global.absolute_time;

			order_t supp;
				supp.x = 0;
				supp.y = 0;
				supp.teta = 0;
				supp.relative = PROP_ABSOLUTE;
				supp.way = ANY_WAY;
				supp.border_mode = NOT_BORDER_MODE;
				supp.propEndCondition = PROP_END_AT_POINT;
				supp.speed = FAST;
				supp.acknowledge = NO_ACKNOWLEDGE;
				supp.corrector = CORRECTOR_ENABLE;
				supp.avoidance = AVOID_DISABLED;
				supp.total_wait_time = 0;
				supp.trajectory = WAIT_FOREVER;
				supp.idTraj = 0;

			BUFFER_add_begin(&supp);

			AVOIDANCE_said_foe_detected(FALSE, TRUE);

			supp.trajectory = TRAJECTORY_STOP;
			current_order = supp;
		}

	}else{

		//IMPORTANT, à ce stade, le type de trajectoire peut etre ROTATION, TRANSLATION, AUTOMATIC_CURVE ou STOP
		//Les coordonnées ne sont PLUS relatives !!!
		current_order = *order;

		if(in_rush){
			rush_count_traj++;

			if(rush_count_traj == 1)
				PILOT_set_coef(PILOT_ACCELERATION_NORMAL, rush_first_traj_acc);
			else if(rush_count_traj == 2){
				PILOT_set_coef(PILOT_ACCELERATION_NORMAL, rush_second_traj_acc);
				PILOT_set_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION, rush_acc_rot_trans);
			}else if(rush_count_traj == 3){
				if(avoid_in_rush){
					PILOT_set_coef(PILOT_ACCELERATION_NORMAL, BIG_ACCELERATION_AVOIDANCE_RUSH);
				}else{
					PILOT_set_coef(PILOT_ACCELERATION_NORMAL, rush_brake_acc);
					//CORRECTOR_set_coef(CORRECTOR_COEF_KP_ROTATION, BIG_KP_ROTATION/2);
					//CORRECTOR_set_coef(CORRECTOR_COEF_KD_ROTATION, BIG_KD_ROTATION/2);
				}
				//SUPERVISOR_set_treshold_error_translation(400);
				//SUPERVISOR_set_treshold_error_rotation(THRESHOLD_ERROR_ROTATION*2);
			}
		}
	}

	actualTrajID = current_order.idTraj;

	//MAJ Vitesse
	PILOT_set_speed(current_order.speed);

	SUPERVISOR_state_machine(EVENT_NEW_ORDER, current_order.acknowledge);

	arrived = NOT_ARRIVED;	//Ceci pour éviter, dans le main, de croire qu'on est arrivé AVANT la mise a jour de l'état arrive dans it.c
	arrived_rotation = NOT_ARRIVED;
	arrived_translation = NOT_ARRIVED;
	braking_translation = NOT_BRAKING;
	braking_rotation = NOT_BRAKING;
	already_braking_rotation = NOT_BRAKING;
	already_braking_translation = NOT_BRAKING;
	//On remet à jour la destination (pour que la détection d'erreur soit OK...)
	COPILOT_update_destination_translation();
	COPILOT_update_destination_rotation();
	COPILOT_update_arrived();
	COPILOT_update_brake_state();

}


///--------------------------------------------------------------------------------------------------------------
///-----------------------------------------Mise à jours des destinations----------------------------------------
///--------------------------------------------------------------------------------------------------------------

static void COPILOT_update_destination_rotation(void)
{
	Sint32 angle;

	//La mise à jour de la destination en angle souhaitée (référentiel du terrain) se fait lorsqu'on parcours des courbes, ou plus général des trajectoires autres que des rotations pures... même si a priori ca doit pas etre genant !
	if(		current_order.trajectory != TRAJECTORY_ROTATION &&
			current_order.border_mode == NOT_BORDER_MODE &&
			arrived == NOT_ARRIVED &&
			(absolute(PILOT_get_destination_translation()) > THRESHOLD_STOPPING_CALCULATION_VIEWING_ANGLE)
		)
	{
		angle = CALCULATOR_viewing_angle_22(ODOMETRY_get_65536_x(), ODOMETRY_get_65536_y(), (current_order.x << 16), (current_order.y << 16));

		//supposons qu'on soit en marche avant...

		//Si l'angle a parcourir est plus grand que PI/2... on aurait mieux fait de choisir la marche arrière !
		//l'angle a parcourir sera recalculé ensuite...
		if(current_order.way != FORWARD)
			angle += PI_22;

		angle = CALCULATOR_modulo_angle_22(angle);

	}else{
		angle = current_order.teta << 10;
	}

	if((current_order.trajectory == TRAJECTORY_ROTATION && current_order.way == ANY_WAY)
			||current_order.trajectory != TRAJECTORY_ROTATION){

		//Si destination plus loin que PI... autant tourner dans l'autre sens !
		PILOT_set_destination_rotation(CALCULATOR_modulo_angle_22(angle - (global.position.teta << 10)));

	}else{

		Sint32 dest_angle = CALCULATOR_modulo_angle_22(angle - (global.position.teta << 10));

		if(current_order.way == CLOCKWISE && dest_angle > 0)
			PILOT_set_destination_rotation(dest_angle - TWO_PI22);
		else if(current_order.way == TRIGOWISE && dest_angle < 0)
			PILOT_set_destination_rotation(dest_angle + TWO_PI22);
		else
			PILOT_set_destination_rotation(dest_angle);
	}


	current_order.teta = angle >> 10;
}

//Attention, toujours appeler en premier MAJ_rotation... car MAJ_translation se sert de l'angle calculé dans MAJ_rotation
//mais en fait pas forcément...
static void COPILOT_update_destination_translation(void)
{
	if(current_order.trajectory == TRAJECTORY_ROTATION)
		PILOT_set_destination_translation(0);
	else
		PILOT_set_destination_translation((CALCULATOR_viewing_algebric_distance_mm16(	// Attention on ne peut pas mettre plus que ça sinon il y a overflow
												(ODOMETRY_get_65536_x() >> 12),
												(ODOMETRY_get_65536_y() >> 12),
												(current_order.x << 4),
												(current_order.y << 4),
												CALCULATOR_modulo_angle(
																		CALCULATOR_viewing_angle(
																			global.position.x,
																			global.position.y,
																			current_order.x,
																			current_order.y)
																		- global.position.teta)
																)
											));
}

void COPILOT_reset_absolute_destination(void)
{
	//La destination est la position actuelle...
		current_order.x = global.position.x;
		current_order.y = global.position.y;
		current_order.teta = global.position.teta;
}


///--------------------------------------------------------------------------------------------------------------
///-----------------------------------------Décision concernant le robot-----------------------------------------
///--------------------------------------------------------------------------------------------------------------

static void COPILOT_update_arrived(void)
{
	if(current_order.trajectory == TRAJECTORY_STOP)
	{
		arrived = Decision_robot_arrive_arret();
		return;
	}

	if(arrived_rotation  != ARRIVED)	//Maintient du ARRIVED en rotation.
	{
		if(current_order.border_mode != NOT_BORDER_MODE)
			arrived_rotation 	= ARRIVED;
		else
			arrived_rotation 	= Decision_robot_arrive_rotation();
	}

	if(arrived_translation != ARRIVED)
		arrived_translation = Decision_robot_arrive_translation();

	if(arrived == ARRIVED)
		return;

	if (current_order.trajectory == TRAJECTORY_ROTATION)
		arrived = arrived_rotation;
	else
		arrived = arrived_translation;

	//si le mode bordure est actif
	if(current_order.border_mode != NOT_BORDER_MODE)
		if(arrived != ARRIVED)	//MAINTIENT DE ROBOT ARRIVE
			arrived = Decision_robot_arrive_bordure();
}

static arrived_e Decision_robot_arrive_bordure(void)
{
	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		return DEBUG_get_we_touch_border();
	#endif
	//si on est en mode calage, la condition d'arrivée est un écart entre le point fictif et notre position...
	if(absolute(global.ecart_translation)>((QS_WHO_AM_I_get()==BIG_ROBOT)?BIG_TRESHOLD_CALIBRATION_TRANSLATION:SMALL_TRESHOLD_CALIBRATION_TRANSLATION))
	{
		//accepter l'écart obtenu...
		PILOT_referential_reset();
		PILOT_referential_init();
		COPILOT_update_destination_translation();
		COPILOT_update_destination_rotation();

		//Mise à jour de la position robot demandée.
		if(current_order.border_mode == BORDER_MODE_WITH_UPDATE_POSITION)
			ODOMETRY_correct_with_border(current_order.way);

	//debug_printf("ARRIVE_BORDURE_ASSER_AVANT : trans:%ld | rot:%ld\n",abs(global.ecart_translation), abs(global.ecart_rotation));


	//	global.ecart_translation = global.position_translation - global.position_translation_reelle; // positif si le robot doit avancer
	//	global.ecart_rotation = global.position_rotation - global.position_rotation_reelle;  //positif pour rotation dans le sens trigo
	//debug_printf("ARRIVE_BORDURE_ASSER_APRES : trans:%ld | rot:%ld\n",abs(global.ecart_translation), abs(global.ecart_rotation));

		return ARRIVED;
	}
	else
		return NOT_ARRIVED;


}

static arrived_e Decision_robot_arrive_arret(void)
{
	if( absolute(global.real_speed_rotation) < PRECISION_ARRIVE_SPEED_ROTATION )
		arrived_rotation = ARRIVED;
	else
		arrived_rotation = NOT_ARRIVED;

	if( absolute(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION )
		arrived_translation = ARRIVED;
	else
		arrived_translation = NOT_ARRIVED;

	if((arrived_translation == ARRIVED) && (arrived_rotation == ARRIVED))
	{
		COPILOT_reset_absolute_destination();
		return ARRIVED;
	}
	else
		return NOT_ARRIVED;
}

static arrived_e Decision_robot_arrive_translation(void)
{

	if(current_order.trajectory == TRAJECTORY_ROTATION)
	{
					//Je n'ai pas d'explication pertinente pour justifier cette division par 4...
		if ( (absolute(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION/4 ) &&
				(absolute(global.real_position_translation-PILOT_get_destination_translation()) < PRECISION_ARRIVE_POSITION_TRANSLATION)  )	//juste pour qu'il ne pense pas qu'il est arrivé tout au début de la trajectoire, au moment où sa vitesse est nulle !!!
			return ARRIVED;
		else
			return NOT_ARRIVED;
	}
	else
	{
		if ( (absolute(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION ) &&
				(absolute(global.real_position_translation-PILOT_get_destination_translation()) < PRECISION_ARRIVE_POSITION_TRANSLATION)  )	//juste pour qu'il ne pense pas qu'il est arrivé tout au début de la trajectoire, au moment où sa vitesse est nulle !!!
			return ARRIVED;
		else
			return NOT_ARRIVED;
	}
}

static arrived_e Decision_robot_arrive_rotation(void)
{
	//CONDITION DE CONSIDERATION ROBOT ARRIVE SUR ROTATION = vitesse faible et position proche
	if (
			(absolute(global.real_speed_rotation) < PRECISION_ARRIVE_SPEED_ROTATION)
			&&
			(absolute(CALCULATOR_modulo_angle_22(global.real_position_rotation - PILOT_get_destination_rotation())) < PRECISION_ARRIVE_POSITION_ROTATION)
	   )
		return ARRIVED;
	else
		return NOT_ARRIVED;
}

///--------------------------------------------------------------------------------------------------------------
///--------------------------------------Décision concernant le point fictif-------------------------------------
///--------------------------------------------------------------------------------------------------------------

static void COPILOT_update_brake_state(void)
{
	braking_translation = COPILOT_update_brake_state_translation();
	braking_rotation = COPILOT_update_brake_state_rotation();

	if(braking_rotation)
		already_braking_rotation = BRAKING;

	if(braking_translation)
		already_braking_translation = BRAKING;
}

static braking_e COPILOT_update_brake_state_rotation(void)
{
	Sint32 rotation_restante, angle_frein;		//[rad.4096.1024]

	//Calcul de la distance entre l'angle actuel et l'angle final
	//on mesure l'écart réel entre l'angle de destination et l'angle actuel référentiel IT
	rotation_restante = CALCULATOR_modulo_angle_22(PILOT_get_destination_rotation() - global.position_rotation);
	global.rotation_restante = rotation_restante;

	angle_frein = (global.vitesse_rotation >> 2)*(global.vitesse_rotation >> 2); // Division par 4 car sinon overflow
	if(in_rush)
		angle_frein /= (2 * PILOT_get_coef(PILOT_ACCELERATION_NORMAL) * BIG_ACCELERATION_RUSH_ROT * PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION));
	else
		angle_frein /= (2 * PILOT_get_coef(PILOT_ACCELERATION_NORMAL) * PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION));
	angle_frein <<= 4;
	//angle_frein -= absolute(global.vitesse_rotation/2);
	global.angle_frein = angle_frein;

	//Si je m'éloigne de l'objectif, je dois accélérer pour m'en rapprocher...
	if	((global.vitesse_rotation >= 0 && rotation_restante < 0 )
		 ||(global.vitesse_rotation <= 0 && rotation_restante > 0 )
		 ||(global.vitesse_rotation == 0 && rotation_restante == 0 ))
	{

		already_braking_rotation = NOT_BRAKING; // On peut considérer qu'on n'a jamais freiner vue qu'on a dépasser le point voulu
		return NOT_BRAKING;
	}

	if(global.vitesse_rotation != 0 && rotation_restante == 0){ // Si on tourne encore alors qu'on doit être arrêté
		PILOT_set_custom_acceleration_rotation(TRUE, 250);
		return BRAKING;
	}

	// A partir d'ici on travail en absolue
	rotation_restante = absolute(rotation_restante);

	if((rotation_restante - angle_frein < 0) // Si le freinage standart ne suffit pas
			|| ((rotation_restante - angle_frein > 0) && (already_braking_rotation == BRAKING))){ // ou si on a déjà commencé à freiner on continue mais plus doucement

		Sint32 acc_frein;			//[mm.4096/5ms/5ms]

		acc_frein = (global.vitesse_rotation >> 2)*(global.vitesse_rotation >> 2); // Division par 4 car sinon overflow

		if(in_rush)
			acc_frein /= absolute(2 * (rotation_restante + absolute(global.vitesse_rotation/2)) * PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION) * BIG_ACCELERATION_RUSH_ROT);
		else
			acc_frein /= absolute(2 * (rotation_restante + absolute(global.vitesse_rotation/2)) * PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION));

		acc_frein <<= 4;

		if(acc_frein > PILOT_get_coef(PILOT_ACCELERATION_NORMAL) * 2)
			acc_frein = PILOT_get_coef(PILOT_ACCELERATION_NORMAL) * 2;
		else if(acc_frein <= PILOT_get_coef(PILOT_ACCELERATION_NORMAL) / 2)
			acc_frein = PILOT_get_coef(PILOT_ACCELERATION_NORMAL) / 2;

		PILOT_set_custom_acceleration_rotation(TRUE, acc_frein);

		return BRAKING;

	}else if(rotation_restante - angle_frein == 0){	// On peut freiner normalement

		PILOT_set_custom_acceleration_rotation(FALSE, 0);

		return BRAKING;

	}else{	// On n'a pas a freiner... On fonce !

		PILOT_set_custom_acceleration_rotation(FALSE, 0);

		return NOT_BRAKING;
	}
}

static braking_e COPILOT_update_brake_state_translation(void)
{
	Sint32 translation_restante, translation_frein;		//[mm.4096]

	translation_restante = PILOT_get_destination_translation() - global.position_translation;
	global.translation_restante = translation_restante;

	translation_frein = global.vitesse_translation * global.vitesse_translation;
	if(in_rush)
		translation_frein /= (2 * rush_brake_acc);
	else
		translation_frein /= (2 * PILOT_get_coef(PILOT_ACCELERATION_NORMAL));
	translation_frein -= absolute(global.vitesse_translation/2);
	global.distance_frein = translation_frein;

	//Si je m'éloigne de l'objectif, je dois accélérer pour m'en rapprocher...
	if( (global.vitesse_translation > 0 && translation_restante < 0)
		|| (global.vitesse_translation < 0 && translation_restante > 0))
	{

		if(braking_translation == BRAKING && !COPILOT_is_arrived() && (COPILOT_get_trajectory() == TRAJECTORY_TRANSLATION))
		{
			SUPERVISOR_number_of_rounds_returns_increment();
			already_braking_translation = NOT_BRAKING; // On peut considérer qu'on n'a jamais freiner vue qu'on a dépasser le point voulu
		}
		return NOT_BRAKING;
	}

	if(global.vitesse_translation == 0 && translation_restante == 0){
		already_braking_translation = NOT_BRAKING; // On peut considérer qu'on n'a jamais freiner vue qu'on a dépasser le point voulu
		return NOT_BRAKING;
	}

	if(global.vitesse_translation != 0 && translation_restante == 0){ // Si on roule encore alors qu'on doit être arrêté
		PILOT_set_custom_acceleration_translation(TRUE, 250);
		return BRAKING;
	}

	// A partir d'ici on travail en absolue
	translation_restante = absolute(translation_restante);

	if(in_rush && (translation_restante - translation_frein < 0) && rush_count_traj == 2){
		ROADMAP_add_order(  TRAJECTORY_STOP,
							0,
							0,
							0,					//teta
							PROP_ABSOLUTE,		//relative
							PROP_NOW,			//maintenant
							ANY_WAY,			//sens de marche
							NOT_BORDER_MODE,	//mode bordure
							PROP_END_AT_POINT, //mode multipoints
							FAST,				//Vitesse
							ACKNOWLEDGE_ASKED,
							CORRECTOR_ENABLE,
							AVOID_DISABLED,
							current_order.idTraj
						);

		return NOT_BRAKING;

	}else if((translation_restante - translation_frein < 0) // Si le freinage standart ne suffit pas
			|| ((translation_restante - translation_frein > 0) && (already_braking_translation == BRAKING))){ // ou si on a déjà commencé à freiner on continue mais plus doucement

		Sint32 acc_frein;	//[mm.4096/5ms/5ms]

		acc_frein = global.vitesse_translation * global.vitesse_translation;
		acc_frein /= absolute(2 * translation_restante + absolute(global.vitesse_translation/2));

		if(in_rush){
			if(acc_frein > rush_brake_acc * 2)
				acc_frein = rush_brake_acc * 2;
			else if(acc_frein <= rush_brake_acc / 2)
				acc_frein = rush_brake_acc / 2;

		}else{
			if(acc_frein > PILOT_get_coef(PILOT_ACCELERATION_NORMAL) * 2)
				acc_frein = PILOT_get_coef(PILOT_ACCELERATION_NORMAL) * 2;
			else if(acc_frein <= PILOT_get_coef(PILOT_ACCELERATION_NORMAL) / 2)
				acc_frein = PILOT_get_coef(PILOT_ACCELERATION_NORMAL) / 2;
		}

		PILOT_set_custom_acceleration_translation(TRUE, acc_frein);

		return BRAKING;

	}else if(translation_restante - translation_frein == 0){	// On peut freiner normalement

		PILOT_set_custom_acceleration_translation(FALSE, 0);

		return BRAKING;

	}else{	// On n'a pas a freiner... On fonce !

		PILOT_set_custom_acceleration_translation(FALSE, 0);

		return NOT_BRAKING;
	}
}


///--------------------------------------------------------------------------------------------------------------
///--------------------------------------------Fonction calculatoire---------------------------------------------
///--------------------------------------------------------------------------------------------------------------


static void COPILOT_destination_angle(Sint16 x, Sint16 y, Sint16 * angle_a_parcourir, Sint16 * teta_destination, way_e way, way_e * chosen_way)
{

	//supposons qu'on soit en marche avant...
	*teta_destination = CALCULATOR_viewing_angle(global.position.x, global.position.y, x, y);



	if(way == FORWARD)
	{
		*chosen_way = FORWARD;
	}

	if(way == BACKWARD)
	{
		*teta_destination += PI4096;
		*teta_destination = CALCULATOR_modulo_angle(*teta_destination);
		*chosen_way = BACKWARD;
	}

	//on cherche l'angle que nous devons parcourir pour joindre cette destination
	*angle_a_parcourir = *teta_destination - global.position.teta;
	*angle_a_parcourir = CALCULATOR_modulo_angle(*angle_a_parcourir);

	if(way == ANY_WAY)
	{
		//supposons que l'on soit en marche avant...
		*chosen_way = FORWARD;


		//Si l'angle a parcourir est plus grand que PI/2... on aurait mieux fait de choisir la marche arrière !
		//l'angle a parcourir sera recalculé ensuite...
		if(absolute(*angle_a_parcourir) > PI4096/2)
		{
			//et on recalcule tout...
			*chosen_way = BACKWARD;
			*teta_destination += PI4096;
			*teta_destination = CALCULATOR_modulo_angle(*teta_destination);
			*angle_a_parcourir = *teta_destination - global.position.teta;
			*angle_a_parcourir = CALCULATOR_modulo_angle(*angle_a_parcourir);
		}
	}
}


///--------------------------------------------------------------------------------------------------------------
///--------------------------------------------Accesseur et Mutateur---------------------------------------------
///--------------------------------------------------------------------------------------------------------------

order_t COPILOT_get_current_order(void)
{
	return current_order;
}

volatile order_t* COPILOT_get_buffer_order(void)
{
	return &buffer_order;
}

void COPILOT_buffering_order(void)
{
	buffer_order = current_order;
}

braking_e COPILOT_braking_rotation_get(void)
{
	return braking_rotation;
}

braking_e COPILOT_braking_translation_get(void)
{
	return braking_translation;
}

way_e COPILOT_get_way(void)
{
	return current_order.way;
}

bool_e COPILOT_is_arrived(void)
{
	if(arrived)
		return TRUE;
	else
		return FALSE;
}

bool_e COPILOT_is_arrived_rotation(void)
{
	if(arrived_rotation)
		return TRUE;
	else
		return FALSE;
}

bool_e COPILOT_is_arrived_translation(void)
{
	if(arrived_translation)
		return TRUE;
	else
		return FALSE;
}

border_mode_e COPILOT_get_border_mode(void)
{
	return current_order.border_mode;
}

trajectory_e COPILOT_get_trajectory(void)
{
	return current_order.trajectory;
}

Uint8 COPILOT_get_actualTrajID(){
	return actualTrajID;
}

void COPILOT_set_in_rush(bool_e in_rush_msg, Sint16 first_traj_acc_msg, Sint16 second_traj_acc_msg, Sint16 brake_acc_msg, Uint8 acc_rot_trans_msg){
	if(in_rush_msg){
		rush_count_traj = 0;
		rush_first_traj_acc = first_traj_acc_msg;
		rush_second_traj_acc = second_traj_acc_msg;
		rush_brake_acc = brake_acc_msg;
		rush_acc_rot_trans = acc_rot_trans_msg;
		in_rush = TRUE;
	}else{
		in_rush = FALSE;
		avoid_in_rush = FALSE;
		PILOT_set_coef(PILOT_ACCELERATION_NORMAL, 0);
		PILOT_set_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION, 0);
		CORRECTOR_reset_coef();
	}

}

void COPILOT_set_avoid_in_rush(bool_e avoid){
	avoid_in_rush = avoid;
}
