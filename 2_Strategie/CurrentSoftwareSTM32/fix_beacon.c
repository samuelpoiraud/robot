/**
 * \file fix_beacon.c
 * \brief Dialogue avec la balise fixe / récupération des infos d'observations de l'adversaire
 * \author Nirgal
 * \version 201403
 * \date 19 mars 2014
 *
 */

#include "fix_beacon.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_watchdog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"

static volatile bool_e initialized = FALSE;



zone_t zones[ZONE_NUMBER];
volatile bool_e zones_updated = FALSE;

void FIX_BEACON_init(void)
{
	zone_e i;
	for(i = 0; i<ZONE_NUMBER; i++)
	{
		zones[i].events = EVENT_NO_EVENT;
		zones[i].min_detection_time = 0;
		zones[i].presence_duration = 0;
		zones[i].someone_is_here = FALSE;
		zones[i].specific_param_x = 0;
		zones[i].specific_param_y = 0;
		zones[i].updated = FALSE;
	}
	zones_updated = FALSE;
	initialized = TRUE;
}

//Doit être appelé après le any_match (car reset des flag updated)
void FIX_BEACON_clean(void)
{
	zone_e i;
	if(!initialized)
		return;
	if(zones_updated)
	{
		zones_updated = FALSE;
		for(i = 0; i<ZONE_NUMBER; i++)
		{
			if(zones[i].updated)
			{
				debug_printf("Infos from FIX_BEACON : zone %d",i);
				zones[i].updated = FALSE;		//Ràz des flags.
			}
		}
	}
}
zone_t * FIX_BEACON_get_pzone(zone_e i)
{
	return &zones[i];
}


//Demande l'activation d'une zone
void FIX_BEACON_zone_enable(zone_e zone, zone_event_t events)
{
	CAN_msg_t msg;
	msg.sid = ENABLE_WATCHING_ZONE;
	msg.data[0] = zone;
	msg.data[1] = QS_WHO_AM_I_get();
	msg.data[2] = events;
	msg.size = 3;
	CANMsgToXBeeDestination(&msg,BALISE_MERE);
}

void FIX_BEACON_zone_disable(zone_e zone)
{
	CAN_msg_t msg;
	msg.sid = DISABLE_WATCHING_ZONES;
	msg.data[0] = zone;
	msg.data[1] = QS_WHO_AM_I_get();
	msg.size = 2;
	CANMsgToXBeeDestination(&msg,BALISE_MERE);
}

static void FIX_BEACON_ask_infos(zone_e zone)
{
	CAN_msg_t msg;
	msg.sid = GET_ZONE_INFOS;
	msg.data[0] = zone;
	msg.data[1] = QS_WHO_AM_I_get();
	msg.size = 2;
	CANMsgToXBeeDestination(&msg,BALISE_MERE);
}

#define TIMEOUT_FOR_GETTING_INFOS	500	//500ms
//Sub-action demandant une info sur une zone
//Il FAUT envoyer NULL dans le paramètre msg !!!
error_e FIX_BEACON_get_infos(zone_e zone, CAN_msg_t * msg)
{
	typedef enum
	{
		INIT = 0,
		SEND_ASK,
		WAIT_ANSWER,
		ANSWER_RECEIVED
	}state_e;
	static state_e state = INIT;
	static bool_e flag_timeout = FALSE;
	static watchdog_id_t watchdog_id;
	static zone_e current_zone = ZONE_NUMBER;	//Aucune zone...
	error_e ret;
	ret = IN_PROGRESS;

	if(!initialized)
		return NOT_HANDLED;
	if(msg != NULL && state == WAIT_ANSWER)
	{
		if(msg->data[0] == current_zone)
		{
			//TODO sauvegarder les infos...
			state = ANSWER_RECEIVED;
		}
	}
	else
	{
		switch(state)
		{
			case INIT:
				if(XBee_is_module_reachable(BALISE_MERE))
					state = SEND_ASK;
				else
					ret = NOT_HANDLED;
				break;
			case SEND_ASK:
				if(zone < ZONE_NUMBER)
				{
					current_zone = zone;
					FIX_BEACON_ask_infos(zone);
					watchdog_id = WATCHDOG_create_flag(TIMEOUT_FOR_GETTING_INFOS,&flag_timeout);
					state = WAIT_ANSWER;
				}
				break;
			case WAIT_ANSWER:
				//Cf traitement effectué sur réception d'un message CAN, au dessus de la MAE.
				if(flag_timeout)
					ret = END_WITH_TIMEOUT;
				break;
			case ANSWER_RECEIVED:
				if(!flag_timeout)
					WATCHDOG_stop(watchdog_id);	//Arrêt prore du Watchdog
				//TODO Traitement...
				ret = END_OK;
				break;

			default:
				state = INIT;
				break;
		}
	}
	if(ret != IN_PROGRESS)
		state = INIT;			//Réinitialisation de la MAE.
	return ret;
}


//Traitement du message STRAT_ZONE_INFOS
void FIX_BEACON_process_msg(CAN_msg_t * msg)
{
	zone_e i;
	if(!initialized)
		return;
	if(msg->sid != STRAT_ZONE_INFOS)
	{
		i = msg->data[0];
		FIX_BEACON_get_infos(i, msg);
		zones[i].events 			= msg->data[1];	//Liste des évènements actuellement connus pour cette zone.;
		zones[i].min_detection_time = msg->data[2];
		zones[i].someone_is_here 	= msg->data[3];	//Occupation actuelle de la zone;
		zones[i].specific_param_x 	= (Uint16)(msg->data[4])*20;	//[unité : 2cm] -> [unité mm]
		zones[i].specific_param_y 	= (Uint16)(msg->data[5])*20;	//[unité : 2cm] -> [unité mm]
		zones[i].presence_duration	= msg->data[6];	//Temps de présence dans la zone des objets qui s'y trouvent où qui en sont sortis.
		zones[i].updated = TRUE;
		zones_updated = TRUE;	//Pour éviter d'avoir à scruter les updated de chaque zone pour savoir si qch a changé.
	}
}

