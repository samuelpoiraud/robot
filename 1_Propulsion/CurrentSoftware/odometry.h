 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : odometry.h
 *  Package : Propulsion
 *  Description : Odométrie du mobile en mouvement.
 *					Calcul de la position (x,y,teta), d'après les mouvements mesurés des encodeurs.
 *  Auteur : Val' 2007 - modifié par Nirgal 2009
 *  Version 201203
 */

#ifndef _ODOMETRY_H
	#define _ODOMETRY_H

	#include "QS/QS_all.h"
	#include "QS/QS_CANmsgList.h"

	void ODOMETRY_init(void);

	void ODOMETRY_update(void);

	Sint16 get_calibration_backward_distance(void);

	void ODOMETRY_set_coef(PROPULSION_coef_e coef, Sint32 value);
	
	Sint32 ODOMETRY_get_coef(PROPULSION_coef_e coef);
	
	color_e ODOMETRY_get_color();

	//Permet d'imposer une positon au robot... utile pour les tests !!!
	//Par exemple pour les tests de trajectoires rectangles sans se prendre la tete !
	void ODOMETRY_set(Sint16 x, Sint16 y, Sint16 teta);

	//set la position de départ selon la couleur demandée
	void ODOMETRY_set_color(color_e color);

	//met a jour la position en considérant que l'on est contre un bord du terrain
	//(L'angle devient un multiple de PI/2 le plus logique et la distance en x OU en y est la distance entre l'arrière du robot et le centre.)
	void ODOMETRY_correct_with_border(way_e way);


#endif //def _ODOMETRY_H
