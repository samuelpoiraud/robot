/*
*  Club Robot ESEO 2014
*
*  Fichier : avoidance.c
*  Package : Propulsion
*  Description : Analyse la trajectoire que va emprunter le robot par rapport au robot adversaire, afin de savoir si le chemin est r�alisable
*  Auteur : Arnaud
*  Version 201203
*/


#include "avoidance.h"
#include "detection.h"
#include "QS/QS_maths.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_maths.h"
#include "copilot.h"
#include "buffer.h"
#include "secretary.h"
#include "pilot.h"

adversary_t *adversary; // adversaire d�tect� stock� dans cette variable pour pouvoir envoyer l'information � la strat�gie

void AVOIDANCE_init(){

}

void AVOIDANCE_process_it(){
#ifdef USE_PROP_AVOIDANCE
	order_t current_order = COPILOT_get_current_order();
	volatile order_t *buffer_order;

	if((current_order.trajectory == TRAJECTORY_TRANSLATION || current_order.trajectory == TRAJECTORY_TRANSLATION) &&
		current_order.avoidance != AVOID_DISABLED){

		if(AVOIDANCE_target_safe(current_order.x, current_order.y, FALSE)){
			if(current_order.avoidance == AVOID_ENABLED){

				// Si on rencontre un adversaire on s'arrete
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
									NO_ACKNOWLEDGE,
									CORRECTOR_ENABLE,
									AVOID_DISABLED
								);

				// Puis on avertie la strat�gie qu'il y a eu �vitement

				SECRETARY_send_foe_detected(adversary->x, adversary->y, FALSE);

			}else if(current_order.avoidance == AVOID_ENABLED_AND_WAIT){

				debug_printf("t : %ld      buffering !\n", global.absolute_time);

				// On met l'ordre actuel dans le buffer
				COPILOT_buffering_order();
				buffer_order = COPILOT_get_buffer_order();

				// On met � jours la variable qui contient le temps du d�but du wait
				buffer_order->wait_time_begin = global.absolute_time;

				order_t supp;
					supp.x = 0;
					supp.y = 0;
					supp.teta = 0;
					supp.relative = NOT_RELATIVE;
					supp.way = ANY_WAY;
					supp.border_mode = NOT_BORDER_MODE;
					supp.multipoint = NO_MULTIPOINT;
					supp.speed = FAST;
					supp.acknowledge = NO_ACKNOWLEDGE;
					supp.corrector = CORRECTOR_ENABLE;
					supp.avoidance = AVOID_DISABLED;
					supp.total_wait_time = 0;
					supp.trajectory = WAIT_FOREVER;
				BUFFER_add_begin(&supp);

				supp.trajectory = TRAJECTORY_STOP;
				BUFFER_add_begin(&supp);

				ROADMAP_launch_next_order();
			}
		}
	}else if(current_order.trajectory == WAIT_FOREVER){

		buffer_order = COPILOT_get_buffer_order();

		// Si il y a timeout
		if(buffer_order->total_wait_time + global.absolute_time - buffer_order->wait_time_begin > 3000){

			debug_printf("t : %ld      timeout !\n", global.absolute_time);

			// On remplace la trajectoire courante
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
								NO_ACKNOWLEDGE,
								CORRECTOR_ENABLE,
								AVOID_DISABLED
							);

			SECRETARY_send_foe_detected(adversary->x, adversary->y, TRUE);

		}else if(AVOIDANCE_foe_in_zone(FALSE, buffer_order->x, buffer_order->y, FALSE) == FALSE){
			debug_printf("t : %ld      free !\n", global.absolute_time);
			debug_printf("Rien sur la trajectoire %dx %dy\n", buffer_order->x, buffer_order->y);
			buffer_order->total_wait_time += global.absolute_time - buffer_order->wait_time_begin;
			ROADMAP_add_simple_order(*buffer_order, TRUE, FALSE, TRUE);
			ROADMAP_launch_next_order();
		}
	}

#endif
}

