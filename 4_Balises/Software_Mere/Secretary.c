/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Secretary.c
 *	Package : Projet Balise Mère
 *	Description : Gestion de la communication avec l'extérieur.
 *	Auteur : Nirgal
 *	Version 201208
 */
#include "Secretary.h"

#include "QS/QS_all.h"
#include "QS/QS_CAN.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_can_over_xbee.h"
#include "Brain_US.h"

void SECRETARY_init(void)
{
	global.color = 0;
	#ifdef USE_CAN
		CAN_init();
	#endif
}	


void SECRETARY_process_msg(CAN_msg_t * msg)
{
	//debug_printf("\nmsg:%d\n ",msg->sid);

	//Message can recu.
	switch(msg->sid)
	{
		case BROADCAST_COULEUR:
			global.color = msg->data[0];
		break;
		case BEACON_ADVERSARY_POSITION_US:
			//On envoie les données reçues au BRAIN_US...
			BRAIN_US_add_datas((beacon_id_e)(msg->data[3]), ADVERSARY_1, (Sint16)(U16FROMU8(msg->data[1], msg->data[2])), msg->data[0]);
			BRAIN_US_add_datas((beacon_id_e)(msg->data[3]), ADVERSARY_2, (Sint16)(U16FROMU8(msg->data[5], msg->data[6])), msg->data[4]);
			LED_USER = !LED_USER;
			switch(msg->data[3])	//BEACON_ID
			{
				case BEACON_ID_MOTHER:
					LED_MOTHER_BEACON = !LED_MOTHER_BEACON;
				break;
				case BEACON_ID_CORNER:
					LED_CORNER_BEACON = !LED_CORNER_BEACON;
				break;
				case BEACON_ID_MIDLE:
					LED_MIDLE_BEACON = !LED_MIDLE_BEACON;
				break;
				default:
					LED_ERROR = !LED_ERROR;
				break;
			}
			//debug_printf("B%d, %dcm, f%x\n",msg->data[3],U16FROMU8(msg->data[1], msg->data[2]), msg->data[0]);
		break;
		default:
		//Ce message ne nous était pas destiné
		break;
	}
	
	
	
}	


void SECRETARY_process_main(void)
{	
	//[_______________________ CAN ________________________]
	
	#ifdef USE_CAN
		CAN_msg_t msg;
		if(CAN_data_ready())
		{
			LED_CAN = !LED_CAN;		
			msg = CAN_get_next_msg();
			SECRETARY_process_msg(&msg);
			#ifdef CAN_OVER_UART1
				CANmsgToU1rx(&msg);
			#endif			
		}
	#endif	
	
	
	//[______________________ UART1 _______________________]
	
	#if (defined USE_UART1 && defined USE_UART1RXINTERRUPT)
		static CAN_msg_t msg_over_uart1;
		if(u1rxToCANmsg(&msg_over_uart1))		//Recu de l'UART1
		{
			SECRETARY_process_msg(&msg_over_uart1);
			#ifdef USE_CAN
			#ifdef ECHO_UART1_TO_CAN
				CAN_send(&msg_over_uart1);
			#endif
			#endif
		}
	#endif 

		
	//[_________________________ XBEE _____________________]
	
		CAN_over_XBee_process_main();
		static CAN_msg_t msg_over_xbee;
		if(XBeeToCANmsg(&msg_over_xbee))		//Recu de l'UART2
		{
			SECRETARY_process_msg(&msg_over_xbee);
			#ifdef CAN_OVER_UART1
				CANmsgToU1rx(&msg_over_xbee);
			#endif
		}
	

}	
	
	/*
	@param us_ir : si TRUE : message US, si FALSE : message IR.
	*/
	void SECRETARY_send_adversary_position(bool_e us_ir, adversary_e adversary, Sint16 x, Sint16 y, Uint8 fiability)
	{
		module_id_e module_id;
		CAN_msg_t msg;
		msg.sid = (us_ir)?BEACON_ADVERSARY_POSITION_US_ARROUND_AREA:BEACON_ADVERSARY_POSITION_IR_ARROUND_AREA;
		msg.size = 6;
		msg.data[0] = adversary;
		msg.data[1] = fiability;
		msg.data[2] = HIGHINT(x);
		msg.data[3] = LOWINT(x);
		msg.data[4] = HIGHINT(y);
		msg.data[5] = LOWINT(y);
		
		//On envoie le message à tout le monde sur le réseau XBEE 
		// (la fonction CANMsgToXBee ne fera rien pour des envois vers nous même... ou vers une destination non opérationelle !)
		for(module_id = 0; module_id < MODULE_NUMBER; module_id++)
			CANMsgToXbee(&msg, module_id);
	}	

	void SECRETARY_can_send_beacon_enable_periodic_sending(void)
	{
		CAN_msg_t msg;
		msg.sid = BEACON_ENABLE_PERIODIC_SENDING;
		msg.size = 0;

	#ifdef USE_CAN
		CAN_send(&msg);
	#endif
		LED_CAN = !LED_CAN;
	}
	
	void SECRETARY_can_send_beacon_disable_periodic_sending(void)
	{
		CAN_msg_t msg;
		msg.sid = BEACON_DISABLE_PERIODIC_SENDING;
		msg.size = 0;
	#ifdef USE_CAN
		CAN_send(&msg);
	#endif
		LED_CAN = !LED_CAN;
	}
	


