/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : prop_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'asser
 *	Auteur : Jacen
 *	Version 20110313
 */

#include "QS/QS_all.h"

#ifndef PROP_FUNCTIONS_H
	#define PROP_FUNCTIONS_H

	#include "avoidance.h"

// ---------------------------------------------------------------------------- Fonctions de d�placement

	void PROP_stop();

	void PROP_goTo(Sint16 x, Sint16 y, prop_referential_e referential, PROP_speed_e speed, way_e way, bool_e multipoint, propEndCondition_e propEndCondition, Uint8 curve, avoidance_type_e avoidance, prop_border_mode_e border_mode, Uint8 idTraj);

	void PROP_goAngle(Sint16 angle, prop_referential_e referential, PROP_speed_e speed, way_e way, bool_e multipoint, propEndCondition_e propEndCondition, Uint8 idTraj);

	void PROP_rushInTheWall(way_e way, bool_e asserRot, Sint16 angle);

	error_e wait_move_and_wait_detection(trajectory_e trajectory_type, Uint8 nb_trajectory, Uint8 idLastTraj, STRAT_endCondition_e end_condition, time32_t begin_time);

// ---------------------------------------------------------------------------- Fonctions de warner

	/*
		Fonction permettant d'armer un avertisseur sur la propulsion.
		Un message de BROACAST_POSITION avec raison |= WARNING_REACH_X sera envoy� d�s que le robot atteindra cette ligne virtuelle...
		Ce message d�clenchera la lev�e en environnement strat�gie du flag global.prop.reach_x
		@param : 0 permet de demander un d�sarmement de l'avertisseur.
	*/
	void PROP_WARNER_arm_x(Sint16 x);

	/*
		Fonction permettant d'armer un avertisseur sur la propulsion.
		Un message de BROACAST_POSITION avec raison |= WARNING_REACH_Y sera envoy� d�s que le robot atteindra cette ligne virtuelle...
		Ce message d�clenchera la lev�e en environnement strat�gie du flag global.prop.reach_y
		@param : 0 permet de demander un d�sarmement de l'avertisseur.
	*/
	void PROP_WARNER_arm_y(Sint16 y);

	/*
		Fonction permettant d'armer un avertisseur sur la propulsion.
		Un message de BROACAST_POSITION avec raison |= WARNING_REACH_TETA sera envoy� d�s que le robot atteindra cette ligne angulaire virtuelle...
		Ce message d�clenchera la lev�e en environnement strat�gie du flag global.prop.reach_teta
		@param : 0 permet de demander un d�sarmement de l'avertisseur.
	*/
	void PROP_WARNER_arm_teta(Sint16 teta);

	/*
		Fonction permettant d'armer un avertisseur sur la propulsion.
		Un message de BROACAST_POSITION avec raison |= WARNING_REACH_DISTANCE sera envoy� d�s que le robot atteindra ce cercle virtuelle autour du point de destination...
		C'est � dire que d�s que le robot entre dans le cercle de rayon la  distnce donn�e et de centre le point de destination donn�,
		le message BROADCAST_POSITION sera envoy�
		Ce message d�clenchera la lev�e en environnement strat�gie du flag global.prop.reach_distance
		@param : 0 permet de demander un d�sarmement de l'avertisseur.
	*/
	void PROP_WARNER_arm_distance(Uint16 distance, Sint16 x, Sint16 y);

	Uint8 PROP_getNextIdTraj();


// ---------------------------------------------------------------------------- Fonctions autres

	Uint8 PROP_custom_speed_convertor(Uint16 speed); // en mm/s cette valeur sera limit�e par la propulsion en cas de vitesse trop �lev�e
	// A titre indicatif la vitesse maximum du robot est de 1200 mm/sec

	void PROP_debug_move_position(Sint16 x, Sint16 y, Sint16 teta);
	// Alt�ration des coordonn�es du robot x/y/teta

	void PROP_set_threshold_error_translation(Uint8 value, bool_e reset);

	void PROP_set_position(Sint16 x, Sint16 y, Sint16 teta);

	void PROP_set_correctors(bool_e corrector_rotation, bool_e corrector_translation);
	//Modifie l'�tat des correcteurs de la propulsion. (attention, les correcteurs sont remis � un bon fonctionnement � chaque nouvel ordre de d�placement !)
	//Cela permet notamment de d�sasservir le robot lorsqu'on le souhaite...

	void PROP_set_acceleration(Uint32 acceleration);

	/* Demande un envoi par la propulsion de l'ensemble de ses coefs */
	void PROP_ask_propulsion_coefs(void);

	//D�finition de l'accesseur en lecture et en �criture de distance_to_reach
	void PROP_set_distance_to_reach(Uint16 dist);
	Uint16 PROP_get_distance_to_reach(void);

	void PROP_set_detected_foe(CAN_msg_t *msg);

#endif /* ndef PROP_FUNCTIONS_H */
