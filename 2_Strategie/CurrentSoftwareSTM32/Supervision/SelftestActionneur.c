#include "SelftestActionneur.h"
#include "Selftest.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_types.h"
#include "../actuator/act_functions.h"
#include "../actuator/queue.h"
#include "../utils/generic_functions.h"
#include "../utils/actionChecker.h"
#include "../strats_2017/big/action_big.h"

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


	//#define TABLEAU_SELFTEST_BIG_VOYAGE_D_UN_MODULE

	#if defined(TABLEAU_SELFTEST_BIG_VOYAGE_D_UN_MODULE)


	static const struct_selftest_t tableau_selftest_big[]  = {

			// cote droit

			{1, ACT_POMPE_SLIDER_RIGHT , 		 ACT_POMPE_NORMAL,						 0,		ACT_QUEUE_Pompe_act_slider_right,		SELFTEST_ACT_POMPE_SLIDER_RIGHT			  },
			{2, ACT_CYLINDER_SLIDER_RIGHT,		 ACT_CYLINDER_SLIDER_RIGHT_OUT,	 		 0,		ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT   },
			{3, ACT_CYLINDER_SLIDER_RIGHT,		 ACT_CYLINDER_SLIDER_RIGHT_IN,	 		 0,		ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT   },
			{4, ACT_POMPE_ELEVATOR_RIGHT , 		 ACT_POMPE_NORMAL,						 0,		ACT_QUEUE_Pompe_act_elevator_right,	SELFTEST_ACT_POMPE_ELEVATOR_RIGHT		  },
			{5, ACT_POMPE_SLIDER_RIGHT , 		 ACT_POMPE_STOP,						 0,		ACT_QUEUE_Pompe_act_slider_right,		SELFTEST_ACT_POMPE_SLIDER_RIGHT			  },
			{6, ACT_CYLINDER_ELEVATOR_RIGHT,	 ACT_CYLINDER_ELEVATOR_RIGHT_TOP,		 0,		ACT_QUEUE_Cylinder_elevator_right, 	SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT },
			{7, ACT_CYLINDER_SLOPE_RIGHT,		 ACT_CYLINDER_SLOPE_RIGHT_UP,			 0,		ACT_QUEUE_Cylinder_slope_right,		SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT    },
			{9, ACT_POMPE_ELEVATOR_RIGHT , 		 ACT_POMPE_STOP,						 0,		ACT_QUEUE_Pompe_act_elevator_right,	SELFTEST_ACT_POMPE_ELEVATOR_RIGHT		  },
			{10, ACT_CYLINDER_ELEVATOR_RIGHT,	 ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM,	 0,		ACT_QUEUE_Cylinder_elevator_right,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT },
			// je laisse une etae pour s'assurer de la chute du module
			{11, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_OUT,		 0,		ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },
			{12, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_IN,		 0,		ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT  },
			{13, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_OUT,			 0,		ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },
			//no use of PUSHER
			{14, ACT_POMPE_DISPOSE_RIGHT , 		 ACT_POMPE_NORMAL,						 0,		ACT_QUEUE_Pompe_act_dispose_right, 	SELFTEST_ACT_POMPE_DISPOSE_RIGHT		   },
			{15, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			//{6, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED,	 0,		ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
			{16, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_RAISE,		 0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			//{8, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_ZERO_SPEED,	 0,		ACT_QUEUE_Cylinder_color_right,		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT     },
			{17, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE,	 0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			//besoin de retourner a l'etat raise ?
			{18, ACT_POMPE_DISPOSE_RIGHT , 		 ACT_POMPE_STOP,						 0,		ACT_QUEUE_Pompe_act_dispose_right,		SELFTEST_ACT_POMPE_DISPOSE_RIGHT		   },
			{19, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		 0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			{20, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_IDLE,		 0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT   },
			{21, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_IN,				 0,		ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT       },


			// cote gauche

			{22, ACT_POMPE_SLIDER_LEFT , 		 ACT_POMPE_NORMAL,						 0,		ACT_QUEUE_Pompe_act_slider_left,		SELFTEST_ACT_POMPE_SLIDER_LEFT			  },
			{23, ACT_CYLINDER_SLIDER_LEFT,		 ACT_CYLINDER_SLIDER_LEFT_OUT,	 		 0,		ACT_QUEUE_Cylinder_slider_left,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT   },
			{24, ACT_CYLINDER_SLIDER_LEFT,		 ACT_CYLINDER_SLIDER_LEFT_IN,	 		 0,		ACT_QUEUE_Cylinder_slider_left,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT   },
			{25, ACT_POMPE_ELEVATOR_LEFT , 		 ACT_POMPE_NORMAL,						 0,		ACT_QUEUE_Pompe_act_elevator_left,		SELFTEST_ACT_POMPE_ELEVATOR_LEFT			  },
			{26, ACT_POMPE_SLIDER_LEFT , 		 ACT_POMPE_STOP,						 0,		ACT_QUEUE_Pompe_act_slider_left,		SELFTEST_ACT_POMPE_SLIDER_LEFT			  },
			{27, ACT_CYLINDER_ELEVATOR_LEFT,	 ACT_CYLINDER_ELEVATOR_LEFT_TOP,		 0,		ACT_QUEUE_Cylinder_elevator_left, 		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT },
			{28, ACT_CYLINDER_SLOPE_LEFT,		 ACT_CYLINDER_SLOPE_LEFT_UP,			 0,		ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT    },
			{29, ACT_POMPE_ELEVATOR_LEFT , 		 ACT_POMPE_STOP,						 0,		ACT_QUEUE_Pompe_act_elevator_left,		SELFTEST_ACT_POMPE_ELEVATOR_LEFT			  },
			{30, ACT_CYLINDER_ELEVATOR_LEFT,	 ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM,		 0,		ACT_QUEUE_Cylinder_elevator_left,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT },
			// je laisse une etae pour s'assurer de la chute du module
			{31, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_OUT,		 0,		ACT_QUEUE_Cylinder_balancer_left,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT  },
			{32, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_IN,			 0,		ACT_QUEUE_Cylinder_balancer_left,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT  },
			{33, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_OUT,				 0,		ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },
			//no use of PUSHER
			{34, ACT_POMPE_DISPOSE_LEFT , 		 ACT_POMPE_NORMAL,						 0,		ACT_QUEUE_Pompe_act_dispose_left, 	SELFTEST_ACT_POMPE_DISPOSE_LEFT		   		  },
			{35, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			//{6, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED,	 0,		ACT_QUEUE_Cylinder_color_left,			SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
			{36, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_RAISE,		 0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			//{8, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_ZERO_SPEED,	 0,		ACT_QUEUE_Cylinder_color_left,			SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT     },
			{37, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_DISPOSE,		 0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			//besoin de retourner a l'etat raise ?
			{38, ACT_POMPE_DISPOSE_LEFT , 		 ACT_POMPE_STOP,						 0,		ACT_QUEUE_Pompe_act_dispose_left, 	SELFTEST_ACT_POMPE_DISPOSE_LEFT		   		  },
			{39, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		 0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			{40, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_IDLE,		 0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT   },
			{41, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_IN,				 0,		ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT       },


	};

	#else

	static const struct_selftest_t tableau_selftest_big[]  = {

			//-------------------------------------------------------- AVANT DU ROBOT----------------------------------------------------------------------------------------
			// Billes porteuses
			{1, ACT_BIG_BALL_FRONT_LEFT,	 	ACT_BIG_BALL_FRONT_LEFT_UP,		 		0,		ACT_QUEUE_Big_bearing_front_left, 		SELFTEST_ACT_AX12_BIG_BALL_FRONT_LEFT 		},
			{2, ACT_BIG_BALL_FRONT_LEFT,	 	ACT_BIG_BALL_FRONT_LEFT_DOWN,	 		0,		ACT_QUEUE_Big_bearing_front_left, 		SELFTEST_ACT_AX12_BIG_BALL_FRONT_LEFT 		},
			{3, ACT_BIG_BALL_FRONT_RIGHT,	 	ACT_BIG_BALL_FRONT_RIGHT_UP,		 	0,		ACT_QUEUE_Big_bearing_front_right, 		SELFTEST_ACT_AX12_BIG_BALL_FRONT_RIGHT 		},
			{4, ACT_BIG_BALL_FRONT_RIGHT,	 	ACT_BIG_BALL_FRONT_RIGHT_DOWN,	 		0,		ACT_QUEUE_Big_bearing_front_right, 		SELFTEST_ACT_AX12_BIG_BALL_FRONT_RIGHT 		},


			// Actionneurs de prise module
			{5, ACT_CYLINDER_ELEVATOR_RIGHT,	ACT_CYLINDER_ELEVATOR_RIGHT_TOP,		0,		ACT_QUEUE_Cylinder_elevator_right, 		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT	},
			{5, ACT_POMPE_ELEVATOR_RIGHT,	 	ACT_POMPE_NORMAL,		 				0,		ACT_QUEUE_Pompe_act_elevator_right, 	SELFTEST_ACT_POMPE_ELEVATOR_RIGHT 			},
			{6, ACT_CYLINDER_ELEVATOR_RIGHT,	ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM,	 	0,		ACT_QUEUE_Cylinder_elevator_right,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT	},
			{6, ACT_POMPE_ELEVATOR_RIGHT,	 	ACT_POMPE_STOP,		 					0,		ACT_QUEUE_Pompe_act_elevator_right, 	SELFTEST_ACT_POMPE_ELEVATOR_RIGHT 			},

			{7, ACT_CYLINDER_ELEVATOR_LEFT, 	ACT_CYLINDER_ELEVATOR_LEFT_TOP,		 	0,		ACT_QUEUE_Cylinder_elevator_left, 		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT	},
			{7, ACT_POMPE_ELEVATOR_LEFT,	 	ACT_POMPE_NORMAL,		 				0,		ACT_QUEUE_Pompe_act_elevator_left, 		SELFTEST_ACT_POMPE_ELEVATOR_LEFT			},
			{8, ACT_CYLINDER_ELEVATOR_LEFT,		ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM,		0,		ACT_QUEUE_Cylinder_elevator_left,		SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT	},
			{8, ACT_POMPE_ELEVATOR_LEFT,	 	ACT_POMPE_STOP,		 					0,		ACT_QUEUE_Pompe_act_elevator_left, 		SELFTEST_ACT_POMPE_ELEVATOR_LEFT			},

			{9, ACT_CYLINDER_SLIDER_RIGHT,		ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT,	0,		ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT 	},
			{9, ACT_POMPE_SLIDER_RIGHT,		 	ACT_POMPE_NORMAL,		 				0,		ACT_QUEUE_Pompe_act_slider_right, 		SELFTEST_ACT_POMPE_SLIDER_RIGHT				},
			{10, ACT_CYLINDER_SLIDER_RIGHT,		ACT_CYLINDER_SLIDER_RIGHT_IN,			0,		ACT_QUEUE_Cylinder_slider_right,		SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT 	},
			{10, ACT_POMPE_SLIDER_RIGHT,		ACT_POMPE_STOP,		 					0,		ACT_QUEUE_Pompe_act_slider_right, 		SELFTEST_ACT_POMPE_SLIDER_RIGHT				},

			{11, ACT_CYLINDER_SLIDER_LEFT,		ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT,	0,		ACT_QUEUE_Cylinder_slider_left, 		SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT		},
			{11, ACT_POMPE_SLIDER_LEFT,		 	ACT_POMPE_NORMAL,		 				0,		ACT_QUEUE_Pompe_act_slider_left, 		SELFTEST_ACT_POMPE_SLIDER_LEFT				},
			{12, ACT_CYLINDER_SLIDER_LEFT,		ACT_CYLINDER_SLIDER_LEFT_IN,			0,		ACT_QUEUE_Cylinder_slider_left,			SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT		},
			{12, ACT_POMPE_SLIDER_LEFT,		 	ACT_POMPE_STOP,		 					0,		ACT_QUEUE_Pompe_act_slider_left, 		SELFTEST_ACT_POMPE_SLIDER_LEFT				},

			{13, ACT_CYLINDER_SLOPE_RIGHT,		 ACT_CYLINDER_SLOPE_RIGHT_UP,			0,		ACT_QUEUE_Cylinder_slope_right,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT		},
			{14, ACT_CYLINDER_SLOPE_RIGHT,		 ACT_CYLINDER_SLOPE_RIGHT_DOWN,			0,		ACT_QUEUE_Cylinder_slope_right,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT		},
			{15, ACT_CYLINDER_SLOPE_LEFT,		 ACT_CYLINDER_SLOPE_LEFT_UP,			0,		ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT		},
			{16, ACT_CYLINDER_SLOPE_LEFT,		 ACT_CYLINDER_SLOPE_LEFT_DOWN,			0,		ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT		},


			// Actionneurs de dépose module droit
			{17, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED,	0,		ACT_QUEUE_Cylinder_color_right,			SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT 		},
			{18, ACT_CYLINDER_COLOR_RIGHT,		 ACT_CYLINDER_COLOR_RIGHT_ZERO_SPEED,	0,		ACT_QUEUE_Cylinder_color_right,			SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT		},
			{19, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_OUT,			0,		ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT		},
			{20, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_TAKE,		0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT	},
			{20, ACT_POMPE_DISPOSE_RIGHT,		 ACT_POMPE_NORMAL,						0,		ACT_QUEUE_Pompe_act_dispose_right,		SELFTEST_ACT_POMPE_DISPOSE_RIGHT			},
			{21, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE,	0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT	},
			{22, ACT_CYLINDER_DISPOSE_RIGHT,	 ACT_CYLINDER_DISPOSE_RIGHT_IDLE,		0,		ACT_QUEUE_Cylinder_dispose_right,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT	},
			{22, ACT_POMPE_DISPOSE_RIGHT,		 ACT_POMPE_STOP,						0,		ACT_QUEUE_Pompe_act_dispose_right,		SELFTEST_ACT_POMPE_DISPOSE_RIGHT			},
			{23, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_OUT,	 	0,		ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT	},
			{24, ACT_CYLINDER_BALANCER_RIGHT,	 ACT_CYLINDER_BALANCER_RIGHT_IDLE,		0,		ACT_QUEUE_Cylinder_balancer_right,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT	},
			{25, ACT_CYLINDER_ARM_RIGHT,		 ACT_CYLINDER_ARM_RIGHT_IN,				0,		ACT_QUEUE_Cylinder_arm_right,			SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT		},
			{26, ACT_CYLINDER_PUSHER_RIGHT,		 ACT_CYLINDER_PUSHER_RIGHT_OUT,			0,		ACT_QUEUE_Cylinder_slope_right,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_RIGHT		},
			{27, ACT_CYLINDER_PUSHER_RIGHT,		 ACT_CYLINDER_PUSHER_RIGHT_IN,			0,		ACT_QUEUE_Cylinder_slope_right,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_RIGHT		},

			// Actionneurs de dépose module gauche
			{28, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED,	0,		ACT_QUEUE_Cylinder_color_left,			SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT		},
			{29, ACT_CYLINDER_COLOR_LEFT,		 ACT_CYLINDER_COLOR_LEFT_ZERO_SPEED,	0,		ACT_QUEUE_Cylinder_color_left,			SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT		},
			{30, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_OUT,			 	0,		ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT			},
			{31, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_TAKE,		0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT		},
			{31, ACT_POMPE_DISPOSE_LEFT,		 ACT_POMPE_NORMAL,						0,		ACT_QUEUE_Pompe_act_dispose_left,		SELFTEST_ACT_POMPE_DISPOSE_LEFT				},
			{32, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_DISPOSE,		0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT		},
			{33, ACT_CYLINDER_DISPOSE_LEFT,		 ACT_CYLINDER_DISPOSE_LEFT_IDLE,		0,		ACT_QUEUE_Cylinder_dispose_left,		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT		},
			{33, ACT_POMPE_DISPOSE_LEFT,		 ACT_POMPE_STOP,						0,		ACT_QUEUE_Pompe_act_dispose_left,		SELFTEST_ACT_POMPE_DISPOSE_LEFT				},
			{34, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_OUT,		0,		ACT_QUEUE_Cylinder_balancer_left,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT	},
			{35, ACT_CYLINDER_BALANCER_LEFT,	 ACT_CYLINDER_BALANCER_LEFT_IDLE,		0,		ACT_QUEUE_Cylinder_balancer_left,		SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT	},
			{36, ACT_CYLINDER_ARM_LEFT,			 ACT_CYLINDER_ARM_LEFT_IN,				0,		ACT_QUEUE_Cylinder_arm_left,			SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT			},
			{37, ACT_CYLINDER_PUSHER_LEFT,		 ACT_CYLINDER_PUSHER_LEFT_OUT,			0,		ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT		},
			{38, ACT_CYLINDER_PUSHER_LEFT,		 ACT_CYLINDER_PUSHER_LEFT_IN,			0,		ACT_QUEUE_Cylinder_slope_left,			SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT		},


			//------------------------------------------------------- ARRIERE DU ROBOT ---------------------------------------------------------------------------------------
			// Billes porteuses
			{39, ACT_BIG_BALL_BACK_LEFT,	 	ACT_BIG_BALL_BACK_LEFT_UP,		 		0,		ACT_QUEUE_Big_bearing_back_left, 		SELFTEST_ACT_AX12_BIG_BALL_BACK_LEFT },
			{40, ACT_BIG_BALL_BACK_LEFT,	 	ACT_BIG_BALL_BACK_LEFT_DOWN,	 		0,		ACT_QUEUE_Big_bearing_back_left, 		SELFTEST_ACT_AX12_BIG_BALL_BACK_LEFT },
			{41, ACT_BIG_BALL_BACK_RIGHT,	 	ACT_BIG_BALL_BACK_RIGHT_UP,		 		0,		ACT_QUEUE_Big_bearing_back_right, 		SELFTEST_ACT_AX12_BIG_BALL_BACK_RIGHT },
			{42, ACT_BIG_BALL_BACK_RIGHT,	 	ACT_BIG_BALL_BACK_RIGHT_DOWN,	 		0,		ACT_QUEUE_Big_bearing_back_right, 		SELFTEST_ACT_AX12_BIG_BALL_BACK_RIGHT },

			// Actionneurs minerais
			{43, ACT_TURBINE,	 				ACT_TURBINE_NORMAL,						0,							ACT_QUEUE_Turbine,						SELFTEST_ACT_TURBINE },
			{44, ACT_ORE_GUN,	 				ACT_ORE_GUN_DOWN,						0,							ACT_QUEUE_Ore_gun,						SELFTEST_ACT_RX24_ORE_GUN },
			{45, ACT_ORE_GUN,	 				ACT_ORE_GUN_UP,						 	0,							ACT_QUEUE_Ore_gun,						SELFTEST_ACT_RX24_ORE_GUN },
			{45, ACT_TURBINE,	 				ACT_TURBINE_STOP,						0,							ACT_QUEUE_Turbine,						SELFTEST_ACT_TURBINE },
			{46, ACT_BEARING_BALL_WHEEL,	 	ACT_BEARING_BALL_WHEEL_UP,		 		0,							ACT_QUEUE_Bearing_ball_wheel, 			SELFTEST_ACT_AX12_BEARING_BALL_WHEEL },
			{47, ACT_BEARING_BALL_WHEEL,	 	ACT_BEARING_BALL_WHEEL_DOWN,	 		0,							ACT_QUEUE_Bearing_ball_wheel, 			SELFTEST_ACT_AX12_BEARING_BALL_WHEEL },

			{48, ACT_ORE_ROLLER_ARM,	 		ACT_ORE_ROLLER_ARM_OUT,				 	0,							ACT_QUEUE_Ore_roller_arm,				SELFTEST_ACT_RX24_ORE_ROLLER_ARM },
			{49, ACT_ORE_ROLLER_FOAM,	 		ACT_ORE_ROLLER_FOAM_RUN,				ACT_ROLLER_FOAM_SPEED_RUN,	ACT_QUEUE_Ore_roller_foam,				SELFTEST_ACT_RX24_ORE_FOAM },
			{50, ACT_ORE_WALL,	 				ACT_ORE_WALL_OUT,						0,							ACT_QUEUE_Ore_wall,						SELFTEST_ACT_RX24_ORE_WALL },
			{51, ACT_ORE_TRIHOLE,	 			ACT_ORE_TRIHOLE_RUN,					ACT_TRIHOLE_SPEED_RUN,		ACT_QUEUE_Ore_trihole,					SELFTEST_ACT_RX24_ORE_TRIHOLE },
			{52, ACT_ORE_ROLLER_FOAM,	 		ACT_ORE_ROLLER_FOAM_STOP,				0,							ACT_QUEUE_Ore_roller_foam,				SELFTEST_ACT_RX24_ORE_FOAM },
			{53, ACT_ORE_TRIHOLE,	 			ACT_ORE_TRIHOLE_STOP,					0,							ACT_QUEUE_Ore_trihole,					SELFTEST_ACT_RX24_ORE_TRIHOLE },
			{54, ACT_ORE_WALL,	 				ACT_ORE_WALL_IN,						0,							ACT_QUEUE_Ore_wall,						SELFTEST_ACT_RX24_ORE_WALL },
			{55, ACT_ORE_ROLLER_ARM,	 		ACT_ORE_ROLLER_ARM_IN,					0,							ACT_QUEUE_Ore_roller_arm,				SELFTEST_ACT_RX24_ORE_ROLLER_ARM },
		};
	#endif

	static const struct_selftest_t tableau_selftest_small[]  = {

			{1, ACT_SMALL_BALL_BACK,	 		ACT_SMALL_BALL_BACK_UP,				 	0,		ACT_QUEUE_Small_bearing_back, 			SELFTEST_ACT_AX12_SMALL_BALL_BACK },
			{2, ACT_SMALL_BALL_BACK,	 		ACT_SMALL_BALL_BACK_DOWN,			 	0,		ACT_QUEUE_Small_bearing_back, 			SELFTEST_ACT_AX12_SMALL_BALL_BACK },

			{5, ACT_SMALL_BALL_FRONT_LEFT,	 	ACT_SMALL_BALL_FRONT_LEFT_UP,		 	0,		ACT_QUEUE_Small_bearing_front_left, 	SELFTEST_ACT_AX12_SMALL_BALL_FRONT_LEFT },
			{6, ACT_SMALL_BALL_FRONT_LEFT,	 	ACT_SMALL_BALL_FRONT_LEFT_DOWN,		 	0,		ACT_QUEUE_Small_bearing_front_left, 	SELFTEST_ACT_AX12_SMALL_BALL_FRONT_LEFT },
			{7, ACT_SMALL_BALL_FRONT_RIGHT,	 	ACT_SMALL_BALL_FRONT_RIGHT_UP,		 	0,		ACT_QUEUE_Small_bearing_front_right, 	SELFTEST_ACT_AX12_SMALL_BALL_FRONT_RIGHT },
			{8, ACT_SMALL_BALL_FRONT_RIGHT,	 	ACT_SMALL_BALL_FRONT_RIGHT_DOWN,		0,		ACT_QUEUE_Small_bearing_front_right, 	SELFTEST_ACT_AX12_SMALL_BALL_FRONT_RIGHT },
			{9, ACT_SMALL_CYLINDER_SLIDER,	 	ACT_SMALL_CYLINDER_SLIDER_OUT,		 	0,		ACT_QUEUE_Small_cylinder_slider, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_SLIDER },
			{9, ACT_SMALL_POMPE_PRISE,	 		ACT_POMPE_SMALL_SLIDER_NORMAL,		 	100,	ACT_QUEUE_Small_pompe_prise, 			SELFTEST_ACT_AX12_SMALL_POMPE_PRISE },
			{10, ACT_SMALL_CYLINDER_SLIDER,	 	ACT_SMALL_CYLINDER_SLIDER_IN,		 	0,		ACT_QUEUE_Small_cylinder_slider, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_SLIDER },
			{10, ACT_SMALL_POMPE_PRISE,	 		ACT_POMPE_SMALL_ELEVATOR_NORMAL,		100,	ACT_QUEUE_Small_pompe_prise, 			SELFTEST_ACT_AX12_SMALL_POMPE_PRISE },
			{11, ACT_SMALL_CYLINDER_ELEVATOR,	ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM,		0,		ACT_QUEUE_Small_cylinder_elevator, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_ELEVATOR },
			{12, ACT_SMALL_CYLINDER_ELEVATOR,	ACT_SMALL_CYLINDER_ELEVATOR_TOP,		0,		ACT_QUEUE_Small_cylinder_elevator, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_ELEVATOR },
			{12, ACT_SMALL_POMPE_PRISE,	 		ACT_POMPE_STOP,							0,		ACT_QUEUE_Small_pompe_prise, 			SELFTEST_ACT_AX12_SMALL_POMPE_PRISE },
			{13, ACT_SMALL_CYLINDER_SLOPE,		ACT_SMALL_CYLINDER_SLOPE_UP,			0,		ACT_QUEUE_Small_cylinder_slope, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_SLOPE },
			{14, ACT_SMALL_CYLINDER_SLOPE,		ACT_SMALL_CYLINDER_SLOPE_DOWN,			0,		ACT_QUEUE_Small_cylinder_slope, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_SLOPE },
			{15, ACT_SMALL_CYLINDER_BALANCER,	ACT_SMALL_CYLINDER_BALANCER_OUT,		0,		ACT_QUEUE_Small_cylinder_balancer, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_BALANCER },
			{16, ACT_SMALL_CYLINDER_BALANCER,	ACT_SMALL_CYLINDER_BALANCER_IDLE,		0,		ACT_QUEUE_Small_cylinder_balancer, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_BALANCER },
			{17, ACT_SMALL_CYLINDER_COLOR,		ACT_SMALL_CYLINDER_COLOR_NORMAL_SPEED,	0,		ACT_QUEUE_Small_cylinder_color, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_COLOR },
			{18, ACT_SMALL_CYLINDER_COLOR,		ACT_SMALL_CYLINDER_COLOR_ZERO_SPEED,	0,		ACT_QUEUE_Small_cylinder_color, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_COLOR },

			{19, ACT_SMALL_CYLINDER_MULTIFONCTION,	ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH,	0,	ACT_QUEUE_Small_cylinder_multifonction, SELFTEST_ACT_AX12_SMALL_CYLINDER_MULTIFONCTION },
			{20, ACT_SMALL_CYLINDER_MULTIFONCTION,	ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK,	0,	ACT_QUEUE_Small_cylinder_multifonction, SELFTEST_ACT_AX12_SMALL_CYLINDER_MULTIFONCTION },
			{21, ACT_SMALL_CYLINDER_MULTIFONCTION,	ACT_SMALL_CYLINDER_MULTIFONCTION_IN,	0,	ACT_QUEUE_Small_cylinder_multifonction, SELFTEST_ACT_AX12_SMALL_CYLINDER_MULTIFONCTION },

			{22, ACT_SMALL_CYLINDER_ARM,		ACT_SMALL_CYLINDER_ARM_TAKE,			0,		ACT_QUEUE_Small_cylinder_arm, 			SELFTEST_ACT_AX12_SMALL_CYLINDER_ARM },
			{23, ACT_SMALL_CYLINDER_DISPOSE,	ACT_SMALL_CYLINDER_DISPOSE_DISPOSE,		0,		ACT_QUEUE_Small_cylinder_dispose, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_DISPOSE },
			{23, ACT_SMALL_POMPE_DISPOSE,		ACT_POMPE_NORMAL,						100,	ACT_QUEUE_Small_pompe_dispose, 			SELFTEST_ACT_AX12_SMALL_POMPE_DISPOSE },
			{24, ACT_SMALL_CYLINDER_DISPOSE,	ACT_SMALL_CYLINDER_DISPOSE_IDLE,		0,		ACT_QUEUE_Small_cylinder_dispose, 		SELFTEST_ACT_AX12_SMALL_CYLINDER_DISPOSE },
			{24, ACT_SMALL_POMPE_DISPOSE,		ACT_POMPE_STOP,							0,		ACT_QUEUE_Small_pompe_dispose, 			SELFTEST_ACT_AX12_SMALL_POMPE_DISPOSE },
			{25, ACT_SMALL_CYLINDER_ARM,		ACT_SMALL_CYLINDER_ARM_IN,				0,		ACT_QUEUE_Small_cylinder_arm, 			SELFTEST_ACT_AX12_SMALL_CYLINDER_ARM },

			{26, ACT_SMALL_MAGIC_ARM,			ACT_SMALL_MAGIC_ARM_OUT,				0,		ACT_QUEUE_Small_magic_arm, 				SELFTEST_ACT_AX12_SMALL_MAGIC_ARM },
			{27, ACT_SMALL_MAGIC_COLOR,			ACT_SMALL_MAGIC_COLOR_NORMAL_SPEED,		0,		ACT_QUEUE_Small_magic_color, 			SELFTEST_ACT_AX12_SMALL_MAGIC_COLOR },
			{28, ACT_SMALL_MAGIC_COLOR,			ACT_SMALL_MAGIC_COLOR_ZERO_SPEED,		0,		ACT_QUEUE_Small_magic_color, 			SELFTEST_ACT_AX12_SMALL_MAGIC_COLOR },
			{29, ACT_SMALL_MAGIC_ARM,			ACT_SMALL_MAGIC_ARM_OUT,				0,		ACT_QUEUE_Small_magic_arm, 				SELFTEST_ACT_AX12_SMALL_MAGIC_ARM },

			{30, ACT_SMALL_ORE,					ACT_SMALL_ORE_DOWN,						0,		ACT_QUEUE_Small_ore, 					SELFTEST_ACT_AX12_SMALL_ORE},
			{31, ACT_SMALL_ORE,					ACT_SMALL_ORE_UP,						0,		ACT_QUEUE_Small_ore, 					SELFTEST_ACT_AX12_SMALL_ORE},
	};

	#define NB_MAX_ACTIONS	 (100)

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
			assert(nb_actions < NB_MAX_ACTIONS);

			// Initialisation de la liste des états actionneurs
			for(int i=0; i<nb_actions; i++){
				liste_etat_actionneur[i] = IN_PROGRESS;
			}

			etape_en_cours = 1;
			state = MOVE_ACTIONNEUR;
			break;

		case MOVE_ACTIONNEUR:
			if(entrance){
				debug_printf("Etape en cours (Move actionneur): %d\n", etape_en_cours);
			}
			if(etape_en_cours == 43){
				Uint8 baba = 2;
			}
			ind_start_etape = indice;
			while(indice < nb_actions && tableau_selftest[indice].numero_etape == etape_en_cours ){
				ACT_push_order_with_param(tableau_selftest[indice].actionneur, tableau_selftest[indice].position, tableau_selftest[indice].param);
				indice++;
			}
			ind_end_etape = indice - 1;

			state = CHECK_STATUS;
			break;

		case CHECK_STATUS:
			if(entrance){
				debug_printf("Etape en cours (Check status): %d\n", etape_en_cours);
			}
			if(etape_en_cours == 43){
				Uint8 baba = 2;
			}

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
			if(entrance){
				debug_printf("Declare error\n");
			}
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


