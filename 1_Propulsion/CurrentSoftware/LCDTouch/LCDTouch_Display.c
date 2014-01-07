/*
 * LCDTouch_Display.c
 *
 *  Created on: 18 déc. 2013
 *      Author: Herzaeone
 */


#include "LCDTouch_Display.h"

#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
#define LCDTOUCH_DISPLAY_C_

#include "fonts.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"

volatile Uint8 pos = 0;

void LCD_write_text(char *str){
	LCD_SetTextColor(Green);
	LCD_Clear(Black);
	LCD_DisplayStringLine(LINE(1),str);
}

#endif
