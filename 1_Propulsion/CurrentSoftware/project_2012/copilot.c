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
#include "calculator.h"
#include "odometry.h"
#include "cos_sin.h"

volatile order_t current_order;

	
trajectory_e COPILOT_decision_rotation_before_translation(Sint16 destination_x, Sint16 destination_y, Sint16 viewing_angle, way_e way);
typedef enum {
	NOT_ARRIVED = 0,
	ARRIVED,
} arrived_e;
volatile arrived_e arrived;
volatile arrived_e arrived_rotation;
volatile arrived_e arrived_translation;

volatile braking_e braking;	//a commencé a freiner depuis le début de la trajectoire, ou non.
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

	if(current_order.trajectory == TRAJECTORY_STOP)
		COPILOT_reset_absolute_destination();

	if(arrived !=ARRIVED)
	{
		//la position du point fictif dans le référentiel PD est juste devant nous, à l'écart pret...
		//si vous comprenez pas, contactez Nirgal : samuelp5@gmail.com
		global.position_rotation = global.ecart_rotation_prec;
		global.position_translation = global.ecart_translation_prec;
	
		//on remet a jour le point destination dans le référentiel IT
		COPILOT_update_destination_translation();
		COPILOT_update_destination_rotation();
	}
		
	COPILOT_update_arrived();
	COPILOT_update_brake_state();
	 
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
			//Si la rotation et la translation ont déjà freiné au moins une fois depuis le début de la trajectoire multipoint
			//Alors, on passe au point suivant sans même accepter de freiner une fois !!!
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
			//Si je ne suis pas en multipoint et que je suis arrivé.	
			SUPERVISOR_state_machine(EVENT_ARRIVED, 0);
				
			if(!buffer_vide)		//Il y a un ordre à traiter.
				return TRUE;	
			else					//Il n'y a rien à faire.
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

	
	if(order->relative == RELATIVE) //Si l'ordre est relatif, c'est maintenant qu'il doit devenir absolu !
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

			order->relative = NOT_RELATIVE; 	//Ce n'est PLUS relatif !!!!!!!!
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
			if( abs(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
			{
				//On réécrit le reste de la trajectoire pour la suite...
				BUFFER_add_begin(order);	//On remet l'ordre en l'état dans le buffer...
				order->trajectory = TRAJECTORY_STOP;
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
			switch(COPILOT_decision_rotation_before_translation(order->x, order->y, angle_a_parcourir, order->way))
			{
				case TRAJECTORY_ROTATION:
					//On réécrit le reste de la trajectoire pour la suite...
					BUFFER_add_begin(order);
						
					//Les rotations préalables pourraient se faire en mode multipoints.
					order->multipoint = NO_MULTIPOINT;
					
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
					order->multipoint = NO_MULTIPOINT;
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
			d = abs((Sint32)(CALCULATOR_viewing_algebric_distance (global.position.x, global.position.y, order->x, order->y, angle_a_parcourir))<<12);

			if(d < PRECISION_ARRIVE_POSITION_TRANSLATION && abs(global.vitesse_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION)
			{
				order->trajectory = TRAJECTORY_STOP;
								order->x = 0;
								order->y = 0;
						 	//Nous y sommes déjà !
			}
			else
			{
				if(angle_a_parcourir<-HALF_PI4096 || angle_a_parcourir > HALF_PI4096)
				{
					//debug_printf("rotation préalable courbe\r\n");
					//On réécrit le reste de la trajectoire pour la suite...
					BUFFER_add_begin(order);
						
					//Les rotations préalables pourraient se faire en mode multipoints.
					order->multipoint = NO_MULTIPOINT;
					order->acknowledge = NO_ACKNOWLEDGE;
					//on ne prévient pas la carte stratégie sur rotation préalable ajoutée par nos soins.
									
					//on ne fait qu'une rotation préalable pour le moment (ou un arrêt si nécessaire...)
					order->x = global.position.x;
					order->y = global.position.y;
					if(abs(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
						order->trajectory = TRAJECTORY_STOP;		//Besoin de faire un arrêt préalable à la rotation préalable
					else	
						order->trajectory = TRAJECTORY_ROTATION;	//Nous sommes bien arretés : nous pouvons faire une rotation préalable !
											
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
	//IMPORTANT, à ce stade, le type de trajectoire peut etre ROTATION, TRANSLATION, AUTOMATIC_CURVE ou STOP
	//Les coordonnées ne sont PLUS relatives !!!
	current_order = *order;

	//MAJ Vitesse
	PILOT_set_speed(current_order.speed);

	CORRECTOR_PD_enable(current_order.corrector);
	
	SUPERVISOR_state_machine(EVENT_NEW_ORDER, current_order.acknowledge);
	
	arrived = NOT_ARRIVED;	//Ceci pour éviter, dans le main, de croire qu'on est arrivé AVANT la mise a jour de l'état arrive dans it.c
	arrived_rotation = NOT_ARRIVED;
	arrived_translation = NOT_ARRIVED;
	braking_translation = NOT_BRAKING;
	braking_rotation = NOT_BRAKING;
	braking = NOT_BRAKING;
	//On remet à jour la destination (pour que la détection d'erreur soit OK...)
	COPILOT_update_destination_translation();
	COPILOT_update_destination_rotation();
	COPILOT_update_arrived();
	COPILOT_update_brake_state();
}



//CI DESSUS DES FONCTIONS ISSUS D'APPEL DANS LE MAIN
//////////////////////////////////////////////////////////////////////////////////////////////
//CI DESSOUS DES FONCTIONS ESSENTIELLEMENT APPELLES EN IT


void COPILOT_update_destination_rotation(void)
{
	Sint32 angle;
	
	//La mise à jour de la destination en angle souhaitée (référentiel du terrain) se fait lorsqu'on parcours des courbes, ou plus général des trajectoires autres que des rotations pures... même si a priori ca doit pas etre genant !
	if(		current_order.trajectory != TRAJECTORY_ROTATION &&
			current_order.border_mode == FALSE &&
			arrived == NOT_ARRIVED &&
			(abs(PILOT_get_destination_translation()) > THRESHOLD_STOPPING_CALCULATION_VIEWING_ANGLE)
		)
	{
		angle = CALCULATOR_viewing_angle(global.position.x, global.position.y, current_order.x, current_order.y);
		//supposons qu'on soit en marche avant...
		//current_order.teta = calcul_angle_de_vue(global.position.x, global.position.y, current_order.x, current_order.y);
		if(abs(angle) > PI4096/32)
			current_order.teta = angle;

		
		//Si l'angle a parcourir est plus grand que PI/2... on aurait mieux fait de choisir la marche arrière !
		//l'angle a parcourir sera recalculé ensuite...
		if(abs(CALCULATOR_modulo_angle(current_order.teta - global.position.teta)) > HALF_PI4096
			 && current_order.way != FORWARD)
		{
			current_order.teta += PI4096;	
		}	
		current_order.teta = CALCULATOR_modulo_angle(current_order.teta);
	}

										//Si destination plus loin que PI... autant tourner dans l'autre sens !
	PILOT_set_destination_rotation (CALCULATOR_modulo_angle(((Sint32)current_order.teta - (Sint32)global.position.teta)));


}	


//Attention, toujours appeler en premier MAJ_rotation... car MAJ_translation se sert de l'angle calculé dans MAJ_rotation
//mais en fait pas forcément...
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




// Cette décision concerne le ROBOT et non le point fictif...
arrived_e Decision_robot_arrive_bordure(void)
{
	
	//si on est en mode calage, la condition d'arrivée est un écart entre le point fictif et notre position...
	if(abs(global.ecart_translation)>TRESHOLD_CALIBRATION_TRANSLATION)
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
	//on a déjà freiné une fois depuis le début de la trajectoire...
	if(braking == BRAKING)
		return BRAKING;
		
	//La première fois que rot et trans sont en freinage, on considère que le ROBOT BRAKING
	//on pourra alors prévenir la carte P qu'on est en approche,
	if( braking_translation == BRAKING && braking_rotation == BRAKING)
		return BRAKING;

	return NOT_BRAKING;
}	





	
// Cette décision concerne le ROBOT et non le point fictif...	
void COPILOT_update_arrived(void)
{
	
//	if(arrived_rotation  != ARRIVED)
	if(current_order.border_mode == BORDER_MODE || current_order.border_mode == BORDER_MODE_WITH_UPDATE_POSITION)
		arrived_rotation 	= ARRIVED;
	else
		arrived_rotation 	= Decision_robot_arrive_rotation();
	
	if(arrived_translation != ARRIVED)
		arrived_translation = Decision_robot_arrive_translation();
		
	if(arrived == ARRIVED)
		return;
	
	if(current_order.trajectory == TRAJECTORY_ROTATION)
		arrived = arrived_rotation;
	else
		arrived = arrived_translation;
	
	//si le mode bordure est actif
	if(current_order.border_mode == BORDER_MODE || current_order.border_mode == BORDER_MODE_WITH_UPDATE_POSITION)
		if(arrived != ARRIVED)	//MAINTIENT DE ROBOT ARRIVE
			arrived = Decision_robot_arrive_bordure();

}


	
// Cette décision concerne le ROBOT et non le point fictif...
arrived_e Decision_robot_arrive_arret(void)
{
	if( 	abs(global.real_speed_rotation) < PRECISION_ARRIVE_SPEED_ROTATION
		&&  abs(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION )
	{
		
		COPILOT_reset_absolute_destination();
		return ARRIVED;
	}	
	else
		return NOT_ARRIVED;
}







// Cette décision concerne le ROBOT et non le point fictif...	
arrived_e Decision_robot_arrive_rotation(void)
{
	
	//CONDITION DE CONSIDERATION ROBOT ARRIVE SUR ROTATION = vitesse faible et position proche
	if ((abs(global.real_speed_rotation) < PRECISION_ARRIVE_SPEED_ROTATION/*/diviseur*/) &&
		(CALCULATOR_modulo_angle(abs((global.real_position_rotation >> 10) - PILOT_get_destination_rotation())) < PRECISION_ARRIVE_POSITION_ROTATION  ))
		return ARRIVED;
	else
		return NOT_ARRIVED;
}


// Cette décision concerne le ROBOT et non le point fictif...	
arrived_e Decision_robot_arrive_translation(void)
{
			
	if(current_order.trajectory == TRAJECTORY_ROTATION)
	{
					//Je n'ai pas d'explication pertinente pour justifier cette division par 4...
		if ( (abs(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION/4 ) &&
				(abs(global.real_position_translation-PILOT_get_destination_translation()) < PRECISION_ARRIVE_POSITION_TRANSLATION)  )	//juste pour qu'il ne pense pas qu'il est arrivé tout au début de la trajectoire, au moment où sa vitesse est nulle !!!
			return ARRIVED;
		else
			return NOT_ARRIVED;
	}		
	else
	{
		if ( (abs(global.real_speed_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION ) &&
				(abs(global.real_position_translation-PILOT_get_destination_translation()) < PRECISION_ARRIVE_POSITION_TRANSLATION)  )	//juste pour qu'il ne pense pas qu'il est arrivé tout au début de la trajectoire, au moment où sa vitesse est nulle !!!
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



// Cette décision concerne le point fictif...
braking_e COPILOT_update_brake_state_rotation()
{
	//Si je m'éloigne de l'objectif, je dois accélérer pour m'en rapprocher...
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
	//on mesure l'écart réel entre l'angle de destination et l'angle actuel référentiel IT

	rotation_restante = abs(CALCULATOR_modulo_angle( PILOT_get_destination_rotation() - global.position_rotation));

//	float angle_frein;
	// Calcul de la distance pour freiner
//	angle_frein = (global.vitesse_rotation)*(global.vitesse_rotation >>10); //a cause des vitesses exprimées en <<10...
//	angle_frein /= (2*ACCELERATION_NORMAL*COEFF_ACCELERATION_ROTATION_TRANSLATION);
//
	Sint32 angle_frein;
	angle_frein = (global.vitesse_rotation >> 2)*(global.vitesse_rotation >> 2);
	angle_frein /= (2*PILOT_get_coef(PILOT_ACCELERATION_NORMAL)*PILOT_get_coef(PILOT_ACCELERATION_ROTATION_TRANSLATION));
	angle_frein -= abs(global.vitesse_rotation/2 >> 4);
	angle_frein >>= 6;

	PILOT_set_extra_braking_rotation((rotation_restante - angle_frein<0)?TRUE:FALSE);


	//On BRAKING si la distance qui nous sépare de l'angle final est inférieure à la distance qu'il faut pour freiner
	if (rotation_restante <= angle_frein)
			return BRAKING;
	else
			return NOT_BRAKING;
}

// Cette décision concerne le point fictif...
braking_e COPILOT_update_brake_state_translation(void)
{
	//Si je m'éloigne de l'objectif, je dois accélérer pour m'en rapprocher...
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
	translation_frein = abs(((global.vitesse_translation*global.vitesse_translation)/(PILOT_get_coef(PILOT_ACCELERATION_NORMAL)))/2);
	translation_frein -= abs(global.vitesse_translation/2);

	translation_restante = abs(PILOT_get_destination_translation() - global.position_translation);

	PILOT_set_extra_braking_translation((translation_restante - translation_frein < 0)?TRUE:FALSE);


//	if (global.robot_freine_translation == BRAKING)
//		return BRAKING;

	if (translation_restante <= translation_frein)
		return BRAKING;
	else
		return NOT_BRAKING;
}







/*Fonction test avant le depart*/
//Fonction appelée si la trajectoire demandée est une translation...
//TRANSLATION suppose qu'on veuille rejoindre le point en LIGNE DROITE...
//On s'engage donc à ne pas faire de 'courbe' trop marquée...
trajectory_e COPILOT_decision_rotation_before_translation(Sint16 destination_x, Sint16 destination_y, Sint16 angle_a_parcourir, way_e sens_marche)
{
	//Informations :
				//on ne fait une rotation prï¿½alable qu'aprï¿½s ARRET du robot (vitesses nulles...)
				//il faut donc lancer un type de trajectoire ARRET avant de relancer la rotation
				//donc, on rï¿½ï¿½crit la traj actuelle, et on ne fait la rotation que SI nos vitesses sont nulles
				//si ma vitesse en translation est trop grande, je marque un arret...
	Sint32 d;
	d = abs((Sint32)(CALCULATOR_viewing_algebric_distance (global.position.x, global.position.y, destination_x, destination_y, angle_a_parcourir))<<12);
		

	if (global.vitesse_rotation > PRECISION_ARRIVE_SPEED_ROTATION )
	{
	//	debug_printf("arret préalable\n");
		return TRAJECTORY_STOP;	//Si notre vitesse en rotation est non nulle, il est difficile de savoir si on réussiera à parcourir la courbe
	}	

	//Si on est dï¿½jï¿½ sur le point demandï¿½ >>> aucune trajectoire..... on fait donc la trajectoire "AUCUNE", qui arrivera directement.
	if(d < PRECISION_ARRIVE_POSITION_TRANSLATION && abs(global.vitesse_translation) < PRECISION_ARRIVE_SPEED_TRANSLATION)
	{
	//	debug_printf("Point trop proche\n");
		return TRAJECTORY_NONE;
	}
	if(abs(angle_a_parcourir) > ANGLE_MAXI_LANCEMENT_TRANSLATION)
	{
		//larotation prï¿½alable doit dï¿½marrer par un arrï¿½t si la vitesse est non nulle !
		//sinon, on voudra tourner sur le point actuel, alors qu'on est encore en train d'avancer !
		if(abs(global.vitesse_translation) > PRECISION_ARRIVE_SPEED_TRANSLATION)
			return TRAJECTORY_STOP;
		else	//cool, c'est parti pour une rotation prï¿½alable !
			return TRAJECTORY_ROTATION;	
	}
	
	//Je n'autorise la translation que si l'angle à parcourir est très faible...
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


		//Si l'angle a parcourir est plus grand que PI/2... on aurait mieux fait de choisir la marche arrière !
		//l'angle a parcourir sera recalculé ensuite...
		if(abs(*angle_a_parcourir) > PI4096/2)
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



