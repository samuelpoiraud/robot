/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Can_watch.h
 *	Package : Supervision
 *	Description : Traitement des messages arrivant sur CAN 
 *	Auteur : Ronan
 *	Version 20100422
 */
 
 #include "QS/QS_all.h"
 
 #ifndef CAN_WATCH_H
	#define CAN_WATCH_H
	#include "QS/QS_can.h"
	#include "Buffer.h"
	#include "Interface.h"
	#include "Can_watch.h"
	#include "Selftest.h"
	#include "Eeprom_can_msg.h"
	#include "RTC.h"
	
	void CAN_WATCH_init();
	void CAN_WATCH_update();
	void _ISR _T1Interrupt();
	
	
	/*
	@brief	Messages can reçus sur un uart et qui sont destinés uniquement ou en partie à la supervision.
	@ret	bool_e : TRUE si le message doit être propagé sur le bus can, FALSE si il ne concernait que nous... => pas de propagation.
	@param	can_msg : message can reçu sur l'uart...
	*/
	bool_e CAN_WATCH_process_msg(CAN_msg_t * can_msg);
	
#endif
