/*
 * stmpe811.h
 *
 *  Created on: 20 févr. 2017
 *      Author: guill
 */

#ifndef LCD_LOW_LAYER_STMPE811_H_
#define LCD_LOW_LAYER_STMPE811_H_

#include "../../QS/QS_all.h"

typedef enum{
	STMPE811_COORDINATE_RAW,
	STMPE811_COORDINATE_SCREEN_RELATIVE
} STMPE811_coordinateMode_e;

void STMPE811_init(void);
bool_e STMPE811_getCoordinates(Sint16 * pX, Sint16 * pY, STMPE811_coordinateMode_e coordinateMode);
bool_e STMPE811_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, STMPE811_coordinateMode_e coordinateMode);
bool_e STMPE811_isConnected(void);

#endif /* LCD_LOW_LAYER_STMPE811_H_ */
