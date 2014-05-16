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


	typedef enum
	{
		BEACON_ERROR,
		BEACON_NEAR,
		BEACON_FAR
	}selftest_beacon_e;

	void SELFTEST_init(void);

	void SELFTEST_update(CAN_msg_t* CAN_msg_received);

	void SELFTEST_ask_launch(void);	//Demande de lancement du selftest au prochain process_main

	void SELFTEST_process_main(void);

	void SELFTEST_process_500ms(void);

	Uint16 SELFTEST_measure24_mV(void);

	void led_ir_update(selftest_beacon_e state);

	void SELFTEST_update_led_beacon(CAN_msg_t * can_msg);

	void SELFTEST_beacon_counter_init(void);
	void SELFTEST_get_match_report_IR(CAN_msg_t * msg);
	void SELFTEST_get_match_report_US(CAN_msg_t * msg);

	SELFTEST_error_code_e SELFTEST_getError(Uint8 index);
	char * SELFTEST_getError_string(SELFTEST_error_code_e error_num);
	bool_e SELFTEST_is_running(void);
	bool_e SELFTEST_is_over(void);
	Uint8 SELFTEST_get_errors_number(void);

	bool_e get_warning_bat();
	void clean_warning_bat();


#endif
