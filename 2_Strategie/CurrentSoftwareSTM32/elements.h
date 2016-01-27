#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"

	typedef enum{
		OUR_ELEMENT,
		ADV_ELEMENT
	}ELEMENTS_property_e;

	typedef enum	{
		FIRST_DOOR_CLOSED,
		SECOND_DOOR_CLOSED,
		OUR_START_ZONE_BLOC,
		ADV_START_ZONE_BLOC,
		OUR_DUNE_BLOC,
		ADV_DUNE_BLOC,
		SAND_DUNE_FRONT,  //flag ppour savoir si on a des blocs de sable à l'avant du robot: A mettre à jour !!!
		SAND_DUNE_BACK,   //flag ppour savoir si on a des blocs de sable à l'arriere du robot: A mettre à jour !!!
		FISHS_TAKEN,
		SEASHELLS_BOTTOM,
		OUR_BLOCK8_DEPOSED,
		ADV_BLOCK8_DEPOSED,
		OUR_BLOC_SCAN,
		ADV_BLOC_SCAN,
		APOCALYPSE_SCAN,
		APOCALYPSE_PRESENT,
		DUNE_TAKEN,
		APOCALYPSE_TAKEN,
		ELEMENTS_FLAGS_NB
	}elements_flags_e;


void ELEMENTS_init();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);
void ELEMENTS_inc_fishs_passage();
bool_e ELEMENTS_fishs_passage_completed();

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif


#endif // ELEMENTS_H
