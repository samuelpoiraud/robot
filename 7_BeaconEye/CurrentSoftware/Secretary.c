/**
 * \file Secretary.c
 * \brief Interface de communication avec le reste du monde...
 * \author Nirgal
 * \version 201403
 * \date 18 mars 2014
 *
 */

#include "Secretary.h"
#include "zone.h"
#include "main.h"
#include "environment.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_xbee/QS_canOverXbee.h"
#include "QS/QS_can_verbose.h"

volatile bool_e isNewEvent = FALSE;
static ZONE_event_t events[ZONE_NUMBER];

static void SECRETARY_processCanMsg(CAN_msg_t * msg);
static void SECRETARY_sendZoneInfos(ZONE_zoneId_e zone, robot_id_e robot_id);
static void SECRETARY_sendCallback(robot_id_e robotId, CAN_msg_t * can_msg);
static void SECRETARY_processSendZone(ZONE_zoneId_e zone, ZONE_event_t event);


void SECRETARY_init(void) {
	ZONE_zoneId_e i;

	for(i = 0; i < ZONE_NUMBER; i++) {
		events[i] = EVENT_NO_EVENT;
	}

#if defined(USE_XBEE)
	XBEE_init();
	CAN_OVER_XBEE_init();
	CAN_OVER_XBEE_setSendCallback(SECRETARY_sendCallback);
#endif

}

void SECRETARY_processMain(void) {
	ZONE_zoneId_e i;

	if(isNewEvent) {
		isNewEvent = FALSE;

		for(i = 0; i < ZONE_NUMBER; i++) {
			if(events[i] != EVENT_NO_EVENT) {
				SECRETARY_processSendZone(i, events[i]);
				events[i] = EVENT_NO_EVENT;
			}
		}
	}

#if defined(USE_XBEE)
	static CAN_msg_t msg;
	static robot_id_e robotId;

	XBEE_processMain();
	CAN_OVER_XBEE_processMain();

	if(CAN_OVER_XBEE_getInputMsg(&robotId, &msg)) {
		SECRETARY_processCanMsg(&msg);

#if defined(CAN_VERBOSE_MODE)
		QS_CAN_VERBOSE_can_msg_print(&msg, VERB_INPUT_MSG);
#endif

	}

#endif

}

void SECRETARY_setEvent(ZONE_zoneId_e id, ZONE_event_t event) {
	events[id] |= event;
	isNewEvent = TRUE;
}

static void SECRETARY_processCanMsg(CAN_msg_t * msg) {

	switch(msg->sid) {

		case BROADCAST_START:
			ZONE_cleanAllEvents();

			if(!global.flags.match_started) {
				global.flags.match_started = TRUE;
				global.match_time = 0;
			}

			break;

		case XBEE_REACH_POINT_GET_OUT_INIT:
		break;

		case BROADCAST_STOP_ALL:
			ZONE_disableAll();
			break;

		case BROADCAST_COULEUR:
			ENVIRONMENT_setColor(msg->data.broadcast_couleur.color);
			break;

		case ENABLE_WATCHING_ZONE:
			if(msg->data.enable_watching_zone.zone < ZONE_NUMBER && msg->data.enable_watching_zone.robot <= SMALL_ROBOT) {
				ZONE_enable(
						msg->data.enable_watching_zone.zone,
						msg->data.enable_watching_zone.robot,
						msg->data.enable_watching_zone.event
				);
			} else {
				debug_printf("Message ENABLE_WATCHING_ZONE : données non cohérentes, vérifiez le message\n");
			}
			break;

		case DISABLE_WATCHING_ZONES:
			if(msg->data.disable_watching_zone.zone < ZONE_NUMBER && msg->data.disable_watching_zone.robot <= SMALL_ROBOT) {
				ZONE_disable(
						msg->data.disable_watching_zone.zone,
						msg->data.disable_watching_zone.robot
				);
			} else {
				debug_printf("Message DISABLE_WATCHING_ZONES : données non cohérentes, vérifiez le message\n");
			}
			break;

		case GET_ZONE_INFOS:
			if(msg->data.get_zone_infos.zone < ZONE_NUMBER && msg->data.get_zone_infos.robot <= SMALL_ROBOT)
				SECRETARY_sendZoneInfos(msg->data.get_zone_infos.zone, msg->data.get_zone_infos.robot);
			break;

		default:
			break;
	}
}

static void SECRETARY_sendZoneInfos(ZONE_zoneId_e zone, robot_id_e robot_id) {
	CAN_msg_t msg;
	ZONE_zone_t* pzone;

	pzone = ZONE_getZone(zone);
	pzone->alertSent = TRUE;

	msg.sid = STRAT_ZONE_INFOS;
	msg.size = SIZE_STRAT_ZONE_INFOS;
	msg.data.strat_zone_infos.zone = zone;
	msg.data.strat_zone_infos.event = events[zone];
	msg.data.strat_zone_infos.min_detection_time = pzone->thresholdDetectionTime;
	msg.data.strat_zone_infos.someone_is_here = pzone->isSomeone;
	msg.data.strat_zone_infos.presence_duration = pzone->presenceTime;

#if defined(USE_XBEE)
	CAN_OVER_XBEE_sendCANMsgToModule(robot_id, &msg);
#endif

}

static void SECRETARY_processSendZone(ZONE_zoneId_e zone, ZONE_event_t event) {
	ZONE_zone_t* pzone;
	pzone = ZONE_getZone(zone);

	if((pzone->enable & ZONE_ENABLE_FOR_BIG) && (pzone->eventsBig & event)) {
		//Zone activée pour le GROS et évènement correspondant aux évènement demandés par le gros
		SECRETARY_sendZoneInfos(zone, BIG_ROBOT);
	}

	if((pzone->enable & ZONE_ENABLE_FOR_SMALL) && (pzone->eventsSmall & event)) {
		//Zone activée pour le PETIT et évènement correspondant aux évènement demandés par le petit
		SECRETARY_sendZoneInfos(zone, SMALL_ROBOT);
	}
}

static void SECRETARY_sendCallback(robot_id_e robotId, CAN_msg_t * can_msg) {
	UNUSED_VAR(can_msg);
#if defined(CAN_VERBOSE_MODE)
	QS_CAN_VERBOSE_can_msg_print(can_msg, VERB_OUTPUT_MSG);
#endif
}
