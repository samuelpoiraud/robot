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
		MIDDLEWARE_addAnimatedImage(140, 30, &waiting);
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

	static bool_e test;
	Uint32 var = SWITCH_getDebug();
	static Uint32 lastVar1, lastVar2, lastVar3, lastVar4, lastVar5, lastVar6, lastVar7, lastVar8, lastVar9, lastVar10, lastVar11;
	static objectId_t id1, id2, id3, id4, id5, id6, id7, id8, id9, id10, id11, id12;

	if(!displayed){

		id1 = MIDDLEWARE_addText(50, 50, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%l8d", var);
		id2 = MIDDLEWARE_addText(50, 60, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON_GO_TO_HOME_PORT);
		id3 = MIDDLEWARE_addText(50, 70, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON_CALIBRATION_PORT);
		id4 = MIDDLEWARE_addText(50, 80, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON_PRINTMATCH_PORT);
		id5 = MIDDLEWARE_addText(50, 90, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON_SUSPEND_RESUMSE_PORT);
		id6 = MIDDLEWARE_addText(50, 100, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON0_PORT);
		id7 = MIDDLEWARE_addText(50, 110, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON1_PORT);
		id8 = MIDDLEWARE_addText(50, 120, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON2_PORT);
		id9 = MIDDLEWARE_addText(50, 130, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON3_PORT);
		id10 = MIDDLEWARE_addText(50, 140, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON4_PORT);
		id11 = MIDDLEWARE_addText(50, 150, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%d", BUTTON5_PORT);

		id12 = MIDDLEWARE_addButton(200, 100, 50, 50, "0", TRUE, &test, 0x0000, 0xF800, 0x001F, 0x0000);

		displayed = TRUE;
	}

	if(var != lastVar1){
		char txt[50];
		sprintf(txt, "%l8d", var);
		MIDDLEWARE_setText(id1, txt);
		lastVar1 = var;
	}

	if(BUTTON_GO_TO_HOME_PORT != lastVar2){
		char txt[50];
		sprintf(txt, "%d", BUTTON_GO_TO_HOME_PORT);
		MIDDLEWARE_setText(id2, txt);
		lastVar2 = var;
	}

	if(BUTTON_CALIBRATION_PORT != lastVar3){
		char txt[50];
		sprintf(txt, "%d", BUTTON_CALIBRATION_PORT);
		MIDDLEWARE_setText(id3, txt);
		lastVar3 = var;
	}

	if(BUTTON_PRINTMATCH_PORT != lastVar4){
		char txt[50];
		sprintf(txt, "%d", BUTTON_PRINTMATCH_PORT);
		MIDDLEWARE_setText(id4, txt);
		lastVar4 = var;
	}

	if(BUTTON_SUSPEND_RESUMSE_PORT != lastVar5){
		char txt[50];
		sprintf(txt, "%d", BUTTON_SUSPEND_RESUMSE_PORT);
		MIDDLEWARE_setText(id5, txt);
		lastVar5 = var;
	}

	if(BUTTON0_PORT != lastVar6){
		char txt[50];
		sprintf(txt, "%d", BUTTON0_PORT);
		MIDDLEWARE_setText(id6, txt);
		lastVar6 = var;
	}

	if(BUTTON1_PORT != lastVar7){
		char txt[50];
		sprintf(txt, "%d", BUTTON1_PORT);
		MIDDLEWARE_setText(id7, txt);
		lastVar7 = var;
	}

	if(BUTTON2_PORT != lastVar8){
		char txt[50];
		sprintf(txt, "%d", BUTTON2_PORT);
		MIDDLEWARE_setText(id8, txt);
		lastVar8 = var;
	}

	if(BUTTON3_PORT != lastVar9){
		char txt[50];
		sprintf(txt, "%d", BUTTON3_PORT);
		MIDDLEWARE_setText(id9, txt);
		lastVar9 = var;
	}

	if(BUTTON4_PORT != lastVar10){
		char txt[50];
		sprintf(txt, "%d", BUTTON4_PORT);
		MIDDLEWARE_setText(id10, txt);
		lastVar10 = var;
	}

	if(BUTTON5_PORT != lastVar11){
		char txt[50];
		sprintf(txt, "%d", BUTTON5_PORT);
		MIDDLEWARE_setText(id11, txt);
		lastVar11 = var;
	}


}

static void INTERFACE_IHM_custom(void){

}

bool_e INTERFACE_ihmAvailable(void){
	return TRUE;
}
