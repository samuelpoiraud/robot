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
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"

#include "../config/config_pin.h"


bool_e PORTS_secure_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	/* GPIOE */
	Uint32 port_e_mask =
			(1  << 7 ) |
			(1  << 8 ) |
			(1  << 9 );
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)port_e_mask) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	UART_init();
	CAN_init();

	bool_e i_can_run = FALSE;

	CAN_msg_t msg;

	msg.sid = BROADCAST_I_AM_AND_I_AM_HERE;
	msg.size = SIZE_BROADCAST_I_AM_AND_I_AM_HERE;

	#if defined(I_AM_CARTE_PROP) || defined(I_AM_CARTE_ACT) || defined(I_AM_CARTE_STRAT)
		 PORTS_set_pull(PORT_I_AM_STRAT, GPIO_PuPd_UP);
	#endif
	#if defined(I_AM_CARTE_PROP) || defined(I_AM_CARTE_ACT)
		PORTS_set_pull(PORT_I_AM_PROP, GPIO_PuPd_UP);
		PORTS_set_pull(PORT_I_AM_ACT, GPIO_PuPd_UP);
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
		//if(!READ_I_AM_PROP && READ_I_AM_ACT && READ_I_AM_STRAT)
			i_can_run = TRUE;
		/*else{
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
		}*/
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
	RCC_AHB1PeriphClockCmd(
		  RCC_AHB1Periph_GPIOA
		| RCC_AHB1Periph_GPIOB
		| RCC_AHB1Periph_GPIOC
		| RCC_AHB1Periph_GPIOD
		| RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

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
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)port_a_mask) & 0xFFFF9FFF;	//JTMS-SWDIO, JTCK-SWCLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = (~((uint32_t)port_a_mask)) & 0xFFFF9FFF;	//JTMS-SWDIO, JTCK-SWCLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	PORTS_set_pull(GPIOA, GPIO_Pin_0, PORT_OPT_A_0);
	PORTS_set_pull(GPIOA, GPIO_Pin_1, PORT_OPT_A_1);
	PORTS_set_pull(GPIOA, GPIO_Pin_2, PORT_OPT_A_2);
	PORTS_set_pull(GPIOA, GPIO_Pin_3, PORT_OPT_A_3);
	PORTS_set_pull(GPIOA, GPIO_Pin_4, PORT_OPT_A_4);
	PORTS_set_pull(GPIOA, GPIO_Pin_5, PORT_OPT_A_5);
	PORTS_set_pull(GPIOA, GPIO_Pin_6, PORT_OPT_A_6);
	PORTS_set_pull(GPIOA, GPIO_Pin_7, PORT_OPT_A_7);
	PORTS_set_pull(GPIOA, GPIO_Pin_8, PORT_OPT_A_8);
	PORTS_set_pull(GPIOA, GPIO_Pin_9, PORT_OPT_A_9);
	PORTS_set_pull(GPIOA, GPIO_Pin_10, PORT_OPT_A_10);
	PORTS_set_pull(GPIOA, GPIO_Pin_11, PORT_OPT_A_11);
	PORTS_set_pull(GPIOA, GPIO_Pin_12, PORT_OPT_A_12);
	PORTS_set_pull(GPIOA, GPIO_Pin_13, PORT_OPT_A_13);
	PORTS_set_pull(GPIOA, GPIO_Pin_14, PORT_OPT_A_14);
	PORTS_set_pull(GPIOA, GPIO_Pin_15, PORT_OPT_A_15);

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
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)port_b_mask) & 0xFFFFFFEF; //JTDO/TRACESWO,
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = (~((uint32_t)port_b_mask)) & 0xFFFFFFEF; //JTDO/TRACESWO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	PORTS_set_pull(GPIOB, GPIO_Pin_0, PORT_OPT_B_0);
	PORTS_set_pull(GPIOB, GPIO_Pin_1, PORT_OPT_B_1);
	PORTS_set_pull(GPIOB, GPIO_Pin_2, PORT_OPT_B_2);
	PORTS_set_pull(GPIOB, GPIO_Pin_3, PORT_OPT_B_3);
	PORTS_set_pull(GPIOB, GPIO_Pin_4, PORT_OPT_B_4);
	PORTS_set_pull(GPIOB, GPIO_Pin_5, PORT_OPT_B_5);
	PORTS_set_pull(GPIOB, GPIO_Pin_6, PORT_OPT_B_6);
	PORTS_set_pull(GPIOB, GPIO_Pin_7, PORT_OPT_B_7);
	PORTS_set_pull(GPIOB, GPIO_Pin_8, PORT_OPT_B_8);
	PORTS_set_pull(GPIOB, GPIO_Pin_9, PORT_OPT_B_9);
	PORTS_set_pull(GPIOB, GPIO_Pin_10, PORT_OPT_B_10);
	PORTS_set_pull(GPIOB, GPIO_Pin_11, PORT_OPT_B_11);
	PORTS_set_pull(GPIOB, GPIO_Pin_12, PORT_OPT_B_12);
	PORTS_set_pull(GPIOB, GPIO_Pin_13, PORT_OPT_B_13);
	PORTS_set_pull(GPIOB, GPIO_Pin_14, PORT_OPT_B_14);
	PORTS_set_pull(GPIOB, GPIO_Pin_15, PORT_OPT_B_15);

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
	GPIO_InitStructure.GPIO_Pin = (uint32_t)port_c_mask;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ~((uint32_t)port_c_mask);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	PORTS_set_pull(GPIOC, GPIO_Pin_0, PORT_OPT_C_0);
	PORTS_set_pull(GPIOC, GPIO_Pin_1, PORT_OPT_C_1);
	PORTS_set_pull(GPIOC, GPIO_Pin_2, PORT_OPT_C_2);
	PORTS_set_pull(GPIOC, GPIO_Pin_3, PORT_OPT_C_3);
	PORTS_set_pull(GPIOC, GPIO_Pin_4, PORT_OPT_C_4);
	PORTS_set_pull(GPIOC, GPIO_Pin_5, PORT_OPT_C_5);
	PORTS_set_pull(GPIOC, GPIO_Pin_6, PORT_OPT_C_6);
	PORTS_set_pull(GPIOC, GPIO_Pin_7, PORT_OPT_C_7);
	PORTS_set_pull(GPIOC, GPIO_Pin_8, PORT_OPT_C_8);
	PORTS_set_pull(GPIOC, GPIO_Pin_9, PORT_OPT_C_9);
	PORTS_set_pull(GPIOC, GPIO_Pin_10, PORT_OPT_C_10);
	PORTS_set_pull(GPIOC, GPIO_Pin_11, PORT_OPT_C_11);
	PORTS_set_pull(GPIOC, GPIO_Pin_12, PORT_OPT_C_12);
	PORTS_set_pull(GPIOC, GPIO_Pin_13, PORT_OPT_C_13);
	PORTS_set_pull(GPIOC, GPIO_Pin_14, PORT_OPT_C_14);
	PORTS_set_pull(GPIOC, GPIO_Pin_15, PORT_OPT_C_15);

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
	GPIO_InitStructure.GPIO_Pin = (uint32_t)port_d_mask;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ~((uint32_t)port_d_mask);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	PORTS_set_pull(GPIOD, GPIO_Pin_0, PORT_OPT_D_0);
	PORTS_set_pull(GPIOD, GPIO_Pin_1, PORT_OPT_D_1);
	PORTS_set_pull(GPIOD, GPIO_Pin_2, PORT_OPT_D_2);
	PORTS_set_pull(GPIOD, GPIO_Pin_3, PORT_OPT_D_3);
	PORTS_set_pull(GPIOD, GPIO_Pin_4, PORT_OPT_D_4);
	PORTS_set_pull(GPIOD, GPIO_Pin_5, PORT_OPT_D_5);
	PORTS_set_pull(GPIOD, GPIO_Pin_6, PORT_OPT_D_6);
	PORTS_set_pull(GPIOD, GPIO_Pin_7, PORT_OPT_D_7);
	PORTS_set_pull(GPIOD, GPIO_Pin_8, PORT_OPT_D_8);
	PORTS_set_pull(GPIOD, GPIO_Pin_9, PORT_OPT_D_9);
	PORTS_set_pull(GPIOD, GPIO_Pin_10, PORT_OPT_D_10);
	PORTS_set_pull(GPIOD, GPIO_Pin_11, PORT_OPT_D_11);
	PORTS_set_pull(GPIOD, GPIO_Pin_12, PORT_OPT_D_12);
	PORTS_set_pull(GPIOD, GPIO_Pin_13, PORT_OPT_D_13);
	PORTS_set_pull(GPIOD, GPIO_Pin_14, PORT_OPT_D_14);
	PORTS_set_pull(GPIOD, GPIO_Pin_15, PORT_OPT_D_15);

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
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)port_e_mask) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = (~((uint32_t)port_e_mask)) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	PORTS_set_pull(GPIOE, GPIO_Pin_0, PORT_OPT_E_0);
	PORTS_set_pull(GPIOE, GPIO_Pin_1, PORT_OPT_E_1);
	PORTS_set_pull(GPIOE, GPIO_Pin_2, PORT_OPT_E_2);
	PORTS_set_pull(GPIOE, GPIO_Pin_3, PORT_OPT_E_3);
	PORTS_set_pull(GPIOE, GPIO_Pin_4, PORT_OPT_E_4);
	PORTS_set_pull(GPIOE, GPIO_Pin_5, PORT_OPT_E_5);
	PORTS_set_pull(GPIOE, GPIO_Pin_6, PORT_OPT_E_6);
	PORTS_set_pull(GPIOE, GPIO_Pin_7, PORT_OPT_E_7);
	PORTS_set_pull(GPIOE, GPIO_Pin_8, PORT_OPT_E_8);
	PORTS_set_pull(GPIOE, GPIO_Pin_9, PORT_OPT_E_9);
	PORTS_set_pull(GPIOE, GPIO_Pin_10, PORT_OPT_E_10);
	PORTS_set_pull(GPIOE, GPIO_Pin_11, PORT_OPT_E_11);
	PORTS_set_pull(GPIOE, GPIO_Pin_12, PORT_OPT_E_12);
	PORTS_set_pull(GPIOE, GPIO_Pin_13, PORT_OPT_E_13);
	PORTS_set_pull(GPIOE, GPIO_Pin_14, PORT_OPT_E_14);
	PORTS_set_pull(GPIOE, GPIO_Pin_15, PORT_OPT_E_15);

	PORTS_adc_init(NULL, ADC_SampleTime_28Cycles);

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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	number_of_channels = 0;

