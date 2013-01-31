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

#include <gtk/gtk.h>

GtkButton *button_save;

void
on_nouveau1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ouvrir1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_enregistrer1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_enregistrer_sous1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quitter1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_couper1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copier1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_coller1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_supprimer1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on____propos1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_can_button_clear_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_can_msg_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_can_send_button_add_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_can_send_button_send_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_can_filt_chk_all_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_can_filt_chk_bal_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_can_filt_chk_sup_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_can_filt_chk_stra_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_can_filt_chk_pro_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_can_filt_chk_act_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_can_filt_button_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_os_button_play_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_os_button_pause_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_os_button_stop_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_os_button_jumpOk_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_act_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_act_button_bt2_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_act_button_bt3_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_act_button_bt4_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_act_button_reset_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_pro_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_pro_button_bt2_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_pro_button_bt3_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_pro_button_bt4_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_pro_button_reset_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_stra_button_bt1_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_stra_button_bt2_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_stra_button_bt3_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_stra_button_bt4_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_stra_button_reset_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_sup_button_bt1_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_sup_button_bt2_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_sup_button_bt3_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_sup_button_bt4_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cei_sup_button_reset_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_uart_cb_cb1_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_uart_button_clear1_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_uart_chk_keep1_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_uart_cb_cb2_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_uart_button_clear2_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_uart_chk_keep2_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
