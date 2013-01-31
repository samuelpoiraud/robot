/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : manager.c
 *	Package : EVE
 *	Description : Gestionnaire des threads et processus
 *	Auteur : Julien Franchineau
 *	Version 20111201
 */

#define MANAGER_C

#include "manager.h"

// Programme principal de gestion de threads
Uint16 MANAGER_main()
{
	Uint16 error;
	Uint16 current_status,old_status;
	bool_e quit_authorization = FALSE;
	Uint8 i;

	// Initialisation et Compilation des codes
	error = MANAGER_init();
	if(MANAGER_errors(error,"Initialisation")!=END_SUCCESS)
	{
		return error;
	}

	// Démarrage propre des threads EVE
	MANAGER_global_clock_thread();	// Démarrage de l'horloge globale
	MANAGER_can_thread();			// Démarrage du CAN
	MANAGER_ihm_thread();			// Démarrage de l'IHM

	current_status = GLOBAL_VARS_get_sim_status();
	old_status = current_status;

	// Boucle d'exécution, on sort quand on a un status SIMULATION_QUIT
	while(current_status != SIMULATION_QUIT)
	{
		if((current_status == SIMULATION_RUN) && (old_status == SIMULATION_STOP))
		{
			// Nouveau départ de simulation, on relance les cartes
			MANAGER_cards_threads();	// Démarrage des codes
		}
		else if((current_status == SIMULATION_RUN) && (GLOBAL_VARS_get_reset_cards() == TRUE))
		{
			// On libère un peu de temps CPU pour s'assurer que le code s'est bien arrêté
			usleep(10000);	// 10ms
			MANAGER_cards_threads();	// Va relancer le code qui vient de s'arrêter
			GLOBAL_VARS_set_reset_cards(FALSE);	// On met la variable à jour 
		}
		else
		{
			// On libère du temps CPU vu qu'il n'y a rien à faire
			usleep(500000);	// 500 ms
		}

		// Sauvegarde de l'ancien status
		old_status = current_status;
		// Mise à jour du status
		current_status = GLOBAL_VARS_get_sim_status();
	}

	// On doit quitter, on va attendre que tout soit fermé 
	while(quit_authorization != TRUE)
	{
		quit_authorization = TRUE;
		for(i=0;i<THREADS_NUMBER;i++)
		{
			// Si un des threads est toujours en cours, on attend !
			if(GLOBAL_VARS_get_threads_state(i) == TRUE)
			{
				quit_authorization = FALSE;
printf("Thread : %d\n",i);
			}
		}
		// Libération de temps CPU
		usleep(500000);
	}

	// Tout est fermé, et détruit, on sort
	return END_SUCCESS;
}

// Compilation de toutes les cartes demandées, chargement des options
static Uint16 MANAGER_init()
{
	FILE* config_file;
	Uint16 error;
	Uint16 card_id;

	// Vérification de la présence du Makefile
	if(fopen("Makefile","r")==NULL)
	{
		return ERROR_MAKEFILE;	
	}

	// Ouverture et vérification de la présence du fichier eve.config
	config_file = fopen("_Configuration/eve.config","r");
	if(config_file==NULL)
	{
		return MANAGER_errors(ERROR_CONFIG, "Fichier config inexistant");
	}

	// Lecture et chargement du fichier de configuration
	error = MANAGER_load_configuration_file(config_file);
	// Fermeture du fichier
	if(fclose(config_file) != 0)
	{
		return MANAGER_errors(ERROR_CONFIG,"Fermeture impossible");
	}
	// Vérification du chargement de la configuration
	if(error!=END_SUCCESS)
	{
		return MANAGER_errors(error, "Load configuration");
	}

	// Compilation des codes
	for(card_id=0;card_id<CARDS_NB;card_id++)
	{
		if(GLOBAL_VARS_get_use_cards(card_id) == TRUE)
		{
			error = MANAGER_compile(card_id);
			if(error != END_SUCCESS)
			{
				return ERROR_COMPILATION;
			}
		}
	}

	return END_SUCCESS;
}

