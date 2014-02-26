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

#include "QS/QS_outputlog.h"
#include "button.h"
#include "act_functions.h"
#include "Supervision/Selftest.h"
#include "Supervision/Eeprom_can_msg.h"
#include "Supervision/SD/SD.h"
#include "config_use.h"
#include "config_pin.h"
#include "Supervision/LCD_interface.h"

/*
static void BUTTON_TEST_button2();
static void BUTTON_TEST_button3();
static void BUTTON_TEST_button4();
static void BUTTON_TEST_button6();
*/
void BUTTON_verbose(void);

void BUTTON_init()
{
	ADC_init();
	BUTTONS_init();

	BUTTONS_define_actions(BUTTON0,BUTTON_start, NULL, 1);
	BUTTONS_define_actions(BUTTON1,SELFTEST_ask_launch, NULL, 0);
	BUTTONS_define_actions(BUTTON2,LCD_button_ok, NULL, 1);
	BUTTONS_define_actions(BUTTON3,LCD_button_minus, NULL, 1);
	BUTTONS_define_actions(BUTTON4,LCD_button_plus, NULL, 1);

	#ifdef EEPROM_CAN_MSG_ENABLE
		BUTTONS_define_actions(BUTTON5,EEPROM_CAN_MSG_verbose_previous_match, NULL, 0);
	#else
		BUTTONS_define_actions(BUTTON5,SD_print_previous_match, NULL, 0);
	#endif
	BUTTONS_define_actions(BUTTON6,LCD_button_set, NULL, 1);

}

void BUTTON_update()
{
	static bool_e biroute_forgotten = TRUE;
	bool_e biroute_current;

	biroute_current = BIROUTE;

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

	BUTTON_verbose();
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
	relative_move (1000, FAST, FORWARD, END_AT_LAST_POINT);
}

void BUTTON_verbose(void)
{
	#define SW_BP_NUMBER 20
	static Uint32 previous_state = 0;
	Uint32 current_state, up, down;

	current_state = 	(BUTTON0_PORT	<< 0) 	|	//Run match
						(SWITCH_DEBUG	<< 1) 	|
						(SWITCH_VERBOSE	<< 2) 	|
						(BUTTON5_PORT	<< 3) 	| //Print match
						(SWITCH_XBEE	<< 4) 	|
						(SWITCH_SAVE	<< 5) 	|
						(FRESCO_1		<< 6) 	|
						(FRESCO_2		<< 7) 	|
						(BUTTON6_PORT	<< 8) 	|	 //BP Set
						(SWITCH_COLOR	<< 9) 	|
						(BIROUTE		<< 10) 	|
						(SWITCH_LCD		<< 11) 	|
						(SWITCH_EVIT	<< 12) 	|
						(SWITCH_STRAT_1	<< 13) 	|
						(SWITCH_STRAT_2	<< 14) 	|
						(SWITCH_STRAT_3	<< 15) 	|
						(BUTTON1_PORT	<< 16) 	|	//Selftest
						(BUTTON2_PORT	<< 17) 	|	//LCD OK
						(BUTTON3_PORT	<< 18) 	|	//LCD Menu +
						(BUTTON4_PORT	<< 19);	//LCD Menu

	up = ~previous_state & current_state;
	down = previous_state & ~current_state;
	previous_state = current_state;

	if(up & ((Uint32)(1) << 0	))	debug_printf("BP run match pressed\n");
	if(up & ((Uint32)(1) << 1	))	debug_printf("SW debug enabled\n");
	if(up & ((Uint32)(1) << 2	))	debug_printf("SW verbose enabled\n");
	if(up & ((Uint32)(1) << 3	))	debug_printf("BP print match pressed\n");
	if(up & ((Uint32)(1) << 4	))	debug_printf("SW XBEE enabled\n");
	if(up & ((Uint32)(1) << 5	))	debug_printf("SW saved enabled\n");
	if(up & ((Uint32)(1) << 6	))	debug_printf("FRESCO1 added\n");
	if(up & ((Uint32)(1) << 7	))	debug_printf("FRESCO2 added\n");
	if(up & ((Uint32)(1) << 8	))	debug_printf("BP Set pressed\n");
	if(up & ((Uint32)(1) << 9	))	debug_printf("SW color changed\n");
	if(up & ((Uint32)(1) << 10	))	debug_printf("Biroute inserted\n");
	if(up & ((Uint32)(1) << 11	))	debug_printf("SW LCD : coord/config\n");
	if(up & ((Uint32)(1) << 12	))	debug_printf("SW Evit enabled\n");
	if(up & ((Uint32)(1) << 13	))	debug_printf("SW Strat1 enabled\n");
	if(up & ((Uint32)(1) << 14	))	debug_printf("SW Strat2 enabled\n");
	if(up & ((Uint32)(1) << 15	))	debug_printf("SW Strat3 enabled\n");
	if(up & ((Uint32)(1) << 16	))	debug_printf("BP Selftest pressed\n");
	if(up & ((Uint32)(1) << 17	))	debug_printf("BP OK pressed\n");
	if(up & ((Uint32)(1) << 18	))	debug_printf("BP MenuUp pressed\n");
	if(up & ((Uint32)(1) << 19	))	debug_printf("BP MenuDown pressed\n");

	if(down & ((Uint32)(1) << 1	))	debug_printf("SW debug disabled\n");
	if(down & ((Uint32)(1) << 2	))	debug_printf("SW verbose disabled\n");
	if(down & ((Uint32)(1) << 4	))	debug_printf("SW XBEE disabled\n");
	if(down & ((Uint32)(1) << 5	))	debug_printf("SW saved disabled\n");
	if(down & ((Uint32)(1) << 6	))	debug_printf("FRESCO1 removed\n");
	if(down & ((Uint32)(1) << 7	))	debug_printf("FRESCO2 removed\n");
	if(down & ((Uint32)(1) << 9	))	debug_printf("SW color changed\n");
	if(down & ((Uint32)(1) << 10	))	debug_printf("Biroute removed\n");
	if(down & ((Uint32)(1) << 11	))	debug_printf("SW LCD : CAN msgs\n");
	if(down & ((Uint32)(1) << 12	))	debug_printf("SW Evit disabled\n");
	if(down & ((Uint32)(1) << 13	))	debug_printf("SW Strat1 disabled\n");
	if(down & ((Uint32)(1) << 14	))	debug_printf("SW Strat2 disabled\n");
	if(down & ((Uint32)(1) << 15	))	debug_printf("SW Strat3 disabled\n");

}
