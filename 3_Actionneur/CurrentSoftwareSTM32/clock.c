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
#include "QS/QS_DCMotor2.h"

#include "config_pin.h"

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
	TIMER_SRC_TIMER_start_ms(10);
}

#include "QS/QS_uart.h"
void TIMER_SRC_TIMER_interrupt()
{
	#ifdef USE_DCMOTOR2
		DCM_process_it();
	#endif

	LED_RUN = !LED_RUN;
	global.absolute_time += 10;
#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME)
	BUTTONS_process_it();
#endif
	TIMER_SRC_TIMER_resetFlag();
}
