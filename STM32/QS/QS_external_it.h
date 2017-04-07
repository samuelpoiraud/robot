/*
 *  Club Robot ESEO 2013 - 2014
 *
 *
 *  Fichier : QS_external_it.h
 *  Package : Qualité Soft
 *  Description : Gestion des interruptions externes
 *  Auteur : Alexis
 *  Version 20130929
 */

/** ----------------  Defines possibles  --------------------
 *	USE_EXTERNAL_IT				: Activation de QS_external_it
 *
 * ----------------  Choses à savoir  --------------------
 * Elles font facilement planté la carte. Ne pas laisser une pin
 *  en l'air possédant une it externe sinon risque de plantage.
 */

#ifndef QS_EXTERNAL_IT_H
	#define QS_EXTERNAL_IT_H

	#include "QS_all.h"

	#ifdef USE_EXTERNAL_IT

		#include "../stm32f4xx/stm32f4xx_gpio.h"

		typedef enum{
			EXTIT_GpioA,
			EXTIT_GpioB,
			EXTIT_GpioC,
			EXTIT_GpioD,
			EXTIT_GpioE,
			EXTIT_GpioF,
			EXTIT_GpioG
		}EXTERNAL_IT_port_e;

		typedef enum{
			EXTIT_PIN_0 = GPIO_PinSource0,
			EXTIT_PIN_1 = GPIO_PinSource1,
			EXTIT_PIN_2 = GPIO_PinSource2,
			EXTIT_PIN_3 = GPIO_PinSource3,
			EXTIT_PIN_4 = GPIO_PinSource4,
			EXTIT_PIN_5 = GPIO_PinSource5,
			EXTIT_PIN_6 = GPIO_PinSource6,
			EXTIT_PIN_7 = GPIO_PinSource7,
			EXTIT_PIN_8 = GPIO_PinSource8,
			EXTIT_PIN_9 = GPIO_PinSource9,
			EXTIT_PIN_10 = GPIO_PinSource10,
			EXTIT_PIN_11 = GPIO_PinSource11,
			EXTIT_PIN_12 = GPIO_PinSource12,
			EXTIT_PIN_13 = GPIO_PinSource13,
			EXTIT_PIN_14 = GPIO_PinSource14,
			EXTIT_PIN_15 = GPIO_PinSource15
		}EXTERNAL_IT_pin_e;

		typedef enum{
			EXTIT_Edge_Rising,
			EXTIT_Edge_Falling,
			EXTIT_Edge_Both
		}EXTERNAL_IT_edge_e;

		typedef void (*EXTERNALIT_callback_it_t)(void);
		typedef void (*EXTERNALIT_callback_it_with_id_t)(Uint8 id);

		void EXTERNALIT_init();
		void EXTERNALIT_configure(EXTERNAL_IT_port_e port, EXTERNAL_IT_pin_e pin, EXTERNAL_IT_edge_e edge, EXTERNALIT_callback_it_t callback);
		void EXTERNALIT_configureWithId(EXTERNAL_IT_port_e port, EXTERNAL_IT_pin_e pin, EXTERNAL_IT_edge_e edge, EXTERNALIT_callback_it_with_id_t callback, Uint8 id);
		void EXTERNALIT_set_edge(EXTERNAL_IT_port_e port, EXTERNAL_IT_pin_e pin, EXTERNAL_IT_edge_e edge);
		void EXTERNALIT_set_priority(EXTERNAL_IT_port_e port, EXTERNAL_IT_pin_e pin, Uint8 priority);
		void EXTERNALIT_disable(EXTERNAL_IT_port_e port, EXTERNAL_IT_pin_e pin);
		void EXTERNALIT_set_it_enabled(EXTERNAL_IT_port_e port, EXTERNAL_IT_pin_e pin, bool_e enabled);

	#endif

#endif /* ndef QS_EXTERNAL_IT_H */
