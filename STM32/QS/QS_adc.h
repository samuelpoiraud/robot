/*
 *  Club Robot ESEO 2009 - 2013
 *
 *  Fichier : QS_adc.h
 *  Package : Qualit� Soft
 *  Description : Gestion du convertisseur analogique/num�rique
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
	// Enum�ration des convertisseurs analogique num�rique s�lectionnable


	/**
	 * @brief Fonction d'initialisation des ADCs
	 */
	void ADC_init();

	/**
	 * @brief Cette fonction permet de r�cup�r� la valeur echantillonn�e d'un canal voulu
	 * @param channel : valeur de l'�num�ration adc_id_e
	 * @return la valeur echantillonn�e du canal
	 */
	Sint16 ADC_getValue(adc_id_e channel);

#endif /* ndef QS_ADC_H */
