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
#include "QS/QS_can.h"
#include "copilot.h"
#include "buffer.h"
#include "secretary.h"
#include "pilot.h"
#include "LCDTouch/stm32f4_discovery_lcd.h"

#define WAIT_TIME_DISPLAY_AVOID		50
#define NB_SAMPLE					30
#define SAMPLE_TIME					5		//(en ms)

static Sint16 ecretage_debug_rect(Sint16 val);

adversary_t *adversary; // adversaire détecté stocké dans cette variable pour pouvoir envoyer l'information à la stratégie

typedef struct{
	Uint16 Xleft;
	Uint16 Xright;
	Uint16 Yfront;
	Uint16 Yback;
}offset_avoid_s;

static offset_avoid_s offset_avoid = {0};

void AVOIDANCE_init(){

}

void AVOIDANCE_process_it(){
#ifdef USE_PROP_AVOIDANCE
	order_t current_order = COPILOT_get_current_order();
	volatile order_t *buffer_order;

	if((current_order.trajectory == TRAJECTORY_TRANSLATION || current_order.trajectory == TRAJECTORY_TRANSLATION) &&
		current_order.avoidance != AVOID_DISABLED){

		if(AVOIDANCE_target_safe(current_order.way, FALSE)){
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

				// Puis on avertie la stratégie qu'il y a eu évitement

				SECRETARY_send_foe_detected(adversary->x, adversary->y, FALSE);

			}else if(current_order.avoidance == AVOID_ENABLED_AND_WAIT){

				debug_printf("t : %ld      buffering !\n", global.absolute_time);

				// On met l'ordre actuel dans le buffer
				COPILOT_buffering_order();
				buffer_order = COPILOT_get_buffer_order();

				// On met à jours la variable qui contient le temps du début du wait
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

		}else if(AVOIDANCE_target_safe(buffer_order->way, FALSE) == FALSE){
			debug_printf("t : %ld      free !\n", global.absolute_time);
			debug_printf("Rien sur la trajectoire %dx %dy\n", buffer_order->x, buffer_order->y);
			buffer_order->total_wait_time += global.absolute_time - buffer_order->wait_time_begin;
			ROADMAP_add_simple_order(*buffer_order, TRUE, FALSE, TRUE);
			ROADMAP_launch_next_order();
		}
	}

#endif
}

GEOMETRY_point_t avoid_poly[4];