// Programme de compilation d'une carte
static Uint16 MANAGER_compile(cards_id_e card_id)
{
	Uint16 error;

	switch(card_id)
	{
		case ACTIONNEURS:
			error = system("make compile_actionneurs");
			if(error != END_SUCCESS)
			{
				return MANAGER_errors(ERROR_COMPILATION,"Actionneurs");
			}
			break;

		case BALISES:
			error = system("make compile_balises");
			if(error != END_SUCCESS)
			{
				return MANAGER_errors(ERROR_COMPILATION,"Balises");
			}
			break;

		case PROPULSION:
			error = system("make compile_propulsion");
			if(error != END_SUCCESS)
			{
				return MANAGER_errors(ERROR_COMPILATION,"Propulsion");
			}
			break;

		case STRATEGIE:
			error = system("make compile_strategie");
			if(error != END_SUCCESS)
			{
				return MANAGER_errors(ERROR_COMPILATION,"Strategie");
			}
			break;

		case SUPERVISION:
			error = system("make compile_supervision");
			if(error != END_SUCCESS)
			{
				return MANAGER_errors(ERROR_COMPILATION,"Supervision");
			}
			break;

		default:
			// Carte inconnue
			return ERROR_UNKNOWN_CARD;
			break;
	}

	return END_SUCCESS;
}

// Permet de lire le fichier de configuration et de charger options
static Uint16 MANAGER_load_configuration_file(FILE* config_file)
{
	char buffer[64];
	bool_e end_of_file = FALSE;

	// lecture du fichier de configuration
	pthread_mutex_lock(&global.use_cards_mutex);
	while(end_of_file==FALSE)
	{
		fscanf(config_file,"%s",buffer);
		// on vérifie qu'on a bien une ligne de commande
		// les lignes de commandes débutent par le caractère '$'
		if(buffer[0]=='$')
		{
			if(strcmp(buffer,END_OF_FILE)==0)
			{
				// Fin du fichier atteinte
				end_of_file = TRUE;
			}
			else if(strcmp(buffer,COMPILATION)==0)
			{
				// Codes à utiliser par EVE
				int use_value[1];
				fscanf(config_file,"%s",buffer);
				if(strcmp(buffer,"ACTIONNEURS")==0)
				{
					fscanf(config_file,"%d",use_value);
					global.use_cards[ACTIONNEURS] = use_value[0];
				}
				else if(strcmp(buffer,"BALISES")==0)
				{
					fscanf(config_file,"%d",use_value);
					global.use_cards[BALISES] = use_value[0];
				}
				else if(strcmp(buffer,"PROPULSION")==0)
				{
					fscanf(config_file,"%d",use_value);
					global.use_cards[PROPULSION] = use_value[0];
				}
				else if(strcmp(buffer,"STRATEGIE")==0)
				{
					fscanf(config_file,"%d",use_value);
					global.use_cards[STRATEGIE] = use_value[0];
				}
				else if(strcmp(buffer,"SUPERVISION")==0)
				{
					fscanf(config_file,"%d",use_value);
					global.use_cards[SUPERVISION] = use_value[0];
				}
				else
				{
					printf("USE UNKNOWN CARD\n");
					//return ERROR_UNKNOWN_CARD;
				}
			}
		}
	}
	pthread_mutex_unlock(&global.use_cards_mutex);
	
	return END_SUCCESS;
}

