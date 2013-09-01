/* Martin Thomas 4/2009, 3/2010 */
#include "../../stm32f4xx/stm32f4xx.h"
#include "comm.h"
#include "../../QS/QS_uart.h"

#define USARTx USART1

int comm_test(void)
{
	return 1;
}

char comm_get(void)
{
	while(UART1_data_ready() == 0);
	return UART1_get_next_msg();
}

void comm_init (void)
{
	// already done in main.c
}

void comm_put(char c)
{
	 UART1_putc(c);
}
