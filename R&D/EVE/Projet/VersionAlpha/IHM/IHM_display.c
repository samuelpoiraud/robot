/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : IHM_display.c
 *	Package : EVE
 *	Description : Gestion de l'interface Gtk
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */
#define IHM_DISPLAY_C

#include "IHM_display.h"

void* IHM_display_thread()
{
	GtkWidget *Fenetre;
	FILE* config_data_file;
	FILE* config_msg_file;

	gtk_set_locale ();
	gtk_init (0,0);
	
	IHM_display_init();

	add_pixmap_directory ("IHM/pixmaps");
	//ouverture du fichier de configuration des data
	config_data_file = fopen("_Configuration/data.config","r");
	if(config_data_file==NULL)
	{
		printf("Fichier non existant\n");
		//return IHM_display_errors(ERROR_CONFIG, "Fichier config des data inexistant");
	}
	//ouverture du fichier de configuration des messages enregistrés
	config_msg_file = fopen("_Configuration/messages.config","r");
	if(config_msg_file==NULL)
	{
		printf("Fichier non existant\n");
		//return IHM_display_errors(ERROR_CONFIG, "Fichier config des messages inexistant");
	}

	//chargement des configurations de l'IHM
	IHM_display_load_configuration_file(config_data_file,config_msg_file);
	//création et lancement de l'interface
	Fenetre = create_Fenetre();
	gtk_widget_show (Fenetre);
	gtk_main();
	IHM_display_destroy();

	GLOBAL_VARS_set_threads_state(THREAD_IHM_DISPLAY,FALSE);

	return END_SUCCESS;
}
/*

// Fonction qui permet à l'utilisateur d'imposer un temps de simulation
void IHM_display_set_current_time(EVE_time_t current_time)
{
}

// Fonction qui permet l'envoi d'un nouveau filtre pour les messages can
void IHM_display_update_filters(can_filter_t can_filters)
{
}

// Fonction qui permet l'envoi des datas ajoutées par l'utilisateur 
void IHM_display_send_data(EVE_QS_data_msg_t data)
{
}

// Fonction qui envoie la demande activation ou de déactivation des UART
void IHM_display_change_uart_display(int uart_id,uart_list_e new_uart_display)
{
}

*/

// Fonction d'initialisation du thread IHM_display
void IHM_display_init(void)
{
	
	int i=0;
	for(i=0;i<DATA_SIZE;i++)
	{
		data_config[i].type = 0;
		data_config[i].param = 0;
	}

	i=0;
	for(i=0;i<NUMBER_MSG;i++)
	{
		int j = 0;
		for(j=0;j<DESCRIPTION_SIZE;j++)
		{
			msg_config[i].description[j] = 0 ;
		}
		msg_config[i].can.mtype = QUEUE_CAN_MTYPE;
		msg_config[i].can.start_msg = 0;
		msg_config[i].can.sid = 0;
		msg_config[i].can.data[CAN_MSG_DATA_SIZE] = 0;
		msg_config[i].can.size = 0;
		msg_config[i].can.end_msg = 0;
	}

	// Création des files qui envoient des messages DATA de l'IHM au carte
	pthread_mutex_lock(&global.use_cards_mutex);
	pthread_mutex_lock(&global.queue_id_mutex);
	if(global.use_cards[ACTIONNEURS] == 1)
	{
		global.queue_data_id_actionneurs = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_ACTIONNEURS);;
	}
	if(global.use_cards[BALISES] == 1 )
	{
		global.queue_data_id_balises = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_BALISES);
	}
	if(global.use_cards[PROPULSION] ==1)
	{
		global.queue_data_id_propulsion = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_PROPULSION);
	}	
	if(global.use_cards[STRATEGIE] == 1)
	{
		global.queue_data_id_strategie = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_STRATEGIE); 
	}
	if(global.use_cards[SUPERVISION] == 1)
	{
		global.queue_data_id_supervision = EVE_create_msg_queue(BAL_IHM_QS_DATA_TO_SUPERVISION);
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_cards_mutex);
}

