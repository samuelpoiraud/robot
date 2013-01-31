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
	
	#include "../EVE_QS/EVE_all.h"
	#include "IHM_interface.h"
	#include "IHM_support.h"
	
	#define DATA_SIZE 45
	#define NUMBER_MSG 30
	#define DESCRIPTION_SIZE 20

	typedef struct{
        	int type;
		int param;
    	}data_config_t;

	typedef struct{
		char description[DESCRIPTION_SIZE];
		EVE_CAN_msg_t can;
	}msg_config_t;

	volatile data_config_t data_config[DATA_SIZE];
	volatile msg_config_t msg_config[NUMBER_MSG];

	/* always defined to indicate that i18n is enabled */
	#define ENABLE_NLS 1

	/* Gettext package. */
	#define GETTEXT_PACKAGE "eve"

	/* Define to 1 if you have the `bind_textdomain_codeset' function. */
	#define HAVE_BIND_TEXTDOMAIN_CODESET 1

	/* Define to 1 if you have the `dcgettext' function. */
	#define HAVE_DCGETTEXT 1

	/* Define if the GNU gettext() function is already present or preinstalled. */
	#define HAVE_GETTEXT 1

	/* Define to 1 if you have the <inttypes.h> header file. */
	#define HAVE_INTTYPES_H 1

	/* Define if your <locale.h> file defines LC_MESSAGES. */
	#define HAVE_LC_MESSAGES 1

	/* Define to 1 if you have the <locale.h> header file. */
	#define HAVE_LOCALE_H 1

	/* Define to 1 if you have the <memory.h> header file. */
	#define HAVE_MEMORY_H 1

	/* Define to 1 if you have the <stdint.h> header file. */
	#define HAVE_STDINT_H 1

	/* Define to 1 if you have the <stdlib.h> header file. */
	#define HAVE_STDLIB_H 1

	/* Define to 1 if you have the <strings.h> header file. */
	#define HAVE_STRINGS_H 1

	/* Define to 1 if you have the <string.h> header file. */
	#define HAVE_STRING_H 1

	/* Define to 1 if you have the <sys/stat.h> header file. */
	#define HAVE_SYS_STAT_H 1

	/* Define to 1 if you have the <sys/types.h> header file. */
	#define HAVE_SYS_TYPES_H 1

	/* Define to 1 if you have the <unistd.h> header file. */
	#define HAVE_UNISTD_H 1

	/* Name of package */
	#define PACKAGE "eve"

	/* Define to the address where bug reports for this package should be sent. */
	#define PACKAGE_BUGREPORT ""

	/* Define to the full name of this package. */
	#define PACKAGE_NAME ""

	/* Define to the full name and version of this package. */
	#define PACKAGE_STRING ""

	/* Define to the one symbol short name of this package. */
	#define PACKAGE_TARNAME ""

	/* Define to the home page for this package. */
	#define PACKAGE_URL ""

	/* Define to the version of this package. */
	#define PACKAGE_VERSION ""

	/* Define to 1 if you have the ANSI C header files. */
	#define STDC_HEADERS 1

	/* Version number of package */
	#define VERSION "0.1"
	
	//#ifdef IHM_DISPLAY_C
		void* IHM_display_thread();

		//fonction non implémentée 
		//void IHM_display_set_current_time(EVE_time_t current_time);  //envoi le temps
		
		//void IHM_display_set_match_time(EVE_time_t match_time); 

		//void IHM_display_update_filters(can_filter_t can_filters); 

		//void IHM_display_send_data(EVE_QS_data_msg_t data);
		//void IHM_display_change_uart_display(int uart_id,uart_list_e new_uart_display);

		void IHM_display_init(void);

		void IHM_display_destroy(void);

		void IHM_display_load_configuration_file(FILE* config_data_file,FILE* config_msg_file);
	//#endif
#endif
