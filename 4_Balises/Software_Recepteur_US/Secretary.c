/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : Secretary.c
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion de la communication avec l'extérieur.
 *	Auteur : Nirgal
 *	Version 201205
 */
#include "Secretary.h"

#include "QS/QS_all.h"
#include "ReceptionUS.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_timer.h"

	typedef struct
	{
		Uint16 distance;
		Uint8 fiability;
	}adversary_data_t;

static volatile adversary_data_t adversary_datas[ADVERSARY_NUMBER];
static Uint8	nb_adversary_datas_available = 0;
volatile Uint16 count_intervalle_envois_can;	

volatile Uint32 periodic_sending_enabled;	//activation de l'envoi périodique pour les X prochaines [ms]

void SECRETARY_selftest(void);
void SECRETARY_send_adversary_location(void);
bool_e SECRETARY_msg_processing_direct_treatment_function(CAN_msg_t* msg);

void SECRETARY_init(void)
{
	count_intervalle_envois_can = 3;	
	periodic_sending_enabled = 0;
	adversary_e i;
	
	#ifdef USE_CAN
		CAN_init();
		CAN_set_direct_treatment_function(SECRETARY_msg_processing_direct_treatment_function);
	#endif
	for(i=0;i<ADVERSARY_NUMBER;i++)
	{
		adversary_datas[i].distance = 0;
		adversary_datas[i].fiability = ERREUR_PAS_DE_SYNCHRO;
	}
	
	TIMER4_run(100);	
}	

//De nouvelles données sont disponibles. On les sauvegarde pour le prochain envoi !
void SECRETARY_add_datas(adversary_e current_adversary, Uint16 distance, Uint8 fiability)
{
	adversary_datas[current_adversary].distance = distance;
	adversary_datas[current_adversary].fiability = fiability;
	nb_adversary_datas_available++;	//Des données sont récemment arrivées.
}	


void _ISR _T4Interrupt(void)
{
	//TOUTES LES 100ms
	if(count_intervalle_envois_can)
		count_intervalle_envois_can--;
	periodic_sending_enabled = (periodic_sending_enabled>100)?periodic_sending_enabled-100:0;
	IFS1bits.T4IF = 0;
}	



bool_e SECRETARY_msg_processing_direct_treatment_function(CAN_msg_t* msg)
{
	bool_e bret = FALSE;
	LED_CAN = !LED_CAN;
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
		case SUPER_ASK_BEACON_SELFTEST:
			SECRETARY_selftest();
		break;	
		default:	//Actuellement, tout les autres messages sont traités en IT...
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

	if(nb_adversary_datas_available >= 2)	//Dès que 2 données sont dispos (pour 2 adversaires, ou deux fois le même), on envoie ce qu'on a...
	{
		nb_adversary_datas_available = 0;
		if(periodic_sending_enabled != 0)
		{
			SECRETARY_send_adversary_location();
		}
	}

	//Vieux code 
	/*
	//Stratégie d'envoi des données (dépend de quel récepteur on est !) :
	switch(MY_BEACON_ID)
	{
		case BEACON_ID_MOTHER:
		case BEACON_ID_CORNER:
		case BEACON_ID_MIDLE:
			if(nb_adversary_datas_available >= 2)	//Dès que 2 données sont dispos (pour 2 adversaires, ou deux fois le même), on envoie ce qu'on a...
			{
				nb_adversary_datas_available = 0;
				if(periodic_sending_enabled != 0)
				{
					SECRETARY_send_adversary_location();	
				}	
			}
		case BEACON_ID_ROBOT_1:
		case BEACON_ID_ROBOT_2:
		default:
			if(count_intervalle_envois_can == 0 && periodic_sending_enabled != 0)
			{
				//On envois un message de position adverse QUE SI ca n'a pas été fait depuis un certain temps, et que la fonctionnalité d'envoi est activée !
				count_intervalle_envois_can = 3;	//On recharge pour 300ms.
				SECRETARY_send_adversary_location();
			}
		break;	
	}	
	*/
}	
	


void SECRETARY_send_adversary_location(void)
{
	CAN_msg_t msg;
	
	msg.sid = BEACON_ADVERSARY_POSITION_US;
	msg.data[0] = 			adversary_datas[ADVERSARY_1].fiability;	//Si !=0 c'est qu'il y a une erreur ! (et on sait laquelle dans le debug msg can !)	
	msg.data[1] = HIGHINT(	adversary_datas[ADVERSARY_1].distance);
	msg.data[2] = LOWINT (	adversary_datas[ADVERSARY_1].distance);
	msg.data[3]	= MY_BEACON_ID;	//numéro de la balise qui envoie ce message...
	msg.data[4] = 			adversary_datas[ADVERSARY_2].fiability;	//Si !=0 c'est qu'il y a une erreur ! (et on sait laquelle dans le debug msg can !)	
	msg.data[5] = HIGHINT(	adversary_datas[ADVERSARY_2].distance);
	msg.data[6] = LOWINT (	adversary_datas[ADVERSARY_2].distance);
	msg.data[7] = 0xEE;	//RFU : Reserved for futur use
	msg.size = 8;

	#ifdef USE_CAN	
		CAN_send(&msg);
	#endif
	#ifdef CAN_SEND_OVER_UART1
		#warning "envois des messages can dupliqués sur UART"
		CANmsgToU1tx(&msg);
	#endif
	#ifdef VERBOSE_CAN_OVER_UART1
		debug_printf("sid=%x|f1=%x|d1=%d|f2=%x|d2=%d\n",msg.sid, adversary_datas[ADVERSARY_1].fiability, adversary_datas[ADVERSARY_1].distance,adversary_datas[ADVERSARY_2].fiability, adversary_datas[ADVERSARY_2].distance );
	#endif
}



void SECRETARY_selftest(void)
{
	static CAN_msg_t msg_selftest;

	msg_selftest.sid = BEACON_IR_SELFTEST;
	msg_selftest.size = 3;
	msg_selftest.data[1] =	adversary_datas[ADVERSARY_1].fiability;
	msg_selftest.data[2] = 	adversary_datas[ADVERSARY_2].fiability;						

	msg_selftest.data[0] = 	(msg_selftest.data[1] | msg_selftest.data[2] )?1:0;
							//0 si tout va bien, 1 si quelque chose va mal...

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



