/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : zone_mutex.c
 *	Package :
 *	Description :
 *	Auteur : amurzeau
 *	Version 2 mai 2013
 */

#include "zone_mutex.h"
#include "QS/QS_CANmsgList.h"
#include "avoidance.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_can_over_xbee.h"
#include "state_machine_helper.h"

#include "config_debug.h"
#define LOG_PREFIX "zone_mutex: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES
#include "../QS/QS_outputlog.h"

#define RESPONSE_WAIT_TIMEOUT	1000
#define RETRY_TIMEOUT			400

//Zone ou les 2 robots peuvent passer, donc on doit éviter le cas ou les 2 robots sont en même temps dans la même zone

static zone_info_t zones[ZONE_MUTEX_NUMBER] = ZONE_INITIAL_STATE_DATA;

static void ZONE_send_lock_request(map_zone_e zone);
static void ZONE_send_lock_response(map_zone_e zone);


Uint8 try_lock_zone(map_zone_e zone, Uint16 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state) {
	switch(ZONE_try_lock(zone, timeout_msec)) {
		case IN_PROGRESS:
			return in_progress_state;

		case END_OK:
			return success_state;

		case NOT_HANDLED:
			return cant_lock_state;

		case END_WITH_TIMEOUT:
		default:
			return no_response_state;
	}
}

//Tente de lock une zone
error_e ZONE_try_lock(map_zone_e zone, Uint16 timeout_ms) {
	CREATE_MAE_WITH_VERBOSE(SM_ID_ZONE_TRY_LOCK,
		TL_INIT,
		TL_SEND_REQUEST,
		TL_WAIT_RESPONSE,
		TL_LOCKED,
		TL_TIMEOUT,
		TL_ERROR,
		TL_NO_RESPONSE
	);

	static time32_t last_try_time;
	static time32_t begin_lock_time;

	static zone_owner_e me;

	switch(state) {
		case TL_INIT:

			me = ((QS_WHO_AM_I_get() == BIG_ROBOT)? ZIS_BIG : ZIS_SMALL);

			last_try_time = 0;
			begin_lock_time = global.env.match_time;
			if(zones[zone].owner == me ) {  // Si la zone est occupé par moi-même
				state = TL_LOCKED;
			} else {
				state = TL_SEND_REQUEST;
			}
			break;

		case TL_SEND_REQUEST: // Nous avons envoyer un message de prise de zone, nous attendons mnt la réponse de l'autre
			last_try_time = global.env.match_time;

			if(zones[zone].owner == ZIS_Free && zones[zone].state == ZS_Free){ // Si c'est une zone libre non prise
				zones[zone].state = ZS_OwnedByMe;
				ZONE_send_lock_request(zone);
				state = TL_WAIT_RESPONSE;

			}else if(zones[zone].owner == ZIS_Free && zones[zone].state != ZS_Free){ // Zone libre, mais occupé
				state = TL_ERROR;

			}else{ // Zone de l'aute robot
				zones[zone].state = ZS_Acquiring;
				ZONE_send_lock_request(zone);
				state = TL_WAIT_RESPONSE;
			}

			break;

		case TL_WAIT_RESPONSE:
			if(zones[zone].state == ZS_OwnedByMe) {		//C'est bon, on a verrouillé la zone pour nous
				state = TL_LOCKED;

			} else if(zones[zone].state == ZS_Acquiring && global.env.match_time >= begin_lock_time + RESPONSE_WAIT_TIMEOUT) {
				//On a pas eu de réponse depuis trop de temps, l'autre robot ne répond pas ...
				state = TL_NO_RESPONSE;

			} else if(zones[zone].state == ZS_Acquiring && global.env.match_time >= last_try_time + RETRY_TIMEOUT) {
				//La zone est verrouillée par l'autre robot, on retente
				state = TL_SEND_REQUEST;

			}else if(timeout_ms && global.env.match_time >= begin_lock_time + timeout_ms) {
				//On est en train de tenter de verrouiller la zone depuis trop longtemps
				state = TL_TIMEOUT;
			}

			break;

		case TL_LOCKED:
			state = TL_INIT;
			return END_OK;

		case TL_TIMEOUT:
			zones[zone].state = ZS_Free;
			state = TL_INIT;
			return NOT_HANDLED;

		case TL_ERROR:
			state = TL_INIT;
			return NOT_HANDLED;

		case TL_NO_RESPONSE:
			zones[zone].state = ZS_Free;
			state = TL_INIT;
			return END_WITH_TIMEOUT;
	}

	return IN_PROGRESS;
}

