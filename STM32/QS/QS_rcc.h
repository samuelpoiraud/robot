/*
 *  Club Robot ESEO 2015 - 2016
 *
 *  Fichier : QS_rcc.h
 *  Package : Qualit� Soft
 *  Description : Couche bas niveau RCC
 *  Auteur : Arnaud
 *  Version 20130518
 */

/** ----------------  Defines possibles  --------------------
 *	aucun
 */

#ifndef QS_RCC_H
	#define QS_RCC_H

	#include "QS_all.h"

	/**
	 * @brief RCC_read
	 * Affiche l'�tat des registres RCC
	 */
	void RCC_read(void);

	/**
	 * @brief RCC_board_just_reboot
	 * @return si la carte vient d'�tre red�marr� via soft ou bouton reset (pas un d�marrage normale du robot)
	 */
	bool_e RCC_board_just_reboot(void);


#endif /* ndef QS_RCC_H */
