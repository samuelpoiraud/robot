/*
 *	Club Robot ESEO 2015 - 2016
 *	Black & Pearl
 *
 *	Fichier : detection_choc.c
 *	Package : Carte Stratégie
 *	Description : Detection de choc avec un robot adverse entrainant une perte de notre position
 *	Auteur :
 *	Version 2015/10/14
 */

#include "QS/QS_all.h"
#include "QS/QS_outputlog.h"
#include "detection_choc.h"

// Macros
#define NB_POINTS 5
#define BANDWIDTH  60

// Variables Globales
static Sint8 acc_rotation[5];
static Uint8 acc_rotation_index = 0;
static Sint8 odo_rotation[5];
static Uint8 odo_rotation_index = 0;
static Sint32 last_odo_speed_rotation = 0;

/*
static Sint8 acc_translation[5];
static Uint8 acc_translation_index = 0;
static Sint8 odo_translation[5];
static Uint8 odo_translation_index = 0;
*/



void DETECTION_CHOC_init(){
	static bool_e initialized = FALSE;

	if(initialized)
		return;

	//Initialisation de l'accéléromètre
	ACC_init();

	initialized = TRUE;
}

//Fonction qui regarde si il y a eu un choc
void DETECTION_CHOC_detect_choc(){
	Sint16 avg_acc_rotation = 0;
	Sint16 avg_odo_rotation = 0;
	Uint8 i;

	for(i=0; i<NB_POINTS; i++){
		avg_acc_rotation += acc_rotation[i];
		avg_odo_rotation += odo_rotation[i];
	}

	//Calcul des moyennes des accélérations
	if(NB_POINTS){  //inutile mais safe...
		avg_acc_rotation = avg_acc_rotation/NB_POINTS;
		avg_odo_rotation =avg_odo_rotation/NB_POINTS;
	}

	debug_printf("Average rotation values are accelero = %d and odometry = %d\n", avg_acc_rotation, avg_odo_rotation);

	//on regarde si il y a un choc
	if(avg_acc_rotation > avg_odo_rotation + BANDWIDTH/2  || avg_acc_rotation < avg_odo_rotation - BANDWIDTH/2){
		debug_printf("Choc détecté %d not beetween %d and %d\n", avg_acc_rotation, avg_odo_rotation - BANDWIDTH/2,avg_odo_rotation + BANDWIDTH/2 );
	}
}


void DETECTION_CHOC_acc_rotation_get_value(){
	acc_rotation[acc_rotation_index] = ACC_getX();
	acc_rotation_index = (acc_rotation_index + 1)%NB_POINTS;
}

void DETECTION_CHOC_odo_rotation_get_value(){
	Sint8 current_speed_rotation = global.real_speed_rotation;
	acc_rotation[odo_rotation_index] = (current_speed_rotation - last_odo_speed_rotation)>>24;  //calcul de l'accélération
	last_odo_speed_rotation = current_speed_rotation;
	acc_rotation_index = (odo_rotation_index + 1)%NB_POINTS;
}


void DETECTION_CHOC_process_it(){
	DETECTION_CHOC_acc_rotation_get_value();
	DETECTION_CHOC_odo_rotation_get_value();
}
















