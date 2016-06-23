/*
 *	Club Robot ESEO 2013 - 2014
 *
 *	Fichier : QS_clocks_freq.h
 *	Package : Qualite Soft
 *	Description : Défini les fréquences des clocks disponibles
 *	Auteur : Alexis
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20130526
 */

#include "QS_all.h"

//Pour avoir un PLLP entier, on le force à une valeur, ce qui définira VCO_OUTPUT_HZ en conséquence
#define FORCED_PLLP  2
#define FORCED_HCLK_DIV	1

//L'entrée du VCO doit avoir une freqence entre 1Mhz et 2Mhz
#define VCO_INPUT_HZ  1000000	//1Mhz
//La sortie du VCO doit avoir une freqence entre 192Mhz et 432Mhz
#define VCO_OUTPUT_HZ (HCLK_FREQUENCY_HZ*FORCED_HCLK_DIV*FORCED_PLLP)
#define SYSCLK_HZ (HCLK_FREQUENCY_HZ*FORCED_HCLK_DIV)
#define USB_RNG_SDIO_CLK_HZ  48000000	//L'usb doit utiliser une fréquence de 48Mhz, RNG et SDIO <= 48Mhz, donc on prend 48Mhz

#if (SYSCLK_HZ / PCLK1_FREQUENCY_HZ) > 1
	#define TIM_CLK1_FREQUENCY_HZ (PCLK1_FREQUENCY_HZ*2)
#else
	#define TIM_CLK1_FREQUENCY_HZ PCLK1_FREQUENCY_HZ
#endif

#if (SYSCLK_HZ / PCLK2_FREQUENCY_HZ) > 1
	#define TIM_CLK2_FREQUENCY_HZ (PCLK2_FREQUENCY_HZ*2)
#else
	#define TIM_CLK2_FREQUENCY_HZ PCLK2_FREQUENCY_HZ
#endif
