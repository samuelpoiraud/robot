/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : USB Host
 *  Description : Gère les classes suivant le prériphérique USB parmi celle définie dans usbh_class_list.c
 *  Auteur : amurzeau
 */

#ifndef USBH_CLASS_SWITCH_H
#define USBH_CLASS_SWITCH_H

#include "../Core/usbh_core.h"

//Indique quelle classe utiliser selon les infos données par le périph (USB = MSC, Hokuyo = CDC)
typedef struct {
	Uint8 interface_class;
	USBH_Class_cb_TypeDef *USBH_class_handler;
} USBH_ClassSwitch_ClassInfo_TypeDef;

extern USBH_Class_cb_TypeDef  USBH_ClassSwitch_cb;

#endif  /* USBH_CLASS_SWITCH_H */
