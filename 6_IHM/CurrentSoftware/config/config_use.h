/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_use.h 2706 2014-10-04 13:06:44Z aguilmet $
 *
 *  Package : Carte Strategie
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_USE_H
#define CONFIG_USE_H

#define USE_LCD
#define USE_I2C2
#define USE_WATCHDOG
#define WATCHDOG_TIMER 3
#define WATCHDOG_MAX_COUNT 5
#define WATCHDOG_QUANTUM 1

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_IHM
	#define I_AM_CARTE_IHM

#if defined(STM32F40XX)
	#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe
#else
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
#endif



	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique
	 */

	#define USE_CAN
		#ifndef USE_CAN
			#warning "can désactivé !"
		#endif

/*	Nombre de messages CAN conservés
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		32
#if defined(STM32F40XX)
	#define QS_CAN_RX_IT_PRI	5	//Plus faible = plus prioritaire
#else
	#define QS_CAN_RX_IT_PRI	CAN_INT_PRI_6	//Modif de la priorité de l'IT can pour rendre la priorité des codeurs plus grande !
#endif


	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART1_BAUDRATE	1382400
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE	128

/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	512


/* choix de la fréquence des PWM */
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_20KHZ
	#define FREQ_PWM_50KHZ

	#define USE_AN4		// Mesure 12V hokuyo
	#define USE_AN6		// Mesure 24V puissance
	#define USE_AN10	// Mesure 24V permanence

#endif /* CONFIG_USE_H */
