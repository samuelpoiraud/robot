/*
 *	Club Robot ESEO 2014 - 2015
 *	Holly & Wood
 *
 *	Fichier : QS_can_verbose.c
 *	Package : Qualit� Soft
 *	Description : Verbosit� des messages CAN sur uart
 *	Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100111
 */

#include "QS_can_verbose.h"
#include "QS_outputlog.h"
#include "QS_IHM.h"
#include <stdio.h>

char *it_state_name[] = {
	"NONE",
	"ODOMETRY",
	"SECRETARY",
	"WARNER",
	"JOYSTICK",
	"AVOIDANCE",
	"COPILOT",
	"PILOT",
	"SUPERVISOR",
	"MAIN",
	"HOKUYO",
	"DETECTION",
	"SCAN_CUP",
	"DEBUG",
	"LCD"
};

static void print_ir_result(CAN_msg_t * msg, char ** string, int * len);
static void print_ihm_result(CAN_msg_t * msg, char ** string, int * len);
static Uint16 QS_CAN_VERBOSE_can_msg_sprint(CAN_msg_t * msg, char * string, int len, QS_VERBOSE_msg_type_e verbose_msg_type);

#define	u32(x1,x2,x3,x4)		(U32FROMU8(msg->data[x1], msg->data[x2], msg->data[x3], msg->data[x4]))
#define	u16(x,y)				(U16FROMU8(msg->data[x], msg->data[y]))
#define s16(x,y)				((Sint16)(U16FROMU8(msg->data[x], msg->data[y])))
#define	u8(x)					(msg->data[x])
#define	s8(x)					((Sint8)(msg->data[x]))
#define angle_deg(x, y)			((Sint16)((((Sint32)((Sint16)(U16FROMU8(msg->data[x], msg->data[y]))))*180/PI4096)))
#define angle_rad(x, y)			s16(x,y)
//#define test(condition, string, len)	if(condition)	print(string, len, string)
#define print(buffer, len, ...) \
	do { \
		int addlen = snprintf(buffer, len, __VA_ARGS__); \
		if(addlen > 0) { \
			buffer += addlen; len -= addlen; \
		} \
	} while(0)

void QS_CAN_VERBOSE_can_msg_print(CAN_msg_t * msg, QS_VERBOSE_msg_type_e verbose_msg_type)
{
	char str[512];

	if(QS_CAN_VERBOSE_can_msg_sprint(msg, str, 512, verbose_msg_type) > 511)
	{
		debug_printf("ATTENTION, LA GENERATION DU MESSAGE CAN SUIVANT A DEBORDE DANS LA PILE, C'EST CRADE...\n");
	}

	OUTPUTLOG_printf(LOG_LEVEL_Always, str);
}




