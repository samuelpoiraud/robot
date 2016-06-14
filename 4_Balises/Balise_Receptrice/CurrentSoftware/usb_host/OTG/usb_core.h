/**
  ******************************************************************************
  * @file    usb_core.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   Header of the Core Layer
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CORE_H__
#define __USB_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "../usb_conf.h"
#include "usb_regs.h"
#include "usb_defines.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */

/** @defgroup USB_CORE
  * @brief usb otg driver core layer
  * @{
  */


/** @defgroup USB_CORE_Exported_Defines
  * @{
  */

#define USB_OTG_EP0_IDLE                          0
#define USB_OTG_EP0_SETUP                         1
#define USB_OTG_EP0_DATA_IN                       2
#define USB_OTG_EP0_DATA_OUT                      3
#define USB_OTG_EP0_STATUS_IN                     4
#define USB_OTG_EP0_STATUS_OUT                    5
#define USB_OTG_EP0_STALL                         6

#define USB_OTG_EP_TX_DIS       0x0000
#define USB_OTG_EP_TX_STALL     0x0010
#define USB_OTG_EP_TX_NAK       0x0020
#define USB_OTG_EP_TX_VALID     0x0030

#define USB_OTG_EP_RX_DIS       0x0000
#define USB_OTG_EP_RX_STALL     0x1000
#define USB_OTG_EP_RX_NAK       0x2000
#define USB_OTG_EP_RX_VALID     0x3000
/**
  * @}
  */
#define   MAX_DATA_LENGTH                        0xFF

/** @defgroup USB_CORE_Exported_Types
  * @{
  */


typedef enum {
  USB_OTG_OK = 0,
  USB_OTG_FAIL
}USB_OTG_STS;

typedef enum {
  HC_IDLE = 0,
  HC_XFRC,     //Transfert complete
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,
  HC_BBLERR,
  HC_DATATGLERR //Data toggle error (data0/data1)
}HC_STATUS;

typedef enum {
  URB_DONE,		//URB finished successfully
  URB_INPROGRESS,		//USB has not been processed yet (IN_PROGRESS)
  URB_NOTREADY,	//Peer is not ready (either nothing to send or it received too much data)
  URB_ERROR,    //An error occured
  URB_STALL     //Another type of error
}URB_STATE;     //if URB_STATE != URB_INPROGRESS, the URB is terminated and URB_STATE contains the URB result

typedef enum {
  CTRL_START = 0,
  CTRL_XFRC,
  CTRL_HALTED,
  CTRL_NAK,
  CTRL_STALL,
  CTRL_XACTERR,
  CTRL_BBLERR,
  CTRL_DATATGLERR,
  CTRL_FAIL
}CTRL_STATUS;


typedef struct USB_OTG_hc
{
  Uint8       dev_addr ;
  Uint8       ep_num;
  Uint8       ep_is_in;
  Uint8       speed;
  Uint8       do_ping;
  Uint8       ep_type;
  Uint16      max_packet;
  Uint8       data_pid;
  Uint8       *xfer_buff;
  Uint32      xfer_len;		//bytes to be sent or received
  Uint32      xfer_count;	//bytes transfered (sent or received)
  Uint8       toggle_in;
  Uint8       toggle_out;
  Uint32       dma_addr;
}
USB_OTG_HC , *PUSB_OTG_HC;

typedef struct USB_OTG_ep
{
  Uint8        num;
  Uint8        is_in;
  Uint8        is_stall;
  Uint8        type;
  Uint8        data_pid_start;
  Uint8        even_odd_frame;
  Uint16       tx_fifo_num;
  Uint32       maxpacket;
  /* transaction level variables*/
  Uint8        *xfer_buff;
  Uint32       dma_addr;
  Uint32       xfer_len;
  Uint32       xfer_count;
  /* Transfer level variables*/
  Uint32       rem_data_len;
  Uint32       total_data_len;
  Uint32       ctl_data_len;

}

USB_OTG_EP , *PUSB_OTG_EP;



typedef struct USB_OTG_core_cfg
{
  Uint8       host_channels;
  Uint8       dev_endpoints;
  Uint8       speed;
  Uint8       dma_enable;
  Uint16      mps;
  Uint16      TotalFifoSize;
  Uint8       phy_itface;
  Uint8       Sof_output;
  Uint8       low_power;
  Uint8       coreID;

}
USB_OTG_CORE_CFGS, *PUSB_OTG_CORE_CFGS;



typedef  struct  usb_setup_req {

	Uint8   bmRequest;
	Uint8   bRequest;
	Uint16  wValue;
	Uint16  wIndex;
	Uint16  wLength;
} USB_SETUP_REQ;

