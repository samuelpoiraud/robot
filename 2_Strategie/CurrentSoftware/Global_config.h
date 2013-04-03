/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Carte Principale
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20110227
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

	#define VERBOSE_MODE
	#define MODE_HOMOLOGATION
//	#define OUTPUT_LOG  //utilisé dans act_function.h/c seulement actuellement
	#define ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_STRAT
	#define I_AM_CARTE_STRAT
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ

	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic)
	 */
	#define FDP_2013
	
	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0xC0FF
	#define PORT_E_IO_MASK	0xFFFF
		#define SWITCH_STRAT_1	PORTEbits.RE0
		#define SWITCH_STRAT_2	PORTEbits.RE1
		#define SWITCH_STRAT_3	PORTEbits.RE2
		#define SWITCH_EVIT		PORTEbits.RE3
		#define SWITCH_COLOR	PORTEbits.RE4
		#define SWITCH_STRAT_4	PORTEbits.RE6
		#define LAT_ROBOT_ID_OUTPUT		LATEbits.LATE7
		#define TRIS_ROBOT_ID_OUTPUT	TRISEbits.TRISE7
		#define PORT_ROBOT_ID_INPUT		PORTEbits.RE5
		#define TRIS_ROBOT_ID_INPUT		TRISEbits.TRISE5
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFF3F
		#define SWITCH_RG0	PORTGbits.RG0
		#define SWITCH_RG1	PORTGbits.RG1
	#ifdef FDP_2013
		#define BLUE_LEDS	LATGbits.LATG7
		#define RED_LEDS	LATGbits.LATG6
	#else
		#define BLUE_LEDS	LATGbits.LATG6
		#define RED_LEDS	LATGbits.LATG7
	#endif
	#define PIN_BIROUTE	PORTGbits.RG8
		
	
	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique
	 */

	#define USE_CAN
/*	Nombre de messages CAN conservés
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		32

	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	
	#define USE_UART2
	#define USE_UART2RXINTERRUPT
	
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	12
	
/* Utilisation des entrées analogiques */
	#define USE_ANALOG_EXT_VREF

	#define ADC_PIN_DT10_0		0 		
		#define USE_AN2 			// DT10 0
	//#define ADC_PIN_DT10_1 	 	1
		//#define USE_AN3	 			// DT10 1
	#define ADC_PIN_DT10_2		1
		#define USE_AN4 			// DT10	2
	//#define ADC_PIN_DT10_3		3
		//#define USE_AN5 			// DT10	3
		
	#define ADC_PIN_DT50_FRONT 	2
		#define USE_AN13 			// DT50 droit
	#define ADC_PIN_DT50_LEFT	3
		#define USE_AN14 			// DT50 left
	#define ADC_PIN_DT50_TOP	4
		#define USE_AN15 			// DT50 top
		
	/*
	 *
	 *	La section config carte P
	 *
	 *
	 */

	 //FIXME: ça marche d'avoir 16 au lieu de 32 ? Jean Claude: Mais oui bien sur !
	//On utilise très rarement plus de 5 empilement donc 16 devrait suffire (et puis il manquait de place en mémoire ...)
	#define STACKS_SIZE 16 //doit être < à 256

	//Utilisé par act_function
	#define QUEUE_SIZE	16

	//utilisation du module BUTTONS
	#define USE_BUTTONS
	//utilise le timer 2 pour les boutons
	#define BUTTONS_TIMER 2
		
	//demande à l'asser d'envoyer regulierement la position du robot
	//utilisé en 2010-2011 pour la détection des pions avec les capteurs de distance SICK (cf. brain.c)
	//#define USE_SCHEDULED_POSITION_REQUEST
	
	//utilisation des télémètres laser SICK
	//#define USE_SICK_DT50_LEFT
	//#define USE_SICK_DT50_RIGHT
	//#define USE_SICK_DT50_TOP
	#define USE_SICK_DT10_0
	#define USE_SICK_DT10_1
	#define USE_SICK_DT10_2
	#define USE_SICK_DT10_3
	
	//utilisation du lecteur de codes-barres
//	#define USE_BARCODE_ON_UART1
	//#define USE_BARCODE_ON_UART2
	
	//envoi des messages de debug de timeout et activité des piles
//	#define USE_SEND_CAN_DEBUG

	//envoie la position de l'adversaire sur le bus à chaque update (max 1 fois par seconde (a l'ecriture de ce commentaire))
	#define USE_FOE_POS_CAN_DEBUG
	
	//envoie de l'ajout, la mise a jour ou la suppresion d'un element de jeu sur le CAN
//	#define USE_ELEMENT_CAN_DEBUG

	//mode homologation (évitement très simple)
//	#define MODE_HOMOLOGATION

	//utilise le mode de déplacement avec les polygones
	//si cette ligne est commentée, on utilise par défaut le mode de déplacement avec pathfind
	//#define USE_POLYGON
	
	//utilise le buffer de l'asser pour les trajectoires multi-points et permet d'utiliser les courbes
	#define USE_ASSER_MULTI_POINT
	
	//les ASSER_TRAJECTOIRE_FINIE dépilent les asser goto meme si on est loin de la destination
	#define ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
	
	//active les debug_printf pour les sections concernées
//	#define DEBUG_ASSER_FUN
//	#define DEBUG_ACT_FUN
//	#define DEBUG_SICK
//	#define DEBUG_ELEMENTS
//	#define DEBUG_BARCODE
	#define DEBUG_DETECTION
	#define DEBUG_AVOIDANCE
//	#define DEBUG_POLYGON
//	#define DEBUG_TELEMETER

	//définition de la précision et des modes de calcul des sinus et cosinus
//	#define FAST_COS_SIN
//	#define COS_SIN_16384



/*
CAN_send_debug("");
0 timeout asser goto
1 timeout asser goangle
2 timeout asser rush
3 timeout asser relative go angle
A pull
B flush
C asser far from destination

[a-z] temporaires
*/

#endif /* ndef GLOBAL_CONFIG_H */
