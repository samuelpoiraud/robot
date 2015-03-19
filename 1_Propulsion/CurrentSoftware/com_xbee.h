#define COM_XBEE
#ifdef COM_XBEE

#include "../QS/QS_all.h"
#include "../QS/QS_watchdog.h"


typedef struct{
   Uint16 x,y;
   bool_e onTheDefensive;
   bool_e dataSend;
}ELEMENTS_info_wood_protection;


/*		0	: Indique si wood est en train de défendre
 * 		1	: x HIGH bit
 * 		2	: x LOW bit
 * 		3	: y HIGH bit
 *		4	: y LOW bit
 */
void holly_receive_wood_position(CAN_msg_t *msg);

Uint16 get_wood_position_x();
Uint16 get_wood_position_y();
bool_e get_wood_state_defensive();

#endif
