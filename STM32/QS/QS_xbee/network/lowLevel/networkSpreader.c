#include "networkSpreader.h"

#ifdef USE_XBEE

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_XBEE
	#define LOG_PREFIX LOG_PREFIX_XBEE
	#include "../../../QS_outputlog.h"

	static volatile NETWORK_SPREADER_UartUser_e uartUser = SYSTEM_UART_USER;
	static volatile UART_dataReceivedFunctionPtr networkListenner = NULL;
	static volatile UART_dataReceivedFunctionPtr systemListenner = NULL;

	static void NETWORK_SPREADER_listenner(char Byte);

	void NETWORK_SPREADER_init(){
		UART_setListenner(XBee_uartId, (UART_dataReceivedFunctionPtr)&NETWORK_SPREADER_listenner);
	}

	void NETWORK_SPREADER_setUser(NETWORK_SPREADER_UartUser_e UartUser){
		assert(IS_NETWORK_SPREADER_UART_USER(uartUser));
		uartUser = UartUser;
	}

	void NETWORK_SPREADER_networkWrite(char Byte){
		if(uartUser == NETWORK_UART_USER)
			XBee_putc(Byte);
	}

	void NETWORK_SPREADER_networkListen(UART_dataReceivedFunctionPtr Function){
		networkListenner = Function;
	}

	void NETWORK_SPREADER_systemWrite(char Byte){
		if(uartUser == SYSTEM_UART_USER)
			XBee_putc(Byte);
	}

	void NETWORK_SPREADER_systemListen(UART_dataReceivedFunctionPtr Function){
		systemListenner = Function;
	}

	static void NETWORK_SPREADER_listenner(char Byte){
		switch(uartUser){
			case NETWORK_UART_USER:
				if(networkListenner != NULL)
					networkListenner(Byte);
				break;

			case SYSTEM_UART_USER:
				if(systemListenner != NULL)
					systemListenner(Byte);
				break;

			default:
				break;
		}
	}

#endif
