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
#include "QS_all.h"
#include "QS_can.h"
#include "QS_uart.h"
#include "QS_outputlog.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_gpio.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_adc.h"

#include "../config/config_pin.h"


bool_e PORTS_secure_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = 0;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	/* GPIOE */
	Uint32 port_e_mask =
			(1  << 7 ) |
			(1  << 8 ) |
			(1  << 9 );
	GPIO_InitStructure.Pin = ((uint32_t)port_e_mask) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	UART_init();
	CAN_init();

	bool_e i_can_run = FALSE;

	CAN_msg_t msg;

	msg.sid = BROADCAST_I_AM_AND_I_AM_HERE;
	msg.size = SIZE_BROADCAST_I_AM_AND_I_AM_HERE;

	#if defined(I_AM_CARTE_PROP) || defined(I_AM_CARTE_ACT) || defined(I_AM_CARTE_STRAT)
		 PORTS_set_pull(PORT_I_AM_STRAT, GPIO_PULLUP);
	#endif
	#if defined(I_AM_CARTE_PROP) || defined(I_AM_CARTE_ACT)
		PORTS_set_pull(PORT_I_AM_PROP, GPIO_PULLUP);
		PORTS_set_pull(PORT_I_AM_ACT, GPIO_PULLUP);
	#endif

	#ifdef I_AM_CARTE_STRAT
		if(!READ_I_AM_STRAT)
			i_can_run = TRUE;
		else{
			i_can_run = FALSE;
			error_printf("!!! Code Stratégie sur un emplacement inconnu !!!\n");
			msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_INCONNU;
			msg.data.broadcast_i_am_and_i_am_where.code_id = CODE_STRAT;
		}
	#elif defined(I_AM_CARTE_ACT)
		if(!READ_I_AM_ACT && READ_I_AM_STRAT && READ_I_AM_PROP)
			i_can_run = TRUE;
		else{
			i_can_run = FALSE;
			msg.data.broadcast_i_am_and_i_am_where.code_id = CODE_ACT;
			if(!READ_I_AM_STRAT){
				error_printf("!!! Code Actionneur sur l'emplacement Stratégie !!!\n");
				msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_STRAT;
			}else if(!READ_I_AM_PROP){
				error_printf("!!! Code Actionneur sur l'emplacement Propulsion !!!\n");
				msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_PROP;
			}else{
				error_printf("!!! Code Actionneur sur un emplacement inconnu !!!\n");
				msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_INCONNU;
			}
		}
	#elif defined(I_AM_CARTE_PROP)
		if(!READ_I_AM_PROP && READ_I_AM_ACT && READ_I_AM_STRAT)
			i_can_run = TRUE;
		else{
			i_can_run = FALSE;
			msg.data.broadcast_i_am_and_i_am_where.code_id = CODE_PROP;
			if(!READ_I_AM_STRAT){
				error_printf("!!! Code Propulsion sur l'emplacement Stratégie !!!\n");
				msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_STRAT;
			}else if(!READ_I_AM_ACT){
				error_printf("!!! Code Propulsion sur l'emplacement Actionneur !!!\n");
				msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_ACT;
			}else{
				error_printf("!!! Code Propulsion sur un emplacement inconnu !!!\n");
				msg.data.broadcast_i_am_and_i_am_where.slot_id = SLOT_INCONNU;
			}
		}
	#else
		i_can_run = TRUE;
	#endif

	if(i_can_run == FALSE){
		CAN_send(&msg);
		return FALSE;
	}

	UART_deInit();

#ifndef I_AM_CARTE_BEACON_EYE
	CAN_reset();
#endif

	PORTS_init();
	return TRUE;
}

