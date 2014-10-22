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
#include "QS/QS_maths.h"

//Ne doit pas �tre trop petit dans le cas de courbe multipoint assez grande: on doit pouvoir contenir tous les messages CAN qu'on re�oit en 5ms dans ce buffer
#define SECRETARY_MAILBOX_SIZE (32)

static void SECRETARY_send_report();

void SECRETARY_process_CANmsg(CAN_msg_t * msg);
void SECRETARY_mailbox_add(CAN_msg_t * msg);
volatile static Uint8 index_read;
volatile static Uint8 index_write;
volatile static Uint8 index_nb;

CAN_msg_t mailbox[SECRETARY_MAILBOX_SIZE];  //Les messages CAN re�us dans le main sont ajout� � la mailbox et trait�s en IT...
//Cela permet d'�viter les nombreux probl�mes li�s � la pr�emption (notamment li�s au buffer d'ordres... dont les fonctions ne peuvent �tre r�entraintes)


void SECRETARY_init(void)
{
	#ifdef ENABLE_CAN
		CAN_init();
	#endif
	index_read = 0;
	index_write = 0;
}

void SECRETARY_process_main(void)
{

	static CAN_msg_t receivedCanMsg_over_uart;
	#ifdef ENABLE_CAN
	static CAN_msg_t received_msg;
		// gestion du CAN conforme QS
		while(CAN_data_ready())
		{
				received_msg = CAN_get_next_msg();
				SECRETARY_mailbox_add(&received_msg);
		}
	#endif

	while(UART1_data_ready())
	{
		LED_UART = !LED_UART;
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
					SECRETARY_mailbox_add(&receivedCanMsg_over_uart);
			}
		#else
			if(u1rxToCANmsg(&receivedCanMsg_over_uart))
				SECRETARY_mailbox_add(&receivedCanMsg_over_uart);
		#endif
	}
}


void SECRETARY_mailbox_add(CAN_msg_t * msg) //Fonction appel�e en t�che de fond uniquement !
{
	if(index_nb < SECRETARY_MAILBOX_SIZE)
	{
		mailbox[index_write] = *msg;	//J'�cris tranquillement mon message (tant pis si je suis pr�empt� maintenant...)
		TIMER1_disableInt();
			index_nb++; //Il ne faut pas que la pr�emption ait lieu maintenant !
		TIMER1_enableInt();
		index_write = (index_write + 1) % SECRETARY_MAILBOX_SIZE;
	}

}

bool_e SECRETARY_mailbox_get(Uint8 * index)	 //Fonction appel�e en IT uniquement !
{
	if(index_nb >0)
	{
		*index = index_read;
		index_read = (index_read + 1) % SECRETARY_MAILBOX_SIZE;
		index_nb--;
		return TRUE;	//Il y a un message � traiter.
	}
	return FALSE;	//Pas de message � traiter.
}

void SECRETARY_process_it(void)
{
	Uint8 index;
	while(SECRETARY_mailbox_get(&index))  //Tant qu'il y a un message � traiter...
	{
		SECRETARY_process_CANmsg(&mailbox[index]);  //Traitement du message CAN.
	}
}



