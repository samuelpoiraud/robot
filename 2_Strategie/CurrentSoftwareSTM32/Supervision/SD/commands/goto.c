#include "goto.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../prop_functions.h"
#include "../../../QS/QS_all.h"

const char term_cmd_goto_brief[] = "D�place le robot � la position voulue";
const char term_cmd_goto_help[] =
		"Utilisation : goto x y\n"
		"\n"
		"x          position en X\n"
		"y          position en Y\n";


int term_cmd_goto(int argc, const char *argv[]) {
	long x, y;

	if(argc < 2)
		return EINVAL;
	else if(argc > 2)
		return EINVAL;

	if(!argtolong(argv[0], 0, &x))
		return EINVAL;

	if(!argtolong(argv[1], 0, &y))
		return EINVAL;

	if(x >= 200 && x <= 1800 && y >= 200 && y <= 2800){
			if(global.env.pos.x < 250 || global.env.pos.x > 1750 || global.env.pos.y < 250 || global.env.pos.y > 2750)
				PROP_push_goto(x, y, FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BREAK, FALSE, FALSE);
			else
				PROP_push_goto(x, y, FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BREAK, FALSE, TRUE);
	}else
		return EINVAL;

	if(global.env.pos.x < 250)
		PROP_push_goto(250, global.env.pos.y, FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BREAK, FALSE, TRUE);
	else if(global.env.pos.x > 1750)
		PROP_push_goto(1750, global.env.pos.y, FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BREAK, FALSE, TRUE);

	if(global.env.pos.y < 250)
		PROP_push_goto(global.env.pos.x, 250, FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BREAK, FALSE, TRUE);
	else if(global.env.pos.y > 2750)
		PROP_push_goto(global.env.pos.x, 2750, FAST, ANY_WAY, 0, AVOID_DISABLED, END_AT_BREAK, FALSE, TRUE);


	else
		return EINVAL;

	return 0;
}
