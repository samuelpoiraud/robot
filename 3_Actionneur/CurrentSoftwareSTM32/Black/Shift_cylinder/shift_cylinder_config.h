/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : shift_cylinder_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 SHIFT_CYLINDER
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef SHIFT_CYLINDER_CONFIG_H
#define	SHIFT_CYLINDER_CONFIG_H

//Config
	#define SHIFT_CYLINDER_RX24_ASSER_TIMEOUT				600		// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define SHIFT_CYLINDER_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define SHIFT_CYLINDER_RX24_ASSER_POS_LARGE_EPSILON		28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define SHIFT_CYLINDER_RX24_MAX_TORQUE_PERCENT		    100		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define SHIFT_CYLINDER_RX24_MAX_TEMPERATURE				65		// Temp�rature maximale en degr�

//Position
	#define SHIFT_CYLINDER_RX24_IDLE_POS					512
	#define SHIFT_CYLINDER_RX24_CLOSE_POS			     	512	//en degr�
	#define SHIFT_CYLINDER_RX24_OPEN_POS					512	//en degr�

//Position initiale de l'RX24
	#define SHIFT_CYLINDER_RX24_INIT_POS					SHIFT_CYLINDER_RX24_IDLE_POS

//Extremum valeur
	#define SHIFT_CYLINDER_RX24_MIN_VALUE					0
	#define SHIFT_CYLINDER_RX24_MAX_VALUE					1024

#endif	/* SHIFT_CYLINDER_CONFIG_H */


