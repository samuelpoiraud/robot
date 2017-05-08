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

	#ifdef USE_LCD_OVER_UART

		#define LCD_NB_MAX_OBJECT		30
		#define OBJECT_TEXT_MAX_SIZE	50

		typedef enum{
			LCD_ANIMATION_WAIT_CIRCLE
		}LCD_animationId_e;
		#define IS_LCD_ANIMATION_ID(x)	(										\
											((x) == LCD_ANIMATION_WAIT_CIRCLE)	\
										)

		typedef enum{
			LCD_IMAGE_BUTTON_CHECKED,
			LCD_IMAGE_BUTTON_OFF,
			LCD_IMAGE_BUTTON_ON,
			LCD_IMAGE_BUTTON_UNCHECKED,
			LCD_IMAGE_ARROW_RIGHT,
			LCD_IMAGE_LOGO
		}LCD_imageId_e;
		#define IS_LCD_IMAGE_ID(x)	(											\
											((x) == LCD_IMAGE_BUTTON_CHECKED)	\
										|| 	((x) == LCD_IMAGE_BUTTON_OFF)		\
										|| 	((x) == LCD_IMAGE_BUTTON_ON)		\
										|| 	((x) == LCD_IMAGE_BUTTON_UNCHECKED)	\
										|| 	((x) == LCD_IMAGE_ARROW_RIGHT)		\
										|| 	((x) == LCD_IMAGE_LOGO)				\
									)

		typedef enum{
			LCD_MENU_DEBUG,
			LCD_MENU_WAIT,
			LCD_MENU_CUSTOM,
			LCD_MENU_HOKUYO,
			LCD_MENU_POSITION
		}LCD_menuId_e;
		#define IS_LCD_MENU_ID(x)	(								\
										((x) == LCD_MENU_DEBUG)		\
									|| 	((x) == LCD_MENU_WAIT)		\
									|| 	((x) == LCD_MENU_CUSTOM)	\
									|| 	((x) == LCD_MENU_HOKUYO)	\
									|| 	((x) == LCD_MENU_POSITION)	\
									)

		typedef enum{
			LCD_OBJECT_HORIZONTAL_L_2_R,
			LCD_OBJECT_HORIZONTAL_R_2_L,
			LCD_OBJECT_VERTICAL_T_2_B,
			LCD_OBJECT_VERTICAL_B_2_T
		}LCD_objectOrientation_e;
		#define IS_LCD_OBJECT_ORIENTATION(x)	(											\
													((x) == LCD_OBJECT_HORIZONTAL_L_2_R)	\
												|| 	((x) == LCD_OBJECT_HORIZONTAL_R_2_L)	\
												|| 	((x) == LCD_OBJECT_VERTICAL_T_2_B)		\
												|| 	((x) == LCD_OBJECT_VERTICAL_B_2_T)		\
												)

		typedef enum{
			LCD_TEXT_FONTS_7x10,
			LCD_TEXT_FONTS_11x18,
			LCD_TEXT_FONTS_16x26
		}LCD_textFonts_e;
		#define IS_LCD_TEXT_FONTS(x)	(									\
											((x) == LCD_TEXT_FONTS_7x10)	\
										|| 	((x) == LCD_TEXT_FONTS_11x18)	\
										|| 	((x) == LCD_TEXT_FONTS_16x26)	\
										)

		typedef enum{
			LCD_COLOR_WHITE = 0xFFFF,
			LCD_COLOR_BLACK = 0x0000,
			LCD_COLOR_RED = 0xF800,
			LCD_COLOR_GREEN = 0x07E0,
			LCD_COLOR_BLUE = 0x001F,

			LCD_COLOR_TRANSPARENT = 0x80000000
		}LCD_color_e;
		#define IS_LCD_COLOR_REAL(x)		((Uint32)(x) <= (Uint32) 0xFFFF)

		typedef Uint8 LCD_objectId_t;
		#define LCD_OBJECT_ID_ERROR_FULL		255

		void LCD_OVER_UART_init(void);

		void LCD_OVER_UART_processMain(void);

		//////////////////////////////////////////////////////////////////
		//---------------------Fonction Création------------------------//
		//////////////////////////////////////////////////////////////////

		#ifndef LCD_OVER_UART__IHM_MODE
			LCD_objectId_t LCD_OVER_UART_addText(Sint16 x, Sint16 y, LCD_color_e colorText, LCD_color_e colorBackground, LCD_textFonts_e fonts, const char *text, ...)  __attribute__((format (printf, 6, 7)));

			LCD_objectId_t LCD_OVER_UART_addButtonImg(Sint16 x, Sint16 y, LCD_imageId_e imageId, LCD_imageId_e imageLockId, bool_e lockTouch, bool_e * touch);

			LCD_objectId_t LCD_OVER_UART_addButton(Sint16 x, Sint16 y, Uint16 width, Uint16 height, bool_e lockTouch, bool_e *touch, LCD_color_e colorText, LCD_color_e colorButton, LCD_color_e colorButtonTouch, LCD_color_e colorBorder, LCD_textFonts_e fonts, const char *text, ...)  __attribute__((format (printf, 12, 13)));

			LCD_objectId_t LCD_OVER_UART_addProgressBar(Sint16 x, Sint16 y, Uint16 width, Uint16 height, LCD_objectOrientation_e orientation, Uint8 *value);

			LCD_objectId_t LCD_OVER_UART_addSlider(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Sint32 minValue, Sint32 maxValue, LCD_objectOrientation_e orientation, Sint32 *value);

			LCD_objectId_t LCD_OVER_UART_addImage(Sint16 x, Sint16 y, LCD_imageId_e imageId);

			LCD_objectId_t LCD_OVER_UART_addAnimatedImage(Sint16 x, Sint16 y, LCD_animationId_e animationId);

			LCD_objectId_t LCD_OVER_UART_addRectangle(Sint16 x, Sint16 y, Uint16 width, Uint16 height, LCD_color_e colorBorder, LCD_color_e colorCenter);

			LCD_objectId_t LCD_OVER_UART_addCircle(Sint16 x, Sint16 y, Uint16 r, LCD_color_e colorBorder, LCD_color_e colorCenter);

			LCD_objectId_t LCD_OVER_UART_addLine(Sint16 x0, Sint16 y0, Sint16 x1, Sint16 y1, LCD_color_e color);
		#else
			void LCD_OVER_UART_ihmControl(bool_e ihmUnderControl);
		#endif


		//////////////////////////////////////////////////////////////////
		//---------------------Fonction Mutation------------------------//
		//////////////////////////////////////////////////////////////////

		#ifndef LCD_OVER_UART__IHM_MODE
			void LCD_OVER_UART_setBackground(LCD_color_e color);

			void LCD_OVER_UART_setText(LCD_objectId_t id, const char *text, ...)  __attribute__((format (printf, 2, 3)));

			void LCD_OVER_UART_setMenu(LCD_menuId_e menuId);
		#else
			void LCD_OVER_UART_ihmControl(bool_e ihmUnderControl);
		#endif


		//////////////////////////////////////////////////////////////////
		//------------------Fonction Destruction------------------------//
		//////////////////////////////////////////////////////////////////

		#ifndef LCD_OVER_UART__IHM_MODE
			void LCD_OVER_UART_deleteObject(LCD_objectId_t id);

			void LCD_OVER_UART_deleteAllObject();

			void LCD_OVER_UART_resetScreen();
		#else

		#endif

	#endif

#endif	/* ndef QS_LCD_OVER_UART_H */
