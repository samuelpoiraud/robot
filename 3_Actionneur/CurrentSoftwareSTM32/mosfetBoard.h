/*
 *	Club Robot ESEO 2016 - 2017
 *	Harry & Anne
 *
 *	Fichier : mosfetBoard.h
 *	Package : Actionneur
 *	Description : Communication avec la carte mosfet
 *  Auteurs : Arnaud
 *  Version 20110225
 */

#ifndef MOSFET_BOARD_H
	#define MOSFET_BOARD_H

	#include "QS/QS_all.h"
	#include "QS/QS_CANmsgList.h"

	#ifdef USE_MOSTFET_BOARD
	
		void MOSFET_BOARD_init();
		void MOSFET_BOARD_processMain();
		void MOSFET_BOARD_getPumpStatus(Uint8 id);
		void MOSFET_BOARD_setMostfet(Uint8 id, bool_e state);
		void MOSFET_BOARD_putCanMsg(CAN_msg_t * msg);
		void MOSFET_BOARD_getTurbineSpeed();
		void MOSFET_BOARD_setTurbineSpeed(Uint16 speed);

		void MOSFET_BOARD_ACT_init();
		void MOSFET_BOARD_ACT_init_pos();
		void MOSFET_BOARD_ACT_stop();
		bool_e MOSFET_BOARD_ACT_CAN_process_msg(CAN_msg_t* msg);
		void MOSFET_BOARD_ACT_reset_config();

	#endif

#endif /* ndef MOSFET_BOARD_H */

