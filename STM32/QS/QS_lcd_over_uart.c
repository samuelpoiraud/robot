/*
 *	Club Robot ESEO 2016 - 2017
 *	Harry Anne
 *
 *	Fichier : QS_can_over_uart.c
 *	Package : Qualité Soft
 *	Description : fonctions d'encapsulation des messages LCD
					sur uart.
 *	Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_lcd_over_uart.h"
#include "QS_lcd_over_uart_type.h"
#include "QS_uart.h"
#include "stdarg.h"

#ifdef USE_LCD_OVER_UART

	#if LCD_OVER_UART__UART_ID == 1
		#define MOSFET_BOARD_sendByte		UART1_putc
	#elif LCD_OVER_UART__UART_ID == 2
		#define MOSFET_BOARD_sendByte		UART2_putc
	#elif LCD_OVER_UART__UART_ID == 3
		#define MOSFET_BOARD_sendByte		UART3_putc
	#elif LCD_OVER_UART__UART_ID == 4
		#define MOSFET_BOARD_sendByte		UART4_putc
	#elif LCD_OVER_UART__UART_ID == 5
		#define MOSFET_BOARD_sendByte		UART5_putc
	#elif LCD_OVER_UART__UART_ID == 6
		#define MOSFET_BOARD_sendByte		UART6_putc

	#else
		#ifdef LCD_OVER_UART__UART_ID
			#error "Uart de communication lcd via uart non implémenté"
		#else
			#error "LCD_OVER_UART__UART_ID non déclaré"
		#endif
	#endif

	static volatile LCD_menuId_e LCD_menuId = LCD_MENU_CUSTOM;
	static volatile LCD_objectStorage_s LCD_objects[LCD_NB_MAX_OBJECT];

	static void LCD_OVER_UART_receiveByte(Uint8 byte);
	static void LCD_OVER_UART_receiveMsg(LCD_msg_s * msg);
	static void LCD_OVER_UART_sendMsg(LCD_msg_s * msg);
	static LCD_objectId_t LCD_OVER_UART_addObject(LCD_object_s object);

	void LCD_OVER_UART_init(void){
		UART_init();
		UART_setListenner(LCD_OVER_UART__UART_ID, &LCD_OVER_UART_receiveByte);
	}

	void LCD_OVER_UART_setMenu(LCD_menuId_e menuId){
		LCD_menuId = menuId;
		LCD_OVER_UART_resetScreen();
	}

	LCD_objectId_t LCD_OVER_UART_addText(Sint16 x, Sint16 y, Uint16 colorText, Uint32 colorBackground, const char *text, ...){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;

		LCD_object_s object;
		object.type = LCD_OBJECT_TYPE_TEXT;

		LCD_msg_s msg;
		msg.header.type = LCD_MSG_TYPE_ADD_TEXT;
		msg.body.addText.x = x;
		msg.body.addText.y = y;
		msg.body.addText.colorText = colorText;
		msg.body.addText.colorBackground = colorBackground;

		va_list args_list;
		va_start(args_list, text);
		vsnprintf((char *)msg.body.addText.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
		va_end(args_list);

		msg.body.addText.textSize = strlen(msg.body.addText.text);

		msg.header.size = msg.body.addText.textSize + SIZE_LCD_ADD_TEXT_BASE;

		msg.body.addText.id = LCD_OVER_UART_addObject(object);

		if(msg.body.addText.id != LCD_OBJECT_ID_ERROR_FULL){
			LCD_OVER_UART_sendMsg(&msg);
		}
	}

	LCD_objectId_t LCD_OVER_UART_addButtonImg(Sint16 x, Sint16 y, LCD_imageId_e imageId, bool_e lockTouch, bool_e * touch){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addButton(Sint16 x, Sint16 y, Uint16 width, Uint16 height, char * text, bool_e lockTouch, bool_e *touch, Uint16 colorText, Uint16 colorButton, Uint16 colorButtonTouch, Uint32 colorBorder){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addProgressBar(Sint16 x, Sint16 y, Uint16 width, Uint16 height, LCD_objectOrientation_e orientation, Uint8 *value){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addSlider(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Sint32 minValue, Sint32 maxValue, LCD_objectOrientation_e orientation, Sint32 *value){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addImage(Sint16 x, Sint16 y, LCD_imageId_e imageId){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addAnimatedImage(Sint16 x, Sint16 y, LCD_animationId_e animationId){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addRectangle(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Uint32 colorBorder, Uint32 colorCenter){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addCircle(Sint16 x, Sint16 y, Uint16 r, Uint32 colorBorder, Uint32 colorCenter){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	LCD_objectId_t LCD_OVER_UART_addLine(Sint16 x0, Sint16 y0, Sint16 x1, Sint16 y1, Uint16 color){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return 0;
	}

	void LCD_OVER_UART_setBackground(Uint16 color){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return;
	}

	void LCD_OVER_UART_setText(LCD_objectId_t id, const char * text, ...){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return;
	}

	void LCD_OVER_UART_deleteObject(LCD_objectId_t id){
		assert(id < LCD_NB_MAX_OBJECT);
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return;

		LCD_objects[id].used = FALSE;
	}

	void LCD_OVER_UART_resetScreen(){
		if(LCD_menuId != LCD_MENU_CUSTOM)
			return;

		LCD_objectId_t id;
		for(id = 0; id < LCD_NB_MAX_OBJECT; id++){
			LCD_objects[id].used = FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////
	//---------------------Fonction Interne-------------------------//
	//////////////////////////////////////////////////////////////////

	static LCD_objectId_t LCD_OVER_UART_addObject(LCD_object_s object){
		LCD_objectId_t id;
		for(id = 0; id < LCD_NB_MAX_OBJECT; id++){
			if(LCD_objects[id].used == FALSE){
				LCD_objects[id].used = TRUE;
				LCD_objects[id].object = object;
				return id;
			}
		}

		return LCD_OBJECT_ID_ERROR_FULL;
	}

	static void LCD_OVER_UART_receiveByte(Uint8 byte){

	}

	static void LCD_OVER_UART_sendMsg(LCD_msg_s * msg){
		Uint8 i;

		for(i=0; i<LCD_OVER_UART_HEADER_SIZE; i++)
			MOSFET_BOARD_sendByte(msg->header.rawData[i]);

		for(i=0; i<msg->header.size; i++)
			MOSFET_BOARD_sendByte(msg->body.rawData[i]);
	}

#endif
