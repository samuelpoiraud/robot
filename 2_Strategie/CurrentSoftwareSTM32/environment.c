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
#include "strats_2015/actions_both_2015.h"
#include "act_functions.h"
#include "zone_mutex.h"
#include "fix_beacon.h"
#include "Supervision/Buffer.h"
#include "Supervision/RTC.h"
#include "Supervision/Selftest.h"
#include "Supervision/Supervision.h"
#include "Supervision/Buzzer.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_can_over_xbee.h"
#include "Supervision/SD/term_io.h"
#include "Supervision/SD/SD.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_adc.h"
#include "QS/QS_uart.h"
#include "QS/QS_maths.h"
#include "QS/QS_can_verbose.h"
#include "button.h"
#include "elements.h"
#include "QS/QS_IHM.h"
#include "Supervision/LCD_interface.h"
#include "QS/QS_ports.h"

/* met à jour l'environnement en fonction du message CAN reçu */
void CAN_update (CAN_msg_t* incoming_msg);

/* met a jour la position a partir d'un message asser la délivrant */
void ENV_pos_update (CAN_msg_t* msg);

void ENV_process_can_msg_sent(CAN_msg_t * sent_msg);

void ENV_clean (void);

/* Regarde si les switchs à risque sont activés et mets des avertissements (LED,LCD) */
void ENV_warning_switch();

#define ADC_THRESHOLD 10 //Valeur de l'ADC sans dispositif de connecté

/* initialise les variables d'environnement */
void ENV_init(void)
{
	Uint8 i;
	CAN_init();
	CAN_set_send_callback(ENV_process_can_msg_sent);
	BUTTON_init();
	DETECTION_init();

	ENV_clean();
	global.env.wanted_color=COLOR_INIT_VALUE;
	global.env.color = COLOR_INIT_VALUE; //update -> color = wanted + dispatch
	global.env.color_updated = TRUE;
	global.env.match_started = FALSE;
	global.env.match_over = FALSE;
	for(i=0;i<MAX_NB_FOES;i++)
	{
		global.env.foe[i].fiability_error = 0;
		global.env.foe[i].enable = FALSE;
		global.env.foe[i].update_time = 0;
	}
	global.env.foes_updated_for_lcd = FALSE;
	global.env.match_time = 0;
	global.env.pos.dist = 0;
	global.env.ask_start = FALSE;
	global.env.prop.calibrated = FALSE;
	global.env.prop.current_way = ANY_WAY;
	global.env.prop.is_in_translation = FALSE;
	global.env.prop.is_in_rotation = FALSE;
	global.env.prop.current_status = NO_ERROR;
	global.env.alim = FALSE;
	global.env.alim_value = 0;
	global.env.destination = (GEOMETRY_point_t){0,0};
	for(i=0;i<PROPULSION_NUMBER_COEFS;i++)
		global.env.propulsion_coefs[i] = 0;
	global.env.com.reach_point_get_out_init = FALSE;
	global.env.initial_position_received = FALSE;

	FIX_BEACON_init();
}



