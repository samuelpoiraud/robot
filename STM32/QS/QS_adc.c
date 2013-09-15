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
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"

#define MAX_ADC_CHANNELS 16

#define ADC_CONTINUOUS_CONVERSION

static ADC_TypeDef* ADCx = ADC1;
static Sint16 adc_converted_value[MAX_ADC_CHANNELS];

void ADC_init(void) {
	static bool_e initialized = FALSE;
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

	initialized = TRUE;
}


Sint16 ADC_getValue(Uint8 channel) {
#ifndef ADC_CONTINUOUS_CONVERSION
	/* Start conversions manually*/
	ADC_SoftwareStartConv(ADCx);
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);	//Tant qu'on a pas lu les registres
#endif
	return adc_converted_value[channel];
}
