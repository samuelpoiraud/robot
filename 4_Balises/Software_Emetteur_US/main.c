/*
 *	Club Robot ESEO 2010 - 2012 
 *	CheckNorris - Shark & Fish
 *
 *	Fichier : main.c
 *	Package : Projet Balise R�cepteur US
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple
 *	Auteur : Nirgal
 *	Version 201012
 */

#include "main.h"
#include "QS/QS_outputlog.h"
#include "SynchroRF.h"


#if NUMERO_BALISE_EMETTRICE == 1
	#warning "Vous compilez pour la balise �mettrice no1"
#endif
#if NUMERO_BALISE_EMETTRICE == 2
	#warning "Vous compilez pour la balise �mettrice no2"
#endif

static volatile bool_e bt = TRUE;
static volatile bool_e bt_prec = TRUE;


int main (void)
{		
	Uint32 temps;
	OSCCONbits.NOSC  = 0b001;
	PORTS_init();
	UART_init();
	MOTOR_init();
	
	global.mode_double_emetteurs = FALSE;
	LED_RUN = 1;
	LED_CAN = 0;
	LED_USER2 = 0;

	debug_printf(".RST\r\n");
		




	//Tempo, pour �viter une d�tection du bouton au d�marrage (car chargement condo lent !)
	//Et pour �viter une d�tection de synchro qui n'en est pas une...
	for(temps = 0; temps < (Uint32)(1000000) ; temps++);	//Ordre de grandeur 100ms...

	EmissionIR_init();
	SYNCRF_init();
	
	/*-------------------------------------
		Boucle principale
	-------------------------------------*/	
	while(1)
	{
		bt = !PUSHBUTTON;
		
		if(bt && !bt_prec)
			MOTOR_togle_enable();
		
		bt_prec = bt;
		
		if(SWITCH_MULTI_EMETTEURS)
			global.mode_double_emetteurs = TRUE;
		else
			global.mode_double_emetteurs = FALSE;
			
		MOTOR_process_main();
		SYNCRF_process_main();
	}
	return 0;
}





