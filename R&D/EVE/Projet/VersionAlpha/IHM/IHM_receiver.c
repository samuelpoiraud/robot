/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : IHM_receiver.c
 *	Package : EVE
 *	Description : Gestion de la réception et de l'affichage des uart et du CAN 
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */
#define IHM_RECEIVER_C

#include "IHM_receiver.h"

//id des files de communication pour les UART
static Uint32 queue_uart_id_actionneurs;
static Uint32 queue_uart_id_balises;
static Uint32 queue_uart_id_strategie;
static Uint32 queue_uart_id_supervision;
static Uint32 queue_uart_id_propulsion;

//id des files de communication pour les DATA
static Uint32 queue_data_id_actionneurs;
static Uint32 queue_data_id_balises;
static Uint32 queue_data_id_strategie;
static Uint32 queue_data_id_supervision;
static Uint32 queue_data_id_propulsion;

//fonction du thread de reception de l'IHM
void* IHM_receiver_thread()
{
	Uint16 sim_status;

	IHM_receiver_init();
 
	g_thread_init(NULL);
	gdk_threads_init();

	sim_status = GLOBAL_VARS_get_sim_status();

	while(sim_status != SIMULATION_QUIT)
	{
		if(sim_status == SIMULATION_RUN)
		{
			gdk_threads_enter();
			
			IHM_receiver_update_time();
			IHM_receiver_update_picture();
			IHM_receiver_update_uart();
			IHM_receiver_update_data();

			gdk_threads_leave();
		}

		// Temps permettant de libérer un peu de temps CPU
		// A faire ici juste avant de reboucler pour que sim_status soit le plus à jour possible
		usleep(100000);

		sim_status = GLOBAL_VARS_get_sim_status();
	}

	IHM_receiver_destroy();
	GLOBAL_VARS_set_threads_state(THREAD_IHM_RECEIVER,FALSE);

	return END_SUCCESS;
}
// Fonction qui affiche le message CAN reçue sur l'IHM
void IHM_receiver_update_can(EVE_CAN_msg_t can_message)
{
	// Déclaration des variables locales
	GtkTextIter iter;
	GtkTextBuffer *buffer;
	const gchar text_buffer[40] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	// Récupération des objets GTK
	pthread_mutex_lock(&global.button_save_mutex);
	GtkWidget *can_zone = lookup_widget (GTK_WIDGET(global.button_save), "can_zt_main");
	
	// Préparation du texte
		snprintf((char*)text_buffer,40,"%2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X\n",
		can_message.start_msg,
		can_message.sid >> 8,
		can_message.sid & 0x00FF,
		can_message.data[0],
		can_message.data[1],
		can_message.data[2],
		can_message.data[3],
		can_message.data[4],
		can_message.data[5],
		can_message.data[6],
		can_message.data[7],
		can_message.size,
		can_message.end_msg);

	// Ecriture dans le buffer de CAN
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (can_zone));;
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	gtk_text_buffer_insert_at_cursor(buffer, text_buffer, -1);
	pthread_mutex_unlock(&global.button_save_mutex);
}
 
// Fonction de mise à jour du temps
void IHM_receiver_update_time(void)
{
	// Déclaration des variables locales
	const gchar time[6] = {0,0,0,0,0,0};
	GtkTextBuffer *buffer;

	pthread_mutex_lock(&global.button_save_mutex);

	// Récupération des objets GTK
	GtkWidget *time_zone = lookup_widget (GTK_WIDGET(global.button_save), "os_zt_CurrentTime");

	// Ecriture dans la zone de temps de l'IHM
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (time_zone));
	pthread_mutex_lock(&global.global_time_mutex);
	snprintf((char*)time, 6, "%lu", global.global_time);
	pthread_mutex_unlock(&global.global_time_mutex);
	gtk_text_buffer_set_text (buffer,time,-1);

	pthread_mutex_unlock(&global.button_save_mutex);
}

