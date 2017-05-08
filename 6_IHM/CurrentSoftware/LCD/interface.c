#include "interface.h"
#include "middleware.h"
#include "image/image.h"
#include "low layer/ili9341.h"
#include "low layer/xpt2046.h"
#include "../switch.h"
#include "../QS/QS_outputlog.h"

static volatile INTERFACE_ihm_e actualIhm = INTERFACE_IHM_WAIT;

static void INTERFACE_IHM_wait(bool_e entrance);
static void INTERFACE_IHM_custom(bool_e entrance);
static void INTERFACE_IHM_position(bool_e entrance);
static void INTERFACE_IHM_debug(bool_e entrance);

void INTERFACE_init(void){
	ILI9341_init();
	MIDDLEWARE_init();
	#ifdef USE_LCD_OVER_UART
		LCD_OVER_UART_init();
	#endif
}

void INTERFACE_processMain(void){
	static INTERFACE_ihm_e lastIhm = -10;
	bool_e entrance = FALSE;
	static bool_e lastStateSwitchLCD = FALSE;

	#ifdef USE_LCD_OVER_UART
		LCD_OVER_UART_processMain();
	#endif

	if(SWITCH_LCD_PORT != lastStateSwitchLCD && actualIhm != INTERFACE_IHM_WAIT){
		if(SWITCH_LCD_PORT)
			INTERFACE_setInterface(INTERFACE_IHM_POSITION);
		else
			INTERFACE_setInterface(INTERFACE_IHM_CUSTOM);

		LCD_OVER_UART_ihmControl(SWITCH_LCD_PORT);

		lastStateSwitchLCD = SWITCH_LCD_PORT;
	}

	if(lastIhm != actualIhm)
		entrance = TRUE;

	lastIhm= actualIhm;

	switch(actualIhm){
		case INTERFACE_IHM_WAIT:
			INTERFACE_IHM_wait(entrance);
			break;

		case INTERFACE_IHM_DEBUG:
			INTERFACE_IHM_debug(entrance);
			break;

		case INTERFACE_IHM_POSITION:
			INTERFACE_IHM_position(entrance);
			break;

		case INTERFACE_IHM_CUSTOM:
			INTERFACE_IHM_custom(entrance);
			break;

		case INTERFACE_IHM_HOKUYO:

			break;
	}

	MIDDLEWARE_processMain();
}

void INTERFACE_setInterface(INTERFACE_ihm_e ihm){
	debug_printf("setInterface %d\n", ihm);
	actualIhm = ihm;
	MIDDLEWARE_resetScreen();
}

INTERFACE_ihm_e INTERFACE_getInterface(){
	return actualIhm;
}

static void INTERFACE_IHM_wait(bool_e entrance){
	if(entrance){

		MIDDLEWARE_addImage(0, 0, &logoESEO);
		MIDDLEWARE_addAnimatedImage(140, 30, &waiting);
		MIDDLEWARE_addText(20, 225, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10,  "En attente de l'initialisation des cartes");
	}

	if(global.absolute_time > 5000){
		INTERFACE_setInterface(INTERFACE_IHM_POSITION);
	}
}

