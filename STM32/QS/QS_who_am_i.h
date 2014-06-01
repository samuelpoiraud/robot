/*
 *	Club Robot ESEO 2013
 *	Tiny & Krusty
 *
 *	Fichier : QS_who_am_i.h
 *	Package : Qualite Software
 *	Description :	Utilise des ports de la carte facto pour identifier
 *					le robot sur lequel la carte se trouve.
 *	Auteur : Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201302
 */

#ifndef QS_WHO_AM_I_H
#define QS_WHO_AM_I_H
#include "QS_all.h"

	typedef enum
	{
		KRUSTY = 0,
		TINY,
		PIERRE = KRUSTY,
		GUY = TINY,
		SMALL_ROBOT = TINY,
		BIG_ROBOT = KRUSTY
	}robot_id_e;

	/** @brief  Cette fonction doit être appelée lors de l'initialisation.
	 *  @pre    Le fond de panier sur lequel la carte est branché doit être cablé conformément à "_PORTS.xlsx"
	 *  @post   Le module QS_WHO_AM_I sait alors sur quel robot la carte est branchée
	 */
	void QS_WHO_AM_I_find(void);

	/** @return L'identifiant du robot sur lequel est branchée la carte qui exécute ce code ! */
	robot_id_e QS_WHO_AM_I_get(void);

	/**	@brief	Cette fonction permet de retourner le nom du robot ou un code d'erreur au première appel de celle-ci
	 *	@return	Une chaine de caractère du nom du robot ou vide si celui ci n'est pas initialisé ou désactivé
	  */
	const char * QS_WHO_AM_I_get_name(void);

	/**
	 * @brief Cette fonction permet de savoir si on est le gros robot
	 * @return un boolean de si on est le gros robot ou pas
	 */
	bool_e I_AM_BIG();

	/**
	 * @brief Cette fonction permet de savoir si on est le petit robot
	 * @return un boolean de si on est le petit robot ou pas
	 */
	bool_e I_AM_SMALL();


	/*
		Exemple de définition pour global_config, conforme à _PORTS.xlsx à l'heure où j'écris ces lignes.
		#define LAT_ROBOT_ID_OUTPUT		LATEbits.LATE7
		#define TRIS_ROBOT_ID_OUTPUT	TRISEbits.TRISE7
		#define PORT_ROBOT_ID_INPUT		PORTEbits.RE5
		#define TRIS_ROBOT_ID_INPUT		TRISEbits.TRISE5

	*/
	/*
	#ifndef	LAT_ROBOT_ID_OUTPUT
		#warning "vous devez définir LAT_ROBOT_ID_OUTPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branchée la carte facto qui exécute ce code"
	#endif
	#ifndef	TRIS_ROBOT_ID_OUTPUT
		#warning "vous devez définir TRIS_ROBOT_ID_OUTPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branchée la carte facto qui exécute ce code"
	#endif

	#ifndef	PORT_ROBOT_ID_INPUT
		#warning "vous devez définir PORT_ROBOT_ID_INPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branchée la carte facto qui exécute ce code"
	#endif
	#ifndef	TRIS_ROBOT_ID_INPUT
		#warning "vous devez définir TRIS_ROBOT_ID_INPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branchée la carte facto qui exécute ce code"
	#endif*/

#endif /* QS_WHO_AM_I_H */
