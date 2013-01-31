
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_CONF_H
	#define __STM32F10x_CONF_H
	
	/* Modules indispensables */
	#include "stm32f10x_gpio.h"
	#include "stm32f10x_rcc.h"
	#include "misc.h"
	
	/* Modules mat�riels utiles */
	#include "stm32f10x_tim.h"
	#include "stm32f10x_usart.h"
	#include "stm32f10x_adc.h"
	#include "stm32f10x_can.h"
	#include "stm32f10x_exti.h"
	
	/*  Modules non utilis�s */
	/* #include "stm32f10x_pwr.h" */
	/* #include "stm32f10x_dbgmcu.h" */
	/* #include "stm32f10x_dac.h" */
	/* #include "stm32f10x_flash.h" */
	/* #include "stm32f10x_fsmc.h" */
	/* #include "stm32f10x_i2c.h" */
	/* #include "stm32f10x_sdio.h" */
	/* #include "stm32f10x_spi.h" */
	/* #include "stm32f10x_iwdg.h" */
	/* #include "stm32f10x_wwdg.h" */
	/* #include "stm32f10x_rtc.h" */
	/* #include "stm32f10x_bkp.h" */
	/* #include "stm32f10x_dma.h" */
	/* #include "stm32f10x_crc.h" */
	
	/* Gestion de ASSERT */
	#ifdef  USE_FULL_ASSERT
		#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
		void assert_failed(uint8_t* file, uint32_t line);
	#else
		#define assert_param(expr) ((void)0)
	#endif /* USE_FULL_ASSERT */

#endif /* __STM32F10x_CONF_H */
