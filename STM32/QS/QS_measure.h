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

#define CENTRAL_HEARTH_RADIUS			150		//Rayon du foyer centrale [mm]
#define RADIUS_TORCH					80		//Rayon d'une torche [mm]

#define LARGEUR_LABIUM					235		//Largeur du labium par rapport au centre du robot [mm]
#define ELOIGNEMENT_ARBRE				(LARGEUR_LABIUM+100) // Distance à laquel on doit se tenir par rapport à l'arbre [mm]
#define ELOIGNEMENT_POSE_BAC_FRUIT		470		// Distance à laquel on doit se tenir par rapport au bac à fruit [mm]



// Variable commune à la stratégie et la propulsion pour la gestion de l'évitement

#define	FOE_SIZE										400		//taille supposée de l'adversaire
#define MARGE_COULOIR_EVITEMENT_STATIC_BIG_ROBOT		(300 + 100)
#define MARGE_COULOIR_EVITEMENT_STATIC_SMALL_ROBOT		(240 + 100)
#define DISTANCE_EVITEMENT_STATIC						500

#define SMALL_ROBOT_ACCELERATION_NORMAL					468*2	//Réglage d'accélération de la propulsion : 625 	mm/sec = 64 	[mm/4096/5ms/5ms]
#define BIG_ROBOT_ACCELERATION_NORMAL					937*2	//Réglage d'accélération de la propulsion : 1094 	mm/sec = 112 	[mm/4096/5ms/5ms]
#define SMALL_ROBOT_RESPECT_DIST_MIN					400		//Distance à laquelle on se tient d'un adversaire [mm]
#define BIG_ROBOT_RESPECT_DIST_MIN						550		//Distance à laquelle on se tient d'un adversaire [mm]
#define SMALL_ROBOT_DIST_MIN_SPEED_SLOW					550		//Distance à laquelle on ralentie s'il ya un adversaire [mm]
#define BIG_ROBOT_DIST_MIN_SPEED_SLOW					750		//Distance à laquelle on ralentie s'il ya un adversaire [mm]

#endif /* ndef QS_MEASURE_H */
