/*
 *  Club Robot ESEO 2009 - 2013
 *
 *  Fichier : QS_adc.h
 *  Package : Qualité Soft
 *  Description : Gestion du convertisseur analogique/numérique
 *  Auteur : Alexis
 *  Version 20130518
 */

/** ----------------  Defines possibles  --------------------
 *
 *	USE_AN0						: Activation du convertisseur analogique 0
 *	USE_AN1						: Activation du convertisseur analogique 1
 *	USE_AN2						: Activation du convertisseur analogique 2
 *	USE_AN3						: Activation du convertisseur analogique 3
 *	USE_AN4						: Activation du convertisseur analogique 4
 *	USE_AN5						: Activation du convertisseur analogique 5
 *	USE_AN6						: Activation du convertisseur analogique 6
 *	USE_AN7						: Activation du convertisseur analogique 7
 *	USE_AN8						: Activation du convertisseur analogique 8
 *	USE_AN9						: Activation du convertisseur analogique 9
 *	USE_AN10					: Activation du convertisseur analogique 10
 *	USE_AN11					: Activation du convertisseur analogique 11
 *	USE_AN12					: Activation du convertisseur analogique 12
 *	USE_AN13					: Activation du convertisseur analogique 13
 *	USE_AN14					: Activation du convertisseur analogique 14
 *	USE_AN15					: Activation du convertisseur analogique 15
 */

#ifndef QS_ADC_H
	#define QS_ADC_H

	#include "QS_all.h"

	/**
	 * @brief Enumération des convertisseurs analogique numérique sélectionnable
	 */
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
