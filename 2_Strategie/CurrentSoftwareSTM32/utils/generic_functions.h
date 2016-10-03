/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Generic_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions generiques pour piles de gestion
 *					des actionneurs
 *	Auteur : Jacen
 *	Version 20090111
 */

#include "../QS/QS_all.h"

#ifndef GENERIC_FUNCTIONS_H
	#define GENERIC_FUNCTIONS_H

	Uint8 wait_time(time32_t time, Uint8 in_progress, Uint8 success_state);

	// Fonction à utiliser en entrée et sortie de sub_action pour vérifier le bon placement du robot
	void on_turning_point();


	//Le point passé en paramètre permet-il les rotations ?
	bool_e is_possible_point_for_rotation(GEOMETRY_point_t * p);

	//Le point passé en paramètre permet-il une extraction ?
	bool_e is_possible_point_for_dodge(GEOMETRY_point_t * p);

	// Renvoi TRUE si nous somme dans la carré passé en paramètre est à l'intérieur (S'inverse automatique en fonction de la couleur)
	bool_e i_am_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2);

	// Renvoi TRUE si le point passé en paramètre est à l'intérieur du carré (S'inverse automatique en fonction de la couleur)
	bool_e is_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, GEOMETRY_point_t current);

#endif /* ndef GENERIC_FUNCTIONS_H */
