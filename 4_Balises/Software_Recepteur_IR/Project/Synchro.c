/*
 *	Club Robot ESEO 2010 - 2011 - 2012
 *
 *	Fichier : Synchro.c
 *	Package : Projet Balise
 *	Description : fonctions assurant la synchronisation entre �metteur et r�cepteur.
 *	Auteur : Nirgal
 *	Version 201203
 */
#define SYNCHRO_C
#include "Synchro.h"
#include "QS/QS_timer.h"
#include "main.h"
#include "eyes.h"
#include <timer.h>
#include <ports.h>







#define DUREE_STEP		2000	//Dur�e d'un step [us]


volatile Sint16 correction_timer = 0;
volatile Uint16 cable_synchro_present = 0;

volatile bool_e synchro_received = FALSE;
void Synchro_correction_deviation_timer(void);

void Synchro_init(void)
{



	synchro_received = FALSE;
	SetPriorityInt0(7);
	//SetPriorityIntT3(6);

	EXTERN_IT_enable(INT0, RISING_EDGE);
}



//IT externe assurant le point de d�part du synchronisme..
//Cette IT se d�clenche lorsque le cable est branch�...
void _ISR _INT0Interrupt()	//Attention IT de priorit� 7 !!! (le maximum...)
{
	static bool_e first_it = TRUE;

	if(first_it)
	{
		TIMER2_run_us(DUREE_STEP);
		TIMER3_run_us(DUREE_STEP);

		TMR2 = 0;
		TMR3 = 2;
		first_it = FALSE;
	}
	else
	{
		TMR2 = 0;
	}

	//ReceptionUS_step_init();
	//EmissionUS_step_init();
	//EmissionIR_step_init();
	EYES_step_init();
	cable_synchro_present = 120;	//IT recue = le cable de synchro est pr�sent !
	SECRETARY_synchro_received();

	synchro_received = TRUE;
	//EmissionUS_next_step();
	EXTERN_IT_acknowledge(INT0);
}

bool_e SYNCHRO_get_synchro_received(void)
{
	return synchro_received;
}


//Timer de r�f�rence, resynchronis� � chaque r�ception de signal de synchro...
void _ISR _T2Interrupt()
{

	IFS0bits.T2IF = 0;
}


//Timer de fonctionnement, autonome, lanc� une fois, et dont la correction doit suivre l'�metteur...au mieux !
	//Pour cela, soit on suit le timer 1 de r�f�rence, LORSQUE le c�ble est branch�.
	//Soit on suit une correction fig�e, LORSQUE le c�ble est d�branch�.
void _ISR _T3Interrupt()
{
	LED_RUN = 0;
	Synchro_correction_deviation_timer();


	//ReceptionUS_next_step();
	//EmissionUS_next_step();
	//EmissionIR_next_step();
	EYES_next_step();
	LED_RUN = 1;
	IFS0bits.T3IF = 0;
}


volatile Uint16 step_correction;


#define TAILLE_MOYENNE_MOBILE 4
Sint16 Synchro_moyenne_mobile(Sint16 nouvelle_valeur)
{
	static Sint16 tableau_valeurs[TAILLE_MOYENNE_MOBILE];	//m�morisation des variables pr�c�dentes en buffer circulaire
	static Uint8 index = 0;									//index courant du buffer
	static Uint8 init = 0;									//nombre de valeurs dans le tableau (initialement � 0, puis en r�gime permanent � TAILLE_MOYENNE_MOBILE)
	Uint8 i;
	Sint32 moyenne;	//Valeur � retourner

	if(nouvelle_valeur == 0)	//demande d'initialisation
	{
		index = 0;
		init = 0;
		return 0;
	}

	tableau_valeurs[index] = nouvelle_valeur;		//Nouvelle valeur dans le tableau
	index  = (index + 1)%TAILLE_MOYENNE_MOBILE;		//M�j index courant du tableau


	if(init < TAILLE_MOYENNE_MOBILE)
		init++;										//M�j init, si n�cessaire

	//Calcul de la moyenne
	moyenne = 0;
	for(i=0;i<init;i++)
		moyenne += tableau_valeurs[i];
	moyenne /= init;

	return (Sint16)(moyenne);
}

static volatile Sint16 correction_demandee = 0;

#define PERIODE_CORRECTION 1000	//Chaque correction s'effectue � cette p�riode (unit� = dur�e d'un step = 2000�s).
void Synchro_correction_deviation_timer(void)
{


	if(cable_synchro_present)
		cable_synchro_present--;		//Obsolescence progressive de la pr�sence du signal de synchro... Si == 0, le c�ble n'est plus consid�r� comme pr�sent.

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
		//La correction de la d�viation du TMR3 est faite "loin" de ses extr�mes... pour �viter de sauter un step !!!

	if(correction_demandee && TMR3 > 5000 && TMR3 < 15000)
	{
		correction_demandee = FALSE;

		step_correction = 0;

		if(cable_synchro_present)
		{

			difference_timer = (Sint16)(TMR2) - (Sint16)(TMR3);		//Mesure de la diff�rence entre les timers.




			if(first_synchro)
			{
				//Synchro_moyenne_mobile(0);	//initialiser car valeurs obsol�tes !
				first_synchro = FALSE;
			}
			else

				correction_timer = Synchro_moyenne_mobile(difference_timer); //on met � jour la moyenne mobile pour pauffiner "correction_timer"

			//TODO led qui indique que la synchro est OK...
			SRbits.IPL = 7;
			TMR3 = TMR2;
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



