   /*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : Global_config.h
 *	Package : Balises 2010
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen && Nirgal
 *	Version 200912
 */

#include "QS/QS_config_doc.h"

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"


	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_BALISE
	#define I_AM_CARTE_BALISE
	
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
//	#define FREQ_INTERNAL_CLK	/* uniquement pour tester le quartz */
//	#define FREQ_20MHZ
//	#define FREQ_40MHZ
	
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

#define CARTE_2012
#ifndef CARTE_2010 
	#ifndef CARTE_2011
		#ifndef CARTE_2012
			#error "Il faut définir CARTE_2010, CARTE_2011 ou CARTE_2012"
		#endif
	#endif
#endif

#ifdef CARTE_2012
		#define PORT_A_IO_MASK	0xFFFF
		#define BOUTON PORTAbits.RA14
		#define BOUTON2 PORTAbits.RA15
		
	#define PORT_B_IO_MASK	0xFFFF
	
		
	#define PORT_C_IO_MASK	0xFFFF	
		
	#define PORT_D_IO_MASK	0xFFFF
		#define PORT_TSOP PORTD
		#define TSOP_0 PORTDbits.RD0
		#define TSOP_1 PORTDbits.RD1
		#define TSOP_2 PORTDbits.RD2
		#define TSOP_3 PORTDbits.RD3
		#define TSOP_4 PORTDbits.RD4
		#define TSOP_5 PORTDbits.RD5
		#define TSOP_6 PORTDbits.RD6
		#define TSOP_7 PORTDbits.RD7
		#define TSOP_8 PORTDbits.RD8
		#define TSOP_9 PORTDbits.RD9
		#define TSOP_10 PORTDbits.RD10
		#define TSOP_11 PORTDbits.RD11
		#define TSOP_12 PORTDbits.RD12
		#define TSOP_13 PORTDbits.RD13
		#define TSOP_14 PORTDbits.RD14
		#define TSOP_15 PORTDbits.RD15
		
	#define PORT_E_IO_MASK	0xFCFF
		
	#define PORT_F_IO_MASK	0xFFFF	
		#define	SYNCHRO	PORTFbits.RF6
	
	#define PORT_G_IO_MASK	0xFFFF
	
#endif
#ifdef CARTE_2010

	#define PORT_A_IO_MASK	0xFFFF
		#define BOUTON PORTAbits.RA14
		
	#define PORT_B_IO_MASK	0x00FF
		#define PICOT_RB2 PORTBbits.RB2
		#define PICOT_RB3 PORTBbits.RB3
		#define PICOT_RB4 PORTBbits.RB4
		#define PICOT_RB5 PORTBbits.RB5
		#define PICOT_RB6 PORTBbits.RB6
		#define PICOT_RB7 PORTBbits.RB7
		#define LED_8 LATBbits.LATB8
		#define LED_9 LATBbits.LATB9
		#define LED_10 LATBbits.LATB10
		#define LED_11 LATBbits.LATB11
		#define LED_12 LATBbits.LATB12
		#define LED_13 LATBbits.LATB13
		#define LED_14 LATBbits.LATB14
		#define LED_15 LATBbits.LATB15
		
	#define PORT_C_IO_MASK	0xFFFF	
		
	#define PORT_D_IO_MASK	0xFFFF
		#define PORT_TSOP PORTD
		#define TSOP_0 PORTDbits.RD0
		#define TSOP_1 PORTDbits.RD1
		#define TSOP_2 PORTDbits.RD2
		#define TSOP_3 PORTDbits.RD3
		#define TSOP_4 PORTDbits.RD4
		#define TSOP_5 PORTDbits.RD5
		#define TSOP_6 PORTDbits.RD6
		#define TSOP_7 PORTDbits.RD7
		#define TSOP_8 PORTDbits.RD8
		#define TSOP_9 PORTDbits.RD9
		#define TSOP_10 PORTDbits.RD10
		#define TSOP_11 PORTDbits.RD11
		#define TSOP_12 PORTDbits.RD12
		#define TSOP_13 PORTDbits.RD13
		#define TSOP_14 PORTDbits.RD14
		#define TSOP_15 PORTDbits.RD15
		
	#define PORT_E_IO_MASK	0xFC00
		#define LED_0 LATEbits.LATE0
		#define LED_1 LATEbits.LATE1
		#define LED_2 LATEbits.LATE2
		#define LED_3 LATEbits.LATE3
		#define LED_4 LATEbits.LATE4
		#define LED_5 LATEbits.LATE5
		#define LED_6 LATEbits.LATE6
		#define LED_7 LATEbits.LATE7
		//#define LED_RUN LATEbits.LATE8	//cf balise_recepteur.h...
		//#define LED_CAN LATEbits.LATE9
		
	#define PORT_F_IO_MASK	0xFFFF
	
	#define PORT_G_IO_MASK	0xFFFF

#endif
#ifdef CARTE_2011

		#define PORT_A_IO_MASK	0xFFFF
		#define BOUTON PORTAbits.RA14
		#define BOUTON2 PORTAbits.RA15
		
	#define PORT_B_IO_MASK	0x00FF
		#define LED_8 LATBbits.LATB8
		#define LED_9 LATBbits.LATB9
		#define LED_10 LATBbits.LATB10
		#define LED_11 LATBbits.LATB11
		#define LED_12 LATBbits.LATB12
		#define LED_13 LATBbits.LATB13
		#define LED_14 LATBbits.LATB14
		#define LED_15 LATBbits.LATB15
		
	#define PORT_C_IO_MASK	0xFFFF	
		
	#define PORT_D_IO_MASK	0xFFFF
		#define PORT_TSOP PORTD
		#define TSOP_0 PORTDbits.RD0
		#define TSOP_1 PORTDbits.RD1
		#define TSOP_2 PORTDbits.RD2
		#define TSOP_3 PORTDbits.RD3
		#define TSOP_4 PORTDbits.RD4
		#define TSOP_5 PORTDbits.RD5
		#define TSOP_6 PORTDbits.RD6
		#define TSOP_7 PORTDbits.RD7
		#define TSOP_8 PORTDbits.RD8
		#define TSOP_9 PORTDbits.RD9
		#define TSOP_10 PORTDbits.RD10
		#define TSOP_11 PORTDbits.RD11
		#define TSOP_12 PORTDbits.RD12
		#define TSOP_13 PORTDbits.RD13
		#define TSOP_14 PORTDbits.RD14
		#define TSOP_15 PORTDbits.RD15
		
	#define PORT_E_IO_MASK	0xFC00
		#define LED_0 LATEbits.LATE0
		#define LED_1 LATEbits.LATE1
		#define LED_2 LATEbits.LATE2
		#define LED_3 LATEbits.LATE3
		#define LED_4 LATEbits.LATE4
		#define LED_5 LATEbits.LATE5
		#define LED_6 LATEbits.LATE6
		#define LED_7 LATEbits.LATE7
	//	#define LED_RUN LATEbits.LATE8	//cf balise_recepteur.h...
	//	#define LED_CAN LATEbits.LATE9
		
	#define PORT_F_IO_MASK	0xFFFF
	
	#define PORT_G_IO_MASK	0xFFFF
	
#endif


	#define USE_ANALOG_EXT_VREF
	
	#include "balise_config.h"
	
#endif /* ndef GLOBAL_CONFIG_H */
