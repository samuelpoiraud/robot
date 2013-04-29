/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : can.c
 *	Package : EVE
 *	Description : Thread de gestion du bus CAN
 *	Auteur : Julien Franchineau & François Even
 *	Version 20120114
 */

#define CAN_C

#include "can.h"

//TODO
// Création de toutes les BAL, définir les variables utiles queue_id
// Réception des messages CAN de chaque carte
// Renvoi des messages CAN vers toutes les cartes actives
// Affichage des messages sur l'IHM

// Variables de référence des BAL can vers cartes
static Uint32 bal_id_can_to_actionneurs;
static Uint32 bal_id_can_to_balises;
static Uint32 bal_id_can_to_propulsion;
static Uint32 bal_id_can_to_strategie;
static Uint32 bal_id_can_to_supervision;

// Variables de référence des BAL cartes vers can
static Uint32 bal_id_actionneurs_to_can;
static Uint32 bal_id_balises_to_can;
static Uint32 bal_id_propulsion_to_can;
static Uint32 bal_id_strategie_to_can;
static Uint32 bal_id_supervision_to_can;

// Filtres chargés du fichier de config
static Uint16* can_filter_sid;

// Thread de gestion du CAN
void* CAN_thread()
{
	Uint16 current_status;
	Uint16 nb_can_msg;
	EVE_CAN_msg_t can_msg;
	Uint16 i;
	bool_e wait_cards = TRUE;

	// On indique que le thread est démarré
	GLOBAL_VARS_set_threads_state(THREAD_CAN,TRUE);

	// Initialisation des références et création des BAL
	if(CAN_init_bal()!=END_SUCCESS)
	{
		// BAL mal initialisées
		EVE_printf("Thread can : Fail create BAL\n");
		return END_THREAD;
	}

	// Chargement des filtres par défaut de l'IHM
	CAN_load_filters();

	current_status = GLOBAL_VARS_get_sim_status();

	while(current_status != SIMULATION_QUIT)
	{
		if(current_status == SIMULATION_RUN)
		{
			if(GLOBAL_VARS_get_use_cards(ACTIONNEURS) == 1)
			{
				nb_can_msg = EVE_check_messages_numbers(bal_id_actionneurs_to_can);
				if(nb_can_msg > 0)
				{
					// On reçoit les messages
					for(i=0;i<nb_can_msg;i++)
					{
						EVE_read_new_msg_CAN(bal_id_actionneurs_to_can,&can_msg);	// Réception
						CAN_broadcast_msg(can_msg, ACTIONNEURS);						// Broadcast à toutes les cartes
						CAN_ihm_send_with_filter(can_msg);							// Envoi à l'IHM
					}
				}
			}
			if(GLOBAL_VARS_get_use_cards(BALISES) == 1)
			{
				nb_can_msg = EVE_check_messages_numbers(bal_id_balises_to_can);
				if(nb_can_msg > 0)
				{
					// On reçoit les messages
					for(i=0;i<nb_can_msg;i++)
					{
						EVE_read_new_msg_CAN(bal_id_balises_to_can,&can_msg);		// Réception
						CAN_broadcast_msg(can_msg, BALISES);							// Broadcast à toutes les cartes
						CAN_ihm_send_with_filter(can_msg);							// Envoi à l'IHM
					}
				}
			}
			if(GLOBAL_VARS_get_use_cards(PROPULSION) == 1)
			{
				nb_can_msg = EVE_check_messages_numbers(bal_id_propulsion_to_can);
				if(nb_can_msg > 0)
				{
					// On reçoit les messages
					for(i=0;i<nb_can_msg;i++)
					{
						EVE_read_new_msg_CAN(bal_id_propulsion_to_can,&can_msg);	// Réception
						CAN_broadcast_msg(can_msg, PROPULSION);						// Broadcast à toutes les cartes
						CAN_ihm_send_with_filter(can_msg);							// Envoi à l'IHM
					}
				}
			}
			if(GLOBAL_VARS_get_use_cards(STRATEGIE) == 1)
			{
				nb_can_msg = EVE_check_messages_numbers(bal_id_strategie_to_can);
				if(nb_can_msg > 0)
				{
					// On reçoit les messages
					for(i=0;i<nb_can_msg;i++)
					{
						EVE_read_new_msg_CAN(bal_id_strategie_to_can,&can_msg);		// Réception
						CAN_broadcast_msg(can_msg, STRATEGIE);						// Broadcast à toutes les cartes
						CAN_ihm_send_with_filter(can_msg);							// Envoi à l'IHM
					}
				}
			}
			if(GLOBAL_VARS_get_use_cards(SUPERVISION) == 1)
			{
				nb_can_msg = EVE_check_messages_numbers(bal_id_supervision_to_can);
				if(nb_can_msg > 0)
				{
					// On reçoit les messages
					for(i=0;i<nb_can_msg;i++)
					{
						EVE_read_new_msg_CAN(bal_id_supervision_to_can,&can_msg);	// Réception
						CAN_broadcast_msg(can_msg, SUPERVISION);						// Broadcast à toutes les cartes
						CAN_ihm_send_with_filter(can_msg);							// Envoi à l'IHM
					}
				}
			}
		}

		// Libère du temps CPU
		usleep(1000);	

		// Mise à jour de l'information du status
		current_status = GLOBAL_VARS_get_sim_status();
	}

	// Destruction des BAL du CAN
	// On attend d'être sûr que les cartes sont bien détruites
	while(wait_cards == TRUE)
	{
		wait_cards = FALSE;
		if(GLOBAL_VARS_get_threads_state(THREAD_ACTIONNEURS) == TRUE)
		{
			wait_cards = TRUE;
		}
		if(GLOBAL_VARS_get_threads_state(THREAD_BALISES) == TRUE)
		{
			wait_cards = TRUE;
		}
		if(GLOBAL_VARS_get_threads_state(THREAD_PROPULSION) == TRUE)
		{
			wait_cards = TRUE;
		}
		if(GLOBAL_VARS_get_threads_state(THREAD_STRATEGIE) == TRUE)
		{
			wait_cards = TRUE;
		}
		if(GLOBAL_VARS_get_threads_state(THREAD_SUPERVISION) == TRUE)
		{
			wait_cards = TRUE;
		}
		
		usleep(50000); // Libère du temps CPU
	}
	CAN_destroy_all(); // Destruction des BAL

	// Fermer le CAN !
	GLOBAL_VARS_set_threads_state(THREAD_CAN,FALSE);

	return END_THREAD;
}

