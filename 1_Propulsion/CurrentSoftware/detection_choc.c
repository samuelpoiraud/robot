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

// Variables Globales pour la rotation
volatile Sint8 acc_rotation[NB_POINTS];
Uint8 acc_rotation_index = 0;
Sint8 acc_rotation_tab[4];
Uint8 acc_rotation_index_tab = 0;
volatile Sint32 odo_rotation[NB_POINTS];
Uint8 odo_rotation_index = 0;

//Variables globales pour la translation
volatile Sint8 acc_translation[NB_POINTS];
Uint8 acc_translation_index = 0;
Sint8 acc_translation_tab[4];
Uint8 acc_translation_index_tab = 0;
volatile Sint32 odo_translation[NB_POINTS];
Uint8 odo_translation_index = 0;

Sint32 current_speed_translation = 0;
Sint32 last_speed_translation = 0;


//Variables globales pour l'affichage
volatile Sint32 save_acc_rot = 1;
volatile Sint32 save_odo_rot = 1;
volatile Sint32 save_acc_trans = 1;
volatile Sint32 save_odo_trans = 1;
volatile bool_e flag_update_save = FALSE;
volatile bool_e flag_recouvrement =  FALSE;
time_t prev_time = 0;


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
	Sint32 avg_acc_rotation = 0;
	Sint32 avg_odo_rotation = 0;
	Sint32 avg_acc_translation = 0;
	Sint32 avg_odo_translation = 0;
	Uint8 i;

	for(i=0; i<NB_POINTS; i++){
		avg_acc_rotation += acc_rotation[i];
		avg_odo_rotation += odo_rotation[i];
		avg_acc_translation += acc_translation[i];
		avg_odo_translation += odo_translation[i];
	}
	//debug_printf("sum = %ld\n", avg_acc_rotation);

	//Calcul des moyennes des accélérations
	if(NB_POINTS){  //inutile mais safe...
		avg_acc_rotation = avg_acc_rotation/NB_POINTS;
		avg_odo_rotation = avg_odo_rotation/NB_POINTS;
		avg_acc_translation = 65*avg_acc_translation/NB_POINTS;
		avg_odo_translation = avg_odo_translation/NB_POINTS;
	}
	//debug_printf("avg = %ld\n", avg_odo_rotation);
	save_acc_rot = avg_acc_rotation;
	save_odo_rot = avg_odo_rotation;
	save_acc_trans = avg_acc_translation;
	save_odo_trans = avg_odo_translation;
	if(flag_update_save)
		flag_recouvrement = TRUE;
	flag_update_save = TRUE;

	//on regarde si il y a un choc
	/*if(avg_acc_rotation > avg_odo_rotation + BANDWIDTH/2  || avg_acc_rotation < avg_odo_rotation - BANDWIDTH/2){
		//debug_printf("Choc détecté en rotation %d not beetween %ld and %ld\n", avg_acc_rotation, avg_odo_rotation - BANDWIDTH/2,avg_odo_rotation + BANDWIDTH/2 );
	}
	if(avg_acc_translation > avg_odo_translation + BANDWIDTH/2  || avg_acc_translation < avg_odo_translation - BANDWIDTH/2){
		//debug_printf("Choc détecté %d not beetween %ld and %ld\n", avg_acc_translation, avg_odo_translation - BANDWIDTH/2,avg_odo_translation + BANDWIDTH/2 );
	}*/
}


void DETECTION_CHOC_acc_rotation_get_value(){
	acc_rotation_tab[acc_rotation_index_tab] = ACC_getZ();  //C'est bien Z
	//debug_printf("ACC_Z %d\n", acc_rotation_tab[acc_rotation_index_tab]);
	acc_rotation_index_tab = (acc_rotation_index_tab + 1)%4;
	//debug_printf("index_tab_4 %d\n", acc_rotation_index_tab);
	if(acc_rotation_index_tab == 3){
		Uint8 i;
		Sint32 sum = 0;
		for(i=0; i<4; i++){
			sum += acc_rotation_tab[i];
		}
		//acc_rotation[acc_rotation_index] = sum/4;
		//debug_printf("acc_rot %d\n", acc_rotation[acc_rotation_index]);
		acc_rotation_index = acc_rotation_index + 1;
		//debug_printf("acc_rot_index %d\n", acc_rotation_index);
	}
}

void DETECTION_CHOC_acc_translation_get_value(){
	acc_translation_tab[acc_translation_index_tab] = ACC_getX();//C'est bien X
	acc_translation_index_tab = (acc_translation_index_tab + 1)%4;
	if(acc_translation_index_tab == 3){
		Uint8 i;
		Sint32 sum = 0;
		for(i=0; i<4; i++){
			sum += acc_translation_tab[i];
		}
	acc_translation[acc_translation_index] = sum/4;
	acc_translation_index = (acc_translation_index + 1)%NB_POINTS;
	//debug_printf("acc_translation_index %d\n", acc_translation_index);
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

void DETECTION_CHOC_process_main(){
	if(flag_update_save){
		prev_time = global.absolute_time;
		flag_update_save = FALSE;
		//1er argument acc_trans
		//2eme argument odo_trans
		//3eme argument acc_rot
		//4eme argument odo_rot
		//debug_printf("%d, %d, %d, %d\r\n", save_acc_trans, save_odo_trans, save_acc_rot, save_odo_rot);
/*		save_acc_trans = 0;
		save_odo_trans = 0;
		save_acc_rot = 0;
		save_odo_rot = 0;*/
		if(flag_recouvrement){
			//debug_printf("RECOUVREMENT\n");
			flag_recouvrement = 0;
		}
	}
}
