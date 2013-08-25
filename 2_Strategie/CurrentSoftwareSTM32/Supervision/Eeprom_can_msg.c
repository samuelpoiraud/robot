/*
 *	Club Robot ESEO 2011
 *	Check Norris
 *
 *	Fichier : eeprom_can_msg.c
 *	Package : Supervision
 *	Description : Module de gestion de l'enregistrement des messages can dans l'eeprom SPI
 *	Auteur : Nirgal
 *	Version 20110514
 */

#define EEPROM_CAN_MSG_C
#include "Eeprom_can_msg.h"
#include "Selftest.h"


#define TEST_STRING_ADDRESS		(Uint32)(0x00000000)	//Adresse de la chaine de test de la m�moire EEPROM
#define TEST_STRING_SIZE		(Uint8)(8)				//Taille de la chaien de test de la m�moire EEPROM
#define MIN_MATCH_ADDRESS_X8 	(Uint16)(1)				//Premi�re adresse dans la table des matchs (en adresse/8)
#define MAX_MATCH_ADDRESS_X8 	(Uint16)(0x0200)	//0x200		//Derni�er adresse dans la table des matchs (en adresse/8)
#define MATCH_SIZE				(Uint8)(8)				//Taille d'un header de match
#define MAX_MSG_IN_MATCH		(Uint16)(2048)			//Nb max de message pour un match
#define MIN_MSG_ADDRESS_X16		(Uint16)(0x0200)
#define MAX_MSG_ADDRESS_X16		(Uint16)(0x1744)	//0x1744	//TODO v�rifier !
/*
Conception...

M�moire EEPROM
0x00000				-> 0xCAFE DECA E5E0 2011
0x00009 � 0x001FF 	-> table de matchs
0x00200 � 0x1FFFF 	-> messages can, sur des addresses multiples de 16 octets

Structure d'un message can :
		MATCH_ID	TIME		SID			DATA		SIZE		RFU (reserved for futur use)
bits	16			16			16			64			8			8
octets	2			2			2			8			1			1

Structure d'un ent�te de match
		ADRESSE*16	JOUR	MOIS	HEURE	MN		TAILLE_MATCH ([nb msg] si 0 : match non termin�)
bits	16			8		8		8		8		16
octets	2			1		1		1		1		2




INIT (au reset du micro)
-> on v�rifie le fonctionnement de la m�moire (lecture � l'adresse 0 du code de v�rif et "�criture puis relecture" si n�cessaire)
-> on consulte la table des matchs pour connaitre le match le plus r�cent => maj current_match_id
	-> si ce dernier match est termin� (taille != 0), next_msg_address = taille du dernier match + adresse du dernier match
	-> sinon, il faut parcourir le match avec une boucle qui s'arr�te d�s que match_id est diff�rent...
		-> on enregistre alors le match comme termin� et on met � jour son adresse en EEPROM
	-> passage en �tat WAIT_MATCH
WAIT_MATCH
-> d�s qu'un message BROADCAST_START arrive
	-> enregistrement du nouveau match dans la table des matchs (temps rtc actuel, match_id suivant, taille � 0)
	-> on ajoute le message au match nouvellement commenc�
	-> on passe dans l'�tat IN_MATCH
IN_MATCH
-> d�s r�ception d'un message
	-> si ce message n'est pas filtr� (r�gles de filtrages... pour �liminer les messages r�currents de position notamment)
		-> on enregistre le message � la position suivante dans la m�moire
	-> si ce message est un BROADCAST_STOP
		-> le nombre de message est inscrit dans la table des matchs, le message est copi� en EEPROM, et le match se termine, on passe en �tat WAIT_MATCH
	-> si ce message est un BROADCAST_START
		-> 	le nombre de message est inscrit dans la table des matchs, on fait la m�me chose que pour un nouveau match...


Enregistrement des messages CAN
-> la circularit� du buffer doit �tre maintenue. attention donc lorsque l'on passe au "message suivant".



*/


//variables de l'objet :
Uint16 current_match_id;		//Num�ro du match en cours ou � venir
Uint16 current_match_address_x8;		//adresse de l'ent�te du match en cours o� � venir
Uint16 current_msg_address_x16;	//adresse du prochain message � �crire
Uint16 nb_msg_in_match;
bool_e initialized = FALSE;		//Lorsque le module est correctement initialis� (n�cessite que la m�moire soit c�bl�e et fonctionne !)

