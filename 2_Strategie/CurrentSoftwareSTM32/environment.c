/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP
 *
 *	Fichier : environment.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de supervision de l'environnement
 *	Auteur : Jacen, Nirgal
 *	Version 20100420, modifs 201304
 */

#define ENVIRONMENT_C

#include "environment.h"
#include "act_functions.h"
#include "can_utils.h"
#include "zone_mutex.h"
#include "Supervision/Buffer.h"
#include "Supervision/Eeprom_can_msg.h"
#include "Supervision/RTC.h"
#include "Supervision/Selftest.h"
#include "Supervision/Supervision.h"
#include "Supervision/Verbose_can_msg.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_can_over_xbee.h"
#include "Supervision/SD/term_io.h"
#include "Supervision/SD/SD.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_adc.h"
#include "QS/QS_uart.h"
#include "QS/QS_CANmsgList.h"
#include "maths_home.h"
#include "sick.h"
#include "telemeter.h"
#include "button.h"
#include "elements.h"
#include "Supervision/LCD_CAN_injector.h"

#include "config_use.h"
#include "config_pin.h"

/* met à jour l'environnement en fonction du message CAN reçu */
void CAN_update (CAN_msg_t* incoming_msg);

/* met a jour la position a partir d'un message asser la délivrant */
void ENV_pos_update (CAN_msg_t* msg);

/* met a jour la position de l'adversaire à partir des balises */
void ENV_pos_foe_update (CAN_msg_t* msg);

#define ADC_THRESHOLD 10 //Valeur de l'ADC sans dispositif de connecté

		//une couleur erronnée pour forcer la maj de la couleur
		#define COLOR_INIT_VALUE 0xFF


void ENV_check_filter(CAN_msg_t * msg, bool_e * bUART_filter, bool_e * bCAN_filter, bool_e * bSAVE_filter)
 {
	static time32_t filter_beacon_ir = 0;
	static time32_t filter_beacon_us = 0;
	static time32_t filter_broadcast_position = 0;

	*bUART_filter = TRUE;	//On suppose que le message est autorisé.
	*bSAVE_filter = TRUE;

	switch(msg->sid)
	{
		//FILTRAGE POUR NE PAS ETRE SPAMMES PAR LE MESSAGE DE POSITION_ROBOT....
		case BROADCAST_POSITION_ROBOT:
			if(! (msg->data[6] & 0xF0))	//on ne filtre pas les messages dont l'une des raisons est un WARN.
			{
				//On ne propage pas les messages de BROADCAST_POSITION_ROBOT (dans le cas où les raisons ne sont pas des WARN).
				*bSAVE_filter = FALSE;
				//Traitement spécial pour les messages d'asser position : maxi 1 par seconde !
				if(global.env.absolute_time-1000>filter_broadcast_position) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
					filter_broadcast_position=global.env.absolute_time;
				else
					*bUART_filter = FALSE;	//Ca passe pas...
			}

		break;
		case STRAT_ADVERSARIES_POSITION:
			*bUART_filter = FALSE;	//Ca passe pas... 				(mieux vaut carrément afficher 	ponctuellement les infos qui découlent de ce message)
			*bSAVE_filter = FALSE;	//Pas d'enregistrement non plus	(mieux vaut carrément sauver 	ponctuellement les infos qui découlent de ce message)
			break;
		case BEACON_ADVERSARY_POSITION_IR:
			if(global.env.absolute_time-1000>filter_beacon_ir) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
				filter_beacon_ir=global.env.absolute_time;
			else
				*bUART_filter = FALSE;	//Ca passe pas...
		break;
		case BEACON_ADVERSARY_POSITION_US:
			if(global.env.absolute_time-1000>filter_beacon_us) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
				filter_beacon_us=global.env.absolute_time;
			else
				*bUART_filter = FALSE;	//Ca passe pas...
		break;
		default:
			//Message autorisé.
		break;
	}

	if	( ((msg->sid & 0xF00) == STRAT_FILTER) ||  ((msg->sid & 0xF00) == XBEE_FILTER) )
		*bCAN_filter = FALSE;	//On n'envoie pas sur le bus CAN des messages qui nous sont destinés uniquement.
	else
		*bCAN_filter = TRUE;	//Seuls les messages BROADCAST, DEBUG, ou destinés aux cartes PROPULSION, ACTIONNEUR, BALISES sont transmis sur le bus can.
}


