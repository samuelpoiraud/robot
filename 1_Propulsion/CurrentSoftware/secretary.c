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
#include "warner.h"
#include "corrector.h"
#include "odometry.h"
#include "roadmap.h"
#include "sequences.h"
#include "copilot.h"
#include "supervisor.h"
#include "joystick.h"
#include "scan_triangle.h"
#include "LCDTouch/LCD.h"

//Ne doit pas �tre trop petit dans le cas de courbe multipoint assez grande: on doit pouvoir contenir tous les messages CAN qu'on re�oit en 5ms dans ce buffer
#define SECRETARY_MAILBOX_SIZE (32)

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
				SECRETARY_mailbox_add(&receivedCanMsg_over_uart);
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



void SECRETARY_send_canmsg(Uint16 sid, Uint8* data,Uint8 taille)
{
	CAN_msg_t msg;
	msg.sid=sid;
	msg.size=taille;
	Uint8 i;

	/* recopie les data */
	for(i=0;i<taille;i++)
		msg.data[i]=data[i];
	for(;i<8;i++)
		msg.data[i]=0xFF;

	#ifdef ENABLE_CAN
		CAN_send(&msg);
	#else
		#warning "Messages can d�sactiv�s"
	#endif

	#ifdef CAN_SEND_OVER_UART
		CANmsgToU1tx(&msg);
	#endif

	#ifdef VERBOSE_MSG_SEND_OVER_UART
		switch(sid)
		{
			case BROADCAST_POSITION_ROBOT:
				debug_printf("Pos:");
			break;
			case CARTE_P_TRAJ_FINIE:
				debug_printf("TrajFinie:");
			break;
			case CARTE_P_ASSER_ERREUR:
				debug_printf("Err:0x%x", msg.data[7]&0b111);
			break;
			case CARTE_P_ROBOT_FREINE:
				debug_printf("Freine:");
			break;
						//case ASSER_SELFTEST:
						//break;
						case CARTE_P_ROBOT_CALIBRE:
						break;
			case DEBUG_ASSER_POINT_FICTIF:
				debug_printf("PF:");
				break;
			default:
				debug_printf("SID=%x ", msg.sid);
			break;
		}
		debug_printf(" x=%d y=%d t=%d\n", global.position.x, global.position.y, global.position.teta);
	#endif
}

//si result == TRUE : le selftest s'est bien d�roul�, sinon on remonte l'erreur � la strat�gie.
void SECRETARY_send_selftest_result(bool_e result)
{
	CAN_msg_t msg;
	Uint8 i;
	msg.sid = STRAT_PROP_SELFTEST_DONE;
	msg.size = 8;
	msg.data[0] = (result)?SELFTEST_NO_ERROR:SELFTEST_PROP_FAILED;
	for(i=1;i<8;i++)
		msg.data[i] = SELFTEST_NO_ERROR;

	//TODO enrichir ce message du statut de fonctionnement des autres p�riphs... hokuyo....
	CAN_send(&msg);
}

//x : mm, y : mm, teta : rad4096, distance : mm
void SECRETARY_send_adversary_position(Uint8 adversary_number, Uint16 x, Uint16 y, Sint16 teta, Uint16 distance, Uint8 fiability)
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
	msg.sid = STRAT_ADVERSARIES_POSITION;
	msg.data[0] = adversary_number;	//ADVERSARY 0
	msg.data[1] = x/20;	//X [2cm]
	msg.data[2] = y/20;	//Y [2cm]
	msg.data[3] = HIGHINT(teta);	//teta
	msg.data[4] = LOWINT(teta);	//teta
	msg.data[5] = distance/20;	//distance [2cm]
	msg.data[6] = 0x0F;	//fiability : x et y fiables
	msg.size = 7;
	CAN_send(&msg);
}

void SECRETARY_send_pong(void)
{
	CAN_msg_t msg;
	msg.sid = STRAT_PROP_PONG;
	msg.size = 1;
	msg.data[0] = QS_WHO_AM_I_get();
	CAN_send(&msg);
}


void SECRETARY_process_send(Uint11 sid, Uint8 reason, SUPERVISOR_error_source_e error_source)	//La raison de l'envoi est d�finie dans avertisseur.h
{
	Uint8 tabTemp[8];
	Uint8 error_byte;
	Sint32 rot_speed;
	Uint8 trajectory_status; //3 bits

	trajectory_status = (global.vitesse_translation != 0) << 2 | (global.vitesse_rotation != 0) << 1;
	error_byte = ((Uint8)(trajectory_status) << 5) | (Uint8)(COPILOT_get_way()) << 3 | (Uint8)(error_source & 0x07);

	tabTemp[0] = (HIGHINT(global.position.x) & 0x1F) | (((global.real_speed_translation>>10)/5) << 5);	//Vitesse sur 3 bits forts, en [250mm/s]
	tabTemp[1] = LOWINT(global.position.x);
	rot_speed = ((absolute(global.real_speed_rotation)>>10)*200)>>12;
	if(rot_speed > 7)
		rot_speed = 7;	//ecretage pour tenir sur 3 bits
	tabTemp[2] = (HIGHINT(global.position.y) & 0x1F) | ((Uint8)(rot_speed) << 5);	//Vitesse angulaire en radians
	tabTemp[3] = LOWINT(global.position.y);
	tabTemp[4] = HIGHINT(global.position.teta);
	tabTemp[5] = LOWINT(global.position.teta);
	tabTemp[6] = reason;
	tabTemp[7] = error_byte;	//Octet d'erreur... voir warner.c qui rempli cet octet d'erreur...
			/*	Octet d'erreur :   0bTTTWWEEE
								 TTT = trajectory_e
								 WW  = way_e
								 EEE = SUPERVISOR_error_source_e
									*/
	SECRETARY_send_canmsg(sid,tabTemp,8);

}


