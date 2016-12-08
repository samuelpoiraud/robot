/*
 *  Club Robot ESEO 2016 - 2017
 *
 *  Fichier : stmpe811.h
 *  Package : Beacon Eye
 *  Description : Driver STMPE811 de l'écran tactile
 *  Auteur : Guillaume Berland inspiré du code d'Arnaud Guilmet
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20161208
 */


#ifndef STMPE811_H
	#define STMPE811_H
	#include "../../QS/QS_all.h"

	typedef enum{
		STMPE811_COORDINATE_RAW,
		STMPE811_COORDINATE_SCREEN_RELATIVE
	}STMPE811_coordinateMode_e;

#endif //STMPE811_H
