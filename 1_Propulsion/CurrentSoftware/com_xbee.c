#include "../QS/QS_outputlog.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_CANmsgList.h"
#include "com_xbee.h"

#define WAIT_TIME_POSITION         800

static ELEMENTS_info_wood_protection infoWood = {0,0,0,0,FALSE};
static watchdog_id_t watchdog_position_id = 255;


void holly_timeout_wood_protection(void);


void holly_receive_wood_position(CAN_msg_t * msg)
{
	if(msg->sid != PROP_WOOD_PROTECT_ZONE)
		return;

	if(watchdog_position_id != 255)
	{
		WATCHDOG_stop(watchdog_position_id);	//on arrête un éventuel watchdog précédemment lancé.
		watchdog_position_id = 255;
	}

	infoWood.x1=msg->data.prop_wood_protect_zone.x1;
	infoWood.x2=msg->data.prop_wood_protect_zone.x2;
	infoWood.y1=msg->data.prop_wood_protect_zone.y1;
	infoWood.y2=msg->data.prop_wood_protect_zone.y2;

	if(infoWood.x1 || infoWood.x2 || infoWood.y1 || infoWood.y2)
	{	//Le rectangle de protection est non nul : wood protège
		infoWood.onTheDefensive = TRUE;
		watchdog_position_id = WATCHDOG_create(WAIT_TIME_POSITION, &holly_timeout_wood_protection,FALSE);
		//Le lancement du watchdog permet d'assurer une durée de vie limitée à la levée du flag onTheDefensive
	}
	else	//Rectangle d'évitement nul : plus de protection par wood
		infoWood.onTheDefensive = FALSE;
}

//Fonction appelée en IT par le watchdog......
//Objectif : faire en sorte que le bool_e onTheDefensive ne puisse être levé en absence de réception périodique de message plus longtemps que WAIT_TIME_POSITION
void holly_timeout_wood_protection(void)
{
	if(infoWood.onTheDefensive)
		infoWood.onTheDefensive = FALSE;
}


bool_e get_wood_state_defensive()
{
	return infoWood.onTheDefensive;
}

ELEMENTS_info_wood_protection * get_info_wood_protection(void)
{
	return &infoWood;
}

bool_e is_point_protected_by_wood(GEOMETRY_point_t p)
{
	if(infoWood.onTheDefensive && is_in_square(infoWood.x1,infoWood.x2,infoWood.y1,infoWood.y2,p))
		return TRUE;
	return FALSE;
}
