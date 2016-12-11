/*
 *  Club Robot ESEO 2016 - 2017
 *
 *  Fichier : ssd2119.c
 *  Package : Beacon Eye
 *  Description : Driver SSD2119 de l'écran tactile
 *  Auteur : Guillaume Berland inspiré du code d'Arnaud Guilmet
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20161208
 */

#include "ssd2119.h"
#include "stm32f4xx_fsmc.h"
#include "../../QS/QS_ports.h"

#include <stdio.h>
#include <stdarg.h>

/* Pin definitions */
#define SSD2119_RST_SET()			GPIO_SetBits(LCD_RESET_PORT)
#define SSD2119_RST_RESET()			GPIO_ResetBits(LCD_RESET_PORT)
#define LCD_RST_PIN                 (GPIO_Pin_3)
#define LCD_RST_PORT                (GPIOD)
#define LCD_PWM_PIN                 (GPIO_Pin_13)
#define LCD_PWM_PORT                (GPIOD)

/* Private defines */
#define SSD2119_DEVICE_CODE_READ_REG  0x00
#define SSD2119_OSC_START_REG         0x00
#define SSD2119_OUTPUT_CTRL_REG       0x01
#define SSD2119_LCD_DRIVE_AC_CTRL_REG 0x02
#define SSD2119_PWR_CTRL_1_REG        0x03
#define SSD2119_DISPLAY_CTRL_REG      0x07
#define SSD2119_FRAME_CYCLE_CTRL_REG  0x0B
#define SSD2119_PWR_CTRL_2_REG        0x0C
#define SSD2119_PWR_CTRL_3_REG        0x0D
#define SSD2119_PWR_CTRL_4_REG        0x0E
#define SSD2119_GATE_SCAN_START_REG   0x0F
#define SSD2119_SLEEP_MODE_1_REG      0x10
#define SSD2119_ENTRY_MODE_REG        0x11
#define SSD2119_SLEEP_MODE_2_REG      0x12
#define SSD2119_GEN_IF_CTRL_REG       0x15
#define SSD2119_PWR_CTRL_5_REG        0x1E
#define SSD2119_RAM_DATA_REG          0x22
#define SSD2119_FRAME_FREQ_REG        0x25
#define SSD2119_ANALOG_SET_REG        0x26
#define SSD2119_VCOM_OTP_1_REG        0x28
#define SSD2119_VCOM_OTP_2_REG        0x29
#define SSD2119_GAMMA_CTRL_1_REG      0x30
#define SSD2119_GAMMA_CTRL_2_REG      0x31
#define SSD2119_GAMMA_CTRL_3_REG      0x32
#define SSD2119_GAMMA_CTRL_4_REG      0x33
#define SSD2119_GAMMA_CTRL_5_REG      0x34
#define SSD2119_GAMMA_CTRL_6_REG      0x35
#define SSD2119_GAMMA_CTRL_7_REG      0x36
#define SSD2119_GAMMA_CTRL_8_REG      0x37
#define SSD2119_GAMMA_CTRL_9_REG      0x3A
#define SSD2119_GAMMA_CTRL_10_REG     0x3B
#define SSD2119_V_RAM_POS_REG         0x44
#define SSD2119_H_RAM_START_REG       0x45
#define SSD2119_H_RAM_END_REG         0x46
#define SSD2119_X_RAM_ADDR_REG        0x4E
#define SSD2119_Y_RAM_ADDR_REG        0x4F

// Datasheet p 50 :
// ????????????????????????????????????????????????????????????????????????????????????????????????????????
// ? VSMode ? DFM1 ? DFM0 ? 0 ? DenMode ? Wmode ? Nosync ? Dmode ? TY1 ? TY0 ? ID1 ? ID0 ? AM ? 0 ? 0 ? 0 ?
// ????????????????????????????????????????????????????????????????????????????????????????????????????????
// ?   0    ?  1   ?  1   ? 0 ?    1    ?   0   ?    0   ?   0   ?  0  ?  0  ?  1  ?  1  ?  0 ? 0 ? 0 ? 0 ?
// ????????????????????????????????????????????????????????????????????????????????????????????????????????
//
// VSMode    Fréquence de rafraichissement dépendante de VSYNC
// DFM  	 Mode d'utilisation des couleurs (11 = 65k color | 10 = 262k color)
// DenMode   Prise en compte des signaux HSYNC et VSYNC (1 = ignore | 0 = control)
// Wmode	 Mode d'écriture dans la RAM (1 = Generic RGB Data | 0 = SPI)
// Nosync	 Synchronisation
// Dmode	 Choix de l'horloge pour le contrôleur graphique (1 = DOTCLK pin | 0 = Chip Oscillator)
// TY		 Encodage des couleurs pour le mode couleur 262k
// ID		 Incrémentation ou décrémentation automatique de GDDRAM après une écriture
// AM		 Direction du compteur de l'adresse de parcours de l'écran
#define ENTRY_MODE_LANDSCAPE		0x6830 // Default : 6830
#define ENTRY_MODE_LANDSCAPE_FLIP	0x6800
#define ENTRY_MODE_PORTRAIT			0x6828
#define ENTRY_MODE_PORTRAIT_FLIP	0x6818

