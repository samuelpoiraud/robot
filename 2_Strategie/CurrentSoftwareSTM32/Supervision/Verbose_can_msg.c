/*
 *	Club Robot ESEO 2011
 *	Check Norris
 *
 *	Fichier : Verbose_can_msg.c
 *	Package : Supervision
 *	Description : Module d'affichage dans un texte "humain" du contenu d'un message can.
 *	Auteur : Nirgal
 *	Version 201302
 */

#define VERBOSE_CAN_MSG_C
#include "Verbose_can_msg.h"
#include "../QS/QS_outputlog.h"
Sint32 print_ir_result(CAN_msg_t * msg, char * string);
Sint32 print_us_result(CAN_msg_t * msg, char * string);

Sint32 print_broadcast_start_infos(CAN_msg_t * msg, char * string);

#define	u16(x,y)		(U16FROMU8(msg->data[x], msg->data[y]))
#define s16(x,y)		((Sint16)(U16FROMU8(msg->data[x], msg->data[y])))
#define	u8(x)			(msg->data[x])
#define	s8(x)			((Sint8)(msg->data[x]))
#define angle_deg(x, y) ((Sint16)((((Sint32)((Sint16)(U16FROMU8(msg->data[x], msg->data[y]))))*180/PI4096)))
#define angle_rad(x, y) s16(x,y)
#define test(condition,string)	if(condition)	string+=sprintf(string,string)

