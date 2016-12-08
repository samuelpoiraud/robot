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
 *		- numéro de la zone concernée (enum publique !)
 * 		- événements demandés...
 * 				- get in
 * 				- get out
 *	 	 		- temps de passage minimum ? (0 si un simple passage immédiat suffit)
 * 				- activation de la surveillance d'un mode spécifique (coordonée Y du X mini atteinte dans la zone / position de dépose sur un foyer) (le message sera envoyé à la sortie de la zone)
 * 		- size... !
 *
 * DISABLE_WATCHING_ZONES
 * 		- robot destinataire de cette demande (gros ou petit) enum de WHO_AM_I
 * 		- numéro de la zone à désactiver
 * 		- size ...
 *
 * GET_ZONE_INFOS
 * 	récupération des infos accumulées (sans qu'on nous l'ai demandé) depuis le début du match.... + de l'état de la config de la zone !
 * 		- numéro de zone concernée.
 * 		- robot de destination de la demande
 *
 * 	BROADCAST_START
 * 	 reset des infos zones
 *
 * 	BROADCAST_STOP_ALL
 * 	 arrêt de toutes les surveillances + éventuellement log des infos mesurées... (ou envoi de ces infos pour log sur la strat du match)
 *
 */
/*
 * --------------------------------Messages sortants -----------------------------
 *
 * DETECTION_ADVERSARY
 * 	- numéro de la zone
 * 	- il vient d'entrer
 * 	- il vient de sortir
 * 	- il y a passé tant de temps
 * 	- sa coordonnée en Y du X mini atteinte est : (si activée ! 0 sinon) OU sa coordonnée spécifique sur foyer...
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
	CAN_over_XBee_init(BALISE_MERE, BIG_ROBOT_MODULE);	//La destination ne sera pas utilisée (car en fait on en a deux).
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

	if(XBeeToCANmsg(&msg)){	//Message reçu !
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
	events[i] |= event;	//On cumule les évènements pour chaque zone
	flag_new_event = TRUE;
}

void SECRETARY_process_canmsg(CAN_msg_t * msg){
	switch(msg->sid){
		case BROADCAST_START:
			ZONE_clean_all_detections();	//On réinitialise toutes les détections !
			if(!global.flags.match_started){
				global.flags.match_started = TRUE;
				global.match_time = 0;
			}
			break;
		case XBEE_REACH_POINT_GET_OUT_INIT:
		break;
		case BROADCAST_STOP_ALL:
			//On désactive toutes les surveillances... (en gardant les évènements déjà levés)
			ZONE_disable_all();
			break;
		case BROADCAST_COULEUR:
			ZONE_clean_all_detections();	//On réinitialise toutes les zones (évènements par défaut...)
			if(msg->size >= 1 && msg->data.broadcast_couleur.color <= BOT_COLOR)
				ENV_set_color(msg->data.broadcast_couleur.color);
			break;
		case ENABLE_WATCHING_ZONE:
			if(msg->size >= 3 && msg->data.enable_watching_zone.zone < ZONE_NUMBER && msg->data.enable_watching_zone.robot <= SMALL_ROBOT)
				ZONE_enable(msg->data.enable_watching_zone.zone, msg->data.enable_watching_zone.robot, msg->data.enable_watching_zone.event);
			else
				debug_printf("Message ENABLE_WATCHING_ZONE : données non cohérentes, vérifiez le message\n");
			break;
		case DISABLE_WATCHING_ZONES:
			if(msg->size >= 2 && msg->data.disable_watching_zone.zone < ZONE_NUMBER && msg->data.disable_watching_zone.robot <= SMALL_ROBOT)
				ZONE_disable(msg->data.disable_watching_zone.zone, msg->data.disable_watching_zone.robot);
			else
				debug_printf("Message DISABLE_WATCHING_ZONES : données non cohérentes, vérifiez le message\n");
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
	msg.data.strat_zone_infos.event = events[zone];	//Liste des évènements actuellement connus pour cette zone.
	msg.data.strat_zone_infos.min_detection_time = pzone->min_detection_time;
	msg.data.strat_zone_infos.someone_is_here = pzone->someone_is_here;	//Occupation actuelle de la zone
	msg.data.strat_zone_infos.specific_param_x = MIN(0xFF, pzone->specific_analyse_param_x/20);	//[unité : 2cm]
	msg.data.strat_zone_infos.specific_param_y = MIN(0xFF, pzone->specific_analyse_param_y/20);	//[unité : 2cm]
	msg.data.strat_zone_infos.presence_duration = pzone->presence_duration;	//Temps de présence dans la zone des objets qui s'y trouvent où qui en sont sortis.

	switch(robot_id){
	#ifdef USE_XBEE
		case BIG_ROBOT:		CANMsgToXBeeDestination(&msg, BIG_ROBOT_MODULE);	dest_string = "BIG";		break;
		case SMALL_ROBOT:	CANMsgToXBeeDestination(&msg, SMALL_ROBOT_MODULE);	dest_string = "SMALL";		break;
	#endif
		default:																dest_string = "UNKNOW";		break;
	}
	debug_printf("\t\t\t\t\t\t\tXBEE msg to %s | sid=%x zone=%d | events=%x | time=%ld\n", dest_string, msg.sid, zone, events[zone], pzone->min_detection_time);
	//TODO compléter le printf...
}

static void SECRETARY_process_send_zone(zone_e zone, zone_event_t event){
	zones_t * pzone;
	pzone = ZONE_get_pzone(zone);

	//Rappel : aucun message ne sera envoyé si la destination demandée n'a pas été jointe par PING ou PONG...
	if(	(pzone->enable & ZONE_ENABLE_FOR_BIG)  		&& 	(pzone->events_for_big & event))		//Zone activée pour le GROS, et évènement correspondant aux évènement demandés par le gros
		SECRETARY_send_to_XBEE_zone_infos(zone, BIG_ROBOT);
	if(	(pzone->enable & ZONE_ENABLE_FOR_SMALL) 	&& 	(pzone->events_for_small & event))		//Zone activée pour le PETIT, et évènement correspondant aux évènement demandés par le petit
		SECRETARY_send_to_XBEE_zone_infos(zone, SMALL_ROBOT);

	pzone->alert_was_send = TRUE;
}

static void SECRETARY_send_callback(CAN_msg_t * can_msg){
	UNUSED_VAR(can_msg);
#ifdef CAN_VERBOSE_MODE
	QS_CAN_VERBOSE_can_msg_print(can_msg, VERB_OUTPUT_MSG);
#endif
}
