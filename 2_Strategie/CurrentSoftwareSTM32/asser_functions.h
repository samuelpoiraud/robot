/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : asser_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'asser
 *	Auteur : Jacen
 *	Version 20110313
 */

#include "QS/QS_all.h"

#ifndef ASSER_FUNCTIONS_H
	#define ASSER_FUNCTIONS_H

	#include "asser_types.h"
	#include "config_use.h"
	#include "Stacks.h"

	typedef struct
	{
		Uint16 x,y;
		Sint16 angle;
		ASSER_speed_e speed;
		way_e way;
		Uint8 curve;
		Uint8 priority_order; 
	}asser_arg_t;
		
	/* Accesseur en lecture sur les arguments de la pile ASSER */ 
	asser_arg_t ASSER_get_stack_arg(Uint8 index);
	
	/* Accesseur en écriture sur les arguments de la pile ASSER */ 
	void ASSER_set_stack_arg(asser_arg_t arg, Uint8 index);
	
	/* Va a la position indiquée, se termine à l'arret */
	void ASSER_goto (stack_id_e stack_id, bool_e init);

	#ifdef USE_ASSER_MULTI_POINT
		void ASSER_goto_multi_point (stack_id_e stack_id, bool_e init);
		void ASSER_goto_multi_point_until_break(stack_id_e stack_id, bool_e init);
		void ASSER_push_goto_multi_point (Sint16 x, Sint16 y, ASSER_speed_e speed, way_e way, Uint8 curve, Uint8 priority_order, ASSER_end_condition_e end_condition, bool_e run);
		void ASSER_relative_goangle_multi_point (stack_id_e stack_id, bool_e init);
		void ASSER_push_relative_goangle_multi_point (Sint16 angle, ASSER_speed_e speed, bool_e run);
	#endif /* def USE_ASSER_MULTI_POINT */
	
	/* Va a l'angle indiquée, se termine à l'arret */
	void ASSER_goangle (stack_id_e stack_id, bool_e init);

	
	/* Tourne par rapport à l'angle actuel, se termine à l'arret */
	void ASSER_relative_goangle (stack_id_e stack_id, bool_e init);
	
	/* Va se caler contre le mur */
	void ASSER_rush_in_the_wall(stack_id_e stack_id, bool_e init);
	
	/* Arrête le robot, ne touche pas à la pile */
	void ASSER_push_stop ();
	//Ancienne fonction n'utilisant pas la pile.
	void ASSER_stop () __attribute__ ((deprecated)); //veuillez utiliser ASSER_push_stop maintenant
	
	/* ajoute une instruction goto sur la pile asser */
	void ASSER_push_goto (Sint16 x, Sint16 y, ASSER_speed_e speed, way_e way, Uint8 curve, ASSER_end_condition_e end_condition, bool_e run);
	
	/* ajoute une instruction goangle sur la pile asser */
	void ASSER_push_goangle (Sint16 angle, ASSER_speed_e speed, bool_e run);
	
	/* ajoute une instruction relative_goangle sur la pile asser */
	void ASSER_push_relative_goangle (Sint16 angle, ASSER_speed_e speed, bool_e run);
	
	void ASSER_push_relative_goto(Sint16 x, Sint16 y, ASSER_speed_e speed, way_e way, Uint8 curve, bool_e run);
	
	/* ajoute une instruction rush_in_the_wall sur la pile asser */
	void ASSER_push_rush_in_the_wall (way_e way, bool_e asser_rotate,Sint16 angle, bool_e run);
	
	void ASSER_WARNER_arm_x(Sint16 x);
	
	void ASSER_WARNER_arm_y(Sint16 y);
	
	void ASSER_WARNER_arm_teta(Sint16 teta);
	
	/* fonction retournant si on se situe à moins de 15 cm de la destination.
	Fonctionne en distance Manhattan */
	bool_e ASSER_near_destination();
	
	/* fonction retournant si on se situe à moins de 2 degrés cm de la destination. */
	bool_e ASSER_near_destination_angle();
	
	/* Regarde si la pile contient un goto vers (x, y) */
	bool_e ASSER_has_goto(Sint16 x, Sint16 y);
	
	/* Affiche le contenu formaté de la pile asser, le haut de la pile en premier */
	void ASSER_dump_stack ();
	
	#ifdef ASSER_FUNCTIONS_C
	
		#include "QS/QS_can.h"
		#include "QS/QS_CANmsgList.h"
		#include "Geometry.h"
		
		typedef enum
		{CONFIG, XMSB, XLSB, YMSB, YLSB, VITESSE, MARCHE, RAYONCRB} ASSER_CAN_msg_bytes_e;
	
		// Timeout en ms
		#define GOTO_TIMEOUT_TIME							5000	// On n'attend que 3 secondes sur les ASSER_push_goto cette année
																	// car on ne fait pas de gros déplacements sur le terrain
			
		#define GOTO_MULTI_POINT_TIMEOUT_TIME				4000	//Nombre de secondes de timeout PAR POINT en mode multipoint.
		#define RELATIVE_GOANGLE_MULTI_POINT_TIMEOUT_TIME	3000
				
		#define RUSH_TIMEOUT_TIME							2500
		// suffisant pour un demi tour en vitesse lente
		#define GOANGLE_TIMEOUT_TIME						6000
		
		// Coefficient multiplicatif de la distance pour obtenir le temps avant le timeout
		#define COEFF_TIMEOUT_GOTO_MULTI_POINT_FAST			4 
		#define COEFF_TIMEOUT_GOTO_MULTI_POINT_SLOW			8 	
		
		#ifdef DEBUG_ASSER_FUN
			#define asser_fun_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define asser_fun_printf(...)	(void)0
		#endif
		
	#endif /* def ASSER_FUNCTIONS_C */
	
#endif /* ndef ASSER_FUNCTIONS_H */
