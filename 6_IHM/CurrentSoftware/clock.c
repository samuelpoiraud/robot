/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : clock.c
 *  Package : IHM
 *  Description : Gestion de l'IT pour la clock
 *  Auteur : Arnaud
 *  Version 201203
 */

#define _CLOCK_C

#include "clock.h"

#include "QS/QS_timer.h"

#define IT_TIME		1						// en ms (maximum 65)

void CLOCK_init(void){
	//Et c'est parti pour les it !!!
	//Tache d'interruption principale...
	TIMER_init();

	//Note : run_us 5000 est beaucoup plus pr�cis que run tout court � 5...
	TIMER2_run_us(1000*IT_TIME);			//IT clock vitesse de 1ms
}


//Sur interruption timer 2...
void _ISR _T2Interrupt(){
	TIMER2_AckIT(); /* interruption trait�e */
	global.absolute_time += IT_TIME;
}
