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
#include "QS/QS_can_verbose.h"
#include "QS/QS_systick.h"
#include "it.h"
#include "button.h"
#include "switch.h"
#include "led.h"
#include "voltage_measure.h"
#include "Can_msg_processing.h"

volatile Uint8 t_ms = 0;
volatile Uint8 t_ms_biroute = 0;

static void MAIN_global_var_init();

void initialisation(void){
	SYS_init();											// Initialisation du système
	MAIN_global_var_init();								// Init variable globale
	SYSTICK_init((time32_t*)&(global.absolute_time));	// Init du compteur de temps
	PORTS_init();
	UART_init();
	CAN_process_init();
	BUTTONS_init();
	SWITCHS_init();
	LEDS_init();
	VOLTAGE_MEASURE_init();
	IT_init();
}

int main (void){
	volatile Uint32 i;
	for(i=0;i<1000000;i++);	//Sinon état des switchs erronés à l'init et envoient des messages CAN

	initialisation();

	QS_WHO_AM_I_find();
	debug_printf("------- Hello, I'm IHM (%s) -------\n", QS_WHO_AM_I_get_name());
	GPIO_SetBits(I_AM_READY);
	CAN_msg_t msg;

	while(1){

		while(CAN_data_ready()){
			// Réception et acquittement
			msg = CAN_get_next_msg();
			CAN_process_msg(&msg);		// Traitement du message pour donner les consignes à la machine d'état
			#ifdef CAN_VERBOSE_MODE
				QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
			#endif
		}

		if(t_ms_biroute >= 10){ //Regarde toute les 10ms si la biroute est présente ou non, avec sécurité
			t_ms_biroute = 0;
			SWITCHS_biroute_update();
		}

		if(t_ms >= 20){	//Pour éviter les rebonds, au dessus de 20ms
			t_ms = 0;
			BUTTONS_update();			//Gestion des boutons
			SWITCHS_update();			//Surveillance des switchs
		}
		VOLTAGE_MEASURE_process_main();	//Surveillance des tensions
		OUTPUTLOG_process_main();
	}

	return 0;
}


void MAIN_process_it(Uint8 ms){
	t_ms += ms;
	t_ms_biroute += ms;
}

static void MAIN_global_var_init(){
	// Initialisation de la variable global

	global.absolute_time = 0;
}
