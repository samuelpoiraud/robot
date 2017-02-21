#include "QS/QS_all.h"

#ifndef ENVIRONMENT_H
	#define ENVIRONMENT_H

	/* #define communs � plusieurs modules */
	#define MATCH_DURATION 				90000UL //90 secondes)
	#define GAME_ZONE_SIZE_X			2000 //2000 mm
	#define GAME_ZONE_SIZE_Y			3000 //3000 mm

	// Macros permettant de sym�triser le terrain
	#define COLOR_Y(y)		((global.current_color == BOT_COLOR) ? (y) : (GAME_ZONE_SIZE_Y - (y)))

	void ENVIRONMENT_init();

	void ENVIRONMENT_processIt(Uint8 ms);

	void ENVIRONMENT_processMain();

	void ENVIRONMENT_setColor(color_e color);

	color_e ENVIRONMENT_getColor();


#endif /* ndef ENVIRONMENT_H */