/* Note: LCD /CS is NE1 - Bank 1 of NOR/SRAM Bank 1~4 */
#define  LCD_BASE_Data               ((u32)(0x60000000|0x00100000))
#define  LCD_BASE_Addr               ((u32)(0x60000000|0x00000000))
#define  LCD_CMD                     (*(vu16 *)LCD_BASE_Addr)
#define  LCD_Data                    (*(vu16 *)LCD_BASE_Data)

static volatile Uint16 SSD2119_x;
static volatile Uint16 SSD2119_y;
static volatile SSD2119_Options_t SSD2119_Opts;
static volatile bool_e initialised = FALSE;

/* Private functions */
static void SSD2119_defaultInit(void);
static void SSD2119_ctrlLinesConfig(void);
static void SSD2119_FSMCConfig(void);
static void SSD2119_INT_fill(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color);
static void SSD2119_delay(volatile Uint32 delay);
static void SSD2119_initLCD(void);
static void SSD2119_setCursor(Uint16 x, Uint16 y);
static void SSD2119_setCursorPosition(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1);
static void SSD2119_writeReg(Uint8 reg, Uint16 value);
static Uint16 SSD2119_readReg(Uint8 reg);
static void SSD2119_writeRAMPrepare(void);
static void SSD2119_writeRAM(Uint16 RGB_Code);
static Uint16 SSD2119_readRAM(void);

void SSD2119_init() {

	if(initialised) {
		return;
	}
	initialised = TRUE;

	// Init LCD
	SSD2119_initLCD();

	// Set default settings
	SSD2119_rotate(SSD2119_Orientation_Landscape_1);

	SSD2119_setCursor(0, 0);

	// Fill with white color
	SSD2119_fill(SSD2119_COLOR_BLACK);
}

void SSD2119_setConfig(void){

	// For middleware
}

void SSD2119_displayOn(void) {
	SSD2119_writeReg(SSD2119_DISPLAY_CTRL_REG, 0x0033);
}

void SSD2119_displayOff(void) {
	SSD2119_writeReg(SSD2119_DISPLAY_CTRL_REG, 0x0031);
}

SSD2119_Options_t SSD2119_getOptions(void){
	return SSD2119_Opts;
}

void SSD2119_drawPixel(Uint16 x, Uint16 y, Uint16 color) {

	SSD2119_setCursorPosition(x, y, x, y);
	SSD2119_writeRAMPrepare();
	SSD2119_writeRAM(color);
}

void SSD2119_fill(Uint16 color) {

	SSD2119_INT_fill(0, 0, SSD2119_Opts.width - 1, SSD2119_Opts.height, color);
}

void SSD2119_rotate(SSD2119_Orientation_t orientation) {

	if (orientation == SSD2119_Orientation_Portrait_1) {
		SSD2119_writeReg(SSD2119_ENTRY_MODE_REG, ENTRY_MODE_PORTRAIT);
	} else if (orientation == SSD2119_Orientation_Portrait_2) {
		SSD2119_writeReg(SSD2119_ENTRY_MODE_REG, ENTRY_MODE_PORTRAIT_FLIP);
	} else if (orientation == SSD2119_Orientation_Landscape_1) {
		SSD2119_writeReg(SSD2119_OUTPUT_CTRL_REG, 0x72EF);
	} else if (orientation == SSD2119_Orientation_Landscape_2) {
		SSD2119_writeReg(SSD2119_OUTPUT_CTRL_REG, 0x30EF);
	}

	SSD2119_Opts.orientation = orientation;

	if (orientation == SSD2119_Orientation_Portrait_1 || orientation == SSD2119_Orientation_Portrait_2) {
		SSD2119_Opts.width = SSD2119_HEIGHT;
		SSD2119_Opts.height = SSD2119_WIDTH;
	} else {
		SSD2119_Opts.width = SSD2119_WIDTH;
		SSD2119_Opts.height = SSD2119_HEIGHT;
	}
}

