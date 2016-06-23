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

#ifdef USE_PWM_MODULE

#include "QS_ports.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_tim.h"
#include "QS_clocks_freq.h"
#include "QS_outputlog.h"

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
	TIM_HandleTypeDef TIM_HandleStructure;
	TIM_OC_InitTypeDef TIM_OCInitStructure;

#if !defined(USE_PWM1) && !defined(USE_PWM2) && !defined(USE_PWM3) && !defined(USE_PWM4)
	UNUSED_VAR(TIM_HandleStructure);
	UNUSED_VAR(TIM_OCInitStructure);
#endif


	__HAL_RCC_TIM8_CLK_ENABLE();

	PORTS_pwm_init();

	TIM_HandleStructure.Instance = TIM8;
	TIM_HandleStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM_HandleStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_HandleStructure.Init.Period = PWM_PERIOD - 1;	//Le timer compte de 0 à period inclus
	TIM_HandleStructure.Init.Prescaler = PWM_PRESC - 1;
	TIM_HandleStructure.Init.RepetitionCounter = 0;
	if(HAL_TIM_Base_Init(&TIM_HandleStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de la pwm channel 1\n");
	}
	__HAL_TIM_ENABLE(&TIM_HandleStructure);

	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE; // (only available with PWM1 and PWM2 mode)
	TIM_OCInitStructure.Pulse = 0;
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_OC_Init(&TIM_HandleStructure);

#ifdef USE_PWM1
	HAL_TIM_OC_ConfigChannel(&TIM_HandleStructure, &TIM_OCInitStructure, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_HandleStructure, TIM_CHANNEL_2);
#endif

#ifdef USE_PWM2
	HAL_TIM_OC_ConfigChannel(&TIM_HandleStructure, &TIM_OCInitStructure, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_HandleStructure, TIM_CHANNEL_2);
#endif

#ifdef USE_PWM3
	HAL_TIM_OC_ConfigChannel(&TIM_HandleStructure, &TIM_OCInitStructure, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TIM_HandleStructure, TIM_CHANNEL_3);
#endif

#ifdef USE_PWM4
	HAL_TIM_OC_ConfigChannel(&TIM_HandleStructure, &TIM_OCInitStructure, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&TIM_HandleStructure, TIM_CHANNEL_4);
#endif

	initialized = TRUE;
}

void PWM_set_frequency(Uint32 freq)
{
	if(!initialized){
		error_printf("PWM non initialisé ! Appeler PWM_init\n");
		return;
	}

	assert(freq != 0);

	TIM_HandleTypeDef TIM_HandleStructure;
	TIM_HandleStructure.Instance = TIM8;
	Uint32 presc32;
	presc32 = (TIM_CLK2_FREQUENCY_HZ / freq / PWM_PERIOD) - 1;
	TIM_HandleStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM_HandleStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_HandleStructure.Init.Period = PWM_PERIOD - 1;	//Le timer compte de 0 à period inclus
	TIM_HandleStructure.Init.Prescaler = (Uint16)presc32;
	TIM_HandleStructure.Init.RepetitionCounter = 0;

	if(HAL_TIM_Base_Init(&TIM_HandleStructure) != HAL_OK)
	{
		debug_printf("Erreur lors de l'initialisation de la pwm channel 1\n");
	}
}

/* Fonctions de manipulation des PWM */
void PWM_run(Uint8 duty /* en pourcents*/, Uint8 channel)
{
	if(!initialized){
		error_printf("PWM non initialisé ! Appeler PWM_init\n");
		return;
	}

	assert(duty >= 0 && duty <= 100);

	switch(channel)
	{
#ifdef USE_PWM1
		case 1:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH1, TIM_CHANNEL_1, (uint32_t)duty);
			break;
#endif
#ifdef USE_PWM2
		case 2:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH2, TIM_CHANNEL_2, (uint32_t)duty);
			break;
#endif
#ifdef USE_PWM3
		case 3:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH3, TIM_CHANNEL_3, (uint32_t)duty);
			break;
#endif
#ifdef USE_PWM4
		case 4:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH4, TIM_CHANNEL_4, (uint32_t)duty);
			break;
#endif
		default:
#ifdef VERBOSE_MODE
			if(channel > 0 && channel <= 4)
				fprintf(stderr, "PWM_run: PWM %d non active, USE_PWM%d n'a pas été défini dans la config_use !\n", channel, channel);
			else
				fprintf(stderr, "PWM_run: PWM %d invalide, seulement les PWM de 1 à 4 incluses existent\n", channel);
			NVIC_SystemReset();
#endif
			break;
	}
}

Uint8 PWM_get_duty(Uint8 channel){

	if(!initialized){
		error_printf("PWM non initialisé ! Appeler PWM_init\n");
		return 0;
	}

	switch(channel)
	{
		case 1:
			return TIM8->CCR1 * 100/PWM_PERIOD;

		case 2:
			return TIM8->CCR2 * 100/PWM_PERIOD;

		case 3:
			return TIM8->CCR3 * 100/PWM_PERIOD;

		case 4:
			return TIM8->CCR4 * 100/PWM_PERIOD;

		default:
		#ifdef VERBOSE_MODE
			fprintf(stderr, "PWM_get_duty: PWM %d invalide, seulement les PWM de 1 à 4 incluses existent\n", channel);
		#endif
			return 0;
	}
}

void PWM_stop(Uint8 channel)
{
	PWM_run(0, channel);
}

#if PWM_PERIOD == PWM_FINE_DUTY
void PWM_run_fine(Uint16 duty, Uint8 channel)
{

	if(!initialized){
		error_printf("PWM non initialisé ! Appeller PWM_init\n");
		return;
	}

	assert(duty >= 0 && duty <= 100);

	switch(channel)
	{
#ifdef USE_PWM1
		case 1:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH1, TIM_CHANNEL_1, (uint32_t)duty);
			break;
#endif
#ifdef USE_PWM2
		case 2:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH2, TIM_CHANNEL_2, (uint32_t)duty);
			break;
#endif
#ifdef USE_PWM3
		case 3:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH3, TIM_CHANNEL_3, (uint32_t)duty);
			break;
#endif
#ifdef USE_PWM4
		case 4:
			__HAL_TIM_SET_COMPARE(&TIM_HandleStructure_CH4, TIM_CHANNEL_4, (uint32_t)duty);
			break;
#endif
		default:
#ifdef VERBOSE_MODE
			if(channel > 0 && channel <= 4)
				fprintf(stderr, "PWM_run_fine: PWM %d non active, USE_PWM%d n'a pas été défini dans la config_use !\n", channel, channel);
			else
				fprintf(stderr, "PWM_run_fine: PWM %d invalide, seulement les PWM de 1 à 4 incluses existent\n", channel);
			NVIC_SystemReset();
#endif
			break;
	}
}
#endif

#endif /* def PWM_PRESC */

#endif /* def USE_PWM_MODULE */
