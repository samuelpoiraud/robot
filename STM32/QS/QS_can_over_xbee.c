/*
 *	Club Robot ESEO 2011-2012
 *	Shark & Fish
 *
 *	Fichier : QS_can_over_xbee.c
 *	Package : Qualité Soft
 *	Description : fonctions d'encapsulation des messages CAN
					pour envoi via module XBEE configuré en mode API
 *	Auteur : Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20120224
 */


#define QS_CAN_OVER_XBEE_C
#include "QS_can_over_xbee.h"
#ifdef USE_XBEE
#include "QS_uart.h"
#include "QS_CANmsgList.h"
#include "QS_outputlog.h"

#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif

	#include <stdio.h>


	#ifdef XBEE_PLUGGED_ON_UART1
		#define XBee_putc(c) UART1_putc(c)
	#endif
	#ifdef XBEE_PLUGGED_ON_UART2
		#define XBee_putc(c) UART2_putc(c)
	#endif



	#ifdef XBEE_PLUGGED_ON_UART1
		#define XBee_data_ready() UART1_data_ready()
	#endif
	#ifdef XBEE_PLUGGED_ON_UART2
		#define XBee_data_ready() UART2_data_ready()
	#endif


	#ifdef XBEE_PLUGGED_ON_UART1
		#define XBee_get_next_msg() UART1_get_next_msg()
	#endif
	#ifdef XBEE_PLUGGED_ON_UART2
		#define XBee_get_next_msg() UART2_get_next_msg()
	#endif


volatile static bool_e initialized = FALSE;
volatile static bool_e module_reachable[MODULE_NUMBER];	//Etat des autres modules (joignables ou non...)
volatile static bool_e flag_1s = FALSE;

void CAN_over_XBee_every_second(void)
{
	//if(flag_1s == TRUE)
	//	debug_printf("Attention, vous n'avez pas appelé CAN_over_XBee_process_main depuis 1 seconde !\n");
	flag_1s = TRUE;
}

volatile module_id_e XBee_i_am_module = ROBOT_1;
volatile module_id_e XBee_module_id_destination = ROBOT_2;


void CAN_over_XBee_init(module_id_e me, module_id_e destination)
{
	module_id_e module;
	assert(me<MODULE_NUMBER);
	assert(destination<MODULE_NUMBER);
	XBee_i_am_module = me;
	XBee_module_id_destination = destination;
	for(module = 0; module<MODULE_NUMBER; module++)
		 module_reachable[module] = FALSE;		//Tout les modules sont injoignables.
	assert(XBee_i_am_module < MODULE_NUMBER);
	module_reachable[XBee_i_am_module] = TRUE;	//En principe, on a pas besoin de vérifier qu'on peut se parler à soit-même.
	initialized = TRUE;
}



void XBee_Pong(module_id_e module)
{
	CAN_msg_t msg;
	if(module >= MODULE_NUMBER)
		return;
	msg.sid = XBEE_PONG;
	msg.data[0] = XBee_i_am_module;
	msg.size = 1;
	CANMsgToXBeeDestination(&msg, module);
}

void XBee_Ping(module_id_e  module)
{
	CAN_msg_t msg;
	if(module >= MODULE_NUMBER)
		return;
	msg.sid = XBEE_PING;
	msg.data[0] = XBee_i_am_module;
	msg.size = 1;
	CANMsgToXBeeDestination(&msg, module);
}



