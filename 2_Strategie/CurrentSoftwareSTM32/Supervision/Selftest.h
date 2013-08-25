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
#include "../QS/QS_all.h"

#ifndef SELFTEST_H
	#define SELFTEST_H
	
	#include "../QS/QS_can.h"
	#include "../QS/QS_CANmsgList.h"
	#include "../QS/QS_uart.h"
	#include "../QS/QS_watchdog.h"
	#include "../QS/QS_can_over_uart.h"

	
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
		

	void SELFTEST_ask_launch(void);

	//Machine a état du selftest, doit être appelée : dans le process de tâche de fond ET à chaque réception d'un message can de selftest.
	void SELFTEST_update(CAN_msg_t* CAN_msg_received);

	void SELFTEST_print_errors(void);

	//Fonction qui leve le flag précédent appelée par le timer4: 250ms
	void SELFTEST_process_1sec();

	void led_us_update(selftest_beacon_e state);

	void led_ir_update(selftest_beacon_e state);

	void SELFTEST_balise_update();

	void SELFTEST_beacon_counter_init();

	void error_counters_update(CAN_msg_t * msg);
	void SELFTEST_get_match_report_IR(CAN_msg_t * msg);

	void SELFTEST_get_match_report_US(CAN_msg_t * msg);


#endif
