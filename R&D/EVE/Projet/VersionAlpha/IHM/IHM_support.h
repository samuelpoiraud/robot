/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : support.h
 *	Package : EVE
 *	Description : Fonction nécessaire au fonctionnement de l'interface Gtk
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */

#ifndef IHM_SUPPORT_H
	#define IHM_SUPPORT_H

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <string.h>
	#include <stdio.h>
	#include <gtk/gtk.h>

	/*
	 * Standard gettext macros.
	 */
	#ifdef ENABLE_NLS
	#  include <libintl.h>
	#  undef _
	#  define _(String) dgettext (PACKAGE, String)
	#  define Q_(String) g_strip_context ((String), gettext (String))
	#  ifdef gettext_noop
	#    define N_(String) gettext_noop (String)
	#  else
	#    define N_(String) (String)
	#  endif
	#else
	#  define textdomain(String) (String)
	#  define gettext(String) (String)
	#  define dgettext(Domain,Message) (Message)
	#  define dcgettext(Domain,Message,Type) (Message)
	#  define bindtextdomain(Domain,Directory) (Domain)
	#  define _(String) (String)
	#  define Q_(String) g_strip_context ((String), (String))
	#  define N_(String) (String)
	#endif

	//#ifdef IHM_SUPPORT_C
		GtkWidget*  lookup_widget              (GtkWidget       *widget,
				                        const gchar     *widget_name);


		/* Use this function to set the directory containing installed pixmaps. */
		void        add_pixmap_directory       (const gchar     *directory);


		/*
		 * Private Functions.
		 */

		/* This is used to create the pixmaps used in the interface. */
		GtkWidget*  create_pixmap              (GtkWidget       *widget,
				                        const gchar     *filename);

		/* This is used to create the pixbufs used in the interface. */
		GdkPixbuf*  create_pixbuf              (const gchar     *filename);

		/* This is used to set ATK action descriptions. */
		void        glade_set_atk_action_description (AtkAction       *action,
				                              const gchar     *action_name,
				                              const gchar     *description);
	//#endif


#endif

