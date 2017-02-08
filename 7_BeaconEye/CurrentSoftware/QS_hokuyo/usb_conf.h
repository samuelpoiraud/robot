/**
  ******************************************************************************
  * @file    usb_conf.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   General low level driver configuration
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

#ifndef __USB_CONF__H__
#define __USB_CONF__H__

#include "../stm32f4xx/stm32f4xx.h"
#include "../QS/QS_all.h"

/*******************************************************************************
*                     FIFO Size Configuration in Host mode
*
*  (i) Receive data FIFO size = (Largest Packet Size / 4) + 1 or
*                             2x (Largest Packet Size / 4) + 1,  If a
*                             high-bandwidth channel or multiple isochronous
*                             channels are enabled
*
*  (ii) For the host nonperiodic Transmit FIFO is the largest maximum packet size
*      for all supported nonperiodic OUT channels. Typically, a space
*      corresponding to two Largest Packet Size is recommended.
*
*  (iii) The minimum amount of RAM required for Host periodic Transmit FIFO is
*        the largest maximum packet size for all supported periodic OUT channels.
*        If there is at least one High Bandwidth Isochronous OUT endpoint,
*        then the space must be at least two times the maximum packet size for
*        that channel.
*******************************************************************************/

/****************** USB OTG FS CONFIGURATION **********************************/
#define USE_USB_OTG_FS
#define USB_OTG_FS_CORE
#define USE_EMBEDDED_PHY
#define RX_FIFO_FS_SIZE                          128
#define TXH_NP_FS_FIFOSIZ                         96
#define TXH_P_FS_FIFOSIZ                          96


/****************** USB OTG MODE CONFIGURATION ********************************/
#define USE_HOST_MODE

/****************** C Compilers dependant keywords ****************************/
#define __ALIGN_BEGIN
#define __ALIGN_END


#endif //__USB_CONF__H__
