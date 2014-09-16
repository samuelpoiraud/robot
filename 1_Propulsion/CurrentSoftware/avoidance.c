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

void AVOIDANCE_init(){

}

bool_e AVOIDANCE_target_safe(Sint32 destx, Sint32 desty){
	Sint32 vrot;		//[rad/4096/1024/5ms]
	Sint32 vtrans;		//[mm/4096/5ms]

	Sint32 px;			//[mm/4096]
	Sint32 py;			//[mm/4096]
	Sint16 teta;		//[rad/4096]

	Sint16 sin, cos;	//[/4096]

	adversary_t *adversaries;
	Uint8 max_foes;
	Uint8 i;


	TIMER1_disableInt(); // Inibation des ITs critique

	vrot = global.vitesse_rotation;
	vtrans = global.vitesse_translation;
	px = global.position.x;
	py = global.position.y;
	teta = global.position.teta;

	TIMER1_enableInt(); // Dé-inibation des ITs critique

	COS_SIN_4096_get(teta, &cos, &sin);
	adversaries = DETECTION_get_adversaries(&max_foes); // Récupération des adversaires

	for(i=0; i<max_foes; i++){

		if(adversaries[i].enable){

		}
	}

	return FALSE;
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
