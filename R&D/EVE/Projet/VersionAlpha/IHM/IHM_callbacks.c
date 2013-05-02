/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : callback.c
 *	Package : EVE
 *	Description : Fichier gérant l'ensemble des fonctions liés à l'utilisation des 	*                    boutons
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */
#define IHM_CALLBACKS_C
#include "IHM_callbacks.h"
#include "../can.h"
#include <stdlib.h>


void on_nouveau1_activate(GtkMenuItem *menuitem,gpointer user_data)
{

}

void on_ouvrir1_activate (GtkMenuItem     *menuitem, gpointer user_data)
{

}

void on_enregistrer1_activate (GtkMenuItem     *menuitem, gpointer user_data)
{

}

void
on_enregistrer_sous1_activate(GtkMenuItem     *menuitem, gpointer         user_data)
{

}

void on_quitter1_activate(GtkMenuItem     *menuitem, gpointer         user_data)
{
	EVE_QS_data_msg_t data_msg;
	int i =0;
	Uint32 queue_id = 0;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;

	// Sortir de la boucle gtk_main()
	gtk_main_quit();

	pthread_mutex_lock(&global.use_cards_mutex);
	pthread_mutex_lock(&global.queue_id_mutex);
	
	for(i=0;i<5;i++)
	{
		if(global.use_cards[i]==1)
		{
			data_msg.data_id = QS_SIMULATION_STOP;
			switch(i)
			{
				case 0:
					queue_id = global.queue_data_id_actionneurs;
					break;
				case 1:
					queue_id = global.queue_data_id_balises;
					break;
				case 2:
					queue_id = global.queue_data_id_propulsion;
					break;
				case 3:
					queue_id = global.queue_data_id_strategie;
					break;
				case 4:
					queue_id = global.queue_data_id_supervision;
					break;
			}
			EVE_write_new_msg_QS(queue_id,data_msg);
		}
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_cards_mutex);

	GLOBAL_VARS_set_sim_status(SIMULATION_QUIT);
}

void on_couper1_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{

}

void on_copier1_activate(GtkMenuItem     *menuitem, gpointer         user_data)
{

}

void on_coller1_activate(GtkMenuItem     *menuitem, gpointer         user_data)
{

}

void on_supprimer1_activate(GtkMenuItem     *menuitem, gpointer         user_data)
{

}

void on____propos1_activate(GtkMenuItem     *menuitem, gpointer         user_data)
{

}

void on_can_button_clear_clicked(GtkButton       *button,gpointer         user_data)
{
	GtkTextBuffer *buffer;
	GtkWidget *can_zone = lookup_widget (GTK_WIDGET(button), "can_zt_main");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (can_zone));
	gtk_text_buffer_set_text (buffer," ",-1);
}

void on_can_msg_button_bt1_clicked (GtkButton       *button,gpointer         user_data)
{
	//récupération du message can
	GtkTextBuffer *buffer;

	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"can_msg_zt_msg1");
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (entry));

	//envoi au can de entry_text TODO
}


void on_can_send_button_add_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"can_send_st_main");
	const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	printf("entry %s\n",entry_text);	
	//ecriture dans le fichier de configuration TODO

	FILE* monFichier;
	monFichier = fopen("config.txt","w+") ;
	fwrite(entry_text, sizeof(char),26, monFichier) ;
	fclose(monFichier) ;
}

void on_can_send_button_send_clicked        (GtkButton       *button,gpointer         user_data)
{
	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"can_send_st_main");
	const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	//envoi du message au CAN TODO
	EVE_CAN_msg_t eve_can_msg;
	int i;   
	char hexByte[3] = {0};
	
	eve_can_msg.mtype = QUEUE_CAN_MTYPE;
	eve_can_msg.start_msg = CAN_MESSAGE_START;
	eve_can_msg.sid = 0;
	eve_can_msg.size = 0;
	eve_can_msg.end_msg = CAN_MESSAGE_STOP;
	
	for(i=0;i<CAN_MSG_DATA_SIZE;i++)
	{
		eve_can_msg.data[i] = 0;
	}
	
	printf("CAN Message send:");
	for(i = 1; i < 12; i++) {
		unsigned int val;
		hexByte[0] = entry_text[i*2];
		hexByte[1] = entry_text[i*2+1];
		hexByte[2] = 0;
		val = strtoul(hexByte, NULL, 16);
		printf(" %02X", val);
		if(i == 1)
			eve_can_msg.sid = val << 8;
		else if(i == 2)
			eve_can_msg.sid |= val;
		else if(i >= 3 && i <= 10)
			eve_can_msg.data[i - 3] = val;
		else if(i == 11)
			eve_can_msg.size = val;
	}
	printf("\n");
	
	CAN_broadcast_msg(eve_can_msg, -1);
}


