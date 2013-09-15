/*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : warner.h
 *  Package : Propulsion
 *  Description : Le "warner" avertit de la position actuelle 
 *					une carte qui l'a demand�, lorsque que l'on est proche d'un angle o� d'une position, 
 *  				ou qu'un d�lai p�riodique est �coul�.
 *  Auteur : Nirgal (2009)
 *  Version 201203
 */
 

#ifndef WARNER_H
	#define WARNER_H
	
	#include "QS/QS_all.h"
	#include "supervisor.h"

	//Cette fonction DOIT �tre appel�e � l'initialisation.
	void WARNER_init(void);
	
	void WARNER_process_main(void);
	/////////////////////////////////////////////////////////////////////////////

	//Ces fonctions sont appel�es pour activer l'avertisseur, aux positions indiqu�es...
	void WARNER_arm_teta(Sint16 teta);
	
	void WARNER_arm_x(Sint16 x);			//Position [mm]
	
	void WARNER_arm_y(Sint16 y);			//Position [mm]
	
	void WARNER_arm_timer(Uint16 duree); 	//Duree en [ms]

	void WARNER_arm_translation(Uint32 translation);	//distance en mm
	
	void WARNER_arm_rotation(Sint16 rotation);	//en rad4096

	void WARNER_brake(bool_e last_order);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	//cette fonction doit �tre appel�e en IT
	//elle est le coeur de l'avertisseur, et l�ve les flags n�cessaires	
	void WARNER_process_it(void);


	typedef Uint16 WARNER_state_t;


	#define WARNING_BRAKE					(0b00000000100000000)
	#define WARNING_ARRIVED					(0b00000001000000000)
	#define WARNING_CALIBRATION_FINISHED	(0b00000010000000000)
	#define WARNING_ERROR					(0b00000100000000000)
	//cette fonction sert � avertir, en envoyant des messages CAN d'asser position si n�cessaire
	WARNER_state_t WARNER_get_warning_state(void);
	
	void WARNER_inform(WARNER_state_t new_warnings, SUPERVISOR_error_source_e new_error_source);


#endif //ndef WARNER_H
