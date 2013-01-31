/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : QS_adc.h
 *  Package : Qualité Soft
 *  Description : 	Configuration et utilisation des convertisseurs
					Analogiques/Numeriques.
 *  Auteur : Lugancy, modifié par Jacen
 *  Version 20081205
 */

#ifndef QS_ADC_H
	#define QS_ADC_H
	
	#include "../QS/QS_all.h"
	
	void ADC_init();
	
	Uint16 ADC_getValue(Uint8 channel);
	/*
	 *	cette fonction permet de récupérer la valeur echantillonnée par
	 *	le convertisseur analogique numerique (adc). Le channel est le
	 *	numero de la voie choisie parmis les voies selectionnées,
	 *	numerotees a partir de 0.
	 *	Par exemple, si l'utilisateur a choisi USE_AN4 et USE_AN8,
	 *	il faudra lire ADC_getValue(0) pour lire l'entree analogique 4 et ADC_getValue(1) pour lire l'entree analogique 8.
	 *
	 */
	
	#ifdef QS_ADC_C	
		#include <adc10.h>
		#include "../QS/QS_ports.h"
		void _ISR _ADCInterrupt(void); //désactivée
	#endif /* def QS_ADC_C */	
#endif /* ndef QS_ADC_H */
