/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : stock_RX24_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ensemble des RX24 du stock de Holly
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef STOCK_RX24_CONFIG_H
#define	STOCK_RX24_CONFIG_H

//Config
	#define STOCK_RX24_ASSER_TIMEOUT								1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define STOCK_RX24_ASSER_POS_EPSILON							6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define STOCK_RX24_ASSER_POS_LARGE_EPSILON						28				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define STOCK_RX24_MAX_TORQUE_PERCENT							100				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position

// Etage 1
	// Pince droite, mord droit
	#define STOCKR_RIGHT_F1_CLOSE									512
	#define STOCKR_RIGHT_F1_LOCK									512
	#define STOCKR_RIGHT_F1_UNLOCK									512
	#define STOCKR_RIGHT_F1_OPEN									512

	// Pince droite, mord gauche
	#define STOCKR_LEFT_F1_CLOSE									512
	#define STOCKR_LEFT_F1_LOCK										512
	#define STOCKR_LEFT_F1_UNLOCK									512
	#define STOCKR_LEFT_F1_OPEN										512

	// Pince gauche, mord droit
	#define STOCKL_RIGHT_F1_CLOSE									512
	#define STOCKL_RIGHT_F1_LOCK									512
	#define STOCKL_RIGHT_F1_UNLOCK									512
	#define STOCKL_RIGHT_F1_OPEN									512

	// Pince gauche, mord gauche
	#define STOCKL_LEFT_F1_CLOSE									401
	#define STOCKL_LEFT_F1_LOCK										450
	#define STOCKL_LEFT_F1_UNLOCK									512
	#define STOCKL_LEFT_F1_OPEN										596


// Etage 2
	// Pince droite, mord droit
	#define STOCKR_RIGHT_F2_CLOSE									STOCKR_RIGHT_F1_CLOSE
	#define STOCKR_RIGHT_F2_LOCK									STOCKR_RIGHT_F1_LOCK
	#define STOCKR_RIGHT_F2_UNLOCK									STOCKR_RIGHT_F1_UNLOCK
	#define STOCKR_RIGHT_F2_OPEN									STOCKR_RIGHT_F1_OPEN

	// Pince droite, mord gauche
	#define STOCKR_LEFT_F2_CLOSE									STOCKR_LEFT_F1_CLOSE
	#define STOCKR_LEFT_F2_LOCK										STOCKR_LEFT_F1_LOCK
	#define STOCKR_LEFT_F2_UNLOCK									STOCKR_LEFT_F1_UNLOCK
	#define STOCKR_LEFT_F2_OPEN										STOCKR_LEFT_F1_OPEN

	// Pince gauche, mord droit
	#define STOCKL_RIGHT_F2_CLOSE									STOCKL_RIGHT_F1_CLOSE
	#define STOCKL_RIGHT_F2_LOCK									STOCKL_RIGHT_F1_LOCK
	#define STOCKL_RIGHT_F2_UNLOCK									STOCKL_RIGHT_F1_UNLOCK
	#define STOCKL_RIGHT_F2_OPEN									STOCKL_RIGHT_F1_OPEN

	// Pince gauche, mord gauche
	#define STOCKL_LEFT_F2_CLOSE									STOCKL_LEFT_F1_CLOSE
	#define STOCKL_LEFT_F2_LOCK										STOCKL_LEFT_F1_LOCK
	#define STOCKL_LEFT_F2_UNLOCK									STOCKL_LEFT_F1_UNLOCK
	#define STOCKL_LEFT_F2_OPEN										STOCKL_LEFT_F1_OPEN

//Position initiale du RX24
	#define STOCKR_RIGHT_F1_INIT_POS								STOCKR_RIGHT_F1_CLOSE
	#define STOCKR_LEFT_F1_INIT_POS									STOCKR_LEFT_F1_CLOSE
	#define STOCKL_RIGHT_F1_INIT_POS								STOCKL_RIGHT_F1_CLOSE
	#define STOCKL_LEFT_F1_INIT_POS									STOCKL_LEFT_F1_CLOSE

	#define STOCKR_RIGHT_F2_INIT_POS								STOCKR_RIGHT_F2_CLOSE
	#define STOCKR_LEFT_F2_INIT_POS									STOCKR_LEFT_F2_CLOSE
	#define STOCKL_RIGHT_F2_INIT_POS								STOCKL_RIGHT_F2_CLOSE
	#define STOCKL_LEFT_F2_INIT_POS									STOCKL_LEFT_F2_CLOSE


//Extremum valeur
	#define STOCK_RX24_MIN_VALUE									400
	#define STOCK_RX24_MAX_VALUE									600


#endif	/* STOCK_RX24_CONFIG_H */




