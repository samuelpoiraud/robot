/*
 *	Club Robot ESEO 2016 - 2017
 *
 *	Fichier : ili9341.h
 *	Package : IHM
 *	Description : Driver du ILI9341
 *	Auteur : reprise Arnaud Guilmet de la base de Tilen Majerle (http://stm32f4-discovery.com/2014/04/library-08-ili9341-lcd-on-stm32f429-discovery-board/)
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100929
 */

/** ----------------  Defines possibles  --------------------
 *	LCD_RESET_PORT				: GPIO d'écriture pour la broche de reset du chip
 *	LCD_CS						: GPIO d'écriture pour la broche de sélection du chip
 *	LCD_D_C_PORT				: GPIO d'écriture pour la broche du mode commande / donnée du chip
 *	LCD_DMA						: Activation du DMA pour le SPI
 */

#ifndef ILI9341_H
	#define ILI9341_H

	#include "../QS/QS_all.h"
	#include "fonts.h"

	/* LCD settings */
	#define ILI9341_WIDTH        240
	#define ILI9341_HEIGHT       320
	#define ILI9341_PIXEL        76800

	/* Colors */
	#define ILI9341_COLOR_WHITE			0xFFFF
	#define ILI9341_COLOR_BLACK			0x0000
	#define ILI9341_COLOR_RED			0xF800
	#define ILI9341_COLOR_GREEN			0x07E0
	#define ILI9341_COLOR_GREEN2		0xB723
	#define ILI9341_COLOR_BLUE			0x001F
	#define ILI9341_COLOR_BLUE2			0x051D
	#define ILI9341_COLOR_YELLOW		0xFFE0
	#define ILI9341_COLOR_ORANGE		0xFBE4
	#define ILI9341_COLOR_CYAN			0x07FF
	#define ILI9341_COLOR_MAGENTA		0xA254
	#define ILI9341_COLOR_GRAY			0x7BEF
	#define ILI9341_COLOR_BROWN			0xBBCA

	/* Transparent background, only for strings and chars */
	#define ILI9341_TRANSPARENT			0x80000000

	/**
	 * @brief  Possible orientations for LCD
	 */
	typedef enum{
		ILI9341_Orientation_Portrait_1,  /*!< Portrait orientation mode 1 */
		ILI9341_Orientation_Portrait_2,  /*!< Portrait orientation mode 2 */
		ILI9341_Orientation_Landscape_1, /*!< Landscape orientation mode 1 */
		ILI9341_Orientation_Landscape_2  /*!< Landscape orientation mode 2 */
	}ILI9341_Orientation_t;

	/**
	 * @brief  Screen options structure
	 */
	typedef struct{
		Uint16 width;						/*!< Largeur de l'écran */
		Uint16 height;						/*!< Haute de l'écran */
		ILI9341_Orientation_t orientation;	/*!< Orientation de l'écran */
	}ILI931_Options_t;

	/**
	 * @brief  Initializes ILI9341 LCD with LTDC peripheral
	 *         It also initializes external SDRAM
	 * @param  None
	 * @retval None
	 */
	void ILI9341_init(void);

	/**
	 * @brief  Reconfigure SPI speed
	 */
	void ILI9341_setConfig(void);

	/**
	 * @brief  Get option screen like actual orientation, width, height
	 * @retval screen option structure
	 */
	ILI931_Options_t ILI9341_getOptions(void);

	/**
	 * @brief  Draws single pixel to LCD
	 * @param  x: X position for pixel
	 * @param  y: Y position for pixel
	 * @param  color: Color of pixel
	 * @retval None
	 */
	void ILI9341_drawPixel(Uint16 x, Uint16 y, Uint16 color);

	/**
	 * @brief  Fills entire LCD with color
	 * @param  color: Color to be used in fill
	 * @retval None
	 */
	void ILI9341_fill(Uint16 color);

	/**
	 * @brief  Rotates LCD to specific orientation
	 * @param  orientation: LCD orientation. This parameter can be a value of @ref ILI9341_Orientation_t enumeration
	 * @retval None
	 */
	void ILI9341_rotate(ILI9341_Orientation_t orientation);

	/**
	 * @brief  Puts single character to LCD
	 * @param  x: X position of top left corner
	 * @param  y: Y position of top left corner
	 * @param  c: Character to be displayed
	 * @param  *font: Pointer to @ref FontDef_t used font
	 * @param  foreground: Color for char
	 * @param  background: Color for char background
	 * @retval None
	 */
	void ILI9341_putc(Uint16 x, Uint16 y, char c, FontDef_t* font, Uint16 foreground, Uint32 background);

	/**
	 * @brief  Puts string to LCD
	 * @param  x: X position of top left corner of first character in string
	 * @param  y: Y position of top left corner of first character in string
	 * @param  *str: Pointer to first character
	 * @param  *font: Pointer to @ref FontDef_t used font
	 * @param  foreground: Color for string
	 * @param  background: Color for string background
	 * @retval None
	 */
	void ILI9341_puts(Uint16 x, Uint16 y, char* str, FontDef_t *font, Uint16 foreground, Uint32 background);

	/**
	 * @brief  Puts formated string to LCD
	 * @param  x: X position of top left corner of first character in string
	 * @param  y: Y position of top left corner of first character in string
	 * @param  *font: Pointer to @ref FontDef_t used font
	 * @param  foreground: Color for string
	 * @param  background: Color for string background
	 * @param  format : Formated string like printf
	 * @retval None
	 */
	void ILI9341_printf(Uint16 x, Uint16 y, FontDef_t *font, Uint16 foreground, Uint32 background, const char *format, ...)  __attribute__((format (printf, 6, 7)));

	/**
	 * @brief  Gets width and height of box with text
	 * @param  *str: Pointer to first character
	 * @param  *font: Pointer to @ref FontDef_t used font
	 * @param  *width: Pointer to variable to store width
	 * @param  *height: Pointer to variable to store height
	 * @retval None
	 */
	void ILI9341_getStringSize(char* str, FontDef_t* font, Uint16* width, Uint16* height);

	/**
	 * @brief  Draws line to LCD
	 * @param  x0: X coordinate of starting point
	 * @param  y0: Y coordinate of starting point
	 * @param  x1: X coordinate of ending point
	 * @param  y1: Y coordinate of ending point
	 * @param  color: Line color
	 * @retval None
	 */
	void ILI9341_drawLine(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color);

	/**
	 * @brief  Draws rectangle on LCD
	 * @param  x0: X coordinate of top left point
	 * @param  y0: Y coordinate of top left point
	 * @param  x1: X coordinate of bottom right point
	 * @param  y1: Y coordinate of bottom right point
	 * @param  color: Rectangle color
	 * @retval None
	 */
	void ILI9341_drawRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color);

	/**
	 * @brief  Draws filled rectangle on LCD
	 * @param  x0: X coordinate of top left point
	 * @param  y0: Y coordinate of top left point
	 * @param  x1: X coordinate of bottom right point
	 * @param  y1: Y coordinate of bottom right point
	 * @param  color: Rectangle color
	 * @retval None
	 */
	void ILI9341_drawFilledRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint16 color);

	/**
	 * @brief  Draws circle on LCD
	 * @param  x0: X coordinate of center circle point
	 * @param  y0: Y coordinate of center circle point
	 * @param  r: Circle radius
	 * @param  color: Circle color
	 * @retval None
	 */
	void ILI9341_drawCircle(Uint16 x0, Uint16 y0, Uint16 r, Uint16 color);

	/**
	 * @brief  Draws filled circle on LCD
	 * @param  x0: X coordinate of center circle point
	 * @param  y0: Y coordinate of center circle point
	 * @param  r: Circle radius
	 * @param  color: Circle color
	 * @retval None
	 */
	void ILI9341_drawFilledCircle(Uint16 x0, Uint16 y0, Uint16 r, Uint16 color);

	/**
	 * @brief  Put Image on LCD
	 * @param  x0: X coordinate of starting point
	 * @param  y0: Y coordinate of starting point
	 * @param  width: width of image
	 * @param  height: height of image
	 * @param  img: Pointeur sur le tableau des pixels de l'image en BMP 565
	 * @param  size: Nombre d'élément dans le tableau
	 * @retval None
	 */
	void ILI9341_putImage(Uint16 x0, Uint16 y0, Uint16 width, Uint16 height, const Uint16 *img, Uint32 size);

	/**
	 * @brief  Put Image with transparence on LCD
	 * @param  x0: X coordinate of starting point
	 * @param  y0: Y coordinate of starting point
	 * @param  width: width of image
	 * @param  height: height of image
	 * @param  img: Pointeur sur le tableau des pixels de l'image en BMP 565
	 * @param  colorTransparence: color of transparence in BMP 565
	 * @param  size: Nombre d'élément dans le tableau
	 * @retval None
	 */
	void ILI9341_putImageWithTransparence(Uint16 x0, Uint16 y0, Uint16 width, Uint16 height, const Uint16 *img, Uint16 colorTransparence, Uint32 size);

	/**
	 * @brief   Enables display
	 * @note    After initialization, LCD is enabled and you don't need to call this function
	 * @param   None
	 * @retval  None
	 */
	void ILI9341_displayOn(void);

	/**
	 * @brief   Disables display
	 * @param   None
	 * @retval  None
	 */
	void ILI9341_displayOff(void);

#endif

