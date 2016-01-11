#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"

	typedef enum{
		OUR_BLOC,
		ADV_BLOC
	}Sand_bloc_e;

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
		FISHS_DISPOSED,
		SEASHELLS_BOTTOM,
		OUR_BLOCK8_DEPOSED,
		ADV_BLOCK8_DEPOSED,
		ELEMENTS_FLAGS_NB
	}elements_flags_e;

void ELEMENTS_init();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);


#endif // ELEMENTS_H
