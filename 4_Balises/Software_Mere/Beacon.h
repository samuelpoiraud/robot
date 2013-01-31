/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Beacon.h
 *	Package : Balises
 *	Description : Exploitation des balises
 *	Auteur : Nirgal
 *	Version 200905
 */
 
#ifndef BEACON_H
	#define BEACON_H
	#include "QS\QS_all.h"
	
		#include "QS/QS_timer.h"
		#include "QS/QS_CANmsgList.h"
		#include "QS/QS_adc.h"
		#include "QS/QS_can_over_uart.h"
		#include "QS/QS_extern_it.h"
		#include "Tableau.h"
		#include "Beacon_it.h"
		
	void Beacon_init(void);

	void process_envoi(void);
	void process_it(void);
	void initialiser_instants_detections_balises(void);
	void calcul_angles(void);
	void calcul_xy(void);
	void process_beacon(void);
	bool_e detection_erreur_angles(void);
	bool_e detection_erreur_xy(void);
	
#endif /* ndef BEACON_H */