void SECRETARY_send_position(robots_e robot_selected, Sint16 x, Sint16 y, Sint16 d, Sint16 teta){
	Uint8 data[8];

	switch(robot_selected){
			case FRIEND_2:
				//force pos friend 2
				data[0] = x/2;	//X [2cm]
				data[1] = y/2;	//Y [2cm]
				SECRETARY_send_canmsg(STRAT_FRIEND_FORCE_POSITION, data, 2);
				break;
			case ADVERSARY_1:
				data[0] = 0;	//ADVERSARY 0
				data[1] = x/2;	//X [2cm]
				data[2] = y/2;	//Y [2cm]
				data[3] = HIGHINT(teta);	//teta
				data[4] = LOWINT(teta);		//teta
				data[5] = d;	//distance
				data[6] = 0x03;	//fiability : x et y fiables
				SECRETARY_send_canmsg(STRAT_ADVERSARIES_POSITION, data, 7);
				break;

			case ADVERSARY_2:
				//force pos adversaries
				/*		0 : ADVERSARY_NUMBER	//de 0 � n, il peut y avoir plus de deux adversaires si l'on inclut notre ami...
				 * 		1 :  x [2cm]
				 * 		2 :  y [2cm]
				 * 		3-4 : teta
				 * 		5 : distance [2cm]
				 * 		6 : fiability	:    "0 0 0 0 d t y x" (distance, teta, y, x) : 1 si fiable, 0 sinon.
				 */

				data[0] = 1;	//ADVERSARY 1
				data[1] = x/2;	//X [2cm]
				data[2] = y/2;	//Y [2cm]
				data[3] = HIGHINT(teta);	//teta
				data[4] = LOWINT(teta);		//teta
				data[5] = d;	//distance
				data[6] = 0x03;	//fiability : x et y fiables
				SECRETARY_send_canmsg(STRAT_ADVERSARIES_POSITION, data, 7);

				break;
			default:
				break;
	}
}



/*
types d'ordres

ASSER_GO_ANGLE
ASSER_GO_POSITION

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



		case ASSER_TELL_POSITION:		RIEN
		case CARTE_ASSER_FIN_ERREUR:	RIEN
		case ASSER_STOP:				RIEN
		case ASSER_SET_POSITION:		x, y, teta
		case ASSER_TYPE_ASSERVISSEMENT:	mode d'asser rot et trans
		case ASSER_RUSH_IN_THE_WALL:	0.CONFIG, 5.VITESSE, 6.MARCHE			//TODO !!!!!

		case ASSER_CALIBRATION:			RIEN (la couleur doit etre bonne...)
		case ASSER_WARN_ANGLE:			teta
		case ASSER_WARN_X:				x
		case ASSER_WARN_Y:				y

*/



