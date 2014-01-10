/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : USB Host
 *  Description : Liste des correspondances des classes USB et des handlers
 *  Auteur : amurzeau
 */

#include "usbh_class_list.h"
#include "CDC/usbh_cdc_core.h"
#include "MSC/usbh_msc_core.h"

const USBH_ClassSwitch_ClassInfo_TypeDef usbh_class_list[] = {
	{USB_CDC_CLASS_ID, &USBH_CDC_cb},
	{USB_MSC_CLASS_ID, &USBH_MSC_cb}
};

const Uint8 usbh_class_list_count = sizeof(usbh_class_list) / sizeof(USBH_ClassSwitch_ClassInfo_TypeDef);
