/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Configuration du STM32 - Horloges - Démarrage
 *	Auteur : Gwenn
 *	Version 20100421
 */

#define QS_SYS_C

#include "QS_sys.h"

/** Initialisation du système
	* HSE = 8MHz
	* PLLSRC = 8MHz
	* PLLCLK = 72MHz
	* SYSCLK = 72MHz
	* PCLK1 = 36MHz
	* PCLK2 = 72MHz
**/
void SYS_init(void)
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
		/* PCLK1 = HCLK/2, PCLK2 = HCLK | HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PCLK2Config(RCC_HCLK_Div1); 

		/* PLLCLK = PLLSRC * 9 = 72 MHz | PLLSRC = HSE */
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
	
	/* Table d'IT dans la flash */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
}