void ENV_process_can_msg(CAN_msg_t * incoming_msg, bool_e bCAN, bool_e bU1, bool_e bU2, bool_e bXBee)
{
	bool_e bUART_filter = FALSE;	//bUART_filter indique si ce message est filtré ou s'il doit être propagé sur les uarts...
	bool_e bCAN_filter = FALSE;		//bCAN_filter indique si ce message doit être propagé sur le CAN...où s'il nous était directement destiné...
	bool_e bSAVE_filter = FALSE;	//bEEPROM_filter indique si ce message doit être enregistré en EEPROM

	CAN_update(incoming_msg);
	ENV_check_filter(incoming_msg, &bUART_filter, &bCAN_filter, &bSAVE_filter);

	//Enregistrement du message CAN.
	BUFFER_add(incoming_msg);						//BUFFERISATION

	if(bSAVE_filter && SWITCH_SAVE)
	{
		#ifdef EEPROM_CAN_MSG_ENABLE
			EEPROM_CAN_MSG_process_msg(incoming_msg);
		#endif
		#ifdef SD_ENABLE
			source_e source;
			if(bCAN == FALSE)
				source = FROM_BUS_CAN;
			else if(bU1 == FALSE)
				source = FROM_UART1;
			else if(!SWITCH_XBEE)
				source = FROM_UART2;
			else
				source = FROM_XBEE;
			SD_new_event(source, incoming_msg, NULL, TRUE);
		#endif
	}

	//Propagation du message CAN.
	if(bCAN && bCAN_filter)
		CAN_send(incoming_msg);

	if(bUART_filter)
	{
		#ifdef USE_XBEE
			if(bXBee && SWITCH_XBEE)
			{
				if((incoming_msg->sid & 0xF00) == XBEE_FILTER)
					CANMsgToXbee(incoming_msg,TRUE);	//Envoi en BROADCAST... aux modules joignables
			}
		#endif
		if(SWITCH_DEBUG && bU1)
		{
			if(SWITCH_VERBOSE)
				VERBOSE_CAN_MSG_print(incoming_msg);
			else
				CANmsgToU1tx(incoming_msg);
		}

		if(SWITCH_DEBUG  && !SWITCH_XBEE && bU2)
		{
			//Désactivé parce qu'on en a pas besoin...
			//CANmsgToU2tx(incoming_msg);
		}
	}
}

//Traite les messages que nous venons juste d'envoyer sur le bus can (via un CAN_send)
void ENV_process_can_msg_sent(CAN_msg_t * sent_msg)
{

	BUFFER_add(sent_msg);	//BUFFERISATION
	if(SWITCH_SAVE)			//Enregistrement du message CAN.
	{
		#ifdef EEPROM_CAN_MSG_ENABLE
			EEPROM_CAN_MSG_process_msg(sent_msg);
		#endif
		#ifdef SD_ENABLE
			SD_new_event(TO_BUSCAN, sent_msg, NULL, TRUE);
		#endif
	}

	//UART1
	if(SWITCH_DEBUG)
	{
		if(SWITCH_VERBOSE)
			VERBOSE_CAN_MSG_print(sent_msg);
		else
			CANmsgToU1tx(sent_msg);
	}

	//UART2 - désactivé volontairement -> parce qu'on en a pas besoin...
	//if(SWITCH_DEBUG  && !SWITCH_XBEE)
	//	CANmsgToU2tx(sent_msg);

}



