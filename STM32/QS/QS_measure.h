/*
 *	Club Robot ESEO 2014 - 2015
 *
 *	Fichier : QS_measure.h
 *	Package : Qualite Soft
 *	Description : Header regroupant l'ensemble des mesures du robots / terrains
 */

#ifndef QS_MEASURE_H
	#define QS_MEASURE_H

// Mensuration des robots
	//à regler avec éléments mécaniques de blocages extrèmement parallèles à l'axe des codeurs !

// BIG
#define BIG_ROBOT_WIDTH									292  //310		//Largeur du gros robot [mm]
#define BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE		96		//Distance entre le 'centre' du robot et l'arrière en calage [mm]
#define BIG_CALIBRATION_FORWARD_BORDER_DISTANCE			146		//Distance entre le 'centre' du robot et l'avant en calage [mm]

// SMALL
#define SMALL_ROBOT_WIDTH								190		//Largeur du petit robot [mm]
#define SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE		90		//Distance entre le 'centre' du robot et l'arrière en calage [mm]
#define SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE		92		//Distance entre le 'centre' du robot et l'avant en calage [mm]


// Variable commune à la stratégie et la propulsion pour la gestion de l'évitement

#define	FOE_SIZE										400		//taille supposée de l'adversaire
#define SMALL_ROBOT_RESPECT_DIST_MIN					350		//Distance à laquelle on se tient d'un adversaire [mm]
#define BIG_ROBOT_RESPECT_DIST_MIN						400		//Distance à laquelle on se tient d'un adversaire [mm]
#define SMALL_ROBOT_DIST_MIN_SPEED_SLOW					550		//Distance à laquelle on ralentie s'il ya un adversaire [mm]
#define BIG_ROBOT_DIST_MIN_SPEED_SLOW					750		//Distance à laquelle on ralentie s'il ya un adversaire [mm]
#define DISTANCE_EVITEMENT_ROTATION						400		//Distance à laquelle on ralentie en rotation s'il ya un adversaire [mm]

#endif /* ndef QS_MEASURE_H */
