/*
 *	Club Robot ESEO 2010 - 2015
 *	CheckNorris
 *
 *	Fichier : Emission_ir.c
 *	Package : Projet Balise Emission IR
 *	Description : Gestion de l'émission IR...
 *	Auteur : Nirgal(PIC)/Arnaud(STM32)
 *	Version 201101
 */

#include "emission_ir.h"

#include "QS/QS_ports.h"
#include "QS/QS_pwm.h"
#include "stm32f4xx/stm32f4xx_gpio.h"
#include "stm32f4xx/stm32f4xx_tim.h"
#include "synchro_rf.h"

#define TIM_EMISSION_IR_PERIOD		  369
#define TIM_EMISSION_IR_PRESCALER		1
//Explication: On veut une PWM à 455kHz, la fréquence de fonctionnement des TSOP
//On a une horloge interne à 168MHz. ClockDivision = 1 et Prescaler = 1
//Donc l'horloge du timer est à 168MHz
//La période vaut 168MHz / 455kHz = 369

static volatile ir_modulation_e ir_modulation_check = 0x00;

static volatile bool_e PWM_initialized = FALSE;

static void EmissionIR_refresh();

void EmissionIR_init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPInit;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPInit.GPIO_Mode = GPIO_Mode_AF;
	GPInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPInit.GPIO_OType = GPIO_OType_PP;
	GPInit.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPInit.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPInit);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = TIM_EMISSION_IR_PERIOD - 1;	//Le timer compte de 0 à period inclus
	TIM_TimeBaseStructure.TIM_Prescaler     = TIM_EMISSION_IR_PRESCALER - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	PWM_initialized = TRUE;
}

void EmissionIR_activate(ir_modulation_e ir_modulation){
	ir_modulation_check |= ir_modulation;
	EmissionIR_refresh();
}

void EmissionIR_deactivate(ir_modulation_e ir_modulation){
	ir_modulation_check &= ~ir_modulation;
	EmissionIR_refresh();
}

static void EmissionIR_refresh(){
	if(ir_modulation_check == IR_MODULATION_ALL)
		TIM_SetCompare4(TIM1, TIM_EMISSION_IR_PERIOD/2);  //50% de rapport cyclique
	else
		TIM_SetCompare4(TIM1, TIM_EMISSION_IR_PERIOD);
}