void ENV_check_filter(CAN_msg_t * msg, bool_e * bUART_filter, bool_e * bCAN_filter, bool_e * bSAVE_filter)
 {
	//static time32_t filter_beacon_ir = 0;
	static time32_t filter_broadcast_position = 0;

	*bUART_filter = TRUE;	//On suppose que le message est autorisé.
	*bSAVE_filter = TRUE;

	switch(msg->sid)
	{
		//FILTRAGE POUR NE PAS ETRE SPAMMES PAR LE MESSAGE DE POSITION_ROBOT....
		case BROADCAST_POSITION_ROBOT:
			//si le message est porteur d'un warning, on ne le filtre pas.
			if((msg->data[6] & (WARNING_TRANSLATION | WARNING_ROTATION | WARNING_TIMER)) || !msg->data[0])	//Si le message ne porte pas de warning : on filtre.
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
		case BROADCAST_ADVERSARIES_POSITION:
			*bUART_filter = FALSE;	//Ca passe pas... 				(mieux vaut carrément afficher 	ponctuellement les infos qui découlent de ce message)
			*bSAVE_filter = FALSE;	//Pas d'enregistrement non plus	(mieux vaut carrément sauver 	ponctuellement les infos qui découlent de ce message)
			break;
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			//if(global.env.absolute_time-1000>filter_beacon_ir) //global.compteur_de_secondes est incrémenté toutes les 250 ms ...
			//	filter_beacon_ir=global.env.absolute_time;
			//else
			*bUART_filter = FALSE;	//Ca passe pas...
			*bSAVE_filter = FALSE;
			break;
		case DEBUG_AVOIDANCE_POLY:
			*bUART_filter = FALSE;	//Ca passe pas...
			*bSAVE_filter = FALSE;
			break;
		default:
			//Message autorisé.
			break;
	}

	if	( ((msg->sid & 0xF00) == STRAT_FILTER) ||  ((msg->sid & 0xF00) == XBEE_FILTER) || ((msg->sid & 0xF00) == BROADCAST_FILTER))
		*bCAN_filter = FALSE;	//On n'envoie pas sur le bus CAN des messages qui nous sont destinés uniquement.
	else
		*bCAN_filter = TRUE;	//Seuls les messages DEBUG, ou destinés aux cartes PROPULSION, ACTIONNEUR, BALISES, IHM sont transmis sur le bus can.
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

	if(bSAVE_filter)// && IHM_switchs_get(SWITCH_SAVE))
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
			else if(!IHM_switchs_get(SWITCH_XBEE))
				source = FROM_UART2;
			else
				source = FROM_XBEE;
			SD_new_event(source, incoming_msg, NULL, TRUE);
		#endif
	}

	//Propagation du message CAN.
	#ifndef MODE_SIMULATION
		if(bCAN && bCAN_filter)
			CAN_send(incoming_msg);
	#endif

	if(bUART_filter)
	{
		#ifdef USE_XBEE
			if(bXBee && IHM_switchs_get(SWITCH_XBEE))
			{
				if((incoming_msg->sid & 0xF00) == XBEE_FILTER)
					CANMsgToXbee(incoming_msg,TRUE);	//Envoi en BROADCAST... aux modules joignables
			}
		#endif
		if(IHM_switchs_get(SWITCH_DEBUG) && bU1)
		{
			if(IHM_switchs_get(SWITCH_VERBOSE))
				QS_CAN_VERBOSE_can_msg_print(incoming_msg, VERB_INPUT_MSG);
			else
			{
				#ifndef MODE_SIMULATION	//Pour ne pas spammer l'UART en mode simulation... On ne doit y voir que les messages CAN réellement envoyés.
					CANmsgToU1tx(incoming_msg);
				#endif
			}
		}

		if(IHM_switchs_get(SWITCH_DEBUG)  && !IHM_switchs_get(SWITCH_XBEE) && bU2)
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
	//if(IHM_switchs_get(SWITCH_SAVE))			//Enregistrement du message CAN.
	//{
		#ifdef SD_ENABLE
			SD_new_event(TO_BUSCAN, sent_msg, NULL, TRUE);
		#endif
	//}

	//UART1
	if(IHM_switchs_get(SWITCH_DEBUG))
	{
		if(IHM_switchs_get(SWITCH_VERBOSE)){
			QS_CAN_VERBOSE_can_msg_print(sent_msg, VERB_OUTPUT_MSG);
		}else
			CANmsgToU1tx(sent_msg);
	}

	//UART2 - désactivé volontairement -> parce qu'on en a pas besoin...
	//if(IHM_switchs_get(SWITCH_DEBUG)  && !IHM_switchs_get(SWITCH_XBEE))
	//	CANmsgToU2tx(sent_msg);

	//Messages de BROADCAST transmis aussi à la balise mère.
	switch(sent_msg->sid)
	{
		case BROADCAST_START:
			//no break;
		case BROADCAST_STOP_ALL:
			//no break;
		case BROADCAST_COULEUR:
			CANMsgToXBeeDestination(sent_msg, BALISE_MERE);
			break;
		default:
			break;
	}
}