void SECRETARY_send_canmsg(CAN_msg_t * msg)
{

	Uint8 i;
	for(i=msg->size;i<8;i++)
		msg->data[i]=0xFF;		//On remplace les donn�es hors size par des FF (notamment pour verbose et envoi sur uart)

	#ifdef ENABLE_CAN
		CAN_send(msg);
	#else
		#warning "Messages can d�sactiv�s"
	#endif

	#ifdef CAN_SEND_OVER_UART
		CANmsgToU1tx(msg);
	#endif

	#ifdef VERBOSE_MSG_SEND_OVER_UART
		bool_e add_pos_datas;
		add_pos_datas = TRUE;	//On suppose qu'il faut ajouter les donn�es de position.
		switch(msg->sid)
		{
			case BROADCAST_POSITION_ROBOT:
				debug_printf("Pos:");
			break;
			case STRAT_TRAJ_FINIE:
				debug_printf("TrajFinie:");
			break;
			case STRAT_PROP_ERREUR:
				debug_printf("Err:0x%x", msg->data[7]&0b111);
			break;
			case STRAT_ROBOT_FREINE:
				debug_printf("Freine:");
			break;
						//case PROP_SELFTEST:
						//break;
						case PROP_ROBOT_CALIBRE:
						break;
			case STRAT_PROP_PONG:
				debug_printf("Pong\n");
				add_pos_datas = FALSE;
				break;
			case STRAT_PROP_SELFTEST_DONE:
				debug_printf("Selftest done :\n");
				for(i=0;i<msg->size;i++)
				{
					switch(msg->data[i])
					{
						case SELFTEST_NO_ERROR:																											break;
						case SELFTEST_PROP_FAILED:			debug_printf(" | error %x SELFTEST_PROP_FAILED\n"			,SELFTEST_PROP_FAILED);			break;
						case SELFTEST_PROP_HOKUYO_FAILED:	debug_printf(" | error %x SELFTEST_PROP_HOKUYO_FAILED\n"	,SELFTEST_PROP_HOKUYO_FAILED);	break;
						case SELFTEST_PROP_IN_SIMULATION_MODE:	debug_printf(" | error %x SELFTEST_PROP_IN_SIMULATION_MODE\n"	,SELFTEST_PROP_IN_SIMULATION_MODE); break;
						case SELFTEST_PROP_IN_LCD_TOUCH_MODE:	debug_printf(" | error %x SELFTEST_PROP_IN_LCD_TOUCH_MODE\n"	,SELFTEST_PROP_IN_LCD_TOUCH_MODE); 	break;
						default:							debug_printf(" | error %x UNKNOW_ERROR you should add it in secretaty.c\n", msg->data[i]);				break;
					}
				}
				add_pos_datas = FALSE;
				break;
			case DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE:
				debug_printf ("Trajectory_for_test_coef_done : %d", U16FROMU8(msg->data[0], msg->data[1]) );
				add_pos_datas = FALSE;
				break;
			case BROADCAST_ADVERSARIES_POSITION:
				//Nothing. affichage d�j� g�r� dans la fonction appelante.
				add_pos_datas = FALSE;
				break;
			case DEBUG_PROPULSION_COEF_IS:
				add_pos_datas = FALSE;
				break;
			case STRAT_PROP_FOE_DETECTED:
				debug_printf("FOE Detected !\n");
				add_pos_datas = TRUE; // Permet de savoir ou est-ce que la d�tection � eu lieu sur le terrain
				break;
			default:
				debug_printf("SID=%x ", msg->sid);
			break;
		}
		if(add_pos_datas)
			debug_printf(" x=%d y=%d t=%d\n", global.position.x, global.position.y, global.position.teta);
	#endif
}

volatile bool_e selftest_validated = FALSE;

bool_e SECRETARY_is_selftest_validated(void)
{
	return selftest_validated;
}

//si result == TRUE : le selftest s'est bien d�roul�, sinon on remonte l'erreur � la strat�gie.
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

	#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT	//L'odom�trie est faite sur un robot virtuel parfait.
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
	/*		0 : ADVERSARY_NUMBER	//de 0 � n, il peut y avoir plus de deux adversaires si l'on inclut notre ami...
	 * 		1 :  x [2cm]
	 * 		2 :  y [2cm]
	 * 		3-4 : teta
	 * 		5 : distance [2cm]
	 * 		6 : fiability	:    "0 0 0 0 d t y x" (distance, teta, y, x) : 1 si fiable, 0 sinon.
	 */
	msg.sid = BROADCAST_ADVERSARIES_POSITION;
	msg.data[0] = adversary_number | ((it_is_the_last_adversary)?IT_IS_THE_LAST_ADVERSARY:0);	//n� de l'ADVERSARY  + bit de poids fort si c'est le dernier adversaire
	msg.data[1] = x/20;	//X [2cm]
	msg.data[2] = y/20;	//Y [2cm]
	msg.data[3] = HIGHINT(teta);	//teta
	msg.data[4] = LOWINT(teta);	//teta
	msg.data[5] = distance/20;	//distance [2cm]
	msg.data[6] = fiability;	//fiability : x et y fiables
	msg.size = 7;
	SECRETARY_send_canmsg(&msg);
	#ifdef VERBOSE_MSG_SEND_OVER_UART
		//debug_printf("Adv%d\t%4d\t%4d\t%5d\t%4d\n%s",adversary_number,x,y,teta,distance,((it_is_the_last_adversary)?"\n":""));
	#endif
}


