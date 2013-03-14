/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20081010
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"
	
	/* En premier lieu, choisir si on souhaite executer un code de
	 * test ou le code de match */

	#define VERBOSE_MODE
	
	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM_CARTE_SUPER	//C'est voulu... on recevra ainsi TOUT les messages CAN (notamment ceux qui sont envoyés par les filles comme si on était une carte strat !)
	#define I_AM CARTE_SUPER	//De toute façon, sur ce bus can balises, on est concerné par tout ce qui passe !
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
//	#define FREQ_10MHZ_FS_CLK	/* uniquement pour tester le quartz */
//	#define FREQ_20MHZ
//	#define FREQ_40MHZ
	
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */



	#define PORT_A_IO_MASK	0xFFFF
		#define RETOUR_RECEPTEUR_3 PORTAbits.RA14
	#define PORT_B_IO_MASK	0xFFFF
	
	#define PORT_C_IO_MASK	0xFFFF
		
	#define PORT_D_IO_MASK	0xC8FF	//RD11, RD14 et RD15 en entrée pour test input capture 4.

	#define PORT_E_IO_MASK	0xFFEA
		#define SYNCHRO_1	LATEbits.LATE0
		#define SYNCHRO_2	LATEbits.LATE2
		#define SYNCHRO_3	LATEbits.LATE4
		#define RETOUR_RECEPTEUR_1 	PORTEbits.RE8
		#define RETOUR_RECEPTEUR_2 	PORTEbits.RE9
	#define PORT_F_IO_MASK	0xFFFF	
	
	#define PORT_G_IO_MASK	0xFC3B
		#define RESET_XBEE			LATGbits.LATG2
		#define LED_MOTHER_BEACON	LATGbits.LATG6
		#define LED_CORNER_BEACON	LATGbits.LATG7
		#define LED_MIDLE_BEACON	LATGbits.LATG8
		#define LED_BAT_FAIBLE		LATGbits.LATG9


	#define USE_BUTTONS
	#define I_ASSUME_I_WILL_CALL_BUTTONS_PROCESS_IT_ON_MY_OWN
	#define USE_CAN
	#define CAN_SEND_TIMEOUT_ENABLE
//	#define USE_CAN2
/*	Nombre de messages CAN conservés 
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		16
//
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define USE_UART2
	#define USE_UART2RXINTERRUPT
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	16
//
/* utilise les PWM L au lieu des H */
///////////////////////////////////////////////Attention, la carte 2008 utilise les PWM_L
//	#define USE_PWM_L
/* choix de la fréquence des PWM */
//	#define FREQ_PWM_50HZ
	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_50KHZ
//
//
/* utilisation des entrées analogiques */
//	#define USE_AN0
//	#define USE_AN1
	#define USE_AN2		//Mesure de Vbat.
//	#define USE_AN3
//	#define USE_AN4
//	#define USE_AN5
//	#define USE_AN6   //Telemetre
//	#define USE_AN7   //Telemetre
//	#define USE_AN8
//	#define USE_AN9
//	#define USE_AN10
//	#define USE_AN11
//	#define USE_AN12
//	#define USE_AN13
//	#define USE_AN14
//	#define USE_AN15
	#define USE_ANALOG_EXT_VREF

	#include "Beacon_config.h"
	
#endif /* ndef GLOBAL_CONFIG_H */
