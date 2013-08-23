/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Can_watch.c
 *	Package : Supervision
 *	Description : 
 *	Auteur : Ronan
 *	Version 20100422
 */
 
#define CAN_WATCH_C
#include "Can_watch.h"
#ifdef ENABLE_XBEE
	#include "QS/QS_can_over_xbee.h"
#endif
#include "Verbose_can_msg.h"
#include "Can_injector.h"
#include "main.h"

void CAN_WATCH_init() {
	TIMER1_run(1);	//Timer cadencé EN PERMANENCE à 1ms...
}
 
 
 /**
 //Propagation d'un message venant du bus CAN :
 
 SI XBEE = ON
 	Si le message est un SID XBEE autorisé
 		Envoi vers XBEE
 
 SI DEBUG = ON
 	SI (XBEE = OFF)
 		Envoi filtré vers U2
 	SI U1_VERBOSE = ON
 		Envoi filtré vers U1 en verbose
 	SINON
 		Envoi filtré vers U1 en brut
 	
 */
 void CAN_WATCH_update() {
	Uint8 filtre;
	static Uint32 filter_beacon_ir = 0;
	static Uint32 filter_beacon_us = 0;
	static Uint32 filter_broadcast_position = 0;
	if(CAN_data_ready())
	{ 
		CAN_msg_t can_msg;

		LED_CAN= !LED_CAN;
		can_msg=CAN_get_next_msg();
		filtre = TRUE;	//Message autorisé.
		
		if(global.config[DEBUG])
		{	
			
			switch(can_msg.sid)
			{
				// Messages que la super envoi.
				
				//FILTRAGE POUR NE PAS ETRE SPAMMES PAR LE MESSAGE DE POSITION_ROBOT....
				case BROADCAST_POSITION_ROBOT:
					if(! (can_msg.data[6] & 0xF0))	//on ne filtre pas les messages dont l'une des raisons est un WARN.
					{
						filtre = FALSE;	//Ca passe pas...
						//Traitement spécial pour les messages d'asser position : maxi 1 par seconde !
						if(global.compteur_de_secondes-1000>filter_broadcast_position) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
						{
							filtre = TRUE;	//En fait ca passe.
							filter_broadcast_position=global.compteur_de_secondes;
						}
					}
				break;		
				
				case BEACON_ADVERSARY_POSITION_IR:
					filtre = FALSE;	//Ca passe pas..
					if(global.compteur_de_secondes-1000>filter_beacon_ir) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
					{
						filtre = TRUE;	
						filter_beacon_ir=global.compteur_de_secondes;
					}
				break;
				case BEACON_ADVERSARY_POSITION_US:
					filtre = FALSE;	//Ca passe pas..
					if(global.compteur_de_secondes-1000>filter_beacon_us) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
					{
						filtre = TRUE;	
						filter_beacon_us=global.compteur_de_secondes;
					}
				break;
				default:	
					filtre = TRUE;	//Message autorisé.
				break;	
			}
		}
		if(filtre)
			CAN_INJECTOR_send_msg(&can_msg,FALSE, TRUE, TRUE, TRUE);	//Everywhere except CAN
	}		
}	


