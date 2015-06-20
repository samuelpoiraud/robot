/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD_interface.c
 *	Package : Supervision
 *	Description : impl�mentation de l'interface de l'�cran LCD MCCOG42005A6W-BNMLWI
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */


#include "../QS/QS_outputlog.h"
#include "../QS/QS_IHM.h"
#include "LCD_MIDAS_4x20.h"
#include "LCD_interface.h"
#include "Buffer.h"
#include "../QS/QS_who_am_i.h"
#include "../environment.h"
#include "Selftest.h"
#include "../high_level_strat.h"
#include "SD/SD.h"
#include "../brain.h"
#include "stdarg.h"
#include "../brain.h"
#include "Buzzer.h"
#include "../QS/QS_i2c.h"
#include <stdio.h>

#define LINE_NUMBER (4)
	volatile bool_e flag_bp_set 	= FALSE;
	volatile bool_e flag_bp_ok 		= FALSE;
	volatile bool_e flag_bp_up 		= FALSE;
	volatile bool_e flag_bp_down 	= FALSE;
	volatile bool_e ask_for_menu_user = FALSE;
	volatile bool_e can_msg_updated = FALSE;
	static bool_e reset_config = FALSE;

	static void LCD_menu_infos(bool_e init);
	static void LCD_menu_strategy(bool_e init);
	static void LCD_menu_selftest(bool_e init);
	static void LCD_menu_user(bool_e init);
	static void LCD_menu_select_strategy(bool_e init);
	static void display_pos(Uint8 line);
	static void display_beacon(Uint8 line);
	static void display_strats(Uint8 line);
	static void display_debug_msg(Uint8 line);
	static void display_subaction_struct(Uint8 line, subaction_id_e subaction);
	void clear_line(Uint8 line);
	void IHM_LEDS(bool_e led_set, bool_e led_down, bool_e led_up, bool_e led_ok);

// Etats de l'affichage de l'ecran
typedef enum
{
	INFO_s = 0,
	USER_MODE,
	MENU,
	SELFTEST,
	INIT = 0XFF
}lcd_state;

lcd_state state;
lcd_state previous_state;

const char *strategy[6] = {"STR","TRG","MSN","BAL","FRT","SCN"};
Uint8 strat_nb[LINE_NUMBER];

/* Variable contenant un message libre*/
#define FREE_MSG_NB	4
char free_msg[FREE_MSG_NB][21];
bool_e free_msg_updated; // Commande le rafraichissement de l'�cran

/* Chaines affich�es � l'�cran */
char lines[4][21];
bool_e update_line[4] = {TRUE, TRUE, TRUE, TRUE};

typedef enum
{
	CURSOR_OFF = 0,
	CURSOR_SHOW,
	CURSOR_BLINK
}cursor_e;
cursor_e cursor = CURSOR_OFF;
Uint8 cursor_line, cursor_column;

static bool_e initialized = FALSE;

////////////////////////////////////////////////////////////////////////
/// INIT

bool_e init_LCD_interface(void){
	if(IHM_IS_READY && !initialized)	//Si la carte IHM est pr�sente... on initialise le LCD...
	{
		LCD_I2C_init();
		LCD_init();
		LCD_clear_display();
		LCD_cursor_display(FALSE,FALSE);
		state = INFO_s;
		previous_state = INFO_s;
		free_msg_updated = TRUE;
		Uint8 i;
		for(i=0;i<LINE_NUMBER;i++)
			clear_line(i);
		for(i=0;i<FREE_MSG_NB;i++)
			free_msg[i][0] = 0;
		//IHM_LEDS(FALSE, FALSE, FALSE, FALSE);

		initialized = TRUE;
	}
	return initialized;
}

// Enregistre une chaine formatt�e vers une ligne donn�e.
//Cette fonction, bien que publique, n'est PAS DESTINEE � un utilisateur..... mais uniquement aux fichiers du LCD.
void sprintf_line(Uint8 line, const char * format, ...)
{
	Uint8 i;
	va_list args_list;
	assert(line<LINE_NUMBER);
	va_start(args_list, format);
	i = vsnprintf(lines[line], 21, format, args_list);
	if(i>21)
		debug_printf("ATTENTION, chaine LCD plus grande que 20 !\n");
	va_end(args_list);
	for(;i<20;i++)
		lines[line][i] = ' ';	//On rempli la ligne par des espaces
	lines[line][20] = '\0';
	update_line[line] = TRUE;
}

