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

#include "config_use.h"

// Etats de l'affichage de l'ecran
typedef enum
{
	INFO_s = 0,
	CAN_s,
	USER_MODE,
	MENU,
	INIT = 0XFF
}lcd_state;

lcd_state state;
lcd_state previous_state;

// Etats relatifs au changement de strat
typedef enum
{
	STR = 0,
	KDO,
	CAK,
	MOIS
}strat_s;

strat_s strat;

const char *strategy[6] = {"STR","TRG","MSN","BAL","FRT","SCN"};
Uint8 strat_nb[4];

// Variable de menu
enum{
	SELF_TEST = 0,
	LAST_MATCH,
	REGLAGE_ODO,
	RETURN
}menu_choice;


/* Position du robot sert seulement de buffer intermédiaire entre le can et l'ecran pour eviter certains bug d'interruptions */
Sint16 x_pos,y_pos,t_angle;
bool_e change; // Commande le rafraichissement de l'écran

/* Variable contenant un message libre*/
char free_msg[5][20];


/* Chaines affichées à l'écran */
char line[4][20];

////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS

/* Affiche toutes les lignes enregistrées du LCD */
void display_line(void){
	Uint8 i;
	if(change == TRUE){
		LCD_clear_display();
		for(i=0;i<4;i++){
			LCD_set_cursor(i,0);
			if(state == INFO_s)
				LCD_Write_text(line[i]);
			else if(state == USER_MODE)
				LCD_Write_text(free_msg[i+1]);
		}
		change = FALSE;
	}
}

/* Affiche la position du robot sur la première ligne du mode Info */
void display_pos(){
	Sint16 x,y,t;

	if(global.env.pos.updated == TRUE){
		x = x_pos;
		y = y_pos;
		t = t_angle;

		if(!(x<=2000 && x>=0)) // En dehors du terrain
			x = 0;

		if(!(y<=3000 && y>=0))
			y = 0;

		if(t <= (-PI4096) && t >= PI4096)
			t = 0;

		t = t*180/PI4096;	// Conversion en degré

		sprintf(line[0],"x%4d y%4d t%5d",x,y,t);

		change = TRUE;
	}
}

/* Affiche les infos de la balise sur la seconde ligne du mode info */
void display_beacon(){
	static Sint16 d1,d2,a1,a2;

	if(global.env.foe[0].updated==TRUE || global.env.foe[1].updated==TRUE){
		d1 = global.env.foe[0].dist;
		a1 = global.env.foe[0].angle;
		d2 = global.env.foe[1].dist;
		a2 = global.env.foe[1].angle;

		a1 *= (180/PI4096);
		a2 *= (180/PI4096);
		d1 *= 2;
		d2 *= 2;

		sprintf(line[1],"d%3d a%3d d%3d a%3d",d1,a1,d2,a2);
		change = TRUE;
	}
}

/* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
void display_strats(Uint8 pos){
	if(QS_WHO_AM_I_get() == SMALL_ROBOT){
		sprintf(line[2],"%s%1d %s%1d %s%1d", strategy[0], strat_nb[0], strategy[1], strat_nb[1], strategy[2], strat_nb[2]);
	}else{
		sprintf(line[2],"%s%1d %s%1d %s%1d %s%1d", strategy[0], strat_nb[0]+5, strategy[3], strat_nb[1], strategy[4], strat_nb[2], strategy[5], strat_nb[3]);
	}
}

/* Affiche le dernier message demandé par l'utilisateur sur la derniere ligne du mode info */
void display_debug_msg(Uint8 pos){

	sprintf(line[3],"%s",free_msg[0]);
}

/* Affiche les quatre derniers messages can en mode CAN */
void LCD_display_can_msg(){

	if(change == TRUE){
		LCD_clear_display();
		change = FALSE;
	}

	Uint8 start = message.start;

	display_can(message.msg[start], 0);
	display_can(message.msg[(start+1)%4], 1);
	display_can(message.msg[(start+2)%4], 2);
	display_can(message.msg[(start+3)%4], 3);
}

/* Previent un changement de mode pour le nettoyage de l'afichage et éviter le scintillement */
bool_e LCD_transition(void){
	return (state == previous_state)? FALSE : TRUE;
}

void display_menu(void){
	LCD_set_cursor(0, 1);
	LCD_Write_text("SELF TEST");
	LCD_set_cursor(1, 1);
	LCD_Write_text("LAST MATCH'N SD");
	LCD_set_cursor(2, 1);
	LCD_Write_text("Reglage Odo");
	LCD_set_cursor(3, 1);
	LCD_Write_text("Return");
}

void IHM_LEDS(void){

	LED_IHM_OK = (state != INIT);
	LED_IHM_UP = (state == MENU);
	LED_IHM_DOWN = (state == MENU);
	LED_IHM_SET = (state != INIT);
}

////////////////////////////////////////////////////////////////////////
/// INIT
static bool_e initialized = FALSE;

