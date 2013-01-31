/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_uart.c
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour 
 *					interfacage rs232
 *	Auteur : Jacen
 *	Version 20091014
 */

#define QS_UART_C
#include "QS_uart.h"

#ifdef USE_UART1
	void UART1_putc(Uint8 mes)
	{
		while(BusyUART1());
		putcUART1(mes);	
	}
#endif /* def USE_UART1 */

#ifdef USE_UART2
	void UART2_putc(Uint8 mes)
	{
		while(BusyUART2());
		putcUART2(mes);	
	}
#endif /* def USE_UART2 */



//	Determination de la vitesse de l'uart en fonction
//	de la PLL utilisée
	#ifdef FREQ_10MHZ
		#define UART_SPEED 64
	#elif (defined FREQ_20MHZ)
		#define UART_SPEED 128
	#elif (defined FREQ_40MHZ)
		#define UART_SPEED 256
	#elif (defined FREQ_INTERNAL_CLK)
		#define UART_SPEED 50	//49.536
	#endif
	/* A 10MHz de freq interne Vitesse 16 -> 38400bps 64-> 9600*/
 
//	Fin de determination de la vitesse.



/*	fonction initialisant les uart choisis
	vitesse : 9600 bauds
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/
void UART_init(void)
{
	Sint16 U1MODEvalue;
	Sint16 U1STAvalue;
	U1MODEvalue = UART_EN & UART_IDLE_CON &
		UART_DIS_WAKE & UART_DIS_LOOPBACK &
		UART_DIS_ABAUD & UART_NO_PAR_8BIT &
		UART_1STOPBIT;
	U1STAvalue = UART_INT_TX &
		UART_TX_PIN_NORMAL &
		UART_TX_ENABLE &
		UART_INT_RX_CHAR &
		UART_ADR_DETECT_DIS &
		UART_RX_OVERRUN_CLEAR ;

	#ifdef USE_UART1
		OpenUART1(U1MODEvalue, U1STAvalue, UART_SPEED);
		#ifndef USE_UART1RXINTERRUPT
			DisableIntU1RX;
		#else
			global.u1rxnum = 0;
			global.flags.u1rx =0;
		#endif /* ndef USE_UART1RXINTERRUPT */
		DisableIntU1TX;
		ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR4 &
			UART_TX_INT_DIS & UART_TX_INT_PR3);
	#endif /* def USE_UART1 */

	#ifdef USE_UART2
		OpenUART2(U1MODEvalue, U1STAvalue, UART_SPEED);
		#ifndef USE_UART2RXINTERRUPT
			DisableIntU2RX;
		#else
			global.u2rxnum = 0;
			global.flags.u2rx =0;
		#endif /* ndef USE_UART2RXINTERRUPT */
		DisableIntU2TX;
		ConfigIntUART2(UART_RX_INT_EN & UART_RX_INT_PR4 &
			UART_TX_INT_DIS & UART_TX_INT_PR3);
	#endif /* def USE_UART2 */
}


#ifdef USE_UART1RXINTERRUPT
	void _ISR _U1RXInterrupt(void)
	{
		Uint8 * receiveddata = global.u1rxbuf + (global.u1rxnum%UART_RX_BUF_SIZE);
		
		while( DataRdyUART1())
		{
			*(receiveddata++) = ReadUART1();
			global.u1rxnum++;
			global.flags.u1rx =1;
			/* pour eviter les comportements indésirables */
			if (receiveddata - global.u1rxbuf >= UART_RX_BUF_SIZE)
				receiveddata = global.u1rxbuf;
		}
		IFS0bits.U1RXIF = 0;
	}

	Uint8 UART1_get_next_msg()
	{
		static Uint32 next_to_read =0;		
		if (global.u1rxnum > next_to_read)
		{
			if (global.u1rxnum - next_to_read == 1)
				global.flags.u1rx = 0;
			return *(global.u1rxbuf + ((next_to_read++) % UART_RX_BUF_SIZE));
		}
		else
			return 0;
	}
#endif /* def USE_UART1RXINTERRUPT */


#ifdef USE_UART2RXINTERRUPT
	void _ISR _U2RXInterrupt(void)
	{
		Uint8 * receiveddata = global.u2rxbuf + (global.u2rxnum%UART_RX_BUF_SIZE);
		
		while( DataRdyUART2())
		{
			*(receiveddata++) = ReadUART2();
			global.u2rxnum++;
			global.flags.u2rx =1;
			/* pour eviter les comportements indésirables */
			if (receiveddata - global.u2rxbuf >= UART_RX_BUF_SIZE)
				receiveddata = global.u2rxbuf;
		}
		IFS1bits.U2RXIF = 0;
	}

	Uint8 UART2_get_next_msg()
	{
		static Uint32 next_to_read =0;		
		if (global.u2rxnum > next_to_read)
		{
			if (global.u2rxnum - next_to_read == 1)
				global.flags.u2rx = 0;
			return *(global.u2rxbuf + ((next_to_read++) % UART_RX_BUF_SIZE));
		}
		else
			return 0;
	}
#endif /* def USE_UART2RXINTERRUPT */
