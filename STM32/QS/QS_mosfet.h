/*
 *  Club Robot ESEO 2016
 *
 *  Fichier : QS_mosfet.h
 *  Package : Qualit� Soft
 *  Description : Regroupement de toutes les fonctions utiles � la gestion d'une carte MOSFET
 *  Auteur : Valentin
 *
 */


/* Pour utiliser ce module, vous devez :
 *
 * D�finir dans le fichier config_use (de big ou de small) la macro USE_MOSFETS_MODULE et la/les macro
 * USE_MOSFET_X o� X est le num�ro du mosfet souhait�.
 *		exemple : #define USE_MOSFETS_MODULE
 *				  #define USE_MOSFET_3
 *
 * Dans QS_CAN_msgList.h,vous devez d�finir les SID de chacun des mosfets ACT_MOSFET_1 pour un mosfet
 * situ� en actionneur ou STRAT_MOSFET_1 pour un mosfet situ� en strat�gie. Vous pouvez �galement red�finir
 * ces SID pour leur donner un nom.
 *
 * Dans QS_can_verbose.c, ajouter/modifier les mosfets
 *
 * En strategie dans act_functions.c, ajouter/modifier mosfets
 *
 * Dans QS_mosfet.c, ajouter/modifier les mosfets et leurs queue
 *
 * Dans queue.h de la strat�gie, vous devez d�finir ACT_QUEUE_Mosfet_act_1 pour un mosfet en actionneur
 * ou ACT_QUEUE_Mosfet_strat_1 en strat�gie.
 *
 * Vous devez aussi d�finir si cela n'est pas fait des macros pour chaque mosfet dans config_pin (ex MOSFET_1_PIN).
 *
 * Il est �galement possible d'activer plusieurs mosfets � la fois en d�finissant, de la m�me fa�on que pour un
 * mosfet seul, un actionneur ACT_MOSFET_MULTI (pour les mosfets pilot�s par l'actionneur) et/ou
 * STRAT_MOSFET_MULTI (pour les mosfets pilot�s par la strat�gie). L'ordre � �x�cuter est du type :
 * ACT_push_order(ACT_MOSFET_MULTI, arg) o� arg vaut une valeur binaire (bit de poid fort -> MOSFET_8, bit de
 * poid faible -> MOSFET_1).
 *
 * Exemple : ACT_push_order(ACT_MOSFET_MULTI, 0b01000110) activera les mosfets 7, 3 et 2 en actionneur. Leurs
 * activation sera d�cal� de quelques millisecondes pour �viter les pics de courant.
 */


#ifndef MOSFET_H
	#define	MOSFET_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_CARTE_STRAT
		#include "../actuator/queue.h"

		// Regarde si le sid correspond � celui d'un mosfet g�r� par la strat�gie
		bool_e MOSFET_isStratMosfetSid(queue_id_e act_id);
	#endif

	#ifdef USE_MOSFETS_MODULE

		// Initialisation du gestionnaire MOSFET.
		void MOSFET_init();

		// Initialisation en position du gestionnaire de MOSFET.
		void MOSFET_init_pos();

		// Stoppe les mosfets. Cette fonction est appel�e en fin de match
		void MOSFET_stop();

		// R�initialise la configuration
		void MOSFET_reset_config();

		// Machine � �tat pour activer en d�cal� les mosfets
		bool_e MOSFET_do_order_multi(CAN_msg_t* msg);

		// Lance le selftest d'un mosfet
		bool_e MOSFET_selftest_this_mosfet(bool_e entrance, ACT_sid_e sid, code_id_e code);

	#endif

	// G�re les messages CAN li�s au MOSFET
	bool_e MOSFET_CAN_process_msg(CAN_msg_t* msg);

	// Selftest des mosfets en actionneur  //TODO : A enlever lorsque le nouveau selftest sera op�
	bool_e MOSFET_selftest_act(Uint8 nb_mosfets);

	// Selftest des mosfets en strat�gie  //TODO : A enlever lorsque le nouveau selftest sera op�
	bool_e MOSFET_selftest_strat();


#endif	/* MOSFET_H */

