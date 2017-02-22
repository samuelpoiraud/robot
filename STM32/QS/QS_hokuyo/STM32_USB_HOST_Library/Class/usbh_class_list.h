/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: usbh_class_list.h 1056 2013-11-09 21:34:05Z amurzeau $
 *
 *  Package : USB Host
 *  Description : Liste des correspondances des classes USB et des handlers
 *  Auteur : amurzeau
 */

#ifndef USBH_CLASS_LIST_H
#define USBH_CLASS_LIST_H

#include "usbh_class_switch.h"

extern const USBH_ClassSwitch_ClassInfo_TypeDef usbh_class_list[];
extern const Uint8 usbh_class_list_count;

#endif // USBH_CLASS_LIST_H
