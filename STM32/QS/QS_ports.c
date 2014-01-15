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
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"

#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif


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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	/* GPIOA */
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)PORT_A_IO_MASK) & 0xFFFF9FFF;	//JTMS-SWDIO, JTCK-SWCLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = (~((uint32_t)PORT_A_IO_MASK)) & 0xFFFF9FFF;	//JTMS-SWDIO, JTCK-SWCLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB */
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)PORT_B_IO_MASK) & 0xFFFFFFEF; //JTDO/TRACESWO,
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = (~((uint32_t)PORT_B_IO_MASK)) & 0xFFFFFFEF; //JTDO/TRACESWO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOC */
	GPIO_InitStructure.GPIO_Pin = (uint32_t)PORT_C_IO_MASK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ~((uint32_t)PORT_C_IO_MASK);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOD */
	GPIO_InitStructure.GPIO_Pin = (uint32_t)PORT_D_IO_MASK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ~((uint32_t)PORT_D_IO_MASK);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOE */
	GPIO_InitStructure.GPIO_Pin = ((uint32_t)PORT_E_IO_MASK) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = (~((uint32_t)PORT_E_IO_MASK)) & 0xFFFFFF83;	//TRACECLK, TRACED[0-3]
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	PORTS_adc_init(NULL, ADC_SampleTime_28Cycles);

	initialized = TRUE;
}

Uint8 PORTS_adc_init(void* adc_handle, Uint8 ADC_sampleTime) {
	ADC_TypeDef* ADCx = adc_handle;
	Uint8 number_of_channels;
	GPIO_InitTypeDef GPIO_InitStructure;

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

	/* Configuration GPIO et remappings */
	GPInit.GPIO_Mode = GPIO_Mode_AF;
	GPInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPInit.GPIO_OType = GPIO_OType_PP;
	GPInit.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPInit);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
}

void PORTS_uarts_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);	//U1TX
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);	//U1RX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);	//U2TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);	//U2RX
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);	//U3TX
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);	//U3RX

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	//USART1 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//USART1 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART2 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART2 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART3 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//USART3 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void PORTS_qei_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2);
}

void PORTS_spi_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

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
}

void PORTS_i2c_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;

	#ifdef USE_I2C1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C);
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
