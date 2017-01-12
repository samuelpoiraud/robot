/*
 *	Club Robot 2015
 *
 *	Fichier : main.c
 *	Package : Balise émettrice
 *	Description : Main
 *	Auteur : Arnaud
 */


#include "main.h"
#include "QS/QS_ports.h"
#include "QS/QS_sys.h"
#include "QS/QS_uart.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_systick.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_can_verbose.h"
#include "QS/QS_rf.h"
#include "it.h"
#include "motor.h"
#include "synchro_rf.h"
#include "can_msg_processing.h"
#include "supervision.h"
#include "emission_ir.h"

void init(void){
	SYS_init();
	SYSTICK_init(&global.absolute_time);
	PORTS_init();

	QS_WHO_AM_I_find();

	UART_init();
	IT_init();

	debug_printf("--- Hello, I'm Beacon transmitter (%s)---\n", (QS_WHO_AM_I_get() == BIG_ROBOT)?"BIG ROBOT":"SMALL ROBOT");

	MOTOR_init();
	SYNCRF_init();
	EmissionIR_init();

	SUPERVISION_init();

}

int main (void){
#ifdef CONFIG_RF_RC1240
	char c;
#endif
	CAN_msg_t msg;
	init();

	GPIO_WriteBit(LED_G_SYNC, 1);
	GPIO_WriteBit(LED_R_SYNC, 1);
	GPIO_WriteBit(LED_BAT, 1);

	while(1){

		/*-------------------------------------
			Réception CAN et exécution
		-------------------------------------*/
		while(RF_can_data_ready()){
			msg = RF_can_get_next_msg();
			CAN_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			#ifdef CAN_VERBOSE_MODE
				QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
			#endif
		}

#ifdef CONFIG_RF_RC1240
		while(UART1_data_ready()){
			c = UART1_get_next_msg();
			debug_printf("We will send %c (0x%x) on RF\n", c, c);
			RF_putc(c);
		}
#endif

		SYNCRF_process_main();
		SUPERVISION_process_main();
		OUTPUTLOG_process_main();
	}

	return 0;
}
