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
	#define STOCK_RX24_MAX_TORQUE_PERCENT							30				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define PINCEMI_RX24_MAX_TEMPERATURE							60				// Température maximale en degré

//Position

// Etage 1
	// Pince droite, mord droit		ID:29
	#define STOCKR_RIGHT_F1_CLOSE									426
	#define STOCKR_RIGHT_F1_LOCK									493
	#define STOCKR_RIGHT_F1_UNLOCK									525
	#define STOCKR_RIGHT_F1_OPEN									538

	// Pince droite, mord gauche	ID:30
	#define STOCKR_LEFT_F1_CLOSE									597
	#define STOCKR_LEFT_F1_LOCK										537
	#define STOCKR_LEFT_F1_UNLOCK									500
	#define STOCKR_LEFT_F1_OPEN										492

	// Pince gauche, mord droit		ID:27
	#define STOCKL_RIGHT_F1_CLOSE									426
	#define STOCKL_RIGHT_F1_LOCK									474
	#define STOCKL_RIGHT_F1_UNLOCK									525
	#define STOCKL_RIGHT_F1_OPEN									538

	// Pince gauche, mord gauche	ID:28
	#define STOCKL_LEFT_F1_CLOSE									597
	#define STOCKL_LEFT_F1_LOCK										524
	#define STOCKL_LEFT_F1_UNLOCK									500
	#define STOCKL_LEFT_F1_OPEN										492


// Etage 2
	// Pince droite, mord droit		ID:33
	#define STOCKR_RIGHT_F2_CLOSE									426
	#define STOCKR_RIGHT_F2_LOCK									495
	#define STOCKR_RIGHT_F2_UNLOCK									525
	#define STOCKR_RIGHT_F2_OPEN									538

	// Pince droite, mord gauche	ID:34
	#define STOCKR_LEFT_F2_CLOSE									597
	#define STOCKR_LEFT_F2_LOCK										545
	#define STOCKR_LEFT_F2_UNLOCK									500
	#define STOCKR_LEFT_F2_OPEN										492

	// Pince gauche, mord droit		ID:31
	#define STOCKL_RIGHT_F2_CLOSE									370
	#define STOCKL_RIGHT_F2_LOCK									442
	#define STOCKL_RIGHT_F2_UNLOCK									472
	#define STOCKL_RIGHT_F2_OPEN									482

	// Pince gauche, mord gauche	ID:32
	#define STOCKL_LEFT_F2_CLOSE									597
	#define STOCKL_LEFT_F2_LOCK										543
	#define STOCKL_LEFT_F2_UNLOCK									500
	#define STOCKL_LEFT_F2_OPEN										492

//Position initiale du RX24
	#define STOCKR_RIGHT_F1_INIT_POS								STOCKR_RIGHT_F1_OPEN
	#define STOCKR_LEFT_F1_INIT_POS									STOCKR_LEFT_F1_OPEN
	#define STOCKL_RIGHT_F1_INIT_POS								STOCKL_RIGHT_F1_OPEN
	#define STOCKL_LEFT_F1_INIT_POS									STOCKL_LEFT_F1_OPEN

	#define STOCKR_RIGHT_F2_INIT_POS								STOCKR_RIGHT_F2_OPEN
	#define STOCKR_LEFT_F2_INIT_POS									STOCKR_LEFT_F2_OPEN
	#define STOCKL_RIGHT_F2_INIT_POS								STOCKL_RIGHT_F2_OPEN
	#define STOCKL_LEFT_F2_INIT_POS									STOCKL_LEFT_F2_OPEN


//Extremum valeur
	#define STOCK_RX24_MIN_VALUE									0
	#define STOCK_RX24_MAX_VALUE									1024


#endif	/* STOCK_RX24_CONFIG_H */




