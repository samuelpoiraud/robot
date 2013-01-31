/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionUS.c
 *	Package : Projet Balise Emission US
 *	Description : Gestion de l'�mission US...
 *	Auteur : Nirgal
 *	Version 201103
 */

#include "EmissionUS.h"

#define PERIODE_FLASH	50		//P�riode de r�p�tition des flashs [nombre de step]	//P�riode du flash en �s = PERIODE_FLASH * DUREE_STEP
#define DUREE_STEP		2000	//Dur�e d'un step [us]
#define DUREE_MOTIF 	27		//Dur�e du motif [nombre de step]					//Dur�e du motif en �s = DUREE_MOTIF * DUREE_STEP


//volatile bool_e motif[DUREE_MOTIF] = { 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0};
const bool_e motif[DUREE_MOTIF] = { 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0};
	//ATTENTION, le motif doit se terminer par un z�ro !!! pour d�sactiver la pwm... avant de sortir du motif...


//Pour observer la r�ponse impulsionelle !!! (r�gler �galement la dur�e du step � 25�s !)
//volatile bool_e motif[DUREE_MOTIF] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


#define CHANNEL_PWM 1
volatile bool_e running = FALSE;
volatile Uint16 step = 0;


void EmissionUS_init(void)
{	
	PWM_init();
	#ifdef MODE_MAITRE	//Mode o� il y a une seule balise �mettrice, qui est maitre de la synchro !
		TRISFbits.TRISF8 = 0;
		TIMER1_run_us(DUREE_STEP);
	#endif
}

void EmissionUS_stop(void)
{
	PWM_stop(CHANNEL_PWM);
	#ifdef MODE_MAITRE
		TIMER1_stop();
	#endif
}	
	
volatile bool_e request_reset_step = FALSE;
void EmissionUS_step_init(void)
{
	request_reset_step = TRUE;
}
	
#ifdef MODE_MAITRE
	void _ISR _T1Interrupt()
	{
		EmissionUS_next_step();
	
		IFS0bits.T1IF = 0;
	}	
#endif

/*
TODO : synchronisation et cr�ation du step avec OFFSET d�pendant d'un #define
1 balise �mettrice "maitre", les autres en esclaves
le reste du code reste identique (1 motif � partir de step = 0...)
Les codes sont justes d�phas�s pour que les steps = 0 ne se recoupent pas ! (d�calage 200ms)

*/



void EmissionUS_next_step(void)
{
	bool_e motif_is_now = FALSE;
	Uint16 index_motif = 0;
	
	
	if(request_reset_step == TRUE && step > 0 && step <99)
		step = 0;	
	else
		step = (step==2*PERIODE_FLASH-1)?0:step+1;	
	
	request_reset_step = FALSE;
		
	
	#ifdef MODE_MAITRE
		PORTFbits.RF8 = (step == 0)?1:0;	//Signal de synchro ! � 1 pendant le premier step...
	#endif //MODE_MAITRE
	
	//Le code qui suit peut �tre optimis�, mais il a �t� �crit pour �tre LISIBLE... !
	if(global.mode_double_emetteurs == TRUE)
	{
		#if (NUMERO_BALISE_EMETTRICE == 1)
			//balise 1, le motif est envoy� entre 	0 				et 		  DUREE_MOTIF
			if(step < DUREE_MOTIF)
			{
				motif_is_now = TRUE;
				index_motif = step;
			}
		#else
			//balise 2, le motif est envoy� entre 	 PERIODE_FLASH	et 		  DUREE_MOTIF + PERIODE_FLASH
			if (step >= PERIODE_FLASH 	&& step < (DUREE_MOTIF + PERIODE_FLASH))
			{
				motif_is_now = TRUE;
				index_motif = step - PERIODE_FLASH;
			}	
		#endif
	}	
	else //	global.mode_double_emetteurs == FALSE
	{
		// une seule balise, le motif est envoy� au d�but, ET entre 	 PERIODE_FLASH	et 		  DUREE_MOTIF + PERIODE_FLASH
		if(/* step >= 0 			&& */	step < DUREE_MOTIF)
		{													
			motif_is_now = TRUE;
			index_motif = step;
		}
		else if(step >= PERIODE_FLASH 	&& 	step < (DUREE_MOTIF + PERIODE_FLASH))
		{
			motif_is_now = TRUE;
			index_motif = step - PERIODE_FLASH;
		}	
	}	
	
	
	if(motif_is_now)
	{
		LED_UART = 1;
		assert(index_motif < DUREE_MOTIF);
		if(running == FALSE && motif[index_motif] == TRUE)
		{
			running = TRUE;
			//Activation des transducteurs
			PWMCON1bits.PEN1L = 1;
			PWMCON1bits.PEN1H = 1;
			PTMR = 0x0000;	//permet de toujours lancer un signal identique (et non d�pendant de l'�tat actuel du compteur PWM !)
			PWM_run(60, CHANNEL_PWM);	//Meilleurs r�sultats avec PWM de 60... va savoir pourquoi...
//			LED_RUN = 1;		
		}		
		else if(running == TRUE && motif[index_motif] == FALSE)
		{
			running = FALSE;
		
			//D�sactivation des transducteurs
			PWM_stop(CHANNEL_PWM);
			PWMCON1bits.PEN1L = 0;
			PWMCON1bits.PEN1H = 0;
			
			//Imposons l'�tat pour activer le frein... (voir doc du pont en H)
			TRISEbits.TRISE0 = 0;	//TODO : voir si on peut enlever ces deux lignes !
			TRISEbits.TRISE1 = 0;
			PORTEbits.RE0 = 1; nop();
			PORTEbits.RE1 = 1;
	
//			LED_RUN = 0;
		}
	}
	else
		LED_UART = 0;
	
}