void IHM_receiver_update_uart(void)
{
	// Déclaration des variables locales
	Uint16 msg_num = 0;
	int i,j = 0;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	EVE_UART_msg_t current_message;
	char* uart_widget_name = "";
	char* uart1_widget_name = "";
	char* uart2_widget_name = "";
	int key,card = 0;
	int text_area;

	uart1_widget_name = "uart_zt_uart1";
	uart2_widget_name = "uart_zt_uart2";

	//Traitement et affichage pour les deux zones de texte
	for(text_area = 0;text_area<2;text_area++)
	{
		// Récupére l'UART choisit par l'utilisateur
		pthread_mutex_lock(&global.use_uart_mutex);
		j = global.use_uart[text_area];
		pthread_mutex_unlock(&global.use_uart_mutex);

		// Récupération des queue ID des bals
		pthread_mutex_lock(&global.use_cards_mutex);
		switch(j)
		{
			case 1:
			case 2:
				key = queue_uart_id_actionneurs;
				card= 0;
				break;
			case 3:
			case 4:
				key = queue_uart_id_balises;
				card = 1;
				break;
			case 5:
			case 6:
				key = queue_uart_id_propulsion;
				card = 2;
				break;
			case 7:
			case 8:
				key = queue_uart_id_strategie;
				card = 3;
				break;
			case 9:
			case 10:
				key = queue_uart_id_supervision;
				card = 4;
				break;
			default:
				//TODO erreur
				break;
		}
		// Choix du nom de l'objet GTK
		if(text_area == 0)
		{
			uart_widget_name = uart1_widget_name;
		}
		else
		{
			uart_widget_name = uart2_widget_name;
		}
		// Vérifie si les cartes à visualisé sont utilisées pour la simulation
		if(global.use_cards[card] == 1) 
		{
			// Récupération du nombre de messages à lire
			msg_num = EVE_check_messages_numbers(key);

			for(i=0;i<msg_num;i++)
			{
				// Lecture du message
				EVE_read_new_msg_UART(key,&current_message);

				// Affichage du message en fonction de l'UART
				if(((j%2) == 1 && current_message.uart_number == 1) || ((j%2) == 0 && current_message.uart_number == 2))
				{
	
					pthread_mutex_lock(&global.button_save_mutex);
					GtkWidget *uart_zone = lookup_widget (GTK_WIDGET(global.button_save),(const gchar*)uart_widget_name);
					pthread_mutex_unlock(&global.button_save_mutex);

					buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (uart_zone));
					gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
					gtk_text_buffer_insert_at_cursor(buffer,(const gchar*)current_message.data, -1);

				}
			}
		}
		pthread_mutex_unlock(&global.use_cards_mutex);
	}
}
void IHM_receiver_update_data(void)
{
	// Déclaration des variables locales
	Uint16 msg_num = 0;
	Uint8 card;
	Uint32 queue_id;
	EVE_QS_data_msg_t current_message;
	int i,j = 0;
	char* led_run= malloc(8 * sizeof(char));
	char* led_can= malloc(8 * sizeof(char));
	char* led_uart= malloc(8 * sizeof(char));
	char* led_user= malloc(8 * sizeof(char));
	char* led_user2= malloc(8 * sizeof(char));
	char* led_error= malloc(8 * sizeof(char)); 
	GtkWidget *led;

	// Récupération de l'onglet utilisé par l'utilisateur
	pthread_mutex_lock(&global.notebook_page_mutex);
	card = global.notebook_page;
	pthread_mutex_unlock(&global.notebook_page_mutex);

	// Récupération des LED à allumer en fonction de l'onglet
	switch(card)
	{
		case 0:
			queue_id = queue_data_id_actionneurs;
			strcpy(led_error,"image165");
			strcpy(led_user,"image166");
			strcpy(led_user2,"image167");
			strcpy(led_uart,"image168");
			strcpy(led_can,"image169");
			strcpy(led_run,"image170");
			break;
		case 1:
			queue_id = queue_data_id_propulsion;
			strcpy(led_error,"image186");
			strcpy(led_user,"image187");
			strcpy(led_user2,"image188");
			strcpy(led_uart,"image189");
			strcpy(led_can,"image190");
			strcpy(led_run,"image191");
			break;
		case 2:
			queue_id = queue_data_id_strategie;
			strcpy(led_error,"image199");
			strcpy(led_user,"image200");
			strcpy(led_user2,"image201");
			strcpy(led_uart,"image202");
			strcpy(led_can,"image203");
			strcpy(led_run,"image204");
			break;
		case 3:
			queue_id = queue_data_id_supervision;
			strcpy(led_error,"image212");
			strcpy(led_user,"image213");
			strcpy(led_user2,"image214");
			strcpy(led_uart,"image215");
			strcpy(led_can,"image216");
			strcpy(led_run,"image217");
			break;
		case 4:
			queue_id = queue_data_id_balises;
			strcpy(led_error,"image225");
			strcpy(led_user,"image226");
			strcpy(led_user2,"image227");
			strcpy(led_uart,"image228");
			strcpy(led_can,"image229");
			strcpy(led_run,"image230");
			break;
		default:
			//TODO gérer l'erreur
			break;
	}
		
	// On vérifie que la file existe
	if(queue_id != 0)
	{
		// Récupération des messages à lire
		msg_num = EVE_check_messages_numbers(queue_id);
		for(i=0;i<msg_num;i++)
		{
			// Récupération du message
			EVE_read_new_msg_QS(queue_id,&current_message);//TODO success

			// Regarder pour tout les encadrés DATA de l'IHM (non utile pour les LED)
			for(j=0;j<9;j++)
			{
				// On regarde le type de DATA
				switch(current_message.data_id)
				{
					case QS_PWM:
						//data_config[j+(9*card)].type retrouver l'id de la page courante
						//if(data_config[j+(9*card)].param == current_message.data1)
						//{
						//}
						break;
					case QS_LED_RUN:
						pthread_mutex_lock(&global.button_save_mutex);
						led = lookup_widget (GTK_WIDGET(global.button_save), (const gchar*)led_run);
						pthread_mutex_unlock(&global.button_save_mutex);

						if(current_message.data1 == 1)
						{
							gtk_widget_set_sensitive (led, TRUE);
						}
						else
						{
							gtk_widget_set_sensitive (led, FALSE);
						}
						break;

					case QS_LED_CAN :
						pthread_mutex_lock(&global.button_save_mutex);
						led = lookup_widget (GTK_WIDGET(global.button_save),(const gchar*)led_can);
						pthread_mutex_unlock(&global.button_save_mutex);

						if(current_message.data1 == 1)
						{
							gtk_widget_set_sensitive (led, TRUE);
						}
						else
						{
							gtk_widget_set_sensitive (led, FALSE);
						}
						break;

					case QS_LED_UART :
						pthread_mutex_lock(&global.button_save_mutex);
						led = lookup_widget (GTK_WIDGET(global.button_save), (const gchar*)led_uart);
						pthread_mutex_unlock(&global.button_save_mutex);

						if(current_message.data1 == 1)
						{
							gtk_widget_set_sensitive (led, TRUE);
						}
						else
						{
							gtk_widget_set_sensitive (led, FALSE);
						}
						break;

					case QS_LED_USER :
						pthread_mutex_lock(&global.button_save_mutex);
						led = lookup_widget (GTK_WIDGET(global.button_save), (const gchar*)led_user);
						pthread_mutex_unlock(&global.button_save_mutex);

						if(current_message.data1 == 1)
						{
							gtk_widget_set_sensitive (led, TRUE);
						}
						else
						{
							gtk_widget_set_sensitive (led, FALSE);
						}
						break;

					case QS_LED_USER2 :
						pthread_mutex_lock(&global.button_save_mutex);
						led = lookup_widget (GTK_WIDGET(global.button_save),(const gchar*) led_user2);
						pthread_mutex_unlock(&global.button_save_mutex);

						if(current_message.data1 == 1)
						{
							gtk_widget_set_sensitive (led, TRUE);
						}
						else
						{
							gtk_widget_set_sensitive (led, FALSE);
						}
						break;

					case QS_LED_ERROR :
						pthread_mutex_lock(&global.button_save_mutex);
						led = lookup_widget (GTK_WIDGET(global.button_save), (const gchar*)led_error);
						pthread_mutex_unlock(&global.button_save_mutex);

						if(current_message.data1 == 1)
						{
							gtk_widget_set_sensitive (led, TRUE);
						}
						else
						{
							gtk_widget_set_sensitive (led, FALSE);
						}
						break;

					default:
						break;
				}	
			}
		} // Fin du for
	} // Fin du if
}