void ENV_update()
{
	static Uint8 u1rx_receiving_can_msg = 0;
	CAN_msg_t incoming_msg_from_bus_can;
	static CAN_msg_t can_msg_from_uart1;
	static CAN_msg_t can_msg_from_uart2;
	char c;

	/* RAZ des drapeaux temporaires pour la prochaine itération */
	ENV_clean();
	//ENV_pos_update((CAN_msg_t *)NULL, FALSE);	//On récupère l'éventuel position sauvée en IT...

	/* Récuperation de l'évolution de l'environnement
	renseignee par les messages CAN */
	while (CAN_data_ready())
	{
		LED_CAN=!LED_CAN;
		incoming_msg_from_bus_can = CAN_get_next_msg();
		ENV_process_can_msg(&incoming_msg_from_bus_can,FALSE, TRUE, TRUE, TRUE);	//Everywhere except CAN
	}


	while(UART1_data_ready())
	{
		c = UART1_get_next_msg();
		if(u1rx_receiving_can_msg == 0 && c == SOH)	//Début d'un message CAN détecté : on dirige les 13 prochains octets vers u1rxToCANmsg
			u1rx_receiving_can_msg = CAN_MSG_LENGTH + 2;	//SOH, EOT

		if(u1rx_receiving_can_msg)
		{
			u1rx_receiving_can_msg--;
			if(u1rxToCANmsg(&can_msg_from_uart1,c))
			{
				ENV_process_can_msg(&can_msg_from_uart1,TRUE, FALSE, TRUE, TRUE);	//Everywhere except U1.
				u1rx_receiving_can_msg = 0;	//où que soit notre compteur, on le remet à 0.
			}
		}
		else
			SD_char_from_user(c);

	}


#ifdef USE_XBEE
	if(SWITCH_XBEE)
	{
		if(XBeeToCANmsg(&can_msg_from_uart2))
			ENV_process_can_msg(&can_msg_from_uart2,TRUE, TRUE, FALSE, FALSE);	//Everywhere except U2 and XBee.
	}
#endif


	/* Récupération des données des télémètres*/
	#ifdef USE_TELEMETER
		TELEMETER_update();
	#endif

	/* Récupération des données des boutons */
	BUTTON_update();

	/* Mise à jour de l'info de position de l'adversaire */
	/* à faire après récupération des infos des capteurs */
	DETECTION_update();


	/* Traitement des données des capteurs Sick (télémètres LASER)
	obtenues lors de la réception d'un message CAN d'avancement/de rotation de la propulsion */
	#ifdef USE_SICK
		SICK_update();
	#endif
}

