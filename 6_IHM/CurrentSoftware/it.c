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


void IT_init(void){
	//Et c'est parti pour les it !!!
	//Tache d'interruption principale...
	TIMER_init();
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10);

	//Note : run_us 5000 est beaucoup plus précis que run tout court à 5...
	TIMER1_run(10);			//IT trajectoire et Correcteur,va à une vitesse de 10ms
}


//Sur interruption timer 1...
void _ISR _T1Interrupt(){
	TIMER1_AckIT(); /* interruption traitée */

	MAIN_process_it(10);
	BUTTONS_IHM_process_it();
}
