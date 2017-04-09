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
#include "QS_outputlog.h"
#include "stdarg.h"
#include "string.h"

#ifdef USE_LCD_OVER_UART

	#ifdef LCD_OVER_UART__IHM_MODE
		#include "../LCD/interface.h"
		#include "../LCD/middleware.h"
		#include "../LCD/image/image.h"
	#endif

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

	#define LCD_BUFFER_SIZE		5

	typedef struct{
		LCD_msg_s buffer[LCD_BUFFER_SIZE];
		Uint8 readIndex;
		Uint8 writeIndex;
	}LCD_FIFO_msg_t;

	typedef enum{
		LCD_PARSER_STATE_DELIMITER,
		LCD_PARSER_STATE_HEADER,
		LCD_PARSER_STATE_DATA
	}UART_OVER_LCD_parserState_e;

	#ifndef LCD_OVER_UART__IHM_MODE

	#else
		typedef struct{
			LCD_objectType_e objectType;
			LCD_objectId_t remoteId;
		}objectIdMatch_s;
	#endif

	#ifndef LCD_OVER_UART__IHM_MODE
		static volatile LCD_menuId_e LCD_menuId = LCD_MENU_CUSTOM;
		static volatile LCD_objectStorage_s LCD_objects[LCD_NB_MAX_OBJECT];
	#else
		objectIdMatch_s objectId[LCD_NB_MAX_OBJECT];
	#endif
	static volatile LCD_FIFO_msg_t LCD_FIFO_msg;

	#ifndef LCD_OVER_UART__IHM_MODE
		static LCD_objectId_t LCD_OVER_UART_newObject();
	#else
		static void UART_OVER_LCD_syncObject(objectId_t id, objectId_t remoteId, LCD_objectType_e objectType);
		static const imageInfo_s* UART_OVER_LCD_imageIdConvertor(LCD_imageId_e imageId);
		static const animatedImageInfo_s* UART_OVER_LCD_animationIdConvertor(LCD_animationId_e animationId);
		static objectId_t UART_OVER_LCD_getSyncObjectByRemote(objectId_t remoteId);
	#endif

	static void LCD_OVER_UART_receiveMsg(LCD_msg_s * msg);
	static void LCD_OVER_UART_receiveByte(Uint8 byte);
	static void LCD_OVER_UART_sendMsg(LCD_msg_s * msg);
	static void LCD_FIFO_init();
	static bool_e LCD_FIFO_isFull();
	static bool_e LCD_FIFO_isEmpty();
	static bool_e LCD_FIFO_insertMsg(LCD_msg_s * msg);
	static bool_e LCD_FIFO_getMsg(LCD_msg_s * msg);
	static void LCD_FIFO_incWriteIndex();
	static void LCD_FIFO_incReadIndex();

	void LCD_OVER_UART_init(void){
		UART_init();
		LCD_FIFO_init();
		UART_setListenner(LCD_OVER_UART__UART_ID, &LCD_OVER_UART_receiveByte);
	}

	void LCD_OVER_UART_processMain(void){
		LCD_msg_s msg;

		while(LCD_FIFO_getMsg(&msg)){
			LCD_OVER_UART_receiveMsg(&msg);
		}
	}

	#ifndef LCD_OVER_UART__IHM_MODE
		void LCD_OVER_UART_setMenu(LCD_menuId_e menuId){
			LCD_menuId = menuId;
			LCD_OVER_UART_deleteAllObject();
			//LCD_OVER_UART_resetScreen();

			LCD_msg_s msg;
			msg.header.type = LCD_MSG_TYPE_SET_MENU;
			msg.header.size = SIZE_LCD_SET_MENU;
			msg.body.setMenu.menuId = menuId;

			LCD_OVER_UART_sendMsg(&msg);
		}

		LCD_objectId_t LCD_OVER_UART_addText(Sint16 x, Sint16 y, LCD_color_e colorText, LCD_color_e colorBackground, LCD_textFonts_e fonts, const char *text, ...){
			assert(IS_LCD_COLOR_REAL(colorText));
			assert(IS_LCD_TEXT_FONTS(fonts));
			assert(text != NULL);

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_TEXT;

			msg.header.type = LCD_MSG_TYPE_ADD_TEXT;
			msg.body.addText.id = id;
			msg.body.addText.x = x;
			msg.body.addText.y = y;
			msg.body.addText.fonts = fonts;
			msg.body.addText.colorText = colorText;
			msg.body.addText.colorBackground = colorBackground;

			va_list args_list;
			va_start(args_list, text);
			vsnprintf((char *)msg.body.addText.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
			va_end(args_list);

			msg.body.addText.textSize = strlen(msg.body.addText.text) + 1;

			msg.header.size = msg.body.addText.textSize + SIZE_LCD_ADD_TEXT_BASE;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addButtonImg(Sint16 x, Sint16 y, LCD_imageId_e imageId, LCD_imageId_e imageLockId, bool_e lockTouch, bool_e * touch){
			assert(IS_LCD_IMAGE_ID(imageId));
			assert(touch != NULL);

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_BUTTON_IMG;

			msg.header.type = LCD_MSG_TYPE_ADD_BUTTON_IMG;
			msg.body.addButtonImg.id = id;
			msg.body.addButtonImg.imageId = imageId;
			msg.body.addButtonImg.imageLockId = imageLockId;
			msg.body.addButtonImg.lockTouch = lockTouch;
			msg.body.addButtonImg.x = x;
			msg.body.addButtonImg.y = y;

			msg.header.size = SIZE_LCD_ADD_BUTTON_IMG;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addButton(Sint16 x, Sint16 y, Uint16 width, Uint16 height, bool_e lockTouch, bool_e *touch, LCD_color_e colorText, LCD_color_e colorButton, LCD_color_e colorButtonTouch, LCD_color_e colorBorder, LCD_textFonts_e fonts, const char *text, ...){
			assert(touch != NULL);
			assert(IS_LCD_COLOR_REAL(colorText));
			assert(IS_LCD_COLOR_REAL(colorButton));
			assert(IS_LCD_TEXT_FONTS(fonts));
			assert(text != NULL);

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_BUTTON;

			msg.header.type = LCD_MSG_TYPE_ADD_BUTTON;
			msg.body.addButton.id = id;
			msg.body.addButton.colorBorder = colorBorder;
			msg.body.addButton.colorButton = colorButton;
			msg.body.addButton.colorButtonTouch = colorButtonTouch;
			msg.body.addButton.colorText = colorText;
			msg.body.addButton.height = height;
			msg.body.addButton.lockTouch = lockTouch;
			msg.body.addButton.width = width;
			msg.body.addButton.fonts = fonts;
			msg.body.addButton.x = x;
			msg.body.addButton.y = y;

			va_list args_list;
			va_start(args_list, text);
			vsnprintf((char *)msg.body.addButton.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
			va_end(args_list);

			msg.body.addButton.textSize = strlen(msg.body.addButton.text) + 1;

			msg.header.size = msg.body.addButton.textSize + SIZE_LCD_ADD_BUTTON_BASE;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addProgressBar(Sint16 x, Sint16 y, Uint16 width, Uint16 height, LCD_objectOrientation_e orientation, Uint8 *value){
			assert(IS_LCD_OBJECT_ORIENTATION(orientation));
			assert(value != NULL);

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_PROGRESS_BAR;

			msg.header.type = LCD_MSG_TYPE_ADD_PROGRESS_BAR;
			msg.body.addProgressBar.id = id;
			msg.body.addProgressBar.x = x;
			msg.body.addProgressBar.y = y;
			msg.body.addProgressBar.height = height;
			msg.body.addProgressBar.width = width;
			msg.body.addProgressBar.orientation = orientation;
			msg.body.addProgressBar.baseValue = *value;

			msg.header.size = SIZE_LCD_ADD_PROGRESS_BAR;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addSlider(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Sint32 minValue, Sint32 maxValue, LCD_objectOrientation_e orientation, Sint32 *value){
			assert(IS_LCD_OBJECT_ORIENTATION(orientation));
			assert(value != NULL);

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_SLIDER;

			msg.header.type = LCD_MSG_TYPE_ADD_SLIDER;
			msg.body.addSlider.id = id;
			msg.body.addSlider.x = x;
			msg.body.addSlider.y = y;
			msg.body.addSlider.height = height;
			msg.body.addSlider.width = width;
			msg.body.addSlider.orientation = orientation;
			msg.body.addSlider.minValue = minValue;
			msg.body.addSlider.maxValue = maxValue;
			msg.body.addSlider.baseValue = *value;

			msg.header.size = SIZE_LCD_ADD_SLIDER;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addImage(Sint16 x, Sint16 y, LCD_imageId_e imageId){
			assert(IS_LCD_IMAGE_ID(imageId));

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_IMAGE;

			msg.header.type = LCD_MSG_TYPE_ADD_IMAGE;
			msg.body.addImage.id = id;
			msg.body.addImage.x = x;
			msg.body.addImage.y = y;
			msg.body.addImage.imageId = imageId;

			msg.header.size = SIZE_LCD_ADD_IMAGE;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addAnimatedImage(Sint16 x, Sint16 y, LCD_animationId_e animationId){
			assert(IS_LCD_ANIMATION_ID(animationId));

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_ANIMATED_IMAGE;

			msg.header.type = LCD_MSG_TYPE_ADD_ANIMATION;
			msg.body.addAnimation.id = id;
			msg.body.addAnimation.x = x;
			msg.body.addAnimation.y = y;
			msg.body.addAnimation.animationId = animationId;

			msg.header.size = SIZE_LCD_ADD_ANIMATION;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addRectangle(Sint16 x, Sint16 y, Uint16 width, Uint16 height, LCD_color_e colorBorder, LCD_color_e colorCenter){
			assert(IS_LCD_COLOR_REAL(colorBorder) || IS_LCD_COLOR_REAL(colorCenter));

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_RECTANGLE;

			msg.header.type = LCD_MSG_TYPE_ADD_RECTANGLE;
			msg.body.addRectangle.id = id;
			msg.body.addRectangle.x = x;
			msg.body.addRectangle.y = y;
			msg.body.addRectangle.height = height;
			msg.body.addRectangle.width = width;
			msg.body.addRectangle.colorBorder = colorBorder;
			msg.body.addRectangle.colorCenter = colorCenter;

			msg.header.size = SIZE_LCD_ADD_RECTANGLE;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addCircle(Sint16 x, Sint16 y, Uint16 r, LCD_color_e colorBorder, LCD_color_e colorCenter){
			assert(IS_LCD_COLOR_REAL(colorBorder) || IS_LCD_COLOR_REAL(colorCenter));

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_CIRCLE;

			msg.header.type = LCD_MSG_TYPE_ADD_CIRCLE;
			msg.body.addCircle.id = id;
			msg.body.addCircle.x = x;
			msg.body.addCircle.y = y;
			msg.body.addCircle.r = r;
			msg.body.addCircle.colorBorder = colorBorder;
			msg.body.addCircle.colorCenter = colorCenter;

			msg.header.size = SIZE_LCD_ADD_CIRCLE;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		LCD_objectId_t LCD_OVER_UART_addLine(Sint16 x0, Sint16 y0, Sint16 x1, Sint16 y1, LCD_color_e color){
			assert(IS_LCD_COLOR_REAL(color));

			LCD_objectId_t id;
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return LCD_OBJECT_ID_ERROR_FULL;

			id = LCD_OVER_UART_newObject();

			if(id == LCD_OBJECT_ID_ERROR_FULL)
				return LCD_OBJECT_ID_ERROR_FULL;

			LCD_objects[id].object.type = LCD_OBJECT_TYPE_LINE;

			msg.header.type = LCD_MSG_TYPE_ADD_LINE;
			msg.body.addLine.id = id;
			msg.body.addLine.x0 = x0;
			msg.body.addLine.x1 = x1;
			msg.body.addLine.y0 = y0;
			msg.body.addLine.y1 = y1;
			msg.body.addLine.color = color;

			msg.header.size = SIZE_LCD_ADD_LINE;

			LCD_OVER_UART_sendMsg(&msg);

			return id;
		}

		void LCD_OVER_UART_setBackground(LCD_color_e color){
			assert(IS_LCD_COLOR_REAL(color));

			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return;

			msg.header.type = LCD_MSG_TYPE_SET_BACKGROUND;
			msg.body.setBackground.color = color;

			msg.header.size = SIZE_LCD_SET_BACKGROUND;

			LCD_OVER_UART_sendMsg(&msg);
		}

		void LCD_OVER_UART_setText(LCD_objectId_t id, const char * text, ...){
			assert(id < LCD_NB_MAX_OBJECT);
			assert(text != NULL);

			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return;

			if(LCD_objects[id].used == FALSE)
				return;

			msg.header.type = LCD_MSG_TYPE_SET_TEXT;

			msg.body.setText.id = id;

			va_list args_list;
			va_start(args_list, text);
			vsnprintf((char *)msg.body.setText.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
			va_end(args_list);

			msg.body.setText.textSize = strlen(msg.body.setText.text) + 1;

			msg.header.size = msg.body.setText.textSize + SIZE_LCD_SET_TEXT_BASE;

			LCD_OVER_UART_sendMsg(&msg);
		}

		void LCD_OVER_UART_deleteObject(LCD_objectId_t id){
			assert(id < LCD_NB_MAX_OBJECT);

			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return;

			if(LCD_objects[id].used == FALSE)
				return;

			LCD_objects[id].used = FALSE;

			msg.header.type = LCD_MSG_TYPE_DELETE_OBJECT;
			msg.body.deleteObject.id = id;

			msg.header.size = SIZE_LCD_DELETE_OBJECT;

			LCD_OVER_UART_sendMsg(&msg);
		}

		void LCD_OVER_UART_deleteAllObject(){
			if(LCD_menuId != LCD_MENU_CUSTOM)
				return;

			LCD_objectId_t id;
			for(id = 0; id < LCD_NB_MAX_OBJECT; id++){
				LCD_objects[id].used = FALSE;
			}
		}

		void LCD_OVER_UART_resetScreen(){
			LCD_msg_s msg;

			if(LCD_menuId != LCD_MENU_CUSTOM)
				return;

			msg.header.type = LCD_MSG_TYPE_RESET_SCREEN;
			msg.header.size = 0;

			LCD_OVER_UART_sendMsg(&msg);
		}
	#else


	#endif

	//////////////////////////////////////////////////////////////////
	//---------------------Fonction Interne-------------------------//
	//////////////////////////////////////////////////////////////////

	#ifndef LCD_OVER_UART__IHM_MODE
		static LCD_objectId_t LCD_OVER_UART_newObject(){
			LCD_objectId_t id;
			for(id = 0; id < LCD_NB_MAX_OBJECT; id++){
				if(LCD_objects[id].used == FALSE){
					LCD_objects[id].used = TRUE;
					return id;
				}
			}

			return LCD_OBJECT_ID_ERROR_FULL;
		}
	#else
		static void UART_OVER_LCD_syncObject(objectId_t id, objectId_t remoteId, LCD_objectType_e objectType){
			if(id != LCD_OBJECT_ID_ERROR_FULL && id < LCD_NB_MAX_OBJECT){
				objectId[id].remoteId = remoteId;
				objectId[id].objectType = objectType;
			}
		}

		static objectId_t UART_OVER_LCD_getSyncObjectByRemote(objectId_t remoteId){
			objectId_t i;

			for(i=0; i<LCD_NB_MAX_OBJECT; i++){
				if(objectId[i].remoteId == remoteId)
					return i;
			}

			return LCD_OBJECT_ID_ERROR_FULL;
		}

		static const imageInfo_s* UART_OVER_LCD_imageIdConvertor(LCD_imageId_e imageId){
			assert(IS_LCD_IMAGE_ID(imageId));

			switch(imageId){
				case LCD_IMAGE_LOGO :
					return &logoESEO;

				case LCD_IMAGE_BUTTON_CHECKED :
					return &buttonChecked;

				case LCD_IMAGE_BUTTON_OFF :
					return &buttonOff;

				case LCD_IMAGE_BUTTON_ON :
					return &buttonOn;

				case LCD_IMAGE_BUTTON_UNCHECKED :
					return &buttonUnchecked;

				case LCD_IMAGE_ARROW_RIGHT :
					return &flecheDroite;

				default:
					return &flecheDroite;
			}
		}

		static const animatedImageInfo_s* UART_OVER_LCD_animationIdConvertor(LCD_animationId_e animationId){
			assert(IS_LCD_ANIMATION_ID(animationId));

			switch(animationId){
				case LCD_ANIMATION_WAIT_CIRCLE:
					return &waiting;

				default:
					return &waiting;
			}
		}
	#endif

	static void LCD_OVER_UART_sendMsg(LCD_msg_s * msg){
		Uint8 i;

		MOSFET_BOARD_sendByte(LCD_OVER_UART_DELIMITER);

		for(i=0; i<LCD_OVER_UART_HEADER_SIZE; i++)
			MOSFET_BOARD_sendByte(msg->header.rawData[i]);

		for(i=0; i<msg->header.size; i++)
			MOSFET_BOARD_sendByte(msg->body.rawData[i]);
	}

	static void LCD_OVER_UART_receiveMsg(LCD_msg_s * msg){
		#ifndef LCD_OVER_UART__IHM_MODE

		#else
			objectId_t id;
			static Uint32 var;
		#endif
		switch(msg->header.type){

			#ifndef LCD_OVER_UART__IHM_MODE

			#else
				case LCD_MSG_TYPE_ADD_ANIMATION :
					id = MIDDLEWARE_addAnimatedImage(msg->body.addAnimation.x, msg->body.addAnimation.y, UART_OVER_LCD_animationIdConvertor(msg->body.addAnimation.animationId));
					UART_OVER_LCD_syncObject(id, msg->body.addAnimation.id, LCD_OBJECT_TYPE_ANIMATED_IMAGE);
					break;

				case LCD_MSG_TYPE_ADD_BUTTON :
					id = MIDDLEWARE_addButton(msg->body.addButton.x, msg->body.addButton.y, msg->body.addButton.width, msg->body.addButton.height, msg->body.addButton.lockTouch, &var, msg->body.addButton.colorText, msg->body.addButton.colorButton, msg->body.addButton.colorButtonTouch, msg->body.addButton.colorBorder, msg->body.addButton.fonts, msg->body.addButton.text);
					UART_OVER_LCD_syncObject(id, msg->body.addButton.id, LCD_OBJECT_TYPE_BUTTON);
					break;

				case LCD_MSG_TYPE_ADD_BUTTON_IMG :
					id = MIDDLEWARE_addButtonImg(msg->body.addButtonImg.x, msg->body.addButtonImg.y, UART_OVER_LCD_imageIdConvertor(msg->body.addButtonImg.imageId), UART_OVER_LCD_imageIdConvertor(msg->body.addButtonImg.imageLockId), msg->body.addButtonImg.lockTouch, &var);
					UART_OVER_LCD_syncObject(id, msg->body.addButtonImg.id, LCD_OBJECT_TYPE_BUTTON_IMG);
					break;

				case LCD_MSG_TYPE_ADD_CIRCLE :
					id = MIDDLEWARE_addCircle(msg->body.addCircle.x, msg->body.addCircle.y, msg->body.addCircle.r, msg->body.addCircle.colorBorder, msg->body.addCircle.colorCenter);
					UART_OVER_LCD_syncObject(id, msg->body.addCircle.id, LCD_OBJECT_TYPE_CIRCLE);
					break;

				case LCD_MSG_TYPE_ADD_IMAGE :
					id = MIDDLEWARE_addImage(msg->body.addImage.x, msg->body.addImage.y, UART_OVER_LCD_imageIdConvertor(msg->body.addImage.imageId));
					UART_OVER_LCD_syncObject(id, msg->body.addImage.id, LCD_OBJECT_TYPE_IMAGE);
					break;

				case LCD_MSG_TYPE_ADD_LINE :
					id = MIDDLEWARE_addLine(msg->body.addLine.x0, msg->body.addLine.y0, msg->body.addLine.x1, msg->body.addLine.y1, msg->body.addLine.color);
					UART_OVER_LCD_syncObject(id, msg->body.addLine.id, LCD_OBJECT_TYPE_LINE);
					break;

				case LCD_MSG_TYPE_ADD_PROGRESS_BAR :
					id = MIDDLEWARE_addProgressBar(msg->body.addProgressBar.x, msg->body.addProgressBar.y, msg->body.addProgressBar.width, msg->body.addProgressBar.height, msg->body.addProgressBar.orientation, &var);
					UART_OVER_LCD_syncObject(id, msg->body.addProgressBar.id, LCD_OBJECT_TYPE_PROGRESS_BAR);
					break;

				case LCD_MSG_TYPE_ADD_RECTANGLE :
					id = MIDDLEWARE_addRectangle(msg->body.addRectangle.x, msg->body.addRectangle.y, msg->body.addRectangle.width, msg->body.addRectangle.height, msg->body.addRectangle.colorBorder, msg->body.addRectangle.colorCenter);
					UART_OVER_LCD_syncObject(id, msg->body.addRectangle.id, LCD_OBJECT_TYPE_RECTANGLE);
					break;

				case LCD_MSG_TYPE_ADD_SLIDER :
					id = MIDDLEWARE_addSlider(msg->body.addSlider.x, msg->body.addSlider.y, msg->body.addSlider.width, msg->body.addSlider.height, msg->body.addSlider.minValue, msg->body.addSlider.maxValue, msg->body.addSlider.orientation, &var);
					UART_OVER_LCD_syncObject(id, msg->body.addSlider.id, LCD_OBJECT_TYPE_SLIDER);
					break;

				case LCD_MSG_TYPE_ADD_TEXT :
					id = MIDDLEWARE_addText(msg->body.addText.x, msg->body.addText.y, msg->body.addText.colorText, msg->body.addText.colorBackground, msg->body.addText.fonts, msg->body.addText.text);
					UART_OVER_LCD_syncObject(id, msg->body.addText.id, LCD_OBJECT_TYPE_TEXT);
					break;

				case LCD_MSG_TYPE_DELETE_OBJECT :
					id = UART_OVER_LCD_getSyncObjectByRemote(msg->body.deleteObject.id);
					if(id != LCD_OBJECT_ID_ERROR_FULL)
						MIDDLEWARE_deleteObject(id);
					break;

				case LCD_MSG_TYPE_RESET_SCREEN :
					MIDDLEWARE_resetScreen();
					break;

				case LCD_MSG_TYPE_SET_BACKGROUND :
					MIDDLEWARE_setBackground(msg->body.setBackground.color);
					break;

				case LCD_MSG_TYPE_SET_TEXT :
					id = UART_OVER_LCD_getSyncObjectByRemote(msg->body.setText.id);
					if(id != LCD_OBJECT_ID_ERROR_FULL)
						MIDDLEWARE_setText(id, msg->body.setText.text);
					break;

				case LCD_MSG_TYPE_SET_MENU :
					INTERFACE_setInterface(msg->body.setMenu.menuId);
					break;
			#endif

			default:
				break;
		}
	}

	//////////////////////////////////////////////////////////////////
	//------------------------Fonction FIFO-------------------------//
	//////////////////////////////////////////////////////////////////

	static void LCD_OVER_UART_receiveByte(Uint8 byte){
		static UART_OVER_LCD_parserState_e state = LCD_PARSER_STATE_DELIMITER;
		static UART_OVER_LCD_parserState_e last_state = LCD_PARSER_STATE_DELIMITER;

		static LCD_msg_s msg;

		static Uint16 byteToRead;
		static Uint16 byteCounter;

		static time32_t lastTimeByte;

		if(global.absolute_time - lastTimeByte > LCD_OVER_UART_COM_TIMEOUT){
			state = LCD_PARSER_STATE_DELIMITER;
			last_state = LCD_PARSER_STATE_DELIMITER;

			lastTimeByte = global.absolute_time;
		}

		bool_e entrance = last_state != state;
		last_state = state;

		switch (state)
		{
			case LCD_PARSER_STATE_DELIMITER:

				if(byte != LCD_OVER_UART_DELIMITER)
					error_printf("Invalid start 0x%02x != 0x%02x\n", byte, LCD_OVER_UART_DELIMITER);
				else{
					state = LCD_PARSER_STATE_HEADER;
				}
				break;

			case LCD_PARSER_STATE_HEADER:

				if(entrance){
					byteToRead = LCD_OVER_UART_HEADER_SIZE;
					byteCounter = 0;
				}

				if(byteCounter < LCD_OVER_UART_HEADER_SIZE)
					msg.header.rawData[byteCounter++] = byte;

				byteToRead--;

				if(byteToRead == 0){
					if(msg.header.size == 0){

						if(LCD_FIFO_insertMsg(&msg) == FALSE){
							error_printf("Fifo full\n");
						}
						state = LCD_PARSER_STATE_DELIMITER;

					}else if(msg.header.size <= LCD_OVER_UART_MAX_SIZE)
						state = LCD_PARSER_STATE_DATA;
					else
						state = LCD_PARSER_STATE_DELIMITER;
				}
				break;

			case LCD_PARSER_STATE_DATA:

				if(entrance){
					byteToRead = msg.header.size;
					byteCounter = 0;
				}

				if(byteCounter < LCD_OVER_UART_MAX_SIZE)
					msg.body.rawData[byteCounter++] = byte;
				else
					state = LCD_PARSER_STATE_DELIMITER;

				byteToRead--;

				if(byteToRead == 0){

					if(LCD_FIFO_insertMsg(&msg) == FALSE){
						error_printf("Fifo full\n");
					}

					state = LCD_PARSER_STATE_DELIMITER;
				}
				break;

			default:
				error_printf("ERROR : LCD Low Level parser default case !\n");
				state = LCD_PARSER_STATE_DELIMITER;
				break;
		}
	}

	static void LCD_FIFO_init(){
		LCD_FIFO_msg.readIndex = 0;
		LCD_FIFO_msg.writeIndex = 0;
	}

	static bool_e LCD_FIFO_isFull(){
		Uint8 writeIndex = LCD_FIFO_msg.writeIndex + 1;

		if(writeIndex >= LCD_BUFFER_SIZE)
			writeIndex = 0;

		return (writeIndex == LCD_FIFO_msg.readIndex);
	}

	static bool_e LCD_FIFO_isEmpty(){
		return (LCD_FIFO_msg.readIndex == LCD_FIFO_msg.writeIndex);
	}

	static bool_e LCD_FIFO_insertMsg(LCD_msg_s * msg){
		Uint16 i;

		if(LCD_FIFO_isFull())
			return FALSE;

		for(i=0; i < LCD_OVER_UART_HEADER_SIZE; i++)
			LCD_FIFO_msg.buffer[LCD_FIFO_msg.writeIndex].header.rawData[i] = msg->header.rawData[i];

		for(i=0; i < msg->header.size; i++)
			LCD_FIFO_msg.buffer[LCD_FIFO_msg.writeIndex].body.rawData[i] = msg->body.rawData[i];

		assert(!LCD_FIFO_isFull());

		LCD_FIFO_incWriteIndex();

		return TRUE;
	}

	static bool_e LCD_FIFO_getMsg(LCD_msg_s * msg){
		if(LCD_FIFO_isEmpty())
			return FALSE;

		Uint8 i;

		for(i=0; i < LCD_OVER_UART_HEADER_SIZE; i++)
			msg->header.rawData[i] = LCD_FIFO_msg.buffer[LCD_FIFO_msg.readIndex].header.rawData[i];

		for(i=0; i < msg->header.size; i++)
			msg->body.rawData[i] = LCD_FIFO_msg.buffer[LCD_FIFO_msg.readIndex].body.rawData[i];

		LCD_FIFO_incReadIndex();

		return TRUE;
	}

	static void LCD_FIFO_incWriteIndex(){
		Uint8 writeIndex = LCD_FIFO_msg.writeIndex + 1;

		if(writeIndex >= LCD_BUFFER_SIZE)
			LCD_FIFO_msg.writeIndex = 0;
		else
			LCD_FIFO_msg.writeIndex = writeIndex;
	}

	static void LCD_FIFO_incReadIndex(){
		__disable_irq();
		Uint8 readIndex = LCD_FIFO_msg.readIndex + 1;

		if(readIndex >= LCD_BUFFER_SIZE)
			LCD_FIFO_msg.readIndex = 0;
		else
			LCD_FIFO_msg.readIndex = readIndex;
		__enable_irq();
	}
#endif
