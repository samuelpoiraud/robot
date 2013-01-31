/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_global_vars.h
 *	Package : QSx86
 *	Description : Déclaration de la structure contenant les variables globales
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_GLOBAL_VARS_H
	#define QS_GLOBAL_VARS_H

	#include "../QS/QS_types.h"
	#include "../Global_config.h"
	#include "../Global_vars_types.h"


	typedef struct
	{		
		/* INSERTION DES FLAGS UTILISATEUR */
		#include "../Global_flags.h"
	} flag_list_t;



	typedef struct
	{		
		/* les drapeaux */
		volatile flag_list_t flags;

		/* INSERTION DES VARIABLES GLOBALES UTILISATEUR */
		#include "../Global_vars.h"
	} global_data_storage_t;

	extern global_data_storage_t global;
#endif /* ndef QS_GLOBAL_VARS_H */
