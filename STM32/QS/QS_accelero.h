/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : QS_accelero.h
 *  Package : Qualité Soft
 *  Description : Gestion de l'accélérométre de la catre STM32
 *  Auteur : Anthony
 */

/** ----------------  Defines possibles  --------------------
 *	Aucun
 */

#ifndef ACCELEROMETRE_H
	#define ACCELEROMETRE_H

	#include "QS_types.h"

	//Procédure initialisant l'accéléromètre
	void ACC_init(void);

	//Procédure lisant les valeurs données par l'accéléromètre et les sauvegardes dans les variables Xlu, Ylu et Zlu
	void ACC_read(void);

	//Procédure donnant accès à la variable Xlu
	Sint8 ACC_getX(void);

	//Procédure donnant accès à la variable Ylut
	Sint8 ACC_getY(void);

	//Procédure donnant accès à la variable Zlu
	Sint8 ACC_getZ(void);


	/*  Exemple pour récupérer l'angle
	 *		Quand la carte est à plat.
	 *		A adapter, selon la position de la carte
	 *
	 *		Angle autour de l'axe X
	 *		debug_printf("Pitch   %d\n",(Sint16)(atan2(ACC_getX(),ACC_getZ())*180/3.14));
	 *		Angle autour de l'axe Y
	 *		debug_printf("Roll   %d\n",(Sint16)(atan2(ACC_getY(),ACC_getZ())*180/3.14));
	 */

#endif /* ACCELEROMETRE_H */
