/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: uart_via_usb.c 1191 2013-12-09 20:34:44Z jroy $
 *
 *  Package : USB Host
 *  Description : Gère les classes suivant le prériphérique USB parmi celle définie dans usbh_class_list.c
 *  Auteur : amurzeau
 */

#include "uart_via_usb.h"
#include "QS/QS_buffer_fifo.h"
#include "usb_host/Class/CDC/usbh_cdc_core.h"
#include "QS/QS_outputlog.h"

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

#define UART_USB_BUFFER_SIZE 4096

static Uint8 fifo_buffer_tx[UART_USB_BUFFER_SIZE];
static Uint8 fifo_buffer_rx[UART_USB_BUFFER_SIZE];
static FIFO_t fifo_tx;
static FIFO_t fifo_rx;

static bool_e overflow;

bool_e UART_USB_init(Uint32 baudrate) {
	FIFO_init(&fifo_tx, fifo_buffer_tx, UART_USB_BUFFER_SIZE, 1);
	FIFO_init(&fifo_rx, fifo_buffer_rx, UART_USB_BUFFER_SIZE, 1);

	USB_CDC_set_interface(cdc_interface);
	USB_CDC_set_baudrate(baudrate);

	return TRUE;
}


void   UART_USB_write(Uint8 data) {
	if(data == '\r')
		return;
	if(FIFO_insertData(&fifo_tx, &data) == FALSE)
		debug_printf("FIFO TX full\n");
}



Uint8  UART_USB_read() {
	Uint8 *data;

	data = FIFO_getData(&fifo_rx);
	if(data)
		return *data;
	else
		return 0;
}

bool_e UART_USB_isRxEmpty() {
	return FIFO_isEmpty(&fifo_rx);
}

bool_e UART_USB_isTxFull() {
	return FIFO_isFull(&fifo_tx);
}

bool_e UART_USB_isIdle() {
	return FIFO_isEmpty(&fifo_tx);
}

bool_e UART_USB_hasOverflow() {
	return overflow;
}

bool_e UART_USB_hasFrameError() {
	return FALSE;
}

bool_e UART_USB_resetErrors() {
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
	return UART_USB_BUFFER_SIZE - FIFO_availableElements(&fifo_rx) - 1;
}

