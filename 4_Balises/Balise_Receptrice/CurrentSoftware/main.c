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
#include "synchro.h"
#include "eyes.h"
#include "brain.h"
#include "secretary.h"
#include "leds.h"

void init(void){
	SYS_init();
	SYSTICK_init(&global.absolute_time);
	PORTS_init();
	QS_WHO_AM_I_find();

	UART_init();
	CAN_init();
	IT_init();

	debug_printf("--- Hello, I'm Beacon receiver %s ---\n", QS_WHO_AM_I_get_name());

	SYNCHRO_init();
	EYES_init();
	BRAIN_init();
	SECRETARY_init();
	LEDS_init();
}

int main (void){
#ifdef CONFIG_RF_RC1240
	char c;
#endif
	init();

	//Temporaire... ces lignes peuvent être supprimées dès qu'on donne un autre usage à ces leds.
	GPIO_WriteBit(LED_RUN,1);
	GPIO_WriteBit(LED_USER,1);
	GPIO_WriteBit(LED_R_SYNC,1);
	GPIO_WriteBit(LED_G_SYNC,1);

	while(1){
		time32_t begin = global.absolute_time;
#ifdef CONFIG_RF_RC1240
		while(UART1_data_ready()){
			c = UART1_get_next_msg();
			debug_printf("We will send %c (0x%x) on RF\n", c, c);
			RF_putc(c);
		}
#endif
		SYNCHRO_process_main();
		EYES_process_main();
		BRAIN_process_main();
		SECRETARY_process_main();

		if(global.absolute_time - begin > 20){
			debug_printf("L%ld\n", global.absolute_time - begin);
		}
	}

	return 0;
}
