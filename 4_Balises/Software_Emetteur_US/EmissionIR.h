/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionIR.h
 *	Package : Balise Récepteur US et IR
 *	Description : 
 *	Auteur : Nirgal
 *	Version 201101
 */

#ifndef EMISSIONIR_H
	#define EMISSIONIR_H
	
	#include "QS/QS_all.h"

	
	void EmissionIR_init(void);
	void EmissionIR_stop(void);
	void EmissionIR_next_step(void);
	void EmissionIR_step_init(void);
	
#endif /* ndef EMISSIONIR_H */
