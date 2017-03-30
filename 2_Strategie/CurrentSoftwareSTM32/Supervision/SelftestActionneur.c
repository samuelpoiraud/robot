#include "SelftestActionneur.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_types.h"
#include "../actuator/act_functions.h"
#include "../actuator/queue.h"
#include "../utils/generic_functions.h"
#include "../utils/actionChecker.h"

#if 1


error_e SELFTESTACT_run(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SELFTEST_ACT, //a declarer
			INIT,
			MOVE_ACTIONNEUR,
			CHECK_STATUS,
			COMPUTE_NEXT_ETAPE,
			DECLARE_ERROR,
			ERROR,
			DONE
	);

	static const struct_selftest_t tableau_selftest[]  = {


			//actionneurs prise de modules :

			{1, ACT_CYLINDER_ELEVATOR_RIGHT,	 ACT_CYLINDER_ELEVATOR_RIGHT_TOP,		 ACT_QUEUE_Cylinder_elevator_right, 	SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT },
			{1, ACT_CYLINDER_ELEVATOR_LEFT, 	 ACT_CYLINDER_ELEVATOR_LEFT_TOP,		 ACT_QUEUE_Cylinder_elevator_left, 		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT  },
			{2, ACT_CYLINDER_ELEVATOR_RIGHT,	 ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM,	 ACT_QUEUE_Cylinder_elevator_right,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT },
			{2, ACT_CYLINDER_ELEVATOR_LEFT,		 ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM,		 ACT_QUEUE_Cylinder_elevator_left,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT  },
			{3, ACT_CYLINDER_SLIDER_RIGHT,		 ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT,	 ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT   },
			{3, ACT_CYLINDER_SLIDER_LEFT,		 ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT,	 ACT_QUEUE_Cylinder_slider_left, 		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT    },
			{3, ACT_CYLINDER_SLOPE_RIGHT,		 ACT_CYLINDER_SLOPE_RIGHT_UP,			 ACT_QUEUE_Cylinder_slope_right,		SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT    },
			{3, ACT_CYLINDER_SLOPE_LEFT,		 ACT_CYLINDER_SLOPE_LEFT_UP,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT     },
			{4, ACT_CYLINDER_SLIDER_RIGHT,		 ACT_CYLINDER_SLIDER_RIGHT_IN,			 ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT   },
			{4, ACT_CYLINDER_SLIDER_LEFT,		 ACT_CYLINDER_SLIDER_LEFT_IN,			 ACT_QUEUE_Cylinder_slider_left,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT    },
			{4, ACT_CYLINDER_SLOPE_RIGHT,		 ACT_CYLINDER_SLOPE_RIGHT_DOWN,			 ACT_QUEUE_Cylinder_slope_right,		SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT    },
			{4, ACT_CYLINDER_SLOPE_LEFT,		 ACT_CYLINDER_SLOPE_LEFT_DOWN,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT     },


			//actionneurs depose cote droit

			{5, ACT_CYLINDER_PUSHER_RIGHT,		 ACT_CYLINDER_PUSHER_RIGHT_OUT,			 ACT_QUEUE_Cylinder_slope_right,		SELFTEST_ACT_AX12_CYLINDER_PUSHER_RIGHT    },
			{5, ACT_CYLINDER_ARM_RIGHT,			 ACT_CYLINDER_ARM_RIGHT_OUT,			 ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },
			{6, ACT_CYLINDER_PUSHER_RIGHT,		 ACT_CYLINDER_PUSHER_RIGHT_IN,			 ACT_QUEUE_Cylinder_slope_right,		SELFTEST_ACT_AX12_CYLINDER_PUSHER_RIGHT    },
			{6, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_RIGHT   },
			//{6, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_UNLOCK,		 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
			{7, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_RIGHT   },
			//{8, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_LOCK,			 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
			{8, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE,	 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_RIGHT   },
			{9, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_RIGHT   },
			{10, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_RIGHT   },
			{11, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_IN,				 ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },
			{11, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_IN,	 	 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },
			{12, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_OUT,		 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },


			//actionneurs depose cote gauche

			{13, ACT_CYLINDER_PUSHER_LEFT,		 ACT_CYLINDER_PUSHER_LEFT_OUT,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT    },
			{14, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_OUT,			 	 ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },
			{15, ACT_CYLINDER_PUSHER_LEFT,		 ACT_CYLINDER_PUSHER_LEFT_IN,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT    },
			{16, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_LEFT   },
			//{16, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_UNLOCK,		 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
			{17, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_LEFT   },
			//{18, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_LOCK,			 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
			{18, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_DISPOSE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_LEFT   },
			{19, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_LEFT   },
			{20, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_RX24_CYLINDER_DISPOSE_LEFT   },
			{21, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_IN,				 ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },
			{21, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_IN,			 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT },
			{22, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_OUT,		 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT },


			//actionneur prise balle

			{23, ACT_ORE_WALL,	 				 ACT_ORE_WALL_OUT,						 ACT_QUEUE_Ore_wall,					SELFTEST_ACT_RX24_ORE_WALL },
			{24, ACT_ORE_TRIHOLE,	 			 ACT_ORE_TRIHOLE_RUN,					 ACT_QUEUE_Ore_trihole,					SELFTEST_ACT_RX24_ORE_TRIHOLE },
			{24, ACT_ORE_ROLLER_ARM,	 		 ACT_ORE_ROLLER_ARM_OUT,				 ACT_QUEUE_Ore_roller_arm,				SELFTEST_ACT_RX24_ORE_ROLLER_ARM },
			{25, ACT_ORE_ROLLER_FOAM,	 		 ACT_ORE_ROLLER_FOAM_RUN,				 ACT_QUEUE_Ore_foam,					SELFTEST_ACT_RX24_ORE_FOAM },
			{26, ACT_ORE_ROLLER_FOAM,	 		 ACT_ORE_ROLLER_FOAM_STOP,				 ACT_QUEUE_Ore_foam,					SELFTEST_ACT_RX24_ORE_FOAM },
			{27, ACT_ORE_ROLLER_ARM,	 		 ACT_ORE_ROLLER_ARM_IN,					 ACT_QUEUE_Ore_roller_arm,				SELFTEST_ACT_RX24_ORE_ROLLER_ARM },
			{27, ACT_ORE_TRIHOLE,	 			 ACT_ORE_TRIHOLE_RUN,					 ACT_QUEUE_Ore_trihole,					SELFTEST_ACT_RX24_ORE_TRIHOLE },
			{28, ACT_ORE_WALL,	 				 ACT_ORE_WALL_IN,						 ACT_QUEUE_Ore_wall,					SELFTEST_ACT_RX24_ORE_WALL },


			//ACT_ORE_GUN ?


	};

	#define LAST_ETAPE_SELFTEST   (20)
	#define NB_ACTIONS	 (sizeof(tableau_selftest) / sizeof(struct_selftest_t))

	static error_e liste_etat_actionneur[NB_ACTIONS];
	static Uint8 indice = 0; //static?
	static Uint8 indice_check = 0;
	static Uint8 etape_en_cours = 1;
	static Uint8 indice_err = 0;


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
			if (etape_en_cours < LAST_ETAPE_SELFTEST){
				state = MOVE_ACTIONNEUR;
			}else{
				// Ici il faudra penser à envoyer les erreurs à l'algo de selftest
				// Utiliser et compléter la fonction suivante : SELFTESTACT_check_errors()

				state = DECLARE_ERROR;
			}
			break;

		case DECLARE_ERROR :
			for(int i=0; i < LAST_ETAPE_SELFTEST; i++){
				while(tableau_selftest[indice_err].numero_etape == i){
					if (liste_etat_actionneur[indice_check] == NOT_HANDLED){
						SELFTEST_declare_errors(NULL, tableau_selftest[indice_err].error_code);
					}
					indice_check += 1;
				}
			}
			state = DONE;
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

