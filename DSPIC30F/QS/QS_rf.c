/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Module RF: synchro + envoi messages CAN (attention c'est très lent ! 4,8kbits/sec, 13 octets/paquet + nos données)
 *  Auteur : amurzeau
 */

#include "QS_rf.h"

#ifdef USE_RF

#include "impl/QS_uart_impl.h"
#ifdef STM32F40XX
#include "stm32f4xx_gpio.h"
#endif
#include "QS_buffer_fifo.h"
#include "QS_outputlog.h"

//>1.5 STOP BIT !!!
//CRC8 poly: 0x2F (HD=4 @data size < 120)
static const Uint8 crc8_table[256] = {
	0x00, 0x2f, 0x5e, 0x71, 0xbc, 0x93, 0xe2, 0xcd, 0x57, 0x78, 0x09, 0x26, 0xeb, 0xc4, 0xb5, 0x9a,
	0xae, 0x81, 0xf0, 0xdf, 0x12, 0x3d, 0x4c, 0x63, 0xf9, 0xd6, 0xa7, 0x88, 0x45, 0x6a, 0x1b, 0x34,
	0x73, 0x5c, 0x2d, 0x02, 0xcf, 0xe0, 0x91, 0xbe, 0x24, 0x0b, 0x7a, 0x55, 0x98, 0xb7, 0xc6, 0xe9,
	0xdd, 0xf2, 0x83, 0xac, 0x61, 0x4e, 0x3f, 0x10, 0x8a, 0xa5, 0xd4, 0xfb, 0x36, 0x19, 0x68, 0x47,
	0xe6, 0xc9, 0xb8, 0x97, 0x5a, 0x75, 0x04, 0x2b, 0xb1, 0x9e, 0xef, 0xc0, 0x0d, 0x22, 0x53, 0x7c,
	0x48, 0x67, 0x16, 0x39, 0xf4, 0xdb, 0xaa, 0x85, 0x1f, 0x30, 0x41, 0x6e, 0xa3, 0x8c, 0xfd, 0xd2,
	0x95, 0xba, 0xcb, 0xe4, 0x29, 0x06, 0x77, 0x58, 0xc2, 0xed, 0x9c, 0xb3, 0x7e, 0x51, 0x20, 0x0f,
	0x3b, 0x14, 0x65, 0x4a, 0x87, 0xa8, 0xd9, 0xf6, 0x6c, 0x43, 0x32, 0x1d, 0xd0, 0xff, 0x8e, 0xa1,
	0xe3, 0xcc, 0xbd, 0x92, 0x5f, 0x70, 0x01, 0x2e, 0xb4, 0x9b, 0xea, 0xc5, 0x08, 0x27, 0x56, 0x79,
	0x4d, 0x62, 0x13, 0x3c, 0xf1, 0xde, 0xaf, 0x80, 0x1a, 0x35, 0x44, 0x6b, 0xa6, 0x89, 0xf8, 0xd7,
	0x90, 0xbf, 0xce, 0xe1, 0x2c, 0x03, 0x72, 0x5d, 0xc7, 0xe8, 0x99, 0xb6, 0x7b, 0x54, 0x25, 0x0a,
	0x3e, 0x11, 0x60, 0x4f, 0x82, 0xad, 0xdc, 0xf3, 0x69, 0x46, 0x37, 0x18, 0xd5, 0xfa, 0x8b, 0xa4,
	0x05, 0x2a, 0x5b, 0x74, 0xb9, 0x96, 0xe7, 0xc8, 0x52, 0x7d, 0x0c, 0x23, 0xee, 0xc1, 0xb0, 0x9f,
	0xab, 0x84, 0xf5, 0xda, 0x17, 0x38, 0x49, 0x66, 0xfc, 0xd3, 0xa2, 0x8d, 0x40, 0x6f, 0x1e, 0x31,
	0x76, 0x59, 0x28, 0x07, 0xca, 0xe5, 0x94, 0xbb, 0x21, 0x0e, 0x7f, 0x50, 0x9d, 0xb2, 0xc3, 0xec,
	0xd8, 0xf7, 0x86, 0xa9, 0x64, 0x4b, 0x3a, 0x15, 0x8f, 0xa0, 0xd1, 0xfe, 0x33, 0x1c, 0x6d, 0x42
};

static inline Uint8 crc8_incremental(Uint8 crc, Uint8 data) {
	return crc8_table[crc ^ data];
}

static Uint8 crc8(Uint8 *data, Uint8 size) {
	Uint8 crc = 0;
	Uint8 i;

	if(size <= 0)
		return 0;

	for(i=0; i < size; i++) {
		crc = crc8_table[crc ^ data[i]];
	}
	return crc;
}

#if defined(RF_TIMER_ID)
	#define TIMER_SRC_TIMER_ID RF_TIMER_ID
#elif defined(RF_USE_WATCHDOG)
	#define TIMER_SRC_USE_WATCHDOG RF_USE_WATCHDOG
#endif
#include "QS_setTimerSource.h"

#define RF_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
#define RF_TEMP_CONCAT(a,b,c) RF_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)

#if !defined(RF_UART)
#error "Vous devez definir RF_UART a une valeur entre 0 et 2 (1 pour le dspic) avec 0 pour UART1"
#elif RF_UART == 0
	#define RF_RX_Interrupt RF_TEMP_CONCAT(UART, 1, _RX_Interrupt)
	#define RF_TX_Interrupt RF_TEMP_CONCAT(UART, 1, _TX_Interrupt)
#elif RF_UART == 1
	#define RF_RX_Interrupt RF_TEMP_CONCAT(UART, 2, _RX_Interrupt)
	#define RF_TX_Interrupt RF_TEMP_CONCAT(UART, 2, _TX_Interrupt)
#elif RF_UART == 2
	#define RF_RX_Interrupt RF_TEMP_CONCAT(UART, 3, _RX_Interrupt)
	#define RF_TX_Interrupt RF_TEMP_CONCAT(UART, 3, _TX_Interrupt)
#endif

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

static FIFO_t fifo_tx;
static char buffer_tx[50];
static RF_onReceive_ptr packet_received_fct = NULL;
static RF_onCanMsg_ptr canmsg_received_fct = NULL;
static bool_e canTransmitData = TRUE;
static RF_module_e currentModule;

typedef enum {
	RF_PS_Incomplete,
	RF_PS_Full,
	RF_PS_Ignore,
	RF_PS_Bad
} RF_packet_status_e;

#define RF_SYNCHRO_RESPONSE_TIMER_OFFSET 0
#define RF_CAN_SIZE 0
#define RF_CAN_SID  1
#define RF_CAN_DATA 3

#define RF_SYNCHRO_REQUEST_SIZE 0
#define RF_SYNCHRO_RESPONSE_SIZE 2
#define RF_CAN_MIN_SIZE 1
#define RF_CAN_MAX_DATA_SIZE 10

static void RF_send(RF_packet_type_e type, RF_module_e target_id, const Uint8 *data, Uint8 size);
static void RF_putc(Uint8 c);
static void RF_process_data(RF_header_t header, Uint8 *data, Uint8 size);


void RF_init(RF_module_e module, RF_onReceive_ptr onReceiveCallback, RF_onCanMsg_ptr onCanMsgCallback) {
	currentModule = module;
	packet_received_fct = onReceiveCallback;
	canmsg_received_fct = onCanMsgCallback;

#ifdef STM32F40XX
	GPIO_InitTypeDef GPIO_InitStructure;


	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);	//U3TX
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);	//U3RX

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//USART3 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//USART3 RX
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif

	UART_IMPL_init_ex(RF_UART, 19200, 10, 10, UART_I_StopBit_1_5, UART_I_Parity_None);
	UART_IMPL_setRxItEnabled(RF_UART, TRUE);
	//TIMER_SRC_TIMER_init();

	FIFO_init(&fifo_tx, buffer_tx, 50, 1);
}

