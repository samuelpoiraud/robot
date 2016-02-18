/*
 *	Club Robot 2015
 *
 *	Fichier : main.c
 *	Package : Balise receptrice
 *	Description : Main
 *	Auteur : Arnaud
 */


#include "main.h"
#include "QS/QS_ports.h"
#include "QS/QS_sys.h"
#include "QS/QS_uart.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_systick.h"
#include "it.h"
#include "eyes.h"
#include "brain.h"
#include "secretary.h"

void init(void){
	SYS_init();
	SYSTICK_init(&global.absolute_time);
	PORTS_init();

	UART_init();
	CAN_init();
	IT_init();

	debug_printf("--- Hello, I'm Beacon receiver ---\n");

	EYES_init();
	BRAIN_init();
	SECRETARY_init();
}

int main (void){
	init();

	//Temporaire... ces lignes peuvent être supprimées dès qu'on donne un autre usage à ces leds.
	GPIO_WriteBit(LED_RUN,1);
	GPIO_WriteBit(LED_USER,1);
	GPIO_WriteBit(LED_R_SYNC,1);
	GPIO_WriteBit(LED_G_SYNC,1);

	while(1){
		EYES_process_main();
		BRAIN_process_main();
		SECRETARY_process_main();
	}

	return 0;
}
