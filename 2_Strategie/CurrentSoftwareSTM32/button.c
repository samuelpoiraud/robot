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
#include "actuator/act_functions.h"
#include "elements.h"
#include "main.h"
#include "Supervision/Selftest.h"
#include "Supervision/LCD_interface.h"
#include "Supervision/SD/SD.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_IHM.h"
#include "QS/QS_who_am_i.h"


/*
static void BUTTON_TEST_button2();
static void BUTTON_TEST_button3();
static void BUTTON_TEST_button4();
static void BUTTON_TEST_button6();
*/
void BUTTON_verbose(void);
static void BUTTON_suspend_match();

void BUTTON_init()
{
	ADC_init();
	BUTTONS_init();
	BUTTONS_define_actions(BUTTON0,BUTTON_start, NULL, 1);

	IHM_define_act_button(BP_0_IHM,NULL, BUTTON_0_long_push);
	IHM_define_act_button(BP_3_IHM,NULL, BUTTON_3_long_push);
	IHM_define_act_button(BP_4_IHM,NULL, BUTTON_4_long_push);
	IHM_define_act_button(BP_5_IHM,NULL, BUTTON_5_long_push);
	IHM_define_act_button(BP_GO_TO_HOME,BOUTTON_go_to_home, NULL);
	IHM_define_act_button(BP_OK,LCD_button_ok, NULL);
	IHM_define_act_button(BP_UP,LCD_button_up, LCD_button_up);
	IHM_define_act_button(BP_DOWN,LCD_button_down, LCD_button_down);
	IHM_define_act_button(BP_PRINTMATCH,SD_print_previous_match, NULL);
	IHM_define_act_button(BP_SET,LCD_button_set, NULL);
	IHM_define_act_button(BP_SUSPEND_RESUME_MATCH, BUTTON_suspend_match, NULL);
}


void BUTTON_update()
{
	BUTTONS_update();
	BUTTON_verbose();
}

void BUTTON_start()
{
	debug_printf("START\r\n");
	global.flags.ask_start = TRUE;
}

void BUTTON_calibration()
{
	debug_printf("CALIBRATION\r\n");
	global.flags.ask_prop_calibration = TRUE;
}

void BUTTON_change_color()
{
	debug_printf("COLOR\r\n");
	global.flags.color_updated = TRUE;
	global.wanted_color = ((global.color!=BOT_COLOR)?BOT_COLOR:TOP_COLOR);
}

void SWITCH_change_color()
{
	if((IHM_switchs_get(SWITCH_COLOR) != (global.color == 1)? TRUE:FALSE) || global.color == COLOR_INIT_VALUE){
		global.flags.color_updated = TRUE;
		global.wanted_color = ((IHM_switchs_get(SWITCH_COLOR)==1)?TOP_COLOR:BOT_COLOR);
		debug_printf("COLOR\r\n");
	}

}

static void BUTTON_suspend_match(){
	if(global.flags.match_started){
		if(!global.flags.match_suspended)
			global.flags.ask_suspend_match = TRUE;
	}
}

void BOUTTON_go_to_home(void){
	debug_printf("Flag Boutton go to home activé\n");
	global.flags.go_to_home = TRUE;
}

void BUTTON_0_long_push(void){
	/*if(I_AM_BIG()){
		static Uint8 state = 0;
		if(state == 0)
			ACT_push_order(ACT_POMPE_ALL, ACT_POMPE_ALL_NORMAL_5);
		else if(state == 1)
			ACT_push_order(ACT_POMPE_ALL, ACT_POMPE_ALL_STOP);

		state = (state==1)? 0: state+1;
	}*/
}

void BUTTON_3_long_push(void){}

void BUTTON_4_long_push(void){}

void BUTTON_5_long_push(void){}


void BUTTON_verbose(void)
{
	#define SW_BP_NUMBER 20
	static Uint32 previous_state = 0;
	Uint32 current_state = 0x00, up, down;
	bool_e change;

	current_state = 	(BUTTON0_PORT						<< 0) 	|	//Run match
						(IHM_switchs_get(SWITCH_RAW_DATA)	<< 1) 	|
						(IHM_switchs_get(SWITCH_VERBOSE)	<< 2) 	|
						(IHM_switchs_get(SWITCH_XBEE)		<< 4) 	|
						(IHM_switchs_get(SWITCH_COLOR)		<< 9) 	|
						(IHM_switchs_get(BIROUTE)			<< 10) 	|
						(IHM_switchs_get(SWITCH_LCD)		<< 11) 	|
						(IHM_switchs_get(SWITCH_EVIT)		<< 12);

	up = ~previous_state & current_state;
	down = previous_state & ~current_state;
	change = previous_state != current_state;
	previous_state = current_state;

	if(change){
		LCD_printf(1, FALSE, FALSE, "EVIT%d DBG%d XBEE%d", (Sint16)((current_state >> 12) & 1), (Sint16)((current_state >> 1) & 1), (Sint16)((current_state >> 4) & 1));
		if(QS_WHO_AM_I_get() == BIG_ROBOT)
			LCD_printf(2, FALSE, FALSE, "VERBOSE%d SAVE%d", (Sint16)((current_state >> 2) & 1), (Sint16)((current_state >> 5) & 1));
		else
			LCD_printf(2, FALSE, FALSE, "VERBOSE%d", (Sint16)((current_state >> 2) & 1));

		if(up & ((Uint32)(1) << 0	))	debug_printf("BP run match pressed\n");
		if(up & ((Uint32)(1) << 1	))	debug_printf("SW debug enabled\n");
		if(up & ((Uint32)(1) << 2	))	debug_printf("SW verbose enabled\n");
		if(up & ((Uint32)(1) << 3	))	debug_printf("BP print match pressed\n");
		if(up & ((Uint32)(1) << 4	))	debug_printf("SW XBEE enabled\n");
		if(QS_WHO_AM_I_get() == BIG_ROBOT){
			if(up & ((Uint32)(1) << 5	))	debug_printf("SW saved enabled\n");
		}else{
			if(up & ((Uint32)(1) << 5	))	debug_printf("SW SAVE/Strat 4 (our torch) enabled\n");
		}
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
		if(QS_WHO_AM_I_get() == BIG_ROBOT){
			if(down & ((Uint32)(1) << 5	))	debug_printf("SW saved disabled\n");
		}else{
			if(down & ((Uint32)(1) << 5	))	debug_printf("SW SAVE/Strat 4 (our torch) disabled\n");
		}
		if(down & ((Uint32)(1) << 9	))	debug_printf("SW color changed\n");
		if(down & ((Uint32)(1) << 10	))	debug_printf("Biroute removed\n");
		if(down & ((Uint32)(1) << 11	))	debug_printf("SW LCD : CAN msgs\n");
		if(down & ((Uint32)(1) << 12	))	debug_printf("SW Evit disabled\n");
		if(down & ((Uint32)(1) << 13	))	debug_printf("SW Strat1 disabled\n");
		if(down & ((Uint32)(1) << 14	))	debug_printf("SW Strat2 disabled\n");
		if(down & ((Uint32)(1) << 15	))	debug_printf("SW Strat3 disabled\n");
	}

}
