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

	/**
	 * Ecrit l'état d'une sortie GPIO.
	 * @param gpio_port port à écrire
	 * @param bit bit du port à changer
	 * @param set_bit valeur du bit
	 */
	void PORTS_WritePin(GPIO_TypeDef* gpio_port, Uint8 bit, bool_e set_bit);
	bool_e PORTS_ReadPin(GPIO_TypeDef* gpio_port, Uint8 bit);

#endif /* ndef QS_PORTS_H */
