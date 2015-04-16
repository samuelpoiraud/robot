/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi'Tech, Chomp
 *
 *  Fichier : can.c
 *  Package : Asser
 *  Description : CAN
 *  Auteur : Val', Nirgal
 *  Version 20100211
 */



#include "secretary.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_adc.h"
#include "QS/QS_maths.h"
#include "QS/QS_ports.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_verbose.h"
#include "pilot.h"
#include "warner.h"
#include "corrector.h"
#include "odometry.h"
#include "roadmap.h"
#include "sequences.h"
#include "copilot.h"
#include "supervisor.h"
#include "joystick.h"
#include "LCDTouch/LCD.h"
#include "hokuyo.h"
#include "supervisor.h"
#include "detection.h"
#include "gyroscope.h"
#include "hokuyo.h"
#include "avoidance.h"
#include "scan_cup.h"
#include "com_xbee.h"



typedef struct{
	CAN_msg_t can_msg;
	MAIL_from_to_e from_to;
}mailbox_t;

static void SECRETARY_send_callback(CAN_msg_t * can_msg);
static void SECRETARY_send_report();
static bool_e SECRETARY_mailbox_get(Uint8 * index);
static void SECRETARY_send_pong(void);
static void SECRETARY_send_coef(PROPULSION_coef_e i);
static void SECRETARY_send_all_coefs(void);
static void SECRETARY_mailbox_out_add(CAN_msg_t * msg, MAIL_from_to_e from_to);
static void SECRETARY_mailbox_out_process_main(void);
static void SECRETARY_mailbox_in_add(CAN_msg_t * msg, MAIL_from_to_e from_to);

volatile static Uint8 mailbox_in_index_read;
volatile static Uint8 mailbox_in_index_write;
volatile static Uint8 mailbox_in_index_nb;
volatile static Uint8 mailbox_out_index_read;
volatile static Uint8 mailbox_out_index_write;
volatile static Uint8 mailbox_out_index_nb;
volatile bool_e selftest_validated = FALSE;

//Ne doit pas être trop petit dans le cas de courbe multipoint assez grande: on doit pouvoir contenir tous les messages CAN qu'on reçoit en 5ms dans ce buffer
#define SECRETARY_MAILBOX_IN_SIZE (32)
#define SECRETARY_MAILBOX_OUT_SIZE (16)

mailbox_t mailbox_in[SECRETARY_MAILBOX_IN_SIZE];  //Les messages CAN reçus dans le main sont ajouté à la mailbox et traités en IT...
//Cela permet d'éviter les nombreux problèmes liés à la préemption (notamment liés au buffer d'ordres... dont les fonctions ne peuvent être réentraintes)

mailbox_t mailbox_out[SECRETARY_MAILBOX_OUT_SIZE];	//Messages fournit en IT... à sortir en tâche de fond...


//////////////////////////////////////////////////////////////////
//-------------------FONCTION PRINCIPALE------------------------//
//////////////////////////////////////////////////////////////////

void SECRETARY_init(void)
{
	CAN_init();
	CAN_set_send_callback(SECRETARY_send_callback);
	mailbox_in_index_read = 0;
	mailbox_in_index_write = 0;
	mailbox_in_index_nb = 0;
	mailbox_out_index_read = 0;
	mailbox_out_index_write = 0;
	mailbox_out_index_nb = 0;
}

void SECRETARY_process_main(void)
{

	static CAN_msg_t receivedCanMsg_over_uart;
	static CAN_msg_t received_msg;
	// gestion du CAN conforme QS
	while(CAN_data_ready())
	{
			received_msg = CAN_get_next_msg();
			SECRETARY_mailbox_in_add(&received_msg, FROM_CAN);
			#ifdef CAN_VERBOSE_MODE
				QS_CAN_VERBOSE_can_msg_print(&received_msg, VERB_INPUT_MSG);
			#endif
	}

	while(UART1_data_ready())
	{
		toggle_led(LED_UART);
		#if defined(STM32F40XX)
			if(u1rxToCANmsg(&receivedCanMsg_over_uart, UART1_get_next_msg()))
			{

				#ifdef LCD_TOUCH
				if(	global.disable_virtual_perfect_robot == FALSE 				||	//Si le robot virtuel est actif, je laisse passer le message
					receivedCanMsg_over_uart.sid == BROADCAST_POSITION_ROBOT 	||	//Sinon, je laisse passer les messages de position et de position adverse.
					receivedCanMsg_over_uart.sid == BROADCAST_ADVERSARIES_POSITION	||
					receivedCanMsg_over_uart.sid == BROADCAST_COULEUR
					)
				#endif
					SECRETARY_mailbox_in_add(&receivedCanMsg_over_uart, FROM_UART);
			}
		#else
			if(u1rxToCANmsg(&receivedCanMsg_over_uart))
				SECRETARY_mailbox_add(&receivedCanMsg_over_uart, FROM_UART);
		#endif
	}

	SECRETARY_mailbox_out_process_main();
}

