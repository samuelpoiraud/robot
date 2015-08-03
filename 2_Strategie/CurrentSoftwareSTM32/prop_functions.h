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

	#include "prop_types.h"
	#include "Stacks.h"
	#include "avoidance.h"


	typedef struct
	{
		Uint16 x,y;
		Sint16 angle;
		PROP_speed_e speed;
		way_e way;
		prop_curve_e curve;
		prop_buffer_mode_e priority_order;
		avoidance_e avoidance;
		prop_border_mode_e border_mode;
	}prop_arg_t;





// ---------------------------------------------------------------------------- Fonctions de d�placement

	/* Arr�te le robot, ne touche pas � la pile */
	void PROP_push_stop ();

	/* ajoute une instruction goto sur la pile asser */
	void PROP_push_goto (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, avoidance_type_e avoidance, PROP_end_condition_e end_condition, prop_border_mode_e border_mode, bool_e run);

	/* ajoute une instruction goangle sur la pile asser */
	void PROP_push_goangle (Sint16 angle, PROP_speed_e speed, bool_e run);

	/* ajoute une instruction goto_multi_point sur la pile asser */
	void PROP_push_goto_multi_point (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, avoidance_type_e avoidance, Uint8 priority_order, PROP_end_condition_e end_condition, prop_border_mode_e border_mode, bool_e run);

	/* ajoute une instruction rush_in_the_wall sur la pile asser */
	void PROP_push_rush_in_the_wall (way_e way, bool_e prop_rotate,Sint16 angle, bool_e run);

	/* ajoute une instruction relative_goangle sur la pile asser */
	void PROP_push_relative_goangle (Sint16 angle, PROP_speed_e speed, bool_e run);

	/* ajoute une instruction relative_goangle_multi_point sur la pile asser */
	void PROP_push_relative_goangle_multi_point (Sint16 angle, PROP_speed_e speed, bool_e run);


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


// ---------------------------------------------------------------------------- Fonctions autres

	Uint8 PROP_custom_speed_convertor(Uint16 speed); // en mm/s cette valeur sera limit�e par la propulsion en cas de vitesse trop �lev�e
	// A titre indicatif la vitesse maximum du robot est de 1200 mm/sec

	void PROP_debug_move_position(Sint16 x, Sint16 y, Sint16 teta);
	// Alt�ration des coordonn�es du robot x/y/teta

	void PROP_set_threshold_error_translation(Uint8 value, bool_e reset);

	/* Accesseur en lecture sur les arguments de la pile PROP */
	prop_arg_t PROP_get_stack_arg(Uint8 index);

	/* Accesseur en �criture sur les arguments de la pile PROP */
	void PROP_set_stack_arg(prop_arg_t arg, Uint8 index);

	void PROP_set_position(Sint16 x, Sint16 y, Sint16 teta);

	void PROP_set_correctors(bool_e corrector_rotation, bool_e corrector_translation);
	//Modifie l'�tat des correcteurs de la propulsion. (attention, les correcteurs sont remis � un bon fonctionnement � chaque nouvel ordre de d�placement !)
	//Cela permet notamment de d�sasservir le robot lorsqu'on le souhaite...

	void PROP_set_acceleration(Uint16 acceleration);

	/* Demande un envoi par la propulsion de l'ensemble de ses coefs */
	void PROP_ask_propulsion_coefs(void);

	/* fonction retournant si on se situe � moins de 15 cm de la destination.
	Fonctionne en distance Manhattan */
	bool_e PROP_near_destination();

	/* fonction retournant si on se situe � moins de 2 degr�s cm de la destination. */
	bool_e PROP_near_destination_angle();

	/* Regarde si la pile contient un goto vers (x, y) */
	bool_e PROP_has_goto(Sint16 x, Sint16 y);

	/* Affiche le contenu format� de la pile asser, le haut de la pile en premier */
	void PROP_dump_stack ();

#endif /* ndef PROP_FUNCTIONS_H */
