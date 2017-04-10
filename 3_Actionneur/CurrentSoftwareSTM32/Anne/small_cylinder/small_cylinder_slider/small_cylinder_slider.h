/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : small_cylinder_slider.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 SMALL_CYLINDER_SLIDER
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef SMALL_CYLINDER_SLIDER_H
    #define	SMALL_CYLINDER_SLIDER_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

    /** Initialisation du gestionnaire de SMALL_CYLINDER_SLIDER.
	 *
	 * Configure l'asservissement du RX24
	 */
    void SMALL_CYLINDER_SLIDER_init();

    /** Initialisation en position du gestionnaire de SMALL_CYLINDER_SLIDER.
	 *
	 * Initialise la position du RX24
	 */
    void SMALL_CYLINDER_SLIDER_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
    void SMALL_CYLINDER_SLIDER_stop();

    /** Gère les messages CAN liés à la SMALL_CYLINDER_SLIDER.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
    bool_e SMALL_CYLINDER_SLIDER_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
    void SMALL_CYLINDER_SLIDER_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
    void SMALL_CYLINDER_SLIDER_reset_config();

#endif	/* SMALL_CYLINDER_SLIDER_H */

