/*
 *	Club Robot ESEO 2009 - 2011
 *	Chomp, ChekNorris
 *
 *	Fichier : Buttons.c
 *	Package : Supervision
 *	Description : Gestion de l'appui sur les boutons de la carte supervision 
 *	Auteur : Ronan
 *	Version 20110413
 */
 
#define BUTTON_C
#include "Buttons.h"
	
void BUTTON_selftest();
void BUTTON_interface_graphique();
	
void BUTTON_init() 
{
	BUTTONS_init();
	BUTTONS_define_actions(BUTTON1,BUTTON_selftest, NULL, 0);
//	OBSOLETE :
//	BUTTONS_define_actions(BUTTON2,BUTTON_interface_graphique, NULL, 0);
	BUTTONS_define_actions(BUTTON3,BUFFER_flush, NULL, 0);
	#ifdef EEPROM_CAN_MSG_ENABLE
		BUTTONS_define_actions(BUTTON4,EEPROM_CAN_MSG_verbose_previous_match, NULL, 0);
	#else
		BUTTONS_define_actions(BUTTON4,NULL, NULL, 0);
	#endif
}

void BUTTON_update() 
{
	BUTTONS_update();
}	

void BUTTON_selftest()
{
	global.test_selftest_enable = TRUE;	
}

void BUTTON_interface_graphique()
{
	global.interface_graphique_enable = TRUE;
//	global.config[TRANSMIT]=FALSE;
		
	#ifdef INTERFACE_GRAPHIQUE
		INTERFACE_GRAPHIQUE_afficher_tout_le_menu();
	#endif /* def INTERFACE_GRAPHIQUE */	

	#ifdef INTERFACE_TEXTE
		print_UART1("Bienvenue sur l'interface de configuration du robot CheckNorris !\r\n");
		print_UART1("\r\nPour configurer le ChekNorris, veuillez entrer le parametre, UN ESPACE, et le choix souhaite\r\n\r\n");
		INTERFACE_TEXTE_afficher_menu();
	#endif /* def INTERFACE_TEXTE */
}
