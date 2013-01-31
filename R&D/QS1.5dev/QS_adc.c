/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : QS_adc.c
 *  Package : Qualité Soft
 *  Description : 	Configuration et utilisation des convertisseurs
					Analogiques/Numeriques.
 *  Auteur : Lugancy, modifié par Jacen
 *  Version 20081214
 */

#define QS_ADC_C

#include "QS_adc.h"


void ADC_init()
{
	Uint16 pinConfig, Scanselect, config1, config2, config3;
	
	ADCON1bits.ADON = 0;	// Désactive le module
	
	ConfigIntADC10(ADC_INT_DISABLE & ADC_INT_PRI_3);
	
	pinConfig = ENABLE_ALL_ANA;
	/* on refait la config nous meme apres, pour etre sur de l'orientation du portB */
	
	// Liste des entrées ANA à lire.
	Scanselect =  0
		#ifdef	USE_AN0
			| 1 << 0
		#endif
		#ifdef	USE_AN1
			| 1 << 1
		#endif
		#ifdef	USE_AN2
			| 1 << 2
		#endif
		#ifdef	USE_AN3
			| 1 << 3
		#endif
		#ifdef	USE_AN4
			| 1 << 4
		#endif
		#ifdef	USE_AN5
			| 1 << 5
		#endif
		#ifdef	USE_AN6
			| 1 << 6
		#endif
		#ifdef	USE_AN7
			| 1 << 7
		#endif
		#ifdef	USE_AN8
			| 1 << 8
		#endif
		#ifdef	USE_AN9
			| 1 << 9
		#endif
		#ifdef	USE_AN10
			| 1 << 10
		#endif
		#ifdef	USE_AN11
			| 1 << 11
		#endif
		#ifdef	USE_AN12
			| 1 << 12
		#endif
		#ifdef	USE_AN13
			| 1 << 13
		#endif
		#ifdef	USE_AN14
			| 1 << 14
		#endif
		#ifdef	USE_AN15
			| 1 << 15
		#endif
		; // pour finir l'instruction
	
	config1 = ADC_MODULE_ON			// Active le module adc
			& ADC_IDLE_STOP			// L'arrête quand on est idle
			& ADC_FORMAT_INTG		// Renvoie un entier non signé
			& ADC_CLK_AUTO			// Internal counter ends sampling and starts conversion (Auto convert)
			& ADC_SAMPLE_INDIVIDUAL	// Samples multiple channels individually in sequence
			& ADC_AUTO_SAMPLING_ON // On sample en mettant SAMP à 1
			;

	config2 = 
			#ifdef USE_ANALOG_EXT_VREF
				ADC_VREF_EXT_EXT			// Vref+ = ext et Vref- = ext */
			#else
				/* A/D Voltage reference configuration Vref+ is AVdd and Vref- is AVss */
				ADC_VREF_AVDD_AVSS
			#endif	/* def USE_ANALOG_EXT_VREF */
			& ADC_SCAN_ON				// Scan Input Selections for CH0+ S/H Input for MUX A Input Multiplexer Setting bit */
			& ADC_CONVERT_CH0			// On utilise que CH0 */
			& ADC_SAMPLES_PER_INT_16	// Lance une interruption tous les 16 samples
			& ADC_ALT_BUF_OFF			// Buffer configured as 1 16-word buffer */
			& ADC_ALT_INPUT_OFF			// ? Use only MUX A
			;

	config3 = ADC_SAMPLE_TIME_1		// (Pour l'auto sample)
			& ADC_CONV_CLK_SYSTEM	// Utiliser l'horloge du système pour la conversion
			& ADC_CONV_CLK_Tcy		// Ne pas diviser cette horloge
			;

	OpenADC10(config1, config2, config3, pinConfig, Scanselect);
	PORTS_adc_init(); /*on vérifie que le port B est bien configuré */
}

Uint16 ADC_getValue(Uint8 channel)
{
	switch (channel)
	{
		case 0:
			return ADCBUF0;
		case 1:
			return ADCBUF1;
		case 2:
			return ADCBUF2;
		case 3:
			return ADCBUF3;
		case 4:
			return ADCBUF4;
		case 5:
			return ADCBUF5;
		case 6:
			return ADCBUF6;
		case 7:
			return ADCBUF7;
		case 8:
			return ADCBUF8;
		case 9:
			return ADCBUF9;
		case 10:
			return ADCBUFA;
		case 11:
			return ADCBUFB;
		case 12:
			return ADCBUFC;
		case 13:
			return ADCBUFD;
		case 14:
			return ADCBUFE;
		case 15:
			return ADCBUFF;
		default:
			break;
	}
	/* mauvais canal demandé, renvoie 0 */
	return 0;	
}	

// _ADCInterrupt() is the A/D interrupt service routine (ISR).
// The routine must have global scope in order to be an ISR.
// The ISR name is chosen from the device linker script.
void _ISR _ADCInterrupt(void)
{
	//IT Desactivée
		
	//Clear the A/D Interrupt flag bit or else the CPU will
	//keep vectoring back to the ISR
	IFS0bits.ADIF = 0;
}
