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
#include "QS/QS_uart.h"
#include "warner.h"
#include "corrector.h"
#include "odometry.h"
#include "roadmap.h"
#include "sequences.h"
#include "copilot.h"
#include "supervisor.h"
#include "joystick.h"
#include <timer.h>

#define SECRETARY_MAILBOX_SIZE (8)

void SECRETARY_process_CANmsg(CAN_msg_t * msg);
void SECRETARY_mailbox_add(CAN_msg_t * msg);
volatile static Uint8 index_read;
volatile static Uint8 index_write;
volatile static Uint8 index_nb;

CAN_msg_t mailbox[SECRETARY_MAILBOX_SIZE];  //Les messages CAN reçus dans le main sont ajouté à la mailbox et traités en IT...
//Cela permet d'éviter les nombreux problèmes liés à la préemption (notamment liés au buffer d'ordres... dont les fonctions ne peuvent être réentraintes)


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

	if(UART1_data_ready())
	{
		LED_UART = !LED_UART;
		if(u1rxToCANmsg(&receivedCanMsg_over_uart))
				SECRETARY_mailbox_add(&receivedCanMsg_over_uart);
	}
}


void SECRETARY_mailbox_add(CAN_msg_t * msg) //Fonction appelée en tâche de fond uniquement !
{
	if(index_nb < SECRETARY_MAILBOX_SIZE)
	{
		mailbox[index_write] = *msg;	//J'écris tranquillement mon message (tant pis si je suis préempté maintenant...)
		DisableIntT1;
			index_nb++; //Il ne faut pas que la préemption ait lieu maintenant !
		EnableIntT1;
		index_write = (index_write + 1) % SECRETARY_MAILBOX_SIZE;
	}

}

bool_e SECRETARY_mailbox_get(Uint8 * index)	 //Fonction appelée en IT uniquement !
{
	if(index_nb >0)
	{
		*index = index_read;
		index_read = (index_read + 1) % SECRETARY_MAILBOX_SIZE;
		index_nb--;
		return TRUE;	//Il y a un message à traiter.
	}
	return FALSE;	//Pas de message à traiter.
}

void SECRETARY_process_it(void)
{
	Uint8 index;
	while(SECRETARY_mailbox_get(&index))  //Tant qu'il y a un message à traiter...
	{
		SECRETARY_process_CANmsg(&mailbox[index]);  //Traitement du message CAN.
	}
}


void SECRETARY_CAN_send(Uint16 sid, Uint8* data,Uint8 taille)
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
		#warning "Messages can désactivés"
	#endif
	
	#ifdef CAN_SEND_OVER_UART
		CANmsgToU1tx(&msg);
	#endif

	#ifdef VERBOSE_MSG_SEND_OVER_UART
		switch(sid)
		{
			case CARTE_P_POSITION_ROBOT:
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
						case ASSER_SELFTEST:
						break;
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


void SECRETARY_process_send(Uint11 sid, Uint8 reason, SUPERVISOR_error_source_e error_source)	//La raison de l'envoi est définie dans avertisseur.h
{
	Uint8 tabTemp[8];
	Uint8 error_byte;
	Sint32 rot_speed;
	error_byte = ((Uint8)(COPILOT_get_trajectory()) << 5 | (Uint8)(COPILOT_get_way())) << 3 | (Uint8)(error_source & 0x07);

	tabTemp[0] = (HIGHINT(global.position.x) & 0x1F) | (((global.real_speed_translation>>10)/5) << 5);	//Vitesse sur 3 bits forts, en [250mm/s]
	tabTemp[1] = LOWINT(global.position.x);
	rot_speed = ((abs(global.real_speed_rotation)>>10)*200)>>12;
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
	#warning "le code stratégie ne prend pas encore correctement cet octet d'erreur. Merci de leur rappeler... avant d'enlever ce warning"
	#warning "ils doivent notamment intégrer la récup des infos de way_e et trajectory a la place du code actuel qui bricole des vitesses d'erreurs pour déduire du déplacement du robot avant l'erreur l'action a effectuer pour se sortir du blocage !"
	
	SECRETARY_CAN_send(sid,tabTemp,8);
	
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
			
			//Réglage sens:		
			if ((msg->data[6] == BACKWARD) || (msg->data[6] == FORWARD))
				sens_marche = msg->data[6];	//LE SENS EST imposé
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
			else	//Par défaut, l'autocallage est arrière.
				SEQUENCES_calibrate(BACKWARD,msg->data[1]);

		break;
		
		//demande de selftest pour la propulsion
		case SUPER_ASK_ASSER_SELFTEST:
			//SELFTEST désactivé, car l'autocallage qui y était fait dépend maintenant de la stratégie !!!
		
		break;
			
		// Modifie la position de départ en fonction de la couleur 
		case BROADCAST_COULEUR :
			//Le type couleur est normalisé en QS...
			ODOMETRY_set_color((color_e)(msg->data[0]));
			SECRETARY_process_send(CARTE_P_POSITION_ROBOT, WARNING_NO, 0);
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
			SEQUENCES_rush_in_the_wall((U16FROMU8(msg->data[2],msg->data[3])), msg->data[0], ACKNOWLEDGE_ASKED, 0, 0, BORDER_MODE, (msg->data[1])?CORRECTOR_ENABLE:CORRECTOR_TRANSLATION_ONLY);	//BORDER_MODE = sans mise à jour de position odométrie !
		break;
		
		//Stop tout
		case BROADCAST_STOP_ALL:
			// désactivation de tout les avertisseurs.
			WARNER_init();
			//arret robot
			SUPERVISOR_state_machine(EVENT_BROADCAST_STOP, 0);
		break;		

		//Carte stratégie demande la position
		case ASSER_TELL_POSITION:
			SECRETARY_process_send(CARTE_P_POSITION_ROBOT, WARNING_NO, 0);
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
						//durée d'activation du joystick en ms
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
		default :
		break;
	}
			
}
