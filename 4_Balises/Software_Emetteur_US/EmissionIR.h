/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionIR.h
 *	Package : Balise R�cepteur US et IR
 *	Description : 
 *	Auteur : Nirgal
 *	Version 201101
 */

#ifndef EMISSIONIR_H
	#define EMISSIONIR_H
	
	#include "QS/QS_all.h"

        #define PERIODE_FLASH	50		//P�riode de r�p�tition des flashs [nombre de step]	//P�riode du flash en �s = PERIODE_FLASH * DUREE_STEP
        #define FLASH_CYCLE (PERIODE_FLASH*NOMBRE_BALISES_EMETTRICES) //Un cycle d'�mission des N balises
        #define NO_FLASH_TIME 4  //[nb de step] 8ms ou on emet rien au d�but et � la fin, pour avoir donc 8 ms entre la fin de l'�mission d'un balise et le d�but de l'autre


	extern volatile Uint8 step_ir; //utilis� dans SynchroRF.c
	
	void EmissionIR_init(void);
	void EmissionIR_stop(void);
	void EmissionIR_next_step(void);
	void EmissionIR_step_init(void);
	
#endif /* ndef EMISSIONIR_H */
