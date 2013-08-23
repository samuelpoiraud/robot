/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Can_injector.h
 *	Package : Supervision
 *	Description : Injection des messages arrivant de l'UART2 et de l'UART1 sur le CAN 
 *	Auteur : Ronan
 *	Version 20100422
 */

#include "QS/QS_all.h"
 
#ifndef CAN_INJECTOR_H
	#define CAN_INJECTOR_H
	#include "QS/QS_can.h"
	#include "QS/QS_timer.h"
	#include "QS/QS_can_over_uart.h"
	#include "QS/QS_CANmsgList.h"
	#include "RTC.h"
	#include "Eeprom_can_msg.h"
	
	void CAN_INJECTOR_init();
	void CAN_INJECTOR_update();
	void CAN_INJECTOR_send_msg(CAN_msg_t * can_msg, bool_e bCAN, bool_e bU1, bool_e bU2, bool_e bXBee);
		
#endif
