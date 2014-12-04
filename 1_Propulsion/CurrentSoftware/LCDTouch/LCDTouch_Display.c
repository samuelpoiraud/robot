/*
 * LCDTouch_Display.c
 *
 *  Created on: 18 déc. 2013
 *      Author: Herzaeone
 */


#include "LCDTouch_Display.h"

#ifdef LCD_TOUCH
#define LCDTOUCH_DISPLAY_C_

#include "fonts.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"

volatile Uint8 pos = 0;

void LCD_write_text(char *str){
	LCD_SetTextColor(Green);
	LCD_Clear(Black);
	LCD_DisplayStringLine(LINE(1),(uint8_t *)str);
}

#endif
