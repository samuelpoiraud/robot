/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   This file includes the usb host library user callbacks
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

#include "usbh_usr.h"
#include "STM32_USB_HOST_Library/Class/CDC/usbh_cdc_core.h"
#include "STM32_USB_HOST_Library/Class/MSC/usbh_msc_core.h"
#include "hokuyo.h"

#define LOG_PREFIX "usbh_usr : "
#define LOG_COMPONENT OUTPUTLOG_COMPONENT_USBH_USR
#include "../QS/QS_outputlog.h"

/* Communication class code */
#define COMMUNICATION_DEVICE_CLASS_CODE		0x02
#define MASS_STORAGE_CLASS_CODE				0x08
#define HID_CLASS_CODE						0x03

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
USBH_Usr_cb_TypeDef USR_cb =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};

/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void) {
  
	trace_printf("Init\n");

	static Uint8 startup = 0;

	if(startup == 0 ) {
		startup = 1;
	}

}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void) {
	trace_printf("Device Attached\n");
	HOKUYO_deviceConnected();
}

/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError(void) {
	error_printf("Unrecovered Error\n");
}

/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected(void) {
	trace_printf("Device Disconnected\n");
	HOKUYO_deviceDisconnected();
}

/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void) {
	trace_printf("Reset Device\n");
}

/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(Uint8 DeviceSpeed) {

	if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED) {
		trace_printf("High speed device detected\n");
	} else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED) {
		trace_printf("Full speed device detected\n");
	} else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED) {
		trace_printf("Low speed device detected\n");
	} else {
		trace_printf("Device fault speed detection\n");
	}

}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc) {
  
	USBH_DevDesc_TypeDef *hs;
	hs = DeviceDesc;
  
	trace_printf("VID : %04lXh\n", (Uint32)(*hs).idVendor);
	trace_printf("PID : %04lXh\n", (Uint32)(*hs).idProduct);

}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void) {
	trace_printf("Device Address Assigned\n");
}

/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef *cfgDesc, USBH_InterfaceDesc_TypeDef *itfDesc, USBH_EpDesc_TypeDef *epDesc) {

	USBH_InterfaceDesc_TypeDef *id;
	id = itfDesc;
  
	if((*id).bInterfaceClass  == MASS_STORAGE_CLASS_CODE) {
		trace_printf("Mass storage device connected\n");
	} else if((*id).bInterfaceClass  == HID_CLASS_CODE) {
		trace_printf("HID device connected\n");
	} else if((*id).bInterfaceClass  == COMMUNICATION_DEVICE_CLASS_CODE) {
		trace_printf("CDC device connected\n");
	}

}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString) {
	trace_printf("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString) {
	trace_printf("Product : %s\n", (char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString) {
	trace_printf( "Serial Number : %s\n", (char *)SerialNumString);
}

/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void) {
	trace_printf("Enumeration completed\n");
}

/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void) {
	trace_printf("Device not supported\n");
}  

/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void) {
  return USBH_USR_RESP_OK;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void) {
	error_printf("Overcurrent detected\n");
}

/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void) {
	trace_printf("IN THIS CODE, MSC IS DISABLED, BECAUSE USELESS\n");
	return 0;
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void) {
	trace_printf("DeInit\n");
}
