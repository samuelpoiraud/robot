/*
 *  Club Robot ESEO 2015 - 2016
 *
 *  Fichier : QS_rcc.c
 *  Package : Qualité Soft
 *  Description : Couche bas niveau RCC
 *  Auteur : Arnaud
 *  Version 20130518
 */

#include "QS_rcc.h"
#include "../stm32f4xx/stm32f4xx_rcc.h"
#include "QS_outputlog.h"

void RCC_read(void){
#if defined(STM32F40XX)
	debug_printf("STM32F4xx reset source :\n");
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
		debug_printf("- Low power management\n");
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
		debug_printf("- Window watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
		debug_printf("- Independent watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST))
		debug_printf("- Software reset\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
		debug_printf("- POR\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST))
		debug_printf("- Pin NRST\n");
	if(RCC_GetFlagStatus(RCC_FLAG_BORRST))
		debug_printf("- POR or BOR\n");
	RCC_ClearFlag();
#endif
}

bool_e RCC_board_just_reboot(){
	return (RCC_GetFlagStatus(RCC_FLAG_PORRST) == RESET);
}
