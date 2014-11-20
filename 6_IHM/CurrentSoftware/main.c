/*
 *	Club Robot 2014
 *
 *	Fichier : main.c
 *	Package : IHM
 *	Description : fonction principale
 *	Auteur : Anthony
 *	Version 20080924
 */


#include "main.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "it.h"
#include "button.h"
#include "switch.h"
#include "led.h"
#include "voltage_measure.h"
#include "clock.h"

#if defined (STM32F40XX)
	#include "QS/QS_sys.h"
#endif

volatile Uint8 t_10ms = 0;

void initialisation(void){
	#if defined(STM32F40XX)
		SYS_init();
	#endif
	// Config des ports
	PORTS_init();

	volatile Uint32 i;
	for(i=0;i<1000000;i++);	//tempo (env 50ms) pour un bon fonctionnement de l'UART lorsqu'on branche les cartes. Sinon, les premiers printf ne sont pas envoyés -> ????


	UART_init();
	CAN_init();
	BUTTONS_IHM_init();
	SWITCHS_init();
	VOLTAGE_MEASURE_init();
	CLOCK_init();

	debug_printf("--- Hello, I'm IHM ---\n");
	debug_printf("\n-------\nWaiting for other boards ready\n-------\n");

	GPIO_SetBits(I_AM_READY);

	time32_t begin_waiting_time = global.absolute_time;
	bool_e FDP_init = FALSE;
	CAN_msg_t waiting_msg;
	while(global.absolute_time - begin_waiting_time < 2000 && !FDP_init){
		while (CAN_data_ready())
		{
			waiting_msg = CAN_get_next_msg();
			if(waiting_msg.sid == BROADCAST_FDP_READY)
				FDP_init = TRUE;
		}
	}

	if(FDP_init)
		debug_printf("Démarrage synchronisé\n");
	else
		debug_printf("Démarrage non syncrhonisé via timeout\n");

	IT_init();
}

int main (void){
	initialisation();

	while(1){

		if(t_10ms > 2){	//Pour éviter les rebonds, au dessus de 20ms
			t_10ms = 0;
			BUTTONS_IHM_update();			//Gestion des boutons
			SWITCHS_update();			//Surveillance des switchs
		}
	}

	return 0;
}


void MAIN_process_it(Uint8 tp_10ms){
	t_10ms += tp_10ms;
}