// Enregistre une ligne blanche � la ligne donn�e
//Cette fonction, bien que publique, n'est PAS DESTINEE � un utilisateur..... mais uniquement aux fichiers du LCD.
void clear_line(Uint8 line)
{
	sprintf_line(line,"");
}


////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
// Affiche � l'�cran les lignes ayant �t� mises � jour, et affiche le curseur si demand�
void LCD_refresh_lines(void){
	Uint8 i;
	static cursor_e previous_cursor = CURSOR_OFF;
	static Uint8 previous_cursor_line = 0;
	static Uint8 previous_cursor_column = 0;
	bool_e updated;
	updated = FALSE;
	for(i=0;i<LINE_NUMBER;i++)
	{
		if(update_line[i])
		{
			LCD_set_cursor(i,0);
			LCD_Write_text(lines[i]);
			update_line[i] = FALSE;
			updated = TRUE;
		}
	}
	if(updated || (previous_cursor_line != cursor_line) || (previous_cursor_column != cursor_column) || (previous_cursor != cursor))
	{
		if(cursor != CURSOR_OFF)
		{
			LCD_set_cursor(cursor_line,cursor_column);
			LCD_cursor_display(TRUE, (cursor == CURSOR_BLINK)?TRUE:FALSE);
		}
		else if(previous_cursor != cursor)
			LCD_cursor_display(FALSE, FALSE);
		previous_cursor_line = cursor_line;
		previous_cursor_column = cursor_column;
		previous_cursor = cursor;
	}
}



/*
 * Envoit la demande de mise � jour des leds � la carte IHM... SEULEMENT s'il y a un changement sur l'une des leds... (ou en cas d'initialisation)
 */
void IHM_LEDS(bool_e led_set, bool_e led_down, bool_e led_up, bool_e led_ok){
	static bool_e previous_led_set = FALSE;
	static bool_e previous_led_down = FALSE;
	static bool_e previous_led_up = FALSE;
	static bool_e previous_led_ok = FALSE;
	static bool_e initialized = FALSE;
	bool_e send_to_ihm = FALSE;
	if(!initialized)
	{
		initialized = TRUE;
		send_to_ihm = TRUE;
	}
	if(	led_set 	!= previous_led_set 	||
		led_down	!= previous_led_down 	||
		led_up 		!= previous_led_up 		||
		led_ok 		!= previous_led_ok			)
	{
		send_to_ihm = TRUE;
	}

	if(send_to_ihm)
	{
		IHM_leds_send_msg(4,(led_ihm_t){LED_SET_IHM,led_set},(led_ihm_t){LED_DOWN_IHM,led_down},(led_ihm_t){LED_UP_IHM,led_up},(led_ihm_t){LED_OK_IHM,led_ok});
		previous_led_set 	= led_set;
		previous_led_down 	= led_down;
		previous_led_up 	= led_up;
		previous_led_ok 	= led_ok;
	}
}




////////////////////////////////////////////////////////////////////////
/// REFRESHING

/*
 * Switch � gauche :
 * 		Position robot, positions adverses, n� strat�gie.....
 *  	| Appui sur SET : changement d�roulant des diff�rents menus...
 *
 * Switch � droite
 *  	Strat�gie haut niveau...
 *  	| Appui sur SET : mode �dition d'une ligne, appuis sur SET : �dition tournante des param�tres
 *  	| Appui sur OK : sortie du mode �dition d'une ligne
 *
 *

 */

