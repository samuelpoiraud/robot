#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"
#include "propulsion/movement.h"


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

void ELEMENTS_init();
void ELEMENTS_process_main();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);

error_e ELEMENTS_check_communication(CAN_msg_t * msg);

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif


#endif // ELEMENTS_H