RF_module_e RF_get_module_id() {
	return currentModule;
}

static void RF_putc(Uint8 c)
{
	//UART1_putc(c);

	//possible full
	if(canTransmitData && FIFO_isEmpty(&fifo_tx) && !UART_IMPL_isTxFull(RF_UART))
		UART_IMPL_write(RF_UART, c);
	else
	{
		bool_e byte_sent = FALSE;
		//mise en buffer + activation IT U3TX.
		while(!byte_sent) {
			UART_IMPL_setTxItEnabled(RF_UART, TRUE);
			UART_IMPL_setTxItPaused(RF_UART, FALSE);
			while(FIFO_isFull(&fifo_tx));	//ON BLOQUE ICI

			//Critical section (Interrupt inibition)


			UART_IMPL_setRxItPaused(RF_UART, TRUE);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.
			UART_IMPL_setTxItPaused(RF_UART, TRUE);
			if(!FIFO_isFull(&fifo_tx))
			{
				FIFO_insertData(&fifo_tx, &c);
				byte_sent = TRUE;
			} else {
				printf("bug\n");
			}
			//Si en fait c'est toujours full (une IT qui a fait un putc pendant ce putc), on retentera

			UART_IMPL_setTxItEnabled(RF_UART, TRUE);
			UART_IMPL_setRxItPaused(RF_UART, FALSE);	//On active l'IT sur TX... lors du premier caractère à envoyer...
		}
	}
	UART_IMPL_setTxItPaused(RF_UART, FALSE);
}

