/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionIR.h
 *	Package : Balise Récepteur US et IR
 *	Description : 
 *	Auteur : Nirgal
 *	Version 201101
 */

#ifndef EMISSIONIR_H
	#define EMISSIONIR_H
	
	#include "QS/QS_all.h"

        #define PERIODE_FLASH	50		//Période de répétition des flashs [nombre de step]	//Période du flash en µs = PERIODE_FLASH * DUREE_STEP
        #define FLASH_CYCLE (PERIODE_FLASH*NOMBRE_BALISES_EMETTRICES) //Un cycle d'émission des N balises
        #define NO_FLASH_TIME 4  //[nb de step] 8ms ou on emet rien au début et à la fin, pour avoir donc 8 ms entre la fin de l'émission d'un balise et le début de l'autre


	extern volatile Uint8 step_ir; //utilisé dans SynchroRF.c
	
	void EmissionIR_init(void);
	void EmissionIR_stop(void);
	void EmissionIR_next_step(void);
	void EmissionIR_step_init(void);
	
#endif /* ndef EMISSIONIR_H */