/* met à jour l'environnement en fonction du message CAN reçu */
void CAN_update (CAN_msg_t* incoming_msg)
{
	date_t date;
//****************************** Messages carte supervision *************************/
	switch (incoming_msg->sid)
	{
		case BROADCAST_POSITION_ROBOT:	   //Les raisons seront ensuite traitees dans la tache de fond
		case CARTE_P_ROBOT_FREINE:
		case CARTE_P_ROBOT_CALIBRE:
		case CARTE_P_ASSER_ERREUR:
		case CARTE_P_TRAJ_FINIE:
			ENV_pos_update(incoming_msg);	//Tout ces messages contiennent une position... et d'autres infos communes
		break;
		default:
		break;
	}

	switch (incoming_msg->sid)
	{
//****************************** Messages venant des geeks du club robot  *************************/
		case DEBUG_RTC_SET:
			date.seconds 	= incoming_msg->data[0];
			date.minutes 	= incoming_msg->data[1];
			date.hours 		= incoming_msg->data[2];
			date.day 		= incoming_msg->data[3];
			date.date 		= incoming_msg->data[4];
			date.month 		= incoming_msg->data[5];
			date.year 		= incoming_msg->data[6];	//13 pour 2013...
			RTC_set_time(&date);
			RTC_print_time();
			RTC_can_send();	//Retour ... pour vérifier que ca a fonctionné..
			break;
		case DEBUG_RTC_GET:
			RTC_print_time();
			RTC_can_send();
			break;
		case DEBUG_RTC_TIME:
			RTC_print_time();
			break;
		case DEBUG_SELFTEST_LAUNCH:
			SELFTEST_ask_launch();
			break;
//****************************** Messages carte propulsion/asser *************************/
		case CARTE_P_TRAJ_FINIE:
			global.env.asser.fini = TRUE;
			break;
		case CARTE_P_ASSER_ERREUR:

			global.env.asser.erreur = TRUE;
			global.env.asser.vitesse_translation_erreur =
				((Sint32)U16FROMU8(incoming_msg->data[1],incoming_msg->data[0]) << 16)
					+ U16FROMU8(incoming_msg->data[3],incoming_msg->data[2]);
			global.env.asser.vitesse_rotation_erreur =
				((Sint32)U16FROMU8(incoming_msg->data[5],incoming_msg->data[4]) << 16)
					+ U16FROMU8(incoming_msg->data[7],incoming_msg->data[6]);
			break;
		case CARTE_P_ROBOT_CALIBRE:
			global.env.asser.calibrated = TRUE;

			break;
		case BROADCAST_POSITION_ROBOT:
			//Remarque : Si USE_SICK... si un broadcast_position est arrivé avec pour seule(s) raison(s) WARNING_ROTATION et WARNING_TRANSLATION, il est traité dès réception par CAN_fast_update, et n'arrive pas ici !

			//ATTENTION : Pas de switch car les raisons peuvent être cumulées !!!
			//Les raisons WARNING_TRANSLATION, WARNING_ROTATION, WARNING_NO et WARNING_TIMER ne font rien d'autres que déclencher un ENV_pos_update();

			if(incoming_msg->data[6] & WARNING_REACH_X)		//Nous venons d'atteindre une position en X pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.asser.reach_x = TRUE;
				debug_printf("Rx\n");
			}

			if(incoming_msg->data[6] & WARNING_REACH_Y)	//Nous venons d'atteindre une position en Y pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.asser.reach_y = TRUE;
				debug_printf("Ry\n");
			}
			if(incoming_msg->data[6] & WARNING_REACH_TETA)	//Nous venons d'atteindre une position en Teta pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.asser.reach_teta = TRUE;
				debug_printf("Rt\n");
			}

			break;
		case CARTE_P_ROBOT_FREINE:
			global.env.asser.freine = TRUE;

			break;
//****************************** Messages de la carte actionneur *************************/
		case ACT_RESULT:
			ACT_process_result(incoming_msg);
			break;

		case ACT_BALLSORTER_RESULT:
			global.env.color_ball = incoming_msg->data[0];
			break;

/************************************ Récupération des données de la balise *******************************/
		case BEACON_ADVERSARY_POSITION_IR:
			//En absence d'hokuyo et du fonctionnement correct de la carte propulsion, les msg balises IR sont très important pour l'évitement.
			ENV_pos_foe_update(incoming_msg);
			SELFTEST_update_led_beacon(incoming_msg);
			break;
		case BEACON_ADVERSARY_POSITION_US:
			//Pour l'Histoire... (avec un grand H)
			//ENV_pos_foe_update(incoming_msg);
			//SELFTEST_update_led_beacon(incoming_msg);
			break;
		case STRAT_ADVERSARIES_POSITION:
			ENV_pos_foe_update(incoming_msg);
			break;
/************************************* Récupération des envois de l'autre robot ***************************/
		case XBEE_START_MATCH:
			global.env.ask_start = TRUE;
			break;
		case XBEE_PING:
			//On recoit un ping, on répond par un PONG.
			//Le lien est établi
			//Le module QS_CanoverXBee se débrouille pour PONGer
			break;
		case XBEE_PONG:
			//On reçoit un pong, tant mieux, le lien est établi

			break;
		case DEBUG_DETECT_FOE:
			global.env.debug_force_foe = TRUE;
			break;

		case XBEE_ZONE_COMMAND:
			ZONE_CAN_process_msg(incoming_msg);
			break;
