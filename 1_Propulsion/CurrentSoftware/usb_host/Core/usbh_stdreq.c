/**
  ******************************************************************************
  * @file    usbh_stdreq.c 
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   This file implements the standard requests for device enumeration
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

#include "usbh_ioreq.h"
#include "usbh_stdreq.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_LIB_CORE
* @{
*/

/** @defgroup USBH_STDREQ 
* @brief This file implements the standard requests for device enumeration
* @{
*/


/** @defgroup USBH_STDREQ_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 



/** @defgroup USBH_STDREQ_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_Variables
* @{
*/
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_FunctionPrototypes
* @{
*/
static void USBH_ParseDevDesc (USBH_DevDesc_TypeDef* , Uint8 *buf, Uint16 length);

static void USBH_ParseCfgDesc (USBH_CfgDesc_TypeDef* cfg_desc,
                               USBH_InterfaceDesc_TypeDef* itf_desc,
                               USBH_EpDesc_TypeDef*  ep_desc,                                                           
                               Uint8 *buf, 
                               Uint16 length);
static  USBH_DescHeader_t      *USBH_GetNextDesc (Uint8   *pbuf, 
                                                  Uint16  *ptr);

static void USBH_ParseInterfaceDesc (USBH_InterfaceDesc_TypeDef  *if_descriptor, Uint8 *buf);
static void USBH_ParseEPDesc (USBH_EpDesc_TypeDef  *ep_descriptor, Uint8 *buf);

static void USBH_ParseStringDesc (Uint8* psrc, Uint8* pdest, Uint16 length);
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_Functions
* @{
*/ 


/**
* @brief  USBH_Get_DevDesc
*         Issue Get Device Descriptor command to the device. Once the response 
*         received, it parses the device descriptor and updates the status.
* @param  pdev: Selected device
* @param  dev_desc: Device Descriptor buffer address
* @param  pdev->host.Rx_Buffer: Receive Buffer address
* @param  length: Length of the descriptor
* @retval Status
*/
USBH_Status USBH_Get_DevDesc(USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST *phost,
                             Uint8 length)
{
  
  USBH_Status status;
  
  if((status = USBH_GetDescriptor(pdev, 
                                  phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,                          
                                  USB_DESC_DEVICE, 
                                  pdev->host.Rx_Buffer,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received */       
    USBH_ParseDevDesc(&phost->device_prop.Dev_Desc, pdev->host.Rx_Buffer, length);
  }
  return status;      
}

/**
* @brief  USBH_Get_CfgDesc
*         Issues Configuration Descriptor to the device. Once the response 
*         received, it parses the configuartion descriptor and updates the 
*         status.
* @param  pdev: Selected device
* @param  cfg_desc: Configuration Descriptor address
* @param  itf_desc: Interface Descriptor address
* @param  ep_desc: Endpoint Descriptor address
* @param  length: Length of the descriptor
* @retval Status
*/
USBH_Status USBH_Get_CfgDesc(USB_OTG_CORE_HANDLE *pdev, 
                             USBH_HOST           *phost,                      
                             Uint16 length)

{
  USBH_Status status;
  
  if((status = USBH_GetDescriptor(pdev,
                                  phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,                          
                                  USB_DESC_CONFIGURATION, 
                                  pdev->host.Rx_Buffer,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received  */       
    USBH_ParseCfgDesc (&phost->device_prop.Cfg_Desc,
                       phost->device_prop.Itf_Desc,
                       phost->device_prop.Ep_Desc[0], 
                       pdev->host.Rx_Buffer,
                       length); 
    
  }
  return status;
}


/**
* @brief  USBH_Get_StringDesc
*         Issues string Descriptor command to the device. Once the response 
*         received, it parses the string descriptor and updates the status.
* @param  pdev: Selected device
* @param  string_index: String index for the descriptor
* @param  buff: Buffer address for the descriptor
* @param  length: Length of the descriptor
* @retval Status
*/
USBH_Status USBH_Get_StringDesc(USB_OTG_CORE_HANDLE *pdev,
                                USBH_HOST *phost,
                                Uint8 string_index, 
                                Uint8 *buff, 
                                Uint16 length)
{
  USBH_Status status;
  
  if((status = USBH_GetDescriptor(pdev,
                                  phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,                                    
                                  USB_DESC_STRING | string_index, 
                                  pdev->host.Rx_Buffer,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received  */       
    USBH_ParseStringDesc(pdev->host.Rx_Buffer,buff, length);    
  }
  return status;
}

/**
* @brief  USBH_GetDescriptor
*         Issues Descriptor command to the device. Once the response received,
*         it parses the descriptor and updates the status.
* @param  pdev: Selected device
* @param  req_type: Descriptor type
* @param  value_idx: wValue for the GetDescriptr request
* @param  buff: Buffer to store the descriptor
* @param  length: Length of the descriptor
* @retval Status
*/
USBH_Status USBH_GetDescriptor(USB_OTG_CORE_HANDLE *pdev,
                               USBH_HOST           *phost,                                
                               Uint8  req_type,
                               Uint16 value_idx, 
                               Uint8* buff, 
                               Uint16 length )
{ 
  phost->Control.setup.b.bmRequestType = USB_D2H | req_type;
  phost->Control.setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
  phost->Control.setup.b.wValue.w = value_idx;
  
  if ((value_idx & 0xff00) == USB_DESC_STRING)
  {
    phost->Control.setup.b.wIndex.w = 0x0409;
  }
  else
  {
    phost->Control.setup.b.wIndex.w = 0;
  }
  phost->Control.setup.b.wLength.w = length;           
  return USBH_CtlReq(pdev, phost, buff , length );     
}

/**
* @brief  USBH_SetAddress
*         This command sets the address to the connected device
* @param  pdev: Selected device
* @param  DeviceAddress: Device address to assign
* @retval Status
*/
USBH_Status USBH_SetAddress(USB_OTG_CORE_HANDLE *pdev, 
                            USBH_HOST *phost,
                            Uint8 DeviceAddress)
{
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | \
    USB_REQ_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQ_SET_ADDRESS;
  
  phost->Control.setup.b.wValue.w = (Uint16)DeviceAddress;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );
}

/**
* @brief  USBH_SetCfg
*         The command sets the configuration value to the connected device
* @param  pdev: Selected device
* @param  cfg_idx: Configuration value
* @retval Status
*/
USBH_Status USBH_SetCfg(USB_OTG_CORE_HANDLE *pdev, 
                        USBH_HOST *phost,
                        Uint16 cfg_idx)
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE |\
    USB_REQ_TYPE_STANDARD;
  phost->Control.setup.b.bRequest = USB_REQ_SET_CONFIGURATION;
  phost->Control.setup.b.wValue.w = cfg_idx;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );      
}

/**
* @brief  USBH_ClrFeature
*         This request is used to clear or disable a specific feature.

* @param  pdev: Selected device
* @param  ep_num: endpoint number 
* @param  hc_num: Host channel number 
* @retval Status
*/
USBH_Status USBH_ClrFeature(USB_OTG_CORE_HANDLE *pdev,
                            USBH_HOST *phost,
                            Uint8 ep_num, 
                            Uint8 hc_num) 
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | 
                                         USB_REQ_RECIPIENT_ENDPOINT |
                                         USB_REQ_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQ_CLEAR_FEATURE;
  phost->Control.setup.b.wValue.w = FEATURE_SELECTOR_ENDPOINT;
  phost->Control.setup.b.wIndex.w = ep_num;
  phost->Control.setup.b.wLength.w = 0;           
  
  if ((ep_num & USB_REQ_DIR_MASK ) == USB_D2H)
  { /* EP Type is IN */
    pdev->host.hc[hc_num].toggle_in = 0; 
  }
  else
  {/* EP Type is OUT */
    pdev->host.hc[hc_num].toggle_out = 0; 
  }
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );   
}

/**
* @brief  USBH_ParseDevDesc 
*         This function Parses the device descriptor
* @param  dev_desc: device_descriptor destinaton address 
* @param  buf: Buffer where the source descriptor is available
* @param  length: Length of the descriptor
* @retval None
*/
static void  USBH_ParseDevDesc (USBH_DevDesc_TypeDef* dev_desc,
                                Uint8 *buf, 
                                Uint16 length)
{
  dev_desc->bLength            = *(Uint8  *) (buf +  0);
  dev_desc->bDescriptorType    = *(Uint8  *) (buf +  1);
  dev_desc->bcdUSB             = LE16 (buf +  2);
  dev_desc->bDeviceClass       = *(Uint8  *) (buf +  4);
  dev_desc->bDeviceSubClass    = *(Uint8  *) (buf +  5);
  dev_desc->bDeviceProtocol    = *(Uint8  *) (buf +  6);
  dev_desc->bMaxPacketSize     = *(Uint8  *) (buf +  7);
  
  if (length > 8)
  { /* For 1st time after device connection, Host may issue only 8 bytes for 
    Device Descriptor Length  */
    dev_desc->idVendor           = LE16 (buf +  8);
    dev_desc->idProduct          = LE16 (buf + 10);
    dev_desc->bcdDevice          = LE16 (buf + 12);
    dev_desc->iManufacturer      = *(Uint8  *) (buf + 14);
    dev_desc->iProduct           = *(Uint8  *) (buf + 15);
    dev_desc->iSerialNumber      = *(Uint8  *) (buf + 16);
    dev_desc->bNumConfigurations = *(Uint8  *) (buf + 17);
  }
}

/**
* @brief  USBH_ParseCfgDesc 
*         This function Parses the configuration descriptor
* @param  cfg_desc: Configuration Descriptor address
* @param  itf_desc: Interface Descriptor address
* @param  ep_desc: Endpoint Descriptor address
* @param  buf: Buffer where the source descriptor is available
* @param  length: Length of the descriptor
* @retval None
*/
static void  USBH_ParseCfgDesc (USBH_CfgDesc_TypeDef* cfg_desc,
                                USBH_InterfaceDesc_TypeDef* itf_desc,
                                USBH_EpDesc_TypeDef*  ep_desc, 
                                Uint8 *buf, 
                                Uint16 length)
{  
  USBH_InterfaceDesc_TypeDef    *pif ;
  USBH_EpDesc_TypeDef           *pep;  
  USBH_DescHeader_t             *pdesc = (USBH_DescHeader_t *)buf;
  Uint16                      ptr;
  int8_t                        if_ix;
  int8_t                        ep_ix;  
  
  pdesc   = (USBH_DescHeader_t *)buf;
  
  /* Parse configuration descriptor */
  cfg_desc->bLength             = *(Uint8  *) (buf + 0);
  cfg_desc->bDescriptorType     = *(Uint8  *) (buf + 1);
  cfg_desc->wTotalLength        = LE16 (buf + 2);
  cfg_desc->bNumInterfaces      = *(Uint8  *) (buf + 4);
  cfg_desc->bConfigurationValue = *(Uint8  *) (buf + 5);
  cfg_desc->iConfiguration      = *(Uint8  *) (buf + 6);
  cfg_desc->bmAttributes        = *(Uint8  *) (buf + 7);
  cfg_desc->bMaxPower           = *(Uint8  *) (buf + 8);    
  
  
  if (length > USB_CONFIGURATION_DESC_SIZE)
  {
    ptr = USB_LEN_CFG_DESC;
    
    if ( cfg_desc->bNumInterfaces <= USBH_MAX_NUM_INTERFACES) 
    {
      if_ix = 0;
      pif = (USBH_InterfaceDesc_TypeDef *)0;
      
      /* Parse Interface descriptor relative to the current configuration */
      if(cfg_desc->bNumInterfaces <= USBH_MAX_NUM_INTERFACES)
      {
        while (if_ix < cfg_desc->bNumInterfaces) 
        {
          pdesc = USBH_GetNextDesc((Uint8 *)pdesc, &ptr);
          if (pdesc->bDescriptorType   == USB_DESC_TYPE_INTERFACE) 
          {  
            pif               = &itf_desc[if_ix];
            USBH_ParseInterfaceDesc (pif, (Uint8 *)pdesc);
            ep_ix = 0;
            
            /* Parse Ep descriptors relative to the current interface */
            if(pif->bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS)
            {          
              while (ep_ix < pif->bNumEndpoints) 
              {
                pdesc = USBH_GetNextDesc((void* )pdesc, &ptr);
                if (pdesc->bDescriptorType   == USB_DESC_TYPE_ENDPOINT) 
                {  
                  pep               = &ep_desc[ep_ix];
                  USBH_ParseEPDesc (pep, (Uint8 *)pdesc);
                  ep_ix++;
                }
                else
                {
                  ptr += pdesc->bLength;
                }
              }
            }
            if_ix++;
          }
          else
          {
            ptr += pdesc->bLength;
          }
        }
      }
    }
  }  
}


/**
* @brief  USBH_ParseInterfaceDesc 
*         This function Parses the interface descriptor
* @param  if_descriptor : Interface descriptor destination
* @param  buf: Buffer where the descriptor data is available
* @retval None
*/
static void  USBH_ParseInterfaceDesc (USBH_InterfaceDesc_TypeDef *if_descriptor, 
                                      Uint8 *buf)
{
  if_descriptor->bLength            = *(Uint8  *) (buf + 0);
  if_descriptor->bDescriptorType    = *(Uint8  *) (buf + 1);
  if_descriptor->bInterfaceNumber   = *(Uint8  *) (buf + 2);
  if_descriptor->bAlternateSetting  = *(Uint8  *) (buf + 3);
  if_descriptor->bNumEndpoints      = *(Uint8  *) (buf + 4);
  if_descriptor->bInterfaceClass    = *(Uint8  *) (buf + 5);
  if_descriptor->bInterfaceSubClass = *(Uint8  *) (buf + 6);
  if_descriptor->bInterfaceProtocol = *(Uint8  *) (buf + 7);
  if_descriptor->iInterface         = *(Uint8  *) (buf + 8);
}

/**
* @brief  USBH_ParseEPDesc 
*         This function Parses the endpoint descriptor
* @param  ep_descriptor: Endpoint descriptor destination address
* @param  buf: Buffer where the parsed descriptor stored
* @retval None
*/
static void  USBH_ParseEPDesc (USBH_EpDesc_TypeDef  *ep_descriptor, 
                               Uint8 *buf)
{
  
  ep_descriptor->bLength          = *(Uint8  *) (buf + 0);
  ep_descriptor->bDescriptorType  = *(Uint8  *) (buf + 1);
  ep_descriptor->bEndpointAddress = *(Uint8  *) (buf + 2);
  ep_descriptor->bmAttributes     = *(Uint8  *) (buf + 3);
  ep_descriptor->wMaxPacketSize   = LE16 (buf + 4);
  ep_descriptor->bInterval        = *(Uint8  *) (buf + 6);
}

/**
* @brief  USBH_ParseStringDesc 
*         This function Parses the string descriptor
* @param  psrc: Source pointer containing the descriptor data
* @param  pdest: Destination address pointer
* @param  length: Length of the descriptor
* @retval None
*/
static void USBH_ParseStringDesc (Uint8* psrc, 
                                  Uint8* pdest, 
                                  Uint16 length)
{
  Uint16 strlength;
  Uint16 idx;
  
  /* The UNICODE string descriptor is not NULL-terminated. The string length is
  computed by substracting two from the value of the first byte of the descriptor.
  */
  
  /* Check which is lower size, the Size of string or the length of bytes read 
  from the device */
  
  if ( psrc[1] == USB_DESC_TYPE_STRING)
  { /* Make sure the Descriptor is String Type */
    
    /* psrc[0] contains Size of Descriptor, subtract 2 to get the length of string */      
    strlength = ( ( (psrc[0]-2) <= length) ? (psrc[0]-2) :length); 
    psrc += 2; /* Adjust the offset ignoring the String Len and Descriptor type */
    
    for (idx = 0; idx < strlength; idx+=2 )
    {/* Copy Only the string and ignore the UNICODE ID, hence add the src */
      *pdest =  psrc[idx];
      pdest++;
    }  
    *pdest = 0; /* mark end of string */  
  }
}

/**
* @brief  USBH_GetNextDesc 
*         This function return the next descriptor header
* @param  buf: Buffer where the cfg descriptor is available
* @param  ptr: data popinter inside the cfg descriptor
* @retval next header
*/
static  USBH_DescHeader_t  *USBH_GetNextDesc (Uint8   *pbuf, Uint16  *ptr)
{
  USBH_DescHeader_t  *pnext;
 
  *ptr += ((USBH_DescHeader_t *)pbuf)->bLength;  
  pnext = (USBH_DescHeader_t *)((Uint8 *)pbuf + \
         ((USBH_DescHeader_t *)pbuf)->bLength);
 
  return(pnext);
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