void LCD_Update(void){
	typedef enum
	{
		INIT = 0,
		MENU_INFOS,
		MENU_SELFTEST,
		MENU_SELECT_STRATEGY,
		MENU_USER,
		MENU_STRATEGY
	}menu_e;
	static menu_e previous_menu = INIT;
	static menu_e menu = INIT;
	bool_e entrance;
	if(!initialized)
		if(init_LCD_interface() == FALSE)	//on retente l'initialisation...
			return;
	//Navigation dans les menus.
	if(ask_for_menu_user)	//Flag pr�pond�rant... ce menu rebaissera le bool�en avec un appui SET. (car prioritaire devant le SWITCH !)
		menu = MENU_USER;
	else if(IHM_switchs_get(SWITCH_LCD))
		menu = MENU_STRATEGY;

	switch(menu)
	{
		case INIT:					if(initialized)				menu = MENU_INFOS;				break;
		case MENU_INFOS:			if(flag_bp_set)				menu = MENU_SELFTEST;			break;
		case MENU_SELFTEST:			if(flag_bp_set)				menu = MENU_SELECT_STRATEGY;	break;
		case MENU_SELECT_STRATEGY:	if(flag_bp_set)				menu = MENU_USER;				break;
		case MENU_USER:				if(flag_bp_set)			{	menu = MENU_INFOS;		ask_for_menu_user = FALSE;	}	break;
		case MENU_STRATEGY:			if(!IHM_switchs_get(SWITCH_LCD))	menu = MENU_INFOS;			break;


		default:
			menu = INIT;
			break;
	}

	entrance = (previous_menu != menu || reset_config)? TRUE : FALSE;
	previous_menu = menu;
	if(reset_config)
		reset_config = FALSE;

	//Affichage des menus et actions boutons.
	//Chaque menu est responsable de son raffraichissement raisonnable, et de la mise � jour des leds.
	switch(menu)
	{
		case INIT:															break;
		case MENU_INFOS:			LCD_menu_infos(entrance);				break;
		case MENU_SELFTEST:			LCD_menu_selftest(entrance);			break;
		case MENU_SELECT_STRATEGY:	LCD_menu_select_strategy(entrance);		break;
		case MENU_USER:				LCD_menu_user(entrance);				break;
		case MENU_STRATEGY:			LCD_menu_strategy(entrance);			break;
		default:					menu = INIT;							break;
	}

	LCD_refresh_lines();	//Affichage sur le LCD des lignes ayant chang�es.

	flag_bp_ok = FALSE;
	flag_bp_set = FALSE;
	flag_bp_up = FALSE;
	flag_bp_down = FALSE;
}

static void LCD_menu_infos(bool_e init)
{
	if(init)
	{
		IHM_LEDS(TRUE, FALSE, FALSE, TRUE);
		cursor = CURSOR_OFF;
	}

	if(init || global.pos.updated)
		display_pos(0);

	if(init || global.flags.foes_updated_for_lcd)
		display_beacon(1);

	if(init || BRAIN_get_strat_updated())	//Temporaire !	//TODO add updated
		display_strats(2);

	if(init || free_msg_updated)
	{
		free_msg_updated = FALSE;
		display_debug_msg(3);
	}

	if(flag_bp_ok){
		BUZZER_play(80, DEFAULT_NOTE, 2);
		reset_config = TRUE;
		cursor = CURSOR_SHOW; // le cursor est visible apr�s un reset
		I2C_reset();
		LCD_I2C_init();
		LCD_init();
	}
}

