/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_uart.c
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour
 *					interfacage rs232
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100924
 */

#include "QS_uart.h"

#include <uart.h>

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

/* Variables Globales pour l'UART */
#ifdef USE_UART1RXINTERRUPT
	static Uint8 m_u1rxbuf[UART_RX_BUF_SIZE];
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
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

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
		ConfigIntUART1(UART_RX_INT_DIS & UART_RX_INT_PR4 &
			UART_TX_INT_DIS & UART_TX_INT_PR3);
		DisableIntU1TX;
		#ifdef USE_UART1RXINTERRUPT
			EnableIntU1RX;
			m_u1rxnum = 0;
			m_u1rx =0;
		#else
			DisableIntU1RX;
		#endif /* def USE_UART1RXINTERRUPT */
	#endif /* def USE_UART1 */

	#ifdef USE_UART2
		OpenUART2(U1MODEvalue, U1STAvalue, UART_SPEED);
		ConfigIntUART2(UART_RX_INT_DIS & UART_RX_INT_PR4 &
			UART_TX_INT_DIS & UART_TX_INT_PR3);
		DisableIntU2TX;
		#ifdef USE_UART2RXINTERRUPT
			EnableIntU2RX;
			m_u2rxnum = 0;
			m_u2rx =0;
		#else
			DisableIntU2RX;
		#endif /* def USE_UART2RXINTERRUPT */
	#endif /* def USE_UART2 */
}


#ifdef USE_UART1

	#ifdef USE_UART1RXINTERRUPT

		bool_e UART1_data_ready()
		{
			return m_u1rx;
		}

		void _ISR _U1RXInterrupt(void)
		{
			Uint8 * receiveddata = m_u1rxbuf + (m_u1rxnum%UART_RX_BUF_SIZE);

			while( DataRdyUART1())
			{
				LED_UART=!LED_UART;
				*(receiveddata++) = ReadUART1();
				m_u1rxnum++;
				m_u1rx =1;
				/* pour eviter les comportements indésirables */
				if (receiveddata - m_u1rxbuf >= UART_RX_BUF_SIZE)
					receiveddata = m_u1rxbuf;
			}
			IFS0bits.U1RXIF = 0;
		}

		Uint8 UART1_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u1rxnum > next_to_read)
			{
				if (m_u1rxnum - next_to_read == 1)
					m_u1rx = 0;
				return *(m_u1rxbuf + ((next_to_read++) % UART_RX_BUF_SIZE));
			}
			else
				return 0;
		}
	#endif /* def USE_UART1RXINTERRUPT */
	
	
	#ifdef USE_UART1TXINTERRUPT
		#warning "USE_UART1TXINTERRUPT défini... Ce mode n'est pas testé... et même qu'en plus il est même pas implémenté !"
		#warning "USE_UART1TXINTERRUPT : ce mode a un impact qui doit être assumé par l'utilisateur. (risque de Buffer Overflow)"
		
		//fonction dont on ne choisi par le nom, appelée par fprintf(_H_USER...) pour chaque caractère à envoyer.
		void _user_putc(char c)
		{
			UART1_putc(c);	//On bufferise...
		}
			
		void _ISR _U1RXInterrupt(void)
		{
			//debufferiser. 
			
			//Si buffer vide -> désactiver IT TX.
		}	
		
		//Fonction non blocante, mais risque de buffer overflow.
		void UART1_putc(Uint8 mes)
		{
			
				//mise en buffer + activation IT U1TX.
		}
	

	#else	/* def USE_UART1TXINTERRUPT */
	
		//Fonction blocante
		void UART1_putc(Uint8 mes)
		{
			while(BusyUART1());
			putcUART1(mes);
		}

	
	
	#endif /* def USE_UART1TXINTERRUPT */
		
	
		
	
	
	
#endif /* def USE_UART1 */

#ifdef USE_UART2
	void UART2_putc(Uint8 mes)
	{
		while(BusyUART2());
		putcUART2(mes);
	}

	#ifdef USE_UART2RXINTERRUPT

		bool_e UART2_data_ready()
		{
			return m_u2rx;
		}

		void _ISR _U2RXInterrupt(void)
		{
			Uint8 * receiveddata = m_u2rxbuf + (m_u2rxnum%UART_RX_BUF_SIZE);

			while( DataRdyUART2())
			{
				LED_UART=!LED_UART;
				*(receiveddata++) = ReadUART2();
				m_u2rxnum++;
				m_u2rx =1;
				/* pour eviter les comportements indésirables */
				if (receiveddata - m_u2rxbuf >= UART_RX_BUF_SIZE)
					receiveddata = m_u2rxbuf;
			}
			IFS1bits.U2RXIF = 0;
		}

		Uint8 UART2_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u2rxnum > next_to_read)
			{
				if (m_u2rxnum - next_to_read == 1)
					m_u2rx = 0;
				return *(m_u2rxbuf + ((next_to_read++) % UART_RX_BUF_SIZE));
			}
			else
				return 0;
		}
	#endif /* def USE_UART2RXINTERRUPT */

#endif /* def USE_UART2 */