void SSD2119_puts(Uint16 x, Uint16 y, char *str, FontDef_t *font, Uint16 foreground, Uint32 background) {
	Uint16 startX = x;

	// Set X and Y coordinates
	SSD2119_x = x;
	SSD2119_y = y;

	while (*str) {
		// New line
		if (*str == '\n') {
			SSD2119_y += font->FontHeight + 1;
			// if after \n is also \r, than go to the left of the screen
			if (*(str + 1) == '\r') {
				SSD2119_x = 0;
				str++;
			} else {
				SSD2119_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}

		// Put character to LCD
		SSD2119_putc(SSD2119_x, SSD2119_y, *str++, font, foreground, background);
	}
}

void SSD2119_getStringSize(char *str, FontDef_t *font, Uint16 *width, Uint16 *height) {
	Uint16 w = 0;
	*height = font->FontHeight;
	while (*str++) {
		w += font->FontWidth;
	}
	*width = w;
}

void SSD2119_putc(Uint16 x, Uint16 y, char c, FontDef_t *font, Uint16 foreground, Uint32 background) {
	Uint32 i, b, j;

	// Set coordinates
	SSD2119_x = x;
	SSD2119_y = y;

	if ((SSD2119_x + font->FontWidth) > SSD2119_Opts.width) {
		// If at the end of a line of display, go to new line and set x to 0 position
		SSD2119_y += font->FontHeight;
		SSD2119_x = 0;
	}

	// Draw rectangle for background
	if(background != SSD2119_TRANSPARENT) {
		SSD2119_INT_fill(SSD2119_x, SSD2119_y, SSD2119_x + font->FontWidth, SSD2119_y + font->FontHeight, background);
	}

	// Draw font data
	for (i = 0; i < font->FontHeight; i++) {
		b = font->data[(c - 32) * font->FontHeight + i];
		for (j = 0; j < font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				SSD2119_drawPixel(SSD2119_x + j, (SSD2119_y + i), foreground);
			}
		}
	}

	// Set new pointer
	SSD2119_x += font->FontWidth;
}

void SSD2119_drawLine(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color) {
	/* Code by dewoller: https://github.com/dewoller */

	Sint16 dx, dy, sx, sy, err, e2;
	Uint16 tmp;

	// Check for overflow
	if (x0 >= SSD2119_Opts.width) {
		x0 = SSD2119_Opts.width - 1;
	}
	if (x1 >= SSD2119_Opts.width) {
		x1 = SSD2119_Opts.width - 1;
	}
	if (y0 >= SSD2119_Opts.height) {
		y0 = SSD2119_Opts.height - 1;
	}
	if (y1 >= SSD2119_Opts.height) {
		y1 = SSD2119_Opts.height - 1;
	}

	// Check correction
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	dx = x1 - x0;
	dy = y1 - y0;

	// Vertical or horizontal line
	if (dx == 0 || dy == 0) {
		SSD2119_INT_fill(x0, y0, x1, y1, color);
		return;
	}

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	while (1) {
		SSD2119_drawPixel(x0, y0, color);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void SSD2119_drawRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color) {

	SSD2119_drawLine(x0, y0, x1, y0, color);	//Top
	SSD2119_drawLine(x0, y0, x0, y1, color);	//Left
	SSD2119_drawLine(x1, y0, x1, y1, color);	//Right
	SSD2119_drawLine(x0, y1, x1, y1, color);	//Bottom
}

void SSD2119_drawFilledRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color) {

	Uint16 tmp;

	// Check correction
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	// Fill rectangle
	SSD2119_INT_fill(x0, y0, x1, y1, color);
}

