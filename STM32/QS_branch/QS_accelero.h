/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : QS_accelero.h
 *  Package : Qualit� Soft
 *  Description : Gestion de l'acc�l�rom�tre de la catre STM32
 *  Auteur : Anthony
 */

/** ----------------  Defines possibles  --------------------
 *	Aucun
 */

#ifndef ACCELEROMETRE_H
	#define ACCELEROMETRE_H

	#include "QS_types.h"

	//Proc�dure initialisant l'acc�l�rom�tre
	void ACC_init(void);

	//Proc�dure lisant l'ID de l'acc�l�rom�tre pour savoir de quel type il est
	Uint8 ACC_readID(void);

	//Proc�dure pour faire un reset de l'acc�l�rom�tre
	void ACC_reset(void);

	//Proc�dure pour tout nettoyer avant de d�sallouer l'objet
	void ACC_deInit(void);

	//Proc�dure lisant les valeurs donn�es par l'acc�l�rom�tre et les sauvegardes dans les variables Xlu, Ylu et Zlu
	void ACC_read(void);

	//Proc�dure donnant acc�s � la variable Xlu
	Sint16 ACC_getX(void);

	//Proc�dure donnant acc�s � la variable Ylut
	Sint16 ACC_getY(void);

	//Proc�dure donnant acc�s � la variable Zlu
	Sint16 ACC_getZ(void);


	/*  Exemple pour r�cup�rer l'angle
	 *		Quand la carte est � plat.
	 *		A adapter, selon la position de la carte
	 *
	 *		Angle autour de l'axe X
	 *		debug_printf("Pitch   %d\n",(Sint16)(atan2(ACC_getX(),ACC_getZ())*180/3.14));
	 *		Angle autour de l'axe Y
	 *		debug_printf("Roll   %d\n",(Sint16)(atan2(ACC_getY(),ACC_getZ())*180/3.14));
	 */

#endif /* ACCELEROMETRE_H */