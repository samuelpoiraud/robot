#include "../QS/QS_all.h"
#include "../../elements.h"

#ifndef ACTION_SMALL_H
	#define ACTION_SMALL_H

	// action_big_common
	error_e sub_anne_initiale();
	error_e sub_anne_end_of_match();

	// actions_prise_minerais
	error_e sub_anne_prise_gros_cratere(ELEMENTS_property_e mimerais);

	// action_big_depose_minerais
	error_e sub_anne_depose_minerais();

	// action_big_prise_modules
	error_e sub_anne_prise_modules_centre(ELEMENTS_property_e modules);
	error_e sub_harry_prise_module_start_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_side_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_base_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_unicolor_north(ELEMENTS_side_e side);
	error_e sub_harry_prise_module_unicolor_south(ELEMENTS_side_e side);
	error_e sub_anne_fusee_color();
	error_e sub_anne_fusee_multicolor(ELEMENTS_property_e fusee);

	// action_big_depose_modules
	error_e sub_anne_depose_modules_centre();
	error_e sub_anne_depose_modules_side(ELEMENTS_property_e modules);

#endif

