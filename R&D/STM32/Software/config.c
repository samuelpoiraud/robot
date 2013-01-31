/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : config.c
 *  Package : Qualité Soft
 *  Description : 	Configuration d'un STM32
 *  Auteur : Gwenn
 *  Version 20100131
 */

#include "config.h"


/*-----------------------------------------------------
              Initialisation timer 1
-----------------------------------------------------*/
void TIMER_run(u16 period){
	
	/* Choix de l'unité */
	#define UNIT_US 1
	#define UNIT_MS 1024
	#define UNIT UNIT_MS

	TIM_TypeDef* TIMx = TIM1;
	
	/* Horloge */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	/* NVIC */
	NVIC_InitTypeDef NVICInit;
	NVICInit.NVIC_IRQChannel = TIM1_UP_IRQChannel;
	NVICInit.NVIC_IRQChannelPreemptionPriority = 0;
	NVICInit.NVIC_IRQChannelSubPriority = 1;
	NVICInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICInit);
	
	/* Timer */
	TIM_TimeBaseInitTypeDef TInit;
	TIM_TimeBaseStructInit(&TInit);
	TInit.TIM_Period = UNIT * period;
	TInit.TIM_Prescaler = 36; // = 2 * fmicro
	TInit.TIM_ClockDivision = TIM_CKD_DIV1;
	TInit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TInit);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIMx, ENABLE);
}




/*-----------------------------------------------------
              Initialisation des UART
-----------------------------------------------------*/
void UART_config(){
	GPIO_InitTypeDef GPInit;
	USART_InitTypeDef UInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	GPInit.GPIO_Pin = GPIO_Pin_10;
	GPInit.GPIO_Mode = GPIO_Mode_AF_PP;
	GPInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPInit);

	GPInit.GPIO_Pin = GPIO_Pin_11;
	GPInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPInit);

	UInit.USART_BaudRate    = 9600; 
	UInit.USART_WordLength  = USART_WordLength_8b;
	UInit.USART_StopBits    = USART_StopBits_1;
	UInit.USART_Parity      = USART_Parity_No;
	UInit.USART_Mode        = USART_Mode_Rx | USART_Mode_Tx;
	UInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(UART4, &UInit);
	USART_Cmd(UART4, ENABLE);
}


/*-----------------------------------------------------
           Configuration des entrées-sorties
-----------------------------------------------------*/
void GPIO_config(){
		 
	/* Horloge GPIO */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	/* Configuration des pins */
	{
		GPIO_InitTypeDef GPInit;
		GPInit.GPIO_Pin =
			  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
			| GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPInit.GPIO_Mode = GPIO_Mode_Out_PP;
		GPInit.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOE, &GPInit);
	}
}



/*-----------------------------------------------------
           Configuration des horloges
-----------------------------------------------------*/
void RCC_config(){
	 ErrorStatus HSEStartUpStatus;															
	 /* RCC system reset(for debug purpose) */
	 RCC_DeInit();

	 /* Enable HSE */
	 RCC_HSEConfig(RCC_HSE_ON);

	 /* Wait till HSE is ready */
	 HSEStartUpStatus = RCC_WaitForHSEStartUp();

	 if(HSEStartUpStatus == SUCCESS)
	 {
		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);

		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		/* PLLCLK = 8MHz * 9 = 72 MHz */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource() != 0x08){}
	}
}


/*-----------------------------------------------------
           Configuration des interruptions
-----------------------------------------------------*/
void NVIC_config(){
	#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
}
