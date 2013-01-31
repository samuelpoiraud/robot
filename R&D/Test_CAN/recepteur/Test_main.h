/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Test_main.h
 *	Package : Test CAN reception
 *	Description : Fonction receptrice pour test CAN
 *	Auteur : Jacen
 *	Version 20090502
 */

#ifndef TEST_MAIN_H
	#define TEST_MAIN_H
	
	#include "../QS/QS_all.h"
	#ifdef TEST_MODE
		#include "../QS/QS_ports.h"
		#include "../QS/QS_can.h"
//		#include "../QS/QS_timer.h"
//		#include "../QS/QS_pwm.h"
		#include "../QS/QS_uart.h"
		#include <stdio.h>

	#endif /* def TEST_MODE */	
#endif /* ndef MAIN_H */
