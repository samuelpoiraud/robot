/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_EVE_manager.c
 *	Package : QS_cartes
 *	Description : Gestions spécifiques au logiciel EVE dans les codes des cartes
 *	Auteur : Julien Franchineau & François Even
 *	Version 20120105
 */

#define QS_EVE_MANAGER_C

#include "QS_EVE_manager.h"

EVE_global_vars_t eve_global;

extern void _T1interrupt() __attribute__((weak));
extern void _T2Interrupt() __attribute__((weak));
extern void _T3Interrupt() __attribute__((weak));
extern void _T4Interrupt() __attribute__((weak));


// Initialisation des fonctions relatives au fonctionnement d'EVE
Uint16 EVE_manager_card_init()
{
	// Initialisation des émulations hardware
	EVE_HARD_init();

	// Initialisation des variables de contrôle de la simulation
	eve_global.simulation_pause = FALSE;
	eve_global.simulation_stop = FALSE;

	// Création des boîtes aux lettres de communication
	// Voir QS_types pour les noms des cartes
	#warning Modifier QS avec CARTE_P => CARTE_STRAT
	switch(I_AM)
	{
		case CARTE_ACT:
			// Création des BALs de la carte ACTIONNEURS
			eve_global.bal_id_card_to_ihm_qs_data = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_IHM_QS_DATA);
			eve_global.bal_id_card_to_ihm_uart = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_IHM_UART);
			eve_global.bal_id_ihm_qs_data_to_card = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_ACTIONNEURS);
			#ifdef USE_CAN
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_CAN);
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN_TO_ACTIONNEURS);
			#else
				eve_global.bal_id_card_to_can = 0;
				eve_global.bal_id_can_to_card = 0;
			#endif /* USE_CAN */
			#ifdef USE_CAN2
				eve_global.bal_id_card_to_can2 = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_CAN);
				eve_global.bal_id_can2_to_card = EVE_create_msg_queue(BAL_CAN_TO_ACTIONNEURS);
			#else
				eve_global.bal_id_card_to_can2 = 0;
				eve_global.bal_id_can2_to_card = 0;
			#endif /* USE_CAN2 */
			break;

		case CARTE_BALISE:
			// Création des BALs de la carte BALISES
			eve_global.bal_id_card_to_ihm_qs_data = EVE_create_msg_queue(BAL_BALISES_TO_IHM_QS_DATA);
			eve_global.bal_id_card_to_ihm_uart = EVE_create_msg_queue(BAL_BALISES_TO_IHM_UART);
			eve_global.bal_id_ihm_qs_data_to_card = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_BALISES);
			#ifdef USE_CAN
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN_TO_BALISES);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_BALISES_TO_CAN);
			#else
				eve_global.bal_id_card_to_can = 0;
				eve_global.bal_id_can_to_card = 0;
			#endif /* USE_CAN */
			#ifdef USE_CAN2
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN2_TO_BALISES);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_BALISES_TO_CAN2);
			#else
				eve_global.bal_id_card_to_can2 = 0;
				eve_global.bal_id_can2_to_card = 0;
			#endif /* USE_CAN2 */		
			break;

		case CARTE_ASSER:
			// Création des BALs de la carte PROPULSION
			eve_global.bal_id_card_to_ihm_qs_data = EVE_create_msg_queue(BAL_PROPULSION_TO_IHM_QS_DATA);
			eve_global.bal_id_card_to_ihm_uart = EVE_create_msg_queue(BAL_PROPULSION_TO_IHM_UART);
			eve_global.bal_id_ihm_qs_data_to_card = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_PROPULSION);
			#ifdef USE_CAN
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN_TO_PROPULSION);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_PROPULSION_TO_CAN);
			#else
				eve_global.bal_id_card_to_can = 0;
				eve_global.bal_id_can_to_card = 0;
			#endif /* USE_CAN */
			#ifdef USE_CAN2
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN2_TO_PROPULSION);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_PROPULSION_TO_CAN2);
			#else
				eve_global.bal_id_card_to_can2 = 0;
				eve_global.bal_id_can2_to_card = 0;
			#endif /* USE_CAN2 */
			break;

		case CARTE_STRAT:
			// Création des BALs de la carte STRATEGIE
			eve_global.bal_id_card_to_ihm_qs_data = EVE_create_msg_queue(BAL_STRATEGIE_TO_IHM_QS_DATA);
			eve_global.bal_id_card_to_ihm_uart = EVE_create_msg_queue(BAL_STRATEGIE_TO_IHM_UART);
			eve_global.bal_id_ihm_qs_data_to_card = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_STRATEGIE);
			#ifdef USE_CAN
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN_TO_STRATEGIE);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_STRATEGIE_TO_CAN);
			#else
				eve_global.bal_id_card_to_can = 0;
				eve_global.bal_id_can_to_card = 0;
			#endif /* USE_CAN */
			#ifdef USE_CAN2
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN2_TO_STRATEGIE);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_STRATEGIE_TO_CAN2);
			#else
				eve_global.bal_id_card_to_can2 = 0;
				eve_global.bal_id_can2_to_card = 0;
			#endif /* USE_CAN2 */
			break;

		case CARTE_SUPER:
			// Création des BALs de la carte SUPERVISION
			eve_global.bal_id_card_to_ihm_qs_data = EVE_create_msg_queue(BAL_SUPERVISION_TO_IHM_QS_DATA);
			eve_global.bal_id_card_to_ihm_uart = EVE_create_msg_queue(BAL_SUPERVISION_TO_IHM_UART);
			eve_global.bal_id_ihm_qs_data_to_card = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_SUPERVISION);
			#ifdef USE_CAN
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN_TO_SUPERVISION);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_SUPERVISION_TO_CAN);
			#else
				eve_global.bal_id_card_to_can = 0;
				eve_global.bal_id_can_to_card = 0;
			#endif /* USE_CAN */
			#ifdef USE_CAN2
				eve_global.bal_id_can_to_card = EVE_create_msg_queue(BAL_CAN2_TO_SUPERVISION);
				eve_global.bal_id_card_to_can = EVE_create_msg_queue(BAL_SUPERVISION_TO_CAN2);
			#else
				eve_global.bal_id_card_to_can2 = 0;
				eve_global.bal_id_can2_to_card = 0;
			#endif /* USE_CAN2 */
			break;

		default:
			// Situation délicate... la carte est inconnue, ou ne s'identifie pas
			return ERROR_INIT_EVE_CARD_MANAGER;
			break;
	}

	if( eve_global.bal_id_card_to_can == -1 ||
		eve_global.bal_id_card_to_ihm_qs_data == -1 ||
		eve_global.bal_id_card_to_ihm_uart == -1 ||
		eve_global.bal_id_can_to_card == -1 ||
		eve_global.bal_id_ihm_qs_data_to_card == -1 ||
		eve_global.bal_id_card_to_can2 == -1 ||
		eve_global.bal_id_can2_to_card == -1 )
	{
		// Erreur de création de des BAL !
		return ERROR_INIT_EVE_CARD_MANAGER;
	}

	// Blocage des sorties UART tant que l'IHM n'a pas demandé un envoi de flux sur l'UART
	eve_global.ihm_use_uart1 = FALSE;
	eve_global.ihm_use_uart2 = FALSE;

	// Création et liaison à la mémoire partagée de l'horloge générale
	if(EVE_create_and_bound_global_clock_memory(&eve_global.clock_memory_reference) != END_SUCCESS)
	{
		return ERROR_INIT_EVE_CARD_MANAGER;
	}

	// Les Timers sont initialisés dans TIMER_init() qui est appelé par le code lui-même

	// On fait un premier update pour récupérer les informations déjà envoyées par l'IHM
	EVE_manager_card();

	return END_SUCCESS;
}

