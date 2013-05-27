/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_pwm.c
 *  Package : Qualit� Soft
 *  Description : Gestion des PWM sur TIM1, TIM3, TIM4
 *  Auteur : Gwenn
 *  Version 20100424
 */


#include "QS_pwm.h"
#include "QS_ports.h"
#include "stm32f4xx_tim.h"
#include "QS_clocks_freq.h"

/* Configuration du pr�diviseur */
#if PWM_FREQ > 50000
	#error "Les ponts en H actuel ne supporte pas cette fr�quence. Si c'est le cas un jour, veuillez passer ce #error en #warning. si tout ce qui existe au robot peut supporter 200Khz, veuillez supprimer ce warning/error"
#endif

#if PWM_FREQ <= (TIM_CLK2_FREQUENCY_HZ / PWM_FINE_DUTY)
	#define PWM_PERIOD PWM_FINE_DUTY
#else
	#define PWM_PERIOD PWM_HIGH_SPEED_DUTY
#endif
#define PWM_PRESC (TIM_CLK2_FREQUENCY_HZ / PWM_FREQ / PWM_PERIOD)


#if PWM_PRESC > 65535
#error "PWM Period is too low !"
#endif
#if PWM_PRESC == 0
#error "PWM Period is too large !"
#endif

#ifdef PWM_PRESC

/* Configuration des PWM */
void PWM_init(void) {
	static bool_e initialized = FALSE;
	if(initialized)
		return;

	/* Structures */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	PORTS_pwm_init();

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = PWM_PERIOD - 1;	//Le timer compte de 0 � period inclus
	TIM_TimeBaseStructure.TIM_Prescaler     = PWM_PRESC - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_Cmd(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);

	initialized = TRUE;
}

/* Fonctions de manipulation des PWM */
void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel) {
	switch(channel) {
		case 1:
			TIM_SetCompare1(TIM8, duty * (PWM_PERIOD/100));
			break;

		case 2:
			TIM_SetCompare2(TIM8, duty * (PWM_PERIOD/100));
			break;

		case 3:
			TIM_SetCompare3(TIM8, duty * (PWM_PERIOD/100));
			break;

		case 4:
			TIM_SetCompare4(TIM8, duty * (PWM_PERIOD/100));
			break;
	}
}

void PWM_stop(Uint8 channel) {
	PWM_run(0, channel);
}

#if PWM_PERIOD == PWM_FINE_DUTY
void PWM_run_fine(Uint16 duty, Uint8 channel) {
	switch(channel) {
		case 1:
			TIM_SetCompare1(TIM8, duty);
			break;

		case 2:
			TIM_SetCompare2(TIM8, duty);
			break;

		case 3:
			TIM_SetCompare3(TIM8, duty);
			break;

		case 4:
			TIM_SetCompare4(TIM8, duty);
			break;
	}
}
#endif

#endif /* def PWM_PRESC */

