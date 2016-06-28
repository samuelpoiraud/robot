/*
 *	Club Robot ESEO 2008 - 2015
 *	Archi-Tech', Pierre & Guy, Holly & Wood
 *
 *	Fichier : QS_uart.c
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour
 *					interfacage rs232
 *	Auteur : Jacen / Alexis / Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100924
 */

#include "QS_uart.h"
#include "QS_ports.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_usart.h"

#include "../config/config_pin.h"

#ifndef USART_FOR_PRINTF
	#define USART_FOR_PRINTF	UART1_ID	//Default uart for output : 1.
#endif

#if USART_FOR_PRINTF == UART1_ID
	#define USART_FOR_PRINTF_PTR USART1
#elif USART_FOR_PRINTF == UART2_ID
	#define USART_FOR_PRINTF_PTR USART2
#elif USART_FOR_PRINTF == UART3_ID
	#define USART_FOR_PRINTF_PTR USART3
#elif USART_FOR_PRINTF == UART4_ID
	#define USART_FOR_PRINTF_PTR USART4
#elif USART_FOR_PRINTF == UART5_ID
	#define USART_FOR_PRINTF_PTR USART5
#elif USART_FOR_PRINTF == UART6_ID
	#define USART_FOR_PRINTF_PTR USART6
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
#ifndef UART4_BAUDRATE
	#define UART4_BAUDRATE 115200
#endif
#ifndef UART5_BAUDRATE
	#define UART5_BAUDRATE 115200
#endif
#ifndef UART6_BAUDRATE
	#define UART6_BAUDRATE 115200
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
#ifdef USE_UART4RXINTERRUPT
	static Uint8 m_u4rxbuf[UART_RX_BUF_SIZE];
	static volatile Uint32 m_u4rxnum;
	static volatile bool_e m_u4rx;	// message reçu sur uart4
#endif /* def USE_UART3RXINTERRUPT */
#ifdef USE_UART5RXINTERRUPT
	static Uint8 m_u5rxbuf[UART_RX_BUF_SIZE];
	static volatile Uint32 m_u5rxnum;
	static volatile bool_e m_u5rx;	// message reçu sur uart5
#endif /* def USE_UART3RXINTERRUPT */
#ifdef USE_UART6RXINTERRUPT
	static Uint8 m_u6rxbuf[UART_RX_BUF_SIZE];
	static volatile Uint32 m_u6rxnum;
	static volatile bool_e m_u6rx;	// message reçu sur uart6
#endif /* def USE_UART3RXINTERRUPT */
#if (defined USE_UART1TXINTERRUPT || defined USE_UART2TXINTERRUPT || defined USE_UART3TXINTERRUPT || defined USE_UART4TXINTERRUPT || defined USE_UART6TXINTERRUPT || defined USE_UART6TXINTERRUPT)
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

	// Fonctions privées
	static void UART_init(uart_id_e uart_id, uint32_t baudrate);

	// Variables globales
	static volatile bool_e initialized = FALSE;
	static UART_HandleTypeDef UART_HandleStructure[UART_ID_NB];
	const USART_TypeDef * instance_array[UART_ID_NB] = {USART1, USART2, USART3, UART4, UART5, USART6};
	const IRQn_Type nvic_irq_array[UART_ID_NB] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};

void UART_deInit(void){
#ifdef USE_UART1
	__HAL_RCC_USART1_CLK_DISABLE();
	HAL_UART_DeInit(&UART_HandleStructure[UART1_ID]);
#endif

#ifdef USE_UART2
	__HAL_RCC_USART2_CLK_DISABLE();
	HAL_UART_DeInit(&UART_HandleStructure[UART2_ID]);
#endif

#ifdef USE_UART3
	__HAL_RCC_USART3_CLK_DISABLE();
	HAL_UART_DeInit(&UART_HandleStructure[UART3_ID]);
#endif
#ifdef USE_UART4
	__HAL_RCC_USART4_CLK_DISABLE();
	HAL_UART_DeInit(&UART_HandleStructure[UART4_ID]);
#endif
#ifdef USE_UART5
	__HAL_RCC_USART5_CLK_DISABLE();
	HAL_UART_DeInit(&UART_HandleStructure[UART5_ID]);
#endif
#ifdef USE_UART6
	__HAL_RCC_USART6_CLK_DISABLE();
	HAL_UART_DeInit(&UART_HandleStructure[UART6_ID]);
#endif
	initialized = FALSE;
}

