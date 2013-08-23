/*
 *	Club Robot ESEO 2009 - 2011
 *	Chek'Norris
 *
 *	Fichier : Selftest.h
 *	Package : Supervision
 *	Description : Envoi et réception des messages de selftest. 
 *	Auteur : Ronan & Patman
 *	Version 20100422
 */
#include "QS/QS_all.h"

#ifndef SELFTEST_H
	#define SELFTEST_H
	
	#include "QS/QS_can.h"
	#include "QS/QS_CANmsgList.h"
	#include "QS/QS_uart.h"
	#include "QS/QS_watchdog.h"
	#include "QS/QS_can_over_uart.h"

	#define LED_R_BALISE_US LATBbits.LATB4
	#define LED_R_BALISE_IR LATBbits.LATB14
	#define LED_R_BALISE LATBbits.LATB12
	#define LED_R_ROBOT LATBbits.LATB10
	#define LED_R_UNUSED LATBbits.LATB8

	#define LED_V_BALISE_US LATBbits.LATB5
	#define LED_V_BALISE_IR LATBbits.LATB15
	#define LED_V_BALISE LATBbits.LATB13
	#define LED_V_ROBOT LATBbits.LATB11
	#define LED_V_UNUSED LATBbits.LATB9
	
	#define LED_ON	1
	#define LED_OFF	0
	
	#define FORWARD 0x01
	#define	REAR 0x10
		
	#define TEMPS_SYNCHRO 10
	#define NOMBRE_TESTS_BALISE 2
	
	typedef enum
	{
		BEACON_ERROR,
		BEACON_NEAR,
		BEACON_FAR
	}selftest_beacon_e;	
		
	void SELFTEST_init();
	void SELFTEST_update();
	void beacon_flag_update();
	void led_us_update(selftest_beacon_e state);
	void led_ir_update(selftest_beacon_e state);
	void SELFTEST_balise_update();
	void LED_init();
	void SELFTEST_beacon_counter_init();
	void error_counters_update(CAN_msg_t * msg);
	void SELFTEST_get_match_report_IR(CAN_msg_t * msg);
	void SELFTEST_get_match_report_US(CAN_msg_t * msg);

#endif
