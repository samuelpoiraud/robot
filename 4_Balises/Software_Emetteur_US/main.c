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
	#warning "Vous compilez pour la balise emettrice no1"
#endif
#if NUMERO_BALISE_EMETTRICE == 2
	#warning "Vous compilez pour la balise emettrice no2"
#endif


int main (void)
{
	Uint32 t;
	OSCCONbits.NOSC  = 0b001;
	PORTS_init();
	UART_init();

	LED_RUN = 1;
	LED_CAN = 0;
	LED_USER2 = 0;

	debug_printf(".RST\r\n");

	//Tempo, pour �viter une d�tection du bouton au d�marrage (car chargement condo lent !)
	for(t = 0; t < (Uint32)(1000000) ; t++);	//Ordre de grandeur 100ms...

	MOTOR_init();
	EmissionIR_init();
	SYNCRF_init();		//le lancement de l'IT timer se fait ici.

	while(1)
	{
		SYNCRF_process_main();
	}
	return 0;
}





