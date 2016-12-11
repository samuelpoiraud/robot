#include "QS/QS_all.h"

#ifndef ENVIRONMENT_H
	#define ENVIRONMENT_H

	/* #define communs à plusieurs modules */
	#define MATCH_DURATION 				90000UL //90 secondes)
	#define GAME_ZONE_SIZE_X			2000 //2000 mm
	#define GAME_ZONE_SIZE_Y			3000 //3000 mm

	void ENVIRONMENT_init();

	void ENVIRONMENT_processIt(Uint8 ms);

	void ENVIRONMENT_processMain();

	void ENVIRONMENT_setColor(color_e color);


#endif /* ndef ENVIRONMENT_H */
