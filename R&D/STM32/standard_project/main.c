/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : main.c
 *  Package : Qualité Soft
 *  Description : Programme de test de la carte STM32 - allume quelques LEDs
 *  Auteur : Gwenn
 *  Version 20100424
 */

#include "main.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"


void USB_QS_init(){

	/* Structure */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Initialisation des horloges */
	RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLVCO_Div3);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE);
	
	/* Configuration NVIC */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EVAL_COM1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Démarrage */
	USB_Init();
}



int main(void)
{
	/* Variables	 */
	
	/* Modules QS */
	SYS_init();
	PORTS_init();

	LED_write(1);
	
	USB_QS_init();
	LED_write(2);
	
	/* Tâche de fond */
	while (1){
	}
	return 0;
}