/************************************* Récupération des messages liés au selftest ***************************/
		case STRAT_BEACON_IR_SELFTEST_DONE :
		case STRAT_BEACON_US_SELFTEST_DONE :
		case STRAT_ACT_SELFTEST_DONE :
		case STRAT_PROP_SELFTEST_DONE :
		case STRAT_ACT_PONG:
		case STRAT_PROP_PONG:
		case STRAT_BEACON_PONG:
			SELFTEST_update(incoming_msg);
			break;
		default:
			break;
	}

/************************************* Envoi du message pour stockage pour affichage LCD ********************/
	LCD_incoming_can(incoming_msg);
	return;
}

/* Cette fonction permet d'éxécuter un traitement rapide dans l'interruption suivant
   le message CAN reçu et renvoie si le message doit être placé dans le buffer */
bool_e CAN_fast_update(CAN_msg_t* msg)
{
	//Samuel : TODO cette fonction peut génèrer des situations foireuses où la position est mise à jour en pleine mauvaise préemption...
		switch (msg->sid)
		{
			case BROADCAST_POSITION_ROBOT:	   //Les raisons seront ensuite traitees dans la tache de fond
			case CARTE_P_ROBOT_FREINE:
			case CARTE_P_ROBOT_CALIBRE:
			case CARTE_P_ASSER_ERREUR:
			case CARTE_P_TRAJ_FINIE:
				//ENV_pos_fast_update(msg, TRUE);
				#ifdef USE_SICK
					SICK_update_points();
				#endif
			break;
			default:
				return TRUE;
		}

		return TRUE;

}
//#include <can.h>

/* met a jour la position a partir d'un message asser la délivrant */
void ENV_fast_pos_update (CAN_msg_t* msg, bool_e on_it)
{
	volatile static bool_e new_pos_available = FALSE;
	static position_t new_pos;
	static time32_t new_time;
	static way_e new_way;
	static trajectory_e new_trajectory;
	static SUPERVISOR_error_source_e new_status;

	if(on_it == TRUE)
	{
		new_pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
		new_pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
		new_pos.translation_speed = ((Uint16)(msg->data[0] >> 5))*250;	// [mm/sec]
		new_pos.rotation_speed =	((Uint16)(msg->data[2] >> 5));		// [rad/sec]
		new_pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
		new_pos.cosAngle = cos4096(global.env.pos.angle);
		new_pos.sinAngle = sin4096(global.env.pos.angle);
		new_time = global.env.match_time;
		new_way = (way_e)((msg->data[7] >> 3) & 0x03);
		new_status = (SUPERVISOR_error_source_e)((msg->data[7]) & 0x07);
		new_trajectory = (trajectory_e)((msg->data[7] >> 5) & 0x07);
			/*msg->data[7] : 8 bits  : T T T W W E E E
				TTT : trajectoire actuelle
					TRAJECTORY_TRANSLATION		= 0,
					TRAJECTORY_ROTATION			= 1,
					TRAJECTORY_STOP				= 2,
					TRAJECTORY_AUTOMATIC_CURVE	= 3,
					TRAJECTORY_NONE				= 4
				 WW : Way, sens actuel
					ANY_WAY						= 0,
					BACKWARD					= 1,
					FORWARD						= 2,
				 EEE : Erreur
					SUPERVISOR_INIT				= 0,
					SUPERVISOR_IDLE				= 1,		//Rien à faire
					SUPERVISOR_TRAJECTORY		= 2,		//Trajectoire en cours
					SUPERVISOR_ERROR			= 3,		//Carte en erreur - attente d'un nouvel ordre pour en sortir
					SUPERVISOR_MATCH_ENDED		= 4			//Match terminé
				*/
		new_pos_available = TRUE;
	}
	else		//En tache de fond.
	{
		//save_int = _C1IE;
		//if(save_int == 1)
		//	_C1IE = 0;
		if(new_pos_available)
		{
			new_pos_available = FALSE;
			global.env.asser.last_time_pos_updated = new_time;
			global.env.asser.current_way = new_way;
			global.env.asser.current_status = new_status;
			global.env.asser.is_in_translation = (new_trajectory >> 2) & 1;
			global.env.asser.is_in_rotation = (new_trajectory >> 1) & 1;
			global.env.pos = new_pos;
			//debug_printf("\n Pos update :  (%lf : %lf)\n",global.env.pos.cosAngle,global.env.pos.sinAngle);
			global.env.pos.updated = TRUE;
		}
		//if(save_int == 1)
		//	_C1IE = 1;

	}

}

