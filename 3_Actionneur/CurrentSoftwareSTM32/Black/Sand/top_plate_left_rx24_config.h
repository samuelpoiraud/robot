/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : fich_brush_back_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur rx24 TOP_PLATE_LEFT
 *  Auteur : Julien
 *  Version 2016
 *  Robot : BIG
 */

#ifndef TOP_PLATE_LEFT_CONFIG_H
#define	TOP_PLATE_LEFT_CONFIG_H

//Config
	#define TOP_PLATE_LEFT_AX12_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define TOP_PLATE_LEFT_AX12_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define TOP_PLATE_LEFT_AX12_RX24_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define TOP_PLATE_LEFT_AX12_RX24_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define TOP_PLATE_LEFT_AX12_RX24_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�

//Position
	#define TOP_PLATE_LEFT_AX12_RX24_IDLE_POS					380
	#define TOP_PLATE_LEFT_AX12_RX24_CLOSE_POS			     	380	//en degr�
	#define TOP_PLATE_LEFT_AX12_RX24_OPEN_POS					210		//en degr�

//Position initiale de l'AX12_RX24
	#define TOP_PLATE_LEFT_AX12_RX24_INIT_POS					TOP_PLATE_LEFT_AX12_RX24_IDLE_POS

//Extremum valeur
	#define TOP_PLATE_LEFT_AX12_RX24_MIN_VALUE					0
	#define TOP_PLATE_LEFT_AX12_RX24_MAX_VALUE					1024

#endif	/* TOP_PLATE_LEFT_CONFIG_H */


