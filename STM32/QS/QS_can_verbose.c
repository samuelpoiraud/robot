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
#include "QS_maths.h"
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
	"DEBUG"
};

static void print_ir_result(CAN_msg_t * msg, char ** string, int * len);
static void print_ihm_result(CAN_msg_t * msg, char ** string, int * len);
static Uint16 QS_CAN_VERBOSE_can_msg_sprint(CAN_msg_t * msg, char * string, int len, QS_VERBOSE_msg_type_e verbose_msg_type);

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
		case BROADCAST_PAUSE_ALL:						print(string, len, "%.3x BROADCAST_PAUSE_ALL                    ", BROADCAST_PAUSE_ALL							);	break;
		case BROADCAST_RESUME_ALL:						print(string, len, "%.3x BROADCAST_RESUME_ALL                   ", BROADCAST_RESUME_ALL							);	break;
		case BROADCAST_I_AM_AND_I_AM_HERE:				print(string, len, "%.3x BROADCAST_I_AM_AND_I_AM_HERE           ", BROADCAST_I_AM_AND_I_AM_HERE					);	break;



		case DEBUG_FOE_POS:								print(string, len, "%x DEBUG_FOE_POS                          ", DEBUG_FOE_POS									);	break;
		case DEBUG_PROPULSION_MAILBOX_IN_IS_FULL:		print(string, len, "%x DEBUG_PROPULSION_MAILBOX_IN_IS_FULL    ", DEBUG_PROPULSION_MAILBOX_IN_IS_FULL			);  break;
		case DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL:		print(string, len, "%x DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL   ", DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL			);  break;
		case DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT:	print(string, len, "%x DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT", DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT		);  break;
		case DEBUG_PROPULSION_SET_COEF:					print(string, len, "%x DEBUG_PROPULSION_SET_COEF              ", DEBUG_PROPULSION_SET_COEF						);	break;
		case DEBUG_PROPULSION_SET_ACCELERATION:			print(string, len, "%x DEBUG_PROPULSION_SET_ACCELERATION      ", DEBUG_PROPULSION_SET_ACCELERATION				);	break;
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
		case IHM_DO_SELFTEST:							print(string, len, "%x IHM_DO_SELFTEST                        ", IHM_DO_SELFTEST								);	break;
		case STRAT_ACT_SELFTEST_DONE:					print(string, len, "%x STRAT_ACT_SELFTEST_DONE                ", STRAT_ACT_SELFTEST_DONE						);	break;
		case STRAT_PROP_SELFTEST_DONE:					print(string, len, "%x STRAT_PROP_SELFTEST_DONE               ", STRAT_PROP_SELFTEST_DONE						);	break;
		case STRAT_IHM_SELFTEST_DONE:					print(string, len, "%x STRAT_IHM_SELFTEST_DONE                ", STRAT_IHM_SELFTEST_DONE						);	break;
		case STRAT_BEACON_SELFTEST_DONE:				print(string, len, "%x STRAT_BEACON_SELFTEST_DONE             ", STRAT_BEACON_SELFTEST_DONE						);	break;
		case ACT_PING:									print(string, len, "%x ACT_PING                               ", ACT_PING										);	break;
		case PROP_PING:									print(string, len, "%x PROP_PING                              ", PROP_PING										);	break;
		case BEACON_PING:								print(string, len, "%x BEACON_PING                            ", BEACON_PING									);	break;
		case IHM_PING:									print(string, len, "%x IHM_PING                               ", IHM_PING										);	break;
		case STRAT_ACT_PONG:							print(string, len, "%x STRAT_ACT_PONG                         ", STRAT_ACT_PONG									);	break;
		case STRAT_PROP_PONG:							print(string, len, "%x STRAT_PROP_PONG                        ", STRAT_PROP_PONG								);	break;
		case STRAT_BEACON_PONG:							print(string, len, "%x STRAT_BEACON_PONG                      ", STRAT_BEACON_PONG								);	break;
		case STRAT_IHM_PONG:							print(string, len, "%x STRAT_IHM_PONG                         ", STRAT_IHM_PONG									);	break;



		case PROP_ROBOT_CALIBRE:						print(string, len, "%x PROP_ROBOT_CALIBRE                     ", PROP_ROBOT_CALIBRE								);	break;
		case PROP_GO_ANGLE:								print(string, len, "%x PROP_GO_ANGLE                          ", PROP_GO_ANGLE									);	break;
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


