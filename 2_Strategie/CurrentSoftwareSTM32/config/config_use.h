/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : Activation de modules et fonctionnalit�s
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

//d�finition de la pr�cision et des modes de calcul des sinus et cosinus
//	#define FAST_COS_SIN
//	#define COS_SIN_16384


//demande � l'asser d'envoyer regulierement la position du robot
//utilis� en 2010-2011 pour la d�tection des pions avec les capteurs de distance SICK (cf. brain.c)
#define USE_SCHEDULED_POSITION_REQUEST

//#define USE_TELEMETER
#ifdef USE_TELEMETER
//utilisation des t�l�m�tres laser SICK
//#define USE_SICK_DT50_LEFT
//#define USE_SICK_DT50_RIGHT
//#define USE_SICK_DT50_TOP
#define USE_SICK_DT10_0
#define USE_SICK_DT10_1
#define USE_SICK_DT10_2
#define USE_SICK_DT10_3
#endif

//Module de gestion des capteurs SICK pour scan de la zone de jeu... Ce module est tr�s d�pendant du r�glement et du robot !
//#define USE_SICK

//utilisation du lecteur de codes-barres
//	#define USE_BARCODE_ON_UART1
//#define USE_BARCODE_ON_UART2

//envoi des messages de debug de timeout et activit� des piles
//	#define USE_SEND_CAN_DEBUG

//envoie la position de l'adversaire sur le bus � chaque update (max 1 fois par seconde (a l'ecriture de ce commentaire))
//	#define USE_FOE_POS_CAN_DEBUG

//envoie de l'ajout, la mise a jour ou la suppresion d'un element de jeu sur le CAN
//	#define USE_ELEMENT_CAN_DEBUG


//utilise le mode de d�placement avec les polygones
//si cette ligne est comment�e, on utilise par d�faut le mode de d�placement avec pathfind
//#define USE_POLYGON

//utilise le buffer de l'asser pour les trajectoires multi-points et permet d'utiliser les courbes
#define USE_ASSER_MULTI_POINT

//les ASSER_TRAJECTOIRE_FINIE d�pilent les asser goto meme si on est loin de la destination
#define ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION


#define SD_ENABLE

//Activation du module d'enregistrement des messages CAN re�us pendant les matchs en m�moire EEPROM.
//#define EEPROM_CAN_MSG_ENABLE


#endif /* CONFIG_PIN_H */
