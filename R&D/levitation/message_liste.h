#ifndef MESSAGES_LISTE_H
#define MESSAGES_LISTE_H

#include "macro_types.h"
#include"mySDLlib.h"

	void position_adversaire_CAN(CAN_msg_t * msg, SDL_Rect * pos, SDL_Event *event);

	void set_red_CAN(CAN_msg_t * msg);

	void set_blue_CAN(CAN_msg_t * msg);

	void calibrage_CAN(CAN_msg_t * msg);

	void gotopos_CAN(CAN_msg_t * msg, SDL_Rect * pos, SDL_Event *event);

	void asser_stop_CAN(CAN_msg_t * msg);

#endif //def MESSAGES_LISTE_H

