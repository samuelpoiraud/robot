/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: usbh_class_switch.h 1111 2013-11-21 22:59:32Z amurzeau $
 *
 *  Package : USB Host
 *  Description : G�re les classes suivant le pr�riph�rique USB parmi celle d�finie dans usbh_class_list.c
 *  Auteur : amurzeau
 */

#ifndef USBH_CLASS_SWITCH_H
#define USBH_CLASS_SWITCH_H

#include "../Core/usbh_core.h"

//Indique quelle classe utiliser selon les infos donn�es par le p�riph (USB = MSC, Hokuyo = CDC)
typedef struct {
	Uint8 interface_class;
	USBH_Class_cb_TypeDef *USBH_class_handler;
} USBH_ClassSwitch_ClassInfo_TypeDef;

extern USBH_Class_cb_TypeDef  USBH_ClassSwitch_cb;

#endif  /* USBH_CLASS_SWITCH_H */