void on_can_filt_chk_all_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.button_save_mutex);

	// Gestion des filtres
	GtkWidget *pro = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_pro");
	GtkWidget *stra = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_stra");
	GtkWidget *sup = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_sup");
	GtkWidget *act = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_act");
	GtkWidget *bal = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_bal");
	
	gtk_toggle_button_set_active((GtkToggleButton *)pro,FALSE);
	gtk_toggle_button_set_active((GtkToggleButton *)stra,FALSE);
	gtk_toggle_button_set_active((GtkToggleButton *)sup,FALSE);
	gtk_toggle_button_set_active((GtkToggleButton *)act,FALSE);
	gtk_toggle_button_set_active((GtkToggleButton *)bal,FALSE);

	//TODO: envoi au thread CAN du filtre actif

	pthread_mutex_unlock(&global.button_save_mutex);
}


void on_can_filt_chk_bal_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.button_save_mutex);

	GtkWidget *all = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active((GtkToggleButton *)all,FALSE);

	//TODO: envoi au thread CAN du filtre actif

	pthread_mutex_unlock(&global.button_save_mutex);
}


void on_can_filt_chk_sup_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.button_save_mutex);

	GtkWidget *all = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active((GtkToggleButton *)all,FALSE);

	//TODO: envoi au thread CAN du filtre actif

	pthread_mutex_unlock(&global.button_save_mutex);
}


void on_can_filt_chk_stra_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.button_save_mutex);

	GtkWidget *all = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active((GtkToggleButton *)all,FALSE);

	//TODO: envoi au thread CAN du filtre actif

	pthread_mutex_unlock(&global.button_save_mutex);
}


void on_can_filt_chk_pro_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.button_save_mutex);

	GtkWidget *all = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active((GtkToggleButton *)all,FALSE);

	//TODO: envoi au thread CAN du filtre actif

	pthread_mutex_unlock(&global.button_save_mutex);
}


void on_can_filt_chk_act_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.button_save_mutex);

	GtkWidget *all = lookup_widget(GTK_WIDGET(global.button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active((GtkToggleButton *)all,FALSE);

	//TODO: envoi au thread CAN du filtre actif

	pthread_mutex_unlock(&global.button_save_mutex);
}


void on_can_filt_button_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	gchar can[26];
	GtkWidget *sid = lookup_widget (GTK_WIDGET(button), "can_filt_st_sid");
	GtkWidget *data = lookup_widget (GTK_WIDGET(button), "can_filt_st_data");
	GtkWidget *size = lookup_widget (GTK_WIDGET(button), "can_filt_st_taille");
	
	// Récupération du message CAN entré par l'utilisateur
	const gchar *entry_sid = gtk_entry_get_text(GTK_ENTRY(sid));
	const gchar *entry_data = gtk_entry_get_text(GTK_ENTRY(data));
	const gchar *entry_size = gtk_entry_get_text(GTK_ENTRY(size));

	//TODO placer cette entrée dans un type CAN et l'envoyer au thread CAN
	strcpy(can,"01");
	strcat(can,entry_sid);
	strcat(can,entry_data);
	strcat(can,entry_size);
	strcat(can,"04");
}