/*
@return 	TRUE : la m�moire est l� et fonctionne
@return		FALSE : la m�moire n'est pas l� !
*/
bool_e EEPROM_CAN_MSG_test_eeprom_available()
{
	const Uint8 test_string[TEST_STRING_SIZE] = {0xCA, 0xFE, 0xDE, 0xCA, 0xE5, 0xE0, 0x20, 0x11};
	Uint8 read_test_string[TEST_STRING_SIZE];
	bool_e b;
	Uint8 i;

	EEPROM_Read(TEST_STRING_ADDRESS, read_test_string, TEST_STRING_SIZE);
	b = TRUE;
	for(i = 0; i< TEST_STRING_SIZE; i++)
	{
		if(test_string[i] != read_test_string[i])
		{
			b = FALSE;
			break;
		}	
	}

	if(!b)	//Soit la m�moire n'est pas l�, soit le test string n'y est pas. On tente d'�crire le test string
	{
		EEPROM_Write(TEST_STRING_ADDRESS, (Uint8*)test_string, TEST_STRING_SIZE);
		EEPROM_Read(TEST_STRING_ADDRESS, read_test_string, TEST_STRING_SIZE);
		b = TRUE;
		for(i = 0; i< TEST_STRING_SIZE; i++)
		{
			if(test_string[i] != read_test_string[i])
				b = FALSE;
		}
	}	
	if(!b)
		debug_printf("test EEPROM failed\n");
	//else
	//	debug_printf("test EEPROM ok\n");

	return b;
}	






/*
@param msg_address : adresse en m�moire * 16 (ainsi elle est stockable sur 16 bits, et on ne risque pas d'aller �crire un message � une adresse autre que multiple de 16 !)
@param * msg : message � sauvegarder
@param	current_time : temps en [250ms]
@param match_id : num�ro du match auquel appartient le message
*/
#define MSG_SIZE_IN_EEPROM 	((Uint8)(16))
void EEPROM_CAN_MSG_save_msg(Uint16 msg_address_x16, CAN_msg_t * msg, Uint16 match_id, Uint16 current_time)
{
	Uint8 msg_to_save[16];
	msg_to_save[0]	= (Uint8)(HIGHINT(match_id));
	msg_to_save[1]	= (Uint8)(LOWINT(match_id));
	msg_to_save[2]	= (Uint8)(HIGHINT(current_time));
	msg_to_save[3]	= (Uint8)(LOWINT(current_time));
	msg_to_save[4]	= (Uint8)(HIGHINT(msg->sid));
	msg_to_save[5]	= (Uint8)(LOWINT(msg->sid));
	msg_to_save[6]	= msg->data[0];
	msg_to_save[7]	= msg->data[1];
	msg_to_save[8]	= msg->data[2];
	msg_to_save[9]	= msg->data[3];
	msg_to_save[10]	= msg->data[4];
	msg_to_save[11]	= msg->data[5];
	msg_to_save[12]	= msg->data[6];
	msg_to_save[13]	= msg->data[7];
	msg_to_save[14]	= msg->size;
	msg_to_save[15]	= 0;
	//debug_printf("SAVE sid=%d address_x16=%d\n", msg->sid, msg_address_x16); 
	EEPROM_Write((Uint32)(msg_address_x16) * 16, msg_to_save, MSG_SIZE_IN_EEPROM);
}	

