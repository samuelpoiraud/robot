/*
 *  Club Robot ESEO 2009 - 2013
 *
 *  Fichier : QS_adc.h
 *  Package : Qualité Soft
 *  Description : Gestion du convertisseur analogique/numérique
 *  Auteur : Alexis
 *  Version 20130518
 */


#include "QS_adc.h"
#include "QS_ports.h"
#include "QS_outputlog.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_adc.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_dma.h"

#define MAX_ADC_CHANNELS 19

#define ADC_CONTINUOUS_CONVERSION

static ADC_TypeDef* ADCx = ADC1;
static ADC_HandleTypeDef ADC_HandleStructure;
static Sint16 adc_converted_value[MAX_ADC_CHANNELS];
static Sint8 adc_id[MAX_ADC_CHANNELS];
static bool_e initialized = FALSE;

void ADC_init(void) {
	Uint8 i;
	if(initialized == TRUE)
		return;
	Uint8 number_of_channels;

	ADC_ChannelConfTypeDef ADC_configChannelStructure;
	DMA_HandleTypeDef DMA_HandleStructure;

#ifdef MAIN_IR_RCVA
	static const Uint8 ADC_sampleTime = ADC_SAMPLETIME_3CYCLES;
#else
	static const Uint8 ADC_sampleTime = ADC_SAMPLETIME_28CYCLES;
#endif


	/* Enable ADCx, DMA and GPIO clocks ****************************************/
	__HAL_RCC_GPIOC_CLK_ENABLE(); //Activation de l'horloge du GPIO utilise
	__HAL_RCC_DMA2_CLK_ENABLE(); //Activation de l'horloge du peripherique DMA
	__HAL_RCC_ADC1_CLK_ENABLE(); //Activation de l'horloge du peripherique ADC1

	number_of_channels = PORTS_adc_init(ADCx, ADC_sampleTime);


	/* ADC1 Init ****************************************************************/

	#ifdef ADC_CONTINUOUS_CONVERSION //Mode continu : ENABLE si mesures en continu. DISABLE si une seule mesure a effectuer.
		ADC_HandleStructure.Init.ContinuousConvMode = ENABLE;
	#else
		ADC_HandleStructure.Init.ContinuousConvMode = DISABLE;
	#endif

	#ifdef MAIN_IR_RCVA
		ADC_HandleStructure.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		ADC_HandleStructure.Init.Resolution = ADC_RESOLUTION_12B;
	#else
		ADC_HandleStructure.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
		ADC_HandleStructure.Init.Resolution = ADC_RESOLUTION_10B;
	#endif

	//Initialisation de ADC1
	ADC_HandleStructure.Instance = ADCx;
	ADC_HandleStructure.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC_HandleStructure.Init.ScanConvMode = ENABLE;
	ADC_HandleStructure.Init.EOCSelection = ADC_EOC_SEQ_CONV ;
	ADC_HandleStructure.Init.DMAContinuousRequests = ENABLE;
	ADC_HandleStructure.Init.NbrOfConversion = number_of_channels;
	ADC_HandleStructure.Init.DiscontinuousConvMode = DISABLE;
	ADC_HandleStructure.Init.NbrOfDiscConversion = 0;
	ADC_HandleStructure.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	ADC_HandleStructure.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;//Reglage du declencheur de la mesure (en cas de declenchement externe) None si pas de declenchement externe.

	if(HAL_ADC_Init(&ADC_HandleStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc\n");
	}

	// Initialisation de chaque channel
	ADC_configChannelStructure.SamplingTime = ADC_sampleTime;
	ADC_configChannelStructure.Offset = 0;

	// Initialisation du tableau des id des convertisseurs analogique numérique
	for(i=0;i<MAX_ADC_CHANNELS;i++)
		adc_id[i] = -1;

	// Attribution de l'id des convertisseurs utilisés
	i = 0;
#ifdef	USE_AN0
	ADC_configChannelStructure.Channel = ADC_CHANNEL_0;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_0] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 0\n");
	}
#endif
#ifdef	USE_AN1
	ADC_configChannelStructure.Channel = ADC_CHANNEL_1;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_1] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 1\n");
	}
#endif
#ifdef	USE_AN2
	ADC_configChannelStructure.Channel = ADC_CHANNEL_2;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_2] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 2\n");
	}
#endif
#ifdef	USE_AN3
	ADC_configChannelStructure.Channel = ADC_CHANNEL_3;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_3] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 3\n");
	}
#endif
#ifdef	USE_AN4
	ADC_configChannelStructure.Channel = ADC_CHANNEL_4;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_4] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 4\n");
	}
