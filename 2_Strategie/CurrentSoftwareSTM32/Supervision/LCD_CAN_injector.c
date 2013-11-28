/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD__CAN_injector.c
 *	Package : Supervision
 *	Description : Affiche les messages can sur l'écran selon leur SID
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */


#include "../QS/QS_all.h"
#include "../QS/QS_CANmsgList.h"
#include "LCD_interface.h"
#include "LCD_MIDAS_4x20.h"

void LCD_incoming_can(CAN_msg_t* incomming_msg){
	switch(incomming_msg->sid){
		case ASSER_WARN_ANGLE:
		case ASSER_WARN_X:
		case ASSER_WARN_Y:
		case ASSER_STOP:
		case ASSER_GO_POSITION:
		case ASSER_GO_ANGLE:
		case ASSER_CALIBRATION:
		case CARTE_P_ASSER_ERREUR:
		case CARTE_P_ROBOT_CALIBRE:
		case CARTE_P_TRAJ_FINIE:
		case BROADCAST_START:
		case BROADCAST_STOP_ALL:
		case BROADCAST_COULEUR:
			LCD_add_can(*incomming_msg);
			break;
		default:
			break;
	}
}


void display_can(CAN_msg_t msg, Uint8 pos){
	char buf[20];

	LCD_set_cursor(pos, 0);
	sprintf(buf,"%3.3x",msg.sid);
	LCD_Write_text(buf);

	LCD_set_cursor(pos, 4);

	switch(msg.sid){
		case ASSER_WARN_ANGLE:
			LCD_Write_text("Warner angle");
			break;
		case ASSER_WARN_X:
			LCD_Write_text("Warner X");
			break;
		case ASSER_WARN_Y:
			LCD_Write_text("Warner Y");
			break;
		case ASSER_STOP:
			LCD_Write_text("ASSER STOP");
			break;
		case ASSER_GO_POSITION:
			LCD_Write_text("GOTO POS");
			break;
		case ASSER_GO_ANGLE:
			LCD_Write_text("GOTO ANGLE");
			break;
		case ASSER_CALIBRATION:
			LCD_Write_text("CALIBRATE");
			break;
		case CARTE_P_ASSER_ERREUR:
			LCD_Write_text("ASSER ERROR");
			break;
		case CARTE_P_ROBOT_CALIBRE:
			LCD_Write_text("CALIBRATE DONE");
			break;
		case CARTE_P_TRAJ_FINIE:
			LCD_Write_text("END OF MOVE");
			break;
		case BROADCAST_START:
			LCD_Write_text("BROAD. START");
			break;
		case BROADCAST_STOP_ALL:
			LCD_Write_text("BROAD. STOP ALL");
			break;
		case BROADCAST_COULEUR:
			LCD_Write_text("NEW COLOR");
			break;
		default:
			break;
	}
}
