/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_timer.c
 *	Package : QSx86
 *	Description : Gestion et configuration des timers
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20120113
 */

#include "QS_timer.h"


#define PULSE_PER_MS	1
//#define PULSE_PER_US		Inutilisable sur EVE pour l'instant	

void TIMER_init()
{
	Uint8 i;

	for(i=0;i<TIMER_NB;i++)
	{
		eve_global.timers[i].init_value = 0;
		eve_global.timers[i].user_period = 0;
		eve_global.timers[i].timer_run = FALSE;
	}
}

void TIMER1_stop(void)
{	
	eve_global.timers[ID_TIMER_1].init_value = 0;
	eve_global.timers[ID_TIMER_1].user_period = 0;
	eve_global.timers[ID_TIMER_1].timer_run = FALSE;
}

void TIMER1_run(Uint8 period /* en millisecondes */)
{
	eve_global.timers[ID_TIMER_1].init_value = EVE_get_global_clock(eve_global.clock_memory_reference);
	eve_global.timers[ID_TIMER_1].user_period = period;
	eve_global.timers[ID_TIMER_1].timer_run = TRUE;
}

void TIMER1_run_us (Uint16 period /* en microsecondes */)
{	
	Uint16 test = period/1000;

	if(test!=0)
	{
		TIMER1_run(test);
	}
	else
	{
		debug_printf("Temps minimal impossible à appliquer sur TIMER1 : %dms\n",period);
	}
}

void TIMER2_stop(void)
{	
	eve_global.timers[ID_TIMER_2].init_value = 0;
	eve_global.timers[ID_TIMER_2].user_period = 0;
	eve_global.timers[ID_TIMER_2].timer_run = FALSE;
}

void TIMER2_run(Uint8 period /* en millisecondes */)
{
	eve_global.timers[ID_TIMER_2].init_value = EVE_get_global_clock(eve_global.clock_memory_reference);
	eve_global.timers[ID_TIMER_2].user_period = period;
	eve_global.timers[ID_TIMER_2].timer_run = TRUE;
}

void TIMER2_run_us(Uint16 period /* en microsecondes */)
{
	Uint16 test = period/1000;

	if(test!=0)
	{
		TIMER2_run(test);
	}
	else
	{
		debug_printf("Temps minimal impossible à appliquer sur TIMER2 : %dms\n",period);
	}
}

void TIMER3_stop(void)
{	
	eve_global.timers[ID_TIMER_3].init_value = 0;
	eve_global.timers[ID_TIMER_3].user_period = 0;
	eve_global.timers[ID_TIMER_3].timer_run = FALSE;
}

void TIMER3_run(Uint8 period /* en millisecondes */)
{
	eve_global.timers[ID_TIMER_3].init_value = EVE_get_global_clock(eve_global.clock_memory_reference);
	eve_global.timers[ID_TIMER_3].user_period = period;
	eve_global.timers[ID_TIMER_3].timer_run = TRUE;
}
void TIMER3_run_us(Uint16 period /* en microsecondes */)
{
	Uint16 test = period/1000;

	if(test!=0)
	{
		TIMER3_run(test);
	}
	else
	{
		debug_printf("Temps minimal impossible à appliquer sur TIMER3 : %dms\n",period);
	}
}

void TIMER4_stop(void)
{	
	/* Il s'agit en fait de la combinaison des Timers 4 et 5 */
	eve_global.timers[ID_TIMER_4].init_value = 0;
	eve_global.timers[ID_TIMER_4].user_period = 0;
	eve_global.timers[ID_TIMER_4].timer_run = FALSE;
}

void TIMER4_run(Uint16 period /* en millisecondes */)
{
	/* Il s'agit en fait de la combinaison des Timers 4 et 5 */
	eve_global.timers[ID_TIMER_4].init_value = EVE_get_global_clock(eve_global.clock_memory_reference);
	eve_global.timers[ID_TIMER_4].user_period = period;
	eve_global.timers[ID_TIMER_4].timer_run = TRUE;
}

Uint16 TIMER4_read() /* retour en ms*/
{
	return ((EVE_get_global_clock(eve_global.clock_memory_reference) - eve_global.timers[ID_TIMER_4].init_value)/PULSE_PER_MS);
}

/* Suppléments */
Uint16 ConfigIntTimer1(Uint16 unknow_flag)
{
	// Pas implémentée
	return END_SUCCESS;
}

Uint16 DisableIntT1()
{
	// Pas implémentée
	return END_SUCCESS;
}

Uint16 EnableIntT1()
{
	// Pas implémentée
	return END_SUCCESS;
}