void SECRETARY_process_it(void)
{
	Uint8 index;
	while(SECRETARY_mailbox_get(&index))  //Tant qu'il y a un message à traiter...
	{
		SECRETARY_process_CANmsg(&(mailbox_in[index].can_msg), mailbox_in[index].from_to);  //Traitement du message CAN.
	}
}




//////////////////////////////////////////////////////////////////
//--------------------FONCTION RECEPTION------------------------//
//////////////////////////////////////////////////////////////////

/* types d'ordres

PROP_GO_ANGLE
PROP_GO_POSITION

	arguments
	dans l'ordre :
	0.CONFIG, 	1.TETAHIGH, 	2.TETALOW, 		3.0, 			4.0,  		5.VITESSE, 		6.MARCHE, 		7.RAYONCRB
	0.CONFIG, 	1.XHIGH, 		2.XLOW, 		3.YHIGH, 		4.YLOW, 	5.VITESSE, 		6.MARCHE, 		7.RAYONCRB


	octet de CONFIG
		...0 .... > maintenant
		...1 .... > a la fin du buffer
		..0. .... > pas multipoint
		..1. .... > multipoint
		.... ...0 > pas relatif
		.... ...1 > relatif
		.... 00.. > asservissement en rotation et translation
		.... 01.. > asservissement en rotation seulement
		.... 10.. > asservissement en translation seulement
		.... 11.. > asservissement désactivé !!!

	octet de VITESSE
		0x00 : rapide
		0x01 : lent
		0x02 : très lent
		....
		0x08 à 0xFF : vitesse "analogique"

	octet de MARCHE
		...0 ...0 > marche avt ou arrière
		...0 ...1 > marche avant obligé
		...1 ...0 > marche arrière obligée
		...1 ...1 > marche avt ou arrière

	octet de RAYONCRB
		.... ...0 > courbe automatique désactivé (choix en propulsion)
		.... ...1 > courbe automatique activé
		xxxx .... > type d'évitement sur la trajectoire



		case PROP_TELL_POSITION:		RIEN
		case PROP_STOP:					RIEN
		case PROP_SET_POSITION:			x, y, teta
		case PROP_RUSH_IN_THE_WALL:	0.CONFIG, 5.VITESSE, 6.MARCHE			//TODO !!!!!

		case PROP_CALIBRATION:			RIEN (la couleur doit etre bonne...)
		case PROP_WARN_ANGLE:			teta
		case PROP_WARN_X:				x
		case PROP_WARN_Y:				y

*/
void SECRETARY_process_CANmsg(CAN_msg_t* msg, MAIL_from_to_e from)
{
	way_e sens_marche;
	toggle_led(LED_CAN);
	//debug_printf("#%x\n",msg->sid);

#ifdef XBEE_SIMULATION
	if((msg->sid & 0xF00) == XBEE_FILTER){
		if(from == FROM_CAN) // Vient du CAN(strat)
			CANmsgToU1tx(msg);
		else if(from == FROM_UART) // Vient de la simulation
			CAN_send(msg);
	}
#endif

	switch (msg->sid)
	{
		case BROADCAST_RESET:
			NVIC_SystemReset();
			break;

		case PROP_STOP:
			ROADMAP_add_order(  TRAJECTORY_STOP,
								0,
								0,
								0,					//teta
								NOT_RELATIVE,		//relative
								NOW,			//maintenant
								ANY_WAY,	//sens de marche
								NOT_BORDER_MODE,	//mode bordure
								NO_MULTIPOINT, 	//mode multipoints
								FAST,				//Vitesse
								ACKNOWLEDGE_ASKED,
								CORRECTOR_ENABLE,
								AVOID_DISABLED
							);
		break;

		case PROP_GO_ANGLE:

			ROADMAP_add_order( 	TRAJECTORY_ROTATION,							// type trajectoire
								0,									//x
								0,									//y
								(U16FROMU8(msg->data[1],msg->data[2])),	//teta
								(msg->data[0] & 0x01)?RELATIVE:NOT_RELATIVE,	//relative
								(msg->data[0] & 0x10)?NOT_NOW:NOW,//maintenant
								ANY_WAY,					//sens de marche
								NOT_BORDER_MODE,					//mode bordure
								(msg->data[0] & 0x20)?MULTIPOINT:NO_MULTIPOINT, 	//mode multi points
								//NOT_MULTIPOINT,
								msg->data[5],						//Vitesse
								ACKNOWLEDGE_ASKED,
								(corrector_e)((msg->data[0] & 0x0C)>>2),
								AVOID_DISABLED
							);
		break;

		case PROP_GO_POSITION:
			//debug_printf("Receveid new go_position : x -> %d, y -> %d\n", U16FROMU8(msg->data[1],msg->data[2]), U16FROMU8(msg->data[3],msg->data[4]));

			//Réglage sens:
			if ((msg->data[6] == BACKWARD) || (msg->data[6] == FORWARD))
				sens_marche = msg->data[6];	//LE SENS EST imposé
			else
				sens_marche = ANY_WAY;	//ON SE FICHE DU SENS

			ROADMAP_add_order(  ((msg->data[7]&0x0F) != 0)?TRAJECTORY_AUTOMATIC_CURVE:TRAJECTORY_TRANSLATION,
								(U16FROMU8(msg->data[1],msg->data[2])),	//x
								(U16FROMU8(msg->data[3],msg->data[4])),	//y
								0,									//teta
								(msg->data[0] & 0x01)?RELATIVE:NOT_RELATIVE,	//relative
								(msg->data[0] & 0x10)?NOT_NOW:NOW,//maintenant
								sens_marche,						//sens de marche
								(msg->data[0] & 0x02)?BORDER_MODE:NOT_BORDER_MODE,	//mode bordure
								(msg->data[0] & 0x20)?MULTIPOINT:NO_MULTIPOINT, //mode multipoints
								msg->data[5],						//Vitesse
								ACKNOWLEDGE_ASKED,
								(corrector_e)((msg->data[0] & 0x0C)>>2),
								(avoidance_e)((msg->data[7] & 0xF0)>>4)
							);
		break;

		case PROP_CALIBRATION:	//Autocalage !
			SEQUENCES_calibrate(msg->data[0]);

		break;

		// Modifie la position de départ en fonction de la couleur
		case BROADCAST_COULEUR :
			//Le type couleur est normalisé en QS...
			ODOMETRY_set_color((color_e)(msg->data[0]));
			//Propagation sur UART (pour un éventuel robot virtuel branché sur notre UART)
			#ifdef CAN_SEND_OVER_UART
				CANmsgToU1tx(msg);
			#endif
			if(global.disable_virtual_perfect_robot == FALSE)
				SECRETARY_process_send(BROADCAST_POSITION_ROBOT, WARNING_NO, 0);
		break;

		// Impose une position (uniquement pour les tests !!!)
		case PROP_SET_POSITION:
			ODOMETRY_set(
							(U16FROMU8(msg->data[0],msg->data[1])),	//x
							(U16FROMU8(msg->data[2],msg->data[3])), 	//y
							(U16FROMU8(msg->data[4],msg->data[5]))	//teta
						);
			COPILOT_reset_absolute_destination();

		break;
		case PROP_RUSH_IN_THE_WALL:
			SEQUENCES_rush_in_the_wall((U16FROMU8(msg->data[2],msg->data[3])), msg->data[0], SLOW_TRANSLATION_AND_FAST_ROTATION, ACKNOWLEDGE_ASKED, 0, 0, BORDER_MODE, (msg->data[1])?CORRECTOR_ENABLE:CORRECTOR_TRANSLATION_ONLY);	//BORDER_MODE = sans mise à jour de position odométrie !
		break;

		//Stop tout
		case BROADCAST_STOP_ALL:
			//debug_printf("Broadcast_stop\n");
			global.match_over = TRUE;
			ROADMAP_add_order(  TRAJECTORY_STOP,
								0,
								0,
								0,					//teta
								NOT_RELATIVE,		//relative
								NOW,			//maintenant
								ANY_WAY,	//sens de marche
								NOT_BORDER_MODE,	//mode bordure
								NO_MULTIPOINT, 	//mode multipoints
								FAST,				//Vitesse
								ACKNOWLEDGE_ASKED,
								CORRECTOR_ENABLE,
								AVOID_DISABLED
							);
			// désactivation de tout les avertisseurs.
			WARNER_init();
			//arret robot
			SUPERVISOR_state_machine(EVENT_BROADCAST_STOP, 0);
			SECRETARY_send_report();
		break;

		case BROADCAST_START:
			//debug_printf("Broadcast_start, \n");
			global.match_started = TRUE;
		break;

		case BROADCAST_ALIM :
			global.alim_value = (((Uint16)(msg->data[1]) << 8) & 0xFF00) | ((Uint16)(msg->data[2]) & 0x00FF);
			break;

		//Carte stratégie demande la position
		case PROP_TELL_POSITION:
			SECRETARY_process_send(BROADCAST_POSITION_ROBOT, WARNING_NO, 0);
		break;
		case PROP_SET_CORRECTORS:
			if(msg->data[0])	//Rotation
			{
				if(msg->data[1])	//Translation
					CORRECTOR_PD_enable(CORRECTOR_ENABLE);
				else
					CORRECTOR_PD_enable(CORRECTOR_ROTATION_ONLY);
			}
			else
			{
				if(msg->data[1])	//Translation
					CORRECTOR_PD_enable(CORRECTOR_TRANSLATION_ONLY);
				else
					CORRECTOR_PD_enable(CORRECTOR_DISABLE);
			}
		break;
		//Une carte nous demande de l'avertir lorsque nous serons en approche d'une position...
		case PROP_WARN_ANGLE:
			WARNER_arm_teta(U16FROMU8(msg->data[0],msg->data[1]));
		break;
		case PROP_WARN_X:
			WARNER_arm_x(U16FROMU8(msg->data[0],msg->data[1]));
		break;
		case PROP_WARN_Y:
			WARNER_arm_y(U16FROMU8(msg->data[0],msg->data[1]));
		break;
		case PROP_SEND_PERIODICALLY_POSITION:
			WARNER_arm_timer(U16FROMU8(msg->data[0],msg->data[1]));
			WARNER_arm_translation(U16FROMU8(msg->data[2],msg->data[3]));
			WARNER_arm_rotation(U16FROMU8(msg->data[4],msg->data[5]));
		break;

		case PROP_JOYSTICK:
			JOYSTICK_enable(100, (Sint8)msg->data[0], (Sint8)msg->data[1], (bool_e)msg->data[2], (bool_e)msg->data[3]);
						//durée d'activation du joystick en ms
		break;

		//REGLAGES DES COEFFICIENTS !!!!!!!!
		case DEBUG_PROPULSION_SET_ACCELERATION:
			if(U16FROMU8(msg->data[0], msg->data[1]) < 200)	//Sécurité...
				PILOT_set_coef(PILOT_ACCELERATION_NORMAL, (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;

		case DEBUG_PROPULSION_GET_COEFS:
			SECRETARY_send_all_coefs();
		break;
		case DEBUG_PROPULSION_SET_COEF:
			if(msg->data[0] < PROPULSION_NUMBER_COEFS && msg->size >= 5)
			{
				if(msg->data[0] <= ODOMETRY_COEF_CENTRIFUGAL)
					ODOMETRY_set_coef(msg->data[0], (Sint32)(U32FROMU8(msg->data[1], msg->data[2], msg->data[3], msg->data[4])));
				else if(msg->data[0] == GYRO_COEF_GAIN)
					GYRO_set_coef(msg->data[0], (Sint32)(U32FROMU8(msg->data[1], msg->data[2], msg->data[3], msg->data[4])));
				else
					CORRECTOR_set_coef(msg->data[0], (Sint32)(U32FROMU8(msg->data[1], msg->data[2], msg->data[3], msg->data[4])));
			}
			else
				debug_printf("WARNING : bad datas in DEBUG_PROPULSION_SET_COEF message received\n");
			SECRETARY_send_coef(msg->data[0]);	//feedback
		break;
		case DEBUG_ENABLE_MODE_BEST_EFFORT:
			#ifndef MODE_SIMULATION
				global.mode_best_effort_enable = TRUE;
			#endif
		break;
		case DEBUG_SET_ERROR_TRESHOLD_TRANSLATION:
			SUPERVISOR_set_treshold_error_translation(msg->data[0]);
			break;

		case DEBUG_PROP_MOVE_POSITION:
			ODOMETRY_set(
							global.position.x + (U16FROMU8(msg->data[0],msg->data[1])),	//x
							global.position.y + (U16FROMU8(msg->data[2],msg->data[3])), 	//y
							GEOMETRY_modulo_angle(global.position.teta + (U16FROMU8(msg->data[4],msg->data[5])))	//teta
						);
			COPILOT_reset_absolute_destination();
			break;

		case PROP_PING:
			SECRETARY_send_pong();
		break;
		case PROP_DO_SELFTEST:
			SEQUENCES_selftest();
		break;
		case DEBUG_DO_TRAJECTORY_FOR_TEST_COEFS:
			SEQUENCES_trajectory_for_test_coefs();
			WARNER_enable_counter_trajectory_for_test_coefs_finished();
		break;

		case PROP_OFFSET_AVOID:
			AVOIDANCE_process_CAN_msg(msg);
			break;

		#ifdef LCD_TOUCH
		case BROADCAST_POSITION_ROBOT:
			//On a reçu une position robot... on la prend... et on ne bouge plus par nous même.
			global.disable_virtual_perfect_robot = TRUE;
			ODOMETRY_set(
							(U16FROMU8(msg->data[0],msg->data[1]))&0x1FFF,	//x
							(U16FROMU8(msg->data[2],msg->data[3]))&0x1FFF, //y
							(U16FROMU8(msg->data[4],msg->data[5]))	//teta
						);
		break;
		#endif
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			#ifdef CAN_SEND_OVER_UART
				CANmsgToU1tx(msg);
			#endif
			DETECTION_new_adversary_position(msg, NULL, 0);
			break;

		case BROADCAST_ADVERSARIES_POSITION:
			#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
				DETECTION_new_adversary_position(msg, NULL, 0);
				CAN_send(msg);
			#endif
			break;

		case PROP_SCAN_CUP:
			#ifdef SCAN_CUP
				SCAN_CUP_canMsg(msg);
			#endif
			break;

		case PROP_START_ROTATION:
			START_ROTATION_canMsg(msg); // Modification calibration
			break;

		case DEBUG_HOKUYO_ADD_POINT:
		case DEBUG_HOKUYO_RESET:
			#ifdef CAN_SEND_OVER_UART
				CANmsgToU1tx(msg);
			#endif
			break;

		case IHM_SWITCH_ALL:
		case IHM_BUTTON:
		case IHM_SWITCH:
		case IHM_POWER:
			if(from == FROM_UART) // Le message vient de la simulation sur pc, le message doit être répété pour les autres cartes
				CAN_send(msg);

			IHM_process_main(msg);
			break;

		case PROP_WOOD_PROTECT_ZONE :
			holly_receive_wood_position(msg);
			break;

		default :
		break;
	}
}




//////////////////////////////////////////////////////////////////
//---------------------FONCTION EMISSION------------------------//
//////////////////////////////////////////////////////////////////

void SECRETARY_send_canmsg(CAN_msg_t * msg)
{

	Uint8 i;
	for(i=msg->size;i<8;i++)
		msg->data[i]=0xFF;		//On remplace les données hors size par des FF (notamment pour verbose et envoi sur uart)

	CAN_send(msg);

	#ifdef CAN_SEND_OVER_UART
		CANmsgToU1tx(msg);
	#endif
}


void SECRETARY_send_canmsg_from_it(CAN_msg_t * msg)
{
	SECRETARY_mailbox_out_add(msg,TO_CAN);
}



bool_e SECRETARY_is_selftest_validated(void)
{
	return selftest_validated;
}

//si result == TRUE : le selftest s'est bien déroulé, sinon on remonte l'erreur à la stratégie.
void SECRETARY_send_selftest_result(bool_e result)
{
	CAN_msg_t msg;
	Uint8 i;
	selftest_validated = result;
	msg.sid = STRAT_PROP_SELFTEST_DONE;
	msg.size = 8;
	i = 0;
	if(result == FALSE)
		msg.data[i++] = SELFTEST_PROP_FAILED;
	if(HOKUYO_is_working_well() == FALSE)
		msg.data[i++] = SELFTEST_PROP_HOKUYO_FAILED;
	if(QS_WHO_AM_I_get() == SMALL_ROBOT){
		if(ADC_getValue(SCAN_CUP_SENSOR_LEFT) > 15)
			msg.data[i++] = SELFTEST_PROP_SENSOR_CUP_LEFT;
		if(ADC_getValue(SCAN_CUP_SENSOR_RIGHT) > 15)
			msg.data[i++] = SELFTEST_PROP_SENSOR_CUP_RIGHT;
	}

	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT	//L'odométrie est faite sur un robot virtuel parfait.
		msg.data[i++] = SELFTEST_PROP_IN_SIMULATION_MODE;
	#endif
	#ifdef LCD_TOUCH
		msg.data[i++] = SELFTEST_PROP_IN_LCD_TOUCH_MODE;
	#endif

	for(;i<8;i++)
		msg.data[i] = SELFTEST_NO_ERROR;

	SECRETARY_send_canmsg(&msg);
}

void SECRETARY_send_trajectory_for_test_coefs_finished(Uint16 duration)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE;
	msg.data[0] = HIGHINT(duration);
	msg.data[1] = LOWINT(duration);
	msg.size = 2;
	SECRETARY_send_canmsg(&msg);
}

//x : mm, y : mm, teta : rad4096, distance : mm
void SECRETARY_send_adversary_position(bool_e it_is_the_last_adversary, Uint8 adversary_number, Uint16 x, Uint16 y, Sint16 teta, Uint16 distance, Uint8 fiability)
{
	CAN_msg_t msg;
//force pos adversaries
	/*		0 : ADVERSARY_NUMBER	//de 0 à n, il peut y avoir plus de deux adversaires si l'on inclut notre ami...
	 * 		1 :  x [2cm]
	 * 		2 :  y [2cm]
	 * 		3-4 : teta
	 * 		5 : distance [2cm]
	 * 		6 : fiability	:    "0 0 0 0 d t y x" (distance, teta, y, x) : 1 si fiable, 0 sinon.
	 */
	msg.sid = BROADCAST_ADVERSARIES_POSITION;
	msg.data[0] = adversary_number | ((it_is_the_last_adversary)?IT_IS_THE_LAST_ADVERSARY:0);	//n° de l'ADVERSARY  + bit de poids fort si c'est le dernier adversaire
	msg.data[1] = x/20;	//X [2cm]
	msg.data[2] = y/20;	//Y [2cm]
	msg.data[3] = HIGHINT(teta);	//teta
	msg.data[4] = LOWINT(teta);	//teta
	msg.data[5] = distance/20;	//distance [2cm]
	msg.data[6] = fiability;	//fiability : x et y fiables
	msg.size = 7;
	SECRETARY_send_canmsg(&msg);
	//debug_printf("Adv%d\t%4d\t%4d\t%5d\t%4d\n%s",adversary_number,x,y,teta,distance,((it_is_the_last_adversary)?"\n":""));
}


void SECRETARY_process_send(Uint11 sid, Uint8 reason, SUPERVISOR_error_source_e error_source)	//La raison de l'envoi est définie dans avertisseur.h
{
	CAN_msg_t msg;
	Uint8 error_byte;
	Sint32 rot_speed;
	Uint8 trajectory_status; //3 bits

	trajectory_status = (global.vitesse_translation != 0) << 2 | (global.vitesse_rotation != 0) << 1;
	error_byte = ((Uint8)(trajectory_status) << 5) | (Uint8)(COPILOT_get_way()) << 3 | (Uint8)(error_source & 0x07);

	msg.sid = sid;
	msg.data[0] = (HIGHINT(global.position.x) & 0x1F) | (((absolute(global.real_speed_translation)>>10)/5) << 5);	//Vitesse sur 3 bits forts, en [250mm/s]
	msg.data[1] = LOWINT(global.position.x);
	rot_speed = ((absolute(global.real_speed_rotation)>>10)*200)>>12;
	if(rot_speed > 7)
		rot_speed = 7;	//ecretage pour tenir sur 3 bits
	msg.data[2] = (HIGHINT(global.position.y) & 0x1F) | ((Uint8)(rot_speed) << 5);	//Vitesse angulaire en radians
	msg.data[3] = LOWINT(global.position.y);
	msg.data[4] = HIGHINT(global.position.teta);
	msg.data[5] = LOWINT(global.position.teta);
	msg.data[6] = reason;
	msg.data[7] = error_byte;	//Octet d'erreur... voir warner.c qui rempli cet octet d'erreur...
			/*	Octet d'erreur :   0bTTTWWEEE
								 TTT = trajectory_e
								 WW  = way_e
								 EEE = SUPERVISOR_error_source_e
									*/
	msg.size = 8;
	SECRETARY_send_canmsg(&msg);
}

#ifdef LCD_TOUCH
	void SECRETARY_send_friend_position(Sint16 x, Sint16 y)
	{
		CAN_msg_t msg;
		msg.sid = STRAT_FRIEND_FORCE_POSITION;
		msg.data[0] = x/2;
		msg.data[1] = y/2;
		msg.size = 2;
		SECRETARY_send_canmsg(&msg);
	}
#endif

//Fonction appelée uniquement en IT.
void SECRETARY_send_foe_detected(Uint16 x, Uint16 y, bool_e timeout){
	CAN_msg_t msg;
		msg.sid = STRAT_PROP_FOE_DETECTED;
		msg.size = 5;
		msg.data[0] = HIGHINT(x);
		msg.data[1] = LOWINT(x);
		msg.data[2] = HIGHINT(y);
		msg.data[3] = LOWINT(y);
		msg.data[4] = timeout;
	SECRETARY_send_canmsg_from_it(&msg);
}

#ifdef SCAN_CUP
//Fonction appelée uniquement en IT.
void SECRETARY_send_cup_position(bool_e it_is_the_last_cup, bool_e error_scan, bool_e cup_detected, Sint16 x, Sint16 y)
{
	CAN_msg_t msg;
	/*		0:7		: Indique si c'est le dernier gobelet
	 *		0:6		: Indique s'il y a eu une erreur lors du scan
	 *		0:5		: Est à 1 s'il y a un ou des gobelets de détecté(s)
	 * 		1		: x HIGH bit
	 * 		2		: x LOW bit
	 * 		3		: y HIGH bit
	 *		4		: y LOW bit
	 */
	msg.sid = STRAT_CUP_POSITION;
	msg.data[0] = ((it_is_the_last_cup)?1:0) | (((error_scan)?1:0) << 1) | (((cup_detected)?1:0) << 2);
	msg.data[1] = HIGHINT(x);
	msg.data[2] = LOWINT(x);
	msg.data[3] = HIGHINT(y);
	msg.data[4] = LOWINT(y);
	msg.size = 5;
	SECRETARY_send_canmsg_from_it(&msg);
}

void SECRETARY_ask_start_rotation()  // Modification calibration
{
	CAN_msg_t msg;

	//		0		: Rotation ou non

	msg.sid = STRAT_PROP_START_ROTATION;
	msg.size = 0;
	SECRETARY_send_canmsg_from_it(&msg);
}

#endif



//////////////////////////////////////////////////////////////////
//----------------------FONCTION AUTRE--------------------------//
//////////////////////////////////////////////////////////////////

static void SECRETARY_mailbox_in_add(CAN_msg_t * msg, MAIL_from_to_e from_to) //Fonction appelée en tâche de fond uniquement !
{
	if(mailbox_in_index_nb < SECRETARY_MAILBOX_IN_SIZE)
	{
		mailbox_in[mailbox_in_index_write].can_msg = *msg;	//J'écris tranquillement mon message (tant pis si je suis préempté maintenant...)
		mailbox_in[mailbox_in_index_write].from_to = from_to;
		TIMER2_disableInt();
			mailbox_in_index_nb++; //Il ne faut pas que la préemption ait lieu maintenant !
		TIMER2_enableInt();
		mailbox_in_index_write = (mailbox_in_index_write + 1) % SECRETARY_MAILBOX_IN_SIZE;
	}
	else
		CAN_send_sid(DEBUG_PROPULSION_MAILBOX_IN_IS_FULL);
}

static void SECRETARY_mailbox_out_add(CAN_msg_t * msg, MAIL_from_to_e from_to)	//Fonction appelée en IT uniquement !
{
	if(mailbox_out_index_nb < SECRETARY_MAILBOX_OUT_SIZE)
	{
		mailbox_out[mailbox_out_index_write].can_msg = *msg;	//J'écris tranquillement mon message (tant pis si je suis préempté maintenant...)
		mailbox_out[mailbox_out_index_write].from_to = from_to;
		mailbox_out_index_nb++;
		mailbox_out_index_write = (mailbox_out_index_write + 1) % SECRETARY_MAILBOX_OUT_SIZE;
	}
}


static void SECRETARY_mailbox_out_process_main(void)	 //Fonction appelée en tâche de fond
{
	while(mailbox_out_index_nb > 0)
	{
		if(mailbox_out_index_nb == SECRETARY_MAILBOX_OUT_SIZE)
			CAN_send_sid(DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL);

		switch(mailbox_out[mailbox_out_index_read].from_to)
		{
			case TO_CAN:
				SECRETARY_send_canmsg(&mailbox_out[mailbox_out_index_read].can_msg);
				break;
			case TO_UART:
				CANmsgToU1tx(&mailbox_out[mailbox_out_index_read].can_msg);
				break;
			default:
				break;
		}

		TIMER2_disableInt();	//Section critique, pas de préemption maintenant !
			mailbox_out_index_read = (mailbox_out_index_read + 1) % SECRETARY_MAILBOX_OUT_SIZE;
			mailbox_out_index_nb--;
		TIMER2_enableInt();
	}
}


//////////////////////////////////////////////////////////////////
//----------------------FONCTION INTERNE------------------------//
//////////////////////////////////////////////////////////////////

static void SECRETARY_send_report(){
	CAN_msg_t msg;
	msg.sid = STRAT_SEND_REPORT;
	msg.size = 6;
	msg.data[0] = HIGHINT(ODOMETRY_get_total_teta() >> 3);
	msg.data[1] = LOWINT(ODOMETRY_get_total_teta() >> 3);
	msg.data[2] = HIGHINT(ODOMETRY_get_max_total_teta() >> 3);
	msg.data[3] = LOWINT(ODOMETRY_get_max_total_teta() >> 3);
	msg.data[4] = HIGHINT(ODOMETRY_get_total_dist() >> 1);
	msg.data[5] = LOWINT(ODOMETRY_get_total_dist() >> 1);
	CAN_send(&msg);
}

static void SECRETARY_send_callback(CAN_msg_t * can_msg){
	UNUSED_VAR(can_msg);
#ifdef CAN_VERBOSE_MODE
	QS_CAN_VERBOSE_can_msg_print(can_msg, VERB_OUTPUT_MSG);
#endif
}

static bool_e SECRETARY_mailbox_get(Uint8 * index)	 //Fonction appelée en IT uniquement !
{
	if(mailbox_in_index_nb >0)
	{
		*index = mailbox_in_index_read;
		mailbox_in_index_read = (mailbox_in_index_read + 1) % SECRETARY_MAILBOX_IN_SIZE;
		mailbox_in_index_nb--;
		return TRUE;	//Il y a un message à traiter.
	}
	return FALSE;	//Pas de message à traiter.
}

static void SECRETARY_send_pong(void)
{
	CAN_msg_t msg;
	msg.sid = STRAT_PROP_PONG;
	msg.size = 1;
	msg.data[0] = QS_WHO_AM_I_get();
	SECRETARY_send_canmsg(&msg);
}

static void SECRETARY_send_coef(PROPULSION_coef_e i)
{
	const char * PROPULSION_coefs_strings[PROPULSION_NUMBER_COEFS] = {
																		"ODOMETRY_COEF_TRANSLATION",
																		"ODOMETRY_COEF_SYM",
																		"ODOMETRY_COEF_ROTATION",
																		"ODOMETRY_COEF_CENTRIFUGAL",
																		"CORRECTOR_COEF_KP_TRANSLATION",
																		"CORRECTOR_COEF_KD_TRANSLATION",
																		"CORRECTOR_COEF_KV_TRANSLATION",
																		"CORRECTOR_COEF_KA_TRANSLATION",
																		"CORRECTOR_COEF_KP_ROTATION",
																		"CORRECTOR_COEF_KD_ROTATION",
																		"CORRECTOR_COEF_KV_ROTATION",
																		"CORRECTOR_COEF_KA_ROTATION",
																		"GYRO_COEF_GAIN"};

	CAN_msg_t msg;
	Sint32 coef;
	if(i < PROPULSION_NUMBER_COEFS)
	{
		if(i <= ODOMETRY_COEF_CENTRIFUGAL)
			coef = ODOMETRY_get_coef(i);
		else
			coef = CORRECTOR_get_coef(i);
		msg.sid = DEBUG_PROPULSION_COEF_IS;
		msg.data[0] = i;
		msg.data[1] = (Uint8)((coef >> 24) & 0xFF);
		msg.data[2] = (Uint8)((coef >> 16) & 0xFF);
		msg.data[3] = (Uint8)((coef >> 8) & 0xFF);
		msg.data[4] = (Uint8)((coef 	) & 0xFF);
		msg.size = 5;
		debug_printf("Coef %d:%s is %ld\n",i,PROPULSION_coefs_strings[i],coef);
		SECRETARY_send_canmsg(&msg);
	}
	else
	{
		debug_printf("WARNING : Coef %d > PROPULSION_NUMBER_COEFS=%d !\n",i,PROPULSION_NUMBER_COEFS);
	}
}

static void SECRETARY_send_all_coefs(void)
{
	PROPULSION_coef_e i;
	for(i=(PROPULSION_coef_e)(0);i<PROPULSION_NUMBER_COEFS;i++)
		SECRETARY_send_coef(i);
}
