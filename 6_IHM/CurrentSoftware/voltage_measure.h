/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : voltage_measure.h
 *	Package : Carte IHM
 *	Description : Contrôle de la tension des batteries
 *	Auteur : Anthony / Arnaud
 *	Version 2012/01/14
 */

#ifndef VOLTAGE_MEASURE_H
#define VOLTAGE_MEASURE_H

#include "QS/QS_all.h"
#include "QS/QS_adc.h"

#define ADC_12_HOKUYO		ADC_4
#define ADC_24_PUISSANCE	ADC_6
#define ADC_24_PERMANENCE	ADC_10

void VOLTAGE_MEASURE_init();
void VOLTAGE_MEASURE_process_it(Uint8 ms);
Uint16 VOLTAGE_MEASURE_measure24_mV(adc_id_e id);

#endif /* ndef VOLTAGE_MEASURE_H */
