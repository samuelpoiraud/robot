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

static time_t time=0;

void CLOCK_init()
{
	CLOCK_TIMER_run(100);
}

#include "QS/QS_uart.h"
void _ISR CLOCK_TIMER_isr()
{
	LED_RUN = !LED_RUN;
	time++;
	CLOCK_TIMER_flag = 0;
}	

time_t CLOCK_get_time()
{
	return time;
}
