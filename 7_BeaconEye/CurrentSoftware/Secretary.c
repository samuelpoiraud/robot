/**
 * \file Secretary.c
 * \brief Interface de communication avec le reste du monde...
 * \author Nirgal
 * \version 201403
 * \date 18 mars 2014
 *
 */
/*--------------------------------Messages entrants -----------------------------
 *
 *
 * Demande d'activation d'une zone :
 * 	ENABLE_WATCHING_ZONE
 * 		- robot destinataire de cette demande (gros ou petit) enum de WHO_AM_I
 *		- num�ro de la zone concern�e (enum publique !)
 * 		- �v�nements demand�s...
 * 				- get in
 * 				- get out
 *	 	 		- temps de passage minimum ? (0 si un simple passage imm�diat suffit)
 * 				- activation de la surveillance d'un mode sp�cifique (coordon�e Y du X mini atteinte dans la zone / position de d�pose sur un foyer) (le message sera envoy� � la sortie de la zone)
 * 		- size... !
 *
 * DISABLE_WATCHING_ZONES
 * 		- robot destinataire de cette demande (gros ou petit) enum de WHO_AM_I
 * 		- num�ro de la zone � d�sactiver
 * 		- size ...
 *
 * GET_ZONE_INFOS
 * 	r�cup�ration des infos accumul�es (sans qu'on nous l'ai demand�) depuis le d�but du match.... + de l'�tat de la config de la zone !
 * 		- num�ro de zone concern�e.
 * 		- robot de destination de la demande
 *
 * 	BROADCAST_START
 * 	 reset des infos zones
 *
 * 	BROADCAST_STOP_ALL
 * 	 arr�t de toutes les surveillances + �ventuellement log des infos mesur�es... (ou envoi de ces infos pour log sur la strat du match)
 *
 */
/*
 * --------------------------------Messages sortants -----------------------------
 *
 * DETECTION_ADVERSARY
 * 	- num�ro de la zone
 * 	- il vient d'entrer
 * 	- il vient de sortir
 * 	- il y a pass� tant de temps
 * 	- sa coordonn�e en Y du X mini atteinte est : (si activ�e ! 0 sinon) OU sa coordonn�e sp�cifique sur foyer...
 *
 *
 */
#include "Secretary.h"
#include "zone.h"
#include "main.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_can_verbose.h"
#include "environment.h"

volatile bool_e flag_new_event = FALSE;
static zone_event_t events[ZONE_NUMBER];

static void SECRETARY_process_send_zone(zone_e zone, zone_event_t event);
void SECRETARY_process_canmsg(CAN_msg_t * msg);
static void SECRETARY_send_to_XBEE_zone_infos(zone_e zone, robot_id_e robot_id);
static void SECRETARY_send_callback(CAN_msg_t * can_msg);


void SECRETARY_init(void){
	zone_e i;
	for(i=0;i<ZONE_NUMBER;i++)
		events[i] = EVENT_NO_EVENT;

#ifdef USE_XBEE
	CAN_over_XBee_init(BALISE_MERE, BIG_ROBOT_MODULE);	//La destination ne sera pas utilis�e (car en fait on en a deux).
	CAN_over_XBee_set_send_callback(SECRETARY_send_callback);
#endif
}


void SECRETARY_process_main(void){
	zone_e i;

	if(flag_new_event){
		flag_new_event = FALSE;

		for(i=0;i<ZONE_NUMBER;i++){
			if(events[i] != EVENT_NO_EVENT){
				SECRETARY_process_send_zone(i, events[i]);
				events[i] = EVENT_NO_EVENT;
			}
		}
	}

#ifdef USE_XBEE
	static CAN_msg_t msg;
	CAN_over_XBee_process_main();

	if(XBeeToCANmsg(&msg)){	//Message re�u !
		SECRETARY_process_canmsg(&msg);
#ifdef CAN_VERBOSE_MODE
		QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
#endif
	}
#endif

}

void SECRETARY_process_ms(void){
	#ifdef USE_XBEE
	CAN_over_XBee_process_ms();
	#endif
}

void SECRETARY_set_event(zone_e i, zone_event_t event){
	events[i] |= event;	//On cumule les �v�nements pour chaque zone
	flag_new_event = TRUE;
}