typedef struct _Device_TypeDef
{
  Uint8  *(*GetDeviceDescriptor)( Uint8 speed , Uint16 *length);
  Uint8  *(*GetLangIDStrDescriptor)( Uint8 speed , Uint16 *length);
  Uint8  *(*GetManufacturerStrDescriptor)( Uint8 speed , Uint16 *length);
  Uint8  *(*GetProductStrDescriptor)( Uint8 speed , Uint16 *length);
  Uint8  *(*GetSerialStrDescriptor)( Uint8 speed , Uint16 *length);
  Uint8  *(*GetConfigurationStrDescriptor)( Uint8 speed , Uint16 *length);
  Uint8  *(*GetInterfaceStrDescriptor)( Uint8 speed , Uint16 *length);
} USBD_DEVICE, *pUSBD_DEVICE;

typedef struct USB_OTG_hPort
{
  void (*Disconnect) (void *phost);
  void (*Connect) (void *phost);
  Uint8 ConnStatus;
  Uint8 DisconnStatus;
  Uint8 ConnHandled;
  Uint8 DisconnHandled;
} USB_OTG_hPort_TypeDef;

typedef struct _Device_cb
{
  Uint8  (*Init)         (void *pdev , Uint8 cfgidx);
  Uint8  (*DeInit)       (void *pdev , Uint8 cfgidx);
 /* Control Endpoints*/
  Uint8  (*Setup)        (void *pdev , USB_SETUP_REQ  *req);
  Uint8  (*EP0_TxSent)   (void *pdev );
  Uint8  (*EP0_RxReady)  (void *pdev );
  /* Class Specific Endpoints*/
  Uint8  (*DataIn)       (void *pdev , Uint8 epnum);
  Uint8  (*DataOut)      (void *pdev , Uint8 epnum);
  Uint8  (*SOF)          (void *pdev);
  Uint8  (*IsoINIncomplete)  (void *pdev);
  Uint8  (*IsoOUTIncomplete)  (void *pdev);

  Uint8  *(*GetConfigDescriptor)( Uint8 speed , Uint16 *length);
#ifdef USB_OTG_HS_CORE
  Uint8  *(*GetOtherConfigDescriptor)( Uint8 speed , Uint16 *length);
#endif

#ifdef USB_SUPPORT_USER_STRING_DESC
  Uint8  *(*GetUsrStrDescriptor)( Uint8 speed ,Uint8 index,  Uint16 *length);
#endif

} USBD_Class_cb_TypeDef;



typedef struct _USBD_USR_PROP
{
  void (*Init)(void);
  void (*DeviceReset)(Uint8 speed);
  void (*DeviceConfigured)(void);
  void (*DeviceSuspended)(void);
  void (*DeviceResumed)(void);

  void (*DeviceConnected)(void);
  void (*DeviceDisconnected)(void);

}
USBD_Usr_cb_TypeDef;

typedef struct _DCD
{
  Uint8        device_config;
  Uint8        device_state;
  Uint8        device_status;
  Uint8        device_address;
  Uint32       DevRemoteWakeup;
  USB_OTG_EP     in_ep   [USB_OTG_MAX_TX_FIFOS];
  USB_OTG_EP     out_ep  [USB_OTG_MAX_TX_FIFOS];
  Uint8        setup_packet [8*3];
  USBD_Class_cb_TypeDef         *class_cb;
  USBD_Usr_cb_TypeDef           *usr_cb;
  USBD_DEVICE                   *usr_device;
  Uint8        *pConfig_descriptor;
 }
DCD_DEV , *DCD_PDEV;


typedef struct _HCD
{
  Uint8                  Rx_Buffer [MAX_DATA_LENGTH];
  __IO Uint32            ConnSts;
  __IO Uint32            ErrCnt[USB_OTG_MAX_TX_FIFOS];
  __IO Uint32            XferCnt[USB_OTG_MAX_TX_FIFOS];
  __IO HC_STATUS           HC_Status[USB_OTG_MAX_TX_FIFOS];  //Utilisé en IT pour déterminer URB_State lorsque le channel est disabled
  __IO URB_STATE           URB_State[USB_OTG_MAX_TX_FIFOS];
  USB_OTG_HC               hc [USB_OTG_MAX_TX_FIFOS];
  Uint16                 channel [USB_OTG_MAX_TX_FIFOS];
  USB_OTG_hPort_TypeDef    *port_cb;
}
HCD_DEV , *USB_OTG_USBH_PDEV;


typedef struct _OTG
{
  Uint8    OTG_State;
  Uint8    OTG_PrevState;
  Uint8    OTG_Mode;
}
OTG_DEV , *USB_OTG_USBO_PDEV;

typedef struct USB_OTG_handle
{
  USB_OTG_CORE_CFGS    cfg;
  USB_OTG_CORE_REGS    regs;
#ifdef USE_DEVICE_MODE
  DCD_DEV     dev;
#endif
#ifdef USE_HOST_MODE
  HCD_DEV     host;
#endif
#ifdef USE_OTG_MODE
  OTG_DEV     otg;
#endif
}
USB_OTG_CORE_HANDLE , *PUSB_OTG_CORE_HANDLE;

/**
  * @}
  */


