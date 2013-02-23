/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : button.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion du bouton et de la biroute
 *	Auteur : Jacen & Ronan
 *	Version 20100408
 */
 
#define BUTTON_C
 
#include "button.h"

void BUTTON_init()
{
	ADC_init();
	BUTTONS_init();
	BUTTONS_define_actions(BUTTON1,BUTTON_start,NULL,1);
#ifndef FDP_2013
	BUTTONS_define_actions(BUTTON2,BUTTON_change_color, BUTTON_calibration,1);
#endif
//	BUTTONS_define_actions(BUTTON3,BUTTON_servo,NULL,1);
	BUTTONS_define_actions(BUTTON4,BUTTON_pi_rotation,BUTTON_translation,1);
}

void BUTTON_update() 
{
	static bool_e biroute_forgotten = TRUE;
	bool_e biroute_current;
						
	biroute_current = PIN_BIROUTE;
	
	BUTTONS_update();
	
	if (biroute_current)
	{
		biroute_forgotten = FALSE;
	}
	
	/* regarder si le match doit commencer */
	if((biroute_forgotten == FALSE && !biroute_current))
	{
		global.env.ask_start = TRUE;
	}	
}

void BUTTON_start()
{
	debug_printf("START\r\n");
	global.env.ask_start = TRUE;
}

void BUTTON_calibration()
{
	debug_printf("CALIBRATION\r\n");
	global.env.ask_asser_calibration = TRUE;
}

void BUTTON_change_color()
{
	debug_printf("COLOR\r\n");
	global.env.color_updated = TRUE;
	global.env.wanted_color = ((global.env.color!=RED)?RED:BLUE);
}

void SWITCH_change_color()
{
	if(SWITCH_COLOR == global.env.color)	
	{	
		global.env.color_updated = TRUE;
		global.env.wanted_color = ((SWITCH_COLOR==1)?RED:BLUE);
		debug_printf("COLOR\r\n");
	}	
}	

void BUTTON_rotation()
{
	ASSER_push_goangle ((-PI4096)/2, FAST, TRUE);
}

void BUTTON_pi_rotation()
{
	ASSER_push_relative_goangle (PI4096,FAST,TRUE);
}
void BUTTON_translation()
{
	relative_move (1000, FAST, TRUE);
}