Uint16 VERBOSE_CAN_MSG_sprint(CAN_msg_t * msg, char * string)
{
	Uint8 i;
	char * ret = string;
		string+=sprintf(string,"sid=");
		switch(msg->sid)
		{
			case BROADCAST_START:					string+=sprintf(string,"%.3x BROADCAST_START                  ", BROADCAST_START					);	break;
			case BROADCAST_STOP_ALL:				string+=sprintf(string,"%.3x BROADCAST_STOP_ALL               ", BROADCAST_STOP_ALL				);	break;
			case BROADCAST_COULEUR:					string+=sprintf(string,"%.3x BROADCAST_COULEUR                ", BROADCAST_COULEUR				);	break;
			case BROADCAST_POSITION_ROBOT:			string+=sprintf(string,"%.3x BROADCAST_POSITION_ROBOT         ", BROADCAST_POSITION_ROBOT			);	break;
			case DEBUG_CARTE_P:						string+=sprintf(string,"%x DEBUG_CARTE_P                    	", DEBUG_CARTE_P					);	break;
			case DEBUG_FOE_POS:						string+=sprintf(string,"%x DEBUG_FOE_POS                    	", DEBUG_FOE_POS					);	break;
			case DEBUG_ELEMENT_UPDATED:				string+=sprintf(string,"%x DEBUG_ELEMENT_UPDATED            	", DEBUG_ELEMENT_UPDATED			);	break;
			case DEBUG_ASSER_POINT_FICTIF:			string+=sprintf(string,"%x DEBUG_ASSER_POINT_FICTIF         	", DEBUG_ASSER_POINT_FICTIF			);	break;
			case SUPER_EEPROM_RESET:				string+=sprintf(string,"%x SUPER_EEPROM_RESET               	", SUPER_EEPROM_RESET				);	break;
			case SUPER_EEPROM_PRINT_MATCH:			string+=sprintf(string,"%x SUPER_EEPROM_PRINT_MATCH         	", SUPER_EEPROM_PRINT_MATCH			);	break;
			case ACT_DO_SELFTEST:					string+=sprintf(string,"%x ACT_DO_SELFTEST           	", ACT_DO_SELFTEST			);	break;
			case PROP_DO_SELFTEST:					string+=sprintf(string,"%x PROP_DO_SELFTEST         	", PROP_DO_SELFTEST			);	break;
			case BEACON_DO_SELFTEST:				string+=sprintf(string,"%x BEACON_DO_SELFTEST        	", BEACON_DO_SELFTEST		);	break;
			case STRAT_ACT_SELFTEST_DONE :			string+=sprintf(string,"%x STRAT_ACT_SELFTEST_DONE                     	", STRAT_ACT_SELFTEST_DONE 					);	break;
			case STRAT_PROP_SELFTEST_DONE :			string+=sprintf(string,"%x STRAT_PROP_SELFTEST_DONE                   	", STRAT_PROP_SELFTEST_DONE 					);	break;
			case STRAT_BEACON_IR_SELFTEST_DONE:		string+=sprintf(string,"%x STRAT_BEACON_IR_SELFTEST_DONE               	", STRAT_BEACON_IR_SELFTEST_DONE				);	break;
			case STRAT_BEACON_US_SELFTEST_DONE:		string+=sprintf(string,"%x STRAT_BEACON_US_SELFTEST_DONE               	", STRAT_BEACON_US_SELFTEST_DONE				);	break;
			case CARTE_P_TRAJ_FINIE:				string+=sprintf(string,"%x CARTE_P_TRAJ_FINIE               	", CARTE_P_TRAJ_FINIE				);	break;
			case CARTE_P_ASSER_ERREUR:				string+=sprintf(string,"%x CARTE_P_ASSER_ERREUR             	", CARTE_P_ASSER_ERREUR				);	break;
			case CARTE_P_ROBOT_FREINE:				string+=sprintf(string,"%x CARTE_P_ROBOT_FREINE             	",	CARTE_P_ROBOT_FREINE			);	break;
			case CARTE_P_ROBOT_CALIBRE:				string+=sprintf(string,"%x CARTE_P_ROBOT_CALIBRE            	", CARTE_P_ROBOT_CALIBRE			);	break;
			case ASSER_GO_ANGLE:					string+=sprintf(string,"%x ASSER_GO_ANGLE                   	", ASSER_GO_ANGLE 					);	break;
			case ASSER_GO_POSITION:					string+=sprintf(string,"%x ASSER_GO_POSITION                	", ASSER_GO_POSITION 				);	break;
			case ASSER_SET_POSITION:				string+=sprintf(string,"%x ASSER_SET_POSITION               	", ASSER_SET_POSITION				);	break;
			case ASSER_SEND_PERIODICALLY_POSITION: 	string+=sprintf(string,"%x ASSER_SEND_PERIODICALLY_POSITION 	", ASSER_SEND_PERIODICALLY_POSITION	);	break;
			case ASSER_STOP:						string+=sprintf(string,"%x ASSER_STOP                       	", ASSER_STOP						);	break;
			case ASSER_TELL_POSITION:				string+=sprintf(string,"%x ASSER_TELL_POSITION              	", ASSER_TELL_POSITION				);	break;
			case ASSER_TYPE_ASSERVISSEMENT:			string+=sprintf(string,"%x ASSER_TYPE_ASSERVISSEMENT        	", ASSER_TYPE_ASSERVISSEMENT		);	break;
			case ASSER_RUSH_IN_THE_WALL:			string+=sprintf(string,"%x ASSER_RUSH_IN_THE_WALL           	", ASSER_RUSH_IN_THE_WALL			);	break;
			case ASSER_CALIBRATION:					string+=sprintf(string,"%x ASSER_CALIBRATION                	", ASSER_CALIBRATION				);	break;
			case ASSER_WARN_ANGLE:					string+=sprintf(string,"%x ASSER_WARN_ANGLE                 	", ASSER_WARN_ANGLE					);	break;
			case ASSER_WARN_X:						string+=sprintf(string,"%x ASSER_WARN_X                     	", ASSER_WARN_X						);	break;
			case ASSER_WARN_Y:						string+=sprintf(string,"%x ASSER_WARN_Y                     	", ASSER_WARN_Y						);	break;
			case CARTE_ASSER_FIN_ERREUR:			string+=sprintf(string,"%x CARTE_ASSER_FIN_ERREUR           	", CARTE_ASSER_FIN_ERREUR			);	break;
	/*		case ACT_DCM_POS:						string+=sprintf(string,"%x ACT_DCM_POS                      	", ACT_DCM_POS						);	break;
			case ACT_CLAMP_PREPARED:				string+=sprintf(string,"%x ACT_CLAMP_PREPARED               	", ACT_CLAMP_PREPARED				);	break;
			case ACT_PAWN_GOT:						string+=sprintf(string,"%x ACT_PAWN_GOT                     	", ACT_PAWN_GOT						);	break;
			case ACT_PAWN_FILED:					string+=sprintf(string,"%x ACT_PAWN_FILED                   	", ACT_PAWN_FILED 					);	break;
			case ACT_PAWN_DETECTED:					string+=sprintf(string,"%x ACT_PAWN_DETECTED              	", ACT_PAWN_DETECTED 				);	break;
			case ACT_PAWN_NO_LONGER_DETECTED :		string+=sprintf(string,"%x ACT_PAWN_NO_LONGER_DETECTED    	", ACT_PAWN_NO_LONGER_DETECTED 		);	break;
			case ACT_EMPTY:							string+=sprintf(string,"%x ACT_EMPTY                        	", ACT_EMPTY						);	break;
			case ACT_FULL:							string+=sprintf(string,"%x ACT_FULL                         	", ACT_FULL							);	break;
			case ACT_FAILURE:						string+=sprintf(string,"%x ACT_FAILURE                      	", ACT_FAILURE						);	break;
			case ACT_READY:							string+=sprintf(string,"%x ACT_READY                        	", ACT_READY						);	break;
			case ACT_DCM_SETPOS:					string+=sprintf(string,"%x ACT_DCM_SETPOS                   	", ACT_DCM_SETPOS					);	break;
			case ACT_PREPARE_CLAMP:					string+=sprintf(string,"%x ACT_PREPARE_CLAMP                	", ACT_PREPARE_CLAMP				);	break;
			case ACT_TAKE_PAWN:						string+=sprintf(string,"%x ACT_TAKE_PAWN                    	", ACT_TAKE_PAWN					);	break;
			case ACT_FILE_PAWN:						string+=sprintf(string,"%x ACT_FILE_PAWN                    	", ACT_FILE_PAWN					);	break;
	*/		case BEACON_ENABLE_PERIODIC_SENDING: 	string+=sprintf(string,"%x BEACON_ENABLE_PERIODIC_SENDING   	", BEACON_ENABLE_PERIODIC_SENDING	);	break;
			case BEACON_DISABLE_PERIODIC_SENDING: 	string+=sprintf(string,"%x BEACON_DISABLE_PERIODIC_SENDING  	", BEACON_DISABLE_PERIODIC_SENDING	);	break;
			case BEACON_ADVERSARY_POSITION_IR:		string+=sprintf(string,"%x BEACON_ADVERSARY_POSITION_IR     	", BEACON_ADVERSARY_POSITION_IR		);	break;
			case BEACON_ADVERSARY_POSITION_US:		string+=sprintf(string,"%x BEACON_ADVERSARY_POSITION_US     	", BEACON_ADVERSARY_POSITION_US		);	break;
			case DEBUG_STRAT_STATE_CHANGED:			string+=sprintf(string,"%x DEBUG_STRAT_STATE_CHANGED          ", DEBUG_STRAT_STATE_CHANGED		);	break;

			default:
													string+=sprintf(string,"SID=%x                            | ", msg->sid);								break;
		}


	// Les lignes commentées à gauche sont celles qui ne sont pas traitées.
		switch(msg->sid)
		{
			case BROADCAST_START:					string+=print_broadcast_start_infos(msg, string);									break;
		//	case BROADCAST_STOP_ALL:				string+=sprintf(string,"|\n");												break;
			case IR_ERROR_RESULT:					string+=print_ir_result(msg, string);												break;
			case US_ERROR_RESULT:					string+=print_us_result(msg, string);												break;
			case BROADCAST_COULEUR:					string+=sprintf(string,"| CouleurEst %s\n", (u8(0))?"BLEU":"ROUGE"	);		break;
			case BROADCAST_POSITION_ROBOT :			string+=sprintf(string,"| JeSuisEn  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		//	case DEBUG_CARTE_P:						string+=sprintf(string,"|\n");												break;
	//		case DEBUG_FOE_POS:						string+=sprintf(string,"|\n");												break;
	//		case DEBUG_ELEMENT_UPDATED:				string+=sprintf(string,"|\n");												break;
	//		case DEBUG_ASSER_POINT_FICTIF:			string+=sprintf(string,"|\n");												break;
		//	case SUPER_EEPROM_RESET:				string+=sprintf(string,"|\n");												break;
			case SUPER_EEPROM_PRINT_MATCH:			string+=sprintf(string,"| match_address_x8 : %x\n", u16(0,1));				break;
		//	case SUPER_ASK_STRAT_SELFTEST:			string+=sprintf(string,"|\n");												break;
		//	case SUPER_ASK_ACT_SELFTEST:			string+=sprintf(string,"|\n");												break;
		//	case SUPER_ASK_ASSER_SELFTEST:			string+=sprintf(string,"|\n");												break;
		//	case SUPER_ASK_BEACON_SELFTEST:			string+=sprintf(string,"|\n");												break;
	//		case SUPER_CONFIG_IS:					string+=sprintf(string,"|\n");												break;
	//		case STRAT_SELFTEST:					string+=sprintf(string,"|\n");												break;
	//		case ACT_SELFTEST :						string+=sprintf(string,"|\n");												break;
	//		case ASSER_SELFTEST :					string+=sprintf(string,"|\n");												break;
	//		case BEACON_IR_SELFTEST:				string+=sprintf(string,"|\n");												break;
	//		case BEACON_US_SELFTEST:				string+=sprintf(string,"|\n");												break;
			case CARTE_P_TRAJ_FINIE:				string+=sprintf(string,"| J'arrive  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
			case CARTE_P_ASSER_ERREUR:				string+=sprintf(string,"| J'erreur  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
			case CARTE_P_ROBOT_FREINE:				string+=sprintf(string,"| J'freine  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		//	case CARTE_P_ROBOT_CALIBRE:				string+=sprintf(string,"|\n");												break;
			case ASSER_GO_ANGLE:					string+=sprintf(string,"| VaAngle   teta=%d=%d° %s %s %s %d%s %s\n",  angle_rad(1, 2),  angle_deg(1, 2), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);						break;
			case ASSER_GO_POSITION:					string+=sprintf(string,"| VaPos     x=%d y=%d %s %s %s vitesse %d%s %s\n", u16(1,2), u16(3,4), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);	break;
			case ASSER_SET_POSITION:				string+=sprintf(string,"| PrendPos  X=%d | Y=%d | teta=0x%x=%d°\n", u16(0,1), u16(2,3),angle_rad(4, 5),  angle_deg(4, 5));													break;
			case ASSER_SEND_PERIODICALLY_POSITION: 	string+=sprintf(string,"| DitPos    période=%d | translation = %dmm | rotation = %d°\n", u16(0,1),u16(2, 3),  angle_deg(4, 5));										break;
		//	case ASSER_STOP:						string+=sprintf(string,"|\n");												break;
		//	case ASSER_TELL_POSITION:				string+=sprintf(string,"|\n");												break;
			case ASSER_TYPE_ASSERVISSEMENT:			string+=sprintf(string,"| asservissement rotation %sactivée | asservissement translation %sactivée\n", (u8(0))?"":"dés", (u8(1))?"":"dés");						break;
			case ASSER_RUSH_IN_THE_WALL:			string+=sprintf(string,"| sens %s | asservissement rotation %sactivée\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"), (u8(1))?"":"dés");				break;
			case ASSER_CALIBRATION:					string+=sprintf(string,"| Calibre   sens %s\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"));																	break;
			case ASSER_WARN_ANGLE:					string+=(u8(0))?(sprintf(string,"| avertisseur en teta=0x%d=%d°\n",angle_rad(0, 1), angle_deg(0, 1))):(sprintf(string,"désactivation de l'avertisseur en angle\n"));	break;
			case ASSER_WARN_X:						string+=(u8(0))?(sprintf(string,"| avertisseur en x=%d\n",u16(0,1))):(sprintf(string,"désactivation de l'avertisseur en X\n"));											break;
			case ASSER_WARN_Y:						string+=(u8(0))?(sprintf(string,"| avertisseur en y=%d\n",u16(0,1))):(sprintf(string,"désactivation de l'avertisseur en Y\n"));											break;
		//	case CARTE_ASSER_FIN_ERREUR:			string+=sprintf(string,"|\n");												break;
	/*		case ACT_DCM_POS:
			case ACT_FAILURE:
			case ACT_READY:
			case ACT_EMPTY:							if(msg->sid == ACT_EMPTY)			string+=sprintf(string,"| Pince vide ");			//PAS DE BREAK !
			case ACT_FULL:							if(msg->sid == ACT_FULL)			string+=sprintf(string,"| Pince pleine ");		//PAS DE BREAK !
													switch(u8(0))
													{
														case ACT_CLAMP_FRONT: 	string+=sprintf(string,"|Pince avant\n");			break;
														case ACT_CLAMP_BACK: 	string+=sprintf(string,"|Pince arrière\n");		break;
														case ACT_LIFT_FRONT: 	string+=sprintf(string,"|Ascenseur avant\n");		break;
														case ACT_LIFT_BACK : 	string+=sprintf(string,"|Ascenseur arrière\n");	break;
														default: 				string+=sprintf(string,"|Argument invalide\n");	break;
													}																	break;
			case ACT_CLAMP_PREPARED:				if(msg->sid == ACT_CLAMP_PREPARED)	string+=sprintf(string,"| Pince préparée");	//PAS DE BREAK !
			case ACT_PAWN_GOT:						if(msg->sid == ACT_PAWN_GOT)		string+=sprintf(string,"| Prendre pion");		//PAS DE BREAK !
			case ACT_PAWN_FILED:					if(msg->sid == ACT_PAWN_FILED)		string+=sprintf(string,"| Pion relaché");		//PAS DE BREAK !
			case ACT_PAWN_DETECTED:					if(msg->sid == ACT_PAWN_DETECTED)	string+=sprintf(string,"| Pion magnétisé");	//PAS DE BREAK !
													switch(u8(0))
													{
														case ACT_FRONT: 		string+=sprintf(string," avant\n");				break;
														case ACT_BACK: 			string+=sprintf(string," arrière\n");				break;
														default: 				string+=sprintf(string," Argument invalide\n");	break;
													}																	break;
																														break;
			case ACT_DCM_SETPOS:
													switch(u8(1))
													{
														case ACT_CLAMP_CLOSED_ON_PAWN: 	string+=sprintf(string,"|fermer sur le pion la pince ");			break;
														case ACT_CLAMP_OPENED: 			string+=sprintf(string,"|ouvrir la pince ");						break;
														case ACT_CLAMP_CLOSED: 			string+=sprintf(string,"|fermer la pince ");						break;
														case ACT_LIFT_BOTTOM : 			string+=sprintf(string,"|descendre l'ascenceur ");				break;
														case ACT_LIFT_MIDDLE:			string+=sprintf(string,"|placer au milieu l'ascenseur ");			break;
														case ACT_LIFT_TOP:				string+=sprintf(string,"|monter l'ascenseur ");					break;
														case ACT_LIFT_MOVE_TOWER:		string+=sprintf(string,"|déplacer une tour avec l'ascenceur ");	break;
														default: 						string+=sprintf(string,"|Argument invalide ");					break;
													}
													switch(u8(0))
													{
														case ACT_CLAMP_FRONT: 	string+=sprintf(string,"avant\n");				break;
														case ACT_CLAMP_BACK: 	string+=sprintf(string,"arrière\n");				break;
														case ACT_LIFT_FRONT: 	string+=sprintf(string,"avant\n");				break;
														case ACT_LIFT_BACK : 	string+=sprintf(string,"arrière\n");				break;
														default: 				string+=sprintf(string,"|Argument invalide\n");	break;
													}																	break;
			case ACT_PREPARE_CLAMP:					string+=sprintf(string,"| Préparer la pince et ");
													switch(u8(1))
													{
														case ACT_LIFT_BOTTOM : 			string+=sprintf(string,"descendre l'ascenceur ");					break;
														case ACT_LIFT_MIDDLE:			string+=sprintf(string,"placer au milieu l'ascenseur ");			break;
														case ACT_LIFT_TOP:				string+=sprintf(string,"monter l'ascenseur ");					break;
														case ACT_LIFT_MOVE_TOWER:		string+=sprintf(string,"déplacer une tour avec l'ascenceur ");	break;
														default: 						string+=sprintf(string,"Argument invalide ");						break;
													}
													switch(u8(0))
													{
														case ACT_FRONT: 				string+=sprintf(string,"avant\n");								break;
														case ACT_BACK: 					string+=sprintf(string,"arrière\n");								break;
														default: 						string+=sprintf(string,"|Argument invalide\n");					break;
													}																	break;
			case ACT_TAKE_PAWN:						string+=sprintf(string,"| Ramasser");		//Pas de break.. même suite que ACT_FILE_PAWN
			case ACT_FILE_PAWN:
													string+=sprintf(string,"%s un pion à l'", (msg->sid==ACT_FILE_PAWN)?"| Lâcher":"");
													switch(u8(0))
													{
														case ACT_FRONT: 				string+=sprintf(string,"avant");									break;
														case ACT_BACK: 					string+=sprintf(string,"arrière");								break;
														default: 						string+=sprintf(string,"Argument invalide");						break;
													}
													string+=sprintf(string," et le stocker ");
													switch(u8(1))
													{
														case ACT_LIFT_BOTTOM : 			string+=sprintf(string,"en bas");									break;
														case ACT_LIFT_MIDDLE:			string+=sprintf(string,"au milieu");								break;
														case ACT_LIFT_TOP:				string+=sprintf(string,"en haut");								break;
														case ACT_LIFT_MOVE_TOWER:		string+=sprintf(string,"en position de déplacement de tour");		break;
														default: 						string+=sprintf(string,"Argument invalide ");						break;
													}
													switch(u8(2))
													{
														case ACK_LIFT_ACTION: 			string+=sprintf(string,". Ack ascenseur\n");				break;
														case ACK_CLAMP_ACTION: 			string+=sprintf(string,". Ack pince\n");					break;
														default: 						string+=sprintf(string,"|Argument invalide\n");					break;
													}																	break;
	*/		//case BEACON_ENABLE_PERIODIC_SENDING: 	string+=sprintf(string,"|\n");												break;
			//case BEACON_DISABLE_PERIODIC_SENDING: 	string+=sprintf(string,"|\n");												break;
			case BEACON_ADVERSARY_POSITION_IR:		if(u8(0) || u8(4))
														string+=sprintf(string,"ERRs:0x%x 0x%x|",u8(0), u8(4));
													string+=sprintf(string,"angleR1=%d |dR1=%dcm |angleR2=%d |dR2=%dcm \n", angle_deg(1,2), (Uint16)(u8(3)),angle_deg(5,6), (Uint16)(u8(7)));	break;
			case BEACON_ADVERSARY_POSITION_US:		if(u8(0))
														string+=sprintf(string,"ERRs:0x%x 0x%x|",u8(0), u8(4));
													string+=sprintf(string,"dR1=%dmm|dR2=%dmm|id=%d\n", u16(1,2), u16(5,6), u8(3));								break;
			case DEBUG_STRAT_STATE_CHANGED:			string+=sprintf(string,"| Strat state changed 0x%04X: %d -> %d", u16(0,1), u8(2), u8(3));
													if(msg->size > 4) {
														string+=sprintf(string," | params:");
														for(i = 4; i < msg->size; i++)
															string+=sprintf(string," 0x%02X(%d)", u8(i), u8(i));

													}
													string+=sprintf(string,"\n");
			break;	/* DEBUG_STRAT_STATE_CHANGED */

			default:
				if(msg->size)
				{
					string+=sprintf(string,"| DATA=");
					for(i = 0; i<msg->size && i<8; i++)
						string+=sprintf(string,"%x ", u8(i));
					string+=sprintf(string,"| SIZE=%x\n", msg->size);
				}
				else
					string+=sprintf(string,"|\n");
			break;
		}
	return string - ret;
}

void VERBOSE_CAN_MSG_print(CAN_msg_t * msg)
{
	char str[512];

	if(VERBOSE_CAN_MSG_sprint(msg, str) > 511)
	{
		debug_printf("ATTENTION, LA GENERATION DU MESSAGE CAN SUIVANT A DEBORDE DANS LA PILE, C'EST CRADE...\n");
	}

	printf(str);
}


Sint32 print_broadcast_start_infos(CAN_msg_t * msg, char * string)
{
	char * ret = string;
	if(msg->size == 0 || global.env.match_started == TRUE)
	{
		string+=sprintf(string,"\n");
		return string - ret;	//Ce n'est pas un message broadcast où l'on a ajouté des infos... OU BIEN, le match est commencé, on ne veux pas polluer le mode débug...
	}
	string+=sprintf(string,"Couleur :%s\n",   (u8(0))?"BLEU":"ROUGE");
	string+=sprintf(string,"Stratégie :%d\n",  msg->data[1]);
	string+=sprintf(string,"Evitement :%s\n", (msg->data[2])?"ACTIF":"INACTIF");
	string+=sprintf(string,"Balise :%s\n",    (msg->data[3])?"ACTIF":"INACTIF");

	string+=sprintf(string,"Selftest : ");
	if(~msg->data[5] || ~msg->data[6] || ~msg->data[7])
	{
		if(!(msg->data[5] & 0b00000001))	string+=sprintf(string,"Etat pince avant\n");
		if(!(msg->data[5] & 0b00000010))	string+=sprintf(string,"Etat pince arrière\n");
		if(!(msg->data[5] & 0b00000100))	string+=sprintf(string,"Etat ascenceur avant\n");
		if(!(msg->data[5] & 0b00001000))	string+=sprintf(string,"Etat ascenceur arrière\n");
		if(!(msg->data[5] & 0b00010000))	string+=sprintf(string,"Etat moteur gauche\n");
		if(!(msg->data[5] & 0b00100000))	string+=sprintf(string,"Etat moteur droit\n");
		if(!(msg->data[5] & 0b01000000))	string+=sprintf(string,"Etat roue codeuse gauche\n");
		if(!(msg->data[5] & 0b10000000))	string+=sprintf(string,"Etat roue codeuse droite\n");
		if(!(msg->data[6] & 0b00000001))	string+=sprintf(string,"Etat capteurs\n");
		if(!(msg->data[6] & 0b00000010))	string+=sprintf(string,"Etat biroute\n");
		if(!(msg->data[6] & 0b00000100))	string+=sprintf(string,"Etat balise ir\n");
		if(!(msg->data[6] & 0b00001000))	string+=sprintf(string,"Etat balise us\n");
		if(!(msg->data[6] & 0b00010000))	string+=sprintf(string,"Etat synchro balise us\n");
		if(!(msg->data[7] & 0b00000001))	string+=sprintf(string,"Selftest stratégie à échoué\n");
		if(!(msg->data[7] & 0b00000010))	string+=sprintf(string,"Selftest propulsion à échoué\n");
		if(!(msg->data[7] & 0b00000100))	string+=sprintf(string,"Selftest actionneur à échoué\n");
		if(!(msg->data[7] & 0b00001000))	string+=sprintf(string,"Selftest balise à échoué\n");
	}
	else
		string+=sprintf(string,"VALIDE");
	string+=sprintf(string,"\n");
	return string - ret;
}

Sint32 print_ir_result(CAN_msg_t * msg, char * string){
	char * ret = string;
	string+=sprintf(string,"\n_____Infos erreurs balise(s) infrarouge____\n");
	string+=sprintf(string,"%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	string+=sprintf(string,"%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	string+=sprintf(string,"%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	string+=sprintf(string,"%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	string+=sprintf(string,"%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	string+=sprintf(string,"%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	string+=sprintf(string,"%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);
	return string - ret;
}

Sint32 print_us_result(CAN_msg_t * msg, char * string){
	char * ret = string;
	string+=sprintf(string,"\n_____Infos erreurs balise(s) Ulstrason____\n");
	string+=sprintf(string,"%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	string+=sprintf(string,"%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	string+=sprintf(string,"%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	string+=sprintf(string,"%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	string+=sprintf(string,"%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	string+=sprintf(string,"%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	string+=sprintf(string,"%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);
	return string - ret;
}