/*	fonction initialisant les uart choisis
	vitesse : 115200 bauds (par défaut)
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/
void UART_init_all(void)
{
	if(initialized)
		return;
	initialized = TRUE;

	PORTS_uarts_init();

#ifdef USE_UART1
	__HAL_RCC_USART1_CLK_ENABLE();
	UART_init(UART1_ID, UART1_BAUDRATE);

	#ifdef USE_UART1RXINTERRUPT
		m_u1rxnum = 0;
		m_u1rx = 0;
		/* Enable the USART1 Interrupt */
		HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_UART_Receive_IT(&UART_HandleStructure[UART1_ID],&m_u1rxbuf[m_u1rxnum],1);
	#endif
#endif


#ifdef USE_UART2
	__HAL_RCC_USART2_CLK_ENABLE();
	UART_init(UART2_ID, UART2_BAUDRATE);

	#ifdef USE_UART2RXINTERRUPT
		m_u2rxnum = 0;
		m_u2rx = 0;
		/* Enable the USART2 Interrupt */
		HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		HAL_UART_Receive_IT(&UART_HandleStructure[UART2_ID],&m_u2rxbuf[m_u2rxnum],1);
	#endif
#endif

#ifdef USE_UART3
	__HAL_RCC_USART3_CLK_ENABLE();
	UART_init(UART3_ID, UART3_BAUDRATE);

	#ifdef USE_UART3RXINTERRUPT
		m_u3rxnum = 0;
		m_u3rx = 0;
		/* Enable the USART3 Interrupt */
		HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		HAL_UART_Receive_IT(&UART_HandleStructure[UART3_ID],&m_u3rxbuf[m_u3rxnum],1);
	#endif
#endif
#ifdef USE_UART4
	__HAL_RCC_USART4_CLK_ENABLE();
	UART_init(UART4_ID, UART4_BAUDRATE);

	#ifdef USE_UART4RXINTERRUPT
		m_u4rxnum = 0;
		m_u4rx = 0;
		/* Enable the USART4 Interrupt */
		HAL_NVIC_SetPriority(USART4_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART4_IRQn);
		HAL_UART_Receive_IT(&UART_HandleStructure[UART4_ID],&m_u4rxbuf[m_u4rxnum],1);
	#endif
#endif
#ifdef USE_UART5
	__HAL_RCC_USART5_CLK_ENABLE();
	UART_init(UART5_ID, UART5_BAUDRATE);

	#ifdef USE_UART5RXINTERRUPT
		m_u5rxnum = 0;
		m_u5rx = 0;
		/* Enable the USART5 Interrupt */
		HAL_NVIC_SetPriority(USART5_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART5_IRQn);
		HAL_UART_Receive_IT(&UART_HandleStructure[UART5_ID],&m_u5rxbuf[m_u5rxnum],1);
	#endif
#endif
#ifdef USE_UART6
	__HAL_RCC_USART6_CLK_ENABLE();
	UART_init(UART6_ID, UART6_BAUDRATE);

	#ifdef USE_UART6RXINTERRUPT
		m_u6rxnum = 0;
		m_u6rx = 0;
		/* Enable the USART6 Interrupt */
		HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART6_IRQn);
		HAL_UART_Receive_IT(&UART_HandleStructure[UART6_ID],&m_u6rxbuf[m_u6rxnum],1);
	#endif
#endif
}

