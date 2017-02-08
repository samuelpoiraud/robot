/**
  ******************************************************************************
  * @file    usbh_def.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   Definitions used in the USB host library
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
/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_DEF
  * @brief This file is includes USB descriptors
  * @{
  */ 

#ifndef  USBH_DEF_H
#define  USBH_DEF_H

#ifndef USBH_NULL
#define USBH_NULL ((void *)0)
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#define ValBit(VAR,POS)                               (VAR & (1 << POS))
#define SetBit(VAR,POS)                               (VAR |= (1 << POS))
#define ClrBit(VAR,POS)                               (VAR &= ((1 << POS)^255))

#define  LE16(addr)             (((Uint16)(*((Uint8 *)(addr))))\
                                + (((Uint16)(*(((Uint8 *)(addr)) + 1))) << 8))

#define  USB_LEN_DESC_HDR                               0x02
#define  USB_LEN_DEV_DESC                               0x12
#define  USB_LEN_CFG_DESC                               0x09
#define  USB_LEN_IF_DESC                                0x09
#define  USB_LEN_EP_DESC                                0x07
#define  USB_LEN_OTG_DESC                               0x03
#define  USB_LEN_SETUP_PKT                              0x08

/* bmRequestType :D7 Data Phase Transfer Direction  */
#define  USB_REQ_DIR_MASK                               0x80
#define  USB_H2D                                        0x00
#define  USB_D2H                                        0x80

/* bmRequestType D6..5 Type */
#define  USB_REQ_TYPE_STANDARD                          0x00
#define  USB_REQ_TYPE_CLASS                             0x20
#define  USB_REQ_TYPE_VENDOR                            0x40
#define  USB_REQ_TYPE_RESERVED                          0x60

/* bmRequestType D4..0 Recipient */
#define  USB_REQ_RECIPIENT_DEVICE                       0x00
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02
#define  USB_REQ_RECIPIENT_OTHER                        0x03

/* Table 9-4. Standard Request Codes  */
/* bRequest , Value */
#define  USB_REQ_GET_STATUS                             0x00
#define  USB_REQ_CLEAR_FEATURE                          0x01
#define  USB_REQ_SET_FEATURE                            0x03
#define  USB_REQ_SET_ADDRESS                            0x05
#define  USB_REQ_GET_DESCRIPTOR                         0x06
#define  USB_REQ_SET_DESCRIPTOR                         0x07
#define  USB_REQ_GET_CONFIGURATION                      0x08
#define  USB_REQ_SET_CONFIGURATION                      0x09
#define  USB_REQ_GET_INTERFACE                          0x0A
#define  USB_REQ_SET_INTERFACE                          0x0B
#define  USB_REQ_SYNCH_FRAME                            0x0C

/* Table 9-5. Descriptor Types of USB Specifications */
#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_INTERFACE_POWER                     8
#define  USB_DESC_TYPE_HID                                 0x21
#define  USB_DESC_TYPE_HID_REPORT                          0x22


#define USB_DEVICE_DESC_SIZE                               18
#define USB_CONFIGURATION_DESC_SIZE                        9
#define USB_HID_DESC_SIZE                                  9
#define USB_INTERFACE_DESC_SIZE                            9
#define USB_ENDPOINT_DESC_SIZE                             7

/* Descriptor Type and Descriptor Index  */
/* Use the following values when calling the function USBH_GetDescriptor  */
#define  USB_DESC_DEVICE                    ((USB_DESC_TYPE_DEVICE << 8) & 0xFF00)
#define  USB_DESC_CONFIGURATION             ((USB_DESC_TYPE_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_STRING                    ((USB_DESC_TYPE_STRING << 8) & 0xFF00)
#define  USB_DESC_INTERFACE                 ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_ENDPOINT                  ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_DEVICE_QUALIFIER          ((USB_DESC_TYPE_DEVICE_QUALIFIER << 8) & 0xFF00)
#define  USB_DESC_OTHER_SPEED_CONFIGURATION ((USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_INTERFACE_POWER           ((USB_DESC_TYPE_INTERFACE_POWER << 8) & 0xFF00)
#define  USB_DESC_HID_REPORT                ((USB_DESC_TYPE_HID_REPORT << 8) & 0xFF00)
#define  USB_DESC_HID                       ((USB_DESC_TYPE_HID << 8) & 0xFF00)


#define  USB_EP_TYPE_CTRL                               0x00
#define  USB_EP_TYPE_ISOC                               0x01
#define  USB_EP_TYPE_BULK                               0x02
#define  USB_EP_TYPE_INTR                               0x03

#define  USB_EP_DIR_OUT                                 0x00
#define  USB_EP_DIR_IN                                  0x80
#define  USB_EP_DIR_MSK                                 0x80  

/* supported classes */
#define USB_MSC_CLASS                                   0x08
#define USB_HID_CLASS                                   0x03

/* Interface Descriptor field values for HID Boot Protocol */
#define HID_BOOT_CODE                                  0x01    
#define HID_KEYBRD_BOOT_CODE                           0x01
#define HID_MOUSE_BOOT_CODE                            0x02

