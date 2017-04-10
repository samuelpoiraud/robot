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
#include "stm32f4xx_usart.h"

#include "../config/config_pin.h"

#ifndef USART_FOR_PRINTF
	#define USART_FOR_PRINTF		1	//Default uart for output : 1.
#endif

#if USART_FOR_PRINTF == 1
	#define USART_FOR_PRINTF_PTR USART1
#elif USART_FOR_PRINTF == 2
	#define USART_FOR_PRINTF_PTR USART2
#elif	USART_FOR_PRINTF == 3
	#define USART_FOR_PRINTF_PTR USART3
#elif USART_FOR_PRINTF == 4
	#define USART_FOR_PRINTF_PTR USART4
#elif	 USART_FOR_PRINTF == 5
	#define USART_FOR_PRINTF_PTR USART5
#elif	 USART_FOR_PRINTF == 6
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
#ifdef USE_UART1_RX_BUFFER
	#ifndef UART1_RX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART1 RX."
	#endif
	static Uint8 m_u1rxbuf[UART1_RX_BUFFER_SIZE];
	static volatile Uint32 m_u1rxnum;
	static volatile bool_e m_u1rx;	// message reçu sur uart1
#endif /* def USE_UART1_RX_BUFFER */

#ifdef USE_UART2_RX_BUFFER
	#ifndef UART2_RX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART2 RX."
	#endif
	static Uint8 m_u2rxbuf[UART2_RX_BUFFER_SIZE];
	static volatile Uint32 m_u2rxnum;
	static volatile bool_e m_u2rx;	// message reçu sur uart2
#endif /* def USE_UART2_RX_BUFFER */

#ifdef USE_UART3_RX_BUFFER
	#ifndef UART3_RX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART3 RX."
	#endif
	static Uint8 m_u3rxbuf[UART3_RX_BUFFER_SIZE];
	static volatile Uint32 m_u3rxnum;
	static volatile bool_e m_u3rx;	// message reçu sur uart3
#endif /* def USE_UART3_RX_BUFFER */

#ifdef USE_UART4_RX_BUFFER
	#ifndef UART4_RX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART4 RX."
	#endif
	static Uint8 m_u4rxbuf[UART4_RX_BUFFER_SIZE];
	static volatile Uint32 m_u4rxnum;
	static volatile bool_e m_u4rx;	// message reçu sur uart4
#endif /* def USE_UART4_RX_BUFFER */

#ifdef USE_UART5_RX_BUFFER
	#ifndef UART5_RX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART5 RX."
	#endif
	static Uint8 m_u5rxbuf[UART5_RX_BUFFER_SIZE];
	static volatile Uint32 m_u5rxnum;
	static volatile bool_e m_u5rx;	// message reçu sur uart5
#endif /* def USE_UART5_RX_BUFFER */

#ifdef USE_UART6_RX_BUFFER
	#ifndef UART6_RX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART6 RX."
	#endif
	static Uint8 m_u6rxbuf[UART6_RX_BUFFER_SIZE];
	static volatile Uint32 m_u6rxnum;
	static volatile bool_e m_u6rx;	// message reçu sur uart6
#endif /* def USE_UART6_RX_BUFFER */

#if (defined USE_UART1_TX_BUFFER || defined USE_UART2_TX_BUFFER || defined USE_UART3_TX_BUFFER || defined USE_UART4_TX_BUFFER || defined USE_UART5_TX_BUFFER || defined USE_UART6_TX_BUFFER)
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

#ifdef USE_UART1_TX_BUFFER
	#ifndef UART1_TX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART1 TX."
	#endif
	static volatile Uint8 b1tx[UART1_TX_BUFFER_SIZE];
	static volatile buffer_t buffer1tx = {b1tx, UART1_TX_BUFFER_SIZE, 0, 0, 0};
#endif

#ifdef USE_UART2_TX_BUFFER
	#ifndef UART2_TX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART2 TX."
	#endif
	static volatile Uint8 b2tx[UART2_TX_BUFFER_SIZE];
	static volatile buffer_t buffer2tx = {b2tx, UART2_TX_BUFFER_SIZE, 0, 0, 0};
#endif

#ifdef USE_UART3_TX_BUFFER
	#ifndef UART3_TX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART3 TX."
	#endif
	static volatile Uint8 b3tx[UART3_TX_BUFFER_SIZE];
	static volatile buffer_t buffer3tx = {b3tx, UART3_TX_BUFFER_SIZE, 0, 0, 0};