void ENV_update(void)
{
	static Uint8 u1rx_receiving_can_msg = 0;
	CAN_msg_t incoming_msg_from_bus_can;
	static CAN_msg_t can_msg_from_uart1;
	static CAN_msg_t can_msg_from_uart2;
	static time32_t last_time_tell_position = 0;
	char c;

	// RAZ des drapeaux temporaires pour la prochaine itération
	ENV_clean();

	if(global.env.initial_position_received == FALSE && last_time_tell_position != 0 && global.env.absolute_time - last_time_tell_position > 500){
		CAN_send_sid(PROP_TELL_POSITION);
		last_time_tell_position = global.env.absolute_time;
	}

	// Récuperation de l'évolution de l'environnement renseignee par les messages CAN
	while (CAN_data_ready())
	{
		toggle_led(LED_CAN);
		incoming_msg_from_bus_can = CAN_get_next_msg();
		ENV_process_can_msg(&incoming_msg_from_bus_can,FALSE, TRUE, TRUE, FALSE);	//Everywhere except CAN
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
			char_from_user(c);

	}


#ifdef USE_XBEE
	if(IHM_switchs_get(SWITCH_XBEE))
	{
		if(XBeeToCANmsg(&can_msg_from_uart2))
			ENV_process_can_msg(&can_msg_from_uart2,TRUE, TRUE, FALSE, FALSE);	//Everywhere except U2 and XBee.
	}
#endif

#ifndef FDP_2014
	ENV_warning_switch();
#endif

	/* Récupération des données des boutons */
	BUTTON_update();
}

/* met à jour l'environnement en fonction du message CAN reçu */
void CAN_update (CAN_msg_t* incoming_msg)
{
	date_t date;
//****************************** Messages carte supervision *************************/
	switch (incoming_msg->sid)
	{
		case BROADCAST_POSITION_ROBOT:	   //Les raisons seront ensuite traitees dans la tache de fond
		case STRAT_ROBOT_FREINE:
		case PROP_ROBOT_CALIBRE:
		case STRAT_PROP_ERREUR:
		case STRAT_TRAJ_FINIE:
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
		case STRAT_BUZZER_PLAY:
			BUZZER_play(U16FROMU8(incoming_msg->data[0], incoming_msg->data[1]),
						BUZZER_convert_enum_QS(incoming_msg->data[2]),
						incoming_msg->data[3]);
			break;
//****************************** Messages carte propulsion/asser *************************/
		case STRAT_TRAJ_FINIE:
			global.env.prop.ended = TRUE;
			break;
		case STRAT_PROP_FOE_DETECTED:
			set_prop_detected_foe(incoming_msg);
			break;
		case STRAT_PROP_ERREUR:
			global.env.prop.erreur = TRUE;
			global.env.prop.vitesse_translation_erreur =
				((Sint32)U16FROMU8(incoming_msg->data[1],incoming_msg->data[0]) << 16)
					+ U16FROMU8(incoming_msg->data[3],incoming_msg->data[2]);
			global.env.prop.vitesse_rotation_erreur =
				((Sint32)U16FROMU8(incoming_msg->data[5],incoming_msg->data[4]) << 16)
					+ U16FROMU8(incoming_msg->data[7],incoming_msg->data[6]);
			break;
		case PROP_ROBOT_CALIBRE:
			global.env.prop.calibrated = TRUE;
			if(QS_WHO_AM_I_get() == SMALL_ROBOT)
			{
				CAN_msg_t msg;
				Sint16 teta;

				//TODO enrichir en fonction des différentes stratégies prévues...
				/*
				 * - se rendre au sud...
				 * - se rendre tout droit pour ramassage gobelet
				 * - viser le rush gobelet central (cette position est probablement différente pour gagner du temps ?)
				 * => autre avantage, ca nous permet de savoir ce que va faire le robot (contrôle supplémentaire de l'état des switchs)
				 */
				if(IHM_switchs_get(SWITCH_CUP_NORTH_OUR))
					teta = COLOR_ANGLE((-PI4096*68)/180);		  //On vise le gobelet nord
				else if(IHM_switchs_get(SWITCH_CUP_SOUTH_OUR))
					teta = 0;									 //On vise le gobelet sud
				else
					teta = COLOR_ANGLE(-PI4096/2);

				msg.sid = PROP_GO_ANGLE;
				msg.size = 8;
				msg.data[0] = NO_ACKNOWLEDGE | NOW | ABSOLUTE | NO_MULTIPOINT;	//Demande spécifique de NON Acquittement.
				msg.data[1] = HIGHINT(teta);	//teta high
				msg.data[2] = LOWINT(teta);		//teta low
				msg.data[3] = 0;				//RFU
				msg.data[4] = 0;				//RFU
				msg.data[5] = SLOW;				//
				msg.data[6] = ANY_WAY;			//
				msg.data[7] = 0;				//RFU
				CAN_send(&msg);

			}

			//position de départ 2014, Guy ne doit pas empêcher le passage de Pierre si jamais son début de match n'a pas été détecté... Pierre poussera ainsi Guy... autrement dit : Pierre qui roule n'amasse pas de Guy...
				//PROP_set_correctors(FALSE, FALSE);
			break;
		case DEBUG_PROPULSION_COEF_IS:
			if(incoming_msg->data[0] < PROPULSION_NUMBER_COEFS)
			{
				global.env.propulsion_coefs_updated |=  (Uint32)(1) << incoming_msg->data[0];
				global.env.propulsion_coefs[incoming_msg->data[0]] = (Sint32)(U32FROMU8(incoming_msg->data[1],incoming_msg->data[2],incoming_msg->data[3],incoming_msg->data[4]));
			}
			break;
		case BROADCAST_POSITION_ROBOT:
			//ATTENTION : Pas de switch car les raisons peuvent être cumulées !!!
			//Les raisons WARNING_TRANSLATION, WARNING_ROTATION, WARNING_NO et WARNING_TIMER ne font rien d'autres que déclencher un ENV_pos_update();

			if(incoming_msg->data[6] & WARNING_REACH_X)		//Nous venons d'atteindre une position en X pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.prop.reach_x = TRUE;
				debug_printf("Rx\n");
			}

			if(incoming_msg->data[6] & WARNING_REACH_Y)	//Nous venons d'atteindre une position en Y pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.prop.reach_y = TRUE;
				debug_printf("Ry\n");
			}
			if(incoming_msg->data[6] & WARNING_REACH_TETA)	//Nous venons d'atteindre une position en Teta pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.env.prop.reach_teta = TRUE;
				debug_printf("Rt\n");
			}

			break;
		case STRAT_ROBOT_FREINE:
			global.env.prop.freine = TRUE;

			break;
		case DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE:
			global.env.duration_trajectory_for_test_coefs = U16FROMU8(incoming_msg->data[0], incoming_msg->data[1]);
			break;
		case STRAT_SEND_REPORT:
			LCD_printf(1, TRUE, FALSE, "Dist:%d", U16FROMU8(incoming_msg->data[4], incoming_msg->data[5]) << 1);
			LCD_printf(2, TRUE, FALSE, "Rot :%4ld MRot:%4ld", ((Sint32)((Sint16)(U16FROMU8(incoming_msg->data[0], incoming_msg->data[1]))) << 3)*180/PI4096, ((Sint32)((Sint16)(U16FROMU8(incoming_msg->data[2], incoming_msg->data[3]))) << 3)*180/PI4096);
			break;
		case STRAT_CUP_POSITION:
			collect_cup_coord(incoming_msg);
			break;

