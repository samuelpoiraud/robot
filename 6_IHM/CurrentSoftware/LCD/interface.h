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
 *
 *	TODO : Gestion de IRQPen en interruption
 */

#ifndef _H_INTERFACE
	#define _H_INTERFACE
	#include "../QS/QS_all.h"
	#include "../QS/QS_lcd_over_uart.h"

	typedef enum{
		INTERFACE_IHM_WAIT = -1,
		INTERFACE_IHM_CUSTOM = LCD_MENU_CUSTOM,
		INTERFACE_IHM_HOKUYO = LCD_MENU_HOKUYO,
		INTERFACE_IHM_POSITION = LCD_MENU_POSITION
	}INTERFACE_ihm_e;

	void INTERFACE_init(void);
	void INTERFACE_processMain(void);

	void INTERFACE_setInterface(INTERFACE_ihm_e ihm);

	bool_e INTERFACE_ihmAvailable(void);

#endif //_H_INTERFACE