// Black
		//case ACT_POP_COLLECT_LEFT:						print(string, len, "%x ACT_POP_COLLECT_LEFT                   ", ACT_POP_COLLECT_LEFT							);	break;
// Pearl

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
		case BROADCAST_START:					print(string, len, "| match N�%d  %.2d:%.2d:%.2d %.2d/%.2d/20%.2d\n", msg->data.broadcast_start.matchId, msg->data.broadcast_start.heure, msg->data.broadcast_start.minute, msg->data.broadcast_start.seconde, msg->data.broadcast_start.jour, msg->data.broadcast_start.mois, msg->data.broadcast_start.annee);	break;
		case BROADCAST_ALIM:				    print(string, len, "| �tat : %s\n", (msg->data.broadcast_alim.state)?"ALIM_ON":"ALIM_OFF");		break;
		case BROADCAST_COULEUR:					print(string, len, "| CouleurEst %s\n", (msg->data.broadcast_couleur.color == GREEN)?"VERT":"VIOLET"	);		break;
		case BROADCAST_POSITION_ROBOT:			print(string, len, "| JeSuisEn  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", msg->data.broadcast_position_robot.x, msg->data.broadcast_position_robot.y, msg->data.broadcast_position_robot.angle, RAD_TO_DEG(msg->data.broadcast_position_robot.angle), (Uint16)(msg->data.broadcast_position_robot.speed_trans)*250, msg->data.broadcast_position_robot.speed_rot, msg->data.broadcast_position_robot.reason ,msg->data.broadcast_position_robot.error);								break;

		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:		if(msg->data.broadcast_beacon_adversary_position_ir.adv[0].error || msg->data.broadcast_beacon_adversary_position_ir.adv[1].error)
																print(string, len, "ERRs:0x%2x 0x%2x|",msg->data.broadcast_beacon_adversary_position_ir.adv[0].error, msg->data.broadcast_beacon_adversary_position_ir.adv[1].error);
															print(string, len, "angleR1=%3d |dR1=%3dcm |angleR2=%3d |dR2=%3dcm \n", RAD_TO_DEG(msg->data.broadcast_beacon_adversary_position_ir.adv[0].angle), msg->data.broadcast_beacon_adversary_position_ir.adv[0].dist, RAD_TO_DEG(msg->data.broadcast_beacon_adversary_position_ir.adv[1].angle), msg->data.broadcast_beacon_adversary_position_ir.adv[1].dist);	break;

		case BROADCAST_I_AM_AND_I_AM_HERE:			print(string, len, "| Code %s sur emplacement %s !!!\n", (msg->data.broadcast_i_am_and_i_am_where.code_id == CODE_STRAT)?"Strat":((msg->data.broadcast_i_am_and_i_am_where.code_id == CODE_PROP)?"Prop":"Act"),  (msg->data.broadcast_i_am_and_i_am_where.slot_id == SLOT_STRAT)?"Strat":((msg->data.broadcast_i_am_and_i_am_where.slot_id == SLOT_PROP)?"Prop":((msg->data.broadcast_i_am_and_i_am_where.slot_id == SLOT_ACT)?"Act":"Inconnu"))); break;

		case ACT_RESULT:
			print(string,len, "| act0x%x : ", msg->data.act_result.sid);
			switch(msg->data.act_result.sid)
			{
				// EXEMPLE: case (Uint8)ACT_POP_COLLECT_LEFT:		print(string,len, "ACT_POP_COLLECT_LEFT ");		break;
				default:								print(string,len, "UNKNOW ACT -> complete verbose !");		break;
			}


			  switch(msg->data.act_result.sid){
				/*EXEMPLE
				case (Uint8)ACT_POP_COLLECT_LEFT:
					switch(msg->data.act_result.cmd){
						case (Uint8)ACT_POP_COLLECT_LEFT_CLOSED:	print(string,len, "| ACT_POP_COLLECT_LEFT_CLOSED |");	break;
						case (Uint8)ACT_POP_COLLECT_LEFT_OPEN:	print(string,len, "| ACT_POP_COLLECT_LEFT_OPEN |");			break;
						case (Uint8)ACT_POP_COLLECT_LEFT_MID:	print(string,len, "| ACT_POP_COLLECT_LEFT_MID |");			break;
						case (Uint8)ACT_POP_COLLECT_LEFT_STOP:	print(string,len, "| ACT_POP_COLLECT_LEFT_STOP |");			break;
					}
				break;
				*/
			}

			//Todo ajouter la commande dont on envoie le r�sultat (data 1) ???
			switch(msg->data.act_result.result)
			{
				case ACT_RESULT_DONE:			print(string,len, "| DONE | ");			break;
				case ACT_RESULT_FAILED:			print(string,len, "| FAILED | ");		break;
				case ACT_RESULT_NOT_HANDLED:	print(string,len, "| NOT_HANDLED | ");	break;
				default:						print(string,len, "| UNKNOW status | ");		break;
			}

			switch(msg->data.act_result.error_code)
			{
				case ACT_RESULT_ERROR_OK:			if(msg->data.act_result.result != ACT_RESULT_DONE)	print(string,len, "NO ERROR ");		break;
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
		case DEBUG_PROPULSION_SET_COEF:			print(string, len, "| COEF_ID=%d  VALUE=%ld\n", msg->data.debug_propulsion_set_coef.id, msg->data.debug_propulsion_set_coef.value);	break;
		case DEBUG_PROPULSION_SET_ACCELERATION:	print(string, len, "| Acc=%d\n", msg->data.debug_propulsion_set_acceleration.acceleration_coef);								break;
		case IR_ERROR_RESULT:					print_ir_result(msg, &string, &len);									break;

		case DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT :	print(string, len, "| duration : %dms  section : %s\n", msg->data.debug_propulsion_erreur_recouvrement_it.duration, it_state_name[msg->data.debug_propulsion_erreur_recouvrement_it.id_it_state_name]);										break;

		case STRAT_TRAJ_FINIE:					print(string, len, "| J'arrive  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", msg->data.strat_traj_finie.x, msg->data.strat_traj_finie.y, msg->data.strat_traj_finie.angle, RAD_TO_DEG(msg->data.strat_traj_finie.angle), (Uint16)(msg->data.strat_traj_finie.speed_trans)*250, msg->data.strat_traj_finie.speed_rot, msg->data.strat_traj_finie.reason ,msg->data.strat_traj_finie.error);								break;
		case STRAT_PROP_ERREUR:					print(string, len, "| J'erreur  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", msg->data.strat_robot_freine.x, msg->data.strat_robot_freine.y, msg->data.strat_robot_freine.angle, RAD_TO_DEG(msg->data.strat_robot_freine.angle), (Uint16)(msg->data.strat_robot_freine.speed_trans)*250, msg->data.strat_robot_freine.speed_rot, msg->data.strat_robot_freine.reason ,msg->data.strat_robot_freine.error);								break;
		case STRAT_ROBOT_FREINE:				print(string, len, "| J'freine  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", msg->data.strat_prop_erreur.x, msg->data.strat_prop_erreur.y, msg->data.strat_prop_erreur.angle, RAD_TO_DEG(msg->data.strat_prop_erreur.angle), (Uint16)(msg->data.strat_prop_erreur.speed_trans)*250, msg->data.strat_prop_erreur.speed_rot, msg->data.strat_prop_erreur.reason ,msg->data.strat_prop_erreur.error);								break;
		case STRAT_PROP_FOE_DETECTED:			print(string, len, "| x : %d   y : %d   dist : %d   angle : %d   %s   %s\n", msg->data.strat_prop_foe_detected.x, msg->data.strat_prop_foe_detected.y, msg->data.strat_prop_foe_detected.dist, msg->data.strat_prop_foe_detected.angle,(msg->data.strat_prop_foe_detected.hokuyo_detection)?"hokuyo":"balise", (msg->data.strat_prop_foe_detected.timeout)?"avec timeout":"");	break;
		case STRAT_SEND_REPORT:					print(string, len, "| Distance : %d | Rotation : %u� | Rotation max : %d�\n", msg->data.strat_send_report.actual_trans, RAD_TO_DEG(msg->data.strat_send_report.actual_rot), RAD_TO_DEG(msg->data.strat_send_report.max_rot));	break;

		case PROP_ROBOT_CALIBRE:				print(string, len, "| J'calibre  x=%4d y=%4d t=0x%8x=%3d� Vt=%4dmm/s Vr=%2drd/s reas=0x%2x st=0x%2x\n", msg->data.prop_robot_calibre.x, msg->data.prop_robot_calibre.y, msg->data.prop_robot_calibre.angle, RAD_TO_DEG(msg->data.prop_robot_calibre.angle), (Uint16)(msg->data.prop_robot_calibre.speed_trans)*250, msg->data.prop_robot_calibre.speed_rot, msg->data.prop_robot_calibre.reason ,msg->data.prop_robot_calibre.error);								break;
		case PROP_GO_ANGLE:						print(string, len, "| VaAngle   teta=%d=%d� %s %s %s %d%s %s\n", msg->data.prop_go_angle.teta, RAD_TO_DEG(msg->data.prop_go_angle.teta), (msg->data.prop_go_angle.multipoint == PROP_MULTIPOINT)?"multi":" ", (msg->data.prop_go_angle.buffer_mode == PROP_NOW)?"now":"pas_now", (msg->data.prop_go_angle.referential == PROP_RELATIVE)?"relatif":" ", msg->data.prop_go_angle.speed,(msg->data.prop_go_angle.speed == FAST)?"=rapide":((msg->data.prop_go_angle.speed == SLOW)?"=lente":""), (msg->data.prop_go_angle.way == FORWARD)?"avant":((msg->data.prop_go_angle.way == BACKWARD)?"arri�re":"any_way")	);						break;
		case PROP_OFFSET_AVOID:					print(string, len, "| Xleft : %d  Xright : %d  Yfront : %d  Yback : %d\n", msg->data.prop_offset_avoid.x_left, msg->data.prop_offset_avoid.x_right, msg->data.prop_offset_avoid.y_front, msg->data.prop_offset_avoid.y_back);		break;
		case PROP_GO_POSITION:					print(string, len, "| VaPos     x=%d y=%d %s %s %s vitesse %d%s %s %s\n", msg->data.prop_go_position.x, msg->data.prop_go_position.y, (msg->data.prop_go_position.multipoint == PROP_MULTIPOINT)?"multi":" ", (msg->data.prop_go_position.buffer_mode == PROP_NOW)?"now":"pas_now", (msg->data.prop_go_position.referential == PROP_RELATIVE)?"relatif":" ", msg->data.prop_go_position.speed,(msg->data.prop_go_position.speed == FAST)?"=rapide":((msg->data.prop_go_position.speed == SLOW)?"=lente":""), (msg->data.prop_go_position.way == FORWARD)?"avant":((msg->data.prop_go_position.way == BACKWARD)?"arri�re":"any_way"), ((msg->data.prop_go_position.border_mode == PROP_BORDER_MODE)?"mode bordure":""));	break;
		case PROP_SET_POSITION:					print(string, len, "| PrendPos  X=%d | Y=%d | teta=0x%x=%d�\n", msg->data.prop_set_position.x, msg->data.prop_set_position.y, RAD_TO_DEG(msg->data.prop_set_position.teta), msg->data.prop_set_position.teta);													break;
		case PROP_SEND_PERIODICALLY_POSITION: 	print(string, len, "| DitPos    p�riode=%d | translation = %dmm | rotation = %d�\n", msg->data.prop_send_periodically_position.period, msg->data.prop_send_periodically_position.translation, msg->data.prop_send_periodically_position.rotation);										break;
		case PROP_RUSH_IN_THE_WALL:				print(string, len, "| sens %s | asservissement rotation %sactiv�e\n", (msg->data.prop_rush_in_the_wall.way == BACKWARD)?"arri�re":((msg->data.prop_rush_in_the_wall.way == FORWARD)?"avant":"quelconque"), (msg->data.prop_rush_in_the_wall.asser_rot)?"":"d�s");				break;
		case PROP_WARN_ANGLE:					if(msg->data.prop_warn_angle.angle) print(string, len, "| avertisseur en teta=0x%d=%d�\n", msg->data.prop_warn_angle.angle, RAD_TO_DEG(msg->data.prop_warn_angle.angle)); else print(string, len, "d�sactivation de l'avertisseur en angle\n");	break;
		case PROP_WARN_X:						if(msg->data.prop_warn_x.x) print(string, len, "| avertisseur en x=%d\n", msg->data.prop_warn_x.x);                                  else print(string, len, "d�sactivation de l'avertisseur en X\n");		break;
		case PROP_WARN_Y:						if(msg->data.prop_warn_y.y) print(string, len, "| avertisseur en y=%d\n", msg->data.prop_warn_y.y);                                  else print(string, len, "d�sactivation de l'avertisseur en Y\n");		break;
		case PROP_WARN_DISTANCE:                if(msg->data.prop_warn_distance.distance) print(string, len, "| avertisseur au point x=%d  y=%d  distance=%d\n", msg->data.prop_warn_distance.x, msg->data.prop_warn_distance.y, msg->data.prop_warn_distance.distance);            else print(string, len, "d�sactivation de l'avertisseur en distance\n");		break;
		case DEBUG_PROP_MOVE_POSITION:			print(string, len, "| offset d'aleration x : %d    y : %d    teta : %d\n", msg->data.debug_prop_move_position.xOffset, msg->data.debug_prop_move_position.yOffset, msg->data.debug_prop_move_position.tetaOffset);	break;

		case IHM_BUTTON:						print_ihm_result(msg, &string, &len);			break;
		case IHM_SWITCH:						print_ihm_result(msg, &string, &len);			break;
		case IHM_GET_SWITCH:					print_ihm_result(msg, &string, &len);			break;
		case IHM_POWER:							print_ihm_result(msg, &string, &len);			break;
		case IHM_SET_LED:						print_ihm_result(msg, &string, &len);			break;
		case IHM_SET_ERROR:						print_ihm_result(msg, &string, &len);			break;


		default:								print(string, len, "|\n");						break;
	}
	return string - ret;
}

