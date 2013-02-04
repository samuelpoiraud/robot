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
	#include "QS/QS_pwm.h"
	#include "QS/QS_adc.h"
	#include "QS/QS_timer.h"
	#include "QS/QS_watchdog.h"
	#include "Test_carte_facto.h"
	
	#ifdef MAIN_C
	
		void init(void);
		
	#endif
	
#endif /* ndef MAIN_H */
