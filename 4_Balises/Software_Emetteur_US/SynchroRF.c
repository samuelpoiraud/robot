#include "SynchroRF.h"
#include "EmissionIR.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/impl/QS_uart_impl.h"

static bool_e synchro_received;
static Uint16 request_duration;
static Sint16 offset;
static Uint8 failed_synchro_count;

static bool_e canmsg_received;
static CAN_msg_t canmsg_pending;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);

void SYNCRF_init() {
	global.is_synchronized = FALSE;

	synchro_received = FALSE;
	request_duration = 0;
	offset = 0x0FFF;
	failed_synchro_count = 0;

	canmsg_received = FALSE;

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
	//Pour information, si on est pas synchro on allume la led uart
	LED_UART = !global.is_synchronized;

	if(canmsg_received) {
		//Traitement message CAN
		canmsg_received = FALSE;
	}

	if(request_duration > 0) {
		info_printf("Req duration: %dms\n", request_duration * DUREE_STEP);
		request_duration = 0;
	}

	if(offset != 0x0FFF) {
		info_printf("Offset: %dms\n", offset * DUREE_STEP);
		offset = 0x0FFF;
	}
}

void SYNCRF_sendRequest() {
	//LED_RUN = !LED_RUN;
	RF_synchro_request(RF_BROADCAST);

	//Verif de la synchro précédente (on fait la demande avant, au plus près de l'IT, la réponse reviendra dans assez longtemps (80ms), on a le temps

	if(synchro_received) {
		LED_RUN = !LED_RUN;
		LED_USER = 0;
		failed_synchro_count = 0;
		global.is_synchronized = TRUE; //Synchro reçue, donc on peut passer en mode double adversaire
	} else if(failed_synchro_count < (SYNCHRO_TIMEOUT/TOTAL_STEP_COUNT)) {
		LED_USER = !LED_USER;
		LED_RUN = 0;
		failed_synchro_count++;
	} else {
		//Si on a fail trop de fois à suivre, on indique qu'on est plus synchro (et donc passage en mode 1 adversaire, on emet des IR tout le temps)
		global.is_synchronized = FALSE;
		LED_USER = !LED_USER;
		LED_RUN = 0;
	}
	synchro_received = FALSE;
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	size = size;
	if(for_me && header.type == RF_PT_SynchroResponse) {
		if(step_ir > TIME_WHEN_SYNCHRO)
			request_duration = step_ir - TIME_WHEN_SYNCHRO;
		else
			request_duration = step_ir + TOTAL_STEP_COUNT - TIME_WHEN_SYNCHRO;

		//unités: tick / (us/localtick / us/tick) = tick * us/tick / us/localtick = us/(us/localtick) = localtick
		Sint16 fullOffset = (data[0] | data[1] << 8) / (DUREE_STEP / TIME_BASE_UNIT);

		offset = fullOffset - (request_duration >> 1) + 2; // fullOffset - request_duration/2 + erreur systématique (en step)
		const Sint16 adjusted = ((Sint16)step_ir) + offset;

		if(adjusted > TOTAL_STEP_COUNT)
			step_ir = adjusted - TOTAL_STEP_COUNT;
		else if(adjusted > 0)
			step_ir = adjusted;
		else
			step_ir = adjusted + TOTAL_STEP_COUNT;

		//LED_USER = !LED_USER;

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
