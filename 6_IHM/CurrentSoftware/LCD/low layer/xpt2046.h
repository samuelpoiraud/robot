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

/** ----------------  Defines possibles  --------------------
 *	IRQ_TOUCH					: GPIO de lecture de l'interruption
 *	USE_IRQ_TOUCH_VALIDATION	: Utiliser la broche d'interruption de tactile pour valider les données
 *	LCD_CS_TOUCH				: GPIO d'écriture du chip select
 *	LCD_TOUCH_SPI				: Le périphérique SPI utilisé pour le LCD
 *
 *	TODO : Gestion de IRQPen en interruption
 */

#ifndef _H_XPT2046_STM32F4
	#define _H_XPT2046_STM32F4
	#include "../../QS/QS_all.h"

	typedef enum{
		XPT2046_COORDINATE_RAW,
		XPT2046_COORDINATE_SCREEN_RELATIVE
	}XPT2046_coordinateMode_e;

	void XPT2046_init(void);
	void XPT2046_setConfig(void);
	bool_e XPT2046_getCoordinates(Sint16 * pX, Sint16 * pY, XPT2046_coordinateMode_e coordinateMode);
	bool_e XPT2046_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, XPT2046_coordinateMode_e coordinateMode);

#endif //_H_XPT2046_STM32F4
