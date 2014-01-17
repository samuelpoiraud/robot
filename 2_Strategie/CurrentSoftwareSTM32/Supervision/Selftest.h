/*
 *	Club Robot ESEO 2009 - 2011
 *	Chek'Norris
 *
 *	Fichier : Selftest.h
 *	Package : Supervision
 *	Description : Envoi et r�ception des messages de selftest. 
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

	void SELFTEST_ask_launch(void);

	void SELFTEST_process_main(void);

	//Machine a �tat du selftest, doit �tre appel�e : dans le process de t�che de fond ET � chaque r�ception d'un message can de selftest.
	void SELFTEST_update(CAN_msg_t* CAN_msg_received);

	//Enregistre (et affiche si verbose) la liste des erreurs du selftest.
	void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size);

	//Fonction qui leve le flag pr�c�dent appel�e par le timer4: 250ms
	void SELFTEST_process_1sec();

	void led_us_update(selftest_beacon_e state);

	void led_ir_update(selftest_beacon_e state);

	void SELFTEST_balise_update();

	void SELFTEST_beacon_counter_init();

	void error_counters_update(CAN_msg_t * msg);
	void SELFTEST_get_match_report_IR(CAN_msg_t * msg);

	void SELFTEST_get_match_report_US(CAN_msg_t * msg);


#endif