void PORTS_init(void){
	static bool_e initialized = FALSE;
	if(initialized)
		return;

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Horloges périphériques: tout ce qui est utilisé */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = 0;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	/* GPIOA */
	Uint32 port_a_mask =
			(PORT_IO_A_0  << 0 ) |
			(PORT_IO_A_1  << 1 ) |
			(PORT_IO_A_2  << 2 ) |
			(PORT_IO_A_3  << 3 ) |
			(PORT_IO_A_4  << 4 ) |
			(PORT_IO_A_5  << 5 ) |
			(PORT_IO_A_6  << 6 ) |
			(PORT_IO_A_7  << 7 ) |
			(PORT_IO_A_8  << 8 ) |
			(PORT_IO_A_9  << 9 ) |
			(PORT_IO_A_10 << 10) |
			(PORT_IO_A_11 << 11) |
			(PORT_IO_A_12 << 12) |
			(PORT_IO_A_13 << 13) |
			(PORT_IO_A_14 << 14) |
			(PORT_IO_A_15 << 15);
	GPIO_InitStructure.Pin = ((uint32_t)port_a_mask) & 0xFFFF9FFF;	//JTMS-SWDIO, JTCK-SWCLK
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = (~((uint32_t)port_a_mask)) & 0xFFFF9FFF;	//JTMS-SWDIO, JTCK-SWCLK
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB */
	Uint32 port_b_mask =
			(PORT_IO_B_0  << 0 ) |
			(PORT_IO_B_1  << 1 ) |
			(PORT_IO_B_2  << 2 ) |
			(PORT_IO_B_3  << 3 ) |
			(PORT_IO_B_4  << 4 ) |
			(PORT_IO_B_5  << 5 ) |
			(PORT_IO_B_6  << 6 ) |
			(PORT_IO_B_7  << 7 ) |
			(PORT_IO_B_8  << 8 ) |
			(PORT_IO_B_9  << 9 ) |
			(PORT_IO_B_10 << 10) |
			(PORT_IO_B_11 << 11) |
			(PORT_IO_B_12 << 12) |
			(PORT_IO_B_13 << 13) |
			(PORT_IO_B_14 << 14) |
			(PORT_IO_B_15 << 15);
	GPIO_InitStructure.Pin = ((uint32_t)port_b_mask) & 0xFFFFFFEF; //JTDO/TRACESWO,
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = (~((uint32_t)port_b_mask)) & 0xFFFFFFEF; //JTDO/TRACESWO
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOC */
	Uint32 port_c_mask =
			(PORT_IO_C_0  << 0 ) |
			(PORT_IO_C_1  << 1 ) |
			(PORT_IO_C_2  << 2 ) |
			(PORT_IO_C_3  << 3 ) |
			(PORT_IO_C_4  << 4 ) |
			(PORT_IO_C_5  << 5 ) |
			(PORT_IO_C_6  << 6 ) |
			(PORT_IO_C_7  << 7 ) |
			(PORT_IO_C_8  << 8 ) |
			(PORT_IO_C_9  << 9 ) |
			(PORT_IO_C_10 << 10) |
			(PORT_IO_C_11 << 11) |
			(PORT_IO_C_12 << 12) |
			(PORT_IO_C_13 << 13) |
			(PORT_IO_C_14 << 14) |
			(PORT_IO_C_15 << 15);
	GPIO_InitStructure.Pin = (uint32_t)port_c_mask;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = ~((uint32_t)port_c_mask);
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOD */
	Uint32 port_d_mask =
			(PORT_IO_D_0  << 0 ) |
			(PORT_IO_D_1  << 1 ) |
			(PORT_IO_D_2  << 2 ) |
			(PORT_IO_D_3  << 3 ) |
			(PORT_IO_D_4  << 4 ) |
			(PORT_IO_D_5  << 5 ) |
			(PORT_IO_D_6  << 6 ) |
			(PORT_IO_D_7  << 7 ) |
			(PORT_IO_D_8  << 8 ) |
			(PORT_IO_D_9  << 9 ) |
			(PORT_IO_D_10 << 10) |
			(PORT_IO_D_11 << 11) |
			(PORT_IO_D_12 << 12) |
			(PORT_IO_D_13 << 13) |
			(PORT_IO_D_14 << 14) |
			(PORT_IO_D_15 << 15);
	GPIO_InitStructure.Pin = (uint32_t)port_d_mask;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = ~((uint32_t)port_d_mask);
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOE */
	Uint32 port_e_mask =
			(PORT_IO_E_0  << 0 ) |
			(PORT_IO_E_1  << 1 ) |
			(PORT_IO_E_2  << 2 ) |
			(PORT_IO_E_3  << 3 ) |
			(PORT_IO_E_4  << 4 ) |
			(PORT_IO_E_5  << 5 ) |
			(PORT_IO_E_6  << 6 ) |
			(PORT_IO_E_7  << 7 ) |
			(PORT_IO_E_8  << 8 ) |
			(PORT_IO_E_9  << 9 ) |
			(PORT_IO_E_10 << 10) |
			(PORT_IO_E_11 << 11) |
			(PORT_IO_E_12 << 12) |
			(PORT_IO_E_13 << 13) |
			(PORT_IO_E_14 << 14) |
			(PORT_IO_E_15 << 15);
	GPIO_InitStructure.Pin = ((uint32_t)port_e_mask) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = (~((uint32_t)port_e_mask)) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	PORTS_adc_init(NULL, ADC_SAMPLETIME_28CYCLES);

	initialized = TRUE;
}

Uint8 PORTS_adc_init(void* adc_handle, Uint8 ADC_sampleTime) {
	ADC_TypeDef* ADCx = adc_handle;
	Uint8 number_of_channels;
	GPIO_InitTypeDef GPIO_InitStructure;


	//Ignore des warnings si aucun ADC n'est utilisé
	UNUSED_VAR(ADCx);
	UNUSED_VAR(GPIO_InitStructure);

	/* Configure ADC1 Channelx pin as analog input ******************************/
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = 0;
	GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;

	number_of_channels = 0;

//#ifdef	USE_AN0
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN1
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN2
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN3
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN4
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN5
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN6
	GPIO_InitStructure.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN7
	GPIO_InitStructure.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN8
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN9
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN10
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN11
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN12
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN13
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN14
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN15
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
#endif
#ifdef	USE_AN_TEMP_SENSOR	// Temperature sensor
	number_of_channels++;
#endif
#ifdef	USE_AN_VREFIN		// VRefIn
	number_of_channels++;
#endif
#ifdef	USE_AN_VBAT			// VBat
	number_of_channels++;
#endif
	return number_of_channels;
}