/* As per USB specs 9.2.6.4 :Standard request with data request timeout: 5sec
   Standard request with no data stage timeout : 50ms */
#define DATA_STAGE_TIMEOUT                              5000 
#define NODATA_STAGE_TIMEOUT                            50

/**
  * @}
  */ 


#define USBH_CONFIGURATION_DESCRIPTOR_SIZE (USB_CONFIGURATION_DESC_SIZE \
                                           + USB_INTERFACE_DESC_SIZE\
                                           + (USBH_MAX_NUM_ENDPOINTS * USB_ENDPOINT_DESC_SIZE))


#define CONFIG_DESC_wTOTAL_LENGTH (ConfigurationDescriptorData.ConfigDescfield.\
                                          ConfigurationDescriptor.wTotalLength)


/*  This Union is copied from usb_core.h  */
typedef union
{
  Uint16 w;
  struct BW
  {
    Uint8 msb;
    Uint8 lsb;
  }
  bw;
}
Uint16_Uint8;


typedef union _USB_Setup
{
  Uint8 d8[8];
  
  struct _SetupPkt_Struc
  {
    Uint8           bmRequestType;
    Uint8           bRequest;
    Uint16_Uint8  wValue;
    Uint16_Uint8  wIndex;
    Uint16_Uint8  wLength;
  } b;
} 
USB_Setup_TypeDef;  

typedef  struct  _DescHeader 
{
    Uint8  bLength;       
    Uint8  bDescriptorType;
} 
USBH_DescHeader_t;

typedef struct _DeviceDescriptor
{
  Uint8   bLength;
  Uint8   bDescriptorType;
  Uint16  bcdUSB;        /* USB Specification Number which device complies too */
  Uint8   bDeviceClass;
  Uint8   bDeviceSubClass; 
  Uint8   bDeviceProtocol;
  /* If equal to Zero, each interface specifies its own class
  code if equal to 0xFF, the class code is vendor specified.
  Otherwise field is valid Class Code.*/
  Uint8   bMaxPacketSize;
  Uint16  idVendor;      /* Vendor ID (Assigned by USB Org) */
  Uint16  idProduct;     /* Product ID (Assigned by Manufacturer) */
  Uint16  bcdDevice;     /* Device Release Number */
  Uint8   iManufacturer;  /* Index of Manufacturer String Descriptor */
  Uint8   iProduct;       /* Index of Product String Descriptor */
  Uint8   iSerialNumber;  /* Index of Serial Number String Descriptor */
  Uint8   bNumConfigurations; /* Number of Possible Configurations */
}
USBH_DevDesc_TypeDef;


typedef struct _ConfigurationDescriptor
{
  Uint8   bLength;
  Uint8   bDescriptorType;
  Uint16  wTotalLength;        /* Total Length of Data Returned */
  Uint8   bNumInterfaces;       /* Number of Interfaces */
  Uint8   bConfigurationValue;  /* Value to use as an argument to select this configuration*/
  Uint8   iConfiguration;       /*Index of String Descriptor Describing this configuration */
  Uint8   bmAttributes;         /* D7 Bus Powered , D6 Self Powered, D5 Remote Wakeup , D4..0 Reserved (0)*/
  Uint8   bMaxPower;            /*Maximum Power Consumption */
}
USBH_CfgDesc_TypeDef;


typedef struct _HIDDescriptor
{
  Uint8   bLength;
  Uint8   bDescriptorType;
  Uint16  bcdHID;               /* indicates what endpoint this descriptor is describing */
  Uint8   bCountryCode;        /* specifies the transfer type. */
  Uint8   bNumDescriptors;     /* specifies the transfer type. */
  Uint8   bReportDescriptorType;    /* Maximum Packet Size this endpoint is capable of sending or receiving */  
  Uint16  wItemLength;          /* is used to specify the polling interval of certain transfers. */
}
USBH_HIDDesc_TypeDef;


typedef struct _InterfaceDescriptor
{
  Uint8 bLength;
  Uint8 bDescriptorType;
  Uint8 bInterfaceNumber;
  Uint8 bAlternateSetting;    /* Value used to select alternative setting */
  Uint8 bNumEndpoints;        /* Number of Endpoints used for this interface */
  Uint8 bInterfaceClass;      /* Class Code (Assigned by USB Org) */
  Uint8 bInterfaceSubClass;   /* Subclass Code (Assigned by USB Org) */
  Uint8 bInterfaceProtocol;   /* Protocol Code */
  Uint8 iInterface;           /* Index of String Descriptor Describing this interface */
  
}
USBH_InterfaceDesc_TypeDef;


typedef struct _EndpointDescriptor
{
  Uint8   bLength;
  Uint8   bDescriptorType;
  Uint8   bEndpointAddress;   /* indicates what endpoint this descriptor is describing */
  Uint8   bmAttributes;       /* specifies the transfer type. */
  Uint16  wMaxPacketSize;    /* Maximum Packet Size this endpoint is capable of sending or receiving */  
  Uint8   bInterval;          /* is used to specify the polling interval of certain transfers. */
}
USBH_EpDesc_TypeDef;
#endif
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

