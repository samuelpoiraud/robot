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
	#include "QS_lcd_over_uart.h"


	#define LCD_OVER_UART_DELIMITER			0x08
	#define LCD_OVER_UART_HEADER_SIZE		2		// [byte]
	#define LCD_OVER_UART_MAX_SIZE			100		// [byte]
	#define LCD_OVER_UART_COM_TIMEOUT		200		// [ms]

	//////////////////////////////////////////////////////////////////
	//-----------------Type relatif au objet------------------------//
	//////////////////////////////////////////////////////////////////

	typedef enum{
		LCD_OBJECT_TYPE_TEXT,
		LCD_OBJECT_TYPE_BUTTON_IMG,
		LCD_OBJECT_TYPE_BUTTON_BASE,
		LCD_OBJECT_TYPE_PROGRESS_BAR,
		LCD_OBJECT_TYPE_SLIDER,
		LCD_OBJECT_TYPE_IMAGE,
		LCD_OBJECT_TYPE_ANIMATED_IMAGE,
		LCD_OBJECT_TYPE_MULTI_TEXT,
		LCD_OBJECT_TYPE_RECTANGLE,
		LCD_OBJECT_TYPE_CIRCLE,
		LCD_OBJECT_TYPE_LINE
	}LCD_objectType_e;

	typedef union{
		bool_e * touch;
	}LCD_objectLink_u;

	typedef struct{
		LCD_objectType_e type;
		LCD_objectLink_u link;
	}LCD_object_s;

	typedef struct{
		bool_e used;
		LCD_object_s object;
	}LCD_objectStorage_s;

	//////////////////////////////////////////////////////////////////
	//-----------------Type relatif au message----------------------//
	//////////////////////////////////////////////////////////////////

	typedef enum{
		LCD_MSG_TYPE_SET_MENU,
		LCD_MSG_TYPE_ADD_TEXT
	}LCD_msgType_e;

	typedef union{
		struct{
			LCD_msgType_e type;
			Uint8 size;
		};

		Uint8 rawData[LCD_OVER_UART_HEADER_SIZE];
	}LCD_msgHeader_u;

	typedef union{
		struct{
			LCD_menuId_e menuId;
		}setMenu;

		#define SIZE_LCD_ADD_TEXT_BASE 12
		struct{
			Uint8 textSize;
			LCD_objectId_t id;
			Uint16 x;
			Uint16 y;
			Uint16 colorText;
			Uint32 colorBackground;
			char text[OBJECT_TEXT_MAX_SIZE];
		}addText;

		Uint8 rawData[LCD_OVER_UART_MAX_SIZE];
	}LCD_msgBody_u;

	typedef struct{
		LCD_msgHeader_u header;
		LCD_msgBody_u body;
	}LCD_msg_s;

#endif	/* ndef QS_LCD_OVER_UART_TYPE_H */
