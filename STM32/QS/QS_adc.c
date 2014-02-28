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
#include "QS_all.h"
#include "QS_outputlog.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"

#define MAX_ADC_CHANNELS 16

#define ADC_CONTINUOUS_CONVERSION

static ADC_TypeDef* ADCx = ADC1;
static Sint16 adc_converted_value[MAX_ADC_CHANNELS];
static Sint8 adc_id[MAX_ADC_CHANNELS];

void ADC_init(void) {
	static bool_e initialized = FALSE;
	Uint8 i;
	if(initialized == TRUE)
		return;
	Uint8 number_of_channels;

	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	static const Uint8 ADC_sampleTime = ADC_SampleTime_28Cycles;

	/* Enable ADCx, DMA and GPIO clocks ****************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	//DMA2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//ADCx

	number_of_channels = PORTS_adc_init(ADCx, ADC_sampleTime);

	/* DMA2 Stream0 channel0 configuration **************************************/
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADCx->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&adc_converted_value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = number_of_channels;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
#ifdef ADC_CONTINUOUS_CONVERSION
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
#else
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
#endif
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = number_of_channels;
	ADC_Init(ADCx, &ADC_InitStructure);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADCx, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADCx, ENABLE);

#ifdef ADC_CONTINUOUS_CONVERSION
	ADC_SoftwareStartConv(ADCx);
#endif
	// Initialisation du tableau des id des convertisseurs analogique numérique
	for(i=0;i<MAX_ADC_CHANNELS;i++)
		adc_id[i] = -1;

	// Attribution de l'id des convertisseurs utilisés
	i = 0;
#ifdef	USE_AN0
	adc_id[ADC_0] = i++;
#endif
#ifdef	USE_AN1
	adc_id[ADC_1] = i++;
#endif
#ifdef	USE_AN2
	adc_id[ADC_2] = i++;
#endif
#ifdef	USE_AN3
	adc_id[ADC_3] = i++;
#endif
#ifdef	USE_AN4
	adc_id[ADC_4] = i++;
#endif
#ifdef	USE_AN5
	adc_id[ADC_5] = i++;
#endif
#ifdef	USE_AN6
	adc_id[ADC_6] = i++;
#endif
#ifdef	USE_AN7
	adc_id[ADC_7] = i++;
#endif
#ifdef	USE_AN8
	adc_id[ADC_8] = i++;
#endif
#ifdef	USE_AN9
	adc_id[ADC_9] = i++;
#endif
#ifdef	USE_AN10
	adc_id[ADC_10] = i++;
#endif
#ifdef	USE_AN11
	adc_id[ADC_11] = i++;
#endif
#ifdef	USE_AN12
	adc_id[ADC_12] = i++;
#endif
#ifdef	USE_AN13
	adc_id[ADC_13] = i++;
#endif
#ifdef	USE_AN14
	adc_id[ADC_14] = i++;
#endif
#ifdef	USE_AN15
	adc_id[ADC_15] = i++;
#endif

	initialized = TRUE;
}


Sint16 ADC_getValue(adc_id_e channel) {
	if(adc_id[channel] == -1 || channel >= MAX_ADC_CHANNELS){
		error_printf("Lecture de la valeur du convertisseur analogique numérique %d non utilisé ou non initialisé ! \n", channel);
		return 0;
	}
#ifndef ADC_CONTINUOUS_CONVERSION
	/* Start conversions manually*/
	ADC_SoftwareStartConv(ADCx);
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);	//Tant qu'on a pas lu les registres
#endif
	return adc_converted_value[adc_id[channel]];
}