/* mise à jour de la position reçue dans l'un des messages de la propulsion.*/
void ENV_pos_update (CAN_msg_t* msg)
{
	global.env.pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
	global.env.pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
	global.env.pos.translation_speed = ((Uint16)(msg->data[0] >> 5))*250;	// [mm/sec]
	global.env.pos.rotation_speed =	((Uint16)(msg->data[2] >> 5));		// [rad/sec]
	global.env.pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
	global.env.pos.cosAngle = cos4096(global.env.pos.angle);
	global.env.pos.sinAngle = sin4096(global.env.pos.angle);
	global.env.asser.last_time_pos_updated = global.env.match_time;
	global.env.asser.current_way = (way_e)((msg->data[7] >> 3) & 0x03);
	global.env.asser.current_status = (SUPERVISOR_error_source_e)((msg->data[7]) & 0x07);
	global.env.asser.is_in_translation = (((msg->data[7] >> 5) & 0x07) >> 2) & 1;
	global.env.asser.is_in_rotation = (((msg->data[7] >> 5) & 0x07) >> 1) & 1;

	global.env.pos.updated = TRUE;
			/*msg->data[7] : 8 bits  : T R x W W E E E
				 T : TRUE si robot en translation
				 R : TRUE si robot en rotation
				 x : non utilisé
				 WW : Way, sens actuel
					ANY_WAY						= 0,
					BACKWARD					= 1,
					FORWARD						= 2,
				 EEE : Erreur
					SUPERVISOR_INIT				= 0,
					SUPERVISOR_IDLE				= 1,		//Rien à faire
					SUPERVISOR_TRAJECTORY		= 2,		//Trajectoire en cours
					SUPERVISOR_ERROR			= 3,		//Carte en erreur - attente d'un nouvel ordre pour en sortir
					SUPERVISOR_MATCH_ENDED		= 4			//Match terminé
				*/
}