static void LCD_menu_selftest(bool_e init)
{
	typedef enum
	{
		INIT = 0,
		SELFTEST_WAIT,
		SELFTEST_RUNNING,
		SELFTEST_ENDED
	}selftest_state_e;
	bool_e update_lines;
	static Uint8 index;
	static selftest_state_e selftest_state = INIT;
	static selftest_state_e previous_selftest_state = INIT;
	bool_e entrance;
	Uint8 i;
	entrance = (selftest_state != previous_selftest_state)? TRUE : FALSE;
	previous_selftest_state = selftest_state;
	update_lines = FALSE;

	if(init)
	{
		selftest_state = INIT;
		previous_selftest_state = INIT;
	}

	switch(selftest_state)
	{
		case INIT:
			sprintf_line(0,"SELFTEST...");
			clear_line(1);
			clear_line(2);
			clear_line(3);
			cursor = CURSOR_OFF;
			if(SELFTEST_is_over())
				selftest_state = SELFTEST_ENDED;
			else
				selftest_state = SELFTEST_WAIT;
			break;
		case SELFTEST_WAIT:
			if(entrance)
			{
				IHM_LEDS(TRUE, FALSE, FALSE, TRUE);	//SET et OK
				sprintf_line(1," Press OK to launch");
				clear_line(2);
				clear_line(3);
			}
			if(flag_bp_ok)
				SELFTEST_ask_launch();
			if(SELFTEST_is_running())
				selftest_state = SELFTEST_RUNNING;
			else if(SELFTEST_is_over())
				selftest_state = SELFTEST_ENDED;
			break;
		case SELFTEST_RUNNING:
			if(entrance)
			{
				IHM_LEDS(TRUE, FALSE, FALSE, FALSE);	//SET
				sprintf_line(0," SELFTEST is running");
				sprintf_line(1," Please Wait !");
				clear_line(2);
				clear_line(3);
			}
			if(SELFTEST_is_over())
				selftest_state = SELFTEST_ENDED;
			break;
		case SELFTEST_ENDED:
			if(entrance)
			{
				sprintf_line(0,"SELFTEST : %2d errors",SELFTEST_get_errors_number());
				update_lines = TRUE;
				index = 0;
			}
			if(flag_bp_down)
			{
				if(	SELFTEST_get_errors_number() > (LINE_NUMBER - 1) && index < (SELFTEST_get_errors_number() - LINE_NUMBER + 1)  )
				{
					index++;
					update_lines = TRUE;
				}
			}

			if(flag_bp_up)
			{
				if(index > 0)
				{
					index--;
					update_lines = TRUE;
				}
			}
			if(flag_bp_ok)
				SELFTEST_ask_launch();
			if(SELFTEST_is_running())
				selftest_state = SELFTEST_RUNNING;
			if(update_lines)
				IHM_LEDS(TRUE, SELFTEST_get_errors_number() > (LINE_NUMBER - 1) && index < (SELFTEST_get_errors_number() - LINE_NUMBER + 1), index > 0, TRUE);
			break;
	}

	if(update_lines)
	{
		for(i=1;i<LINE_NUMBER;i++)
			sprintf_line(i, SELFTEST_getError_string(SELFTEST_getError(index+i-1)));
	}
}

static void LCD_menu_select_strategy(bool_e init)
{
	typedef enum
	{
		INIT = 0,
		LIST
	}select_strategie_state_e;
	bool_e update_lines;
	bool_e update_led_button;
	static Uint8 index;
	static select_strategie_state_e select_strategie_state = INIT;
	static select_strategie_state_e previous_select_strategie_state = INIT;
	bool_e entrance;
	char chaine[20];
	Uint8 i;
	entrance = (select_strategie_state != previous_select_strategie_state)? TRUE : FALSE;
	previous_select_strategie_state = select_strategie_state;
	update_lines = FALSE;
	update_led_button = FALSE;

	if(init)
	{
		select_strategie_state = INIT;
		previous_select_strategie_state = INIT;
		update_lines = TRUE;
		update_led_button = TRUE;
	}

	switch(select_strategie_state)
	{
		case INIT:
			clear_line(0);
			clear_line(1);
			clear_line(2);
			clear_line(3);
			cursor = CURSOR_BLINK;
			cursor_line = 1;
			select_strategie_state = LIST;
			break;

		case LIST:
			if(entrance)
			{
				sprintf_line(0,"Select strat�gie : ");
				update_led_button = TRUE;
				update_lines = TRUE;
				index = 0;
			}
			if(flag_bp_down)
			{
				if(cursor_line < (LINE_NUMBER - 1)){
					cursor_line++;
					update_led_button = TRUE;
				}else if(BRAIN_get_number_of_displayed_strategy() > (LINE_NUMBER - 1) && index < (BRAIN_get_number_of_displayed_strategy() - LINE_NUMBER + 1))
				{
					index++;
					update_led_button = TRUE;
					update_lines = TRUE;
				}
			}
			if(flag_bp_up)
			{
				if(cursor_line > 1){
					cursor_line--;
					update_led_button = TRUE;
				}else if(index > 0)
				{
					index--;
					update_led_button = TRUE;
					update_lines = TRUE;
				}
			}

			if(flag_bp_ok){
				BRAIN_set_strategy_index(index+cursor_line-1);
				update_lines = TRUE;
			}
			if(update_led_button)
				IHM_LEDS(TRUE, (index+cursor_line < BRAIN_get_number_of_displayed_strategy()), (index+cursor_line > 1), TRUE);
			break;
	}

	if(update_lines)
	{
		for(i=0;i<LINE_NUMBER-1;i++){
			if(BRAIN_get_current_strat_function() == BRAIN_get_displayed_strat_function((index+i)))
				snprintf(chaine, 20, ">%s", BRAIN_get_displayed_strat_name(index+i));
			else
				snprintf(chaine, 20, " %s", BRAIN_get_displayed_strat_name(index+i));
			sprintf_line(i+1, chaine);
		}
	}
}

