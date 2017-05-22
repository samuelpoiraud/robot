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
#include "string.h"

#ifdef USE_LCD_OVER_UART

#define LOG_PREFIX "LCD : "
#include "QS_outputlog.h"

	#ifdef LCD_OVER_UART__IHM_MODE
		#include "../LCD/interface.h"
		#include "../LCD/middleware.h"
		#include "../LCD/image/image.h"
	#else
		#include "../Supervision/LCD_interface.h"
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

	#define LCD_BUFFER_SIZE		10

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

	typedef enum{
		LCD_OBJECT_TYPE_TEXT,
		LCD_OBJECT_TYPE_BUTTON_IMG,
		LCD_OBJECT_TYPE_BUTTON,
		LCD_OBJECT_TYPE_PROGRESS_BAR,
		LCD_OBJECT_TYPE_SLIDER,
		LCD_OBJECT_TYPE_IMAGE,
		LCD_OBJECT_TYPE_ANIMATED_IMAGE,
		LCD_OBJECT_TYPE_RECTANGLE,
		LCD_OBJECT_TYPE_CIRCLE,
		LCD_OBJECT_TYPE_LINE
	}LCD_objectType_e;

	typedef union{
		struct{
			bool_e lastTouchState;
			bool_e * touchState;
		}buttonImg;

		struct{
			bool_e lastTouchState;
			bool_e * touchStatePtr;
			bool_e touchState;
		}button;

		struct{
			Uint8 lastValue;
			Uint8 * valuePtr;
			Uint8  value;
		}progressBar;

		struct{
			Sint32 lastValue;
			Sint32 * value;
		}slider;
	}LCD_objectLink_u;

	#ifndef LCD_OVER_UART__IHM_MODE

		typedef struct{
			LCD_objectType_e type;
			LCD_objectLink_u link;
		}LCD_object_s;

		typedef struct{
			bool_e used;
			LCD_object_s object;
		}LCD_objectStorage_s;
	#else
		typedef struct{
			struct{
				LCD_objectType_e objectType;
				LCD_objectLink_u link;
				LCD_objectId_t remoteId;
				LCD_objectId_t localId;
			}data;
			bool_e used;
		}objectIdMatch_s;
	#endif

	#ifndef LCD_OVER_UART__IHM_MODE
		static volatile LCD_menuId_e LCD_menuId = LCD_MENU_CUSTOM;
		static volatile LCD_objectStorage_s LCD_objects[LCD_NB_MAX_OBJECT];
	#else
		static volatile objectIdMatch_s objectId[LCD_NB_MAX_OBJECT];
	#endif
	static volatile LCD_FIFO_msg_t LCD_FIFO_msg;

	#ifndef LCD_OVER_UART__IHM_MODE
		static LCD_objectId_t LCD_OVER_UART_newObject(void);
	#else
		static objectId_t UART_OVER_LCD_newObject(void);
		static void UART_OVER_LCD_deleteObject(objectId_t id);
		static void UART_OVER_LCD_deleteAllObject();
		static objectId_t UART_OVER_LCD_getIdLocalByRemoteId(objectId_t remoteId);
		static objectId_t UART_OVER_LCD_getIdStorageByRemoteId(objectId_t remoteId);

		static void UART_OVER_LCD_setSyncObject(objectId_t idStorage, objectId_t localId, objectId_t remoteId, LCD_objectType_e type);
		static const imageInfo_s* UART_OVER_LCD_imageIdConvertor(LCD_imageId_e imageId);
		static const animatedImageInfo_s* UART_OVER_LCD_animationIdConvertor(LCD_animationId_e animationId);
	#endif

	static void LCD_OVER_UART_receiveMsg(LCD_msg_s * msg);
	static void LCD_OVER_UART_receiveByte(Uint8 byte);
	static void LCD_OVER_UART_sendMsg(LCD_msg_s * msg);
	static void LCD_OVER_UART_refreshObject(void);
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
		//UART_setListenner(LCD_OVER_UART__UART_ID, &LCD_OVER_UART_receiveByte);
	}

	void LCD_OVER_UART_processMain(void){
		LCD_msg_s msg;

		while(UART3_data_ready()){
			LCD_OVER_UART_receiveByte(UART3_get_next_msg());
		}

		while(LCD_FIFO_getMsg(&msg)){
			LCD_OVER_UART_receiveMsg(&msg);
		}

		LCD_OVER_UART_refreshObject();
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
			LCD_objects[id].object.link.buttonImg.touchState = touch;

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
			LCD_objects[id].object.link.button.touchStatePtr = touch;

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
			LCD_objects[id].object.link.progressBar.lastValue = *value;
			LCD_objects[id].object.link.progressBar.valuePtr = value;

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
			LCD_objects[id].object.link.slider.value = value;

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

			LCD_OVER_UART_deleteAllObject();
		}
	#else
		void LCD_OVER_UART_ihmControl(bool_e ihmUnderControl){
			LCD_msg_s msg;

			msg.header.type = LCD_MSG_TYPE_IHM_CONTROL;
			msg.header.size = SIZE_LCD_IHM_CONTROL;

			msg.body.ihmControl.ihmUnderControl = ihmUnderControl;

			LCD_OVER_UART_sendMsg(&msg);
		}

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
		static objectId_t UART_OVER_LCD_newObject(void){
			objectId_t i;
			for(i=0; i<LCD_NB_MAX_OBJECT; i++){
				if(objectId[i].used == FALSE){
					objectId[i].used = TRUE;
					return i;
				}
			}
			return LCD_OBJECT_ID_ERROR_FULL;
		}

		static void UART_OVER_LCD_deleteObject(objectId_t id){
			assert(id < LCD_NB_MAX_OBJECT);

			objectId[id].used = FALSE;
		}

		static void UART_OVER_LCD_deleteAllObject(){
			objectId_t i;
			for(i=0; i<LCD_NB_MAX_OBJECT; i++){
				UART_OVER_LCD_deleteObject(i);
			}
		}

		static objectId_t UART_OVER_LCD_getIdLocalByRemoteId(objectId_t remoteId){
			objectId_t i;

			for(i=0; i<LCD_NB_MAX_OBJECT; i++){
				if(objectId[i].used && objectId[i].data.remoteId == remoteId)
					return objectId[i].data.localId;
			}

			return LCD_OBJECT_ID_ERROR_FULL;
		}

		static objectId_t UART_OVER_LCD_getIdStorageByRemoteId(objectId_t remoteId){
			objectId_t i;

			for(i=0; i<LCD_NB_MAX_OBJECT; i++){
				if(objectId[i].used && objectId[i].data.remoteId == remoteId)
					return i;
			}

			return LCD_OBJECT_ID_ERROR_FULL;
		}

		static void UART_OVER_LCD_setSyncObject(objectId_t idStorage, objectId_t localId, objectId_t remoteId, LCD_objectType_e type){
			objectId[idStorage].data.localId = localId;
			objectId[idStorage].data.remoteId = remoteId;
			objectId[idStorage].data.objectType = type;
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
			objectId_t idStorage;
			objectId_t idObject;

			if(INTERFACE_getInterface() != INTERFACE_IHM_CUSTOM && INTERFACE_getInterface() != INTERFACE_IHM_WAIT)
				return;

		#endif
		switch(msg->header.type){

			#ifndef LCD_OVER_UART__IHM_MODE
				case LCD_MSG_TYPE_UPDATE_BUTTON:{
					LCD_objectId_t id = msg->body.updateButton.id;
					assert(id < LCD_NB_MAX_OBJECT);
					if(LCD_objects[id].used && LCD_objects[id].object.type == LCD_OBJECT_TYPE_BUTTON){
						*(LCD_objects[id].object.link.button.touchStatePtr) = msg->body.updateButton.touchState;
					}
					}break;

				case LCD_MSG_TYPE_UPDATE_BUTTON_IMG:{
					LCD_objectId_t id = msg->body.updateButtonImg.id;
					assert(id < LCD_NB_MAX_OBJECT);
					if(LCD_objects[id].used && LCD_objects[id].object.type == LCD_OBJECT_TYPE_BUTTON_IMG){
						*(LCD_objects[id].object.link.buttonImg.touchState) = msg->body.updateButtonImg.touchState;
					}
					}break;

				case LCD_MSG_TYPE_UPDATE_SLIDER:{
					LCD_objectId_t id = msg->body.updateSlider.id;
					assert(id < LCD_NB_MAX_OBJECT);
					if(LCD_objects[id].used && LCD_objects[id].object.type == LCD_OBJECT_TYPE_BUTTON_IMG){
						*(LCD_objects[id].object.link.slider.value) = msg->body.updateSlider.value;
					}
					}break;

				case LCD_MSG_TYPE_IHM_CONTROL :
					LCD_IHM_control(msg->body.ihmControl.ihmUnderControl);
					break;
			#else
				case LCD_MSG_TYPE_UPDATE_PROGRESS_BAR:
					idStorage = UART_OVER_LCD_getIdStorageByRemoteId(msg->body.updateProgressBar.id);
					objectId[idStorage].data.link.progressBar.value = msg->body.updateProgressBar.value;
					break;

				case LCD_MSG_TYPE_ADD_ANIMATION :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addAnimatedImage(msg->body.addAnimation.x, msg->body.addAnimation.y, UART_OVER_LCD_animationIdConvertor(msg->body.addAnimation.animationId));
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addAnimation.id, LCD_OBJECT_TYPE_ANIMATED_IMAGE);
					break;

				case LCD_MSG_TYPE_ADD_BUTTON :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addButton(msg->body.addButton.x, msg->body.addButton.y, msg->body.addButton.width, msg->body.addButton.height, msg->body.addButton.lockTouch, (bool_e *)&(objectId[idStorage].data.link.button.touchState), msg->body.addButton.colorText, msg->body.addButton.colorButton, msg->body.addButton.colorButtonTouch, msg->body.addButton.colorBorder, msg->body.addButton.fonts, msg->body.addButton.text);
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addButton.id, LCD_OBJECT_TYPE_BUTTON);
					break;

				case LCD_MSG_TYPE_ADD_BUTTON_IMG :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addButtonImg(msg->body.addButtonImg.x, msg->body.addButtonImg.y, UART_OVER_LCD_imageIdConvertor(msg->body.addButtonImg.imageId), UART_OVER_LCD_imageIdConvertor(msg->body.addButtonImg.imageLockId), msg->body.addButtonImg.lockTouch, (bool_e *)&(objectId[idStorage].data.link.buttonImg.touchState));
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addButtonImg.id, LCD_OBJECT_TYPE_BUTTON_IMG);
					break;

				case LCD_MSG_TYPE_ADD_CIRCLE :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addCircle(msg->body.addCircle.x, msg->body.addCircle.y, msg->body.addCircle.r, msg->body.addCircle.colorBorder, msg->body.addCircle.colorCenter);
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addCircle.id, LCD_OBJECT_TYPE_CIRCLE);
					break;

				case LCD_MSG_TYPE_ADD_IMAGE :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addImage(msg->body.addImage.x, msg->body.addImage.y, UART_OVER_LCD_imageIdConvertor(msg->body.addImage.imageId));
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addImage.id, LCD_OBJECT_TYPE_IMAGE);
					break;

				case LCD_MSG_TYPE_ADD_LINE :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addLine(msg->body.addLine.x0, msg->body.addLine.y0, msg->body.addLine.x1, msg->body.addLine.y1, msg->body.addLine.color);
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addLine.id, LCD_OBJECT_TYPE_LINE);
					break;

				case LCD_MSG_TYPE_ADD_PROGRESS_BAR :
					idStorage = UART_OVER_LCD_newObject();
					objectId[idStorage].data.link.progressBar.value = msg->body.addProgressBar.baseValue;
					idObject = MIDDLEWARE_addProgressBar(msg->body.addProgressBar.x, msg->body.addProgressBar.y, msg->body.addProgressBar.width, msg->body.addProgressBar.height, msg->body.addProgressBar.orientation, (Uint8 *)&(objectId[idStorage].data.link.progressBar.value));
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addProgressBar.id, LCD_OBJECT_TYPE_PROGRESS_BAR);
					break;

				case LCD_MSG_TYPE_ADD_RECTANGLE :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addRectangle(msg->body.addRectangle.x, msg->body.addRectangle.y, msg->body.addRectangle.width, msg->body.addRectangle.height, msg->body.addRectangle.colorBorder, msg->body.addRectangle.colorCenter);
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addRectangle.id, LCD_OBJECT_TYPE_RECTANGLE);
					break;

				case LCD_MSG_TYPE_ADD_SLIDER :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addSlider(msg->body.addSlider.x, msg->body.addSlider.y, msg->body.addSlider.width, msg->body.addSlider.height, msg->body.addSlider.minValue, msg->body.addSlider.maxValue, msg->body.addSlider.orientation, (Sint32 *)&(objectId[idStorage].data.link.slider.value));
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addSlider.id, LCD_OBJECT_TYPE_SLIDER);
					break;

				case LCD_MSG_TYPE_ADD_TEXT :
					idStorage = UART_OVER_LCD_newObject();
					idObject = MIDDLEWARE_addText(msg->body.addText.x, msg->body.addText.y, msg->body.addText.colorText, msg->body.addText.colorBackground, msg->body.addText.fonts, msg->body.addText.text);
					UART_OVER_LCD_setSyncObject(idStorage, idObject, msg->body.addText.id, LCD_OBJECT_TYPE_TEXT);
					break;

				case LCD_MSG_TYPE_DELETE_OBJECT :
					UART_OVER_LCD_deleteObject(msg->body.deleteObject.id);
					break;

				case LCD_MSG_TYPE_RESET_SCREEN :
					MIDDLEWARE_resetScreen();
					UART_OVER_LCD_deleteAllObject();
					break;

				case LCD_MSG_TYPE_SET_BACKGROUND :
					MIDDLEWARE_setBackground(msg->body.setBackground.color);
					break;

				case LCD_MSG_TYPE_SET_TEXT :
					idObject = UART_OVER_LCD_getIdLocalByRemoteId(msg->body.setText.id);
					if(idObject != LCD_OBJECT_ID_ERROR_FULL)
						MIDDLEWARE_setText(idObject, msg->body.setText.text);
					break;

				case LCD_MSG_TYPE_SET_MENU :
					INTERFACE_setInterface(msg->body.setMenu.menuId);
					break;
			#endif

			default:
				break;
		}
	}

	static void LCD_OVER_UART_refreshObject(void){
			#ifndef LCD_OVER_UART__IHM_MODE
				LCD_objectId_t id;
				for(id = 0; id < LCD_NB_MAX_OBJECT; id++){
					if(LCD_objects[id].used == TRUE){
						switch(LCD_objects[id].object.type){

							case LCD_OBJECT_TYPE_PROGRESS_BAR :
								if(LCD_objects[id].object.link.progressBar.lastValue != *(LCD_objects[id].object.link.progressBar.valuePtr)){
									LCD_objects[id].object.link.progressBar.lastValue = *(LCD_objects[id].object.link.progressBar.valuePtr);
									LCD_msg_s msg;
									msg.header.type = LCD_MSG_TYPE_UPDATE_PROGRESS_BAR;
									msg.header.size = SIZE_LCD_UPDATE_PROGRESS_BAR;
									msg.body.updateProgressBar.value = *(LCD_objects[id].object.link.progressBar.valuePtr);
									msg.body.updateProgressBar.id = id;
									LCD_OVER_UART_sendMsg(&msg);
								}
								break;

							case LCD_OBJECT_TYPE_BUTTON_IMG :
							case LCD_OBJECT_TYPE_SLIDER :
							case LCD_OBJECT_TYPE_BUTTON :
							case LCD_OBJECT_TYPE_TEXT :
							case LCD_OBJECT_TYPE_IMAGE :
							case LCD_OBJECT_TYPE_ANIMATED_IMAGE :
							case LCD_OBJECT_TYPE_RECTANGLE :
							case LCD_OBJECT_TYPE_CIRCLE :
							case LCD_OBJECT_TYPE_LINE :
							default:
								break;
						}
					}
				}
			#else
				LCD_objectId_t id;
				for(id = 0; id < LCD_NB_MAX_OBJECT; id++){
					if(objectId[id].used == TRUE){
						switch(objectId[id].data.objectType){

							case LCD_OBJECT_TYPE_BUTTON :
								if(objectId[id].data.link.button.lastTouchState != objectId[id].data.link.button.touchState){
									objectId[id].data.link.button.lastTouchState = objectId[id].data.link.button.touchState;
									LCD_msg_s msg;
									msg.header.type = LCD_MSG_TYPE_UPDATE_BUTTON;
									msg.header.size = SIZE_LCD_UPDATE_BUTTON;
									msg.body.updateButton.touchState =  objectId[id].data.link.button.touchState;
									msg.body.updateButton.id = id;
									LCD_OVER_UART_sendMsg(&msg);
								}
								break;

							case LCD_OBJECT_TYPE_BUTTON_IMG :
								if(objectId[id].data.link.buttonImg.lastTouchState != *(objectId[id].data.link.buttonImg.touchState)){
									objectId[id].data.link.buttonImg.lastTouchState = *(objectId[id].data.link.buttonImg.touchState);
									LCD_msg_s msg;
									msg.header.type = LCD_MSG_TYPE_UPDATE_BUTTON_IMG;
									msg.header.size = SIZE_LCD_UPDATE_BUTTON_IMG;
									msg.body.updateButtonImg.touchState =  *(objectId[id].data.link.buttonImg.touchState);
									msg.body.updateButtonImg.id = id;
									LCD_OVER_UART_sendMsg(&msg);
								}
								break;

							case LCD_OBJECT_TYPE_SLIDER :
								if(objectId[id].data.link.slider.lastValue != *(objectId[id].data.link.slider.value)){
									objectId[id].data.link.slider.lastValue = *(objectId[id].data.link.slider.value);
									LCD_msg_s msg;
									msg.header.type = LCD_MSG_TYPE_UPDATE_SLIDER;
									msg.header.size = SIZE_LCD_UPDATE_SLIDER;
									msg.body.updateSlider.value =  *(objectId[id].data.link.slider.value);
									msg.body.updateSlider.id = id;
									LCD_OVER_UART_sendMsg(&msg);
								}
								break;

							case LCD_OBJECT_TYPE_TEXT :
							case LCD_OBJECT_TYPE_IMAGE :
							case LCD_OBJECT_TYPE_ANIMATED_IMAGE :
							case LCD_OBJECT_TYPE_RECTANGLE :
							case LCD_OBJECT_TYPE_CIRCLE :
							case LCD_OBJECT_TYPE_LINE :
							default:
								break;
						}
					}
				}
			#endif
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
