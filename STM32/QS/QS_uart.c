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
#include "QS_ports.h"
#include "stm32f4xx_usart.h"

#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif

//	Determination de la vitesse de l'uart en fonction
//	de la PLL utilisée
#ifndef UART1_BAUDRATE
	#define UART1_BAUDRATE 115200
#endif
#ifndef UART2_BAUDRATE
	#define UART2_BAUDRATE 115200
#endif
#ifndef UART3_BAUDRATE
	#define UART3_BAUDRATE 115200
#endif

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
#ifdef USE_UART3RXINTERRUPT
	static Uint8 m_u3rxbuf[UART_RX_BUF_SIZE];
	static volatile Uint32 m_u3rxnum;
	static volatile bool_e m_u3rx;	// message reçu sur uart3
#endif /* def USE_UART3RXINTERRUPT */
#if (defined USE_UART1TXINTERRUPT || defined USE_UART2TXINTERRUPT || defined USE_UART3TXINTERRUPT)
	typedef struct
	{
		volatile Uint8 * datas;
		Uint16 size;
		Uint16 nb_datas;
		Uint16 index_read;
		Uint16 index_write;
	}buffer_t;

	bool_e IsFull_buffer(volatile buffer_t * b)
	{
		if(b->nb_datas >= b->size)
			return TRUE;
		else
			return FALSE;
	}

	bool_e IsNotEmpty_buffer(volatile buffer_t * b)
	{

		if(b->nb_datas > (Uint8)0)
			return TRUE;
		else
			return FALSE;
	}
#endif

