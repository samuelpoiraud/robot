/*
 *	Club Robot ESEO 2013
 *	Tiny & Krusty
 *
 *	Fichier : QS_who_am_i.c
 *	Package : Qualite Software
 *	Description :	Utilise des ports de la carte facto pour identifier 
 *					le robot sur lequel la carte se trouve.
 *	Auteur : Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201302
 */
#include "QS_who_am_i.h"


#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif


	volatile robot_id_e robot_id = 0;
	volatile bool_e found = FALSE;
	
	//@brief 	Cette fonction doit être appelée lors de l'initialisation.
	//@pre 		le fond de panier sur lequel la carte est branché doit être cablé conformément à "_PORTS.xlsx"
	//@post		Le module QS_WHO_AM_I sait alors sur quel robot la carte est branchée
	void QS_WHO_AM_I_find(void)
	{
		if(found)
			return;		//Vous ne devez appeler QS_WHO_AM_I_find qu'une seule fois !
		found = TRUE;

		#if !defined(DISABLE_WHO_AM_I)
			//On lit l'entrée...
			if(PORT_ROBOT_ID)
				robot_id = TINY;
			else
				robot_id = KRUSTY;
		#elif 1-DISABLE_WHO_AM_I-1 == 2		//Permet de savoir si DISABLE_WHO_AM_I est défini à quelque chose. Si c'est pas le cas, on a 1--1 == 1+1 == 2
			#warning "Definir DISABLE_WHO_AM_I a KRUSTY ou TINY pour definir le robot explicitement. Ici, TINY sera choisi par defaut."
			robot_id = TINY;	//Par défaut, le robot est ...
		#else
#warning test
			robot_id = DISABLE_WHO_AM_I;
		#endif
	}
	
	
	//@return	L'identifiant du robot sur lequel est branchée la carte qui exécute ce code !
	robot_id_e QS_WHO_AM_I_get(void)
	{
		assert(found);		//Vous devez appeler QS_WHO_AM_I_find dans l'initialisation avant tout appel à QS_WHO_AM_I_get.
		return robot_id;
	}
	

	
