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

//////////////////////////////////////////////////////////////////
//-------------------------MODE ET USE--------------------------//
//////////////////////////////////////////////////////////////////

	//Pour l'utilisation de l'écran LCD tactile et de la propulsion virtuelle hors du robot, activez ceci :
	//#define SIMULATION_VIRTUAL_PERFECT_ROBOT		//L'odométrie est faite sur un robot virtuel parfait.
	//#define MODE_SIMULATION						//Dans ce mode, le bus CAN est désactivé.
	//#define CAN_SEND_OVER_UART					//Envoi des msg can sur l'uart, en utilisant le format normalisé des msg can over uart
	//#define XBEE_SIMULATION						//Envoi des messages XBEE venant de la simulation sur le CAN
	//#define DISPLAY_AVOIDANCE_POLY				//Envoi des messages contenant les informations du polygone d'évitement pour la simulation graphique

/* MODE d'EMPLOI MODE SIMULATION (en dehors d'un fond de panier !)
 *  1 - activez les 4 defines ci-dessus
 *  2 - activez MODE_SIMULATION sur la carte STRATEGIE
 *  3 - avec 4 fils : reliez entre les cartes PROP et STRAT (éventuellement le 5V...) :
 *  	GND<->GND
 *  	5V<->5V
 *  	PB6<->PB7
 *  	PB7<->PB6
 *  4 - désactivez le verbose stratégie en reliant PA7 à un potentiel GND. (par exemple jumper entre PA7 et PA5).
 *  Vous avez un robot virtuel parfait...
 */

/* MODE d'EMPLOI : carte propulsion sur un fond de panier sans robot réel
 *  Activer les define SIMULATION_VIRTUAL_PERFECT_ROBOT et CAN_SEND_OVER_UART (et c'est tout !)
 */

//MODES INDISPENSABLES EN MATCHS
	#define PERIODE_IT_ASSER (5)		//[ms] ne pas y toucher sans savoir ce qu'on fait, (ou bien vous voulez vraiment tout casser !)

	//#define DISABLE_SECURE_GPIO_INIT
	#ifdef DISABLE_SECURE_GPIO_INIT
		#warning 'ATTENTION SECURITE DU GPIO DESACTIVER'
	#endif

	//#define DISABLE_WHO_AM_I				// Désactive la détection du robot.
	#ifdef DISABLE_WHO_AM_I
		#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
	#endif

	//#define DISABLED_BALISE_AVOIDANCE		// Désactivation de l'évitement balise
	#ifdef DISABLED_BALISE_AVOIDANCE
		#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
	#endif

	#define USE_NEW_POWER_DETECTION
	#ifndef USE_NEW_POWER_DETECTION
		#warning "Nouvelle détection de puissance non active"
	#endif

	#define BUFFER_SIZE 64	//maximum : 255


    #define USE_HOKUYO					//Active le module HOKUYO et la détection des ennemis... !

	//#define USE_GYROSCOPE				//Activation du gyroscope

	#define USE_ACT_AVOID				//Activation de la modification du rectangle d'évitement en fonction des actionneurs

	#define FAST_COS_SIN				//Calcul rapide des cos et sin à l'aide d'un GRAND tableau de valeur

	#define VERBOSE_MODE				//Activation du verbose

    //#define CAN_VERBOSE_MODE			//Activation de la verbosité des message CAN

	#define LIMITATION_PWM_BORDER_MODE

	//#define DETECTION_CHOC

	//#define SCAN
		//#define USE_ADS1118_ON_ADC
		//#define SCAN_BORDURE
		//#define SCAN_ROTATION
		//#define SCAN_OBJETS


