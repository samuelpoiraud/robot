/*
 *  Club Robot ESEO 2008 - 2012
 *
 *  Fichier : Global_vars_types.h
 *  Package : Standard_Project
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen - Nirgal 2009
 *  Version 201203
 */

#ifndef QS_GLOBAL_VARS_H
	#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

/*
 *	inserez ici vos inclusions de fichers et vos definitions de types
 *	nécessaires à Global_vars.h
 */

	/*Position du robot et vitesse de dcp*/
	typedef struct {
			Sint16 x;		//[mm]
			Sint16 y;		//[mm]
			Sint16 teta;	//[rad/4096]
	}position_t;







	typedef enum {
			NO_ACKNOWLEDGE = 0,	 //Aucun acquittement demandé
			ACKNOWLEDGE_ASKED,   //Acquittement demandé
			BRAKE_ACKNOWLEDGED,  //Acquittement du freinage fourni
			ACKNOWLEDGE_CALIBRATION,
			ACKNOWLEDGE_SELFTEST,
			ACKNOWLEDGE_TRAJECTORY_FOR_TEST_COEFS,
			ACKNOWLEDGED,		 //Acquittement complet fourni
			INTERN_ACKNOWLEDGE	 //Acquittement interne pour fonction de la propulsion
	}acknowledge_e;

	typedef enum {
			NOT_NOW = 0,
			NOW
	}now_e;

	typedef enum {
			NOT_BORDER_MODE = 0,
			BORDER_MODE,
			BORDER_MODE_WITH_UPDATE_POSITION
	}border_mode_e;

	typedef enum {
			NO_MULTIPOINT = 0,
			MULTIPOINT
	}multipoint_e;

	typedef enum {
			NOT_RELATIVE = 0,
			RELATIVE
	}relative_e;




	  
	
		
	
	
	