// Cette fonction permet de changer l'état de la LED de fonctionnement
void IHM_receiver_update_picture(void)
{
	// Déclaration des variables locales
	GtkWidget* led1,*led2,*led3,*led4,*led5;

	// Récupération des élements de l'interface
	pthread_mutex_lock(&global.button_save_mutex);

	led1 = lookup_widget (GTK_WIDGET(global.button_save),"image167");
	led2 = lookup_widget (GTK_WIDGET(global.button_save),"image231");
	led3 = lookup_widget (GTK_WIDGET(global.button_save),"image192");
	led4 = lookup_widget (GTK_WIDGET(global.button_save),"image205");
	led5 = lookup_widget (GTK_WIDGET(global.button_save),"image218");
	
	pthread_mutex_unlock(&global.button_save_mutex);


	// Active/Désactive les LED des cartes utilisées lors de la simulation
	pthread_mutex_lock(&global.use_cards_mutex);
	pthread_mutex_lock(&global.sim_status_mutex);

	if(global.sim_status == SIMULATION_RUN)
	{
		if(global.use_cards[ACTIONNEURS] == 1)
		{
			gtk_widget_set_sensitive (led1, TRUE);
		}
		if(global.use_cards[BALISES] == 1 )
		{
			gtk_widget_set_sensitive (led2, TRUE);
		}
		if(global.use_cards[PROPULSION] ==1)
		{
			gtk_widget_set_sensitive (led3, TRUE);
		}	
		if(global.use_cards[STRATEGIE] == 1)
		{
			gtk_widget_set_sensitive (led4, TRUE);
		}
		if(global.use_cards[SUPERVISION] == 1)
		{
			gtk_widget_set_sensitive (led5, TRUE);
		}
	}
	else
	{
		if(global.use_cards[ACTIONNEURS] == 1)
		{
			gtk_widget_set_sensitive (led1, FALSE);
		}
		if(global.use_cards[BALISES] == 1 )
		{
			gtk_widget_set_sensitive (led2, FALSE);
		}
		if(global.use_cards[PROPULSION] ==1)
		{
			gtk_widget_set_sensitive (led3, FALSE);
		}	
		if(global.use_cards[STRATEGIE] == 1)
		{
			gtk_widget_set_sensitive (led4, FALSE);
		}
		if(global.use_cards[SUPERVISION] == 1)
		{
			gtk_widget_set_sensitive (led5, FALSE);
		}
		
	}
	pthread_mutex_unlock(&global.use_cards_mutex);	
	pthread_mutex_unlock(&global.sim_status_mutex);
}
	