void SSD2119_drawCircle(Uint16 x0, Uint16 y0, Uint16 r, Uint16 color) {

	Sint16 f = 1 - r;
	Sint16 ddF_x = 1;
	Sint16 ddF_y = -2 * r;
	Sint16 x = 0;
	Sint16 y = r;

	SSD2119_drawPixel(x0, y0 + r, color);
	SSD2119_drawPixel(x0, y0 - r, color);
	SSD2119_drawPixel(x0 + r, y0, color);
	SSD2119_drawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		SSD2119_drawPixel(x0 + x, y0 + y, color);
		SSD2119_drawPixel(x0 - x, y0 + y, color);
		SSD2119_drawPixel(x0 + x, y0 - y, color);
		SSD2119_drawPixel(x0 - x, y0 - y, color);

		SSD2119_drawPixel(x0 + y, y0 + x, color);
		SSD2119_drawPixel(x0 - y, y0 + x, color);
		SSD2119_drawPixel(x0 + y, y0 - x, color);
		SSD2119_drawPixel(x0 - y, y0 - x, color);
	}
}

void SSD2119_drawFilledCircle(Uint16 x0, Uint16 y0, Uint16 r, Uint16 color) {

	Sint16 f = 1 - r;
	Sint16 ddF_x = 1;
	Sint16 ddF_y = -2 * r;
	Sint16 x = 0;
	Sint16 y = r;

	SSD2119_drawPixel(x0, y0 + r, color);
	SSD2119_drawPixel(x0, y0 - r, color);
	SSD2119_drawPixel(x0 + r, y0, color);
	SSD2119_drawPixel(x0 - r, y0, color);
	SSD2119_drawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		SSD2119_drawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		SSD2119_drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		SSD2119_drawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		SSD2119_drawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
}

void SSD2119_putImage(Uint16 x, Uint16 y, Uint16 width, Uint16 height, const Uint16 *img, Uint32 size){

	SSD2119_setCursorPosition(x, y, x + width, y + height);

	// Overclock LCD (Increase consumption)
	SSD2119_writeReg(SSD2119_PWR_CTRL_1_REG, 0x1008);

	SSD2119_writeRAMPrepare();

#if defined(USE_RGB_565) && !defined(USE_RGB_332)
	Uint32 i;
	for(i=0; i < size; i++) {
		SSD2119_writeRAM(img[i]);
	}
#elif defined(USE_RGB_332) && !defined(USE_RGB_565)
	Uint16 temp;
	Uint32 i;
	for(i=0; i < size; i++) {
		temp=tab[i];
		temp=((temp&0xE0)<<8)+((temp&0x1C)<<6)+((temp&0x03)<<3);
		SSD2119_writeRAM(temp);
	}
#else
	#error "Veuillez mettre un type pour l'image (USE_RGB_332 ou USE_RGB_565)"
#endif

	// DeOverclock LCD (Normal consumption)
	SSD2119_writeReg(SSD2119_PWR_CTRL_1_REG, 0x1018);
}

void SSD2119_putImageWithTransparence(Uint16 x, Uint16 y, Uint16 width, Uint16 height, const Uint16 *img, Uint16 colorTransparence, Uint32 size){
	Uint32 i;

	// Overclock LCD (Increase consumption)
	SSD2119_writeReg(SSD2119_PWR_CTRL_1_REG, 0x1008);

	for(i=0; i < size; i++){
		if(img[i] != colorTransparence) {
			SSD2119_drawPixel(x + i % width, y + i / width, img[i]);
		}
	}

	// DeOverclock LCD (Normal consumption)
	SSD2119_writeReg(SSD2119_PWR_CTRL_1_REG, 0x1018);
}

void SSD2119_putColorInvertedImage(Uint16 x, Uint16 y, Uint16 width, Uint16 height, Uint16 replaceColor, const Uint16 *img, Uint16 colorTransparence, Uint32 size){
	Uint32 i;

	// Overclock LCD (Increase consumption)
	SSD2119_writeReg(SSD2119_PWR_CTRL_1_REG, 0x1008);

	for(i=0; i < size; i++){
		if(img[i] != colorTransparence) {
			SSD2119_drawPixel(x + i % width, y + i / width, replaceColor);
		}
	}

	// DeOverclock LCD (Normal consumption)
	SSD2119_writeReg(SSD2119_PWR_CTRL_1_REG, 0x1018);
}

void SSD2119_printf(Uint16 x, Uint16 y, FontDef_t *font, Uint16 foreground, Uint32 background, const char *format, ...){
	char buffer[256];

	va_list args_list;
	va_start(args_list, format);
	vsnprintf(buffer, 256, format, args_list);
	va_end(args_list);

	SSD2119_puts(x, y, buffer, font, foreground, background);
}