static void print_ir_result(CAN_msg_t * msg, char ** string, int * len){
	print(*string, *len, "\n_____Infos erreurs balise(s) infrarouge____\n");
	print(*string, *len, "%d erreurs de type 0 : AUCUNE_ERREUR\n", msg->data.ir_error_result.error_counter[0]);
	print(*string, *len, "%d erreurs de type 1 : ERREUR_PAS_DE_SYNCHRO\n", msg->data.ir_error_result.error_counter[1]);
	print(*string, *len, "%d erreurs de type 2 : ERREUR_SIGNAL_INSUFFISANT\n", msg->data.ir_error_result.error_counter[2]);
	print(*string, *len, "%d erreurs de type 3 : ERREUR_SIGNAL_SATURE\n", msg->data.ir_error_result.error_counter[3]);
	print(*string, *len, "%d erreurs de type 4 : ERREUR_TROP_PROCHE\n", msg->data.ir_error_result.error_counter[4]);
	print(*string, *len, "%d erreurs de type 5 : ERREUR_TROP_LOIN\n", msg->data.ir_error_result.error_counter[5]);
	print(*string, *len, "%d erreurs de type 6 : ERROR_OBSOLESCENCE\n", msg->data.ir_error_result.error_counter[6]);
}

static void print_ihm_result(CAN_msg_t * msg, char ** string, int * len){
	switch (msg->sid) {
		case IHM_SET_ERROR:
			print(*string, *len, "error type : %d     state : %s\n", msg->data.ihm_set_error.error, (msg->data.ihm_set_error.state)?"set":"reset");
			break;
		case IHM_BUTTON:
			switch(msg->data.ihm_button.id){
				case BP_GO_TO_HOME_IHM:				print(*string, *len, "| bp_go_to_home");		break;
				case BP_CALIBRATION_IHM:			print(*string, *len, "| bp_calibration");		break;
				case BP_PRINTMATCH_IHM:				print(*string, *len, "| bp_print_match");		break;
				case BP_OK_IHM:						print(*string, *len, "| bp_ok");				break;
				case BP_UP_IHM:						print(*string, *len, "| bp_up");				break;
				case BP_DOWN_IHM:					print(*string, *len, "| bp_down");				break;
				case BP_SET_IHM:					print(*string, *len, "| bp_set");				break;
				case BP_SUSPEND_RESUME_MATCH_IHM:	print(*string, *len, "| bp_suspende_resume");	break;
				default:					print(*string, *len, "| Button %d active", msg->data.ihm_button.id);
			}
			print(*string, *len, ", %s\n",(msg->data.ihm_button.long_push)?"long push" : "direct push");
			break;
		case IHM_SWITCH:{
			Uint8 i;
			for(i = 0; i < msg->size; i++){
				switch(msg->data.ihm_switch.switchs[i].id){
					case BIROUTE_IHM:			print(*string, *len, "| sw_biroute");					break;
					case SWITCH_COLOR_IHM:		print(*string, *len, "| sw_color = %s\n",(msg->data.ihm_switch.switchs[i].state)? "VERT":"JAUNE");				break;
					case SWITCH_LCD_IHM:		print(*string, *len, "| sw_lcd");						break;
					case SWITCH0_IHM:			print(*string, *len, "| %s",SWITCH0_IHM_NAME);		break;
					case SWITCH1_IHM:			print(*string, *len, "| %s",SWITCH1_IHM_NAME);		break;
					case SWITCH2_IHM:			print(*string, *len, "| %s",SWITCH2_IHM_NAME);		break;
					case SWITCH3_IHM:			print(*string, *len, "| %s",SWITCH3_IHM_NAME);		break;
					case SWITCH4_IHM:			print(*string, *len, "| %s",SWITCH4_IHM_NAME);		break;
					case SWITCH5_IHM:			print(*string, *len, "| %s",SWITCH5_IHM_NAME);		break;
					case SWITCH6_IHM:			print(*string, *len, "| %s",SWITCH6_IHM_NAME);		break;
					case SWITCH7_IHM:			print(*string, *len, "| %s",SWITCH7_IHM_NAME);		break;
					case SWITCH8_IHM:			print(*string, *len, "| %s",SWITCH8_IHM_NAME);		break;
					case SWITCH9_IHM:			print(*string, *len, "| %s",SWITCH9_IHM_NAME);		break;
					case SWITCH10_IHM:			print(*string, *len, "| %s",SWITCH10_IHM_NAME);		break;
					case SWITCH11_IHM:			print(*string, *len, "| %s",SWITCH11_IHM_NAME);		break;
					case SWITCH12_IHM:			print(*string, *len, "| %s",SWITCH12_IHM_NAME);		break;
					case SWITCH13_IHM:			print(*string, *len, "| %s",SWITCH13_IHM_NAME);		break;
					case SWITCH14_IHM:			print(*string, *len, "| %s",SWITCH14_IHM_NAME);		break;
					case SWITCH15_IHM:			print(*string, *len, "| %s",SWITCH15_IHM_NAME);		break;
					case SWITCH16_IHM:			print(*string, *len, "| %s",SWITCH16_IHM_NAME);		break;
					case SWITCH17_IHM:			print(*string, *len, "| %s",SWITCH17_IHM_NAME);		break;
					case SWITCH18_IHM:			print(*string, *len, "| %s",SWITCH18_IHM_NAME);		break;
					default:					print(*string, *len, "| SWITCH %d", msg->data.ihm_switch.switchs[i].id-3);	// -3 car biroute, color, lcd avant
				}
				if(msg->data.ihm_switch.switchs[i].id != SWITCH_COLOR_IHM)
					print(*string, *len, " = %s\n", (msg->data.ihm_switch.switchs[i].state)? "ON":"OFF");
			}
		}break;
		case IHM_GET_SWITCH:{
			Uint8 i;
			if(msg->size == 0)
				print(*string, *len,"| ALL SWITCHS\n");
			else
			{
				print(*string, *len, "| ");
				for(i = 0; i < msg->size; i++)
				{
					switch (msg->data.ihm_get_switch.id[i]) {
						case BIROUTE_IHM:			print(*string, *len, "sw_biroute ");					break;
						case SWITCH_COLOR_IHM:		print(*string, *len, "sw_color ");					break;
						case SWITCH_LCD_IHM:		print(*string, *len, "sw_lcd ");						break;
						case SWITCH0_IHM:			print(*string, *len, "%s ",SWITCH0_IHM_NAME);		break;
						case SWITCH1_IHM:			print(*string, *len, "%s ",SWITCH1_IHM_NAME);		break;
						case SWITCH2_IHM:			print(*string, *len, "%s ",SWITCH2_IHM_NAME);		break;
						case SWITCH3_IHM:			print(*string, *len, "%s ",SWITCH3_IHM_NAME);		break;
						case SWITCH4_IHM:			print(*string, *len, "%s ",SWITCH4_IHM_NAME);		break;
						case SWITCH5_IHM:			print(*string, *len, "%s ",SWITCH5_IHM_NAME);		break;
						case SWITCH6_IHM:			print(*string, *len, "%s ",SWITCH6_IHM_NAME);		break;
						case SWITCH7_IHM:			print(*string, *len, "%s ",SWITCH7_IHM_NAME);		break;
						case SWITCH8_IHM:			print(*string, *len, "%s ",SWITCH8_IHM_NAME);		break;
						case SWITCH9_IHM:			print(*string, *len, "%s ",SWITCH9_IHM_NAME);		break;
						case SWITCH10_IHM:			print(*string, *len, "%s ",SWITCH10_IHM_NAME);		break;
						case SWITCH11_IHM:			print(*string, *len, "%s ",SWITCH11_IHM_NAME);		break;
						case SWITCH12_IHM:			print(*string, *len, "%s ",SWITCH12_IHM_NAME);		break;
						case SWITCH13_IHM:			print(*string, *len, "%s ",SWITCH13_IHM_NAME);		break;
						case SWITCH14_IHM:			print(*string, *len, "%s ",SWITCH14_IHM_NAME);		break;
						case SWITCH15_IHM:			print(*string, *len, "%s ",SWITCH15_IHM_NAME);		break;
						case SWITCH16_IHM:			print(*string, *len, "%s ",SWITCH16_IHM_NAME);		break;
						case SWITCH17_IHM:			print(*string, *len, "%s ",SWITCH17_IHM_NAME);		break;
						case SWITCH18_IHM:			print(*string, *len, "%s ",SWITCH18_IHM_NAME);		break;
						default:					print(*string, *len, "SWITCH %d ",msg->data.ihm_get_switch.id[i]-3);	// -3 car biroute, color, lcd avant
					}
				}
				print(*string, *len, "\n");
			}
			break;}
		case IHM_POWER:
			switch(msg->data.ihm_power.state){
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
			for(i = 0; i < msg->size; i++)
			{
				print(*string, *len, "| led%d:", msg->data.ihm_set_led.led[i].id);
				switch(msg->data.ihm_set_led.led[i].blink)
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
