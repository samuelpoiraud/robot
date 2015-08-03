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

	typedef enum
	{
		HOLLY = 0,
		WOOD,
		BIG_ROBOT = HOLLY,
		SMALL_ROBOT = WOOD,
		BEACON_EYE
	}robot_id_e;

	#include "QS_all.h"

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

#endif /* QS_WHO_AM_I_H */
