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
#include "QS/QS_sys.h"
#include "QS/QS_who_am_i.h"
#include "it.h"
#include "button.h"
#include "switch.h"
#include "led.h"
#include "voltage_measure.h"

volatile Uint8 t_10ms = 0;

static void MAIN_global_var_init();

void initialisation(void){
	// Initialisation du système
	SYS_init();				// Init système
	PORTS_init();			// Config des ports
	MAIN_global_var_init();	// Init variable globale

	UART_init();
	CAN_init();
	BUTTONS_init();
	SWITCHS_init();
	VOLTAGE_MEASURE_init();
	IT_init();
}

int main (void){
	volatile Uint32 i;
	for(i=0;i<1000000;i++);	//Sinon état des switchs erronés à l'init et envoient des messages CAN

	initialisation();

	QS_WHO_AM_I_find();
	debug_printf("------- Hello, I'm IHM (%s) -------\n", QS_WHO_AM_I_get_name());

	CAN_msg_t msg;
	msg.sid = IHM_SET_LED;
	msg.data[0] = OFF << 5 | LED_3_IHM;
	msg.data[1] = OFF << 5 | LED_3_IHM;
	msg.data[2] = OFF << 5 | LED_3_IHM;
	msg.data[3] = OFF << 5 | LED_3_IHM;
	msg.data[4] = OFF << 5 | LED_4_IHM;
	msg.data[5] = ON << 5 | LED_5_IHM;
	msg.data[6] = OFF << 5 | LED_UP_IHM;
	msg.data[7] = LED_COLOR_BLACK << 5 | LED_COLOR_IHM;
	msg.size = 8;
	LEDS_get_msg(&msg);

	while(1){

		if(t_10ms > 2){	//Pour éviter les rebonds, au dessus de 20ms
			t_10ms = 0;
			BUTTONS_update();			//Gestion des boutons
			SWITCHS_update();			//Surveillance des switchs
		}
	}

	return 0;
}


void MAIN_process_it(Uint8 tp_10ms){
	t_10ms += tp_10ms;
}

static void MAIN_global_var_init(){
	// Initialisation de la variable global

	global.absolute_time = 0;
}
