/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : utils.c
 *  Package : Qualité Soft
 *  Description : 	Gestion d'erreurs: assert, fautes matérielles...
 *  Auteur : Gwenn
 *  Version 20100131
 */

#include "error.h"


/*-----------------------------------------------------
                Gestion du fail
-----------------------------------------------------*/
void assert_failed(u8* file, u32 line){
    
    // LED allumée et arrêt du code
    GPIO_SetBits(GPIOE, GPIO_Pin_7);
	while (1){
	}
}


