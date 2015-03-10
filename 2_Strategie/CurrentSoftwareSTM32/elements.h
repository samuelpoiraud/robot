#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"

	#define NB_SPOT		16
	#define NB_CUP		5



	typedef enum{
		AVAILABLE,
		TAKE,
		LOST
	}ELEMENTS_state_s;

	typedef enum{
		NORTH,
		SOUTH
	}ELEMENTS_order_s;

	typedef enum{
		ACTIVATE,
		DISABLE_SEND,
		AUTO_SEND,
		NO_SET
	}ELEMENTS_protect_s;

	typedef struct{
		ELEMENTS_state_s state;
		color_e color;
		Uint16 x;
		Uint16 y;
	}ELEMENTS_element_t;

	typedef enum{
		DISPOSE_SPOT_RIGHT,
		DISPOSE_SPOT_LEFT,
		DISPOSE_SPOT_RIGHT_AND_LEFT
	}ELEMENTS_dispose_spot_e;

	typedef enum{
		OUR_ELEMENT = 0,
		ADV_ELEMENT
	}element_group_e;


	typedef struct{
	   Uint16 x,y;
	   ELEMENTS_state_s state_cup;
	}ELEMENTS_position;


	typedef enum
	{
		OUR_BORDER_DISPENSER = 0,
		OUR_STAIRS_DISPENSER,
		ADVERSARY_STAIRS_DISPENSER,
		ADVERSARY_BORDER_DISPENSER,
		NB_DISPENSERS
	}elements_dispenser_e;

	typedef enum	{

		HOLLY_STORAGE_CENTER_CUP,					//HOOLY
		WOOD_STORAGE_LEFT_CUP,					//WOOD ONLY
		WOOD_STORAGE_RIGHT_CUP,					//WOOD ONLY
		STORAGE_POPCORNS,
		HOLLY_LEFT_REAR_SUCKER_FULL,		//Un pied dans la ventouse arrière gauche
		HOLLY_RIGHT_REAR_SUCKER_FULL,		//Un pied dans la ventouse arrière droite
		HOLLY_DISPOSED_CUP_IN_START_ZONE,
		HOLLY_DISPOSED_SPOT_IN_START_ZONE,
		HOLLY_DISPOSED_SPOT_IN_ESTRAD,
		HOLLY_DISPOSED_FIRST_CARPET,
		HOLLY_DISPOSED_SECOND_CARPET,
		HOLLY_DISPOSED_POPCORN_IN_BAC,
		HOLLY_STOLE_NORTH_ADVERSARY_CUP,	//vol dans la salle de cinéma adverse du NORD.
		HOLLY_STOLE_SOUTH_ADVERSARY_CUP,	//vol dans la salle de cinéma adverse du SUD.
		WOOD_DISPOSED_CUP_IN_SOUTH_ADVERSARY_CINEMA,
		WOOD_DISPOSED_CUP_IN_NORTH_ADVERSARY_CINEMA,
		WOOD_DISPOSED_STOLEN_CUP_IN_SOUTH_ADVERSARY_CINEMA,
		WOOD_DISPOSED_STOLEN_CUP_IN_NORTH_ADVERSARY_CINEMA,
		ELEMENTS_POPCORN,
		ELEMENTS_ADV_POPCORN,
		ELEMENTS_BORDER_CLAP,
		ELEMENTS_ESTRAD_CLAP,
		ELEMENTS_IN_ADVERSARY_ZONE_CLAP,
		ELEMENTS_NORTH_CORNER_FOOT_TOOK,
		ELEMENTS_NORTH_STAIRS_FEET_TOOK,
		ELEMENTS_MIDDLE_FEET_TOOK,
		ELEMENTS_ESTRAD_FOOT_TOOK,
		ELEMENTS_SOUTH_CUP_TOOK,
		ELEMENTS_SOUTH_CORNER_FEET_TOOK,
		ELEMENTS_PROTECT_MESSAGE,
		ELEMENTS_FLAGS_PROTECT_SOUTH,
		ELEMENTS_FLAGS_PROTECT_NORTH,
		ELEMENTS_STEAL_CUP_IN_NORTH_CINEMA,
		ELEMENTS_STEAL_CUP_IN_SOUTH_CINEMA,
		ELEMENTS_FLAGS_NB
	}elements_flags_e;

void ELEMENTS_init();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);

Uint8 ELEMENTS_get_holly_left_spot_level(void);
void ELEMENTS_inc_holly_left_spot_level(void);
void ELEMENTS_reset_holly_left_spot_level(void);
Uint8 ELEMENTS_get_holly_right_spot_level(void);
void ELEMENTS_inc_holly_right_spot_level(void);
void ELEMENTS_reset_holly_right_spot_level(void);
void ELEMENTS_set_cup(Uint8 number, ELEMENTS_state_s stateCup);
ELEMENTS_state_s ELEMENTS_get_cup(Uint8 number);
bool_e get_cup_transmission(void);
Uint8 get_number_cup(void);
void copy_cup_position(ELEMENTS_position cup_copy[]);

// Récupérer l'état ET les coordonnées d'un distributeur de popcorns dont l'id est passé en paramètre.
void ELEMENTS_get_dispenser_state(elements_flags_e dispenser_id, bool_e * dispenser_done, Uint16 * x, Uint16 * y);

//Message CAN de récupération du centre des gobelets
 void collect_cup_coord( CAN_msg_t *msg);


#endif // ELEMENTS_H