// Fonction d'initialisation et de création des BAL pour le CAN
static Uint16 CAN_init_bal()
{
	// On met tout à zéro avant
	CAN_set_zero_can_vars();

	pthread_mutex_lock(&global.use_cards_mutex);

	// On check chaque carte si elle est utilisée
	// Si oui, on crée les 2 BAL correspondantes
	if(global.use_cards[ACTIONNEURS] == TRUE)
	{
		bal_id_can_to_actionneurs = EVE_create_msg_queue(BAL_CAN_TO_ACTIONNEURS);
		if(bal_id_can_to_actionneurs == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
		bal_id_actionneurs_to_can = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_CAN);
		if(bal_id_actionneurs_to_can == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
	}

	if(global.use_cards[BALISES] == TRUE)
	{
		bal_id_can_to_balises = EVE_create_msg_queue(BAL_CAN_TO_BALISES);
		if(bal_id_can_to_balises == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
		bal_id_balises_to_can = EVE_create_msg_queue(BAL_BALISES_TO_CAN);
		if(bal_id_balises_to_can == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
	}

	if(global.use_cards[PROPULSION] == TRUE)
	{
		bal_id_can_to_propulsion = EVE_create_msg_queue(BAL_CAN_TO_PROPULSION);
		if(bal_id_can_to_propulsion == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
		bal_id_propulsion_to_can = EVE_create_msg_queue(BAL_PROPULSION_TO_CAN);
		if(bal_id_propulsion_to_can == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
	}

	if(global.use_cards[STRATEGIE] == TRUE)
	{
		bal_id_can_to_strategie = EVE_create_msg_queue(BAL_CAN_TO_STRATEGIE);
		if(bal_id_can_to_strategie == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
		bal_id_strategie_to_can = EVE_create_msg_queue(BAL_STRATEGIE_TO_CAN);
		if(bal_id_strategie_to_can == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
	}

	if(global.use_cards[SUPERVISION] == TRUE)
	{
		bal_id_can_to_supervision = EVE_create_msg_queue(BAL_CAN_TO_SUPERVISION);
		if(bal_id_can_to_supervision == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
		bal_id_supervision_to_can = EVE_create_msg_queue(BAL_SUPERVISION_TO_CAN);
		if(bal_id_supervision_to_can == -1)
		{
			pthread_mutex_unlock(&global.use_cards_mutex);
			return ERROR_INIT_CAN_BAL;
		}
	}

	pthread_mutex_unlock(&global.use_cards_mutex);

	// On a fini !
	return CAN_errors(END_SUCCESS,"Init CAN\n");
}

// Fonction de mise à zéro de toutes les variables générales du CAN
static void CAN_set_zero_can_vars()
{
	bal_id_can_to_actionneurs = 0;
	bal_id_can_to_balises = 0;
	bal_id_can_to_propulsion = 0;
	bal_id_can_to_strategie = 0;
	bal_id_can_to_supervision = 0;

	bal_id_actionneurs_to_can = 0;
	bal_id_balises_to_can = 0;
	bal_id_propulsion_to_can = 0;
	bal_id_strategie_to_can = 0;
	bal_id_supervision_to_can = 0;

	can_filter_sid = NULL;
}

// Fonction de chargement des filtres contenus dans le fichier filtres.config
static void CAN_load_filters()
{
	char buffer[64];
	int int_buffer = 0;
	Uint16 can_filter_count = 0;
	bool_e end_of_file = FALSE;
	FILE* config_file;

	// Ouverture du fichier de configuration
	config_file = fopen("_Configuration/filtres.config","r");
	if(config_file!=NULL)
	{
		// lecture du fichier de configuration
		while(end_of_file==FALSE)
		{
			fscanf(config_file,"%s",buffer);
			// on vérifie qu'on a bien une ligne de commande
			// les lignes de commandes débutent par le caractère '$'
			if(buffer[0]=='$')
			{
				if(strcmp(buffer,"$END_OF_FILE")==0)
				{
					// Fin du fichier atteinte
					end_of_file = TRUE;
				}
				else if(strcmp(buffer,"$NB_FILTER")==0)
				{
					fscanf(config_file,"%d",&int_buffer);
					// On fait péter un malloc pour avoir un tableau de la bonne taille
					can_filter_sid = malloc(int_buffer);
				}
				else if(strcmp(buffer,"$FILTER")==0)
				{
					// On ajoute le sid qu'il faudra filtrer
					fscanf(config_file,"%d",&int_buffer);
					can_filter_sid[can_filter_count] = int_buffer;
					can_filter_count++;
				}
			}	// Fin du if (lecture fichier)
		}	// Fin du while (lecture fichier)
	}	// Fin du if(config_file != NULL)
}

// Fonction qui envoie un message CAN à toutes les cartes (actives)
static void CAN_broadcast_msg(EVE_CAN_msg_t can_msg, cards_id_e originating_card)
{
	if(originating_card != ACTIONNEURS && GLOBAL_VARS_get_use_cards(ACTIONNEURS) == 1)
	{
		EVE_write_new_msg_CAN(bal_id_can_to_actionneurs,can_msg);
	}	
	if(originating_card != BALISES && GLOBAL_VARS_get_use_cards(BALISES) == 1)
	{
		EVE_write_new_msg_CAN(bal_id_can_to_balises,can_msg);
	}
	if(originating_card != PROPULSION && GLOBAL_VARS_get_use_cards(PROPULSION) == 1)
	{
		EVE_write_new_msg_CAN(bal_id_can_to_propulsion,can_msg);
	}
	if(originating_card != STRATEGIE && GLOBAL_VARS_get_use_cards(STRATEGIE) == 1)
	{
		EVE_write_new_msg_CAN(bal_id_can_to_strategie,can_msg);
	}
	if(originating_card != SUPERVISION && GLOBAL_VARS_get_use_cards(SUPERVISION) == 1)
	{
		EVE_write_new_msg_CAN(bal_id_can_to_supervision,can_msg);
	}
}

// Fonction qui met à jour l'IHM après un filtrage selon les filtres imposés par le fichier de configuration
static Uint16 CAN_ihm_send_with_filter(EVE_CAN_msg_t can_msg)
{
	Uint16 i;
	bool_e send_msg = TRUE;

	for(i=0;i<sizeof(can_filter_sid);i++)
	{
		if(can_filter_sid[i] == can_msg.sid)
		{
			send_msg = FALSE;
		}
	}

	if(send_msg == TRUE)
	{
		// Message non filtré, on expédie sur l'IHM
		gdk_threads_enter();	// Blocage des threads gtk
		IHM_receiver_update_can(can_msg);
		gdk_threads_leave();	// Déblocage des threads gtk
	}

	return END_SUCCESS;
}

// Fonction qui détruit les BALs du CAN
static Uint16 CAN_destroy_all()
{
	if(GLOBAL_VARS_get_use_cards(ACTIONNEURS) == 1)
	{
		EVE_destroy_msg_queue(bal_id_can_to_actionneurs);
		EVE_destroy_msg_queue(bal_id_actionneurs_to_can);
	}	
	if(GLOBAL_VARS_get_use_cards(BALISES) == 1)
	{
		EVE_destroy_msg_queue(bal_id_can_to_balises);
		EVE_destroy_msg_queue(bal_id_balises_to_can);
	}
	if(GLOBAL_VARS_get_use_cards(PROPULSION) == 1)
	{
		EVE_destroy_msg_queue(bal_id_can_to_propulsion);
		EVE_destroy_msg_queue(bal_id_propulsion_to_can);
	}
	if(GLOBAL_VARS_get_use_cards(STRATEGIE) == 1)
	{
		EVE_destroy_msg_queue(bal_id_can_to_strategie);
		EVE_destroy_msg_queue(bal_id_strategie_to_can);
	}
	if(GLOBAL_VARS_get_use_cards(SUPERVISION) == 1)
	{
		EVE_destroy_msg_queue(bal_id_can_to_supervision);
		EVE_destroy_msg_queue(bal_id_supervision_to_can);
	}

	return END_SUCCESS;
}

// Gestion des retours d'erreurs
static Uint16 CAN_errors(Uint16 error_id, char* commentary)
{
	switch(error_id)
	{
		case END_SUCCESS:
			printf("END_SUCCESS : %s\n",commentary);
			break;

		default:
			printf("error_id inconnu : %d, %s\n",error_id,commentary);
			break;
	}

	return error_id;
}

