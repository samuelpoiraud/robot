/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Interface.c
 *	Package : Supervision
 *	Description : Interface de configuration
 *	Auteur : Ronan & Aurélien
 *	Version 20100119
 */

#define INTERFACE_C
#include "Interface.h"

#ifdef INTERFACE_GRAPHIQUE

static element_list_t strategy_list = {10, {"strat0","strat1","strat2","strat3","strat4","strat5","strat6","strat7","strat8","strat9"}};
static element_list_t color_list = {2,{"Bleu","Rouge"}};
static element_list_t yes_no_list = {2,{"Non", "Oui"}};

void INTERFACE_GRAPHIQUE_init()
{	
	global.interface_graphique_enable = FALSE;
	global.indice_line = STRATEGIE;
	
	/* Stratégies */
	global.interface_line[STRATEGIE].name = "Strategie";
	global.interface_line[STRATEGIE].element_list = &strategy_list;
	
	/* Couleurs */
	global.interface_line[COLOR].name = "Couleur";
	global.interface_line[COLOR].element_list = &color_list;

	/* Evitement */
	global.interface_line[EVITEMENT].name = "Evitement";
	global.interface_line[EVITEMENT].element_list = &yes_no_list;

	/* Balise */
	global.interface_line[BALISE].name = "Balise";
	global.interface_line[BALISE].element_list = &yes_no_list;

	/* Transmission */
	global.interface_line[DEBUG].name = "Debug";
	global.interface_line[DEBUG].element_list = &yes_no_list;

	/* Buffer */
	global.interface_line[BUFFER].name = "Buffer";
	global.interface_line[BUFFER].element_list = &yes_no_list;

}

void INTERFACE_GRAPHIQUE_process_u1rx(void)
{
	static Uint32 sequence_read;
	Uint8 byte_read;
				
	if(UART1_data_ready() && global.interface_graphique_enable)
	{
		byte_read = UART1_get_next_msg();
		
		switch(byte_read)
		{
			case 0x01:	//Début d'un message can, oops !!! c'était pas pour nous
				global.interface_graphique_enable = FALSE;
			break;
			case '\E':
				sequence_read=0;
				sequence_read=byte_read;
			break;
			case '[':
				if(sequence_read=='\E')
					sequence_read = (sequence_read<<8) + byte_read;
				else sequence_read=0;
			break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
				if(sequence_read==0x1B5B) /* 1B : '\E' et 5B ='[' */
				{
					sequence_read = (sequence_read<<8) + byte_read;
					INTERFACE_GRAPHIQUE_analyse_sequence_read(sequence_read);	
				}
				else sequence_read=0;
			break;	
			case '\r':
			case '\n':
				interface_graphique_envoi_message_can();
			break;
			default:
				sequence_read=0;
			break;
		}		
	}	
}	

void INTERFACE_GRAPHIQUE_analyse_sequence_read(Uint32 sequence_read)
{
	volatile line_t* line = &((global.interface_line)[global.indice_line]);
	element_list_t* element_list = line->element_list;

	switch(sequence_read)
	{
		case FLECHE_HAUT: 
			INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, FALSE);
			global.indice_line=(global.indice_line-1+NB_CONFIG_FIELDS)%NB_CONFIG_FIELDS;
			INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, TRUE);	
		break;
		case FLECHE_BAS:
			INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, FALSE);
			global.indice_line=(global.indice_line+1)%NB_CONFIG_FIELDS;
			INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, TRUE);
		break;
		case FLECHE_GAUCHE:
			global.config[global.indice_line]=(global.config[global.indice_line]-1+(element_list->nb_elements))%(element_list->nb_elements);
			INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, TRUE);
		break;
		case FLECHE_DROITE:
			global.config[global.indice_line]=(global.config[global.indice_line]+1)%(element_list->nb_elements);
			INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, TRUE);
		break;
		default:
		break;
	}
}

