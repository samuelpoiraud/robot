/*
 * QS_hokuyo.h
 *
 *  Created on: 8 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_QS_HOKUYO_H_
#define QS_HOKUYO_QS_HOKUYO_H_

#include "../QS/QS_all.h"

void HOKUYO_init(void);
void HOKUYO_processMain(void);
void HOKUYO_putsCommand(Uint8 tab[], Uint16 length);

#endif /* QS_HOKUYO_QS_HOKUYO_H_ */