#endif

#ifdef USE_UART4_TX_BUFFER
	#ifndef UART4_TX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART1 TX."
	#endif
	static volatile Uint8 b4tx[UART4_TX_BUFFER_SIZE];
	static volatile buffer_t buffer4tx = {b4tx, UART4_TX_BUFFER_SIZE, 0, 0, 0};
#endif

#ifdef USE_UART5_TX_BUFFER
	#ifndef UART5_TX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART5 TX."
	#endif
	static volatile Uint8 b5tx[UART5_TX_BUFFER_SIZE];
	static volatile buffer_t buffer5tx = {b5tx, UART5_TX_BUFFER_SIZE, 0, 0, 0};
#endif

#ifdef USE_UART6_TX_BUFFER
	#ifndef UART6_TX_BUFFER_SIZE
		#error "Vous devez définir la taille du buffer UART6 TX."
	#endif
	static volatile Uint8 b6tx[UART6_TX_BUFFER_SIZE];
	static volatile buffer_t buffer6tx = {b6tx, UART6_TX_BUFFER_SIZE, 0, 0, 0};
#endif

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static volatile UART_dataReceivedFunctionPtr listennerDataReceived[6] = {NULL};
static volatile bool_e initialized = FALSE;

void UART_deInit(void){
	#ifdef USE_UART1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
	#endif
	#ifdef USE_UART2
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
	#endif
	#ifdef USE_UART3
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
	#endif
	#ifdef USE_UART4
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, DISABLE);
	#endif
	#ifdef USE_UART5
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, DISABLE);
	#endif
	#ifdef USE_UART6
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, DISABLE);
	#endif
	initialized = FALSE;
}

void UART_init(void){
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

		#ifdef USE_UART1_RX_BUFFER
			m_u1rxnum = 0;
			m_u1rx = 0;
		#endif

		#if defined(USE_UART1_RX_BUFFER) || defined(USE_UART1_RX_INTERRUPT)
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

		#ifdef USE_UART2_RX_BUFFER
			m_u2rxnum = 0;
			m_u2rx = 0;
		#endif

		#if defined(USE_UART2_RX_BUFFER) || defined(USE_UART2_RX_INTERRUPT)
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

		#ifdef USE_UART3_RX_BUFFER
			m_u3rxnum = 0;
			m_u3rx = 0;
		#endif

		#if defined(USE_UART3_RX_BUFFER) || defined(USE_UART3_RX_INTERRUPT)
			/* Enable the USART1 Interrupt */
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
			NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
			NVIC_Init(&NVIC_InitStructure);
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		#endif

		/* Enable USART */
		USART_Cmd(USART3, ENABLE);
	#endif

	#ifdef USE_UART4
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		USART_OverSampling8Cmd(UART4, ENABLE);
		UART_set_baudrate(4, UART4_BAUDRATE);

		#ifdef USE_UART4_RX_BUFFER
			m_u4rxnum = 0;
			m_u4rx = 0;
		#endif

		#if defined(USE_UART4_RX_BUFFER) || defined(USE_UART4_RX_INTERRUPT)
			/* Enable the USART1 Interrupt */
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
			NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
			NVIC_Init(&NVIC_InitStructure);
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
		#endif

		/* Enable USART */
		USART_Cmd(UART4, ENABLE);
	#endif

	#ifdef USE_UART5
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		USART_OverSampling8Cmd(UART5, ENABLE);
		UART_set_baudrate(5, UART5_BAUDRATE);

		#ifdef USE_UART5_RX_BUFFER
			m_u5rxnum = 0;
			m_u5rx = 0;
		#endif

		#if defined(USE_UART5_RX_BUFFER) || defined(USE_UART5_RX_INTERRUPT)
			/* Enable the USART1 Interrupt */
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
			NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
			NVIC_Init(&NVIC_InitStructure);
			USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
		#endif

		/* Enable USART */
		USART_Cmd(UART5, ENABLE);
	#endif

	#ifdef USE_UART6
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		USART_OverSampling8Cmd(USART6, ENABLE);
		UART_set_baudrate(6, UART6_BAUDRATE);

		#ifdef USE_UART6_RX_BUFFER
			m_u6rxnum = 0;
			m_u6rx = 0;
		#endif

		#if defined(USE_UART6_RX_BUFFER) || defined(USE_UART6_RX_INTERRUPT)
			/* Enable the USART1 Interrupt */
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
			NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
			NVIC_Init(&NVIC_InitStructure);
			USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
		#endif

		/* Enable USART */
		USART_Cmd(USART6, ENABLE);
	#endif
}

