/*
 *  Club Robot ESEO 2017
 *
 *  Fichier : buzzer.c
 *  Package : Balise émettrice
 *  Description : Gestion du buzzer
 *  Auteur : Arnaud
 *  Version 200904
 */

#ifndef _BUZZER_H
	#define _BUZZER_H

	#include "QS/QS_all.h"

	#define IS_BUZZER_MODE(x)	((x) == BUZZER_PIN_MODE \
								|| (x) == UART_PIN_MODE)

	typedef enum{
		NO_MODE,
		BUZZER_PIN_MODE,
		UART_PIN_MODE
	}BUZZER_mode_e;

	void BUZZER_init(void);
	void BUZZER_processMain(void);
	void BUZZER_switchMode(BUZZER_mode_e mode);

#endif	//def _IT_H
