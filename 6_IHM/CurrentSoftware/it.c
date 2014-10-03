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

#define IT_TIME		10		// en ms (maximum 65)

void IT_init(void){
	//Et c'est parti pour les it !!!
	//Tache d'interruption principale...
	TIMER_init();
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10);

	//Note : run_us 5000 est beaucoup plus précis que run tout court à 5...
	TIMER1_run_us(1000*IT_TIME);			//IT principale vitesse de 10ms
}


//Sur interruption timer 1...
void _ISR _T1Interrupt(){
	TIMER1_AckIT(); /* interruption traitée */

	MAIN_process_it(IT_TIME);
	BUTTONS_IHM_process_it(IT_TIME);
	LEDS_process_it();
}
