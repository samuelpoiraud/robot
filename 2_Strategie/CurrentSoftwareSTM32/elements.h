#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"
#include "propulsion/movement.h"


	typedef struct{
		Sint16 xmin;
		Sint16 xmax;
		Sint16 ymin;
		Sint16 ymax;
		bool_e enable_zone;
		Uint8 nb_cylinder_max;
		const Uint8 nb_cylinder;
	}module_zone_characteristics_s;


	typedef enum{
		NO_ELEMENT,
		OUR_ELEMENT,
		ADV_ELEMENT,
		NEUTRAL_ELEMENT
	}ELEMENTS_property_e;


	typedef struct{
		GEOMETRY_point_t pos;
		ELEMENTS_property_e property;
	}COQUILLAGES_t;

	typedef enum{
	   NO_COQUILLAGES_CONFIG = 0,
	   CONFIG_COQUILLAGES_1,
	   CONFIG_COQUILLAGES_2,
	   CONFIG_COQUILLAGES_3,
	   CONFIG_COQUILLAGES_4,
	   CONFIG_COQUILLAGES_5
	}COQUILLAGES_config_e;

	typedef enum{
		MODULE_EMPTY = 0,
		MODULE_BLUE,
		MODULE_YELLOW,
		MODULE_POLY
	}moduleType_e;

	typedef enum{
		MODULE_DROP_MIDDLE,
		MODULE_DROP_NORTH_CENTER,
		MODULE_DROP_SOUTH_CENTER,
		MODULE_DROP_NORTH,
		MODULE_DROP_SOUTH,
		NB_MODULE_LOCATION
	}moduleDropLocation_e;

	extern const module_zone_characteristics_s module_zone[NB_MODULE_LOCATION];
	// COMMENTAIRE POUR COCO : module_zone[first_zone].xmin

void ELEMENTS_init();
void ELEMENTS_process_main();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);

error_e ELEMENTS_check_communication(CAN_msg_t * msg);

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif


#endif // ELEMENTS_H
