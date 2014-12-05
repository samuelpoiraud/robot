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
#include "it.h"
#include "button.h"
#include "switch.h"
#include "led.h"
#include "voltage_measure.h"
#include "clock.h"

volatile Uint8 t_10ms = 0;

static void MAIN_global_var_init();

void initialisation(void){
	// Initialisation du système
	SYS_init();				// Init système
	PORTS_init();			// Config des ports
	MAIN_global_var_init();	// Init variable globale

	UART_init();
	CAN_init();
	BUTTONS_IHM_init();
	SWITCHS_init();
	VOLTAGE_MEASURE_init();
	CLOCK_init();
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

static void MAIN_global_var_init(){
	// Initialisation de la variable global

}
