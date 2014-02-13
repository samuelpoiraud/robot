/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionUS.c
 *	Package : Projet Balise Emission US & IR
 *	Description : Gestion de l'�mission IR...
 *	Auteur : Nirgal
 *	Version 201101
 */

#include "EmissionIR.h"
#include "SynchroRF.h"
#include "QS/QS_rf.h"


#define IR_ON 		(LATEbits.LATE7)
#define IR_OFF 		(LATEbits.LATE6)
#define TRIS_IR_ON 	TRISEbits.TRISE7
#define TRIS_IR_OFF	TRISEbits.TRISE6

//#define PERIODE_FLASH	50		//P�riode de r�p�tition des flashs [nombre de step]	//P�riode du flash en �s = PERIODE_FLASH * DUREE_STEP
#define PERIODE_FLASH (TIME_PER_MODULE/2)


void EmissionIR_ON(void)	//IR Toujours allum�
{
	IR_ON = 1;
}	

void EmissionIR_OFF(void)	//IR Toujours �teint
{
	IR_OFF = 0;
	LED_USER = 0;
}

void EmissionIR_AUTO(void)	//IR laiss� aux oscillateurs externes...
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


//Varie de 0 � 50*nbmodules, 100 ms / module
static volatile Uint8 step = 0;
volatile bool_e request_reset_step_ir = FALSE;

//@pre appeler cette fonction lors de la r�ception du signal de synchro...
void EmissionIR_step_init(void)
{
	request_reset_step_ir = TRUE;
}	

//#define DUREE_POUR_UN_EMETTEUR	50 //[2ms] => un �metteur est actif pour 100ms.
//@pre Appeler cette fonction toutes les 2ms
void EmissionIR_next_step(void)
{
	if(request_reset_step_ir == TRUE)
		step = 0;	
	else
		step = (step == RF_MODULE_COUNT*PERIODE_FLASH-1)? 0: step+1;
	
	request_reset_step_ir = FALSE;

	if(step == TIME_WHEN_SYNCHRO)
		SYNCRF_sendRequest();
	
	if(global.mode_double_emetteurs == TRUE)
	{
		#if(NUMERO_BALISE_EMETTRICE == 1)
		
			if(step % 2*PERIODE_FLASH == 0)
				EmissionIR_AUTO();
			if(step % 2*PERIODE_FLASH == PERIODE_FLASH)
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
	//Le d�compte du temps est maintenu m�me si l'on est en simple �metteurs, 
	//pour permettre un passage plus tard (= apr�s la synchro) en mode de double �mission !
}	

