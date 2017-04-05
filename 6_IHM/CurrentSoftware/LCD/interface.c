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
static void INTERFACE_IHM_debug(void);

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

static void INTERFACE_IHM_debug(void){
	static bool_e displayed = FALSE;
	static bool_e test;
	Uint32 var = SWITCH_getDebug();
	static Uint32 lastVar1, lastVar2, lastVar3, lastVar4, lastVar5, lastVar6, lastVar7, lastVar8, lastVar9, lastVar10, lastVar11;
	static objectId_t id1, id2, id3, id4, id5, id6, id7, id8, id9, id10, id11, id12;

	if(!displayed){

		id1 = MIDDLEWARE_addText(50, 50, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, "%8d", var);
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
		MIDDLEWARE_setText(id1, "%8d", var);
		lastVar1 = var;
	}

	if(BUTTON_GO_TO_HOME_PORT != lastVar2){
		MIDDLEWARE_setText(id2, "%d", BUTTON_GO_TO_HOME_PORT);
		lastVar2 = BUTTON_GO_TO_HOME_PORT;
	}

	if(BUTTON_CALIBRATION_PORT != lastVar3){
		MIDDLEWARE_setText(id3, "%d", BUTTON_CALIBRATION_PORT);
		lastVar3 = BUTTON_CALIBRATION_PORT;
	}

	if(BUTTON_PRINTMATCH_PORT != lastVar4){
		MIDDLEWARE_setText(id4, "%d", BUTTON_PRINTMATCH_PORT);
		lastVar4 = BUTTON_PRINTMATCH_PORT;
	}

	if(BUTTON_SUSPEND_RESUMSE_PORT != lastVar5){
		MIDDLEWARE_setText(id5, "%d", BUTTON_SUSPEND_RESUMSE_PORT);
		lastVar5 = BUTTON_SUSPEND_RESUMSE_PORT;
	}

	if(BUTTON0_PORT != lastVar6){
		MIDDLEWARE_setText(id6, "%d", BUTTON0_PORT);
		lastVar6 = BUTTON0_PORT;
	}

	if(BUTTON1_PORT != lastVar7){
		MIDDLEWARE_setText(id7, "%d", BUTTON1_PORT);
		lastVar7 = BUTTON1_PORT;
	}

	if(BUTTON2_PORT != lastVar8){
		MIDDLEWARE_setText(id8, "%d", BUTTON2_PORT);
		lastVar8 = BUTTON2_PORT;
	}

	if(BUTTON3_PORT != lastVar9){
		MIDDLEWARE_setText(id9, "%d", BUTTON3_PORT);
		lastVar9 = BUTTON3_PORT;
	}

	if(BUTTON4_PORT != lastVar10){
		MIDDLEWARE_setText(id10, "%d", BUTTON4_PORT);
		lastVar10 = BUTTON4_PORT;
	}

	if(BUTTON5_PORT != lastVar11){
		MIDDLEWARE_setText(id11, "%d", BUTTON5_PORT);
		lastVar11 = BUTTON5_PORT;
	}
}

static void INTERFACE_IHM_position(void){
	static bool_e init = FALSE;
	static objectId_t x, y, teta, voltage;
	static time32_t lastRefresh;

	if(!init){
		x = MIDDLEWARE_addText(50, 50, 0x0000, 0xFFFF, "x : %d", global.pos.x);
		y = MIDDLEWARE_addText(50, 65, 0x0000, 0xFFFF, "y : %d", global.pos.y);
		teta = MIDDLEWARE_addText(50, 80, 0x0000, 0xFFFF, "teta : %d  |  %d°", global.pos.teta, global.pos.teta*180/PI4096);

		voltage = MIDDLEWARE_addText(50, 100, 0x0000, 0xFFFF, "%d mV", global.voltage.Vpermanent);

		init = TRUE;
	}

	if(global.absolute_time - lastRefresh > 100){
		MIDDLEWARE_setText(x, "x : %d", global.pos.x);
		MIDDLEWARE_setText(y, "y : %d", global.pos.y);
		MIDDLEWARE_setText(teta, "teta : %d  |  %d°", global.pos.teta, global.pos.teta*180/PI4096);
		MIDDLEWARE_setText(voltage, "%d mV", global.voltage.Vpermanent);
	}


}

static void INTERFACE_IHM_custom(void){

}

bool_e INTERFACE_ihmAvailable(void){
	return TRUE;
}
