/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_USE_H
#define CONFIG_USE_H


#define USE_LCD

#define USE_RTC

//définition de la précision et des modes de calcul des sinus et cosinus
	#define FAST_COS_SIN


//Module de gestion des capteurs SICK pour scan de la zone de jeu... Ce module est très dépendant du règlement et du robot !
//#define USE_SICK


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


#endif /* CONFIG_USE_H */