static void LCD_menu_user(bool_e init)
{
	Uint8 i;
	if(init)
	{
		IHM_LEDS(TRUE, FALSE, FALSE, FALSE);	//SET
		sprintf_line(0,"Menu utilisateur...");
		cursor = CURSOR_OFF;
	}
	if(init || free_msg_updated)
	{
		free_msg_updated = FALSE;
		for(i=1;i<LINE_NUMBER;i++)
			sprintf_line(i, free_msg[i]);
	}
	if(get_warning_bat())
		IHM_LEDS(TRUE, FALSE, FALSE, TRUE);
	else
		IHM_LEDS(TRUE, FALSE, FALSE, FALSE);
	if(flag_bp_ok)
		clean_warning_bat();
}

static void LCD_menu_strategy(bool_e init)
{
	typedef enum
	{
		EDIT_OFF,
		EDIT_ENABLE,
		EDIT_PRIORITY,
		EDIT_T_BEGIN,
		EDIT_T_END
	}edit_step_e;
	static edit_step_e edit_step = EDIT_OFF;
	static edit_step_e previous_edit_step = EDIT_OFF;
	static subaction_id_e index = 0;
	Uint8 i;
	bool_e update_lines;
	bool_e update_led_button;
	update_lines = FALSE;
	update_led_button = FALSE;
	if(init)
	{
		sprintf_line(0,"R�glages strat�gie..");
		clear_line(1);
		clear_line(2);
		clear_line(3);
		index = 0;
		update_lines = TRUE;
		update_led_button = TRUE;
		edit_step = EDIT_OFF;
		cursor = CURSOR_BLINK;
		cursor_line = 1;
		cursor_column = 0;
	}

	bool_e entrance;
	entrance = init || ((edit_step != previous_edit_step)? TRUE : FALSE);
	previous_edit_step = edit_step;

	switch(edit_step)
	{
		case EDIT_OFF:
			if(entrance)
			{
				cursor_column = 0;
				update_lines = TRUE;
				update_led_button = TRUE;
				set_update_subaction_order(TRUE);
			}

			if(BRAIN_get_current_strat_function() != high_level_strat){
				if(entrance){
					IHM_LEDS(TRUE, FALSE, FALSE, FALSE);
					cursor = CURSOR_OFF;
					sprintf_line(1,"high strat disabled");
				}
				return;//----------------------------------------------------------------------------------------RETURN ICI
			}
			if(flag_bp_down)
			{
				if(cursor_line < (LINE_NUMBER - 1)){
					cursor_line++;
					update_led_button = TRUE;
				}else if(number_of_sub_action > (LINE_NUMBER - 1) && index < (number_of_sub_action - LINE_NUMBER + 1))
				{
					index++;
					update_led_button = TRUE;
					update_lines = TRUE;
				}
			}
			if(flag_bp_up)
			{
				if(cursor_line > 1){
					cursor_line--;
					update_led_button = TRUE;
				}else if(index > 0)
				{
					index--;
					update_led_button = TRUE;
					update_lines = TRUE;
				}
			}

			if(flag_bp_set)
				edit_step = EDIT_ENABLE;

			if(update_led_button && init)
				IHM_LEDS(TRUE, TRUE, FALSE, TRUE);
			else if(update_led_button)
				IHM_LEDS(TRUE, ((index+cursor_line < number_of_sub_action)?TRUE:FALSE), ((index+cursor_line > 1)?TRUE:FALSE), TRUE);
			break;
		case EDIT_ENABLE:
			if(entrance)
			{
				cursor_column = 10;
				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
			}
			if(!get_sub_act_done(sub_action_order[index+cursor_line-1]))		//Si l'action n'est pas DONE
			{
				if(flag_bp_down || flag_bp_up)
					set_sub_act_enable(sub_action_order[index+cursor_line-1],!get_sub_act_enable(sub_action_order[index+cursor_line-1]));	//On inverse l'activation
			}
			if(flag_bp_set)
				edit_step = EDIT_PRIORITY;
			if(flag_bp_ok)
				edit_step = EDIT_OFF;
			break;
		case EDIT_PRIORITY:
			if(entrance)
			{
				cursor_column = 13;
				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
			}
			if(flag_bp_down)
				dec_sub_act_priority(sub_action_order[index+cursor_line-1]);
			if(flag_bp_up)
				inc_sub_act_priority(sub_action_order[index+cursor_line-1]);
			if(flag_bp_set)
				edit_step = EDIT_T_BEGIN;
			if(flag_bp_ok)
				edit_step = EDIT_OFF;
			break;
		case EDIT_T_BEGIN:
			if(entrance)
			{
				cursor_column = 16;
				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
			}
			if(flag_bp_down)
				dec_sub_act_t_begin(sub_action_order[index+cursor_line-1]);
			if(flag_bp_up)
				inc_sub_act_t_begin(sub_action_order[index+cursor_line-1]);
			if(flag_bp_set)
				edit_step = EDIT_T_END;
			if(flag_bp_ok)
				edit_step = EDIT_OFF;
			break;
		case EDIT_T_END:
			if(entrance)
			{
				cursor_column = 19;
				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);
			}
			if(flag_bp_down)
				dec_sub_act_t_end(sub_action_order[index+cursor_line-1]);
			if(flag_bp_up)
				inc_sub_act_t_end(sub_action_order[index+cursor_line-1]);
			if(flag_bp_set)
				edit_step = EDIT_ENABLE;
			if(flag_bp_ok)
				edit_step = EDIT_OFF;
			break;
		default:
			edit_step = EDIT_OFF;
			break;
	}



	for(i=0;i<MIN(LINE_NUMBER-1, number_of_sub_action);i++)
	{
		if(update_lines || get_sub_act_updated_for_lcd(sub_action_order[i+index]))
		{
			reset_sub_act_updated_for_lcd(sub_action_order[i+index]);
			display_subaction_struct(i+1, sub_action_order[i+index]);
		}
	}


}

