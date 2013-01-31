/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_ports.c
 *  Package : Qualité Soft
 *  Description : Gestion des entrées-sorties GPIO
 *  Auteur : Gwenn
 *  Version 20100418
 */


#include "QS_ports.h"


/** Configure le module GPIO pour l'usage des LEDs
 **/
void LED_init(void);


void PORTS_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* Horloges périphériques: tout ce qui est utilisé */
	RCC_APB2PeriphClockCmd(
		  RCC_APB2Periph_GPIOA
		| RCC_APB2Periph_GPIOB
		| RCC_APB2Periph_GPIOC
		| RCC_APB2Periph_GPIOD
		| RCC_APB2Periph_GPIOE
		| RCC_APB2Periph_AFIO, ENABLE);
	
	/* Remappings de ports */
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_CAN2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

	/* GPIOA */
	#ifdef PORTA_INPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTA_INPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif
	#ifdef PORTA_OUTPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTA_OUTPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif

	/* GPIOB */
	#ifdef PORTB_INPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTB_INPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif
	#ifdef PORTB_OUTPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTB_OUTPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

	/* GPIOC */
	#ifdef PORTC_INPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTC_INPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif
	#ifdef PORTC_OUTPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTC_OUTPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif

	/* GPIOD */
	#ifdef PORTD_INPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTD_INPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif
	#ifdef PORTD_OUTPUT_PIN_LIST
		GPIO_InitStructure.GPIO_Pin = PORTD_OUTPUT_PIN_LIST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif
	
	/* LEDs embarquées sur la carte */
	LED_init();
}


void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* LEDs : PE0 -> PE8, Push-pull, 50MHz*/
	GPIO_InitStructure.GPIO_Pin =
		  GPIO_Pin_0
		| GPIO_Pin_1
		| GPIO_Pin_2
		| GPIO_Pin_3
		| GPIO_Pin_4
		| GPIO_Pin_5
		| GPIO_Pin_6
		| GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void LED_write(Uint8 value){
	GPIO_Write(GPIOE, value);
}

