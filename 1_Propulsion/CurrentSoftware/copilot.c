/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier :  copilot.c
 *  Package : Asser
 *  Description : Fonctions qui g�rent les trajectoires
 *  Auteur : Inspir� par Val' 2007 - Nirgal 2009
 *  Version 200904
 */


#include "copilot.h"
#include "roadmap.h"
#include "buffer.h"
#include "supervisor.h"
#include "pilot.h"
#include "calculator.h"
#include "odometry.h"
#include "QS/QS_maths.h"
#include "debug.h"
#include "QS/QS_watchdog.h"
#include "QS/QS_who_am_i.h"
volatile order_t current_order;

trajectory_e COPILOT_decision_rotation_before_translation(Sint16 destination_x, Sint16 destination_y, Sint16 viewing_angle, way_e way);
typedef enum {
	NOT_ARRIVED = 0,
	ARRIVED,
} arrived_e;
volatile arrived_e arrived;
volatile arrived_e arrived_rotation;
volatile arrived_e arrived_translation;

volatile braking_e braking;	//a commenc� a freiner depuis le d�but de la trajectoire, ou non.
volatile braking_e braking_translation;
volatile braking_e braking_rotation;



void COPILOT_destination_angle(Sint16 x, Sint16 y, Sint16 * angle_a_parcourir, Sint16 * teta_destination, way_e sens_marche, way_e * sens_marche_choisi);
void COPILOT_update_arrived(void);
void COPILOT_reset_absolute_destination(void);
void COPILOT_update_destination_translation(void);
void COPILOT_update_destination_rotation(void);
bool_e COPILOT_decision_change_order(bool_e * change_order_in_multipoint_without_reaching_destination);
void COPILOT_try_order(order_t * order, bool_e change_order_in_multipoint_without_reaching_destination);
arrived_e Decision_robot_arrive_translation(void);
arrived_e Decision_robot_arrive_rotation(void);
void COPILOT_update_brake_state(void);
braking_e COPILOT_update_brake_state_rotation(void);
braking_e COPILOT_update_brake_state_translation(void);

void COPILOT_init(void)
{
	arrived = ARRIVED;
	COPILOT_reset_absolute_destination();
	current_order = (order_t){TRAJECTORY_NONE, 0,0,0, NOT_RELATIVE, NO_MULTIPOINT, NOT_BORDER_MODE, FORWARD, FAST, ACKNOWLEDGED, 0};
	braking = NOT_BRAKING;
	braking_translation = NOT_BRAKING;
	braking_rotation = NOT_BRAKING;
}