/* met a jour la position de l'adversaire à partir des balises */
void ENV_pos_foe_update (CAN_msg_t* msg)
{
	bool_e slashn;
	Uint8 fiability;
	Uint8 adversary_nb;
	if(msg->sid == STRAT_ADVERSARIES_POSITION)
	{
		adversary_nb = msg->data[0];
		if(adversary_nb < NB_FOES)
		{
			fiability = msg->data[6];
			if(fiability)
			{
				global.env.foe[adversary_nb].updated = TRUE;
				global.env.foe[adversary_nb].update_time = global.env.match_time;
			}
			if(fiability & ADVERSARY_DETECTION_FIABILITY_X)
				global.env.foe[adversary_nb].x = ((Sint16)msg->data[1])*20;
			if(fiability & ADVERSARY_DETECTION_FIABILITY_Y)
				global.env.foe[adversary_nb].y = ((Sint16)msg->data[2])*20;
			if(fiability & ADVERSARY_DETECTION_FIABILITY_TETA)
				global.env.foe[adversary_nb].angle = (Sint16)(U16FROMU8(msg->data[3],msg->data[4]));
			else	//je dois calculer moi-même l'angle de vue relatif de l'adversaire
				global.env.foe[adversary_nb].angle = GEOMETRY_viewing_angle(global.env.pos.x, global.env.pos.y,global.env.foe[adversary_nb].x, global.env.foe[adversary_nb].y);
			if(fiability & ADVERSARY_DETECTION_FIABILITY_DISTANCE)
				global.env.foe[adversary_nb].dist = ((Sint16)msg->data[5])*20;
			else	//je dois calculer moi-même la distance de l'adversaire
				global.env.foe[adversary_nb].dist = GEOMETRY_distance(	(GEOMETRY_point_t){global.env.pos.x, global.env.pos.y},
																		(GEOMETRY_point_t){global.env.foe[adversary_nb].x, global.env.foe[adversary_nb].y}
																		);
		}
	}
	if(msg->sid==BEACON_ADVERSARY_POSITION_IR)
	{
		slashn = FALSE;
		if((msg->data[0] & 0xFE) == AUCUNE_ERREUR)	//Si l'octet de fiabilité vaut SIGNAL_INSUFFISANT, on le laisse passer quand même
		{
			//slashn = TRUE;
			global.env.sensor[BEACON_IR_FOE_1].angle = U16FROMU8(msg->data[1],msg->data[2]);
			/* Pour gérer l'inversion de la balise */
			//global.env.sensor[BEACON_IR_FOE_1].angle += (global.env.sensor[BEACON_IR_FOE_1].angle > 0)?-PI4096:PI4096;
			global.env.sensor[BEACON_IR_FOE_1].distance = (Uint16)(msg->data[3])*10;
			global.env.sensor[BEACON_IR_FOE_1].update_time = global.env.match_time;
			global.env.sensor[BEACON_IR_FOE_1].updated = TRUE;
			//debug_printf("IR1=%dmm", global.env.sensor[BEACON_IR_FOE_1].distance);
			//debug_printf("|%d", ((Sint16)((((Sint32)(global.env.sensor[BEACON_IR_FOE_1].angle))*180/PI4096))));
		} //else debug_printf("NO IR 1 err %d!\n", msg->data[0]);
		if((msg->data[4] & 0xFE) == AUCUNE_ERREUR)
		{
			//slashn = TRUE;
			global.env.sensor[BEACON_IR_FOE_2].angle = (Sint16)(U16FROMU8(msg->data[5],msg->data[6]));
			/* Pour gérer l'inversion de la balise */
			//global.env.sensor[BEACON_IR_FOE_2].angle += (global.env.sensor[BEACON_IR_FOE_2].angle > 0)?-PI4096:PI4096;
			global.env.sensor[BEACON_IR_FOE_2].distance = (Uint16)(msg->data[7])*10;
			global.env.sensor[BEACON_IR_FOE_2].update_time = global.env.match_time;
			global.env.sensor[BEACON_IR_FOE_2].updated = TRUE;
			//debug_printf(" IR2=%dmm", global.env.sensor[BEACON_IR_FOE_2].distance);
			//debug_printf("|%d", ((Sint16)((((Sint32)(global.env.sensor[BEACON_IR_FOE_2].angle))*180/PI4096))));
		} //else debug_printf("NO IR 2 err %d!\n", msg->data[4]);
		if(slashn)
			debug_printf("\n");
	}
}


/* baisse les drapeaux d'environnement pour préparer la prochaine MaJ */
void ENV_clean ()
{
	STACKS_clear_timeouts();
	DETECTION_clear_updates();
	if(global.env.color == global.env.wanted_color)
		global.env.color_updated = FALSE;
	global.env.config_updated = FALSE;
	global.env.asser.fini = FALSE;
	global.env.asser.erreur = FALSE;
	global.env.asser.freine = FALSE;
	global.env.asser.reach_x = FALSE;
	global.env.asser.reach_y = FALSE;
	global.env.asser.reach_teta = FALSE;
		//global.env.asser.last_time_pos_updated = 0;
	global.env.pos.updated = FALSE;
	global.env.foe[FOE_1].updated = FALSE;
	global.env.foe[FOE_2].updated = FALSE;
	global.env.ask_asser_calibration = FALSE;
	global.env.debug_force_foe = FALSE;

	/*global.env.act[BROOM_LEFT].ready == TRUE;
	global.env.act[BROOM_RIGHT].ready == TRUE;
	global.env.act[F].ready == TRUE;
	global.env.act[BROOM_LEFT].closed == TRUE;
	global.env.act[BROOM_RIGHT].closed == TRUE;
	global.env.act[F].closed == TRUE;
	global.env.act[BROOM_LEFT].opened == TRUE;
	global.env.act[BROOM_RIGHT].opened == TRUE;
	global.env.act[F].opened == TRUE;
	global.env.act[BROOM_LEFT].failure == TRUE;
	global.env.act[BROOM_RIGHT].failure == TRUE;
	global.env.act[F].failure == TRUE;*/
}


