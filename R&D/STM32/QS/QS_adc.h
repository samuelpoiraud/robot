/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_adc.h
 *  Package : Qualité Soft
 *  Description : Gestion du convertisseur analogique/numérique
 *  Auteur : Gwenn
 *  Version 20100424
 */

#ifndef QS_ADCH
	#define QS_ADC_H
	
	#include "QS_all.h"
	
	#ifdef USE_ADC
	
		/* Définition d'un canal ADC */
		typedef enum {
			AN1,
			AN2,
			AN3,
			AN4,
			AN5,
			AN6,
			AN7,
			AN8,
		} adc_channel_t;
		
		/** Initialise le module ADC
			* 
		**/
		void ADC_init(void);
		
		
		/** Réalise une acquisition analogique
			* return Résultat sur 16 bits (12 bits utiles)
			* param channel Voie sur laquelle la mesure est réalisée
		**/
		Uint16 ADC_getValue(adc_channel_t channel);
		

	#endif /* def USE_ADC */

#endif /* ndef QS_PORTS_H */