/*
@brief incr�mente le match_id et la match_address et cr�e un match dans la table...
*/
void EEPROM_CAN_MSG_new_match()
{
	Uint8 seconds, minutes, hours, day, date, month, year;
	Uint16 condensed_time;
	Uint8 temp[MATCH_SIZE];
	current_match_address_x8 = (current_match_address_x8 >= MAX_MATCH_ADDRESS_X8)?(MIN_MATCH_ADDRESS_X8):(current_match_address_x8+1);
	current_match_id++;
	//debug_printf("new match_id : %d | new match_address_x8 : %d | msg address : %d\n", current_match_id, current_match_address_x8, current_msg_address_x16);
	nb_msg_in_match = 0;
	/*  Structure d'un ent�te de match
				ADRESSE*16	MATCH_ID	HOUR		TAILLE_MATCH ([nb msg] si 0 : match non termin�)
		bits	16			16			16			16
		octets	2			2			2			2
	*/
	temp[0] = HIGHINT(current_msg_address_x16);
	temp[1] = LOWINT(current_msg_address_x16);
	temp[2] = HIGHINT(current_match_id);
	temp[3] = LOWINT(current_match_id);
	
	RTC_get_time(&seconds, &minutes, &hours, &day, &date, &month, &year);
	condensed_time = 0;
	condensed_time |= (date		&0x1F) << 11;
	condensed_time |= (hours	&0x1F) << 6;
	condensed_time |= (minutes	&0x3F);
	
	temp[4] = (Uint8)(condensed_time >> 8);
	temp[5] = (Uint8)(condensed_time);
	
	EEPROM_Write(((Uint32)(current_match_address_x8)) * 8, temp, 8);	
}
	

/*
@brief clos le match (soit lors d'un broadcast_stop/start, soit apr�s un reset qui d�couvre un match non termin�...
*/
void EEPROM_CAN_MSG_finish_match()
{
	Uint8 temp[2];
	temp[0] = HIGHINT(nb_msg_in_match);
	temp[1] = LOWINT(nb_msg_in_match);
	//debug_printf("fin du match : %d messages dans le match termin�\n", nb_msg_in_match);
	EEPROM_Write(((Uint32)(current_match_address_x8)) * 8 + 6, temp, 2);	
}	




void EEPROM_CAN_MSG_verbose_msg(Uint8 * msg)
{
	CAN_msg_t can_msg;
	Uint16 time;
	can_msg.sid = U16FROMU8(msg[4], msg[5]);
	can_msg.data[0] = msg[6];
	can_msg.data[1] = msg[7];
	can_msg.data[2] = msg[8];
	can_msg.data[3] = msg[9];
	can_msg.data[4] = msg[10];
	can_msg.data[5] = msg[11];
	can_msg.data[6] = msg[12];
	can_msg.data[7] = msg[13];
	can_msg.size 	= msg[14];
	time = U16FROMU8(msg[2], msg[3]);	//[2ms]
	debug_printf("t=%.2d.%03ds ",time/500, (time%500)*2);
	VERBOSE_CAN_MSG_print(&can_msg);
}	




