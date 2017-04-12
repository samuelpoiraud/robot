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

#ifndef QS_LCD_OVER_UART_TYPE_H
	#define	QS_LCD_OVER_UART_TYPE_H

	#include "QS_all.h"

	#ifdef USE_LCD_OVER_UART

		#include "QS_lcd_over_uart.h"


		#define LCD_OVER_UART_DELIMITER			0x08
		#define LCD_OVER_UART_HEADER_SIZE		2		// [byte]
		#define LCD_OVER_UART_MAX_SIZE			100		// [byte]
		#define LCD_OVER_UART_COM_TIMEOUT		200		// [ms]

		//////////////////////////////////////////////////////////////////
		//-----------------Type relatif au message----------------------//
		//////////////////////////////////////////////////////////////////

		typedef enum{
			LCD_MSG_TYPE_SET_MENU,
			LCD_MSG_TYPE_SET_BACKGROUND,
			LCD_MSG_TYPE_DELETE_OBJECT,
			LCD_MSG_TYPE_RESET_SCREEN,
			LCD_MSG_TYPE_SET_TEXT,
			LCD_MSG_TYPE_ADD_TEXT,
			LCD_MSG_TYPE_ADD_BUTTON_IMG,
			LCD_MSG_TYPE_ADD_BUTTON,
			LCD_MSG_TYPE_ADD_PROGRESS_BAR,
			LCD_MSG_TYPE_ADD_SLIDER,
			LCD_MSG_TYPE_ADD_IMAGE,
			LCD_MSG_TYPE_ADD_ANIMATION,
			LCD_MSG_TYPE_ADD_RECTANGLE,
			LCD_MSG_TYPE_ADD_CIRCLE,
			LCD_MSG_TYPE_ADD_LINE,
			LCD_MSG_TYPE_UPDATE_PROGRESS_BAR,
			LCD_MSG_TYPE_UPDATE_BUTTON,
			LCD_MSG_TYPE_UPDATE_BUTTON_IMG,
			LCD_MSG_TYPE_UPDATE_SLIDER
		}LCD_msgType_e;

		typedef union{
			struct{
				LCD_msgType_e type;
				Uint8 size;
			};

			Uint8 rawData[LCD_OVER_UART_HEADER_SIZE];
		}LCD_msgHeader_u;

		typedef union{

			#define SIZE_LCD_SET_MENU 			1
			struct{
				LCD_menuId_e menuId				:8;
			}setMenu;

			#define SIZE_LCD_SET_BACKGROUND		4
			struct{
				LCD_color_e color				:32;
			}setBackground;

			#define SIZE_LCD_DELETE_OBJECT		1
			struct{
				LCD_objectId_t id				:8;
			}deleteObject;

			#define SIZE_LCD_SET_TEXT_BASE		2
			struct{
				LCD_objectId_t id				:8;
				Uint8 textSize					:8;
				char text[OBJECT_TEXT_MAX_SIZE];
			}setText;

			#define SIZE_LCD_ADD_TEXT_BASE 		15
			struct{
				LCD_color_e colorBackground		:32;
				LCD_color_e colorText			:32;
				Sint16 x						:16;
				Sint16 y						:16;
				LCD_objectId_t id				:8;
				LCD_textFonts_e fonts			:8;
				Uint8 textSize					:8;
				char text[OBJECT_TEXT_MAX_SIZE];
			}addText;

			#define SIZE_LCD_ADD_BUTTON_IMG		8
			struct{
				Sint16 x						:16;
				Sint16 y						:16;
				LCD_imageId_e imageId			:8;
				LCD_imageId_e imageLockId		:8;
				bool_e lockTouch				:8;
				LCD_objectId_t id				:8;
			}addButtonImg;

			#define SIZE_LCD_ADD_BUTTON_BASE	28
			struct{
				LCD_color_e colorText			:32;
				LCD_color_e colorButton			:32;
				LCD_color_e colorButtonTouch	:32;
				LCD_color_e colorBorder			:32;
				Sint16 x						:16;
				Sint16 y						:16;
				Uint16 width					:16;
				Uint16 height					:16;
				bool_e lockTouch				:8;
				LCD_objectId_t id				:8;
				LCD_textFonts_e fonts			:8;
				Uint8 textSize					:8;
				char text[OBJECT_TEXT_MAX_SIZE];
			}addButton;

			#define SIZE_LCD_ADD_PROGRESS_BAR	11
			struct{
				Sint16 x						:16;
				Sint16 y						:16;
				Uint16 width					:16;
				Uint16 height					:16;
				LCD_objectOrientation_e orientation	:8;
				Uint8 baseValue					:8;
				LCD_objectId_t id				:8;
			}addProgressBar;

			#define SIZE_LCD_ADD_SLIDER			22
			struct{
				Sint32 minValue					:32;
				Sint32 maxValue					:32;
				Sint32 baseValue				:32;
				Sint16 x						:16;
				Sint16 y						:16;
				Uint16 width					:16;
				Uint16 height					:16;
				LCD_objectOrientation_e orientation	:8;
				LCD_objectId_t id				:8;
			}addSlider;

			#define SIZE_LCD_ADD_IMAGE 			6
			struct{
				Sint16 x						:16;
				Sint16 y						:16;
				LCD_imageId_e imageId			:8;
				LCD_objectId_t id				:8;
			}addImage;

			#define SIZE_LCD_ADD_ANIMATION		6
			struct{
				Sint16 x						:16;
				Sint16 y						:16;
				LCD_animationId_e animationId	:8;
				LCD_objectId_t id				:8;
			}addAnimation;

			#define SIZE_LCD_ADD_RECTANGLE		19
			struct{
				LCD_color_e colorBorder			:32;
				LCD_color_e colorCenter			:32;
				Sint16 x						:16;
				Sint16 y						:16;
				Uint16 width					:16;
				Uint16 height					:16;
				LCD_objectId_t id				:8;
			}addRectangle;

			#define SIZE_LCD_ADD_CIRCLE 		15
			struct{
				LCD_color_e colorBorder			:32;
				LCD_color_e colorCenter			:32;
				Sint16 x						:16;
				Sint16 y						:16;
				Uint16 r						:16;
				LCD_objectId_t id				:8;
			}addCircle;

			#define SIZE_LCD_ADD_LINE 			13
			struct{
				LCD_color_e color				:32;
				Sint16 x0						:16;
				Sint16 y0						:16;
				Sint16 x1						:16;
				Sint16 y1						:16;
				LCD_objectId_t id				:8;
			}addLine;

			#define SIZE_LCD_UPDATE_PROGRESS_BAR 2
			struct{
				LCD_objectId_t id				:8;
				Uint8 value						:8;
			}updateProgressBar;

			#define SIZE_LCD_UPDATE_BUTTON 		2
			struct{
				LCD_objectId_t id				:8;
				bool_e touchState				:8;
			}updateButton;

			#define SIZE_LCD_UPDATE_BUTTON_IMG 	2
			struct{
				LCD_objectId_t id				:8;
				bool_e touchState				:8;
			}updateButtonImg;

			#define SIZE_LCD_UPDATE_SLIDER 		5
			struct{
				LCD_objectId_t id				:8;
				Sint32 value					:32;
			}updateSlider;

			Uint8 rawData[LCD_OVER_UART_MAX_SIZE];
		}LCD_msgBody_u;

		typedef struct{
			LCD_msgHeader_u header;
			LCD_msgBody_u body;
		}LCD_msg_s;

	#endif

#endif	/* ndef QS_LCD_OVER_UART_TYPE_H */
