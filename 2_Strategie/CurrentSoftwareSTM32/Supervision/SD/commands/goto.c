#include "goto.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../asser_functions.h"
#include "../../../QS/QS_all.h"

const char term_cmd_goto_brief[] = "Déplace le robot à la position voulue";
const char term_cmd_goto_help[] =
		"Utilisation : goto x y\n"
		"\n"
		"x          position en X\n"
		"y          position en Y\n";


int term_cmd_goto(int argc, const char *argv[]) {
	long x, y;

	if(argc < 2)
		return EINVAL;

	if(!argtolong(argv[0], 0, &x))
		return EINVAL;

	if(!argtolong(argv[1], 0, &y))
		return EINVAL;

	if(x >= 200 && x <= 1800 && y >= 200 && y <= 2800)
			ASSER_push_goto(x, y, FAST, ANY_WAY, 0, END_AT_BREAK, FALSE);

	if(global.env.pos.x < 250)
		ASSER_push_goto(250, global.env.pos.y, FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
	else if(global.env.pos.x > 1750)
		ASSER_push_goto(1750, global.env.pos.y, FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);

	if(global.env.pos.y < 250)
		ASSER_push_goto(global.env.pos.x, 250, FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);
	else if(global.env.pos.y > 2750)
		ASSER_push_goto(global.env.pos.x, 2750, FAST, ANY_WAY, 0, END_AT_BREAK, TRUE);


	else
		return EINVAL;

	return 0;
}
