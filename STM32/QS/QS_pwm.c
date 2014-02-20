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
#include "QS_clocks_freq.h"

/* Configuration du prédiviseur */
#if PWM_FREQ > 50000
	#error "Les ponts en H actuel ne supporte pas cette fréquence. Si c'est le cas un jour, veuillez passer ce #error en #warning. si tout ce qui existe au robot peut supporter 200Khz, veuillez supprimer ce warning/error"
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

static bool_e initialized = FALSE;

/* Configuration des PWM */
void PWM_init(void)
{
	if(initialized)
		return;

	/* Structures */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	PORTS_pwm_init();

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = PWM_PERIOD - 1;	//Le timer compte de 0 à period inclus
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

void PWM_set_frequency(Uint32 freq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	Uint32 presc32;
	if(!initialized)
		return;
	if(freq == 0)
		return;
	presc32 = (TIM_CLK2_FREQUENCY_HZ / freq / PWM_PERIOD) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = PWM_PERIOD - 1;	//Le timer compte de 0 à period inclus
	TIM_TimeBaseStructure.TIM_Prescaler     = (Uint16)presc32;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
}

/* Fonctions de manipulation des PWM */
void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel)
{
	if(!initialized)
		return;

	switch(channel)
	{
#ifdef USE_PWM1
		case 1:
			TIM_SetCompare1(TIM8, duty * (PWM_PERIOD/100));
			break;
#endif
#ifdef USE_PWM2
		case 2:
			TIM_SetCompare2(TIM8, duty * (PWM_PERIOD/100));
			break;
#endif
#ifdef USE_PWM3
		case 3:
			TIM_SetCompare3(TIM8, duty * (PWM_PERIOD/100));
			break;
#endif
#ifdef USE_PWM4
		case 4:
			TIM_SetCompare4(TIM8, duty * (PWM_PERIOD/100));
			break;
#endif
		default:
#ifdef VERBOSE_MODE
			if(channel > 0 && channel <= 4)
				fprintf(stderr, "PWM_run: PWM %d non active, USE_PWM%d n'a pas été défini dans la config_qs !\n", channel, channel);
			else
				fprintf(stderr, "PWM_run: PWM %d invalide, seulement les PWM de 1 à 4 incluses existent\n", channel);
			NVIC_SystemReset();
#endif
			break;
	}
}

void PWM_stop(Uint8 channel)
{
	PWM_run(0, channel);
}

#if PWM_PERIOD == PWM_FINE_DUTY
void PWM_run_fine(Uint16 duty, Uint8 channel)
{
	switch(channel)
	{
#ifdef USE_PWM1
		case 1:
			TIM_SetCompare1(TIM8, duty);
			break;
#endif
#ifdef USE_PWM2
		case 2:
			TIM_SetCompare2(TIM8, duty);
			break;
#endif
#ifdef USE_PWM3
		case 3:
			TIM_SetCompare3(TIM8, duty);
			break;
#endif
#ifdef USE_PWM4
		case 4:
			TIM_SetCompare4(TIM8, duty);
			break;
#endif
		default:
#ifdef VERBOSE_MODE
			if(channel > 0 && channel <= 4)
				fprintf(stderr, "PWM_run_fine: PWM %d non active, USE_PWM%d n'a pas été défini dans la config_qs !\n", channel, channel);
			else
				fprintf(stderr, "PWM_run_fine: PWM %d invalide, seulement les PWM de 1 à 4 incluses existent\n", channel);
			NVIC_SystemReset();
#endif
			break;
	}
}
#endif

#endif /* def PWM_PRESC */

