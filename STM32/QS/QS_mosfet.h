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

	#ifdef USE_MOSFETS_MODULE

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

		// Machine à état pour activer en décalé les mosfets
		bool_e MOSFET_do_order_multi(CAN_msg_t* msg);

		// Lance le selftest d'un mosfet
		bool_e MOSFET_selftest_this_mosfet(bool_e entrance, ACT_sid_e sid, code_id_e code);

		// Selftest des mosfets en actionneur  //TODO : A enlever lorsque le nouveau selftest sera opé
		bool_e MOSFET_selftest_act(Uint8 nb_mosfets);

		// Selftest des mosfets en stratégie  //TODO : A enlever lorsque le nouveau selftest sera opé
		bool_e MOSFET_selftest_strat();

	#endif

	#ifdef I_AM_CARTE_STRAT
		// Regarde si le sid correspond à celui d'un mosfet géré par la stratégie
		bool_e MOSFET_isStratMosfetSid(queue_id_e act_id);
	#endif
#endif	/* MOSFET_H */

