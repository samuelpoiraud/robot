#include "lcd.h"
#include "image/terrain.h"
#include "../IHM/view.h"
#include "middleware.h"
#include "low layer/ssd2119.h"
#include "low layer/touch.h"
#include "../QS/QS_outputlog.h"
#include "low layer/fonts.h"

#define REFRESH_TIME	(100) // Temps en ms entre chaque affichage des positions hokuyo
#define NB_MAX_CIRCLE	(50)
#define CIRCLE_COLOR	(SSD2119_COLOR_RED)
#define CROSS_MARGIN	(20)

typedef enum{
	INIT_LCD,
	INIT_TOUCH,
	DISPLAY_TERRAIN,
	DISPLAY_HOKUYO,
	DISPLAY_MENU,
	CALIBRATION,
	HOKUYO_MANAGER,
	DONE
}LCD_state_e;

typedef enum{
	CAL_INIT_LCD,
	CAL_FIRST_CROSS,
	CAL_SECOND_CROSS,
	CAL_COMPUTING,
	CAL_WAIT,
	CAL_DONE
}CALIBRATION_state_e;

typedef struct{
	Uint16 x;
	Uint16 y;
	Uint16 r;
}LCD_Circle_s;

typedef struct{
	LCD_Circle_s data[NB_MAX_CIRCLE];
	Uint8 size;
}LCD_hokuyoPosition_s;

static volatile LCD_hokuyoPosition_s hokuyoPosition;
static CALIBRATION_state_e LCD_calibrate(void);
static void LCD_recoverI2C(void);

void LCD_processMain(void) {
	static LCD_state_e state = INIT_LCD;
	static LCD_state_e lastState = DONE;
	static bool_e entrance = FALSE;
	static time32_t previousTime = 0;

	if(state != lastState) {
		entrance = TRUE;
	} else {
		entrance = FALSE;
	}
	lastState = state;

	// On ne veux pas utiliser la middleware dans certains états
	if(state != DISPLAY_TERRAIN || state != DISPLAY_HOKUYO || state != CALIBRATION) {
		MIDDLEWARE_processMain();
	}

	switch(state) {
		case INIT_LCD:
			//LCD_recoverI2C();
			state = DISPLAY_TERRAIN;
			break;
		case INIT_TOUCH:
			//LCD_recoverI2C();
			// Ce que l'on affiche au démarrage de la balise
			state = CALIBRATION;//DISPLAY_TERRAIN;
			break;
		case DISPLAY_TERRAIN:
			VIEW_drawEmptyTerrain(&terrain);
			state = DISPLAY_HOKUYO;
			break;
		case DISPLAY_HOKUYO:
			if((previousTime + REFRESH_TIME) < global.absolute_time){
				int i = 0;

				// On efface les positions hokuyo précédentes
				for(i = 0; i < hokuyoPosition.size; i++) {
					VIEW_drawCircle(hokuyoPosition.data[i].x, hokuyoPosition.data[i].y, hokuyoPosition.data[i].r, TRUE, CIRCLE_COLOR, &terrain);
				}

				// On récupère les positions hokuyo

				// On affiche les nouvelles positions
				for(i = 0; i < hokuyoPosition.size; i++) {
					VIEW_drawCircle(hokuyoPosition.data[i].x, hokuyoPosition.data[i].y, hokuyoPosition.data[i].r, FALSE, CIRCLE_COLOR, &terrain);
				}

				previousTime = global.absolute_time;
			}
			break;
		case DISPLAY_MENU:

			break;
		case CALIBRATION:
			if(LCD_calibrate() == CAL_DONE) {
				state = DISPLAY_MENU;
			}
			break;
		case HOKUYO_MANAGER:

			break;
		case DONE:

			break;
		default:
			break;
	}

}