void SSD2119_drawRoundRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint8 radius, Uint16 color) {

	Sint16 tSwitch;
	Uint16 tmpX = 0, tmpY = radius;
	tSwitch = 3 - 2 * radius;
	Uint16 width = x1 - x0;
	Uint16 height = y1 - y0;

	while (tmpX <= tmpY) {
		// upper left corner
		SSD2119_drawPixel(x0+radius - tmpX, y0+radius - tmpY, color); // upper half
		SSD2119_drawPixel(x0+radius - tmpY, y0+radius - tmpX, color); // lower half

		// upper right corner
		SSD2119_drawPixel(x0+width-radius + tmpX, y0+radius - tmpY, color); // upper half
		SSD2119_drawPixel(x0+width-radius + tmpY, y0+radius - tmpX, color); // lower half

		// lower right corner
		SSD2119_drawPixel(x0+width-radius + tmpX, y0+height-radius + tmpY, color); // lower half
		SSD2119_drawPixel(x0+width-radius + tmpY, y0+height-radius + tmpX, color); // upper half

		// lower left corner
		SSD2119_drawPixel(x0+radius - tmpX, y0+height-radius + tmpY, color); // lower half
		SSD2119_drawPixel(x0+radius - tmpY, y0+height-radius + tmpX, color); // upper half

		if (tSwitch < 0) {
				tSwitch += (4 * tmpX + 6);
		} else {
			tSwitch += (4 * (tmpX - tmpY) + 10);
			tmpY--;
		}
		tmpX++;
	}

	SSD2119_drawLine(x0 + radius, y0, x1 - radius, y0, color);	// top
	SSD2119_drawLine(x0 + radius, y1, x1 - radius, y1, color);	// bottom
	SSD2119_drawLine(x0, y0 + radius, x0, y1 - radius, color);	// left
	SSD2119_drawLine(x1, y0 + radius, x1, y1 - radius, color);	// right
}

void SSD2119_drawFilledRoundRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint8 radius, Uint16 color) {

	Sint16 tSwitch;
	Uint16 tmpX = 0, tmpY = radius;
	tSwitch = 3 - 2 * radius;
	Uint16 width = x1 - x0;
	Uint16 height = y1 - y0;

	// center block
	// filling center block first makes it apear to fill faster
	SSD2119_drawFilledRectangle(x0 + radius, y0, x1 - radius, y1, color);

	while (tmpX <= tmpY)
	{
		// left side
		SSD2119_drawLine(
			x0+radius - tmpX, y0+radius - tmpY,			// upper left corner upper half
			x0+radius - tmpX, y0+height-radius + tmpY,	// lower left corner lower half
			color);
		SSD2119_drawLine(
			x0+radius - tmpY, y0+radius - tmpX,			// upper left corner lower half
			x0+radius - tmpY, y0+height-radius + tmpX,	// lower left corner upper half
			color);

		// right side
		SSD2119_drawLine(
			x0+width-radius + tmpX, y0+radius - tmpY,			// upper right corner upper half
			x0+width-radius + tmpX, y0+height-radius + tmpY,	// lower right corner lower half
			color);
		SSD2119_drawLine(
			x0+width-radius + tmpY, y0+radius - tmpX,			// upper right corner lower half
			x0+width-radius + tmpY, y0+height-radius + tmpX,	// lower right corner upper half
			color);

		if (tSwitch < 0) {
			tSwitch += (4 * tmpX + 6);
		} else {
			tSwitch += (4 * (tmpX - tmpY) + 10);
			tmpY--;
		}
		tmpX++;
	}
}

/***************************************************************
 *                       Fonctions privées
 ***************************************************************/

/**
 * @brief Initialisation par défaut du FSMC.
 * @version 1.0
 */
static void SSD2119_defaultInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	SSD2119_displayOff();

	/* BANK 3 (of NOR/SRAM Bank 1~4) is disabled */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);

	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM3);

	/*-- GPIO Configuration ------------------------------------------------------*/
	/* SRAM Data lines configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_MCO);

	/* SRAM Address lines configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource0, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource1, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource2, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource3, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource4, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource5, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource12, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource13, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource14, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource15, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOG,GPIO_PinSource0, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource1, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource2, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource3, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource4, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource5, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13, GPIO_AF_MCO);

	/* NOE and NWE configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;

	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5, GPIO_AF_MCO);

	/* NE3 configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;

	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_MCO);

	/* NBL0, NBL1 configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1, GPIO_AF_MCO);
}

/**
  * @brief Configure la ligne de contrôle de l'écran LCD (FSMC Pins) dans le mode alternatif.
  * @version 1.0
  */