void on_os_button_play_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	EVE_QS_data_msg_t data_msg;
	int i =0;
	Uint32 queue_id = 0;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;

	// Déclare que la simulation est en cours
	GLOBAL_VARS_set_sim_status(SIMULATION_RUN);

	pthread_mutex_lock(&global.button_save_mutex);

	// Variable permettant la liaison entre les deux threads de l'IHM
	global.button_save = button;

	// Récupération des éléments de l'interface
	GtkWidget *play = lookup_widget (GTK_WIDGET(button), "os_button_play");
	GtkWidget *pause = lookup_widget (GTK_WIDGET(button), "os_button_pause");
	GtkWidget *stop = lookup_widget (GTK_WIDGET(button), "os_button_stop");
	GtkWidget *all = lookup_widget (GTK_WIDGET(button), "cei_ong_all");
	GtkWidget *filtre = lookup_widget (GTK_WIDGET(button), "frame13");

	// Activation et déactivation des boutons sur l'interface
	gtk_widget_set_sensitive (play, FALSE);
	gtk_widget_set_sensitive (pause,TRUE);
	gtk_widget_set_sensitive (stop, TRUE);
	gtk_widget_set_sensitive (all, TRUE);
	gtk_widget_set_sensitive (filtre, TRUE);

	pthread_mutex_unlock(&global.button_save_mutex);

	// Envoi à toutes les cartes utilisées le message RUN	
	pthread_mutex_lock(&global.use_cards_mutex);
	pthread_mutex_lock(&global.queue_id_mutex);
	
	for(i=0;i<5;i++)
	{
		if(global.use_cards[i]==1)
		{
			data_msg.data_id = QS_SIMULATION_RUN;
			switch(i)
			{
				case 0:
					queue_id = global.queue_data_id_actionneurs;
					break;
				case 1:
					queue_id = global.queue_data_id_balises;
					break;
				case 2:
					queue_id = global.queue_data_id_propulsion;
					break;
				case 3:
					queue_id = global.queue_data_id_strategie;
					break;
				case 4:
					queue_id = global.queue_data_id_supervision;
					break;
			}
			EVE_write_new_msg_QS(queue_id,data_msg);
		}
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_cards_mutex);

	// Envoi l'ordre de start uart aux cartes concernées
	send_start_uart();
}	

void on_os_button_pause_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	EVE_QS_data_msg_t data_msg;
	int i =0;
	Uint32 queue_id = 0;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;

	// Récupération des éléments de l'interface
	GtkWidget *play = lookup_widget (GTK_WIDGET(button), "os_button_play");
	GtkWidget *pause = lookup_widget (GTK_WIDGET(button), "os_button_pause");

	gtk_widget_set_sensitive (play, TRUE);
	gtk_widget_set_sensitive (pause, FALSE);

	GLOBAL_VARS_set_sim_status(SIMULATION_PAUSE);

	pthread_mutex_lock(&global.use_cards_mutex);
	pthread_mutex_lock(&global.queue_id_mutex);
	
	for(i=0;i<5;i++)
	{
		if(global.use_cards[i]==1)
		{
			data_msg.data_id = QS_SIMULATION_PAUSE;
			switch(i)
			{
				case 0:
					queue_id = global.queue_data_id_actionneurs;
					break;
				case 1:
					queue_id = global.queue_data_id_balises;
					break;
				case 2:
					queue_id = global.queue_data_id_propulsion;
					break;
				case 3:
					queue_id = global.queue_data_id_strategie;
					break;
				case 4:
					queue_id = global.queue_data_id_supervision;
					break;
			}
			EVE_write_new_msg_QS(queue_id,data_msg);
		}
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_cards_mutex);
}


void on_os_button_stop_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	EVE_QS_data_msg_t data_msg;
	int i =0;
	Uint32 queue_id = 0;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;

	// Récupération des éléments de l'interface
	GtkWidget *play = lookup_widget (GTK_WIDGET(button), "os_button_play");
	GtkWidget *pause = lookup_widget (GTK_WIDGET(button), "os_button_pause");
	GtkWidget *stop = lookup_widget (GTK_WIDGET(button), "os_button_stop");
	GtkWidget *all = lookup_widget (GTK_WIDGET(button), "cei_ong_all");


	gtk_widget_set_sensitive (play, TRUE);
	gtk_widget_set_sensitive (pause, FALSE);
	gtk_widget_set_sensitive (stop, FALSE);
	gtk_widget_set_sensitive (all, FALSE);

	GLOBAL_VARS_set_sim_status(SIMULATION_STOP);
	
		pthread_mutex_lock(&global.use_cards_mutex);
	pthread_mutex_lock(&global.queue_id_mutex);
	
	for(i=0;i<5;i++)
	{
		if(global.use_cards[i]==1)
		{
			data_msg.data_id = QS_SIMULATION_STOP;
			switch(i)
			{
				case 0:
					queue_id = global.queue_data_id_actionneurs;
					break;
				case 1:
					queue_id = global.queue_data_id_balises;
					break;
				case 2:
					queue_id = global.queue_data_id_propulsion;
					break;
				case 3:

					queue_id = global.queue_data_id_strategie;
					break;
				case 4:
					queue_id = global.queue_data_id_supervision;
					break;
			}
			EVE_write_new_msg_QS(queue_id,data_msg);
		}
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_cards_mutex);

}

