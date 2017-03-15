#include "SelftestActionneur.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_types.h"
#include "../actuator/act_functions.h"
#include "../utils/generic_functions.h"
#include "../utils/actionChecker.h"

#if 1

//Pour Valentin :
// Il s'agit d'une premiere version, le code n'est en rien optimise

error_e SELFTESTACT_run(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SELFTEST_ACT,
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

	#define LAST_ETAPE_SELFTEST   (4)
	#define NB_ACTIONS	 (sizeof(tableau_selftest) / sizeof(struct_selftest_t))

	static error_e liste_etat_actionneur[NB_ACTIONS];
	static Uint8 indice = 0; //static?
	static Uint8 indice_check = 0;
	static Uint8 etape_en_cours = 1;

	switch(state){
		case INIT:
			for(int i=0; i<NB_ACTIONS; i++){
				liste_etat_actionneur[i] = IN_PROGRESS;
			}
			state = MOVE_ACTIONNEUR;
			break;

		case MOVE_ACTIONNEUR:

			while(tableau_selftest[indice].numero_etape == etape_en_cours && indice < NB_ACTIONS){
				ACT_push_order(tableau_selftest[indice].actionneur, tableau_selftest[indice].position);
				indice += 1;
			}
			state = wait_time(1000, state, CHECK_STATUS); // Attention au wait_time ici, à remplacer par une attente global.absolute_time
			break;

		case CHECK_STATUS:

			while(tableau_selftest[indice_check].numero_etape == etape_en_cours && indice < NB_ACTIONS){
				liste_etat_actionneur[indice_check] = check_act_status(tableau_selftest[indice_check].queue_id, IN_PROGRESS, END_OK, NOT_HANDLED);
				indice_check += 1;
			}

			state = wait_time(1000, state, COMPUTE_NEXT_ETAPE); // Attention au wait_time ici, à remplacer par une attente global.absolute_time
			break;

		case COMPUTE_NEXT_ETAPE:
			etape_en_cours += 1;
			if (etape_en_cours <= LAST_ETAPE_SELFTEST){
				state = MOVE_ACTIONNEUR;
			}else{
				// Ici il faudra penser à envoyer les erreurs à l'algo de selftest
				// Utiliser et compléter la fonction suivante : SELFTESTACT_check_errors()
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
				debug_printf("default case in SELFTESTACT_run\n");
			break;
	}

	return IN_PROGRESS;
}

/*
void SELFTESTACT_check_errors(tab){
	SELFTEST_error_code_e error;

	for(tous les ordres actionneur){
		if(actionneur failed){
			switch(liste_etat_actionneur[i].actionneur){
			case ACT_CYLINDER_ELEVATOR_RIGHT :
				//error = SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT;
				SELFTEST_declare_errors(NULL, SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT);
				break;
			case etc....
			case
			}
			SELFTEST_declare_errors(NULL, error);
		}
	}
}
*/

#endif

