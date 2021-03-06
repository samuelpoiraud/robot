/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : it.c
 *  Package : Balise �mettrice
 *  Description : Gestion des IT
 *  Auteur : Arnaud
 *  Version 200904
 */

#include "it.h"
#include "QS/QS_timer.h"
#include "motor.h"
#include "synchro_rf.h"
#include "emission_ir.h"

#define PERIOD_IT_GENERAL		2
#define PERIOD_IT_MODULATION_US	150  //Fr�quence de 6,66 kHz (Modulation 3,3KHz)

void IT_init(void){
	TIMER_init();

	TIMER1_run(PERIOD_IT_GENERAL);
	TIMER2_run_us(PERIOD_IT_MODULATION_US);
}

void _ISR _T1Interrupt(){

	MOTOR_process_it(PERIOD_IT_GENERAL);
	SyncRF_process_it();

	TIMER1_AckIT();
}

void _ISR _T2Interrupt(){

	static bool_e toggle = FALSE;

	if(toggle){
		EmissionIR_activate(IR_MODULATION_3_3K);
	}else{
		EmissionIR_deactivate(IR_MODULATION_3_3K);
	}

	toggle = !toggle;

	TIMER2_AckIT();
}
