#include "SelftestActionneur.h"
#include "Selftest.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_types.h"
#include "../actuator/act_functions.h"
#include "../actuator/queue.h"
#include "../utils/generic_functions.h"
#include "../utils/actionChecker.h"


error_e SELFTESTACT_run(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SELFTEST_ACT,
			INIT,
			MOVE_ACTIONNEUR,
			CHECK_STATUS,
			COMPUTE_NEXT_ETAPE,
			DECLARE_ERROR,
			ERROR,
			DONE
	);


	#define TABLEAU_SELFTEST_BIG_VOYAGE_D_UN_MODULE

	#if defined(TABLEAU_SELFTEST_BIG_VOYAGE_D_UN_MODULE)


	static const struct_selftest_t tableau_selftest_big[]  = {

			// cote droit

			{1, ACT_POMPE_SLIDER_RIGHT , 		 ACT_POMPE_NORMAL,						 ACT_QUEUE_Pompe_act_slider_right,		SELFTEST_ACT_POMPE_SLIDER_RIGHT			  },
			{2, ACT_CYLINDER_SLIDER_RIGHT,		 ACT_CYLINDER_SLIDER_RIGHT_OUT,	 		 ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT   },
			{3, ACT_CYLINDER_SLIDER_RIGHT,		 ACT_CYLINDER_SLIDER_RIGHT_IN,	 		 ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT   },
			{4, ACT_POMPE_ELEVATOR_RIGHT , 		 ACT_POMPE_NORMAL,						 ACT_QUEUE_Pompe_act_elevator_right,	SELFTEST_ACT_POMPE_ELEVATOR_RIGHT		  },
			{5, ACT_POMPE_SLIDER_RIGHT , 		 ACT_POMPE_STOP,						 ACT_QUEUE_Pompe_act_slider_right,		SELFTEST_ACT_POMPE_SLIDER_RIGHT			  },
			{6, ACT_CYLINDER_ELEVATOR_RIGHT,	 ACT_CYLINDER_ELEVATOR_RIGHT_TOP,		 ACT_QUEUE_Cylinder_elevator_right, 	SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT },
			{7, ACT_CYLINDER_SLOPE_RIGHT,		 ACT_CYLINDER_SLOPE_RIGHT_UP,			 ACT_QUEUE_Cylinder_slope_right,		SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT    },
			{9, ACT_POMPE_ELEVATOR_RIGHT , 		 ACT_POMPE_STOP,						 ACT_QUEUE_Pompe_act_elevator_right,	SELFTEST_ACT_POMPE_ELEVATOR_RIGHT		  },
			{10, ACT_CYLINDER_ELEVATOR_RIGHT,	 ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM,	 ACT_QUEUE_Cylinder_elevator_right,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT },
			// je laisse une etae pour s'assurer de la chute du module
			{11, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_OUT,		 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },
			{12, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_IN,		 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },
			{13, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_OUT,			 ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },
			//no use of PUSHER
			{14, ACT_POMPE_DISPOSE_RIGHT , 		 ACT_POMPE_NORMAL,						 ACT_QUEUE_Pompe_act_dispose_right, 	SELFTEST_ACT_POMPE_DISPOSE_RIGHT		   },
			{15, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			//{6, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED,	 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
			{16, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			//{8, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_ZERO_SPEED,	 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
			{17, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE,	 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			//besoin de retourner a l'etat raise ?
			{18, ACT_POMPE_DISPOSE_RIGHT , 		 ACT_POMPE_STOP,						 ACT_QUEUE_Pompe_act_dispose_right,		SELFTEST_ACT_POMPE_DISPOSE_RIGHT		   },
			{19, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			{20, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_IDLE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			{21, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_IN,				 ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },


			// cote gauche

			{22, ACT_POMPE_SLIDER_LEFT , 		 ACT_POMPE_NORMAL,						 ACT_QUEUE_Pompe_act_slider_left,		SELFTEST_ACT_POMPE_SLIDER_LEFT			  },
			{23, ACT_CYLINDER_SLIDER_LEFT,		 ACT_CYLINDER_SLIDER_LEFT_OUT,	 		 ACT_QUEUE_Cylinder_slider_left,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT   },
			{24, ACT_CYLINDER_SLIDER_LEFT,		 ACT_CYLINDER_SLIDER_LEFT_IN,	 		 ACT_QUEUE_Cylinder_slider_left,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT   },
			{25, ACT_POMPE_ELEVATOR_LEFT , 		 ACT_POMPE_NORMAL,						 ACT_QUEUE_Pompe_act_elevator_left,		SELFTEST_ACT_POMPE_ELEVATOR_LEFT			  },
			{26, ACT_POMPE_SLIDER_LEFT , 		 ACT_POMPE_STOP,						 ACT_QUEUE_Pompe_act_slider_left,		SELFTEST_ACT_POMPE_SLIDER_LEFT			  },
			{27, ACT_CYLINDER_ELEVATOR_LEFT,	 ACT_CYLINDER_ELEVATOR_LEFT_TOP,		 ACT_QUEUE_Cylinder_elevator_left, 		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT },
			{28, ACT_CYLINDER_SLOPE_LEFT,		 ACT_CYLINDER_SLOPE_LEFT_UP,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT    },
			{29, ACT_POMPE_ELEVATOR_LEFT , 		 ACT_POMPE_STOP,						 ACT_QUEUE_Pompe_act_elevator_left,		SELFTEST_ACT_POMPE_ELEVATOR_LEFT			  },
			{30, ACT_CYLINDER_ELEVATOR_LEFT,	 ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM,		 ACT_QUEUE_Cylinder_elevator_left,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT },
			// je laisse une etae pour s'assurer de la chute du module
			{31, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_OUT,		 ACT_QUEUE_Cylinder_balancer_left,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT  },
			{32, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_IN,			 ACT_QUEUE_Cylinder_balancer_left,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT  },
			{33, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_OUT,				 ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },
			//no use of PUSHER
			{34, ACT_POMPE_DISPOSE_LEFT , 		 ACT_POMPE_NORMAL,						 ACT_QUEUE_Pompe_act_dispose_left, 	SELFTEST_ACT_POMPE_DISPOSE_LEFT		   		  },
			{35, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			//{6, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED,	 ACT_QUEUE_Cylinder_color_left,			SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
			{36, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_RAISE,		 ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			//{8, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_ZERO_SPEED,	 ACT_QUEUE_Cylinder_color_left,			SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
			{37, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_DISPOSE,		 ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			//besoin de retourner a l'etat raise ?
			{38, ACT_POMPE_DISPOSE_LEFT , 		 ACT_POMPE_STOP,						 ACT_QUEUE_Pompe_act_dispose_left, 	SELFTEST_ACT_POMPE_DISPOSE_LEFT		   		  },
			{39, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			{40, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_IDLE,		 ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			{41, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_IN,				 ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },


	};

	#else

	static const struct_selftest_t tableau_selftest_big[]  = {


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
				{6, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
				//{6, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED,	 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
				{7, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
				//{8, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_ZERO_SPEED,	 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
				{8, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE,	 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
				//besoin de retourner a l'etat raise ?
				{9, ACT_CYLINDER_DISPOSE_RIGHT,		 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
				{10, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_STOP,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
				{11, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_IN,				 ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },
				{11, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_IN,	 	 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },
				{12, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_OUT,		 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },


				//actionneurs depose cote gauche

				{13, ACT_CYLINDER_PUSHER_LEFT,		 ACT_CYLINDER_PUSHER_LEFT_OUT,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT    },
				{14, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_OUT,			 	 ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },
				{15, ACT_CYLINDER_PUSHER_LEFT,		 ACT_CYLINDER_PUSHER_LEFT_IN,			 ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT    },
				{16, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
				//{16, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED,	 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
				{17, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_RAISE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
				//{18, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_ZERO_SPEED,	 ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
				{18, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_DISPOSE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
				{19, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
				{20, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_STOP,		 ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
				{21, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_IN,				 ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },
				{21, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_IN,			 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT },
				{22, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_OUT,		 ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT },


				//actionneur prise balle

				{23, ACT_ORE_WALL,	 				 ACT_ORE_WALL_OUT,						 ACT_QUEUE_Ore_wall,					SELFTEST_ACT_RX24_ORE_WALL },
				{24, ACT_ORE_TRIHOLE,	 			 ACT_ORE_TRIHOLE_RUN,					 ACT_QUEUE_Ore_trihole,					SELFTEST_ACT_RX24_ORE_TRIHOLE },
				{24, ACT_ORE_ROLLER_ARM,	 		 ACT_ORE_ROLLER_ARM_OUT,				 ACT_QUEUE_Ore_roller_arm,				SELFTEST_ACT_RX24_ORE_ROLLER_ARM },
				{25, ACT_ORE_ROLLER_FOAM,	 		 ACT_ORE_ROLLER_FOAM_RUN,				 ACT_QUEUE_Ore_roller_foam,				SELFTEST_ACT_RX24_ORE_FOAM },
				{26, ACT_ORE_ROLLER_FOAM,	 		 ACT_ORE_ROLLER_FOAM_STOP,				 ACT_QUEUE_Ore_roller_foam,				SELFTEST_ACT_RX24_ORE_FOAM },
				{27, ACT_ORE_ROLLER_ARM,	 		 ACT_ORE_ROLLER_ARM_IN,					 ACT_QUEUE_Ore_roller_arm,				SELFTEST_ACT_RX24_ORE_ROLLER_ARM },
				{27, ACT_ORE_TRIHOLE,	 			 ACT_ORE_TRIHOLE_RUN,					 ACT_QUEUE_Ore_trihole,					SELFTEST_ACT_RX24_ORE_TRIHOLE },
				{28, ACT_ORE_WALL,	 				 ACT_ORE_WALL_IN,						 ACT_QUEUE_Ore_wall,					SELFTEST_ACT_RX24_ORE_WALL },
				{29, ACT_ORE_GUN,	 				 ACT_ORE_GUN_DOWN,						 ACT_QUEUE_Ore_gun,						SELFTEST_ACT_RX24_ORE_GUN },
				{30, ACT_ORE_GUN,	 				 ACT_ORE_GUN_UP,						 ACT_QUEUE_Ore_gun,						SELFTEST_ACT_RX24_ORE_GUN },

		};
	#endif

	static const struct_selftest_t tableau_selftest_small[]  = {

			{1, ACT_SMALL_BALL_BACK_LEFT,	 	ACT_SMALL_BALL_BACK_LEFT_UP,		 	ACT_QUEUE_Small_bearing_back_left, 		SELFTEST_ACT_AX12_SMALL_BALL_BACK_LEFT },
			{2, ACT_SMALL_BALL_BACK_LEFT,	 	ACT_SMALL_BALL_BACK_LEFT_DOWN,		 	ACT_QUEUE_Small_bearing_back_left, 		SELFTEST_ACT_AX12_SMALL_BALL_BACK_LEFT },

	};

	#define NB_MAX_ACTIONS	 (60)

	static const struct_selftest_t *tableau_selftest;
	static error_e liste_etat_actionneur[NB_MAX_ACTIONS];
	static ACT_sid_e liste_error_actionneur[NB_MAX_ACTIONS];
	static Uint8 nb_actions;
	static Uint8 nb_etapes;
	static Uint8 indice = 0;
	static Uint8 ind_start_etape = 0, ind_end_etape = 0;
	static Uint8 etape_en_cours = 1;
	static bool_e check_finish = FALSE;
	static bool_e new_error = FALSE;
	static Uint8 ind_nb_errors = 0;
	Uint8 i;
	Uint8 j;


	switch(state){
		case INIT:
			// Choix des infos BIG ou SMALL robot
			if(I_AM_BIG()){
				tableau_selftest = tableau_selftest_big;
				nb_actions = (sizeof(tableau_selftest_big) / sizeof(struct_selftest_t));
				nb_etapes = tableau_selftest_big[nb_actions - 1].numero_etape;
			}else{
				tableau_selftest = tableau_selftest_small;
				nb_actions = (sizeof(tableau_selftest_small) / sizeof(struct_selftest_t));
				nb_etapes = tableau_selftest_small[nb_actions - 1].numero_etape;
			}
			debug_printf("SELFTEST ACT nb_actions = %d  nb_etapes = %d\n", nb_actions, nb_etapes);

			// Initialisation de la liste des états actionneurs
			for(int i=0; i<nb_actions; i++){
				liste_etat_actionneur[i] = IN_PROGRESS;
			}

			etape_en_cours = 1;
			state = MOVE_ACTIONNEUR;
			break;

		case MOVE_ACTIONNEUR:
			ind_start_etape = indice;
			while(tableau_selftest[indice].numero_etape == etape_en_cours && indice < nb_actions){
				ACT_push_order(tableau_selftest[indice].actionneur, tableau_selftest[indice].position);
				indice++;
			}
			ind_end_etape = indice - 1;

			state = CHECK_STATUS;
			break;

		case CHECK_STATUS:
			check_finish = TRUE;
			for(i = ind_start_etape; i <= ind_end_etape; i++){
				if(liste_etat_actionneur[i] == IN_PROGRESS){
					liste_etat_actionneur[i] = check_act_status(tableau_selftest[i].queue_id, IN_PROGRESS, END_OK, NOT_HANDLED);
					if(liste_etat_actionneur[i] == IN_PROGRESS){
						check_finish = FALSE;
					}
				}
			}

			if(check_finish){
				state = COMPUTE_NEXT_ETAPE;
			}
			break;

		case COMPUTE_NEXT_ETAPE:
			etape_en_cours += 1;
			if (etape_en_cours <= nb_etapes){
				state = MOVE_ACTIONNEUR;
			}else{
				state = DECLARE_ERROR;
			}
			break;

		case DECLARE_ERROR :
			for(i = 0; i < nb_actions; i++){
				if (liste_etat_actionneur[i] == NOT_HANDLED){
					new_error = TRUE;

					// On recherche si l'erreur a déjà été déclarée
					for(j=0; j<ind_nb_errors; j++){
						if(liste_error_actionneur[j]==tableau_selftest[i].actionneur){
							new_error = FALSE;
						}
					}

					// Si l'erreur n'a pas été déclarée, on la déclare
					if (new_error == TRUE){
						SELFTEST_declare_errors(NULL, tableau_selftest[i].error_code);
						debug_printf("ERROR %d\n", tableau_selftest[i].error_code);
						liste_error_actionneur[ind_nb_errors] = tableau_selftest[i].actionneur;
						ind_nb_errors += 1;
					}
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


