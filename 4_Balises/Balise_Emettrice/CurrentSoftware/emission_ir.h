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

	void EmissionIR_init(void);
	void EmissionIR_activate(void);
	void EmissionIR_deactivate(void);

#endif /* ndef EMISSIONIR_H */

