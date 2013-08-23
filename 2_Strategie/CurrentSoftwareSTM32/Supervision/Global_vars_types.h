/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech
 *
 *  Fichier : Global_vars_types.h
 *  Package : Standard_Project
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 *  Version 20081205
 */

#ifndef QS_GLOBAL_VARS_H
	#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

/* 
STRUCTURES UTILISEES POUR L'INTERFACE GRAPHIQUE : 
une ligne de l'interface est constituée d'un nom et d'une liste d'éléments (ex: Bleu, Jaune) 
*/

#define TAILLE_MAX_ELEMENT	32

typedef char element_t[TAILLE_MAX_ELEMENT];

typedef const struct
{
	Uint8 nb_elements;
	element_t elements[];
}element_list_t;	

typedef struct
{
	char* name;
	element_list_t* element_list;
}line_t;	

typedef enum{
	ERREUR=0,OK=1
}resultat_test_e;

typedef struct{
	resultat_test_e etat_moteur_gche;
	resultat_test_e etat_moteur_dt;
	resultat_test_e etat_roue_codeuse_gche;
	resultat_test_e etat_roue_codeuse_dte;
	Uint8 etats_capteurs;
	resultat_test_e biroute_placee;
	resultat_test_e etat_balise_ir;
	resultat_test_e etat_balise_us;
	resultat_test_e synchro_balise;
	bool_e test_strat;
	bool_e test_asser;
	bool_e test_act;
	bool_e test_balise;
}test_carte_t;

/*
 *	inserez ici vos inclusions de fichers et vos definitions de types
 *	nécessaires à Global_vars.h
 */

typedef enum{
	STRATEGIE,
	COLOR,
	EVITEMENT,
	BALISE,
	DEBUG,
	BUFFER,
	EEPROM,
	XBEE,
	U1_VERBOSE,
	NB_CONFIG_FIELDS
	}config_fields_e;

#include "Buffer_types.h" //implémentation des structures du buffer circulaire qui sauvegarde les messages can échangés durant le match 

#define FLECHE_HAUT 0x1B5B41
	#define FLECHE_BAS  0x1B5B42
	#define FLECHE_GAUCHE 0x1B5B44
	#define FLECHE_DROITE 0x1B5B43
	#define WRITE_BLACK_ON_WHITE "\E[30;47m"
	#define WRITE_WHITE_ON_BLACK "\E[37;40m"
	#define WRITE_BLUE "\E[34m"
	#define WRITE_YELLOW "\E[33m"
	#define WRITE_RED "\E[31m"
	#define UNDERLINE "\E[7m"
	#define ERASE_DISPLAY "\E[2J"
	#define ERASE_LINE "\E[K"
	#define HIDE_CURSOR "\E[?25l"
