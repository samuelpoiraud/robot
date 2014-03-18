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


#include "../QS/QS_all.h"
#include "../QS/QS_outputlog.h"
#include "LCD_MIDAS_4x20.h"
#include "LCD_interface.h"
#include "LCD_CAN_injector.h"
#include "Buffer.h"
#include "config_pin.h"
#include "../QS/QS_who_am_i.h"
#include "../environment.h"
#include "Selftest.h"
#include "../high_level_strat.h"
#include "SD/SD.h"
#include "../brain.h"
#include "stdarg.h"
#include "config_use.h"

#define LINE_NUMBER (4)
	volatile bool_e flag_bp_set 	= FALSE;
	volatile bool_e flag_bp_ok 		= FALSE;
	volatile bool_e flag_bp_up 		= FALSE;
	volatile bool_e flag_bp_down 	= FALSE;
	volatile bool_e ask_for_menu_user = FALSE;
	volatile bool_e can_msg_updated = FALSE;

	static void LCD_menu_infos(bool_e init);
	static void LCD_menu_strategy(bool_e init);
	static void LCD_menu_selftest(bool_e init);
	static void LCD_menu_can_msg(bool_e init);
	static void LCD_menu_user(bool_e init);
	static bool_e LCD_strat_number_update(void);
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
	CAN_s,
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
bool_e free_msg_updated; // Commande le rafraichissement de l'écran

#define NB_CAN_MSG	4
CAN_msg_t can_msgs[NB_CAN_MSG];	/* Tableau des messages can à afficher */
volatile Uint8 index_can_msg;

/* Chaines affichées à l'écran */
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

void init_LCD_interface(void){
	LCD_I2C_init();
	LCD_init();
	LCD_clear_display();
	LCD_cursor_display(FALSE,FALSE);
	state = INFO_s;
	previous_state = INFO_s;
	free_msg_updated = TRUE;
	index_can_msg = 0;
	Uint8 i;
	for(i=0; i<NB_CAN_MSG; i++){
		can_msgs[i].sid = 0;
		can_msgs[i].size = 0;
		strat_nb[i] = 0;	//TODO déplacer..
	}
	for(i=0;i<LINE_NUMBER;i++)
		clear_line(i);
	for(i=0;i<FREE_MSG_NB;i++)
		free_msg[i][0] = 0;
	IHM_LEDS(FALSE, FALSE, FALSE, FALSE);

	initialized = TRUE;
}

// Enregistre une chaine formattée vers une ligne donnée.
//Cette fonction, bien que publique, n'est PAS DESTINEE à un utilisateur..... mais uniquement aux fichiers du LCD.
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

// Enregistre une ligne blanche à la ligne donnée
//Cette fonction, bien que publique, n'est PAS DESTINEE à un utilisateur..... mais uniquement aux fichiers du LCD.
void clear_line(Uint8 line)
{
	sprintf_line(line,"");
}