void SECRETARY_process_CANmsg(CAN_msg_t* msg)
{
	way_e sens_marche;
	LED_CAN=!LED_CAN;
//	debug_printf("#%x\n",msg->sid);

	switch (msg->sid)
	{
		case ASSER_STOP :
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
								CORRECTOR_ENABLE
							);
		break;

		case ASSER_GO_ANGLE:

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
								(corrector_e)((msg->data[0] & 0x0C)>>2)
							);
		break;

		case ASSER_GO_POSITION:

			//R�glage sens:
			if ((msg->data[6] == BACKWARD) || (msg->data[6] == FORWARD))
				sens_marche = msg->data[6];	//LE SENS EST impos�
			else
				sens_marche = ANY_WAY;	//ON SE FICHE DU SENS

			ROADMAP_add_order(  	(msg->data[7] !=0)?TRAJECTORY_AUTOMATIC_CURVE:TRAJECTORY_TRANSLATION,
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
								(corrector_e)((msg->data[0] & 0x0C)>>2)
							);
		break;

		case ASSER_CALIBRATION:	//Autocalage !
			if (msg->data[0] == FORWARD) //ON IMPOSE L'autocalage AVANT
				SEQUENCES_calibrate(FORWARD,msg->data[1]);
			else	//Par d�faut, l'autocallage est arri�re.
				SEQUENCES_calibrate(BACKWARD,msg->data[1]);

		break;

		// Modifie la position de d�part en fonction de la couleur
		case BROADCAST_COULEUR :
			//Le type couleur est normalis� en QS...
			ODOMETRY_set_color((color_e)(msg->data[0]));
			SECRETARY_process_send(BROADCAST_POSITION_ROBOT, WARNING_NO, 0);
		break;

		// Impose une position (uniquement pour les tests !!!)
		case ASSER_SET_POSITION:
			ODOMETRY_set(
							(U16FROMU8(msg->data[0],msg->data[1])),	//x
							(U16FROMU8(msg->data[2],msg->data[3])), 	//y
							(U16FROMU8(msg->data[4],msg->data[5]))	//teta
						);
			COPILOT_reset_absolute_destination();

		break;
		case ASSER_RUSH_IN_THE_WALL:
			SEQUENCES_rush_in_the_wall((U16FROMU8(msg->data[2],msg->data[3])), msg->data[0], ACKNOWLEDGE_ASKED, 0, 0, BORDER_MODE, (msg->data[1])?CORRECTOR_ENABLE:CORRECTOR_TRANSLATION_ONLY);	//BORDER_MODE = sans mise � jour de position odom�trie !
		break;

		//Stop tout
		case BROADCAST_STOP_ALL:
			// d�sactivation de tout les avertisseurs.
			WARNER_init();
			//arret robot
			SUPERVISOR_state_machine(EVENT_BROADCAST_STOP, 0);
		break;

		//Carte strat�gie demande la position
		case ASSER_TELL_POSITION:
			SECRETARY_process_send(BROADCAST_POSITION_ROBOT, WARNING_NO, 0);
		break;

		//Une carte nous demande de l'avertir lorsque nous serons en approche d'une position...
		case ASSER_WARN_ANGLE:
			WARNER_arm_teta(U16FROMU8(msg->data[0],msg->data[1]));
		break;
		case ASSER_WARN_X:
			WARNER_arm_x(U16FROMU8(msg->data[0],msg->data[1]));
		break;
		case ASSER_WARN_Y:
			WARNER_arm_y(U16FROMU8(msg->data[0],msg->data[1]));
		break;
		case ASSER_SEND_PERIODICALLY_POSITION:
			WARNER_arm_timer(U16FROMU8(msg->data[0],msg->data[1]));
			WARNER_arm_translation(U16FROMU8(msg->data[2],msg->data[3]));
			WARNER_arm_rotation(U16FROMU8(msg->data[4],msg->data[5]));
		break;

		case ASSER_JOYSTICK:
			JOYSTICK_enable(100, (Sint8)msg->data[0], (Sint8)msg->data[1], (bool_e)msg->data[2], (bool_e)msg->data[3]);
						//dur�e d'activation du joystick en ms
		break;

		//REGLAGES DES COEFFICIENTS !!!!!!!!
		case DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_SYM:
			ODOMETRY_set_coef(ODOMETRY_COEF_SYM,(Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_ROTATION:
			ODOMETRY_set_coef(ODOMETRY_COEF_ROTATION, (Sint32)(U32FROMU8(msg->data[0], msg->data[1], msg->data[2], msg->data[3])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_TRANSLATION:
			ODOMETRY_set_coef(ODOMETRY_COEF_TRANSLATION, (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_CENTRIFUGE:
			ODOMETRY_set_coef(ODOMETRY_COEF_CENTRIFUGAL,(Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_KP_ROTATION:
			CORRECTOR_set_coef(CORRECTOR_COEF_KP_ROTATION, (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_KD_ROTATION:
			CORRECTOR_set_coef(CORRECTOR_COEF_KD_ROTATION,  (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_KP_TRANSLATION:
			CORRECTOR_set_coef(CORRECTOR_COEF_KP_TRANSLATION,  (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_KD_TRANSLATION:
			CORRECTOR_set_coef(CORRECTOR_COEF_KD_TRANSLATION,  (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_KV_ROTATION:
			CORRECTOR_set_coef(CORRECTOR_COEF_KV_ROTATION,  (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case DEBUG_PROPULSION_REGLAGE_COEF_KV_TRANSLATION:
			CORRECTOR_set_coef(CORRECTOR_COEF_KV_TRANSLATION,  (Sint32)(U16FROMU8(msg->data[0], msg->data[1])));
		break;
		case CARTE_ASSER_FIN_ERREUR:
			SUPERVISOR_state_machine(EVENT_ERROR_EXIT, 0);
		break;
		case PROP_PING:
			SECRETARY_send_pong();
		break;
		case PROP_DO_SELFTEST:
			SEQUENCES_selftest();
		break;
		#ifdef SCAN_TRIANGLE
		case ASSER_LAUNCH_SCAN_TRIANGLE :
			SCAN_TRIANGLE_canMsg();
		break;
		#endif

		default :
		break;
	}

}
