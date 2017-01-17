/*
 *  Club Robot ESEO 2017
 *
 *  Fichier : buzzer.c
 *  Package : Balise émettrice
 *  Description : Gestion du buzzer
 *  Auteur : Arnaud
 *  Version 200904
 */

#include "buzzer.h"
#include "QS/QS_ports.h"

typedef struct{
	bool_e actif;

	time32_t durationOn;
	time32_t durationOff;
	Uint8 count;

	Uint8 actualCount;
	time32_t initialTime;
}BUZZER_play_s;

volatile static BUZZER_mode_e currentMode = NO_MODE;
volatile static BUZZER_play_s currentPlay;


void BUZZER_init(void){
	currentPlay.actif = FALSE;
	BUZZER_switchMode(BUZZER_PIN_MODE);
	GPIO_ResetBits(BUZZER_PIN);
}

void BUZZER_processMain(void){

	if(currentPlay.actif){
		if(currentPlay.initialTime == 0)
			currentPlay.initialTime = global.absolute_time;

		if(currentPlay.initialTime != 0){
			time32_t duration = global.absolute_time - currentPlay.initialTime;
			if(duration > currentPlay.durationOn + currentPlay.durationOff){
				currentPlay.actualCount++;
				currentPlay.initialTime = 0;
				if(currentPlay.actualCount == currentPlay.count){
					currentPlay.actif = FALSE;
					GPIO_ResetBits(BUZZER_PIN);
				}
			}else if(duration < currentPlay.durationOn){
				GPIO_SetBits(BUZZER_PIN);
			}else if(duration > currentPlay.durationOn){
				GPIO_ResetBits(BUZZER_PIN);
			}
		}
	}
}

void BUZZER_switchMode(BUZZER_mode_e mode){
	assert(IS_BUZZER_MODE(mode));
	if(mode == currentMode)
		return;

	if(mode == BUZZER_PIN_MODE){
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}else{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
}

void BUZZER_play(time32_t durationOn, time32_t durationOff, Uint8 count){
	currentPlay.actualCount = 0;
	currentPlay.initialTime = 0;
	currentPlay.count = count;
	currentPlay.durationOn = durationOn;
	currentPlay.durationOff = durationOff;
	currentPlay.actif = TRUE;
}
