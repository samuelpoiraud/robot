/**
  ******************************************************************************
  * @file    usbh_usr.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   Header file for usbh_usr.c
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

#ifndef __USH_USR_H__
#define __USH_USR_H__

#include "STM32_USB_HOST_Library/Core/usbh_core.h"
#include "STM32_USB_HOST_Library/Class/CDC/usbh_cdc_core.h"
#include "STM32_USB_HOST_Library/Class/MSC/usbh_msc_core.h"

extern  USBH_Usr_cb_TypeDef USR_cb;
extern  Uint8 USBH_USR_ApplicationState ;

void USBH_USR_ApplicationSelected(void);
void USBH_USR_Init(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceAttached(void);
void USBH_USR_ResetDevice(void);
void USBH_USR_DeviceDisconnected (void);
void USBH_USR_OverCurrentDetected (void);
void USBH_USR_DeviceSpeedDetected(Uint8 DeviceSpeed); 
void USBH_USR_Device_DescAvailable(void *);
void USBH_USR_DeviceAddressAssigned(void);
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc, USBH_InterfaceDesc_TypeDef *itfDesc, USBH_EpDesc_TypeDef *epDesc);
void USBH_USR_Manufacturer_String(void *);
void USBH_USR_Product_String(void *);
void USBH_USR_SerialNum_String(void *);
void USBH_USR_EnumerationDone(void);
USBH_USR_Status USBH_USR_UserInput(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceNotSupported(void);
void USBH_USR_UnrecoveredError(void);
int USBH_USR_MSC_Application(void);

#endif /*__USH_USR_H__*/
