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


#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME) && defined(BUTTONS_TIMER)
#error "Clock ne doit pas mettre a jour les temps d'appui des boutons si un autre timer est utilisé !"
#endif

#if (!defined(CLOCK_TIMER_ID) || CLOCK_TIMER_ID > 4 || CLOCK_TIMER_ID < 1) && !defined(CLOCK_USE_WATCHDOG)
	#error "Clock: CLOCK_TIMER_ID non défini ou invalide, vous devez choisir le numéro du timer entre 1 et 4 inclus, ou utiliser le watchdog avec CLOCK_USE_WATCHDOG"
#elif defined(CLOCK_USE_WATCHDOG)
	#include "QS/QS_watchdog.h"

	static Uint8 CLOCK_watchdog_timeout_id;
	static void CLOCK_watchdog_timeout();

	#define CLOCK_TIMER_init() WATCHDOG_init()
	#define CLOCK_TIMER_interrupt CLOCK_watchdog_timeout
	#define CLOCK_TIMER_resetFlag() CLOCK_TIMER_start(100)
	#define CLOCK_TIMER_start(period_ms) (CLOCK_watchdog_timeout_id = WATCHDOG_create(period_ms, &CLOCK_watchdog_timeout))
	#define CLOCK_TIMER_stop() WATCHDOG_stop(CLOCK_watchdog_timeout_id)
#else
	#include "QS/QS_timer.h"

	//Pour plus d'info sur la concaténation de variable dans des macros, voir http://stackoverflow.com/questions/1489932/c-preprocessor-and-concatenation
	#define CLOCK_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
	#define CLOCK_TEMP_CONCAT(a,b,c) CLOCK_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)
	#define CLOCK_TIMER_interrupt _ISR CLOCK_TEMP_CONCAT(_T, CLOCK_TIMER_ID, Interrupt)
	#if CLOCK_TIMER_ID < 4
		#define CLOCK_TIMER_resetFlag() CLOCK_TEMP_CONCAT(IFS0bits.T, CLOCK_TIMER_ID, IF) = 0
	#else
		#define CLOCK_TIMER_resetFlag() CLOCK_TEMP_CONCAT(IFS1bits.T, CLOCK_TIMER_ID, IF) = 0
	#endif
	#define CLOCK_TIMER_init() TIMER_init()
	#define CLOCK_TIMER_start(period_ms) CLOCK_TEMP_CONCAT(TIMER, CLOCK_TIMER_ID, _run)(period_ms)
	#define CLOCK_TIMER_stop() CLOCK_TEMP_CONCAT(TIMER, CLOCK_TIMER_ID, _stop)()
#endif

static time_t time=0;

void CLOCK_init()
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME)
	BUTTONS_init();
#endif
	CLOCK_TIMER_init();
	CLOCK_TIMER_start(100);

	debug_printf("Clock initialized\n");
}

#include "QS/QS_uart.h"
void CLOCK_TIMER_interrupt()
{
	LED_RUN = !LED_RUN;
	time++;
#if defined(CLOCK_UPDATE_BUTTONS_PRESS_TIME)
	BUTTONS_process_it();
#endif
	CLOCK_TIMER_resetFlag();
}	

time_t CLOCK_get_time()
{
	return time;
}
