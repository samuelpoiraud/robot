/*
 *	Club Robot ESEO 2016 - 2017
 *	Harry Anne
 *
 *	Fichier : QS_lcd_over_uart.h
 *	Package : Qualité Soft
 *	Description : fonctions d'encapsulation des messages LCD
					sur uart.
 *	Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100111
 */

#ifndef QS_LCD_OVER_UART_H
	#define	QS_LCD_OVER_UART_H

	#include "QS_all.h"

	#define LCD_NB_MAX_OBJECT		30
	#define OBJECT_TEXT_MAX_SIZE	50

	typedef enum{
		LCD_ANIMATION_WAIT_CIRCLE
	}LCD_animationId_e;

	typedef enum{
		LCD_IMAGE_LOGO,
		LCD_IMAGE_ARROW_RIGHT,
		LCD_IMAGE_ARROW_LEFT
	}LCD_imageId_e;

	typedef enum{
		LCD_MENU_CUSTOM,
		LCD_MENU_HOKUYO,
		LCD_MENU_POSITION
	}LCD_menuId_e;

	typedef enum{
		LCD_OBJECT_HORIZONTAL_L_2_R,
		LCD_OBJECT_HORIZONTAL_R_2_L,
		LCD_OBJECT_VERTICAL_T_2_B,
		LCD_OBJECT_VERTICAL_B_2_T
	}LCD_objectOrientation_e;

	typedef Uint8 LCD_objectId_t;
	#define LCD_OBJECT_ID_ERROR_FULL		255

	#ifdef USE_LCD_OVER_UART

		void LCD_OVER_UART_init(void);

		void LCD_OVER_UART_setMenu(LCD_menuId_e menuId);

		//////////////////////////////////////////////////////////////////
		//---------------------Fonction Création------------------------//
		//////////////////////////////////////////////////////////////////

		LCD_objectId_t LCD_OVER_UART_addText(Sint16 x, Sint16 y, Uint16 colorText, Uint32 colorBackground, const char *text, ...)  __attribute__((format (printf, 5, 6)));

		LCD_objectId_t LCD_OVER_UART_addButtonImg(Sint16 x, Sint16 y, LCD_imageId_e imageId, bool_e lockTouch, bool_e * touch);

		LCD_objectId_t LCD_OVER_UART_addButton(Sint16 x, Sint16 y, Uint16 width, Uint16 height, char * text, bool_e lockTouch, bool_e *touch, Uint16 colorText, Uint16 colorButton, Uint16 colorButtonTouch, Uint32 colorBorder);

		LCD_objectId_t LCD_OVER_UART_addProgressBar(Sint16 x, Sint16 y, Uint16 width, Uint16 height, LCD_objectOrientation_e orientation, Uint8 *value);

		LCD_objectId_t LCD_OVER_UART_addSlider(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Sint32 minValue, Sint32 maxValue, LCD_objectOrientation_e orientation, Sint32 *value);

		LCD_objectId_t LCD_OVER_UART_addImage(Sint16 x, Sint16 y, LCD_imageId_e imageId);

		LCD_objectId_t LCD_OVER_UART_addAnimatedImage(Sint16 x, Sint16 y, LCD_animationId_e animationId);

		LCD_objectId_t LCD_OVER_UART_addRectangle(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Uint32 colorBorder, Uint32 colorCenter);

		LCD_objectId_t LCD_OVER_UART_addCircle(Sint16 x, Sint16 y, Uint16 r, Uint32 colorBorder, Uint32 colorCenter);

		LCD_objectId_t LCD_OVER_UART_addLine(Sint16 x0, Sint16 y0, Sint16 x1, Sint16 y1, Uint16 color);


		//////////////////////////////////////////////////////////////////
		//---------------------Fonction Mutation------------------------//
		//////////////////////////////////////////////////////////////////

		void LCD_OVER_UART_setBackground(Uint16 color);

		void LCD_OVER_UART_setText(LCD_objectId_t id, const char *text, ...)  __attribute__((format (printf, 2, 3)));



		//////////////////////////////////////////////////////////////////
		//------------------Fonction Destruction------------------------//
		//////////////////////////////////////////////////////////////////

		void LCD_OVER_UART_deleteObject(LCD_objectId_t id);

		void LCD_OVER_UART_resetScreen();

	#endif

#endif	/* ndef QS_LCD_OVER_UART_H */