/*	fonction initialisant les uart choisis
	vitesse : 115200 bauds (par défaut)
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

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	PORTS_uarts_init();

#ifdef USE_UART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_OverSampling8Cmd(USART1, ENABLE);
	UART_set_baudrate(1, UART1_BAUDRATE);

	#ifdef USE_UART1RXINTERRUPT
		m_u1rxnum = 0;
		m_u1rx = 0;
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	#endif

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
#endif


#ifdef USE_UART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_OverSampling8Cmd(USART2, ENABLE);
	UART_set_baudrate(2, UART2_BAUDRATE);

	#ifdef USE_UART2RXINTERRUPT
		m_u2rxnum = 0;
		m_u2rx = 0;
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	#endif

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
#endif

#ifdef USE_UART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	USART_OverSampling8Cmd(USART3, ENABLE);
	UART_set_baudrate(3, UART3_BAUDRATE);

	#ifdef USE_UART3RXINTERRUPT
		m_u3rxnum = 0;
		m_u3rx = 0;
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	#endif

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);
#endif

}

void UART_set_baudrate(Uint8 uart_id, Uint32 baudrate) {
	USART_InitTypeDef USART_InitStructure;

	assert(uart_id <= 3);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	switch(uart_id) {
		case 1:
			USART_Init(USART1, &USART_InitStructure);
			break;
		case 2:
			USART_Init(USART2, &USART_InitStructure);
			break;
		case 3:
			USART_Init(USART3, &USART_InitStructure);
			break;
	}
}


#ifdef USE_UART1
	#ifdef USE_UART1RXINTERRUPT
		bool_e UART1_data_ready()
		{
			return m_u1rx;
		}

		Uint8 UART1_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u1rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART1_IRQn);
				if (m_u1rxnum - next_to_read == 1)
					m_u1rx = 0;
				NVIC_EnableIRQ(USART1_IRQn);
				return m_u1rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART1RXINTERRUPT */


	#ifdef USE_UART1TXINTERRUPT
		#ifndef BUFFER_U1TX_SIZE
			#warning "Vous devez définir la taille du buffer U1TX."
		#endif
		volatile Uint8 b1tx[BUFFER_U1TX_SIZE];
		volatile buffer_t buffer1tx = {b1tx,BUFFER_U1TX_SIZE,0,0,0};

		//Permet de savoir, venant de l'extérieur, si des données sont en cours d'envoi.
		bool_e IsNotEmpty_buffer_u1tx(void)
		{
			return IsNotEmpty_buffer(&buffer1tx);
		}

		//Permet de savoir, venant de l'extérieur, si le buffer est plein
		bool_e IsFull_buffer_u1tx(void)
		{
			return IsFull_buffer(&buffer1tx);
		}

		//Appelée par un printf
		int _write(int file, char *ptr, int len)
		{
			int i;

			switch (file)
			{
				case 0:
				case 1:
					for (i = 0; i < len; ++i)
					{
						UART1_putc(*ptr++);	//MODIFICATION APPORTEE AU CODE DE MICROCHIP...
					}
					return len;
					break;

				case 2:  //stderr = problème (entre autre trap_handler) donc pas de buffering
					for (i = 0; i < len; ++i)
					{
						while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
						USART_SendData(USART1, *ptr++);	//MODIFICATION APPORTEE AU CODE DE MICROCHIP...
					}
					return len;
					break;

				default:
					return 0;
					break;
			}
			return 0;
		}

		void UART1_putc(Uint8 c)
		{
			if(USART_GetFlagStatus(USART1, USART_IT_TXE))
				USART_SendData(USART1, c);
			else
			{
				//mise en buffer + activation IT U1TX.

				assert(buffer1tx.nb_datas < buffer1tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer1tx.nb_datas >= buffer1tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART1_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer1tx.nb_datas < buffer1tx.size)
				{
					buffer1tx.datas[buffer1tx.index_write] = c;
					buffer1tx.index_write = (buffer1tx.index_write>=buffer1tx.size-1)?0:(buffer1tx.index_write + 1);
					buffer1tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART1_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
			}
		}
	#else	/* def USE_UART1TXINTERRUPT */

		//Fonction blocante
		void UART1_putc(Uint8 mes)
		{
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			USART_SendData(USART1, mes);
		}
	#endif /* def USE_UART1TXINTERRUPT */


	void _ISR USART1_IRQHandler(void)
	{
		#ifdef USE_UART1RXINTERRUPT
			if(USART_GetITStatus(USART1, USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u1rxbuf[(m_u1rxnum%UART_RX_BUF_SIZE)]);

				while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
				{
					LED_UART=!LED_UART;

					#ifdef MODE_SIMULATION
					static Uint8 we_are_receiving_can_msg = FALSE;
					Uint8 c;
					c = USART_ReceiveData(USART1);
					if(we_are_receiving_can_msg == 0)
					{
						if(c==0x01)
							we_are_receiving_can_msg = 1;
					}
					else
						we_are_receiving_can_msg++;

					if(we_are_receiving_can_msg)
					{
						*(receiveddata++) = c;
						m_u1rxnum++;
						m_u1rx =1;
						/* pour eviter les comportements indésirables */
						if (receiveddata - m_u1rxbuf >= UART_RX_BUF_SIZE)
							receiveddata = m_u1rxbuf;
					}
					if(we_are_receiving_can_msg == 13)	//Dernier octet du message can (0x04...)
						we_are_receiving_can_msg = 0;
					#else
						*(receiveddata++) = USART_ReceiveData(USART1);;
						m_u1rxnum++;
						m_u1rx =1;
						/* pour eviter les comportements indésirables */
						if (receiveddata - m_u1rxbuf >= UART_RX_BUF_SIZE)
							receiveddata = m_u1rxbuf;
					#endif
				}
				USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			}
		#endif
		#ifdef USE_UART1TXINTERRUPT
			if(USART_GetITStatus(USART1, USART_IT_TXE)) {
				Uint8 c;

				//debufferiser.
				if(IsNotEmpty_buffer(&buffer1tx))
				{
					assert(buffer1tx.index_read < buffer1tx.size);
					//Critical section
					if(buffer1tx.nb_datas > (Uint8)0)
					{
						c = buffer1tx.datas[buffer1tx.index_read];
						buffer1tx.index_read = (buffer1tx.index_read>=buffer1tx.size-1)?0:(buffer1tx.index_read + 1);
						buffer1tx.nb_datas--;
						USART_SendData(USART1, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer1tx))
					USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			}
		#endif
	}

#endif /* def USE_UART1 */

#ifdef USE_UART2
	#ifdef USE_UART2TXINTERRUPT
		#ifndef BUFFER_U2TX_SIZE
			#warning "Vous devez définir la taille du buffer U2TX."
		#endif
		volatile Uint8 b2tx[BUFFER_U2TX_SIZE];
		volatile buffer_t buffer2tx = {b2tx,BUFFER_U2TX_SIZE,0,0,0};

		//Permet de savoir, venant de l'extérieur, si des données sont en cours d'envoi.
		bool_e IsNotEmpty_buffer_u2tx(void)
		{
			return IsNotEmpty_buffer(&buffer2tx);
		}

		//Permet de savoir, venant de l'extérieur, si le buffer est plein
		bool_e IsFull_buffer_u2tx(void)
		{
			return IsFull_buffer(&buffer2tx);
		}


		void UART2_putc(Uint8 c)
		{
			if(USART_GetFlagStatus(USART2, USART_IT_TXE))
				USART_SendData(USART2, c);
			else
			{
				//mise en buffer + activation IT U2TX.

				assert(buffer2tx.nb_datas < buffer2tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer2tx.nb_datas >= buffer2tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART2_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer2tx.nb_datas < buffer2tx.size)
				{
					buffer2tx.datas[buffer2tx.index_write] = c;
					buffer2tx.index_write = (buffer2tx.index_write>=buffer2tx.size-1)?0:(buffer2tx.index_write + 1);
					buffer2tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART2_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
			}
		}
	#else	/* def USE_UART2TXINTERRUPT */
		void UART2_putc(Uint8 mes)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, mes);
		}
	#endif
	void _ISR USART2_IRQHandler(void)
	{
		#ifdef USE_UART2RXINTERRUPT
			if(USART_GetITStatus(USART2, USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u2rxbuf[(m_u2rxnum%UART_RX_BUF_SIZE)]);

				while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
				{
					LED_UART=!LED_UART;
					*(receiveddata++) = USART_ReceiveData(USART2);
					m_u2rxnum++;
					m_u2rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u2rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u2rxbuf;
				}
				USART_ClearITPendingBit(USART2, USART_IT_RXNE);
				NVIC_ClearPendingIRQ(USART2_IRQn);
			}
		#endif	//def USE_UART2RXINTERRUPT
		#ifdef USE_UART2TXINTERRUPT
			if(USART_GetITStatus(USART2, USART_IT_TXE)) {
				Uint8 c;

				//debufferiser.
				if(IsNotEmpty_buffer(&buffer2tx))
				{
					assert(buffer2tx.index_read < buffer2tx.size);
					//Critical section
					if(buffer2tx.nb_datas > (Uint8)0)
					{
						c = buffer2tx.datas[buffer2tx.index_read];
						buffer2tx.index_read = (buffer2tx.index_read>=buffer2tx.size-1)?0:(buffer2tx.index_read + 1);
						buffer2tx.nb_datas--;
						USART_SendData(USART2, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer2tx))
					USART_ITConfig(USART2, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			}
		#endif
	}

	#ifdef USE_UART2RXINTERRUPT
		bool_e UART2_data_ready()
		{
			return m_u2rx;
		}

		Uint8 UART2_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u2rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART2_IRQn);
				if (m_u2rxnum - next_to_read == 1)
					m_u2rx = 0;
				NVIC_EnableIRQ(USART2_IRQn);
				return m_u2rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART2RXINTERRUPT */
#endif /* def USE_UART2 */

#ifdef USE_UART3
	#ifdef USE_UART3TXINTERRUPT
		#ifndef BUFFER_U3TX_SIZE
			#warning "Vous devez définir la taille du buffer U3TX."
		#endif
		volatile Uint8 b3tx[BUFFER_U3TX_SIZE];
		volatile buffer_t buffer3tx = {b3tx,BUFFER_U3TX_SIZE,0,0,0};

		//Permet de savoir, venant de l'extérieur, si des données sont en cours d'envoi.
		bool_e IsNotEmpty_buffer_u3tx(void)
		{
			return IsNotEmpty_buffer(&buffer3tx);
		}

		//Permet de savoir, venant de l'extérieur, si le buffer est plein
		bool_e IsFull_buffer_u3tx(void)
		{
			return IsFull_buffer(&buffer3tx);
		}


		void UART3_putc(Uint8 c)
		{
			if(USART_GetFlagStatus(USART3, USART_IT_TXE))
				USART_SendData(USART3, c);
			else
			{
				//mise en buffer + activation IT U3TX.

				assert(buffer3tx.nb_datas < buffer3tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer3tx.nb_datas >= buffer3tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART3_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer3tx.nb_datas < buffer3tx.size)
				{
					buffer3tx.datas[buffer3tx.index_write] = c;
					buffer3tx.index_write = (buffer3tx.index_write>=buffer3tx.size-1)?0:(buffer3tx.index_write + 1);
					buffer3tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART3_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
			}
		}
	#else	/* def USE_UART3TXINTERRUPT */
		void UART3_putc(Uint8 mes)
		{
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
			USART_SendData(USART3, mes);
		}
	#endif
	void _ISR USART3_IRQHandler(void)
	{
		#ifdef USE_UART3RXINTERRUPT
			if(USART_GetITStatus(USART3, USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u3rxbuf[(m_u3rxnum%UART_RX_BUF_SIZE)]);

				while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE))
				{
					LED_UART=!LED_UART;
					*(receiveddata++) = USART_ReceiveData(USART3);
					m_u3rxnum++;
					m_u3rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u3rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u3rxbuf;
				}
				USART_ClearITPendingBit(USART3, USART_IT_RXNE);
				NVIC_ClearPendingIRQ(USART3_IRQn);
			}
		#endif	//def USE_UART3RXINTERRUPT
		#ifdef USE_UART3TXINTERRUPT
			if(USART_GetITStatus(USART3, USART_IT_TXE)) {
				Uint8 c;

				//debufferiser.
				if(IsNotEmpty_buffer(&buffer3tx))
				{
					assert(buffer3tx.index_read < buffer3tx.size);
					//Critical section
					if(buffer3tx.nb_datas > (Uint8)0)
					{
						c = buffer3tx.datas[buffer3tx.index_read];
						buffer3tx.index_read = (buffer3tx.index_read>=buffer3tx.size-1)?0:(buffer3tx.index_read + 1);
						buffer3tx.nb_datas--;
						USART_SendData(USART3, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer3tx))
					USART_ITConfig(USART3, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			}
		#endif
	}

	#ifdef USE_UART3RXINTERRUPT
		bool_e UART3_data_ready()
		{
			return m_u3rx;
		}

		Uint8 UART3_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u3rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART3_IRQn);
				if (m_u3rxnum - next_to_read == 1)
					m_u3rx = 0;
				NVIC_EnableIRQ(USART3_IRQn);
				return m_u3rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART3RXINTERRUPT */
#endif /* def USE_UART3 */
