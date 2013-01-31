/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_pwm.h
 *  Package : Qualit� Soft
 *  Description : Gestion des PWM sur TIM1, TIM3, TIM4
 *  Auteur : Gwenn
 *  Version 20100424
 */

#ifndef QS_PWM_H
	#define QS_PWM_H
	
	#include "QS_all.h"
	#include "QS_sys.h"
	
	#ifdef USE_PWM
		
		/* D�finition d'un canal PWM */
		typedef enum {
			CH1,
			CH2,
			CH3,
			CH4
		} pwm_channel_t;
		
		/* Configure le module PWM */
		void PWM_init(void);
		

		/** D�marre ou arr�te la g�n�ration PWM sur la voie cibl�e
			* La PWM sera envoy�e sur TIMx_CHy , x �tant le timer, y la voie
			* Exemple: PWM_run(80, TIM1, CH1);
			* On pourra �crire: #define MY_PWM TIM1, CH1
			* Ce qui donne l'�criture: PWM_run(80, MY_PWM);
			* 
			* param duty Rapport cyclique en % � appliquer � la voie
			* param TIMx Timer concern�
			* param channel Voie sur laquelle g�n�rer le signal
		**/
		void PWM_run(Uint8 duty, TIM_TypeDef* TIMx, pwm_channel_t channel);
		
		/* Arr�te la PWM cibl�e */
		void PWM_stop(TIM_TypeDef* TIMx, pwm_channel_t channel);
		
		/* Arr�te toutes les PWM */
		void PWM_stop_all(void);
			
	#endif /* def USE_PWM */
#endif /* ndef QS_PWM_H */