//MODES NON INDISPENSABLES OU INPENSABLES EN MATCHS

	//#define MODE_REGLAGE_KV
	#ifdef MODE_REGLAGE_KV
		#ifndef VERBOSE_MODE
			#warning "Le mode réglage KV a besoin du VERBOSE_MODE"
		#endif
	#endif

    //#define MODE_PRINT_FIRST_TRAJ		// Il est recommandé de baisser le baudrate pour éviter les erreurs de transmission et le flood du récepteur

	//#define MODE_PRINTF_TABLEAU		//Module permettant de visualiser après coup une grande série de valeur quelconque pour chaque IT...

	//#define MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT	//Permet des affichage en débug d'un tas de variables
													//Mais comme le temps passé à l'affichage est supérieur au rythme d'évolution des variables
													//Il est pratique de figer une sauvegarde de toutes la structure global_t et d'afficher ensuite des données "cohérentes", prises au même "instant" !
													//Voir le code associé à cette macro !

	//#define SUPERVISOR_DISABLE_ERROR_DETECTION			//Dangereux, mais parfois utile !


	//#define CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION //Inutile... Voir wiki...


//////////////////////////////////////////////////////////////////
//----------------------------QS--------------------------------//
//////////////////////////////////////////////////////////////////

/* Define pour identifier la carte */
	#define I_AM_CARTE_PROP

/* Il faut choisir à quelle frequence on fait tourner la STM32 */
	#define HCLK_FREQUENCY_HZ		168000000	//168Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ		42000000	//42Mhz,  Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ		84000000	//84Mhz,  Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ	8000000		//8Mhz,   Fréquence de l'horloge externe

/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE		32	//Nombre de messages CAN conservés pour traitement hors interuption
	#define QS_CAN_RX_IT_PRI	2	//Modif de la priorité de l'IT can pour rendre la priorité des codeurs plus grande ! (Plus faible = plus prioritaire)

/* Réglages UART */
	#define USE_UART1
		#define UART1_BAUDRATE				230400

		#define USE_UART1_TX_BUFFER
			#define UART1_TX_BUFFER_SIZE 	128

		#define USE_UART1_RX_BUFFER
			#define UART1_RX_BUFFER_SIZE 	32


/* Bouton */
	#define USE_BUTTONS
	#define BUTTONS_TIMER 3

/* Réglages watchdog */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 4
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

/* Réglages PWM */
	#define USE_PWM_MODULE
		#define PWM_FREQ 50000
		#define USE_PWM3    //moteur droite
		#define USE_PWM4    //moteur gauche

/* Réglages SPI */
	#ifdef DETECTION_CHOC
		#define USE_SPI1
	#endif
	#if defined(USE_GYROSCOPE) || defined(USE_ADS1118_ON_ADC)
		#define USE_SPI2 // GYROSCOPE
	#endif

/* Réglages ADS 1118 */
	#ifdef USE_ADS1118_ON_ADC
		#define USE_ADS_1118
	#endif

/* Réglages QEI */
	#define USE_QUEI1
	#define USE_QUEI2

/* Réglages FIFO */
	#define USE_FIFO

/* Réglages ADC */
	//#define ADC_12_BIT
	#define USE_AN_VREFIN
	#define USE_AN11		// Télémètre laser gauche (Black) et Télémètre poissons (Pearl)
	#ifndef USE_ADS1118_ON_ADC
		#define USE_AN12		// Télémètre laser droite (Black)
	#endif
	#define ADC_SENSOR_LASER_LEFT			ADC_11
	#define ADC_SENSOR_LASER_RIGHT			ADC_12

#include "../_Propulsion_config.h"

/* Récapitulatif TIMERs :
 * TIMER 1 : Gyroscope		(it.c/h)
 * TIMER 2 : IT principale	(it.c/h)
 * TIMER 3 : Boutons		(QS_buttons.c/h)
 * TIMER 4 : Watchdog		(QS_watchdog.c/h)
 * TIMER 5 : ...
 */

/* Récapitulatif IT priorité :
 * 15 : TIMER_5
 * 14 : TIMER_4
 * 13 : TIMER_3
 * 11 : TIMER_2
 *  9 : TIMER_1
 *  3 : USART_1
 *  2 : CAN
 *  1 : USB
 *  0 : Systick
 */

#endif /* CONFIG_USE_H */
