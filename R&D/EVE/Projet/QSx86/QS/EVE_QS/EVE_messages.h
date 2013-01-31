/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : EVE_messages.h
 *	Package : EVE_QS
 *	Description : Messages utilisés par le logiciel
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef EVE_MESSAGES_H
	#define EVE_MESSAGES_H

	#include "EVE_all_x86.h"

	#include <sys/ipc.h>
	#include <sys/msg.h>
	#include <sys/shm.h>
	#include <sys/types.h>

	// Chaque Processus/Thread est identifié par un numéro pour simplifier la communication entre processus
	#define ID_ACTIONNEURS	21
	#define ID_BALISES		22
	#define ID_PROPULSION	23
	#define ID_STRATEGIE	24
	#define ID_SUPERVISION	25
	#define ID_CAN			26
	#define ID_CAN2			27
	#define ID_IHM_QS_DATA	28
	#define ID_IHM_UART		29
	#define ID_GLOBAL_CLOCK	99

	// Les BAL (Boîtes Aux Lettres) pour les envois de messages sont utilisées à sens unique
	// La convention de nommage pour les noms est la suivante :
	// ID_envoyeur suivit de ID_receveur

	// BAL cartes vers CAN, CAN2, IHM QS_DATA et IHM UART
	#define	BAL_ACTIONNEURS_TO_CAN			2126
	#define	BAL_ACTIONNEURS_TO_CAN2			2127
	#define BAL_ACTIONNEURS_TO_IHM_QS_DATA	2128
	#define BAL_ACTIONNEURS_TO_IHM_UART		2129
	#define BAL_BALISES_TO_CAN				2226
	#define BAL_BALISES_TO_CAN2				2227
	#define BAL_BALISES_TO_IHM_QS_DATA		2228
	#define BAL_BALISES_TO_IHM_UART			2229
	#define BAL_PROPULSION_TO_CAN			2326
	#define BAL_PROPULSION_TO_CAN2			2327
	#define BAL_PROPULSION_TO_IHM_QS_DATA	2328
	#define BAL_PROPULSION_TO_IHM_UART		2329
	#define BAL_STRATEGIE_TO_CAN			2426
	#define BAL_STRATEGIE_TO_CAN2			2427
	#define BAL_STRATEGIE_TO_IHM_QS_DATA	2428
	#define BAL_STRATEGIE_TO_IHM_UART		2429
	#define BAL_SUPERVISION_TO_CAN			2526
	#define BAL_SUPERVISION_TO_CAN2			2527
	#define BAL_SUPERVISION_TO_IHM_QS_DATA	2528
	#define BAL_SUPERVISION_TO_IHM_UART		2529

	// BAL CAN vers cartes
	#define BAL_CAN_TO_ACTIONNEURS	2621
	#define BAL_CAN_TO_BALISES		2622
	#define BAL_CAN_TO_PROPULSION	2623
	#define BAL_CAN_TO_STRATEGIE	2624
	#define BAL_CAN_TO_SUPERVISION	2625

	// BAL CAN2 vers cartes
	#define BAL_CAN2_TO_ACTIONNEURS	2721
	#define BAL_CAN2_TO_BALISES		2722
	#define BAL_CAN2_TO_PROPULSION	2723
	#define BAL_CAN2_TO_STRATEGIE	2724
	#define BAL_CAN2_TO_SUPERVISION	2725

	// BAL IHM_QS_DATA vers cartes
	#define BAL_IHM_QS_DATA_TO_ACTIONNEURS	2821
	#define BAL_IHM_QS_DATA_TO_BALISES		2822
	#define BAL_IHM_QS_DATA_TO_PROPULSION	2823
	#define BAL_IHM_QS_DATA_TO_STRATEGIE	2824
	#define BAL_IHM_QS_DATA_TO_SUPERVISION	2825

	// BAL IHM_UART vers cartes n'existe pas, EVE ne va pas faire de printf sur un code de test :)


	// L'accès à GLOBAL_CLOCK se fait par mémoire partagée
	// Ici, on n'a qu'une seule zone mémoire accessible par tout le monde
	// Seul GLOBAL_CLOCK écrit dans la zone, les autres processus se contentent de lire
	// On peut ainsi se passer de sécurités type sémaphores
	// Un seul identifiant est nécessaire ici et accessible par tout le monde
	#define MEMORY_GLOBAL_CLOCK		9999

	// Types de messages à insérer dans la variable mtype des structures des BAL
	#define QUEUE_QS_DATA_MTYPE	10	
	#define QUEUE_CAN_MTYPE		11
	#define QUEUE_UART_MTYPE	12

	// Liste des différents identifiants possibles pour la communication entre l'IHM et les cartes
	typedef enum
	{
		QS_UNDEFINED_DATA = 0,	// Utile pour l'initialisation
		QS_START_UART1 = 1, 
		QS_STOP_UART1 = 2,
		QS_START_UART2 = 3, 
		QS_STOP_UART2 = 4,
		QS_LED_RUN = 5,
		QS_LED_CAN = 6,
		QS_LED_UART = 7,
		QS_LED_USER = 8,
		QS_LED_USER2 = 9,
		QS_LED_ERROR = 10,
		QS_BUTTON_1 = 11,
		QS_BUTTON_2 = 12,
		QS_BUTTON_3 = 13,
		QS_BUTTON_4 = 14,
		QS_BUTTON_RESET = 15,
		QS_PWM = 20,	// Message contenant les informations de la PWM
		QS_SIMULATION_RUN = 8000,	// Demande à la simulation de se lancer ou de reprendre son exécution après une pause
		QS_SIMULATION_PAUSE = 8001,	// Demande de pause de la simulation de l'IHM
		QS_SIMULATION_STOP = 8002	// Demande d'arrêt de la simulation de l'IHM
	} QS_data_e;

	/* Information relatives aux contenus des messages QS_DATA */
	#define BUTTON_PRESSED	1	// Indique que le bouton vient d'être pressé
	#define BUTTON_RELEASED	0	// Indique que le bouton vient d'être relaché

	// structure pour les BAL entre les cartes et l'IHM pour les valeurs QS
	typedef struct
	{
		long mtype;		// type de message, doit être strictement supérieur à 0 !
		QS_data_e data_id;
		Uint32 data1;
		Uint32 data2;
		Uint32 data3;
	} EVE_QS_data_msg_t;

	// taille maximale de caractères encapsulable dans un message à destination de l'IHM
	#define MAX_SIZE_DEBUG_PRINTF	400	// Taille max d'un debug_printf
	#define MAX_UART_CHARACTER	20		// Taille max d'un message printf par la BAL pour l'UART

	// structure pour les BAL entre les cartes et l'IHM pour l'UART
	typedef struct
	{
		long mtype;		// type de message, doit être strictement supérieur à 0 !
		Uint8 uart_number;	// Numéro de l'UART utilisé
		Uint8 data[MAX_UART_CHARACTER];
		Uint8 end_data;	// Doit être mis à zéro pour faciliter la lecture des messages... à voir si on ne l'enlève pas...
//#warning END DATA FOR DEBUG_PRINTF
	} EVE_UART_msg_t;

	// balises des messages CAN
	#define CAN_MESSAGE_START	01
	#define CAN_MESSAGE_STOP	04
	#define CAN_ADD_FILTER		10	// Indique qu'il faut ajouter un filtre
	#define CAN_REMOVE_FILTER	20	// Indique qu'il faut supprimer un filtre

	#define CAN_MSG_DATA_SIZE	8	// Taille des données des messages CAN

	// structure pour les BAL entre le CAN, les cartes et l'IHM pour les messages CAN
	typedef struct
	{
		long mtype;		// type de message, doit être strictement supérieur à 0 !
		Uint8 start_msg;
		Uint16 sid;
		Uint8 data[CAN_MSG_DATA_SIZE];
		Uint8 size;
		Uint8 end_msg;
	} EVE_CAN_msg_t;

	// Macros de calcul de la taille utile lors des appels de fonctions d'envoi et réception
	#define EVE_QS_DATA_MSG_T_SIZE	(sizeof(EVE_QS_data_msg_t)-sizeof(long))
	#define EVE_UART_MSG_T_SIZE		(sizeof(EVE_UART_msg_t)-sizeof(long))
	#define EVE_CAN_MSG_T_SIZE		(sizeof(EVE_CAN_msg_t)-sizeof(long))

	// Structure pour la mémoire partagée utilisée avec l'horloge générale
	typedef struct
	{
		volatile EVE_time_t global_time_memory;
	} EVE_clock_memory_t;


	/**************************** Fonctions relatives aux BAL de communication ****************************/
	// BAL : Boîte Aux Lettres, sorte de file de messages de type FIFO permettant la communication interprocessus

	// Fonction de création de la BAL
	// param : key_queue est l'identifiant spécifique requis pour créer la file
	// return : -1 si il y a une erreur lors de la création de la file
	// return : un entier positif qui est l'identificateur de la file de messages
	Uint32 EVE_create_msg_queue(Uint32 key_queue);

	// Fonction de destruction de la BAL
	// A n'utiliser que lorsque plus aucun processus ne va envoyer de message sinon il y aura un code erreur retourné
	// param : queue_id est l'identifiant unique de la BAL
	// return : END_SUCCESS si la file a correctement été détruite
	// return : ERROR_DESTROY_QUEUE si la file n'a pas pu être correctement détruite
	Uint16 EVE_destroy_msg_queue(Uint32 queue_id);

	// Fonction qui contrôle le nombre de messages disponibles dans la BAL
	// param : queue_id est l'identifiant unique de la BAL
	// return : 0 si aucun message n'est disponible
	// return : > 0 indiquant le nombre de messages disponibles
	// return : -1 si une erreur est survenue pendant la lecture
	Uint16 EVE_check_messages_numbers(Uint32 queue_id);


	/********************************* Ecriture/Lecture données QS ********************************************/

	// Fonction de lecture de la BAL spécifique aux données QS
	// param : queue_id est l'identifiant unique de la BAL
	// param : data permettra de renvoyer les données lues dans la BAL
	// return : END_SUCCESS si la réception s'est correctement déroulée
	// return : ERROR_RECEIVE_QUEUE si la réception s'est mal déroulée
	// return : ERROR_NO_MESSAGE_QUEUE si il n'y a aucun message dans la file
	Uint16 EVE_read_new_msg_QS(Uint32 queue_id, EVE_QS_data_msg_t* data);

	// Fonction d'écriture de la BAL spécifique aux données QS
	// param : queue_id est l'identifiant unique de la BAL
	// param : data contient les données qui devront être envoyées dans la BAL
	// return : END_SUCCESS si le message est correctement envoyé
	// return : ERROR_SEND_QUEUE si l'envoi ne s'est pas correctement déroulé
	Uint16 EVE_write_new_msg_QS(Uint32 queue_id, EVE_QS_data_msg_t data);


	/********************************* Ecriture/Lecture données UART ******************************************/

	// Fonction de lecture de la BAL spécifique aux données UART
	// param : queue_id est l'identifiant unique de la BAL
	// param : data permettra de renvoyer les données lues dans la BAL
	// return : END_SUCCESS si la réception s'est correctement déroulée
	// return : ERROR_RECEIVE_QUEUE si la réception s'est mal déroulée
	// return : ERROR_NO_MESSAGE_QUEUE si il n'y a aucun message dans la file
	Uint16 EVE_read_new_msg_UART(Uint32 queue_id, EVE_UART_msg_t* data);

	// Fonction d'écriture de la BAL spécifique aux données UART
	// param : queue_id est l'identifiant unique de la BAL
	// param : data contient les données qui devront être envoyées dans la BAL
	// return : END_SUCCESS si le message est correctement envoyé
	// return : ERROR_SEND_QUEUE si l'envoi ne s'est pas correctement déroulé
	Uint16 EVE_write_new_msg_UART(Uint32 queue_id, EVE_UART_msg_t data);


	/********************************* Ecriture/Lecture données CAN *******************************************/

	// Fonction de lecture de la BAL spécifique aux données CAN
	// param : queue_id est l'identifiant unique de la BAL
	// param : data permettra de renvoyer les données lues dans la BAL
	// return : END_SUCCESS si la réception s'est correctement déroulée
	// return : ERROR_RECEIVE_QUEUE si la réception s'est mal déroulée
	// return : ERROR_NO_MESSAGE_QUEUE si il n'y a aucun message dans la file
	Uint16 EVE_read_new_msg_CAN(Uint32 queue_id, EVE_CAN_msg_t* data);

	// Fonction d'écriture de la BAL spécifique aux données CAN
	// param : queue_id est l'identifiant unique de la BAL
	// param : data contient les données qui devront être envoyées dans la BAL
	// return : END_SUCCESS si le message est correctement envoyé
	// return : ERROR_SEND_QUEUE si l'envoi ne s'est pas correctement déroulé
	Uint16 EVE_write_new_msg_CAN(Uint32 queue_id, EVE_CAN_msg_t data);



	/*********************** Fonctions relatives à la gestion de la mémoire partagée *************************/

	// WARNING : la fonction d'écriture dans la mémoire partagée est déportée dans le fichier global_clock.h
	// En effet, seule l'horloge a le droit d'écrire dans la mémoire partagée
	// Cela permet d'éviter d'utiliser des sémaphores pour sécuriser les accès et rendre le code plus simple
	// Nous avons choisi de mettre cette fonction d'écriture dans un autre fichier pour éviter tout problème d'écriture concurrente
	
	// Fonction de création  et de liaison à la mémoire partagée
	// Permet de créer le segment mémoire qui sera utilisé et de lier le processus au segment
	// ATTENTION il faut correctement détacher ce segment à la fin de l'exécution du processus !
	// param : memory_reference correspond à l'adresse du segment mémoire qui sera utilisé
	// return END_SUCCESS en cas de succès de la fonction
	// return ERROR_SHMGET en cas d'échec de la création
	// return ERROR_SHMAT en cas d'échec de la liaison
	Uint16 EVE_create_and_bound_global_clock_memory(char** memory_reference);

	// Fonction permettant de se détacher d'un segment mémoire
	// Cette fonction doit obligatoirement être appelée à la fin de l'exécution d'un processus utilisant un segment de mémoire partagée !
	// Lorsque tous les segments liés seront détachés du segment mémoire, il s'autodétruira 
	// param : memory_reference correspond à l'adresse du segment mémoire qui sera utilisé
	// return END_SUCCESS en cas de succès 
	// return ERROR_SHMDT en cas d'échec
	Uint16 EVE_unbound_global_clock_memory(char* memory_reference);
	
	// Fonction de lecture de la mémoire partagée de l'horloge
	// On peut ainsi accéder au temps général
	// param : memory_reference correspond à l'adresse du segment mémoire qui sera utilisé
	// return le temps actuel de l'horloge en millisecondes
	EVE_time_t EVE_get_global_clock(char* memory_reference);


	#ifdef EVE_MESSAGES_C
		// Gestion des retours d'erreurs
		// return la valeur 'error_id' permettant de la faire remonter
		static Uint16 EVE_MESSAGES_errors(Uint16 error_id, char* commentary);

	#endif /* EVE_MESSAGES_C */

#endif /* EVE_MESSAGES_H */

