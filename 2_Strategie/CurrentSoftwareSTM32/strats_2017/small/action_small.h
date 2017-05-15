#include "../QS/QS_all.h"
#include "../../elements.h"

#ifndef ACTION_SMALL_H
	#define ACTION_SMALL_H

	// action_big_common
	error_e sub_anne_initiale(void);
	error_e sub_anne_chose_moonbase_and_dispose_modules(void);
	error_e sub_anne_end_of_match(void);

	// actions_prise_minerais
	error_e sub_anne_prise_gros_cratere(ELEMENTS_property_e mimerais);

	// action_big_depose_minerais
	error_e sub_anne_depose_minerais(void);

	// action_big_prise_modules
	error_e sub_anne_prise_modules_centre(ELEMENTS_property_e modules);
	error_e sub_harry_prise_module_start_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_side_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_base_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_unicolor_north(ELEMENTS_side_e side);
	error_e sub_harry_prise_module_unicolor_south(ELEMENTS_side_e side);
	error_e sub_anne_fusee_color();
	error_e sub_anne_fusee_multicolor(ELEMENTS_property_e fusee);
	error_e sub_act_anne_take_rocket_down_to_top(moduleRocketLocation_e rocket, bool_e module_very_down, bool_e module_down, bool_e module_top, bool_e module_very_top);
	error_e sub_act_anne_mae_store_modules(bool_e trigger);
	error_e sub_act_anne_mae_prepare_modules_for_dispose(bool_e trigger);
	error_e sub_act_anne_mae_dispose_modules(arg_dipose_mae_e arg_dispose);


	//Belgique
	error_e sub_anne_homologation_belgique();


	// action_big_depose_modules
	error_e sub_anne_depose_modules_centre(moduleTypeDominating_e modules_type, moduleStockLocation_e robot_side, ELEMENTS_side_match_e basis_side);
	error_e sub_anne_get_in_depose_modules_centre(moduleTypeDominating_e module_type, moduleStockLocation_e robot_side, ELEMENTS_side_match_e basis_side);

	error_e sub_anne_depose_modules_side(ELEMENTS_property_e modules);
	error_e sub_anne_depose_modules_side_fin(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);

	error_e sub_anne_get_in_pos_1_depose_module_centre();
	error_e sub_anne_get_in_pos_2_depose_module_centre();
	error_e sub_anne_get_in_pos_3_depose_module_centre();
	error_e sub_anne_get_in_pos_4_depose_module_centre();
	error_e sub_anne_get_in_pos_5_depose_module_centre();
	error_e sub_anne_get_in_pos_6_depose_module_centre();

	error_e sub_anne_return_modules_centre_get_in(ELEMENTS_property_e modules);
	error_e sub_act_anne_return_module();
	error_e sub_anne_manager_return_modules(ELEMENTS_property_e modules);
	error_e sub_anne_return_modules_centre(ELEMENTS_property_e modules);
	error_e sub_anne_return_modules_side_get_in(ELEMENTS_property_e modules);


	error_e sub_anne_dispose_modules_side(ELEMENTS_property_e modules);



#endif

