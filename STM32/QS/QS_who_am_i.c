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

	#include "../config/config_pin.h"


	volatile robot_id_e robot_id = 0;
	volatile bool_e found = FALSE;
	static volatile bool_e initialized = FALSE;

	//@brief 	Cette fonction doit �tre appel�e lors de l'initialisation.
	//@pre 		le fond de panier sur lequel la carte est branch� doit �tre cabl� conform�ment � "_PORTS.xlsx"
	//@post		Le module QS_WHO_AM_I sait alors sur quel robot la carte est branch�e
	void QS_WHO_AM_I_find(void)
	{
		initialized = TRUE;
		if(found)
			return;		//Vous ne devez appeler QS_WHO_AM_I_find qu'une seule fois !
		found = TRUE;

		#if !defined(DISABLE_WHO_AM_I)
			//On lit l'entr�e...
			if(PORT_ROBOT_ID)
				robot_id = BIG_ROBOT;
			else
				robot_id = SMALL_ROBOT;
		#elif defined(I_AM_CARTE_BEACON_EYE)
			robot_id = BEACON_EYE;
		#elif 1-DISABLE_WHO_AM_I-1 == 2		//Permet de savoir si DISABLE_WHO_AM_I est d�fini � quelque chose. Si c'est pas le cas, on a 1--1 == 1+1 == 2
			#warning "Definir DISABLE_WHO_AM_I a BIG ou SMALL pour definir le robot explicitement. Ici, SMALL sera choisi par defaut."
			robot_id = SMALL_ROBOT;	//Par d�faut, le robot est ...
		#else
			robot_id = DISABLE_WHO_AM_I;
		#endif
	}


	//@return	L'identifiant du robot sur lequel est branch�e la carte qui ex�cute ce code !
	robot_id_e QS_WHO_AM_I_get(void)
	{
		assert(found);		//Vous devez appeler QS_WHO_AM_I_find dans l'initialisation avant tout appel � QS_WHO_AM_I_get.
		return robot_id;
	}

	const char * QS_WHO_AM_I_get_name(void){
		static bool_e asked = FALSE;

		if(!initialized && !asked){
			asked = TRUE;
			return "#########Who am i non initialis�#########";
		}
		switch(robot_id){
			case BIG_ROBOT :
				return "Harry";
				break;
			case SMALL_ROBOT :
				return "Anne";
				break;
			case BEACON_EYE :
				return "Beacon_Eye";
				break;

			case NB_ROBOT_ID:		// Normalement impossible
				break;

			//Pas de default pour avoir un warning quand un cas de robot_id n'est pas g�r� dans ce switch
		}
		return "Robot inconnu";
	}

	bool_e I_AM_BIG(){
		return robot_id == BIG_ROBOT;
	}

	bool_e I_AM_SMALL(){
		return robot_id == SMALL_ROBOT;
	}



