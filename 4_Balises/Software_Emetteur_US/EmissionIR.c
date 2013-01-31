/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionUS.c
 *	Package : Projet Balise Emission US & IR
 *	Description : Gestion de l'émission IR...
 *	Auteur : Nirgal
 *	Version 201101
 */

#include "EmissionIR.h"


#define IR_ON 		(LATEbits.LATE7)
#define IR_OFF 		(LATEbits.LATE6)
#define TRIS_IR_ON 	TRISEbits.TRISE7
#define TRIS_IR_OFF	TRISEbits.TRISE6

#define PERIODE_FLASH	50		//Période de répétition des flashs [nombre de step]	//Période du flash en µs = PERIODE_FLASH * DUREE_STEP


void EmissionIR_ON(void)	//IR Toujours allumé
{
	IR_ON = 1;
}	

void EmissionIR_OFF(void)	//IR Toujours éteint
{
	IR_OFF = 0;
	LED_USER = 0;
}

void EmissionIR_AUTO(void)	//IR laissé aux oscillateurs externes...
{
	IR_ON = 0;
	IR_OFF = 1;	
	LED_USER = 1;
}	
	
	
void EmissionIR_init(void)
{	
	EmissionIR_AUTO();
	TRIS_IR_ON  = 0;
	TRIS_IR_OFF = 0;
}	

void EmissionIR_stop(void)
{
	EmissionIR_OFF();
}	

static volatile Uint8 step = 0;
volatile bool_e request_reset_step_ir = FALSE;

//@pre appeler cette fonction lors de la réception du signal de synchro...
void EmissionIR_step_init(void)
{
	request_reset_step_ir = TRUE;
}	

#define DUREE_POUR_UN_EMETTEUR	50 //[2ms] => un émetteur est actif pour 100ms.
//@pre Appeler cette fonction toutes les 2ms
void EmissionIR_next_step(void)
{
	if(request_reset_step_ir == TRUE && step > 0 && step <99)
		step = 0;	
	else
		step = (step==2*PERIODE_FLASH-1)?0:step+1;	
	
	request_reset_step_ir = FALSE;
	
	if(global.mode_double_emetteurs == TRUE)
	{
		#if(NUMERO_BALISE_EMETTRICE == 1)
		
			if(step == 0)
				EmissionIR_AUTO();
			if(step == PERIODE_FLASH)
				EmissionIR_OFF();	//On impose l'extinction.
				
		#else
		
			if(step == PERIODE_FLASH)
				EmissionIR_AUTO();
			if(step == 0)
				EmissionIR_OFF();	//On impose l'extinction.
		#endif	
		
	}
	else
		EmissionIR_AUTO();
	//Le décompte du temps est maintenu même si l'on est en simple émetteurs, 
	//pour permettre un passage plus tard (= après la synchro) en mode de double émission !
}	

