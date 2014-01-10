/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : USB Host
 *  Description : Gère les classes suivant le prériphérique USB parmi celle définie dans usbh_class_list.c
 *  Auteur : amurzeau
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_class_switch.h"
#include "usbh_class_list.h"
#include "../Core/usbh_core.h"
#include "../../QS/QS_outputlog.h"


static USBH_Status USBH_ClassSwitch_InterfaceInit(USB_OTG_CORE_HANDLE *pdev, void *phost);
static void USBH_ClassSwitch_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev, void *phost);
static USBH_Status USBH_ClassSwitch_ClassRequest(USB_OTG_CORE_HANDLE *pdev, void *phost);
static USBH_Status USBH_ClassSwitch_Handle(USB_OTG_CORE_HANDLE *pdev, void *phost);


USBH_Class_cb_TypeDef  USBH_ClassSwitch_cb =
{
	USBH_ClassSwitch_InterfaceInit,
	USBH_ClassSwitch_InterfaceDeInit,
	USBH_ClassSwitch_ClassRequest,
	USBH_ClassSwitch_Handle
};

void USBH_ClassSwitch_ErrorHandle(uint8_t status);

//Class USB choisi suivant le périphérique connecté, voir usbh_class_list.c pour la liste des classes reconnues
static USBH_Class_cb_TypeDef *device_class = NULL;


/**
  * @brief  USBH_ClassSwitch_InterfaceInit
  *         Interface initialization for MSC class.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status : Status of class request handled.
  */
static USBH_Status USBH_ClassSwitch_InterfaceInit(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
	USBH_HOST *pphost = phost;

	Uint8 i;
	Uint8 interface_class = pphost->device_prop.Itf_Desc[0].bInterfaceClass;

	device_class = NULL;

	for(i = 0; i < usbh_class_list_count; i++) {
		if(usbh_class_list[i].interface_class == interface_class) {
			device_class = usbh_class_list[i].USBH_class_handler;
			break;
		}
	}

	if(device_class == NULL) {
		pphost->usr_cb->USBH_USR_DeviceNotSupported();
		return USBH_OK;
	}

	return device_class->Init(pdev, phost);
}

/**
  * @brief  USBH_ClassSwitch_InterfaceDeInit
  *         De-Initialize interface by freeing host channels allocated to interface
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval None
  */
void USBH_ClassSwitch_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
	if(device_class == NULL) {
		error_printf("No class set for USB device, can\'t call DeInit !\n");
		return;
	}

	device_class->DeInit(pdev, phost);
	device_class = NULL;
}

/**
  * @brief  USBH_ClassSwitch_ClassRequest
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status : Status of class request handled.
  */
static USBH_Status USBH_ClassSwitch_ClassRequest(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
	if(device_class == NULL) {
		error_printf("No class set for USB device, can\'t call DeInit !\n");
		return USBH_OK;
	}

	return device_class->Requests(pdev, phost);
}


/**
  * @brief  USBH_ClassSwitch_Handle
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status
  */
static USBH_Status USBH_ClassSwitch_Handle(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
	if(device_class == NULL) {
		error_printf("No class set for USB device, can\'t call DeInit !\n");
		return USBH_OK;
	}

	return device_class->Machine(pdev, phost);
}
