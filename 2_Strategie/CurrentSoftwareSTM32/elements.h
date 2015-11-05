#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"

	#define NB_SPOT		16
	#define NB_CUP		5


	typedef enum	{

		ELEMENTS_FLAGS_NB
	}elements_flags_e;

void ELEMENTS_init();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);


#endif // ELEMENTS_H
