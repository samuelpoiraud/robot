/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Configuration du STM32 - Horloges - DÃ©marrage
 *	Auteur : Gwenn
 *	Version 20100421
 */

#define QS_SYS_C

#include "QS_sys.h"

/** Initialisation du systÃ¨me
	* HSE = 8MHz
	* PLLSRC = 8MHz
	* PLLCLK = 72MHz
	* SYSCLK = 72MHz
	* PCLK1 = 36MHz
	* PCLK2 = 72MHz
**/

//Pour avoir un PLLP entier, on le force à une valeur, ce qui définira VCO_OUTPUT_HZ en conséquence
#define FORCED_PLLP  8
#define FORCED_HCLK_DIV	1

//L'entrée du VCO doit avoir une freqence entre 1Mhz et 2Mhz
#define VCO_INPUT_HZ  1000000	//2Mhz
//La sortie du VCO doit avoir une freqence entre 192Mhz et 432Mhz
#define VCO_OUTPUT_HZ (HCLK_FREQUENCY_HZ*FORCED_PLLP)
#define SYSCLK_HZ (HCLK_FREQUENCY_HZ*FORCED_HCLK_DIV)
#define USB_RNG_SDIO_CLK_HZ  48000000	//L'usb doit utiliser une fréquence de 48Mhz, RNG et SDIO <= 48Mhz, donc on prend 48Mhz

#define HCLK_DIV	FORCED_HCLK_DIV	//HCLK = SYSCLK_HZ / HCLK_DIV
#define PCLK1_DIV	(HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ)	//PCLK1 = SYSCLK_HZ / PCLK1_DIV
#define PCLK2_DIV	(HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ)	//PCLK2 = SYSCLK_HZ / PCLK2_DIV

//VCO_INPUT_HZ = CPU_EXTERNAL_CLOCK_HZ / PLLM
//VCO_OUTPUT_HZ = VCO_INPUT_HZ * PLLN
//SYSCLK_HZ = VCO_OUTPUT_HZ / PLLP
//USB_RNG_SDIO_CLK_HZ = SYSCLK_HZ / PLLQ
//SYSCLK = CPU_FREQUENCY_HZ = CPU_EXTERNAL_CLOCK_HZ / PLLM * PLLN / PLLP

#define PLLM (CPU_EXTERNAL_CLOCK_HZ / VCO_INPUT_HZ)
#define PLLN (VCO_OUTPUT_HZ / 64 * PLLM / CPU_EXTERNAL_CLOCK_HZ * 64)	//On utilise pas directement VCO_INPUT_HZ car il peut ne pas pouvoir être exact
#define PLLP FORCED_PLLP
#define PLLQ (VCO_OUTPUT_HZ / USB_RNG_SDIO_CLK_HZ)

#if HCLK_FREQUENCY_HZ != (((CPU_EXTERNAL_CLOCK_HZ / PLLM) * PLLN) / PLLP)
#warning "Computed CPU frequency is not exactly CPU_FREQUENCY_HZ"
#endif

#if USB_RNG_SDIO_CLK_HZ != (((CPU_EXTERNAL_CLOCK_HZ / PLLM) * PLLN) / PLLQ)
#warning "USB Frequency is not exactly USB_RNG_SDIO_CLK_HZ"
#endif

#if VCO_INPUT_HZ < 1000000
#error "VCO_INPUT_HZ must be >= 1Mhz"
#elif VCO_INPUT_HZ > 2000000
#error "VCO_INPUT_HZ must <= 2Mhz"
#endif

#if VCO_OUTPUT_HZ < 192000000
#error "VCO_OUTPUT_HZ must be >= 192Mhz"
#elif VCO_OUTPUT_HZ > 432000000
#error "VCO_OUTPUT_HZ must be <= 432Mhz"
#endif

#if PLLM < 0 || PLLM > 63
#error "PLLM be be a unsigned integer <= 63"
#endif

#if PLLN < 192 || PLLN > 432
#error "PLLN must be >= 192 and <= 432"
#endif

#if PLLP != 2 && PLLP != 4 && PLLP != 6 && PLLP != 8
#error "PLLP must be either 2, 4, 6 or 8"
#endif

#if PLLQ < 4 || PLLQ > 15
#error "PLLQ must be >= 4 and <= 15"
#endif

#if (SYSCLK_HZ / HCLK_DIV) > 168000000
#error "HCLK Frequency is too high, increase HCLK_DIV"
#endif

#if (SYSCLK_HZ / HCLK_DIV / PCLK1_DIV) > 42000000
#error "PCLK1 Frequency is too high, increase PCLK1_DIV"
#endif

#if (SYSCLK_HZ / HCLK_DIV / PCLK2_DIV) > 84000000
#error "PCLK2 Frequency is too high, increase PCLK2_DIV"
#endif

void SYS_init(void)
{
	SystemInit();
	RCC_DeInit();

	/* Oscillateur externe */
	ErrorStatus HSEStartUpStatus;
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus != ERROR)
	{
		/* PCLK1 = HCLK/2, PCLK2 = HCLK | HCLK = SYSCLK */
		//Pour savoir si les valeurs sont correctes, veuillez changer HCLK_DIV, PCLK1_DIV et PCLK2_DIV. Une erreur de précompilation indiquera s'il y a un problème
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PCLK2Config(RCC_HCLK_Div1);

		RCC_PLLConfig(RCC_PLLSource_HSE, PLLM, PLLN, PLLP, PLLQ);

		/* Enable PLL1 */
		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while (RCC_GetSYSCLKSource() != 0x08){}
	}
	
	SystemCoreClockUpdate();


	//Pas de subpriority sur les interruptions
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
