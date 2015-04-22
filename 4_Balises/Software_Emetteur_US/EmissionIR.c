/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : EmissionUS.c
 *	Package : Projet Balise Emission US & IR
 *	Description : Gestion de l'émission IR...
 *	Auteur : Nirgal
 *	Version 201101
 */

#include "EmissionIR.h"
#include "SynchroRF.h"

#define IR_ON 		(LATEbits.LATE7)
#define IR_OFF 		(LATEbits.LATE6)
#define TRIS_IR_ON 	TRISEbits.TRISE7
#define TRIS_IR_OFF	TRISEbits.TRISE6

void EmissionIR_init(void)
{
	EmissionIR_AUTO();
	TRIS_IR_ON  = 0;
	TRIS_IR_OFF = 0;
}

void EmissionIR_AUTO(void)	//IR laissé aux oscillateurs externes...
{
	IR_ON = 0;
	IR_OFF = 1;
	LED_UART = 1;
}

void EmissionIR_OFF(void)	//IR Toujours éteint
{
	IR_ON = 0;
	IR_OFF = 0;
	LED_UART = 0;
}

void EmissionIR_ON(void)	//IR Toujours allumé -> en principe jamais utilisé.
{
	IR_ON = 1;
	IR_OFF = 0;
	LED_UART = 1;
}
