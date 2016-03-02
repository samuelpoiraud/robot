

#include "zones.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_can_over_xbee.h"
#include "state_machine_helper.h"

#define LOG_PREFIX "zones: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES
#include "../QS/QS_outputlog.h"


#define RESPONSE_WAIT_TIMEOUT	1000
#define RETRY_TIMEOUT			400

static zone_meta_t zones[NB_ZONES];

// Déclarations des fonctions internes
static void ZONE_send_lock_response(zone_id_e zone);
static void initZone(zone_id_e id, rectangle_t rectangle, zone_owner_e owner, zone_state_e state, Uint16 timeout);
bool_e ZONE_validate(zone_id_e zone);

void initZones()
{
	initZone(ZONE_OUR_DOORS,		(rectangle_t){0, COLOR_Y(0), 600, COLOR_Y(800)},			ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_OUR_DUNE,			(rectangle_t){0, COLOR_Y(800), 750, COLOR_Y(1500)},			ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_DUNE,			(rectangle_t){0, COLOR_Y(1500), 750, COLOR_Y(2200)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_DOORS,		(rectangle_t){0, COLOR_Y(2200), 600, COLOR_Y(3000)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_OUR_START_ZONE,	(rectangle_t){600, COLOR_Y(0), 1100, COLOR_Y(300)},			ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_OUR_START,		(rectangle_t){600, COLOR_Y(300), 1300, COLOR_Y(800)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_OUR_DEPOSE,		(rectangle_t){750, COLOR_Y(800), 1300, COLOR_Y(1500)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_DEPOSE,		(rectangle_t){750, COLOR_Y(1500), 1300, COLOR_Y(2200)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_START,		(rectangle_t){600, COLOR_Y(2200), 1300, COLOR_Y(2700)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_START_ZONE,	(rectangle_t){600, COLOR_Y(2700), 1100, COLOR_Y(3000)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_OUR_ROCK,			(rectangle_t){1100, COLOR_Y(0), 2000, COLOR_Y(300)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_OUR_FISH,			(rectangle_t){1300, COLOR_Y(300), 2000, COLOR_Y(1500)},		ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_FISH,			(rectangle_t){1300, COLOR_Y(1500), 2000, COLOR_Y(2700)},	ZIS_BIG, ZS_Free, TIMEOUT_10s);
	initZone(ZONE_ADV_ROCK,			(rectangle_t){1100, COLOR_Y(2700), 2000, COLOR_Y(3000)},	ZIS_BIG, ZS_Free, TIMEOUT_10s);
	debug_printf("Initialisation des zones effectuee.\n");
}

static void initZone(zone_id_e id, rectangle_t rectangle, zone_owner_e owner, zone_state_e state, Uint16 timeout)
{
	debug_printf("initZone : %d (%s)\n",id,stringNameZone(id));
	assert(id < NB_ZONES);
	zones[id].rectangle = rectangle;
	zones[id].owner = owner;
	zones[id].state = state;
	zones[id].askTime = 0;
	zones[id].timeout = timeout;
}

void ZONES_process_main()
{
	int k;
	for(k=0;k<NB_ZONES;k++)
	{
		if(zones[k].askTime > 0 && zones[k].state != ZS_Free)
			if(zones[k].askTime + zones[k].timeout < global.absolute_time  && !robotIsInSquare(zones[k].rectangle))
			{
				zones[k].state = ZS_Free;
				debug_printf("Zone liberee après timeout : %s\n",stringNameZone(k));
			}
	}
}

Uint8 ZONES_try_lock_zone(zone_id_e id, Uint16 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state)
{
	switch(ZONES_try_lock(id, timeout_msec)){
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

error_e ZONES_try_lock(zone_id_e zone, Uint16 timeout_ms)
{
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
			begin_lock_time = global.match_time;
			if(zones[zone].owner == me ) {  // Si la zone est occupé par moi-même
				state = TL_LOCKED;
			} else {
				state = TL_SEND_REQUEST;
			}
			break;

		case TL_SEND_REQUEST: // Nous avons envoyer un message de prise de zone, nous attendons mnt la réponse de l'autre
			last_try_time = global.match_time;

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

			} else if(zones[zone].state == ZS_Acquiring && global.match_time >= begin_lock_time + RESPONSE_WAIT_TIMEOUT) {
				//On a pas eu de réponse depuis trop de temps, l'autre robot ne répond pas ...
				state = TL_NO_RESPONSE;

			} else if(zones[zone].state == ZS_Acquiring && global.match_time >= last_try_time + RETRY_TIMEOUT) {
				//La zone est verrouillée par l'autre robot, on retente
				state = TL_SEND_REQUEST;

			}else if(timeout_ms && global.match_time >= begin_lock_time + timeout_ms) {
				//On est en train de tenter de verrouiller la zone depuis trop longtemps
				state = TL_TIMEOUT;
			}

			break;

		case TL_LOCKED:
			state = TL_INIT;
			return END_OK;

		case TL_TIMEOUT:
			zones[zone].state = ZS_Free;
			zones[zone].accept_donate = FALSE;
			state = TL_INIT;
			return NOT_HANDLED;

		case TL_ERROR:
			state = TL_INIT;
			return NOT_HANDLED;

		case TL_NO_RESPONSE:
			zones[zone].state = ZS_Free;
			zones[zone].accept_donate = FALSE;
			state = TL_INIT;
			return END_WITH_TIMEOUT;
	}

	return IN_PROGRESS;
}

void ZONE_CAN_process_msg(CAN_msg_t *msg) {
	assert(msg->sid == XBEE_ZONE_COMMAND);	//Si le SID correspond à une commande liée aux zones

	if(msg->data.xbee_zone_command.zone >= NB_ZONES) {
		debug_printf("zone: unknown zone %d !!!\n", msg->data.xbee_zone_command.zone);
		return;
	}


	switch(msg->data.xbee_zone_command.order) {
		case XBEE_ZONE_LOCK_RESULT:

			if(msg->data.xbee_zone_command.lock == TRUE) // La zone peut-être prise par celui qui l'a demandé
				zones[msg->data.xbee_zone_command.zone].state = ZS_OwnedByMe;

			break;

		case XBEE_ZONE_TRY_LOCK:

			if(zones[msg->data.xbee_zone_command.zone].owner == ((QS_WHO_AM_I_get() == BIG_ROBOT)? ZIS_BIG : ZIS_SMALL)){
				zones[msg->data.xbee_zone_command.zone].state = ZS_OtherTryLock;
				ZONE_send_lock_response(msg->data.xbee_zone_command.zone);

			}else if(zones[msg->data.xbee_zone_command.zone].owner == ZIS_Free)				// Si l'autre robot prend une zone libre
				zones[msg->data.xbee_zone_command.zone].state = ZS_OwnedByOther; // Ne renvois pas de message le premier à la demander gagne


			break;

		case XBEE_ZONE_UNLOCK:
			if(zones[msg->data.xbee_zone_command.zone].state == ZS_OwnedByOther){
				zones[msg->data.xbee_zone_command.zone].state = ZS_Free;
				zones[msg->data.xbee_zone_command.zone].accept_donate = FALSE;
			}
			break;
	}
}

static void ZONE_send_lock_response(zone_id_e zone) {

	if(zones[zone].owner != ((QS_WHO_AM_I_get() == BIG_ROBOT)? ZIS_BIG : ZIS_SMALL)) // Si on a une demande dont la zone nous appartient pas
		return;

	CAN_msg_t msg;

	msg.sid = XBEE_ZONE_COMMAND;
	msg.size = SIZE_XBEE_ZONE_COMMAND;
	msg.data.xbee_zone_command.order = XBEE_ZONE_LOCK_RESULT;
	msg.data.xbee_zone_command.zone = zone;


	if(zones[zone].state == ZS_OtherTryLock) {  // L'autre robot veut aller dans une de nos zones
		if(ZONE_validate(zone)){ // Fonction de demande de prise de zone
				msg.data.xbee_zone_command.lock = TRUE;
				zones[zone].state = ZS_OwnedByOther;  // Si nous avons renvoyer TRUE, l'autre robot va donc occuper cette zone
			}else{
				msg.data.xbee_zone_command.lock = FALSE;
				zones[zone].state = ZS_Free; // La zone nous appartient pas donc repasse en libre
				zones[zone].accept_donate = FALSE;
			}

	}else{
		msg.data.xbee_zone_command.lock = FALSE;
		zones[zone].state = ZS_Free;
	}

	CANMsgToXbee(&msg,FALSE);
}

void ZONE_send_lock_request(zone_id_e zone) {
	CAN_msg_t msg;

	msg.sid = XBEE_ZONE_COMMAND;
	msg.size = SIZE_XBEE_ZONE_COMMAND;
	msg.data.xbee_zone_command.order = XBEE_ZONE_TRY_LOCK;
	msg.data.xbee_zone_command.zone = zone;

	CANMsgToXbee(&msg,FALSE);
}

zone_owner_e getZoneOwner(zone_id_e id)
{
	assert(id < NB_ZONES);
	return zones[id].owner;
}

zone_state_e getZoneState(zone_id_e id)
{
	assert(id < NB_ZONES);
	return zones[id].state;
}

void setZoneTimeout(zone_id_e id, time32_t timeout)
{
	assert(id < NB_ZONES);
	zones[id].timeout = timeout;
}

void setZoneAskTime(zone_id_e id, time32_t askTime)
{
	assert(id < NB_ZONES);
	zones[id].askTime = askTime;
}

bool_e robotIsInSquare(rectangle_t rectangle)
{
	return is_in_square(rectangle.x1,rectangle.x2,COLOR_Y(rectangle.y1),COLOR_Y(rectangle.y2),(GEOMETRY_point_t){global.pos.x, global.pos.y});
}

bool_e ZONE_validate(zone_id_e zone){
	return zones[zone].accept_donate;
}

bool_e checkZones(zones_list_t list)
{
	bool_e freeZone = TRUE;
	Uint8 k;
	zone_owner_e other = ((QS_WHO_AM_I_get() == BIG_ROBOT)? ZIS_BIG : ZIS_SMALL); // L'autre robot
	if(NO_ZONE)
		return TRUE;
	for(k=0;k<NB_ZONES;k++)
	{
		if(((list>>k) & 0x0001) && (zones[k].owner = other))
			freeZone = FALSE;
	}
	return freeZone;
}

char* stringNameZone(zone_id_e id)
{
	switch(id)
	{
		case ZONE_OUR_DOORS:
			return "ZONE_OUR_DOORS";
		case ZONE_OUR_DUNE:
			return "ZONE_OUR_DUNE";
		case ZONE_ADV_DUNE:
			return "ZONE_ADV_DUNE";
		case ZONE_ADV_DOORS:
			return "ZONE_ADV_DOORS";
		case ZONE_OUR_START_ZONE:
			return "ZONE_OUR_START_ZONE";
		case ZONE_OUR_START:
			return "ZONE_OUR_START";
		case ZONE_OUR_DEPOSE:
			return "ZONE_OUR_DEPOSE";
		case ZONE_ADV_DEPOSE:
			return "ZONE_ADV_DEPOSE";
		case ZONE_ADV_START:
			return "ZONE_ADV_START";
		case ZONE_ADV_START_ZONE:
			return "ZONE_ADV_START_ZONE";
		case ZONE_OUR_ROCK:
			return "ZONE_OUR_ROCK";
		case ZONE_OUR_FISH:
			return "ZONE_OUR_FISH";
		case ZONE_ADV_FISH:
			return "ZONE_ADV_FISH";
		case ZONE_ADV_ROCK:
			return "ZONE_ADV_ROCK";
		default:
			return "UNKNOW_ZONE";
	}
}
