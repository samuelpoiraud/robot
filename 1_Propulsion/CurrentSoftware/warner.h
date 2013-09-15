/*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : warner.h
 *  Package : Propulsion
 *  Description : Le "warner" avertit de la position actuelle 
 *					une carte qui l'a demandé, lorsque que l'on est proche d'un angle où d'une position, 
 *  				ou qu'un délai périodique est écoulé.
 *  Auteur : Nirgal (2009)
 *  Version 201203
 */
 

#ifndef WARNER_H
	#define WARNER_H
	
	#include "QS/QS_all.h"
	#include "supervisor.h"

	//Cette fonction DOIT être appelée à l'initialisation.
	void WARNER_init(void);
	
	void WARNER_process_main(void);
	/////////////////////////////////////////////////////////////////////////////

	//Ces fonctions sont appelées pour activer l'avertisseur, aux positions indiquées...
	void WARNER_arm_teta(Sint16 teta);
	
	void WARNER_arm_x(Sint16 x);			//Position [mm]
	
	void WARNER_arm_y(Sint16 y);			//Position [mm]
	
	void WARNER_arm_timer(Uint16 duree); 	//Duree en [ms]

	void WARNER_arm_translation(Uint32 translation);	//distance en mm
	
	void WARNER_arm_rotation(Sint16 rotation);	//en rad4096

	void WARNER_brake(bool_e last_order);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	//cette fonction doit être appelée en IT
	//elle est le coeur de l'avertisseur, et lève les flags nécessaires	
	void WARNER_process_it(void);


	typedef Uint16 WARNER_state_t;


	#define WARNING_BRAKE					(0b00000000100000000)
	#define WARNING_ARRIVED					(0b00000001000000000)
	#define WARNING_CALIBRATION_FINISHED	(0b00000010000000000)
	#define WARNING_ERROR					(0b00000100000000000)
	//cette fonction sert à avertir, en envoyant des messages CAN d'asser position si nécessaire
	WARNER_state_t WARNER_get_warning_state(void);
	
	void WARNER_inform(WARNER_state_t new_warnings, SUPERVISOR_error_source_e new_error_source);


#endif //ndef WARNER_H