/*
@function 	Fonction d'initialisation du dialogue avec le module XBee
@pre		Cette fonction doit être appelée périodiquement jusqu'à ce qu'elle renvoit TRUE !
*/
void CAN_over_XBee_process_main(void)
{
	static Uint8 time = 0;
	bool_e everyone_is_reachable;
	typedef enum
	{
		INIT = 0,					//init variables et lancement du reset du module XBee
		WAIT_BEFORE_RELEASE_RESET,	//Reset du module XBee
		WAIT_FOR_NETWORK,			//Attente que le module XBee soit prêt
		PING_PONG,					//Etat d'échanges de ping et de pong entre les différents modules
		IDLE						//Réseau en fonctionnement
	}XBee_state_e;
	static XBee_state_e XBee_state = INIT;
	module_id_e module;
	//Note : la mise en place d'un PING au niveau applicatif peut être sympatique... et peut permettre de déclencher l'envoi des messages !
	//Dès que je reçois un ping, je réponds pong, et je m'autorise à contacter ce correspondant !
	//Tant que je n'ai pas reçu de ping de tous les correspondants et que je n'ai pas envoyé un pong à tout les correspondants, je leur envoie un ping périodiquement (1 seconde par exemple)
	//A terme, il faudrait traiter l'acquittement des messages envoyés pour maintenir à jour la connaissance du fonctionnement du réseau...
	//Et pouvoir reprendre les ping envers un destinataire absenté... ! (Donc géré dans un état IDLE normal)
	if(flag_1s)
	{
		flag_1s = FALSE;
		switch(XBee_state)	//Machine à état exécutée toutes les secondes !
		{
			case INIT:
				if(initialized == FALSE)
				{
					debug_printf("ERREUR : VOUS DEVEZ APPELER CAN_over_XBee_init() AVANT d'APPELER LE PROCESS MAIN...");
					return;
				}
				//Transition immédiate.
				XBee_state = WAIT_BEFORE_RELEASE_RESET;
				//RESET du module XBee
				XBEE_RESET = 1;
			break;
			case WAIT_BEFORE_RELEASE_RESET:
				XBEE_RESET = 0;
				XBee_state = WAIT_FOR_NETWORK;
				time = 5;
			break;
			case WAIT_FOR_NETWORK:
				if(time)
					time--;
				else
					XBee_state = PING_PONG;
			break;
			case PING_PONG:
				everyone_is_reachable = TRUE;	//on suppose que tout le monde est joignable
				for(module = 0; module<MODULE_NUMBER; module++)
				{	//Pour tout les modules non reachable, on envoi un ping !
					if(module_reachable[module] == FALSE)
					{
						everyone_is_reachable = FALSE;	//si quelqu'un n'est pas joignable, alors tout le monde ne l'est pas !
						XBee_Ping(module);
						debug_printf("ping %d->%d\n",XBee_i_am_module, module);
					}
				}
				if(everyone_is_reachable || global.env.match_started)
					XBee_state = IDLE;
			break;
			case IDLE:
				//Soit le match à commencé, soit tout le monde a été joint -> on arrête les pings...
				//TOUT LES MESSAGES CAN ENVOYES SONT ALORS CONSECUTIFS A UNE DEMANDE PROVENANT DU DESTINATAIRE DES MESSAGES !
				//ces demandes sont périssables en quelques secondes. Ceci afin d'éviter qu'un flux de donnée ne soit actif longtemps vers un destinataire qui a été éteint.
				if(global.env.match_started == FALSE)
					XBee_state = INIT;
			break;
			default:
			break;
		}
	}

}

bool_e process_received_can_msg(CAN_msg_t * msg)
{
	switch(msg->sid)	//Messages CAN reçus, premier filtrage.
	{
		case XBEE_PONG:
			if(msg->data[0] < MODULE_NUMBER)
				module_reachable[msg->data[0]] = TRUE;
			return FALSE;	//Le message ne sera pas transmis au reste du code.
		break;
		case XBEE_PING:
			if(msg->data[0] < MODULE_NUMBER)
			{
				module_reachable[msg->data[0]] = TRUE;
				XBee_Pong(msg->data[0]);
				debug_printf("pong %d->%d\n",XBee_i_am_module, msg->data[0]);
			}
			return FALSE;	//Le message ne sera pas transmis au reste du code.
		break;
		default:
		break;
	}
	return TRUE;	//Le message sera transmis au reste du code.
}

typedef enum
{
	HEADER,
	SID_MSB,
	SID_LSB,
	DATA0, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7,
	SIZE_FIELD,
	FOOTER
}can_msg_on_char_array_fields_e;

#define CAN_MSG_LENGTH	11