//****************************** Messages de la carte actionneur *************************/
		case ACT_RESULT:
			ACT_process_result(incoming_msg);
			break;

		case STRAT_INFORM_CAPTEUR:
			ACT_sensor_answer(incoming_msg);
			break;

/************************************ Récupération des données de la balise *******************************/
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			//En absence d'hokuyo et du fonctionnement correct de la carte propulsion, les msg balises IR sont très important pour l'évitement.
			DETECTION_pos_foe_update(incoming_msg);
			SELFTEST_update_led_beacon(incoming_msg);
			break;
		case BROADCAST_ADVERSARIES_POSITION:
			DETECTION_pos_foe_update(incoming_msg);
			break;
/************************************* Récupération des envois de l'autre robot ***************************/
		case XBEE_START_MATCH:
			if(global.env.prop.calibrated)	//Vérification pour éviter de lancer un match si on est pas "sur le terrain"...et près à partir.
				global.env.ask_start = TRUE;	//Inconvénient : il FAUT être calibré pour lancer un match à partir de l'autre robot.
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

		case XBEE_REACH_POINT_GET_OUT_INIT:
			global.env.com.reach_point_get_out_init = TRUE;
			break;

		case XBEE_HOLLY_ASK_PROTECT:
			wood_receive_protect(incoming_msg);
			break;

		case XBEE_WOOD_PROTECT_ZONE:
			incoming_msg->sid = PROP_WOOD_PROTECT_ZONE;	//On change le SID et on restransmet à la prop.
			CAN_send(incoming_msg);
			break;

		case XBEE_WOOD_TAKE_CUP:
			holly_recieve_cup_state(incoming_msg);
			break;
		case XBEE_HOLLY_START_MATCH:
			holly_start_match(incoming_msg);
			break;
		case XBEE_WOOD_CAN_DO_CLAP:
			Wood_can_do_our_clap(incoming_msg);
			break;
		case XBEE_WOOD_CAN_GO:
			ELEMENTS_set_flag(ELEMENTS_WOOD_CAN_GO, TRUE);
			break;
		case XBEE_HOLLY_TAKE_MIDDLE_FEET:
			ELEMENTS_set_flag(ELEMENTS_MIDDLE_FEET_TOOK, TRUE);
			break;