// Fonction générale de la boucle while du main
void EVE_manager_card()
{
	Uint16 i = 0;
	Uint16 nb_msg = 0;
	pid_t processus_pid;
	bool_e execution_authorized = FALSE;
	// Récupération des messages en provenance d'autres processus/threads
	// On peut bloquer l'exécution du code en restant dans le while lorsque l'on reçoit une demande de pause de l'IHM
	// On en sort alors lorsque l'on reçoit une demande de relance de l'IHM
	while(execution_authorized == FALSE)
	{
		/* Incrémentation de l'horloge des timers */
		EVE_timers_update();

		/* Réception des messages QS_DATA en provenance de l'IHM */
		nb_msg = EVE_check_messages_numbers(eve_global.bal_id_ihm_qs_data_to_card);
		// On a reçu des messages de type QS_DATA
		if(nb_msg > 0)
		{
			EVE_QS_data_msg_t data_msg;
			for(i=0;i<nb_msg;i++)
			{
				
				if(EVE_read_new_msg_QS(eve_global.bal_id_ihm_qs_data_to_card,&data_msg)==END_SUCCESS)
				{
					// Message reçu correctement, donc on le traite
					switch(data_msg.data_id)
					{
						case QS_SIMULATION_RUN:
							// Normalement, si on passe ici, c'est que la simulation est déjà en cours d'exécution
							eve_global.simulation_pause = FALSE;
							break;

						case QS_SIMULATION_PAUSE:
							eve_global.simulation_pause = TRUE;
							break;

						case QS_SIMULATION_STOP:
							eve_global.simulation_stop = TRUE;	// A voir si il est utile... 
							EVE_destroy_all();					// Destructions propres
							processus_pid = getpid();			// Récupération de l'identifiant du processus
							kill(processus_pid,SIGKILL);		// Extinction du processus de cette carte
							break;

						case QS_START_UART1:
							eve_global.ihm_use_uart1 = TRUE;;
							break;

						case QS_STOP_UART1:
							eve_global.ihm_use_uart1 = FALSE;
							break;

						case QS_START_UART2:
							eve_global.ihm_use_uart2 = TRUE;
							break;

						case QS_STOP_UART2:
							eve_global.ihm_use_uart2 = FALSE;
							break;

						case QS_BUTTON_1:
							// On met à jour les données du bouton
							PORTDbits.RD4 = data_msg.data1;
							break;

						case QS_BUTTON_2:
							// On met à jour les données du bouton
							PORTDbits.RD5 = data_msg.data1;
							break;

						case QS_BUTTON_3:
							// On met à jour les données du bouton
							PORTDbits.RD6 = data_msg.data1;
							break;

						case QS_BUTTON_4:
							// On met à jour les données du bouton
							PORTDbits.RD7 = data_msg.data1;
							break;

						case QS_BUTTON_RESET:
							// On doit arrêter le processus
							if(data_msg.data1 == 1)
							{
								EVE_destroy_all();				// Destructions propres
								processus_pid = getpid();		// Récupération de l'identifiant du processus
								kill(processus_pid,SIGKILL);	// Extinction du processus de cette carte
							}
							break;

						default:
							// Message non pris en compte
							break;
					}
				} 
			} 	// Fin du for
		}	// Fin du if

		if(eve_global.simulation_pause == FALSE)
		{
			// On autorise à sortir de la boucle
			// Sinon on continue de boucler en recevant des messages jusqu'à un nouvel ordre de simulation de l'IHM
			execution_authorized = TRUE;
		}
	} 	// Fin du while

	// On vérifie les modifications hardware et on envoie des messages en conséquence
	EVE_HARD_check_ports_modifications();
}