void PORTS_set_pull(GPIO_TypeDef* GPIOx, Uint16 GPIO_Pin, Uint8 pull_up)
{
	  Uint32 pinpos = 0x00, pos = 0x00 , currentpin = 0x00;
	  /* Check the parameters */
	  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
	  assert_param(IS_GPIO_PIN(GPIO_Pin));
	  assert_param(IS_GPIO_PULL(pull_up));

	  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
	  {
		pos = ((Uint32)0x01) << pinpos;
		/* Get the port pins position */
		currentpin = GPIO_Pin & pos;
		if (currentpin == pos)
		{
		  /* Pull-up Pull down resistor configuration*/
		  GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((Uint16)pinpos * 2));
		  GPIOx->PUPDR |= (((Uint32)pull_up) << (pinpos * 2));
		}
	  }
}

void PORTS_pwm_init() {
	GPIO_InitTypeDef GPIO_InitStructure.;


#if !defined(USE_PWM1) && !defined(USE_PWM2) && !defined(USE_PWM3) && !defined(USE_PWM4)
	UNUSED_VAR(GPInit);
#endif

	/* Configuration GPIO et remappings */
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = GPIO_AF3_TIM8;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

#ifdef USE_PWM1
	GPIO_InitStructure.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef USE_PWM2
	GPIO_InitStructure.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef USE_PWM3
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef USE_PWM4
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
}

void PORTS_uarts_init() {

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_PULLUP;

#ifdef I_AM_CARTE_ACT
	//Activation de l'UART AX_12
	#if AX12_UART_ID==1
		#ifndef USE_UART1
			#define USE_UART1
		#endif
	#elif AX12_UART_ID==2
		#ifndef USE_UART2
			#define USE_UART2
		#endif
	#elif AX12_UART_ID==3
		#ifndef USE_UART3
			#define USE_UART3
		#endif
	#else
		#warning "Aucun UART n'a ete selectionne pour les AX_12"
	#endif

	//Activation de l'UART RX_24
	#if RX24_UART_ID==1
		#ifndef USE_UART1
			#define USE_UART1
		#endif
	#elif RX24_UART_ID==2
		#ifndef USE_UART2
			#define USE_UART2
		#endif
	#elif RX24_UART_ID==3
		#ifndef USE_UART3
			#define USE_UART3
		#endif
	#else
		#warning "Aucun UART n'a ete selectionne pour les RX_24"
	#endif
#endif

	#ifdef USE_UART1
		GPIO_InitStructure.Alternate = GPIO_AF7_USART1;

		//USART1 TX
		GPIO_InitStructure.Pin = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		//USART1 RX
		GPIO_InitStructure.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

	#ifdef USE_UART2
		GPIO_InitStructure.Alternate = GPIO_AF7_USART2;

		//USART2 TX
		GPIO_InitStructure.Pin = GPIO_PIN_2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
		//USART2 RX
		GPIO_InitStructure.Pin = GPIO_PIN_3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif

	#ifdef USE_UART3
		#ifdef USE_USART3_ON_PB10_PB11
			GPIO_InitStructure.Alternate = GPIO_AF7_USART3;

			//USART3 TX
			GPIO_InitStructure.Pin = GPIO_PIN_10;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
			//USART3 RX
			GPIO_InitStructure.Pin = GPIO_PIN_11;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		#else

			GPIO_InitStructure.Alternate = GPIO_AF7_USART3;

			//USART3 TX
			GPIO_InitStructure.Pin = GPIO_PIN_8;
			HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
			//USART3 RX
			GPIO_InitStructure.Pin = GPIO_PIN_9;
			HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
		#endif
	#endif
}

void PORTS_qei_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;

	#ifdef USE_QUEI1
		GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
		GPIO_InitStructure.Pin = GPIO_PIN_4;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.Pin = GPIO_PIN_5;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	#endif

	#ifdef USE_QUEI2
		GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
		GPIO_InitStructure.Pin = GPIO_PIN_1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.Pin = GPIO_PIN_15;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif
}

void PORTS_spi_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;

	//GPIO_InitStructure.Pin = GPIO_PIN_12;				//NSS : seulement en Slave
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void PORTS_i2c_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;

	#ifdef USE_I2C1
		GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
		GPIO_InitStructure.Pin = GPIO_PIN_8;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.Pin = GPIO_PIN_9;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif
	#ifdef USE_I2C2
		GPIO_InitStructure.Alternate = GPIO_AF4_I2C2;
		GPIO_InitStructure.Pin = GPIO_PIN_10;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.Pin = GPIO_PIN_11;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif
}

void PORTS_can_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF9_CAN1;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;

	GPIO_InitStructure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
}
