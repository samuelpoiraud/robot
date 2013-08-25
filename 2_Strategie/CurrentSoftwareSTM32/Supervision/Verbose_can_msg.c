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


void print_broadcast_start_infos(CAN_msg_t * msg);

#define	u16(x,y)		(U16FROMU8(msg->data[x], msg->data[y]))
#define s16(x,y)		((Sint16)(U16FROMU8(msg->data[x], msg->data[y])))
#define	u8(x)			(msg->data[x])
#define	s8(x)			((Sint8)(msg->data[x]))
#define angle_deg(x, y) ((Sint16)((((Sint32)((Sint16)(U16FROMU8(msg->data[x], msg->data[y]))))*180/PI4096)))
#define angle_rad(x, y) s16(x,y)
#define test(condition,string)	if(condition)	debug_printf(string)


void VERBOSE_CAN_MSG_print(CAN_msg_t * msg)
{
	Uint8 i;
	debug_printf("sid=");
	switch(msg->sid)
	{
		case BROADCAST_START:					debug_printf("%.3x BROADCAST_START                  ", BROADCAST_START					);	break;
		case BROADCAST_STOP_ALL:				debug_printf("%.3x BROADCAST_STOP_ALL               ", BROADCAST_STOP_ALL				);	break;
		case BROADCAST_COULEUR:					debug_printf("%.3x BROADCAST_COULEUR                ", BROADCAST_COULEUR				);	break;
		case BROADCAST_POSITION_ROBOT:			debug_printf("%.3x BROADCAST_POSITION_ROBOT         ", BROADCAST_POSITION_ROBOT			);	break;
		case DEBUG_CARTE_P:						debug_printf("%x DEBUG_CARTE_P                    	", DEBUG_CARTE_P					);	break;
		case DEBUG_FOE_POS:						debug_printf("%x DEBUG_FOE_POS                    	", DEBUG_FOE_POS					);	break;
		case DEBUG_ELEMENT_UPDATED:				debug_printf("%x DEBUG_ELEMENT_UPDATED            	", DEBUG_ELEMENT_UPDATED			);	break;
		case DEBUG_ASSER_POINT_FICTIF:			debug_printf("%x DEBUG_ASSER_POINT_FICTIF         	", DEBUG_ASSER_POINT_FICTIF			);	break;
		case SUPER_EEPROM_RESET:				debug_printf("%x SUPER_EEPROM_RESET               	", SUPER_EEPROM_RESET				);	break;
		case SUPER_EEPROM_PRINT_MATCH:			debug_printf("%x SUPER_EEPROM_PRINT_MATCH         	", SUPER_EEPROM_PRINT_MATCH			);	break;
		case ACT_DO_SELFTEST:			debug_printf("%x ACT_DO_SELFTEST           	", ACT_DO_SELFTEST			);	break;
		case PROP_DO_SELFTEST:			debug_printf("%x PROP_DO_SELFTEST         	", PROP_DO_SELFTEST			);	break;
		case BEACON_DO_SELFTEST:			debug_printf("%x BEACON_DO_SELFTEST        	", BEACON_DO_SELFTEST		);	break;
		case STRAT_ACT_SELFTEST_DONE :						debug_printf("%x STRAT_ACT_SELFTEST_DONE                     	", STRAT_ACT_SELFTEST_DONE 					);	break;
		case STRAT_PROP_SELFTEST_DONE :					debug_printf("%x STRAT_PROP_SELFTEST_DONE                   	", STRAT_PROP_SELFTEST_DONE 					);	break;
		case STRAT_BEACON_IR_SELFTEST_DONE:				debug_printf("%x STRAT_BEACON_IR_SELFTEST_DONE               	", STRAT_BEACON_IR_SELFTEST_DONE				);	break;
		case STRAT_BEACON_US_SELFTEST_DONE:				debug_printf("%x STRAT_BEACON_US_SELFTEST_DONE               	", STRAT_BEACON_US_SELFTEST_DONE				);	break;
		case CARTE_P_TRAJ_FINIE:				debug_printf("%x CARTE_P_TRAJ_FINIE               	", CARTE_P_TRAJ_FINIE				);	break;
		case CARTE_P_ASSER_ERREUR:				debug_printf("%x CARTE_P_ASSER_ERREUR             	", CARTE_P_ASSER_ERREUR				);	break;
		case CARTE_P_ROBOT_FREINE:				debug_printf("%x CARTE_P_ROBOT_FREINE             	",	CARTE_P_ROBOT_FREINE			);	break;
		case CARTE_P_ROBOT_CALIBRE:				debug_printf("%x CARTE_P_ROBOT_CALIBRE            	", CARTE_P_ROBOT_CALIBRE			);	break;
		case ASSER_GO_ANGLE:					debug_printf("%x ASSER_GO_ANGLE                   	", ASSER_GO_ANGLE 					);	break;
		case ASSER_GO_POSITION:					debug_printf("%x ASSER_GO_POSITION                	", ASSER_GO_POSITION 				);	break;	
		case ASSER_SET_POSITION:				debug_printf("%x ASSER_SET_POSITION               	", ASSER_SET_POSITION				);	break;
		case ASSER_SEND_PERIODICALLY_POSITION: 	debug_printf("%x ASSER_SEND_PERIODICALLY_POSITION 	", ASSER_SEND_PERIODICALLY_POSITION	);	break;
		case ASSER_STOP:						debug_printf("%x ASSER_STOP                       	", ASSER_STOP						);	break;
		case ASSER_TELL_POSITION:				debug_printf("%x ASSER_TELL_POSITION              	", ASSER_TELL_POSITION				);	break;
		case ASSER_TYPE_ASSERVISSEMENT:			debug_printf("%x ASSER_TYPE_ASSERVISSEMENT        	", ASSER_TYPE_ASSERVISSEMENT		);	break;
		case ASSER_RUSH_IN_THE_WALL:			debug_printf("%x ASSER_RUSH_IN_THE_WALL           	", ASSER_RUSH_IN_THE_WALL			);	break;
		case ASSER_CALIBRATION:					debug_printf("%x ASSER_CALIBRATION                	", ASSER_CALIBRATION				);	break;
		case ASSER_WARN_ANGLE:					debug_printf("%x ASSER_WARN_ANGLE                 	", ASSER_WARN_ANGLE					);	break;
		case ASSER_WARN_X:						debug_printf("%x ASSER_WARN_X                     	", ASSER_WARN_X						);	break;
		case ASSER_WARN_Y:						debug_printf("%x ASSER_WARN_Y                     	", ASSER_WARN_Y						);	break;
		case CARTE_ASSER_FIN_ERREUR:			debug_printf("%x CARTE_ASSER_FIN_ERREUR           	", CARTE_ASSER_FIN_ERREUR			);	break;
/*		case ACT_DCM_POS:						debug_printf("%x ACT_DCM_POS                      	", ACT_DCM_POS						);	break;
		case ACT_CLAMP_PREPARED:				debug_printf("%x ACT_CLAMP_PREPARED               	", ACT_CLAMP_PREPARED				);	break;
		case ACT_PAWN_GOT:						debug_printf("%x ACT_PAWN_GOT                     	", ACT_PAWN_GOT						);	break;
		case ACT_PAWN_FILED:					debug_printf("%x ACT_PAWN_FILED                   	", ACT_PAWN_FILED 					);	break;
		case ACT_PAWN_DETECTED:					debug_printf("%x ACT_PAWN_DETECTED              	", ACT_PAWN_DETECTED 				);	break;
		case ACT_PAWN_NO_LONGER_DETECTED :		debug_printf("%x ACT_PAWN_NO_LONGER_DETECTED    	", ACT_PAWN_NO_LONGER_DETECTED 		);	break;
		case ACT_EMPTY:							debug_printf("%x ACT_EMPTY                        	", ACT_EMPTY						);	break;
		case ACT_FULL:							debug_printf("%x ACT_FULL                         	", ACT_FULL							);	break;
		case ACT_FAILURE:						debug_printf("%x ACT_FAILURE                      	", ACT_FAILURE						);	break;
		case ACT_READY:							debug_printf("%x ACT_READY                        	", ACT_READY						);	break;
		case ACT_DCM_SETPOS:					debug_printf("%x ACT_DCM_SETPOS                   	", ACT_DCM_SETPOS					);	break;
		case ACT_PREPARE_CLAMP:					debug_printf("%x ACT_PREPARE_CLAMP                	", ACT_PREPARE_CLAMP				);	break;
		case ACT_TAKE_PAWN:						debug_printf("%x ACT_TAKE_PAWN                    	", ACT_TAKE_PAWN					);	break;
		case ACT_FILE_PAWN:						debug_printf("%x ACT_FILE_PAWN                    	", ACT_FILE_PAWN					);	break;
*/		case BEACON_ENABLE_PERIODIC_SENDING: 	debug_printf("%x BEACON_ENABLE_PERIODIC_SENDING   	", BEACON_ENABLE_PERIODIC_SENDING	);	break;
		case BEACON_DISABLE_PERIODIC_SENDING: 	debug_printf("%x BEACON_DISABLE_PERIODIC_SENDING  	", BEACON_DISABLE_PERIODIC_SENDING	);	break;
		case BEACON_ADVERSARY_POSITION_IR:		debug_printf("%x BEACON_ADVERSARY_POSITION_IR     	", BEACON_ADVERSARY_POSITION_IR		);	break;
		case BEACON_ADVERSARY_POSITION_US:		debug_printf("%x BEACON_ADVERSARY_POSITION_US     	", BEACON_ADVERSARY_POSITION_US		);	break;
		case DEBUG_STRAT_STATE_CHANGED:			debug_printf("%x DEBUG_STRAT_STATE_CHANGED          ", DEBUG_STRAT_STATE_CHANGED		);	break;
				
		default:
												debug_printf("SID=%x                            | ", msg->sid);								break;
	}
	
	
// Les lignes commentées à gauche sont celles qui ne sont pas traitées.	
	switch(msg->sid)
	{
		case BROADCAST_START:					print_broadcast_start_infos(msg);									break;
	//	case BROADCAST_STOP_ALL:				debug_printf("|\n");												break;
		case IR_ERROR_RESULT:					print_ir_result(msg);												break;
		case US_ERROR_RESULT:					print_us_result(msg);												break;
		case BROADCAST_COULEUR:					debug_printf("| CouleurEst %s\n", (u8(0))?"BLEU":"ROUGE"	);		break;
		case BROADCAST_POSITION_ROBOT :			debug_printf("| JeSuisEn  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
	//	case DEBUG_CARTE_P:						debug_printf("|\n");												break;
//		case DEBUG_FOE_POS:						debug_printf("|\n");												break;
//		case DEBUG_ELEMENT_UPDATED:				debug_printf("|\n");												break;
//		case DEBUG_ASSER_POINT_FICTIF:			debug_printf("|\n");												break;
	//	case SUPER_EEPROM_RESET:				debug_printf("|\n");												break;
		case SUPER_EEPROM_PRINT_MATCH:			debug_printf("| match_address_x8 : %x\n", u16(0,1));				break;
	//	case SUPER_ASK_STRAT_SELFTEST:			debug_printf("|\n");												break;
	//	case SUPER_ASK_ACT_SELFTEST:			debug_printf("|\n");												break;
	//	case SUPER_ASK_ASSER_SELFTEST:			debug_printf("|\n");												break;
	//	case SUPER_ASK_BEACON_SELFTEST:			debug_printf("|\n");												break;
//		case SUPER_CONFIG_IS:					debug_printf("|\n");												break;
//		case STRAT_SELFTEST:					debug_printf("|\n");												break;
//		case ACT_SELFTEST :						debug_printf("|\n");												break;
//		case ASSER_SELFTEST :					debug_printf("|\n");												break;
//		case BEACON_IR_SELFTEST:				debug_printf("|\n");												break;
//		case BEACON_US_SELFTEST:				debug_printf("|\n");												break;
		case CARTE_P_TRAJ_FINIE:				debug_printf("| J'arrive  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case CARTE_P_ASSER_ERREUR:				debug_printf("| J'erreur  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case CARTE_P_ROBOT_FREINE:				debug_printf("| J'freine  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
	//	case CARTE_P_ROBOT_CALIBRE:				debug_printf("|\n");												break;
		case ASSER_GO_ANGLE:					debug_printf("| VaAngle   teta=%d=%d° %s %s %s %d%s %s\n",  angle_rad(1, 2),  angle_deg(1, 2), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);						break;
		case ASSER_GO_POSITION:					debug_printf("| VaPos     x=%d y=%d %s %s %s vitesse %d%s %s\n", u16(1,2), u16(3,4), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);	break;
		case ASSER_SET_POSITION:				debug_printf("| PrendPos  X=%d | Y=%d | teta=0x%x=%d°\n", u16(0,1), u16(2,3),angle_rad(4, 5),  angle_deg(4, 5));													break;
		case ASSER_SEND_PERIODICALLY_POSITION: 	debug_printf("| DitPos    période=%d | translation = %dmm | rotation = %d°\n", u16(0,1),u16(2, 3),  angle_deg(4, 5));										break;
	//	case ASSER_STOP:						debug_printf("|\n");												break;
	//	case ASSER_TELL_POSITION:				debug_printf("|\n");												break;
		case ASSER_TYPE_ASSERVISSEMENT:			debug_printf("| asservissement rotation %sactivée | asservissement translation %sactivée\n", (u8(0))?"":"dés", (u8(1))?"":"dés");						break;
		case ASSER_RUSH_IN_THE_WALL:			debug_printf("| sens %s | asservissement rotation %sactivée\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"), (u8(1))?"":"dés");				break;
		case ASSER_CALIBRATION:					debug_printf("| Calibre   sens %s\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"));																	break;
		case ASSER_WARN_ANGLE:					(u8(0))?debug_printf("| avertisseur en teta=0x%d=%d°\n",angle_rad(0, 1), angle_deg(0, 1)):debug_printf("désactivation de l'avertisseur en angle\n");	break;
		case ASSER_WARN_X:						(u8(0))?debug_printf("| avertisseur en x=%d\n",u16(0,1)):debug_printf("désactivation de l'avertisseur en X\n");											break;
		case ASSER_WARN_Y:						(u8(0))?debug_printf("| avertisseur en y=%d\n",u16(0,1)):debug_printf("désactivation de l'avertisseur en Y\n");											break;
	//	case CARTE_ASSER_FIN_ERREUR:			debug_printf("|\n");												break;
/*		case ACT_DCM_POS:
		case ACT_FAILURE:
		case ACT_READY:
		case ACT_EMPTY:							if(msg->sid == ACT_EMPTY)			debug_printf("| Pince vide ");			//PAS DE BREAK !
		case ACT_FULL:							if(msg->sid == ACT_FULL)			debug_printf("| Pince pleine ");		//PAS DE BREAK !
												switch(u8(0)) 
												{ 
													case ACT_CLAMP_FRONT: 	debug_printf("|Pince avant\n");			break;
													case ACT_CLAMP_BACK: 	debug_printf("|Pince arrière\n");		break;
													case ACT_LIFT_FRONT: 	debug_printf("|Ascenseur avant\n");		break;
													case ACT_LIFT_BACK : 	debug_printf("|Ascenseur arrière\n");	break;
													default: 				debug_printf("|Argument invalide\n");	break;
												}																	break;
		case ACT_CLAMP_PREPARED:				if(msg->sid == ACT_CLAMP_PREPARED)	debug_printf("| Pince préparée");	//PAS DE BREAK !
		case ACT_PAWN_GOT:						if(msg->sid == ACT_PAWN_GOT)		debug_printf("| Prendre pion");		//PAS DE BREAK !
		case ACT_PAWN_FILED:					if(msg->sid == ACT_PAWN_FILED)		debug_printf("| Pion relaché");		//PAS DE BREAK !
		case ACT_PAWN_DETECTED:					if(msg->sid == ACT_PAWN_DETECTED)	debug_printf("| Pion magnétisé");	//PAS DE BREAK !
												switch(u8(0)) 
												{ 
													case ACT_FRONT: 		debug_printf(" avant\n");				break;
													case ACT_BACK: 			debug_printf(" arrière\n");				break;
													default: 				debug_printf(" Argument invalide\n");	break;
												}																	break;
																													break;
		case ACT_DCM_SETPOS:										
												switch(u8(1)) 
												{ 
													case ACT_CLAMP_CLOSED_ON_PAWN: 	debug_printf("|fermer sur le pion la pince ");			break;
													case ACT_CLAMP_OPENED: 			debug_printf("|ouvrir la pince ");						break;
													case ACT_CLAMP_CLOSED: 			debug_printf("|fermer la pince ");						break;
													case ACT_LIFT_BOTTOM : 			debug_printf("|descendre l'ascenceur ");				break;
													case ACT_LIFT_MIDDLE:			debug_printf("|placer au milieu l'ascenseur ");			break;
													case ACT_LIFT_TOP:				debug_printf("|monter l'ascenseur ");					break;
													case ACT_LIFT_MOVE_TOWER:		debug_printf("|déplacer une tour avec l'ascenceur ");	break;	
													default: 						debug_printf("|Argument invalide ");					break;
												}
												switch(u8(0)) 
												{ 
													case ACT_CLAMP_FRONT: 	debug_printf("avant\n");				break;
													case ACT_CLAMP_BACK: 	debug_printf("arrière\n");				break;
													case ACT_LIFT_FRONT: 	debug_printf("avant\n");				break;
													case ACT_LIFT_BACK : 	debug_printf("arrière\n");				break;
													default: 				debug_printf("|Argument invalide\n");	break;
												}																	break;
		case ACT_PREPARE_CLAMP:					debug_printf("| Préparer la pince et ");
												switch(u8(1)) 
												{ 
													case ACT_LIFT_BOTTOM : 			debug_printf("descendre l'ascenceur ");					break;
													case ACT_LIFT_MIDDLE:			debug_printf("placer au milieu l'ascenseur ");			break;
													case ACT_LIFT_TOP:				debug_printf("monter l'ascenseur ");					break;
													case ACT_LIFT_MOVE_TOWER:		debug_printf("déplacer une tour avec l'ascenceur ");	break;	
													default: 						debug_printf("Argument invalide ");						break;
												}
												switch(u8(0)) 
												{ 
													case ACT_FRONT: 				debug_printf("avant\n");								break;
													case ACT_BACK: 					debug_printf("arrière\n");								break;
													default: 						debug_printf("|Argument invalide\n");					break;
												}																	break;
		case ACT_TAKE_PAWN:						debug_printf("| Ramasser");		//Pas de break.. même suite que ACT_FILE_PAWN
		case ACT_FILE_PAWN:						
												debug_printf("%s un pion à l'", (msg->sid==ACT_FILE_PAWN)?"| Lâcher":"");
												switch(u8(0)) 
												{ 
													case ACT_FRONT: 				debug_printf("avant");									break;
													case ACT_BACK: 					debug_printf("arrière");								break;
													default: 						debug_printf("Argument invalide");						break;
												}
												debug_printf(" et le stocker ");
												switch(u8(1)) 
												{ 
													case ACT_LIFT_BOTTOM : 			debug_printf("en bas");									break;
													case ACT_LIFT_MIDDLE:			debug_printf("au milieu");								break;
													case ACT_LIFT_TOP:				debug_printf("en haut");								break;
													case ACT_LIFT_MOVE_TOWER:		debug_printf("en position de déplacement de tour");		break;	
													default: 						debug_printf("Argument invalide ");						break;
												}
												switch(u8(2)) 
												{ 
													case ACK_LIFT_ACTION: 			debug_printf(". Ack ascenseur\n");				break;
													case ACK_CLAMP_ACTION: 			debug_printf(". Ack pince\n");					break;
													default: 						debug_printf("|Argument invalide\n");					break;
												}																	break;
*/		//case BEACON_ENABLE_PERIODIC_SENDING: 	debug_printf("|\n");												break;
		//case BEACON_DISABLE_PERIODIC_SENDING: 	debug_printf("|\n");												break;
		case BEACON_ADVERSARY_POSITION_IR:		if(u8(0) || u8(4))
													debug_printf("ERRs:0x%x 0x%x|",u8(0), u8(4));
												debug_printf("angleR1=%d |dR1=%dcm |angleR2=%d |dR2=%dcm \n", angle_deg(1,2), (Uint16)(u8(3)),angle_deg(5,6), (Uint16)(u8(7)));	break;
		case BEACON_ADVERSARY_POSITION_US:		if(u8(0))	
													debug_printf("ERRs:0x%x 0x%x|",u8(0), u8(4));
												debug_printf("dR1=%dmm|dR2=%dmm|id=%d\n", u16(1,2), u16(5,6), u8(3));								break;
		case DEBUG_STRAT_STATE_CHANGED:			debug_printf("| Strat state changed 0x%04X: %d -> %d", u16(0,1), u8(2), u8(3));
												if(msg->size > 4) {
													debug_printf(" | params:");
													for(i = 4; i < msg->size; i++)
														debug_printf(" 0x%02X(%d)", u8(i), u8(i));

												}
												debug_printf("\n");
		break;	/* DEBUG_STRAT_STATE_CHANGED */

		default:
			if(msg->size)
			{
				debug_printf("| DATA=");
				for(i = 0; i<msg->size && i<8; i++)
					debug_printf("%x ", u8(i));
				debug_printf("| SIZE=%x\n", msg->size);
			}
			else
				debug_printf("|\n");
		break;
	}			
}