bool_e AVOIDANCE_target_safe(way_e way, bool_e verbose){
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
	Sint32 avoidance_rectangle_width_y_min;
	Sint32 avoidance_rectangle_width_y_max;

	Uint32 breaking_acceleration;
	Uint32 current_speed;
	Uint32 break_distance;
	Uint32 respect_distance;
	Uint32 slow_distance;

	Sint32 relative_foe_x;
	Sint32 relative_foe_y;

	static time32_t last_time_refresh_avoid_displayed = 0;

	vtrans = global.vitesse_translation;
	teta = global.position.teta;

	COS_SIN_4096_get(teta, &cos, &sin);
	adversaries = DETECTION_get_adversaries(&max_foes); // Récupération des adversaires

	bool_e in_path = FALSE;	//On suppose que pas d'adversaire dans le chemin
	bool_e in_slow_zone = FALSE;

	/*	On définit un "rectangle d'évitement" comme la somme :
	 * 		- du rectangle que le robot va recouvrir s'il décide de freiner maintenant.
	 *  	- du rectangle de "respect" qui nous sépare de l'adversaire lorsqu'on se sera arreté
	 *  On calcule la position relative des robots adverses pour savoir s'ils se trouvent dans ce rectangle
	 */

	/*[mm/4096/5ms/5ms]*/	breaking_acceleration = ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ACCELERATION_NORMAL:BIG_ACCELERATION_NORMAL)*4; // Dans le cas d'un freinage on applique 4 fois notre potentiel d'accélération
	/*[mm/4096/5ms]*/		current_speed = (Uint32)(absolute(vtrans)*1);
	/*[mm]*/				break_distance = SQUARE(current_speed)/(2*breaking_acceleration) >> 12;	//distance que l'on va parcourir si l'on décide de freiner maintenant. (Division par 4096 car on calcule avec des variables /4096)
	/*[mm]*/				respect_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_RESPECT_DIST_MIN:BIG_ROBOT_RESPECT_DIST_MIN;	//Distance à laquelle on souhaite s'arrêter
	/*[mm]*/				slow_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_DIST_MIN_SPEED_SLOW:BIG_ROBOT_DIST_MIN_SPEED_SLOW;	//Distance à laquelle on souhaite ralentir

	avoidance_rectangle_width_y_min = -((FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH))/2 + offset_avoid.Xright);
	avoidance_rectangle_width_y_max = (FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH))/2 + offset_avoid.Xleft;

	if(way == FORWARD || way == ANY_WAY)	//On avance
		avoidance_rectangle_max_x = break_distance + respect_distance + offset_avoid.Yfront;
	else
		avoidance_rectangle_max_x = -100;

	if(way == BACKWARD || way == ANY_WAY)	//On recule
		avoidance_rectangle_min_x = -(break_distance + respect_distance + offset_avoid.Yback);
	else
		avoidance_rectangle_min_x = 100;


		if(global.absolute_time - last_time_refresh_avoid_displayed > WAIT_TIME_DISPLAY_AVOID){

				Sint16 angle[4];
				angle[0] = global.position.teta + atan2(avoidance_rectangle_width_y_max, avoidance_rectangle_max_x)*4096;
				angle[1] = global.position.teta + atan2(avoidance_rectangle_width_y_min, avoidance_rectangle_max_x)*4096;
				angle[2] = global.position.teta + atan2(avoidance_rectangle_width_y_min, avoidance_rectangle_min_x)*4096;
				angle[3] = global.position.teta + atan2(avoidance_rectangle_width_y_max, avoidance_rectangle_min_x)*4096;

				Uint16 longueur[4];
				longueur[0] = GEOMETRY_distance((GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){avoidance_rectangle_width_y_max, avoidance_rectangle_max_x});
				longueur[1] = GEOMETRY_distance((GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){avoidance_rectangle_width_y_min, avoidance_rectangle_max_x});
				longueur[2] = GEOMETRY_distance((GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){avoidance_rectangle_width_y_min, avoidance_rectangle_min_x});
				longueur[3] = GEOMETRY_distance((GEOMETRY_point_t){0, 0}, (GEOMETRY_point_t){avoidance_rectangle_width_y_max, avoidance_rectangle_min_x});

				avoid_poly[0] = (GEOMETRY_point_t){MAX(global.position.x+cos4096(angle[0])*longueur[0], 0), ecretage_debug_rect(global.position.y+sin4096(angle[0])*longueur[0])};
				avoid_poly[1] = (GEOMETRY_point_t){MIN(global.position.x+cos4096(angle[1])*longueur[1], 2000), ecretage_debug_rect(global.position.y+sin4096(angle[1])*longueur[1])};
				avoid_poly[2] = (GEOMETRY_point_t){MIN(global.position.x+cos4096(angle[2])*longueur[2], 2000), ecretage_debug_rect(global.position.y+sin4096(angle[2])*longueur[2])};
				avoid_poly[3] = (GEOMETRY_point_t){MAX(global.position.x+cos4096(angle[3])*longueur[3], 0), ecretage_debug_rect(global.position.y+sin4096(angle[3])*longueur[3])};

				Uint8 nb_point = 4;
				Uint16 max = AROUND_UP((nb_point+1)/3.);
				CAN_msg_t msg;
				msg.sid = DEBUG_AVOIDANCE_POLY;
				msg.size = 8;
				for(i=0;i<max;i++){
					Uint8 num = 0;
					if(i==0)
						msg.data[0] = TRUE;
					else
						msg.data[0] = FALSE;

					if(i*3 < nb_point){
						num++;
						msg.data[2] = (Uint8)(avoid_poly[i*3].x >> 4);
						msg.data[3] = (Uint8)(avoid_poly[i*3].y >> 4);
					}else if(i*3 == nb_point){
						num++;
						msg.data[2] = (Uint8)(avoid_poly[0].x >> 4);
						msg.data[3] = (Uint8)(avoid_poly[0].y >> 4);
					}

					if(i*3+1 < nb_point){
						num++;
						msg.data[4] = (Uint8)(avoid_poly[i*3+1].x >> 4);
						msg.data[5] = (Uint8)(avoid_poly[i*3+1].y >> 4);
					}else if(i*3+1 == nb_point){
						num++;
						msg.data[4] = (Uint8)(avoid_poly[0].x >> 4);
						msg.data[5] = (Uint8)(avoid_poly[0].y >> 4);
					}

					if(i*3+2 < nb_point){
						num++;
						msg.data[6] = (Uint8)(avoid_poly[i*3+2].x >> 4);
						msg.data[7] = (Uint8)(avoid_poly[i*3+2].y >> 4);
					}else if(i*3+2 == nb_point){
						num++;
						msg.data[6] = (Uint8)(avoid_poly[0].x >> 4);
						msg.data[7] = (Uint8)(avoid_poly[0].y >> 4);
					}

					msg.data[1] = num;
					SECRETARY_send_canmsg(&msg);
				}

				last_time_refresh_avoid_displayed = global.absolute_time;
			}

	for(i=0; i<max_foes; i++){

		if(adversaries[i].enable){
			COS_SIN_4096_get(adversaries[i].angle, &cosinus, &sinus);
			relative_foe_x = (((Sint32)(cosinus)) * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]
			relative_foe_y = (((Sint32)(sinus))   * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]

			if(		relative_foe_y > avoidance_rectangle_width_y_min && 	relative_foe_y < avoidance_rectangle_width_y_max
				&& 	relative_foe_x > avoidance_rectangle_min_x 		 && 	relative_foe_x < avoidance_rectangle_max_x)
				{
					in_path = TRUE;	//On est dans le rectangle d'évitement !!!
					adversary = &adversaries[i]; // On sauvegarde l'adversaire nous ayant fait évité
				}
		}
	}

	if(in_path == FALSE && (current_order.trajectory == TRAJECTORY_TRANSLATION || current_order.trajectory == TRAJECTORY_TRANSLATION)){

		avoidance_rectangle_width_y_min = -((FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH))/2 + offset_avoid.Xright + 50);
		avoidance_rectangle_width_y_max = (FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH))/2 + offset_avoid.Xleft + 50;

		if(way == FORWARD || way == ANY_WAY)	//On avance
			avoidance_rectangle_max_x = break_distance + slow_distance + offset_avoid.Yfront;
		else
			avoidance_rectangle_max_x = 0;

		if(way == BACKWARD || way == ANY_WAY)	//On recule
			avoidance_rectangle_min_x = -(break_distance + slow_distance + offset_avoid.Yback);
		else
			avoidance_rectangle_min_x = 0;

		for(i=0; i<max_foes; i++){

			if(adversaries[i].enable){
				COS_SIN_4096_get(adversaries[i].angle, &cosinus, &sinus);
				relative_foe_x = (((Sint32)(cosinus)) * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]
				relative_foe_y = (((Sint32)(sinus))   * adversaries[i].dist) >> 12;		//[rad/4096] * [mm] / 4096 = [mm]

				if(		relative_foe_y > avoidance_rectangle_width_y_min && 	relative_foe_y < avoidance_rectangle_width_y_max
					&& 	relative_foe_x > avoidance_rectangle_min_x 		 && 	relative_foe_x < avoidance_rectangle_max_x)
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


bool_e AVOIDANCE_target_safe_curve(way_e way, bool_e verbose){
	Sint32 vtrans;		//[mm/4096/5ms]
	Sint32 vrot;		//[rad/4096/1024/5ms]
	Sint16 teta;		//[rad/4096]
	Sint16 px;			//[mm]
	Sint16 py;			//[mm]

	adversary_t *adversaries;
	Uint8 max_foes;
	Uint8 i;
	order_t current_order = COPILOT_get_current_order();

	Sint32 avoidance_rectangle_min_x;
	Sint32 avoidance_rectangle_max_x;
	Sint32 avoidance_rectangle_width_y_min;
	Sint32 avoidance_rectangle_width_y_max;

	Uint32 breaking_acceleration;
	Uint32 current_speed;
	Uint32 break_distance;
	Uint32 respect_distance;

	GEOMETRY_point_t pts[NB_SAMPLE];

	static time32_t last_time_refresh_avoid_displayed = 0;

	vrot = global.vitesse_rotation;
	vtrans = global.vitesse_translation/12;
	teta = global.position.teta;

	adversaries = DETECTION_get_adversaries(&max_foes); // Récupération des adversaires

	bool_e in_path = FALSE;	//On suppose que pas d'adversaire dans le chemin


	breaking_acceleration = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_ACCELERATION_NORMAL:BIG_ROBOT_ACCELERATION_NORMAL;
	current_speed = (Uint32)(absolute(vtrans)*1);
	break_distance = SQUARE(current_speed)/(4*breaking_acceleration);	//distance que l'on va parcourir si l'on décide de freiner maintenant.
	respect_distance = (QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_RESPECT_DIST_MIN:BIG_ROBOT_RESPECT_DIST_MIN;	//Distance à laquelle on souhaite s'arrêter

	avoidance_rectangle_width_y_min = -((FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH))/2 + offset_avoid.Xright);
	avoidance_rectangle_width_y_max = (FOE_SIZE + ((QS_WHO_AM_I_get() == SMALL_ROBOT)?SMALL_ROBOT_WIDTH:BIG_ROBOT_WIDTH))/2 + offset_avoid.Xleft;

	if(way == FORWARD || way == ANY_WAY)	//On avance
		avoidance_rectangle_max_x = break_distance + respect_distance + offset_avoid.Yfront;
	else
		avoidance_rectangle_max_x = 0;

	if(way == BACKWARD || way == ANY_WAY)	//On recule
		avoidance_rectangle_min_x = -(break_distance + respect_distance + offset_avoid.Yback);
	else
		avoidance_rectangle_min_x = 0;

	#ifdef MODE_SIMULATION

		if(global.absolute_time - last_time_refresh_avoid_displayed > WAIT_TIME_DISPLAY_AVOID){
				last_time_refresh_avoid_displayed = global.absolute_time;
			}

	#endif

	for(i=0; i<max_foes; i++){

		if(adversaries[i].enable){

		}
	}

	return in_path;
}

void AVOIDANCE_refresh_avoid_poly(){
	Point avoid[5];

	avoid[0] = (Point){avoid_poly[0].y*299/3000, avoid_poly[0].x*199/2000};
	avoid[1] = (Point){avoid_poly[1].y*299/3000, avoid_poly[1].x*199/2000};
	avoid[2] = (Point){avoid_poly[2].y*299/3000, avoid_poly[2].x*199/2000};
	avoid[3] = (Point){avoid_poly[3].y*299/3000, avoid_poly[3].x*199/2000};
	avoid[4] = (Point){avoid_poly[0].y*299/3000, avoid_poly[0].x*199/2000};

	LCD_PolyLine(avoid, 5);
}

// Retourne si un adversaire est dans la chemin entre nous et la position
bool_e AVOIDANCE_foe_in_zone(bool_e verbose, Sint16 x, Sint16 y, bool_e check_on_all_traject){
	bool_e inZone;
	Uint8 i;
	Sint32 a, b, c; // avec a, b et c les coefficients de la droite entre nous et la cible
	Sint32 NCx, NCy, NAx, NAy;

	Sint16 px;			//[mm]
	Sint16 py;			//[mm]


	Uint32 width_distance;
	Uint32 length_distance;

	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		width_distance = BIG_ROBOT_RESPECT_DIST_MIN;
		length_distance = FOE_SIZE + BIG_ROBOT_WIDTH;
	}else{
		width_distance = SMALL_ROBOT_RESPECT_DIST_MIN;
		length_distance = FOE_SIZE + SMALL_ROBOT_WIDTH;
	}

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

			if(absolute((Sint32)a*adversaries[i].x + (Sint32)b*adversaries[i].y + c) / (Sint32)sqrt((Sint32)a*a + (Sint32)b*b) < length_distance){
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
							(!check_on_all_traject &&(NCx*NAx + NCy*NAy) < (Sint32)dist_point_to_point(px, py, x, y)*width_distance)
							||
							(check_on_all_traject &&(NCx*NAx + NCy*NAy) < SQUARE((Sint32)dist_point_to_point(px, py, x, y))))){

					adversary = &adversaries[i]; // On sauvegarde l'adversaire nous ayant fait évité
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

void AVOIDANCE_process_CAN_msg(CAN_msg_t *msg){
#ifdef USE_ACT_AVOID
	offset_avoid.Xleft = U16FROMU8(msg->data[0], msg->data[1]);
	offset_avoid.Xright = U16FROMU8(msg->data[2], msg->data[3]);
	offset_avoid.Yfront = U16FROMU8(msg->data[4], msg->data[5]);
	offset_avoid.Yback = U16FROMU8(msg->data[6], msg->data[7]);
#endif
}

bool_e AVOIDANCE_foe_near(){
	Uint8 i;
	adversary_t *adversaries;
	Uint8 max_foes;
	bool_e foe_near = FALSE;
	adversaries = DETECTION_get_adversaries(&max_foes); // Récupération des adversaires

	for(i=0; i<max_foes; i++){
		if(adversaries[i].enable && adversaries[i].dist <= DISTANCE_EVITEMENT_ROTATION)
				foe_near = TRUE;
	}
	return foe_near;
}

static Sint16 ecretage_debug_rect(Sint16 val){
	if(val < 0)
		return 0;
	else if(val > 2999)
		return 2999;
	else
		return val;
}