/*
@brief lire le match dont l'adresse est pass�e en param�tre
*/
void EEPROM_CAN_MSG_verbose_match(Uint16 match_address_x8)
{
	Uint8 temp[MATCH_SIZE];
	Uint8 msg[MSG_SIZE_IN_EEPROM];
	Uint16 match_id;
	Uint16 nb_msg;
	Uint16 msg_address_x16;
	Uint16 i;
	Uint16 condensed_time;
	
	
	if(match_address_x8 < MIN_MATCH_ADDRESS_X8 || match_address_x8 > MAX_MATCH_ADDRESS_X8)
	{
	debug_printf("\n\nAdresse de l'ent�te du match non valide :                 0x%x n'est pas compris entre 0x%x et 0x%x\n", match_address_x8, MIN_MATCH_ADDRESS_X8, MAX_MATCH_ADDRESS_X8);
		return;
	}	
	debug_printf("\n\nAdresse de l'ent�te du match dans la table des matchs :   0x%x\n", match_address_x8);
	debug_printf("Adresse physique de l'ent�te du match en EEPROM :         0x%lx\n", ((Uint32)(match_address_x8)) * 8);
	
	EEPROM_Read(((Uint32)(match_address_x8)) * 8, temp, 8);
	
	match_id 		= U16FROMU8(temp[2], temp[3]);
	nb_msg 	 		= U16FROMU8(temp[6], temp[7]);
	msg_address_x16 = U16FROMU8(temp[0], temp[1]);
	condensed_time  = U16FROMU8(temp[4], temp[5]);
	
	if(match_id != 0)
		debug_printf("ID du match :                                             0x%x\n", match_id);
	else
	{
		debug_printf("L'ID du match est nul. Ce match n'est pas valide.\n");
		return;
	}
	
	debug_printf("Date du match :                                           %d � %.2dh%.2d\n", (condensed_time >> 11)&0x1F, (condensed_time >> 6)&0x1F, condensed_time&0x3F);
		
	debug_printf("Adresse du premier message dans la table des messages :   0x%x\n", msg_address_x16);
	debug_printf("Adresse physique du premier message en EEPROM :           0x%lx\n", ((Uint32)(msg_address_x16))*16);
	if(nb_msg)
		debug_printf("Nombre de messages dans le match :                        %d\n", nb_msg);
	else
		debug_printf("Le match ne s'est pas termin� par un broadcast_stop ou le nombre de message max est atteint\n");
	
	//Si le match n'est pas termin�, on suppose que l'on parcourt le nombre max de message. on abandonnera d�s qu'on verra qu'on atteint le dernier message connu de ce match
	if(!nb_msg)
		nb_msg = MAX_MSG_IN_MATCH;
	for(i=0;i<nb_msg;i++)
	{
		EEPROM_Read(((Uint32)(msg_address_x16)) * 16, msg, MSG_SIZE_IN_EEPROM);
		msg_address_x16 = (msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(msg_address_x16+1);
		if(match_id != U16FROMU8(msg[0], msg[1]))
		{
			debug_printf("fin du match en m�moire (match termin� ou bien messages �cras�s par d'autres matchs plus r�cents\n");
			break;	//On est rendu sur un autre match.
		}
		 debug_printf("@:%.4x ",msg_address_x16);
		EEPROM_CAN_MSG_verbose_msg(msg);
		
		if(BUTTON5_PORT)	//Si on rappuie sur le bouton, on arr�te l'affichage du match.
			break;
	}
	debug_printf("fin du match en m�moire\n");
}	




	
//TODO : fonction de vidage d'EEPROM sur message CAN explicite.
#define WINDOW_SIZE	(64)	//Doit �tre une puissance de 2 < strictement � 256. (pour ne pas acc�der � la m�moire sur deux pages en un acc�s...
void EEPROM_CAN_MSG_flush_eeprom(void)
{
	Uint8 temp[WINDOW_SIZE];
	Uint32 j;
	Uint16 i;
	for(i=0;i<WINDOW_SIZE;i++)
		temp[i] = 0;
	
	for(j=0;j<EEPROM_SIZE_OCTETS;j+=WINDOW_SIZE)
		EEPROM_Write(j, temp, WINDOW_SIZE);
}	

/*
	Compte le nombre de message dans un match dont l'adresse du premier message et l'id sont connus.
*/
Uint16 EEPROM_CAN_MSG_count_nb_msg(Uint16 msg_address_x16, Uint16 match_id)
{
	Uint16 read_match_id;
	Uint8 msg[2];
	Uint16 nb_msg = 0;
	bool_e b = TRUE;
	//debug_printf("comptage � effectuer sur le match_id %d � l'adresse %d\n",match_id, msg_address_x16);
	while(b)
	{
		EEPROM_Read(((Uint32)(msg_address_x16)) * 16, msg, 2);	//On lit que la partie "id" du message can enregistr�.
		msg_address_x16 = (msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(msg_address_x16+1);
		read_match_id = U16FROMU8(msg[0], msg[1]);
		if(read_match_id != match_id || nb_msg >= MAX_MSG_IN_MATCH)
			b = FALSE;	//Si l'id lu ne correspond "plus", c'est la fin du match...
		else
			nb_msg++;
	}
	//debug_printf("j'ai compt� %d message(s)\n",nb_msg);
	return nb_msg;
}	

/*
@brief	fonction d'initialisation du module de sauvegarde en EEPROM. Tr�s importante, cette fonction scrute l'�tat actuel de la m�moire pour en reprendre le cours !
@post 	initialized est pass� � TRUE
@note	V�rification de pr�sence m�moire : lecture adresse 0, si != 0xCAFEDECA, �criture de 0xCAFEDECA, relecture et v�rif...
*/
void EEPROM_CAN_MSG_init()
{
	Uint8 temp_match[MATCH_SIZE];
	Uint16 read_match_id;
	
	EEPROM_init();
//	EEPROM_CAN_MSG_flush_eeprom();	//Activer ceci permet de vider la m�moire EEPROM lors de l'init (attention, cela prend 10 secondes environ !!)
	if(!EEPROM_CAN_MSG_test_eeprom_available())	//La m�moire n'est pas l�...!
		return;
	
	//Si la m�moire est bien pr�sente... et fonctionne...
	
	current_match_address_x8 = MIN_MATCH_ADDRESS_X8;	//On commence � l'adresse 8..., donc au current_match_address 1
	current_match_id = 0;
	current_msg_address_x16 = MAX_MSG_ADDRESS_X16;	//si si, c'est bien �a !
	while(current_match_address_x8 < MAX_MATCH_ADDRESS_X8)
	{
		EEPROM_Read(((Uint32)(current_match_address_x8)) * 8, temp_match, MATCH_SIZE);
		read_match_id = U16FROMU8(temp_match[2],temp_match[3]);
		if(read_match_id == 0)
			break;	//Pas de match dans cette case...
		if(read_match_id < current_match_id)
			break;	//On a d�pass� le dernier match
		else
		{	//On a pas d�pass� le dernier match
			current_match_id = read_match_id;
			current_msg_address_x16 = U16FROMU8(temp_match[0],temp_match[1]);
			nb_msg_in_match = U16FROMU8(temp_match[6],temp_match[7]);
			current_match_address_x8++;
		}	
	}
	
	//On doit se placer sur l'ent�te du dernier match... s'il n'y en a pas, on se replace sur la derni�re des cases !
	current_match_address_x8 = (current_match_address_x8 == MIN_MATCH_ADDRESS_X8)?MAX_MATCH_ADDRESS_X8:current_match_address_x8-1;	
			
	if(current_match_id != 0)	//Si on a vu au moins un match dont l'id est non nul.
	{	
		//On a l'adresse du dernier match enregistr�, sa taille s'il est fini et son match_id...
		if(nb_msg_in_match == 0)	//Si le match n'est pas termin� !
		{
			// on termine le match
			nb_msg_in_match = EEPROM_CAN_MSG_count_nb_msg(current_msg_address_x16, current_match_id);
			if(!nb_msg_in_match)	//Le comptage � �chou� (il n'y a pas de message dans le match, c'est anormal)
				nb_msg_in_match = 1;	//On r�pare le match en mettant le nb de msg � 1...
			EEPROM_CAN_MSG_finish_match();	//On termine le match non termin�.
		}
	}
	
	current_msg_address_x16 += nb_msg_in_match;	//On met � jour le current_msg_address_x16...
	if(current_msg_address_x16 > MAX_MSG_ADDRESS_X16)
		current_msg_address_x16 = MIN_MSG_ADDRESS_X16 + (current_msg_address_x16 - MAX_MSG_ADDRESS_X16);
	
	//On est positionn� sur le DERNIER MATCH (que l'on a valid� si c'�tait n�cessaire)
	debug_printf("init eeprom ok : current_match_id = %d | current_match_address_x8 : %d\n", current_match_id, current_match_address_x8);
	initialized = TRUE;
}	

/*
@brief 	fonction incluant la machine � �tat du module de sauvegarde en EEPROM. 
@pre	Cette fonction est appel�e � chaque nouveau message re�u
@pre	initialized doit �tre � TRUE ! sinon rien n'est fait. (lorsque la m�moire n'est pas reli�e notamment !)
*/
void EEPROM_CAN_MSG_process_msg(CAN_msg_t * msg)
{
	CAN_msg_t local_msg;
	typedef enum
	{
		WAIT_MATCH,
		IN_MATCH
	}EEPROM_CAN_MSG_state_e;
	static EEPROM_CAN_MSG_state_e state = WAIT_MATCH;

	if(!initialized)	//V�rification. Si la m�moire n'est pas initialis�e, on abandonne.
		return;
	
	if(msg->sid == SUPER_EEPROM_RESET)
		EEPROM_CAN_MSG_flush_eeprom();
	if(msg->sid== SUPER_EEPROM_PRINT_MATCH)
		EEPROM_CAN_MSG_verbose_match(U16FROMU8(msg->data[0],msg->data[1]));
	
	if(msg->sid == BROADCAST_START)
	{
		//Dans le cas de la r�ception d'un broadcast start, on ajoute des informations dans les datas inutilis�es afin d'enregistrer des infos sur le match lanc� !
		msg->data[0] = global.env.color;
		msg->data[1] = global.env.config.strategie;
		msg->data[2] = global.env.config.evitement;
		msg->data[3] = global.env.config.balise;
		msg->data[4] = SWITCH_DEBUG;
		//TODO : r�cup�rer le r�sultat du selftest pour enregistrement...
		msg->size = 8;
	}	
	switch(state)
	{
		case WAIT_MATCH:
			if(msg->sid == BROADCAST_START)
			{
				SELFTEST_beacon_counter_init(); // Peut etre pas la meilleur place mais n�cessaire pour le broadcast_stop qui suit

				EEPROM_CAN_MSG_new_match();
				EEPROM_CAN_MSG_save_msg(current_msg_address_x16, msg, current_match_id,  (Uint16)(global.env.match_time/2));
				current_msg_address_x16 = (current_msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(current_msg_address_x16+1);
				nb_msg_in_match++;
				state = IN_MATCH;
			}	
		break;
		case IN_MATCH:
			if(msg->sid == BROADCAST_START)
			{
				EEPROM_CAN_MSG_finish_match();
				EEPROM_CAN_MSG_new_match();
				EEPROM_CAN_MSG_save_msg(current_msg_address_x16, msg, current_match_id,  (Uint16)(global.env.match_time/2));
				current_msg_address_x16 = (current_msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(current_msg_address_x16+1);
				nb_msg_in_match++;
			}
			else
			{
				//VERBOSE_CAN_MSG_print(msg); 
				if(nb_msg_in_match < MAX_MSG_IN_MATCH)	//Un nombre max de message par match est autoris�...
				{
					EEPROM_CAN_MSG_save_msg(current_msg_address_x16, msg, current_match_id, (Uint16)(global.env.match_time/2));
					current_msg_address_x16 = (current_msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(current_msg_address_x16+1);
					nb_msg_in_match++;
				}	
			}
				
			if(msg->sid == BROADCAST_STOP_ALL)
			{
				SELFTEST_get_match_report_IR(&local_msg);
				EEPROM_CAN_MSG_save_msg(current_msg_address_x16, &local_msg, current_match_id,  (Uint16)(global.env.match_time/2));
				current_msg_address_x16 = (current_msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(current_msg_address_x16+1);
				nb_msg_in_match++;

				SELFTEST_get_match_report_US(&local_msg);
				EEPROM_CAN_MSG_save_msg(current_msg_address_x16, &local_msg, current_match_id,  (Uint16)(global.env.match_time/2));
				current_msg_address_x16 = (current_msg_address_x16 >= MAX_MSG_ADDRESS_X16)?(MIN_MSG_ADDRESS_X16):(current_msg_address_x16+1);
				nb_msg_in_match++;

				EEPROM_CAN_MSG_finish_match();
				state = WAIT_MATCH;
			}	
		break;
		default:
			state = WAIT_MATCH;
		break;
	}	
/*
INIT (au reset du micro)
-> on v�rifie le fonctionnement de la m�moire (lecture � l'adresse 0 du code de v�rif et "�criture puis relecture" si n�cessaire)
-> on consulte la table des matchs pour connaitre le match le plus r�cent => maj current_match_id
	-> si ce dernier match est termin� (taille != 0), next_msg_address = taille du dernier match + adresse du dernier match
	-> sinon, il faut parcourir le match avec une boucle qui s'arr�te d�s que match_id est diff�rent...
		-> on enregistre alors le match comme termin� et on met � jour son adresse en EEPROM
	-> passage en �tat WAIT_MATCH
WAIT_MATCH
-> d�s qu'un message BROADCAST_START arrive
	-> enregistrement du nouveau match dans la table des matchs (temps rtc actuel, match_id suivant, taille � 0)
	-> on ajoute le message au match nouvellement commenc�
	-> on passe dans l'�tat IN_MATCH
IN_MATCH
-> d�s r�ception d'un message
	-> si ce message n'est pas filtr� (r�gles de filtrages... pour �liminer les messages r�currents de position notamment)
		-> on enregistre le message � la position suivante dans la m�moire
	-> si ce message est un BROADCAST_STOP
		-> le nombre de message est inscrit dans la table des matchs, le message est copi� en EEPROM, et le match se termine, on passe en �tat WAIT_MATCH
	-> si ce message est un BROADCAST_START
		-> 	le nombre de message est inscrit dans la table des matchs, on fait la m�me chose que pour un nouveau match...
*/

}	

	
void EEPROM_CAN_MSG_verbose_previous_match(void)
{
	static Sint16 nb_match_verbosed = 0;
	Sint16 match_address_x8_to_verbose;
	
	match_address_x8_to_verbose = current_match_address_x8 - nb_match_verbosed;
	if(match_address_x8_to_verbose < (Sint16)(MIN_MATCH_ADDRESS_X8))
		match_address_x8_to_verbose += MAX_MATCH_ADDRESS_X8;
		
	if(nb_match_verbosed > MAX_MATCH_ADDRESS_X8 - MIN_MATCH_ADDRESS_X8 + 1)
	{
		debug_printf("Tout les matchs de la table ont �t�s affich�s\n");
		return;
	}
	
	nb_match_verbosed++;
	if(nb_match_verbosed > MAX_MATCH_ADDRESS_X8 - MIN_MATCH_ADDRESS_X8 + 1)
		nb_match_verbosed = 0;
		
	EEPROM_CAN_MSG_verbose_match(match_address_x8_to_verbose);
}	




void EEPROM_CAN_MSG_verbose_all_match_header()
{
	Uint8 temp[MATCH_SIZE];
	Uint16 match_id;
	Uint16 nb_msg;
	Uint16 msg_address_x16;
	Uint16 i;
	Uint16 condensed_time;
	
	for(i=MIN_MATCH_ADDRESS_X8; i<MAX_MATCH_ADDRESS_X8; i++)
	{
		debug_printf("\n\nAdresse de l'ent�te du match dans la table des matchs :   0x%x\n", i);
		debug_printf("Adresse physique de l'ent�te du match en EEPROM :         0x%lx\n", ((Uint32)(i)) * 8);
		
		EEPROM_Read(((Uint32)(i)) * 8, temp, 8);
		
		match_id 		= U16FROMU8(temp[2], temp[3]);
		nb_msg 	 		= U16FROMU8(temp[6], temp[7]);
		msg_address_x16 = U16FROMU8(temp[0], temp[1]);
		condensed_time  = U16FROMU8(temp[4], temp[5]);
		
		if(match_id != 0)
			debug_printf("ID du match :                                             0x%x\n", match_id);
		else
		{
			debug_printf("L'ID du match est nul. Ce match n'est pas valide.\n");
			return;
		}
	
		debug_printf("Date du match :                                           %d � %.2dh%.2d\n", (condensed_time >> 11)&0x1F, (condensed_time >> 6)&0x1F, condensed_time&0x3F);
			
		debug_printf("Adresse du premier message dans la table des messages :   0x%x\n", msg_address_x16);
		debug_printf("Adresse physique du premier message en EEPROM :           0x%lx\n", ((Uint32)(msg_address_x16))*16);
		if(nb_msg)
			debug_printf("Nombre de messages dans le match :                        %d\n", nb_msg);
		else
			debug_printf("Le match ne s'est pas termin� par un broadcast_stop ou le nombre de message max est atteint\n");
	
	}
}	


void EEPROM_CAN_MSG_print_all_msg(void)
{
	Uint8 msg[MSG_SIZE_IN_EEPROM];
	Uint16 match_id = 0;
	Uint16 i;


	for(i=MIN_MSG_ADDRESS_X16;i<MAX_MSG_ADDRESS_X16;i++)
	{
		EEPROM_Read(((Uint32)(i)) * 16, msg, MSG_SIZE_IN_EEPROM);
		if(match_id && (match_id != U16FROMU8(msg[0], msg[1])))
		{
			debug_printf("fin du match en m�moire (match termin� ou bien messages �cras�s par d'autres matchs plus r�cents\n");
			debug_printf("NOUVEAU MATCH ID : %d", U16FROMU8(msg[0], msg[1]));
		}	
		match_id = U16FROMU8(msg[0], msg[1]);	//Sauvegarde du match id courrant...
		
		debug_printf("@:%.4x ",i);
		EEPROM_CAN_MSG_verbose_msg(msg);
	}
	debug_printf("fin d'affichage de tout les messages enregistr�s (enfin !!!)\n");
}