/* initialise les variables d'environnement */
void ENV_init()
{
	CAN_init();
	CAN_set_send_callback(ENV_process_can_msg_sent);
	BUTTON_init();
	//    CAN_set_direct_treatment_function(CAN_fast_update);

	#ifdef USE_SICK
		SICK_init();
	#endif
	#ifdef USE_TELEMETER
		TELEMETER_init();
	#endif
	DETECTION_init();

	ENV_clean();
	global.env.config=(config_t){/*strategie*/0, /*evitement*/TRUE, /*balise*/TRUE};
	global.env.wanted_color=BLUE;
	global.env.color = COLOR_INIT_VALUE; //update -> color = wanted + dispatch
	global.env.color_updated = TRUE;
	global.env.match_started = FALSE;
	global.env.match_over = FALSE;
	global.env.foe[FOE_1].update_time = 0;
	global.env.foe[FOE_2].update_time = 0;
	global.env.match_time = 0;
	global.env.pos.dist = 0;
	global.env.ask_start = FALSE;
	global.env.asser.calibrated = FALSE;

	global.env.asser.current_way = ANY_WAY;
	global.env.asser.is_in_translation = FALSE;
	global.env.asser.is_in_rotation = FALSE;
	global.env.asser.current_status = NO_ERROR;

	global.env.color_ball = ACT_BALLSORTER_NO_CHERRY;

	//Initialisation des elemnts du terrain
	int i;
	//Init cadeaux
	for(i=GOAL_Cadeau0; i <= GOAL_Cadeau3; i++){
		global.env.map_elements[i] = ELEMENT_TODO;
	}
	//Init Verre + VerreEnnemi + Bougies
	for(i=GOAL_Verres0; i <= GOAL_Etage2Bougie11; i++){
		global.env.map_elements[i] = ELEMENT_TODO;
	}
	//Attention la suite n'est pas definitive et necessitera un changement en fonction des positions de calage
//	global.env.map_elements[4] = ELEMENT_NONE;
//	global.env.map_elements[5] = ELEMENT_NONE;
//	global.env.map_elements[6] = ELEMENT_TODO;
//	global.env.map_elements[7] = ELEMENT_TODO;
//	global.env.map_elements[8] = ELEMENT_TODO;
	for(i = GOAL_Assiette0; i <= GOAL_Assiette4; i++) {
		if(i != GOAL_Assiette0 && i != GOAL_Assiette2)
			global.env.map_elements[i] = ELEMENT_TODO;
		else global.env.map_elements[i] = ELEMENT_NONE;
	}
	for(i = GOAL_AssietteEnnemi0; i <= GOAL_AssietteEnnemi4; i++){
		global.env.map_elements[i] = ELEMENT_NONE;
	}
}

/* envoie un message CAN BROADCAST_COULEUR à jour */
void ENV_set_color(color_e color)
{
	/* changer la couleur */
	global.env.color = color;

	/* indiquer au monde la nouvelle couleur */
	CAN_msg_t msg;
	msg.sid=BROADCAST_COULEUR;
	msg.data[0]=global.env.color;
	msg.size=1;
	CAN_send(&msg);
}






bool_e ENV_game_zone_filter(Sint16 x, Sint16 y, Uint16 delta)
{
	/* Délimitation du terrain */
	if(x < delta || y < delta || x > GAME_ZONE_SIZE_X - delta || y > GAME_ZONE_SIZE_Y - delta
	//|| (x > 875 - delta && x < 1125 + delta  && y > 975 - delta && y < 2025 + delta) // Pour supprimer la zone centrale (totem + palmier)
	|| (x > 1250 - delta && (y < 340 + delta || y > 2660 - delta))) //Pour supprimer les cales
	{
		return FALSE;
	}
	return TRUE;
}
