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

	/** @brief  Cette fonction doit �tre appel�e lors de l'initialisation.
	 *  @pre    Le fond de panier sur lequel la carte est branch� doit �tre cabl� conform�ment � "_PORTS.xlsx"
	 *  @post   Le module QS_WHO_AM_I sait alors sur quel robot la carte est branch�e
	 */
	void QS_WHO_AM_I_find(void);

	/** @return L'identifiant du robot sur lequel est branch�e la carte qui ex�cute ce code ! */
	robot_id_e QS_WHO_AM_I_get(void);

	/**	@brief	Cette fonction permet de retourner le nom du robot ou un code d'erreur au premi�re appel de celle-ci
	 *	@return	Une chaine de caract�re du nom du robot ou vide si celui ci n'est pas initialis� ou d�sactiv�
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
		Exemple de d�finition pour global_config, conforme � _PORTS.xlsx � l'heure o� j'�cris ces lignes.
		#define LAT_ROBOT_ID_OUTPUT		LATEbits.LATE7
		#define TRIS_ROBOT_ID_OUTPUT	TRISEbits.TRISE7
		#define PORT_ROBOT_ID_INPUT		PORTEbits.RE5
		#define TRIS_ROBOT_ID_INPUT		TRISEbits.TRISE5

	*/
	/*
	#ifndef	LAT_ROBOT_ID_OUTPUT
		#warning "vous devez d�finir LAT_ROBOT_ID_OUTPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branch�e la carte facto qui ex�cute ce code"
	#endif
	#ifndef	TRIS_ROBOT_ID_OUTPUT
		#warning "vous devez d�finir TRIS_ROBOT_ID_OUTPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branch�e la carte facto qui ex�cute ce code"
	#endif

	#ifndef	PORT_ROBOT_ID_INPUT
		#warning "vous devez d�finir PORT_ROBOT_ID_INPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branch�e la carte facto qui ex�cute ce code"
	#endif
	#ifndef	TRIS_ROBOT_ID_INPUT
		#warning "vous devez d�finir TRIS_ROBOT_ID_INPUT : le port de sortie permettant au module WHO_AM_I de trouver sur quel robot est branch�e la carte facto qui ex�cute ce code"
	#endif*/

#endif /* QS_WHO_AM_I_H */
