/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : main.c
 *	Package : Projet Standard
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple
 *	Auteur : Jacen
 *	Version 20100620
 */

#include "main.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"

int main (void)
{
	PORTS_init();
//	UART_init();
	TIMER_init();
	TIMER1_run_us(100);
	PORTD = 0;//x0600;
	while (1)
	{
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
		PORTD ^= 0x0100;
	}
	return 0;
}

void _ISR _T1Interrupt ()
{
	PORTD ^= 0x0200;
	IFS0bits.T1IF = 0;
}
