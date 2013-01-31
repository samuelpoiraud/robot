/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_EVE_manager.h
 *	Package : QS_cartes
 *	Description : Gestions spécifiques au logiciel EVE dans les codes des cartes
 *	Auteur : Julien Franchineau & François Even
 *	Version 20120105
 */

#ifndef QS_EVE_MANAGER_H
	#define QS_EVE_MANAGER_H

	#include "QS_all.h"

	#include "QS_EVE_hard_emulation.h"
	
	#include "QS_timer.h"
	#include <signal.h>		// Permet de faire le reset

	// Nombre de timers disponibles
	typedef enum
	{
		ID_TIMER_1 = 0,
		ID_TIMER_2 = 1,
		ID_TIMER_3 = 2,
		ID_TIMER_4 = 3,
		TIMER_NB = 4
	} timers_e;
	// Structure des timers pour EVE
	typedef struct
	{
		Uint32 init_value;		// Temps où on a démarré le timer en ms
		Uint16 user_period;		// Temps demandé par l'utilisateur en ms
		bool_e timer_run;		// Indique si le timer est lancé (TRUE) ou pas (FALSE)
	} timer_structure_t;

	// Informations pour les PWM
	#define PWM_CHANNEL_1	1
	#define PWM_CHANNEL_2	2
	#define PWM_CHANNEL_3	3
	#define PWM_CHANNEL_4	4
	#define PWM_NB			5

	typedef struct
	{
		bool_e simulation_pause; 	// Indique que la simulation doit s'interrompre
		bool_e simulation_stop;		// Indique que la simulation doit se stopper

		// Identifiants de référence pour les BALs correspondantes
		Uint32 bal_id_card_to_can;	// Carte courante vers CAN
		Uint32 bal_id_card_to_can2;	// Carte courante vers CAN2
		Uint32 bal_id_card_to_ihm_qs_data;	// Carte courante vers IHM QS_DATA
		Uint32 bal_id_card_to_ihm_uart;		// Carte courante vers IHM UART
		Uint32 bal_id_can_to_card;	// CAN vers carte courante
		Uint32 bal_id_can2_to_card;	// CAN2 vers carte courrante
		Uint32 bal_id_ihm_qs_data_to_card;	// IHM QS_DATA vers carte courante

		// Pointeur de référence à utiliser pour accéder à la mémoire partagée
		char* clock_memory_reference;

		bool_e ihm_use_uart1;	// Autorise/Interdit l'envoi du flux de l'UART vers l'IHM
		bool_e ihm_use_uart2;	// Autorise/Interdit l'envoi du flux de l'UART2 vers l'IHM

		timer_structure_t timers[TIMER_NB];	// Timers de la carte
		Uint32 pwm_power[PWM_NB];			// Sauvegarde des valeurs de puissance de PWM
	} EVE_global_vars_t;

	extern EVE_global_vars_t eve_global;

// Ce fichier doit inclure :
// - une fonction qui sera appelée à chaque boucle du main de la carte
// - une fonction de réception des messages de la BAL des données
// - l'incrémentation des timers en fonction du temps général

	// Initialisation des fonctions relatives au fonctionnement d'EVE
	// return END_SUCCESS en cas de succès
	// return ERROR_INIT_EVE_CARD_MANAGER en cas d'échec
	Uint16 EVE_manager_card_init();

	// Fonction générale de la boucle while du main
	void EVE_manager_card();



	#ifdef QS_EVE_MANAGER_C
		// Fonction de traitement des timers à chaque boucle
		// Incrémente chaque timer lancé selon global_clock d'EVE, et lance une IT si besoin
		static void EVE_timers_update();

		// Détruit toutes les BALs proprement ainsi que la liaison à la mémoire partagée de l'horloge
		static void EVE_destroy_all();

	#endif /* QS_EVE_MANAGER_C */

#endif /* QS_EVE_MANAGER_H */

