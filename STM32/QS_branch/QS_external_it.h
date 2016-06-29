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

	typedef enum {
		EXTIT_GpioA,
		EXTIT_GpioB,
		EXTIT_GpioC,
		EXTIT_GpioD,
		EXTIT_GpioE,
		EXTIT_GpioF,
		EXTIT_GpioG
	} EXTERNALIT_port_e;

	typedef enum {
		EXTIT_Pin0,
		EXTIT_Pin1,
		EXTIT_Pin2,
		EXTIT_Pin3,
		EXTIT_Pin4,
		EXTIT_Pin5,
		EXTIT_Pin6,
		EXTIT_Pin7,
		EXTIT_Pin8,
		EXTIT_Pin9,
		EXTIT_Pin10,
		EXTIT_Pin11,
		EXTIT_Pin12,
		EXTIT_Pin13,
		EXTIT_Pin14,
		EXTIT_Pin15
	} EXTERNALIT_pin_e;

	typedef enum {
		EXTIT_Edge_Rising,
		EXTIT_Edge_Falling,
		EXTIT_Edge_Both
	} EXTERNALIT_edge_e;

	typedef void (*EXTERNALIT_callback_it_t)(void);

	void EXTERNALIT_init();
	void EXTERNALIT_configure(EXTERNALIT_port_e port, Uint8 pin, EXTERNALIT_edge_e edge, EXTERNALIT_callback_it_t callback);
	void EXTERNALIT_set_edge(EXTERNALIT_port_e port, Uint8 pin, EXTERNALIT_edge_e edge);
	void EXTERNALIT_set_priority(EXTERNALIT_port_e port, Uint8 pin, Uint8 priority);
	void EXTERNALIT_disable(EXTERNALIT_port_e port, Uint8 pin);
	void EXTERNALIT_set_it_enabled(EXTERNALIT_port_e port, Uint8 pin, bool_e enabled);

#endif /* ndef QS_EXTERNAL_IT_H */
