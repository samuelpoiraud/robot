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
	#include "config_use.h"
	#include "Stacks.h"

	typedef struct
	{
		Uint16 x,y;
		Sint16 angle;
		PROP_speed_e speed;
		way_e way;
		Uint8 curve;
		Uint8 priority_order; 
	}prop_arg_t;
		
	void PROP_set_threshold_error_translation(Uint8 value, bool_e reset);

	/* Accesseur en lecture sur les arguments de la pile PROP */ 
	prop_arg_t PROP_get_stack_arg(Uint8 index);
	
	/* Accesseur en écriture sur les arguments de la pile PROP */ 
	void PROP_set_stack_arg(prop_arg_t arg, Uint8 index);
	
	void PROP_set_position(Sint16 x, Sint16 y, Sint16 teta);

	/* Va a la position indiquée, se termine à l'arret */
	void PROP_goto (stack_id_e stack_id, bool_e init);


	void PROP_goto_multi_point (stack_id_e stack_id, bool_e init);
	void PROP_goto_multi_point_until_break(stack_id_e stack_id, bool_e init);
	void PROP_push_goto_multi_point (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, Uint8 priority_order, PROP_end_condition_e end_condition, bool_e run);
	void PROP_relative_goangle_multi_point (stack_id_e stack_id, bool_e init);
	void PROP_push_relative_goangle_multi_point (Sint16 angle, PROP_speed_e speed, bool_e run);

	
	/* Va a l'angle indiquée, se termine à l'arret */
	void PROP_goangle (stack_id_e stack_id, bool_e init);

	
	/* Tourne par rapport à l'angle actuel, se termine à l'arret */
	void PROP_relative_goangle (stack_id_e stack_id, bool_e init);
	
	/* Va se caler contre le mur */
	void PROP_rush_in_the_wall(stack_id_e stack_id, bool_e init);
	
	/* Arrête le robot, ne touche pas à la pile */
	void PROP_push_stop ();

	/* ajoute une instruction goto sur la pile asser */
	void PROP_push_goto (Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, PROP_end_condition_e end_condition, bool_e run);
	
	/* ajoute une instruction goangle sur la pile asser */
	void PROP_push_goangle (Sint16 angle, PROP_speed_e speed, bool_e run);
	
	/* ajoute une instruction relative_goangle sur la pile asser */
	void PROP_push_relative_goangle (Sint16 angle, PROP_speed_e speed, bool_e run);
	
	void PROP_push_relative_goto(Sint16 x, Sint16 y, PROP_speed_e speed, way_e way, Uint8 curve, bool_e run);
	
	/* ajoute une instruction rush_in_the_wall sur la pile asser */
	void PROP_push_rush_in_the_wall (way_e way, bool_e prop_rotate,Sint16 angle, bool_e run);
	
	void PROP_set_correctors(bool_e corrector_rotation, bool_e corrector_translation);
	//Modifie l'état des correcteurs de la propulsion. (attention, les correcteurs sont remis à un bon fonctionnement à chaque nouvel ordre de déplacement !)
	//Cela permet notamment de désasservir le robot lorsqu'on le souhaite...

	void PROP_set_acceleration(Uint8 acceleration);

	/* Demande un envoi par la propulsion de l'ensemble de ses coefs */
	void PROP_ask_propulsion_coefs(void);

	void PROP_WARNER_arm_x(Sint16 x);
	
	void PROP_WARNER_arm_y(Sint16 y);
	
	void PROP_WARNER_arm_teta(Sint16 teta);
	
	/* fonction retournant si on se situe à moins de 15 cm de la destination.
	Fonctionne en distance Manhattan */
	bool_e PROP_near_destination();
	
	/* fonction retournant si on se situe à moins de 2 degrés cm de la destination. */
	bool_e PROP_near_destination_angle();
	
	/* Regarde si la pile contient un goto vers (x, y) */
	bool_e PROP_has_goto(Sint16 x, Sint16 y);
	
	/* Affiche le contenu formaté de la pile asser, le haut de la pile en premier */
	void PROP_dump_stack ();
	
	#ifdef PROP_FUNCTIONS_C
	
		#include "QS/QS_can.h"
		#include "QS/QS_CANmsgList.h"
		#include "Geometry.h"
		
		typedef enum
		{CONFIG, XMSB, XLSB, YMSB, YLSB, VITESSE, MARCHE, RAYONCRB} PROP_CAN_msg_bytes_e;
	
		// Timeout en ms
		#define GOTO_TIMEOUT_TIME							5000	// On n'attend que 3 secondes sur les PROP_push_goto cette année
																	// car on ne fait pas de gros déplacements sur le terrain
			
		#define GOTO_MULTI_POINT_TIMEOUT_TIME				4000	//Nombre de secondes de timeout PAR POINT en mode multipoint.
		#define RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME	3000
				
		#define RUSH_TIMEOUT_TIME							25000
		// suffisant pour un demi tour en vitesse lente
		#define GOANGLE_TIMEOUT_TIME						6000
		
		// Coefficient multiplicatif de la distance pour obtenir le temps avant le timeout
		#define COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST			4 
		#define COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW			8 	
		
		#ifdef DEBUG_PROP_FUN
			#define prop_fun_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define prop_fun_printf(...)	(void)0
		#endif
		
	#endif /* def PROP_FUNCTIONS_C */
	
#endif /* ndef PROP_FUNCTIONS_H */
