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
	//� regler avec �l�ments m�caniques de blocages extr�mement parall�les � l'axe des codeurs !

// BIG
#define BIG_ROBOT_WIDTH									312		//Largeur du gros robot [mm]
#define BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE		146		//Distance entre le 'centre' du robot et l'arri�re en calage [mm]
#define BIG_CALIBRATION_FORWARD_BORDER_DISTANCE			100		//Distance entre le 'centre' du robot et l'avant en calage [mm]

// SMALL
#define SMALL_ROBOT_WIDTH								190		//Largeur du petit robot [mm]
#define SMALL_CALIBRATION_BACKWARD_BORDER_DISTANCE		96		//Distance entre le 'centre' du robot et l'arri�re en calage [mm]
#define SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE		95		//Distance entre le 'centre' du robot et l'avant en calage [mm]


// Variable commune � la strat�gie et la propulsion pour la gestion de l'�vitement

#define	FOE_SIZE										400		//taille suppos�e de l'adversaire
#define MARGE_COULOIR_EVITEMENT_STATIC_BIG_ROBOT		(300 + 70)
#define MARGE_COULOIR_EVITEMENT_STATIC_SMALL_ROBOT		(240 + 70)
#define DISTANCE_EVITEMENT_STATIC						500
#define DISTANCE_EVITEMENT_ROTATION						400		// [mm] Si un adversaire est proche d'au moins DISTANCE_EVITEMENT_ROTATION alors on force une rotation douce

#define SMALL_ROBOT_ACCELERATION_NORMAL					(468*2)	//R�glage d'acc�l�ration de la propulsion : 625 	mm/sec = 64 	[mm/4096/5ms/5ms]
#define BIG_ROBOT_ACCELERATION_NORMAL					(937*2)	//R�glage d'acc�l�ration de la propulsion : 1094 	mm/sec = 112 	[mm/4096/5ms/5ms]
#define SMALL_ROBOT_RESPECT_DIST_MIN					400		//Distance � laquelle on se tient d'un adversaire [mm]
#define BIG_ROBOT_RESPECT_DIST_MIN						500		//Distance � laquelle on se tient d'un adversaire [mm]
#define SMALL_ROBOT_DIST_MIN_SPEED_SLOW					550		//Distance � laquelle on ralentie s'il ya un adversaire [mm]
#define BIG_ROBOT_DIST_MIN_SPEED_SLOW					750		//Distance � laquelle on ralentie s'il ya un adversaire [mm]


#endif /* ndef QS_MEASURE_H */
