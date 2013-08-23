/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : main.h
 *	Package : Supervision
 *	Description : sequenceur de la supervision
 *	Auteur : Jacen
 *	Version 20100105
 */

#ifndef MAIN_H
	#define MAIN_H
	
	#include "QS/QS_all.h"
	#include "QS/QS_ports.h"
	#include "QS/QS_can.h"
	#include "QS/QS_timer.h"
	#include "Can_watch.h"
	#include "Can_injector.h"
	#include "Interface.h"
	#include "Buttons.h"
	#include "Selftest.h"
	#include "Eeprom_can_msg.h"
	#include "RTC.h"
	
	#ifdef MAIN_C
	
		void init(void);
		void RCON_read();
		void _ISR _MathError();
		void _ISR _StackError();
		void _ISR _AddressError();
		void _ISR _OscillatorFail();
		#ifdef TEST_HARD
			void debug_msg_received();
		#endif
		
	#endif
	
#endif /* ndef MAIN_H */
