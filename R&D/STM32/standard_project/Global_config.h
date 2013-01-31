/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, Chomp
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20100418
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "../QS/QS_types.h"

	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT
	
	/* Configuration des entrées-sorties du STM32
		* Chaque ligne correspond à la liste des pins correspondant à la catégorie
		* Exemple: pour configurer PD4 en sortie:
		*	#define PORTD_OUTPUT_PIN_LIST GPIO_Pin_4
		* Exemple: pour configurer PB0 et PB1 en entrée:
		*	#define PORTB_INPUT_PIN_LIST GPIO_Pin_0 | GPIO_Pin_1
	 */
	 
	 #define PORTD_OUTPUT_PIN_LIST GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
	
	/* Définition de noms usuels pour les ports 
		* Syntaxe: 
		#define NOM		 	GPIOx, GPIO_Pin_y
	*/
	
	// Test de sortie: RD4
	#define TEST_GPIO_SORTIE 	GPIOD, GPIO_Pin_4

	// Ports particuliers: PWM sur TIM3_CH1 et TIM3_CH2
	#define PWM_MOTEUR1		TIM3, CH1
	#define PWM_MOTEUR2		TIM3, CH2


	/** Les instructions suivantes permettent de configurer certaines
		* entrees/sorties du pic pour realiser des fonctionnalites
		* particulieres comme une entree analogique 
	**/
	
	/* Watchdog */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 2
	
	/* Utilisation de l'ADC */
	#define USE_ADC
	
	/* Utilisation de l'UART */
	#define USE_UART
	
	/* Utilisation des PWM */
	#define USE_PWM
	#define FREQ_PWM_50KHZ
	//#define USE_PWM1 /* PWM1 peut être utilisé pour QEI ou PWM au choix */
	//#define FREQ_PWM_50HZ
	//#define FREQ_PWM_1KHZ
	//#define FREQ_PWM_10KHZ
	//#define FREQ_PWM_20KHZ
	//#define FREQ_PWM_50KHZ
	
	/* Utilisation du module d'asservissement en position */
//	#define USE_DCMOTOR	
	
#endif /* ndef GLOBAL_CONFIG_H */