// Fonction de destruction des BAL utilisés dans IHM_receiver
void IHM_display_destroy(void)
{
	pthread_mutex_lock(&global.use_cards_mutex);

	if(global.use_cards[ACTIONNEURS] == 1)
	{
		EVE_destroy_msg_queue(global.queue_data_id_actionneurs);
	}
	if(global.use_cards[BALISES] == 1 )
	{
		EVE_destroy_msg_queue(global.queue_data_id_balises);
	}
	if(global.use_cards[PROPULSION] ==1)
	{
		EVE_destroy_msg_queue(global.queue_data_id_propulsion);
	}	
	if(global.use_cards[STRATEGIE] == 1)
	{
		EVE_destroy_msg_queue(global.queue_data_id_strategie);;
	}
	if(global.use_cards[SUPERVISION] == 1)
	{
		EVE_destroy_msg_queue(global.queue_data_id_supervision);
	}

	pthread_mutex_unlock(&global.use_cards_mutex);
}
void IHM_display_load_configuration_file(FILE* config_data_file,FILE* config_msg_file)
{
	char buffer[256];
	int end_of_file_data = 0;
	//int end_of_file_msg = 0;
	int type = 0;
	int position = 0;
	int param =0;
	//int counter = 0;
	int carte= 0; 

	// lecture du fichier de configuration des data
	while(end_of_file_data==0)
	{
		fscanf(config_data_file,"%s",buffer);
		// on vérifie qu'on a bien une ligne de commande
		// les lignes de commandes débutent par le caractère '$'
		if(buffer[0]=='$')
		{
			if(strcmp(buffer,"$END_OF_FILE")==0)
			{
				// Fin du fichier atteinte
				end_of_file_data = 1;
			}
			else if(strcmp(buffer,"$CARTE")==0)
			{
				fscanf(config_data_file,"%d",&carte);
			}
			else if(strcmp(buffer,"$USE")==0)
			{
				fscanf(config_data_file,"%d",&position);
				fscanf(config_data_file,"%s",buffer);
				fscanf(config_data_file,"%d",&type);
				fscanf(config_data_file,"%d",&param);

				if(strcmp(buffer,"STEP_MOTOR")==0)                 
				{
					data_config[(carte*9)+position-1].type = 1;				
				}
				else if(strcmp(buffer,"SERVO")==0)
				{
					data_config[(carte*9)+position-1].type = 2;	
				}
				else if(strcmp(buffer,"QEI")==0)
				{
					data_config[(carte*9)+position-1].type = 3;	
				}
				else if(strcmp(buffer,"DCM")==0)
				{
					data_config[(carte*9)+position-1].type = 4;	
				}	
				else if(strcmp(buffer,"AX12")==0)
				{
					data_config[(carte*9)+position-1].type = 5;	
				}	
				else if(strcmp(buffer,"ADC")==0)
				{
					data_config[(carte*9)+position-1].type = 6;	
				}
				else if(strcmp(buffer,"PWM")==0)
				{
					data_config[(carte*9)+position-1].type = 7;	
				}
				else
				{
					//TODO faire apparaitre une erreur
				}
				//TODO faire enum des types
				data_config[(carte*9)+position-1].param = param;			
			}
		}
	
	}
	
	// lecture du fichier de configuration des messages CAN préenregistrés
	/*
	while(end_of_file_msg==0)
	{
		fscanf(config_msg_file,"%s",buffer);
		// on vérifie qu'on a bien une ligne de commande
		// les lignes de commandes débutent par le caractère '$'
		if(buffer[0]=='$')
		{
			counter++;
			if(strcmp(buffer,"$END_OF_FILE")==0)
			{
				// Fin du fichier atteinte
				end_of_file_msg = 1;
			}
			else if(strcmp(buffer,"$USE")==0)
			{
				if(counter<sizeof(msg_config[0].description))
				{
					int i,j = 0;

					fscanf(config_msg_file,"%s",buffer);
					printf("desc %s\n",buffer);
					msg_config[counter].description[0] = buffer[0]; //TODO changer type de description
					
					fscanf(config_msg_file,"%d",buf[0]);
					printf("%d %s\n",buf[0],buf[0]);
					j = atoi(buf[0]);
					
					msg_config[counter].can.start_msg = buf[0];
					printf("%2.2X %2.2X\n",msg_config[counter].can.start_msg,j);

					fscanf(config_msg_file,"%s",buffer);
					msg_config[i].can.sid = buffer;

					for(i=0;i<CAN_MSG_DATA_SIZE;i++)
					{
						fscanf(config_msg_file,"%d",buffer);
						msg_config[counter].can.data[i] = buffer;
					}

					fscanf(config_msg_file,"%s",buffer);
					msg_config[i].can.size = buffer;

					fscanf(config_msg_file,"%s",buffer);
					msg_config[counter].can.start_msg = buffer;

					printf("%s %s %s%s%s%s%s%s%s%s %s %s\n",msg_config[counter].can.start_msg,
										msg_config[i].can.sid,
										msg_config[counter].can.data[0],										msg_config[counter].can.data[1],
										msg_config[counter].can.data[2],
										msg_config[counter].can.data[3],
										msg_config[counter].can.data[4],
										msg_config[counter].can.data[5],
										msg_config[counter].can.data[6],
										msg_config[counter].can.data[7],
										msg_config[i].can.size,
										msg_config[counter].can.start_msg
										);
				}
				else
				{
					//TODO messages d'erreurs
				}
			}
		}
	}*/
}