/********************************************** GESTION DES THREADS DES CARTES **********************************************/
// Permet de lancer les différents threads liés aux cartes
static Uint16 MANAGER_cards_threads()
{
	pthread_t actionneurs_thread, balises_thread, propulsion_thread, strategie_thread, supervision_thread;
	pthread_attr_t attributes;

	// Initialisation des attributs des threads
	// Par défaut, ils sont en mode normal avec notamment une politique "d'état joignable"
	// Cela nécessite de se synchroniser avec le thread pour correctement le terminer
	// Ici, on souhaite le rendre "détachable", et pouvoir le lancer sans plus jamais s'en préoccuper
	// Il se terminera quand le signal d'arrêt arrivera de l'IHM directement
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);

	pthread_mutex_lock(&global.use_cards_mutex);
	if((global.use_cards[ACTIONNEURS]==1) && (GLOBAL_VARS_get_threads_state(THREAD_ACTIONNEURS) == FALSE))
	{
		// Thread de gestion de la carte ACTIONNEURS
		if (pthread_create(&actionneurs_thread, &attributes, MANAGER_actionneurs_thread, NULL))
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
		    return MANAGER_errors(ERROR_THREAD_CREATION,"actionneurs_thread");
		}
	}

	if((global.use_cards[BALISES]==1) && (GLOBAL_VARS_get_threads_state(THREAD_BALISES) == FALSE))
	{
		// Thread de gestion de la carte BALISES
		if (pthread_create(&balises_thread, &attributes, MANAGER_balises_thread, NULL))
		{ 
			pthread_mutex_unlock(&global.use_cards_mutex);
		    return MANAGER_errors(ERROR_THREAD_CREATION,"balises_thread");
		}
	}

	if((global.use_cards[PROPULSION]==1) && (GLOBAL_VARS_get_threads_state(THREAD_PROPULSION) == FALSE))
	{
		// Thread de gestion de la carte PROPULSION
		if (pthread_create(&propulsion_thread, &attributes, MANAGER_propulsion_thread, NULL))
		{ 
			pthread_mutex_unlock(&global.use_cards_mutex);
		    return MANAGER_errors(ERROR_THREAD_CREATION,"propulsion_thread");
		}
	}

	if((global.use_cards[STRATEGIE]==1) && (GLOBAL_VARS_get_threads_state(THREAD_STRATEGIE) == FALSE))
	{
		// Thread de gestion de la carte STRATEGIE
		if (pthread_create(&strategie_thread, &attributes, MANAGER_strategie_thread, NULL))
		{ 
			pthread_mutex_unlock(&global.use_cards_mutex);
		    return MANAGER_errors(ERROR_THREAD_CREATION,"strategie_thread");
		}
	}

	if((global.use_cards[SUPERVISION]==1) && (GLOBAL_VARS_get_threads_state(THREAD_SUPERVISION) == FALSE))
	{
		// Thread de gestion de la carte SUPERVISION
		if (pthread_create(&supervision_thread, &attributes, MANAGER_supervision_thread, NULL))
		{ 
			pthread_mutex_unlock(&global.use_cards_mutex);
		    return MANAGER_errors(ERROR_THREAD_CREATION,"supervision_thread");
		}
	}
	pthread_mutex_unlock(&global.use_cards_mutex);

	return END_SUCCESS;
}

// Thread de gestion de la carte ACTIONNEURS
static void* MANAGER_actionneurs_thread()
{
	// On indique que le thread est démarré
	GLOBAL_VARS_set_threads_state(THREAD_ACTIONNEURS,TRUE);
	
	// Démarrage du processus de la carte
	system("./CodeActionneurs");
	printf("Fin CodeActionneurs\n");

	GLOBAL_VARS_set_threads_state(THREAD_ACTIONNEURS,FALSE);

	return END_THREAD;
}

// Thread de gestion de la carte BALISES
static void* MANAGER_balises_thread()
{
	// On indique que le thread est démarré
	GLOBAL_VARS_set_threads_state(THREAD_BALISES,TRUE);
	
	// Démarrage du processus de la carte
	system("./CodeBalises");
	printf("Fin CodeBalises\n");

	GLOBAL_VARS_set_threads_state(THREAD_BALISES,FALSE);

	return END_THREAD;
}

// Thread de gestion de la carte PROPULSION
static void* MANAGER_propulsion_thread()
{
	// On indique que le thread est démarré
	GLOBAL_VARS_set_threads_state(THREAD_PROPULSION,TRUE);
	
	// Démarrage du processus de la carte
	system("./CodePropulsion");
	printf("Fin CodePropulsion\n");

	GLOBAL_VARS_set_threads_state(THREAD_PROPULSION,FALSE);

	return END_THREAD;
}

// Thread de gestion de la carte STRATEGIE
static void* MANAGER_strategie_thread()
{
	// On indique que le thread est démarré
	GLOBAL_VARS_set_threads_state(THREAD_STRATEGIE,TRUE);

	// Démarrage du processus de la carte
	system("./CodeStrategie");
	printf("Fin CodeStrategie\n");

	GLOBAL_VARS_set_threads_state(THREAD_STRATEGIE,FALSE);

	return END_THREAD;
}

// Thread de gestion de la carte SUPERVISION
static void* MANAGER_supervision_thread()
{
	// On indique que le thread est démarré
	GLOBAL_VARS_set_threads_state(THREAD_SUPERVISION,TRUE);
	
	// Démarrage du processus de la carte
	system("./CodeSupervision");
	printf("Fin CodeSupervision\n");

	GLOBAL_VARS_set_threads_state(THREAD_SUPERVISION,FALSE);

	return END_THREAD;
}

