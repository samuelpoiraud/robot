/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : utils.c
 *  Package : Qualit� Soft
 *  Description : 	Gestion d'erreurs: assert, fautes mat�rielles...
 *  Auteur : Gwenn
 *  Version 20100131
 */

#include "error.h"


/*-----------------------------------------------------
                Gestion du fail
-----------------------------------------------------*/
void assert_failed(u8* file, u32 line){
    
    // LED allum�e et arr�t du code
    GPIO_SetBits(GPIOE, GPIO_Pin_7);
	while (1){
	}
}


