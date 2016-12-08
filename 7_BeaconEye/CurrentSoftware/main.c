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
	#include "zone.h"
	#include "Hokuyo.h"
	#include "Secretary.h"
	#include "environment.h"
	#include "LCD/middleware.h"
	#include "LCD/low layer/ssd2119.h"
	#include "LCD/low layer/fonts.h"
	#include "LCD/image/logoESEO.h"
	#include "LCD/image/buttonOff.h"
	#include "LCD/image/waiting.h"
	#include "LCD/image/terrain.h"
#include "QS/QS_systick.h"


int main (void){
	#ifdef STM32F40XX
		SYS_init();
	#endif

	PORTS_init();

	ENV_init();
	SYSTICK_init((time32_t*)&(global.absolute_time));

	UART_init();

	debug_printf("------- Hello, I'm BEACON EYE -------\n");

	//ZONE_init();
	//SECRETARY_init();

	ENV_set_color(BOT_COLOR);

	//HOKUYO_init(); // Initialise l'usb pour l'hokuyo NE PAS METTRE DE DELAY JUSTE APRES.

	TIMER_init();
	TIMER1_run_us(1000);

	debug_printf("Before MIDDLEWARE init\n");

	// ################################################
	// ############### Test SSD2119 ###################
	// ################################################
	/*SSD2119_init();
	SSD2119_rotate(SSD2119_Orientation_Landscape_1);
	SSD2119_fill(SSD2119_COLOR_BLACK);
	SSD2119_drawPixel(0,0, SSD2119_COLOR_RED);
	SSD2119_putc(10, 20, 'G', &Font_7x10,SSD2119_COLOR_RED, SSD2119_COLOR_YELLOW);
	SSD2119_printf(30, 20, &Font_7x10, SSD2119_COLOR_RED, SSD2119_TRANSPARENT, "Bonjour, j'ai %d. Ceci est un message qui est tres long pour aller a la ligne.", 23);
	SSD2119_drawLine(0, 50, 319, 50, SSD2119_COLOR_YELLOW);
	SSD2119_drawRectangle(0, 60, 20, 80, SSD2119_COLOR_YELLOW);
	SSD2119_drawFilledRectangle(30, 60, 50, 80, SSD2119_COLOR_YELLOW);
	SSD2119_drawCircle(70, 70, 10, SSD2119_COLOR_YELLOW);
	SSD2119_drawFilledCircle(90, 90, 10, SSD2119_COLOR_YELLOW);*/

	// ################################################
	// ############## Test middleware #################
	// ################################################
	MIDDLEWARE_init();
	SSD2119_rotate(SSD2119_Orientation_Landscape_2);
	MIDDLEWARE_setBackground(SSD2119_COLOR_BLACK);
	MIDDLEWARE_addLine(0, 0, 0, 0, SSD2119_COLOR_RED);
	MIDDLEWARE_addText(10, 20, SSD2119_COLOR_RED, SSD2119_COLOR_YELLOW, "G");
	MIDDLEWARE_addText(30,20,SSD2119_COLOR_RED, SSD2119_TRANSPARENT, "Bonjour, j'ai %d. Ceci est un message qui est tres long pour aller a la ligne.", 23);
	MIDDLEWARE_addLine(0, 50, 319, 50, SSD2119_COLOR_YELLOW);
	MIDDLEWARE_addRectangle(0, 60, 20, 20, SSD2119_COLOR_YELLOW, SSD2119_TRANSPARENT);
	MIDDLEWARE_addRectangle(30, 60, 20, 20, SSD2119_COLOR_YELLOW, SSD2119_COLOR_YELLOW);
	MIDDLEWARE_addImage(60, 60, &buttonOff);
	MIDDLEWARE_addAnimatedImage(100, 60, &waiting);
	MIDDLEWARE_addImage(0, 0, &terrain);

	while(1){
		MIDDLEWARE_processMain();

		ENV_process_main();
		//HOKUYO_process_main();
		//ZONE_process_main();
		//SECRETARY_process_main();	//A venir prochainement...
		//LCD_process_main();
	}
	return 0;
}


void _ISR _T1Interrupt(){
	//ENV_process_it();
	//ZONE_process_it_1ms();
	//LCD_process_it_1ms();
	//SECRETARY_process_ms();
	TIMER1_AckIT();
}