bool_e APIFrameToCANmsg(Uint8 * frame, CAN_msg_t * dest)
{
	/*
	 *	cette fonction lit un octet dans le tableau frame
	 *	et construit un message CAN.  Elle renvoie ensuite si
	 *	oui ou non elle a trouvé un message CAN. Elle vérifie
	 *  aussi si le message est bien conforme au protocole de communication
	 *  (cf QS)
	 */
	can_msg_on_char_array_fields_e next_byte_to_read=0;
	Uint8 byte_read;

	for(next_byte_to_read=0;next_byte_to_read<13;next_byte_to_read++)	//C'est à ces indices que le message CAN peut être récupéré. Voir doc XBEE...	ou commentaires plus bas.
	{
		byte_read = frame[next_byte_to_read];

		switch (next_byte_to_read)
		{
			case HEADER:
				if(byte_read != SOH)
					return FALSE;
				break;
			case SID_MSB:		/*lecture du MSB du sid */
				dest->sid = (Uint16)byte_read <<8;
				break;
			case SID_LSB:		/*lecture du LSB du sid */
				dest->sid |= (Uint16)byte_read;
				break;
			case SIZE_FIELD:	/*lecture du champs size */
				dest->size = byte_read;
				if(dest->size > 8)
					return FALSE;
				break;
			case FOOTER:
				if(byte_read != EOT)
					return FALSE;
				else
					return TRUE;
				break;

			default:	/*lecture d'un octet de data */
				dest->data[next_byte_to_read - DATA0]=byte_read;
				break;
		}
	}
	return FALSE;
}

/*
	Une frame de "données reçues" renvoyée par le module XBee est de la forme :
	fr. delimiter	SizeMSB	SizeLSB		DataReceived	@64bits				@network	ACK'ed	SOH	 SID	DATAS			   SIZE	EOT		Checksum
	0x7E			0x00	25			0x90			0xAAAAAAAAAAAAAAAA	0xNNNN		0x01	0x01 0x0123 0x1122334455667788 0x08 0x04	0x??
				length = 25				<---------------------------------------------------------------------------------------------->
					indice frame		0				  1      à       8    9 10      11      12   13 14   15      à      22   23   24
													le message can est donc de  12  à 24 inclus:<-------------------------------------->
					le checksum porte sur les 25 octets du length, plus le checksum : le total doit valoir 0xFF
*/
typedef enum
{
	START_DELIMITER = 0,
	LENGTH_MSB,
	LENGTH_LSB,
	HEADER_SIZE
}api_frame_on_char_array_fields_e;
#define FRAME_API_BUF_SIZE 32

bool_e XBeeToCANmsg (CAN_msg_t* dest)
{

	static api_frame_on_char_array_fields_e next_byte_to_read=0;
	static Uint16 length;
	static Uint8 checksum;
	static Uint8 frame_api[FRAME_API_BUF_SIZE];
	Uint8 byte_read;


	while(XBee_data_ready())
	{
		byte_read = XBee_get_next_msg();

		switch (next_byte_to_read)
		{
			case START_DELIMITER:
				checksum = 0;
				if(byte_read != 0x7E)
				{
					next_byte_to_read = 0;	//Ce n'est pas le début d'une trame... on refuse de continuer..
					debug_printf("XBee : invalid start 0x%02x != 0x7E\n",byte_read);
					continue;	//Prochain caractère !
				}
				break;
			case LENGTH_MSB:
				length = (Uint16)byte_read <<8;
				break;
			case LENGTH_LSB:
				length |= (Uint16)byte_read;
				if(length != 25)
				{
					next_byte_to_read = 0;
					debug_printf("XBee : invalid length %d != 25\n",length);
					continue;	//Prochain caractère !
				}
				break;
			default:
				//Réception des datas...
				if(next_byte_to_read - HEADER_SIZE < FRAME_API_BUF_SIZE)
				{
					frame_api[next_byte_to_read-HEADER_SIZE] = byte_read;
					checksum += byte_read;
				}
				else
				{
					//Il y a un problème, on a explosé le buffer de réception...
					next_byte_to_read = 0;
					debug_printf("XBee : I do not want to explode buffer...\n");
					continue;	//Prochain caractère !
				}

			break;
		}
		if(next_byte_to_read == length + HEADER_SIZE)	//Dernier octet (qui est d'ailleurs le checksum)
		{
			next_byte_to_read = 0;	//Pour le prochain passage...
			if(checksum	== 0xFF)	//Tout va bien !
			{
				if(frame_api[0] != 0x90)
					continue;

				if(APIFrameToCANmsg(frame_api+12, dest))	//Recherche d'un message can dans la frame reçue.
				{
					//On a reçu un message CAN !!!!!!!
					return  process_received_can_msg(dest);	//Si cette fonction traite un message qui n'est destiné qu'à ce fichier, elle renvoie false, et le message n'est pas remontté à l'applicatif !
				}
			}
		}
		else
		{
			next_byte_to_read++;
		}
	}

	return FALSE;
}



