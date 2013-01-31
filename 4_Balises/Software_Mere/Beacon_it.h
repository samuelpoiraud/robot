/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Beacon_it.h
 *	Package : Balises
 *	Description : Exploitation des balises
 *	Auteur : Nirgal
 *	Version 200905
 */
 
#ifndef BEACON_IT_H
	#define BEACON_IT_H
	#include "QS/QS_all.h"
	#include "QS/QS_extern_it.h"
	#include "QS/QS_timer.h"
	
	#include <timer.h>
		
		
			



	void initialiser_detections(void);
	
	void initialiser_cycle(Uint16 timer_initial);
		
	void fin_de_cycle(void);
	

	void test_oscillo_4voies(Uint8 init);

	
		
	void sauvegardes_instants_detections_balises(Uint16 instant_balise0, Uint16 instant_balise1, Uint16 instant_balise2, Uint16 duree_cycle);

	void it_on(Uint8 balise);
	
	void it_off(Uint8 balise);
	

	void _ISR _INT1Interrupt(void);
		
	
	void _ISR _INT2Interrupt(void);
	
	
	void _ISR _INT3Interrupt(void);
	
	
	void _ISR _T1Interrupt();


	void _ISR _T2Interrupt();
	

	
	
#endif /* ndef BEACON_H */