#endif
#ifdef	USE_AN5
	ADC_configChannelStructure.Channel = ADC_CHANNEL_5;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_5] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 5\n");
	}
#endif
#ifdef	USE_AN6
	ADC_configChannelStructure.Channel = ADC_CHANNEL_6;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_6] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 6\n");
	}
#endif
#ifdef	USE_AN7
	ADC_configChannelStructure.Channel = ADC_CHANNEL_7;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_7] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 7\n");
	}
#endif
#ifdef	USE_AN8
	ADC_configChannelStructure.Channel = ADC_CHANNEL_8;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_8] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 8\n");
	}
#endif
#ifdef	USE_AN9
	ADC_configChannelStructure.Channel = ADC_CHANNEL_9;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_9] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 9\n");
	}
#endif
#ifdef	USE_AN10
	ADC_configChannelStructure.Channel = ADC_CHANNEL_10;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_10] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 10\n");
	}
#endif
#ifdef	USE_AN11
	ADC_configChannelStructure.Channel = ADC_CHANNEL_11;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_11] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 11\n");
	}
#endif
#ifdef	USE_AN12
	ADC_configChannelStructure.Channel = ADC_CHANNEL_12;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_12] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 12\n");
	}
#endif
#ifdef	USE_AN13
	ADC_configChannelStructure.Channel = ADC_CHANNEL_13;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_13] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 13\n");
	}
#endif
#ifdef	USE_AN14
	ADC_configChannelStructure.Channel = ADC_CHANNEL_14;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_14] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 14\n");
	}
#endif
#ifdef	USE_AN15
	ADC_configChannelStructure.Channel = ADC_CHANNEL_15;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_15] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 15\n");
	}
#endif
#ifdef	USE_AN_TEMP_SENSOR
	ADC_configChannelStructure.Channel = ADC_CHANNEL_TEMPSENSOR;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_TEMP_SENSOR] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 16\n");
	}
#endif
#ifdef	USE_AN_VREFIN
	ADC_configChannelStructure.Channel = ADC_CHANNEL_VREFIN;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_VREFIN] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 17\n");
	}
#endif
#ifdef	USE_AN_VBAT
	ADC_configChannelStructure.Channel = ADC_CHANNEL_VBAT;
	ADC_configChannelStructure.Rank = i;
	adc_id[ADC_VBAT] = i++;
	if(HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_configChannelStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de l'adc channel 18\n");
	}
#endif

	/* DMA2 Stream 0, channel 0 configuration **************************************/
	DMA_HandleStructure.Instance = DMA2_Stream0;
	DMA_HandleStructure.Init.Channel = DMA_CHANNEL_0;
	DMA_HandleStructure.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_HandleStructure.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_HandleStructure.Init.MemInc = DMA_MINC_ENABLE;
	DMA_HandleStructure.Init.PeriphDataAlignment =  DMA_PDATAALIGN_HALFWORD ;
	DMA_HandleStructure.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD ;
	DMA_HandleStructure.Init.Mode = DMA_CIRCULAR;
	DMA_HandleStructure.Init.Priority =  DMA_PRIORITY_HIGH;
	DMA_HandleStructure.Init.FIFOMode =  DMA_FIFOMODE_DISABLE;
	DMA_HandleStructure.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
	DMA_HandleStructure.Init.MemBurst = DMA_MBURST_SINGLE;
	DMA_HandleStructure.Init.PeriphBurst =  DMA_PBURST_SINGLE;
	HAL_DMA_Init(&DMA_HandleStructure);

	__HAL_LINKDMA(&ADC_HandleStructure, DMA_Handle, DMA_HandleStructure);

	//Configuration des interruptions de la DMA
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);  // Revoir priorité des interruptions
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	HAL_ADC_Start_DMA(&ADC_HandleStructure, adc_converted_value, i);

	initialized = TRUE;
}


Sint16 ADC_getValue(adc_id_e channel) {
	if(initialized == FALSE){
		error_printf("ADC non initialisé ! Appeller ADC_init");
		return 0;
	}else if(adc_id[channel] == -1 || channel >= MAX_ADC_CHANNELS){
		error_printf("Lecture de la valeur du convertisseur analogique numérique %d non utilisé ou non initialisé ! \n", channel);
		return 0;
	}
#ifndef ADC_CONTINUOUS_CONVERSION
	/* Start conversions manually*/
	ADC_HandleStructure.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
	while(__HAL_ADC_GET_FLAG(&ADC_HandleStructure, ADC_FLAG_EOC) == RESET); //Tant qu'on a pas lu les registres
#endif
	return adc_converted_value[adc_id[channel]];
}
