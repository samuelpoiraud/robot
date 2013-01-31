/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : main.c
 *	Package : Projet Standard
 *	Description : Gestion de la balise réceptrice Ultrason
 *	Auteur : Nirgal
 *	Version 201205
 */
 #include "main.h"
#include "QS/QS_all.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"
#include "QS/QS_ADC.h"
#include "ports.h"
#include "ReceptionUS.h"
#include "Synchro.h"
#include "QS/QS_spi.h"
#include "PGA.h"
#include "Secretary.h"
#include "QS/QS_CAN.h"
#include "QS/QS_CANmsgList.h"

int main (void)
{	
	/*-------------------------------------
		Démarrage
	-------------------------------------*/
	PORTS_init();
	UART_init();
	ADC_init();

	// LEDs
	LED_RUN = 1;
	LED_CAN = 0;
	LED_USER = 0;
	LED_USER2 = 0;

	debug_printf(".RST\r\n");
	
	//Initialisation des modules logiciels de la balise réceptrice US.
	SECRETARY_init();
	PGA_init();
	ReceptionUS_init();
	Synchro_init();

	/*-------------------------------------
		Boucle principale
	-------------------------------------*/

	while(1)
	{
		ReceptionUS_process_main();
		Synchro_process_main();
		SECRETARY_process_main();
	}//Endloop
	return 0;
}


