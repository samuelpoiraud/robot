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
#include "stm32f4xx_tim.h"
#include "QS_outputlog.h"

/*-------------------------------------
	Initialisation de l'interface
-------------------------------------*/

static bool_e initialized = FALSE;

void QEI_init()
{
	if(initialized)
		return;
	initialized = TRUE;

	PORTS_qei_init();

	#if defined(USE_QUEI1) || defined(USE_QUEI2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3  | RCC_APB1Periph_TIM2 , ENABLE);
	#endif

	#ifdef USE_QUEI1
		TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		TIM_SetAutoreload(TIM3, 0xFFFF);
		TIM_Cmd(TIM3, ENABLE);
	#endif

	#ifdef USE_QUEI2
		TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		TIM_SetAutoreload(TIM2, 0xFFFF);
		TIM_Cmd(TIM2, ENABLE);
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

		return (Sint16)TIM_GetCounter(TIM3);
	}

	void QEI1_set_count(Sint16 count)
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return;
		}

		TIM_SetCounter(TIM3, (Sint32)count);
	}
#endif

#ifdef USE_QUEI2
	Sint16 QEI2_get_count()
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return 0;
		}

		return (Sint16)TIM_GetCounter(TIM2);
	}

	void QEI2_set_count(Sint16 count)
	{
		if(!initialized){
			error_printf("QEI non initialisé ! Appeller QEI_init\n");
			return;
		}

		TIM_SetCounter(TIM2, (Sint32)count);
	}
#endif
