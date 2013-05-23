/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_pwm.c
 *  Package : Qualité Soft
 *  Description : Gestion des PWM sur TIM1, TIM3, TIM4
 *  Auteur : Gwenn
 *  Version 20100424
 */


#include "QS_pwm.h"
#include "QS_ports.h"
#include "stm32f4xx_tim.h"

#define FREQ_PWM_50KHZ

/* Configuration du prédiviseur */
//100 = 100 * 1 (period * prescaler)
#ifdef FREQ_PWM_200KHZ
	#define PWM_PRESC (PCLK2_FREQUENCY_HZ / 200000 / 100)
#endif
#ifdef FREQ_PWM_50KHZ
	#define PWM_PRESC (PCLK2_FREQUENCY_HZ / 50000 / 100)
#endif
#ifdef FREQ_PWM_20KHZ
	#define PWM_PRESC (PCLK2_FREQUENCY_HZ / 20000 / 100)
#endif
#ifdef FREQ_PWM_10KHZ
	#define PWM_PRESC (PCLK2_FREQUENCY_HZ / 10000 / 100)
#endif
#ifdef FREQ_PWM_1KHZ
	#define PWM_PRESC (PCLK2_FREQUENCY_HZ / 1000 / 100)
#endif
#ifdef FREQ_PWM_50HZ
	#define PWM_PRESC (PCLK2_FREQUENCY_HZ / 50 / 100)
#endif

#if PWM_PRESC > 65535
#error "PWM Period is too large !"
#endif

#ifdef PWM_PRESC



/** Configuration des PWM
	* PREDIV: Variable suivant fréquence demandée
	* PERIOD: 100
	* PULSE: Rapport cyclique en %
**/
void PWM_init(void) {
	static bool_e initialized = FALSE;
	if(initialized)
		return;

	/* Structures */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	Uint16 prescaler = 1;
	RCC_ClocksTypeDef clocksSpeed;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	PORTS_pwm_init();

	RCC_GetClocksFreq(&clocksSpeed);

	if(clocksSpeed.SYSCLK_Frequency / clocksSpeed.PCLK2_Frequency > 1)
		prescaler = 2;
	prescaler *= PWM_PRESC;

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = 100 - 1;	//Le timer compte de 0 à period inclus
	TIM_TimeBaseStructure.TIM_Prescaler     = prescaler - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 25;
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
// duty est en pourcent, il faut le multiplier par 250 pour l'avoir en "pour 25000"
void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel) {
	PWM_run_fine(((Uint16)duty)*250, channel);
}

void PWM_stop(Uint8 channel) {
	PWM_run_fine(0, channel);
}

void PWM_run_fine(Uint16 duty, Uint8 channel) {
	switch(channel) {
		case 1:
			TIM8->CCR1 = duty/250;
			break;

		case 2:
			TIM8->CCR2 = duty/250;
			break;

		case 3:
			TIM8->CCR3 = duty/250;
			break;

		case 4:
			TIM8->CCR4 = duty/250;
			break;
	}
}

#endif /* def PWM_PRESC */

