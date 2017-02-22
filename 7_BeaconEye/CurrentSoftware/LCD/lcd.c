#include "lcd.h"
#include "image/terrain.h"
#include "../IHM/view.h"
#include "middleware.h"
#include "low layer/ssd2119.h"
#include "../QS/QS_outputlog.h"
#include "low layer/fonts.h"
#include "../QS/QS_hokuyo/hokuyo.h"
#include "../QS/QS_hokuyo/hokuyo_config.h"
#include "../IHM/led.h"
#include "../IHM/terminal.h"
#include "../environment.h"
#include "low layer/stmpe811.h"
#include "../zone.h"

#define REFRESH_TIME	(100) // Temps en ms entre chaque affichage des positions hokuyo
#define NB_MAX_CIRCLE	(HOKUYO_MAX_FOES)
#define CIRCLE_COLOR	(SSD2119_COLOR_RED)
#define CROSS_MARGIN	(20)

#define LCD_COLOR_BACKGROUND		SSD2119_COLOR_BLACK
#define LCD_COLOR_BUTTON_TEXT		SSD2119_COLOR_WHITE
#define LCD_COLOR_BUTTON			SSD2119_COLOR_BLUE
#define LCD_COLOR_TOUCH_BUTTON		SSD2119_COLOR_RED
#define LCD_COLOR_BUTTON_BORDER		SSD2119_COLOR_BLUE2

static bool_e touch_run_screen = FALSE;
static bool_e touch_com_screen = FALSE;
static bool_e touch_infos_screen = FALSE;

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

static LCD_hokuyoPosition_s hokuyoPosition;
static void LCD_saveAdv(LCD_Circle_s *circle, HOKUYO_adversary_position *adv);
static void LCD_displayMainMenu();

void LCD_processMain(void) {
	static LCD_state_e state = INIT_LCD;
	static LCD_state_e lastState = DONE;
	static bool_e entrance = FALSE;
	static time32_t previousTime = 0;

	UNUSED_VAR(entrance);

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
			VIEW_drawBeaconPosition(ENVIRONMENT_getColor());
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
				hokuyoPosition.size = HOKUYO_getAdversariesNumber();
				for(i = 0; i < hokuyoPosition.size; i++) {
					HOKUYO_adversary_position *adv = HOKUYO_getAdversaryPosition(i);
					LCD_saveAdv(&hokuyoPosition.data[i], adv);
				}

				if(HOKUYO_getAdversariesNumber() > 0) {
					//TERMINAL_printf("Nb adv : %2d   (%d ms)   |   %d", HOKUYO_getAdversariesNumber(), HOKUYO_getLastMeasureDuration(), HOKUYO_getNbValidPoints());
				}

				// On affiche les nouvelles positions
				for(i = 0; i < hokuyoPosition.size; i++) {
					VIEW_drawCircle(hokuyoPosition.data[i].x, hokuyoPosition.data[i].y, hokuyoPosition.data[i].r, FALSE, CIRCLE_COLOR, &terrain);
				}

				ZONE_requestUpdate();

				previousTime = global.absolute_time;
			}
			break;
		case DISPLAY_MENU:
			if(entrance) {
				LCD_displayMainMenu();
			}

			break;
		case CALIBRATION:
			state = DISPLAY_MENU;
			break;
		case HOKUYO_MANAGER:

			break;
		case DONE:

			break;
		default:
			break;
	}

}

static void LCD_saveAdv(LCD_Circle_s *circle, HOKUYO_adversary_position *adv) {
	circle->y = FIELD_TO_LCD(adv->coordX);
	circle->x = FIELD_TO_LCD(adv->coordY);
	circle->r = 4;
}

static void LCD_displayMainMenu() {
	MIDDLEWARE_resetScreen();
	MIDDLEWARE_setBackground(LCD_COLOR_BACKGROUND);
	MIDDLEWARE_addButton(50, 60, 220, 50, "START GYROPODE", TRUE, &touch_run_screen, LCD_COLOR_BUTTON_TEXT, LCD_COLOR_BUTTON, LCD_COLOR_TOUCH_BUTTON, LCD_COLOR_BUTTON_BORDER);
	MIDDLEWARE_addButton(50, 120, 220, 50, "TEST COMMUNIATION", FALSE, &touch_com_screen, LCD_COLOR_BUTTON_TEXT, LCD_COLOR_BUTTON, LCD_COLOR_TOUCH_BUTTON, LCD_COLOR_BUTTON_BORDER);
	MIDDLEWARE_addButton(50, 180, 220, 50, "INFOS SYSTEM", FALSE, &touch_infos_screen, LCD_COLOR_BUTTON_TEXT, LCD_COLOR_BUTTON, LCD_COLOR_TOUCH_BUTTON, LCD_COLOR_BUTTON_BORDER);
}
