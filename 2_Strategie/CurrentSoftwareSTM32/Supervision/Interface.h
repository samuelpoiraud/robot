/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : Interface.h
 *	Package : Supervision
 *	Description : Interface de configuration
 *	Auteur : Ronan & Aurélien
 *	Version 20100119
 */
 
#include "QS/QS_all.h"
 
#ifndef INTERFACE_H
	#define INTERFACE_H
	
	#ifndef VERBOSE_MODE
		#undef NULL
		#include <stdio.h>
	#endif /* ndef VERBOSE_MODE */
	
	/************************ INTERFACE GRAPHIQUE *************************/
	
	void INTERFACE_GRAPHIQUE_init();
	
	void INTERFACE_GRAPHIQUE_process_u1rx(void);
	
	void INTERFACE_GRAPHIQUE_afficher_tout_le_menu();
	
	void INTERFACE_GRAPHIQUE_afficher_config_actuelle();
	
	/******************** INTERFACE GRAPHIQUE *********************/
	
	void print_UART1(char* s); 	/* Permet d'evoyer des chaînes de caractères "caractères par caractères" sur l'UART1 */
	
	void UART1_put_Uint8(Uint8 i);

	void moveto(Uint8 x, Uint8 y);

	#ifdef INTERFACE_C
		
		#include <string.h>
		#include "QS/QS_uart.h"
		#include "QS/QS_CANmsgList.h"
		#include "QS/QS_can.h"
		#include "Buffer.h"
		
		/************************ INTERFACE GRAPHIQUE *************************/
				
		void INTERFACE_GRAPHIQUE_analyse_sequence_read(Uint32 sequence_read);
				
		void INTERFACE_GRAPHIQUE_afficher_config_actuelle();
		
		void INTERFACE_GRAPHIQUE_afficher_ligne(Uint8 numero_de_ligne, bool_e surligner);

		
		/******************** INTERFACE GRAPHIQUE *********************/
		
		void interface_graphique_envoi_message_can(void);
			
	#endif				
#endif