void COPILOT_process_it(void)
{
	order_t next_order;
	bool_e  change_order_in_multipoint_without_reaching_destination = FALSE;
	static watchdog_id_t id_end_enslavement = 0xFF;
	static bool_e flag_end_enslavement = FALSE;

	if(current_order.trajectory == TRAJECTORY_STOP)
		COPILOT_reset_absolute_destination();

	if(arrived !=ARRIVED)
	{
		if(global.mode_best_effort_enable)
		{
			//Ce mode est utilis� pour les r�glages de coefs... c'est un mode de d�bogage.
			//Dans ce mode, le robot ne suit pas un point qui a toujours la m�me trajectoire...
			//le point suivi est en fait recalcul� EN FONCTION de la position REELLE du robot
			//Donc si le robot d�vie de sa trajectoire (perturbation...), celle ci est recalcul�e en permanence !
			//Cela provoque des trajectoires al�atoires, surtout si les coefs sont mal r�gl�s
			//Ce qui veut dire que c'est pratique pour r�gler les coefs !
			global.position_rotation = global.ecart_rotation_prec;
			global.position_translation = global.ecart_translation_prec;
		}
		else
		{		//MODE NORMAL !
			//On retranche de la position du r�f�rentiel IT l'avancement r�el du robot (avec un robot parfait, on retomberait � 0)
			//Si le robot n'a pas parfaitement avanc� de ce que l'on esp�rait, cette position sera non nulle...
			//	et prise en compte par l'asservissement, qui devra ajouter/retirer un poil de jus sur les moteurs pour compenser cette erreur...
			global.position_rotation -= global.real_position_rotation >> 10;
			global.position_translation -= global.real_position_translation;
		}
		//on remet a jour le point destination dans le r�f�rentiel IT
		COPILOT_update_destination_translation();
		COPILOT_update_destination_rotation();
	}

	COPILOT_update_arrived();
	COPILOT_update_brake_state();

	if(current_order.trajectory == TRAJECTORY_STOP && arrived == ARRIVED && global.match_over == TRUE && id_end_enslavement == 0xFF)
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




bool_e COPILOT_decision_change_order(bool_e * change_order_in_multipoint_without_reaching_destination)
{
	bool_e buffer_vide;
	buffer_vide = BUFFER_is_empty();

	if(ROADMAP_exists_prioritary_order())
		return TRUE;

	if(current_order.trajectory == WAIT_FOREVER)
		return FALSE;

	if(current_order.multipoint == MULTIPOINT)
	{
		if(arrived == ARRIVED)
		{
			if(buffer_vide)
			{
				SUPERVISOR_state_machine(EVENT_ARRIVED, 0);
				SUPERVISOR_state_machine(EVENT_NOTHING_TO_DO, 0);
			}
			else
			{
				*change_order_in_multipoint_without_reaching_destination = TRUE;
				return TRUE;
			}
		}
		else
		{
			//Si la rotation et la translation ont d�j� frein� au moins une fois depuis le d�but de la trajectoire multipoint
			//Alors, on passe au point suivant sans m�me accepter de freiner une fois !!!
			if( (current_order.trajectory == TRAJECTORY_ROTATION && braking_rotation == BRAKING)
				|| (current_order.trajectory != TRAJECTORY_ROTATION && braking_translation == BRAKING &&
					(arrived_rotation == ARRIVED || braking_rotation == BRAKING)))
			{
				SUPERVISOR_state_machine(EVENT_BRAKING, 0);
				if(!buffer_vide)
				{
					*change_order_in_multipoint_without_reaching_destination = TRUE;
					return TRUE;
				}
			}
		}
	}
	else //en NON MULTIPOINT
	{
		if(arrived == ARRIVED)
		{
			//Si je ne suis pas en multipoint et que je suis arriv�.
			SUPERVISOR_state_machine(EVENT_ARRIVED, 0);

			if(!buffer_vide)		//Il y a un ordre � traiter.
				return TRUE;
			else					//Il n'y a rien � faire.
				SUPERVISOR_state_machine(EVENT_NOTHING_TO_DO, 0);
		}
	}

	return FALSE;
}

border_mode_e COPILOT_get_border_mode(void)
{
	return current_order.border_mode;
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


trajectory_e COPILOT_get_trajectory(void)
{
	return current_order.trajectory;
}
//lance la trajectoire suivante du buffer
void COPILOT_try_order(order_t * order, bool_e change_order_in_multipoint_without_reaching_destination)
{
	Sint32 d;
	//debug_printf("next traj\n");
	static Sint16 angle_a_parcourir;

	CORRECTOR_PD_enable(order->corrector);

	if(order->trajectory == WAIT_FOREVER){
		COPILOT_do_order(order);
		return;
	}


	if(order->relative == RELATIVE) //Si l'ordre est relatif, c'est maintenant qu'il doit devenir absolu !
	{
			//REMARQUE : si on est en multipoint, et qu'on vient de changer d'ordre � l'instant du freinage, sans avoir atteint la position vis�e,
			//La relativit� doit alors �tre calcul�e sur la destination vis�e, et non pas sur la position actuelle !!!!!!
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

			order->relative = NOT_RELATIVE; 	//Ce n'est PLUS relatif !!!!!!!!
					//Note : si la trajectoire est remise en buffer pour une rotation pr�alable par exemple, les nouvelles coordonn�es non relatives seront prises en compte !
	}

	switch(order->trajectory)
	{
		case TRAJECTORY_STOP:
			order->x = 0;
			order->y = 0;
		break;

		case TRAJECTORY_ROTATION:
			//	si rotation pure -> x et y demand� = position actuelle

			//si la vitesse en translation est �lev�e, on commence par s'arreter. Ceci �vite l'effet suivant :
			//on chercherait � tourner autour du point actuel... or si on continue d'avancer, il faut rejoindre ce fameux point...
			if( absolute(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
			{
				//On r��crit le reste de la trajectoire pour la suite...
				BUFFER_add_begin(order);	//On remet l'ordre en l'�tat dans le buffer...
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

			//Quelle trajectoire dois-je commencer par faire ? ( = faut il s'arreter, tourner vers le point, o� y aller directement...?)
			switch(COPILOT_decision_rotation_before_translation(order->x, order->y, angle_a_parcourir, order->way))
			{
				case TRAJECTORY_ROTATION:
					//On r��crit le reste de la trajectoire pour la suite...
					BUFFER_add_begin(order);

					//Les rotations pr�alables pourraient se faire en mode multipoints.
					order->multipoint = NO_MULTIPOINT;

					//on ne pr�vient pas la carte P sur rotation pr�alable ajout�e par nos soins.
					order->acknowledge = NO_ACKNOWLEDGE;
					order->x = global.position.x;
										order->y = global.position.y;
										order->trajectory = TRAJECTORY_ROTATION;
					//on ne fait qu'une rotation pr�alable pour le moment
				break;
				case TRAJECTORY_STOP:
					BUFFER_add_begin(order);
					//on ne pr�vient pas la carte P sur arr�t pr�alable ajout�e par nos soins.
					order->multipoint = NO_MULTIPOINT;
					order->acknowledge = NO_ACKNOWLEDGE;
					order->trajectory = TRAJECTORY_STOP;
										order->x = 0;
										order->y = 0;
				break;
				case TRAJECTORY_NONE:
					//je suis d�j� arriv� au bon point... donc je marque un arr�t histoire de bien r�pondre que je suis arriv�...
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
							//Nous y sommes d�j� !
			}
			else
			{
				if(angle_a_parcourir<=-(HALF_PI4096*9)/10 || angle_a_parcourir >= (HALF_PI4096*9)/10)	//Si l'angle � parcourir est plus grand que PI/2 ou un petit peu moins -> rotation pr�alable.
				{
					//debug_printf("rotation pr�alable courbe\r\n");
					//On r��crit le reste de la trajectoire pour la suite...
					BUFFER_add_begin(order);

					//Les rotations pr�alables pourraient se faire en mode multipoints.
					order->multipoint = NO_MULTIPOINT;
					order->acknowledge = NO_ACKNOWLEDGE;
					//on ne pr�vient pas la carte strat�gie sur rotation pr�alable ajout�e par nos soins.

					//on ne fait qu'une rotation pr�alable pour le moment (ou un arr�t si n�cessaire...)
					order->x = global.position.x;
					order->y = global.position.y;
					if(absolute(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
						order->trajectory = TRAJECTORY_STOP;		//Besoin de faire un arr�t pr�alable � la rotation pr�alable
					else
						order->trajectory = TRAJECTORY_ROTATION;	//Nous sommes bien arret�s : nous pouvons faire une rotation pr�alable !

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
void COPILOT_do_order(order_t * order)
{

	#ifdef USE_PROP_AVOIDANCE
	if((order->trajectory == TRAJECTORY_AUTOMATIC_CURVE || order->trajectory == TRAJECTORY_TRANSLATION) &&
			AVOIDANCE_foe_in_zone(FALSE, order->x, order->y, FALSE)){ // Fonction diff�rente � faire pour une trajectoire en courbe automatique

		if(order->avoidance == AVOID_ENABLED){ // adversaire sur la trajectoire, �vitement sans wait donc annulation de la trajectoire
			CAN_msg_t msg;
			msg.sid = STRAT_PROP_FOE_DETECTED;
			msg.size = 0;

			BUFFER_flush();
			ROADMAP_add_order(  TRAJECTORY_STOP,
								0,
								0,
								0,					//teta
								NOT_RELATIVE,		//relative
								NOW,			//maintenant
								ANY_WAY,	//sens de marche
								NOT_BORDER_MODE,	//mode bordure
								NO_MULTIPOINT, 	//mode multipoints
								FAST,				//Vitesse
								ACKNOWLEDGE_ASKED,
								CORRECTOR_ENABLE,
								AVOID_DISABLED
							);
			SECRETARY_send_canmsg(&msg);
		}else if(order->avoidance == AVOID_ENABLED_AND_WAIT){
			TIMER1_disableInt(); // Inibation des ITs critique
			ROADMAP_add_in_begin_order( WAIT_FOREVER,
										0,					//x
										0,					//y
										0,					//teta
										NOT_RELATIVE,		//relative
										ANY_WAY,	//sens de marche
										NOT_BORDER_MODE,	//mode bordure
										NO_MULTIPOINT, 	//mode multipoints
										FAST,				//Vitesse
										ACKNOWLEDGE_ASKED,
										CORRECTOR_ENABLE,
										AVOID_DISABLED
									);
			ROADMAP_add_in_begin_order( TRAJECTORY_STOP,
										0,					//x
										0,					//y
										0,					//teta
										NOT_RELATIVE,		//relative
										NOW,			//maintenant
										ANY_WAY,	//sens de marche
										NOT_BORDER_MODE,	//mode bordure
										NO_MULTIPOINT, 	//mode multipoints
										FAST,				//Vitesse
										ACKNOWLEDGE_ASKED,
										CORRECTOR_ENABLE,
										AVOID_DISABLED
									);
			TIMER1_enableInt(); // D�-inibation des ITs critique
		}

	}else{
	#endif

		//IMPORTANT, � ce stade, le type de trajectoire peut etre ROTATION, TRANSLATION, AUTOMATIC_CURVE ou STOP
		//Les coordonn�es ne sont PLUS relatives !!!
		current_order = *order;

		if(current_order.trajectory == WAIT_FOREVER)
			return;

		//MAJ Vitesse
		PILOT_set_speed(current_order.speed);

		SUPERVISOR_state_machine(EVENT_NEW_ORDER, current_order.acknowledge);

		arrived = NOT_ARRIVED;	//Ceci pour �viter, dans le main, de croire qu'on est arriv� AVANT la mise a jour de l'�tat arrive dans it.c
		arrived_rotation = NOT_ARRIVED;
		arrived_translation = NOT_ARRIVED;
		braking_translation = NOT_BRAKING;
		braking_rotation = NOT_BRAKING;
		braking = NOT_BRAKING;
		//On remet � jour la destination (pour que la d�tection d'erreur soit OK...)
		COPILOT_update_destination_translation();
		COPILOT_update_destination_rotation();
		COPILOT_update_arrived();
		COPILOT_update_brake_state();
	#ifdef USE_PROP_AVOIDANCE
	}
	#endif
}



//CI DESSUS DES FONCTIONS ISSUS D'APPEL DANS LE MAIN
//////////////////////////////////////////////////////////////////////////////////////////////
//CI DESSOUS DES FONCTIONS ESSENTIELLEMENT APPELLES EN IT


void COPILOT_update_destination_rotation(void)
{
	Sint32 angle;

	//La mise � jour de la destination en angle souhait�e (r�f�rentiel du terrain) se fait lorsqu'on parcours des courbes, ou plus g�n�ral des trajectoires autres que des rotations pures... m�me si a priori ca doit pas etre genant !
	if(		current_order.trajectory != TRAJECTORY_ROTATION &&
			current_order.border_mode == NOT_BORDER_MODE &&
			arrived == NOT_ARRIVED &&
			(absolute(PILOT_get_destination_translation()) > THRESHOLD_STOPPING_CALCULATION_VIEWING_ANGLE)
		)
	{
		angle = CALCULATOR_viewing_angle(global.position.x, global.position.y, current_order.x, current_order.y);
		//supposons qu'on soit en marche avant...
		//current_order.teta = calcul_angle_de_vue(global.position.x, global.position.y, current_order.x, current_order.y);
//		if(abs(angle) > PI4096/32)
			current_order.teta = angle;


		//Si l'angle a parcourir est plus grand que PI/2... on aurait mieux fait de choisir la marche arri�re !
		//l'angle a parcourir sera recalcul� ensuite...
		if(absolute(CALCULATOR_modulo_angle(current_order.teta - global.position.teta)) > HALF_PI4096
			 && current_order.way != FORWARD)
		{
			current_order.teta += PI4096;
		}
		current_order.teta = CALCULATOR_modulo_angle(current_order.teta);
	}

										//Si destination plus loin que PI... autant tourner dans l'autre sens !
	PILOT_set_destination_rotation (CALCULATOR_modulo_angle(((Sint32)current_order.teta - (Sint32)global.position.teta)));


}


//Attention, toujours appeler en premier MAJ_rotation... car MAJ_translation se sert de l'angle calcul� dans MAJ_rotation
//mais en fait pas forc�ment...
void COPILOT_update_destination_translation(void)
{
	if(current_order.trajectory == TRAJECTORY_ROTATION)
		PILOT_set_destination_translation(0);
	else
		PILOT_set_destination_translation((CALCULATOR_viewing_algebric_distance(
												global.position.x,
												global.position.y,
												current_order.x,
												current_order.y,
												CALCULATOR_modulo_angle(
																		CALCULATOR_viewing_angle(
																			global.position.x,
																			global.position.y,
																			current_order.x,
																			current_order.y)
																		- global.position.teta)
																)
											)<<12);
}

void COPILOT_reset_absolute_destination(void)
{
	//La destination est la position actuelle...
		current_order.x = global.position.x;
		current_order.y = global.position.y;
		current_order.teta = global.position.teta;
}




// Cette d�cision concerne le ROBOT et non le point fictif...
arrived_e Decision_robot_arrive_bordure(void)
{
	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
		return DEBUG_get_we_touch_border();
	#endif
	//si on est en mode calage, la condition d'arriv�e est un �cart entre le point fictif et notre position...
	if(absolute(global.ecart_translation)>((QS_WHO_AM_I_get()==BIG_ROBOT)?BIG_TRESHOLD_CALIBRATION_TRANSLATION:SMALL_TRESHOLD_CALIBRATION_TRANSLATION))
	{
		//accepter l'�cart obtenu...
		PILOT_referential_reset();
		PILOT_referential_init();
		COPILOT_update_destination_translation();
		COPILOT_update_destination_rotation();

		//Mise � jour de la position robot demand�e.
		if(current_order.border_mode == BORDER_MODE_WITH_UPDATE_POSITION)
			ODOMETRY_correct_with_border(current_order.way);

	//debug_printf("ARRIVE_BORDURE_ASSER_AVANT : trans:%ld | rot:%ld\n",abs(global.ecart_translation), abs(global.ecart_rotation));


	//	global.ecart_translation = global.position_translation - global.position_translation_reelle; // positif si le robot doit avancer
	//	global.ecart_rotation = global.position_rotation - (global.position_rotation_reelle >> 10);  //positif pour rotation dans le sens trigo
	//debug_printf("ARRIVE_BORDURE_ASSER_APRES : trans:%ld | rot:%ld\n",abs(global.ecart_translation), abs(global.ecart_rotation));

		return ARRIVED;
	}
	else
		return NOT_ARRIVED;


}


braking_e Decision_robot_proche_pour_carte_p()
{
	//MAINTIENT
	//on a d�j� frein� une fois depuis le d�but de la trajectoire...
	if(braking == BRAKING)
		return BRAKING;

	//La premi�re fois que rot et trans sont en freinage, on consid�re que le ROBOT BRAKING
	//on pourra alors pr�venir la carte P qu'on est en approche,
	if( braking_translation == BRAKING && braking_rotation == BRAKING)
		return BRAKING;

	return NOT_BRAKING;
}








// Cette d�cision concerne le ROBOT et non le point fictif...
arrived_e Decision_robot_arrive_arret(void)
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


// Cette d�cision concerne le ROBOT et non le point fictif...
void COPILOT_update_arrived(void)
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





// Cette d�cision concerne le ROBOT et non le point fictif...
arrived_e Decision_robot_arrive_rotation(void)
{
	//CONDITION DE CONSIDERATION ROBOT ARRIVE SUR ROTATION = vitesse faible et position proche
	if ((absolute(global.real_speed_rotation) < PRECISION_ARRIVE_SPEED_ROTATION/*/diviseur*/) &&
		(absolute(CALCULATOR_modulo_angle((global.real_position_rotation >> 10) - PILOT_get_destination_rotation())) < PRECISION_ARRIVE_POSITION_ROTATION  ))
		return ARRIVED;
	else
		return NOT_ARRIVED;
}


// Cette d�cision concerne le ROBOT et non le point fictif...
arrived_e Decision_robot_arrive_translation(void)
{

	if(current_order.trajectory == TRAJECTORY_ROTATION)
	{
					//Je n'ai pas d'explication pertinente pour justifier cette division par 4...
		if ( (absolute(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION/4 ) &&
				(absolute(global.real_position_translation-PILOT_get_destination_translation()) < PRECISION_ARRIVE_POSITION_TRANSLATION)  )	//juste pour qu'il ne pense pas qu'il est arriv� tout au d�but de la trajectoire, au moment o� sa vitesse est nulle !!!
			return ARRIVED;
		else
			return NOT_ARRIVED;
	}
	else
	{
		if ( (absolute(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION ) &&
				(absolute(global.real_position_translation-PILOT_get_destination_translation()) < PRECISION_ARRIVE_POSITION_TRANSLATION)  )	//juste pour qu'il ne pense pas qu'il est arriv� tout au d�but de la trajectoire, au moment o� sa vitesse est nulle !!!
			return ARRIVED;
		else
			return NOT_ARRIVED;
	}
}





void COPILOT_update_brake_state(void)
{
	braking_translation = COPILOT_update_brake_state_translation();
	braking_rotation = COPILOT_update_brake_state_rotation();
}

braking_e COPILOT_braking_rotation_get(void)
{
	return braking_rotation;
}

braking_e COPILOT_braking_translation_get(void)
{
	return braking_translation;
}

braking_e COPILOT_get_braking(void)
{
	return braking;
}



// Cette d�cision concerne le point fictif...
braking_e COPILOT_update_brake_state_rotation()
{
	//Si je m'�loigne de l'objectif, je dois acc�l�rer pour m'en rapprocher...
	if	(
					(
							(global.vitesse_rotation) >= 0
							&&
							(  CALCULATOR_modulo_angle( PILOT_get_destination_rotation() - global.position_rotation) < 0 )
					)
					||
					(
							(global.vitesse_rotation) <= 0
							&&
							(  CALCULATOR_modulo_angle( PILOT_get_destination_rotation() - global.position_rotation) > 0 )
					)
			)
	{
			return NOT_BRAKING;
	}


	Sint32 rotation_restante;


	//Calcul de la distance entre l'angle actuel et l'angle final
	//on mesure l'�cart r�el entre l'angle de destination et l'angle actuel r�f�rentiel IT

	rotation_restante = absolute(CALCULATOR_modulo_angle( PILOT_get_destination_rotation() - global.position_rotation));

//	float angle_frein;
	// Calcul de la distance pour freiner
//	angle_frein = (global.vitesse_rotation)*(global.vitesse_rotation >>10); //a cause des vitesses exprim�es en <<10...
//	angle_frein /= (2*ACCELERATION_NORMAL*COEFF_ACCELERATION_ROTATION_TRANSLATION);
//
	Sint32 angle_frein;
	angle_frein = (global.vitesse_rotation >> 2)*(global.vitesse_rotation >> 2);
	angle_frein /= (2*PILOT_get_coef(PILOT_ACCELERATION_NORMAL)*PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION));
	angle_frein -= absolute(global.vitesse_rotation/2 >> 4);
	angle_frein >>= 6;

	PILOT_set_extra_braking_rotation((rotation_restante - angle_frein<0)?TRUE:FALSE);


	//On BRAKING si la distance qui nous s�pare de l'angle final est inf�rieure � la distance qu'il faut pour freiner
	if (rotation_restante <= angle_frein)
			return BRAKING;
	else
			return NOT_BRAKING;
}

// Cette d�cision concerne le point fictif...
braking_e COPILOT_update_brake_state_translation(void)
{
	//Si je m'�loigne de l'objectif, je dois acc�l�rer pour m'en rapprocher...
	if( (global.vitesse_translation > 0 && global.position_translation > PILOT_get_destination_translation())
		|| (global.vitesse_translation < 0 && global.position_translation < PILOT_get_destination_translation())  )
	{

		if(braking_translation == BRAKING && !COPILOT_is_arrived() && (COPILOT_get_trajectory() == TRAJECTORY_TRANSLATION))
		{
//			CAN_send_point_fictif(0xFFFF, 0xEEEE);
			SUPERVISOR_number_of_rounds_returns_increment();
		}
		return NOT_BRAKING;
	}

	Sint32 translation_frein, translation_restante;
	translation_frein = absolute(((global.vitesse_translation*global.vitesse_translation)/(PILOT_get_coef(PILOT_ACCELERATION_NORMAL)))/2);
	translation_frein -= absolute(global.vitesse_translation/2);

	translation_restante = absolute(PILOT_get_destination_translation() - global.position_translation);

	PILOT_set_extra_braking_translation((translation_restante - translation_frein < 0)?TRUE:FALSE);


//	if (global.robot_freine_translation == BRAKING)
//		return BRAKING;

	if (translation_restante <= translation_frein)
		return BRAKING;
	else
		return NOT_BRAKING;
}







/*Fonction test avant le depart*/
//Fonction appel�e si la trajectoire demand�e est une translation...
//TRANSLATION suppose qu'on veuille rejoindre le point en LIGNE DROITE...
//On s'engage donc � ne pas faire de 'courbe' trop marqu�e...
trajectory_e COPILOT_decision_rotation_before_translation(Sint16 destination_x, Sint16 destination_y, Sint16 angle_a_parcourir, way_e sens_marche)
{
	//Informations :
				//on ne fait une rotation pr�alable qu'apr�s ARRET du robot (vitesses nulles...)
				//il faut donc lancer un type de trajectoire ARRET avant de relancer la rotation
				//donc, on r��crit la traj actuelle, et on ne fait la rotation que SI nos vitesses sont nulles
				//si ma vitesse en translation est trop grande, je marque un arret...
	Sint32 d;
	d = absolute((Sint32)(CALCULATOR_viewing_algebric_distance (global.position.x, global.position.y, destination_x, destination_y, angle_a_parcourir))<<12);


	if (global.vitesse_rotation > PRECISION_ARRIVE_SPEED_ROTATION )
	{
	//	debug_printf("arret pr�alable\n");
		return TRAJECTORY_STOP;	//Si notre vitesse en rotation est non nulle, il est difficile de savoir si on r�ussiera � parcourir la courbe
	}

	//Si on est d�j� sur le point demand� >>> aucune trajectoire..... on fait donc la trajectoire "AUCUNE", qui arrivera directement.
	if(d < PRECISION_ARRIVE_POSITION_TRANSLATION && absolute(global.vitesse_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION)
	{
	//	debug_printf("Point trop proche\n");
		return TRAJECTORY_NONE;
	}
	if(absolute(angle_a_parcourir) > ANGLE_MAXI_LANCEMENT_TRANSLATION)
	{
		//larotation pr�alable doit d�marrer par un arr�t si la vitesse est non nulle !
		//sinon, on voudra tourner sur le point actuel, alors qu'on est encore en train d'avancer !
		if(absolute(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
			return TRAJECTORY_STOP;
		else	//cool, c'est parti pour une rotation pr�alable !
			return TRAJECTORY_ROTATION;
	}

	//Je n'autorise la translation que si l'angle � parcourir est tr�s faible...
	return TRAJECTORY_TRANSLATION;
//	if(d < DISTANCE_TRAJECTOIRE_COURBE_INTERDITE)
//		return TRAJECTORY_ROTATION;


}


void COPILOT_destination_angle(Sint16 x, Sint16 y, Sint16 * angle_a_parcourir, Sint16 * teta_destination, way_e way, way_e * chosen_way)
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


		//Si l'angle a parcourir est plus grand que PI/2... on aurait mieux fait de choisir la marche arri�re !
		//l'angle a parcourir sera recalcul� ensuite...
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

order_t COPILOT_get_current_order(){
	return current_order;
}

