 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : odometry.h
 *  Package : Propulsion
 *  Description : Odom�trie du mobile en mouvement.
 *					Calcul de la position (x,y,teta), d'apr�s les mouvements mesur�s des encodeurs.
 *  Auteur : Val' 2007 - modifi� par Nirgal 2009
 *  Version 201203
 */

#ifndef _ODOMETRY_H
	#define _ODOMETRY_H

	#include "QS/QS_all.h"
	#include "QS/QS_CANmsgList.h"

	void ODOMETRY_init(void);

	void ODOMETRY_update_1ms(void);
	void ODOMETRY_update_5ms(void);

	Sint32 ODOMETRY_get_65536_x(void);
	Sint32 ODOMETRY_get_65536_y(void);
	Sint32 ODOMETRY_get_teta22(void);

	Sint16 get_calibration_backward_distance(void);
	Sint16 get_calibration_forward_distance(void);

	void ODOMETRY_set_coef(PROPULSION_coef_e coef, Sint32 value);

	Sint32 ODOMETRY_get_coef(PROPULSION_coef_e coef);

	color_e ODOMETRY_get_color();


	Sint32 ODOMETRY_get_max_total_teta();	// PI4096
	Sint32 ODOMETRY_get_total_teta();		// PI4096
	Uint32 ODOMETRY_get_total_dist();		// mm

	//Permet d'imposer une positon au robot... utile pour les tests !!!
	//Par exemple pour les tests de trajectoires rectangles sans se prendre la tete !
	void ODOMETRY_set(Sint16 x, Sint16 y, Sint16 teta);

	// Reset la position � la position initiale
	void ODOMETRY_set_init_pos();

	//set la position de d�part selon la couleur demand�e
	void ODOMETRY_set_color(color_e color);

	//met a jour la position en consid�rant que l'on est contre un bord du terrain
	//(L'angle devient un multiple de PI/2 le plus logique et la distance en x OU en y est la distance entre l'arri�re du robot et le centre.)
	void ODOMETRY_correct_with_border(way_e way);

	void ODOMETRY_referential_reset(void);

	// Update position according to IHM change (Utilis� en 2017 pour la bascule)
	void ODOMETRY_update_pos_according_to_ihm(CAN_msg_t *msg);

#endif //def _ODOMETRY_H
