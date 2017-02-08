#ifndef TOUCH_H
	#define TOUCH_H
	#include "../../QS/QS_all.h"

	typedef enum{
		TOUCH_COORDINATE_RAW,
		TOUCH_COORDINATE_SCREEN_RELATIVE
	}TOUCH_coordinateMode_e;

	typedef struct{
		float Ax;
		float Ay;
		float Bx;
		float By;
	}TS_COEFF;

	void TOUCH_init(void);
	void TOUCH_setConfig(void);
	void TOUCH_setCoeff(TS_COEFF coeff);
	TS_COEFF TOUCH_getCoeff();
	bool_e TOUCH_getCoordinates(Sint16 * pX, Sint16 * pY, TOUCH_coordinateMode_e coordinateMode);
	bool_e TOUCH_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, TOUCH_coordinateMode_e coordinateMode);

#endif //TOUCH_H
