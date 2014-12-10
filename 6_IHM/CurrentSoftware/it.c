/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : it.c
 *  Package : IHM
 *  Description : fonctions en it...
 *  Auteur : Anthony inspiré du code de la prop
 *  Version 200904
 */

#define _IT_C

#include "it.h"

#include "QS/QS_timer.h"
#include "main.h"
#include "button.h"
#include "led.h"
#include "voltage_measure.h"

#define IT_TIME		10		// en ms (maximum 65)


void IT_init(void){
	//Et c'est parti pour les it !!!
	//Tache d'interruption principale...
	TIMER_init();
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10);

	TIMER1_run_us(1000);			//IT clock vitesse de 1ms
}


//Sur interruption timer 1...
void _ISR _T1Interrupt(){
	static Uint8 t_ms = 0;
	TIMER1_AckIT(); /* interruption traitée */
	global.absolute_time++;	//[ms]
	t_ms++;					//[ms]
	if(t_ms >= IT_TIME)	//Toutes les 10ms.
	{
		MAIN_process_it(IT_TIME);
		BUTTONS_process_it(IT_TIME);
		LEDS_process_it(IT_TIME);
		VOLTAGE_MEASURE_process_it(IT_TIME);
	}
}
