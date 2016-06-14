/*
 *	Club Robot ESEO 2010 - 2015
 *	CheckNorris
 *
 *	Fichier : Emission_ir.c
 *	Package : Projet Balise Emission IR
 *	Description : Gestion de l'émission IR...
 *	Auteur : Nirgal(PIC)/Arnaud(STM32)
 *	Version 201203
 */

#ifndef EMISSIONIR_H
	#define EMISSIONIR_H

	#include "QS/QS_all.h"

	typedef enum{
		IR_MODULATION_3_3K = 0b00000001,
		IR_MODULATION_SYNC = 0b00000010,
		IR_MODULATION_ALL = (IR_MODULATION_3_3K | IR_MODULATION_SYNC)
	}ir_modulation_e;

	void EmissionIR_init(void);
	void EmissionIR_activate(ir_modulation_e ir_modulation);
	void EmissionIR_deactivate(ir_modulation_e ir_modulation);

#endif /* ndef EMISSIONIR_H */