static void SSD2119_ctrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOB, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);

	/*-- GPIO Configuration ------------------------------------------------------*/
	/* SRAM Data lines,  NOE and NWE configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	/* SRAM Address lines configuration LCD-DC */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF_FSMC);

	/* NE3 configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);

	/* LCD RST configuration */
	GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);

	/* LCD pwm configuration */
	GPIO_InitStructure.GPIO_Pin = LCD_PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(LCD_PWM_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LCD_PWM_PORT, LCD_PWM_PIN);
}

/**
  * @brief Configuretion de l'interface parallèle (FSMC).
  * @version 1.0
  */
static void SSD2119_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;

	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/*-- FSMC Configuration ------------------------------------------------------*/
	/*----------------------- SRAM Bank 1 ----------------------------------------*/
	/* FSMC_Bank1_NORSRAM4 configuration */
	p.FSMC_AddressSetupTime = 1;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 9;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;
	/* Color LCD configuration ------------------------------------
	 LCD configured as follow:
		- Data/Address MUX = Disable
		- Memory Type = SRAM
		- Data Width = 16bit
		- Write Operation = Enable
		- Extended Mode = Enable
		- Asynchronous Wait = Disable */

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/* Enable FSMC NOR/SRAM Bank1 */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/**
 * @brief Fill a zone.
 * @param x0 x start zone.
 * @param y0 y start zone.
 * @param x1 x end zone.
 * @param y1 y end zone.
 * @param color Zone color.
 * @version 1.0
 */
static void SSD2119_INT_fill(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color) {

	Uint32 pixels_count;

	/* Set cursor position */
	SSD2119_setCursorPosition(x0, y0, x1, y1);

	/* Set command for GRAM data */
	SSD2119_writeRAMPrepare();

	/* Calculate pixels count */
	pixels_count = (x1 - x0 + 1) * (y1 - y0 + 1);

	Uint32 i;
	for(i=0;i<pixels_count;i++) {
		SSD2119_writeRAM(color);
	}
}

/**
 * @brief Make a delay.
 * @param delay Delay in ms.
 * @version 1.0
 */
static void SSD2119_delay(volatile Uint32 delay) {

	time32_t beginTime = global.absolute_time;

	while(global.absolute_time <= beginTime + delay);
}

/**
 * @brief LCD initialization
 * @version 1.0
 */
static void SSD2119_initLCD(void) {

	// Configure the LCD Control pins
	SSD2119_ctrlLinesConfig();

	// Configure the FSMC Parallel interface
	SSD2119_FSMCConfig();

	SSD2119_delay(10);

	// Reset LCD
	SSD2119_RST_RESET();
	SSD2119_delay(100);
	SSD2119_RST_SET();

	// Mettre l'écran en mode sleep normal
	SSD2119_writeReg(SSD2119_SLEEP_MODE_1_REG, 0x0001);

	// Set initial power parameters
	SSD2119_writeReg(SSD2119_PWR_CTRL_5_REG, 0x00B2);  // Default value : 0x002B
	SSD2119_writeReg(SSD2119_VCOM_OTP_1_REG, 0x0006);

	// Start the oscillator
	SSD2119_writeReg(SSD2119_OSC_START_REG, 0x0001);

	// Set pixel format and basic display orientation (scanning direction)
	SSD2119_writeReg(SSD2119_OUTPUT_CTRL_REG, 0x32EF);
	SSD2119_writeReg(SSD2119_LCD_DRIVE_AC_CTRL_REG, 0x0600);

	// Exit sleep mode
	SSD2119_writeReg(SSD2119_SLEEP_MODE_1_REG, 0x0000);
	SSD2119_delay(5);

	// Configure pixel color format and MCU interface parameters
	SSD2119_writeReg(SSD2119_ENTRY_MODE_REG, ENTRY_MODE_LANDSCAPE);

	// Set analog parameters
	SSD2119_writeReg(SSD2119_SLEEP_MODE_2_REG, 0x0999);
	SSD2119_writeReg(SSD2119_ANALOG_SET_REG, 0x3800);

	// Enable the display
	SSD2119_writeReg(SSD2119_DISPLAY_CTRL_REG, 0x0033);

	// Set VCIX2 voltage to 6.1V
	SSD2119_writeReg(SSD2119_PWR_CTRL_2_REG, 0x0005);

	// Configure gamma correction
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_1_REG, 0x0000);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_2_REG, 0x0303);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_3_REG, 0x0407);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_4_REG, 0x0301);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_5_REG, 0x0301);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_6_REG, 0x0403);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_7_REG, 0x0707);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_8_REG, 0x0400);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_9_REG, 0x0a00);
	SSD2119_writeReg(SSD2119_GAMMA_CTRL_10_REG, 0x1000);

	// Configure Vlcd63 and VCOMl
	SSD2119_writeReg(SSD2119_PWR_CTRL_3_REG, 0x000A);
	SSD2119_writeReg(SSD2119_PWR_CTRL_4_REG, 0x2E00);

	// Set the display size and ensure that the GRAM window is set to allow access to the full display buffer
	SSD2119_writeReg(SSD2119_V_RAM_POS_REG, (SSD2119_HEIGHT - 1) << 8);
	SSD2119_writeReg(SSD2119_H_RAM_START_REG, 0x0000);
	SSD2119_writeReg(SSD2119_H_RAM_END_REG, SSD2119_WIDTH - 1);

	SSD2119_writeReg(SSD2119_X_RAM_ADDR_REG, 0x00);
	SSD2119_writeReg(SSD2119_Y_RAM_ADDR_REG, 0x00);
}