/************************************* Récupération des messages liés au selftest ***************************/
		case STRAT_BEACON_IR_SELFTEST_DONE :
		case STRAT_BEACON_US_SELFTEST_DONE :
		case STRAT_ACT_SELFTEST_DONE :
		case STRAT_PROP_SELFTEST_DONE :
		case STRAT_IHM_SELFTEST_DONE:
		case STRAT_ACT_PONG:
		case STRAT_PROP_PONG:
		case STRAT_BEACON_PONG:
		case STRAT_IHM_PONG:
			SELFTEST_update(incoming_msg);
			break;
/************************************* Récupération des messages de la balise fixe ***************************/
		case STRAT_ZONE_INFOS:
			FIX_BEACON_process_msg(incoming_msg);
			break;

/************************************* Récupération des messages de la carte IHM ***************************/
		case IHM_SWITCH_ALL:
		case IHM_BUTTON:
		case IHM_SWITCH:
		case IHM_POWER:
			IHM_process_main(incoming_msg);
			break;
		case IHM_BIROUTE_IS_REMOVED:
			global.env.ask_start = TRUE;
			break;

		default:
			break;
	}
	return;
}


/* mise à jour de la position reçue dans l'un des messages de la propulsion.*/
void ENV_pos_update (CAN_msg_t* msg)
{
	Sint16 cosinus, sinus;
	global.env.pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
	global.env.pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
	global.env.pos.translation_speed = ((Uint16)(msg->data[0] >> 5))*250;	// [mm/sec]
//	if(global.env.pos.translation_speed > 1500)
//		debug_printf("");
	global.env.pos.rotation_speed =	((Uint16)(msg->data[2] >> 5));		// [rad/sec]
	global.env.pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
	COS_SIN_4096_get(global.env.pos.angle, &cosinus, &sinus);
	global.env.pos.cosAngle = cosinus;
	global.env.pos.sinAngle = sinus;
	global.env.prop.last_time_pos_updated = global.env.match_time;
	global.env.prop.current_way = (way_e)((msg->data[7] >> 3) & 0x03);
	global.env.prop.current_status = (SUPERVISOR_error_source_e)((msg->data[7]) & 0x07);
	global.env.prop.is_in_translation = (((msg->data[7] >> 5) & 0x07) >> 2) & 1;
	global.env.prop.is_in_rotation = (((msg->data[7] >> 5) & 0x07) >> 1) & 1;

	global.env.initial_position_received = TRUE;
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



/* Appelée en début de tache de fond : baisse les drapeaux d'environnement pour préparer la prochaine MaJ */
void ENV_clean (void)
{
	STACKS_clear_timeouts();
	DETECTION_clean();
	global.env.propulsion_coefs_updated = 0x00000000;
	if(global.env.color == global.env.wanted_color)
		global.env.color_updated = FALSE;
	global.env.prop.ended = FALSE;
	global.env.prop.erreur = FALSE;
	global.env.prop.freine = FALSE;
	global.env.prop.reach_x = FALSE;
	global.env.prop.reach_y = FALSE;
	global.env.prop.reach_teta = FALSE;
		//global.env.prop.last_time_pos_updated = 0;
	global.env.pos.updated = FALSE;
	global.env.ask_prop_calibration = FALSE;
	global.env.debug_force_foe = FALSE;
	global.env.duration_trajectory_for_test_coefs = 0;
	FIX_BEACON_clean();	//Doit être après le any_match !
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

void ENV_warning_switch(){
	static bool_e init = FALSE;
	static bool_e asser_switch;

	if(!init){
		init = TRUE;
		asser_switch = IHM_switchs_get(SWITCH_ASSER);
	}

	if(IHM_switchs_get(SWITCH_ASSER) != asser_switch){
		if(IHM_switchs_get(SWITCH_ASSER))
			CAN_direct_send(IHM_SET_ERROR, 2, (Uint8[]){IHM_ERROR_ASSER, FALSE});
		else
			CAN_direct_send(IHM_SET_ERROR, 2, (Uint8[]){IHM_ERROR_ASSER, TRUE});

		asser_switch = IHM_switchs_get(SWITCH_ASSER);
	}
}

