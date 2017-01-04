#include "avoidance.h"
#include "../propulsion/movement.h"
#include "elements.h"
#include "QS/QS_can.h"

#define LOG_PREFIX "avoid: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_AVOIDANCE
#include "../QS/QS_outputlog.h"

void FOE_ANALYSER_init(){

}

void FOE_ANALYSER_process_main(){
#ifdef USE_FOE_ANALYSER
	if(global.flags.match_started && ELEMENTS_get_flag(F_ADV_START_ZONE_BLOC_TAKEN) == FALSE && foe_in_square(FALSE, 700, 1100, COLOR_Y(2150), COLOR_Y(2550), FOE_TYPE_HOKUYO)){
		ELEMENTS_set_flag(F_ADV_START_ZONE_BLOC_TAKEN, TRUE);
	}
#endif
}

// Vérifie adversaire dans NORTH_US, NORTH_FOE, SOUTH_US, SOUTH_FOE
foe_pos_e AVOIDANCE_where_is_foe(Uint8 foe_id)
{
	assert(foe_id < MAX_NB_FOES);

	if(global.foe[foe_id].x > 1000)
	{
		// Partie SUD
		if(COLOR_Y(global.foe[foe_id].y) > 1500)
		{
			return SOUTH_FOE;
		}
		else
		{
			return SOUTH_US;
		}
	}
	else
	{
		if(COLOR_Y(global.foe[foe_id].y) > 1500)
		{
			return NORTH_FOE;
		}
		else
		{
			return NORTH_US;
		}
	}
}

//Retourne si un adversaire est dans le carré dont deux coins 1 et 2 sont passés en paramètres
bool_e foe_in_square(bool_e verbose, Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, foe_type_e foe_type)
{
	Uint8 i;
	for (i=0; i<MAX_NB_FOES; i++)
	{
		if(
			(i < MAX_HOKUYO_FOES && (foe_type == FOE_TYPE_ALL || foe_type == FOE_TYPE_HOKUYO))
			||
			(i >= MAX_HOKUYO_FOES && (foe_type == FOE_TYPE_ALL || foe_type == FOE_TYPE_IR))
			){
			if (global.foe[i].enable)
			{
				if(is_in_square(x1,x2,y1,y2,(GEOMETRY_point_t){global.foe[i].x,global.foe[i].y}))
				{
					if(verbose)
						info_printf("FOE %d[%d;%d] found in zone x[%d->%d] y[%d->%d]\n",i,global.foe[i].x,global.foe[i].y,x1,x2,y1,y2);
					return TRUE;
				}
			}
		}
	}
	if(verbose)
		info_printf("NO FOE found in zone x[%d->%d] y[%d->%d]\n",x1,x2,y1,y2);
	return FALSE;
}

void AVOIDANCE_forced_foe_dected(){
	CAN_msg_t msg;
	msg.sid = PROP_DEBUG_FORCED_FOE;
	msg.size = 0;
	CAN_send(&msg);
}

void debug_foe_reason(foe_origin_e origin, Sint16 angle, Sint16 distance){
	CAN_msg_t msg_to_send;
	msg_to_send.sid = DEBUG_FOE_REASON;
	msg_to_send.size = SIZE_DEBUG_FOE_REASON;
	msg_to_send.data.debug_foe_reason.foe_origine = origin;
	msg_to_send.data.debug_foe_reason.angle = angle;
	msg_to_send.data.debug_foe_reason.dist = distance;
	CAN_send(&msg_to_send);
}