// Fonction de traitement des timers à chaque boucle
static void EVE_timers_update()
{
	Uint8 i;
	EVE_time_t global_clock_time;

	global_clock_time = EVE_get_global_clock(eve_global.clock_memory_reference);

	for(i=0;i<TIMER_NB;i++)
	{
		// Vérification que le timer est bien lancé
		if(eve_global.timers[i].timer_run == TRUE)	
		{
			// On vérifie si on est arrivé au terme du timer
			if((global_clock_time - eve_global.timers[i].init_value) > eve_global.timers[i].user_period)
			{
				// On n'active pas les IFS0 correspondants, ça ne sert à rien
				// On lance directement l'interruption correspondante :D
				switch(i)
				{
					//Les interruptions sont déclarée en weak symbol, le symbol peut être nul si il n'y a pas de fonction associé. comme ça on sait si la carte compilé implémente le timer
					case ID_TIMER_1:
						#ifdef USE_T1Interrupt
							if(!_T1Interrupt)
								break;
							_T1Interrupt();
							eve_global.timers[ID_TIMER_1].init_value = global_clock_time;
						#endif /* USE_T1Interrupt */
						break;

					case ID_TIMER_2:
						#ifdef USE_T2Interrupt
							if(!_T2Interrupt)
								break;
							_T2Interrupt();
							eve_global.timers[ID_TIMER_2].init_value = global_clock_time;
						#endif /* USE_T2Interrupt */
						break;

					case ID_TIMER_3:
						#ifdef USE_T3Interrupt
							if(!_T3Interrupt)
								break;
							_T3Interrupt();
							eve_global.timers[ID_TIMER_3].init_value = global_clock_time;
						#endif/* USE_T3Interrupt */
						break;

					case ID_TIMER_4:
						#ifdef USE_T4Interrupt
							if(!_T4Interrupt)
								break;
							_T4Interrupt();
							eve_global.timers[ID_TIMER_4].init_value = global_clock_time;
						#endif /* USE_T4Interrupt */
						break;

					default:
						// Timer inconnu
						break;
				}
			}
		}
	}
}

// Détruit toutes les BALs proprement ainsi que la liaison à la mémoire partagée de l'horloge
static void EVE_destroy_all()
{
	// Destruction de la liaison à l'horloge générale
	EVE_unbound_global_clock_memory(eve_global.clock_memory_reference);
}

