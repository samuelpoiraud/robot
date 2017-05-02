/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD_interface.c
 *	Package : Supervision
 *	Description : implémentation de l'interface de l'écran LCD MCCOG42005A6W-BNMLWI
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */

#include "LCD_interface.h"

#ifdef USE_LCD

	#include "../QS/QS_outputlog.h"
	#include "../QS/QS_stateMachineHelper.h"
	#include "../QS/QS_lcd_over_uart.h"
	#include "Selftest.h"
	#include "RTC.h"
	#include "../brain.h"
	#include "SD/SD.h"

	#define SELFTEST_NB_DISPLAYED_LINE	14

	#define LCD_MIN_TIME_REFRESH		200

	typedef enum{
		MENU_WAIT_IHM = 0,
		MENU_WAIT_OHTER_BOARD,
		MENU_MAIN,
		MENU_SELFTEST,
		MENU_IHM_TEST,
		MENU_CHECK_LIST
	}LCD_state_e;

	static LCD_state_e LCD_state = MENU_WAIT_IHM;
	static LCD_state_e LCD_previous_state = MENU_WAIT_IHM;

	static void LCD_processState(void);
	static LCD_state_e LCD_MENU_waitIHM(bool_e init);
	static LCD_state_e LCD_MENU_waitOtherBoard(bool_e init);
	static LCD_state_e LCD_MENU_mainMenu(bool_e init);
	static LCD_state_e LCD_MENU_selftest(bool_e init);
	static LCD_state_e LCD_MENU_ihmTest(bool_e init);
	static LCD_state_e LCD_MENU_checkList(bool_e init);

	void LCD_init(void){
		LCD_OVER_UART_init();
	}

	void LCD_processMain(void){
		LCD_processState();
		LCD_OVER_UART_processMain();
	}

	static void LCD_processState(void){
		bool_e entrance = LCD_state != LCD_previous_state;
		LCD_previous_state = LCD_state;

		if(entrance)
			LCD_OVER_UART_resetScreen();

		switch(LCD_state){
			case MENU_WAIT_IHM:
				LCD_state = LCD_MENU_waitIHM(entrance);
				break;

			case MENU_WAIT_OHTER_BOARD:
				LCD_state = LCD_MENU_waitOtherBoard(entrance);
				break;

			case MENU_MAIN:
				LCD_state = LCD_MENU_mainMenu(entrance);
				break;

			case MENU_SELFTEST:
				LCD_state = LCD_MENU_selftest(entrance);
				break;

			case MENU_IHM_TEST:
				LCD_state = LCD_MENU_ihmTest(entrance);
				break;

			case MENU_CHECK_LIST:
				LCD_state = LCD_MENU_checkList(entrance);
				break;
		}
	}

	static LCD_state_e LCD_MENU_waitIHM(bool_e init){
		static time32_t time;
		if(init)
			time = global.absolute_time;

		if(global.absolute_time - time > 1000){
			LCD_OVER_UART_setMenu(LCD_MENU_CUSTOM);
			return MENU_WAIT_OHTER_BOARD;
		}

		return MENU_WAIT_IHM;
	}

	static LCD_state_e LCD_MENU_ihmTest(bool_e init){
		if(init)
			LCD_OVER_UART_setMenu(LCD_MENU_DEBUG);

		return MENU_IHM_TEST;
	}

	static LCD_state_e LCD_MENU_waitOtherBoard(bool_e init){
		static LCD_objectId_t actReady, propReady;
		static bool_e actDisplay = FALSE, propDisplay = FALSE;
		static bool_e skip = FALSE;

		if(init){
			actDisplay = FALSE;
			propDisplay = FALSE;
			skip = FALSE;

			LCD_OVER_UART_addImage(0, 0, LCD_IMAGE_LOGO);
			LCD_OVER_UART_addAnimatedImage(270, 55, LCD_ANIMATION_WAIT_CIRCLE);
			LCD_OVER_UART_addButton(250, 10, 0, 0, FALSE, &skip, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Skip");
			LCD_OVER_UART_addText(20, 225, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_7x10, "En attente de l'initialisation des cartes");
			actReady = LCD_OVER_UART_addText(10, 10, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "Attente actionneur");
			propReady = LCD_OVER_UART_addText(10, 35, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "Attente propulsion");
		}

		if(ACT_IS_READY && actDisplay == FALSE){
			LCD_OVER_UART_setText(actReady, "Actionneur prêt");
			actDisplay = TRUE;
		}

		if(PROP_IS_READY && propDisplay == FALSE){
			LCD_OVER_UART_setText(propReady, "Propulsion prête");
			propDisplay = TRUE;
		}

		if((ACT_IS_READY && PROP_IS_READY) || skip)
			return MENU_MAIN;

		return MENU_WAIT_OHTER_BOARD;
	}

	static LCD_state_e LCD_MENU_mainMenu(bool_e init){
		static bool_e changeMenuSelftest = FALSE;
		static bool_e changeMenuIhmTest = FALSE;
		static bool_e changeMenuCheckList = FALSE;
		static LCD_objectId_t idX, idY, idAngle, idVoltage, idTime, idStrat, idMatch;
		static LCD_objectId_t idAdv1, idAdv2, idAdvIr1, idAdvIr2;
		static time32_t lastRefresh;

		static Sint16 lastX, lastY, lastTeta, lastVoltate;
		static date_t lastDate;
		static char lastStratName[20];
		static Uint16 lastIdSD;
		static bool_e lastStateSD;

		if(init){
			changeMenuSelftest = FALSE;
			changeMenuIhmTest = FALSE;
			changeMenuCheckList = FALSE;

			LCD_OVER_UART_addButton(10, 210, 0, 0, FALSE, &changeMenuSelftest, LCD_COLOR_BLACK, LCD_COLOR_GREEN, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Selftest");
			LCD_OVER_UART_addButton(130, 210, 0, 0, FALSE, &changeMenuIhmTest, LCD_COLOR_BLACK, LCD_COLOR_GREEN, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "IHM test");
			LCD_OVER_UART_addButton(260, 210, 0, 0, FALSE, &changeMenuCheckList, LCD_COLOR_BLACK, LCD_COLOR_GREEN, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Check");

			idX = LCD_OVER_UART_addText(5, 5, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "%d", global.pos.x);
			idY = LCD_OVER_UART_addText(45, 5, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "%d", global.pos.y);
			idAngle = LCD_OVER_UART_addText(85, 5, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "%d", global.pos.angle);
			lastX = global.pos.x;
			lastY = global.pos.y;
			lastTeta = global.pos.angle;

			idVoltage = LCD_OVER_UART_addText(130, 5, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "%d", global.alim_value);
			lastVoltate = global.alim_value;

			date_t date;
			RTC_get_time(&date);

			lastDate = date;

			idTime = LCD_OVER_UART_addText(200, 5, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "%02d:%02d:%02d", date.hours, date.minutes, date.seconds);

			idStrat = LCD_OVER_UART_addText(15, 30, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "%s", BRAIN_get_current_strat_name());

			strncpy(lastStratName, BRAIN_get_current_strat_name(), 20);

			if(SD_isOK())
				idMatch = LCD_OVER_UART_addText(200, 180, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "SD : %d", SD_get_match_id());
			else
				idMatch = LCD_OVER_UART_addText(200, 180, LCD_COLOR_RED, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "SD : error");

			lastStateSD = SD_isOK();
			lastIdSD = SD_get_match_id();

			LCD_OVER_UART_addLine(0, 25, 319, 25, LCD_COLOR_BLACK);
			LCD_OVER_UART_addLine(125, 0, 125, 25, LCD_COLOR_BLACK);
			LCD_OVER_UART_addLine(195, 0, 195, 25, LCD_COLOR_BLACK);
		}

		if(global.absolute_time - lastRefresh > LCD_MIN_TIME_REFRESH){
			lastRefresh = global.absolute_time;

			date_t date;
			RTC_get_time(&date);

			if(lastX != global.pos.x || lastY != global.pos.y || lastTeta != global.pos.angle){
				LCD_OVER_UART_setText(idX, "%d", global.pos.x);
				LCD_OVER_UART_setText(idY, "%d", global.pos.y);
				LCD_OVER_UART_setText(idAngle, "%d", global.pos.angle);
				lastX = global.pos.x;
				lastY = global.pos.y;
				lastTeta = global.pos.angle;
			}

			if(lastVoltate != global.alim_value){
				LCD_OVER_UART_setText(idVoltage, "%d", global.pos.y);
				lastVoltate = global.alim_value;
			}

			if(lastDate.seconds != date.seconds){
				LCD_OVER_UART_setText(idTime, "%02d:%02d:%02d", date.hours, date.minutes, date.seconds);
				lastDate = date;
			}

			if(strcmp(lastStratName, BRAIN_get_current_strat_name()) != 0){
				LCD_OVER_UART_setText(lastStratName, "%s", BRAIN_get_current_strat_name());
				strncpy(lastStratName, BRAIN_get_current_strat_name(), 20);
			}

			if(lastIdSD != SD_isOK() || lastIdSD != SD_get_match_id()){
				if(SD_isOK())
					LCD_OVER_UART_setText(idMatch, "SD : %d", SD_get_match_id());
				else
					LCD_OVER_UART_setText(idMatch, "SD : error");

				lastStateSD = SD_isOK();
				lastIdSD = SD_get_match_id();
			}
		}

		if(changeMenuSelftest)
			return MENU_SELFTEST;

		if(changeMenuIhmTest)
			return MENU_IHM_TEST;

		if(changeMenuCheckList)
			return MENU_CHECK_LIST;

		return MENU_MAIN;
	}

	static LCD_state_e LCD_MENU_selftest(bool_e init){

		CREATE_MAE(
				INIT,
				WAIT_LAUNCH_SELFTEST,
				WAIT_RESULT,
				DISPLAY_RESULT);

		static bool_e exit = FALSE;
		static bool_e launchSelftest = FALSE;
		static bool_e up = FALSE, lastUp = FALSE;
		static bool_e down = FALSE, lastDown = FALSE;

		static LCD_objectId_t idText[SELFTEST_NB_DISPLAYED_LINE];

		static Uint8 ptrError = 0;

		bool_e refreshDisplay = FALSE;
		Uint8 i;

		if(init){
			exit = FALSE;
			launchSelftest = FALSE;
			up = FALSE;
			lastUp = FALSE;
			down = FALSE;
			lastDown = FALSE;

			RESET_MAE();
		}

		if(entrance)
			LCD_OVER_UART_resetScreen();

		switch(state){
			case INIT:
				for(i=0; i<SELFTEST_NB_DISPLAYED_LINE; i++){
					idText[i] = LCD_OBJECT_ID_ERROR_FULL;
				}
				state = WAIT_LAUNCH_SELFTEST;
				break;

			case WAIT_LAUNCH_SELFTEST:
				if(entrance){
					LCD_OVER_UART_addText(10, 10, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "Selftest");
					LCD_OVER_UART_addButton(100, 100, 0, 0, FALSE, &launchSelftest, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Lancer");
					LCD_OVER_UART_addButton(250, 10, 0, 0, FALSE, &exit, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Exit");
				}

				if(launchSelftest){
					SELFTEST_ask_launch();
					state = WAIT_RESULT;
				}
				break;

			case WAIT_RESULT:
				if(entrance){
					LCD_OVER_UART_addText(10, 10, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "Selftest");
					LCD_OVER_UART_addAnimatedImage(100, 50, LCD_ANIMATION_WAIT_CIRCLE);
					LCD_OVER_UART_addButton(250, 10, 0, 0, FALSE, &exit, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Exit");
				}

				if(SELFTEST_is_over()){
					state = DISPLAY_RESULT;
				}
				break;

			case DISPLAY_RESULT:
				if(entrance){
					LCD_OVER_UART_addText(10, 10, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "Selftest %d erreurs", SELFTEST_get_errors_number());
					LCD_OVER_UART_addButton(250, 10, 0, 0, FALSE, &exit, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Exit");

					if(SELFTEST_get_errors_number() > SELFTEST_NB_DISPLAYED_LINE){
						LCD_OVER_UART_addButton(250, 60, 60, 60, FALSE, &up, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "up");
						LCD_OVER_UART_addButton(250, 140, 60, 60, FALSE, &down, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "down");
					}

					for(i = 0; i < SELFTEST_NB_DISPLAYED_LINE; i++){
						if(i < SELFTEST_get_errors_number())
							idText[i] = LCD_OVER_UART_addText(20, 40 + 15*i, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_7x10, "%s", SELFTEST_getError_string(SELFTEST_getError(i)));
					}
				}

				if(down && !lastDown){
					if(ptrError < SELFTEST_get_errors_number() - SELFTEST_NB_DISPLAYED_LINE)
						ptrError++;
					refreshDisplay = TRUE;
				}

				if(up && !lastUp){
					if(ptrError != 0)
						ptrError--;

					refreshDisplay = TRUE;
				}


				if(refreshDisplay){
					for(i = ptrError; i < ptrError + SELFTEST_NB_DISPLAYED_LINE; i++){
						LCD_OVER_UART_setText(idText[i - ptrError], "%s", SELFTEST_getError_string(SELFTEST_getError(i)));
					}
					refreshDisplay = FALSE;
				}

				lastDown = down;
				lastUp = up;
				break;

			default:
				RESET_MAE();
				break;
		}

		if(exit){
			RESET_MAE();
			return MENU_MAIN;
		}

		return MENU_SELFTEST;
	}


	static LCD_state_e LCD_MENU_checkList(bool_e init){
		CREATE_MAE(
			INIT,
			WAIT_LAUNCH_SELFTEST,
			WAIT_RESULT,
			DISPLAY_RESULT);

		static bool_e exit = FALSE;

		if(init){
			exit = FALSE;

			RESET_MAE();
		}

		switch(state){
			case INIT:
				state = WAIT_LAUNCH_SELFTEST;
				break;

			case WAIT_LAUNCH_SELFTEST:
				if(entrance){
					LCD_OVER_UART_addText(10, 10, LCD_COLOR_BLACK, LCD_COLOR_TRANSPARENT, LCD_TEXT_FONTS_11x18, "Check list");
					LCD_OVER_UART_addButton(250, 10, 0, 0, FALSE, &exit, LCD_COLOR_BLACK, LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_TEXT_FONTS_11x18, "Exit");
				}

				break;

			default:
				RESET_MAE();
				break;
		}

		if(exit){
			RESET_MAE();
			return MENU_MAIN;
		}

		return MENU_CHECK_LIST;
	}

#endif

	void LCD_printf(Uint8 line, bool_e switch_on_menu, bool_e log_on_sd, char * chaine, ...){

	}

	///*
	// * Switch à gauche :
	// * 		Position robot, positions adverses, n° stratégie.....
	// *  	| Appui sur SET : changement déroulant des différents menus...
	// *
	// * Switch à droite
	// *  	Stratégie haut niveau...
	// *  	| Appui sur SET : mode édition d'une ligne, appuis sur SET : édition tournante des paramètres
	// *  	| Appui sur OK : sortie du mode édition d'une ligne
	// *
	// *
	//
	// */
	//
	//void LCD_Update(void){
	//	typedef enum
	//	{
	//		INIT = 0,
	//		MENU_INFOS,
	//		MENU_SELFTEST,
	//		MENU_SELECT_STRATEGY,
	//		MENU_USER,
	//		MENU_STRATEGY
	//	}menu_e;
	//	static menu_e previous_menu = INIT;
	//	static menu_e menu = INIT;
	//	bool_e entrance;
	//	if(!initialized)
	//		if(init_LCD_interface() == FALSE)	//on retente l'initialisation...
	//			return;
	//	//Navigation dans les menus.
	//	if(ask_for_menu_user)	//Flag prépondérant... ce menu rebaissera le booléen avec un appui SET. (car prioritaire devant le SWITCH !)
	//		menu = MENU_USER;
	//	else if(IHM_switchs_get(SWITCH_LCD))
	//		menu = MENU_STRATEGY;
	//
	//	switch(menu)
	//	{
	//		case INIT:					if(initialized)				menu = MENU_INFOS;				break;
	//		case MENU_INFOS:			if(flag_bp_set)				menu = MENU_SELFTEST;			break;
	//		case MENU_SELFTEST:			if(flag_bp_set)				menu = MENU_SELECT_STRATEGY;	break;
	//		case MENU_SELECT_STRATEGY:	if(flag_bp_set)				menu = MENU_USER;				break;
	//		case MENU_USER:				if(flag_bp_set)			{	menu = MENU_INFOS;		ask_for_menu_user = FALSE;	}	break;
	//		case MENU_STRATEGY:			if(!IHM_switchs_get(SWITCH_LCD))	menu = MENU_INFOS;			break;
	//
	//
	//		default:
	//			menu = INIT;
	//			break;
	//	}
	//
	//	entrance = (previous_menu != menu || reset_config)? TRUE : FALSE;
	//	previous_menu = menu;
	//	if(reset_config)
	//		reset_config = FALSE;
	//
	//	//Affichage des menus et actions boutons.
	//	//Chaque menu est responsable de son raffraichissement raisonnable, et de la mise à jour des leds.
	//	switch(menu)
	//	{
	//		case INIT:															break;
	//		case MENU_INFOS:			LCD_menu_infos(entrance);				break;
	//		case MENU_SELFTEST:			LCD_menu_selftest(entrance);			break;
	//		case MENU_SELECT_STRATEGY:	LCD_menu_select_strategy(entrance);		break;
	//		case MENU_USER:				LCD_menu_user(entrance);				break;
	//		case MENU_STRATEGY:			LCD_menu_strategy(entrance);			break;
	//		default:					menu = INIT;							break;
	//	}
	//}
	//
	//static void LCD_menu_infos(bool_e init)
	//{
	//	if(init)
	//	{
	//		IHM_LEDS(TRUE, FALSE, FALSE, TRUE);
	//		cursor = CURSOR_OFF;
	//	}
	//
	//	if(init || global.pos.updated)
	//		display_pos(0);
	//
	//	if(init || global.flags.foes_updated_for_lcd)
	//		display_beacon(1);
	//
	//	if(init || BRAIN_get_strat_updated())	//Temporaire !	//TODO add updated
	//		display_strats(2);
	//
	//	if(init || global.flags.other_color_updated || global.flags.color_updated){
	//		display_other_robot_color(3);
	//	}/*else{
	//		if(init || free_msg_updated)
	//		{
	//			free_msg_updated = FALSE;
	//			display_debug_msg(3);
	//		}
	//	}*/
	//
	//	if(flag_bp_ok){
	//		BUZZER_play(80, DEFAULT_NOTE, 2);
	//		reset_config = TRUE;
	//		cursor = CURSOR_SHOW; // le cursor est visible après un reset
	//		I2C_reset();
	//		LCD_I2C_init();
	//		LCD_init();
	//	}
	//}
	//
	//static void LCD_menu_selftest(bool_e init)
	//{
	//	typedef enum
	//	{
	//		INIT = 0,
	//		SELFTEST_WAIT,
	//		SELFTEST_RUNNING,
	//		SELFTEST_ENDED
	//	}selftest_state_e;
	//	bool_e update_lines;
	//	static Uint8 index;
	//	static selftest_state_e selftest_state = INIT;
	//	static selftest_state_e previous_selftest_state = INIT;
	//	bool_e entrance;
	//	Uint8 i;
	//	entrance = (selftest_state != previous_selftest_state)? TRUE : FALSE;
	//	previous_selftest_state = selftest_state;
	//	update_lines = FALSE;
	//
	//	if(init)
	//	{
	//		selftest_state = INIT;
	//		previous_selftest_state = INIT;
	//	}
	//
	//	switch(selftest_state)
	//	{
	//		case INIT:
	//			sprintf_line(0,"SELFTEST...");
	//			clear_line(1);
	//			clear_line(2);
	//			clear_line(3);
	//			cursor = CURSOR_OFF;
	//			if(SELFTEST_is_over())
	//				selftest_state = SELFTEST_ENDED;
	//			else
	//				selftest_state = SELFTEST_WAIT;
	//			break;
	//		case SELFTEST_WAIT:
	//			if(entrance)
	//			{
	//				IHM_LEDS(TRUE, FALSE, FALSE, TRUE);	//SET et OK
	//				sprintf_line(1," Press OK to launch");
	//				clear_line(2);
	//				clear_line(3);
	//			}
	//			if(flag_bp_ok)
	//				SELFTEST_ask_launch();
	//			if(SELFTEST_is_running())
	//				selftest_state = SELFTEST_RUNNING;
	//			else if(SELFTEST_is_over())
	//				selftest_state = SELFTEST_ENDED;
	//			break;
	//		case SELFTEST_RUNNING:
	//			if(entrance)
	//			{
	//				IHM_LEDS(TRUE, FALSE, FALSE, FALSE);	//SET
	//				sprintf_line(0," SELFTEST is running");
	//				sprintf_line(1," Please Wait !");
	//				clear_line(2);
	//				clear_line(3);
	//			}
	//			if(SELFTEST_is_over())
	//				selftest_state = SELFTEST_ENDED;
	//			break;
	//		case SELFTEST_ENDED:
	//			if(entrance)
	//			{
	//				sprintf_line(0,"SELFTEST : %2d errors",SELFTEST_get_errors_number());
	//				update_lines = TRUE;
	//				index = 0;
	//			}
	//			if(flag_bp_down)
	//			{
	//				if(	SELFTEST_get_errors_number() > (LINE_NUMBER - 1) && index < (SELFTEST_get_errors_number() - LINE_NUMBER + 1)  )
	//				{
	//					index++;
	//					update_lines = TRUE;
	//				}
	//			}
	//
	//			if(flag_bp_up)
	//			{
	//				if(index > 0)
	//				{
	//					index--;
	//					update_lines = TRUE;
	//				}
	//			}
	//			if(flag_bp_ok)
	//				SELFTEST_ask_launch();
	//			if(SELFTEST_is_running())
	//				selftest_state = SELFTEST_RUNNING;
	//			if(update_lines)
	//				IHM_LEDS(TRUE, SELFTEST_get_errors_number() > (LINE_NUMBER - 1) && index < (SELFTEST_get_errors_number() - LINE_NUMBER + 1), index > 0, TRUE);
	//			break;
	//	}
	//
	//	if(update_lines)
	//	{
	//		for(i=1;i<LINE_NUMBER;i++)
	//			sprintf_line(i, SELFTEST_getError_string(SELFTEST_getError(index+i-1)));
	//	}
	//}
	//
	//static void LCD_menu_select_strategy(bool_e init)
	//{
	//	typedef enum
	//	{
	//		INIT = 0,
	//		LIST
	//	}select_strategie_state_e;
	//	bool_e update_lines;
	//	bool_e update_led_button;
	//	static Uint8 index;
	//	static select_strategie_state_e select_strategie_state = INIT;
	//	static select_strategie_state_e previous_select_strategie_state = INIT;
	//	bool_e entrance;
	//	char chaine[20];
	//	Uint8 i;
	//	entrance = (select_strategie_state != previous_select_strategie_state)? TRUE : FALSE;
	//	previous_select_strategie_state = select_strategie_state;
	//	update_lines = FALSE;
	//	update_led_button = FALSE;
	//
	//	if(init)
	//	{
	//		select_strategie_state = INIT;
	//		previous_select_strategie_state = INIT;
	//		update_lines = TRUE;
	//		update_led_button = TRUE;
	//	}
	//
	//	switch(select_strategie_state)
	//	{
	//		case INIT:
	//			clear_line(0);
	//			clear_line(1);
	//			clear_line(2);
	//			clear_line(3);
	//			cursor = CURSOR_BLINK;
	//			cursor_line = 1;
	//			select_strategie_state = LIST;
	//			break;
	//
	//		case LIST:
	//			if(entrance)
	//			{
	//				sprintf_line(0,"Select stratégie : ");
	//				update_led_button = TRUE;
	//				update_lines = TRUE;
	//				index = 0;
	//			}
	//			if(flag_bp_down)
	//			{
	//				if(cursor_line < (LINE_NUMBER - 1)){
	//					cursor_line++;
	//					update_led_button = TRUE;
	//				}else if(BRAIN_get_number_of_displayed_strategy() > (LINE_NUMBER - 1) && index < (BRAIN_get_number_of_displayed_strategy() - LINE_NUMBER + 1))
	//				{
	//					index++;
	//					update_led_button = TRUE;
	//					update_lines = TRUE;
	//				}
	//			}
	//			if(flag_bp_up)
	//			{
	//				if(cursor_line > 1){
	//					cursor_line--;
	//					update_led_button = TRUE;
	//				}else if(index > 0)
	//				{
	//					index--;
	//					update_led_button = TRUE;
	//					update_lines = TRUE;
	//				}
	//			}
	//
	//			if(flag_bp_ok){
	//				BRAIN_set_strategy_index(index+cursor_line-1);
	//				update_lines = TRUE;
	//			}
	//			if(update_led_button)
	//				IHM_LEDS(TRUE, (index+cursor_line < BRAIN_get_number_of_displayed_strategy()), (index+cursor_line > 1), TRUE);
	//			break;
	//	}
	//
	//	if(update_lines)
	//	{
	//		for(i=0;i<LINE_NUMBER-1;i++){
	//			if(BRAIN_get_current_strat_function() == BRAIN_get_displayed_strat_function((index+i)))
	//				snprintf(chaine, 20, ">%s", BRAIN_get_displayed_strat_name(index+i));
	//			else
	//				snprintf(chaine, 20, " %s", BRAIN_get_displayed_strat_name(index+i));
	//			sprintf_line(i+1, chaine);
	//		}
	//	}
	//}
	//
	//static void LCD_menu_user(bool_e init)
	//{
	//	Uint8 i;
	//	if(init)
	//	{
	//		IHM_LEDS(TRUE, FALSE, FALSE, FALSE);	//SET
	//		sprintf_line(0,"Menu utilisateur...");
	//		cursor = CURSOR_OFF;
	//	}
	//	if(init || free_msg_updated)
	//	{
	//		free_msg_updated = FALSE;
	//		for(i=1;i<LINE_NUMBER;i++)
	//			sprintf_line(i, free_msg[i]);
	//	}
	//	if(get_warning_bat())
	//		IHM_LEDS(TRUE, FALSE, FALSE, TRUE);
	//	else
	//		IHM_LEDS(TRUE, FALSE, FALSE, FALSE);
	//	if(flag_bp_ok)
	//		clean_warning_bat();
	//}
	//
	//static void LCD_menu_strategy(bool_e init)
	//{
	//	typedef enum
	//	{
	//		EDIT_OFF,
	//		EDIT_ENABLE,
	//		EDIT_PRIORITY,
	//		EDIT_T_BEGIN,
	//		EDIT_T_END
	//	}edit_step_e;
	//	static edit_step_e edit_step = EDIT_OFF;
	//	static edit_step_e previous_edit_step = EDIT_OFF;
	//	static subaction_id_e index = 0;
	//	Uint8 i;
	//	bool_e update_lines;
	//	bool_e update_led_button;
	//	update_lines = FALSE;
	//	update_led_button = FALSE;
	//	if(init)
	//	{
	//		sprintf_line(0,"Réglages stratégie..");
	//		clear_line(1);
	//		clear_line(2);
	//		clear_line(3);
	//		index = 0;
	//		update_lines = TRUE;
	//		update_led_button = TRUE;
	//		edit_step = EDIT_OFF;
	//		cursor = CURSOR_BLINK;
	//		cursor_line = 1;
	//		cursor_column = 0;
	//	}
	//
	//	bool_e entrance;
	//	entrance = init || ((edit_step != previous_edit_step)? TRUE : FALSE);
	//	previous_edit_step = edit_step;
	//
	//	switch(edit_step)
	//	{
	//		case EDIT_OFF:
	//			if(entrance)
	//			{
	//				cursor_column = 0;
	//				update_lines = TRUE;
	//				update_led_button = TRUE;
	//				set_update_subaction_order(TRUE);
	//			}
	//
	//			if(BRAIN_get_current_strat_function() != high_level_strat){
	//				if(entrance){
	//					IHM_LEDS(TRUE, FALSE, FALSE, FALSE);
	//					cursor = CURSOR_OFF;
	//					sprintf_line(1,"high strat disabled");
	//				}
	//				return;//----------------------------------------------------------------------------------------RETURN ICI
	//			}
	//			if(flag_bp_down)
	//			{
	//				if(cursor_line < (LINE_NUMBER - 1)){
	//					cursor_line++;
	//					update_led_button = TRUE;
	//				}else if(number_of_sub_action > (LINE_NUMBER - 1) && index < (number_of_sub_action - LINE_NUMBER + 1))
	//				{
	//					index++;
	//					update_led_button = TRUE;
	//					update_lines = TRUE;
	//				}
	//			}
	//			if(flag_bp_up)
	//			{
	//				if(cursor_line > 1){
	//					cursor_line--;
	//					update_led_button = TRUE;
	//				}else if(index > 0)
	//				{
	//					index--;
	//					update_led_button = TRUE;
	//					update_lines = TRUE;
	//				}
	//			}
	//
	//			if(flag_bp_set)
	//				edit_step = EDIT_ENABLE;
	//
	//			if(update_led_button && init)
	//				IHM_LEDS(TRUE, TRUE, FALSE, TRUE);
	//			else if(update_led_button)
	//				IHM_LEDS(TRUE, ((index+cursor_line < number_of_sub_action)?TRUE:FALSE), ((index+cursor_line > 1)?TRUE:FALSE), TRUE);
	//			break;
	//		case EDIT_ENABLE:
	//			if(entrance)
	//			{
	//				cursor_column = 10;
	//				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
	//			}
	//			if(!get_sub_act_done(sub_action_order[index+cursor_line-1]))		//Si l'action n'est pas DONE
	//			{
	//				if(flag_bp_down || flag_bp_up)
	//					set_sub_act_enable(sub_action_order[index+cursor_line-1],!get_sub_act_enable(sub_action_order[index+cursor_line-1]));	//On inverse l'activation
	//			}
	//			if(flag_bp_set)
	//				edit_step = EDIT_PRIORITY;
	//			if(flag_bp_ok)
	//				edit_step = EDIT_OFF;
	//			break;
	//		case EDIT_PRIORITY:
	//			if(entrance)
	//			{
	//				cursor_column = 13;
	//				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
	//			}
	//			if(flag_bp_down)
	//				dec_sub_act_priority(sub_action_order[index+cursor_line-1]);
	//			if(flag_bp_up)
	//				inc_sub_act_priority(sub_action_order[index+cursor_line-1]);
	//			if(flag_bp_set)
	//				edit_step = EDIT_T_BEGIN;
	//			if(flag_bp_ok)
	//				edit_step = EDIT_OFF;
	//			break;
	//		case EDIT_T_BEGIN:
	//			if(entrance)
	//			{
	//				cursor_column = 16;
	//				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
	//			}
	//			if(flag_bp_down)
	//				dec_sub_act_t_begin(sub_action_order[index+cursor_line-1]);
	//			if(flag_bp_up)
	//				inc_sub_act_t_begin(sub_action_order[index+cursor_line-1]);
	//			if(flag_bp_set)
	//				edit_step = EDIT_T_END;
	//			if(flag_bp_ok)
	//				edit_step = EDIT_OFF;
	//			break;
	//		case EDIT_T_END:
	//			if(entrance)
	//			{
	//				cursor_column = 19;
	//				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
	//			}
	//			if(flag_bp_down)
	//				dec_sub_act_t_end(sub_action_order[index+cursor_line-1]);
	//			if(flag_bp_up)
	//				inc_sub_act_t_end(sub_action_order[index+cursor_line-1]);
	//			if(flag_bp_set)
	//				edit_step = EDIT_ENABLE;
	//			if(flag_bp_ok)
	//				edit_step = EDIT_OFF;
	//			break;
	//		default:
	//			edit_step = EDIT_OFF;
	//			break;
	//	}
	//
	//
	//
	//	for(i=0;i<MIN(LINE_NUMBER-1, number_of_sub_action);i++)
	//	{
	//		if(update_lines || get_sub_act_updated_for_lcd(sub_action_order[i+index]))
	//		{
	//			reset_sub_act_updated_for_lcd(sub_action_order[i+index]);
	//			display_subaction_struct(i+1, sub_action_order[i+index]);
	//		}
	//	}
	//
	//
	//}
	//
	//////////////////////////////////////////////////////////////////////////
	///// WRITING ACCESSORS
	//
	////Line doit être entre 0 et 3 inclus.
	////La ligne 0 correspond à la dernière ligne du menu principal (INFOS)
	////Les 3 autres lignes correspondent aux lignes du menu utilisateur.
	//void LCD_printf(Uint8 line, bool_e switch_on_menu, bool_e log_on_sd, char * chaine, ...)
	//{
	//	assert(line < 4);
	//
	//	va_list args_list;
	//	va_start(args_list, chaine);
	//	vsnprintf(free_msg[line], 21, chaine, args_list);
	//	va_end(args_list);
	//	if(line>0)
	//		ask_for_menu_user = switch_on_menu;
	//	free_msg_updated = TRUE;
	//	free_msg[line][20] = '\0';	//sécurité...
	//	if(log_on_sd)
	//		info_printf("LCD_printf(%d):%s\n",line,free_msg[line]);	//On logue sur la LCD ce qu'on demande d'afficher à l'écran (parce que si on l'affiche, c'est que c'est important)
	//
	//	//Cela permet aussi d'éviter le spam...
	//}
	//
	//
	//
	//
	//
	//////////////////////////////////////////////////////////////////////////
	///// BUTTON ACTIONS
	//
	//void LCD_button_down(void)
	//{
	//	flag_bp_down = TRUE;
	//}
	//
	//void LCD_button_up(void)
	//{
	//	flag_bp_up = TRUE;
	//}
	//
	//void LCD_button_ok(void)
	//{
	//	flag_bp_ok = TRUE;
	//}
	//
	//void LCD_button_set(void)
	//{
	//	flag_bp_set = TRUE;
	//}
	//
	//
	//////////////////////////////////////////////////////////////////////////
	///// DISPLAY ACTIONS
	//
	//
	///* Affiche la position du robot*/
	//static void display_pos(Uint8 line)
	//{
	//	Sint16 x,y,t;
	//	x = global.pos.x;
	//	y = global.pos.y;
	//	t = global.pos.angle;
	//
	//	//Ecretages...
	//	x = MIN(x,9999);
	//	x = MAX(x,0);
	//	y = MIN(y,9999);
	//	y = MAX(y,0);
	//	t = MIN(t,PI4096);
	//	t = MAX(t,-PI4096);
	//	t = t*180/PI4096;	// Conversion en degré
	//
	//	sprintf_line(line,"x%4d y%4d t%5d",x,y,t);
	//}
	//
	///* Affiche les infos de la balise*/
	//static void display_beacon(Uint8 line)
	//{
	//	Sint16 dist, angle;
	//	Uint8 foe_id, foe_id1, foe_id2;	//Indice des adversaires à afficher.
	//	Uint8 i;
	//	char str[21];
	//	Uint8 index;
	//
	//
	//	//Les deux objets Hokuyo les plus proches sont les objets 0 et 1, on oublie les autres.
	//	//Les deux objets balises, s'ils sont actifs, sont les objets MAX_HOKUYO_FOES et MAX_HOKUYO_FOES+1
	//	index = 0;
	//	str[1] = ' ';
	//	if(global.foe[0].enable || global.foe[1].enable)
	//	{
	//		foe_id1 = 0;
	//		if(global.foe[1].enable)
	//			foe_id2 = 1;
	//		else
	//		{	//1 seul adversaire hokuyo... on voit l'autre avec la balise...
	//			//On prend l'adversaire balise le plus proche, indépendamment de leur état d'activation/fiabilité...
	//			if(global.foe[MAX_HOKUYO_FOES].dist < global.foe[MAX_HOKUYO_FOES+1].dist)
	//				foe_id2 = MAX_HOKUYO_FOES;
	//			else
	//				foe_id2 = MAX_HOKUYO_FOES + 1;
	//		}
	//	}
	//	else if(global.foe[MAX_HOKUYO_FOES].enable || global.foe[MAX_HOKUYO_FOES+1].enable)
	//	{
	//		foe_id1 = MAX_HOKUYO_FOES;
	//		foe_id2 = MAX_HOKUYO_FOES + 1;
	//	}
	//	else			//Aucun objet actif... on affiche rien (les objets balises sont inactifs, mais on peut consulter leur octet de fiabilité)
	//	{
	//		foe_id1 = MAX_HOKUYO_FOES;
	//		foe_id2 = MAX_HOKUYO_FOES+1;
	//	}
	//
	//	for(i=0;i<2;i++)	//Affichage des deux adversaires choisis.
	//	{
	//		foe_id = (i==0)?foe_id1:foe_id2;
	//		assert(foe_id < MAX_NB_FOES);
	//		index += sprintf(str+index,"%c",(global.foe[foe_id].from == DETECTION_FROM_BEACON_IR)?'B':((global.foe[foe_id].from == DETECTION_FROM_PROPULSION)?'H':'?'));
	//		if(global.foe[foe_id].enable)
	//		{
	//			dist = global.foe[foe_id].dist/10;
	//			angle = (((Sint32)(global.foe[foe_id].angle))*180)/PI4096;
	//			index += sprintf(str+index, "%3d %3d° ", dist, angle);
	//		}
	//		else
	//		{
	//			if(global.foe[foe_id].fiability_error)
	//				index += sprintf(str+index, "ERROR %2x ",global.foe[foe_id].fiability_error);
	//			else
	//				index += sprintf(str+index, "OBSOLATE ");
	//		}
	//	}
	//
	//	str[20] = '\0';
	//	sprintf_line(line, str);
	//}
	//
	///* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
	//static void display_strats(Uint8 line)
	//{
	//	sprintf_line(line,BRAIN_get_current_strat_name());
	//}
	//
	///* Affiche le dernier message demandé par l'utilisateur sur la derniere ligne du mode info */
	///*static void display_debug_msg(Uint8 line)
	//{
	//	sprintf_line(line,"%s",free_msg[0]);
	//}*/
	//
	///* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
	//static void display_subaction_struct(Uint8 line, subaction_id_e subaction)
	//{
	//	Uint8 i;
	//	Uint16 t_begin, t_end, priority;
	//	assert(subaction < SUB_NB);
	//	char buf[21];
	//	i = sprintf(buf, "%s", get_sub_act_chaine(subaction));
	//	for(;i<20;i++)
	//		buf[i] = ' ';	//On rempli d'abord par des espaces
	//	buf[10] = (get_sub_act_done(subaction))?'X':((get_sub_act_enable(subaction))?'1':'0');	//Enable et done
	//	t_begin 	= (Uint16)(get_sub_act_t_begin(subaction)/1000);
	//	t_end		= (Uint16)(get_sub_act_t_end(subaction)/1000);
	//	priority 	= (Uint16)(get_sub_act_priority(subaction));
	//
	//	t_begin 	= MIN(t_begin,99);
	//	t_end 		= MIN(t_end,99);
	//	priority 	= MIN(priority,99);
	//
	//	i = sprintf(buf+12, "%2d %2d>%2d", priority, t_begin, t_end);
	//	assert(i<=8);
	//	buf[20] = '\0';
	//	sprintf_line(line, buf);
	//}
	//
	//static void display_other_robot_color(Uint8 line)
	//{
	//	global.flags.other_color_updated = FALSE;
	//	if(global.other_robot_color == COLOR_INIT_VALUE)
	//		sprintf_line(line, "Color not sync");
	//	else if(global.other_robot_color != global.color)
	//		sprintf_line(line, "Color ERROR !");
	//	else
	//		sprintf_line(line, "Color good :)");
	//}



