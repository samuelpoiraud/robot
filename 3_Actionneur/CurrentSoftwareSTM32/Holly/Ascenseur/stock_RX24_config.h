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

#define OFFSET			16	// Augmenter cette valeur permet de serrer moins
#define OFFSET_EN_V		0	//Augmenter cette valeur permet d'accroitre le phénomène en V


//Config
	#define STOCK_RX24_ASSER_TIMEOUT								1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define STOCK_RX24_ASSER_POS_EPSILON							6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define STOCK_RX24_ASSER_POS_LARGE_EPSILON						28				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define STOCK_RX24_MAX_TORQUE_PERCENT							70				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define PINCEMI_RX24_MAX_TEMPERATURE							60				// Température maximale en degré

//Position

// Etage 1
	// Pince droite, mord droit		ID:29
	#define STOCKR_RIGHT_F1_CLOSE									426
	#define STOCKR_RIGHT_F1_LOCK									493 + OFFSET
	#define STOCKR_RIGHT_F1_UNLOCK									540
	#define STOCKR_RIGHT_F1_OPEN									587

	// Pince droite, mord gauche	ID:30
	#define STOCKR_LEFT_F1_CLOSE									597
	#define STOCKR_LEFT_F1_LOCK										537 - OFFSET
	#define STOCKR_LEFT_F1_UNLOCK									495
	#define STOCKR_LEFT_F1_OPEN										466

	// Pince gauche, mord droit		ID:27
	#define STOCKL_RIGHT_F1_CLOSE									426
	#define STOCKL_RIGHT_F1_LOCK									490 + OFFSET
	#define STOCKL_RIGHT_F1_UNLOCK									540
	#define STOCKL_RIGHT_F1_OPEN									550

	// Pince gauche, mord gauche	ID:28
	#define STOCKL_LEFT_F1_CLOSE									597
	#define STOCKL_LEFT_F1_LOCK										524 - OFFSET
	#define STOCKL_LEFT_F1_UNLOCK									495
	#define STOCKL_LEFT_F1_OPEN										436


// Etage 2
	// Pince droite, mord droit		ID:33
	#define STOCKR_RIGHT_F2_CLOSE									426
	#define STOCKR_RIGHT_F2_LOCK									495 + OFFSET + OFFSET_EN_V
	#define STOCKR_RIGHT_F2_UNLOCK									540
	#define STOCKR_RIGHT_F2_OPEN									590

	// Pince droite, mord gauche	ID:34
	#define STOCKR_LEFT_F2_CLOSE									597
	#define STOCKR_LEFT_F2_LOCK										545 - OFFSET - OFFSET_EN_V
	#define STOCKR_LEFT_F2_UNLOCK									495
	#define STOCKR_LEFT_F2_OPEN										466

	// Pince gauche, mord droit		ID:31
	#define STOCKL_RIGHT_F2_CLOSE									370
	#define STOCKL_RIGHT_F2_LOCK									442 + OFFSET + OFFSET_EN_V
	#define STOCKL_RIGHT_F2_UNLOCK									487
	#define STOCKL_RIGHT_F2_OPEN									495

	// Pince gauche, mord gauche	ID:32
	#define STOCKL_LEFT_F2_CLOSE									597
	#define STOCKL_LEFT_F2_LOCK										543 - OFFSET - OFFSET_EN_V
	#define STOCKL_LEFT_F2_UNLOCK									495
	#define STOCKL_LEFT_F2_OPEN										447

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




