/*
 *	Club Robot ESEO 2010 - 2011 - 2012
 *
 *	Fichier : Synchro.c
 *	Package : Projet Balise
 *	Description : fonctions assurant la synchronisation entre émetteur et récepteur.
 *	Auteur : Nirgal
 *	Version 201203
 */
#define SYNCHRO_C
#include "Synchro.h"
#include <timer.h>
#include <ports.h>
#include "ReceptionUS.h"
#include "QS/QS_timer.h"

void Synchro_correction_deviation_timer(void);
Sint16 Synchro_moyenne_mobile(Sint16 nouvelle_valeur);


#define DUREE_STEP		2000	//Durée d'un step [us]


volatile Sint16 correction_timer = 0;
volatile Uint16 cable_synchro_present = 0;

	
void Synchro_init(void)
{
	SetPriorityInt2(7);
	//ATTENTION, sur cette balise réceptrice US, la priorité des interruptions timer est donnée au timer 2 qui cadence les mesures ADC.
	//On lance les acquisitions. Priorité donnée au timer 2.
	SetPriorityIntT3(5);
	SetPriorityIntT2(6);
	EXTERN_IT_enable(INT2, RISING_EDGE);
}	



//IT externe assurant le point de départ du synchronisme..
//Cette IT se déclenche lorsque le cable est branché...
void _ISR _INT2Interrupt()	//Attention IT de priorité 7 !!! (le maximum...)
{
	static bool_e first_it = TRUE;

	if(first_it)
	{
		TIMER1_run_us(DUREE_STEP);
		TIMER3_run_us(DUREE_STEP);	
		TMR1 = 0;
		TMR3 = 2;
		first_it = FALSE;
	}	
	else
	{
		TMR1 = 0;
	}	

	ReceptionUS_step_init();
	//EmissionUS_step_init();
	//EmissionIR_step_init();
	cable_synchro_present = 120;	//IT recue = le cable de synchro est présent !
	//ReceptionUS_next_step();
	//EmissionUS_next_step();
	EXTERN_IT_acknowledge(INT2);
}


//Timer de référence, resynchronisé à chaque réception de signal de synchro...
void _ISR _T1Interrupt()
{		
	
	IFS0bits.T1IF = 0;
}


//Timer de fonctionnement, autonome, lancé une fois, et dont la correction doit suivre l'émetteur...au mieux !
	//Pour cela, soit on suit le timer 1 de référence, LORSQUE le câble est branché.
	//Soit on suit une correction figée, LORSQUE le câble est débranché.
void _ISR _T3Interrupt()
{	
	LED_RUN = 0;
	Synchro_correction_deviation_timer();
		
	ReceptionUS_next_step();
	//EmissionUS_next_step();
	//EmissionIR_next_step();
	LED_RUN = 1;
	IFS0bits.T3IF = 0;
}


volatile Uint16 step_correction;


#define TAILLE_MOYENNE_MOBILE 4
Sint16 Synchro_moyenne_mobile(Sint16 nouvelle_valeur)
{
	static Sint16 tableau_valeurs[TAILLE_MOYENNE_MOBILE];	//mémorisation des variables précédentes en buffer circulaire
	static Uint8 index = 0;									//index courant du buffer
	static Uint8 init = 0;									//nombre de valeurs dans le tableau (initialement à 0, puis en régime permanent à TAILLE_MOYENNE_MOBILE)
	Uint8 i;
	Sint32 moyenne;	//Valeur à retourner
	
	if(nouvelle_valeur == 0)	//demande d'initialisation
	{
		index = 0;
		init = 0;
		return 0;	
	}	
	
	tableau_valeurs[index] = nouvelle_valeur;		//Nouvelle valeur dans le tableau
	index  = (index + 1)%TAILLE_MOYENNE_MOBILE;		//Màj index courant du tableau

	
	if(init < TAILLE_MOYENNE_MOBILE)
		init++;										//Màj init, si nécessaire	
		
	//Calcul de la moyenne
	moyenne = 0;
	for(i=0;i<init;i++)
		moyenne += tableau_valeurs[i];
	moyenne /= init;
	
	return (Sint16)(moyenne);
}

static volatile Sint16 correction_demandee = 0;

#define PERIODE_CORRECTION 1000	//Chaque correction s'effectue à cette période (unité = durée d'un step = 2000µs).
void Synchro_correction_deviation_timer(void)
{

	
	if(cable_synchro_present)
		cable_synchro_present--;		//Obsolescence progressive de la présence du signal de synchro... Si == 0, le câble n'est plus considéré comme présent.

	step_correction++;
	if(step_correction >= PERIODE_CORRECTION)
	{
		correction_demandee = TRUE;	
	}		
}	


void Synchro_process_main(void)
{
	static bool_e first_synchro = TRUE;
	static Sint16 difference_timer;
		//La correction de la déviation du TMR3 est faite "loin" de ses extrêmes... pour éviter de sauter un step !!!
	
	if(correction_demandee && TMR3 > 5000 && TMR3 < 15000)
	{
		correction_demandee = FALSE;
		
		step_correction = 0;
		
		if(cable_synchro_present)
		{

			difference_timer = (Sint16)(TMR1) - (Sint16)(TMR3);		//Mesure de la différence entre les timers.
			
			if(first_synchro)
			{
				//Synchro_moyenne_mobile(0);	//initialiser car valeurs obsolètes !
				first_synchro = FALSE;
			}
			else
				correction_timer = Synchro_moyenne_mobile(difference_timer); //on met à jour la moyenne mobile pour pauffiner "correction_timer"

			//TODO led qui indique que la synchro est OK...
			SRbits.IPL = 7;
			TMR3 = TMR1;	
			SRbits.IPL = 0;
		}
		else
		{
			first_synchro = TRUE;
			SRbits.IPL = 7;
			TMR3 = TMR3 + correction_timer;	
			SRbits.IPL = 0;
		}
	}	
	
	
		
//	debug_printf("Correction : %d\n",correction_timer);
}

	