void ZONE_unlock(map_zone_e zone) {
	if(zones[zone].state == ZS_OwnedByMe) {
		CAN_msg_t msg;
		msg.sid = XBEE_ZONE_COMMAND;
		msg.data[0] = XBEE_ZONE_UNLOCK;
		msg.data[1] = zone;
		msg.size = 2;
		CANMsgToXbee(&msg,FALSE);
		zones[zone].state = ZS_Free; // Remets la zone que j'ai prise en libre
	} else {
		debug_printf("zone: unlock zone %d not owned !!!, state = %d\n", zone, zones[zone].state);
	}
}

bool_e ZONE_is_free(map_zone_e zone) {
	return zones[zone].state == ZS_Free;
}

zone_state_e ZONE_get_status(map_zone_e zone) {
	return zones[zone].state;
}

void ZONE_CAN_process_msg(CAN_msg_t *msg) {
	assert(msg->sid == XBEE_ZONE_COMMAND);	//Si le SID correspond à une commande liée aux zones

	if(msg->data[1] >= ZONE_MUTEX_NUMBER) {
		debug_printf("zone: unknown zone %d !!!\n", msg->data[1]);
		return;
	}


	switch(msg->data[0]) {
		case XBEE_ZONE_LOCK_RESULT:

			if(msg->data[2] == TRUE) // La zone peut-être prise par celui qui l'a demandé
				zones[msg->data[1]].state = ZS_OwnedByMe;

			break;

		case XBEE_ZONE_TRY_LOCK:

			if(zones[msg->data[1]].owner == ((QS_WHO_AM_I_get() == BIG_ROBOT)? ZIS_BIG : ZIS_SMALL)){
				zones[msg->data[1]].state = ZS_OtherTryLock;
				ZONE_send_lock_response(msg->data[1]);

			}else if(zones[msg->data[1]].owner == ZIS_Free)				// Si l'autre robot prend une zone libre
				zones[msg->data[1]].state = ZS_OwnedByOther; // Ne renvois pas de message le premier à la demander gagne


			break;

		case XBEE_ZONE_UNLOCK:
			if(zones[msg->data[1]].state == ZS_OwnedByOther){
				zones[msg->data[1]].state = ZS_Free;
			}
			break;
	}
}

static void ZONE_send_lock_request(map_zone_e zone) {
	CAN_msg_t msg;

	msg.sid = XBEE_ZONE_COMMAND;
	msg.data[0] = XBEE_ZONE_TRY_LOCK;
	msg.data[1] = zone;
	msg.size = 2;

	CANMsgToXbee(&msg,FALSE);
}

static void ZONE_send_lock_response(map_zone_e zone) {

	if(zones[zone].owner != ((QS_WHO_AM_I_get() == BIG_ROBOT)? ZIS_BIG : ZIS_SMALL)) // Si on a une demande dont la zone nous appartient pas
		return;

	CAN_msg_t msg;

	msg.sid = XBEE_ZONE_COMMAND;
	msg.data[0] = XBEE_ZONE_LOCK_RESULT;
	msg.data[1] = zone;


	if(zones[zone].state == ZS_OtherTryLock) {  // L'autre robot veut aller dans une de nos zones
			if(1){ // Fonction de demande de prise de zone
				msg.data[2] = TRUE;
				zones[zone].state = ZS_OwnedByOther;  // Si nous avons renvoyer TRUE, l'autre robot va donc occuper cette zone
			}else{
				msg.data[2] = FALSE;
				zones[zone].state = ZS_Free; // La zone nous appartient pas donc repasse en libre
			}

	}else{
		msg.data[2] = FALSE;
		zones[zone].state = ZS_Free;
	}

	msg.size = 3;

	CANMsgToXbee(&msg,FALSE);
}
