
#include "actions_guy.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../high_level_strat.h"
#include "../elements.h"
#include "../Geometry.h"
#include "../Pathfind.h"
#include "../zone_mutex.h"
#include "../QS/QS_can_over_xbee.h"
#include "../act_functions.h"
#include "../config/config_pin.h"
#include "../maths_home.h"
#include "../Supervision/SD/SD.h"
#include "../Supervision/Buzzer.h"
#include "actions_both_2014.h"
#include <math.h>

// vidage du foyer adverse contenant 6 feux dont on assume la position "parfaite"
// wait_time : temps d'attente au wait_point. si 0 : pas d'attente, le point wait_point peut être NULL.
error_e sub_steal_space_crackers(GEOMETRY_point_t wait_point, time32_t wait_time, bool_e scan_before)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_STEAL_SPACE_CRACKERS,
		INIT,
		GOTO_WAIT_POINT,
		WAIT,
		GOTO_SCAN_POINT,
		SCAN,
		TAKE_OBSTACLE,
		GOTO_HEART,
		TAKE_FIRE,
		BACK_WITH_FIRE,
		RETURN_FIRE,
		DONE,
		ERROR,
		EXTRACT_FROM_HEARTH);



}