void IHM_receiver_init(void) 
{
	//création des boites aux lettres pour l'UART et les DATA
	pthread_mutex_lock(&global.use_cards_mutex);
	if(global.use_cards[ACTIONNEURS] == 1)
	{
		queue_uart_id_actionneurs = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_IHM_UART);
		queue_data_id_actionneurs = EVE_create_msg_queue(BAL_ACTIONNEURS_TO_IHM_QS_DATA);
	}
	else
	{
		queue_uart_id_actionneurs = 0;
		queue_data_id_actionneurs = 0;
	}

	if(global.use_cards[BALISES] == 1)
	{
		queue_uart_id_balises = EVE_create_msg_queue(BAL_BALISES_TO_IHM_UART);
		queue_data_id_balises = EVE_create_msg_queue(BAL_BALISES_TO_IHM_QS_DATA);
	}
	else
	{
		queue_uart_id_balises = 0;
		queue_data_id_balises = 0;
	}

	if(global.use_cards[PROPULSION] ==1)
	{
		queue_uart_id_propulsion = EVE_create_msg_queue(BAL_PROPULSION_TO_IHM_UART);
		queue_data_id_propulsion = EVE_create_msg_queue(BAL_PROPULSION_TO_IHM_QS_DATA);
	}	
	else
	{
		queue_uart_id_propulsion = 0;
		queue_data_id_propulsion = 0;
	}

	if(global.use_cards[STRATEGIE] == 1)
	{
		queue_uart_id_strategie = EVE_create_msg_queue(BAL_STRATEGIE_TO_IHM_UART); 
		queue_data_id_strategie = EVE_create_msg_queue(BAL_STRATEGIE_TO_IHM_QS_DATA); 
	}
	else
	{
		queue_uart_id_strategie = 0;
		queue_data_id_strategie = 0;
	}

	if(global.use_cards[SUPERVISION] == 1)
	{
		queue_uart_id_supervision = EVE_create_msg_queue(BAL_SUPERVISION_TO_IHM_UART);
		queue_data_id_supervision = EVE_create_msg_queue(BAL_SUPERVISION_TO_IHM_QS_DATA);
	}
	else
	{
		queue_uart_id_supervision = 0;
		queue_data_id_supervision = 0;
	}

	pthread_mutex_unlock(&global.use_cards_mutex);
}

// Fonction de destruction des BAL utilisés dans IHM_receiver
void IHM_receiver_destroy(void)
{
	pthread_mutex_lock(&global.use_cards_mutex);

	if(global.use_cards[ACTIONNEURS] == 1)
	{
		EVE_destroy_msg_queue(queue_uart_id_actionneurs);
		EVE_destroy_msg_queue(queue_data_id_actionneurs);
	}
	if(global.use_cards[BALISES] == 1 )
	{
		EVE_destroy_msg_queue(queue_uart_id_balises);
		EVE_destroy_msg_queue(queue_data_id_balises);
	}
	if(global.use_cards[PROPULSION] ==1)
	{
		EVE_destroy_msg_queue(queue_uart_id_propulsion);
		EVE_destroy_msg_queue(queue_data_id_propulsion);
	}	
	if(global.use_cards[STRATEGIE] == 1)
	{
		EVE_destroy_msg_queue(queue_uart_id_strategie);
		EVE_destroy_msg_queue(queue_data_id_strategie);
	}
	if(global.use_cards[SUPERVISION] == 1)
	{
		EVE_destroy_msg_queue(queue_uart_id_supervision);
		EVE_destroy_msg_queue(queue_data_id_supervision);
	}

	pthread_mutex_unlock(&global.use_cards_mutex);
}


