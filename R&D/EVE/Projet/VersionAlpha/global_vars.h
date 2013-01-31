/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : global_vars.h
 *	Package : EVE
 *	Description : Variables globales utilisés dans le simulateur
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111215
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#include "EVE_QS/EVE_all.h"

	// Etats possibles de la simulation
	#define SIMULATION_RUN		2	// Démarrage de la simulation en cours
	#define SIMULATION_PAUSE	1	// Interruption de la simulation en cours
	#define SIMULATION_STOP		0	// Arrêt de la simulation en cours
	#define SIMULATION_QUIT		3	// Arrêt du logiciel

	// Liste des threads avec leurs identifiants
	typedef enum
	{
		THREAD_ACTIONNEURS = 0,
		THREAD_BALISES = 1,
		THREAD_PROPULSION = 2,
		THREAD_STRATEGIE = 3,
		THREAD_SUPERVISION = 4,
		THREAD_CAN = 5,
		THREAD_GLOBAL_CLOCK = 6,
		THREAD_IHM_DISPLAY = 7,
		THREAD_IHM_RECEIVER = 8,
		THREADS_NUMBER = 9
	} threads_id_e;

	// Structure des variables globales
	typedef struct
	{
		// Tableau permettant de synthétiser l'état de tous les threads
		// Si la valeur vaut TRUE, le thread est en cours d'exécution
		// Si la valeur vaut FALSE, le thread est terminé
		bool_e threads_state[THREADS_NUMBER];

		// Cartes à utiliser, dans l'ordre : ACTIONNEURS,BALISES,PROPULSION,STRATEGIE,SUPERVISION
		bool_e use_cards[CARDS_NB];

		// Temps général de l'horloge permettant un accès entre Threads
		// Utiliser le mutex correspondant avant d'accéder à cette variable !
		volatile EVE_time_t global_time;

		// Variable permettant la liaison entre l'IHM display et l'IHM receiver
		GtkButton *button_save;

		// Variable contenant les UARTs utilisés sur l'IHM
		Uint8 use_uart[2];

		// Variable permettant de connaitre le statut de la simulation
		// SIMULATION_STOP : simulation à l'arrêt
		// SIMULATION_RUN : simulation en cours
		// SIMULATION_PAUSE : simulation en pause
		// SIMULATION_QUIT : arrêt d'EVE
		Uint8 sim_status;

		// Variable permettant d'avertir le reset des cartes
		bool_e reset_cards;

		// Variable permettant de connaitre l'onglet affiché sur l'IHM
		Uint8 notebook_page;

		// Variable queue_id des files transmettant les DATA de IHM vers les cartes
		Uint32 queue_data_id_actionneurs;
		Uint32 queue_data_id_balises;
		Uint32 queue_data_id_propulsion; 
		Uint32 queue_data_id_strategie;
		Uint32 queue_data_id_supervision;
	
		// Mutex pour l'horloge
		pthread_mutex_t global_time_mutex;
		// Mutex pour les états des threads
		pthread_mutex_t threads_state_mutex;
		// Mutex pour les cartes à utiliser
		pthread_mutex_t use_cards_mutex;
		// Mutex pour la référence de l'IHM
		pthread_mutex_t button_save_mutex;
		// Mutex pour les variables UART
		pthread_mutex_t use_uart_mutex;
		// Mutex pour le status de la simulation
		pthread_mutex_t sim_status_mutex;
		// Mutex pour l'onglet courant
		pthread_mutex_t notebook_page_mutex;
		// Mutex pour les queue_id des DATA
		pthread_mutex_t queue_id_mutex;
		// Mutex pour l'appui sur les reset des cartes
		pthread_mutex_t reset_cards_mutex;
	} global_t;

	// Variables globales
	global_t global;

	
	

	// Fonction d'initialisation des variables globales
	// return END_SUCCESS en cas de succès
	Uint16 GLOBAL_VARS_init();

	// Fonction permettant de connaître l'état actuel de la simulation
	Uint16 GLOBAL_VARS_get_sim_status();

	// Fonction permettant de mettre à jour l'état courant de la simulation
	Uint16 GLOBAL_VARS_set_sim_status(Uint16 new_status);

	// Fonction permettant de connaître la valeur de use_cards
	Uint16 GLOBAL_VARS_get_use_cards(Uint16 card_id);

	// Fonction permettant de connaître la valeur de threads_state
	bool_e GLOBAL_VARS_get_threads_state(Uint16 thread_id);

	// Fonction permettant de mettre à jour la valeur de threads_state
	Uint16 GLOBAL_VARS_set_threads_state(Uint16 thread_id, bool_e state);

	// Fonction permettant de connaître si un reset a été effectué
	bool_e GLOBAL_VARS_get_reset_cards();

	// Fonction permettant de mettre à jour la valeur reset_cards sur state
	Uint16 GLOBAL_VARS_set_reset_cards(bool_e state);

#endif /* GLOBAL_VARS_H */