static void UART_init(uart_id_e uart_id, uint32_t baudrate)
{
	assert(baudrate > 1000);
	assert(uart_id < UART_ID_NB);

	UART_HandleStructure[uart_id].Instance = (USART_TypeDef*)instance_array[uart_id];
	UART_HandleStructure[uart_id].Init.BaudRate = baudrate;
	UART_HandleStructure[uart_id].Init.WordLength = UART_WORDLENGTH_8B;
	UART_HandleStructure[uart_id].Init.StopBits = UART_STOPBITS_1;
	UART_HandleStructure[uart_id].Init.Parity = UART_PARITY_NONE;
	UART_HandleStructure[uart_id].Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART_HandleStructure[uart_id].Init.Mode = UART_MODE_TX_RX;
	UART_HandleStructure[uart_id].Init.OverSampling = UART_OVERSAMPLING_8;

	/*On applique les parametres d'initialisation ci-dessus */
	HAL_UART_Init(&UART_HandleStructure[uart_id]);

	/*Activation de l'UART */
	__HAL_UART_ENABLE(&UART_HandleStructure[uart_id]);
}

void UART_set_baudrate(Uint8 uart_id, Uint32 baudrate) {

	assert(uart_id <= 6);

	switch(uart_id) {
		case 1:
			UART_init(UART1_ID, baudrate);
			break;
		case 2:
			UART_init(UART2_ID, baudrate);
			break;
		case 3:
			UART_init(UART3_ID, baudrate);
			break;
		case 4:
			UART_init(UART4_ID, baudrate);
			break;
		case 5:
			UART_init(UART5_ID, baudrate);
			break;
		case 6:
			UART_init(UART6_ID, baudrate);
			break;
		default:
			break;
	}
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
				#if USART_FOR_PRINTF == 1
					UART1_putc(*ptr++);
				#elif USART_FOR_PRINTF == 2
					UART2_putc(*ptr++);
				#elif	USART_FOR_PRINTF == 3
					UART3_putc(*ptr++);
				#elif USART_FOR_PRINTF == 4
					UART4_putc(*ptr++);
				#elif	 USART_FOR_PRINTF == 5
					UART5_putc(*ptr++);
				#elif	 USART_FOR_PRINTF == 6
					UART6_putc(*ptr++);
				#endif
			}
			return len;
			break;

		case 2:  //stderr = problème (entre autre trap_handler) donc pas de buffering
			for (i = 0; i < len; ++i)
			{
				while(__HAL_USART_GET_FLAG(&UART_HandleStructure[USART_FOR_PRINTF_PTR], USART_FLAG_TXE) == RESET);
				HAL_USART_Transmit_IT(&UART_HandleStructure[USART_FOR_PRINTF_PTR], *ptr++, 1);
			}
			return len;
			break;

		default:
			return 0;
			break;
	}
	return 0;
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
				HAL_NVIC_DisableIRQ(USART1_IRQn);
				if (m_u1rxnum - next_to_read == 1)
					m_u1rx = 0;
				HAL_NVIC_EnableIRQ(USART1_IRQn);
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



		void UART1_putc(Uint8 c)
		{
			static bool_e reentrance_detection = FALSE;

			if(reentrance_detection)
				return;					//Si printf en IT pendant un printf, on abandonne le caractère du printf en IT..

			reentrance_detection = TRUE;
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART1_ID], USART_IT_TXE))
				HAL_USART_Transmit_IT(&UART_HandleStructure[UART1_ID], c, 1);
			else
			{
				//mise en buffer + activation IT U1TX.

				assert(buffer1tx.nb_datas < buffer1tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer1tx.nb_datas >= buffer1tx.size);	//ON BLOQUE ICI*
				//Si vous êtes bloqués ici....... vérifiez qu'aucun printf n'est fait en IT...

				HAL_NVIC_DisableIRQ(USART1_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer1tx.nb_datas < buffer1tx.size)
				{
					buffer1tx.datas[buffer1tx.index_write] = c;
					buffer1tx.index_write = (buffer1tx.index_write>=buffer1tx.size-1)?0:(buffer1tx.index_write + 1);
					buffer1tx.nb_datas++;
				}

				HAL_NVIC_EnableIRQ(USART1_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				__HAL_USART_ENABLE_IT(__HAL_USART_GET_FLAG, USART_IT_TXE);
			}
			reentrance_detection = FALSE;
		}
	#else	/* def USE_UART1TXINTERRUPT */

		//Fonction blocante
		void UART1_putc(Uint8 mes)
		{
			while(__HAL_USART_GET_FLAG(__HAL_USART_GET_FLAG, USART_FLAG_TXE) == RESET);
			HAL_USART_Transmit(&UART_HandleStructure[UART1_ID], mes, 1, 100);
		}
	#endif /* def USE_UART1TXINTERRUPT */


	void _ISR USART1_IRQHandler(void){
		#ifdef USE_UART1RXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART1_ID], USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u1rxbuf[(m_u1rxnum%UART_RX_BUF_SIZE)]);

				while(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART1_ID], USART_FLAG_RXNE)){
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					#ifdef MODE_SIMULATION
					static Uint8 we_are_receiving_can_msg = FALSE;
					Uint8 c;
					HAL_UART_Receive_IT(&UART_HandleStructure[UART1_ID],&c, 1);
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
						HAL_UART_Receive_IT(&UART_HandleStructure[UART1_ID], receiveddata,1);
						receiveddata++;
						m_u1rxnum++;
						m_u1rx =1;
						/* pour eviter les comportements indésirables */
						if (receiveddata - m_u1rxbuf >= UART_RX_BUF_SIZE)
							receiveddata = m_u1rxbuf;
					#endif
				}
				__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART1_ID], USART_IT_RXNE);
			}
		#endif
		#ifdef USE_UART1TXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART1_ID], USART_IT_TXE)) {
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
						HAL_USART_Transmit_IT(&UART_HandleStructure[UART1_ID], c, 1);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer1tx))
					__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART1_ID], USART_IT_TXE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
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
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART2_ID], USART_IT_TXE))
				HAL_USART_Transmit_IT(&UART_HandleStructure[UART2_ID], c, 1);
			else
			{
				//mise en buffer + activation IT U2TX.

				assert(buffer2tx.nb_datas < buffer2tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer2tx.nb_datas >= buffer2tx.size);	//ON BLOQUE ICI

				AHL_NVIC_DisableIRQ(USART2_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer2tx.nb_datas < buffer2tx.size)
				{
					buffer2tx.datas[buffer2tx.index_write] = c;
					buffer2tx.index_write = (buffer2tx.index_write>=buffer2tx.size-1)?0:(buffer2tx.index_write + 1);
					buffer2tx.nb_datas++;
				}

				HAL_NVIC_EnableIRQ(USART2_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				__HAL_USART_ENABLE_IT(&UART_HandleStructure[UART2_ID],USART_IT_TXE);
			}
		}
	#else	/* def USE_UART2TXINTERRUPT */
		void UART2_putc(Uint8 mes)
		{
			while(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART2_ID], USART_FLAG_TXE) == RESET);
			HAL_USART_Transmit(&UART_HandleStructure[UART2_ID], mes, 1, 100);
		}
	#endif
	void _ISR USART2_IRQHandler(void)
	{
		#ifdef USE_UART2RXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART2_ID], USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u2rxbuf[(m_u2rxnum%UART_RX_BUF_SIZE)]);

				while(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART2_ID], USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					HAL_UART_Receive_IT(&UART_HandleStructure[UART2_ID], receiveddata,1);
					receiveddata++;
					m_u2rxnum++;
					m_u2rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u2rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u2rxbuf;
				}
				__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART2_ID], USART_IT_RXNE);
				HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
			}
		#endif	//def USE_UART2RXINTERRUPT
		#ifdef USE_UART2TXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART2_ID], USART_IT_TXE)) {
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
						HAL_USART_Transmit_IT(&UART_HandleStructure[UART2_ID], &c, 1);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer2tx))
					__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART2_ID], USART_IT_TXE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
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
				HAL_NVIC_DisableIRQ(USART2_IRQn);
				if (m_u2rxnum - next_to_read == 1)
					m_u2rx = 0;
				HAL_NVIC_EnableIRQ(USART2_IRQn);
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
			if(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART3_ID], USART_IT_TXE))
				HAL_USART_Transmit_IT(&UART_HandleStructure[UART3_ID], &c, 1);
			else
			{
				//mise en buffer + activation IT U3TX.

				assert(buffer3tx.nb_datas < buffer3tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer3tx.nb_datas >= buffer3tx.size);	//ON BLOQUE ICI

				HAL_NVIC_DisableIRQ(USART3_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer3tx.nb_datas < buffer3tx.size)
				{
					buffer3tx.datas[buffer3tx.index_write] = c;
					buffer3tx.index_write = (buffer3tx.index_write>=buffer3tx.size-1)?0:(buffer3tx.index_write + 1);
					buffer3tx.nb_datas++;
				}

				HAL_NVIC_EnableIRQ(USART3_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				__HAL_USART_ENABLE_IT(&UART_HandleStructure[UART3_ID], USART_IT_TXE);
			}
		}
	#else	/* def USE_UART3TXINTERRUPT */
		void UART3_putc(Uint8 mes)
		{
			while(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART3_ID], USART_FLAG_TXE) == RESET);
			HAL_USART_Transmit(&UART_HandleStructure[UART3_ID], &mes, 1, 100);
		}
	#endif
	void _ISR USART3_IRQHandler(void)
	{
		#ifdef USE_UART3RXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART3_ID], USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u3rxbuf[(m_u3rxnum%UART_RX_BUF_SIZE)]);

				while(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART3_ID], USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					HAL_UART_Receive_IT(&UART_HandleStructure[UART3_ID], receiveddata,1);
					receiveddata++;
					m_u3rxnum++;
					m_u3rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u3rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u3rxbuf;
				}
				__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART3_ID], USART_IT_RXNE);
				HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
			}
		#endif	//def USE_UART3RXINTERRUPT
		#ifdef USE_UART3TXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART3_ID], USART_IT_TXE)) {
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
						HAL_USART_Transmit_IT(&UART_HandleStructure[UART3_ID], &c, 1);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer3tx))
					__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART3_ID], USART_IT_TXE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
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
				HAL_NVIC_DisableIRQ(USART3_IRQn);
				if (m_u3rxnum - next_to_read == 1)
					m_u3rx = 0;
				HAL_NVIC_EnableIRQ(USART3_IRQn);
				return m_u3rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART3RXINTERRUPT */
