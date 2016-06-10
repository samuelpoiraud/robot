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

#include "environment.h"
#include "strats_2016/actions_both_2016.h"
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
#include "zones.h"

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
	global.wanted_color=COLOR_INIT_VALUE;
	global.color = COLOR_INIT_VALUE; //update -> color = wanted + dispatch
	global.other_robot_color = COLOR_INIT_VALUE;

	global.flags.color_updated = TRUE;
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	global.flags.match_suspended = FALSE;
	global.flags.ask_prop_calibration = FALSE;
	global.flags.ask_suspend_match = FALSE;
	global.flags.ask_start = FALSE;
	global.flags.foes_updated_for_lcd = FALSE;
	global.flags.initial_position_received = FALSE;
	global.flags.alim = FALSE;
	global.flags.go_to_home = FALSE;
	global.friend_position_lifetime = 0;
	global.friend_pos.x = 0;
	global.friend_pos.y = 0;

	for(i=0;i<MAX_NB_FOES;i++)
	{
		global.foe[i].fiability_error = 0;
		global.foe[i].enable = FALSE;
		global.foe[i].update_time = 0;
	}
	global.match_time = 0;
	global.pos.dist = 0;
	global.prop.calibrated = FALSE;
	global.prop.current_way = ANY_WAY;
	global.prop.is_in_translation = FALSE;
	global.prop.is_in_rotation = FALSE;
	global.prop.current_status = NO_ERROR;
	global.alim_value = 0;
	global.destination = (GEOMETRY_point_t){0,0};
	for(i=0;i<PROPULSION_NUMBER_COEFS;i++)
		global.debug.propulsion_coefs[i] = 0;
	global.com.reach_point_get_out_init = FALSE;

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
			if((msg->data.broadcast_position_robot.reason & (WARNING_TRANSLATION | WARNING_ROTATION | WARNING_TIMER)))	//Si le message ne porte pas de warning : on filtre.
			{
				//On ne propage pas les messages de BROADCAST_POSITION_ROBOT (dans le cas où les raisons ne sont pas des WARN).
				*bSAVE_filter = FALSE;
				//Traitement spécial pour les messages d'asser position : maxi 1 par seconde !
				if(global.absolute_time-1000>filter_broadcast_position)
					filter_broadcast_position=global.absolute_time;
				else
					*bUART_filter = FALSE;	//Ca passe pas...
			}

			break;
		case BROADCAST_ADVERSARIES_POSITION:
			*bUART_filter = FALSE;	//Ca passe pas... 				(mieux vaut carrément afficher 	ponctuellement les infos qui découlent de ce message)
			*bSAVE_filter = FALSE;	//Pas d'enregistrement non plus	(mieux vaut carrément sauver 	ponctuellement les infos qui découlent de ce message)
			break;
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			//if(global.absolute_time-1000>filter_beacon_ir)
			//	filter_beacon_ir=global.absolute_time;
			//else
			*bUART_filter = FALSE;	//Ca passe pas...
			*bSAVE_filter = FALSE;
			break;
		case DEBUG_AVOIDANCE_POLY:
			*bUART_filter = FALSE;	//Ca passe pas...
			*bSAVE_filter = FALSE;
			break;
		case XBEE_SEND_COLOR:
			*bUART_filter = FALSE;	//Ca passe pas...
			*bSAVE_filter = FALSE;
			break;
		case XBEE_GET_COLOR:
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

	if(bSAVE_filter)
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
		if(IHM_switchs_get(SWITCH_RAW_DATA) && bU1)
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

		if(IHM_switchs_get(SWITCH_RAW_DATA)  && !IHM_switchs_get(SWITCH_XBEE) && bU2)
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

	#ifdef SD_ENABLE
		SD_new_event(TO_BUSCAN, sent_msg, NULL, TRUE);
	#endif

	//UART1
	if(IHM_switchs_get(SWITCH_RAW_DATA))
	{
		if(IHM_switchs_get(SWITCH_VERBOSE)){
			QS_CAN_VERBOSE_can_msg_print(sent_msg, VERB_OUTPUT_MSG);
		}else
			CANmsgToU1tx(sent_msg);
	}

	//UART2 - désactivé volontairement -> parce qu'on en a pas besoin...
	//if(IHM_switchs_get(SWITCH_RAW_DATA)  && !IHM_switchs_get(SWITCH_XBEE))
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

	if(global.flags.initial_position_received == FALSE && last_time_tell_position != 0 && global.absolute_time - last_time_tell_position > 500){
		CAN_send_sid(PROP_TELL_POSITION);
		last_time_tell_position = global.absolute_time;
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


#define INTERVAL_ASK_TIME 1000  //Temps entre deux demandes d'état de la communication

#ifdef USE_XBEE
	if(IHM_switchs_get(SWITCH_XBEE))
	{
		if(XBeeToCANmsg(&can_msg_from_uart2))
			ENV_process_can_msg(&can_msg_from_uart2,TRUE, TRUE, FALSE, FALSE);	//Everywhere except U2 and XBee.

		//Mise à jour du flag sur l'état de la communication XBEE
		static error_e result = NOT_HANDLED;
		static time32_t time = 0;
		if(result == IN_PROGRESS)
		{
			result = ELEMENTS_check_communication(NULL);
		}
		else if(result == END_WITH_TIMEOUT && ((time + INTERVAL_ASK_TIME) < global.absolute_time) )
		{
			result = ELEMENTS_check_communication(NULL);
			time = global.absolute_time;
		}
		else if(result == END_OK && ((time + INTERVAL_ASK_TIME) < global.absolute_time) )
		{
			result = ELEMENTS_check_communication(NULL);
			time = global.absolute_time;
		}
		else if(result == NOT_HANDLED)
		{
			time = global.absolute_time;
			result = ELEMENTS_check_communication(NULL);
		}
	}

	//Mise à jour des éléments
	ELEMENTS_process_main();
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
			date.seconds 	= incoming_msg->data.debug_rtc_set.seconde;
			date.minutes 	= incoming_msg->data.debug_rtc_set.minute;
			date.hours 		= incoming_msg->data.debug_rtc_set.heure;
			date.day 		= incoming_msg->data.debug_rtc_set.journee;
			date.date 		= incoming_msg->data.debug_rtc_set.jour;
			date.month 		= incoming_msg->data.debug_rtc_set.mois;
			date.year 		= incoming_msg->data.debug_rtc_set.annee;
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
			BUZZER_play(incoming_msg->data.strat_buffer_play.duration,
						BUZZER_convert_enum_QS(incoming_msg->data.strat_buffer_play.note),
						incoming_msg->data.strat_buffer_play.nb_bip);
			break;
//****************************** Messages carte propulsion/asser *************************/
		case STRAT_TRAJ_FINIE:
			global.prop.ended = TRUE;
			break;
		case STRAT_PROP_FOE_DETECTED:
			set_prop_detected_foe(incoming_msg);
			break;
		case STRAT_PROP_ERREUR:
			global.prop.erreur = TRUE;
			break;
		case PROP_ROBOT_CALIBRE:
			global.prop.calibrated = TRUE;
			//position de départ 2014, Guy ne doit pas empêcher le passage de Pierre si jamais son début de match n'a pas été détecté... Pierre poussera ainsi Guy... autrement dit : Pierre qui roule n'amasse pas de Guy...
				//PROP_set_correctors(FALSE, FALSE);
			break;
		case DEBUG_PROPULSION_COEF_IS:
			if(incoming_msg->data.debug_propulsion_coef_is.id < PROPULSION_NUMBER_COEFS)
			{
				global.debug.propulsion_coefs_updated |=  (Uint32)(1) << incoming_msg->data.debug_propulsion_coef_is.id;
				global.debug.propulsion_coefs[incoming_msg->data.debug_propulsion_coef_is.id] = incoming_msg->data.debug_propulsion_coef_is.value;
			}
			break;
		case BROADCAST_POSITION_ROBOT:
			//ATTENTION : Pas de switch car les raisons peuvent être cumulées !!!
			//Les raisons WARNING_TRANSLATION, WARNING_ROTATION, WARNING_NO et WARNING_TIMER ne font rien d'autres que déclencher un ENV_pos_update();

			if((incoming_msg->data.broadcast_position_robot.reason & (WARNING_REACH_X | WARNING_REACH_Y)) == (WARNING_REACH_X | WARNING_REACH_Y)){ //Nous venons d'atteindre une position en END_AT_DISTANCE (ie la position à 100mm près) pour laquelle on a demandé une surveillance à la propulsion.
				global.prop.reach_distance = TRUE;
				debug_printf("Rd\n");
			}else{
				if(incoming_msg->data.broadcast_position_robot.reason & WARNING_REACH_X)	//Nous venons d'atteindre une position en X pour laquelle on a demandé une surveillance à la propulsion.
				{
					global.prop.reach_x = TRUE;
					debug_printf("Rx\n");
				}

				if(incoming_msg->data.broadcast_position_robot.reason & WARNING_REACH_Y)	//Nous venons d'atteindre une position en Y pour laquelle on a demandé une surveillance à la propulsion.
				{
					global.prop.reach_y = TRUE;
					debug_printf("Ry\n");
				}
			}

			if(incoming_msg->data.broadcast_position_robot.reason & WARNING_REACH_TETA)	//Nous venons d'atteindre une position en Teta pour laquelle on a demandé une surveillance à la propulsion.
			{
				global.prop.reach_teta = TRUE;
				debug_printf("Rt\n");
			}

			break;
		case STRAT_ROBOT_FREINE:
			global.prop.freine = TRUE;

			break;
		case DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE:
			global.debug.duration_trajectory_for_test_coefs = incoming_msg->data.debug_trajectory_for_test_coefs_done.duration;
			break;
		case STRAT_SEND_REPORT:
			LCD_printf(1, TRUE, FALSE, "Dist:%d", incoming_msg->data.strat_send_report.actual_trans << 1);
			LCD_printf(2, TRUE, FALSE, "Rot :%4d MRot:%4d", (incoming_msg->data.strat_send_report.actual_rot << 3)*180/PI4096, (incoming_msg->data.strat_send_report.max_rot << 3)*180/PI4096);
			break;
		case STRAT_BACK_SCAN:
			ELEMENTS_set_flag(F_APOCALYPSE_SECOND_PART,incoming_msg->data.strat_back_scan.second_part);
			ELEMENTS_set_flag(F_APOCALYPSE_TOTAL_DUNE,incoming_msg->data.strat_back_scan.total_dune);
			ELEMENTS_set_flag(F_APOCALYPSE_ABSENT,incoming_msg->data.strat_back_scan.nothing);
			ELEMENTS_set_flag(F_APOCALYPSE_WTF,incoming_msg->data.strat_back_scan.wtf);
			if(ELEMENTS_get_flag(F_APOCALYPSE_SECOND_PART))
				ELEMENTS_set_middle_apocalypse(incoming_msg->data.strat_back_scan.middle);
			ELEMENTS_set_flag(F_APOCALYPSE_SCAN,TRUE);
			break;
		case STRAT_BLOC_POSITION:
			global.posBloc.x=incoming_msg->data.strat_bloc_position.x;
			global.posBloc.y=incoming_msg->data.strat_bloc_position.y;
			global.scan_bloc_error=incoming_msg->data.strat_bloc_position.error_scan;
			break;

		case PROP_RESULT_FISHS_SCAN:
			ACT_set_pos_fishs(incoming_msg->data.prop_result_fishs_scan.y_pos);
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
			//SELFTEST_update_led_beacon(incoming_msg);
			break;
		case BROADCAST_ADVERSARIES_POSITION:
			DETECTION_pos_foe_update(incoming_msg);
			break;
/************************************* Récupération des envois de l'autre robot ***************************/
		case XBEE_START_MATCH:
			if(QS_WHO_AM_I_get() == BIG_ROBOT || global.prop.calibrated)	//Vérification pour éviter de lancer un match si on est pas "sur le terrain"...et près à partir.
				global.flags.ask_start = TRUE;	//Inconvénient : il FAUT être calibré pour lancer un match à partir de l'autre robot.
			break;
		case XBEE_SEND_COLOR:
			global.other_robot_color = incoming_msg->data.xbee_send_color.color;
			global.flags.other_color_updated = TRUE;
			break;
		case XBEE_GET_COLOR:{
			CAN_msg_t msg;
			msg.sid = XBEE_SEND_COLOR;
			msg.size = XBEE_SEND_COLOR_SIZE;
			msg.data.xbee_send_color.color = global.color;
			CANMsgToXbee(&msg, FALSE);
		}case XBEE_PING:
			//On recoit un ping, on répond par un PONG.
			//Le lien est établi
			//Le module QS_CanoverXBee se débrouille pour PONGer
			break;
		case XBEE_PONG:
			//On reçoit un pong, tant mieux, le lien est établi
			break;
		case DEBUG_DETECT_FOE:
			global.debug.force_foe = TRUE;
			break;

		case XBEE_ZONE_COMMAND:
			ZONE_CAN_process_msg(incoming_msg);
			break;

		case XBEE_REACH_POINT_GET_OUT_INIT:
			global.com.reach_point_get_out_init = TRUE;
			break;

		case XBEE_MY_POSITION_IS:
			if(QS_WHO_AM_I_get() == PEARL)
			{
				if(incoming_msg->data.xbee_my_position_is.robot_id == BLACK)
				{
					global.friend_position_lifetime = 4000;		//Durée de vie pour cette donnée. (attention à la latence du XBEE)
					global.friend_pos.x = incoming_msg->data.xbee_my_position_is.x;
					global.friend_pos.y = incoming_msg->data.xbee_my_position_is.y;
					PEARL_set_state_black_for_com(incoming_msg);
				}
			}
			break;

		case XBEE_SYNC_ELEMENTS_FLAGS:
#ifdef USE_SYNC_ELEMENTS
			ELEMENTS_receive_flags(incoming_msg);
#endif
			break;

		case XBEE_COMMUNICATION_AVAILABLE:{
			CAN_msg_t answer;
			answer.sid = XBEE_COMMUNICATION_RESPONSE;
			answer.size = 0;
			CANMsgToXbee(&answer,FALSE);
			break;}

		case XBEE_COMMUNICATION_RESPONSE:
			ELEMENTS_check_communication(incoming_msg);
			break;

		case XBEE_ASK_CONFIG_COQUILLAGES:
			ELEMENTS_send_config_coquillages(incoming_msg);
			break;

/************************************* Récupération des messages liés au selftest ***************************/
		case STRAT_BEACON_SELFTEST_DONE :
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
			ENV_warning_switch();
			if(incoming_msg->sid == IHM_SWITCH || incoming_msg->sid == IHM_SWITCH_ALL)
				ELEMENTS_check_configuration_coquillages(TRUE);
			break;
		case IHM_BIROUTE_IS_REMOVED:
			global.flags.ask_start = TRUE;
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
	global.pos.x = msg->data.broadcast_position_robot.x;
	global.pos.y = msg->data.broadcast_position_robot.y;
	global.pos.translation_speed = msg->data.broadcast_position_robot.speed_trans*250;	// [mm/sec]
	global.pos.rotation_speed =	msg->data.broadcast_position_robot.speed_rot >> 5;		// [rad/sec]
	global.pos.angle = msg->data.broadcast_position_robot.angle;
	COS_SIN_4096_get(global.pos.angle, &cosinus, &sinus);
	global.pos.cosAngle = cosinus;
	global.pos.sinAngle = sinus;
	global.prop.last_time_pos_updated = global.match_time;
	global.prop.current_way = msg->data.broadcast_position_robot.way;
	global.prop.current_status = msg->data.broadcast_position_robot.error;
	global.prop.is_in_translation = msg->data.broadcast_position_robot.in_translation;
	global.prop.is_in_rotation = msg->data.broadcast_position_robot.in_rotation;

	global.flags.initial_position_received = TRUE;
	global.pos.updated = TRUE;
	//debug_printf("Position update to x=%d y=%d teta=%ld\n", global.pos.x, global.pos.y, global.pos.angle);
}