/**
  * @brief Sets the cursor position.
  * @param x: specifies the X position.
  * @param y: specifies the Y position.
  * @version 1.0
  */
static void SSD2119_setCursor(Uint16 x, Uint16 y) {

	// Set the X address of the display cursor
	SSD2119_writeReg(SSD2119_X_RAM_ADDR_REG, x);

	// Set the Y address of the display cursor
	SSD2119_writeReg(SSD2119_Y_RAM_ADDR_REG, y);
}

/**
 * @brief Define a window to draw
 * @param x0 left top corner
 * @param y0 left top corner
 * @param x1 rigth bottom corner
 * @param y1 rigth bottom corner
 * @version 1.0
 */
static void SSD2119_setCursorPosition(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1) {

	Uint32 value = 0;

	SSD2119_writeReg(SSD2119_H_RAM_START_REG, x0);

	if (x1 >= SSD2119_Opts.width) {
		SSD2119_writeReg(SSD2119_H_RAM_END_REG, SSD2119_Opts.width - 1);
	} else {
		SSD2119_writeReg(SSD2119_H_RAM_END_REG, x1);
	}

	if (y1 >= SSD2119_Opts.height) {
		value = (SSD2119_Opts.height - 1) << 8;
	} else {
		value = y1 << 8;
	}
	//value |= x0;
	SSD2119_writeReg(SSD2119_V_RAM_POS_REG, value);
	SSD2119_setCursor(x0, y0);

}

/**
  * @brief Writes to the selected LCD register.
  * @param reg: address of the selected register.
  * @param value: value to write to the selected register.
  * @version 1.0
  */
static void SSD2119_writeReg(Uint8 reg, Uint16 value) {

	// Write 16-bit Index, then Write Reg
	LCD_CMD = reg;
	// Write 16-bit Reg
	LCD_Data = value;
}

/**
  * @brief Reads the selected LCD Register.
  * @param reg: address of the selected register.
  * @retval LCD Register Value.
  * @version 1.0
  */
static Uint16 SSD2119_readReg(Uint8 reg) {

	// Write 16-bit Index (then Read Reg)
	LCD_CMD = reg;
	// Read 16-bit Reg
	return (LCD_Data);
}

/**
  * @brief Prepare to write to the LCD RAM.
  * @version 1.0
  */
static void SSD2119_writeRAMPrepare(void) {

	LCD_CMD = SSD2119_RAM_DATA_REG;
}

/**
  * @brief Writes to the LCD RAM.
  * @param RGB_Code: the pixel color in RGB mode (5-6-5).
  * @version 1.0
  */
static void SSD2119_writeRAM(Uint16 RGB_Code) {

	// Write 16-bit GRAM Reg
	LCD_Data = RGB_Code;
}

/**
  * @brief Reads the LCD RAM.
  * @retval LCD RAM Value.
  * @version 1.0
  */
static Uint16 SSD2119_readRAM(void) {

	// Read 16-bit GRAM Reg
	return LCD_Data;
}