bool_e AVOIDANCE_target_safe(Sint32 destx, Sint32 desty, bool_e verbose){
	Sint32 vrot;		//[rad/4096/1024/5ms]
	Sint32 vtrans;		//[mm/4096/5ms]

	Sint16 teta;		//[rad/4096]

	Sint16 sin, cos;	//[/4096]
	Sint16 sinus, cosinus;	//[/4096]

	adversary_t *adversaries;
	Uint8 max_foes;
	Uint8 i;
	order_t current_order = COPILOT_get_current_order();

	Sint32 avoidance_rectangle_min_x;
	Sint32 avoidance_rectangle_max_x;
	Sint32 avoidance_rectangle_width_y;

	Uint32 breaking_acceleration;
	Uint32 current_speed;
	Uint32 break_distance;
	Uint32 respect_distance;
	Uint32 slow_distance;

	Sint32 relative_foe_x;
	Sint32 relative_foe_y;

	vrot = global.vitesse_rotation;
	vtrans = global.vitesse_translation/12; // Pour avoir la vitesse de translation en mm/s comme en strat�gie
	teta = global.position.teta;

	COS_SIN_4096_get(teta, &cos, &sin);
	adversaries = DETECTION_get_adversaries(&max_foes); // R�cup�ration des adversaires

	bool_e in_path = FALSE;	//On suppose que pas d'adversaire dans le chemin
	bool_e in_slow_zone = FALSE;

	/*	On d�finit un "rectangle d'�vitement" comme la somme :
	 * 		- du rectangle que le robot va recouvrir s'il d�cide de freiner maintenant.
	 *  	- du rectangle de "respect" qui nous s�pare de l'adversaire lorsqu'on se sera arret�
	 *  On calcule la position relative des robots adverses pour savoir s'ils se trouvent dans ce rectangle
	 */

	way_e move_way = current_order.way;

	breaking_acceleration = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_ACCELERATION_NORMAL:BIG_ROBOT_ACCELERATION_NORMAL;
	current_speed = (Uint32)(absolute(vtrans)*1);
	break_distance = SQUARE(current_speed)/(2*breaking_acceleration);	//distance que l'on va parcourir si l'on d�cide de freiner maintenant.
	respect_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_RESPECT_DIST_MIN:BIG_ROBOT_RESPECT_DIST_MIN;	//Distance � laquelle on souhaite s'arr�ter
	slow_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_DIST_MIN_SPEED_SLOW:BIG_ROBOT_DIST_MIN_SPEED_SLOW;	//Distance � laquelle on souhaite ralentir

	avoidance_rectangle_width_y = FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH);

	if(move_way == FORWARD || move_way == ANY_WAY)	//On avance
		avoidance_rectangle_max_x = break_distance + respect_distance;
	else
		avoidance_rectangle_max_x = 0;

	if(move_way == BACKWARD || move_way == ANY_WAY)	//On recule
		avoidance_rectangle_min_x = -(break_distance + respect_distance);
	else
		avoidance_rectangle_min_x = 0;

	for(i=0; i<max_foes; i++){

		if(adversaries[i].enable){
			COS_SIN_4096_get(adversaries[i].angle, &cosinus, &sinus);
			relative_foe_x = (((Sint32)(cosinus)) * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]
			relative_foe_y = (((Sint32)(sinus))   * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]

			if(		relative_foe_y > -avoidance_rectangle_width_y/2 && 	relative_foe_y < avoidance_rectangle_width_y/2
				&& 	relative_foe_x > avoidance_rectangle_min_x 		&& 	relative_foe_x < avoidance_rectangle_max_x)
				{
					in_path = TRUE;	//On est dans le rectangle d'�vitement !!!
					adversary = &adversaries[i]; // On sauvegarde l'adversaire nous ayant fait �vit�
				}
		}
	}

	if(in_path == FALSE){

		avoidance_rectangle_width_y = FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH) + 50;

		if(move_way == FORWARD || move_way == ANY_WAY)	//On avance
			avoidance_rectangle_max_x = break_distance + slow_distance;
		else
			avoidance_rectangle_max_x = 0;

		if(move_way == BACKWARD || move_way == ANY_WAY)	//On recule
			avoidance_rectangle_min_x = -(break_distance + slow_distance);
		else
			avoidance_rectangle_min_x = 0;

		for(i=0; i<max_foes; i++){

			if(adversaries[i].enable){
				COS_SIN_4096_get(adversaries[i].angle, &cosinus, &sinus);
				relative_foe_x = (((Sint32)(cosinus)) * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]
				relative_foe_y = (((Sint32)(sinus))   * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]

				if(		relative_foe_y > -avoidance_rectangle_width_y/2 && 	relative_foe_y < avoidance_rectangle_width_y/2
					&& 	relative_foe_x > avoidance_rectangle_min_x 		&& 	relative_foe_x < avoidance_rectangle_max_x)
					{
						PILOT_set_speed(SLOW_TRANSLATION_AND_FAST_ROTATION);
						in_slow_zone = TRUE;
					}
			}
		}
		if(in_slow_zone == FALSE)
			PILOT_set_speed(current_order.speed);
	}

	return in_path;
}

