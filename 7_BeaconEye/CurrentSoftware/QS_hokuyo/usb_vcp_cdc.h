/*
 * usb_vcp_cdc.h
 *
 *  Created on: 8 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_USB_VCP_CDC_H_
#define QS_HOKUYO_USB_VCP_CDC_H_

#include "../QS/QS_all.h"

bool_e VCP_init(Uint32 baudrate);
void VCP_write(Uint8 data);
Uint8 VCP_read();
bool_e VCP_isRxEmpty();
bool_e VCP_isTxFull();
bool_e VCP_isIdle();
bool_e VCP_hasOverflow();
bool_e VCP_hasFrameError();
bool_e VCP_resetErrors();	//retourne TRUE si il y avait des erreurs


#endif /* QS_HOKUYO_USB_VCP_CDC_H_ */
