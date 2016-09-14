/*
 * QS_dac.c
 *
 *  Created on: 23 mars 2016
 *      Author: a.guilmet
 */

#include "QS_dac.h"
#include "QS_ports.h"
#include "../stm32f4xx/stm32f4xx_dac.h"

void DAC_init(void){

#if defined(USE_DAC1) || defined(USE_DAC2)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	PORTS_dac_init();
#endif

	DAC_InitTypeDef DAC_InitStructure;
	UNUSED_VAR(DAC_InitStructure);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

#ifdef USE_DAC1
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_setValueDAC1(0);
	DAC_Cmd(DAC_Channel_1, ENABLE);
#endif

#ifdef USE_DAC2
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_setValueDAC2(0);
	DAC_Cmd(DAC_Channel_2, ENABLE);
#endif

}

void DAC_setValueDAC1(Uint16 Data){
	DAC_SetChannel1Data(DAC_Align_12b_R, Data);
}

void DAC_setValueDAC2(Uint16 Data){
	DAC_SetChannel2Data(DAC_Align_12b_R, Data);
}

void DAC_setDualValueDAC(Uint16 Data1, Uint16 Data2){
	DAC_SetDualChannelData(DAC_Align_12b_R, Data2, Data1);
}