static void RF_send(RF_packet_type_e type, RF_module_e target_id, const Uint8 *data, Uint8 size) {
	RF_header_t packet_header;
	Uint8 i, crc = 0;

	RF_putc(START_OF_PACKET_CHAR);
	packet_header.type = type;
	packet_header.sender_id = currentModule;
	packet_header.target_id = target_id;

	crc = crc8_incremental(crc, packet_header.raw_data);
	RF_putc(packet_header.raw_data);

	for(i = 0; i < size; i++) {
		Uint8 c = data[i];
		crc = crc8_incremental(crc, c);
		if(c == ESCAPE_CHAR || c == START_OF_PACKET_CHAR || c == END_OF_PACKET_CHAR) {
			c &= (~0xC0);
			RF_putc(ESCAPE_CHAR);
			RF_putc(c);
		} else {
			RF_putc(data[i]);
		}
	}
	RF_putc(crc);
	RF_putc(END_OF_PACKET_CHAR);
}

void RF_can_send(RF_module_e target_id, CAN_msg_t *msg) {
	Uint8 data[11];
	Uint8 i;

	if(msg->size > 8)
		msg->size = 8;

	data[RF_CAN_SIZE] = msg->size + 2;
	data[RF_CAN_SID] = msg->sid & 0xFF;
	data[RF_CAN_SID+1] = msg->sid >> 8;

	for(i = 0; i < msg->size; i++) {
		data[RF_CAN_DATA+i] = msg->data[i];
	}
	for(; i < 8; i++)
		data[RF_CAN_DATA+i] = 0;

	RF_send(RF_PT_Can, target_id, data, data[RF_CAN_SIZE]+1);
}

void RF_synchro_request(RF_module_e target_id) {
	RF_send(RF_PT_SynchroRequest, target_id, NULL, 0);
}

void RF_synchro_response(RF_module_e target_id, Sint16 timer_offset) {
	RF_send(RF_PT_SynchroResponse, target_id, (Uint8*)&timer_offset, RF_SYNCHRO_RESPONSE_SIZE);
}

static bool_e RF_recv(Uint8 *c) {
	static bool_e escape_mode = FALSE;

	if(escape_mode) {
		*c = *c | 0xC0;
		escape_mode = FALSE;
	} else if(*c == ESCAPE_CHAR) {
		escape_mode = TRUE;
		return FALSE;
	}

	return TRUE;
}

static RF_packet_status_e RF_get_packet_status(RF_header_t header, Uint8 *data, Uint8 size) {
	if(header.sender_id == RF_BROADCAST)
		return RF_PS_Bad;

	//permettre de voir les transmissions entre les autres modules
//	if(header.target_id != RF_BROADCAST && header.target_id != currentModule)
//		return RF_PS_Ignore;

	switch(header.type) {
		case RF_PT_SynchroRequest:
			//Evite un warning
#if RF_SYNCHRO_REQUEST_SIZE != 0
			if(size >= RF_SYNCHRO_REQUEST_SIZE)
				return RF_PS_Full;
			else
				return RF_PS_Incomplete;
#else
			return RF_PS_Full;
#endif

		case RF_PT_SynchroResponse:
			if(size >= RF_SYNCHRO_RESPONSE_SIZE)
				return RF_PS_Full;
			else
				return RF_PS_Incomplete;

		case RF_PT_Can:
			if(size >= RF_CAN_MIN_SIZE && data[RF_CAN_SIZE] <= RF_CAN_MAX_DATA_SIZE && size >= data[RF_CAN_SIZE]+1)
				return RF_PS_Full;
			else if(size >= RF_CAN_MIN_SIZE && data[RF_CAN_SIZE] > RF_CAN_MAX_DATA_SIZE)
				return RF_PS_Bad;
			else
				return RF_PS_Incomplete;


		default:
			return RF_PS_Bad;
	}
}