#ifdef	USE_AN0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_0, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_1, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_2, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_3, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_4, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_5, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_6, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_7, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_8, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_9, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_10, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_11, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_12, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_13, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_14, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_15, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN_TEMP_SENSOR	// Temperature sensor
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_TempSensor, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN_VREFIN		// VRefIn
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_Vrefint, number_of_channels, ADC_sampleTime);
#endif
#ifdef	USE_AN_VBAT			// VBat
	number_of_channels++;
	if(ADCx) ADC_RegularChannelConfig(ADCx, ADC_Channel_Vbat, number_of_channels, ADC_sampleTime);
#endif
	return number_of_channels;
}


void PORTS_set_pull(GPIO_TypeDef* GPIOx, Uint16 GPIO_Pin, GPIOPuPd_TypeDef pull_up)
{
	  Uint32 pinpos = 0x00, pos = 0x00 , currentpin = 0x00;
	  /* Check the parameters */
	  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	  assert_param(IS_GPIO_PIN(GPIO_Pin));
	  assert_param(IS_GPIO_PUPD(pull_up));

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
	GPIO_InitTypeDef GPInit;


#if !defined(USE_PWM1) && !defined(USE_PWM2) && !defined(USE_PWM3) && !defined(USE_PWM4)
	UNUSED_VAR(GPInit);
#endif

	/* Configuration GPIO et remappings */
	GPInit.GPIO_Mode = GPIO_Mode_AF;
	GPInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPInit.GPIO_OType = GPIO_OType_PP;
	GPInit.GPIO_PuPd  = GPIO_PuPd_NOPULL;

#ifdef USE_PWM1
	GPInit.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPInit);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
