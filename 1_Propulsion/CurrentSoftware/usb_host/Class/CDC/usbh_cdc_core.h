/**
  ******************************************************************************
  * @file    usbh_CDC_core.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   This file contains all the prototypes for the usbh_CDC_core.c
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_CDC_CORE_H
#define __USBH_CDC_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "../../Core/usbh_core.h"
#include "../../Core/usbh_stdreq.h"
#include "../../OTG/usb_bsp.h"
#include "../../Core/usbh_ioreq.h"
#include "../../Core/usbh_hcs.h"

/* Structure for CDC process */
typedef struct _CDC_Process
{
  Uint8              hc_num_in;
  Uint8              hc_num_out;
  Uint8              MSBulkOutEp;
  Uint8              MSBulkInEp;
  Uint16             MSBulkInEpSize;
  Uint16             MSBulkOutEpSize;
  Uint8              buff[USBH_CDC_MPS_SIZE];
  Uint8              maxLun;
}
CDC_Machine_TypeDef;

typedef struct {
	Uint16 (*get_tx_data)(Uint8 *buffer, Uint16 size);		//Récupère les données à envoyer par USB
	void (*put_rx_data)(const Uint8 *buffer, Uint16 size);	//Met des données dans le buffer uart, elle pourront être lue par la suite
	bool_e (*is_tx_empty)();								//Renvoie FALSE si des données doivent être envoyées
	Uint16 (*rx_available)();								//Renvoie le nombre d'octet libre dans le buffer
} CDC_uart_interface_t;


#define USB_CDC_CLASS_ID                 0x02

#define USB_REQ_BOT_RESET                0xFF
#define USB_REQ_GET_MAX_LUN              0xFE


extern USBH_Class_cb_TypeDef  USBH_CDC_cb;
extern CDC_Machine_TypeDef    CDC_Machine;
extern Uint8 CDCErrorCount;

void USB_CDC_set_interface(CDC_uart_interface_t _interface_functions);
void USB_CDC_set_baudrate(Uint32 baudrate);

bool_e USBH_CDC_is_ready_to_run(void);


#endif  /* __USBH_CDC_CORE_H */