void SECRETARY_process_canmsg(CAN_msg_t * msg){
	switch(msg->sid){
		case BROADCAST_START:
			ZONE_clean_all_detections();	//On r�initialise toutes les d�tections !
			if(!global.flags.match_started){
				global.flags.match_started = TRUE;
				global.match_time = 0;
			}
			break;
		case XBEE_REACH_POINT_GET_OUT_INIT:
		break;
		case BROADCAST_STOP_ALL:
			//On d�sactive toutes les surveillances... (en gardant les �v�nements d�j� lev�s)
			ZONE_disable_all();
			break;
		case BROADCAST_COULEUR:
			ZONE_clean_all_detections();	//On r�initialise toutes les zones (�v�nements par d�faut...)
			if(msg->size >= 1 && msg->data.broadcast_couleur.color <= BOT_COLOR)
				ENV_set_color(msg->data.broadcast_couleur.color);
			break;
		case ENABLE_WATCHING_ZONE:
			if(msg->size >= 3 && msg->data.enable_watching_zone.zone < ZONE_NUMBER && msg->data.enable_watching_zone.robot <= SMALL_ROBOT)
				ZONE_enable(msg->data.enable_watching_zone.zone, msg->data.enable_watching_zone.robot, msg->data.enable_watching_zone.event);
			else
				debug_printf("Message ENABLE_WATCHING_ZONE : donn�es non coh�rentes, v�rifiez le message\n");
			break;
		case DISABLE_WATCHING_ZONES:
			if(msg->size >= 2 && msg->data.disable_watching_zone.zone < ZONE_NUMBER && msg->data.disable_watching_zone.robot <= SMALL_ROBOT)
				ZONE_disable(msg->data.disable_watching_zone.zone, msg->data.disable_watching_zone.robot);
			else
				debug_printf("Message DISABLE_WATCHING_ZONES : donn�es non coh�rentes, v�rifiez le message\n");
			break;
		case GET_ZONE_INFOS:
			if(msg->size >= 2 && msg->data.get_zone_infos.zone < ZONE_NUMBER && msg->data.get_zone_infos.robot <= SMALL_ROBOT)
				SECRETARY_send_to_XBEE_zone_infos(msg->data.get_zone_infos.zone, msg->data.get_zone_infos.robot);
			break;
		default:
			break;
	}
}

static void SECRETARY_send_to_XBEE_zone_infos(zone_e zone, robot_id_e robot_id){
	CAN_msg_t msg;
	zones_t * pzone;
	char * dest_string;
	pzone = ZONE_get_pzone(zone);
	pzone->alert_was_send = TRUE;

	msg.sid = STRAT_ZONE_INFOS;
	msg.size = SIZE_STRAT_ZONE_INFOS;
	msg.data.strat_zone_infos.zone = zone;
	msg.data.strat_zone_infos.event = events[zone];	//Liste des �v�nements actuellement connus pour cette zone.
	msg.data.strat_zone_infos.min_detection_time = pzone->min_detection_time;
	msg.data.strat_zone_infos.someone_is_here = pzone->someone_is_here;	//Occupation actuelle de la zone
	msg.data.strat_zone_infos.specific_param_x = MIN(0xFF, pzone->specific_analyse_param_x/20);	//[unit� : 2cm]
	msg.data.strat_zone_infos.specific_param_y = MIN(0xFF, pzone->specific_analyse_param_y/20);	//[unit� : 2cm]
	msg.data.strat_zone_infos.presence_duration = pzone->presence_duration;	//Temps de pr�sence dans la zone des objets qui s'y trouvent o� qui en sont sortis.

	switch(robot_id){
	#ifdef USE_XBEE
		case BIG_ROBOT:		CANMsgToXBeeDestination(&msg, BIG_ROBOT_MODULE);	dest_string = "BIG";		break;
		case SMALL_ROBOT:	CANMsgToXBeeDestination(&msg, SMALL_ROBOT_MODULE);	dest_string = "SMALL";		break;
	#endif
		default:																dest_string = "UNKNOW";		break;
	}
	debug_printf("\t\t\t\t\t\t\tXBEE msg to %s | sid=%x zone=%d | events=%x | time=%ld\n", dest_string, msg.sid, zone, events[zone], pzone->min_detection_time);
	//TODO compl�ter le printf...
}

static void SECRETARY_process_send_zone(zone_e zone, zone_event_t event){
	zones_t * pzone;
	pzone = ZONE_get_pzone(zone);

	//Rappel : aucun message ne sera envoy� si la destination demand�e n'a pas �t� jointe par PING ou PONG...
	if(	(pzone->enable & ZONE_ENABLE_FOR_BIG)  		&& 	(pzone->events_for_big & event))		//Zone activ�e pour le GROS, et �v�nement correspondant aux �v�nement demand�s par le gros
		SECRETARY_send_to_XBEE_zone_infos(zone, BIG_ROBOT);
	if(	(pzone->enable & ZONE_ENABLE_FOR_SMALL) 	&& 	(pzone->events_for_small & event))		//Zone activ�e pour le PETIT, et �v�nement correspondant aux �v�nement demand�s par le petit
		SECRETARY_send_to_XBEE_zone_infos(zone, SMALL_ROBOT);

	pzone->alert_was_send = TRUE;
}

static void SECRETARY_send_callback(CAN_msg_t * can_msg){
	UNUSED_VAR(can_msg);
#ifdef CAN_VERBOSE_MODE
	QS_CAN_VERBOSE_can_msg_print(can_msg, VERB_OUTPUT_MSG);
#endif
}
