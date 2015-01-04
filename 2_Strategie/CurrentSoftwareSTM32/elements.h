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

	typedef struct{
		ELEMENTS_state_s state;
		color_e color;
		Uint16 x;
		Uint16 y;
	}ELEMENTS_element_t;

	typedef enum{
		OUR_ELEMENT = 0,
		ADV_ELEMENT
	}element_group_e;


	typedef enum
	{
		OUR_BORDER_DISPENSER = 0,
		OUR_STAIRS_DISPENSER,
		ADVERSARY_STAIRS_DISPENSER,
		ADVERSARY_BORDER_DISPENSER,
		NB_DISPENSERS
	}elements_dispenser_e;

	typedef enum
	{
		STORAGE_CENTER_CUP,					//WOOD OR HOOLY
		STORAGE_LEFT_CUP,					//WOOD ONLY
		STORAGE_RIGHT_CUP,					//WOOD ONLY
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
		BORDER_CLAP,
		ESTRAD_CLAP,
		IN_ADVERSARY_ZONE_CLAP,
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


// Récupérer l'état ET les coordonnées d'un distributeur de popcorns dont l'id est passé en paramètre.
void ELEMENTS_get_dispenser_state(elements_flags_e dispenser_id, bool_e * dispenser_done, Uint16 * x, Uint16 * y);
// Récupérer l'état ET les coordonnées d'un clap dont l'id est passé en paramètre.
void ELEMENTS_get_clap_state(elements_flags_e clap_id, bool_e * clap_done,  Uint16 * x, Uint16 * y);


#endif // ELEMENTS_H
