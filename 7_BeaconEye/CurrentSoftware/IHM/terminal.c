/*
 * terminal.c
 *
 *  Created on: 14 févr. 2017
 *      Author: guill
 */

#include "terminal.h"
#include "../LCD/low layer/ssd2119.h"
#include <stdio.h>
#include <stdarg.h>

#define BACKGROUND_COLOR		SSD2119_COLOR_BLACK
#define FONT_COLOR				SSD2119_COLOR_GREEN2

#define TERMINAL_X				0
#define TERMINAL_Y				200
#define TERMINAL_WIDTH			280
#define TERMINAL_HEIGHT			40

#define NB_LINE					3
#define GAP						2
#define FONT_HEIGHT				10
#define FONT_WIDTH				7
#define LINE_SIZE_MAX			((Uint8)((TERMINAL_WIDTH - 1) / 7))
#define BORDER					2								// Marge à gauche
#define LINE_1					204
#define LINE_2					(LINE_1 + FONT_HEIGHT + GAP)
#define LINE_3					(LINE_2 + FONT_HEIGHT + GAP)

static char lines[NB_LINE][LINE_SIZE_MAX + 1];
static bool_e isInitialized = FALSE;							// Protection contre le double initialisation

static void TERMINAL_clear();
static void TERMINAL_updateDisplay();
static void TERMINAL_writeLine(Uint8 line, char *str);

void TERMINAL_init() {
	if(!isInitialized) {
		TERMINAL_clear();
		isInitialized = TRUE;
	}
}

void TERMINAL_puts(char *str) {
	strncpy(lines[0], lines[1], LINE_SIZE_MAX);
	strncpy(lines[1], lines[2], LINE_SIZE_MAX);
	strncpy(lines[2], str, LINE_SIZE_MAX);
	TERMINAL_updateDisplay();
}

void TERMINAL_printf(const char *format, ...) {
	char buffer[256];

	va_list args_list;
	va_start(args_list, format);
	vsnprintf(buffer, 256, format, args_list);
	va_end(args_list);

	TERMINAL_puts(buffer);
}

static void TERMINAL_updateDisplay() {
	TERMINAL_clear();
	TERMINAL_writeLine(LINE_1, lines[0]);
	TERMINAL_writeLine(LINE_2, lines[1]);
	TERMINAL_writeLine(LINE_3, lines[2]);
}

static void TERMINAL_writeLine(Uint8 line, char *str) {
	SSD2119_puts(BORDER, line, str, &Font_7x10, FONT_COLOR, BACKGROUND_COLOR);
}

static void TERMINAL_clear() {
	SSD2119_drawFilledRectangle(TERMINAL_X, TERMINAL_Y,
					            TERMINAL_X + TERMINAL_WIDTH - 1,
								TERMINAL_Y + TERMINAL_HEIGHT - 1,
								BACKGROUND_COLOR);
}
