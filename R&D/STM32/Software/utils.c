/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : utils.c
 *  Package : Qualité Soft
 *  Description : 	Utilitaires
 *  Auteur : Gwenn
 *  Version 20100131
 */

#include "utils.h"


/*-----------------------------------------------------
             Temporisation pour le debug
-----------------------------------------------------*/
void delay(vu32 nCount){
	for(; nCount != 0; nCount--);
}

