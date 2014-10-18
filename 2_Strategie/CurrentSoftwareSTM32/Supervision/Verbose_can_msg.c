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

#define	u32(x1,x2,x3,x4) (U32FROMU8(msg->data[x1], msg->data[x2], msg->data[x3], msg->data[x4]))
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

Uint16 VERBOSE_CAN_MSG_sprint(CAN_msg_t * msg, char * string, int len, verbose_msg_type_e verbose_msg_type)
{
	char * ret = string;

	// Lister ici tout les sid des messages auquel vous ne voulez aucun affichage
	// Attention soyez bien sur de ce que vous entrez ici !
	switch(msg->sid){
		case DEBUG_AVOIDANCE_POLY:

			*string = '\0';
			return 0;
	}

	// Affichage du type de message CAN à afficher entrant, sortant ou du log
	if(verbose_msg_type == VERB_INPUT_MSG)
		print(string, len, "I");
	else if(verbose_msg_type == VERB_OUTPUT_MSG)
		print(string, len, "O");
	else
		print(string, len, " ");


	// Lister ici tout les sid des messages que vous voulez voir affichés dans le terminal à la place de "xxx UNKNOW : you should add SID in code !"
	//ATTENTION, ici, ce sont des espaces, pas des tabulations
	print(string, len, "   sid=");
	switch(msg->sid)
	{
		case BROADCAST_START:							print(string, len, "%.3x BROADCAST_START                        ", BROADCAST_START								);	break;
		case BROADCAST_STOP_ALL:						print(string, len, "%.3x BROADCAST_STOP_ALL                     ", BROADCAST_STOP_ALL							);	break;
		case BROADCAST_COULEUR:							print(string, len, "%.3x BROADCAST_COULEUR                      ", BROADCAST_COULEUR							);	break;
		case BROADCAST_POSITION_ROBOT:					print(string, len, "%.3x BROADCAST_POSITION_ROBOT               ", BROADCAST_POSITION_ROBOT						);	break;
		case BROADCAST_ALIM:							print(string, len, "%.3x BROADCAST_ALIM                         ", BROADCAST_ALIM								);	break;
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:	print(string, len, "%.3x BROADCAST_BEACON_ADVERSARY_POS_IR      ", BROADCAST_BEACON_ADVERSARY_POSITION_IR		);	break;
		case BROADCAST_ADVERSARIES_POSITION:			print(string, len, "%.3x BROADCAST_ADVERSARIES_POSITION         ", BROADCAST_ADVERSARIES_POSITION				);	break;

		case DEBUG_CARTE_P:								print(string, len, "%x DEBUG_CARTE_P                          ", DEBUG_CARTE_P									);	break;
		case DEBUG_FOE_POS:								print(string, len, "%x DEBUG_FOE_POS                          ", DEBUG_FOE_POS									);	break;
		case DEBUG_ELEMENT_UPDATED:						print(string, len, "%x DEBUG_ELEMENT_UPDATED                  ", DEBUG_ELEMENT_UPDATED							);	break;
		case DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT:	print(string, len, "%x DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT", DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT		);  break;
		case DEBUG_PROPULSION_SET_COEF:					print(string, len, "%x DEBUG_PROPULSION_SET_COEF              ", DEBUG_PROPULSION_SET_COEF						);	break;
		case DEBUG_PROPULSION_SET_ACCELERATION:			print(string, len, "%x DEBUG_PROPULSION_SET_ACCELERATION      ", DEBUG_PROPULSION_SET_ACCELERATION				);	break;
		case DEBUG_STRAT_STATE_CHANGED:					print(string, len, "%x DEBUG_STRAT_STATE_CHANGED              ", DEBUG_STRAT_STATE_CHANGED						);	break;

		case STRAT_BUZZER_PLAY:							print(string, len, "%x STRAT_BUZZER_PLAY                      ", STRAT_BUZZER_PLAY								);	break;
		case STRAT_ACT_SELFTEST_DONE :					print(string, len, "%x STRAT_ACT_SELFTEST_DONE                ", STRAT_ACT_SELFTEST_DONE						);	break;
		case STRAT_PROP_SELFTEST_DONE :					print(string, len, "%x STRAT_PROP_SELFTEST_DONE               ", STRAT_PROP_SELFTEST_DONE						);	break;
		case STRAT_BEACON_IR_SELFTEST_DONE:				print(string, len, "%x STRAT_BEACON_IR_SELFTEST_DONE          ", STRAT_BEACON_IR_SELFTEST_DONE					);	break;
		case STRAT_BEACON_US_SELFTEST_DONE:				print(string, len, "%x STRAT_BEACON_US_SELFTEST_DONE          ", STRAT_BEACON_US_SELFTEST_DONE					);	break;
		case STRAT_BALISE_BATTERY_STATE	:				print(string, len, "%x STRAT_BALISE_BATTERY_STATE             ", STRAT_BALISE_BATTERY_STATE						);	break;
		case STRAT_TRAJ_FINIE:							print(string, len, "%x STRAT_TRAJ_FINIE                       ", STRAT_TRAJ_FINIE								);	break;
		case STRAT_PROP_ERREUR:							print(string, len, "%x STRAT_PROP_ERREUR                      ", STRAT_PROP_ERREUR								);	break;
		case STRAT_ROBOT_FREINE:						print(string, len, "%x STRAT_ROBOT_FREINE                     ", STRAT_ROBOT_FREINE								);	break;
		case STRAT_PROP_FOE_DETECTED:					print(string, len, "%x STRAT_PROP_FOE_DETECTED                ", STRAT_PROP_FOE_DETECTED						);	break;

		case ACT_DO_SELFTEST:							print(string, len, "%x ACT_DO_SELFTEST                        ", ACT_DO_SELFTEST								);	break;
		case PROP_DO_SELFTEST:							print(string, len, "%x PROP_DO_SELFTEST                       ", PROP_DO_SELFTEST								);	break;
		case BEACON_DO_SELFTEST:						print(string, len, "%x BEACON_DO_SELFTEST                     ", BEACON_DO_SELFTEST								);	break;

		case PROP_ROBOT_CALIBRE:						print(string, len, "%x PROP_ROBOT_CALIBRE                     ", PROP_ROBOT_CALIBRE								);	break;
		case PROP_GO_ANGLE:								print(string, len, "%x PROPO_GO_ANGLE                         ", PROP_GO_ANGLE									);	break;
		case PROP_GO_POSITION:							print(string, len, "%x PROP_GO_POSITION                       ", PROP_GO_POSITION								);	break;
		case PROP_OFFSET_AVOID:							print(string, len, "%x PROP_OFFSET_AVOID                      ", PROP_OFFSET_AVOID								);	break;
		case PROP_SET_POSITION:							print(string, len, "%x PROP_SET_POSITION                      ", PROP_SET_POSITION								);	break;
		case PROP_SEND_PERIODICALLY_POSITION:			print(string, len, "%x PROP_SEND_PERIODICALLY_POSITION        ", PROP_SEND_PERIODICALLY_POSITION				);	break;
		case PROP_STOP:									print(string, len, "%x PROP_STOP                              ", PROP_STOP										);	break;
		case PROP_TELL_POSITION:						print(string, len, "%x PROP_TELL_POSITION                     ", PROP_TELL_POSITION								);	break;
		case PROP_RUSH_IN_THE_WALL:						print(string, len, "%x PROP_RUSH_IN_THE_WALL                  ", PROP_RUSH_IN_THE_WALL							);	break;
		case PROP_CALIBRATION:							print(string, len, "%x PROP_CALIBRATION                       ", PROP_CALIBRATION								);	break;
		case PROP_WARN_ANGLE:							print(string, len, "%x PROP_WARN_ANGLE                        ", PROP_WARN_ANGLE								);	break;
		case PROP_WARN_X:								print(string, len, "%x PROP_WARN_X                            ", PROP_WARN_X									);	break;
		case PROP_WARN_Y:								print(string, len, "%x PROP_WARN_Y                            ", PROP_WARN_Y									);	break;
		case DEBUG_PROP_MOVE_POSITION:					print(string, len, "%x PROP_DEBUG_MOVE_POSITION               ", DEBUG_PROP_MOVE_POSITION						);	break;

		case BEACON_ENABLE_PERIODIC_SENDING: 			print(string, len, "%x BEACON_ENABLE_PERIODIC_SENDING         ", BEACON_ENABLE_PERIODIC_SENDING					);	break;
		case BEACON_DISABLE_PERIODIC_SENDING: 			print(string, len, "%x BEACON_DISABLE_PERIODIC_SENDING        ", BEACON_DISABLE_PERIODIC_SENDING				);	break;

		case XBEE_START_MATCH:							print(string, len, "%x XBEE_START_MATCH                       ", XBEE_START_MATCH								);	break;
		case XBEE_PING:									print(string, len, "%x XBEE_PING                              ", XBEE_PING										);	break;
		case XBEE_PONG:									print(string, len, "%x XBEE_PONG                              ", XBEE_PONG										);	break;
		case XBEE_REACH_POINT_GET_OUT_INIT:				print(string, len, "%x XBEE_REACH_POINT_GET_OUT_INIT          ", XBEE_REACH_POINT_GET_OUT_INIT					);	break;

		case ACT_RESULT:								print(string, len, "%x ACT_RESULT                             ", ACT_RESULT										);	break;
		//case ACT_TORCH_LOCKER:							print(string, len, "%x ACT_TORCH_LOCKER                       ", ACT_TORCH_LOCKER								);	break;
		case ACT_TEST_SERVO:							print(string, len, "%x ACT_TEST_SERVO                         ", ACT_TEST_SERVO									);	break;

		default:										print(string, len, "%x UNKNOW : you should add SID in code !  ", msg->sid										);	break;
	}

	// Lister ici tout les sid des messages dont vous souhaitez afficher d'avantage d'information
	// L'information ajouté par défaut est un retour à la ligne
	switch(msg->sid)
	{
		case BROADCAST_ALIM:				    print(string, len, "| état : %s\n", (u8(0))?"ALIM_ON":"ALIM_OFF");		break;
		case BROADCAST_COULEUR:					print(string, len, "| CouleurEst %s\n", (u8(0))?"JAUNE":"ROUGE"	);		break;
		case BROADCAST_POSITION_ROBOT :			print(string, len, "| JeSuisEn  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:		if(u8(0) || u8(4))
												print(string, len, "ERRs:0x%x 0x%x|",u8(0), u8(4));
												print(string, len, "angleR1=%d |dR1=%dcm |angleR2=%d |dR2=%dcm \n", angle_deg(1,2), (Uint16)(u8(3)),angle_deg(5,6), (Uint16)(u8(7)));	break;

		case DEBUG_FOE_POS:						print(string, len, "|\n");												break;
		case DEBUG_PROPULSION_SET_COEF:			print(string, len, "| COEF_ID=%d  VALUE=%ld\n", u8(0),u32(1,2,3,4));	break;
		case DEBUG_PROPULSION_SET_ACCELERATION:	print(string, len, "| Acc=%d\n", u16(0,1));								break;

		case IR_ERROR_RESULT:					print_ir_result(msg, &string, &len);									break;

		case STRAT_TRAJ_FINIE:					print(string, len, "| J'arrive  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case STRAT_PROP_ERREUR:					print(string, len, "| J'erreur  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case STRAT_ROBOT_FREINE:				print(string, len, "| J'freine  x=%d y=%d t=0x%x=%d° Vt=%dmm/s Vr=%drd/s reas=0x%x st=0x%x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;

		case PROP_GO_ANGLE:						print(string, len, "| VaAngle   teta=%d=%d° %s %s %s %d%s %s\n",  angle_rad(1, 2),  angle_deg(1, 2), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);						break;
		case PROP_OFFSET_AVOID:					print(string, len, "| Xleft : %d  Xright : %d  Yfront : %d  Yback : %d\n",u16(0,1), u16(2,3), u16(4,5), u16(6,7));		break;
		case PROP_GO_POSITION:					print(string, len, "| VaPos     x=%d y=%d %s %s %s vitesse %d%s %s\n", u16(1,2), u16(3,4), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"marche avant":((u8(6)&0x10)?"marche arrière":"")	);	break;
		case PROP_SET_POSITION:					print(string, len, "| PrendPos  X=%d | Y=%d | teta=0x%x=%d°\n", u16(0,1), u16(2,3),angle_rad(4, 5),  angle_deg(4, 5));													break;
		case PROP_SEND_PERIODICALLY_POSITION: 	print(string, len, "| DitPos    période=%d | translation = %dmm | rotation = %d°\n", u16(0,1),u16(2, 3),  angle_deg(4, 5));										break;
		case PROP_RUSH_IN_THE_WALL:				print(string, len, "| sens %s | asservissement rotation %sactivée\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"), (u8(1))?"":"dés");				break;
		case PROP_CALIBRATION:					print(string, len, "| Calibre   sens %s\n", (u8(0)==1)?"arrière":((u8(0)==2)?"avant":"quelconque"));																	break;
		case PROP_WARN_ANGLE:					if(u8(0)) print(string, len, "| avertisseur en teta=0x%d=%d°\n", angle_rad(0, 1), angle_deg(0, 1)); else print(string, len, "désactivation de l'avertisseur en angle\n");	break;
		case PROP_WARN_X:						if(u8(0)) print(string, len, "| avertisseur en x=%d\n", u16(0,1));                                  else print(string, len, "désactivation de l'avertisseur en X\n");		break;
		case PROP_WARN_Y:						if(u8(0)) print(string, len, "| avertisseur en y=%d\n", u16(0,1));                                  else print(string, len, "désactivation de l'avertisseur en Y\n");		break;
		case DEBUG_PROP_MOVE_POSITION:			print(string, len, "| offset d'aleration x : %d    y : %d    teta : %d\n", u16(0,1), u16(2,3), u16(4,5));	break;

		default:								print(string, len, "|\n");												break;
	}
	return string - ret;
}

void VERBOSE_CAN_MSG_print(CAN_msg_t * msg, verbose_msg_type_e verbose_msg_type)
{
	char str[512];

	if(VERBOSE_CAN_MSG_sprint(msg, str, 512, verbose_msg_type) > 511)
	{
		debug_printf("ATTENTION, LA GENERATION DU MESSAGE CAN SUIVANT A DEBORDE DANS LA PILE, C'EST CRADE...\n");
	}

	printf(str);
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