/* Appelée en début de tache de fond : baisse les drapeaux d'environnement pour préparer la prochaine MaJ */
void ENV_clean (void)
{
	STACKS_clear_timeouts();
	DETECTION_clean();
	global.debug.propulsion_coefs_updated = 0x00000000;
	if(global.color == global.wanted_color)
		global.flags.color_updated = FALSE;
	global.flags.ask_prop_calibration = FALSE;
	global.flags.ask_start = FALSE;
	global.flags.ask_suspend_match = FALSE;
	global.prop.ended = FALSE;
	global.prop.erreur = FALSE;
	global.prop.freine = FALSE;
	global.prop.reach_x = FALSE;
	global.prop.reach_y = FALSE;
	global.prop.reach_teta = FALSE;
	global.prop.reach_distance = FALSE;
		//global.prop.last_time_pos_updated = 0;
	global.pos.updated = FALSE;
	global.debug.force_foe = FALSE;
	global.debug.duration_trajectory_for_test_coefs = 0;
	FIX_BEACON_clean();	//Doit être après le any_match !
}

/* envoie un message CAN BROADCAST_COULEUR à jour */
void ENV_set_color(color_e color)
{
	/* changer la couleur */
	global.color = color;

	/* indiquer au monde la nouvelle couleur */
	CAN_msg_t msg;
	msg.sid = BROADCAST_COULEUR;
	msg.size = SIZE_BROADCAST_COULEUR;
	msg.data.broadcast_couleur.color = color;
	CAN_send(&msg);

	msg.sid = XBEE_SEND_COLOR;
	msg.size = XBEE_SEND_COLOR_SIZE;
	msg.data.xbee_send_color.color = color;
	CANMsgToXbee(&msg, FALSE);
}

void ENV_warning_switch(){
	static bool_e previous_asser_switch = FALSE;
	bool_e current_asser_switch;

	current_asser_switch = IHM_switchs_get(SWITCH18_DISABLE_ASSER);
	if(current_asser_switch != previous_asser_switch){
		CAN_msg_t msg;
		msg.sid = IHM_SET_ERROR;
		msg.size = SIZE_IHM_SET_ERROR;
		msg.data.ihm_set_error.error = IHM_ERROR_ASSER;
		msg.data.ihm_set_error.state = current_asser_switch;
		CAN_send(&msg);
	}
	previous_asser_switch = current_asser_switch;
}