/*
@brief	Messages can reçus sur un uart et qui sont destinés uniquement ou en partie à la supervision.
@ret	bool_e : TRUE si le message doit être propagé sur le bus can, FALSE si il ne concernait que nous... => pas de propagation.
@param	can_msg : message can reçu sur l'uart...
*/
bool_e CAN_WATCH_process_msg(CAN_msg_t * can_msg)
{
	bool_e propagation = TRUE;
	// Messages qui nous sont destinés.
	switch(can_msg->sid)
	{

		case BROADCAST_START:
			global.match_started=TRUE;
			global.current_time_ms=0; 		//On initialise la variable qui compte le nombre de boucle du Timer 1
		break;
		
		case BROADCAST_STOP_ALL:
			global.match_started=FALSE;
			global.config[BUFFER]=FALSE;
		break;
		
		case BROADCAST_COULEUR:
			//Attention, cette propagation est faite pour la propulsion, mais la stratégie, si elle est présente, ne prendra pas en compte cette couleur !
			global.config[COLOR]=can_msg->data[0];
		break;
		
		case SUPER_CONFIG_IS:
				global.config[STRATEGIE]=can_msg->data[0];
				global.config[COLOR]=can_msg->data[1];
				global.config[EVITEMENT]=can_msg->data[2];
				global.config[BALISE]=can_msg->data[3];
					
			if(global.interface_graphique_enable) 
			{
				#ifdef INTERFACE_GRAPHIQUE
					
					print_UART1(ERASE_DISPLAY);
					moveto(1,1);
					print_UART1("Envoi du message CAN de la configuration suivante :");
					INTERFACE_GRAPHIQUE_afficher_config_actuelle();
					moveto(20,12);
					print_UART1("\r\nLe Robot est configure !\r\n");
					moveto(22,14);
					print_UART1("\r\nAu revoir, bon match !\r\n");

		
				#endif /* def INTERFACE_GRAPHIQUE */
									
				global.interface_graphique_enable = FALSE;
			}	
		break;
		
		case BEACON_IR_SELFTEST :
		case BEACON_US_SELFTEST :
		case ACT_SELFTEST :
		case ASSER_SELFTEST :
		case STRAT_SELFTEST :
			SELFTEST_update(&can_msg);
			break;

		case BEACON_ADVERSARY_POSITION_IR:
			if(global.match_started == TRUE)
				//Enregistrement dutype d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data[0] || can_msg->data[4])
				led_ir_update(BEACON_ERROR);
			else if(can_msg->data[3] < 102 || can_msg->data[7] < 102) //Distance IR en cm
					led_ir_update(BEACON_NEAR);
				else
					led_ir_update(BEACON_FAR);
			break;

		case BEACON_ADVERSARY_POSITION_US:
			if(global.match_started == TRUE)
				//Enregistrement dutype d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data[0] || can_msg->data[4])
				led_us_update(BEACON_ERROR);
			else if(can_msg->data[1] < 4 || can_msg->data[5] < 4) //Distance US en mm on test seulement le poids fort autrement dit 1024mm
				led_us_update(BEACON_NEAR);
			else
				led_us_update(BEACON_FAR);
			break;

		case SUPER_RTC_SET:
			propagation = FALSE; //Ce message est pour nous...
			RTC_set_time(&(can_msg->data[0]), &(can_msg->data[1]), &(can_msg->data[2]), &(can_msg->data[3]), &(can_msg->data[4]), &(can_msg->data[5]), &(can_msg->data[6]));			
			/*
				Uint8 secondes
				Uint8 minutes
				Uint8 hours
				Uint8 day
				Uint8 date
				Uint8 months
				Uint8 year	(11 pour 2011)
			*/
			//Retour ... pour vérifier que ca a fonctionné..
			RTC_can_send();
		break;
		case SUPER_RTC_GET:
			propagation = FALSE; //Ce message est pour nous...
			RTC_print_time();			
			RTC_can_send();	
		break;
		case DEBUG_TEST_HARD:
#ifdef TEST_HARD
			debug_msg_received();
#endif
			break;
		default:
			break;			
	}
	
	if(global.config[BUFFER])
		BUFFER_add(can_msg);						//BUFFERISATION
			
	#ifdef EEPROM_CAN_MSG_ENABLE
		if(global.config[EEPROM])
			EEPROM_CAN_MSG_process_msg(can_msg);
	#endif
	return propagation;	
}	

void _ISR _T1Interrupt() {
	
	global.compteur_de_secondes++;
	
	if(global.match_started)
	{
		global.current_time_ms++;
	}	
	
	IFS0bits.T1IF=0;
}
