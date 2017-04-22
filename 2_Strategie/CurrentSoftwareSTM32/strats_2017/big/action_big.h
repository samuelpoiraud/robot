#include "../QS/QS_all.h"
#include "../../elements.h"

#ifndef ACTION_BIG_H
	#define ACTION_BIG_H

	#define ACT_TRIHOLE_SPEED_RUN		20	// [rpm]	Maximum théorique 526 RPM
	#define ACT_ROLLER_FOAM_SPEED_RUN	280	// [rpm]	Maximum théorique 510 RPM, Maximum expérimental 310

	#define TRIHOLE_DISPOSE_TIME		5000

	// action_big_common
	error_e sub_harry_initiale();
	error_e sub_harry_end_of_match();

	// actions_prise_minerais
	error_e sub_harry_take_big_crater(ELEMENTS_property_e mimerais);
	error_e sub_harry_take_big_crater_move_pos_yellow();
	error_e sub_harry_take_big_crater_move_pos_blue();

	error_e sub_harry_take_big_crater_yellow_middle();
	error_e sub_harry_take_big_crater_yellow_rocket();
	error_e sub_harry_take_big_crater_yellow_corner();
	error_e sub_harry_take_big_crater_blue_middle();
	error_e sub_harry_take_big_crater_blue_rocket();
	error_e sub_harry_take_big_crater_blue_corner();
	error_e sub_harry_take_north_little_crater(ELEMENTS_property_e minerais);
	error_e sub_harry_get_in_north_little_crater(ELEMENTS_property_e minerais);
	error_e sub_harry_take_south_little_crater(ELEMENTS_property_e minerais);
	error_e sub_harry_get_in_south_little_crater(ELEMENTS_property_e minerais);


	// action_big_depose_minerais
	error_e sub_harry_manager_put_off_ore(); //manager
	error_e sub_harry_depose_minerais();
	error_e sub_harry_get_in_depose_minerais();
	error_e sub_harry_shooting_depose_minerais();
	error_e sub_harry_depose_minerais_alternative();
	error_e sub_harry_get_in_depose_minerais_alternative();


	// action_big_prise_modules
	error_e sub_push_modules_bretagne();
	//error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules, bool_e threeModules);
	error_e sub_harry_prise_module_start_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_side_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_base_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side);
	error_e sub_harry_prise_module_unicolor_north(ELEMENTS_side_e side);
	error_e sub_harry_prise_module_unicolor_south(ELEMENTS_side_e side);
	error_e sub_harry_prise_modules_manager(const get_this_module_s list_modules[], Uint8 modules_nb);
	error_e sub_harry_rocket_monocolor();
	//error_e init_all_actionneur();
	error_e sub_harry_rocket_multicolor(ELEMENTS_property_e rocket);
	error_e sub_act_harry_take_rocket_down_to_top(moduleRocketLocation_e rocket, ELEMENTS_side_e module_very_down, ELEMENTS_side_e module_down, ELEMENTS_side_e module_top, ELEMENTS_side_e module_very_top);
	error_e sub_act_harry_mae_store_modules(moduleStockLocation_e storage, bool_e trigger);
	error_e sub_act_harry_mae_prepare_modules_for_dispose(moduleStockLocation_e storage, bool_e trigger);
	error_e sub_act_harry_mae_dispose_modules(moduleStockLocation_e storage, arg_dipose_mae_e arg_dispose);

	error_e sub_act_harry_get_module(moduleStockPosition_e pos, ELEMENTS_side_e side);
	error_e sub_harry_launch_prise_module_start_centre();
	error_e sub_harry_push_module_north();


	// action_big_depose_modules
	error_e sub_harry_cylinder_depose_manager();
	error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
	error_e sub_harry_get_in_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_match_e basis_side);
	error_e sub_harry_get_in_pos_1_depose_module_centre();
	error_e sub_harry_get_in_pos_2_depose_module_centre();
	error_e sub_harry_get_in_pos_3_depose_module_centre();
	error_e sub_harry_get_in_pos_4_depose_module_centre();
	error_e sub_harry_get_in_pos_5_depose_module_centre();
	error_e sub_harry_get_in_pos_6_depose_module_centre();
	error_e sub_harry_get_1_depose_module_centre();
	error_e sub_harry_get_2_depose_module_centre();
	error_e sub_harry_get_3_depose_module_centre();
	error_e sub_harry_get_4_depose_module_centre();
	error_e sub_harry_get_5_depose_module_centre();
	error_e sub_harry_get_6_depose_module_centre();
	error_e sub_harry_take_big_crater_blue();
	error_e sub_harry_take_big_crater_yellow();
	error_e sub_harry_depose_modules_side_manager(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
	error_e sub_harry_depose_modules_side_intro(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
	error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side,POINT_DE_ROTATION_config_e mon_point_de_rotation,endroit_depose_config_e endroit_depose,COTE_DE_DEPOSE_config_e cote_depose);
	error_e sub_harry_depose_modules_side_fin(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
	error_e sub_harry_return_modules(ELEMENTS_side_e side);
	//error_e manager_return_modules(); //Fonction à ne pas utiliser car dangeureuse et très incomplète

#endif

