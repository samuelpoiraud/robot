#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"

	typedef enum{
		OUR_BLOC,
		ADV_BLOC
	}Sand_bloc_e;

	typedef enum	{

		ELEMENTS_FLAGS_NB
	}elements_flags_e;

void ELEMENTS_init();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);


#endif // ELEMENTS_H
