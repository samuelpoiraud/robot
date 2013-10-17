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
#include "Supervision/SD/SD.h"
#include "Supervision/RTC.h"
#include "Supervision/Supervision.h"
#include "config_pin.h"
#include "QS/QS_can_over_xbee.h"

void CLOCK_run();

void CLOCK_init()
{
	bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	TIMER_init();
	TIMER1_stop();
	CLOCK_run();	//Lancement du timer pour utilisation avant le début du match.
	global.env.match_time = 0;
	global.env.absolute_time = 0;
}

void CLOCK_run()
{
	TIMER1_run_us(1000);
}

void show_color_on_leds()
{
	if(global.env.color!=BLUE)
	{
		RED_LEDS = 1;
		BLUE_LEDS = 0;
	}
	else
	{
		BLUE_LEDS = 1;
		RED_LEDS = 0;
	}
}

void CLOCK_run_match()
{
	TIMER1_stop();
	TIMER1_run_us(1000);
	show_color_on_leds();	//On fixe la couleur sur les leds avant le début du match.. -> plus de clignotement.
	global.env.match_started = TRUE;
}

void CLOCK_stop()
{
	TIMER1_stop();
}

void _ISR _T1Interrupt()
{
	static Uint16 local_time = 0;
	static Uint16 count_1sec = 0;
	global.env.absolute_time++;

	if(global.env.match_started)
	{
		//Pendant le match.
		global.env.match_time++;

		if(global.env.match_time & 0x100)
			LED_USER=!LED_USER;
	}
	else
	{
		//Avant le début du match.
		local_time++;
		if(local_time == 250)
			show_color_on_leds();	//Màj de la couleur des LEDS + clignotement ON si nécessaire.
		if(local_time == 500)
		{
			local_time = 0;
			if(XBee_is_destination_reachable() == FALSE)
			{	//On a pas de lien XBEE avec l'autre Robot : les leds clignotent.
				BLUE_LEDS = 0;
				RED_LEDS = 0;
			}
		}
	}

	count_1sec++;
	if(count_1sec >= 1000)
	{
		count_1sec = 0;
		RTC_process_it_1sec();
		Supervision_process_1sec();
	}
	SD_process_1ms();

	TIMER1_AckIT();
}
