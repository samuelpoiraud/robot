#define COM_XBEE
#ifdef COM_XBEE

#include "../QS/QS_all.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_maths.h"

typedef struct{
   Uint16 x1, y1, x2, y2;	//rectangle de défense protégé par wood...
   bool_e onTheDefensive;	//activation de la défense
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

//Return TRUE si le point p est actuellement protégé par Wood... FALSE sinon.
bool_e is_point_protected_by_wood(GEOMETRY_point_t p);


#endif
