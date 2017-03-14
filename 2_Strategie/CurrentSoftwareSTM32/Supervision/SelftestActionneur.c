#include "SelftestActionneur.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can_over_uart.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_mosfet.h"
#include "../elements.h"
#include "../actuator/act_functions.h"
#include "../detection.h"
#include "SD/Libraries/fat_sd/ff.h"
#include "SD/SD.h"
#include "RTC.h"
#include "Buzzer.h"
#include "LCD_interface.h"

#include "../propulsion/movement.h"
#include "../propulsion/astar.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_types.h"
#include "../QS/QS_outputlog.h"
#include "../propulsion/astar.h"
#include "../utils/generic_functions.h"
#include "../actuator/act_functions.h"
#include "../actuator/queue.h"
#include "../utils/actionChecker.h"
#include "../elements.h"
#include "../high_level_strat.h"

#if 0

//Pour Valentin :
// Il s'agit d'une premiere version, le code n'est en rien optimise

error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules, bool_e onlyTwoModules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_CENTER,
			INIT,
			MOVE_ACTIONNEUR,
			CHECK_STATUS,
			COMPUTE_NEXT_ETAPE,
			ERROR,
			DONE
	);



	static const struct_selftest_t tableau_selftest[]  = {
			{1, ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_TOP, ACT_QUEUE_Cylinder_elevator_right},
			{1, ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_TOP, ACT_QUEUE_Cylinder_elevator_left },
			{2, ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM, ACT_QUEUE_Cylinder_elevator_right },
			{2, ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM, ACT_QUEUE_Cylinder_elevator_left },
			{3, ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT, ACT_QUEUE_Cylinder_slider_right },
			{3, ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT, ACT_QUEUE_Cylinder_slider_left },
			{3, ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UP, ACT_QUEUE_Cylinder_slope_right},
			{3, ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UP, ACT_QUEUE_Cylinder_slope_left },
			{4, ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN, ACT_QUEUE_Cylinder_slider_right },
			{4, ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN, ACT_QUEUE_Cylinder_slider_right},
			{4, ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN, ACT_QUEUE_Cylinder_slope_right},
			{4, ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN, ACT_QUEUE_Cylinder_slope_left},
			//matrice a completer avec les actionneurs arm, color et dispose
	};

	static const Uint8 nombre_actions = sizeof(tableau_selftest);
	//static error_e liste_etat_actionneur[nombre_actions];
	static error_e liste_etat_actionneur[50];
	static Uint8 indice = 0; //static?
	static Uint8 indice_check = 0;
	static Uint8 etape_en_cours = 1;

	switch(state){
		case INIT:
			for(int i=0; i<nombre_actions; i++){
				liste_etat_actionneur[i]= IN_PROGRESS;
			}
			state = MOVE_ACTIONNEUR;
			break;

		case MOVE_ACTIONNEUR:

			while( tableau_selftest[indice][0] == etape_en_cours){
				ACT_push_order(tableau_selftest[indice][1],tableau_selftest[indice][2]);
				indice += 1;
			}
			wait(1000);
			state = CHECK_STATUS;
			break;

		case CHECK_STATUS:

			while(tableau_selftest[indice_check][0] == etape_en_cours){
				liste_etat_actionneur[indice_check] = Check_at_status(tableau_selftest[indice_check][3], IN_PROGRESS, END_OK, NOT_HANDLED);
				indice_check += 1;
			}
			wait(1000);
			state = COMPUTE_NEXT_ETAPE;
			break;

		case COMPUTE_NEXT_ETAPE:
			etape_en_cours += 1;
			if (etape_en_cours <= LAST_ETAPE){
				state = INIT;
			}else{
				state = DONE;
			}

			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}
#endif

