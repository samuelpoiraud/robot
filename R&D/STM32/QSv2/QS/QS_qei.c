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

/*-------------------------------------
	Initialisation de l'interface
-------------------------------------*/
void QEI_init() 
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	PORTS_qei_init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3  | RCC_APB1Periph_TIM4 , ENABLE);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetAutoreload(TIM3, 0xFFFF);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetAutoreload(TIM4, 0xFFFF);

	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

/*-------------------------------------
	Recupération de la valeur du codeur
-------------------------------------*/
Sint16 QEI1_get_count()
{
	return (Sint16)TIM_GetCounter(TIM3);
}

void QEI1_set_count(Sint16 count)
{	
	TIM_SetCounter(TIM3, (Sint32)count);
}

Sint16 QEI2_get_count()
{
	return (Sint16)TIM_GetCounter(TIM4);
}

void QEI2_set_count(Sint16 count)
{
	TIM_SetCounter(TIM4, (Sint32)count);
}
