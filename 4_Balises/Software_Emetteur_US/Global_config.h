/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20090223
 */

#include "QS/QS_config_doc.h"

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"

	// Ne pas toucher, pour reconnaitre en strat qui est qui
	#define BIG_BALISE		0
	#define SMALL_BALISE	1

	// Attention � changer selon la balise que nous programmons
	#define I_AM_CARTE_BALISE	SMALL_BALISE
	
	
	#define NUMERO_BALISE_EMETTRICE 	(1)	//ATTENTION : 1 ou 2... !!!
	#define NOMBRE_BALISES_EMETTRICES 	(2)

	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_BALISE

	/* Il faut choisir � quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	//#define FREQ_INTERNAL_CLK
	
	/* Les instructions ci dessous d�finissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

/* Masquage � effectuer lorsque les nouvelles cartes actionneurs seront arriv�es */

	#define PORT_A_IO_MASK	0xFFFF		
	#define PORT_B_IO_MASK	0xFFFF
		//RB2 = servo porte
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0x70FF
		//#define PORT_RFRESET LATDbits.LATD14
		#define PORT_RFCONFIG LATDbits.LATD15

	#define PORT_E_IO_MASK	0xFFF0
		#define RIN_MOTEUR	LATEbits.LATE2
		#define	FIN_MOTEUR	LATEbits.LATE3
		/*  Bouton-poussoir de test */
		#define PUSHBUTTON	!PORTEbits.RE8
		
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFFFF
		#define SWITCH_MULTI_EMETTEURS !PORTGbits.RG0


//	#define USE_SPI2
//	#define SPI_R_BUF_SIZE 16
	/* CAN */
//	#define USE_CAN
//	#define CAN_BUF_SIZE		4

	/*  R�glages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12
	
	/* R�glages PWM */
	//#define DISABLE_PWM4H
	#define FREQ_PWM_40KHZ

	#define USE_FIFO
	#define USE_RF
	#define RF_UART UART2_ID
	#define RF_TIMER_ID 2


// D�finissions des messages CAN
#define STRAT_BALISE_BATTERY_LOW			0x247


 //RF_MODULE doit �tre d�fini a quelque chose de l'enum RF_module_e dans QS_rf.h
#if NUMERO_BALISE_EMETTRICE == 1
	#define RF_MODULE RF_FOE1
#elif NUMERO_BALISE_EMETTRICE == 2
	#define RF_MODULE RF_FOE2
#else
#error "NUMERO_BALISE_EMETTRICE est invalide (doit �tre 1 ou 2)"
#endif


#endif /* ndef GLOBAL_CONFIG_H */
