/*
 *	Club Robot ESEO 2009 - 2010 - 2012
 *	
 *
 *	Fichier : secretary.c
 *	Package : Balises 2010
 *	Description : gestion du CAN sur la carte Balise Recepteur
 *	Auteur : Nirgal
 *	Version 201203
 */

#define BALISE_CAN_C
#include "secretary.h"
#include "QS/QS_can.h"
#include "brain.h"
#include "Synchro.h"
#include "eyes.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_CANmsgList.h"

volatile Uint16 count_intervalle_envois_can;	

volatile Uint32 periodic_sending_enabled;	//activation de l'envoi périodique pour les X prochaines [ms]



bool_e SECRETARY_msg_processing_direct_treatment_function(CAN_msg_t* msg);
void SECRETARY_process_msg(CAN_msg_t * msg);
void SECRETARY_send_adversary_location(void);
void SECRETARY_selftest(void);
	


void SECRETARY_init(void)
{
	#ifdef USE_CAN
		CAN_init();
		CAN_set_direct_treatment_function(SECRETARY_msg_processing_direct_treatment_function);
	#endif
	count_intervalle_envois_can = 3;
		
	periodic_sending_enabled = 0;
}

void SECRETARY_process_it_100ms(void)
{
	global.flag_100ms = TRUE;
	if(count_intervalle_envois_can)
		count_intervalle_envois_can--;
	periodic_sending_enabled = (periodic_sending_enabled>100)?periodic_sending_enabled-100:0;
}	



bool_e SECRETARY_msg_processing_direct_treatment_function(CAN_msg_t* msg)
{
	bool_e bret = FALSE;
	switch(msg->sid)
	{
		//Un appel explicite à ce message est nécessaire pour déclencher l'envoi périodique.		
		case BEACON_ENABLE_PERIODIC_SENDING:
			periodic_sending_enabled = (Uint32)(100000);	//Activation pour 100 prochaines secondes
			//Todo inclure l'information de durée dans l'argument...
		break;
		case BROADCAST_STOP_ALL:
		case BEACON_DISABLE_PERIODIC_SENDING:
			periodic_sending_enabled = 0;
		break;
		case SUPER_ASK_BEACON_SELFTEST:
			bret = TRUE;	//on laisse passer le message... qui sera traité dans le main !
		break;
		default:
		break;
	}
	
	return bret;	//Inutile de mettre les messages CAN en buffer...
	//Ce code permet d'ignorer totalement l'ensemble des messages BROADCAST qui nous arrivent...
	//Et ainsi de gagner du temps en enregistrant pas bettement ces messages inutiles dans le buffer...
}	


void SECRETARY_process_msg(CAN_msg_t * msg)
{
	bool_e trash;
	//debug_printf("\nmsg:%d\n ",global.Can_msg_received.sid);
	trash = SECRETARY_msg_processing_direct_treatment_function(msg);
	switch(msg->sid)
	{
		//case BROADCAST_START:	
/*			case BROADCAST_POSITION_ROBOT:			
			//MAJ de la 'notre' position...
			global.nous_x = 	((Sint16)(global.Can_msg_received.data[0]) << 8) | (Sint16)(global.Can_msg_received.data[1]);
			global.nous_y = 	((Sint16)(global.Can_msg_received.data[2]) << 8) | (Sint16)(global.Can_msg_received.data[3]);
			global.nous_angle = ((Sint16)(global.Can_msg_received.data[4]) << 8) | (Sint16)(global.Can_msg_received.data[5]);
			#warning "Pas utilisé en 2011..."	
*/			
		case SUPER_ASK_BEACON_SELFTEST:
			SECRETARY_selftest();
		break;	
		default:
		break;
	}
}	


void SECRETARY_process_main(void)
{	
	//dernier message can reçu et traité
	#ifdef USE_CAN
		CAN_msg_t msg;
		if(CAN_data_ready())
		{
			LED_CAN = !LED_CAN;		
			msg = CAN_get_next_msg();
			SECRETARY_process_msg(&msg);			
		}
	#endif	
	
	#if (defined USE_UART1 && defined USE_UART1RXINTERRUPT)
	static CAN_msg_t msg_over_uart1;
	if(u1rxToCANmsg(&msg_over_uart1))
		SECRETARY_process_msg(&msg_over_uart1);
	#endif 
	
	#if (defined USE_UART2 && defined USE_UART2RXINTERRUPT)
	static CAN_msg_t msg_over_uart2;
	if(u2rxToCANmsg(&msg_over_uart2))
		SECRETARY_process_msg(&msg_over_uart2);	
	#endif
	
	if(count_intervalle_envois_can == 0 && periodic_sending_enabled != 0)
	{
		//On envois un message de position adverse QUE SI ca n'a pas été fait depuis un certain temps, et que la fonctionnalité d'envoi est activée !
		count_intervalle_envois_can = 3;	//On recharge pour 300ms.
		SECRETARY_send_adversary_location();
	}
}	
	


