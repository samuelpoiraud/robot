 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : corrector.h
 *  Package : Propulsion
 *  Description : Gestion du correcteur PD.
 *  Auteur : Nirgal 2009, inspiré par le code de Val' 2007
 *  Version 201203
 */
#ifndef _CORRECTOR_H
	#define _CORRECTOR_H
	
	#include "QS/QS_all.h"
	
	void CORRECTOR_init(void);
	
	typedef enum
	{
		CORRECTOR_COEF_KP_TRANSLATION = 0,
		CORRECTOR_COEF_KD_TRANSLATION,
		CORRECTOR_COEF_KV_TRANSLATION,
		CORRECTOR_COEF_KA_TRANSLATION,
		CORRECTOR_COEF_KP_ROTATION,
		CORRECTOR_COEF_KD_ROTATION,
		CORRECTOR_COEF_KV_ROTATION,
		CORRECTOR_COEF_KA_ROTATION,
		CORRECTOR_NUMBER_COEFS
	}CORRECTOR_coef_e;

	typedef enum
	{
		CORRECTOR_ENABLE = 0,
		CORRECTOR_ROTATION_ONLY,
		CORRECTOR_TRANSLATION_ONLY,
		CORRECTOR_DISABLE
	}corrector_e;

	  
	void CORRECTOR_set_coef(CORRECTOR_coef_e coef, Sint32 value);
	
		//fonction d'activation/désactivation du correcteur PD.
	//Rotation = FALSE -> désactive la correction en rotation
	//Translation = FALSE -> désactive la correction en translation
	//et vice versa...
		#define NO_CHANGE (-1)
	void CORRECTOR_PD_enable(corrector_e corrector);

	bool_e CORRECTOR_PD_enable_get_rotation(void);


	bool_e CORRECTOR_PD_enable_get_translation(void);

	void CORRECTOR_update(void);
	
#endif //def _CORRECTOR_H
