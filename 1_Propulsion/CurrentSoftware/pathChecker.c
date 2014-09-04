/*
*  Club Robot ESEO 2014
*
*  Fichier : pathChecker.c
*  Package : Propulsion
*  Description : Analyse la trajectoire que va emprunter le robot par rapport au robot adversaire, afin de savoir si le chemin est réalisable
*  Auteur : Arnaud
*  Version 201203
*/


#include "pathChecker.h"
#include "detection.h"
#include "QS/QS_macro.h"
#include "QS/QS_timer.h"
#include "cos_sin.h"

void PATHCHECKER_init(){

}

bool_e PATHCHECKER_target_safe(Sint32 destx, Sint32 desty){
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