// Retourne si un adversaire est dans la chemin entre nous et la position
bool_e AVOIDANCE_foe_in_zone(bool_e verbose, Sint16 x, Sint16 y, bool_e check_on_all_traject){
	bool_e inZone;
	Uint8 i;
	Sint32 a, b, c; // avec a, b et c les coefficients de la droite entre nous et la cible
	Sint32 NCx, NCy, NAx, NAy;

	Sint16 px;			//[mm]
	Sint16 py;			//[mm]

	adversary_t *adversaries;
	Uint8 max_foes;
	adversaries = DETECTION_get_adversaries(&max_foes); // R�cup�ration des adversaires

	px = global.position.x;
	py = global.position.y;

	a = y - py;
	b = -(x - px);
	c = -(Sint32)px*y + (Sint32)py*x;

	if(px == x && py == y)
		return FALSE;


	inZone = FALSE;	//On suppose que pas d'adversaire dans le chemin

	for (i=0; i<max_foes; i++)
	{
		if (adversaries[i].enable){
			// d(D, A) < L
			// D : droite que le robot empreinte pour aller au point
			// A : Point adversaire
			// L : Largeur du robot maximum * 2

			if((QS_WHO_AM_I_get() == BIG_ROBOT && absolute((Sint32)a*adversaries[i].x + (Sint32)b*adversaries[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < MARGE_COULOIR_EVITEMENT_STATIC_BIG_ROBOT)
					|| (QS_WHO_AM_I_get() == SMALL_ROBOT && absolute((Sint32)a*adversaries[i].x + (Sint32)b*adversaries[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < MARGE_COULOIR_EVITEMENT_STATIC_SMALL_ROBOT)){
				// /NC./NA � [0,NC*d]
				// /NC : Vecteur entre nous et le point cible
				// /NA : Vecteur entre nous et l'adversaire
				// NC : Distance entre nous et le point cible
				// d : Distance d'�vitement de l'adversaire (longueur couloir)

				NCx = x - px;
				NCy = y - py;

				NAx = adversaries[i].x - px;
				NAy = adversaries[i].y - py;


				if((NCx*NAx + NCy*NAy) >= (Sint32)dist_point_to_point(px, py, x, y)*100
						&& (
							(!check_on_all_traject &&(NCx*NAx + NCy*NAy) < (Sint32)dist_point_to_point(px, py, x, y)*DISTANCE_EVITEMENT_STATIC)
							||
							(check_on_all_traject &&(NCx*NAx + NCy*NAy) < SQUARE((Sint32)dist_point_to_point(px, py, x, y))))){

					adversary = &adversaries[i]; // On sauvegarde l'adversaire nous ayant fait �vit�
					inZone = TRUE;
				}
			}
		}
	}
	return inZone;
}

void AVOIDANCE_said_foe_detected(){
	SECRETARY_send_foe_detected(adversary->x, adversary->y, FALSE);
}
