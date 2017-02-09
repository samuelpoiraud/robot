#ifndef NETWORK_SPREADER_H
	#define	NETWORK_SPREADER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"
		#include "../../../QS_uart.h"

		#define IS_NETWORK_SPREADER_UART_USER(x)		(((x) == SYSTEM_UART_USER)	\
														|| ((x) == NETWORK_UART_USER))
		typedef enum{
			SYSTEM_UART_USER,
			NETWORK_UART_USER
		}NETWORK_SPREADER_UartUser_e;

		void NETWORK_SPREADER_init();

		void NETWORK_SPREADER_setUser(NETWORK_SPREADER_UartUser_e UartUser);

		void NETWORK_SPREADER_networkWrite(char Byte);
		void NETWORK_SPREADER_networkListen(UART_dataReceivedFunctionPtr Function);

		void NETWORK_SPREADER_systemWrite(char Byte);
		void NETWORK_SPREADER_systemListen(UART_dataReceivedFunctionPtr Function);

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SPREADER_H */
