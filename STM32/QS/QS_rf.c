#include "QS_rf.h"

#ifdef USE_RF

#include "stm32f4xx_usart.h"
#include "QS_uart.h"
#include "QS_buffer_fifo.h"

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
//Tous doivent être >= 0xC0 && <= 0xFF
#define ESCAPE_CHAR 0xC0
#define END_OF_PACKET_CHAR 0xC1
#define START_OF_PACKET_CHAR 0xC3 //DOIT ETRE 0x?3 !

static FIFO_t fifo_rx;
static char buffer_rx[50];
Uint8 expected_rx_bytes;

static FIFO_t fifo_tx;
static char buffer_tx[50];

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

typedef enum {
	RF_PT_SynchroRequest,
	RF_PT_SynchroResponse,
	RF_PT_Can,
	RF_PT_None = 3
} RF_packet_type_e;

typedef struct {
	RF_packet_type_e type : 2;
	RF_module_e sender_id : 3;
	RF_module_e target_id : 3;
} RF_header_t;

typedef union {
	Uint8 raw_data;
	struct {
		RF_header_t header : 8;
	};
} RF_synchro_request_header_t;

typedef union {
	Uint8 raw_data[2];
	struct {
		RF_header_t header : 8;
		unsigned char timer_offset : 8;
	};
} RF_synchro_response_header_t;

typedef union {
	Uint8 raw_data[2];
	struct {
		RF_header_t header : 8;
		unsigned char size : 8;
	};
} RF_data_header_t;

static void RF_send(RF_packet_type_e type, RF_module_e target_id, const Uint8 *data, Uint8 size);
static void RF_UART3_putc(Uint8 c);


void RF_init() {
	UART_init();
	expected_rx_bytes = 0;
	FIFO_init(&fifo_rx, buffer_rx, 50, 1);
	FIFO_init(&fifo_tx, buffer_tx, 50, 1);
}

static void RF_send(RF_packet_type_e type, RF_module_e target_id, const Uint8 *data, Uint8 size) {
	RF_header_t packet_header;
	Uint8 i;

	RF_UART3_putc(START_OF_PACKET_CHAR);
	packet_header.type = type;
	packet_header.sender_id = RF_MODULE;
	packet_header.target_id = target_id;
	RF_UART3_putc((Uint8)packet_header);

	for(i = 0; i < size; i++) {
		Uint8 c = data[i];
		if(c == ESCAPE_CHAR || c == START_OF_PACKET_CHAR || c == END_OF_PACKET_CHAR) {
			RF_UART3_putc(ESCAPE_CHAR);
			RF_UART3_putc(c & (~0xC0));
		} else {
			RF_UART3_putc(data[i]);
		}
	}
	RF_UART3_putc(END_OF_PACKET_CHAR);
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

	RF_send(RF_PT_Can, target_id, data, 10);
}

void RF_synchro_request(RF_module_e target_id) {
	RF_synchro_request_header_t packet_header;

	packet_header.type = RF_PT_SynchroRequest;
	packet_header.sender_id = RF_MODULE;
	packet_header.target_id = target_id;
	RF_UART3_putc(packet_header.raw_data);
}

void RF_synchro_response(RF_module_e target_id, Uint8 timer_offset) {
	RF_synchro_response_header_t packet_header;

	packet_header.type = RF_PT_SynchroResponse;
	packet_header.sender_id = RF_MODULE;
	packet_header.target_id = target_id;
	packet_header.timer_offset = timer_offset;
	RF_UART3_putc(packet_header.raw_data[0]);
	RF_UART3_putc(packet_header.raw_data[1]);
}

static bool_e RF_recv(Uint8 *c) {
	static bool_e escape_mode = FALSE;

	if(escape_mode) {
		*c = *c | 0xC0;
		escape_mode = FALSE;
	} else if(c == ESCAPE_CHAR) {
		escape_mode = TRUE;
		return FALSE;
	}

	return TRUE;
}

RF_packet_type_e RF_check_packet_type(Uint8 *data, Uint8 size) {
	if(size <= 0)
		return RF_PT_None;

	RF_header_t header = (RF_header_t) data[0];

	if(header.sender_id == RF_BROADCAST)
		return RF_PT_None;

	switch(header.type) {
		case RF_PT_SynchroRequest:
			return header.type;

		case RF_PT_SynchroResponse:
			if(size > 1)
				return header.type;

		case RF_PT_Can:
			if(size > 10)
				return header.type;


		default:
			return RF_PT_None;
	}

	return RF_PT_None;
}

void RF_state_machine(Uint8 c, bool_e new_frame) {
	typedef enum {
		RFS_IDLE,
		RFS_GET_SENDER,
		RFS_GET_TARGET,
		RFS_GET_DATA
	} RF_status_e;

	static Uint8 data[20];
	static RF_status_e state = RFS_IDLE;
	static Uint8 expected_data_bytes = 0;
	static Uint8 i;

	switch(state) {
		case RFS_IDLE:
			if(new_frame) {
				expected_data_bytes = 0;
				i = 0;
				state = RFS_GET_DATA;
			}
			break;

		case RFS_GET_DATA:
			data[i] = c;
			i++;
			if(i == 1) {
				RF_header_t header = (RF_header_t)data[0];
				switch(header.type) {
					case RF_PT_SynchroRequest:
						expected_data_bytes = 1;
						break;

					case RF_PT_SynchroResponse:
						expected_data_bytes = 2;
						break;

					case RF_PT_Can:
						expected_data_bytes = 3;
						break;
				}
			}
			if(i > 0 && i >= expected_data_bytes) {
			}
			break;
	}
}

static void RF_UART3_putc(Uint8 c)
{
	if(USART_GetFlagStatus(USART3, USART_IT_TXE))
		USART_SendData(USART3, c);
	else
	{
		bool_e byte_sent = FALSE;
		//mise en buffer + activation IT U3TX.
		while(!byte_sent) {
			while(FIFO_isFull(&fifo_tx));	//ON BLOQUE ICI

			//Critical section (Interrupt inibition)

			NVIC_DisableIRQ(USART3_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

			if(!FIFO_isFull(&fifo_tx))
			{
				FIFO_insertData(&fifo_tx, &c);
				byte_sent = TRUE;
			}
			//Si en fait c'est toujours full (une IT qui a fait un putc pendant ce putc), on retentera

			NVIC_EnableIRQ(USART3_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		}
	}
}

void _ISR USART3_IRQHandler(void) {
	if(USART_GetITStatus(USART3, USART_IT_RXNE)) {
		Uint8 c;

		while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE))
		{
			c = USART_ReceiveData(USART3);
			if(c == START_OF_PACKET_CHAR)
				RF_state_machine(c, TRUE);
			else if(RF_recv(&c))
				RF_state_machine(c, FALSE);
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		NVIC_ClearPendingIRQ(USART3_IRQn);
	}
	if(USART_GetITStatus(USART3, USART_IT_TXE)) {
		//debufferiser.
		if(!FIFO_isEmpty(&fifo_tx))
		{
			Uint8 *c;
			c = (Uint8*)FIFO_getData(&fifo_tx);
			if(c)
				USART_SendData(USART3, *c);

		}
		else if(FIFO_isEmpty(&fifo_tx))
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
	}
}

#endif