void RF_state_machine(Uint8 c, bool_e new_frame) {
	typedef enum {
		RFS_IDLE,
		RFS_GET_DATA,
		RFS_GET_CRC
	} RF_status_e;

	static Uint8 data[20];
	static RF_status_e state = RFS_IDLE;
	static Uint8 i;

	switch(state) {
		case RFS_IDLE:
			if(new_frame) {
				i = 0;
				state = RFS_GET_DATA;
			}
			break;

		case RFS_GET_DATA:
			data[i++] = c;
			switch(RF_get_packet_status((RF_header_t)data[0], data+1, i-1)) {
				case RF_PS_Ignore:
				case RF_PS_Bad:
					state = RFS_IDLE;
					break;

				case RF_PS_Incomplete:
					break;

				case RF_PS_Full:
					state = RFS_GET_CRC;
					break;
			}
			break;

		case RFS_GET_CRC:
			data[i++] = c;
			if(crc8(data, i) == 0 && packet_received_fct) {
				RF_process_data((RF_header_t)data[0], data+1, i-1);
			}
			state = RFS_IDLE;
			break;
	}
}

static void RF_process_data(RF_header_t header, Uint8 *data, Uint8 size) {
	if(header.type == RF_PT_Can && (header.target_id == currentModule || header.target_id == RF_BROADCAST)) {
		if(canmsg_received_fct && size > RF_CAN_SID+1 && size > data[RF_CAN_SIZE] && data[RF_CAN_SIZE] >= 2 && data[RF_CAN_SIZE] < 8+2) {
			CAN_msg_t msg;
			Uint8 i;
			msg.size = data[RF_CAN_SIZE] - 2;

			assert(msg.size >= 8); //deja checké dans le if mais on ne sait jamais

			msg.sid = (Uint16)data[RF_CAN_SID] | (Uint16)data[RF_CAN_SID+1] << 8;

			for(i = 0; i < msg.size; i++) {
				msg.data[i] = data[RF_CAN_DATA+i];
			}
			for(; i < 8; i++)
				msg.data[i] = 0;
			(*canmsg_received_fct)(&msg);
		}
	} else if(header.type != RF_PT_Can) {
		bool_e for_me = header.target_id == RF_BROADCAST || header.target_id == currentModule;
		(*packet_received_fct)(for_me, header, data, size);
	}
}

void RF_RX_Interrupt() {
	Uint8 c;

	canTransmitData = FALSE;

	while(!UART_IMPL_isRxEmpty(RF_UART))
	{
		c = UART_IMPL_read(RF_UART);
		TIMER_SRC_TIMER_start_ms(10);

		if(c == START_OF_PACKET_CHAR)
			RF_state_machine(c, TRUE);
		else if(RF_recv(&c))
			RF_state_machine(c, FALSE);
	}


	UART_IMPL_ackRxIt(RF_UART);
}

void RF_TX_Interrupt() {
	//debufferiser si on a le droit
	if(canTransmitData) {
		if(!FIFO_isEmpty(&fifo_tx)) {
			Uint8 *c;
			c = (Uint8*)FIFO_getData(&fifo_tx);
			if(c)
				UART_IMPL_write(RF_UART, *c);
			else
				printf("er");

		} else if(FIFO_isEmpty(&fifo_tx)) {
			UART_IMPL_setTxItEnabled(RF_UART, FALSE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
		}
	} else {
		UART_IMPL_setTxItEnabled(RF_UART, FALSE);	//Le timeout du delai d'attente transmission reactivera l'it
		//TIMER_SRC_TIMER_EnableIT();
		debug_printf("TX canttransmit %d left\n", FIFO_availableElements(&fifo_tx));
	}
	UART_IMPL_ackTxIt(RF_UART);
}

void TIMER_SRC_TIMER_interrupt() {
	TIMER_SRC_TIMER_stop();
	TIMER_SRC_TIMER_resetFlag();
	canTransmitData = TRUE;
	UART_IMPL_setTxItEnabled(RF_UART, TRUE);
}

#endif
