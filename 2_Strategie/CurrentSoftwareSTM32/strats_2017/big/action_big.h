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
	error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules, bool_e threeModules);
	error_e sub_harry_rocket_monocolor();
	error_e init_all_actionneur(moduleStockLocation_e nb_cylinder_big_right,moduleStockLocation_e nb_cylinder_big_left);
	error_e sub_harry_rocket_multicolor(ELEMENTS_property_e fusee, bool_e right_side);
	error_e boucle_charge_module(moduleStockLocation_e nb_cylinder_big_right,moduleStockLocation_e nb_cylinder_big_left,moduleRocketLocation_e nb_cylinder_fusee, moduleType_e myModule,  bool_e right_side );


	// action_big_depose_modules
	error_e sub_harry_cylinder_depose_manager();
	error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
	error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules);
	error_e sub_harry_return_modules(ELEMENTS_side_e side);
	error_e manager_return_modules();

#endif

