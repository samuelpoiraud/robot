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
	#ifdef STM32F40XX
		#include "QS/QS_sys.h"
	#endif
	#include "it.h"
	#include "zone.h"
	#include "Hokuyo.h"
	#include "Secretary.h"
	#include "LCD/middleware.h"
	#include "LCD/low layer/ssd2119.h"
#include "QS/QS_systick.h"

static void MAIN_global_var_init();

void initialisation(void) {
	SYS_init();											// Initialisation du système
	MAIN_global_var_init();								// Init variable globale
	SYSTICK_init((time32_t*)&(global.absolute_time));	// Init du compteur de temps
	PORTS_init();
	UART_init();
	IT_init();
	MIDDLEWARE_init();
	//HOKUYO_init();
	//ZONE_init();
	//SECRETARY_init();
}

int main (void){
	initialisation();

	bool_e isTouched = FALSE;
	MIDDLEWARE_addButton(10, 10, 100, 40, "Bouton", FALSE, &isTouched, SSD2119_COLOR_RED, SSD2119_COLOR_YELLOW, SSD2119_COLOR_RED, SSD2119_COLOR_BLACK);
	/*MIDDLEWARE_addLine(10, 20, 30, 20, SSD2119_COLOR_RED);
	MIDDLEWARE_addLine(20, 10, 20, 30, SSD2119_COLOR_RED);

	MIDDLEWARE_addLine(290, 220, 310, 220, SSD2119_COLOR_RED);
	MIDDLEWARE_addLine(300, 210, 300, 230, SSD2119_COLOR_RED);*/


	debug_printf("------- Hello, I'm BEACON EYE -------\n");

	while(1){
		MIDDLEWARE_processMain();
		//HOKUYO_process_main();
		//ZONE_process_main();
		//SECRETARY_process_main();	//A venir prochainement...
	}
	return 0;
}

static void MAIN_global_var_init(){
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.match_suspended = FALSE;
	global.absolute_time = 0;
	global.match_time = 0;
	global.current_color = BOT_COLOR;
}
