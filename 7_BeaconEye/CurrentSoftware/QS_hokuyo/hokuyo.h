/*
 * header.h
 *
 *  Created on: 16 f�vr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYO_H_
#define QS_HOKUYO_HOKUYO_H_

#include "../QS/QS_all.h"

typedef struct{
	Sint32 dist;
	Sint16 teta;
	Sint32 coordX;
	Sint32 coordY;
}HOKUYO_adversary_position;

typedef struct{
	Sint16 power_intensity;
	Sint16 teta;
	Sint32 coordX;
	Sint32 coordY;
}HOKUYO_point_position;

/**
 * @brief Initialisation de l'hokuyo et de l'USB
 */
void HOKUYO_init(void);

/**
 * @brief Tache de fond de l'hokuyo qui g�re le rafraichissement de la position des robots adverses
 */
void HOKUYO_processMain(void);

/**
 * @brief Affichage de la position des adversaires dans un terminal
 */
void HOKUYO_displayAdversariesPosition(void);

/**
 * @brief Affichage des points filtr�s d�tect�s par l'hokuyo
 */
void HOKUYO_dispalyValidPosition(void);

/**
 * @brief Obtenir le nombre d'adversaire actuel
 * @retval Le nombre d'adversaires d�tect�s par l'hokuyo
 */
Uint8 HOKUYO_getAdversariesNumber(void);

/**
 * @brief Obtenir les points filtr�s de l'hokuyo
 * @retval Le tableau de points filtr�s de l'hokuyo
 */
HOKUYO_adversary_position* HOKUYO_getValidPoints(void);

/**
 * @brief Obtenir le nombre de points filtr�s de l'hokuyo
 * @retval Le nombre de points filtr�s de l'hokuyo
 */
Uint16 HOKUYO_getNbValidPoints(void);

/**
 * @brief Obtenir la position d'un adversaire
 * @param i Index correspondant � la postion dans le tableau de l'adversaire
 * @retval La position de l'adversaire choisi
 */
HOKUYO_adversary_position* HOKUYO_getAdversaryPosition(Uint8 i);

/**
 * @brief Permet de pr�venir le module Hokuyo que le p�riph�rique est d�connect�
 */
void HOKUYO_deviceDisconnected(void);

#endif /* QS_HOKUYO_HOKUYO_H_ */
