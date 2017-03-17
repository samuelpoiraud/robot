/*
 *  Club Robot ESEO 2016
 *
 *  Fichier : QS_mosfet.h
 *  Package : Qualité Soft
 *  Description : Regroupement de toutes les fonctions utiles à la gestion d'une carte MOSFET
 *  Auteur : Valentin
 *
 */


/* Pour utiliser ce module, vous devez :
 *
 * Définir dans le fichier config_use (de big ou de small) la macro USE_MOSFETS_MODULE et la/les macro
 * USE_MOSFET_X où X est le numéro du mosfet souhaité.
 *		exemple : #define USE_MOSFETS_MODULE
 *				  #define USE_MOSFET_3
 *
 * Dans QS_CAN_msgList.h,vous devez définir les SID de chacun des mosfets ACT_MOSFET_1 pour un mosfet
 * situé en actionneur ou STRAT_MOSFET_1 pour un mosfet situé en stratégie. Vous pouvez également redéfinir
 * ces SID pour leur donner un nom.
 *
 * Dans QS_can_verbose.c, ajouter/modifier les mosfets
 *
 * En strategie dans act_functions.c, ajouter/modifier mosfets
 *
 * Dans QS_mosfet.c, ajouter/modifier les mosfets et leurs queue
 *
 * Dans queue.h de la stratégie, vous devez définir ACT_QUEUE_Mosfet_act_1 pour un mosfet en actionneur
 * ou ACT_QUEUE_Mosfet_strat_1 en stratégie.
 *
 * Vous devez aussi définir si cela n'est pas fait des macros pour chaque mosfet dans config_pin (ex MOSFET_1_PIN).
 *
 * Il est également possible d'activer plusieurs mosfets à la fois en définissant, de la même façon que pour un
 * mosfet seul, un actionneur ACT_MOSFET_MULTI (pour les mosfets pilotés par l'actionneur) et/ou
 * STRAT_MOSFET_MULTI (pour les mosfets pilotés par la stratégie). L'ordre à éxécuter est du type :
 * ACT_push_order(ACT_MOSFET_MULTI, arg) où arg vaut une valeur binaire (bit de poid fort -> MOSFET_8, bit de
 * poid faible -> MOSFET_1).
 *
 * Exemple : ACT_push_order(ACT_MOSFET_MULTI, 0b01000110) activera les mosfets 7, 3 et 2 en actionneur. Leurs
 * activation sera décalé de quelques millisecondes pour éviter les pics de courant.
 */


#ifndef MOSFET_H
	#define	MOSFET_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_CARTE_STRAT
		#include "../actuator/queue.h"

		// Regarde si le sid correspond à celui d'un mosfet géré par la stratégie
		bool_e MOSFET_isStratMosfetSid(queue_id_e act_id);
	#endif

	#ifdef USE_MOSFETS_MODULE

		// Initialisation du gestionnaire MOSFET.
		void MOSFET_init();

		// Initialisation en position du gestionnaire de MOSFET.
		void MOSFET_init_pos();

		// Stoppe les mosfets. Cette fonction est appelée en fin de match
		void MOSFET_stop();

		// Réinitialise la configuration
		void MOSFET_reset_config();

		// Machine à état pour activer en décalé les mosfets
		bool_e MOSFET_do_order_multi(CAN_msg_t* msg);

		// Lance le selftest d'un mosfet
		bool_e MOSFET_selftest_this_mosfet(bool_e entrance, ACT_sid_e sid, code_id_e code);

	#endif

	// Gère les messages CAN liés au MOSFET
	bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg);

	// Selftest des mosfets en actionneur  //TODO : A enlever lorsque le nouveau selftest sera opé
	bool_e MOSFET_selftest_act(Uint8 nb_mosfets);

	// Selftest des mosfets en stratégie  //TODO : A enlever lorsque le nouveau selftest sera opé
	bool_e MOSFET_selftest_strat();


#endif	/* MOSFET_H */

