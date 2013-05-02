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

#define RESPONSE_WAIT_TIMEOUT	3000
#define RETRY_TIMEOUT			300

//Zone ou les 2 robots peuvent passer, donc on doit éviter le cas ou les 2 robots sont en même temps dans la même zone
static zone_state_e zones[ZONE_NUMBER];
static const zone_info_t ZONE_INITIAL_STATE[ZONE_NUMBER] = ZONE_INITIAL_STATE_DATA;

static void ZONE_send_lock_request(map_zone_e zone);
static void ZONE_send_lock_response(map_zone_e zone);

void ZONE_init() {
	Uint8 i;
	robot_id_e me = QS_WHO_AM_I_get();

	for(i = 0; i < ZONE_NUMBER; i++) {
		if(ZONE_INITIAL_STATE[i].init_state == ZIS_Free)
			zones[i] = ZS_Free;
		else if((ZONE_INITIAL_STATE[i].init_state == ZIS_Krusty && me == KRUSTY) ||
				(ZONE_INITIAL_STATE[i].init_state == ZIS_Tiny   && me == TINY))
		{
			zones[i] = ZS_OwnedByMe;
		}
		else
			zones[i] = ZS_OwnedByOther;
	}
}

Uint8 try_lock_zone(map_zone_e zone, Uint8 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state) {
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
	enum state_e {
		TL_INIT,
		TL_SEND_REQUEST,
		TL_WAIT_RESPONSE,
		TL_LOCKED,
		TL_TIMEOUT,
		TL_NO_RESPONSE
	};
	static enum state_e state;

	static time32_t last_try_time;
	static time32_t begin_lock_time;

	switch(state) {
		case TL_INIT:
			last_try_time = 0;
			begin_lock_time = global.env.match_time;
			if(zones[zone] == ZS_OwnedByMe) {
				state = TL_LOCKED;
			} else {
				state = TL_SEND_REQUEST;
			}
			break;

		case TL_SEND_REQUEST:
			last_try_time = global.env.match_time;
			zones[zone] = ZS_Acquiring;
			ZONE_send_lock_request(zone);
			state = TL_WAIT_RESPONSE;
			break;

		case TL_WAIT_RESPONSE:
			if(zones[zone] == ZS_OwnedByMe) {		//C'est bon, on a verrouillé la zone pour nous
				state = TL_LOCKED;
			} else if(zones[zone] == ZS_OwnedByOther && global.env.match_time >= last_try_time + RETRY_TIMEOUT) {
				//La zone est verrouillée par l'autre robot, on peut pas passer, on retente après un certain temps si on a pas mis timeout_ms à 0
				if(timeout_ms > 0)
					state = TL_SEND_REQUEST;
				else state = TL_TIMEOUT;
			} else if(zones[zone] == ZS_Acquiring && global.env.match_time >= last_try_time + RESPONSE_WAIT_TIMEOUT) {
				//On a pas eu de réponse depuis trop de temps, l'autre robot ne répond pas ...
				state = TL_NO_RESPONSE;
			} else if(timeout_ms && global.env.match_time >= begin_lock_time + timeout_ms) {
				//On est en train de tenter de verrouiller la zone depuis trop longtemps
				state = TL_TIMEOUT;
			}
			break;

		case TL_LOCKED:
			state = TL_INIT;
			return END_OK;

		case TL_TIMEOUT:
			state = TL_INIT;
			return NOT_HANDLED;

		case TL_NO_RESPONSE:
			state = TL_INIT;
			return END_WITH_TIMEOUT;
	}

	return IN_PROGRESS;
}

void ZONE_unlock(map_zone_e zone) {
	if(zones[zone] == ZS_OwnedByMe) {
		CAN_msg_t msg;
		msg.sid = XBEE_ZONE_COMMAND;
		msg.data[0] = XBEE_ZONE_UNLOCK;
		msg.data[1] = zone;
		msg.size = 2;
		CAN_send(&msg);
		zones[zone] = ZS_Free;
	} else {
		debug_printf("zone: unlock zone %d not owned !!!, state = %d\n", zone, zones[zone]);
	}
}

bool_e ZONE_is_free(map_zone_e zone) {
	return zones[zone] == ZS_Free || zones[zone] == ZS_OwnedByMe;
}

zone_state_e ZONE_get_status(map_zone_e zone) {
	return zones[zone];
}

void ZONE_CAN_process_msg(CAN_msg_t *msg) {
	if(((msg->sid & 0xFF0) == STRAT_XBEE_FILTER) &&		//Si c'est un message reçu par XBEE
	   ((msg->sid & 0x00F) == (XBEE_ZONE_COMMAND & 0x00F)))	//Et que le SID correspond à une commande liée au zones
	{
		if(msg->data[1] >= ZONE_NUMBER) {
			debug_printf("zone: unknown zone %d !!!\n", msg->data[1]);
			return;
		}

		switch(msg->data[0]) {
			case XBEE_ZONE_LOCK_RESULT:
				if(zones[msg->data[1]] == ZS_Acquiring) {
					if(msg->data[2] == TRUE)
						zones[msg->data[1]] = ZS_OwnedByMe;
					else zones[msg->data[1]] = ZS_OwnedByOther;
				} else {
					debug_printf("zone: INCOHERENT STATE !! zones[%d] = %d, but response = %d\n", msg->data[1], zones[msg->data[1]], msg->data[2]);
				}
				break;

			case XBEE_ZONE_TRY_LOCK:
				ZONE_send_lock_response(msg->data[1]);
				break;

			case XBEE_ZONE_UNLOCK:
				if(zones[msg->data[1]] == ZS_OwnedByOther)
					zones[msg->data[1]] = ZS_Free;
				break;
		}
	}
}

static void ZONE_send_lock_request(map_zone_e zone) {
	CAN_msg_t msg;

	msg.sid = XBEE_ZONE_COMMAND;
	msg.data[0] = XBEE_ZONE_TRY_LOCK;
	msg.data[1] = zone;
	msg.size = 2;

	CAN_send(&msg);
}

static void ZONE_send_lock_response(map_zone_e zone) {
	CAN_msg_t msg;

	msg.sid = XBEE_ZONE_COMMAND;
	msg.data[0] = XBEE_ZONE_LOCK_RESULT;
	msg.data[1] = zone;

	//L'autre robot veut verrouiller la zone, mais nous aussi !!!
	//On autorise le propriétaire (il faut surtout que ce soit le même pour les 2 robots, et pas que les 2 robots disent Ok a l'autre)
	if(zones[zone] == ZS_Acquiring) {
		//Si on est le robot propriétaire de la zone, on passe, sinon on passe pas
		if(QS_WHO_AM_I_get() == ZONE_INITIAL_STATE[zone].owner) {
			msg.data[2] = TRUE;
			zones[zone] = ZS_OwnedByMe;
		} else {
			msg.data[2] = FALSE;
			zones[zone] = ZS_OwnedByOther;
		}
	} else if(zones[zone] != ZS_OwnedByMe) {	//ZS_Free || ZS_OwnedByOther
		msg.data[2] = TRUE;
		zones[zone] = ZS_OwnedByOther;
	} else msg.data[2] = FALSE;					//ZS_OwnedByMe

	msg.size = 3;

	CAN_send(&msg);
}
