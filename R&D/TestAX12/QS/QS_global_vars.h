/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : QS_global_vars.h
 *  Package : Qualité Soft
 *  Description : Variables globales generiques communes a tous les
 *					codes du robot.
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
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
