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


/** Sécurité d'initialisation **/		
static bool_e initialized = FALSE;


/* Configuration du prédiviseur */
#ifdef FREQ_PWM_50KHZ
	#define PWM_PRESC FREQUENCY / 5
#endif
#ifdef FREQ_PWM_20KHZ
	#define PWM_PRESC FREQUENCY / 2
#endif
#ifdef FREQ_PWM_10KHZ
	#define PWM_PRESC FREQUENCY
#endif
#ifdef FREQ_PWM_1KHZ
	#define PWM_PRESC FREQUENCY * 10
#endif
#ifdef FREQ_PWM_50HZ
	#define PWM_PRESC FREQUENCY * 200
#endif


/* Initialise le module PWM sur le timer ciblé */
void PWM_init_by_timer(TIM_TypeDef* TIMx);

/* Lance une PWM sur le timer et la voie ciblée */
void PWM_run_by_timer(Uint8 duty, TIM_TypeDef* TIMx, pwm_channel_t channel);


/** Configuration des PWM
	* PREDIV: Variable suivant fréquence demandée
	* PERIOD: 100
	* PULSE: Rapport cyclique en %
**/
void PWM_init(void){
	if (initialized) return;
	TIMER_init();
	
	/* Structures */
	GPIO_InitTypeDef GPInit;
	TIM_TypeDef* TIMx = TIM3;
	
	/* Configuration GPIO et remappings */
	GPInit.GPIO_Mode = GPIO_Mode_AF_PP;
	GPInit.GPIO_Speed = GPIO_Speed_50MHz;

	GPInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPInit);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

	GPInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPInit);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

	GPInit.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPInit);
	
	/* Réglage des bases de temps et démarrage à 0% */
	#ifdef USE_PWM1
		PWM_init_by_timer(TIM1);
	#endif
	PWM_init_by_timer(TIM3);
	PWM_init_by_timer(TIM4);
	initialized = TRUE;
}


/* Fonction utilitaire d'initialisation par timer */
void PWM_init_by_timer(TIM_TypeDef* TIMx){
	
	/* Temps */
	TIM_TimeBaseInitTypeDef TInit;
	TIM_TimeBaseStructInit(&TInit);
	TInit.TIM_Period = 100;
	TInit.TIM_Prescaler = PWM_PRESC;
	TInit.TIM_ClockDivision = TIM_CKD_DIV4;
	TInit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TInit);
	
	/* Preload */
	TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
	
	/* Mise à zéro de toutes les PWM */
	PWM_run_by_timer(0, TIMx, CH1);
	PWM_run_by_timer(0, TIMx, CH2);
	PWM_run_by_timer(0, TIMx, CH3);
	PWM_run_by_timer(0, TIMx, CH4);
	
	/* Démarrage du timer ciblé */
	TIM_Cmd(TIMx, ENABLE);
}

/* Fonction utilitaire qui met à jour le registre PULSE avec la valeur indiquée */
void PWM_run_by_timer(Uint8 duty, TIM_TypeDef* TIMx, pwm_channel_t channel){
	TIM_OCInitTypeDef PWMInit;

	/* Configuration PWM */
	PWMInit.TIM_OCMode = TIM_OCMode_PWM1;
	PWMInit.TIM_OutputState = TIM_OutputState_Enable;
	PWMInit.TIM_Pulse = duty;
	PWMInit.TIM_OCPolarity = TIM_OCPolarity_High;
	
	/*  Démarrage */
	switch(channel){
		case CH1:
			TIM_OC1Init(TIMx, &PWMInit);
			break;
		case CH2:
			TIM_OC2Init(TIMx, &PWMInit);
			break;
		case CH3:
			TIM_OC3Init(TIMx, &PWMInit);
			break;
		case CH4:
			TIM_OC4Init(TIMx, &PWMInit);
			break;
	}
}


/* Fonctions de manipulation des PWM */
void PWM_run(Uint8 duty, TIM_TypeDef* TIMx, pwm_channel_t channel){
	PWM_run_by_timer(duty, TIMx, channel);	
}
void PWM1_stop(TIM_TypeDef* TIMx, pwm_channel_t channel){
	PWM_run_by_timer(0, TIMx, channel);	
}

void PWM_stop_all(void){
	#ifdef USE_PWM1
		PWM1_stop(CH1);
		PWM1_stop(CH2);
		PWM1_stop(CH3);
		PWM1_stop(CH4);
	#endif
	
	PWM3_stop(CH1);
	PWM3_stop(CH2);
	PWM3_stop(CH3);
	PWM3_stop(CH4);

	PWM4_stop(CH1);
	PWM4_stop(CH2);
	PWM4_stop(CH3);
	PWM4_stop(CH4);
}
