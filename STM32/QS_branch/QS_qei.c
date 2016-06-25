/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_qei.c
 *	Package : Qualite Software
 *	Description :	Utilise le module QEI pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_qei.h"
#include "QS_ports.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_tim.h"
#include "QS_outputlog.h"

/*-------------------------------------
	Initialisation de l'interface
-------------------------------------*/

static bool_e initialized = FALSE;
static TIM_HandleTypeDef TIM_HandleStructure_2;
static TIM_HandleTypeDef TIM_HandleStructure_3;

void QEI_init()
{
	if(initialized)
		return;
	initialized = TRUE;

	PORTS_qei_init();

	#if defined(USE_QUEI1) || defined(USE_QUEI2)
		 __HAL_RCC_TIM2_CLK_ENABLE();
		 __HAL_RCC_TIM3_CLK_ENABLE();
	#endif

	//Paramétrage des timers
	TIM_HandleStructure_2.Instance = TIM2;
	TIM_HandleStructure_2.Init.Period = 65535;

	TIM_HandleStructure_3.Instance = TIM2;
	TIM_HandleStructure_3.Init.Period = 65535;

	//Parametrage des codeurs
	TIM_Encoder_InitTypeDef enc_init;
	enc_init.EncoderMode = TIM_ENCODERMODE_TI12;

	enc_init.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
	enc_init.IC2Polarity = TIM_INPUTCHANNELPOLARITY_RISING;

	enc_init.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	enc_init.IC2Selection = TIM_ICSELECTION_DIRECTTI;

	enc_init.IC1Prescaler = TIM_ICPSC_DIV1;
	enc_init.IC2Prescaler = TIM_ICPSC_DIV1;

	enc_init.IC1Filter = 0;
	enc_init.IC2Filter = 0;

	#ifdef USE_QUEI1
		  HAL_TIM_Encoder_Init(&TIM_HandleStructure_3, &enc_init);
		  __HAL_TIM_SetCounter(&TIM_HandleStructure_3,0);
		  HAL_TIM_Encoder_Start(&TIM_HandleStructure_3, TIM_CHANNEL_1 | TIM_CHANNEL_2);
	#endif

	#ifdef USE_QUEI2
		  HAL_TIM_Encoder_Init(&TIM_HandleStructure_2, &enc_init);
		  __HAL_TIM_SetCounter(&TIM_HandleStructure_2,0);
		  HAL_TIM_Encoder_Start(&TIM_HandleStructure_2, TIM_CHANNEL_1 | TIM_CHANNEL_2);
	#endif
}

/*-------------------------------------
	Recupération de la valeur du codeur
-------------------------------------*/
#ifdef USE_QUEI1
	Sint16 QEI1_get_count()
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return 0;
		}

		return -__HAL_TIM_GET_COUNTER(TIM_HandleStructure_3);
	}

	void QEI1_set_count(Sint16 count)
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return;
		}

		__HAL_TIM_SET_COUNTER(TIM_HandleStructure_3, (Sint32)count);
	}
#endif

#ifdef USE_QUEI2
	Sint16 QEI2_get_count()
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return 0;
		}

		return -__HAL_TIM_GET_COUNTER(TIM_HandleStructure_2);
	}

	void QEI2_set_count(Sint16 count)
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return;
		}

		__HAL_TIM_SET_COUNTER(TIM_HandleStructure_2, (Sint32)count);
	}
#endif
