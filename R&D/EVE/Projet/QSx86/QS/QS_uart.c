/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_uart.c
 *	Package : QSx86
 *	Description : Fonctions d'utilisation UART pour interfaçage RS232
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#include "QS_uart.h"


//	Determination de la vitesse de l'uart en fonction
//	de la PLL utilisée
/*#ifdef FREQ_10MHZ
	#define UART_SPEED 64
#elif (defined FREQ_20MHZ)
	#define UART_SPEED 128
#elif (defined FREQ_40MHZ)
	#define UART_SPEED 256
#elif (defined FREQ_INTERNAL_CLK)
	#define UART_SPEED 50	//49.536
#endif
*/
/* A 10MHz de freq interne Vitesse 16 -> 38400bps 64-> 9600*/
//	Fin de determination de la vitesse.

/* Variables Globales pour l'UART */
#ifdef USE_UART1RXINTERRUPT
	//static Uint8 m_u1rxbuf[UART_RX_BUF_SIZE];
	static volatile Uint32 m_u1rxnum;
	static volatile bool_e m_u1rx;	// message reçu sur uart1
#endif /* def USE_UART1RXINTERRUPT */
#ifdef USE_UART2RXINTERRUPT
	static Uint8 m_u2rxbuf[UART_RX_BUF_SIZE];
	static volatile Uint32 m_u2rxnum;
	static volatile bool_e m_u2rx;	// message reçu sur uart2
#endif /* def USE_UART2RXINTERRUPT */

/*	fonction initialisant les uart choisis
	vitesse : 9600 bauds
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/
void UART_init(void)
{
	
}


#ifdef USE_UART1
	void UART1_putc(Uint8 mes)
	{
		
	}

	#ifdef USE_UART1RXINTERRUPT

		bool_e UART1_data_ready()
		{
			return m_u1rx;
		}

/*		void _ISR _U1RXInterrupt(void)
		{
			Uint8 * receiveddata = m_u1rxbuf + (m_u1rxnum%UART_RX_BUF_SIZE);

			while( DataRdyUART1())
			{
				LED_UART=!LED_UART;
				*(receiveddata++) = ReadUART1();
				m_u1rxnum++;
				m_u1rx =1;
				// pour eviter les comportements indésirables 
				if (receiveddata - m_u1rxbuf >= UART_RX_BUF_SIZE)
					receiveddata = m_u1rxbuf;
			}
			IFS0bits.U1RXIF = 0;
		}
*/
		Uint8 UART1_get_next_msg()
		{
			return 0;
		}
	#endif /* def USE_UART1RXINTERRUPT */
#endif /* def USE_UART1 */

#ifdef USE_UART2
	void UART2_putc(Uint8 mes)
	{		

	}

	#ifdef USE_UART2RXINTERRUPT

		bool_e UART2_data_ready()
		{
			return m_u2rx;
		}

/*		void _ISR _U2RXInterrupt(void)
		{
			Uint8 * receiveddata = m_u2rxbuf + (m_u2rxnum%UART_RX_BUF_SIZE);

			while( DataRdyUART2())
			{
				LED_UART=!LED_UART;
				*(receiveddata++) = ReadUART2();
				m_u2rxnum++;
				m_u2rx =1;
				 pour eviter les comportements indésirables 
				if (receiveddata - m_u2rxbuf >= UART_RX_BUF_SIZE)
					receiveddata = m_u2rxbuf;
			}
			IFS1bits.U2RXIF = 0;
		}
*/
		Uint8 UART2_get_next_msg()
		{
			return 0;
		}
	#endif /* def USE_UART2RXINTERRUPT */

#endif /* def USE_UART2 */
