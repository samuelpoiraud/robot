/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_uart.c
 *  Package : Qualité Soft
 *  Description : Gestion de l'UART
 *  Auteur : Gwenn
 *  Version 20100419
 */

#define QS_UART_C
#include "QS_uart.h"

#ifdef USE_UART

void UART_init(){
	
	/* Vérification de non-initialisation */
	if (initialized) return;
	
    GPIO_InitTypeDef GPInit;
    USART_InitTypeDef UInit;
	
	/* Horloges */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	/* Configuration des GPIO pour l'UART */
    GPInit.GPIO_Pin = GPIO_Pin_10;
    GPInit.GPIO_Mode = GPIO_Mode_AF_PP;
    GPInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPInit);

    GPInit.GPIO_Pin = GPIO_Pin_11;
    GPInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPInit);
    
	/* Configuration du module UART avec les valeurs par défaut 
		* 9600bps, 8 bits data, 1 bit de stop, pas de contrôle de flux
	*/

	USART_DeInit(UART4);
	USART_StructInit(&UInit);
	UInit.USART_BaudRate = 60000;
	
	/* Démarrage */
    USART_Init(UART4, &UInit);
    USART_Cmd(UART4, ENABLE);
	initialized = TRUE;
}

#endif /* def USE_UART */
