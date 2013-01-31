/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : main.c
 *  Package : Qualité Soft
 *  Description : 	Test d'un STM32
 *  Auteur : Gwenn
 *  Version 20100131
 */

#define DELAY 0xAFFFF

#include "stm32f10x_lib.h"
#include "config.h"
#include "utils.h"


/*-----------------------------------------------------
                    Main
-----------------------------------------------------*/
int main(){

    unsigned char i = 0;
    
    // Configuration
    RCC_config();
    NVIC_config();
    GPIO_config();
    UART_config();

    while(1){
        
        // Compteur
        i++;
        GPIO_Write(GPIOE, i);    
        //delay(DELAY);
        
        // Ecriture UART
        USART_SendData(UART4, 0x00F1);
        while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET){}
    }
    return 0;
}

