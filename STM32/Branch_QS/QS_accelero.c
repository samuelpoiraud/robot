/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : QS_accelero.c
 *  Package : Qualité Soft
 *  Description : Gestion de l'accélérométre de la catre STM32
 *  Auteur : Anthony
 */

#include "QS_accelero.h"
#include "../stm32f4xx/stm32f4_discovery_lis302dl.h"

static Sint8 Xlu,Ylu,Zlu;


void ACC_init(void){
	LIS302DL_InitTypeDef  LIS302DL_InitStruct;

	/* Set configuration of LIS302DL*/
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);
}

void ACC_read(void){
	Uint8 buffer_accelero[6];
	LIS302DL_Read(buffer_accelero, LIS302DL_OUT_X_ADDR, 6);

//	debug_printf("bufferX  %d\n",(Sint8){Buffer_accelero[0]});
//	debug_printf("bufferY  %d\n",(Sint8){Buffer_accelero[2]});
//	debug_printf("bufferZ  %d\n",(Sint8){Buffer_accelero[4]});

	Xlu = (Sint8){buffer_accelero[0]};
	Ylu = (Sint8){buffer_accelero[2]};
	Zlu = (Sint8){buffer_accelero[4]};
}

Sint8 ACC_getX(void){
	return Xlu;
}

Sint8 ACC_getY(void){
	return Ylu;
}

Sint8 ACC_getZ(void){
	return Zlu;
}