////////////////////////////////////////////////////////////////////////
/// WRITING ACCESSORS

//Line doit �tre entre 0 et 3 inclus.
//La ligne 0 correspond � la derni�re ligne du menu principal (INFOS)
//Les 3 autres lignes correspondent aux lignes du menu utilisateur.
void LCD_printf(Uint8 line, bool_e switch_on_menu, bool_e log_on_sd, char * chaine, ...)
{
	assert(line < 4);

	va_list args_list;
	va_start(args_list, chaine);
	vsnprintf(free_msg[line], 21, chaine, args_list);
	va_end(args_list);
	if(line>0)
		ask_for_menu_user = switch_on_menu;
	free_msg_updated = TRUE;
	free_msg[line][20] = '\0';	//s�curit�...
	if(log_on_sd)
		SD_printf("LCD_printf(%d):%s\n",line,free_msg[line]);	//On logue sur la LCD ce qu'on demande d'afficher � l'�cran (parce que si on l'affiche, c'est que c'est important)

	//Cela permet aussi d'�viter le spam...
}





////////////////////////////////////////////////////////////////////////
/// BUTTON ACTIONS

void LCD_button_down(void)
{
	flag_bp_down = TRUE;
}

void LCD_button_up(void)
{
	flag_bp_up = TRUE;
}

void LCD_button_ok(void)
{
	flag_bp_ok = TRUE;
}

void LCD_button_set(void)
{
	flag_bp_set = TRUE;
}


////////////////////////////////////////////////////////////////////////
/// DISPLAY ACTIONS


/* Affiche la position du robot*/
static void display_pos(Uint8 line)
{
	Sint16 x,y,t;
	x = global.pos.x;
	y = global.pos.y;
	t = global.pos.angle;

	//Ecretages...
	x = MIN(x,9999);
	x = MAX(x,0);
	y = MIN(y,9999);
	y = MAX(y,0);
	t = MIN(t,PI4096);
	t = MAX(t,-PI4096);
	t = t*180/PI4096;	// Conversion en degr�

	sprintf_line(line,"x%4d y%4d t%5d",x,y,t);
}

