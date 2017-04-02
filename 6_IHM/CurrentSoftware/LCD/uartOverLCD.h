/*
 *	Club Robot ESEO 2016 - 2017
 *
 *	Fichier : xpt2046.h
 *	Package : IHM
 *	Description : Driver du XPT2046
 *	Auteur : Arnaud Guilmet
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100929
 */

#ifndef UART_OVER_LCD_H_
	#define UART_OVER_LCD_H_
	#include "../QS/QS_all.h"

	#ifdef USE_UART_OVER_LCD

		void UART_OVER_LCD_init(void);
		void UART_OVER_LCD_processMain(void);

	#endif

#endif //UART_OVER_LCD_H_
