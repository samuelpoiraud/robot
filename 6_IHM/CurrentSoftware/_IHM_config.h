/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : _IHM_config.h
 *  Package : IHM
 *  Description : Configuration de l'IHM
 *  Auteur : Anthony
 *  Version 201203
 */

#ifndef _IHM_CONFIG_H
	#define _IHM_CONFIG_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///MODES////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	#define CAN_SEND_OVER_UART					//envoi des msg can sur l'uart, en utilisant le format normalisé des msg can over uart



//MODES INDISPENSABLES EN MATCHS
	#define PERIODE_IT_ASSER (5)	//[ms] ne pas y toucher sans savoir ce qu'on fait, (ou bien vous voulez vraiment tout casser !)

	#define ENABLE_CAN			//Activation du bus CAN...

	#define USE_VERBOSE





#endif /* ndef _IHM_CONFIG_H */