#endif /* def USE_UART3 */


#ifdef USE_UART4
	#ifdef USE_UART4TXINTERRUPT
		#ifndef BUFFER_U4TX_SIZE
			#warning "Vous devez définir la taille du buffer U4TX."
		#endif
		volatile Uint8 b4tx[BUFFER_U4TX_SIZE];
		volatile buffer_t buffer4tx = {b4tx,BUFFER_U4TX_SIZE,0,0,0};

		//Permet de savoir, venant de l'extérieur, si des données sont en cours d'envoi.
		bool_e IsNotEmpty_buffer_u4tx(void)
		{
			return IsNotEmpty_buffer(&buffer4tx);
		}

		//Permet de savoir, venant de l'extérieur, si le buffer est plein
		bool_e IsFull_buffer_u4tx(void)
		{
			return IsFull_buffer(&buffer4tx);
		}


		void UART4_putc(Uint8 c)
		{
			if(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART4_ID], USART_IT_TXE))
				HAL_USART_Transmit_IT(&UART_HandleStructure[UART4_ID], &c, 1);
			else
			{
				//mise en buffer + activation IT U4TX.

				assert(buffer4tx.nb_datas < buffer4tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer4tx.nb_datas >= buffer4tx.size);	//ON BLOQUE ICI

				HAL_NVIC_DisableIRQ(UART4_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer4tx.nb_datas < buffer4tx.size)
				{
					buffer4tx.datas[buffer4tx.index_write] = c;
					buffer4tx.index_write = (buffer4tx.index_write>=buffer4tx.size-1)?0:(buffer4tx.index_write + 1);
					buffer4tx.nb_datas++;
				}

				HAL_NVIC_EnableIRQ(UART4_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				__HAL_USART_ENABLE_IT(&UART_HandleStructure[UART4_ID], USART_IT_TXE);
			}
		}
	#else	/* def USE_UART4TXINTERRUPT */
		void UART4_putc(Uint8 mes)
		{
			while(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART4_ID], USART_FLAG_TXE) == RESET);
			HAL_USART_Transmit(&UART_HandleStructure[UART4_ID], &mes, 1, 100);
		}
	#endif
	void _ISR UART4_IRQHandler(void)
	{
		#ifdef USE_UART4RXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART4_ID], USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u4rxbuf[(m_u4rxnum%UART_RX_BUF_SIZE)]);

				while((__HAL_USART_GET_FLAG(&UART_HandleStructure[UART4_ID], USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					HAL_UART_Receive_IT(&UART_HandleStructure[UART4_ID], receiveddata,1);
					receiveddata++;
					m_u4rxnum++;
					m_u4rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u4rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u4rxbuf;
				}
				__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART4_ID], USART_IT_RXNE);
				HAL_NVIC_ClearPendingIRQ(UART4_IRQn);
			}
		#endif	//def USE_UART4RXINTERRUPT
		#ifdef USE_UART4TXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART4_ID], USART_IT_TXE)) {
				Uint8 c;

				//debufferiser.
				if(IsNotEmpty_buffer(&buffer4tx))
				{
					assert(buffer4tx.index_read < buffer4tx.size);
					//Critical section
					if(buffer4tx.nb_datas > (Uint8)0)
					{
						c = buffer4tx.datas[buffer4tx.index_read];
						buffer4tx.index_read = (buffer4tx.index_read>=buffer4tx.size-1)?0:(buffer4tx.index_read + 1);
						buffer4tx.nb_datas--;
						HAL_USART_Transmit_IT(&UART_HandleStructure[UART4_ID], &c, 1);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer4tx))
					__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART4_ID], USART_IT_TXE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			}
		#endif
	}

	#ifdef USE_UART4RXINTERRUPT
		bool_e UART4_data_ready()
		{
			return m_u4rx;
		}

		Uint8 UART4_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u4rxnum > next_to_read)
			{
				HAL_NVIC_DisableIRQ(UART4_IRQn);
				if (m_u4rxnum - next_to_read == 1)
					m_u4rx = 0;
				HAL_NVIC_EnableIRQ(UART4_IRQn);
				return m_u4rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART4RXINTERRUPT */
#endif /* def USE_UART4 */

#ifdef USE_UART5
	#ifdef USE_UART5TXINTERRUPT
		#ifndef BUFFER_U5TX_SIZE
			#warning "Vous devez définir la taille du buffer U5TX."
		#endif
		volatile Uint8 b5tx[BUFFER_U5TX_SIZE];
		volatile buffer_t buffer5tx = {b5tx,BUFFER_U5TX_SIZE,0,0,0};

		//Permet de savoir, venant de l'extérieur, si des données sont en cours d'envoi.
		bool_e IsNotEmpty_buffer_u5tx(void)
		{
			return IsNotEmpty_buffer(&buffer5tx);
		}

		//Permet de savoir, venant de l'extérieur, si le buffer est plein
		bool_e IsFull_buffer_u5tx(void)
		{
			return IsFull_buffer(&buffer5tx);
		}


		void UART5_putc(Uint8 c)
		{
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART5_ID], USART_IT_TXE))
				HAL_USART_Transmit_IT(&UART_HandleStructure[UART5_ID], &c, 1);
			else
			{
				//mise en buffer + activation IT U5TX.

				assert(buffer5tx.nb_datas < buffer5tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer5tx.nb_datas >= buffer5tx.size);	//ON BLOQUE ICI

				HAL_NVIC_DisableIRQ(USART5_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer5tx.nb_datas < buffer5tx.size)
				{
					buffer5tx.datas[buffer5tx.index_write] = c;
					buffer5tx.index_write = (buffer5tx.index_write>=buffer5tx.size-1)?0:(buffer5tx.index_write + 1);
					buffer5tx.nb_datas++;
				}

				HAL_NVIC_EnableIRQ(USART5_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				__HAL_USART_ENABLE_IT(&UART_HandleStructure[UART5_ID], USART_IT_TXE);
			}
		}
	#else	/* def USE_UART5TXINTERRUPT */
		void UART5_putc(Uint8 mes)
		{
			while(__HAL_USART_GET_FLAG(&UART_HandleStructure[UART5_ID], USART_FLAG_TXE) == RESET);
			HAL_USART_Transmit(&UART_HandleStructure[UART5_ID], &mes, 1, 100);
		}
	#endif
	void _ISR UART5_IRQHandler(void)
	{
		#ifdef USE_UART5RXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART5_ID], USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u5rxbuf[(m_u5rxnum%UART_RX_BUF_SIZE)]);

				while(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART5_ID], USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					HAL_UART_Receive_IT(&UART_HandleStructure[UART5_ID], receiveddata, 1);
					receiveddata++;
					m_u5rxnum++;
					m_u5rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u5rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u5rxbuf;
				}
				__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART5_ID], USART_IT_RXNE);
				HAL_NVIC_ClearPendingIRQ(UART5_IRQn);
			}
		#endif	//def USE_UART5RXINTERRUPT
		#ifdef USE_UART5TXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART5_ID], USART_IT_TXE)) {
				Uint8 c;

				//debufferiser.
				if(IsNotEmpty_buffer(&buffer5tx))
				{
					assert(buffer5tx.index_read < buffer5tx.size);
					//Critical section
					if(buffer5tx.nb_datas > (Uint8)0)
					{
						c = buffer5tx.datas[buffer5tx.index_read];
						buffer5tx.index_read = (buffer5tx.index_read>=buffer5tx.size-1)?0:(buffer5tx.index_read + 1);
						buffer5tx.nb_datas--;
						HAL_USART_Transmit_IT(&UART_HandleStructure[UART5_ID], &c, 1);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer5tx))
					__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART5_ID], USART_IT_TXE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			}
		#endif
	}

	#ifdef USE_UART5RXINTERRUPT
		bool_e UART5_data_ready()
		{
			return m_u5rx;
		}

		Uint8 UART5_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u5rxnum > next_to_read)
			{
				HAL_NVIC_DisableIRQ(UART5_IRQn);
				if (m_u5rxnum - next_to_read == 1)
					m_u5rx = 0;
				HAL_NVIC_EnableIRQ(UART5_IRQn);
				return m_u5rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART5RXINTERRUPT */
#endif /* def USE_UART5 */

#ifdef USE_UART6
	#ifdef USE_UART6TXINTERRUPT
		#ifndef BUFFER_U6TX_SIZE
			#warning "Vous devez définir la taille du buffer U6TX."
		#endif
		volatile Uint8 b6tx[BUFFER_U6TX_SIZE];
		volatile buffer_t buffer6tx = {b6tx,BUFFER_U6TX_SIZE,0,0,0};

		//Permet de savoir, venant de l'extérieur, si des données sont en cours d'envoi.
		bool_e IsNotEmpty_buffer_u6tx(void)
		{
			return IsNotEmpty_buffer(&buffer6tx);
		}

		//Permet de savoir, venant de l'extérieur, si le buffer est plein
		bool_e IsFull_buffer_u6tx(void)
		{
			return IsFull_buffer(&buffer6tx);
		}


		void UART6_putc(Uint8 c)
		{
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART6_ID], USART_IT_TXE))
				HAL_USART_Transmit_IT(&UART_HandleStructure[UART6_ID], &c, 1);
			else
			{
				//mise en buffer + activation IT U6TX.

				assert(buffer6tx.nb_datas < buffer6tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer6tx.nb_datas >= buffer6tx.size);	//ON BLOQUE ICI

				HAL_NVIC_DisableIRQ(USART6_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer6tx.nb_datas < buffer6tx.size)
				{
					buffer6tx.datas[buffer6tx.index_write] = c;
					buffer6tx.index_write = (buffer6tx.index_write>=buffer6tx.size-1)?0:(buffer6tx.index_write + 1);
					buffer6tx.nb_datas++;
				}

				HAL_NVIC_EnableIRQ(USART6_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				__HAL_USART_ENABLE_IT(&UART_HandleStructure[UART6_ID], USART_IT_TXE);
			}
		}
	#else	/* def USE_UART6TXINTERRUPT */
		void UART6_putc(Uint8 mes)
		{
			while(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART6_ID], USART_FLAG_TXE) == RESET);
			HAL_USART_Transmit(&UART_HandleStructure[UART6_ID], &mes, 1, 100);
		}
	#endif
	void _ISR USART6_IRQHandler(void)
	{
		#ifdef USE_UART6RXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART6_ID], USART_IT_RXNE)) {
				Uint8 * receiveddata = &(m_u6rxbuf[(m_u6rxnum%UART_RX_BUF_SIZE)]);

				while(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART6_ID], USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					HAL_UART_Receive_IT(&UART_HandleStructure[UART6_ID], receiveddata, 1);
					receiveddata++;
					m_u6rxnum++;
					m_u6rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u6rxbuf >= UART_RX_BUF_SIZE)
						receiveddata = m_u6rxbuf;
				}
				__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART6_ID], USART_IT_RXNE);
				HAL_NVIC_ClearPendingIRQ(USART6_IRQn);
			}
		#endif	//def USE_UART6RXINTERRUPT
		#ifdef USE_UART6TXINTERRUPT
			if(__HAL_USART_GET_IT_SOURCE(&UART_HandleStructure[UART6_ID], USART_IT_TXE)) {
				Uint8 c;

				//debufferiser.
				if(IsNotEmpty_buffer(&buffer6tx))
				{
					assert(buffer6tx.index_read < buffer6tx.size);
					//Critical section
					if(buffer6tx.nb_datas > (Uint8)0)
					{
						c = buffer6tx.datas[buffer6tx.index_read];
						buffer6tx.index_read = (buffer6tx.index_read>=buffer6tx.size-1)?0:(buffer6tx.index_read + 1);
						buffer6tx.nb_datas--;
						HAL_USART_Transmit_IT(&UART_HandleStructure[UART6_ID], &c, 1);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer6tx))
					__HAL_USART_DISABLE_IT(&UART_HandleStructure[UART6_ID], USART_IT_TXE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			}
		#endif
	}

	#ifdef USE_UART6RXINTERRUPT
		bool_e UART6_data_ready()
		{
			return m_u6rx;
		}

		Uint8 UART6_get_next_msg()
		{
			static Uint32 next_to_read =0;
			if (m_u6rxnum > next_to_read)
			{
				HAL_NVIC_DisableIRQ(USART6_IRQn);
				if (m_u6rxnum - next_to_read == 1)
					m_u6rx = 0;
				HAL_NVIC_EnableIRQ(USART6_IRQn);
				return m_u6rxbuf[((next_to_read++) % UART_RX_BUF_SIZE)];
			}
			else
				return 0;
		}
	#endif /* def USE_UART6RXINTERRUPT */
#endif /* def USE_UART6 */
