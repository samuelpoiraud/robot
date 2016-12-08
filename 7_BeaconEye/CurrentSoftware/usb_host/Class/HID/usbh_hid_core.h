/**
  ******************************************************************************
  * @file    usbh_hid_core.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   This file contains all the prototypes for the usbh_hid_core.c
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
#ifndef __USBH_HID_CORE_H
#define __USBH_HID_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "../../Core/usbh_core.h"
#include "../../Core/usbh_stdreq.h"
#include "../../OTG/usb_bsp.h"
#include "../../Core/usbh_ioreq.h"
#include "../../Core/usbh_hcs.h"
 
/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */
  
/** @defgroup USBH_HID_CORE
  * @brief This file is the Header file for USBH_HID_CORE.c
  * @{
  */ 


/** @defgroup USBH_HID_CORE_Exported_Types
  * @{
  */ 


/* States for HID State Machine */
typedef enum
{
  HID_IDLE= 0,
  HID_SEND_DATA,
  HID_BUSY,
  HID_GET_DATA,   
  HID_POLL,
  HID_ERROR,
}
HID_State;

typedef enum
{
  HID_REQ_IDLE = 0,
  HID_REQ_GET_REPORT_DESC,
  HID_REQ_GET_HID_DESC,
  HID_REQ_SET_IDLE,
  HID_REQ_SET_PROTOCOL,
  HID_REQ_SET_REPORT,

}
HID_CtlState;

typedef struct HID_cb
{
  void  (*Init)   (void);             
  void  (*Decode) (Uint8 *data);       
  
} HID_cb_TypeDef;

typedef  struct  _HID_Report 
{
    Uint8   ReportID;    
    Uint8   ReportType;  
    Uint16  UsagePage;   
    Uint32  Usage[2]; 
    Uint32  NbrUsage;                      
    Uint32  UsageMin;                      
    Uint32  UsageMax;                      
    int32_t   LogMin;                        
    int32_t   LogMax;                        
    int32_t   PhyMin;                        
    int32_t   PhyMax;                        
    int32_t   UnitExp;                       
    Uint32  Unit;                          
    Uint32  ReportSize;                    
    Uint32  ReportCnt;                     
    Uint32  Flag;                          
    Uint32  PhyUsage;                      
    Uint32  AppUsage;                      
    Uint32  LogUsage;   
} 
HID_Report_TypeDef;

/* Structure for HID process */
typedef struct _HID_Process
{
  Uint8              buff[64];
  Uint8              hc_num_in; 
  Uint8              hc_num_out; 
  HID_State            state; 
  Uint8              HIDIntOutEp;
  Uint8              HIDIntInEp;
  HID_CtlState         ctl_state;
  Uint16             length;
  Uint8              ep_addr;
  Uint16             poll; 
  __IO Uint16        timer; 
  HID_cb_TypeDef             *cb;
}
HID_Machine_TypeDef;

/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_Defines
  * @{
  */ 

#define USB_HID_CLASS_ID             0x03

#define USB_HID_REQ_GET_REPORT       0x01
#define USB_HID_GET_IDLE             0x02
#define USB_HID_GET_PROTOCOL         0x03
#define USB_HID_SET_REPORT           0x09
#define USB_HID_SET_IDLE             0x0A
#define USB_HID_SET_PROTOCOL         0x0B    
/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_Variables
  * @{
  */ 
extern USBH_Class_cb_TypeDef  HID_cb;
/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_FunctionsPrototype
  * @{
  */ 

USBH_Status USBH_Set_Report (USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST *phost,
                                  Uint8 reportType,
                                  Uint8 reportId,
                                  Uint8 reportLen,
                                  Uint8* reportBuff);
/**
  * @}
  */ 


#endif /* __USBH_HID_CORE_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

