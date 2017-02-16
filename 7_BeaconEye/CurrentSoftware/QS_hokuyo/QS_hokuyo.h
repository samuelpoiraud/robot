/*
 * QS_hokuyo.h
 *
 *  Created on: 8 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_QS_HOKUYO_H_
#define QS_HOKUYO_QS_HOKUYO_H_

#include "../QS/QS_all.h"

#define LINE_FEED				0x0A							// Caractère de retour à la ligne
#define HOKUYO_BUFFER_READ_TIMEOUT	500  //ms
#define HOKUYO_ECHO_READ_TIMEOUT	100  //ms

void HOKUYO_init(void);
void HOKUYO_processMain(void);
void HOKUYO_putsCommand(Uint8 tab[], Uint16 length);
void HOKUYO_writeCommand(Uint8 tab[]);
Uint8 HOKUYO_convertChecksum(Uint8 checksum);

#endif /* QS_HOKUYO_QS_HOKUYO_H_ */
