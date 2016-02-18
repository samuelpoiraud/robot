/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : it.c
 *  Package : Balise r�ceptrice
 *  Description : Gestion des IT
 *  Auteur : Arnaud
 *  Version 200904
 */

#include "it.h"
#include "QS/QS_timer.h"
#include "eyes.h"
#include "secretary.h"

void IT_init(void){
	TIMER_init();

	TIMER1_run(PERIOD_IT_SECRETARY);
	TIMER2_run_us(PERIOD_IT_GET_TSOP_US);
}

void _ISR _T1Interrupt(){

	SECRETARY_process_it_100ms();
	TIMER1_AckIT();
}

void _ISR _T2Interrupt(){

	EYES_process_it();
	TIMER2_AckIT();
}
