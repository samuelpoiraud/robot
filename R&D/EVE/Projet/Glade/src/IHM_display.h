/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : IHM_display.h
 *	Package : EVE
 *	Description : Gestion de l'interface Gtk
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */

#ifndef IHM_DISPLAY_H
	#define IHM_DISPLAY_H

	#ifdef HAVE_CONFIG_H
	#  include <config.h>
	#endif

	#include <gtk/gtk.h>

	#include "interface.h"
	#include "support.h"

	void* IHM_thread_display(void);

	//void IHM_set_current_time(EVE_time_t current_time);  //envoi le temps
	//void IHM_set_match_time(EVE_time_t match_time); 

	//void IHM_update_filters(can_filter_t can_filters); 

	//void IHM_send_data(EVE_QS_data_msg_t data);

	void IHM_init(void);
#endif
