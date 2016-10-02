#include "goto.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../propulsion/prop_functions.h"
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
	else if(argc > 2)
		return EINVAL;

	if(!argtolong(argv[0], 0, &x))
		return EINVAL;

	if(!argtolong(argv[1], 0, &y))
		return EINVAL;

	if(x >= 200 && x <= 1800 && y >= 200 && y <= 2800)
		PROP_goTo(x, y, PROP_ABSOLUTE, FAST, ANY_WAY, FALSE, END_AT_LAST_POINT, PROP_NO_CURVE, AVOID_DISABLED, PROP_NO_BORDER_MODE, 0);
	else
		return EINVAL;

	return 0;
}
