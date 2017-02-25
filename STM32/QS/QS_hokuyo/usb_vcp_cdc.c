/*
 * usb_vcp_cdc.c
 *
 *  Created on: 8 f�vr. 2017
 *      Author: guill
 */

#include "../../config/config_use.h"

#if defined(USE_HOKUYO)

#include "usb_vcp_cdc.h"
#include "../QS/QS_buffer_fifo.h"
#include "STM32_USB_HOST_Library/Class/CDC/usbh_cdc_core.h"

#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_USB_VCP_CDC
#define LOG_PREFIX "VCP : "
#include "../QS/QS_outputlog.h"

#define VCP_BUFFER_RX_SIZE 4096
#define VCP_BUFFER_TX_SIZE 100

static Uint16 get_tx_data(Uint8 *buffer, Uint16 size);
static void put_rx_data(const Uint8 *buffer, Uint16 size);
static bool_e is_tx_empty();
static Uint16 rx_available();

CDC_uart_interface_t cdc_interface = {
	&get_tx_data,
	&put_rx_data,
	&is_tx_empty,
	&rx_available
};

static Uint8 fifo_buffer_tx[VCP_BUFFER_TX_SIZE];
static Uint8 fifo_buffer_rx[VCP_BUFFER_RX_SIZE];
static FIFO_t fifo_tx;
static FIFO_t fifo_rx;


static bool_e overflow;

bool_e VCP_init(Uint32 baudrate) {
	FIFO_init(&fifo_tx, fifo_buffer_tx, VCP_BUFFER_TX_SIZE, 1);
	FIFO_init(&fifo_rx, fifo_buffer_rx, VCP_BUFFER_RX_SIZE, 1);

	USB_CDC_set_interface(cdc_interface);
	USB_CDC_set_baudrate(baudrate);

	return TRUE;
}


void VCP_write(Uint8 data) {
	if(data == '\r')
		return;
	if(FIFO_insertData(&fifo_tx, &data) == FALSE)
		debug_printf("FIFO TX full\n");
}

Uint8 VCP_read() {
	Uint8 *data;

	data = FIFO_getData(&fifo_rx);
	if(data)
		return *data;
	else
		return 0;
}

bool_e VCP_isRxEmpty() {
	return FIFO_isEmpty(&fifo_rx);
}

bool_e VCP_isTxFull() {
	return FIFO_isFull(&fifo_tx);
}

bool_e VCP_isIdle() {
	return FIFO_isEmpty(&fifo_tx);
}

bool_e VCP_hasOverflow() {
	return overflow;
}

bool_e VCP_hasFrameError() {
	return FALSE;
}

bool_e VCP_resetErrors() {
	bool_e was_overflow = overflow;

	overflow = FALSE;

	return was_overflow;
}


static Uint16 get_tx_data(Uint8 *buffer, Uint16 size) {
	Uint16 i;
	Uint8 *data;

	for(i = 0; i < size && !FIFO_isEmpty(&fifo_tx); i++) {
		data = FIFO_getData(&fifo_tx);
		if(data) {
			buffer[i] = *data;
		} else {
			break;
		}
	}

	return i;
}

static void put_rx_data(const Uint8 *buffer, Uint16 size) {
	Uint16 i;

	for(i = 0; i < size; i++) {
		if(FIFO_insertData(&fifo_rx, &(buffer[i])) == FALSE) {
			overflow = TRUE;
			break;
		}
	}
}

static bool_e is_tx_empty() {
	return FIFO_isEmpty(&fifo_tx);
}

static Uint16 rx_available() {
	return VCP_BUFFER_RX_SIZE - FIFO_availableElements(&fifo_rx) - 1;
}

#endif
