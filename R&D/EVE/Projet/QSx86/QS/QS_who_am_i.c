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


	volatile robot_id_e robot_id = 0;
	void QS_WHO_AM_I_delay(void);
	volatile bool_e found = FALSE;
	
	//@brief 	Cette fonction doit être appelée lors de l'initialisation.
	//@pre 		le fond de panier sur lequel la carte est branché doit être cablé conformément à "_PORTS.xlsx"
	//@post		Le module QS_WHO_AM_I sait alors sur quel robot la carte est branchée
	void QS_WHO_AM_I_find(void)
	{
		//Par défaut, sur un FDP!=2013 le robot est KRUSTY.
		robot_id = KRUSTY;
		found = TRUE;
	}
	
	
	//@return	L'identifiant du robot sur lequel est branchée la carte qui exécute ce code !
	robot_id_e QS_WHO_AM_I_get(void)
	{
		assert(found);		//Vous devez appeler QS_WHO_AM_I_find dans l'initialisation avant tout appel à QS_WHO_AM_I_get.
	
		return robot_id;
	}
	
	void QS_WHO_AM_I_delay(void)
	{
		volatile Uint8 i;
		for(i=0; i<100; i++);
	}
	
	
