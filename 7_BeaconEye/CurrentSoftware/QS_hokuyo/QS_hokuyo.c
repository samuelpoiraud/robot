/*
 * QS_hokuyo.c
 *
 *  Created on: 8 févr. 2017
 *      Author: guill
 */

#include "QS_hokuyo.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_buttons.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_maths.h"
#ifdef STM32F40XX
	#include "../QS/QS_sys.h"
#endif
#include "STM32_USB_HOST_Library/Core/usbh_core.h"
#include "usbh_usr.h"
#include "STM32_USB_OTG_Driver/usb_hcd_int.h"
#include "STM32_USB_OTG_Driver/usb_bsp.h"
#include "STM32_USB_HOST_Library/Class/CDC/usbh_cdc_core.h"
#include "STM32_USB_HOST_Library/Class/MSC/usbh_msc_core.h"
#include "STM32_USB_HOST_Library/Class/usbh_class_switch.h"
#include "usb_vcp_cdc.h"

#define LOG_PREFIX "QS_hokuyo : "
#define LOG_COMPONENT OUTPUTLOG_COMPONENT_QS_HOKUYO
#include "../QS/QS_outputlog.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core 	__ALIGN_END;
__ALIGN_BEGIN USBH_HOST                USB_Host 		__ALIGN_END;

/* Private defines */
#define LINE_FEED				0x0A							// Caractère de retour à la ligne
#define NB_BYTES_FROM_HOKUYO	2500							// Taille du buffer pour la trame hokuyo reçue

/* Private variables */
static   bool_e hokuyo_initialized = FALSE;						// Protection contre le double initialisation
static 	 Uint32 datas_index=0;									// Index pour les données brutes de la trame hokuyo
volatile bool_e flag_device_disconnected = FALSE;				// Indique que le capteur a été débranché

/* Données brutes de la trame hokuyo */
#ifdef USE_CCM_RAM
	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO] __attribute__((section(".ccm")));
#else
	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO];
#endif

/* Private functions */
static void   HOKUYO_writeCommand(Uint8 tab[]);
static bool_e HOKUYO_readBuffer(void);

void HOKUYO_init(void) {

	if(!hokuyo_initialized) {

		debug_printf("Init usb for Hokuyo\n");
		time32_t startTime = global.absolute_time;

		VCP_init(19200);

		USBH_Init(&USB_OTG_Core,
			  #ifdef USE_USB_OTG_FS
				  USB_OTG_FS_CORE_ID,
			  #else
				  USB_OTG_HS_CORE_ID,
			  #endif
				  &USB_Host,
				  &USBH_CDC_cb,
				  &USR_cb);

		debug_printf("Fin Init usb for Hokuyo (%d ms)\n", (global.absolute_time - startTime));

	}

	hokuyo_initialized = TRUE;
}

void HOKUYO_process_main(void) {

	typedef enum {
		INIT = 0,
		HOKUYO_WAIT,
		HOKUYO_READY,
		ERROR,
		RESET_HOKUYO,
		RESET_ACKNOWLEDGE,
		TURN_ON_LASER,
		TURN_OFF_LASER,
		DONE
	}state_e;

	static state_e state = INIT, last_state = INIT;
	bool_e entrance;

	if((state == INIT && last_state == INIT) || state != last_state)
		entrance = TRUE;
	else
		entrance = FALSE;

	last_state = state;

	/* Process main du périphérique USB */
	USBH_Process(&USB_OTG_Core, &USB_Host);

	if(flag_device_disconnected) {
		flag_device_disconnected = FALSE;
		hokuyo_initialized = FALSE;
		HOKUYO_init();
		state = INIT;
	}

	switch(state) {

		case INIT:
			/* Attente de l'initialisation de l'hokuyo */
			if(hokuyo_initialized) {
				state = HOKUYO_WAIT;
			}
			break;

		case HOKUYO_WAIT:
			/* Attente du module de communication avec l'hokuyo */
			if(USBH_CDC_is_ready_to_run()) {
				state = HOKUYO_READY;
			}
			break;

		case HOKUYO_READY:
			if(entrance) {
				debug_printf("Pret\n");
			}
			break;

		case ERROR:
			debug_printf("ERROR SEQUENCE INITIALIZING");
			state=RESET_HOKUYO;
			break;

		case RESET_HOKUYO:
			HOKUYO_writeCommand((Uint8*)"RS");
			datas_index = 0;
			state=RESET_ACKNOWLEDGE;
			break;

		case RESET_ACKNOWLEDGE:
			HOKUYO_readBuffer();
			if(HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A)
				state=TURN_ON_LASER;
			else
				state=TURN_OFF_LASER;
			break;

		case TURN_ON_LASER:
			HOKUYO_writeCommand((Uint8*)"BM");
			state=DONE;
			break;

		case TURN_OFF_LASER:
			HOKUYO_writeCommand((Uint8*)"QT");
			state=DONE;
			break;

		case DONE:
			break;

		default:
			break;
	}
}

/**
 * @brief Envoyer une commande à l'hokuyo
 * @param tab La commande sous forme de chaine de caractères
 * @remark Le caractère de retour à la ligne est automatiquement envoyé à la fin de la commande
 */
static void HOKUYO_writeCommand(Uint8 tab[]) {
	Uint32 i;
	for(i=0;tab[i];i++) {
		VCP_write(tab[i]);
	}
	VCP_write(LINE_FEED);
}

/**
 * @brief Lecture du buffer de réception de l'USB
 * @retval Indique si l'on à reçu l'indication de fin de trame
 */
static bool_e HOKUYO_readBuffer(void) {

	static Uint8 previous_data = 0;
	Uint8 data;

	while(!VCP_isRxEmpty()) {
		data = VCP_read();
		HOKUYO_datas[datas_index] = data;

		if(datas_index < NB_BYTES_FROM_HOKUYO) {
			datas_index++;
		} else {
			fatal_printf("Overflow in hokuyo_read_buffer !\n");
		}

		/* On interrompt la réception des octets lorsque l'on reçoit deux LF consécutifs */
		if(data == LINE_FEED && previous_data == LINE_FEED) {
			return TRUE;
		}

		previous_data = data;
	}
	return FALSE;
}

void user_callback_DeviceDisconnected(void) {
#ifdef USE_HOKUYO
	flag_device_disconnected = TRUE;
#endif
}

/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	if (USB_OTG_IsHostMode(&USB_OTG_Core)) /* ensure that we are in device mode */
	{
		USBH_OTG_ISR_Handler(&USB_OTG_Core);
	}
	else
	{
		//USB Device.
		//USBD_OTG_ISR_Handler(&USB_OTG_Core);
	}
}
