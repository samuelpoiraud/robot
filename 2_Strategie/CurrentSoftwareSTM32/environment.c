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

/* met � jour l'environnement en fonction du message CAN re�u */
void CAN_update (CAN_msg_t* incoming_msg);

/* met a jour la position a partir d'un message asser la d�livrant */
void ENV_pos_update (CAN_msg_t* msg);

void ENV_process_can_msg_sent(CAN_msg_t * sent_msg);

void ENV_clean (void);

/* Regarde si les switchs � risque sont activ�s et mets des avertissements (LED,LCD) */
void ENV_warning_switch();

#define ADC_THRESHOLD 10 //Valeur de l'ADC sans dispositif de connect�

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
	global.flags.color_updated = TRUE;
	global.flags.match_started = FALSE;
	global.flags.match_over = FALSE;
	for(i=0;i<MAX_NB_FOES;i++)
	{
		global.foe[i].fiability_error = 0;
		global.foe[i].enable = FALSE;
		global.foe[i].update_time = 0;
	}
	global.flags.foes_updated_for_lcd = FALSE;
	global.match_time = 0;
	global.pos.dist = 0;
	global.flags.ask_start = FALSE;
	global.prop.calibrated = FALSE;
	global.prop.current_way = ANY_WAY;
	global.prop.is_in_translation = FALSE;
	global.prop.is_in_rotation = FALSE;
	global.prop.current_status = NO_ERROR;
	global.flags.alim = FALSE;
	global.alim_value = 0;
	global.destination = (GEOMETRY_point_t){0,0};
	for(i=0;i<PROPULSION_NUMBER_COEFS;i++)
		global.debug.propulsion_coefs[i] = 0;
	global.com.reach_point_get_out_init = FALSE;
	global.flags.initial_position_received = FALSE;

	FIX_BEACON_init();
}



void ENV_check_filter(CAN_msg_t * msg, bool_e * bUART_filter, bool_e * bCAN_filter, bool_e * bSAVE_filter)
 {
	//static time32_t filter_beacon_ir = 0;
	static time32_t filter_broadcast_position = 0;

	*bUART_filter = TRUE;	//On suppose que le message est autoris�.
	*bSAVE_filter = TRUE;

	switch(msg->sid)
	{
		//FILTRAGE POUR NE PAS ETRE SPAMMES PAR LE MESSAGE DE POSITION_ROBOT....
		case BROADCAST_POSITION_ROBOT:
			//si le message est porteur d'un warning, on ne le filtre pas.
			if((msg->data[6] & (WARNING_TRANSLATION | WARNING_ROTATION | WARNING_TIMER)) || !msg->data[0])	//Si le message ne porte pas de warning : on filtre.
			{
				//On ne propage pas les messages de BROADCAST_POSITION_ROBOT (dans le cas o� les raisons ne sont pas des WARN).
				*bSAVE_filter = FALSE;
				//Traitement sp�cial pour les messages d'asser position : maxi 1 par seconde !
				if(global.absolute_time-1000>filter_broadcast_position)
					filter_broadcast_position=global.absolute_time;
				else
					*bUART_filter = FALSE;	//Ca passe pas...
			}

			break;
		case BROADCAST_ADVERSARIES_POSITION:
			*bUART_filter = FALSE;	//Ca passe pas... 				(mieux vaut carr�ment afficher 	ponctuellement les infos qui d�coulent de ce message)
			*bSAVE_filter = FALSE;	//Pas d'enregistrement non plus	(mieux vaut carr�ment sauver 	ponctuellement les infos qui d�coulent de ce message)
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
		default:
			//Message autoris�.
			break;
	}

	if	( ((msg->sid & 0xF00) == STRAT_FILTER) ||  ((msg->sid & 0xF00) == XBEE_FILTER) || ((msg->sid & 0xF00) == BROADCAST_FILTER))
		*bCAN_filter = FALSE;	//On n'envoie pas sur le bus CAN des messages qui nous sont destin�s uniquement.
	else
		*bCAN_filter = TRUE;	//Seuls les messages DEBUG, ou destin�s aux cartes PROPULSION, ACTIONNEUR, BALISES, IHM sont transmis sur le bus can.
}


