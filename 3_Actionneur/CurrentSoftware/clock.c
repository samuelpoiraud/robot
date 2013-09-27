/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : clock.c
 *	Package : Actionneur
 *	Description : decompte du temps sur actionneur
 *  Auteurs : Jacen
 *  Version 20100412
 */

#define CLOCK_C
#include "clock.h"
#include "QS/QS_buttons.h"

#ifdef CLOCK_TIMER_ID
	#define TIMER_SRC_TIMER_ID CLOCK_TIMER_ID
#endif
#ifdef CLOCK_USE_WATCHDOG
	#define TIMER_SRC_USE_WATCHDOG
#endif

#include "QS/QS_setTimerSource.h"


#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME) && defined(BUTTONS_TIMER)
#error "Clock ne doit pas mettre a jour les temps d'appui des boutons si un autre timer est utilisé !"
#endif

static clock_time_t time=0;

void CLOCK_init()
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME)
	BUTTONS_init();
#endif
	TIMER_SRC_TIMER_init();
	TIMER_SRC_TIMER_start_ms(100);
}

#include "QS/QS_uart.h"
void TIMER_SRC_TIMER_interrupt()
{
	LED_RUN = !LED_RUN;
	time++;
#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME)
	BUTTONS_process_it();
#endif
	TIMER_SRC_TIMER_resetFlag();
}	

clock_time_t CLOCK_get_time()
{
	return time;
}
