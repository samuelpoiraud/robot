/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech'
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen (inspiré du package QS d'Axel Voitier)
 *  Version 20081010
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif
	
	volatile bufferCirculaire_t buffer;
	volatile Uint32 current_time_ms;		//compte les millisecondes écoulées depuis le lancement du Timer 3
	volatile Uint32 compteur_de_secondes;
	volatile Uint32 compteur_de_secondes_precedent_buffer;
	volatile Uint32 compteur_de_secondes_precedent_uart;
	
	volatile bool_e bouton1;						//état du bouton actuel
	volatile bool_e bouton2;
	volatile bool_e bouton3;
	volatile bool_e bouton4;
		
	volatile bool_e interface_graphique_enable;
	volatile bool_e test_selftest_enable;	
	volatile bool_e match_started;
	
	#ifdef INTERFACE_GRAPHIQUE
		volatile line_t interface_line[NB_CONFIG_FIELDS];
		volatile Uint8 indice_line;
	#endif /* def INTERFACE_GRAPHIQUE */
	
		
	volatile Uint8 config[NB_CONFIG_FIELDS];	//tableau de configuration du match
	volatile test_carte_t test_cartes;
	volatile bool_e flag_act;
	volatile bool_e flag_asser;
	volatile bool_e flag_strat;
	volatile bool_e flag_beacon;
	
	volatile module_id_e XBEE_i_am_module;
	volatile module_id_e XBEE_module_id_destination;
	
	/*  Note : Variables globales communes à tous les codes
	 *
	 *	Buffer de reception de l'UART 1
	 *	Uint8 u1rxbuf[UART_RX_BUF_SIZE];
	 *
	 *	Buffer de reception de l'UART 2
	 *	Uint8 u2rxbuf[UART_RX_BUF_SIZE];
	 *
	 *	Buffer de reception des messages du bus CAN
	 *	CAN_msg_t can_buffer[CAN_BUF_SIZE];
	 *	Position du dernier message recu
	 *	volatile Uint16 can_rx_num;
	 *
	 *	Buffer de reception des messages du bus CAN2
	 *	CAN_msg_t can2_buffer[CAN_BUF_SIZE];
	 *	Position du dernier message recu
	 *	volatile Uint16 can2_rx_num;
	 */
	 
	 
#endif /* ndef GLOBAL_VARS_H */