////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
// Affiche à l'écran les lignes ayant été mises à jour, et affiche le curseur si demandé
void LCD_refresh_lines(void)
{
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




void IHM_LEDS(bool_e led_set, bool_e led_down, bool_e led_up, bool_e led_ok)
{
	LED_IHM_SET 	= led_set;
	LED_IHM_DOWN 	= led_down;
	LED_IHM_UP 		= led_up;
	LED_IHM_OK 		= led_ok;
}




////////////////////////////////////////////////////////////////////////
/// REFRESHING

/*
 * Switch à gauche :
 * 		Position robot, positions adverses, n° stratégie.....
 *  	| Appui sur SET : changement déroulant des différents menus...
 *
 * Switch à droite
 *  	Stratégie haut niveau...
 *  	| Appui sur SET : mode édition d'une ligne, appuis sur SET : édition tournante des paramètres
 *  	| Appui sur OK : sortie du mode édition d'une ligne
 *
 *

 */

void LCD_Update(void){
	typedef enum
	{
		INIT = 0,
		MENU_INFOS,
		MENU_SELFTEST,
		MENU_CAN,
		MENU_USER,
		MENU_STRATEGY
	}menu_e;
	static menu_e previous_menu = INIT;
	static menu_e menu = INIT;
	bool_e entrance;

	//Navigation dans les menus.
	if(ask_for_menu_user)	//Flag prépondérant... ce menu rebaissera le booléen avec un appui SET. (car prioritaire devant le SWITCH !)
		menu = MENU_USER;
	else if(SWITCH_LCD)
		menu = MENU_STRATEGY;

	switch(menu)
	{
		case INIT:				if(initialized)				menu = MENU_INFOS;			break;
		case MENU_INFOS:		if(flag_bp_set)				menu = MENU_SELFTEST;		break;
		case MENU_SELFTEST:		if(flag_bp_set)				menu = MENU_CAN;			break;
		case MENU_CAN:			if(flag_bp_set)				menu = MENU_USER;			break;
		case MENU_USER:			if(flag_bp_set)			{	menu = MENU_INFOS;		ask_for_menu_user = FALSE;	}	break;
		case MENU_STRATEGY:		if(!SWITCH_LCD)				menu = MENU_INFOS;			break;
		default:
			menu = INIT;
			break;
	}

	entrance = (previous_menu != menu)? TRUE : FALSE;
	previous_menu = menu;

	//Affichage des menus et actions boutons.
	//Chaque menu est responsable de son raffraichissement raisonnable, et de la mise à jour des leds.
	switch(menu)
	{
		case INIT:															break;
		case MENU_INFOS:			LCD_menu_infos(entrance);				break;
		case MENU_SELFTEST:			LCD_menu_selftest(entrance);			break;
		case MENU_CAN:				LCD_menu_can_msg(entrance);				break;
		case MENU_USER:				LCD_menu_user(entrance);				break;
		case MENU_STRATEGY:			LCD_menu_strategy(entrance);			break;
		default:					menu = INIT;							break;
	}

	LCD_refresh_lines();	//Affichage sur le LCD des lignes ayant changées.
/*
		case MENU:
			if(LCD_transition()){
				display_menu();
				LCD_set_cursor(0,0);
				LCD_cursor_display(TRUE,TRUE);
				menu_choice = SELF_TEST;
			}

			break;
		case SELFTEST:
			if(LCD_transition()){
				display_selftest_result();
				LCD_set_cursor(0,0);
				menu_choice = 0;
			}

			break;
		default:
			break;
	}
	previous_state = state;
	change = FALSE;
*/

	flag_bp_ok = FALSE;
	flag_bp_set = FALSE;
	flag_bp_up = FALSE;
	flag_bp_down = FALSE;
}

static void LCD_menu_infos(bool_e init)
{
	if(init)
	{
		IHM_LEDS(TRUE, FALSE, FALSE, FALSE);	//Seul le bouton SET est actif.
		cursor = CURSOR_OFF;
	}

	if(init || global.env.pos.updated)
		display_pos(0);

	if(init || global.env.foe[0].updated || global.env.foe[1].updated)
		display_beacon(1);

	if(init || BRAIN_get_strat_updated())	//Temporaire !	//TODO add updated
		display_strats(2);

	if(init || free_msg_updated)
	{
		free_msg_updated = FALSE;
		display_debug_msg(3);
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
				IHM_LEDS(TRUE, TRUE, TRUE, TRUE);	//Tous !
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
			break;
	}

	if(update_lines)
	{
		for(i=1;i<LINE_NUMBER;i++)
			sprintf_line(i, SELFTEST_getError_string(SELFTEST_getError(index+i-1)));
	}
}

/* Affiche les quatre derniers messages can en mode CAN */
static void LCD_menu_can_msg(bool_e init)
{
	//TODO améliorer ce menu
	/*Passer à une vingtaine de messages mémorisés (ou plus)
	 * Permettre une navigation (UP / Down) dans ces message... et une désactivation (bouton OK) des nouveaux message pour pas se voir imposer un déplacement.
	 *
	 */
	if(init)
	{
		IHM_LEDS(TRUE, FALSE, FALSE, FALSE);	//SET (pour l'instant...)
		sprintf_line(0,"Menu messages CAN...");
		clear_line(1);
		clear_line(2);
		clear_line(3);
		can_msg_updated = TRUE;
		cursor = CURSOR_OFF;
	}
	if(can_msg_updated)
	{
		can_msg_updated = FALSE;
		LCD_display_CAN_msg(&can_msgs[index_can_msg], 1);
		LCD_display_CAN_msg(&can_msgs[(index_can_msg+1)%4], 2);
		LCD_display_CAN_msg(&can_msgs[(index_can_msg+2)%4], 3);
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
		sprintf_line(0,"Réglages stratégie..");
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
				edit_step = EDIT_T_END;
			if(flag_bp_ok)
				edit_step++;
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
				edit_step--;
			if(flag_bp_ok)
				edit_step++;
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
				edit_step--;
			if(flag_bp_ok)
				edit_step++;
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
				edit_step--;
			if(flag_bp_ok)
				edit_step++;
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
				edit_step--;
			if(flag_bp_ok)
				edit_step = EDIT_OFF;
			break;
		default:
			edit_step = EDIT_OFF;
			break;
	}



	for(i=0;i<LINE_NUMBER - 1;i++)
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

void LCD_add_can(CAN_msg_t * msg)
{
	can_msgs[index_can_msg] = *msg;					// On ajoute le message can a la pile can(du lcd)
	index_can_msg = (index_can_msg+1)%NB_CAN_MSG;	// On incrémente le pointeur de pile can(du lcd)
	can_msg_updated = TRUE;								// On force le rafraichissement de l'écran
}


static bool_e LCD_strat_number_update(void)
{
	static Uint8 previous_strat = 0x0;
	Uint8 new_strat = 0x0;
	Uint8 i;
	bool_e ret;
	ret = FALSE;

	strat_nb[0]=SWITCH_STRAT_1;
	strat_nb[1]=SWITCH_STRAT_2;
	strat_nb[2]=SWITCH_STRAT_3;

	for(i=0; i<3; i++){
		new_strat += strat_nb[i]<<i;
	}

	if(previous_strat != new_strat)
		ret = TRUE;
	previous_strat = new_strat;
	return ret;
}


//Line doit être entre 0 et 3 inclus.
//La ligne 0 correspond à la dernière ligne du menu principal (INFOS)
//Les 3 autres lignes correspondent aux lignes du menu utilisateur.
void LCD_printf(Uint8 line, char * chaine, ...)
{
	assert(line < 4);

	va_list args_list;
	va_start(args_list, chaine);
	vsnprintf(free_msg[line], 21, chaine, args_list);
	va_end(args_list);
	if(line>0)
		ask_for_menu_user = TRUE;
	free_msg_updated = TRUE;
}





////////////////////////////////////////////////////////////////////////
/// BUTTON ACTIONS

void LCD_button_down(void)
{
	flag_bp_down = TRUE;
}

void LCD_button_up(void){
	flag_bp_up = TRUE;
}

void LCD_button_ok(void){
	flag_bp_ok = TRUE;
	/*
	switch(state){
		case MENU:
			switch(menu_choice){
				case SELF_TEST:
					LCD_printf(0,"SelfTest asked");
					SELFTEST_ask_launch();
					break;
				case LAST_MATCH:
					LCD_printf(0,"Decharge match");
					SD_print_previous_match();
					break;
				case REGLAGE_ODO:
					LCD_printf(0,"Reglage ODO");
					//odometry_set(); //N'existe pas encore
					break;
				case RETURN:
				default:
					break;
			}
			LCD_cursor_display(FALSE,FALSE);
			LCD_switch_mode();
			break;
		case USER_MODE:
			LCD_free_control();
			break;
		case SELFTEST:
			LCD_switch_mode();
			break;
		default:
			if(state != INIT){
				LCD_take_control();
			}
			break;
	}*/
}

void LCD_button_set(void){
	flag_bp_set = TRUE;
}


////////////////////////////////////////////////////////////////////////
/// DISPLAY ACTIONS


/* Affiche la position du robot*/
static void display_pos(Uint8 line)
{
	Sint16 x,y,t;
	x = global.env.pos.x;
	y = global.env.pos.y;
	t = global.env.pos.angle;

	//Ecretages...
	x = MIN(x,9999);
	x = MAX(x,0);
	y = MIN(y,9999);
	y = MAX(y,0);
	t = MIN(t,PI4096);
	t = MAX(t,-PI4096);
	t = t*180/PI4096;	// Conversion en degré

	sprintf_line(line,"x%4d y%4d t%5d",x,y,t);
}

/* Affiche les infos de la balise*/
static void display_beacon(Uint8 line)
{
	Sint16 d1,d2,a1,a2;
	d1 = global.env.foe[0].dist;
	a1 = global.env.foe[0].angle;
	d2 = global.env.foe[1].dist;
	a2 = global.env.foe[1].angle;

	a1 *= (180/PI4096);
	a2 *= (180/PI4096);

	sprintf_line(line,"d%3d a%3d d%3d a%3d",d1,a1,d2,a2);
}

/* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
static void display_strats(Uint8 line)
{
	sprintf_line(line,BRAIN_get_current_strat_name());
}

/* Affiche le dernier message demandé par l'utilisateur sur la derniere ligne du mode info */
static void display_debug_msg(Uint8 line)
{
	sprintf_line(line,"%s",free_msg[0]);
}

/* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
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



