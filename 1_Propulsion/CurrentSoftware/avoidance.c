/*
*  Club Robot ESEO 2014
*
*  Fichier : avoidance.c
*  Package : Propulsion
*  Description : Analyse la trajectoire que va emprunter le robot par rapport au robot adversaire, afin de savoir si le chemin est réalisable
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

void AVOIDANCE_init(){

}

void AVOIDANCE_process_it(){
#ifdef USE_PROP_AVOIDANCE
	order_t current_order = COPILOT_get_current_order();

	if(
		((current_order.trajectory == TRAJECTORY_TRANSLATION || current_order.trajectory == TRAJECTORY_TRANSLATION) &&
		(current_order.avoidance == AVOID_ENABLED || current_order.avoidance == AVOID_ENABLED_AND_WAIT)
		) ||
		 current_order.trajectory == WAIT_FOREVER){

	}
#endif
}

bool_e AVOIDANCE_target_safe(Sint32 destx, Sint32 desty, bool_e verbose){
	Sint32 vrot;		//[rad/4096/1024/5ms]
	Sint32 vtrans;		//[mm/4096/5ms]

	Sint32 px;			//[mm/4096]
	Sint32 py;			//[mm/4096]
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

	Sint32 relative_foe_x;
	Sint32 relative_foe_y;



	TIMER1_disableInt(); // Inibation des ITs critique

	vrot = global.vitesse_rotation;
	vtrans = global.vitesse_translation;
	px = global.position.x;
	py = global.position.y;
	teta = global.position.teta;

	TIMER1_enableInt(); // Dé-inibation des ITs critique

	COS_SIN_4096_get(teta, &cos, &sin);
	adversaries = DETECTION_get_adversaries(&max_foes); // Récupération des adversaires

	bool_e in_path = FALSE;	//On suppose que pas d'adversaire dans le chemin

	/*	On définit un "rectangle d'évitement" comme la somme :
	 * 		- du rectangle que le robot va recouvrir s'il décide de freiner maintenant.
	 *  	- du rectangle de "respect" qui nous sépare de l'adversaire lorsqu'on se sera arreté
	 *  On calcule la position relative des robots adverses pour savoir s'ils se trouvent dans ce rectangle
	 *
	 */


	way_e move_way = current_order.way;

	breaking_acceleration = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_ACCELERATION_NORMAL:BIG_ROBOT_ACCELERATION_NORMAL;
	current_speed = (Uint32)(absolute(vtrans)*1);
	break_distance = current_speed*current_speed/(2*breaking_acceleration);	//distance que l'on va parcourir si l'on décide de freiner maintenant.
	respect_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_RESPECT_DIST_MIN:BIG_ROBOT_RESPECT_DIST_MIN;	//Distance à laquelle on souhaite s'arrêter

	avoidance_rectangle_width_y = FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH);
	//avoidance_printf("\n%d[%ld>%ld][%ld>%ld]\n", current_speed,avoidance_rectangle_min_x,avoidance_rectangle_max_x,-avoidance_rectangle_width_y/2,avoidance_rectangle_width_y/2);


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

			if(		relative_foe_y > -avoidance_rectangle_width_y/2 	&& 	relative_foe_y < avoidance_rectangle_width_y/2
				&& 	relative_foe_x > avoidance_rectangle_min_x 		&& 	relative_foe_x < avoidance_rectangle_max_x)
				{
					in_path = TRUE;	//On est dans le rectangle d'évitement !!!
					//if(verbose)
						//SD_printf("FOE[%d]_IN_PATH|%c|d:%d|a:%d|rel_x%ld|rel_y%ld   RECT:[%ld>%ld][%ld>%ld]\n", i, /*(adversaries[i].from == DETECTION_FROM_BEACON_IR)?'B':*/'H',adversaries[i].dist,adversaries[i].angle, relative_foe_x, relative_foe_y,avoidance_rectangle_min_x,avoidance_rectangle_max_x,-avoidance_rectangle_width_y/2,avoidance_rectangle_width_y/2);
				}
		}
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
	adversaries = DETECTION_get_adversaries(&max_foes); // Récupération des adversaires

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
			if(verbose)
				debug_printf("Nous x:%d y:%d / ad x:%d y:%d / destination x:%d y:%d /\n ", px, py, adversaries[i].x, adversaries[i].y,x,y);

			if((QS_WHO_AM_I_get() == BIG_ROBOT && absolute((Sint32)a*adversaries[i].x + (Sint32)b*adversaries[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < MARGE_COULOIR_EVITEMENT_STATIC_BIG_ROBOT)
					|| (QS_WHO_AM_I_get() == SMALL_ROBOT && absolute((Sint32)a*adversaries[i].x + (Sint32)b*adversaries[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < MARGE_COULOIR_EVITEMENT_STATIC_SMALL_ROBOT)){
				// /NC./NA ¤ [0,NC*d]
				// /NC : Vecteur entre nous et le point cible
				// /NA : Vecteur entre nous et l'adversaire
				// NC : Distance entre nous et le point cible
				// d : Distance d'évitement de l'adversaire (longueur couloir)

				NCx = x - px;
				NCy = y - py;

				NAx = adversaries[i].x - px;
				NAy = adversaries[i].y - py;


				if((NCx*NAx + NCy*NAy) >= (Sint32)dist_point_to_point(px, py, x, y)*100
						&& (
							(!check_on_all_traject &&(NCx*NAx + NCy*NAy) < (Sint32)dist_point_to_point(px, py, x, y)*DISTANCE_EVITEMENT_STATIC)
							||
							(check_on_all_traject &&(NCx*NAx + NCy*NAy) < SQUARE((Sint32)dist_point_to_point(px, py, x, y))))){
					inZone = TRUE;
					if(verbose)
						debug_printf("DETECTED\n");
				}else{
					if(verbose)
						debug_printf("NO\n");
				}
			}else{
				if(verbose)
					debug_printf("NO\n");
			}
		}
	}
	return inZone;
}
