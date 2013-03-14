/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : BEACON_config.h
 *  Package : Standard_Project
 *  Description : Configuration du code balise
 *  Auteur : Jacen
 *  Version 20081010
 */

#ifndef BEACON_CONFIG_H
	#define BEACON_CONFIG_H

	#define ENABLE_XBEE
	#define XBEE_PLUGGED_ON_UART2
	#include "QS/QS_can_over_xbee.h"
	
	#define QS_CAN_RX_IT_PRI CAN_INT_PRI_5 //Il est impératif que l'IT timer1 (de priorité 6) soit LA PLUS PRIORITAIRE !

	#define ECHO_UART1_TO_CAN
	
	
	#define NOMBRE_BALISES (3)
	
	#define DUREE_MINI_TOUR_MOTEUR 1500	//(2009-10-23, le moteur tourne a TMR1 = 2050, il faut configurer ici un nombre légèrement inférieur...)
	
	#define DUREE_MINI_ENTRE_DEUX_DETECTIONS 300//TODO regler
	
	#define NOMBRE_DETECTIONS_PAR_BALISE (4)
	
	#define NOMBRE_CYCLES_SAUVES (8)
	
	///////FONCTIONNEMENT DU CODE..........
	
	//commenter cette ligne pour ne pas envoyer les données sur l'uart 1...
//	#define ENVOYER_SUR_UART2_COMME_MSG_CAN
	#define ENVOYER_AUSSI_SUR_UART1

	//#define TEST_OSCILLO_4_VOIES
	
	
#endif /* ndef BEACON_CONFIG_H */
