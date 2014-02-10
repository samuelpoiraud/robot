#include "Synchro_balises.h"

#include "../QS/QS_rf.h"
#include "../QS/QS_timer.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_outputlog.h"

#ifdef SYNCHROBALISE_TIMER
	#define TIMER_SRC_TIMER_ID SYNCHROBALISE_TIMER
#endif
#include "../QS/QS_setTimerSource.h"

#define COMPTEUR_MAX 10000
#define COMPTEUR_TICK_PER_USEC 100

#define RF_MODULE_COUNT 4
#define TIME_PER_MODULE 100
#define LAST_REPLY_TIMEOUT 10000  //temps avant de considérer le robot maitre (pierre) comme éteint (dans ce cas, guy passe en maitre)
#define LAST_SYNCHRO_TIMEOUT 10000  //temps avant de considérer une balise comme éteinte

bool_e balise_here[2] = {FALSE, FALSE};

static CAN_msg_t canmsg_pending;
static bool_e canmsg_received = FALSE;
static Sint16 offset;
static time32_t last_received_reply = 0; //si ça fait longtemps, pierre est off

//temps depuis la dernière requête de synchro
static time32_t last_req_time[RF_MODULE_COUNT];

static bool_e SEND_REQ = TRUE;
static bool_e REPLY_REQ = TRUE;

static Uint16 time_base;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);
static void update_foe_here();

/*void TIMER_SRC_TIMER_interrupt() {
	TIMER_SRC_TIMER_resetFlag();

	time_base++;
	if(time_base >= COMPTEUR_MAX) {
		time_base=0;
	}

	//Un timeslot pour chaque module différent, pour éviter les collisions lorsque tout les modules sont presque synchro
	if(SEND_REQ && time_base == TIME_PER_MODULE*RF_get_module_id()) {
		RF_synchro_request(RF_BROADCAST);
	}

}*/

//static const char* rf_packet_type_str[4] = {
//	"RF_PT_SynchroRequest",
//	"RF_PT_SynchroResponse",
//	"RF_PT_Can",
//	"RF_PT_None"
//};

void SYNCHRO_init() {
	return;
	TIMER_SRC_TIMER_init();

	if(QS_WHO_AM_I_get() == PIERRE) {
		SEND_REQ = FALSE;
		REPLY_REQ = TRUE;
		RF_init(RF_PIERRE, &rf_packet_received_callback, &rf_can_received_callback);
	} else {
		SEND_REQ = TRUE;		//Pour voir si il y a PIERRE
		REPLY_REQ = FALSE;
		RF_init(RF_GUY, &rf_packet_received_callback, &rf_can_received_callback);
	}

	TIMER_SRC_TIMER_start_us(COMPTEUR_TICK_PER_USEC);
}

void SYNCHRO_process_main()
{
	return;
		static Uint16 compteur_last;
		if(canmsg_received) {
			//ENV_process_can_msg(&canmsg_pending);

			canmsg_received = FALSE;
		}

		//Si on voit pas de réponse pendant plus de LAST_REPLY_TIMEOUT ms, on répond au demandes e synchro (si pierre pas là)
		if(REPLY_REQ == FALSE && RF_get_module_id() == RF_GUY && last_received_reply + LAST_REPLY_TIMEOUT <= global.env.absolute_time) {
			REPLY_REQ = TRUE;
			debug_printf("La derniere réponse reçu de pierre est trop vielle, passage en maitre\n");
		}

		update_foe_here();

		if(offset != 0x0FFF) {
			info_printf("Offset: %d\n", offset);
			offset = 0x0FFF;
		}

		if(compteur_last != time_base/1000) {
			compteur_last = time_base/1000;

			debug_printf("Compteur: %u\n", time_base);
		}
		if(time_base < 9900)
		;//RF_can_send(RF_BROADCAST, &msg);
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	if(REPLY_REQ && for_me && header.type == RF_PT_SynchroRequest) {
		Sint16 offset;
		if(time_base > COMPTEUR_MAX/2)
			offset = time_base - COMPTEUR_MAX;
		else
			offset = time_base;
		RF_synchro_response(header.sender_id, offset);
		last_req_time[header.target_id] = global.env.absolute_time;
		update_foe_here();
	} else if(QS_WHO_AM_I_get() == GUY && header.type == RF_PT_SynchroResponse) {
		//On ne se synchronise pas sur PIERRE, c'est nous le maitre
		if(for_me) {
			Sint16 fullOffset = data[0] | data[1] << 8;
			offset = time_base;
			time_base = time_base + fullOffset - (time_base >> 1);
		}
		if(REPLY_REQ) {
			REPLY_REQ = FALSE;
			debug_printf("Réponse reçue de PIERRE, passage en esclave\n");
		}
		last_received_reply = global.env.absolute_time;
	}
}

static void rf_can_received_callback(CAN_msg_t *msg) {
	if(canmsg_received == FALSE) {
		canmsg_pending = *msg;
		canmsg_received = TRUE;
	} else {
		//overflow, mais le rf est lent (si c'est le cas, alors la boucle while du main est lente aussi)
	}
}

static void update_foe_here() {
	balise_here[0] = (last_req_time[RF_FOE1] + LAST_SYNCHRO_TIMEOUT >= global.env.absolute_time);
	balise_here[1] = (last_req_time[RF_FOE2] + LAST_SYNCHRO_TIMEOUT >= global.env.absolute_time);
}

//void MAIN_onButton1() {
//	LED_RUN = !LED_RUN;

//	RF_synchro_request(RF_BROADCAST);
//	info_printf("Button1\n");
//}

//void MAIN_onButton2() {
//	LED_RUN = !LED_RUN;

//	CAN_msg_t msg;
//	msg.sid = 0x1234;
//	msg.size = 4;
//	msg.data[0] = 0xbf;
//	msg.data[1] = 0xc0;
//	msg.data[2] = 0xc1;
//	msg.data[3] = 0xcf;
//	RF_can_send(RF_BROADCAST, &msg);
//	info_printf("Button2\n");
//}

//void MAIN_onButton3() {
//	LED_RUN = !LED_RUN;

//	CAN_msg_t msg;
//	msg.sid = 0x1234;
//	msg.size = 4;
//	msg.data[0] = 'B';
//	msg.data[1] = 'U';
//	msg.data[2] = 'T';
//	msg.data[3] = '3';
//	RF_can_send(RF_BROADCAST, &msg);
//	info_printf("Button3\n");
//}

//void MAIN_onButton4() {
//	LED_RUN = !LED_RUN;

//	send_req = !send_req;
//	info_printf("Button4, %d\n", send_req);
//}
