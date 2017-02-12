#ifndef XBEE_CONFIG_H
	#define	XBEE_CONFIG_H
	#include "../QS_all.h"

	#ifdef USE_XBEE

		#define LOG_PREFIX_XBEE "xbee :"
		#define LOG_PREFIX_COMMUNICATION "com :"

		#ifdef XBEE_PLUGGED_ON_UART1
			#define XBee_uartId	1
			#define XBee_putc(c) UART1_putc(c)
			#define XBee_data_ready() UART1_data_ready()
			#define XBee_get_next_msg() UART1_get_next_msg()
		#endif
		#ifdef XBEE_PLUGGED_ON_UART2
			#define XBee_uartId	2
			#define XBee_putc(c) UART2_putc(c)
			#define XBee_data_ready() UART2_data_ready()
			#define XBee_get_next_msg() UART2_get_next_msg()
		#endif
		#ifdef XBEE_PLUGGED_ON_UART3
			#define XBee_uartId	3
			#define XBee_putc(c) UART3_putc(c)
			#define XBee_data_ready() UART3_data_ready()
			#define XBee_get_next_msg() UART3_get_next_msg()
		#endif
		#ifdef XBEE_PLUGGED_ON_UART4
			#define XBee_uartId	4
			#define XBee_putc(c) UART4_putc(c)
			#define XBee_data_ready() UART4_data_ready()
			#define XBee_get_next_msg() UAR41_get_next_msg()
		#endif
		#ifdef XBEE_PLUGGED_ON_UART5
			#define XBee_uartId	5
			#define XBee_putc(c) UART5_putc(c)
			#define XBee_data_ready() UART5_data_ready()
			#define XBee_get_next_msg() UART5_get_next_msg()
		#endif
		#ifdef XBEE_PLUGGED_ON_UART6
			#define XBee_uartId	6
			#define XBee_putc(c) UART6_putc(c)
			#define XBee_data_ready() UART6_data_ready()
			#define XBee_get_next_msg() UART6_get_next_msg()
		#endif

	#endif // def USE_XBEE

#endif /* ifndef XBEE_CONFIG_H */
