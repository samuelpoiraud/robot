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
	
	//@brief 	Cette fonction doit �tre appel�e lors de l'initialisation.
	//@pre 		le fond de panier sur lequel la carte est branch� doit �tre cabl� conform�ment � "_PORTS.xlsx"
	//@post		Le module QS_WHO_AM_I sait alors sur quel robot la carte est branch�e
	void QS_WHO_AM_I_find(void)
	{
		if(found)
			return;		//Vous ne devez appeler QS_WHO_AM_I_find qu'une seule fois !
		found = TRUE;
		robot_id = TINY;	//Par d�faut, le robot est ...

		#ifdef DISABLE_WHO_AM_I
			return;
		#endif

		//On lit l'entr�e...
		if(PORT_ROBOT_ID)
			robot_id = TINY;
		else
			robot_id = KRUSTY;
	}
	
	
	//@return	L'identifiant du robot sur lequel est branch�e la carte qui ex�cute ce code !
	robot_id_e QS_WHO_AM_I_get(void)
	{
		assert(found);		//Vous devez appeler QS_WHO_AM_I_find dans l'initialisation avant tout appel � QS_WHO_AM_I_get.
		return robot_id;
	}
	

	