static CALIBRATION_state_e LCD_calibrate(void) {
	static CALIBRATION_state_e state = CAL_INIT_LCD;
	static CALIBRATION_state_e lastState = CAL_DONE;
	static bool_e entrance = FALSE;
	static Sint16 pX1 = 0, pX2 = 0;
	static Sint16 pY1 = 0, pY2 = 0;

	if(state != lastState) {
		entrance = TRUE;
	} else {
		entrance = FALSE;
	}
	lastState = state;

	switch(state) {
		case CAL_INIT_LCD:
			SSD2119_fill(SSD2119_COLOR_BLACK);
			state = CAL_FIRST_CROSS;
			break;
		case CAL_FIRST_CROSS:
			if(entrance) {
				debug_printf("FIRST_CROSS\n");
				SSD2119_drawLine(10, 20, 30, 20, SSD2119_COLOR_WHITE);
				SSD2119_drawLine(20, 10, 20, 30, SSD2119_COLOR_WHITE);
				SSD2119_drawPixel(20, 20, SSD2119_COLOR_RED);
			}

			if(TOUCH_getAverageCoordinates(&pX1, &pY1, 2 ,TOUCH_COORDINATE_RAW)) {
				state = CAL_SECOND_CROSS;
			}
			break;
		case CAL_SECOND_CROSS:
			if(entrance) {
				debug_printf("SECOND_CROSS\n");
				SSD2119_fill(SSD2119_COLOR_BLACK);
				SSD2119_drawLine(289, 219, 309, 219, SSD2119_COLOR_WHITE);
				SSD2119_drawLine(299, 209, 299, 229, SSD2119_COLOR_WHITE);
				SSD2119_drawPixel(299, 219, SSD2119_COLOR_RED);
			}

			if(TOUCH_getAverageCoordinates(&pX2, &pY2, 2 ,TOUCH_COORDINATE_RAW)) {
				state = CAL_COMPUTING;
			}
			break;
		case CAL_COMPUTING:{
			debug_printf("COMPUTING\n");
			SSD2119_fill(SSD2119_COLOR_BLACK);
			TS_COEFF coeff;

			coeff.Ax = ((float)(-SSD2119_WIDTH+2*CROSS_MARGIN))/((float)pX1-(float)pX2);
			coeff.Bx = (float)CROSS_MARGIN-coeff.Ax*(float)pX1;
			coeff.Ay = ((float)(-SSD2119_HEIGHT+2*CROSS_MARGIN))/((float)pY1-(float)pY2);
			coeff.By = (float)CROSS_MARGIN-coeff.Ay*(float)pY1;

			TOUCH_setCoeff(coeff);

			state = CAL_WAIT;
		}break;
		case CAL_WAIT:{
			if(entrance) {
				/*TS_COEFF coeff = TOUCH_getCoeff();
				SSD2119_printf(10, 50, &Font_11x18, SSD2119_COLOR_RED, SSD2119_COLOR_BLACK, "Ax = %f", coeff.Ax);
				SSD2119_printf(10, 70, &Font_11x18, SSD2119_COLOR_RED, SSD2119_COLOR_BLACK, "Bx = %f", coeff.Bx);
				SSD2119_printf(10, 90, &Font_11x18, SSD2119_COLOR_RED, SSD2119_COLOR_BLACK, "Ay = %f", coeff.Ay);
				SSD2119_printf(10, 110, &Font_11x18, SSD2119_COLOR_RED, SSD2119_COLOR_BLACK, "By = %f", coeff.By);*/
			}

			if(TOUCH_getAverageCoordinates(&pX1, &pY1, 2 ,TOUCH_COORDINATE_RAW)) {
				state = CAL_DONE;
			}
		}break;
		case CAL_DONE:
			break;
		default:
			break;
	}

	return state;
}

static void LCD_recoverI2C(void) {
	volatile Uint32 i;
	Uint8 loop;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
#ifdef USE_BEACON_EYE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	#define idrSDA GPIOB->IDR11
	#define idrSCK GPIOB->IDR10
	Uint16 SCK_pin = GPIO_Pin_10;
#else
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	#define idrSDA GPIOB->IDR9
	#define idrSCK GPIOB->IDR8
	Uint16 SCK_pin = GPIO_Pin_8;
#endif
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);
	if(idrSCK == 0 || idrSDA == 0)
	{
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStruct.GPIO_Pin = SCK_pin;	//Pin SCK
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		loop = 0;
		do{
			loop++;
			idrSCK = 0;
			for(i=0;i<100000;i++);
			idrSCK = 1;
			for(i=0;i<100000;i++);
		}while(idrSDA == 0 || loop == 10);
		debug_printf("manuals clock pulses to recover I2C : %d\n",loop);
	}
}
