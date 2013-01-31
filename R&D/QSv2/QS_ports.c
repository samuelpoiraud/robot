/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech'
 *
 *	Fichier : QS_ports.c
 *	Package : Qualite Soft
 *	Description : fonction d'initialisation des ports des dsPICs 
 *				prealable a toute utilisation
 *	Auteur : Jacen
 *	Version 20090809
 */

#define QS_PORTS_C

#include "QS_ports.h"

void PORTS_init (void) 
{
	Uint16 mask_a=PORT_A_IO_MASK;
	Uint16 mask_c=PORT_C_IO_MASK;
	Uint16 mask_d=PORT_D_IO_MASK;
	Uint16 mask_e=PORT_E_IO_MASK;
	Uint16 mask_f=PORT_F_IO_MASK;
	Uint16 mask_g=PORT_G_IO_MASK;
	
	#ifdef USE_UART1
		mask_f = (mask_f & 0xFFF3) | 0x0004;
	#endif
	
	#ifdef USE_UART2
		mask_f = (mask_f & 0xFFCF) | 0x0010;
	#endif
	
	#ifdef USE_ANALOG_EXT_VREF
		mask_a |= 0x0600;
	#endif
	
	TRISA = mask_a;
	PORTS_adc_init();
	TRISC = mask_c;
	TRISD = mask_d;
	TRISE = mask_e;
	TRISF = mask_f;
	TRISG = mask_g;
}

void PORTS_adc_init(void)
{
	Uint16 mask_b=PORT_B_IO_MASK;
	Uint16 mask_adc=0xFFFF;

	#ifdef USE_AN0
		CONFIG_ADDPIN(mask_b, mask_adc, 0);
	#endif
	#ifdef USE_AN1
		CONFIG_ADDPIN(mask_b, mask_adc, 1);
	#endif
	
	#ifdef USE_AN2
		CONFIG_ADDPIN(mask_b, mask_adc, 2);
	#endif
	
	#ifdef USE_AN3
		CONFIG_ADDPIN(mask_b, mask_adc, 3);
	#endif
	
	#ifdef USE_AN4
		CONFIG_ADDPIN(mask_b, mask_adc, 4);
	#endif
	
	#ifdef USE_AN5
		CONFIG_ADDPIN(mask_b, mask_adc, 5);
	#endif
	
	#ifdef USE_AN6
		CONFIG_ADDPIN(mask_b, mask_adc, 6);
	#endif
	
	#ifdef USE_AN7
		CONFIG_ADDPIN(mask_b, mask_adc, 7);
	#endif
	
	#ifdef USE_AN8
		CONFIG_ADDPIN(mask_b, mask_adc, 8);
	#endif
	
	#ifdef USE_AN9
		CONFIG_ADDPIN(mask_b, mask_adc, 9);
	#endif
	
	#ifdef USE_AN10
		CONFIG_ADDPIN(mask_b, mask_adc, 10);
	#endif
	
	#ifdef USE_AN11
		CONFIG_ADDPIN(mask_b, mask_adc, 11);
	#endif
	
	#ifdef USE_AN12
		CONFIG_ADDPIN(mask_b, mask_adc, 12);
	#endif
	
	#ifdef USE_AN13
		CONFIG_ADDPIN(mask_b, mask_adc, 13);
	#endif
	
	#ifdef USE_AN14
		CONFIG_ADDPIN(mask_b, mask_adc, 14);
	#endif
	
	#ifdef USE_AN15
		CONFIG_ADDPIN(mask_b, mask_adc, 15);
	#endif
	
	TRISB = mask_b;
	ADPCFG = mask_adc;
}