void UART_set_baudrate(Uint8 uart_id, Uint32 baudrate) {
	USART_InitTypeDef USART_InitStructure;

	assert(uart_id <= 6);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		switch(uart_id) {
		case 1:
			#ifdef USE_UART1_FLOWCONTROL
				USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
			#endif
			USART_Init(USART1, &USART_InitStructure);
			break;

		case 2:
			#ifdef USE_UART2_FLOWCONTROL
				USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
			#endif
			USART_Init(USART2, &USART_InitStructure);
			break;

		case 3:
			#ifdef USE_UART3_FLOWCONTROL
				USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
			#endif
			USART_Init(USART3, &USART_InitStructure);
			break;

		case 4:
			#ifdef USE_UART4_FLOWCONTROL
				USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
			#endif
			USART_Init(UART4, &USART_InitStructure);
			break;

		case 5:
			#ifdef USE_UART5_FLOWCONTROL
				USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
			#endif
			USART_Init(UART5, &USART_InitStructure);
			break;

		case 6:
			#ifdef USE_UART6_FLOWCONTROL
				USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
			#endif
			USART_Init(USART6, &USART_InitStructure);
			break;

		default:
			break;
	}
}

void UART_setListenner(Uint8 uart_id, UART_dataReceivedFunctionPtr function){
	assert(uart_id > 0 && uart_id <= 6);

	listennerDataReceived[uart_id-1] = function;
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
				while(USART_GetFlagStatus(USART_FOR_PRINTF_PTR, USART_FLAG_TXE) == RESET);
				USART_SendData(USART_FOR_PRINTF_PTR, *ptr++);
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

	#ifdef USE_UART1_RX_BUFFER
		bool_e UART1_data_ready(void){
			return m_u1rx;
		}

		Uint8 UART1_get_next_msg(void){
			static Uint32 next_to_read =0;
			if (m_u1rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART1_IRQn);
				if (m_u1rxnum - next_to_read == 1)
					m_u1rx = 0;
				NVIC_EnableIRQ(USART1_IRQn);
				return m_u1rxbuf[((next_to_read++) % UART1_RX_BUFFER_SIZE)];
			}
			else
				return 0;
		}
	#endif

	#ifdef USE_UART1_TX_BUFFER
		void UART1_putc(Uint8 data)
		{
			static bool_e reentrance_detection = FALSE;

			if(reentrance_detection)
				return;					//Si printf en IT pendant un printf, on abandonne le caractère du printf en IT..

			reentrance_detection = TRUE;
			if(USART_GetITStatus(USART1, USART_IT_TXE))
				USART_SendData(USART1, data);
			else
			{
				//mise en buffer + activation IT U1TX.

				assert(buffer1tx.nb_datas < buffer1tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer1tx.nb_datas >= buffer1tx.size);	//ON BLOQUE ICI*
				//Si vous êtes bloqués ici....... vérifiez qu'aucun printf n'est fait en IT...

				NVIC_DisableIRQ(USART1_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer1tx.nb_datas < buffer1tx.size)
				{
					buffer1tx.datas[buffer1tx.index_write] = data;
					buffer1tx.index_write = (buffer1tx.index_write>=buffer1tx.size-1)?0:(buffer1tx.index_write + 1);
					buffer1tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART1_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
			}
			reentrance_detection = FALSE;
		}
	#else
		void UART1_putc(Uint8 data)
		{
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			USART_SendData(USART1, data);
		}
	#endif

	void _ISR USART1_IRQHandler(void){

		if(USART_GetITStatus(USART1, USART_IT_RXNE)){
			#ifdef USE_UART1_RX_BUFFER
				Uint8 * receiveddata = &(m_u1rxbuf[(m_u1rxnum % UART1_RX_BUFFER_SIZE)]);

				while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)){
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					*(receiveddata++) = USART_ReceiveData(USART1);
					m_u1rxnum++;
					m_u1rx =1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u1rxbuf >= UART1_RX_BUFFER_SIZE)
						receiveddata = m_u1rxbuf;
				}

			#elif defined(USE_UART1_RX_INTERRUPT)

				if(listennerDataReceived[0] != NULL)
					listennerDataReceived[0](USART_ReceiveData(USART1));
				else
					USART_ReceiveData(USART1);

			#endif

			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		}

		if(USART_GetITStatus(USART1, USART_IT_TXE)){
			#ifdef USE_UART1_TX_BUFFER
				//debufferiser.
				if(IsNotEmpty_buffer(&buffer1tx))
				{
					assert(buffer1tx.index_read < buffer1tx.size);
					//Critical section
					if(buffer1tx.nb_datas > (Uint8)0)
					{
						Uint8 c = buffer1tx.datas[buffer1tx.index_read];
						buffer1tx.index_read = (buffer1tx.index_read>=buffer1tx.size-1)?0:(buffer1tx.index_read + 1);
						buffer1tx.nb_datas--;
						USART_SendData(USART1, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer1tx))
					USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			#endif
		}

		NVIC_ClearPendingIRQ(USART1_IRQn);
	}