static void INTERFACE_IHM_debug(bool_e entrance){
	static bool_e test;
	Uint32 var = SWITCH_getDebug();
	static Uint32 lastVar1, lastVar2, lastVar3, lastVar4, lastVar5, lastVar6, lastVar7, lastVar8, lastVar9, lastVar10, lastVar11;
	static objectId_t id1, id2, id3, id4, id5, id6, id7, id8, id9, id10, id11;

	if(entrance){

		id1 = MIDDLEWARE_addText(50, 50, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%8d", var);
		id2 = MIDDLEWARE_addText(50, 60, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON_SELFTEST_PORT);
		id3 = MIDDLEWARE_addText(50, 70, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON_CALIBRATION_PORT);
		id4 = MIDDLEWARE_addText(50, 80, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON_PRINTMATCH_PORT);
		id5 = MIDDLEWARE_addText(50, 90, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON_SUSPEND_RESUMSE_PORT);
		id6 = MIDDLEWARE_addText(50, 100, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON0_PORT);
		id7 = MIDDLEWARE_addText(50, 110, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON1_PORT);
		id8 = MIDDLEWARE_addText(50, 120, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON2_PORT);
		id9 = MIDDLEWARE_addText(50, 130, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON3_PORT);
		id10 = MIDDLEWARE_addText(50, 140, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON4_PORT);
		id11 = MIDDLEWARE_addText(50, 150, ILI9341_COLOR_BLACK, ILI9341_TRANSPARENT, TEXT_FONTS_7x10, "%d", BUTTON5_PORT);

		MIDDLEWARE_addButton(200, 100, 50, 50, TRUE, &test, 0x0000, 0xF800, 0x001F, 0x0000, TEXT_FONTS_7x10, "0");
	}

	if(var != lastVar1){
		MIDDLEWARE_setText(id1, "%8d", var);
		lastVar1 = var;
	}

	if(BUTTON_SELFTEST_PORT != lastVar2){
		MIDDLEWARE_setText(id2, "%d", BUTTON_SELFTEST_PORT);
		lastVar2 = BUTTON_SELFTEST_PORT;
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

static void INTERFACE_IHM_position(bool_e entrance){
	static objectId_t x, y, teta, voltage, advIr1, advIr2, adv1, adv2;
	static time32_t lastRefresh;

	if(entrance){
		x = MIDDLEWARE_addText(50, 50, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "x : %4d", global.pos.x);
		y = MIDDLEWARE_addText(50, 65, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "y : %4d", global.pos.y);
		teta = MIDDLEWARE_addText(50, 80, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "teta : %4d  |  %4d°", global.pos.teta, global.pos.teta*180/PI4096);

		voltage = MIDDLEWARE_addText(50, 100, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "%4d mV", global.voltage.Vpermanent);

		advIr1 = MIDDLEWARE_addText(10, 115, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "advIr1 dist : %4d  |  angle : %4d..", global.foe[0].dist, ((Sint32)global.foe[0].angle)*180/PI4096);
		advIr2 = MIDDLEWARE_addText(10, 130, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "advIr2 dist : %4d  |  angle : %4d..", global.foe[1].dist, ((Sint32)global.foe[1].angle)*180/PI4096);
		adv1 = MIDDLEWARE_addText(10, 145, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "adv1 dist : %4d  |  angle : %4d..", global.foe[2].dist, ((Sint32)global.foe[2].angle)*180/PI4096);
		adv2 = MIDDLEWARE_addText(10, 160, 0x0000, 0xFFFF, TEXT_FONTS_7x10, "adv2 dist : %4d  |  angle : %4d..", global.foe[3].dist, ((Sint32)global.foe[3].angle)*180/PI4096);
	}

	if(global.absolute_time - lastRefresh > 100){
		MIDDLEWARE_setText(x, "x : %4d", global.pos.x);
		MIDDLEWARE_setText(y, "y : %4d", global.pos.y);
		MIDDLEWARE_setText(teta, "teta : %4d  |  %4d°", global.pos.teta, global.pos.teta*180/PI4096);
		MIDDLEWARE_setText(voltage, "%4d mV", global.voltage.Vpermanent);

		if(global.foe[0].fiability_error == AUCUNE_ERREUR)
			MIDDLEWARE_setText(advIr1, "advIr1 dist : %4d  |  angle : %4ld", global.foe[0].dist, ((Sint32)global.foe[0].angle)*180/PI4096);
		else
			MIDDLEWARE_setText(advIr1, "advIr1 --------------------------------");

		if(global.foe[1].fiability_error == AUCUNE_ERREUR)
			MIDDLEWARE_setText(advIr2, "advIr2 dist : %4d  |  angle : %4ld", global.foe[1].dist, ((Sint32)global.foe[1].angle)*180/PI4096);
		else
			MIDDLEWARE_setText(advIr2, "advIr2 --------------------------------");


		MIDDLEWARE_setText(adv1, "adv1 dist : %4d  |  angle : %4ld", global.foe[2].dist, ((Sint32)global.foe[2].angle)*180/PI4096);
		MIDDLEWARE_setText(adv2, "adv2 dist : %4d  |  angle : %4ld", global.foe[3].dist, ((Sint32)global.foe[3].angle)*180/PI4096);
	}


}

static void INTERFACE_IHM_custom(bool_e entrance){

}
