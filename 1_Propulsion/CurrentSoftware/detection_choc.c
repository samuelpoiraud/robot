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


#include "detection_choc.h"

// Variables Globales





void DETECTION_CHOC_init(){
	static bool_e initialized = FALSE;

	if(initialized)
		return;

	//Initialisation de l'accéléromètre
	ACC_init();

	initialized = TRUE;
}

//Fonction réalisant un filtre à moyenne mobile  ( a placer en IT ?)
void DETECTION_CHOC_mobile_average_filter(){
	//Sint8 lateral_tab[5];
	//Sint8 frontal_tab[5];

}
