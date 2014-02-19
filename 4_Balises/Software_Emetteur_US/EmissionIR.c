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

#define IR_ON 		(LATEbits.LATE7)
#define IR_OFF 		(LATEbits.LATE6)
#define TRIS_IR_ON 	TRISEbits.TRISE7
#define TRIS_IR_OFF	TRISEbits.TRISE6

#define PERIODE_FLASH	50		//P�riode de r�p�tition des flashs [nombre de step]	//P�riode du flash en �s = PERIODE_FLASH * DUREE_STEP
#define FLASH_CYCLE (PERIODE_FLASH*NOMBRE_BALISES_EMETTRICES) //Un cycle d'�mission des N balises
#define NO_FLASH_TIME 2  //[nb de step] 4ms ou on emet rien au d�but et � la fin, pour avoir donc 4 ms entre la fin de l'�mission d'un balise et le d�but de l'autre

#define TOTAL_STEP_COUNT (RF_MODULE_COUNT*TIME_PER_MODULE)  //step � [0; TOTAL_STEP_COUNT[

#if (TOTAL_STEP_COUNT % FLASH_CYCLE) != 0
#error "Le temps d'un cycle d'emission doit �tre un multiple du temps total de la base de temps de synchro rf"
#endif

#if TOTAL_STEP_COUNT > 255
#error "Il faut utiliser du Uint16 pour la variable step_ir !!!!!! Sinon �a va overflow"
#endif

void EmissionIR_ON(void)	//IR Toujours allum�
{
	IR_ON = 1;
}	

void EmissionIR_OFF(void)	//IR Toujours �teint
{
	IR_OFF = 0;
	//LED_USER = 0;
}

void EmissionIR_AUTO(void)	//IR laiss� aux oscillateurs externes...
{
	IR_ON = 0;
	IR_OFF = 1;	
	//LED_USER = 1;
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
volatile Uint8 step_ir = 0;
static volatile bool_e request_reset_step_ir = FALSE;

//@pre appeler cette fonction lors de la r�ception du signal de synchro...
void EmissionIR_step_init(void)
{
	request_reset_step_ir = TRUE;
}	

//#define DUREE_POUR_UN_EMETTEUR	50 //[2ms] => un �metteur est actif pour 100ms.
//@pre Appeler cette fonction toutes les 2ms
void EmissionIR_next_step(void)
{
	//On compte de 0 � nb_modules*
	if(request_reset_step_ir == TRUE)
		step_ir = 0;
	else
		step_ir = (step_ir == TOTAL_STEP_COUNT - 1)? 0: step_ir+1;
	
	request_reset_step_ir = FALSE;

	if(step_ir == 0) {
		LED_RUN = !LED_RUN;
	}

	if(step_ir == TIME_WHEN_SYNCHRO)
		SYNCRF_sendRequest();
	
	//Si on veut le mode double emetteurs et qu'on a une synchro
	if(global.mode_double_emetteurs == TRUE && global.is_synchronized == TRUE)
	{
		Uint8 step_in_period = step_ir % FLASH_CYCLE;
		#define BEGIN_FLASH_TIME (PERIODE_FLASH*(NUMERO_BALISE_EMETTRICE-1))

		if(step_in_period == (BEGIN_FLASH_TIME + NO_FLASH_TIME))
			EmissionIR_AUTO();
		if(step_in_period == (BEGIN_FLASH_TIME + PERIODE_FLASH - NO_FLASH_TIME))
			EmissionIR_OFF();	//On impose l'extinction.
		
	}
	else
		EmissionIR_AUTO();
	//Le d�compte du temps est maintenu m�me si l'on est en simple �metteurs, 
	//pour permettre un passage plus tard (= apr�s la synchro) en mode de double �mission !
}	

