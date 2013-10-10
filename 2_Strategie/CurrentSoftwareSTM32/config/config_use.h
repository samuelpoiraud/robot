/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_PIN_H
#define CONFIG_PIN_H

//	#define MODE_SIMULATION

#define USE_LCD
#ifdef USE_LCD
#define USE_I2C2
#endif


#define USE_RTC
#ifdef USE_RTC
#define USE_I2C2
#endif

//définition de la précision et des modes de calcul des sinus et cosinus
//	#define FAST_COS_SIN
//	#define COS_SIN_16384


//demande à l'asser d'envoyer regulierement la position du robot
//utilisé en 2010-2011 pour la détection des pions avec les capteurs de distance SICK (cf. brain.c)
#define USE_SCHEDULED_POSITION_REQUEST

//#define USE_TELEMETER
#ifdef USE_TELEMETER
//utilisation des télémètres laser SICK
//#define USE_SICK_DT50_LEFT
//#define USE_SICK_DT50_RIGHT
//#define USE_SICK_DT50_TOP
#define USE_SICK_DT10_0
#define USE_SICK_DT10_1
#define USE_SICK_DT10_2
#define USE_SICK_DT10_3
#endif

//Module de gestion des capteurs SICK pour scan de la zone de jeu... Ce module est très dépendant du règlement et du robot !
//#define USE_SICK

//utilisation du lecteur de codes-barres
//	#define USE_BARCODE_ON_UART1
//#define USE_BARCODE_ON_UART2

//envoi des messages de debug de timeout et activité des piles
//	#define USE_SEND_CAN_DEBUG

//envoie la position de l'adversaire sur le bus à chaque update (max 1 fois par seconde (a l'ecriture de ce commentaire))
//	#define USE_FOE_POS_CAN_DEBUG

//envoie de l'ajout, la mise a jour ou la suppresion d'un element de jeu sur le CAN
//	#define USE_ELEMENT_CAN_DEBUG


//utilise le mode de déplacement avec les polygones
//si cette ligne est commentée, on utilise par défaut le mode de déplacement avec pathfind
//#define USE_POLYGON

//utilise le buffer de l'asser pour les trajectoires multi-points et permet d'utiliser les courbes
#define USE_ASSER_MULTI_POINT

//les ASSER_TRAJECTOIRE_FINIE dépilent les asser goto meme si on est loin de la destination
#define ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION


#define SD_ENABLE

//Activation du module d'enregistrement des messages CAN reçus pendant les matchs en mémoire EEPROM.
//#define EEPROM_CAN_MSG_ENABLE


#endif /* CONFIG_PIN_H */