void on_os_button_jumpOk_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	// Récupération des éléments de l'interface
	//GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"os_st_timeJump");
	//récupération de la valeur de saut de temps
	//const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
}


void on_cei_act_button_bt1_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);	
}

void on_cei_act_button_bt2_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_act_button_bt3_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}


void on_cei_act_button_bt4_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_act_button_reset_pressed        (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_bt1_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}


void on_cei_pro_button_bt2_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_bt3_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_bt4_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}


void on_cei_pro_button_reset_pressed        (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);

}

void on_cei_stra_button_bt1_pressed         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_bt2_pressed         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_bt3_pressed         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_bt4_pressed         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_reset_pressed       (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_bt1_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}


void on_cei_sup_button_bt2_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}


void on_cei_sup_button_bt3_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_bt4_pressed          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_reset_pressed        (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 1;

	//transmittion de l'appuie du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);

}

void on_cei_act_button_bt1_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_act_button_bt2_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_act_button_bt3_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_act_button_bt4_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_act_button_reset_released        (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);

	// Avertir le manageur de relancer le processus
	pthread_mutex_lock(&global.reset_cards_mutex);
	global.reset_cards = TRUE;
	pthread_mutex_unlock(&global.reset_cards_mutex);

	// Renvoi le start uart aux cartes utilisées
	send_start_uart();
}

void on_cei_pro_button_bt1_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_bt2_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_bt3_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_bt4_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_pro_button_reset_released        (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);

	// Avertir le manageur de relancer le processus
	pthread_mutex_lock(&global.reset_cards_mutex);
	global.reset_cards = TRUE;
	pthread_mutex_unlock(&global.reset_cards_mutex);

	// Renvoi le start uart aux cartes utilisées
	send_start_uart();
}

void on_cei_stra_button_bt1_released         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_bt2_released         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_bt3_released         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_bt4_released         (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_stra_button_reset_released       (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);

	// Avertir le manageur de relancer le processus
	pthread_mutex_lock(&global.reset_cards_mutex);
	global.reset_cards = TRUE;
	pthread_mutex_unlock(&global.reset_cards_mutex);

	// Renvoi le start uart aux cartes utilisées
	send_start_uart();
}

void on_cei_sup_button_bt1_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_1;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_bt2_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_2;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_bt3_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_3;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_bt4_released          (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_4;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_cei_sup_button_reset_released        (GtkButton       *button,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.queue_id_mutex);

	// Création du message DATA
	EVE_QS_data_msg_t data_msg;
	data_msg.data_id = QS_BUTTON_RESET;
	data_msg.data1 = 0;

	//transmittion du relachement du bouton
	EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);

	pthread_mutex_unlock(&global.queue_id_mutex);

	// Avertir le manageur de relancer le processus
	pthread_mutex_lock(&global.reset_cards_mutex);
	global.reset_cards = TRUE;
	pthread_mutex_unlock(&global.reset_cards_mutex);

	// Renvoi le start uart aux cartes utilisées
	send_start_uart();
}

void on_uart_cb_cb1_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	gint current_card;	
	current_card = gtk_combo_box_get_active(combobox);
	EVE_QS_data_msg_t data_msg;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;

	pthread_mutex_lock(&global.queue_id_mutex);
	pthread_mutex_lock(&global.use_uart_mutex);
	switch(global.use_uart[0])
	{
		case 0:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 1:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 2:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 3:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 4:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 5:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 6:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 7:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 8: 
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		case 9:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		default:
			printf("test oups\n"); //TODO gestion erreur
			break;
			
			
	}
	global.use_uart[0]= current_card + 1;
	switch(current_card)
	{
		case 0:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 1:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 2:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 3:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 4:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 5:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 6:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 7:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 8: 
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		case 9:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		default:
			printf("test oups\n"); //TODO gestion erreur
			break;
	}
	pthread_mutex_unlock(&global.use_uart_mutex);
	pthread_mutex_unlock(&global.queue_id_mutex);
}

