/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_use.h 2706 2014-10-04 13:06:44Z aguilmet $
 *
 *  Package : Carte Strategie
 *  Description : Activation de modules et fonctionnalit�s
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_USE_H
#define CONFIG_USE_H

//////////////////////////////////////////////////////////////////
//-------------------------MODE ET USE--------------------------//
//////////////////////////////////////////////////////////////////

	//Pour l'utilisation de l'�cran LCD tactile et de la propulsion virtuelle hors du robot, activez ceci :
	//#define SIMULATION_VIRTUAL_PERFECT_ROBOT		//L'odom�trie est faite sur un robot virtuel parfait.
	//#define MODE_SIMULATION							//Dans ce mode, le bus CAN est d�sactiv�.
	//#define CAN_SEND_OVER_UART						//envoi des msg can sur l'uart, en utilisant le format normalis� des msg can over uart
	//#define LCD_TOUCH								//Active le LCD tactile

/* MODE d'EMPLOI MODE SIMULATION ET ECRAN TACTILE (en dehors d'un fond de panier !)
 *  1 - activez les 4 defines ci-dessus
 *  2 - activez MODE_SIMULATION sur la carte STRATEGIE
 *  3 - avec 4 fils : reliez entre les cartes PROP et STRAT (�ventuellement le 5V...) :
 *  	GND<->GND
 *  	5V<->5V
 *  	PB6<->PB7
 *  	PB7<->PB6
 *  4 - d�sactivez le verbose strat�gie en reliant PA7 � un potentiel GND. (par exemple jumper entre PA7 et PA5).
 *  Vous avez un robot virtuel parfait...
 */

/* MODE d'EMPLOI : carte propulsion sur un fond de panier sans robot r�el
 *  Activer les define SIMULATION_VIRTUAL_PERFECT_ROBOT et CAN_SEND_OVER_UART (et c'est tout !)
 */

/* ECRAN TACTILE - sans simulation (= � cot� d'un fond de panier, avec un robot virtuel ou r�el)
 *
 * Vous pouvez brancher une carte avec un LCD, avec les param�tres suivants :
 * - Activer les 4 defines ci-dessus. (oui, oui !)
 * - Relier l'UART Tx de la propulsion vers l'UART Rx de la carte supportant l'�cran tactile.
 */

//MODES INDISPENSABLES EN MATCHS
	#define PERIODE_IT_ASSER (5)		//[ms] ne pas y toucher sans savoir ce qu'on fait, (ou bien vous voulez vraiment tout casser !)

	#define USE_CODEUR_SUR_IT_ET_QE		//Utiliser les IT externes et les QEx pour acqu�rir les infos codeurs au lieu du CPLD !

	#define USE_HOKUYO					//Active le module HOKUYO et la d�tection des ennemis... !

	//#define USE_GYROSCOPE				//Activation du gyroscope

	#define USE_PROP_AVOIDANCE			//Activation de la gestion de l'�vitement en propulsion

	#define USE_ACT_AVOID				//Activation de la modification du rectangle d'�vitement en fonction des actionneurs

	#define FAST_COS_SIN				//Calcul rapide des cos et sin � l'aide d'un GRAND tableau de valeur

	#define VERBOSE_MODE				//Activation du verbose

	//#define VERBOSE_MSG_SEND_OVER_UART//Activation de la verbosit� des messages CAN sur UART

	#define CAN_VERBOSE_MODE			//Activation de la verbosit� des message CAN

	#define FDP_2014					//Define Temporaire pour l'ancien FDP

	#define SCAN_CUP                    //Avtivation du scan de gobelets

//MODES NON INDISPENSABLES OU INPENSABLES EN MATCHS

	//#define MODE_REGLAGE_KV
	#ifdef MODE_REGLAGE_KV
		#ifndef VERBOSE_MODE
			#warning "Le mode r�glage KV a besoin du VERBOSE_MODE"
		#endif
	#endif

//	#define MODE_PRINTF_TABLEAU		//Module permettant de visualiser apr�s coup une grande s�rie de valeur quelconque pour chaque IT...

 //	#define MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT	//Permet des affichage en d�bug d'un tas de variables
													//Mais comme le temps pass� � l'affichage est sup�rieur au rythme d'�volution des variables
													//Il est pratique de figer une sauvegarde de toutes la structure global_t et d'afficher ensuite des donn�es "coh�rentes", prises au m�me "instant" !
													//Voir le code associ� � cette macro !

//	#define SUPERVISOR_DISABLE_ERROR_DETECTION			//Dangereux, mais parfois utile !


//	#define CORRECTOR_ENABLE_ACCELERATION_ANTICIPATION //Inutile... Voir wiki...




//////////////////////////////////////////////////////////////////
//----------------------------QS--------------------------------//
//////////////////////////////////////////////////////////////////

/* ID de la carte: cf le type cartes_e de QS_types.h */
	#define I_AM CARTE_PROP		//A voir avec Gwenn pour changer
	#define I_AM_CARTE_PROP

/* Il faut choisir � quelle frequence on fait tourner la STM32 */
	#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fr�quence de l'horloge externe

/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE		32	//Nombre de messages CAN conserv�s pour traitement hors interuption
	#define QS_CAN_RX_IT_PRI	5	//Modif de la priorit� de l'IT can pour rendre la priorit� des codeurs plus grande ! (Plus faible = plus prioritaire)

/* R�glages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART1_BAUDRATE		230400
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE	128

	//#define USE_UART2
	//#define USE_UART2RXINTERRUPT

	#define UART_RX_BUF_SIZE	512	//Taille de la chaine de caracteres memorisant les caracteres recus sur UART

/* Bouton */
	#define USE_BUTTONS
	#define BUTTONS_TIMER 3

/* R�glages watchdog */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 4
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

/* R�glages PWM */
	#define USE_PWM3    //moteur droite
	#define USE_PWM4    //moteur gauche
	//#define FREQ_PWM_50HZ
	//#define FREQ_PWM_1KHZ
	//#define FREQ_PWM_10KHZ
	//#define FREQ_PWM_20KHZ
	#define FREQ_PWM_50KHZ

/* R�glages SPI */
	#ifdef USE_GYROSCOPE
		#define USE_SPI2 // GYROSCOPE
	#endif

/* R�glages QEI */
	#ifdef USE_CODEUR_SUR_IT_ET_QE
		#define USE_QEI_ON_IT
		#define QEI_ON_IT_QA 1
		#define QEI_ON_IT_QB 2
	#endif

/* R�glages FIFO */
	#define USE_FIFO

#include "../_Propulsion_config.h"

/* R�capitulatif IT :
* TIMER 1 : Gyroscope		(it.c/h)
* TIMER 2 : IT principale	(it.c/h)
* TIMER 3 : Boutons		(QS_buttons.c/h)
* TIMER 4 : Watchdog		(QS_watchdog.c/h)
* TIMER 5 : Clock			(clock.c/h)
*/

#endif /* CONFIG_USE_H */
