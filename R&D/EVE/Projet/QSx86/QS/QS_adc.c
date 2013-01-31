/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_adc.c
 *	Package : QSx86
 *	Description : Utilisation des fonctions de convertion Analogique/Numérique
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#include "QS_adc.h"

#include "QS_ports.h"

void ADC_init()
{
	
}

Sint16 ADC_getValue(Uint8 channel)
{
	
	return 0;	
}	

// _ADCInterrupt() is the A/D interrupt service routine (ISR).
// The routine must have global scope in order to be an ISR.
// The ISR name is chosen from the device linker script.
/*void _ISR _ADCInterrupt(void)
{
	//IT Desactivée
		
	//Clear the A/D Interrupt flag bit or else the CPU will
	//keep vectoring back to the ISR
	IFS0bits.ADIF = 0;
}*/

