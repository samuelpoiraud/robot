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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "IHM_callbacks.h"
#include "IHM_interface.h"
#include "IHM_support.h"
#include "string.h"


void
on_nouveau1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_ouvrir1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_enregistrer1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_enregistrer_sous1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_quitter1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_couper1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copier1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_coller1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_supprimer1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on____propos1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_can_button_clear_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTextBuffer *buffer;
	GtkWidget *can_zone = lookup_widget (GTK_WIDGET(button), "can_zt_main");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (can_zone));
	gtk_text_buffer_set_text (buffer," ",-1);
}


void
on_can_msg_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	//récupération du message can
	GtkTextBuffer *buffer;
	GtkTextBuffer *buffer2;
	GtkTextIter *iter;

	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"can_msg_zt_msg1");
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (entry));

	//fonction de test à supprimer
	GtkWidget *can_zone = lookup_widget (GTK_WIDGET(button), "can_zt_main"); 
	gtk_text_view_set_buffer(can_zone,buffer);

	//envoi au can de entry_text TODO
}


void
on_can_send_button_add_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"can_send_st_main");
	gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	printf("entry %s\n",entry_text);	
	//ecriture dans le fichier de configuration TODO

	FILE* monFichier;
	monFichier = fopen("config.txt","w+") ;
	fwrite(entry_text, sizeof(char),26, monFichier) ;
	fclose(monFichier) ;
	return 0 ;
}


void
on_can_send_button_send_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"can_send_st_main");
	gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	//envoi du message au CAN TODO
}


void
on_can_filt_chk_all_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *pro = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_pro");
	GtkWidget *stra = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_stra");
	GtkWidget *sup = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_sup");
	GtkWidget *act = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_act");
	GtkWidget *bal = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_bal");
	
	gtk_toggle_button_set_active(pro,FALSE);
	gtk_toggle_button_set_active(stra,FALSE);
	gtk_toggle_button_set_active(sup,FALSE);
	gtk_toggle_button_set_active(act,FALSE);
	gtk_toggle_button_set_active(bal,FALSE);

}


void
on_can_filt_chk_bal_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *all = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active(all,FALSE);

}


void
on_can_filt_chk_sup_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *all = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active(all,FALSE);
}


void
on_can_filt_chk_stra_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *all = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active(all,FALSE);
}


void
on_can_filt_chk_pro_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *all = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active(all,FALSE);
}


void
on_can_filt_chk_act_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *all = lookup_widget(GTK_WIDGET(button_save),"can_filt_chk_all");
	gtk_toggle_button_set_active(all,FALSE);
}


void
on_can_filt_button_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	gchar can[26];
	GtkWidget *sid = lookup_widget (GTK_WIDGET(button), "can_filt_st_sid");
	GtkWidget *data = lookup_widget (GTK_WIDGET(button), "can_filt_st_data");
	GtkWidget *size = lookup_widget (GTK_WIDGET(button), "can_filt_st_taille");
	
	gchar *entry_sid = gtk_entry_get_text(GTK_ENTRY(sid));
	gchar *entry_data = gtk_entry_get_text(GTK_ENTRY(data));
	gchar *entry_size = gtk_entry_get_text(GTK_ENTRY(size));

	strcpy(can,"01");
	strcat(can,(const char *)entry_sid);
	strcat(can,(const char *)entry_data);
	strcat(can,(const char *)entry_size);
	strcat(can,"04");
}


void
on_os_button_play_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	button_save = button;

	GtkWidget *play = lookup_widget (GTK_WIDGET(button_save), "os_button_play");
	GtkWidget *pause = lookup_widget (GTK_WIDGET(button_save), "os_button_pause");
	GtkWidget *stop = lookup_widget (GTK_WIDGET(button_save), "os_button_stop");
	GtkWidget *all = lookup_widget (GTK_WIDGET(button_save), "cei_ong_all");
	GtkWidget *filtre = lookup_widget (GTK_WIDGET(button_save), "frame13");

	gtk_widget_set_sensitive (play, FALSE);
	gtk_widget_set_sensitive (pause,TRUE);
	gtk_widget_set_sensitive (stop, TRUE);
	gtk_widget_set_sensitive (all, TRUE);
	gtk_widget_set_sensitive (filtre, TRUE);
}


