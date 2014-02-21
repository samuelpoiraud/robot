#include "SynchroRF.h"
#include "Synchro.h"
#include "EmissionIR.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/impl/QS_uart_impl.h"

static CAN_msg_t canmsg_pending;
static bool_e canmsg_received = FALSE;
static Sint16 offset;
static bool_e request_synchro = FALSE;
static bool_e synchro_received = FALSE;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);

void SYNCRF_init() {
	global.is_synchronized = FALSE;
	PORT_RFCONFIG = 1;
	RF_init(RF_MODULE, &rf_packet_received_callback, &rf_can_received_callback);
	UART_IMPL_write(RF_UART, 'X');
	TIMER3_run_us(DUREE_STEP);
}

void _ISR _T3Interrupt() {
	EmissionIR_next_step();
	IFS0bits.T3IF = 0;
}

void SYNCRF_process_main() {
	static Uint8 compteur_last;
	if(request_synchro) {
		request_synchro = FALSE;
		debug_printf("Retard demande: %d\n", step_ir - TIME_WHEN_SYNCHRO);
	}

	if(canmsg_received) {
		//Traitement message CAN
		canmsg_received = FALSE;
	}

	if(offset != 0x0FFF) {
		info_printf("Offset: %d\n", offset);
		offset = 0x0FFF;
	}

	if(compteur_last != step_ir/50) {
		compteur_last = step_ir/50;

		//debug_printf("Compteur: %u\n", step_ir);
	}
}

void SYNCRF_sendRequest() {
	//synchro dans le main pour éviter les trucs bizarre avec le buffering de l'uart dans QS_rf
	//condition: boucle main rapide (à priori rien de lent, et on synchronise à une précision de 2ms)
	//LED_RUN = !LED_RUN;
	RF_synchro_request(RF_BROADCAST);
	request_synchro = TRUE;
	if(synchro_received) {
		LED_RUN = !LED_RUN;
		LED_USER = FALSE;
	} else {
		LED_USER = !LED_USER;
		LED_RUN = FALSE;
	}
	synchro_received = FALSE;
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	size = size;
	if(for_me && header.type == RF_PT_SynchroResponse) {
		const Uint16 elapsed_time_since_request = step_ir - TIME_WHEN_SYNCHRO;

		//unités: tick / (us/localtick / us/tick) = tick * us/tick / us/localtick = us/(us/localtick) = localtick
		Sint16 fullOffset = (data[0] | data[1] << 8) / (DUREE_STEP / TIME_BASE_UNIT);

		offset = fullOffset - (elapsed_time_since_request >> 1) + 2; // fullOffset - elapsed_time_since_request/2 + erreur systématique
		step_ir += offset;

		//LED_USER = !LED_USER;

		//Synchro reçue, donc on peut passer en mode double adversaire
		global.is_synchronized = TRUE;
		synchro_received = TRUE;
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