/** @defgroup USB_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_FunctionsPrototype
  * @{
  */


USB_OTG_STS  USB_OTG_CoreInit        (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_SelectCore      (USB_OTG_CORE_HANDLE *pdev,
									  USB_OTG_CORE_ID_TypeDef coreID);
USB_OTG_STS  USB_OTG_EnableGlobalInt (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_DisableGlobalInt(USB_OTG_CORE_HANDLE *pdev);
void*           USB_OTG_ReadPacket   (USB_OTG_CORE_HANDLE *pdev ,
	Uint8 *dest,
	Uint16 len);
USB_OTG_STS  USB_OTG_WritePacket     (USB_OTG_CORE_HANDLE *pdev ,
	Uint8 *src,
	Uint8 ch_ep_num,
	Uint16 len);
USB_OTG_STS  USB_OTG_FlushTxFifo     (USB_OTG_CORE_HANDLE *pdev , Uint32 num);
USB_OTG_STS  USB_OTG_FlushRxFifo     (USB_OTG_CORE_HANDLE *pdev);

Uint32     USB_OTG_ReadCoreItr     (USB_OTG_CORE_HANDLE *pdev);
Uint32     USB_OTG_ReadOtgItr      (USB_OTG_CORE_HANDLE *pdev);
Uint8      USB_OTG_IsHostMode      (USB_OTG_CORE_HANDLE *pdev);
Uint8      USB_OTG_IsDeviceMode    (USB_OTG_CORE_HANDLE *pdev);
Uint32     USB_OTG_GetMode         (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_PhyInit         (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_SetCurrentMode  (USB_OTG_CORE_HANDLE *pdev,
	Uint8 mode);

/*********************** HOST APIs ********************************************/
#ifdef USE_HOST_MODE
USB_OTG_STS  USB_OTG_CoreInitHost    (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EnableHostInt   (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_HC_Init         (USB_OTG_CORE_HANDLE *pdev, Uint8 hc_num);
USB_OTG_STS  USB_OTG_HC_Halt         (USB_OTG_CORE_HANDLE *pdev, Uint8 hc_num);
USB_OTG_STS  USB_OTG_HC_StartXfer    (USB_OTG_CORE_HANDLE *pdev, Uint8 hc_num);
USB_OTG_STS  USB_OTG_HC_DoPing       (USB_OTG_CORE_HANDLE *pdev , Uint8 hc_num);
Uint32     USB_OTG_ReadHostAllChannels_intr    (USB_OTG_CORE_HANDLE *pdev);
Uint32     USB_OTG_ResetPort       (USB_OTG_CORE_HANDLE *pdev);
Uint32     USB_OTG_ReadHPRT0       (USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_DriveVbus       (USB_OTG_CORE_HANDLE *pdev, Uint8 state);
void         USB_OTG_InitFSLSPClkSel (USB_OTG_CORE_HANDLE *pdev ,Uint8 freq);
Uint8      USB_OTG_IsEvenFrame     (USB_OTG_CORE_HANDLE *pdev) ;
void         USB_OTG_StopHost        (USB_OTG_CORE_HANDLE *pdev);
#endif
/********************* DEVICE APIs ********************************************/
#ifdef USE_DEVICE_MODE
USB_OTG_STS  USB_OTG_CoreInitDev         (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EnableDevInt        (USB_OTG_CORE_HANDLE *pdev);
Uint32     USB_OTG_ReadDevAllInEPItr           (USB_OTG_CORE_HANDLE *pdev);
enum USB_OTG_SPEED USB_OTG_GetDeviceSpeed (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EP0Activate (USB_OTG_CORE_HANDLE *pdev);
USB_OTG_STS  USB_OTG_EPActivate  (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPDeactivate(USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPStartXfer (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EP0StartXfer(USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPSetStall          (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
USB_OTG_STS  USB_OTG_EPClearStall        (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep);
Uint32     USB_OTG_ReadDevAllOutEp_itr (USB_OTG_CORE_HANDLE *pdev);
Uint32     USB_OTG_ReadDevOutEP_itr    (USB_OTG_CORE_HANDLE *pdev , Uint8 epnum);
Uint32     USB_OTG_ReadDevAllInEPItr   (USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_InitDevSpeed        (USB_OTG_CORE_HANDLE *pdev , Uint8 speed);
Uint8      USBH_IsEvenFrame (USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_EP0_OutStart(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_ActiveRemoteWakeup(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_UngateClock(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_StopDevice(USB_OTG_CORE_HANDLE *pdev);
void         USB_OTG_SetEPStatus (USB_OTG_CORE_HANDLE *pdev , USB_OTG_EP *ep , Uint32 Status);
Uint32     USB_OTG_GetEPStatus(USB_OTG_CORE_HANDLE *pdev ,USB_OTG_EP *ep);
#endif
/**
  * @}
  */

#endif  /* __USB_CORE_H__ */


/**
  * @}
  */

/**
  * @}
  */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