void SECRETARY_send_pong(void)
{
	CAN_msg_t msg;
	msg.sid = STRAT_PROP_PONG;
	msg.size = 1;
	msg.data[0] = QS_WHO_AM_I_get();
	SECRETARY_send_canmsg(&msg);
}


void SECRETARY_process_send(Uint11 sid, Uint8 reason, SUPERVISOR_error_source_e error_source)	//La raison de l'envoi est d�finie dans avertisseur.h
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


void SECRETARY_send_coef(PROPULSION_coef_e i)
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

void SECRETARY_send_all_coefs(void)
{
	PROPULSION_coef_e i;
	for(i=(PROPULSION_coef_e)(0);i<PROPULSION_NUMBER_COEFS;i++)
		SECRETARY_send_coef(i);
}

void SECRETARY_send_foe_detected(Uint16 x, Uint16 y, bool_e timeout){
	CAN_msg_t msg;
		msg.sid = STRAT_PROP_FOE_DETECTED;
		msg.size = 5;
		msg.data[0] = HIGHINT(x);
		msg.data[1] = LOWINT(x);
		msg.data[2] = HIGHINT(y);
		msg.data[3] = LOWINT(y);
		msg.data[4] = timeout;
	SECRETARY_send_canmsg(&msg);
}


/*
types d'ordres

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
		.... 11.. > asservissement d�sactiv� !!!

	octet de VITESSE
		0x00 : rapide
		0x01 : lent
		0x02 : tr�s lent
		....
		0x08 � 0xFF : vitesse "analogique"

	octet de MARCHE
		...0 ...0 > marche avt ou arri�re
		...0 ...1 > marche avant oblig�
		...1 ...0 > marche arri�re oblig�e
		...1 ...1 > marche avt ou arri�re

	octet de RAYONCRB
		.... ...0 > courbe automatique d�sactiv� (choix en propulsion)
		.... ...1 > courbe automatique activ�
		xxxx .... > type d'�vitement sur la trajectoire



		case PROP_TELL_POSITION:		RIEN
		case PROP_STOP:					RIEN
		case PROP_SET_POSITION:			x, y, teta
		case PROP_RUSH_IN_THE_WALL:	0.CONFIG, 5.VITESSE, 6.MARCHE			//TODO !!!!!

		case PROP_CALIBRATION:			RIEN (la couleur doit etre bonne...)
		case PROP_WARN_ANGLE:			teta
		case PROP_WARN_X:				x
		case PROP_WARN_Y:				y

*/