#endif /* def USE_UART1 */

#ifdef USE_UART2

	#ifdef USE_UART2_RX_BUFFER
		bool_e UART2_data_ready(void){
			return m_u2rx;
		}

		Uint8 UART2_get_next_msg(void){
			static Uint32 next_to_read =0;
			if (m_u2rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART2_IRQn);
				if (m_u2rxnum - next_to_read == 1)
					m_u2rx = 0;
				NVIC_EnableIRQ(USART2_IRQn);
				return m_u2rxbuf[((next_to_read++) % UART2_RX_BUFFER_SIZE)];
			}
			else
				return 0;
		}
	#endif

	#ifdef USE_UART2_TX_BUFFER
		void UART2_putc(Uint8 data)
		{
			if(USART_GetITStatus(USART2, USART_IT_TXE))
				USART_SendData(USART2, data);
			else
			{
				//mise en buffer + activation IT U2TX.

				assert(buffer2tx.nb_datas < buffer2tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer2tx.nb_datas >= buffer2tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART2_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer2tx.nb_datas < buffer2tx.size)
				{
					buffer2tx.datas[buffer2tx.index_write] = data;
					buffer2tx.index_write = (buffer2tx.index_write>=buffer2tx.size-1)?0:(buffer2tx.index_write + 1);
					buffer2tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART2_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
			}
		}
	#else
		void UART2_putc(Uint8 data)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, data);
		}
	#endif

	void _ISR USART2_IRQHandler(void){

		if(USART_GetITStatus(USART2, USART_IT_RXNE)){
			#ifdef USE_UART2_RX_BUFFER
				Uint8 * receiveddata = &(m_u2rxbuf[(m_u2rxnum % UART2_RX_BUFFER_SIZE)]);

				while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					*(receiveddata++) = USART_ReceiveData(USART2);
					m_u2rxnum++;
					m_u2rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u2rxbuf >= UART2_RX_BUFFER_SIZE)
						receiveddata = m_u2rxbuf;
				}

			#elif defined(USE_UART2_RX_INTERRUPT)

				if(listennerDataReceived[1] != NULL)
					listennerDataReceived[1](USART_ReceiveData(USART2));
				else
					USART_ReceiveData(USART2);

			#endif

			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		}

		if(USART_GetITStatus(USART2, USART_IT_TXE)){
			#ifdef USE_UART2_TX_BUFFER
				//debufferiser.
				if(IsNotEmpty_buffer(&buffer2tx))
				{
					assert(buffer2tx.index_read < buffer2tx.size);
					//Critical section
					if(buffer2tx.nb_datas > (Uint8)0)
					{
						Uint8 c = buffer2tx.datas[buffer2tx.index_read];
						buffer2tx.index_read = (buffer2tx.index_read>=buffer2tx.size-1)?0:(buffer2tx.index_read + 1);
						buffer2tx.nb_datas--;
						USART_SendData(USART2, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer2tx))
					USART_ITConfig(USART2, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			#endif
		}

		NVIC_ClearPendingIRQ(USART2_IRQn);
	}

#endif /* def USE_UART2 */

#ifdef USE_UART3

	#ifdef USE_UART3_RX_BUFFER
		bool_e UART3_data_ready(void){
			return m_u3rx;
		}

		Uint8 UART3_get_next_msg(void){
			static Uint32 next_to_read =0;
			if (m_u3rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART3_IRQn);
				if (m_u3rxnum - next_to_read == 1)
					m_u3rx = 0;
				NVIC_EnableIRQ(USART3_IRQn);
				return m_u3rxbuf[((next_to_read++) % UART3_RX_BUFFER_SIZE)];
			}
			else
				return 0;
		}
	#endif

	#ifdef USE_UART3_TX_BUFFER
		void UART3_putc(Uint8 data)
		{
			if(USART_GetITStatus(USART3, USART_IT_TXE))
				USART_SendData(USART3, data);
			else
			{
				//mise en buffer + activation IT U3TX.

				assert(buffer3tx.nb_datas < buffer3tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer3tx.nb_datas >= buffer3tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART3_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer3tx.nb_datas < buffer3tx.size)
				{
					buffer3tx.datas[buffer3tx.index_write] = data;
					buffer3tx.index_write = (buffer3tx.index_write>=buffer3tx.size-1)?0:(buffer3tx.index_write + 1);
					buffer3tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART3_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
			}
		}
	#else
		void UART3_putc(Uint8 data)
		{
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
			USART_SendData(USART3, data);
		}
	#endif

	void _ISR USART3_IRQHandler(void){

		if(USART_GetITStatus(USART3, USART_IT_RXNE)){
			#ifdef USE_UART3_RX_BUFFER
				Uint8 * receiveddata = &(m_u3rxbuf[(m_u3rxnum % UART3_RX_BUFFER_SIZE)]);

				while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif

					*(receiveddata++) = USART_ReceiveData(USART3);
					m_u3rxnum++;
					m_u3rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u3rxbuf >= UART3_RX_BUFFER_SIZE)
						receiveddata = m_u3rxbuf;
				}

			#elif defined(USE_UART3_RX_INTERRUPT)

				if(listennerDataReceived[2] != NULL)
					listennerDataReceived[2](USART_ReceiveData(USART3));
				else
					USART_ReceiveData(USART3);

			#endif

			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		}

		if(USART_GetITStatus(USART3, USART_IT_TXE)){
			#ifdef USE_UART3_TX_BUFFER
				//debufferiser.
				if(IsNotEmpty_buffer(&buffer3tx))
				{
					assert(buffer3tx.index_read < buffer3tx.size);
					//Critical section
					if(buffer3tx.nb_datas > (Uint8)0)
					{
						Uint8 c = buffer3tx.datas[buffer3tx.index_read];
						buffer3tx.index_read = (buffer3tx.index_read>=buffer3tx.size-1)?0:(buffer3tx.index_read + 1);
						buffer3tx.nb_datas--;
						USART_SendData(USART3, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer3tx))
					USART_ITConfig(USART3, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			#endif
		}

		NVIC_ClearPendingIRQ(USART3_IRQn);
	}


#endif /* def USE_UART3 */


#ifdef USE_UART4

	#ifdef USE_UART4_RX_BUFFER
		bool_e UART4_data_ready(void){
			return m_u4rx;
		}

		Uint8 UART4_get_next_msg(void){
			static Uint32 next_to_read =0;
			if (m_u4rxnum > next_to_read)
			{
				NVIC_DisableIRQ(UART4_IRQn);
				if (m_u4rxnum - next_to_read == 1)
					m_u4rx = 0;
				NVIC_EnableIRQ(UART4_IRQn);
				return m_u4rxbuf[((next_to_read++) % UART4_RX_BUFFER_SIZE)];
			}
			else
				return 0;
		}
	#endif

	#ifdef USE_UART4_TX_BUFFER
		void UART4_putc(Uint8 data){
			if(USART_GetFlagStatus(UART4, USART_FLAG_TXE))
				USART_SendData(UART4, data);
			else
			{
				//mise en buffer + activation IT U4TX.

				assert(buffer4tx.nb_datas < buffer4tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer4tx.nb_datas >= buffer4tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(UART4_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer4tx.nb_datas < buffer4tx.size)
				{
					buffer4tx.datas[buffer4tx.index_write] = data;
					buffer4tx.index_write = (buffer4tx.index_write>=buffer4tx.size-1)?0:(buffer4tx.index_write + 1);
					buffer4tx.nb_datas++;
				}

				NVIC_EnableIRQ(UART4_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
			}
		}
	#else
		void UART4_putc(Uint8 data){
			while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
			USART_SendData(UART4, data);
		}
	#endif

	void _ISR USART4_IRQHandler(void){

		if(USART_GetITStatus(USART4, USART_IT_RXNE)){
			#ifdef USE_UART4_RX_BUFFER
				Uint8 * receiveddata = &(m_u4rxbuf[(m_u4rxnum % UART4_RX_BUFFER_SIZE)]);

				while(USART_GetFlagStatus(UART4, USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					*(receiveddata++) = USART_ReceiveData(UART4);
					m_u4rxnum++;
					m_u4rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u4rxbuf >= UART4_RX_BUFFER_SIZE)
						receiveddata = m_u4rxbuf;
				}

			#elif defined(USE_UART4_RX_INTERRUPT)

				if(listennerDataReceived[3] != NULL)
					listennerDataReceived[3](USART_ReceiveData(USART4));
				else
					USART_ReceiveData(USART4);

			#endif

			USART_ClearITPendingBit(USART4, USART_IT_RXNE);
		}

		if(USART_GetITStatus(USART4, USART_IT_TXE)){
			#ifdef USE_UART4_TX_BUFFER
				//debufferiser.
				if(IsNotEmpty_buffer(&buffer4tx))
				{
					assert(buffer4tx.index_read < buffer4tx.size);
					//Critical section
					if(buffer4tx.nb_datas > (Uint8)0)
					{
						Uint8 c = buffer4tx.datas[buffer4tx.index_read];
						buffer4tx.index_read = (buffer4tx.index_read>=buffer4tx.size-1)?0:(buffer4tx.index_read + 1);
						buffer4tx.nb_datas--;
						USART_SendData(UART4, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer4tx))
					USART_ITConfig(UART4, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			#endif
		}

		NVIC_ClearPendingIRQ(USART4_IRQn);
	}


#endif /* def USE_UART4 */

#ifdef USE_UART5

	#ifdef USE_UART5_RX_BUFFER
		bool_e UART5_data_ready(void){
			return m_u5rx;
		}

		Uint8 UART5_get_next_msg(void){
			static Uint32 next_to_read =0;
			if (m_u5rxnum > next_to_read)
			{
				NVIC_DisableIRQ(UART5_IRQn);
				if (m_u5rxnum - next_to_read == 1)
					m_u5rx = 0;
				NVIC_EnableIRQ(UART5_IRQn);
				return m_u5rxbuf[((next_to_read++) % UART5_RX_BUFFER_SIZE)];
			}
			else
				return 0;
		}
	#endif

	#ifdef USE_UART5_TX_BUFFER
		void UART5_putc(Uint8 data){
			if(USART_GetFlagStatus(USART5, USART_FLAG_TXE))
				USART_SendData(USART5, data);
			else
			{
				//mise en buffer + activation IT U5TX.

				assert(buffer5tx.nb_datas < buffer5tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer5tx.nb_datas >= buffer5tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART5_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer5tx.nb_datas < buffer5tx.size)
				{
					buffer5tx.datas[buffer5tx.index_write] = data;
					buffer5tx.index_write = (buffer5tx.index_write>=buffer5tx.size-1)?0:(buffer5tx.index_write + 1);
					buffer5tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART5_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART5, USART_IT_TXE, ENABLE);
			}
		}
	#else
		void UART5_putc(Uint8 data){
			while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
			USART_SendData(UART5, data);
		}
	#endif

	void _ISR USART5_IRQHandler(void){

		if(USART_GetITStatus(USART5, USART_IT_RXNE)){
			#ifdef USE_UART5_RX_BUFFER
				Uint8 * receiveddata = &(m_u5rxbuf[(m_u5rxnum % UART5_RX_BUFFER_SIZE)]);

				while(USART_GetFlagStatus(UART5, USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					*(receiveddata++) = USART_ReceiveData(UART5);
					m_u5rxnum++;
					m_u5rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u5rxbuf >= UART5_RX_BUFFER_SIZE)
						receiveddata = m_u5rxbuf;
				}

			#elif defined(USE_UART5_RX_INTERRUPT)

				if(listennerDataReceived[4] != NULL)
					listennerDataReceived[4](USART_ReceiveData(USART5));
				else
					USART_ReceiveData(USART5);

			#endif

			USART_ClearITPendingBit(USART5, USART_IT_RXNE);
		}

		if(USART_GetITStatus(USART5, USART_IT_TXE)){
			#ifdef USE_UART5_TX_BUFFER
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
						USART_SendData(UART5, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer5tx))
					USART_ITConfig(UART5, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			#endif
		}

		NVIC_ClearPendingIRQ(USART5_IRQn);
	}
#endif /* def USE_UART5 */

#ifdef USE_UART6

	#ifdef USE_UART6_RX_BUFFER
		bool_e UART6_data_ready(void){
			return m_u6rx;
		}

		Uint8 UART6_get_next_msg(void){
			static Uint32 next_to_read =0;
			if (m_u6rxnum > next_to_read)
			{
				NVIC_DisableIRQ(USART6_IRQn);
				if (m_u6rxnum - next_to_read == 1)
					m_u6rx = 0;
				NVIC_EnableIRQ(USART6_IRQn);
				return m_u6rxbuf[((next_to_read++) % UART6_RX_BUFFER_SIZE)];
			}
			else
				return 0;
		}
	#endif

	#ifdef USE_UART6_TX_BUFFER
		void UART6_putc(Uint8 data){
			if(USART_GetFlagStatus(USART6, USART_FLAG_TXE))
				USART_SendData(USART6, data);
			else
			{
				//mise en buffer + activation IT U6TX.

				assert(buffer6tx.nb_datas < buffer6tx.size + 1);
				//Critical section (Interrupt inibition)

				while(buffer6tx.nb_datas >= buffer6tx.size);	//ON BLOQUE ICI

				NVIC_DisableIRQ(USART6_IRQn);	//On interdit la préemption ici.. pour éviter les Read pendant les Write.

				if(buffer6tx.nb_datas < buffer6tx.size)
				{
					buffer6tx.datas[buffer6tx.index_write] = data;
					buffer6tx.index_write = (buffer6tx.index_write>=buffer6tx.size-1)?0:(buffer6tx.index_write + 1);
					buffer6tx.nb_datas++;
				}

				NVIC_EnableIRQ(USART6_IRQn);	//On active l'IT sur TX... lors du premier caractère à envoyer...
				USART_ITConfig(USART6, USART_IT_TXE, ENABLE);
			}
		}
	#else
		void UART6_putc(Uint8 data){
			while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
			USART_SendData(USART6, data);
		}
	#endif

	void _ISR USART6_IRQHandler(void){

		if(USART_GetITStatus(USART6, USART_IT_RXNE)){
			#ifdef USE_UART6_RX_BUFFER
				Uint8 * receiveddata = &(m_u6rxbuf[(m_u6rxnum % UART6_RX_BUFFER_SIZE)]);

				while(USART_GetFlagStatus(USART6, USART_FLAG_RXNE))
				{
					#ifdef LED_UART
						toggle_led(LED_UART);
					#endif
					*(receiveddata++) = USART_ReceiveData(USART6);
					m_u6rxnum++;
					m_u6rx = 1;
					/* pour eviter les comportements indésirables */
					if (receiveddata - m_u6rxbuf >= UART6_RX_BUFFER_SIZE)
						receiveddata = m_u6rxbuf;
				}

			#elif defined(USE_UART6_RX_INTERRUPT)

				if(listennerDataReceived[5] != NULL)
					listennerDataReceived[5](USART_ReceiveData(USART6));
				else
					USART_ReceiveData(USART6);

			#endif

			USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		}

		if(USART_GetITStatus(USART6, USART_IT_TXE)){
			#ifdef USE_UART6_TX_BUFFER
				//debufferiser.
				if(IsNotEmpty_buffer(&buffer6tx))
				{
					assert(buffer6tx.index_read < buffer6tx.size);
					//Critical section
					if(buffer6tx.nb_datas > (Uint8)0)
					{
						Uint8 c = buffer6tx.datas[buffer6tx.index_read];
						buffer6tx.index_read = (buffer6tx.index_read>=buffer6tx.size-1)?0:(buffer6tx.index_read + 1);
						buffer6tx.nb_datas--;
						USART_SendData(USART6, c);
					}
					//Critical section

				}
				else if(!IsNotEmpty_buffer(&buffer6tx))
					USART_ITConfig(USART6, USART_IT_TXE, DISABLE);	//Si buffer vide -> Plus rien à envoyer -> désactiver IT TX.
			#endif
		}

		NVIC_ClearPendingIRQ(USART6_IRQn);
	}

#endif /* def USE_UART6 */
