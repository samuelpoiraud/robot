 /*
 *  Club Robot ESEO 2010 - 2012
 *
 *  Fichier : pilot.h
 *  Package : Propulsion
 *  Description : Gestion du point fictif, et suivi de la progression vers l'objectif.
 *					Ce module est également la partie la plus compliquée du code de propulsion... et le fruit de nombreuses heures de réflexions.
 *  Auteur : Nirgal 2010
 *  Version 201203
 */
 
#ifndef _PILOT_H
	#define _PILOT_H

		#include "QS/QS_all.h"

	
	typedef enum
	{
		PILOT_ACCELERATION_NORMAL,
		PILOT_ACCELERATION_ROTATION_TRANSLATION,
		PILOT_TRANSLATION_SPEED_LIGHT,
		PILOT_TRANSLATION_SPEED_MAX,
		PILOT_TRANSLATION_SPEED_LOW,
		PILOT_TRANSLATION_SPEED_VERY_LOW,
		PILOT_TRANSLATION_SPEED_SNAIL,
		PILOT_ROTATION_SPEED_LIGHT,
		PILOT_ROTATION_SPEED_MAX,
		PILOT_ROTATION_SPEED_LOW,
		PILOT_ROTATION_SPEED_VERY_LOW,
		PILOT_ROTATION_SPEED_SNAIL,
		PILOT_NUMBER_COEFS	//Nombre de coefficients.
	}PILOT_coef_e;

	
	void PILOT_init(void);

	Sint32 PILOT_get_coef(PILOT_coef_e id);
	
	void PILOT_process_it(void);

	void PILOT_set_speed(speed_e speed);

	void PILOT_referential_init(void);

	void PILOT_referential_reset(void);
	void PILOT_new_order(void);


	Sint32 PILOT_get_destination_rotation(void);
	Sint32 PILOT_get_destination_translation(void);
	void PILOT_set_destination_rotation(Sint32 dest);
	void PILOT_set_destination_translation(Sint32 dest);

	void PILOT_set_extra_braking_rotation(bool_e enable);
	void PILOT_set_extra_braking_translation(bool_e enable);

#endif  //def _PILOT_H
