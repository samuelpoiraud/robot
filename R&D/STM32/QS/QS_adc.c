/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_adc.h
 *  Package : Qualité Soft
 *  Description : Gestion du convertisseur analogique/numérique
 *  Auteur : Gwenn
 *  Version 20100425
 */


#include "QS_adc.h"


/** Sécurité d'initialisation **/		
static bool_e initialized = FALSE;


void ADC_init(void){
	if (initialized) return;
	
	/* Structures */
	ADC_InitTypeDef ADCInit;
	GPIO_InitTypeDef GPInit;
	
	/* Horloges */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	/* GPIO */ 
	GPIO_StructInit(&GPInit); 
	GPInit.GPIO_Speed = GPIO_Speed_50MHz; 
	GPInit.GPIO_Mode  = GPIO_Mode_AIN;
	GPInit.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPInit);
	GPInit.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPInit); 
	GPInit.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPInit); 

	/* Configuration */
	ADCInit.ADC_Mode = ADC_Mode_Independent;
	ADCInit.ADC_ScanConvMode = ENABLE;
	ADCInit.ADC_ContinuousConvMode = ENABLE;
	ADCInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADCInit.ADC_DataAlign = ADC_DataAlign_Right;
	ADCInit.ADC_NbrOfChannel = 1;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_28Cycles5); 
 
	
	/* Démarrage */
	ADC_Init(ADC1, &ADCInit);
	ADC_Cmd(ADC1, ENABLE);
	initialized = TRUE;
	
	/* Calibration de l'ADC */
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


Uint16 ADC_getValue(adc_channel_t channel){
	return ADC_GetConversionValue(ADC1);
}
