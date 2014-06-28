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

//Filtrage des messages CAN entrant....
void LCD_incoming_can(CAN_msg_t * incomming_msg)
{
	switch(incomming_msg->sid){
		case PROP_WARN_ANGLE:
		case PROP_WARN_X:
		case PROP_WARN_Y:
		case PROP_STOP:
		case PROP_GO_POSITION:
		case PROP_GO_ANGLE:
		case PROP_CALIBRATION:
		case CARTE_P_PROP_ERREUR:
		case CARTE_P_ROBOT_CALIBRE:
		case CARTE_P_TRAJ_FINIE:
		case BROADCAST_START:
		case BROADCAST_STOP_ALL:
		case BROADCAST_COULEUR:
			LCD_add_can(incomming_msg);
			break;
		default:
			break;
	}
}


void LCD_display_CAN_msg(CAN_msg_t * msg, Uint8 line)
{
	char * str;
	switch(msg->sid)
	{
		case PROP_WARN_ANGLE:			str = "Warner angle";		break;
		case PROP_WARN_X:				str = "Warner X";			break;
		case PROP_WARN_Y:				str = "Warner Y";			break;
		case PROP_STOP:					str = "PROP STOP";			break;
		case PROP_GO_POSITION:			str = "GOTO POS";			break;
		case PROP_GO_ANGLE:				str = "GOTO ANGLE";			break;
		case PROP_CALIBRATION:			str = "CALIBRATION";		break;
		case CARTE_P_PROP_ERREUR:		str = "PROP ERROR";			break;
		case CARTE_P_ROBOT_CALIBRE:		str = "CALIBRATE DONE";		break;
		case CARTE_P_TRAJ_FINIE:		str = "END OF MOVE";		break;
		case BROADCAST_START:			str = "BROAD. START";		break;
		case BROADCAST_STOP_ALL:		str = "BROAD. STOP ALL";	break;
		case BROADCAST_COULEUR:			str = "NEW COLOR";			break;
		default:						str = "UNKNOW";				break;
	}
	sprintf_line(line,"%3.3x%s",msg->sid,str);
}
