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
#include "Supervision/Selftest.h"
#include "QS/QS_can_over_xbee.h"
#include "Supervision/Buzzer.h"
#include "act_functions.h"
#include "QS/QS_IHM.h"
#include "QS/QS_ports.h"

void CLOCK_run();

void CLOCK_init()
{
	bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	TIMER_init();
	TIMER1_stop();
	CLOCK_run();	//Lancement du timer pour utilisation avant le d�but du match.
	env.match_time = 0;
	env.absolute_time = 0;
}

void CLOCK_run()
{
	TIMER1_run_us(1000);
}

void show_color_on_leds()
{
#ifndef FDP_2014
	static color_e local_color = -1;
	if(env.color != local_color)
	{
		if(env.color == BOT_COLOR)
			IHM_leds_send_msg(1,(led_ihm_t){LED_COLOR_IHM, LED_COLOR_YELLOW});
		else
			IHM_leds_send_msg(1,(led_ihm_t){LED_COLOR_IHM, LED_COLOR_GREEN});
		local_color = env.color;
	}
#else
	if(env.color == BOT_COLOR)
	{
		GPIO_ResetBits(BLUE_LEDS);
		GPIO_SetBits(GREEN_LEDS);
		GPIO_SetBits(RED_LEDS);
	}
	else
	{
		GPIO_ResetBits(BLUE_LEDS);
		GPIO_SetBits(GREEN_LEDS);
		GPIO_ResetBits(RED_LEDS);
	}
#endif
}

void CLOCK_run_match()
{
	TIMER1_stop();
	TIMER1_run_us(1000);
	show_color_on_leds();	//On fixe la couleur sur les leds avant le d�but du match.. -> plus de clignotement.
}


void _ISR _T1Interrupt()
{
	static Uint16 local_time = 0;
	static Uint16 count_1sec = 0;
	env.absolute_time++;

	if(env.match_started && !env.match_over)	//Match commenc� et NON termin�
	{
		//Pendant le match.
		env.match_time++;

		if(env.match_time & 0x100)
			toggle_led(LED_USER);
	}

	local_time++;
	if(local_time == 250)
		show_color_on_leds();	//M�j de la couleur des LEDS + clignotement ON si n�cessaire.
	if(local_time == 500)
	{
		local_time = 0;
		#ifdef FDP_2014
		if(XBee_is_destination_reachable() == FALSE || IHM_switchs_get(SWITCH_XBEE) == FALSE)
		{	//On a pas de lien XBEE avec l'autre Robot : les leds clignotent.
			//ATTENTION, si l'on d�sactive apr�s allumage le XBEE sur l'un des robot... l'autre robot qui a eu le temps de dialoguer en XBEE ne clignotera pas !
			GPIO_ResetBits(BLUE_LEDS);
			GPIO_ResetBits(GREEN_LEDS);
			GPIO_ResetBits(RED_LEDS);
		}
		#endif
		SELFTEST_process_500ms();
	}

	count_1sec++;
	if(count_1sec >= 1000)
	{
		count_1sec = 0;
		RTC_process_it_1sec();
	}
	SD_process_1ms();
	BUZZER_process_1ms();
	Supervision_process_1ms();
	TIMER1_AckIT();
}
