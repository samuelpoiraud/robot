/*
 *  Club Robot ESEO 2016
 *
 *  Fichier : QS_mosfet.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions utiles à la gestion d'une carte MOSFET
 *  Auteur : Valentin
 *
 */


#ifndef MOSFET_H
	#define	MOSFET_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef USE_MOSFETS

		#include "../queue.h"

		/** Initialisation du gestionnaire de MOSFET.
		 *
		 * Configure du moteur de la MOSFET
		 */
		void MOSFET_init();

		/** Initialisation en position du gestionnaire de MOSFET.
		 *
		 * Initialise de la position de la MOSFET (stopé)
		 */
		void MOSFET_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void MOSFET_stop();

		/** Gère les messages CAN liés au MOSFET.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * Réinitialise la configuration de la pompe
		 */
		void MOSFET_reset_config();

		/**
		  * Selftest des mosfets en actionneur
		  */
		bool_e MOSFET_selftest_act();

		/**
		  * Selftest des mosfets en stratégie
		  */
		bool_e MOSFET_selftest_strat();

		/**
		  * Machine à états d'activation décalée des mosfets
		  */
		void MOSFET_state_machine(CAN_msg_t* msg);

	#endif
#endif	/* MOSFET_H */

