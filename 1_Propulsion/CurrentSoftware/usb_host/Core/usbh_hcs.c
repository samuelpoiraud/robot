/**
  ******************************************************************************
  * @file    usbh_hcs.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   This file implements functions for opening and closing host channels
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

/* Includes ------------------------------------------------------------------*/
#include "usbh_hcs.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_HCS
  * @brief This file includes opening and closing host channels
  * @{
  */ 

/** @defgroup USBH_HCS_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HCS_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_FunctionPrototypes
  * @{
  */ 
static Uint16 USBH_GetFreeChannel (USB_OTG_CORE_HANDLE *pdev);
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_Functions
  * @{
  */ 



/**
  * @brief  USBH_Open_Channel
  *         Open a  pipe
  * @param  pdev : Selected device
  * @param  hc_num: Host channel Number
  * @param  dev_address: USB Device address allocated to attached device
  * @param  speed : USB device speed (Full/Low)
  * @param  ep_type: end point type (Bulk/int/ctl)
  * @param  mps: max pkt size
  * @retval Status
  */
Uint8 USBH_Open_Channel  (USB_OTG_CORE_HANDLE *pdev,
                            Uint8 hc_num,
                            Uint8 dev_address,
                            Uint8 speed,
                            Uint8 ep_type,
                            Uint16 mps)
{

  pdev->host.hc[hc_num].ep_num = pdev->host.channel[hc_num]& 0x7F;
  pdev->host.hc[hc_num].ep_is_in = (pdev->host.channel[hc_num] & 0x80 ) == 0x80;  
  pdev->host.hc[hc_num].dev_addr = dev_address;  
  pdev->host.hc[hc_num].ep_type = ep_type;  
  pdev->host.hc[hc_num].max_packet = mps; 
  pdev->host.hc[hc_num].speed = speed; 
  pdev->host.hc[hc_num].toggle_in = 0; 
  pdev->host.hc[hc_num].toggle_out = 0;   
  if(speed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    pdev->host.hc[hc_num].do_ping = 1;
  }
  
  USB_OTG_HC_Init(pdev, hc_num) ;
  
  return HC_OK; 

}

/**
  * @brief  USBH_Modify_Channel
  *         Modify a  pipe
  * @param  pdev : Selected device
  * @param  hc_num: Host channel Number
  * @param  dev_address: USB Device address allocated to attached device
  * @param  speed : USB device speed (Full/Low)
  * @param  ep_type: end point type (Bulk/int/ctl)
  * @param  mps: max pkt size
  * @retval Status
  */
Uint8 USBH_Modify_Channel (USB_OTG_CORE_HANDLE *pdev,
                            Uint8 hc_num,
                            Uint8 dev_address,
                            Uint8 speed,
                            Uint8 ep_type,
                            Uint16 mps)
{
  
  if(dev_address != 0)
  {
    pdev->host.hc[hc_num].dev_addr = dev_address;  
  }
  
  if((pdev->host.hc[hc_num].max_packet != mps) && (mps != 0))
  {
    pdev->host.hc[hc_num].max_packet = mps; 
  }
  
  if((pdev->host.hc[hc_num].speed != speed ) && (speed != 0 )) 
  {
    pdev->host.hc[hc_num].speed = speed; 
  }
  
  USB_OTG_HC_Init(pdev, hc_num);
  return HC_OK; 

}

/**
  * @brief  USBH_Alloc_Channel
  *         Allocate a new channel for the pipe
  * @param  ep_addr: End point for which the channel to be allocated
  * @retval hc_num: Host channel number
  */
Uint8 USBH_Alloc_Channel  (USB_OTG_CORE_HANDLE *pdev, Uint8 ep_addr)
{
  Uint16 hc_num;
  
  hc_num =  USBH_GetFreeChannel(pdev);

  if (hc_num != HC_ERROR)
  {
	pdev->host.channel[hc_num] = HC_USED | ep_addr;
  }
  return hc_num;
}

/**
  * @brief  USBH_Free_Pipe
  *         Free the USB host channel
  * @param  idx: Channel number to be freed 
  * @retval Status
  */
Uint8 USBH_Free_Channel  (USB_OTG_CORE_HANDLE *pdev, Uint8 idx)
{
   if(idx < HC_MAX)
   {
	 pdev->host.channel[idx] &= HC_USED_MASK;
   }
   return USBH_OK;
}


/**
  * @brief  USBH_DeAllocate_AllChannel
  *         Free all USB host channel
* @param  pdev : core instance
  * @retval Status
  */
Uint8 USBH_DeAllocate_AllChannel  (USB_OTG_CORE_HANDLE *pdev)
{
   Uint8 idx;
   
   for (idx = 2; idx < HC_MAX ; idx ++)
   {
	 pdev->host.channel[idx] = 0;
   }
   return USBH_OK;
}

/**
  * @brief  USBH_GetFreeChannel
  *         Get a free channel number for allocation to a device endpoint
  * @param  None
  * @retval idx: Free Channel number
  */
static Uint16 USBH_GetFreeChannel (USB_OTG_CORE_HANDLE *pdev)
{
  Uint8 idx = 0;
  
  for (idx = 0 ; idx < HC_MAX ; idx++)
  {
	if ((pdev->host.channel[idx] & HC_USED) == 0)
	{
	   return idx;
	} 
  }
  return HC_ERROR;
}


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