void INTERFACE_GRAPHIQUE_afficher_tout_le_menu()
{
	Uint8 i;
	
	print_UART1(HIDE_CURSOR);
	print_UART1(WRITE_WHITE_ON_BLACK);
	print_UART1(ERASE_DISPLAY);
	moveto(1,1);
	
	print_UART1("                     Interface de configuration de ChekNorris !\r\n");
	INTERFACE_GRAPHIQUE_afficher_config_actuelle();
			
	for(i=0; i<NB_CONFIG_FIELDS; i++)
		INTERFACE_GRAPHIQUE_afficher_ligne(i, FALSE);
	INTERFACE_GRAPHIQUE_afficher_ligne(global.indice_line, TRUE);
}

void INTERFACE_GRAPHIQUE_afficher_config_actuelle() 
{
	Uint8 i;
	char chaine_temp[32];
	
	moveto(1,2);
	print_UART1("________________________________________________________________________________\r\n\r\n");
	for(i=0; i<NB_CONFIG_FIELDS; i++)
	{
		sprintf(
				chaine_temp,
				" %s %s \r\n",
				global.interface_line[i].name,
				(global.interface_line[i].element_list)->elements[global.config[i]]
		);
		print_UART1(chaine_temp);
	}
	print_UART1("________________________________________________________________________________\r\n");
}

void INTERFACE_GRAPHIQUE_afficher_ligne(Uint8 numero_de_ligne, bool_e surligner)
{
	char ligne_a_envoyer[100];	//80 caractères plus caracteres de controle
	moveto(2,12+(2*numero_de_ligne));	// positionnement en début de ligne (x, y)
	sprintf(
			ligne_a_envoyer,										//ligne à ecrire par sprintf 
			"%38s %s< %s >%s " ,	 								//format
			global.interface_line[numero_de_ligne].name, 			//nom de la ligne
			surligner?WRITE_BLACK_ON_WHITE:WRITE_WHITE_ON_BLACK,	//surligné ou non
			(global.interface_line[numero_de_ligne].element_list)->elements[global.config[numero_de_ligne]], //config courante sur la ligne
			WRITE_WHITE_ON_BLACK									//si surligné on arrête le surlignement
			);
	print_UART1(ligne_a_envoyer);
}

#endif /* def INTERFACE_GRAPHIQUE */	

void interface_graphique_envoi_message_can()
{
	CAN_msg_t message;
	
	message.sid = SUPER_ASK_CONFIG;	
	message.size = 8;
	message.data[0] = global.config[STRATEGIE];
	message.data[1] = global.config[COLOR];
	message.data[2] = global.config[EVITEMENT];
	message.data[3] = global.config[BALISE];
	
	CAN_send(&message);
	BUFFER_add(&message); // Ajout dans le buffer du message de config envoyé à la carte stratégie
}
	
void print_UART1(char* s)
{
	Uint16 index=0;
	while(s[index]!='\0')
	{
		UART1_putc(s[index]);
		index++;
	}
}

void UART1_put_Uint8(Uint8 i)
{
	if(i>=100)
	{
		UART1_putc((i/100)%10+'0');
	}	
	if(i>=10)
	{
		UART1_putc((i/10)%10+'0');
	}
		
	UART1_putc((i%10)+'0');
}

void moveto(Uint8 x, Uint8 y)
{
	char chaine[8];
	sprintf(chaine,"\E[%d;%dH",y,x);
	print_UART1(chaine);
}

//void INTERFACE_GRAPHIQUE_choice_send_selftest(bool_e surligner){
//	char ligne_a_envoye[100];
//	print_UART1(HIDE_CURSOR);
//	print_UART1(WRITE_WHITE_ON_BLACK);
//	moveto(20,12);
//	printf("Souhaitez-vous lancer le selftest ?");
//	sprintf(
//		ligne_a_envoye,
//		" < %s >%s",
//		surligner?WRITE_BLACK_ON_WHITE:WRITE_WHITE_ON_BLACK,
//}	
