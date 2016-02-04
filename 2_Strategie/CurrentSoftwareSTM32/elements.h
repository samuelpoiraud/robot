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
		OUR_START_ZONE_BLOC,		//on a pris notre bloc de 4
		ADV_START_ZONE_BLOC,		//on a pris le bloc de 4 adv
		OUR_DUNE_BLOC,				//on a pris notre bloc de 8
		ADV_DUNE_BLOC,				//on a pris le bloc de 8 adv
		FISHS_TAKEN,				//poisson embarqué
		SEASHELLS_BOTTOM,			//si les deux coquillages du bas sont pris ou pas
		OUR_BLOCK8_DEPOSED,			//on a depose notre bloc de 8
		ADV_BLOCK8_DEPOSED,			//on a depose le bloc de 8 adv
		OUR_BLOC_PRESENT,			//on a vu (au scan) notre bloc de 8
		ADV_BLOC_PRESENT,			//on a vu (au scan) le bloc de 8 adv
		APOCALYPSE_SCAN,			//on a scanné l'apocalypse (on est passé voir si il y a quelque chose
		APOCALYPSE_PRESENT,			//on a trouvé l'apocalypse
		DUNE_TAKEN,					//on a pris la dune
		APOCALYPSE_TAKEN,			//on a pris l'apocalypse
		DEPOSE_BLOC_ERROR,			//on est dans le cas d'erreur de la depose (si on a pas pu pousser le bloc precedent)
		DEPOSE_BLOC_DOUBLE_ERROR,	//on a pas pu pousser deux fois de suite
		BLOC_IN_ZONE,				//indique si il y a dejà un bloc de déposé dans la zone de construction

		ELEMENTS_FLAGS_END_SYNCH,	//Les flags au-dessus seront synchro entre les deux robots

		SAND_DUNE_FRONT,			//flag ppour savoir si on a des blocs de sable à l'avant du robot: A mettre à jour !!!
		SAND_DUNE_BACK,				//flag ppour savoir si on a des blocs de sable à l'arriere du robot: A mettre à jour !!!
		SAND_BLOC_FRONT,			//flag ppour savoir si on a un bloc de sable à l'avant de pearl

		COMMUNICATION_AVAILABLE,		
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
