#include "../QS/QS_all.h"
#include "../../elements.h"

#ifndef ACTION_BIG_H
	#define ACTION_BIG_H

	// action_big_common
	error_e sub_harry_initiale();
	error_e sub_harry_end_of_match();

	// actions_prise_minerais
	error_e sub_harry_take_big_crater(ELEMENTS_property_e mimerais);
	error_e sub_harry_take_big_crater_move_pos_yellow();
	error_e sub_harry_take_big_crater_move_pos_blue();

	error_e sub_harry_take_big_crater_yellow_middle();
	error_e sub_harry_take_big_crater_yellow_fuse();
	error_e sub_harry_take_big_crater_yellow_corner();
	error_e sub_harry_take_big_crater_blue_middle();
	error_e sub_harry_take_big_crater_blue_fuse();
	error_e sub_harry_take_big_crater_blue_corner();

	// action_big_depose_minerais
	error_e sub_harry_depose_minerais();

	// action_big_prise_modules
	error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules);
	error_e sub_harry_fusee_color();
	error_e init_all_actionneur();
	error_e sub_harry_fusee_multicolor(ELEMENTS_property_e fusee, bool_e right_side);


	// action_big_depose_modules
	error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules);
	error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules);

#endif

