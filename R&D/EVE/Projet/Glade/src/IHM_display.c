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

#include "IHM_display.h"

void* IHM_thread_display()
{
  GtkWidget *Fenetre;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (0,0);

  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  Fenetre = create_Fenetre ();
  gtk_widget_show (Fenetre);
  printf("thread lancé: je confirme \n");
  gtk_main ();
  return 0;
}
