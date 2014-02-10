#include "SynchroRF.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"

static CAN_msg_t canmsg_pending;
static bool_e canmsg_received = FALSE;
static Sint16 offset;

extern volatile Uint8 step; //dans EmmissionIR.c

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);
static void update_foe_here();

//static const char* rf_packet_type_str[4] = {
//	"RF_PT_SynchroRequest",
//	"RF_PT_SynchroResponse",
//	"RF_PT_Can",
//	"RF_PT_None"
//};

void SYNCRF_init() {
	RF_init(RF_MODULE, &rf_packet_received_callback, &rf_can_received_callback);
}

void SYNCRF_process_main()
{
		static Uint8 compteur_last;
		if(canmsg_received) {
			//ENV_process_can_msg(&canmsg_pending);

			canmsg_received = FALSE;
		}

		update_foe_here();

		if(offset != 0x0FFF) {
			info_printf("Offset: %d\n", offset);
			offset = 0x0FFF;
		}

		if(compteur_last != step/20) {
			compteur_last = step/20;

			debug_printf("Compteur: %u\n", time_base);
		}
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	if(header.type == RF_PT_SynchroRequest) {
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
