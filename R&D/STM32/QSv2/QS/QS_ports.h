/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_ports.h
 *  Package : Qualité Soft
 *  Description : Gestion des entrées-sorties GPIO
 *  Auteur : Gwenn
 *  Version 20100418
 */

#ifndef QS_PORTS_H
	#define QS_PORTS_H
	
	#include "QS_all.h"
	
	/** Configure le module GPIO suivant global_config.h
	 **/
	void PORTS_init(void);

	Uint8 PORTS_adc_init(ADC_TypeDef* ADCx, Uint8 ADC_sampleTime);
	void PORTS_pwm_init();
	void PORTS_uarts_init();
	void PORTS_qei_init();
	void PORTS_spi_init();
	void PORTS_can_init();

#endif /* ndef QS_PORTS_H */