void ENV_process_can_msg(CAN_msg_t * incoming_msg, bool_e bCAN, bool_e bU1, bool_e bU2, bool_e bXBee)
{
	bool_e bUART_filter = FALSE;	//bUART_filter indique si ce message est filtr� ou s'il doit �tre propag� sur les uarts...
	bool_e bCAN_filter = FALSE;		//bCAN_filter indique si ce message doit �tre propag� sur le CAN...o� s'il nous �tait directement destin�...
	bool_e bSAVE_filter = FALSE;	//bEEPROM_filter indique si ce message doit �tre enregistr� en EEPROM

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
				#ifndef MODE_SIMULATION	//Pour ne pas spammer l'UART en mode simulation... On ne doit y voir que les messages CAN r�ellement envoy�s.
					CANmsgToU1tx(incoming_msg);
				#endif
			}
		}

		if(IHM_switchs_get(SWITCH_DEBUG)  && !IHM_switchs_get(SWITCH_XBEE) && bU2)
		{
			//D�sactiv� parce qu'on en a pas besoin...
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

	//UART2 - d�sactiv� volontairement -> parce qu'on en a pas besoin...
	//if(IHM_switchs_get(SWITCH_DEBUG)  && !IHM_switchs_get(SWITCH_XBEE))
	//	CANmsgToU2tx(sent_msg);

	//Messages de BROADCAST transmis aussi � la balise m�re.
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

	// RAZ des drapeaux temporaires pour la prochaine it�ration
	ENV_clean();

	if(global.flags.initial_position_received == FALSE && last_time_tell_position != 0 && global.absolute_time - last_time_tell_position > 500){
		CAN_send_sid(PROP_TELL_POSITION);
		last_time_tell_position = global.absolute_time;
	}

	// R�cuperation de l'�volution de l'environnement renseignee par les messages CAN
	while (CAN_data_ready())
	{
		toggle_led(LED_CAN);
		incoming_msg_from_bus_can = CAN_get_next_msg();
		ENV_process_can_msg(&incoming_msg_from_bus_can,FALSE, TRUE, TRUE, FALSE);	//Everywhere except CAN
	}


	while(UART1_data_ready())
	{
		c = UART1_get_next_msg();
		if(u1rx_receiving_can_msg == 0 && c == SOH)	//D�but d'un message CAN d�tect� : on dirige les 13 prochains octets vers u1rxToCANmsg
			u1rx_receiving_can_msg = CAN_MSG_LENGTH + 2;	//SOH, EOT

		if(u1rx_receiving_can_msg)
		{
			u1rx_receiving_can_msg--;
			if(u1rxToCANmsg(&can_msg_from_uart1,c))
			{
				ENV_process_can_msg(&can_msg_from_uart1,TRUE, FALSE, TRUE, TRUE);	//Everywhere except U1.
				u1rx_receiving_can_msg = 0;	//o� que soit notre compteur, on le remet � 0.
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


	/* R�cup�ration des donn�es des boutons */
	BUTTON_update();
}

/* met � jour l'environnement en fonction du message CAN re�u */
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
			RTC_can_send();	//Retour ... pour v�rifier que ca a fonctionn�..
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
			global.prop.ended = TRUE;
			break;
		case STRAT_PROP_FOE_DETECTED:
			set_prop_detected_foe(incoming_msg);
			break;
		case STRAT_PROP_ERREUR:
			global.prop.erreur = TRUE;
			global.prop.vitesse_translation_erreur =
				((Sint32)U16FROMU8(incoming_msg->data[1],incoming_msg->data[0]) << 16)
					+ U16FROMU8(incoming_msg->data[3],incoming_msg->data[2]);
			global.prop.vitesse_rotation_erreur =
				((Sint32)U16FROMU8(incoming_msg->data[5],incoming_msg->data[4]) << 16)
					+ U16FROMU8(incoming_msg->data[7],incoming_msg->data[6]);
			break;
		case PROP_ROBOT_CALIBRE:
			global.prop.calibrated = TRUE;
			if(QS_WHO_AM_I_get() == SMALL_ROBOT)
			{
				CAN_msg_t msg;
				Sint16 teta;

				//enrichir en fonction des diff�rentes strat�gies pr�vues...
				/*
				 * - se rendre au sud...
				 * - se rendre tout droit pour ramassage gobelet
				 * - viser le rush gobelet central (cette position est probablement diff�rente pour gagner du temps ?)
				 * => autre avantage, ca nous permet de savoir ce que va faire le robot (contr�le suppl�mentaire de l'�tat des switchs)
				 */

				if(!IHM_switchs_get(SWITCH_ATTACK_FIRST)){
					if(IHM_switchs_get(SWITCH_CUP_NORTH_OUR)){
						teta = COLOR_ANGLE(-PI4096/2);
					}else if(IHM_switchs_get(SWITCH_CUP_SOUTH_OUR)){
						teta = 0;
					}else{
						teta = COLOR_ANGLE(-PI4096/2);
					}
				}else{
					if(IHM_switchs_get(SWITCH_CUP_SOUTH_OUR)){
						teta = 0;
					}else{
						teta = COLOR_ANGLE(-PI4096/2);
					}
				}


				msg.sid = PROP_GO_ANGLE;
				msg.size = 8;
				msg.data[0] = NO_ACKNOWLEDGE | NOW | ABSOLUTE | NO_MULTIPOINT;	//Demande sp�cifique de NON Acquittement.
				msg.data[1] = HIGHINT(teta);	//teta high
				msg.data[2] = LOWINT(teta);		//teta low
				msg.data[3] = 0;				//RFU
				msg.data[4] = 0;				//RFU
				msg.data[5] = SLOW;				//
				msg.data[6] = ANY_WAY;			//
				msg.data[7] = 0;				//RFU
				CAN_send(&msg);

			}

			//position de d�part 2014, Guy ne doit pas emp�cher le passage de Pierre si jamais son d�but de match n'a pas �t� d�tect�... Pierre poussera ainsi Guy... autrement dit : Pierre qui roule n'amasse pas de Guy...
				//PROP_set_correctors(FALSE, FALSE);
			break;
		case DEBUG_PROPULSION_COEF_IS:
			if(incoming_msg->data[0] < PROPULSION_NUMBER_COEFS)
			{
				global.debug.propulsion_coefs_updated |=  (Uint32)(1) << incoming_msg->data[0];
				global.debug.propulsion_coefs[incoming_msg->data[0]] = (Sint32)(U32FROMU8(incoming_msg->data[1],incoming_msg->data[2],incoming_msg->data[3],incoming_msg->data[4]));
			}
			break;
		case BROADCAST_POSITION_ROBOT:
			//ATTENTION : Pas de switch car les raisons peuvent �tre cumul�es !!!
			//Les raisons WARNING_TRANSLATION, WARNING_ROTATION, WARNING_NO et WARNING_TIMER ne font rien d'autres que d�clencher un ENV_pos_update();

			if(incoming_msg->data[6] & WARNING_REACH_X)		//Nous venons d'atteindre une position en X pour laquelle on a demand� une surveillance � la propulsion.
			{
				global.prop.reach_x = TRUE;
				debug_printf("Rx\n");
			}

			if(incoming_msg->data[6] & WARNING_REACH_Y)	//Nous venons d'atteindre une position en Y pour laquelle on a demand� une surveillance � la propulsion.
			{
				global.prop.reach_y = TRUE;
				debug_printf("Ry\n");
			}
			if(incoming_msg->data[6] & WARNING_REACH_TETA)	//Nous venons d'atteindre une position en Teta pour laquelle on a demand� une surveillance � la propulsion.
			{
				global.prop.reach_teta = TRUE;
				debug_printf("Rt\n");
			}

			break;
		case STRAT_ROBOT_FREINE:
			global.prop.freine = TRUE;

			break;
		case DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE:
			global.debug.duration_trajectory_for_test_coefs = U16FROMU8(incoming_msg->data[0], incoming_msg->data[1]);
			break;
		case STRAT_SEND_REPORT:
			LCD_printf(1, TRUE, FALSE, "Dist:%d", U16FROMU8(incoming_msg->data[4], incoming_msg->data[5]) << 1);
			LCD_printf(2, TRUE, FALSE, "Rot :%4ld MRot:%4ld", ((Sint32)((Sint16)(U16FROMU8(incoming_msg->data[0], incoming_msg->data[1]))) << 3)*180/PI4096, ((Sint32)((Sint16)(U16FROMU8(incoming_msg->data[2], incoming_msg->data[3]))) << 3)*180/PI4096);
			break;
		case STRAT_CUP_POSITION:
			collect_cup_coord(incoming_msg);
			break;


		case DEBUG_PROPULION_HOKUYO_HAS_PLANTED_AND_THAT_IS_NOT_VERY_FUNNY:
			//BUZZER_play(1000,DEFAULT_NOTE,1);
			break;

//****************************** Messages de la carte actionneur *************************/
		case ACT_RESULT:
			ACT_process_result(incoming_msg);
			break;

		case STRAT_INFORM_CAPTEUR:
			ACT_sensor_answer(incoming_msg);
			break;

/************************************ R�cup�ration des donn�es de la balise *******************************/
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			//En absence d'hokuyo et du fonctionnement correct de la carte propulsion, les msg balises IR sont tr�s important pour l'�vitement.
			DETECTION_pos_foe_update(incoming_msg);
			SELFTEST_update_led_beacon(incoming_msg);
			break;
		case BROADCAST_ADVERSARIES_POSITION:
			DETECTION_pos_foe_update(incoming_msg);
			break;
/************************************* R�cup�ration des envois de l'autre robot ***************************/
		case XBEE_START_MATCH:
			if(QS_WHO_AM_I_get() == BIG_ROBOT || global.prop.calibrated)	//V�rification pour �viter de lancer un match si on est pas "sur le terrain"...et pr�s � partir.
				global.flags.ask_start = TRUE;	//Inconv�nient : il FAUT �tre calibr� pour lancer un match � partir de l'autre robot.
			break;
		case XBEE_PING:
			//On recoit un ping, on r�pond par un PONG.
			//Le lien est �tabli
			//Le module QS_CanoverXBee se d�brouille pour PONGer
			break;
		case XBEE_PONG:
			//On re�oit un pong, tant mieux, le lien est �tabli
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

		case XBEE_HOLLY_ASK_PROTECT:
			wood_receive_protect(incoming_msg);
			break;

		case XBEE_WOOD_PROTECT_ZONE:
			incoming_msg->sid = PROP_WOOD_PROTECT_ZONE;	//On change le SID et on restransmet � la prop.
			CAN_send(incoming_msg);
			break;

		case XBEE_WOOD_TAKE_CUP:
			holly_receive_cup_state(incoming_msg);
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
		case XBEE_HOLLY_DISPOSE_ON_ESTRAD:
			ELEMENTS_set_flag(HOLLY_DISPOSED_SPOT_IN_ESTRAD, TRUE);
			break;
		case XBEE_HOLLY_DISPOSE_CARPETS:
			ELEMENTS_set_flag(HOLLY_DISPOSED_FIRST_CARPET, TRUE);
			ELEMENTS_set_flag(HOLLY_DISPOSED_SECOND_CARPET, TRUE);
			break;
		case XBEE_CAN_WOOD_ACCESS_SCAN:
			holly_can_wood_access_scan(incoming_msg);
			break;
		case XBEE_WOOD_ACCESS_SCAN:
			sub_wood_ask_if_i_can_access_scan(NULL,NULL,NULL,incoming_msg);
			break;
/************************************* R�cup�ration des messages li�s au selftest ***************************/
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
/************************************* R�cup�ration des messages de la balise fixe ***************************/
		case STRAT_ZONE_INFOS:
			FIX_BEACON_process_msg(incoming_msg);
			break;

/************************************* R�cup�ration des messages de la carte IHM ***************************/
		case IHM_SWITCH_ALL:
		case IHM_BUTTON:
		case IHM_SWITCH:
		case IHM_POWER:
			IHM_process_main(incoming_msg);
			ENV_warning_switch();
			break;
		case IHM_BIROUTE_IS_REMOVED:
			global.flags.ask_start = TRUE;
			break;

		default:
			break;
	}
	return;
}


/* mise � jour de la position re�ue dans l'un des messages de la propulsion.*/
void ENV_pos_update (CAN_msg_t* msg)
{
	Sint16 cosinus, sinus;
	global.pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
	global.pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
	global.pos.translation_speed = ((Uint16)(msg->data[0] >> 5))*250;	// [mm/sec]
//	if(global.pos.translation_speed > 1500)
//		debug_printf("");
	global.pos.rotation_speed =	((Uint16)(msg->data[2] >> 5));		// [rad/sec]
	global.pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
	COS_SIN_4096_get(global.pos.angle, &cosinus, &sinus);
	global.pos.cosAngle = cosinus;
	global.pos.sinAngle = sinus;
	global.prop.last_time_pos_updated = global.match_time;
	global.prop.current_way = (way_e)((msg->data[7] >> 3) & 0x03);
	global.prop.current_status = (SUPERVISOR_error_source_e)((msg->data[7]) & 0x07);
	global.prop.is_in_translation = (((msg->data[7] >> 5) & 0x07) >> 2) & 1;
	global.prop.is_in_rotation = (((msg->data[7] >> 5) & 0x07) >> 1) & 1;

	global.flags.initial_position_received = TRUE;
	global.pos.updated = TRUE;
			/*msg->data[7] : 8 bits  : T R x W W E E E
				 T : TRUE si robot en translation
				 R : TRUE si robot en rotation
				 x : non utilis�
				 WW : Way, sens actuel
					ANY_WAY						= 0,
					BACKWARD					= 1,
					FORWARD						= 2,
				 EEE : Erreur
					SUPERVISOR_INIT				= 0,
					SUPERVISOR_IDLE				= 1,		//Rien � faire
					SUPERVISOR_TRAJECTORY		= 2,		//Trajectoire en cours
					SUPERVISOR_ERROR			= 3,		//Carte en erreur - attente d'un nouvel ordre pour en sortir
					SUPERVISOR_MATCH_ENDED		= 4			//Match termin�
				*/
}



/* Appel�e en d�but de tache de fond : baisse les drapeaux d'environnement pour pr�parer la prochaine MaJ */
void ENV_clean (void)
{
	STACKS_clear_timeouts();
	DETECTION_clean();
	global.debug.propulsion_coefs_updated = 0x00000000;
	if(global.color == global.wanted_color)
		global.flags.color_updated = FALSE;
	global.prop.ended = FALSE;
	global.prop.erreur = FALSE;
	global.prop.freine = FALSE;
	global.prop.reach_x = FALSE;
	global.prop.reach_y = FALSE;
	global.prop.reach_teta = FALSE;
		//global.prop.last_time_pos_updated = 0;
	global.pos.updated = FALSE;
	global.flags.ask_prop_calibration = FALSE;
	global.debug.force_foe = FALSE;
	global.debug.duration_trajectory_for_test_coefs = 0;
	FIX_BEACON_clean();	//Doit �tre apr�s le any_match !
}

/* envoie un message CAN BROADCAST_COULEUR � jour */
void ENV_set_color(color_e color)
{
	/* changer la couleur */
	global.color = color;

	/* indiquer au monde la nouvelle couleur */
	CAN_msg_t msg;
	msg.sid=BROADCAST_COULEUR;
	msg.data[0]=global.color;
	msg.size=1;
	CAN_send(&msg);
}

void ENV_warning_switch(){
	static bool_e previous_asser_switch = FALSE;
	bool_e current_asser_switch;

	current_asser_switch = IHM_switchs_get(SWITCH18_DISABLE_ASSER);
	if(current_asser_switch != previous_asser_switch)
		CAN_direct_send(IHM_SET_ERROR, 2, (Uint8[]){IHM_ERROR_ASSER, current_asser_switch});
	previous_asser_switch = current_asser_switch;
}

