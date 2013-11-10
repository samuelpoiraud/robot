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
static void BUTTON_TEST_button2();
static void BUTTON_TEST_button3();
static void BUTTON_TEST_button4();
static void BUTTON_TEST_button6();

void BUTTON_init()
{
	ADC_init();
	BUTTONS_init();

	BUTTONS_define_actions(BUTTON0,BUTTON_start, NULL, 1);
	BUTTONS_define_actions(BUTTON1,SELFTEST_ask_launch, NULL, 0);
	BUTTONS_define_actions(BUTTON2,BUTTON_TEST_button2, NULL, 1);
	BUTTONS_define_actions(BUTTON3,BUTTON_TEST_button3, NULL, 1);	//TODO : BOUTON MENU LCD
	BUTTONS_define_actions(BUTTON4,BUTTON_TEST_button4, NULL, 1);	//TODO : BOUTON MENU LCD
	//BUTTONS_define_actions(BUTTON5,BUFFER_flush, NULL, 0);
	#ifdef EEPROM_CAN_MSG_ENABLE
		BUTTONS_define_actions(BUTTON5,EEPROM_CAN_MSG_verbose_previous_match, NULL, 0);
	#else
		BUTTONS_define_actions(BUTTON5,SD_print_previous_match, NULL, 0);
	#endif
//	BUTTONS_define_actions(BUTTON3,BUTTON_servo,NULL,1);
//	BUTTONS_define_actions(BUTTON4,BUTTON_pi_rotation,BUTTON_translation,1);

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

static void BUTTON_TEST_button2() {
	LED_ROUGE = !LED_ROUGE;
}

static void BUTTON_TEST_button3() {
	LED_ORANGE = !LED_ORANGE;
}

static void BUTTON_TEST_button4() {
	LED_BLEU = !LED_BLEU;
}

static void BUTTON_TEST_button6() {
	LED_BLEU = !LED_BLEU;
	LED_ORANGE = !LED_ORANGE;
}
