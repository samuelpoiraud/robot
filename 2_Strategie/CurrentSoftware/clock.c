/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : clock.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de l'horloge
 *	Auteur : Jacen
 *	Version 20090322
 */

#define CLOCK_C

#include "clock.h"

void CLOCK_init()
{
	TIMER_init();
	TIMER1_stop();
	global.env.match_time = 0;
}	

void CLOCK_run_match()
{
	TIMER1_run_us(1000);
	global.env.match_started = TRUE;
}

void CLOCK_stop()
{
	TIMER1_stop();
}	

void _ISR _T1Interrupt()
{
	if(global.env.match_started)
	{
		global.env.match_time++;
		if(global.env.match_time & 0x100)
			LED_USER=!LED_USER;
	}
	IFS0bits.T1IF=0;
}
