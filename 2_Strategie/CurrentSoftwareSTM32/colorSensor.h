/*
 *	Club Robot ESEO 2016 - 2017
 *	Harry & Anne
 *
 *	Fichier : mosfetBoard.h
 *	Package : Actionneur
 *	Description : Communication avec la carte mosfet
 *  Auteurs : Arnaud
 *  Version 20110225
 */

#ifndef COLOR_SENSOR_H
	#define COLOR_SENSOR_H

	#include "QS/QS_all.h"

	#ifdef USE_I2C_COLOR_SENSOR

		typedef enum{
			COLOR_SENSOR_NONE,
			COLOR_SENSOR_BLUE,
			COLOR_SENSOR_WHITE,
			COLOR_SENSOR_YELLOW
		}COLOR_SENSOR_color_e;

		void COLOR_SENSOR_init();
		void COLOR_SENSOR_processMain();

		// Non fonctionnel mais peut fonctionner si on se prend la tête quelque minutes
		bool_e COLOR_SENSOR_getPresence();

		COLOR_SENSOR_color_e COLOR_SENSOR_getColor();

	#endif

#endif /* ndef COLOR_SENSOR_H */

