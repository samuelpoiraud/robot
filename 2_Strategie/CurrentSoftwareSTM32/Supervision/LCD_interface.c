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
#include "LCD_MIDAS_4x20.h"
#include "LCD_interface.h"
#include "LCD_CAN_injector.h"
#include "Buffer.h"
#include "Eeprom_can_msg.h"
#include "config_pin.h"

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

const char *strategy[4] = {"STR","KDO","CAK","MSN"};
Uint8 strat_nb[4];

// Variable de menu
enum{
	SELF_TEST = 0,
	LAST_MATCH,
	BUFFER_FLUSH,
	RETURN
}menu_choice;


/* Position du robot sert seulement de buffer intermédiaire entre le can et l'ecran pour eviter certains bug d'interruptions */
Uint16 x_pos,y_pos,t_angle;
bool_e change; // Commande le rafraichissement de l'écran

/* Variable contenant un message libre*/
char free_msg[20];

/* L'utilisateur prend la main sur l'écran */
bool_e info_on;

/* Chaines affichées à l'écran */
char line[4][20];

////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS

/* Affiche toutes les lignes enregistrées du LCD */
void display_line(void){
	Uint8 i;
	LCD_set_cursor(0,0);
				LCD_Write_text(line[0]);
	if(change == TRUE){
		LED_ROUGE = 1;
		LCD_clear_display();
		for(i=0;i<4;i++){
			LCD_set_cursor(i,0);
			LCD_Write_text(line[i]);
		}
		change = FALSE;
	}
}

/* Affiche la position du robot sur la première ligne du mode Info */
void display_pos(){
	//On force l'affichage au démarrage sinon il n'y a pas de rafraichissement
	static Uint16 x = 0xFFFF;
	static Uint16 y = 0xFFFF;
	static Sint16 t = 0xFFFF;

	if(x!=x_pos || y!=y_pos || t!=t_angle){
		x = x_pos;
		y = y_pos;
		t = t_angle;

		if(!(x<=2000 && x>=0)) // En dehors du terrain
			x = 0;

		if(!(y<=3000 && y>=0))
			y = 0;

		if(t<=PI4096 && t>= (-PI4096))
			t= 0;

		sprintf(line[0],"x%4d y%4d t%5d",x,y,t);

		change = TRUE;
	}
}

/* Affiche les infos de la balise sur la seconde ligne du mode info */
void display_beacon(){
	char buf[20];
	static Uint16 d1,d2,a1,a2;
#warning'simplement pour laffichage : a modifier par la suite'
	d1 = 360;
	a1 = 240;
	d2 = 21;
	a2 = 0;
/*
	// Affichage de la position relative des balises
	LCD_set_cursor(pos, 0);
	sprintf(buf,"d%3d a%3d d%3d a%3d",d1,a1,d2,a2);
	LCD_Write_text(buf);
*/
	sprintf(line[1],"d%3d a%3d d%3d a%3d",d1,a1,d2,a2);
	//change = TRUE;
}

/* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
void display_strats(Uint8 pos){
	char buf[20];

	sprintf(line[2],"%s%1d %s%1d %s%1d %s%1d", strategy[0], strat_nb[0], strategy[1], strat_nb[1], strategy[2], strat_nb[2], strategy[3], strat_nb[3]);
}

/* Affiche le dernier message demandé par l'utilisateur sur la derniere ligne du mode info */
void display_debug_msg(Uint8 pos){

	sprintf(line[3],"%s",free_msg);
}

/* Affiche les quatre derniers messages can en mode CAN */
void LCD_display_can_msg(){
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
	LCD_Write_text("LAST MATCH'N EEPROM");
	LCD_set_cursor(2, 1);
	LCD_Write_text("BUFFER FLUSH");
	LCD_set_cursor(3, 1);
	LCD_Write_text("Return");

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
	initialized = TRUE;
}


////////////////////////////////////////////////////////////////////////
/// REFRESHING

void LCD_Update(void){
	if(!initialized)
		return;
	LED_USER = !LED_USER;
	if(LCD_transition())
		LCD_clear_display();

	if(global.env.ask_start == FALSE)
		LCD_strat_number_update();

	LCD_switch_mode();

	switch(state){
		case INFO_s:
			display_pos(0);
			display_beacon(1);
			display_strats(2);
			display_debug_msg(3);
			display_line();
			LED_ROUGE = 1;
			break;

		case CAN_s:
			LCD_display_can_msg();
			break;

		case USER_MODE:
			LED_ROUGE = 0;
			display_line();
			break;

		case MENU:
			if(LCD_transition()){
				display_menu();
				LCD_set_cursor(0,0);
				menu_choice = SELF_TEST;
			}

			break;
		default:
			break;
	}
	previous_state = state;
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
	message.msg[message.start].sid = msg.sid;
	message.msg[message.start].size = msg.size;
	message.start = (message.start+1)%4;
}

void LCD_change_pos(Uint16 x,Uint16 y,Uint16 t){
	x_pos = x;
	y_pos = y;
	t_angle = t;
}

void LCD_switch_mode(void){
	static lcd_state previous_state = INIT;


#warning 'ajouter le numero du port'
		if(SWITCH_COLOR == 1){
			LED_ORANGE = 1;
			state = INFO_s;
		}else{
			LED_ORANGE = 0;
			state = USER_MODE;
			if(state != previous_state){
				Uint8 i;
			}

			LCD_free_line("Free Mode", 0);
		}
		if(state != previous_state){
			change = TRUE;
		}
	previous_state = state;

}


void LCD_strat_number_update(){
	static Uint8 previous_strat = 0X0;
	Uint8 new_strat = 0x0;
	if(TRUE){//Mettre le port du switch str
		strat_nb[0]=1;
		new_strat = new_strat | 0x1;
	}else{
		strat_nb[0]=0;
		new_strat = new_strat && 0xFE;
	}
	if(TRUE){//Mettre le port du switch KDO
			strat_nb[1]=1;
			new_strat = new_strat | 0x2;
	}else{
			strat_nb[1]=0;
			new_strat = new_strat && 0xFD;
	}
	if(FALSE){//Mettre le port du switch cak
			new_strat = new_strat | 0x4;
			strat_nb[2]=1;
	}else{
			strat_nb[2]=0;
			new_strat = new_strat && 0xFB;
	}
	if(TRUE){//Mettre le port du switch mois
			strat_nb[3]=1;
			new_strat = new_strat | 0x8;
	}else{
			strat_nb[3]=0;
			new_strat = new_strat && 0xF7;
	}
	if(new_strat != previous_strat){
		change = TRUE;
		previous_strat = new_strat;
	}
}


void LCD_free_line(char buf[20], Uint8 pos){
	if(pos<0 || pos>3)
		return;
	else{
		sprintf(line[pos],"%s",buf);
	}
	change = TRUE;
}
