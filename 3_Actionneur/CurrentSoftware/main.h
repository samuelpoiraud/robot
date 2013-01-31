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

        #include "Ball_grabber.h"
        #include "Hammer.h"

	//#include "switch.h"
	
	#ifdef USE_CAN
		#include "QS/QS_can.h"
		#include "Can_msg_processing.h"
		#include "QS/QS_CANmsgList.h"
	#endif
	
	void RCON_read();
	void _ISR _MathError();
	void _ISR _StackError();
	void _ISR _AddressError();
	void _ISR _OscillatorFail();
	
#endif /* ndef MAIN_H */

