/*
 *	Club Robot ESEO 2010 - 2011 - 2012
 *
 *	Fichier : Synchro.c
 *	Package : Projet Balise
 *	Description : fonctions assurant la synchronisation entre émetteur et récepteur.
 *	Auteur : Nirgal
 *	Version 201203
 */
#define SYNCHRO_C
#include "Synchro.h"
#include "QS/QS_timer.h"
#include "main.h"
#include "eyes.h"
#include "secretary.h"
#include <timer.h>
#include <ports.h>


#define SYNCHRO_STATE_DURATION	100//[ms] Durée nominale du signal de synchro à un état haut ou bas
#define SYNCHRO_STATE_TOLERANCE	4//[ms] Durée de la tolérance du signal de synchro à un état haut ou bas

volatile bool_e synchro_received = FALSE;
volatile Uint8 t = 0;

void Synchro_init(void)
{
	TIMER2_run_us(1000);

}

//1ms
void _ISR _T2Interrupt()
{
	if(t)
		t--;
	IFS0bits.T2IF = 0;
}	

bool_e SYNCHRO_get_synchro_received(void)
{
	return synchro_received;
}

	
void Synchro_process_main(void)
{
	static bool_e previous_synchro = FALSE;
	bool_e current_synchro;
	current_synchro = SYNCHRO;
	if(previous_synchro != current_synchro)
	{
		if(t>SYNCHRO_STATE_TOLERANCE)
			synchro_received = FALSE;
		else
			synchro_received = TRUE;
		t = SYNCHRO_STATE_DURATION + SYNCHRO_STATE_DURATION/20;	//105% du temps nominal. (tolérance...)
		
		EYES_set_adversary((current_synchro)?ADVERSARY_1:ADVERSARY_2);
	}
	previous_synchro = current_synchro;
	
	if(t == 0)
		synchro_received = FALSE;
}



