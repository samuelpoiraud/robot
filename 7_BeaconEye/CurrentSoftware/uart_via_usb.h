/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: uart_via_usb.h 1191 2013-12-09 20:34:44Z jroy $
 *
 *  Package : USB Host
 *  Description : Gère les classes suivant le prériphérique USB parmi celle définie dans usbh_class_list.c
 *  Auteur : amurzeau
 */

#ifndef UART_VIA_USB_H
#define UART_VIA_USB_H

#include "QS/QS_all.h"

bool_e UART_USB_init(Uint32 baudrate);

void   UART_USB_write(Uint8 data);
Uint8  UART_USB_read();
bool_e UART_USB_isRxEmpty();
bool_e UART_USB_isTxFull();
bool_e UART_USB_isIdle();
bool_e UART_USB_hasOverflow();
bool_e UART_USB_hasFrameError();
bool_e UART_USB_resetErrors();	//retourne TRUE si il y avait des erreurs


#endif // UART_VIA_USB_H


