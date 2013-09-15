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
	
	//@brief 	Cette fonction doit �tre appel�e lors de l'initialisation.
	//@pre 		le fond de panier sur lequel la carte est branch� doit �tre cabl� conform�ment � "_PORTS.xlsx"
	//@post		Le module QS_WHO_AM_I sait alors sur quel robot la carte est branch�e
	void QS_WHO_AM_I_find(void)
	{
		assert(!found);		//Vous ne devez appeler QS_WHO_AM_I_find qu'une seule fois !
		#ifndef FDP_2013	//Retrocompatibilit� : on renvoie
			//Par d�faut, sur un FDP!=2013 le robot est ...
			robot_id = TINY;
			found = TRUE;
			return;
		#endif
		//Le port robot_id_output est mis en sortie 5V.
		TRIS_ROBOT_ID_OUTPUT 	= 0;
		LAT_ROBOT_ID_OUTPUT 	= 1;
		
		//Le port robot_id_input est mis en entr�e.
		TRIS_ROBOT_ID_INPUT		= 1;
		
		//Delay d'attente de stabilisation de la ligne... par s�curit�.
		QS_WHO_AM_I_delay();
		
		//On lit l'entr�e...
		if(PORT_ROBOT_ID_INPUT)
			robot_id = TINY;
		else
			robot_id = KRUSTY;
		
		//On remet le robot_id_output en entr�e pour ne pas consommer b�tement.
		TRIS_ROBOT_ID_OUTPUT 	= 1;
		found = TRUE;
	}
	
	
	//@return	L'identifiant du robot sur lequel est branch�e la carte qui ex�cute ce code !
	robot_id_e QS_WHO_AM_I_get(void)
	{
		assert(found);		//Vous devez appeler QS_WHO_AM_I_find dans l'initialisation avant tout appel � QS_WHO_AM_I_get.
		return robot_id;
	}
	
	void QS_WHO_AM_I_delay(void)
	{
		volatile Uint8 i;
		for(i=0; i<100; i++);
	}
	
	
