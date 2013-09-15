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
	
		typedef struct
		{
			volatile Uint8 * datas;
			Uint16 size;
			Uint16 nb_datas;
			Uint16 index_read;
			Uint16 index_write;
		}buffer_t;
		volatile Uint8 b1tx[BUFFER_TX_SIZE];
		volatile buffer_t buffer1tx = {b1tx,BUFFER_TX_SIZE,0,0,0};
		extern int __C30_UART;
		
		bool_e IsFull_buffer(volatile buffer_t * b)

		{
			if(buffer1tx.nb_datas >= buffer1tx.size)
				return TRUE;
			else
				return FALSE;
		}

		bool_e IsNotEmpty_buffer(volatile buffer_t * b)
		{

			if(buffer1tx.nb_datas > (Uint8)0)
				return TRUE;
			else
				return FALSE;
		}


		int __attribute__((__weak__, __section__(".libc")))	write(int handle, void *buffer, unsigned int len)
		{
		  int i;
		  volatile UxMODEBITS *umode = &U1MODEbits;
		  volatile UxSTABITS *ustatus = &U1STAbits;
		  volatile unsigned int *txreg = &U1TXREG;
		  volatile unsigned int *brg = &U1BRG;

		  switch (handle)
		  {
			case 0:
			case 1:
			case 2:
			  if ((__C30_UART != 1) && (&U2BRG)) {
				umode = &U2MODEbits;
				ustatus = &U2STAbits;
				txreg = &U2TXREG;
				brg = &U2BRG;
			  }
			  if ((umode->UARTEN) == 0)
			  {
				*brg = 0;
				umode->UARTEN = 1;
			  }
			  if ((ustatus->UTXEN) == 0)
			  {
				ustatus->UTXEN = 1;
			  }
			  for (i = len; i; --i)
			  {
				  UART1_putc(*(char*)buffer++);	//MODIFICATION APPORTEE AU CODE DE MICROCHIP...
			  }
			  break;

			default:
			/*{
			  SIMIO simio;
			  register PSIMIO psimio asm("w0") = &simio;

			  simio.method = SIM_WRITE;
			  simio.u.write.handle = handle;
			  simio.u.write.buffer = buffer;
			  simio.u.write.len = len;
			  dowrite(psimio);
			  len = simio.u.write.len;
			  break;
			}
			 */
				break;
		  }
		  return(len);
		}




		
		void UART1_putc(Uint8 c)
		{
			if(!BusyUART1())
				putcUART1(c);
			else
			{
				assert(buffer1tx.nb_datas < buffer1tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer1tx.nb_datas >= buffer1tx.size);	//ON BLOQUE ICI

				DisableIntU1TX;	//On interdit la préemption ici.. pour éviter les Read pendant les Write.



				if(buffer1tx.nb_datas < buffer1tx.size)
					{
						buffer1tx.datas[buffer1tx.index_write] = c;
						buffer1tx.index_write = (buffer1tx.index_write>=buffer1tx.size-1)?0:(buffer1tx.index_write + 1);
						buffer1tx.nb_datas++;
					}
				EnableIntU1TX;	//On active l'IT sur TX... lors du premier caractère à envoyer...
			}
				//mise en buffer + activation IT U1TX.
		}

		
			
		void _ISR _U1TXInterrupt(void)
		{
			Uint8 c;
			
			//debufferiser. 
			if(!BusyUART1() && IsNotEmpty_buffer(&buffer1tx))
			{
				assert (buffer1tx.index_read < buffer1tx.size);
				//Critical section
				if(buffer1tx.nb_datas > (Uint8)0)
				{
					c = buffer1tx.datas[buffer1tx.index_read];
					buffer1tx.index_read = (buffer1tx.index_read>=buffer1tx.size-1)?0:(buffer1tx.index_read + 1);
					buffer1tx.nb_datas--;
					putcUART1(c);
				}
				//Critical section
				
			}
			else if(!BusyUART1() && !IsNotEmpty_buffer(&buffer1tx))
				DisableIntU1TX;	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
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
