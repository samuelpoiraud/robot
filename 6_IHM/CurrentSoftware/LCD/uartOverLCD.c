#include "uartOverLCD.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_lcd_over_uart.h"
#include "../QS/QS_lcd_over_uart_type.h"
#include "../QS/QS_outputlog.h"
#include "interface.h"
#include "middleware.h"

#ifdef USE_UART_OVER_LCD

	#ifndef UART_OVER_LCD__UART_ID
		#define UART_OVER_LCD__UART_ID	1
		#warning "UART_OVER_LCD__UART_ID non définit mis à 1 par défaut"
	#endif

	#define LCD_BUFFER_SIZE		5

	typedef struct{
		LCD_msg_s buffer[LCD_BUFFER_SIZE];
		Uint8 readIndex;
		Uint8 writeIndex;
	}LCD_FIFO_msg_t;

	typedef enum{
		LCD_PARSER_STATE_DELIMITER,
		LCD_PARSER_STATE_TYPE,
		LCD_PARSER_STATE_SIZE,
		LCD_PARSER_STATE_DATA
	}UART_OVER_LCD_parserState_e;

	typedef enum{
		LCD_PARSER_SIZE_DELIMITER	= 1,
		LCD_PARSER_SIZE_TYPE		= 1,
		LCD_PARSER_SIZE_SIZE		= 1,
	}UART_OVER_LCD_parserSize_e;

	typedef struct{
		LCD_objectId_t remoteId;
	}objectIdMatch_s;

	objectIdMatch_s objectId[LCD_NB_MAX_OBJECT];
	static volatile LCD_FIFO_msg_t LCD_FIFO_msg;

	static void UART_OVER_LCD_receiveByte(Uint8 byte);
	static void UART_OVER_LCD_receiveMsg(LCD_msg_s * msg);

	static void LCD_FIFO_init();
	static bool_e LCD_FIFO_isFull();
	static bool_e LCD_FIFO_isEmpty();
	static bool_e LCD_FIFO_insertMsg(LCD_msg_s * msg);
	static bool_e LCD_FIFO_getMsg(LCD_msg_s * msg);
	static void LCD_FIFO_incWriteIndex();
	static void LCD_FIFO_incReadIndex();


	void UART_OVER_LCD_init(void){
		UART_init();
		LCD_FIFO_init();
		UART_setListenner(UART_OVER_LCD__UART_ID, &UART_OVER_LCD_receiveByte);
	}

	void UART_OVER_LCD_processMain(void){

		LCD_msg_s msg;

		while(LCD_FIFO_getMsg(&msg)){
			UART_OVER_LCD_receiveMsg(&msg);
		}
	}

	static void UART_OVER_LCD_receiveMsg(LCD_msg_s * msg){
		switch(msg->header.type){
			case LCD_MSG_TYPE_SET_MENU :
				INTERFACE_setInterface(msg->body.setMenu.menuId);
				break;

			case LCD_MSG_TYPE_ADD_TEXT :{
				objectId_t id;
				//if(INTERFACE_ihmAvailable() == FALSE)
				//	return;
				id = MIDDLEWARE_addText(msg->body.addText.x, msg->body.addText.y, msg->body.addText.colorText, msg->body.addText.colorBackground, msg->body.addText.text);
				if(id != OBJECT_ID_ERROR_FULL && id < LCD_NB_MAX_OBJECT){
					objectId[id].remoteId = msg->body.addText.id;
				}
				}break;

			default:
				break;
		}
	}

	//////////////////////////////////////////////////////////////////
	//---------------------Fonction Bas niveau----------------------//
	//////////////////////////////////////////////////////////////////

	static void UART_OVER_LCD_receiveByte(Uint8 byte){
		static UART_OVER_LCD_parserState_e state = LCD_PARSER_STATE_DELIMITER;
		static UART_OVER_LCD_parserState_e last_state = LCD_PARSER_STATE_DELIMITER;

		static LCD_msg_s msg;

		static Uint16 byteToRead;
		static Uint16 byteCounter;

		static time32_t lastTimeByte;

		if(lastTimeByte - global.absolute_time > LCD_OVER_UART_COM_TIMEOUT){
			state = LCD_PARSER_STATE_DELIMITER;
			last_state = LCD_PARSER_STATE_DELIMITER;
		}

		bool_e entrance = last_state != state;
		last_state = state;

		switch (state)
		{
			case LCD_PARSER_STATE_DELIMITER:

				if(byte != LCD_OVER_UART_DELIMITER)
					error_printf("Invalid start 0x%02x != 0x%02x\n", byte, LCD_OVER_UART_DELIMITER);
				else{
					state = LCD_PARSER_STATE_TYPE;
				}
				break;

			case LCD_PARSER_STATE_TYPE:

				if(entrance){
					byteToRead = LCD_PARSER_SIZE_TYPE;
					byteCounter = 0;
				}

				if(byteCounter < LCD_OVER_UART_HEADER_SIZE)
					msg.header.rawData[byteCounter++] = byte;

				byteToRead--;

				if(byteToRead == 0){
					state = LCD_PARSER_STATE_SIZE;
				}
				break;

			case LCD_PARSER_STATE_SIZE:

				if(entrance){
					byteToRead = 1;
				}

				if(byteCounter < LCD_OVER_UART_HEADER_SIZE)
					msg.header.rawData[byteCounter++] = byte;

				byteToRead--;

				if(byteToRead == 0){
					if(msg.header.size <= LCD_OVER_UART_MAX_SIZE)
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


	///-------------------------- FIFO function

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

		for(i=0; i < msg->header.size - 1; i++)
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

		for(i=0; i < msg->header.size - 1; i++)
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
