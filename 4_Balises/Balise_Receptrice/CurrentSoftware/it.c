/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : it.c
 *  Package : Balise réceptrice
 *  Description : Gestion des IT
 *  Auteur : Arnaud
 *  Version 200904
 */

#include "it.h"
#include "QS/QS_timer.h"
#include "eyes.h"
#include "secretary.h"
#include "leds.h"

void IT_init(void){
	TIMER_init();

	TIMER1_run(PERIOD_IT_SECRETARY);
	TIMER2_run_us(PERIOD_IT_GET_TSOP_US);
	TIMER5_run_us(PERIOD_IT_LEDS_MUX);
}

void _ISR _T1Interrupt(){

	SECRETARY_process_it_100ms();
	TIMER1_AckIT();
}

void _ISR _T2Interrupt(){

	EYES_process_it();
	TIMER2_AckIT();
}


void _ISR _T5Interrupt(){
	static Uint16 counter = 0;

	LEDS_mux_process_it();

	if(counter * PERIOD_IT_LEDS_MUX >= PERIOD_IT_LEDS_MANAGER){
		counter = 0;
		LEDS_manager_process_it();
	}

	counter++;
	TIMER5_AckIT();
}