#endif
#ifdef USE_PWM2
	GPInit.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPInit);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
#endif
#ifdef USE_PWM3
	GPInit.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPInit);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
#endif
#ifdef USE_PWM4
	GPInit.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPInit);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
#endif
}

void PORTS_uarts_init() {

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

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
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);	//U1TX
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);	//U1RX

		//USART1 TX
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		//USART1 RX
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

	#ifdef USE_UART2
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);	//U2TX
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);	//U2RX

		//USART2 TX
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//USART2 RX
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif

	#ifdef USE_UART3
		#ifdef USE_USART3_ON_PB10_PB11
			GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);	//U3TX
			GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);	//U3RX

			//USART3 TX
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			//USART3 RX
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
		#else

			GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);	//U3TX
			GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);	//U3RX

			//USART3 TX
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
			//USART3 RX
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
		#endif
	#endif
}

void PORTS_qei_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	#ifdef USE_QUEI1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);
	#endif

	#ifdef USE_QUEI2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2);
	#endif
}

void PORTS_spi_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	#ifdef USE_SPI1
		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				//NSS : seulement en Slave
		//GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI1);	//NSS : seulement en Slave
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	#endif

	#ifdef USE_SPI2
		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				//NSS : seulement en Slave
		//GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		//GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);	//NSS : seulement en Slave
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	#endif
}


void PORTS_ssi_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
}

void PORTS_i2c_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;

	#ifdef USE_I2C1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	#endif
	#ifdef USE_I2C2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
	#endif

}

void PORTS_can_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);
}