/********************************************** GESTION DES AUTRES THREADS **********************************************/
// Thread de gestion de GLOBAL CLOCK
static Uint16 MANAGER_global_clock_thread()
{
	pthread_t global_clock_thread;
	pthread_attr_t attributes;

	// Initialisation des attributs des threads
	// Voir l'explication dans le .h
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);

	// Indication que le thread est lancé
	GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,TRUE);

	// Thread de gestion de l'horloge générale
	if (pthread_create(&global_clock_thread, &attributes, GLOBAL_CLOCK_thread, NULL))
	{ 
		// On indique que le thread n'est plus en cours d'exécution
		GLOBAL_VARS_set_threads_state(THREAD_GLOBAL_CLOCK,FALSE);
	    return MANAGER_errors(ERROR_THREAD_CREATION,"global_clock_thread");
	}

	return END_SUCCESS;
}

// Thread de gestion du CAN
static Uint16 MANAGER_can_thread()
{
	pthread_t can_thread;
	pthread_attr_t attributes;

	// Initialisation des attributs des threads
	// Voir l'explication dans le .h
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);

	// Indication que le thread est lancé
	GLOBAL_VARS_set_threads_state(THREAD_CAN,TRUE);

	// Thread de gestion du CAN
	if (pthread_create(&can_thread, &attributes, CAN_thread, NULL))
	{ 
		// On indique que le thread n'est plus en cours d'exécution
		GLOBAL_VARS_set_threads_state(THREAD_CAN,FALSE);
	    return MANAGER_errors(ERROR_THREAD_CREATION,"can_thread");
	}

	return END_SUCCESS;
}

// Thread de gestion de IHM
static Uint16 MANAGER_ihm_thread()
{
	pthread_t ihm_display_thread;
	pthread_t ihm_receiver_thread;
	pthread_attr_t attributes;

	// Initialisation des attributs des threads
	// Voir l'explication dans le .h
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);

	// Indication que le thread est lancé
	GLOBAL_VARS_set_threads_state(THREAD_IHM_DISPLAY,TRUE);

	// Thread de gestion de l'horloge générale
	if (pthread_create(&ihm_display_thread, &attributes, IHM_display_thread, NULL))
	{ 
		// On indique que le thread n'est plus en cours d'exécution
		GLOBAL_VARS_set_threads_state(THREAD_IHM_DISPLAY,FALSE);;
	    return MANAGER_errors(ERROR_THREAD_CREATION,"IHM_DISPLAY_thread");
	}

	// Indication que le thread est lancé
	GLOBAL_VARS_set_threads_state(THREAD_IHM_RECEIVER,TRUE);

	// Thread de gestion de l'horloge générale
	if (pthread_create(&ihm_receiver_thread, &attributes, IHM_receiver_thread, NULL))
	{ 
		// On indique que le thread n'est plus en cours d'exécution
		GLOBAL_VARS_set_threads_state(THREAD_IHM_RECEIVER,FALSE);
	    return MANAGER_errors(ERROR_THREAD_CREATION,"IHM_RECEIVER_thread");
	}

	return END_SUCCESS;
}

// Gestion des retours d'erreurs
static Uint16 MANAGER_errors(int error_id, char* commentary)
{
	switch(error_id)
	{
		case END_SUCCESS:
			printf("END_SUCCESS : %s\n",commentary);
			break;

		case ERROR_STATUS:
			printf("ERROR_STATUS : %s\n",commentary);
			break;

		case ERROR_CONFIG:
			printf("ERROR_CONFIG : %s\n",commentary);
			break;

		case ERROR_MAKEFILE:
			printf("ERROR_MAKEFILE : %s\n",commentary);
			break;

		case ERROR_UNKNOWN_CARD:
			printf("ERROR UNKNOWN_CARD : %s\n",commentary);
			break;

		case ERROR_COMPILATION:
			printf("ERROR_COMPILATION : %s\n",commentary);
			break;

		case ERROR_THREAD_CREATION:
			printf("ERROR_THREAD_CREATION : %s\n",commentary);
			break;

		default:
			printf("error_id inconnu : %d\n",error_id);
			break;
	}

	return error_id;
}

