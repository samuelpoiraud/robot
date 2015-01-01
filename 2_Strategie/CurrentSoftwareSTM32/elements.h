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
	}ELEMENTS_element_s;

	typedef enum{
		OUR_ELEMENT,
		ADV_ELEMENT
	}element_group_e;

void ELEMENTS_init();
void visit_salle_Cinema(Uint8 salle_Number);
bool_e salle_Cinema_Available(Uint8 salle_Number);

#endif // ELEMENTS_H
