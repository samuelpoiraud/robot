#ifndef LED_H
	#define LED_H

	#include "../QS/QS_all.h"

	/**
	 * @brief Led de la Beacon Eye
	 */
	typedef enum{
		LED_0_BEACON_EYE,
		LED_1_BEACON_EYE,
		LED_2_BEACON_EYE,
		LED_COLOR_BEACON_EYE,
		LED_NUMBER_BEACON_EYE
	}ledBeaconEye_e;

	/**
	 * @brief Initialisation du module Led
	 */
	void LED_init();

	/**
	 * @brief Fonction d'interruption du module Led
	 * @param ms Temps de l'interruption
	 */
	void LED_processIt(Uint8 ms);

	/**
	 * @brief Modifier la couleur de la led couleur
	 * @param color Nouvelle couleur pour la led couleur
	 */
	void LED_setColor(led_color_e color);

#endif /* ndef LED_H */