void on_uart_button_clear1_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTextBuffer *buffer;
	GtkWidget *uart_zone = lookup_widget (GTK_WIDGET(button), "uart_zt_uart1");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (uart_zone));
	gtk_text_buffer_set_text (buffer," ",-1);
}


void on_uart_chk_keep1_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	//if() flag baisser 
	//leve un flag	
	//else
	//baisse flag	
}


void on_uart_cb_cb2_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	gint current_card;	
	current_card = gtk_combo_box_get_active(combobox);
	EVE_QS_data_msg_t data_msg;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;

	pthread_mutex_lock(&global.use_uart_mutex);
	pthread_mutex_lock(&global.queue_id_mutex);
	switch(global.use_uart[1])
	{
		case 0:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 1:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 2:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 3:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 4:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 5:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 6:
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 7:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 8: 
			data_msg.data_id = QS_STOP_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		case 9:
			data_msg.data_id = QS_STOP_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		default:
			printf("test oups\n"); //TODO gestion erreur
			break;
			
			
	}
	global.use_uart[1]= current_card + 1;
	switch(current_card)
	{
		case 0:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 1:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
			break;
		case 2:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 3:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
			break;
		case 4:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 5:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
			break;
		case 6:
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 7:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
			break;
		case 8: 
			data_msg.data_id = QS_START_UART1;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		case 9:
			data_msg.data_id = QS_START_UART2;
			EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
			break;
		default:
			printf("test oups\n"); //TODO gestion erreur
			break;
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_uart_mutex);
}

void on_uart_button_clear2_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTextBuffer *buffer;
	GtkWidget *uart_zone = lookup_widget (GTK_WIDGET(button), "uart_zt_uart2");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (uart_zone));
	gtk_text_buffer_set_text (buffer," ",-1);
}

void on_uart_chk_keep2_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	//if() flag baisser 
	//leve un flag	
	//else
	//baisse flag
}

void on_cei_ong_all_switch_page             (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data)
{
	pthread_mutex_lock(&global.notebook_page_mutex);
	global.notebook_page = (Uint8) page_num;
	pthread_mutex_unlock(&global.notebook_page_mutex);
}
void send_start_uart()
{
	int text_area = 0;
	EVE_QS_data_msg_t data_msg;
	data_msg.mtype = QUEUE_QS_DATA_MTYPE;
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_uart_mutex);
	for(text_area = 0; text_area<2;text_area++)
	{
		switch((global.use_uart[text_area]-1))
		{
			case 0:
				data_msg.data_id = QS_START_UART1;
				EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
				break;
			case 1:
				data_msg.data_id = QS_START_UART2;
				EVE_write_new_msg_QS(global.queue_data_id_actionneurs,data_msg);
				break;
			case 2:
				data_msg.data_id = QS_START_UART1;
				EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
				break;
			case 3:
				data_msg.data_id = QS_START_UART2;
				EVE_write_new_msg_QS(global.queue_data_id_balises,data_msg);
				break;
			case 4:
				data_msg.data_id = QS_START_UART1;
				EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
				break;
			case 5:
				data_msg.data_id = QS_START_UART2;
				EVE_write_new_msg_QS(global.queue_data_id_propulsion,data_msg);
				break;
			case 6:
				data_msg.data_id = QS_START_UART1;
				EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
				break;
			case 7:
				data_msg.data_id = QS_START_UART2;
				EVE_write_new_msg_QS(global.queue_data_id_strategie,data_msg);
				break;
			case 8: 
				data_msg.data_id = QS_START_UART1;
				EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
				break;
			case 9:
				data_msg.data_id = QS_START_UART2;
				EVE_write_new_msg_QS(global.queue_data_id_supervision,data_msg);
				break;
			default:
				break;
		}
	}
	pthread_mutex_unlock(&global.queue_id_mutex);
	pthread_mutex_unlock(&global.use_uart_mutex);
}