void SECRETARY_process_CANmsg(CAN_msg_t* msg)
{
	way_e sens_marche;
	LED_CAN=!LED_CAN;
//	debug_printf("#%x\n",msg->sid);

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
			debug_printf("Receveid new go_position : x -> %d, y -> %d\n", U16FROMU8(msg->data[1],msg->data[2]), U16FROMU8(msg->data[3],msg->data[4]));

			//R�glage sens:
			if ((msg->data[6] == BACKWARD) || (msg->data[6] == FORWARD))
				sens_marche = msg->data[6];	//LE SENS EST impos�
			else
				sens_marche = ANY_WAY;	//ON SE FICHE DU SENS

			ROADMAP_add_order(  ((msg->data[7]&0x0F) != 0)?TRAJECTORY_AUTOMATIC_CURVE:TRAJECTORY_TRANSLATION,
								(U16FROMU8(msg->data[1],msg->data[2])),	//x
								(U16FROMU8(msg->data[3],msg->data[4])),	//y
								0,									//teta
								(msg->data[0] & 0x01)?RELATIVE:NOT_RELATIVE,	//relative
								(msg->data[0] & 0x10)?NOT_NOW:NOW,//maintenant
								sens_marche,						//sens de marche
								NOT_BORDER_MODE,					//mode bordure
								(msg->data[0] & 0x20)?MULTIPOINT:NO_MULTIPOINT, //mode multipoints
								msg->data[5],						//Vitesse
								ACKNOWLEDGE_ASKED,
								(corrector_e)((msg->data[0] & 0x0C)>>2),
								(avoidance_e)((msg->data[7] & 0xF0)>>4)
							);
		break;

		case PROP_CALIBRATION:	//Autocalage !
			SEQUENCES_calibrate();

		break;

		// Modifie la position de d�part en fonction de la couleur
		case BROADCAST_COULEUR :
			//Le type couleur est normalis� en QS...
			ODOMETRY_set_color((color_e)(msg->data[0]));
			//Propagation sur UART (pour un �ventuel robot virtuel branch� sur notre UART)
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
			SEQUENCES_rush_in_the_wall((U16FROMU8(msg->data[2],msg->data[3])), msg->data[0], SLOW_TRANSLATION_AND_FAST_ROTATION, ACKNOWLEDGE_ASKED, 0, 0, BORDER_MODE, (msg->data[1])?CORRECTOR_ENABLE:CORRECTOR_TRANSLATION_ONLY);	//BORDER_MODE = sans mise � jour de position odom�trie !
		break;

		//Stop tout
		case BROADCAST_STOP_ALL:
			debug_printf("Broadcast_stop\n");
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
			// d�sactivation de tout les avertisseurs.
			WARNER_init();
			//arret robot
			SUPERVISOR_state_machine(EVENT_BROADCAST_STOP, 0);
			SECRETARY_send_report();
		break;

		case BROADCAST_START:
			debug_printf("Broadcast_start, \n");
			global.match_started = TRUE;
		break;

		case BROADCAST_ALIM :
			if(msg->data[0] == ALIM_OFF){
				component_printf(LOG_LEVEL_Info, "C:BROADCAST_ALIM -> ALIM_OFF\n");
				global.alim = FALSE;
			}else if(msg->data[0] == ALIM_ON){
				component_printf(LOG_LEVEL_Info, "C:BROADCAST_ALIM -> ALIM_ON\n");
				global.alim = TRUE;
			}
			global.alim_value = (((Uint16)(msg->data[1]) << 8) & 0xFF00) | ((Uint16)(msg->data[2]) & 0x00FF);
			break;

		//Carte strat�gie demande la position
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
						//dur�e d'activation du joystick en ms
		break;

		//REGLAGES DES COEFFICIENTS !!!!!!!!
		case DEBUG_PROPULSION_SET_ACCELERATION:
			if(U16FROMU8(msg->data[0], msg->data[1]) < 200)	//S�curit�...
				PILOT_set_coef(PILOT_ACCELERATION_NORMAL, (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;

		case DEBUG_PROPULSION_GET_COEFS:
			SECRETARY_send_all_coefs();
		break;
		case DEBUG_PROPULSION_SET_COEF:
			if(msg->data[0] < PROPULSION_NUMBER_COEFS && msg->size == 5)
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
			//On a re�u une position robot... on la prend... et on ne bouge plus par nous m�me.
			global.disable_virtual_perfect_robot = TRUE;
			ODOMETRY_set(
							(U16FROMU8(msg->data[0],msg->data[1]))&0x1FFF,	//x
							(U16FROMU8(msg->data[2],msg->data[3]))&0x1FFF, //y
							(U16FROMU8(msg->data[4],msg->data[5]))	//teta
						);
		break;
		#endif
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			DETECTION_new_adversary_position(msg, NULL, 0);
			break;

		case BROADCAST_ADVERSARIES_POSITION:
			#ifdef SIMULATION_VIRTUAL_PERFECT_ROBOT
				DETECTION_new_adversary_position(msg, NULL, 0);
			#endif
			break;
		default :
		break;
	}

}

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
