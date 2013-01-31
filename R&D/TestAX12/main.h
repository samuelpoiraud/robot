/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : main.c
 *	Package : Actionneur
 *	Description : Ordonnanceur de la carte actionneur
 *  Auteurs : Aurélien
 *  Version 20110225
 */

#ifndef MAIN_H
	#define MAIN_H
	
	#include "QS/QS_all.h"
	#include "QS/QS_ports.h"
	#include "QS/QS_uart.h"
        #include "QS/QS_timer.h"
        #include "QS/QS_buttons.h"

	void _ISR _MathError();
	void _ISR _StackError();
	void _ISR _AddressError();
	void _ISR _OscillatorFail();
	
#endif /* ndef MAIN_H */

