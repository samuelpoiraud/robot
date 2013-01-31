/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : EVE_types.h
 *	Package : EVE_QS
 *	Description : Types et fonctions utilisés par le logiciel
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111201
 */

#ifndef EVE_TYPES_H
	#define EVE_TYPES_H

	/* Macros de retours d'erreurs */
	// Retour utile pour tout le monde
	#define END_SUCCESS 		0	// Succés de l'exécution de la fonction

	// Retour utile pour toutes les fonctions de Thread (void*)
	#define END_THREAD			0	// Signifie la fin d'un thread

	// Retours utiles pour le Manager 
	#define ERROR_STATUS			1	// Inutile pour l'instant
	#define ERROR_CONFIG			2	// Erreur sur le fichier de configuration
	#define ERROR_MAKEFILE 			3	// Fichier Makefile absent
	#define ERROR_UNKNOWN_CARD		4	// Carte demandée inconnue
	#define ERROR_COMPILATION		5	// Erreur lors de la compilation
	#define ERROR_THREAD_CREATION	6	// Erreur lors de la création du thread

	// Retours utiles pour EVE_messages
	#define ERROR_SHMGET		50		// Création mémoire partagée
	#define ERROR_SHMAT			51		// Liaison mémoire partagée
	#define ERROR_SHMDT			52		// Détachement mémoire partagée
	#define ERROR_CREATE_QUEUE	53		// Création de la BAL
	#define ERROR_DESTROY_QUEUE	54		// Destruction de la BAL
	#define ERROR_SEND_QUEUE	55		// Envoi d'un message par la BAL
	#define ERROR_RECEIVE_QUEUE	56		// Réception d'un message par la BAL
	#define ERROR_NO_MESSAGE_QUEUE	57	// Réception impossible car aucun message dans la BAL
	#define ERROR_CHECK_QUEUE	58		// Contrôle du nombre de messages dans la BAL

	// Retours utiles pour global_clock
	#define ERROR_UNBOUND_CLOCK		60	// Détachement de l'horloge générale
	#define ERROR_CREATE_CLOCK		61	// Création de l'horloge générale
	#define ERROR_UPDATE_CLOCK		62	// Mise à jour de l'horloge

	// Retours utiles pour CAN
	#define ERROR_INIT_CAN_BAL		100	// Création des BAL

	// Retours utiles pour EVE_manager
	#define ERROR_INIT_EVE_CARD_MANAGER		70	// Problème lors de l'initialisation des systèmes de communication

	#ifndef USE_QSx86
		// Redéfinition des types pour avoir les mêmes que sur les codes du robot
		typedef unsigned char Uint8;
		typedef signed char Sint8;
		typedef unsigned short Uint16;
		typedef signed short Sint16;
		typedef unsigned long Uint32;
		typedef signed long Sint32;

		// Type booléen
		typedef enum
		{
			FALSE = 0,
			TRUE = 1
		} bool_e;
	#else
		// On récupère les définitions de la QS de la carte
		#include "../QS_types.h"
	#endif /* USE_QSx86 */

	typedef Uint32 EVE_time_t;

	// Enumération des cartes (très pratique pour faire des tableaux)
	typedef enum
	{
		ACTIONNEURS = 0,
		BALISES = 1,
		PROPULSION = 2,
		STRATEGIE = 3,
		SUPERVISION = 4,
		CARDS_NB = 5
	} cards_id_e;


	// Fonction de printf pour la fin des threads
	// param : error_id est l'identifiant de l'erreur
	// param : commentary contient un commentaire sur la commande
	// return la valeur de 'error_id'
	Uint16 THREAD_finish(Uint16 error_id, char* commentary);


#endif /* EVE_TYPES_H */
