/*
 *  Club Robot ESEO 2008 - 2015
 *  Archi-Tech' - Holly & Wood
 *
 *  Fichier : QS_CANmsgList.h
 *  Package : Qualité Soft
 *  Description : Structure des messages CAN
 *  Auteur : Jacen refactoring Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20101216
 */

#ifndef QS_CANMSGLIST_H
	#define QS_CANMSGLIST_H

	#include "QS_types.h"
	#include "QS_who_am_i.h"

	/* Masque des cartes (des destinataires) */
	#define MASK_BITS					0x700
	#define BROADCAST_FILTER			0x000
	#define PROP_FILTER					0x100
	#define STRAT_FILTER				0x200
	#define ACT_FILTER					0x300
	#define BALISE_FILTER				0x400
	#define MOSFET_FILTER				0x500
	#define IHM_FILTER					0x600
	#define DEBUG_FILTER				0x700

	//Ces messages ne sont pas destinés à voyager sur les bus CAN des robot.
	#define XBEE_FILTER					0x500

	#define SIZE_PROP_TRAJ_MSG			8
	typedef struct{
		Sint32 x						:13;		// [mm]
		Sint32 y						:13;		// [mm]
		Uint8 idTraj					:6;
		Sint16 angle					:16;		// [PI4096]
		prop_warning_reason_e reason	:8;
		SUPERVISOR_error_source_e error	:3;
		way_e way						:2;
		bool_e in_translation			:1;
		bool_e in_rotation				:1;
	}prop_traj_msg_t;

	typedef union{

		// Ne pas toucher
		Uint8 raw_data[8];		// Données brutes

		/**********************************************************************************************************************
		 **********************************************************************************************************************
		 **													Message pour tous												 **
		 **********************************************************************************************************************
		 *********************************************************************************************************************/

		#define BROADCAST_START				0x001
		#define SIZE_BROADCAST_START		7
			struct{
				Uint16 matchId				:16;	// ID du match
				Uint32 seconde				:6;
				Uint32 minute				:6;
				Uint32 heure				:4;
				Uint32 jour					:5;
				Uint32 mois					:4;
				Uint32 annee				:7;		// 20xx (xx = annee)
				Uint32 journee				:3;
			}broadcast_start;

		#define BROADCAST_STOP_ALL			0x002

		#define BROADCAST_I_AM_AND_I_AM_HERE	0x009
		#define SIZE_BROADCAST_I_AM_AND_I_AM_HERE	1
			struct{
				slot_id_e slot_id				:4;
				code_id_e code_id				:4;
			}broadcast_i_am_and_i_am_where;

		#define BROADCAST_COULEUR			0x003
		#define SIZE_BROADCAST_COULEUR		1
			struct{
				color_e color				:1;
			}broadcast_couleur;

		#define BROADCAST_POSITION_ROBOT		0x004
		#define SIZE_BROADCAST_POSITION_ROBOT	SIZE_PROP_TRAJ_MSG
			prop_traj_msg_t broadcast_position_robot;

		/*
		 * Envoyé par la stratégie et provoque la réinitialisation du fond de panier complet
		 */
		#define BROADCAST_RESET				0x0FF


		/*
		 * Message envoyé lors d'un changement d'état de l'alimentation 24V
		 * state peut prendre des valeurs de type "alim_state_e" (possibilité de passer plusieurs infos en même temps avec un OU logique)
		 */
		#define BROADCAST_ALIM				0x005
		#define SIZE_BROADCAST_ALIM			4
			struct{
				Uint16 battery_value		:16;		// [mV]
				Uint16 state				:16;        // Peut prendre des valeurs de type "alim_state_e"
			}broadcast_alim;


		/*
		 * Procédure et messages échangés pour la localisation de l'adversaire.
		 * 1- la carte propulsion localise les adversaires avec l'hokuyo
		 * 2- la carte balise IR envoie ses infos à la carte propulsion
		 * 3- la carte propulsion analyse la correspondance pour comprendre qui est qui (2 adv + friend) + fusion de données et calcul de la fiabilité.
		 * 4- envoi des positions adverses à la stratégie (x, y, dist, teta) + fiabilité pour chaque donnée
		 *
		 * Possibilité de forcer en débog les positions adverses en envoyant le même message de position BROADCAST_ADVERSARIES_POSITION
		 */
		#define BROADCAST_BEACON_ADVERSARY_POSITION_IR	0x006
		#define SIZE_BROADCAST_BEACON_ADVERSARY_POSITION_IR	8
			struct{
				struct{
					Uint16 angle			:16;		// [PI4096]
					Uint8 dist				:8;			// [2 cm]
					beacon_ir_error_e error	:8;
				}adv[2];
			}broadcast_beacon_adversary_position_ir;

		#define BROADCAST_ADVERSARIES_POSITION			0x099
		#define SIZE_BROADCAST_ADVERSARIES_POSITION		7
			struct{
				Uint16 teta					:16;
				Uint8 adversary_number		:7;
				bool_e last_adversary		:1;			// Bit levé si l'adversaire envoyé est le dernier...
				Uint8 x						:8;			// [2 cm]
				Uint8 y						:8;			// [2 cm]
				Uint8 dist					:8;			// [2 cm]
				adversary_detection_fiability_e fiability :8;
			}broadcast_adversaries_position;

		#define STRAT_BUZZER_PLAY			0x2FF
		#define SIZE_STRAT_BUZZER_PLAY		4
			struct{
				Uint16 duration				:16;		// [ms]
				buzzer_play_note_e note		:8;
				Uint8 nb_bip				:8;
			}strat_buffer_play;



		/**********************************************************************************************************************
		 **********************************************************************************************************************
		 **												Message IHM pour tous												 **
		 **********************************************************************************************************************
		 *********************************************************************************************************************/


		/*
		 * Envois un message CAN quand un bouton est déplacé
		 */
		#define IHM_BUTTON					0x060
		#define SIZE_IHM_BUTTON				2
			struct{	//
				button_ihm_e id				:8;
				bool_e long_push			:1; //renvoie 1 si c'est un appuie long ou 0 sinon
			}ihm_button;

		/*
		 * Envois un message CAN quand un switch est déplacer
		 * Chaque data correspond à un switch avec son état
		 * Pour savoir, combien de switch ont été envoyé regarder msg.size
		 */
		#define IHM_SWITCH					0x061
			struct{
				struct{
					switch_ihm_e id			:7;
					bool_e state			:1;
				}switchs[8];
			}ihm_switch; // 1 octets * N_switchs

		/*
		 * Envoie l'états de tous les switchs sur 4 data
		 * Le bit de points faible (soit le bit de points faible de la data[3]) correspond à SW_COLOR de switch_ihm_e
		 */
		#define IHM_SWITCH_ALL				0x062
		#define SIZE_IHM_SWITCH_ALL			4
			struct{
				Uint32 switch_mask			:32;
			}ihm_switch_all;

		/*
		 * La biroute vient d'être retirée, let's go, c'est parti pour la purée !
		 */
		#define IHM_BIROUTE_IS_REMOVED		0x064

		/*
		 * Demande à la carte, IHM l'état de des switchs
		 * Dans chaque data mettre l'id du switch (type : switch_ihm_e) que vous souhaitez
		 * ASTUCE : si vous souhaitez recevoir tous les switchs, mettre 0 en size (msg.size = 0)
		 */
		#define IHM_GET_SWITCH				0x601
			struct{
				Uint8 id[8]; // Du type switch_ihm_e
			}ihm_get_switch;


		#define IHM_SET_LED					0x602
			struct{
				struct{
					led_ihm_e id			:5;
					blinkLED_e blink		:3;
				}led[8];
			}ihm_set_led;

		#define IHM_LCD_BIT_RESET			0x603
		#define SIZE_IHM_LCD_BIT_RESET		1
			struct{
				bool_e state_set			:1;
			}ihm_lcd_bit_reset;

		#define IHM_SET_ERROR				0x604
		#define SIZE_IHM_SET_ERROR			2
			struct{
				ihm_error_e error			:8;
				bool_e state				:1;
			}ihm_set_error;

		/*
		 * Impose la couleur de la LED color
		 */
		#define IHM_SET_LED_COLOR			0x605
		#define SIZE_IHM_SET_LED_COLOR		1
			struct{
				led_color_e led_color		:8;
			}ihm_set_led_color;


		/**********************************************************************************************************************
		 **********************************************************************************************************************
		 **										Carte Strat <-> XBEE <-> Carte Strat										 **
		 **********************************************************************************************************************
		 *********************************************************************************************************************/

		/*
		 * Ce message est envoyé pour lancer le match de l'autre robot
		 */
		#define XBEE_START_MATCH				0x5A4

		/*
		 * PING = pInG = p1n6, Ce message est envoyé pour pinger l'autre carte stratégie
		 */
		#define XBEE_PING						0x516
		#define SIZE_XBEE_PING					1
			struct{
				Uint8 module_id					:8;
			}xbee_ping;
		/*
		 * PONG = pOnG = p0n6, Ce message est envoyé en réponse d'un ping vers l'autre carte stratégie
		 */
		#define XBEE_PONG						0x506
		#define SIZE_XBEE_PONG					1
			struct{
				Uint8 module_id					:8;
			}xbee_pong;

		/*
		 * Envoie un message quand il sort de la zone de départ
		 */
		#define XBEE_REACH_POINT_GET_OUT_INIT	0x507

		/*
		 * Effectue une demande lié au zones (un SID pour toute la gestion des zones comme ça)
		 */
		#define XBEE_ZONE_COMMAND				0x5AA
		#define SIZE_XBEE_ZONE_COMMAND			3
			struct{
				xbee_zone_order_e order			:8;
				zone_id_e zone					:8;
				bool_e lock						:1;
			}xbee_zone_command;


		#define XBEE_MY_POSITION_IS				0x5FF
		#define SIZE_XBEE_MY_POSITION_IS		6
			struct{
				Sint16 x						:16;		// [mm]
				Sint16 y						:16;		// [mm]
				robot_id_e robot_id				:8;
				state_black_for_com_e state 	:8;
			}xbee_my_position_is;

		#define XBEE_COMMUNICATION_AVAILABLE	0x5B0
		#define XBEE_COMMUNICATION_RESPONSE		0x5B1	//Réponse...

		#define XBEE_SYNC_ELEMENTS_FLAGS		0x5B2
		#define SIZE_XBEE_SYNC_ELEMENTS_FLAGS	2
			struct{
				Uint8 flagId					:8;
				bool_e flag						:1;
			}xbee_sync_elements_flags;

		#define XBEE_ELEMENTS_HARDFLAGS			0x5B3
		#define SIZE_XBEE_ELEMENTS_HARDFLAGS	8
			struct{
				Uint8 flagId[8];
			}xbee_elements_hardflags;

		#define XBEE_SEND_COLOR					0x5B5
		#define SIZE_XBEE_SEND_COLOR			1
			struct{
				color_e color					:8;
			}xbee_send_color;

		#define XBEE_GET_COLOR					0x5B6
		#define SIZE_XBEE_GET_COLOR				1
			struct{
				color_e color					:8;
			}xbee_get_color;

		/**********************************************************************************************************************
		 **********************************************************************************************************************
		 **										Carte Strat <-> Balise Ext													 **
		 **********************************************************************************************************************
		 *********************************************************************************************************************/

		#define ENABLE_WATCHING_ZONE			0x5BE
		#define SIZE_ENABLE_WATCHING_ZONE		3
			struct{
				ZONE_zoneId_e zone				:8;
				robot_id_e robot				:8;
				ZONE_event_t event				:8;
			}enable_watching_zone;

		#define DISABLE_WATCHING_ZONES			0x5BD
		#define SIZE_DISABLE_WATCHING_ZONES		2
			struct{
				ZONE_zoneId_e zone				:8;
				robot_id_e robot				:8;
			}disable_watching_zone;

		#define GET_ZONE_INFOS					0x5B1
		#define SIZE_GET_ZONE_INFOS				2
			struct{
				ZONE_zoneId_e zone				:8;
				robot_id_e robot				:8;
			}get_zone_infos;

		#define STRAT_ZONE_INFOS				0x551
		#define SIZE_STRAT_ZONE_INFOS			7
			struct{
				ZONE_zoneId_e zone				:8;
				ZONE_event_t event				:8;
				time32_t min_detection_time		:8;
				Uint16 specific_param_x			:8;
				Uint16 specific_param_y			:8;
				time32_t presence_duration		:8;
				bool_e someone_is_here			:1;
			}strat_zone_infos;


		/**********************************************************************************************************************
		 **********************************************************************************************************************
		 **										Carte Strat <-> Autres (Selftest)											 **
		 **********************************************************************************************************************
		 *********************************************************************************************************************/

		#define ACT_DO_SELFTEST				0x300

		#define ACT_PING					0x380

		#define STRAT_ACT_PONG				0x226
		#define SIZE_STRAT_ACT_PONG			1
			struct{
				robot_id_e robot_id			:8;
			}strat_act_pong;

		#define STRAT_ACT_SELFTEST_DONE 	0x2E3
			struct{
				Uint8 error_code[8];		// Du type SELFTEST_error_code_e
			}strat_act_selftest_done;

		#define PROP_DO_SELFTEST			0x100

		#define PROP_PING					0x116

		#define STRAT_PROP_PONG				0x216
		#define SIZE_STRAT_PROP_PONG		1
			struct{
				robot_id_e robot_id;
			}strat_prop_pong;

		#define STRAT_PROP_SELFTEST_DONE 	0x2E1
			struct{
				Uint8 error_code[8];		// Du type SELFTEST_error_code_e
			}strat_prop_selftest_done;

		#define BEACON_DO_SELFTEST			0x400

		#define BEACON_PING					0x416

		#define STRAT_BEACON_PONG			0x246
		#define SIZE_STRAT_BEACON_PONG		1
			struct{
				robot_id_e robot_id;
			}strat_beacon_pong;

		#define STRAT_BEACON_SELFTEST_DONE	0x2E4
			struct{
				Uint8 error_code[8];		// Du type SELFTEST_error_code_e
			}strat_beacon_selftest_done;

		#define IHM_DO_SELFTEST				0x600

		#define IHM_PING					0x616

		#define STRAT_IHM_PONG				0x266
		#define SIZE_STRAT_IHM_PONG			1
			struct{
				robot_id_e robot_id;
			}strat_ihm_pong;

		#define STRAT_IHM_SELFTEST_DONE 	0x2E6
			struct{
				Uint8 error_code[8];		// Du type SELFTEST_error_code_e
			}strat_ihm_selftest_done;




	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **										Carte Strat <-> Carte Prop													 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

		#define STRAT_TRAJ_FINIE			0x210
		#define SIZE_STRAT_TRAJ_FINIE		SIZE_PROP_TRAJ_MSG
			prop_traj_msg_t strat_traj_finie;

		#define STRAT_PROP_ERREUR			0x211
		#define SIZE_STRAT_PROP_ERREUR		SIZE_PROP_TRAJ_MSG
			prop_traj_msg_t strat_prop_erreur;

		#define STRAT_ROBOT_FREINE			0x213
		#define SIZE_STRAT_ROBOT_FREINE		SIZE_PROP_TRAJ_MSG
			prop_traj_msg_t strat_robot_freine;

		#define PROP_ROBOT_CALIBRE			0x214
		#define SIZE_PROP_ROBOT_CALIBRE		SIZE_PROP_TRAJ_MSG
			prop_traj_msg_t prop_robot_calibre;


		#define STRAT_PROP_FOE_DETECTED		0x015
		#define SIZE_STRAT_PROP_FOE_DETECTED 8
			struct{
				Sint16 x					:16;		// [mm]
				Sint16 y					:16;		// [mm]
				Sint16 angle				:16;		// [PI4096]
				Uint8 dist					:8;			// [mm/20]
				bool_e timeout				:1;
				bool_e hokuyo_detection		:1;
				bool_e in_wait				:1;			// Si à 1 c'est juste une information d'évitement quand on est en évitement avec attente
			}strat_prop_foe_detected;

		#define STRAT_SEND_REPORT			0x217
		#define SIZE_STRAT_SEND_REPORT		6
			struct{
				Sint16 actual_rot			:16;		// [PI4096/8]
				Sint16 max_rot				:16;		// [PI4096/8]
				Sint16 actual_trans			:16;		// [mm/2]
			}strat_send_report;

		#define STRAT_INFORM_CAPTEUR		0x218
		#define SIZE_STRAT_INFORM_CAPTEUR	2
			struct{
				act_sensor_id_e sensor_id	:8;
				bool_e present				:1;
			}strat_inform_capteur;

		#define STRAT_BACK_SCAN				0x219
		#define SIZE_STRAT_BACK_SCAN		3
			struct{
				bool_e second_part			:1;
				bool_e total_dune			:1;
				bool_e wtf					:1;
				bool_e nothing				:1;
				Uint16 middle				:16;
			}strat_back_scan;

		#define STRAT_ROBOT_POSITION		0x21A
		#define SIZE_STRAT_ROBOT_POSITION	7
			struct{
				Sint16 x					:16;
				Sint16 y					:16;
				Sint16 teta					:16;
				bool_e error_scan			:1;
			}strat_robot_position;





		#define PROP_GO_POSITION			0x155
		#define SIZE_PROP_GO_POSITION		8
			struct{
				Sint16 x						:16;	// [mm]
				Sint16 y						:16;	// [mm]
				PROP_speed_e speed				:10;
				Uint8 idTraj					:6;
				way_e way						:2;
				avoidance_e avoidance			:4;
				prop_acknowledge_e acknowledge	:1;
				prop_border_mode_e border_mode	:1;
				prop_curve_e curve				:1;
				propEndCondition_e propEndCondition	:1;
				prop_buffer_mode_e buffer_mode	:1;
				prop_referential_e referential	:1;
			}prop_go_position;


		#define PROP_GO_ANGLE				0x177
		#define SIZE_PROP_GO_ANGLE			6
			struct{
				Sint16 teta						:16;	// [PI4096]
				PROP_speed_e speed				:16;
				Uint8 idTraj					:6;
				way_e way						:2;
				prop_acknowledge_e acknowledge	:1;
				propEndCondition_e propEndCondition	:1;
				prop_buffer_mode_e buffer_mode	:1;
				prop_referential_e referential	:1;
			}prop_go_angle;

		/*
		 * Si l'un des paramètres vaut 0, l'avertisseur correspondant est désactivé.
		 */
		#define PROP_SEND_PERIODICALLY_POSITION		0x188
		#define SIZE_PROP_SEND_PERIODICALLY_POSITION 6
			struct{
				Uint16 period				:16;		// [ms]			Période à laquelle on veut recevoir des messages de BROADCAST_POSITION
				Sint16 translation			:16;		// [mm]			Déplacement du robot au delà duquel on veut recevoir un BROADCAST_POSITION
				Sint16 rotation				:16;		// [rad4096]	Déplacement du robot au delà duquel on veut recevoir un BROADCAST_POSITION
			}prop_send_periodically_position;

		#define PROP_STOP					0x101

		#define PROP_TELL_POSITION			0x105

		#define	PROP_SET_POSITION			0x107
		#define SIZE_PROP_SET_POSITION		6
			struct{
				Sint16 x					:16;		// [mm]
				Sint16 y					:16;		// [mm]
				Sint16 teta					:16;		// [PI4096]
			}prop_set_position;

		#define PROP_RUSH_IN_THE_WALL		0x109
		#define SIZE_PROP_RUSH_IN_THE_WALL	6
			struct{
				Sint16 teta						:16;
				PROP_speed_e speed				:16;
				Uint8 idTraj					:6;
				way_e way						:2;
				bool_e asser_rot				:1;
			}prop_rush_in_the_wall;

		#define PROP_CALIBRATION			0x10B

		/*
		 * 0 pour demander un désarmement !!!
		 * ATTENTION, pas d'armement possible en 0, demandez 1[rad/4096], c'est pas si loin.
		 */
		#define PROP_WARN_ANGLE				0x10C
		#define SIZE_PROP_WARN_ANGLE		2
			struct{
				Sint16 angle				:16;		// [PI4096]
			}prop_warn_angle;

		/*
		 * 0 pour demander un désarmement !!!
		 * ATTENTION, pas d'armement possible en 0
		 */
		#define PROP_WARN_X					0x10D
		#define SIZE_PROP_WARN_X			2
			struct{
				Sint16 x					:16;		// [mm]
			}prop_warn_x;

		/*
		 * 0 pour demander un désarmement !!!
		 * ATTENTION, pas d'armement possible en 0
		 */
		#define PROP_WARN_Y					0x10E
		#define SIZE_PROP_WARN_Y			2
			struct{
				Sint16 y					:16;		// [mm]
			}prop_warn_y;

		/*
		 * 0 pour demander un désarmement !!!
		 * ATTENTION, pas d'armement possible en 0
		 */
		#define PROP_WARN_DISTANCE			0x10F
		#define SIZE_PROP_WARN_DISTANCE		6
			struct{
				Uint16 distance				:16;		// [mm]
				Sint16 x                    :16;        // [mm]
				Sint16 y                    :16;        // [mm]
			}prop_warn_distance;


		#define PROP_SET_CORRECTORS			0x110
		#define SIZE_PROP_SET_CORRECTORS	1
			struct{
				bool_e rot_corrector		:1;
				bool_e trans_corrector		:1;
			}prop_set_correctors;

		#define PROP_JOYSTICK 				0x111
		#define SIZE_PROP_JOYSTICK			3
			struct{
				Uint8 duty_left				:8;
				Uint8 duty_right			:8;
				bool_e buttonA				:1;
				bool_e buttonB				:1;
			}prop_joystick;

		#define PROP_OFFSET_AVOID			0x112
		#define SIZE_PROP_OFFSET_AVOID		8
			struct{
				Uint16 x_left				:16;		// [mm]
				Uint16 x_right				:16;		// [mm]
				Uint16 y_front				:16;		// [mm]
				Uint16 y_back				:16;		// [mm]
			}prop_offset_avoid;

		#define PROP_DEBUG_FORCED_FOE		0x118

		#define PROP_TRANSPARENCY			0x117
		#define SIZE_PROP_TRANSPARENCY		2
			struct{
				Uint8 number				:8;
				bool_e enable				:1;
			}prop_transparency;

		#define PROP_SCAN_DUNE				0x119

		#define PROP_RUSH					0x11A
		#define SIZE_PROP_RUSH				8
			struct{
				Sint16 first_traj_acc			:16;
				Sint16 second_traj_acc			:16;
				Sint16 brake_acc				:16;
				Uint8 acc_rot_trans				:8;
				bool_e rush						:1;
			}prop_rush;

		#define PROP_ACTIVE_PID				0x11B
		#define SIZE_PROP_ACTIVE_PID		1
			struct{
				bool_e state				:1;
			}prop_active_pid;

		#define PROP_DATALASER				0x11C

		#define PROP_CUSTOM_AVOIDANCE		0x11D
		#define SIZE_PROP_CUSTOM_AVOIDANCE		1
			struct{
				bool_e active_small_avoidance :1;
			}prop_custom_avoidance;

		#define PROP_ASK_CORNER_SCAN			0x11C
		#define SIZE_PROP_ASK_CORNER_SCAN		1
			struct{
				color_e color				:1;
				bool_e isRightSensor		:1;
				bool_e startScan			:1;
			}strat_ask_corner_scan;

	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **										Carte Strat <-> Carte Balise												 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

		#define BEACON_ENABLE_PERIODIC_SENDING	0x410

		#define BEACON_DISABLE_PERIODIC_SENDING	0x411

		#define BEACON_WARNING_LOW_BATTERY		0x412
		#define SIZE_BEACON_WARNING_LOW_BATTERY	3
			struct{
				Uint16 battery_voltage		:16;
				RF_module_e module          :3;
			}beacon_warning_low_battery;

		#define BEACON_INFORMATION_XBEE			0x413
		#define SIZE_BEACON_INFORMATION_XBEE	1
			struct{
				bool_e xbeeConnected;
			}beacon_information_xbee;


	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **										Carte Strat <-> Carte Act													 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

		/*
		 * Le sid actionneur & 0xFF (on ne garde que l'octet de poid faible,
		 * l'octet de poids fort contient le filtre et on en a pas besoin)
		 */
		#define ACT_RESULT					(STRAT_FILTER | (ACT_FILTER >> 4) | 0)
		#define SIZE_ACT_RESULT				8
			struct{
				Uint32 param						:32;
				Uint8 sid							:8;
				Uint8 cmd							:8;
				act_result_error_code_e error_code	:8;
				act_result_state_e result			:2;
			}act_result;


		/*
		 * Pour tout les SIDs de l'enum ACT_sid_e
		 */
		#define SIZE_ACT_MSG				8
		#define SIZE_ACT_SET_CONFIG			7
		#define SIZE_ACT_GET_CONFIG			7
		#define SIZE_ACT_WARNER				7
			struct{
				ACT_order_e order					:8;
				union{
					struct{
						bool_e run_now				:1;
						Uint16 act_optionnal_data[2];
					}act_order;

					// For setting a config
					struct{
						act_config_e config			:8;
						act_sub_act_id_e sub_act_id :8;
						union{
							Uint16 raw_data			:16;
							Uint8 speed_wheel		:7;		// wheel mode (0 à 100)
							Uint16 speed_position	:9;		// position mode (0 à 500)
							Uint8 torque			:7;
						}data_config;
					}act_config;

					// For getting a config
					act_config_e config				:8;

					// For setting a warner
					ACT_order_e warner_pos			:8;

				}act_data;
			}act_msg;


		#define ACT_ERROR							0x3EA
		#define SIZE_ACT_ERROR_MSG					7
			struct{
				Uint32 param						:32;
				Uint8 sid							:8;
				Uint8 error_code					:8;
				Uint8 idAct							:8;
			}act_error;

		#define ACT_ASK_SENSOR						0x3FE
		#define SIZE_ACT_ASK_SENSOR					1
			struct{
				act_sensor_id_e act_sensor_id		:8;
			}act_ask_sensor;

		#define ACT_BOOST_ASSER						0x3FD
		#define SIZE_ACT_BOOST_ASSER				1
			struct{
				bool_e enable						:1;
			}act_boost_asser;

		#define ACT_GET_CONFIG_ANSWER				0x21B
		#define SIZE_ACT_GET_CONFIG_ANSWER			5
			struct{
				Uint16 sid							:8;
				act_config_e config					:8;

				union{
					struct{
						ACT_order_e order			:8;
						Uint16 pos					:16;
					}act_get_config_pos_answer;
					Sint8 torque					:8;
					Uint8 temperature				:8;
					Sint8 load						:8;
				}act_get_config_data;
			}act_get_config_answer;

		#define ACT_WARNER_ANSWER					0x21C
		#define SIZE_ACT_WARNER_ANSWER				4
			struct{
				Uint16 sid							:8;
			}act_warner_answer;

		#define ACT_GET_MOSFET_CURRENT_STATE				0x3FC
		#define SIZE_ACT_GET_MOSFET_CURRENT_STATE			1
			struct{
				act_vacuostat_id id							:4;			// De 0 à 7
			}act_get_mosfet_state;

		#define ACT_TELL_MOSFET_CURRENT_STATE				0x21E
		#define SIZE_ACT_TELL_MOSFET_CURRENT_STATE			1
			struct{
				act_vacuostat_id id							:4;			// De 0 à 7
				MOSFET_BOARD_CURRENT_MEASURE_state_e state	:4;
			}act_tell_mosfet_state;

		#define ACT_GET_TURBINE_SPEED						0x3FB
		#define SIZE_ACT_GET_TURBINE_SPEED					0

		#define ACT_TELL_TURBINE_SPEED						0x220
		#define SIZE_ACT_TELL_TURBINE_SPEED					2
			struct{
				Uint16 speed								:16;		// [rpm]
			}act_tell_turbine_speed;

		#define ACT_SET_TURBINE_SPEED						0x3FA
		#define SIZE_ACT_SET_TURBINE_SPEED					2
			struct{
				Uint16 speed								:16;		// [rpm]
			}act_set_turbine_speed;

		#define ACT_GET_COLOR_SENSOR_I2C					0x3EF
		#define SIZE_ACT_GET_COLOR_SENSOR_I2C					0

		#define ACT_TELL_COLOR_SENSOR_I2C					0x221
		#define SIZE_ACT_TELL_COLOR_SENSOR_I2C				1
			struct{
				COLOR_SENSOR_I2C_color_e color				:8;
			}act_tell_color_sensor_i2c;

	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **										Carte Strat <-> Carte Act													 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

		#define MOSFET_BOARD_SET_MOSFET						0x500
		#define SIZE_MOSFET_BOARD_SET_MOSFET				1
			struct{
				Uint8 id									:4;			// De 0 à 7
				bool_e state								:1;
			}mosfet_board_set_mosfet;

		#define MOSFET_BOARD_GET_MOSFET_CURRENT_STATE		0x501
		#define SIZE_MOSFET_BOARD_GET_MOSFET_CURRENT_STATE	1
			struct{
				Uint8 id									:4;			// De 0 à 7
			}mosfet_board_get_mosfet_state;

		#define MOSFET_BOARD_TELL_MOSFET_CURRENT_STATE		0x502
		#define SIZE_MOSFET_BOARD_TELL_MOSFET_CURRENT_STATE	1
			struct{
				Uint8 id									:4;			// De 0 à 7
				MOSFET_BOARD_CURRENT_MEASURE_state_e state	:4;
			}mosfet_board_tell_mosfet_state;

		#define MOSFET_BOARD_GET_TURBINE_SPEED				0x503
		#define SIZE_MOSFET_BOARD_GET_TURBINE_SPEED			0

		#define MOSFET_BOARD_TELL_TURBINE_SPEED				0x504
		#define SIZE_MOSFET_BOARD_TELL_TURBINE_SPEED		1
			struct{
				Uint16 speed								:16;		// [rpm]
			}mosfet_board_tell_turbine_speed;

		#define MOSFET_BOARD_SET_TURBINE_SPEED				0x505
		#define SIZE_MOSFET_BOARD_SET_TURBINE_SPEED			1
			struct{
				Uint16 speed								:16;		// [rpm]
			}mosfet_board_set_turbine_speed;


	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **													DEBUG															 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

		#define DEBUG_SELFTEST_LAUNCH		0x700

		/*
		 * Déclenchement manuel d'un évitement.
		 */
		#define DEBUG_DETECT_FOE			0x799

		#define DEBUG_FOE_POS				0x748
		#define SIZE_DEBUG_FOE_POS			7
			struct{
				Sint16 angle				:16;		// [PI4096]
				Sint16 x					:16;		// [mm]
				Sint16 y					:16;		// [mm]
				Uint8 dist					:8;			// [mm]
			}debug_foe_pos;

		#define DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT	0x710
		#define SIZE_DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT 3
			struct{
				Uint16 duration				:16;		// [mm]
				Uint8 id_it_state_name		:8;
			}debug_propulsion_erreur_recouvrement_it;

		#define DEBUG_PROPULSION_MAILBOX_IN_IS_FULL		0x70D

		#define DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL	0x70E

		#define DEBUG_ENABLE_MODE_BEST_EFFORT			0x70F

		#define DEBUG_DO_TRAJECTORY_FOR_TEST_COEFS		0x711

		#define DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE	0x712
		#define SIZE_DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE 2
			struct{
				Uint16 duration				:16;		// [mm]
			}debug_trajectory_for_test_coefs_done;

		#define DEBUG_PROPULSION_SET_ACCELERATION		0x713
		#define SIZE_DEBUG_PROPULSION_SET_ACCELERATION	4
			struct{
				Uint32 acceleration_coef		:32;	// [mm.4096/5ms/5ms]
			}debug_propulsion_set_acceleration;

		#define DEBUG_SET_ERROR_TRESHOLD_TRANSLATION	0x714
		#define SIZE_DEBUG_SET_ERROR_TRESHOLD_TRANSLATION	4
			struct{
				Sint32 error_treshold_trans		:32;	// [mm]
			}debug_set_error_treshold_translation;

		#define DEBUG_PROPULSION_GET_COEFS	0x720

		#define DEBUG_PROPULSION_COEF_IS	0x721
		#define SIZE_DEBUG_PROPULSION_COEF_IS	5
			struct{
				Sint32 value				:32;
				PROPULSION_coef_e id		:8;
			}debug_propulsion_coef_is;

		#define DEBUG_PROPULSION_SET_COEF	0x722
		#define SIZE_DEBUG_PROPULSION_SET_COEF	5
			struct{
				Sint32 value				:32;
				PROPULSION_coef_e id		:8;
			}debug_propulsion_set_coef;

		#define DEBUG_PROPULSION_RESET_COEF	0x723
		#define SIZE_DEBUG_PROPULSION_RESET_COEF	1
			struct{
				PROPULSION_coef_e id		:8;
			}debug_propulsion_reset_coef;

		#define DEBUG_RTC_GET				0x780

		#define DEBUG_RTC_SET				0x781
		#define SIZE_DEBUG_RTC_SET			5
			struct{
				Uint32 seconde				:6;
				Uint32 minute				:6;
				Uint32 heure				:4;
				Uint32 jour					:5;
				Uint32 mois					:4;
				Uint32 annee				:7;
				Uint32 journee				:3;
			}debug_rtc_set;

		#define	DEBUG_RTC_TIME				0x782
		#define SIZE_DEBUG_RTC_TIME			5
			struct{
				Uint32 seconde				:6;
				Uint32 minute				:6;
				Uint32 heure				:4;
				Uint32 jour					:5;
				Uint32 mois					:4;
				Uint32 annee				:7;
				Uint32 journee				:3;
			}debug_rtc_time;

		#define DEBUG_PROP_MOVE_POSITION	0x783
		#define SIZE_DEBUG_PROP_MOVE_POSITION	6
			struct{
				Sint16 xOffset				:16;		// [mm]
				Sint16 yOffset				:16;		// [mm]
				Sint16 tetaOffset			:16;		// [PI4096]
			}debug_prop_move_position;

		#define DEBUG_AVOIDANCE_POLY		0x790
		#define SIZE_DEBUG_AVOIDANCE_POLY	8
			struct{
				struct{
					Uint8 x					:8;			// [mm/16]
					Uint8 y					:8;			// [mm/16]
				}point[3];
				Uint8 first_point_number	:8;
				bool_e new_polygone			:1;
			}debug_avoidance_poly;


		#define DEBUG_HOKUYO_ADD_POINT		0x791
		#define SIZE_DEBUG_HOKUYO_ADD_POINT	8
			struct{
				struct{
					Uint8 x					:8;			// [mm/16]
					Uint8 y					:8;			// [mm/16]
				}point[4];
			}debug_hokuyo_add_point;

		#define DEBUG_HOKUYO_RESET			0x792

		#define DEBUG_HOKUYO_INTENSITY_ADD_POINT	0x793
		#define SIZE_DEBUG_HOKUYO_INTENSITY_ADD_POINT	8
			struct{
				struct{
					Uint8 x					:8;			// [mm/16]
					Uint8 y					:8;			// [mm/16]
				}point[4];
			}debug_hokuyo_intensity_add_point;

		#define DEBUG_HOKUYO_INTENSITY_RESET	0x794


		/*
		 * Message de la super pour l'EEPROM -> compteur des erreurs de balise
		 * Cummulable par masquage
		 */
		#define IR_ERROR_RESULT				0x753
		#define SIZE_IR_ERROR_RESULT		8
			struct{
				Uint8 error_counter[8];
			}ir_error_result;

		/*
		 * Message de debug qui explique la raison d 'un evitement
		 */
		#define DEBUG_FOE_REASON			0x755
		#define SIZE_DEBUG_FOE_REASON		5
			struct{
				Sint16 angle				:16;		// [PI4096]
				Uint16 dist					:16;		// [mm]
				foe_origin_e foe_origine	:8;
			}debug_foe_reason;

	}msg_can_formated_u;

	typedef struct{
		Uint11 sid;
		msg_can_formated_u data;
		Uint8 size;
	}CAN_msg_t;

	/*
	 * Code des SID des messages: 0x30x = message pour Holly, 0x33x = message pour Wood.
	 * Le SID 0x300 est reservé pour le self_test
	 * Ceci est un enum de SID d'actionneur avec les paramètres de chaque actions définie par des defines.
	 * L'enum est utilisé pour vérifier que tous les messages de retour d'actionneurs sont géré en strat
	 */
	typedef enum {

		//////////////////////////////////////////////////////////////////
		//----------------------------HARRY-----------------------------//
		//////////////////////////////////////////////////////////////////


        ACT_BIG_BALL_FRONT_LEFT         = (ACT_FILTER | 0x01),
        ACT_BIG_BALL_FRONT_RIGHT        = (ACT_FILTER | 0x02),
        ACT_BIG_BALL_BACK_LEFT          = (ACT_FILTER | 0x03),
        ACT_BIG_BALL_BACK_RIGHT         = (ACT_FILTER | 0x04),
        ACT_BEARING_BALL_WHEEL			= (ACT_FILTER | 0x05),

		ACT_CYLINDER_SLOPE_LEFT			= (ACT_FILTER | 0x06),
		ACT_CYLINDER_SLOPE_RIGHT		= (ACT_FILTER | 0x07),
		ACT_CYLINDER_BALANCER_LEFT		= (ACT_FILTER | 0x08),
		ACT_CYLINDER_BALANCER_RIGHT		= (ACT_FILTER | 0x09),
        ACT_CYLINDER_PUSHER_LEFT		= (ACT_FILTER | 0x0A),
        ACT_CYLINDER_PUSHER_RIGHT       = (ACT_FILTER | 0x0B),
        ACT_CYLINDER_ELEVATOR_LEFT		= (ACT_FILTER | 0x0C),
        ACT_CYLINDER_ELEVATOR_RIGHT     = (ACT_FILTER | 0x0D),
        ACT_CYLINDER_SLIDER_LEFT		= (ACT_FILTER | 0x0E),
        ACT_CYLINDER_SLIDER_RIGHT       = (ACT_FILTER | 0x0F),
        ACT_CYLINDER_ARM_LEFT			= (ACT_FILTER | 0x10),
        ACT_CYLINDER_ARM_RIGHT     		= (ACT_FILTER | 0x11),
		ACT_CYLINDER_COLOR_LEFT			= (ACT_FILTER | 0x12),
		ACT_CYLINDER_COLOR_RIGHT		= (ACT_FILTER | 0x13),
		ACT_CYLINDER_DISPOSE_LEFT		= (ACT_FILTER | 0x14),
		ACT_CYLINDER_DISPOSE_RIGHT		= (ACT_FILTER | 0x15),

		ACT_ORE_GUN						= (ACT_FILTER | 0x16),
		ACT_ORE_WALL					= (ACT_FILTER | 0x17),
        ACT_ORE_ROLLER_ARM              = (ACT_FILTER | 0x18),
        ACT_ORE_ROLLER_FOAM             = (ACT_FILTER | 0x19),
        ACT_ORE_TRIHOLE		            = (ACT_FILTER | 0x20),

        ACT_ROCKET                      = (ACT_FILTER | 0x21),


		//////////////////////////////////////////////////////////////////
		//-----------------------------ANNE-----------------------------//
		//////////////////////////////////////////////////////////////////

		ACT_SMALL_BALL_BACK				= (ACT_FILTER | 0x31),
		ACT_SMALL_BALL_FRONT_LEFT		= (ACT_FILTER | 0x33),
		ACT_SMALL_BALL_FRONT_RIGHT		= (ACT_FILTER | 0x34),
		ACT_SMALL_CYLINDER_ARM			= (ACT_FILTER | 0x35),
		ACT_SMALL_CYLINDER_BALANCER		= (ACT_FILTER | 0x36),
		ACT_SMALL_CYLINDER_COLOR		= (ACT_FILTER | 0x37),
		ACT_SMALL_CYLINDER_DISPOSE		= (ACT_FILTER | 0x38),
		ACT_SMALL_CYLINDER_ELEVATOR		= (ACT_FILTER | 0x39),
		ACT_SMALL_CYLINDER_SLIDER		= (ACT_FILTER | 0x3A),
		ACT_SMALL_CYLINDER_SLOPE		= (ACT_FILTER | 0x3B),
		ACT_SMALL_CYLINDER_MULTIFONCTION= (ACT_FILTER | 0x3C),
		ACT_SMALL_MAGIC_ARM				= (ACT_FILTER | 0x3D),
		ACT_SMALL_MAGIC_COLOR			= (ACT_FILTER | 0x3E),
		ACT_SMALL_ORE					= (ACT_FILTER | 0x3F),
		ACT_SMALL_POMPE_PRISE			= (ACT_FILTER | 0x40),
		ACT_SMALL_POMPE_DISPOSE			= (ACT_FILTER | 0x41),





		//////////////////////////////////////////////////////////////////
		//-----------------------------BOTH-----------------------------//
		//////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////
		//-------------------------MOSFETS ACT--------------------------//
		//////////////////////////////////////////////////////////////////

		ACT_MOSFET_1				= (ACT_FILTER | 0x60),
		ACT_TURBINE					= (ACT_FILTER | 0x60),

		ACT_MOSFET_2				= (ACT_FILTER | 0x61),
		ACT_POMPE_ELEVATOR_RIGHT	= (ACT_FILTER | 0x61),

		ACT_MOSFET_3				= (ACT_FILTER | 0x62),
		ACT_POMPE_ELEVATOR_LEFT		= (ACT_FILTER | 0x62),

		ACT_MOSFET_4				= (ACT_FILTER | 0x63),
		ACT_POMPE_DISPOSE_RIGHT		= (ACT_FILTER | 0x63),

		ACT_MOSFET_5				= (ACT_FILTER | 0x64),
		ACT_POMPE_DISPOSE_LEFT		= (ACT_FILTER | 0x64),

		ACT_MOSFET_6				= (ACT_FILTER | 0x65),
		ACT_POMPE_SLIDER_RIGHT		= (ACT_FILTER | 0x65),

		ACT_MOSFET_7				= (ACT_FILTER | 0x66),
		ACT_POMPE_SLIDER_LEFT		= (ACT_FILTER | 0x66),

		ACT_MOSFET_8			    = (ACT_FILTER | 0x67),
		ACT_MOSFET_MULTI			= (ACT_FILTER | 0x68),

	//////////////////////////////////////////////////////////////////
	//-------------------------MOSFETS STRAT------------------------//
	//////////////////////////////////////////////////////////////////
		STRAT_MOSFET_1				= (ACT_FILTER | 0x69),
		STRAT_MOSFET_2				= (ACT_FILTER | 0x70),
		STRAT_MOSFET_3				= (ACT_FILTER | 0x71),
		STRAT_MOSFET_4				= (ACT_FILTER | 0x72),
		STRAT_MOSFET_5				= (ACT_FILTER | 0x73),
		STRAT_MOSFET_6				= (ACT_FILTER | 0x74),
		STRAT_MOSFET_7				= (ACT_FILTER | 0x75),
		STRAT_MOSFET_8				= (ACT_FILTER | 0x76),
		STRAT_MOSFET_MULTI			= (ACT_FILTER | 0x77)

	} ACT_sid_e;

#endif	/* ndef QS_CANMSGLIST_H */


/*
	msg_can_formated_u msg;
	for(i=0;i<8;i++)
		msg.raw_data[i] = 0xFF; // Initialiser toutes les données à 1

	// Mettre les champs à 0
	msg.debug_rtc_set.heure = 0;
	msg.debug_rtc_set.jour = 0;
	msg.debug_rtc_set.journee = 0;
	msg.debug_rtc_set.minute = 0;
	msg.debug_rtc_set.mois = 0;
	msg.debug_rtc_set.seconde = 0;
	msg.debug_rtc_set.annee = 0;

	printf("\n");
	for(i=0;i<8;i++)
		display_bit(msg.raw_data[i]);
	printf("\n\n");

	// Il en résultatera un affichage des bits utilisé à 0 et non utilisé à 1

*/
