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

		// Initialisation du gestionnaire MOSFET.
		void MOSFET_init();

		// Initialisation en position du gestionnaire de MOSFET.
		void MOSFET_init_pos();

		// Stoppe les mosfets. Cette fonction est appelée en fin de match
		void MOSFET_stop();

		// Gère les messages CAN liés au MOSFET
		bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg);

		// Réinitialise la configuration
		void MOSFET_reset_config();

		// Selftest des mosfets en actionneur
		bool_e MOSFET_selftest_act(Uint8 nb_mosfets);

		// Selftest des mosfets en stratégie
		bool_e MOSFET_selftest_strat();

		// Machine à états d'activation décalée des mosfets actionneurs, depuis la strat
		void MOSFET_state_machine(CAN_msg_t* msg);

	#endif
#endif	/* MOSFET_H */