void
on_os_button_pause_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *play = lookup_widget (GTK_WIDGET(button_save), "os_button_play");
	GtkWidget *pause = lookup_widget (GTK_WIDGET(button_save), "os_button_pause");

	gtk_widget_set_sensitive (play, TRUE);
	gtk_widget_set_sensitive (pause, FALSE);
}


void
on_os_button_stop_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *play = lookup_widget (GTK_WIDGET(button_save), "os_button_play");
	GtkWidget *pause = lookup_widget (GTK_WIDGET(button_save), "os_button_pause");
	GtkWidget *stop = lookup_widget (GTK_WIDGET(button_save), "os_button_stop");
	GtkWidget *all = lookup_widget (GTK_WIDGET(button_save), "cei_ong_all");


	gtk_widget_set_sensitive (play, TRUE);
	gtk_widget_set_sensitive (pause, FALSE);
	gtk_widget_set_sensitive (stop, FALSE);
	gtk_widget_set_sensitive (all, FALSE);
}


void
on_os_button_jumpOk_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	//récupération de la valeur de saut de temps
	GtkWidget *entry = lookup_widget(GTK_WIDGET(button),"os_st_timeJump");
	gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
}


void
on_cei_act_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	//transmittion de la commande
}


void
on_cei_act_button_bt2_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	//transmittion de la commande
}


void
on_cei_act_button_bt3_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	//transmittion de la commande
}


void
on_cei_act_button_bt4_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	//transmittion de la commande
}


void
on_cei_act_button_reset_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_pro_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_pro_button_bt2_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_pro_button_bt3_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_pro_button_bt4_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_pro_button_reset_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_stra_button_bt1_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_stra_button_bt2_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_stra_button_bt3_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_stra_button_bt4_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_stra_button_reset_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_sup_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_sup_button_bt2_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_sup_button_bt3_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_sup_button_bt4_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_cei_sup_button_reset_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
//transmittion de la commande
}


void
on_uart_cb_cb1_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	gint current_card;	
	current_card = gtk_combo_box_get_active(combobox);
	switch(current_card)
	{
	case 0:
		printf("actionneur uart1 \n");
	break;
	case 1:
		printf("actionneur uart2 \n");
	break;
	case 2:
		printf("balise uart1\n");
	break;
	case 3:
		printf("balise uart2 \n");
	break;
	case 4:
		printf("propulsion uart1\n");
	break;
	case 5:
		printf("propulsion uart2 \n");
	break;
	case 6:
		printf("strategie uart1 \n");
	break;
	case 7:
		printf("strategie uart2 \n");
	break;
	case 8: 
		printf("supervision uart1 \n");
	break;
	case 9:
		printf("supervision uart2 \n");
	break;
	default:
		printf("test oups\n");
	break;
	}
}

void
on_uart_button_clear1_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTextBuffer *buffer;
	GtkWidget *uart_zone = lookup_widget (GTK_WIDGET(button), "uart_zt_uart2");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (uart_zone));
	gtk_text_buffer_set_text (buffer," ",-1);
}


void
on_uart_chk_keep1_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	//if() flag baisser 
	//leve un flag	
	//else
	//baisse flag	
}


void
on_uart_cb_cb2_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	gint current_card;	
	current_card = gtk_combo_box_get_active(combobox);
	switch(current_card)
	{
	case 0:
		printf("actionneur uart1 \n");
	break;
	case 1:
		printf("actionneur uart2 \n");
	break;
	case 2:
		printf("balise uart1\n");
	break;
	case 3:
		printf("balise uart2 \n");
	break;
	case 4:
		printf("propulsion uart1\n");
	break;
	case 5:
		printf("propulsion uart2 \n");
	break;
	case 6:
		printf("strategie uart1 \n");
	break;
	case 7:
		printf("strategie uart2 \n");
	break;
	case 8: 
		printf("supervision uart1 \n");
	break;
	case 9:
		printf("supervision uart2 \n");
	break;
	default:
		printf("test oups\n");
	break;
	}
}


void
on_uart_button_clear2_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTextBuffer *buffer;
	GtkWidget *uart_zone = lookup_widget (GTK_WIDGET(button), "uart_zt_uart2");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (uart_zone));
	gtk_text_buffer_set_text (buffer," ",-1);
}


void
on_uart_chk_keep2_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	//if() flag baisser 
	//leve un flag	
	//else
	//baisse flag
}