void init_LCD_interface(void){
	LCD_I2C_init();
	LCD_init();
	LCD_clear_display();
	LCD_cursor_display(FALSE,FALSE);
	state = INFO_s;
	previous_state = INFO_s;
	change = TRUE;
	x_pos = global.env.pos.x;
	y_pos = global.env.pos.y;
	t_angle = global.env.pos.angle;
	message.start = 0;
	Uint8 i;
	strat = STR;
	for(i=0; i<4; i++){
		message.msg[i].sid = i;
		message.msg[i].size = 0;
		strat_nb[i] = 0;
	}

	IHM_LEDS();

	initialized = TRUE;
}


////////////////////////////////////////////////////////////////////////
/// REFRESHING

void LCD_Update(void){
	if(!initialized)
		return;

	if(global.env.ask_start == FALSE)
		LCD_strat_number_update();

	if(state != USER_MODE && state!=MENU)
		LCD_switch_mode();

	if(global.env.pos.updated == TRUE){
		LCD_change_pos();
	}


	if(LCD_transition()){
		LCD_clear_display();
		IHM_LEDS();
		change = TRUE;
	}

	switch(state){
		case INFO_s:
			display_pos(0);
			display_beacon(1);
			display_strats(2);
			display_debug_msg(3);
			display_line();
			break;

		case CAN_s:
			LCD_display_can_msg();
			break;

		case USER_MODE:
			display_line();
			break;

		case MENU:
			if(LCD_transition()){
				display_menu();
				LCD_set_cursor(0,0);
				LCD_cursor_display(TRUE,TRUE);
				menu_choice = SELF_TEST;
			}

			break;
		default:
			break;
	}
	previous_state = state;
	change = FALSE;
}


////////////////////////////////////////////////////////////////////////
/// WRITING ACCESSORS

void LCD_printf(char* format){
	char chaine[20];
	Uint8 i = 0;
	while(i<20 && format[i]!='\0'){
		chaine[i] = format[i];
		i++;
	}
	sprintf(line[3],chaine);
	change = TRUE;
}


void LCD_add_can(CAN_msg_t msg){
	message.msg[message.start].sid = msg.sid;	// On ajoute le message can a la pile can(du lcd)
	message.msg[message.start].size = msg.size;
	message.start = (message.start+1)%4;		// On incrémente le pointeur de pile can(du lcd)
	change = TRUE;								// On force le rafraichissement de l'écran
}

void LCD_change_pos(void){
	x_pos = global.env.pos.x;
	y_pos = global.env.pos.y;
	t_angle = global.env.pos.angle;
}

void LCD_switch_mode(void){
	static lcd_state previous_state = INIT;
		if(SWITCH_LCD == 1){
			state = INFO_s;
		}else{
			state = CAN_s;
		}
		if(state != previous_state){
			change = TRUE;
		}
	previous_state = state;

}


void LCD_strat_number_update(){
	static Uint8 previous_strat = 0x0;
	Uint8 new_strat = 0x0;
	Uint8 i;

	strat_nb[0]=SWITCH_STRAT_1;
	strat_nb[1]=SWITCH_STRAT_2;
	strat_nb[2]=SWITCH_STRAT_3;

	for(i=0; i<3; i++){
		new_strat += strat_nb[i]<<i;
	}

	if(previous_strat != new_strat)
		change = TRUE;
	previous_strat = new_strat;
}


void LCD_free_line(char buf[20], Uint8 pos){
	if(pos<0 || pos>4)
		return;
	else{
		sprintf(free_msg[pos],"%s",buf);
	}
	change = TRUE;
}

void LCD_take_control(){
	state = USER_MODE;
}

void LCD_free_control(){
	LCD_switch_mode();
}

////////////////////////////////////////////////////////////////////////
/// BUTTON ACTIONS

void LCD_button_minus(void){
	switch(state){
		case MENU:
			if(menu_choice == 0)
				menu_choice = 3;
			else
				menu_choice--;
			LCD_set_cursor(menu_choice,0);
			break;
		default:
			break;
	}
}

void LCD_button_plus(void){
	switch(state){
		case MENU:
			if(menu_choice == 3)
				menu_choice = 0;
			else
				menu_choice++;
			LCD_set_cursor(menu_choice,0);
			break;
		default:
			break;
	}
}

void LCD_button_ok(void){
	switch(state){
		case MENU:
			switch(menu_choice){
				case SELF_TEST:
					LCD_free_line("SelfTest asked",0);
					break;
				case LAST_MATCH:
					LCD_free_line("Decharge match",0);
					break;
				case REGLAGE_ODO:
					LCD_free_line("Reglage ODO",0);
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
		default:
			if(state != INIT){
				LCD_take_control();
			}
			break;
	}
}

void LCD_button_set(void){
	switch(state){
		case MENU:
			LCD_cursor_display(FALSE,FALSE);
			LCD_switch_mode();
			break;
		default:
			if(state != INIT){
				state = MENU;
			}
			break;
	}
}
