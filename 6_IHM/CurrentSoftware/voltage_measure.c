/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : voltage_measure.c
 *	Package : Carte IHM
 *	Description : Contrôle de la tension des batteries
 *	Auteur : Anthony / Arnaud
 *	Version 2012/01/14
 */

#include "voltage_measure.h"
#include "QS/QS_adc.h"

#define ADC_24_PUISSANCE	ADC_6
#define ADC_24_PERMANENCE	ADC_10

void VOLTAGE_MEASURE_init(){
	ADC_init();
}