void SECRETARY_send_adversary_location(void)
{
	CAN_msg_t msg;
	volatile adversary_location_t * p_adversary_location;
	p_adversary_location = BRAIN_get_adversary_location();
	
	msg.sid = BEACON_ADVERSARY_POSITION_IR;
	msg.data[0] = 			p_adversary_location[ADVERSARY_1].error;	//Si !=0 c'est qu'il y a une erreur ! (et on sait laquelle dans le debug msg can !)	
	msg.data[1] = HIGHINT(	p_adversary_location[ADVERSARY_1].angle);
	msg.data[2] = LOWINT (	p_adversary_location[ADVERSARY_1].angle);
	if(msg.data[0] == AUCUNE_ERREUR || msg.data[0] == SIGNAL_INSUFFISANT)
		msg.data[3] = 			p_adversary_location[ADVERSARY_1].distance;	//ATTENTION, distance sur 8 bits ! (donc en [2cm])
	else
		msg.data[3] = 0xFF;	//Si la mesure de distance n'est pas fiable, on renvoit 0xFF.

	msg.data[4] = 			p_adversary_location[ADVERSARY_2].error;	//Si !=0 c'est qu'il y a une erreur ! (et on sait laquelle dans le debug msg can !)	
	msg.data[5] = HIGHINT(	p_adversary_location[ADVERSARY_2].angle);
	msg.data[6] = LOWINT (	p_adversary_location[ADVERSARY_2].angle);
	if(msg.data[4] == AUCUNE_ERREUR || msg.data[4] == SIGNAL_INSUFFISANT)
		msg.data[7] = LOWINT (	p_adversary_location[ADVERSARY_2].distance);	//ATTENTION, distance sur 8 bits ! (donc en cm)
	else
		msg.data[7] = 0xFF;	//Si la mesure de distance n'est pas fiable, on renvoit 0xFF.


	msg.size = 8;
	
	#ifdef USE_CAN	
		CAN_send(&msg);
	#endif
	#ifdef CAN_SEND_OVER_UART1
		#warning "envois des messages can dupliqués sur UART"
		CANmsgToU1tx(&msg);
	#endif
	#ifdef VERBOSE_CAN_OVER_UART1
		debug_printf("sid=%x|e=%x|a=%d°\t|d=%d\n",message.sid, global.localisation_erreur, /*(Sint16)global.adverse_angle, */(Sint16)(((Sint32)global.adverse_angle*180)/PI4096),(Uint16)global.adverse_distance/10);
	#endif
}



void SECRETARY_selftest(void)
{
	volatile adversary_location_t * p_adversary_location;
	p_adversary_location = BRAIN_get_adversary_location();
	static CAN_msg_t msg_selftest;

	msg_selftest.sid = BEACON_IR_SELFTEST;
	msg_selftest.size = 3;
	msg_selftest.data[1] =	p_adversary_location[ADVERSARY_1].error;
	msg_selftest.data[2] = 	p_adversary_location[ADVERSARY_2].error;						
	//On considère que la balise infrarouge n'a pas besoin de synchro. Elle peut fonctionner sans synchro s'il y a un adversaire.
	
	msg_selftest.data[0] = 	(msg_selftest.data[1] | msg_selftest.data[2] )?1:0;
							//0 si tout va bien, un code d'erreur sinon

#ifdef USE_CAN
	CAN_send(&msg_selftest);
#endif
#ifdef CAN_SEND_OVER_UART1
	CANmsgToU1tx(&msg_selftest);
#endif	
#ifdef VERBOSE_CAN_OVER_UART1
	debug_printf("selftest : %s | erreurs : ADV1=0x%x ADV2=0x%x \n",(msg_selftest.data[0])?"OK":"KO", msg_selftest.data[1], msg_selftest.data[2]);
#endif
}
	
	
