#include "QS_rf.h"

#ifdef USE_RF

#include "QS_uart.h"

#define RF_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
#define RF_TEMP_CONCAT(a,b,c) RF_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)

#if RF_UART <= 0 || RF_UART > 3
#error "RF_UART est invalide: il doit être défini à une valeur entre 1 et 3 inclus"
#else
#define RF_UART_init() UART_init()
#define RF_UART_send(c) RF_TEMP_CONCAT(UART, RF_UART, _putc(c))
#define RF_UART_recv() RF_TEMP_CONCAT(UART, RF_UART, _get_next_msg())
#define RF_UART_is_data_available() RF_TEMP_CONCAT(UART, RF_UART, _data_ready())
#endif

//Arbitraire, doivent être des nombres rarement utilisés pour éviter d'avoir trop d'occurence
#define END_OF_PACKET_CHAR 0x94
#define ESCAPE_CHAR 0x95

//config only one time, internal ram has 10k cycles ...
/*
static void RF_config_set_channel(Uint8 channel);
static void RF_config_set_output_power(Uint8 power);
static void RF_config_set_internal_buffer_size(Uint8 size);
static void RF_config_set_buffer_timeout(Uint16 ms);
static void RF_config_set_packed_end_char(char c);
static void RF_config_set_unique_id(Uint8 id);
static void RF_config_set_system_id(Uint8 id);
static void RF_config_set_broadcast_id(Uint8 id);
static void RF_config_set_destination_id(Uint8 id);
*/
static void RF_data_send(RF_module_e target_id, const Uint8 *data, Uint8 size);

typedef enum {
	RF_PT_SynchroRequest,
	RF_PT_SynchroResponse,
	RF_PT_Can
} RF_packet_type_e;



typedef union {
	Uint8 raw_data;
	struct {
		RF_packet_type_e type : 2;
		RF_module_e sender_id : 3;
		RF_module_e target_id : 3;
	};
} RF_synchro_request_header_t;

typedef union {
	Uint8 raw_data[2];
	struct {
		RF_packet_type_e type : 2;
		RF_module_e sender_id : 3;
		RF_module_e target_id : 3;
		unsigned char timer_diff : 8;
	};
} RF_synchro_response_header_t;

typedef union {
	Uint8 raw_data[2];
	struct {
		RF_packet_type_e type : 2;
		RF_module_e sender_id : 3;
		RF_module_e target_id : 3;
		unsigned char size : 8;
	};
} RF_data_header_t;

void RF_init() {
	RF_UART_init();
}

static void RF_data_send(RF_module_e target_id, const Uint8 *data, Uint8 size) {
	RF_data_header_t packet_header;
	Uint8 i;

	packet_header.type = RF_PT_Can;
	packet_header.sender_id = RF_MODULE;
	packet_header.target_id = target_id;
	packet_header.size = size;
	RF_UART_send(packet_header.raw_data[0]);
	RF_UART_send(packet_header.raw_data[1]);

	for(i = 0; i < size; i++) {
		RF_UART_send(data[i]);
	}
}

void RF_can_send(RF_module_e target_id, CAN_msg_t *msg) {
	Uint8 data[10];
	Uint8 i;

	if(msg->size > 8)
		msg->size = 8;

	data[0] = msg->sid & 0xFF;
	data[1] = msg->sid >> 8;

	for(i = 0; i < msg->size; i++) {
		data[i+2] = msg->data[i];
	}
	for(; i < 8; i++)
		data[i+2] = 0;

	RF_data_send(target_id, data, 10);
}

#endif
