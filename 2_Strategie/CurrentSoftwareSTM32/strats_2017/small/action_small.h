#include "../QS/QS_all.h"
#include "../../elements.h"

#ifndef ACTION_SMALL_H
	#define ACTION_SMALL_H

	// action_big_common
	error_e sub_anne_initiale(void);
	error_e sub_anne_chose_moonbase_and_dispose_modules(void);
	error_e sub_anne_chose_moonbase_and_turn_modules(void);
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
	error_e sub_act_anne_take_rocket_down_to_top(moduleRocketLocation_e rocket, Uint8 max_modules_to_take);
	error_e sub_act_anne_mae_store_modules(bool_e trigger);
	error_e sub_act_anne_mae_prepare_modules_for_dispose(bool_e trigger);
	error_e sub_act_anne_mae_dispose_modules(arg_dipose_mae_e arg_dispose);
	error_e sub_act_anne_mae_store_last_module(bool_e trigger);

	//Belgique
	error_e sub_anne_homologation_belgique();


	// action_small_depose_modules
	error_e sub_anne_depose_modules_centre(moduleMoonbaseLocation_e moonbase, ELEMENTS_side_match_e basis_side);
	error_e sub_anne_dispose_modules_side(ELEMENTS_side_match_e side);

	error_e sub_anne_get_in_pos_1_depose_module_centre();
	error_e sub_anne_get_in_pos_2_depose_module_centre();
	error_e sub_anne_get_in_pos_3_depose_module_centre();
	error_e sub_anne_get_in_pos_4_depose_module_centre();
	error_e sub_anne_get_in_pos_5_depose_module_centre();
	error_e sub_anne_get_in_pos_6_depose_module_centre();




	// action_small_turn_modules
	error_e sub_act_anne_return_module();
	error_e sub_anne_turn_modules_centre(moduleMoonbaseLocation_e moonbase, ELEMENTS_side_match_e basis_side);
	error_e sub_anne_turn_modules_side(ELEMENTS_side_match_e side);



#endif

