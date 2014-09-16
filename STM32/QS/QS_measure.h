/*
 *	Club Robot ESEO 2014 - 2015
 *
 *	Fichier : QS_measure.h
 *	Package : Qualite Soft
 *	Description : Header regroupant l'ensemble des mesures du robots / terrains
 */

#ifndef QS_MEASURE_H
	#define QS_MEASURE_H


#define SMALL_ROBOT_WIDTH				200		//Largeur du petit robot [mm]
#define BIG_ROBOT_WIDTH					300		//Largeur du gros robot [mm]
#define	FOE_SIZE						400		//taille supposée de l'adversaire
#define MARGE_COULOIR_EVITEMENT_STATIC_BIG_ROBOT	(300 + 100)
#define MARGE_COULOIR_EVITEMENT_STATIC_SMALL_ROBOT		(240 + 100)
#define DISTANCE_EVITEMENT_STATIC		500

#define SMALL_ROBOT_ACCELERATION_NORMAL	468*2	//Réglage d'accélération de la propulsion : 625 	mm/sec = 64 	[mm/4096/5ms/5ms]
#define BIG_ROBOT_ACCELERATION_NORMAL	937*2	//Réglage d'accélération de la propulsion : 1094 	mm/sec = 112 	[mm/4096/5ms/5ms]
#define SMALL_ROBOT_RESPECT_DIST_MIN	400		//Distance à laquelle on se tient d'un adversaire [mm]
#define BIG_ROBOT_RESPECT_DIST_MIN		700		//Distance à laquelle on se tient d'un adversaire [mm]


#endif /* ndef QS_MEASURE_H */