void print_broadcast_start_infos(CAN_msg_t * msg)
{
	if(msg->size == 0 || global.env.match_started == TRUE)
	{
		debug_printf("\n");
		return;	//Ce n'est pas un message broadcast où l'on a ajouté des infos... OU BIEN, le match est commencé, on ne veux pas polluer le mode débug...
	}	
	debug_printf("Couleur :%s\n",   (u8(0))?"BLEU":"ROUGE");
	debug_printf("Stratégie :%d\n",  msg->data[1]);
	debug_printf("Evitement :%s\n", (msg->data[2])?"ACTIF":"INACTIF");
	debug_printf("Balise :%s\n",    (msg->data[3])?"ACTIF":"INACTIF");
	
	debug_printf("Selftest : ");
	if(~msg->data[5] || ~msg->data[6] || ~msg->data[7])
	{
		if(!(msg->data[5] & 0b00000001))	debug_printf("Etat pince avant\n");
		if(!(msg->data[5] & 0b00000010))	debug_printf("Etat pince arrière\n");
		if(!(msg->data[5] & 0b00000100))	debug_printf("Etat ascenceur avant\n");
		if(!(msg->data[5] & 0b00001000))	debug_printf("Etat ascenceur arrière\n");
		if(!(msg->data[5] & 0b00010000))	debug_printf("Etat moteur gauche\n");
		if(!(msg->data[5] & 0b00100000))	debug_printf("Etat moteur droit\n");
		if(!(msg->data[5] & 0b01000000))	debug_printf("Etat roue codeuse gauche\n");
		if(!(msg->data[5] & 0b10000000))	debug_printf("Etat roue codeuse droite\n");
		if(!(msg->data[6] & 0b00000001))	debug_printf("Etat capteurs\n");
		if(!(msg->data[6] & 0b00000010))	debug_printf("Etat biroute\n");
		if(!(msg->data[6] & 0b00000100))	debug_printf("Etat balise ir\n");
		if(!(msg->data[6] & 0b00001000))	debug_printf("Etat balise us\n");
		if(!(msg->data[6] & 0b00010000))	debug_printf("Etat synchro balise us\n");
		if(!(msg->data[7] & 0b00000001))	debug_printf("Selftest stratégie à échoué\n");
		if(!(msg->data[7] & 0b00000010))	debug_printf("Selftest propulsion à échoué\n");
		if(!(msg->data[7] & 0b00000100))	debug_printf("Selftest actionneur à échoué\n");
		if(!(msg->data[7] & 0b00001000))	debug_printf("Selftest balise à échoué\n");		
	}	
	else
		debug_printf("VALIDE");
	debug_printf("\n");
	
}	

void print_ir_result(CAN_msg_t * msg){
	debug_printf("\n_____Infos erreurs balise(s) infrarouge____\n");
	debug_printf("%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	debug_printf("%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	debug_printf("%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	debug_printf("%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	debug_printf("%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	debug_printf("%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	debug_printf("%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);

}

void print_us_result(CAN_msg_t * msg){
	debug_printf("\n_____Infos erreurs balise(s) Ulstrason____\n");
	debug_printf("%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	debug_printf("%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	debug_printf("%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	debug_printf("%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	debug_printf("%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	debug_printf("%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	debug_printf("%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);

}