/*
	Une frame de "données à envoyer" pour le module XBee est de la forme :
	fr. delimiter	SizeMSB	SizeLSB		DataToSend		needAck		@64bits				@network	Hope	Unicast	SOH	 SID	DATAS			   SIZE	EOT		Checksum
	0x7E			0x00	27			0x10			i or 0		0xAAAAAAAAAAAAAAAA	0xFFFE		0x00	0x00	0x01 0x0123 0x1122334455667788 0x08 0x04	0x??
				length = 27				<---------------------------------------------------------------------------------------------->
					indice frame		0				  1      	  2	 	 à       9   10 11      12      13  	  14  15 16	  17      à     24   25   26
																							le message can est ici :<-------------------------------------->
					le checksum porte sur les 27 octets du length, plus le checksum : le total doit valoir 0xFF
					l'adresse 64 bits doit être connue (correspond au numéro de série du module destinataire !)
					l'adresse network est décidée par le coordinateur, donc variable. 0xFFFE permet d'indiquer qu'on ne la connait pas (le module déterminera tout seul cette adresse !)
*/

#define SEND(x)	XBee_putc(x); cs+=x

void CANMsgToXBeeDestination(CAN_msg_t * src, module_id_e module_dest)
{
	static Uint8 ack = 0;
	Uint8 cs;
	Uint8 i;

	if(src->sid != XBEE_PING && module_reachable[module_dest] == FALSE)	//Module non atteignable
			return;	//On se refuse d'envoyer un message si le module n'est pas joignable, sauf s'il s'agit d'un ping !

	XBee_putc(0x7E);	//Début de l'ordre API.
	XBee_putc(0x00);
	XBee_putc(27); //SIZE

	cs = 0x00;
	SEND(0x10);	//API identifier pour TRANSMIT REQUEST
	ack++;
	if(ack==0)
		ack=1;	//car à 0 on a pas d'acquittement.
	#ifdef ASK_FOR_ACK
		SEND(ack);//(ack++);	//msg id (pour acknoledge). 0 pour ne pas demander d'acknowledge.
	#else
		SEND(0x00);
	#endif
	for(i=0; i<8; i++)
	{	//Les accollades sont importantes (à cause du fait que SEND est une macro à deux instructions.
		SEND(module_address[module_dest][i]);	//@ du module de destination. Voir le header de ce fichier .c !!!
	}

	SEND(0xFF);	//Network address : on ne la connait pas (variable). 0xFFFE indique au module de la déterminer par lui même...
	SEND(0xFE);	//
	SEND(0x00);	//Nombre max de noeud que le message peut traverser (Si 0 : valeur par défaut = 10)
	SEND(0x00);	//Mode unicast

	//Datas
	SEND(SOH);
	SEND((Uint8)(src->sid >>8));
	SEND((Uint8)src->sid);
	for (i=0; i<src->size && i<8; i++)
	{	//Les accollades sont importantes (à cause du fait que SEND est une macro à deux instructions.
		SEND(src->data[i]);
	}
	for (i=src->size; i<8; i++)
	{	//Les accollades sont importantes (à cause du fait que SEND est une macro à deux instructions.
		SEND(0xFF);
	}

	SEND(src->size);
	SEND(EOT);

	XBee_putc(0xFF-cs); //checksum
}
void CANMsgToXbee(CAN_msg_t * src, bool_e broadcast)
{
	module_id_e module;
	if(broadcast)
	{
		for(module = 0; module<MODULE_NUMBER; module++)
		{
			if(module != XBee_i_am_module)
				CANMsgToXBeeDestination(src, module);
		}
	}
	else
		CANMsgToXBeeDestination(src, XBee_module_id_destination);
}

void XBEE_send_sid(Uint11 sid, bool_e broadcast)
{
	CAN_msg_t msg;
	msg.sid = sid;
	msg.size = 0;
	CANMsgToXbee(&msg, broadcast);
}

bool_e XBee_is_destination_reachable(void)
{
	if(initialized)
		return module_reachable[XBee_module_id_destination];
	else
		return FALSE;
}


#endif //def USE_XBEE


