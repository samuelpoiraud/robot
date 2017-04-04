#include "interface.h"
#include "middleware.h"
#include "image/image.h"
#include "low layer/ili9341.h"
#include "low layer/xpt2046.h"
#include "../switch.h"
#include "uartOverLCD.h"

static volatile INTERFACE_ihm_e actualIhm = INTERFACE_IHM_WAIT;

static void INTERFACE_IHM_wait(void);
static void INTERFACE_IHM_custom(void);
static void INTERFACE_IHM_position(void);

void INTERFACE_init(void){
	ILI9341_init();
	MIDDLEWARE_init();
#ifdef USE_UART_OVER_LCD
	UART_OVER_LCD_init();
#endif
}

void INTERFACE_processMain(void){
#ifdef USE_UART_OVER_LCD
	UART_OVER_LCD_processMain();
#endif

	switch(actualIhm){
		case INTERFACE_IHM_WAIT:
			INTERFACE_IHM_wait();
			break;

		case INTERFACE_IHM_POSITION:
			INTERFACE_IHM_position();
			break;

		case INTERFACE_IHM_CUSTOM:
			INTERFACE_IHM_custom();
			break;
	}

	MIDDLEWARE_processMain();
}

void INTERFACE_setInterface(INTERFACE_ihm_e ihm){
	actualIhm = ihm;
	MIDDLEWARE_resetScreen();
}

static void INTERFACE_IHM_wait(void){
	static bool_e displayed = FALSE;

	if(!displayed){

		MIDDLEWARE_addImage(0, 0, &logoESEO);
		MIDDLEWARE_addAnimatedImage(100, 30, &waiting);
		MIDDLEWARE_addAnimatedImage(140, 30, &waiting);
		MIDDLEWARE_addAnimatedImage(180, 30, &waiting);
		MIDDLEWARE_addText(20, 225, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "En attente de l'initialisation des cartes");
		displayed = TRUE;
	}

	if(global.absolute_time > 5000){
		INTERFACE_setInterface(INTERFACE_IHM_POSITION);
	}
}

static void INTERFACE_IHM_position(void){
	static bool_e displayed = FALSE;

	/*if(!displayed){
		MIDDLEWARE_setBackground(ILI9341_COLOR_WHITE);
		MIDDLEWARE_addCircle(320/2, 240/2, 25, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK);
		MIDDLEWARE_addText(320/2 + 30, 240/2 - 10/5, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "Test");
		displayed = TRUE;
	}*/


	/*static bool_e touch = FALSE, touchC = FALSE, display = FALSE, lastDisplay = FALSE;
	static Uint8 pourcent = 50;

	union{
		Uint16 color;
		struct{
			Uint32 R		:5;
			Uint32 G		:6;
			Uint32 B		:5;
		}formated;
	}setValue, lastValue;

	static Sint32 R = 0b00011111, G = 0b00111111, B = 0b00011111;
	static char buffer[50];
	static objectId_t id;

	setValue.color = 0xFFFF;
	lastValue.color = 0xFFFF;*/

	Uint32 var = SWITCH_getDebug();
	static Uint32 lastVar = 0;
	static objectId_t id;

	if(!displayed){

		id = MIDDLEWARE_addText(50, 50, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%l8d", var);

		displayed = TRUE;
	}

	if(var != lastVar){
		char txt[50];
		sprintf(txt, "%l8d", var);
		MIDDLEWARE_setText(id, txt);
	}

	lastVar = var;
}

static void INTERFACE_IHM_custom(void){

}

bool_e INTERFACE_ihmAvailable(void){
	return TRUE;
}
