/*
 *  Club Robot ESEO 2009 - 2013
 *
 *  Fichier : QS_adc.h
 *  Package : Qualité Soft
 *  Description : Gestion du convertisseur analogique/numérique
 *  Auteur : Alexis
 *  Version 20130518
 */

#ifndef QS_ADC_H
	#define QS_ADC_H

	#include "QS_types.h"

	typedef enum{
		ADC_0 = 0,
		ADC_1,
		ADC_2,
		ADC_3,
		ADC_4,
		ADC_5,
		ADC_6,
		ADC_7,
		ADC_8,
		ADC_9,
		ADC_10,
		ADC_11,
		ADC_12,
		ADC_13,
		ADC_14,
		ADC_15
	}adc_id_e;
	// Enumération des convertisseurs analogique numérique sélectionnable

	void ADC_init();

	Sint16 ADC_getValue(adc_id_e channel);
	/*
	 *	cette fonction permet de récupérer la valeur echantillonnée par
	 *	le convertisseur analogique numerique (adc). Le channel est le
	 *	numero de la voie choisie parmis les voies selectionnées,
	 *	numerotees a partir de 0.
	 *	Par exemple, si l'utilisateur a choisi USE_AN4 et USE_AN8,
	 *	il faudra lire ADC_getValue(0) pour lire l'entree analogique 4 et ADC_getValue(1) pour lire l'entree analogique 8.
	 *
	 */

#endif /* ndef QS_ADC_H */
