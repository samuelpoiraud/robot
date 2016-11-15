/*  Club Robot ESEO 2016 - 2017
 *	BIG
 *
 *	Fichier : billix_test_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur BILLIX_TEST
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */

#ifndef BILLIX_TEST_CONFIG_H
#define	BILLIX_TEST_CONFIG_H

//Config
	#define BILLIX_TEST_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define BILLIX_TEST_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define BILLIX_TEST_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define BILLIX_TEST_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define BILLIX_TEST_AX12_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�

//Position
	#define BILLIX_TEST_AX12_IDLE_POS					512
	#define BILLIX_TEST_AX12_LOCK_POS				    512		//en degr�
	#define BILLIX_TEST_AX12_UNLOCK_POS					1023	//en degr�

//Position initiale de l'AX12
	#define BILLIX_TEST_AX12_INIT_POS					BILLIX_TEST_AX12_IDLE_POS

//Extremum valeur
	#define BILLIX_TEST_AX12_MIN_VALUE					0
	#define BILLIX_TEST_AX12_MAX_VALUE					1024

#endif	/* BILLIX_TEST_CONFIG_H */