static Uint16 QS_CAN_VERBOSE_can_msg_sprint(CAN_msg_t * msg, char * string, int len, QS_VERBOSE_msg_type_e verbose_msg_type)
{
	char * ret = string;

	// Lister ici tout les sid des messages auquel vous ne voulez aucun affichage
	// Attention soyez bien sur de ce que vous entrez ici !
	switch(msg->sid){

#ifdef I_AM_CARTE_STRAT			// Message ignor� par la strat�gie

#endif

#ifdef I_AM_CARTE_PROP			// Message ignor� par la propulsion
		case BROADCAST_ADVERSARIES_POSITION:
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
#endif

#ifdef I_AM_CARTE_ACT			// Message ignor� par l'actionneur
		case BROADCAST_POSITION_ROBOT:
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
		case BROADCAST_ADVERSARIES_POSITION:
#endif

#ifdef I_AM_CARTE_IHM			// Message ignor� par l'IHM
		case BROADCAST_POSITION_ROBOT:
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
#endif

//-------------------------------- Message ignor� par tout le monde
			case DEBUG_AVOIDANCE_POLY:

			*string = '\0';
			return 0;
	}

	// Affichage du type de message CAN � afficher entrant, sortant ou du log
	if(verbose_msg_type == VERB_INPUT_MSG)
		print(string, len, "I");
	else if(verbose_msg_type == VERB_OUTPUT_MSG)
		print(string, len, "O");
	else
		print(string, len, " ");


	// Lister ici tout les sid des messages que vous voulez voir affich�s dans le terminal � la place de "xxx UNKNOW : you should add SID in code !"
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
		case BROADCAST_RESET:							print(string, len, "%.3x BROADCAST_RESET                        ", BROADCAST_RESET								);	break;

		case DEBUG_CARTE_P:								print(string, len, "%x DEBUG_CARTE_P                          ", DEBUG_CARTE_P									);	break;
		case DEBUG_FOE_POS:								print(string, len, "%x DEBUG_FOE_POS                          ", DEBUG_FOE_POS									);	break;
		case DEBUG_ELEMENT_UPDATED:						print(string, len, "%x DEBUG_ELEMENT_UPDATED                  ", DEBUG_ELEMENT_UPDATED							);	break;
		case DEBUG_PROPULSION_MAILBOX_IN_IS_FULL:		print(string, len, "%x DEBUG_PROPULSION_MAILBOX_IN_IS_FULL    ", DEBUG_PROPULSION_MAILBOX_IN_IS_FULL			);  break;
		case DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL:		print(string, len, "%x DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL   ", DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL			);  break;
		case DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT:	print(string, len, "%x DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT", DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT		);  break;
		case DEBUG_PROPULSION_SET_COEF:					print(string, len, "%x DEBUG_PROPULSION_SET_COEF              ", DEBUG_PROPULSION_SET_COEF						);	break;
		case DEBUG_PROPULSION_SET_ACCELERATION:			print(string, len, "%x DEBUG_PROPULSION_SET_ACCELERATION      ", DEBUG_PROPULSION_SET_ACCELERATION				);	break;
		case DEBUG_STRAT_STATE_CHANGED:					print(string, len, "%x DEBUG_STRAT_STATE_CHANGED              ", DEBUG_STRAT_STATE_CHANGED						);	break;
		case DEBUG_SET_ERROR_TRESHOLD_TRANSLATION:		print(string, len, "%x DEBUG_SET_ERROR_TRESHOLD_TRANSLATION   ", DEBUG_SET_ERROR_TRESHOLD_TRANSLATION			);	break;

		case STRAT_BUZZER_PLAY:							print(string, len, "%x STRAT_BUZZER_PLAY                      ", STRAT_BUZZER_PLAY								);	break;
		case STRAT_TRAJ_FINIE:							print(string, len, "%x STRAT_TRAJ_FINIE                       ", STRAT_TRAJ_FINIE								);	break;
		case STRAT_PROP_ERREUR:							print(string, len, "%x STRAT_PROP_ERREUR                      ", STRAT_PROP_ERREUR								);	break;
		case STRAT_ROBOT_FREINE:						print(string, len, "%x STRAT_ROBOT_FREINE                     ", STRAT_ROBOT_FREINE								);	break;
		case STRAT_PROP_FOE_DETECTED:					print(string, len, "%x STRAT_PROP_FOE_DETECTED                ", STRAT_PROP_FOE_DETECTED						);	break;
		case STRAT_SEND_REPORT:							print(string, len, "%x STRAT_SEND_REPORT                      ", STRAT_SEND_REPORT								);	break;

		case ACT_DO_SELFTEST:							print(string, len, "%x ACT_DO_SELFTEST                        ", ACT_DO_SELFTEST								);	break;
		case PROP_DO_SELFTEST:							print(string, len, "%x PROP_DO_SELFTEST                       ", PROP_DO_SELFTEST								);	break;
		case BEACON_DO_SELFTEST:						print(string, len, "%x BEACON_DO_SELFTEST                     ", BEACON_DO_SELFTEST								);	break;
		case IHM_DO_SELFTEST:							print(string, len, "%x IHM_DO_SELFTEST						  ", IHM_DO_SELFTEST								);	break;
		case STRAT_ACT_SELFTEST_DONE:					print(string, len, "%x STRAT_ACT_SELFTEST_DONE                ", STRAT_ACT_SELFTEST_DONE						);	break;
		case STRAT_PROP_SELFTEST_DONE:					print(string, len, "%x STRAT_PROP_SELFTEST_DONE               ", STRAT_PROP_SELFTEST_DONE						);	break;
		case STRAT_IHM_SELFTEST_DONE:					print(string, len, "%x STRAT_IHM_SELFTEST_DONE                ", STRAT_IHM_SELFTEST_DONE						);	break;
		case STRAT_BEACON_IR_SELFTEST_DONE:				print(string, len, "%x STRAT_BEACON_IR_SELFTEST_DONE          ", STRAT_BEACON_IR_SELFTEST_DONE					);	break;
		case STRAT_BEACON_US_SELFTEST_DONE:				print(string, len, "%x STRAT_BEACON_US_SELFTEST_DONE          ", STRAT_BEACON_US_SELFTEST_DONE					);	break;
		case ACT_PING:									print(string, len, "%x ACT_PING                               ", ACT_PING										);	break;
		case PROP_PING:									print(string, len, "%x PROP_PING                              ", PROP_PING										);	break;
		case BEACON_PING:								print(string, len, "%x BEACON_PING                            ", BEACON_PING									);	break;
		case IHM_PING:									print(string, len, "%x IHM_PING					              ", IHM_PING										);	break;
		case STRAT_ACT_PONG:							print(string, len, "%x STRAT_ACT_PONG                         ", STRAT_ACT_PONG									);	break;
		case STRAT_PROP_PONG:							print(string, len, "%x STRAT_PROP_PONG                        ", STRAT_PROP_PONG								);	break;
		case STRAT_BEACON_PONG:							print(string, len, "%x STRAT_BEACON_PONG                      ", STRAT_BEACON_PONG								);	break;
		case STRAT_IHM_PONG:							print(string, len, "%x STRAT_IHM_PONG						  ", STRAT_IHM_PONG									);	break;



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
		case XBEE_HOLLY_ASK_PROTECT:					print(string, len, "%x XBEE_HOLLY_ASK_PROTECT				  ", XBEE_HOLLY_ASK_PROTECT							);	break;
		case XBEE_WOOD_PROTECT_ZONE:					print(string, len, "%x XBEE_WOOD_PROTECT_ZONE				  ", XBEE_WOOD_PROTECT_ZONE							);	break;
		case XBEE_WOOD_TAKE_CUP:						print(string, len, "%x XBEE_WOOD_TAKE_CUP					  ", XBEE_WOOD_TAKE_CUP						    	);	break;
		case XBEE_HOLLY_START_MATCH:					print(string, len, "%x XBEE_HOLLY_START_MATCH				  ", XBEE_HOLLY_START_MATCH						    );	break;
		case XBEE_WOOD_CAN_DO_CLAP:					    print(string, len, "%x XBEE_WOOD_CAN_DO_CLAP				  ", XBEE_WOOD_CAN_DO_CLAP						    );	break;
		case PROP_WOOD_PROTECT_ZONE:					print(string, len, "%x PROP_WOOD_PROTECT_ZONE				  ", PROP_WOOD_PROTECT_ZONE							);	break;
		case ACT_RESULT:								print(string, len, "%x ACT_RESULT                             ", ACT_RESULT										);	break;
		case XBEE_WOOD_CAN_GO:							print(string, len, "%x XBEE_WOOD_CAN_GO                       ", XBEE_WOOD_CAN_GO					            );	break;
		case XBEE_HOLLY_TAKE_MIDDLE_FEET:				print(string, len, "%x XBEE_HOLLY_TAKE_MIDDLE_FEET            ", XBEE_HOLLY_TAKE_MIDDLE_FEET					);	break;
// Holly
		case ACT_POP_COLLECT_LEFT:						print(string, len, "%x ACT_POP_COLLECT_LEFT                   ", ACT_POP_COLLECT_LEFT							);	break;
		case ACT_POP_COLLECT_RIGHT:						print(string, len, "%x ACT_POP_COLLECT_RIGHT                  ", ACT_POP_COLLECT_RIGHT							);	break;
		case ACT_POP_DROP_LEFT:							print(string, len, "%x ACT_POP_DROP_LEFT                      ", ACT_POP_DROP_LEFT								);	break;
		case ACT_POP_DROP_RIGHT:						print(string, len, "%x ACT_POP_DROP_RIGHT                     ", ACT_POP_DROP_RIGHT								);	break;
		case ACT_BACK_SPOT_RIGHT:						print(string, len, "%x ACT_BACK_SPOT_RIGHT                    ", ACT_BACK_SPOT_RIGHT							);	break;
		case ACT_BACK_SPOT_LEFT:						print(string, len, "%x ACT_BACK_SPOT_LEFT                     ", ACT_BACK_SPOT_LEFT								);	break;
		case ACT_SPOT_POMPE_RIGHT:						print(string, len, "%x ACT_SPOT_POMPE_RIGHT                   ", ACT_SPOT_POMPE_RIGHT							);	break;
		case ACT_SPOT_POMPE_LEFT:						print(string, len, "%x ACT_SPOT_POMPE_LEFT                    ", ACT_SPOT_POMPE_LEFT							);	break;
		case ACT_CARPET_LAUNCHER_RIGHT:					print(string, len, "%x ACT_CARPET_LAUNCHER_RIGHT              ", ACT_CARPET_LAUNCHER_RIGHT						);	break;
		case ACT_CARPET_LAUNCHER_LEFT:					print(string, len, "%x ACT_CARPET_LAUNCHER_LEFT               ", ACT_CARPET_LAUNCHER_LEFT						);	break;
		case ACT_ELEVATOR:								print(string, len, "%x ACT_ELEVATOR                           ", ACT_ELEVATOR									);	break;
		case ACT_PINCEMI_RIGHT:							print(string, len, "%x ACT_PINCEMI_RIGHT                      ", ACT_PINCEMI_RIGHT								);	break;
		case ACT_PINCEMI_LEFT:							print(string, len, "%x ACT_PINCEMI_LEFT                       ", ACT_PINCEMI_LEFT								);	break;
		case ACT_STOCK_RIGHT:							print(string, len, "%x ACT_STOCK_RIGHT                        ", ACT_STOCK_RIGHT								);	break;
		case ACT_STOCK_LEFT:							print(string, len, "%x ACT_STOCK_LEFT                         ", ACT_STOCK_LEFT									);	break;
		case ACT_CUP_NIPPER:							print(string, len, "%x ACT_CUP_NIPPER                         ", ACT_CUP_NIPPER									);	break;
		case ACT_CUP_NIPPER_ELEVATOR:					print(string, len, "%x ACT_CUP_NIPPER_ELEVATOR                ", ACT_CUP_NIPPER_ELEVATOR				    	);	break;
		case ACT_CLAP_HOLLY:							print(string, len, "%x ACT_CLAP_HOLLY                         ", ACT_CLAP_HOLLY									);	break;
//Wood
		case ACT_PINCE_GAUCHE:							print(string, len, "%x ACT_PINCE_GAUCHE                       ", ACT_PINCE_GAUCHE					         	);	break;
		case ACT_PINCE_DROITE:							print(string, len, "%x ACT_PINCE_DROITE                       ", ACT_PINCE_DROITE					         	);	break;
		case ACT_CLAP:									print(string, len, "%x ACT_CLAP                               ", ACT_CLAP										);	break;
		case ACT_POP_DROP_LEFT_WOOD:					print(string, len, "%x ACT_POP_DROP_LEFT_WOOD                 ", ACT_POP_DROP_LEFT_WOOD							);	break;
		case ACT_POP_DROP_RIGHT_WOOD:					print(string, len, "%x ACT_POP_DROP_RIGHT_WOOD                ", ACT_POP_DROP_RIGHT_WOOD						);	break;

		case ACT_TEST_SERVO:							print(string, len, "%x ACT_TEST_SERVO                         ", ACT_TEST_SERVO									);	break;
		case PROP_SCAN_CUP:                             print(string, len, "%x PROP_SCAN_CUP                          ", PROP_SCAN_CUP							     	);	break;
		case STRAT_CUP_POSITION:						print(string, len, "%x STRAT_CUP_POSITION                     ", STRAT_CUP_POSITION						        );	break;

		case IHM_LCD_BIT_RESET:							print(string, len, "%x IHM_LCD_BIT_RESET                      ", IHM_LCD_BIT_RESET								);	break;
		case IHM_GET_SWITCH:							print(string, len, "%x IHM_GET_SWITCH                         ", IHM_GET_SWITCH									);	break;
		case IHM_SET_LED:								print(string, len, "%x IHM_SET_LED                            ", IHM_SET_ERROR									);	break;
		case IHM_SET_ERROR:								print(string, len, "%x IHM_SET_ERROR                          ",IHM_BIROUTE_IS_REMOVED							);	break;
		case IHM_BUTTON:								print(string, len, "%.3x IHM_BUTTON                             ", IHM_BUTTON									);	break;
		case IHM_SWITCH:								print(string, len, "%.3x IHM_SWITCH                             ", IHM_SWITCH									);	break;
		case IHM_SWITCH_ALL:							print(string, len, "%.3x IHM_SWITCH_ALL                         ", IHM_SWITCH_ALL								);	break;
		case IHM_POWER:									print(string, len, "%.3x IHM_POWER                              ", IHM_POWER									);	break;
		case IHM_BIROUTE_IS_REMOVED:					print(string, len, "%.3x IHM_BIROUTE_IS_REMOVED                 ",IHM_BIROUTE_IS_REMOVED						);	break;

		default:										print(string, len, "%x UNKNOW : you should add SID in code !  ", msg->sid										);	break;
	}

	// Lister ici tout les sid des messages dont vous souhaitez afficher d'avantage d'information
	// L'information ajout� par d�faut est un retour � la ligne
	switch(msg->sid)
	{
		case BROADCAST_ALIM:				    print(string, len, "| �tat : %s\n", (u8(0))?"ALIM_ON":"ALIM_OFF");		break;
		case BROADCAST_COULEUR:					print(string, len, "| CouleurEst %s\n", (u8(0))?"VERT":"JAUNE"	);		break;
		case BROADCAST_POSITION_ROBOT :			print(string, len, "| JeSuisEn  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:		if(u8(0) || u8(4))
												print(string, len, "ERRs:0x%2x 0x%2x|",u8(0), u8(4));
												print(string, len, "angleR1=%3d |dR1=%3dcm |angleR2=%3d |dR2=%3dcm \n", angle_deg(1,2), (Uint16)(u8(3)),angle_deg(5,6), (Uint16)(u8(7)));	break;

		case ACT_RESULT:
			print(string,len, "| act0x%x : ", u8(0));
			switch(u8(0))
			{
				case (Uint8)ACT_TEST_SERVO:				print(string,len, "ACT_TEST_SERVO ");			break;
				case (Uint8)ACT_POP_COLLECT_LEFT:		print(string,len, "ACT_POP_COLLECT_LEFT ");		break;
				case (Uint8)ACT_POP_COLLECT_RIGHT:		print(string,len, "ACT_POP_COLLECT_RIGHT ");	break;
				case (Uint8)ACT_POP_DROP_LEFT:			print(string,len, "ACT_POP_DROP_LEFT ");		break;
				case (Uint8)ACT_POP_DROP_RIGHT:			print(string,len, "ACT_POP_DROP_RIGHT ");		break;
				case (Uint8)ACT_ELEVATOR:				print(string,len, "ACT_ELEVATOR ");				break;
				case (Uint8)ACT_PINCEMI_RIGHT:			print(string,len, "ACT_PINCEMI_RIGHT ");		break;
				case (Uint8)ACT_PINCEMI_LEFT:			print(string,len, "ACT_PINCEMI_LEFT ");			break;
				case (Uint8)ACT_STOCK_RIGHT:			print(string,len, "ACT_STOCK_RIGHT ");			break;
				case (Uint8)ACT_STOCK_LEFT:				print(string,len, "ACT_STOCK_LEFT ");			break;
				case (Uint8)ACT_CUP_NIPPER:				print(string,len, "ACT_CUP_NIPPER ");			break;
				case (Uint8)ACT_CUP_NIPPER_ELEVATOR:	print(string,len, "ACT_CUP_NIPPER_ELEVATOR ");	break;
				case (Uint8)ACT_BACK_SPOT_RIGHT:		print(string,len, "ACT_BACK_SPOT_RIGHT ");		break;
				case (Uint8)ACT_BACK_SPOT_LEFT:			print(string,len, "ACT_BACK_SPOT_LEFT ");		break;
				case (Uint8)ACT_SPOT_POMPE_LEFT:		print(string,len, "ACT_SPOT_POMPE_LEFT ");		break;
				case (Uint8)ACT_SPOT_POMPE_RIGHT:		print(string,len, "ACT_SPOT_POMPE_RIGHT ");		break;
				case (Uint8)ACT_CARPET_LAUNCHER_RIGHT:	print(string,len, "ACT_CARPET_LAUNCHER_RIGHT ");break;
				case (Uint8)ACT_CARPET_LAUNCHER_LEFT:	print(string,len, "ACT_CARPET_LAUNCHER_LEFT ");	break;
				case (Uint8)ACT_CLAP_HOLLY:				print(string,len, "ACT_CLAP_HOLLY ");			break;
				case (Uint8)ACT_PINCE_GAUCHE:			print(string,len, "ACT_PINCE_GAUCHE ");			break;
				case (Uint8)ACT_PINCE_DROITE:			print(string,len, "ACT_PINCE_DROITE ");			break;
				case (Uint8)ACT_CLAP:					print(string,len, "ACT_CLAP ");					break;
				case (Uint8)ACT_POP_DROP_LEFT_WOOD:		print(string,len, "ACT_POP_DROP_LEFT_WOOD ");	break;
				case (Uint8)ACT_POP_DROP_RIGHT_WOOD:	print(string,len, "ACT_POP_DROP_RIGHT_WOOD ");	break;
				default:								print(string,len, "UNKNOW ACT -> complete verbose !");		break;
			}

			switch(u8(0)){
				case (Uint8)ACT_POP_COLLECT_LEFT:
					switch(u8(1)){
						case (Uint8)ACT_POP_COLLECT_LEFT_CLOSED:	print(string,len, "| ACT_POP_COLLECT_LEFT_CLOSED |");	break;
						case (Uint8)ACT_POP_COLLECT_LEFT_OPEN:	print(string,len, "| ACT_POP_COLLECT_LEFT_OPEN |");			break;
						case (Uint8)ACT_POP_COLLECT_LEFT_MID:	print(string,len, "| ACT_POP_COLLECT_LEFT_MID |");			break;
						case (Uint8)ACT_POP_COLLECT_LEFT_STOP:	print(string,len, "| ACT_POP_COLLECT_LEFT_STOP |");			break;
					}
				break;
				case (Uint8)ACT_POP_COLLECT_RIGHT:
					switch(u8(1)){
						case (Uint8)ACT_POP_COLLECT_RIGHT_CLOSED:	print(string,len, "| ACT_POP_COLLECT_RIGHT_CLOSED |");	break;
						case (Uint8)ACT_POP_COLLECT_RIGHT_OPEN:	print(string,len, "| ACT_POP_COLLECT_RIGHT_OPEN |");			break;
						case (Uint8)ACT_POP_COLLECT_RIGHT_MID:	print(string,len, "| ACT_POP_COLLECT_RIGHT_MID |");			break;
						case (Uint8)ACT_POP_COLLECT_RIGHT_STOP:	print(string,len, "| ACT_POP_COLLECT_RIGHT_STOP |");			break;
					}
				break;
				case (Uint8)ACT_ELEVATOR:
					switch(u8(1)){
						case (Uint8)ACT_ELEVATOR_BOT:	print(string,len, "| ACT_ELEVATOR_BOT |");			break;
						case (Uint8)ACT_ELEVATOR_MID:	print(string,len, "| ACT_ELEVATOR_MID |");			break;
						case (Uint8)ACT_ELEVATOR_PRE_TOP:	print(string,len, "| ACT_ELEVATOR_PRE_TOP |");	break;
						case (Uint8)ACT_ELEVATOR_TOP:	print(string,len, "| ACT_ELEVATOR_TOP |");			break;
						case (Uint8)ACT_ELEVATOR_STOP:	print(string,len, "| ACT_ELEVATOR_STOP |");			break;
					}
				break;
				case (Uint8)ACT_PINCEMI_RIGHT:
					switch(u8(1)){
						case (Uint8)ACT_PINCEMI_RIGHT_CLOSE:		print(string,len, "| ACT_PINCEMI_RIGHT_CLOSE |");			break;
						case (Uint8)ACT_PINCEMI_RIGHT_CLOSE_INNER:	print(string,len, "| ACT_PINCEMI_RIGHT_CLOSE_INNER |");		break;
						case (Uint8)ACT_PINCEMI_RIGHT_LOCK:			print(string,len, "| ACT_PINCEMI_RIGHT_LOCK |");			break;
						case (Uint8)ACT_PINCEMI_RIGHT_OPEN:			print(string,len, "| ACT_PINCEMI_RIGHT_OPEN |");			break;
						case (Uint8)ACT_PINCEMI_RIGHT_OPEN_GREAT:	print(string,len, "| ACT_PINCEMI_RIGHT_OPEN_GREAT |");		break;
						case (Uint8)ACT_PINCEMI_RIGHT_STOP:			print(string,len, "| ACT_PINCEMI_RIGHT_STOP |");			break;
					}
				break;
				case (Uint8)ACT_PINCEMI_LEFT:
					switch(u8(1)){
						case (Uint8)ACT_PINCEMI_LEFT_CLOSE:			print(string,len, "| ACT_PINCEMI_LEFT_CLOSE |");	    break;
						case (Uint8)ACT_PINCEMI_LEFT_CLOSE_INNER:	print(string,len, "| ACT_PINCEMI_LEFT_CLOSE_INNER |");	break;
						case (Uint8)ACT_PINCEMI_LEFT_LOCK:			print(string,len, "| ACT_PINCEMI_LEFT_LOCK |");			break;
						case (Uint8)ACT_PINCEMI_LEFT_OPEN:			print(string,len, "| ACT_PINCEMI_LEFT_OPEN |");			break;
						case (Uint8)ACT_PINCEMI_LEFT_OPEN_GREAT:	print(string,len, "| ACT_PINCEMI_LEFT_OPEN_GREAT |");	break;
						case (Uint8)ACT_PINCEMI_LEFT_STOP:			print(string,len, "| ACT_PINCEMI_LEFT_STOP |");			break;
					}
				break;
				case (Uint8)ACT_STOCK_RIGHT:
					switch(u8(1)){
						case (Uint8)ACT_STOCK_RIGHT_CLOSE:	print(string,len, "| ACT_STOCK_RIGHT_CLOSE |");		break;
						case (Uint8)ACT_STOCK_RIGHT_LOCK:	print(string,len, "| ACT_STOCK_RIGHT_LOCK |");		break;
						case (Uint8)ACT_STOCK_RIGHT_UNLOCK:	print(string,len, "| ACT_STOCK_RIGHT_UNLOCK |");	break;
						case (Uint8)ACT_STOCK_RIGHT_OPEN:	print(string,len, "| ACT_STOCK_RIGHT_OPEN |");		break;
						case (Uint8)ACT_STOCK_RIGHT_STOP:	print(string,len, "| ACT_STOCK_RIGHT_STOP |");		break;
					}
				break;
				case (Uint8)ACT_STOCK_LEFT:
					switch(u8(1)){
						case (Uint8)ACT_STOCK_LEFT_CLOSE:	print(string,len, "| ACT_STOCK_LEFT_CLOSE |");		break;
						case (Uint8)ACT_STOCK_LEFT_LOCK:	print(string,len, "| ACT_STOCK_LEFT_LOCK |");		break;
						case (Uint8)ACT_STOCK_LEFT_UNLOCK:	print(string,len, "| ACT_STOCK_LEFT_UNLOCK |");		break;
						case (Uint8)ACT_STOCK_LEFT_OPEN:	print(string,len, "| ACT_STOCK_LEFT_OPEN |");		break;
						case (Uint8)ACT_STOCK_LEFT_STOP:	print(string,len, "| ACT_STOCK_LEFT_STOP |");		break;
					}
				break;
				case (Uint8)ACT_CUP_NIPPER:
					switch(u8(1)){
						case (Uint8)ACT_CUP_NIPPER_OPEN:	print(string,len, "| ACT_CUP_NIPPER_OPEN |");		break;
						case (Uint8)ACT_CUP_NIPPER_CLOSE:	print(string,len, "| ACT_CUP_NIPPER_CLOSE |");		break;
						case (Uint8)ACT_CUP_NIPPER_LOCK:	print(string,len, "| ACT_CUP_NIPPER_LOCK |");		break;
						case (Uint8)ACT_CUP_NIPPER_STOP:	print(string,len, "| ACT_CUP_NIPPER_STOP |");		break;
					}
				break;
				case (Uint8)ACT_CUP_NIPPER_ELEVATOR:
					switch(u8(1)){
						case (Uint8)ACT_CUP_NIPPER_ELEVATOR_IDLE:	print(string,len, "| ACT_CUP_NIPPER_ELEVATOR_IDLE |");		break;
						case (Uint8)ACT_CUP_NIPPER_ELEVATOR_UP:		print(string,len, "| ACT_CUP_NIPPER_ELEVATOR_UP |");		break;
						case (Uint8)ACT_CUP_NIPPER_ELEVATOR_STOP:	print(string,len, "| ACT_CUP_NIPPER_ELEVATOR_STOP |");		break;

					}
				break;
				case (Uint8)ACT_BACK_SPOT_RIGHT:
					switch(u8(1)){
						case (Uint8)ACT_BACK_SPOT_RIGHT_CLOSED:	print(string,len, "| ACT_BACK_SPOT_RIGHT_CLOSED |");	break;
						case (Uint8)ACT_BACK_SPOT_RIGHT_OPEN:	print(string,len, "| ACT_BACK_SPOT_RIGHT_OPEN |");		break;
						case (Uint8)ACT_BACK_SPOT_RIGHT_STOP:	print(string,len, "| ACT_BACK_SPOT_RIGHT_STOP |");		break;
					}
				break;
				case (Uint8)ACT_BACK_SPOT_LEFT:
					switch(u8(1)){
						case (Uint8)ACT_BACK_SPOT_LEFT_CLOSED:	print(string,len, "| ACT_BACK_SPOT_LEFT_CLOSED |");	break;
						case (Uint8)ACT_BACK_SPOT_LEFT_OPEN:	print(string,len, "| ACT_BACK_SPOT_LEFT_OPEN |");	break;
						case (Uint8)ACT_BACK_SPOT_LEFT_STOP:	print(string,len, "| ACT_BACK_SPOT_LEFT_STOP |");	break;
					}
				break;
				case (Uint8)ACT_SPOT_POMPE_LEFT:
					switch(u8(1)){
						case (Uint8)ACT_SPOT_POMPE_LEFT_NORMAL:	print(string,len, "| ACT_SPOT_POMPE_LEFT_NORMAL |");	  break;
						case (Uint8)ACT_SPOT_POMPE_LEFT_REVERSE:	print(string,len, "| ACT_SPOT_POMPE_LEFT_REVERSE |");  break;
						case (Uint8)ACT_SPOT_POMPE_LEFT_STOP:	print(string,len, "| ACT_SPOT_POMPE_LEFT_STOP |");	       break;
					}
				break;
				case (Uint8)ACT_SPOT_POMPE_RIGHT:
					switch(u8(1)){
						case (Uint8)ACT_SPOT_POMPE_RIGHT_NORMAL:	print(string,len, "| ACT_SPOT_POMPE_LEFT_NORMAL |");	  break;
						case (Uint8)ACT_SPOT_POMPE_RIGHT_REVERSE:	print(string,len, "| ACT_SPOT_POMPE_LEFT_REVERSE |");  break;
						case (Uint8)ACT_SPOT_POMPE_RIGHT_STOP:	print(string,len, "| ACT_SPOT_POMPE_LEFT_STOP |");	       break;
					}
				break;
				case (Uint8)ACT_CARPET_LAUNCHER_LEFT:
					switch(u8(1)){
						case (Uint8)ACT_CARPET_LAUNCHER_LEFT_IDLE:	print(string,len, "| ACT_CARPET_LAUNCHER_LEFT_IDLE |");		break;
						case (Uint8)ACT_CARPET_LAUNCHER_LEFT_LOADING:	print(string,len, "| ACT_CARPET_LAUNCHER_LEFT_LOADING |");		break;
						case (Uint8)ACT_CARPET_LAUNCHER_LEFT_LAUNCH:	print(string,len, "| ACT_CARPET_LAUNCHER_LEFT_LAUNCH |");		break;
						case (Uint8)ACT_CARPET_LAUNCHER_LEFT_STOP:	print(string,len, "| ACT_CARPET_LAUNCHER_LEFT_STOP |");		break;
					}
				break;
				case (Uint8)ACT_CARPET_LAUNCHER_RIGHT:
					switch(u8(1)){
						case (Uint8)ACT_CARPET_LAUNCHER_RIGHT_IDLE:	print(string,len, "| ACT_CARPET_LAUNCHER_RIGHT_IDLE |");		break;
						case (Uint8)ACT_CARPET_LAUNCHER_RIGHT_LOADING:	print(string,len, "| ACT_CARPET_LAUNCHER_RIGHT_LOADING |");		break;
						case (Uint8)ACT_CARPET_LAUNCHER_RIGHT_LAUNCH:	print(string,len, "| ACT_CARPET_LAUNCHER_RIGHT_LAUNCH |");		break;
						case (Uint8)ACT_CARPET_LAUNCHER_RIGHT_STOP:	print(string,len, "| ACT_CARPET_LAUNCHER_RIGHT_STOP |");		break;
					}
				break;
				case (Uint8)ACT_CLAP_HOLLY:
					switch(u8(1)){
						case (Uint8)ACT_CLAP_HOLLY_IDLE:	print(string,len, "| ACT_CLAP_HOLLY_IDLE |");		break;
						case (Uint8)ACT_CLAP_HOLLY_LEFT:	print(string,len, "| ACT_CLAP_HOLLY_LEFT |");		break;
						case (Uint8)ACT_CLAP_HOLLY_RIGHT:	print(string,len, "| ACT_CLAP_HOLLY_RIGHT |");		break;
						case (Uint8)ACT_CLAP_HOLLY_MIDDLE_LEFT:		print(string,len, "| ACT_CLAP_HOLLY_MIDDLE_LEFT |");		break;
						case (Uint8)ACT_CLAP_HOLLY_MIDDLE_RIGHT	:	print(string,len, "| ACT_CLAP_HOLLY_MIDDLE_RIGHT |");		break;
						case (Uint8)ACT_CLAP_HOLLY_STOP:	print(string,len, "| ACT_CLAP_HOLLY_STOP |");		break;
					}
				break;
				case (Uint8)ACT_PINCE_GAUCHE:
					switch(u8(1)){
						case (Uint8)ACT_PINCE_GAUCHE_IDLE_POS:	print(string,len, "| ACT_PINCE_GAUCHE_IDLE_POS |");	break;
						case (Uint8)ACT_PINCE_GAUCHE_CLOSED:	print(string,len, "| ACT_PINCE_GAUCHE_CLOSED |");	break;
						case (Uint8)ACT_PINCE_GAUCHE_OPEN:		print(string,len, "| ACT_PINCE_GAUCHE_OPEN |");		break;
						case (Uint8)ACT_PINCE_GAUCHE_MID_POS:	print(string,len, "| ACT_PINCE_GAUCHE_MID_POS |");	break;
						case (Uint8)ACT_PINCE_GAUCHE_STOP:		print(string,len, "| ACT_PINCE_GAUCHE_STOP |");		break;
						case (Uint8)ACT_CONFIG:					print(string,len, "| ACT_CONFIG |");				break;
					}
					break;
				case (Uint8)ACT_PINCE_DROITE:
					switch(u8(1)){
						case (Uint8)ACT_PINCE_DROITE_IDLE_POS:	print(string,len, "| ACT_PINCE_DROITE_IDLE_POS |");	break;
						case (Uint8)ACT_PINCE_DROITE_CLOSED:	print(string,len, "| ACT_PINCE_DROITE_CLOSED |");	break;
						case (Uint8)ACT_PINCE_DROITE_OPEN:	print(string,len, "| ACT_PINCE_DROITE_OPEN |");			break;
						case (Uint8)ACT_PINCE_DROITE_MID_POS:	print(string,len, "| ACT_PINCE_DROITE_MID_POS |");	break;
						case (Uint8)ACT_PINCE_DROITE_STOP:	print(string,len, "| ACT_PINCE_DROITE_STOP |");			break;
						case (Uint8)ACT_CONFIG:					print(string,len, "| ACT_CONFIG |");				break;
					}
					break;

			}

			//Todo ajouter la commande dont on envoie le r�sultat (data 1) ???
			switch(u8(2))
			{
				case ACT_RESULT_DONE:			print(string,len, "| DONE | ");			break;
				case ACT_RESULT_FAILED:			print(string,len, "| FAILED | ");		break;
				case ACT_RESULT_NOT_HANDLED:	print(string,len, "| NOT_HANDLED | ");	break;
				default:						print(string,len, "| UNKNOW status | ");		break;
			}

			switch(u8(3))
			{
				case ACT_RESULT_ERROR_OK:			if(u8(1) != ACT_RESULT_DONE)	print(string,len, "NO ERROR ");		break;
				case ACT_RESULT_ERROR_TIMEOUT:		print(string,len, "TIMEOUT ");					break;
				case ACT_RESULT_ERROR_OTHER:		print(string,len, "OTHER ");					break;
				case ACT_RESULT_ERROR_NOT_HERE:		print(string,len, "ACTUATOR NO RESPONDING !");	break;
				case ACT_RESULT_ERROR_LOGIC:		print(string,len, "SOFTWARE ERROR !");			break;
				case ACT_RESULT_ERROR_NO_RESOURCES:	print(string,len, "NOT ENOUGH RESOURCES !");	break;
				case ACT_RESULT_ERROR_INVALID_ARG:	print(string,len, "INVALID ARG ! ");			break;
				case ACT_RESULT_ERROR_CANCELED:		print(string,len, "ACTION CANCELED ");			break;
				default:							print(string,len, "UNKNOW ERROR !");			break;
			}
			print(string,len, "\n");
			break;
		case DEBUG_FOE_POS:						print(string, len, "|\n");												break;
		case DEBUG_PROPULSION_SET_COEF:			print(string, len, "| COEF_ID=%d  VALUE=%ld\n", u8(0),u32(1,2,3,4));	break;
		case DEBUG_PROPULSION_SET_ACCELERATION:	print(string, len, "| Acc=%d\n", u16(0,1));								break;
		case DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT :	print(string, len, "| duration : %dms  section : %s\n", u16(0,1), it_state_name[u8(2)]);										break;

		case IR_ERROR_RESULT:					print_ir_result(msg, &string, &len);									break;

		case STRAT_TRAJ_FINIE:					print(string, len, "| J'arrive  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case STRAT_PROP_ERREUR:					print(string, len, "| J'erreur  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case STRAT_ROBOT_FREINE:				print(string, len, "| J'freine  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", u16(0,1)&0x1FFF, u16(2,3)&0x1FFF, angle_rad(4, 5), angle_deg(4, 5), ((Uint16)(u8(0)>>5))*250, u8(2)>>5, u8(6) , u8(7));								break;
		case STRAT_PROP_FOE_DETECTED:			print(string, len, "| x : %d   y : %d   %s\n", u16(0,1), u16(2,3), (u8(4))?"avec timeout":"");	break;
		case STRAT_SEND_REPORT:					print(string, len, "| Distance : %ld | Rotation : %lu | Rotation max : %ld\n", ((Uint32)u16(4,5))<<1, ((Sint32)s16(0,1)<<3)*180/PI4096, ((Sint32)s16(2,3)<<3)*180/PI4096);	break;

		case PROP_GO_ANGLE:						print(string, len, "| VaAngle   teta=%d=%d� %s %s %s %d%s %s\n",  angle_rad(1, 2),  angle_deg(1, 2), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x01)?"avant":((u8(6)&0x10)?"arri�re":"")	);						break;
		case PROP_OFFSET_AVOID:					print(string, len, "| Xleft : %d  Xright : %d  Yfront : %d  Yback : %d\n",u16(0,1), u16(2,3), u16(4,5), u16(6,7));		break;
		case PROP_GO_POSITION:					print(string, len, "| VaPos     x=%d y=%d %s %s %s vitesse %d%s %s %s\n", u16(1,2), u16(3,4), (u8(0) & 0x20)?"multi":" ", (u8(0) & 0x10)?"pas_now":"now", (u8(0) & 0x01)?"relatif":" ", u8(5),(u8(5)==0x00)?"=rapide":((u8(5)==0x01)?"=lente":""), (u8(6)&0x02)?"avant":((u8(6)&0x01)?"arri�re":"any_way"), ((u8(0)&0x02)?"mode bordure":""));	break;
		case PROP_SET_POSITION:					print(string, len, "| PrendPos  X=%d | Y=%d | teta=0x%x=%d�\n", u16(0,1), u16(2,3),angle_rad(4, 5),  angle_deg(4, 5));													break;
		case PROP_SEND_PERIODICALLY_POSITION: 	print(string, len, "| DitPos    p�riode=%d | translation = %dmm | rotation = %d�\n", u16(0,1),u16(2, 3),  angle_deg(4, 5));										break;
		case PROP_RUSH_IN_THE_WALL:				print(string, len, "| sens %s | asservissement rotation %sactiv�e\n", (u8(0)==1)?"arri�re":((u8(0)==2)?"avant":"quelconque"), (u8(1))?"":"d�s");				break;
		case PROP_CALIBRATION:					print(string, len, "| Calibre   sens %s\n", (u8(0)==1)?"arri�re":((u8(0)==2)?"avant":"quelconque"));																	break;
		case PROP_WARN_ANGLE:					if(u8(0)) print(string, len, "| avertisseur en teta=0x%d=%d�\n", angle_rad(0, 1), angle_deg(0, 1)); else print(string, len, "d�sactivation de l'avertisseur en angle\n");	break;
		case PROP_WARN_X:						if(u8(0)) print(string, len, "| avertisseur en x=%d\n", u16(0,1));                                  else print(string, len, "d�sactivation de l'avertisseur en X\n");		break;
		case PROP_WARN_Y:						if(u8(0)) print(string, len, "| avertisseur en y=%d\n", u16(0,1));                                  else print(string, len, "d�sactivation de l'avertisseur en Y\n");		break;
		case DEBUG_PROP_MOVE_POSITION:			print(string, len, "| offset d'aleration x : %d    y : %d    teta : %d\n", u16(0,1), u16(2,3), u16(4,5));	break;

		case IHM_BUTTON:						print_ihm_result(msg, &string, &len);			break;
		case IHM_SWITCH:						print_ihm_result(msg, &string, &len);			break;
		case IHM_GET_SWITCH:					print_ihm_result(msg, &string, &len);			break;
		case IHM_POWER:							print_ihm_result(msg, &string, &len);			break;
		case IHM_SET_LED:						print_ihm_result(msg, &string, &len);			break;
		case IHM_SET_ERROR:						print(string, len, "error type : %d     state : %s\n",u8(0), (u8(1))?"set":"reset");	break;


		default:								print(string, len, "|\n");						break;
	}
	return string - ret;
}