/* Affiche les infos de la balise*/
static void display_beacon(Uint8 line)
{
	Sint16 dist, angle;
	Uint8 foe_id, foe_id1, foe_id2;	//Indice des adversaires � afficher.
	Uint8 i;
	char str[21];
	Uint8 index;


	//Les deux objets Hokuyo les plus proches sont les objets 0 et 1, on oublie les autres.
	//Les deux objets balises, s'ils sont actifs, sont les objets MAX_HOKUYO_FOES et MAX_HOKUYO_FOES+1
	index = 0;
	str[1] = ' ';
	if(global.foe[0].enable || global.foe[1].enable)
	{
		foe_id1 = 0;
		if(global.foe[1].enable)
			foe_id2 = 1;
		else
		{	//1 seul adversaire hokuyo... on voit l'autre avec la balise...
			//On prend l'adversaire balise le plus proche, ind�pendamment de leur �tat d'activation/fiabilit�...
			if(global.foe[MAX_HOKUYO_FOES].dist < global.foe[MAX_HOKUYO_FOES+1].dist)
				foe_id2 = MAX_HOKUYO_FOES;
			else
				foe_id2 = MAX_HOKUYO_FOES + 1;
		}
	}
	else if(global.foe[MAX_HOKUYO_FOES].enable || global.foe[MAX_HOKUYO_FOES+1].enable)
	{
		foe_id1 = MAX_HOKUYO_FOES;
		foe_id2 = MAX_HOKUYO_FOES + 1;
	}
	else			//Aucun objet actif... on affiche rien (les objets balises sont inactifs, mais on peut consulter leur octet de fiabilit�)
	{
		foe_id1 = MAX_HOKUYO_FOES;
		foe_id2 = MAX_HOKUYO_FOES+1;
	}

	for(i=0;i<2;i++)	//Affichage des deux adversaires choisis.
	{
		foe_id = (i==0)?foe_id1:foe_id2;
		assert(foe_id < MAX_NB_FOES);
		index += sprintf(str+index,"%c",(global.foe[foe_id].from == DETECTION_FROM_BEACON_IR)?'B':((global.foe[foe_id].from == DETECTION_FROM_PROPULSION)?'H':'?'));
		if(global.foe[foe_id].enable)
		{
			dist = global.foe[foe_id].dist/10;
			angle = (((Sint32)(global.foe[foe_id].angle))*180)/PI4096;
			index += sprintf(str+index, "%3d %3d� ", dist, angle);
		}
		else
		{
			if(global.foe[foe_id].fiability_error)
				index += sprintf(str+index, "ERROR %2x ",global.foe[foe_id].fiability_error);
			else
				index += sprintf(str+index, "OBSOLATE ");
		}
	}

	str[20] = '\0';
	sprintf_line(line, str);
}

/* Affiche les strats s�lectionn�es pour le match � la troisieme ligne du mode info */
static void display_strats(Uint8 line)
{
	sprintf_line(line,BRAIN_get_current_strat_name());
}

/* Affiche le dernier message demand� par l'utilisateur sur la derniere ligne du mode info */
static void display_debug_msg(Uint8 line)
{
	sprintf_line(line,"%s",free_msg[0]);
}

/* Affiche les strats s�lectionn�es pour le match � la troisieme ligne du mode info */
static void display_subaction_struct(Uint8 line, subaction_id_e subaction)
{
	Uint8 i;
	Uint16 t_begin, t_end, priority;
	assert(subaction < SUB_NB);
	char buf[21];
	i = sprintf(buf, "%s", get_sub_act_chaine(subaction));
	for(;i<20;i++)
		buf[i] = ' ';	//On rempli d'abord par des espaces
	buf[10] = (get_sub_act_done(subaction))?'X':((get_sub_act_enable(subaction))?'1':'0');	//Enable et done
	t_begin 	= (Uint16)(get_sub_act_t_begin(subaction)/1000);
	t_end		= (Uint16)(get_sub_act_t_end(subaction)/1000);
	priority 	= (Uint16)(get_sub_act_priority(subaction));

	t_begin 	= MIN(t_begin,99);
	t_end 		= MIN(t_end,99);
	priority 	= MIN(priority,99);

	i = sprintf(buf+12, "%2d %2d>%2d", priority, t_begin, t_end);
	assert(i<=8);
	buf[20] = '\0';
	sprintf_line(line, buf);
}



