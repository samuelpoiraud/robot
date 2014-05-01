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
void print_ir_result(CAN_msg_t * msg, char ** string, int * len);
void print_us_result(CAN_msg_t * msg, char ** string, int * len);
void print_broadcast_start_infos(CAN_msg_t * msg, char ** string, int * len);

#define	u16(x,y)		(U16FROMU8(msg->data[x], msg->data[y]))
#define s16(x,y)		((Sint16)(U16FROMU8(msg->data[x], msg->data[y])))
#define	u8(x)			(msg->data[x])
#define	s8(x)			((Sint8)(msg->data[x]))
#define angle_deg(x, y) ((Sint16)((((Sint32)((Sint16)(U16FROMU8(msg->data[x], msg->data[y]))))*180/PI4096)))
#define angle_rad(x, y) s16(x,y)
//#define test(condition, string, len)	if(condition)	print(string, len, string)

#define print(buffer, len, ...) \
	do { \
		int addlen = snprintf(buffer, len, __VA_ARGS__); \
		if(addlen > 0) { \
			buffer += addlen; len -= addlen; \
		} \
	} while(0)

Uint16 VERBOSE_CAN_MSG_sprint(CAN_msg_t * msg, char * string, int len)
{
	Uint8 i;
	char * ret = string;
	print(string, len, "sid=");
	switch(msg->sid)
	{
		case BROADCAST_START:					print(string, len, "%.3x BROADCAST_START                  ", BROADCAST_START					);	break;
		case BROADCAST_STOP_ALL:				print(string, len, "%.3x BROADCAST_STOP_ALL               ", BROADCAST_STOP_ALL					);	break;
		case BROADCAST_COULEUR:					print(string, len, "%.3x BROADCAST_COULEUR                ", BROADCAST_COULEUR					);	break;
		case BROADCAST_POSITION_ROBOT:			print(string, len, "%.3x BROADCAST_POSITION_ROBOT         ", BROADCAST_POSITION_ROBOT			);	break;
		case BROADCAST_ALIM:					print(string, len, "%.3x BROADCAST_ALIM				      ", BROADCAST_ALIM						);	break;
		case DEBUG_CARTE_P:						print(string, len, "%x DEBUG_CARTE_P                    	", DEBUG_CARTE_P					);	break;
		case DEBUG_FOE_POS:						print(string, len, "%x DEBUG_FOE_POS                    	", DEBUG_FOE_POS					);	break;
		case DEBUG_ELEMENT_UPDATED:				print(string, len, "%x DEBUG_ELEMENT_UPDATED            	", DEBUG_ELEMENT_UPDATED			);	break;
		case DEBUG_ASSER_POINT_FICTIF:			print(string, len, "%x DEBUG_ASSER_POINT_FICTIF         	", DEBUG_ASSER_POINT_FICTIF			);	break;
		case SUPER_EEPROM_RESET:				print(string, len, "%x SUPER_EEPROM_RESET               	", SUPER_EEPROM_RESET				);	break;
		case SUPER_EEPROM_PRINT_MATCH:			print(string, len, "%x SUPER_EEPROM_PRINT_MATCH         	", SUPER_EEPROM_PRINT_MATCH			);	break;
		case ACT_DO_SELFTEST:					print(string, len, "%x ACT_DO_SELFTEST           	", ACT_DO_SELFTEST			);	break;
		case PROP_DO_SELFTEST:					print(string, len, "%x PROP_DO_SELFTEST         	", PROP_DO_SELFTEST			);	break;
		case BEACON_DO_SELFTEST:				print(string, len, "%x BEACON_DO_SELFTEST        	", BEACON_DO_SELFTEST		);	break;
		case STRAT_ACT_SELFTEST_DONE :			print(string, len, "%x STRAT_ACT_SELFTEST_DONE                     	", STRAT_ACT_SELFTEST_DONE 					);	break;
		case STRAT_PROP_SELFTEST_DONE :			print(string, len, "%x STRAT_PROP_SELFTEST_DONE                   	", STRAT_PROP_SELFTEST_DONE 					);	break;
		case STRAT_BEACON_IR_SELFTEST_DONE:		print(string, len, "%x STRAT_BEACON_IR_SELFTEST_DONE               	", STRAT_BEACON_IR_SELFTEST_DONE				);	break;
		case STRAT_BEACON_US_SELFTEST_DONE:		print(string, len, "%x STRAT_BEACON_US_SELFTEST_DONE               	", STRAT_BEACON_US_SELFTEST_DONE				);	break;
		case STRAT_TRIANGLE_POSITON:			print(string, len, "%x STRAT_TRIANGLE_POSITON              ", STRAT_TRIANGLE_POSITON			);	break;
		case STRAT_TRIANGLE_WARNER:				print(string, len, "%x STRAT_TRIANGLE_WARNER               ", STRAT_TRIANGLE_WARNER				);	break;
		case STRAT_SCAN_ANYTHING:				print(string, len, "%x STRAT_SCAN_ANYTHING                 ", STRAT_SCAN_ANYTHING				);	break;
		case STRAT_ADVERSARIES_POSITION:		print(string, len, "%x STRAT_ADVERSARIES_POSITION          ", STRAT_ADVERSARIES_POSITION		);	break;
		case STRAT_INFORM_FILET:				print(string, len, "%x STRAT_INFORM_FILET				   ", STRAT_INFORM_FILET				);	break;
		case STRAT_INFORM_FRUIT_MOUTH:			print(string, len, "%x STRAT_INFORM_FRUIT_MOUTH			   ", STRAT_INFORM_FRUIT_MOUTH			);	break;
		case STRAT_ANSWER_POMPE:				print(string, len, "%x STRAT_ANSWER_POMPE				   ", STRAT_ANSWER_POMPE				);	break;
		case CARTE_P_TRAJ_FINIE:				print(string, len, "%x CARTE_P_TRAJ_FINIE               	", CARTE_P_TRAJ_FINIE				);	break;
		case CARTE_P_ASSER_ERREUR:				print(string, len, "%x CARTE_P_ASSER_ERREUR             	", CARTE_P_ASSER_ERREUR				);	break;
		case CARTE_P_ROBOT_FREINE:				print(string, len, "%x CARTE_P_ROBOT_FREINE             	",	CARTE_P_ROBOT_FREINE			);	break;
		case CARTE_P_ROBOT_CALIBRE:				print(string, len, "%x CARTE_P_ROBOT_CALIBRE            	", CARTE_P_ROBOT_CALIBRE			);	break;
		case ASSER_GO_ANGLE:					print(string, len, "%x ASSER_GO_ANGLE                   	", ASSER_GO_ANGLE 					);	break;
		case ASSER_GO_POSITION:					print(string, len, "%x ASSER_GO_POSITION                	", ASSER_GO_POSITION 				);	break;
		case ASSER_SET_POSITION:				print(string, len, "%x ASSER_SET_POSITION               	", ASSER_SET_POSITION				);	break;
		case ASSER_SEND_PERIODICALLY_POSITION: 	print(string, len, "%x ASSER_SEND_PERIODICALLY_POSITION 	", ASSER_SEND_PERIODICALLY_POSITION	);	break;
		case ASSER_STOP:						print(string, len, "%x ASSER_STOP                       	", ASSER_STOP						);	break;
		case ASSER_TELL_POSITION:				print(string, len, "%x ASSER_TELL_POSITION              	", ASSER_TELL_POSITION				);	break;
		case ASSER_TYPE_ASSERVISSEMENT:			print(string, len, "%x ASSER_TYPE_ASSERVISSEMENT        	", ASSER_TYPE_ASSERVISSEMENT		);	break;
		case ASSER_RUSH_IN_THE_WALL:			print(string, len, "%x ASSER_RUSH_IN_THE_WALL           	", ASSER_RUSH_IN_THE_WALL			);	break;
		case ASSER_CALIBRATION:					print(string, len, "%x ASSER_CALIBRATION                	", ASSER_CALIBRATION				);	break;
		case ASSER_WARN_ANGLE:					print(string, len, "%x ASSER_WARN_ANGLE                 	", ASSER_WARN_ANGLE					);	break;
		case ASSER_WARN_X:						print(string, len, "%x ASSER_WARN_X                     	", ASSER_WARN_X						);	break;
		case ASSER_WARN_Y:						print(string, len, "%x ASSER_WARN_Y                     	", ASSER_WARN_Y						);	break;
		case CARTE_ASSER_FIN_ERREUR:			print(string, len, "%x CARTE_ASSER_FIN_ERREUR           	", CARTE_ASSER_FIN_ERREUR			);	break;
		case ASSER_LAUNCH_SCAN_TRIANGLE:		print(string, len, "%x ASSER_LAUNCH_SCAN_TRIANGLE           ", ASSER_LAUNCH_SCAN_TRIANGLE		);	break;
		case ASSER_LAUNCH_WARNER_TRIANGLE:		print(string, len, "%x ASSER_LAUNCH_WARNER_TRIANGLE         ", ASSER_LAUNCH_WARNER_TRIANGLE		);	break;
/*		case ACT_DCM_POS:						print(string, len, "%x ACT_DCM_POS                      	", ACT_DCM_POS						);	break;
		case ACT_CLAMP_PREPARED:				print(string, len, "%x ACT_CLAMP_PREPARED               	", ACT_CLAMP_PREPARED				);	break;
		case ACT_PAWN_GOT:						print(string, len, "%x ACT_PAWN_GOT                     	", ACT_PAWN_GOT						);	break;
		case ACT_PAWN_FILED:					print(string, len, "%x ACT_PAWN_FILED                   	", ACT_PAWN_FILED 					);	break;
		case ACT_PAWN_DETECTED:					print(string, len, "%x ACT_PAWN_DETECTED              	", ACT_PAWN_DETECTED 				);	break;
		case ACT_PAWN_NO_LONGER_DETECTED :		print(string, len, "%x ACT_PAWN_NO_LONGER_DETECTED    	", ACT_PAWN_NO_LONGER_DETECTED 		);	break;
		case ACT_EMPTY:							print(string, len, "%x ACT_EMPTY                        	", ACT_EMPTY						);	break;
		case ACT_FULL:							print(string, len, "%x ACT_FULL                         	", ACT_FULL							);	break;
		case ACT_FAILURE:						print(string, len, "%x ACT_FAILURE                      	", ACT_FAILURE						);	break;
		case ACT_READY:							print(string, len, "%x ACT_READY                        	", ACT_READY						);	break;
		case ACT_DCM_SETPOS:					print(string, len, "%x ACT_DCM_SETPOS                   	", ACT_DCM_SETPOS					);	break;
		case ACT_PREPARE_CLAMP:					print(string, len, "%x ACT_PREPARE_CLAMP                	", ACT_PREPARE_CLAMP				);	break;
		case ACT_TAKE_PAWN:						print(string, len, "%x ACT_TAKE_PAWN                    	", ACT_TAKE_PAWN					);	break;
		case ACT_FILE_PAWN:						print(string, len, "%x ACT_FILE_PAWN                    	", ACT_FILE_PAWN					);	break;
*/		case BEACON_ENABLE_PERIODIC_SENDING: 	print(string, len, "%x BEACON_ENABLE_PERIODIC_SENDING   	", BEACON_ENABLE_PERIODIC_SENDING	);	break;
		case BEACON_DISABLE_PERIODIC_SENDING: 	print(string, len, "%x BEACON_DISABLE_PERIODIC_SENDING  	", BEACON_DISABLE_PERIODIC_SENDING	);	break;
		case BEACON_ADVERSARY_POSITION_IR:		print(string, len, "%x BEACON_ADVERSARY_POSITION_IR     	", BEACON_ADVERSARY_POSITION_IR		);	break;
		case BEACON_ADVERSARY_POSITION_US:		print(string, len, "%x BEACON_ADVERSARY_POSITION_US     	", BEACON_ADVERSARY_POSITION_US		);	break;
		case DEBUG_STRAT_STATE_CHANGED:			print(string, len, "%x DEBUG_STRAT_STATE_CHANGED          ", DEBUG_STRAT_STATE_CHANGED		);	break;

		default:
												print(string, len, "SID=%x                            | ", msg->sid);								break;
	}


// Les lignes commentées à gauche sont celles qui ne sont pas traitées.
	switch(msg->sid)
	{
		case BROADCAST_START:					print_broadcast_start_infos(msg, &string, &len);						break;
	//	case BROADCAST_STOP_ALL:				print(string, len, "|\n");												break;
		case BROADCAST_ALIM:				    print(string, len, "état : %s\n", (u8(0))?"ALIM_ON":"ALIM_OFF");		break;
		case IR_ERROR_RESULT:					print_ir_result(msg, &string, &len);									break;
		case US_ERROR_RESULT:					print_us_result(msg, &string, &len);									break;
		case BROADCAST_COULEUR:					print(string, len, "| CouleurEst %s\n", (u8(0))?"JAUNE":"ROUGE"	);		break;
		case BROADCAST_POSITION_ROBOT :			print(string, len, "| JeSuisEn  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
	//	case STRAT_TRIANGLE_POSITON:			print(string, len, "|\n");												break;
		case STRAT_TRIANGLE_WARNER:				print(string, len, "Triangle %d est %s\n", msg->data[0], (msg->data[1])?"Présent":"Absent");	break;
		case STRAT_SCAN_ANYTHING:				print(string, len, "%s\n",(u8(0))?"Element dans la zone":"Zone vide");	break;
	//	case STRAT_ADVERSARIES_POSITION:		print(string, len, "|\n");												break;
		case STRAT_INFORM_FILET:				print(string, len, "Filet %s\n",(u8(0))?"Absent":"Présent");			break;
		case STRAT_INFORM_FRUIT_MOUTH:			print(string, len, "Fruit mouth %s\n",(u8(0))?"Ouvert":"Fermé");		break;
		case STRAT_ANSWER_POMPE:				print(string, len, "Pompe %s\n",(u8(0))?"dans le vide":"sous pression");;	break;
	//	case DEBUG_CARTE_P:						print(string, len, "|\n");												break;
//		case DEBUG_FOE_POS:						print(string, len, "|\n");												break;
//		case DEBUG_ELEMENT_UPDATED:				print(string, len, "|\n");												break;
//		case DEBUG_ASSER_POINT_FICTIF:			print(string, len, "|\n");												break;
	//	case SUPER_EEPROM_RESET:				print(string, len, "|\n");												break;
		case SUPER_EEPROM_PRINT_MATCH:			print(string, len, "| match_address_x8 : %x\n", u16(0,1));				break;
	//	case SUPER_ASK_STRAT_SELFTEST:			print(string, len, "|\n");												break;
	//	case SUPER_ASK_ACT_SELFTEST:			print(string, len, "|\n");												break;
	//	case SUPER_ASK_ASSER_SELFTEST:			print(string, len, "|\n");												break;
	//	case SUPER_ASK_BEACON_SELFTEST:			print(string, len, "|\n");												break;
//		case SUPER_CONFIG_IS:					print(string, len, "|\n");												break;
//		case STRAT_SELFTEST:					print(string, len, "|\n");												break;
//		case ACT_SELFTEST :						print(string, len, "|\n");												break;
//		case ASSER_SELFTEST :					print(string, len, "|\n");												break;
//		case BEACON_IR_SELFTEST:				print(string, len, "|\n");												break;
//		case BEACON_US_SELFTEST:				print(string, len, "|\n");												break;
		case CARTE_P_TRAJ_FINIE:				print(string, len, "| J'arrive  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case CARTE_P_ASSER_ERREUR:				print(string, len, "| J'erreur  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case CARTE_P_ROBOT_FREINE:				print(string, len, "| J'freine  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
	//	case CARTE_P_ROBOT_CALIBRE:				print(string, len, "|\n");												break;
		case ASSER_GO_ANGLE:					print(string, len, "| VaAngle   teta=%d=%d° %s %s %s %d%s %s\n",  angle_rad(1, 2),  angle_deg(1, 2), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);						break;
		case ASSER_GO_POSITION:					print(string, len, "| VaPos     x=%d y=%d %s %s %s vitesse %d%s %s\n", u16(1,2), u16(3,4), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);	break;
		case ASSER_SET_POSITION:				print(string, len, "| PrendPos  X=%d | Y=%d | teta=0x%x=%d°\n", u16(0,1), u16(2,3),angle_rad(4, 5),  angle_deg(4, 5));													break;
	//	case ASSER_LAUNCH_WARNER_TRIANGLE:		print(string, len, "|\n");												break;
		case ASSER_SEND_PERIODICALLY_POSITION: 	print(string, len, "| DitPos    période=%d | translation = %dmm | rotation = %d°\n", u16(0,1),u16(2, 3),  angle_deg(4, 5));										break;
	//	case ASSER_STOP:						print(string, len, "|\n");												break;
	//	case ASSER_TELL_POSITION:				print(string, len, "|\n");												break;
		case ASSER_TYPE_ASSERVISSEMENT:			print(string, len, "| asservissement rotation %sactivée | asservissement translation %sactivée\n", (u8(0))?"":"dés", (u8(1))?"":"dés");						break;
		case ASSER_RUSH_IN_THE_WALL:			print(string, len, "| sens %s | asservissement rotation %sactivée\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"), (u8(1))?"":"dés");				break;
		case ASSER_CALIBRATION:					print(string, len, "| Calibre   sens %s\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"));																	break;
		case ASSER_WARN_ANGLE:					if(u8(0)) print(string, len, "| avertisseur en teta=0x%d=%d°\n", angle_rad(0, 1), angle_deg(0, 1)); else print(string, len, "désactivation de l'avertisseur en angle\n");	break;
		case ASSER_WARN_X:						if(u8(0)) print(string, len, "| avertisseur en x=%d\n", u16(0,1));                                  else print(string, len, "désactivation de l'avertisseur en X\n");		break;
		case ASSER_WARN_Y:						if(u8(0)) print(string, len, "| avertisseur en y=%d\n", u16(0,1));                                  else print(string, len, "désactivation de l'avertisseur en Y\n");		break;
	//	case CARTE_ASSER_FIN_ERREUR:			print(string, len, "|\n");												break;
	//	case ASSER_LAUNCH_SCAN_TRIANGLE:		print(string, len, "|\n");												break;
/*		case ACT_DCM_POS:
		case ACT_FAILURE:
		case ACT_READY:
		case ACT_EMPTY:							if(msg->sid == ACT_EMPTY)			print(string, len, "| Pince vide ");			//PAS DE BREAK !
		case ACT_FULL:							if(msg->sid == ACT_FULL)			print(string, len, "| Pince pleine ");		//PAS DE BREAK !
												switch(u8(0))
												{
													case ACT_CLAMP_FRONT: 	print(string, len, "|Pince avant\n");			break;
													case ACT_CLAMP_BACK: 	print(string, len, "|Pince arrière\n");		break;
													case ACT_LIFT_FRONT: 	print(string, len, "|Ascenseur avant\n");		break;
													case ACT_LIFT_BACK : 	print(string, len, "|Ascenseur arrière\n");	break;
													default: 				print(string, len, "|Argument invalide\n");	break;
												}																	break;
		case ACT_CLAMP_PREPARED:				if(msg->sid == ACT_CLAMP_PREPARED)	print(string, len, "| Pince préparée");	//PAS DE BREAK !
		case ACT_PAWN_GOT:						if(msg->sid == ACT_PAWN_GOT)		print(string, len, "| Prendre pion");		//PAS DE BREAK !
		case ACT_PAWN_FILED:					if(msg->sid == ACT_PAWN_FILED)		print(string, len, "| Pion relaché");		//PAS DE BREAK !
		case ACT_PAWN_DETECTED:					if(msg->sid == ACT_PAWN_DETECTED)	print(string, len, "| Pion magnétisé");	//PAS DE BREAK !
												switch(u8(0))
												{
													case ACT_FRONT: 		print(string, len, " avant\n");				break;
													case ACT_BACK: 			print(string, len, " arrière\n");				break;
													default: 				print(string, len, " Argument invalide\n");	break;
												}																	break;
																													break;
		case ACT_DCM_SETPOS:
												switch(u8(1))
												{
													case ACT_CLAMP_CLOSED_ON_PAWN: 	print(string, len, "|fermer sur le pion la pince ");			break;
													case ACT_CLAMP_OPENED: 			print(string, len, "|ouvrir la pince ");						break;
													case ACT_CLAMP_CLOSED: 			print(string, len, "|fermer la pince ");						break;
													case ACT_LIFT_BOTTOM : 			print(string, len, "|descendre l'ascenceur ");				break;
													case ACT_LIFT_MIDDLE:			print(string, len, "|placer au milieu l'ascenseur ");			break;
													case ACT_LIFT_TOP:				print(string, len, "|monter l'ascenseur ");					break;
													case ACT_LIFT_MOVE_TOWER:		print(string, len, "|déplacer une tour avec l'ascenceur ");	break;
													default: 						print(string, len, "|Argument invalide ");					break;
												}
												switch(u8(0))
												{
													case ACT_CLAMP_FRONT: 	print(string, len, "avant\n");				break;
													case ACT_CLAMP_BACK: 	print(string, len, "arrière\n");				break;
													case ACT_LIFT_FRONT: 	print(string, len, "avant\n");				break;
													case ACT_LIFT_BACK : 	print(string, len, "arrière\n");				break;
													default: 				print(string, len, "|Argument invalide\n");	break;
												}																	break;
		case ACT_PREPARE_CLAMP:					print(string, len, "| Préparer la pince et ");
												switch(u8(1))
												{
													case ACT_LIFT_BOTTOM : 			print(string, len, "descendre l'ascenceur ");					break;
													case ACT_LIFT_MIDDLE:			print(string, len, "placer au milieu l'ascenseur ");			break;
													case ACT_LIFT_TOP:				print(string, len, "monter l'ascenseur ");					break;
													case ACT_LIFT_MOVE_TOWER:		print(string, len, "déplacer une tour avec l'ascenceur ");	break;
													default: 						print(string, len, "Argument invalide ");						break;
												}
												switch(u8(0))
												{
													case ACT_FRONT: 				print(string, len, "avant\n");								break;
													case ACT_BACK: 					print(string, len, "arrière\n");								break;
													default: 						print(string, len, "|Argument invalide\n");					break;
												}																	break;
		case ACT_TAKE_PAWN:						print(string, len, "| Ramasser");		//Pas de break.. même suite que ACT_FILE_PAWN
		case ACT_FILE_PAWN:
												print(string, len, "%s un pion à l'", (msg->sid==ACT_FILE_PAWN)?"| Lâcher":"");
												switch(u8(0))
												{
													case ACT_FRONT: 				print(string, len, "avant");									break;
													case ACT_BACK: 					print(string, len, "arrière");								break;
													default: 						print(string, len, "Argument invalide");						break;
												}
												print(string, len, " et le stocker ");
												switch(u8(1))
												{
													case ACT_LIFT_BOTTOM : 			print(string, len, "en bas");									break;
													case ACT_LIFT_MIDDLE:			print(string, len, "au milieu");								break;
													case ACT_LIFT_TOP:				print(string, len, "en haut");								break;
													case ACT_LIFT_MOVE_TOWER:		print(string, len, "en position de déplacement de tour");		break;
													default: 						print(string, len, "Argument invalide ");						break;
												}
												switch(u8(2))
												{
													case ACK_LIFT_ACTION: 			print(string, len, ". Ack ascenseur\n");				break;
													case ACK_CLAMP_ACTION: 			print(string, len, ". Ack pince\n");					break;
													default: 						print(string, len, "|Argument invalide\n");					break;
												}																	break;
*/		//case BEACON_ENABLE_PERIODIC_SENDING: 	print(string, len, "|\n");												break;
		//case BEACON_DISABLE_PERIODIC_SENDING: 	print(string, len, "|\n");												break;
		case BEACON_ADVERSARY_POSITION_IR:		if(u8(0) || u8(4))
													print(string, len, "ERRs:0x%x 0x%x|",u8(0), u8(4));
												print(string, len, "angleR1=%d |dR1=%dcm |angleR2=%d |dR2=%dcm \n", angle_deg(1,2), (Uint16)(u8(3)),angle_deg(5,6), (Uint16)(u8(7)));	break;
		case BEACON_ADVERSARY_POSITION_US:		if(u8(0))
													print(string, len, "ERRs:0x%x 0x%x|",u8(0), u8(4));
												print(string, len, "dR1=%dmm|dR2=%dmm|id=%d\n", u16(1,2), u16(5,6), u8(3));								break;
		case DEBUG_STRAT_STATE_CHANGED:			print(string, len, "| Strat state changed 0x%04X: %d -> %d", u16(0,1), u8(2), u8(3));
												if(msg->size > 4) {
													print(string, len, " | params:");
													for(i = 4; i < msg->size; i++)
														print(string, len, " 0x%02X(%d)", u8(i), u8(i));

												}
												print(string, len, "\n");
		break;	/* DEBUG_STRAT_STATE_CHANGED */

		default:
			if(msg->size)
			{
				print(string, len, "| DATA=");
				for(i = 0; i<msg->size && i<8; i++)
					print(string, len, "%x ", u8(i));
				print(string, len, "| SIZE=%x\n", msg->size);
			}
			else
				print(string, len, "|\n");
		break;
	}
	return string - ret;
}

