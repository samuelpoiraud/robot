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

// Etats de l'affichage de l'ecran
typedef enum
{
	INFO_s = 0,
	CAN_s,
	STRAT_CHOICE,
	MENU
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

const char *strategy[4] = {"STR","KDO","CAK","MOIS"};
Uint8 strat_number[4];

// Variable de menu
enum{
	SELF_TEST = 0,
	LAST_MATCH,
	BUFFER_FLUSH,
	RETURN
}menu_choice;

/* Tableau des 4 messages can à afficher */
typedef struct{
	CAN_msg_t msg[4];
	Uint8 start;
}can_t;

can_t message;

/* Position du robot sert seulement de buffer intermédiaire entre le can et l'ecran pour eviter certains bug d'interruptions */
Uint16 x_pos,y_pos,t_angle;
bool_e change; // Commande le rafraichissement de l'écran

/* Variable contenant un message libre*/
char free_msg[20];

////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS

/* Affiche la position du robot sur la première ligne du mode Info */
void display_pos(){
	char buf[20];
	Uint16 x,y;
	Sint16 t;

	x = x_pos;
	y = y_pos;
	t = t_angle;

	// Affichage de la position actuelle du robot
	LCD_set_cursor(0, 0);
	if(x<=2000 && x>=0)
		sprintf(buf,"x%4d",x);
	else
		sprintf(buf,"xOMAP");
	LCD_Write_text(buf);

	LCD_set_cursor(0, 6);
	if(y<=3000 && y>=0)
		sprintf(buf,"y%4d",y);
	else
		sprintf(buf,"yOMAP");
	LCD_Write_text(buf);

	LCD_set_cursor(0, 13);
	if(t<=PI4096 && t>= (-PI4096))
		sprintf(buf,"t%4d",t);
	else
		sprintf(buf,"t ERR");
		
	LCD_Write_text(buf);
}

/* Affiche les infos de la balise sur la seconde ligne du mode info */
void display_beacon(){
	char buf[20];
	Uint16 d1,d2,a1,a2;
#warning'simplement pour laffichage : a modifier par la suite'
	d1 = 360;
	a1 = 240;
	d2 = 21;
	a2 = 0;

	// Affichage de la position relative des balises
	LCD_set_cursor(1, 0);
	sprintf(buf,"d%3d a%3d d%3d a%3d",d1,a1,d2,a2);
	LCD_Write_text(buf);
}

/* Affiche les strats sélectionnées pour le match à la troisieme ligne du mode info */
void display_strats(){
	char buf[20];

	// Affichage de la position actuelle du robot
	LCD_set_cursor(2, 0);
	sprintf(buf,"%s%1d %s%1d %s%1d %s%1d", strategy[0], strat_number[0], strategy[1], strat_number[1], strategy[2], strat_number[2], strategy[3], strat_number[3]);
	LCD_Write_text(buf);
}

/* Affiche le dernier message demandé par l'utilisateur sur la derniere ligne du mode info */
void display_debug_msg(void){
	LCD_set_cursor(3, 0);
	LCD_Write_text(free_msg);
}

/* Affiche les quatre derniers messages can en mode CAN */
void LCD_display_can(){
	Uint8 start = message.start;

	display_can(message.msg[start], 0);
	display_can(message.msg[(start+1)%4], 1);
	display_can(message.msg[(start+1)%4], 2);
	display_can(message.msg[(start+1)%4], 3);
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

void init_LCD_interface(void){
	LCD_I2C_init();
	LCD_init();
	LCD_clear_display();
	LCD_cursor_display(FALSE,FALSE);
	state = INFO_s;
	previous_state = INFO_s;
	change = TRUE;
	x_pos = 0;
	y_pos = 0;
	t_angle = 360;
	message.start = 0;
	Uint8 i;
	strat = STR;
	for(i=0; i<4; i++){
		message.msg[i].sid = i;
		message.msg[i].size = 0;
		strat_number[i] = 0;
	}
}


////////////////////////////////////////////////////////////////////////
/// REFRESHING

void LCD_Update(void){

	if(LCD_transition())
		LCD_clear_display();

	switch(state){
		case INFO_s:
			if(change == TRUE){
				change = FALSE;
				display_pos();
				display_beacon();
				display_strats();
				display_debug_msg();
			}
			break;
		case CAN_s:
			LCD_display_can();
			break;
		case STRAT_CHOICE:
			if(change == TRUE){
				change = FALSE;
				display_strats();
			}
			switch(strat){
				case STR:
					LCD_set_cursor(2, 3);
					break;
				case KDO:
					LCD_set_cursor(2, 8);
					break;
				case CAK:
					LCD_set_cursor(2, 13);
					break;
				case MOIS:
					LCD_set_cursor(2, 19);
					break;
				default:
					break;
			}
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
	sprintf(free_msg,chaine);
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
	change = TRUE;
}

void LCD_switch_mode(void){
	switch(state){
		case STRAT_CHOICE:
			LCD_strat_number();
			break;
		case MENU:
			menu_choice = (menu_choice+1)%4;
			LCD_set_cursor(menu_choice, 0);
			break;
		case INFO_s:
			state = CAN_s;
			break;
		case CAN_s:
			state = INFO_s;
			break;
		default:
			break;
	}
	change = TRUE;

#warning 'pour le test'
	char chaine[20];
	sprintf(chaine, "%2d mon message",2),
	LCD_printf(chaine);

	CAN_msg_t msg;
	msg.sid = 0x2A;
	msg.size = 0;
	LCD_add_can(msg);

}


void LCD_strat_mode(void){
	if(state != STRAT_CHOICE){
		state = STRAT_CHOICE;
		strat = STR;
		LCD_cursor_display(TRUE,FALSE);
	}else{
		if(strat == MOIS){
			state = INFO_s;
			strat = STR;
			LCD_cursor_display(FALSE,FALSE);
		}else{
			strat++;
		}
	}
	change = TRUE;
}

void LCD_strat_number(void){
	char buf[20];
	strat_number[strat] = (strat_number[strat]+1)%4;
	sprintf(buf,"%1d",strat_number[strat]);
	switch(strat){
		case STR:
			LCD_set_cursor(2, 3);
			break;
		case KDO:
			LCD_set_cursor(2, 8);
			break;
		case CAK:
			LCD_set_cursor(2, 13);
			break;
		case MOIS:
			LCD_set_cursor(2, 19);
			break;
		default:
			break;
	}
	LCD_Write_text(buf);
}

void LCD_menu_mode(){
	if(state != MENU){
		LCD_cursor_display(TRUE,TRUE);
		state = MENU;
	}else{
		switch(menu_choice){
			case SELF_TEST:
				state = INFO_s;
				LCD_cursor_display(FALSE,FALSE);
				LCD_printf("Self test asked");
				break;
			case LAST_MATCH:
				LCD_clear_display();
				LCD_cursor_display(FALSE,FALSE);
				LCD_set_cursor(0,0);
				LCD_Write_text("Decharge en cours...");
				#ifdef EEPROM_CAN_MSG_ENABLE
					EEPROM_CAN_MSG_verbose_previous_match();
				#endif
				LCD_printf("Decharge done");
				state = INFO_s;
				break;
			case BUFFER_FLUSH:
				LCD_cursor_display(FALSE,FALSE);
				state = INFO_s;
				BUFFER_flush();
				LCD_printf("Buffer flush DONE\0");
				break;
			case RETURN:
				LCD_cursor_display(FALSE,FALSE);
				state = INFO_s;
				break;
			default:
				break;
		}
	}
}

