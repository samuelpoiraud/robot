/*---------------------------------------------------
	Programme de test de la carte STM32
	Allume quelques LEDs
---------------------------------------------------*/

#include "stm32f10x.h"

/*---------------------------------------------------
	Prototypes
---------------------------------------------------*/

void QS_init();
void LEDS_init(void);
void LEDS_write(int value);


/*---------------------------------------------------
	main
---------------------------------------------------*/
int main(void)
{
	QS_init();
	LEDS_init();

	LEDS_write(42);
	
	while (1){}
}


/*---------------------------------------------------
	Fonctions
---------------------------------------------------*/


/**
	* Initialise les LEDs de la carte sur le port E
**/
void LEDS_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

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
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**
	* Ecrit une valeur sur les LEDs de la carte
	* param value Valeur 8 bits à afficher
**/
void LEDS_write(int value){
	GPIO_Write(GPIOE, value);
}


/**
	* Configurations essentielles de la carte
**/
void QS_init(void)
{
	/* Horloges */
	RCC_ClocksTypeDef RCC_ClockFreq;
	SystemInit();
	RCC_DeInit();

	/* Oscillateur externe */
	ErrorStatus HSEStartUpStatus;
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus != ERROR)
	{
		/* PCLK1 = HCLK/2 = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);

		/* PLLCLK = HSE * 9 = 72 MHz */
		RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);

		/* Enable PLL1 */
		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while (RCC_GetSYSCLKSource() != 0x08){}
	}
	RCC_GetClocksFreq(&RCC_ClockFreq);

	/* Horloges périphériques: tout ce qui est utilisé */
	RCC_APB2PeriphClockCmd(
		  RCC_APB2Periph_GPIOA
		| RCC_APB2Periph_GPIOB
		| RCC_APB2Periph_GPIOC
		| RCC_APB2Periph_GPIOD
		| RCC_APB2Periph_GPIOE
		| RCC_APB2Periph_AFIO, ENABLE);
	
	/* Table d'IT dans la flash */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
}

