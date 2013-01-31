/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : interface.h
 *	Package : EVE
 *	Description : Fichier de déclaration des éléments composant l'interface
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */

#ifndef IHM_INTERFACE_H
	
	#define IHM_INTERFACE_H

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <string.h>
	#include <stdio.h>

	#include <gdk/gdkkeysyms.h>
	#include "../EVE_QS/EVE_all.h"

	#include "IHM_callbacks.h"
	#include "IHM_support.h"

	#define GLADE_HOOKUP_OBJECT(component,widget,name) \
	  g_object_set_data_full (G_OBJECT (component), name, \
	    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

	#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
	  g_object_set_data (G_OBJECT (component), name, widget)

	//#ifdef IHM_INTERFACE_C
		GtkWidget* create_Fenetre (void);
	//#endif
#endif
