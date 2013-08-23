/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Can_injector.c
 *	Package : Supervision
 *	Description : Injection des messages arrivant de l'UART2 et de l'UART1 sur le CAN 
 *	Auteur : Ronan
 *	Version 20100422
 */
 
#define CAN_INJECTOR_C

#include "Can_injector.h"
#include "Can_watch.h"
#include "Verbose_can_msg.h"
#ifdef ENABLE_XBEE
	#include "QS/QS_can_over_xbee.h"
#endif
	
void CAN_INJECTOR_init() {

}


/**
	PROPAGATION DES MESSAGES RECUS.
	
	En provenance de U1 :
		Propagé vers XBee si XBEE=ON et SID autorisé.
		Propagé vers U2 si XBEE=OFF
		S'il ne m'est pas destiné
			Propagé vers CAN
	
	En provenance de U2 (Xbee=OFF) :
		S'il ne m'est pas destiné
			Propagé vers CAN
		Propagé vers U1
	
	En provenance de XBEE
		S'il ne m'est pas destiné
			Propagé vers CAN
		Propagé vers U1
		
	Cas particulier des messages qui nous sont destinés : AUCUNE PROPAGATION VERS LE CAN...
		 
*/
void CAN_INJECTOR_update(void) 
{
	static CAN_msg_t can_msg_1;
	static CAN_msg_t can_msg_2;
	

	if(u1rxToCANmsg(&can_msg_1))
		CAN_INJECTOR_send_msg(&can_msg_1,TRUE, FALSE, TRUE, TRUE);	//Everywhere except U1.		

	if(!global.config[XBEE])	
	{
		if(u2rxToCANmsg(&can_msg_2))
			CAN_INJECTOR_send_msg(&can_msg_2,TRUE, TRUE, FALSE, FALSE);	//Everywhere except U2 and XBee.
	}
		
	#ifdef ENABLE_XBEE
		if(global.config[XBEE])
		{
			if(XBeeToCANmsg(&can_msg_2))
			{
				if((can_msg_2.sid & 0xFF0) == XBEE_FILTER)
					can_msg_2.sid = (can_msg_2.sid & 0x00F) | STRAT_XBEE_FILTER;
				CAN_INJECTOR_send_msg(&can_msg_2,TRUE, TRUE, FALSE, FALSE);	//Everywhere except U2 and XBee.		
			}
		}
	#endif
}


void CAN_INJECTOR_send_msg(CAN_msg_t * can_msg, bool_e bCAN, bool_e bU1, bool_e bU2, bool_e bXBee)
{
	bool_e propagate;
	//On traite le message quelque soit sa provenance... 
	propagate = CAN_WATCH_process_msg(can_msg);
	if(bCAN)
	{
		if(propagate)	//On ne propage pas vers le CAN un message qui nous est destiné.
			CAN_send(can_msg);	
	}

	if(bXBee && global.config[XBEE])
	{
		#ifdef ENABLE_XBEE
			if((can_msg->sid & 0xFF0) == XBEE_FILTER)
				CANMsgToXbee(can_msg,global.XBEE_module_id_destination);
		#endif	
	}
		
	if(global.config[DEBUG] && bU1)
	{
		if(global.config[U1_VERBOSE])
			VERBOSE_CAN_MSG_print(can_msg);
		else
			CANmsgToU1tx(can_msg);
	}
	
	if(global.config[DEBUG]  && !global.config[XBEE] && bU2)
	{
		CANmsgToU2tx(can_msg);
	}
	LED_USER = !LED_USER;
}	
