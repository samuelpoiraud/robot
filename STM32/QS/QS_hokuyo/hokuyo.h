/*
 * header.h
 *
 *  Created on: 16 f�vr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYO_H_
#define QS_HOKUYO_HOKUYO_H_

#include "../QS_all.h"
#include "../../config/config_use.h"

#if defined(USE_HOKUYO)

#include "hokuyo_config.h"

typedef struct {
	Sint32 dist;
	Sint16 teta;
	Sint32 coordX;
	Sint32 coordY;
} HOKUYO_adversary_position;

typedef struct {
	Sint16 power_intensity;
	Sint16 teta;
	Sint32 coordX;
	Sint32 coordY;
} HOKUYO_point_position;


/**
 * @brief Initialisation de l'hokuyo et de l'USB
 */
void HOKUYO_init(void);

/**
 * @brief Tache de fond de l'hokuyo qui g�re le rafraichissement de la position des robots adverses
 */
void HOKUYO_processMain(void);

/**
 * @brief Tache d'interruption du module hokuyo
 */
void HOKUYO_processIt(Uint8 ms);

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
HOKUYO_point_position* HOKUYO_getValidPoints(void);

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
 * @brief Obtenir le nombre de d�connexion de l'hokuyo
 * @retval Le nombre de d�connexion de l'hokuyo depuis l'allumage de la carte
 */
Uint16 HOKUYO_getNbHokuyoDisconnection(void);

/**
 * @brief Obtenir la dur�e de la derni�re mesure
 * @retval La dur�e de la derni�re mesure
 */
time32_t HOKUYO_getLastMeasureDuration(void);

#ifndef I_AM_CARTE_BEACON_EYE
	/**
	 * @brief Etat de fonctionnement de l'hokuyo
	 * @retval Indique si l'hokuyo fonctionne bien
	 */
	bool_e HOKUYO_isWorkingWell(void);
#endif

/**
 * @brief Permet de pr�venir le module Hokuyo que le p�riph�rique est d�connect�
 */
void HOKUYO_deviceDisconnected(void);

/**
 * @brief Permet de pr�venir le module Hokuyo que le p�riph�rique est connect�
 */
void HOKUYO_deviceConnected(void);

#endif /* QS_HOKUYO_HOKUYO_H_ */

#endif
