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
#include "QS/QS_who_am_i.h"
#include "detection_choc.h"
#include "encoders.h"


// Macros
#define RATIO_ODO_ACC_TRANSLATION_BIG 80
#define NB_POINTS 10
#define BANDWIDTH  400
#define OFFSET_X  2
#define OFFSET_Z  0

// Variables Globales
static Sint8 acc_rotation[NB_POINTS];
static Sint8 acc_rotation_tab[4];
static Uint8 acc_rotation_index = 0;
static Sint32 odo_rotation[NB_POINTS];
static Uint8 acc_rotation_index_tab = 0;
static Uint8 odo_rotation_index = 0;


static Sint8 acc_translation[NB_POINTS];
static Sint8 acc_translation_tab[4];
static Uint8 acc_translation_index = 0;
static Sint32 odo_translation[NB_POINTS];
static Uint8 acc_translation_index_tab = 0;
static Uint8 odo_translation_index = 0;
static Sint32 current_speed_translation = 0;
static Sint32 last_speed_translation = 0;



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
	Sint32 avg_odo_rotation = 0;
	Sint16 avg_acc_translation = 0;
	Sint32 avg_odo_translation = 0;
	Uint8 i;

	for(i=0; i<NB_POINTS; i++){
		avg_acc_rotation += acc_rotation[i];
		avg_odo_rotation += odo_rotation[i];
		avg_acc_translation += acc_translation[i];
		avg_odo_translation += odo_translation[i];
	}

	//Calcul des moyennes des accélérations
	if(NB_POINTS){  //inutile mais safe...
		avg_acc_rotation = avg_acc_rotation/NB_POINTS;
		avg_odo_rotation = avg_odo_rotation/NB_POINTS;
		avg_acc_translation = RATIO_ODO_ACC_TRANSLATION_BIG*avg_acc_translation/NB_POINTS;
		avg_odo_translation = avg_odo_translation/NB_POINTS;
	}
	//debug_printf("%ld\n",/* avg_acc_translation, avg_odo_translation,*/ global.real_speed_translation_for_accelero);

	//debug_printf("%d                %ld                   %ld\n", avg_acc_translation, avg_odo_translation, global.real_speed_translation_for_accelero);
	//debug_printf("%d                %ld                   %ld\n", avg_acc_translation, avg_odo_translation, global.vitesse_translation);
	//debug_printf("%d                 %d\n", global.acceleration_translation, global.vitesse_translation);

	//on regarde si il y a un choc
	if(avg_acc_rotation > avg_odo_rotation + BANDWIDTH/2  || avg_acc_rotation < avg_odo_rotation - BANDWIDTH/2){
		//debug_printf("Choc détecté en rotation %d not beetween %ld and %ld\n", avg_acc_rotation, avg_odo_rotation - BANDWIDTH/2,avg_odo_rotation + BANDWIDTH/2 );
	}
	if(avg_acc_translation > avg_odo_translation + BANDWIDTH/2  || avg_acc_translation < avg_odo_translation - BANDWIDTH/2){
		//debug_printf("Choc détecté %d not beetween %ld and %ld\n", avg_acc_translation, avg_odo_translation - BANDWIDTH/2,avg_odo_translation + BANDWIDTH/2 );
	}
}


void DETECTION_CHOC_acc_rotation_get_value(){
	acc_rotation_tab[acc_rotation_index_tab] = ACC_getZ() - OFFSET_Z;  //C'est bien Z
	acc_rotation_index_tab = (acc_rotation_index_tab + 1)%4;
	if(acc_rotation_index_tab == 3){
		Uint8 i;
		Sint32 sum = 0;
		for(i=0; i<4; i++){
			sum += acc_rotation_tab[i];
		}
		acc_rotation[acc_rotation_index] = sum/4;
		acc_rotation_index = (acc_rotation_index + 1)%NB_POINTS;
	}
}

void DETECTION_CHOC_acc_translation_get_value(){
	acc_translation_tab[acc_translation_index_tab] = ACC_getX() - OFFSET_X;//C'est bien X
	acc_translation_index_tab = (acc_translation_index_tab + 1)%4;
	if(acc_translation_index_tab == 3){
		Uint8 i;
		Sint32 sum = 0;
		for(i=0; i<4; i++){
			sum += acc_translation_tab[i];
		}
	acc_translation[acc_translation_index] = sum/4;
	acc_translation_index = (acc_translation_index + 1)%NB_POINTS;
	}
}

void DETECTION_CHOC_odo_rotation_get_value(){
	acc_rotation[odo_rotation_index] = global.acceleration_rotation;  //calcul de l'accélération
	acc_rotation_index = (odo_rotation_index + 1)%NB_POINTS;
}


void DETECTION_CHOC_odo_translation_get_value(){
	current_speed_translation = global.vitesse_translation;
	odo_translation[odo_translation_index] = (current_speed_translation - last_speed_translation);
	last_speed_translation = current_speed_translation;
	odo_translation_index = (odo_translation_index + 1)%NB_POINTS;
}

void DETECTION_CHOC_process_it_tim2(){
	ACC_read();
	DETECTION_CHOC_acc_rotation_get_value();
	DETECTION_CHOC_acc_translation_get_value();
}


void DETECTION_CHOC_process_it_tim5(){
	DETECTION_CHOC_odo_rotation_get_value();
	DETECTION_CHOC_odo_translation_get_value();
	DETECTION_CHOC_detect_choc();
}
