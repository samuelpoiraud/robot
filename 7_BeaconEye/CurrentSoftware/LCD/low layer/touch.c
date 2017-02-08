#include "touch.h"

#include <stdio.h>
#include "stmpe811qtr.h"
#include "ssd2119.h"
#include "../QS/QS_outputlog.h"

typedef struct{
	Uint16 X;
	Uint16 Y;
	Uint16 TouchDetected;
}TS_ADC;

TS_COEFF ts_coeff;

static void TOUCH_getFiltredTouch(TS_ADC *ts_adc, Uint8 nSamples);
static void TOUCH_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY);

void TOUCH_init() {

	IOE_Config();
	ts_coeff.Ax = 0.099379;
	ts_coeff.Bx = -29.813665;
	ts_coeff.Ay = -0.101609;
	ts_coeff.By = 289.585097;
}

void TOUCH_setConfig(void) {
	// For middleware
}

void TOUCH_setCoeff(TS_COEFF coeff) {
	ts_coeff.Ax = coeff.Ax;
	ts_coeff.Bx = coeff.Bx;
	ts_coeff.Ay = coeff.Ay;
	ts_coeff.By = coeff.By;
	debug_printf("New coeff :\n");
	debug_printf("Ax = %f\n", ts_coeff.Ax);
	debug_printf("Bx = %f\n", ts_coeff.Bx);
	debug_printf("By = %f\n", ts_coeff.By);
	debug_printf("By = %f\n", ts_coeff.By);
}

TS_COEFF TOUCH_getCoeff() {
	return ts_coeff;
}

bool_e TOUCH_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, TOUCH_coordinateMode_e coordinateMode) {

	TS_ADC ts_adc;
	nSamples = nSamples * 2;
	TOUCH_getFiltredTouch(&ts_adc, nSamples);

	*pX = ts_adc.X;
	*pY = ts_adc.Y;

	if(coordinateMode == TOUCH_COORDINATE_SCREEN_RELATIVE) {
		float xf = ts_coeff.Ax * (float)ts_adc.X + ts_coeff.Bx;
		float yf = ts_coeff.Ay * (float)ts_adc.Y + ts_coeff.By;

		*pX = (Sint16)xf;
		*pY = (Sint16)yf;

		TOUCH_convertCoordinateScreenMode(pX, pY);
	}

	return (ts_adc.TouchDetected > 0) ? TRUE : FALSE;
}

bool_e TOUCH_getCoordinates(Sint16 * pX, Sint16 * pY, TOUCH_coordinateMode_e coordinateMode){

	return TOUCH_getAverageCoordinates(pX, pY, 1, coordinateMode);
}

/**
 * @brief Indique les coordonnées de la zone touchée sur l'écran
 * @param ts_adc Coordonnées et activation ou non de l'interruption
 * @param nSamples Nombre d'échantillons
 */
static void TOUCH_getFiltredTouch(TS_ADC *ts_adc, Uint8 nSamples) {

	TS_STATE *pstate = NULL;

	uint16_t sum_x = 0;
	uint16_t sum_y = 0;

	pstate = IOE_TS_GetState();
	if(pstate->TouchDetected) {
		uint8_t i;

		for(i = 0; i < nSamples; i++) {
			pstate = IOE_TS_GetState();

			if(pstate->TouchDetected) {
				sum_x += pstate->X;
				sum_y += pstate->Y;
			} else {
				break;
			}
		}

		if(i == nSamples) {
			ts_adc->TouchDetected = 1;
			ts_adc->X = sum_x/nSamples;
			ts_adc->Y = sum_y/nSamples;
		} else {
			ts_adc->TouchDetected = 0;
			ts_adc->X = 0;
			ts_adc->Y = 0;
		}
	} else {
		ts_adc->TouchDetected = 0;
		ts_adc->X = 0;
		ts_adc->Y = 0;
	}
}

/**
 * @brief Transforme les coordonnées pour s'adapter à la disposition de l'écran
 * @param pX Coordonnée X de la touche
 * @param pY Coordonnée Y de la touche
 */
static void TOUCH_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY) {
	SSD2119_Options_t screenOption = SSD2119_getOptions();
	Sint16 tempX = *pX, tempY = *pY;

	switch(screenOption.orientation){
		case SSD2119_Orientation_Portrait_1 :
			*pX = (4096 - tempY) * screenOption.width / 4096;
			*pY = (4096 - tempX) * screenOption.height / 4096;
			break;

		case SSD2119_Orientation_Portrait_2 :
			*pX = tempY * screenOption.width / 4096;
			*pY = tempX * screenOption.height / 4096;
			break;

		case SSD2119_Orientation_Landscape_2 :
			*pX = screenOption.width - tempX;
			*pY = screenOption.height - tempY;
			break;

		case SSD2119_Orientation_Landscape_1 :
			*pX = tempX;
			*pY = tempY;
			break;
		}
}