static void print_ir_result(CAN_msg_t * msg, char ** string, int * len){
	print(*string, *len, "\n_____Infos erreurs balise(s) infrarouge____\n");
	print(*string, *len, "%d erreurs de type 0 : AUCUNE_ERREUR\n",msg->data[0]);
	print(*string, *len, "%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n",msg->data[1]);
	print(*string, *len, "%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n",msg->data[2]);
	print(*string, *len, "%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n",msg->data[3]);
	print(*string, *len, "%d erreurs de type 4 : ERREUR_TROP_PROCHE\n",msg->data[4]);
	print(*string, *len, "%d erreurs de type 5 : ERREUR_TROP_LOIN\n",msg->data[5]);
	print(*string, *len, "%d erreurs de type 6 : ERROR_OBSOLESCENCE\n",msg->data[6]);
}

static void print_ihm_result(CAN_msg_t * msg, char ** string, int * len){
	switch (msg->sid) {
		case IHM_BUTTON:
			switch(msg->data[0]){
				case BP_SELFTEST_IHM:		print(*string, *len, "| bp_selftest, %s\n",(msg->data[1])?"long push" : "direct push");			break;
				case BP_CALIBRATION_IHM:	print(*string, *len, "| bp_calibration, %s\n",(msg->data[1])?"long push" : "direct push");		break;
				case BP_PRINTMATCH_IHM:		print(*string, *len, "| bp_print_match, %s\n",(msg->data[1])?"long push" : "direct push");		break;
				case BP_OK_IHM:				print(*string, *len, "| bp_ok, %s\n",(msg->data[1])?"long push" : "direct push");				break;
				case BP_UP_IHM:				print(*string, *len, "| bp_up, %s\n",(msg->data[1])?"long push" : "direct push");				break;
				case BP_DOWN_IHM:			print(*string, *len, "| bp_down, %s\n",(msg->data[1])?"long push" : "direct push");				break;
				case BP_SET_IHM:			print(*string, *len, "| bp_set, %s\n",(msg->data[1])?"long push" : "direct push");				break;
				case BP_RFU_IHM	:			print(*string, *len, "| bp_rfu, %s\n",(msg->data[1])?"long push" : "direct push");				break;
				default:					print(*string, *len, "| Button %d active, %s\n",(msg->data[0])-8,(msg->data[1])?"long push" : "direct push");
			}
			break;
		case IHM_SWITCH:{
			Uint8 i;
			for(i = 0; i < msg->size; i++){
				switch ((msg->data[i]) & IHM_SWITCH_MASK) {
					case BIROUTE_IHM:			print(*string, *len, "| sw_biroute = %s\n",(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");					break;
					case SWITCH_COLOR_IHM:		print(*string, *len, "| sw_color = %s\n",(msg->data[i] & IHM_SWITCH_ON)? "VERT":"JAUNE");				break;
					case SWITCH_LCD_IHM:		print(*string, *len, "| sw_lcd = %s\n",(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");						break;
					case SWITCH0_IHM:			print(*string, *len, "| %s = %s\n",SWITCH0_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH1_IHM:			print(*string, *len, "| %s = %s\n",SWITCH1_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH2_IHM:			print(*string, *len, "| %s = %s\n",SWITCH2_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH3_IHM:			print(*string, *len, "| %s = %s\n",SWITCH3_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH4_IHM:			print(*string, *len, "| %s = %s\n",SWITCH4_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH5_IHM:			print(*string, *len, "| %s = %s\n",SWITCH5_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH6_IHM:			print(*string, *len, "| %s = %s\n",SWITCH6_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH7_IHM:			print(*string, *len, "| %s = %s\n",SWITCH7_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH8_IHM:			print(*string, *len, "| %s = %s\n",SWITCH8_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH9_IHM:			print(*string, *len, "| %s = %s\n",SWITCH9_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH10_IHM:			print(*string, *len, "| %s = %s\n",SWITCH10_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH11_IHM:			print(*string, *len, "| %s = %s\n",SWITCH11_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH12_IHM:			print(*string, *len, "| %s = %s\n",SWITCH12_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH13_IHM:			print(*string, *len, "| %s = %s\n",SWITCH13_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH14_IHM:			print(*string, *len, "| %s = %s\n",SWITCH14_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH15_IHM:			print(*string, *len, "| %s = %s\n",SWITCH15_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH16_IHM:			print(*string, *len, "| %s = %s\n",SWITCH16_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH17_IHM:			print(*string, *len, "| %s = %s\n",SWITCH17_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					case SWITCH18_IHM:			print(*string, *len, "| %s = %s\n",SWITCH18_IHM_NAME,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");		break;
					default:					print(*string, *len, "| SWITCH %d = %s\n",(msg->data[i] & IHM_SWITCH_MASK)-3,(msg->data[i] & IHM_SWITCH_ON)? "ON":"OFF");	// -3 car biroute, color, lcd avant
				}
			}
		}break;
		case IHM_GET_SWITCH:{
			Uint8 i;
			if(msg->size == 0)
				print(*string, *len,"| ALL SWITCHS\n");
			else
			{
				for(i = 0; i < msg->size; i++)
				{
					switch (msg->data[i]) {
						case BIROUTE_IHM:			print(*string, *len, "| sw_biroute\n");					break;
						case SWITCH_COLOR_IHM:		print(*string, *len, "| sw_color\n");					break;
						case SWITCH_LCD_IHM:		print(*string, *len, "| sw_lcd\n");						break;
						case SWITCH0_IHM:			print(*string, *len, "| %s\n",SWITCH0_IHM_NAME);		break;
						case SWITCH1_IHM:			print(*string, *len, "| %s\n",SWITCH1_IHM_NAME);		break;
						case SWITCH2_IHM:			print(*string, *len, "| %s\n",SWITCH2_IHM_NAME);		break;
						case SWITCH3_IHM:			print(*string, *len, "| %s\n",SWITCH3_IHM_NAME);		break;
						case SWITCH4_IHM:			print(*string, *len, "| %s\n",SWITCH4_IHM_NAME);		break;
						case SWITCH5_IHM:			print(*string, *len, "| %s\n",SWITCH5_IHM_NAME);		break;
						case SWITCH6_IHM:			print(*string, *len, "| %s\n",SWITCH6_IHM_NAME);		break;
						case SWITCH7_IHM:			print(*string, *len, "| %s\n",SWITCH7_IHM_NAME);		break;
						case SWITCH8_IHM:			print(*string, *len, "| %s\n",SWITCH8_IHM_NAME);		break;
						case SWITCH9_IHM:			print(*string, *len, "| %s\n",SWITCH9_IHM_NAME);		break;
						case SWITCH10_IHM:			print(*string, *len, "| %s\n",SWITCH10_IHM_NAME);		break;
						case SWITCH11_IHM:			print(*string, *len, "| %s\n",SWITCH11_IHM_NAME);		break;
						case SWITCH12_IHM:			print(*string, *len, "| %s\n",SWITCH12_IHM_NAME);		break;
						case SWITCH13_IHM:			print(*string, *len, "| %s\n",SWITCH13_IHM_NAME);		break;
						case SWITCH14_IHM:			print(*string, *len, "| %s\n",SWITCH14_IHM_NAME);		break;
						case SWITCH15_IHM:			print(*string, *len, "| %s\n",SWITCH15_IHM_NAME);		break;
						case SWITCH16_IHM:			print(*string, *len, "| %s\n",SWITCH16_IHM_NAME);		break;
						case SWITCH17_IHM:			print(*string, *len, "| %s\n",SWITCH17_IHM_NAME);		break;
						case SWITCH18_IHM:			print(*string, *len, "| %s\n",SWITCH18_IHM_NAME);		break;
						default:					print(*string, *len, "| SWITCH %d\n",i-3);	// -3 car biroute, color, lcd avant
					}
				}
			}
			break;}
		case IHM_POWER:
			switch(msg->data[0]){
				case BATTERY_OFF:				print(*string, *len, "| BATTERY_OFF\n");			break;
				case BATTERY_LOW:				print(*string, *len, "| BATTERY_LOW\n");			break;
				case ARU_ENABLE:				print(*string, *len, "| POWER_DISABLE\n");			break;
				case ARU_DISABLE:				print(*string, *len, "| POWER_ENABLE\n");			break;
				case HOKUYO_POWER_FAIL:			print(*string, *len, "| HOKUYO_POWER_FAIL\n");		break;
				default:																			break;
			}
			break;
		case IHM_SET_LED:{
			Uint8 i;
			// 5 bits de poids faible id : 0b...XXXXX
			// 3 bits de poids fort blink (clignotement) ou couleur pour LED_COLOR_IHM : 0bXXX.....
			for(i = 0; i < msg->size; i++)
			{
				print(*string, *len, "| led%d:",msg->data[i]&0x1F);
				switch(msg->data[i]>>5)
				{
					case OFF:				print(*string, *len, "OFF");	break;
					case ON:				print(*string, *len, "ON");		break;
					case BLINK_1HZ:			print(*string, *len, "BLINK");	break;
					case SPEED_BLINK_4HZ:	print(*string, *len, "SPEED");	break;
					case FLASH_BLINK_10MS:	print(*string, *len, "FLASH");	break;
				}
			}
			print(*string, *len, " \n");
			break;}

		default:
			break;
	}
}
