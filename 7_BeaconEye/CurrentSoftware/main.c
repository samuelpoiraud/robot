/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.c
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#define MAIN_C
#include "main.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_timer.h"
#include "QS/QS_systick.h"
#ifdef STM32F40XX
#include "QS/QS_sys.h"
#endif
#include "it.h"
#include "zone.h"
#include "Hokuyo.h"
#include "Secretary.h"
#include "LCD/middleware.h"
#include "LCD/low layer/ssd2119.h"
#include "environment.h"
#include "LCD/low layer/ssd2119.h"
#include "IHM/buzzer.h"
#include "LCD/lcd.h"



static void initialisation(void);
static void processMain(void);

int main(void) {

	initialisation();

	debug_printf("------- Hello, I'm BEACON EYE -------\n");

	while(1){
		processMain();
	}

	return 0;
}

static void initialisation(void) {
	SYS_init();											// Initialisation du système
	SYSTICK_init((time32_t*)&(global.absolute_time));	// Init du compteur de temps
	ENVIRONMENT_init();
	PORTS_init();
	UART_init();
	IT_init();
	MIDDLEWARE_init();
	HOKUYO_init();
}

static void processMain(void) {
	LCD_processMain();
	ENVIRONMENT_processMain();
	HOKUYO_process_main();
}