void VERBOSE_CAN_MSG_print(CAN_msg_t * msg)
{
	char str[512];

	if(VERBOSE_CAN_MSG_sprint(msg, str, 512) > 511)
	{
		debug_printf("ATTENTION, LA GENERATION DU MESSAGE CAN SUIVANT A DEBORDE DANS LA PILE, C'EST CRADE...\n");
	}

	printf(str);
}


void print_broadcast_start_infos(CAN_msg_t * msg, char ** string, int * len)
{
	if(msg->size == 0 || global.env.match_started == TRUE)
	{
		print(*string, *len, "\n"); //Ce n'est pas un message broadcast où l'on a ajouté des infos... OU BIEN, le match est commencé, on ne veux pas polluer le mode débug...
	}
	print(*string, *len, "Couleur :%s\n",   (u8(0))?"JAUNE":"ROUGE");
	print(*string, *len, "Stratégie :%d\n",  msg->data[1]);
	print(*string, *len, "Evitement :%s\n", (msg->data[2])?"ACTIF":"INACTIF");
	print(*string, *len, "Balise :%s\n",    (msg->data[3])?"ACTIF":"INACTIF");

	print(*string, *len, "Selftest : ");
	if(~msg->data[5] || ~msg->data[6] || ~msg->data[7])
	{
		if(!(msg->data[5] & 0b00000001))	print(*string, *len, "Etat pince avant\n");
		if(!(msg->data[5] & 0b00000010))	print(*string, *len, "Etat pince arrière\n");
		if(!(msg->data[5] & 0b00000100))	print(*string, *len, "Etat ascenceur avant\n");
		if(!(msg->data[5] & 0b00001000))	print(*string, *len, "Etat ascenceur arrière\n");
		if(!(msg->data[5] & 0b00010000))	print(*string, *len, "Etat moteur gauche\n");
		if(!(msg->data[5] & 0b00100000))	print(*string, *len, "Etat moteur droit\n");
		if(!(msg->data[5] & 0b01000000))	print(*string, *len, "Etat roue codeuse gauche\n");
		if(!(msg->data[5] & 0b10000000))	print(*string, *len, "Etat roue codeuse droite\n");
		if(!(msg->data[6] & 0b00000001))	print(*string, *len, "Etat capteurs\n");
		if(!(msg->data[6] & 0b00000010))	print(*string, *len, "Etat biroute\n");
		if(!(msg->data[6] & 0b00000100))	print(*string, *len, "Etat balise ir\n");
		if(!(msg->data[6] & 0b00001000))	print(*string, *len, "Etat balise us\n");
		if(!(msg->data[6] & 0b00010000))	print(*string, *len, "Etat synchro balise us\n");
		if(!(msg->data[7] & 0b00000001))	print(*string, *len, "Selftest stratégie à échoué\n");
		if(!(msg->data[7] & 0b00000010))	print(*string, *len, "Selftest propulsion à échoué\n");
		if(!(msg->data[7] & 0b00000100))	print(*string, *len, "Selftest actionneur à échoué\n");
		if(!(msg->data[7] & 0b00001000))	print(*string, *len, "Selftest balise à échoué\n");
	}
	else
		print(*string, *len, "VALIDE");
	print(*string, *len, "\n");
}

void print_ir_result(CAN_msg_t * msg, char ** string, int * len){
	print(*string, *len, "\n_____Infos erreurs balise(s) infrarouge____\n");
	print(*string, *len, "%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	print(*string, *len, "%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	print(*string, *len, "%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	print(*string, *len, "%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	print(*string, *len, "%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	print(*string, *len, "%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	print(*string, *len, "%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);
}

void print_us_result(CAN_msg_t * msg, char ** string, int * len){
	print(*string, *len, "\n_____Infos erreurs balise(s) Ulstrason____\n");
	print(*string, *len, "%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	print(*string, *len, "%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	print(*string, *len, "%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	print(*string, *len, "%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	print(*string, *len, "%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	print(*string, *len, "%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	print(*string, *len, "%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);
}
