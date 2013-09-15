 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : cpld.h
 *  Package : Propulsion
 *  Description : R�cup�ration des informations fournies par le CPLD.
 *  Auteur : Nirgal (2009) Inspir� des codes pr�c�dents.
 *  Version 201203
 */
 #ifndef _CPLD_H
	#define _CPLD_H
	
	#include "QS/QS_all.h"
		
	
	
	void CPLD_init(void);
	
	typedef enum
	{
		CPLD_LEFT_WHEEL = 0,
		CPLD_RIGHT_WHEEL
	}CPLD_wheel_e;

	Sint16 CPLD_get_count(CPLD_wheel_e wheel);



#endif	//def _CPLD_H
