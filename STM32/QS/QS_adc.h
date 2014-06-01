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


	/**
	 * @brief Fonction d'initialisation des ADCs
	 */
	void ADC_init();

	/**
	 * @brief Cette fonction permet de récupéré la valeur echantillonnée d'un canal voulu
	 * @param channel : valeur de l'énumération adc_id_e
	 * @return la valeur echantillonnée du canal
	 */
	Sint16 ADC_getValue(adc_id_e channel);

#endif /* ndef QS_ADC_H */
