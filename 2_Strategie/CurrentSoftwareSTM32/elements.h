#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"
#include "avoidance.h"


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
void ELEMENTS_inc_fishs_passage();
Uint8 ELEMENTS_get_fishs_passage();

void ELEMENTS_inc_seashell_depose();
Uint8 ELEMENTS_get_seashell_depose();

bool_e ELEMENTS_fishs_passage_completed();
error_e ELEMENTS_check_communication(CAN_msg_t * msg);

ELEMENTS_property_e COQUILLAGE_get_property(Uint8 id);
bool_e COQUILLAGE_is_mine(Uint8 id);
bool_e COQUILLAGE_is_present(Uint8 id);
Uint8 COQUILLAGES_get_config();
bool_e COQUILLAGES_is_config(COQUILLAGES_config_e user_config);
void ELEMENTS_check_configuration_coquillages(bool_e activate);
void ELEMENTS_send_config_coquillages(CAN_msg_t *msg);

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif

void ELEMENTS_set_middle_apocalypse(Uint16 middle);
Uint16 ELEMENTS_get_middle_apocalypse();

#endif // ELEMENTS_H
