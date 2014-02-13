#include "SynchroRF.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"

static CAN_msg_t canmsg_pending;
static bool_e canmsg_received = FALSE;
static Sint16 offset;

extern volatile Uint8 step; //dans EmmissionIR.c

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);

void SYNCRF_init() {
	//RF_init(RF_MODULE, &rf_packet_received_callback, &rf_can_received_callback);
}

void SYNCRF_process_main()
{
	/*static Uint8 compteur_last;
	if(canmsg_received) {
		//Traitement message CAN
		canmsg_received = FALSE;
	}

	if(offset != 0x0FFF) {
		info_printf("Offset: %d\n", offset);
		offset = 0x0FFF;
	}

	if(compteur_last != step/20) {
		compteur_last = step/20;

		debug_printf("Compteur: %u\n", step);
	}*/
}

void SYNCRF_sendRequest() {
	//RF_synchro_request(RF_BROADCAST);
	LED_RUN = !LED_RUN;
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	size = size;
	if(for_me && header.type == RF_PT_SynchroResponse) {
		Sint16 fullOffset = data[0] | data[1] << 8;
		offset = step - TIME_WHEN_SYNCHRO;
		step = step + fullOffset - (offset >> 1);
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
