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
	#include "stm32f4xx_gpio.h"
	/** Configure le module GPIO suivant global_config.h
	 **/
	void PORTS_init(void);

	Uint8 PORTS_adc_init(void* adc_handle, Uint8 ADC_sampleTime);
	void PORTS_pwm_init();
	void PORTS_uarts_init();
	void PORTS_qei_init();
	void PORTS_spi_init();
	void PORTS_i2c_init(void);
	void PORTS_can_init();
	void PORTS_set_pull(GPIO_TypeDef* GPIOx, Uint16 GPIO_Pin, GPIOPuPd_TypeDef pull_up);

#endif /* ndef QS_PORTS_H */
