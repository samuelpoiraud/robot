#include "action_small.h"
#include "../actions_both_generic.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_CapteurCouleurCW.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../actuator/act_functions.h"
#include "../../QS/QS_types.h"
#include "../../actuator/queue.h"
#include "../../propulsion/pathfind.h"
#include "../../propulsion/prop_functions.h"
#include "../avoidance.h"


// Zone filtré pour le petit robot
#define FILTER_ZONE_FOR_SMALL	(9)

typedef enum
{
	DZONE0_BLUE_OUTDOOR,
	DZONE1_BLUE_INDOOR,
	DZONE2_MIDDLE_BLUE,
	DZONE3_MIDDLE_YELLOW,
	DZONE4_YELLOW_INDOOR,
	DZONE5_YELLOW_OUTDOOR,
	DZONE6_YELLOW_SIDE,
	DZONE7_BLUE_SIDE
}dispose_zone_t;

#define OFFSET_M_R		25	//distance dans le sens avant-arrière entre le module stocké dans le robot et le centre du robot.
static error_e sub_ExtractMoonbase(dispose_zone_t dzone);

//Cette fonction ne peut être appelée que si on est en sortie de notre fusée color, et qu'on vise l'une des zones adverses !!!
//si cette sub renvoit END_OK, cela suppose qu'on peut rejoindre directement en GET_IN le point d'entrée de la DZONE demandée !
error_e sub_from_our_rocket_to_dzone(dispose_zone_t dzone)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_ANNE_FROM_OUR_ROCKET_TO_DZONE,
		INIT,
		COMPUTE,
		TURN_TO_SEE_FOES,
		GOTO_N1,
		GOTO_N2,
		GOTO_N3,
		GOTO_S1,
		GOTO_S2,
		GOTO_S3,
		GOTO_GOAL,
		DONE,
		ERROR
	);
	const GEOMETRY_point_t GOAL_DZ05 = (GEOMETRY_point_t){1400,2400};
	const GEOMETRY_point_t GOAL_DZ1234 = (GEOMETRY_point_t){1200,1800};
	const GEOMETRY_point_t GOAL_DSIDE_YELLOW = (GEOMETRY_point_t){850+OFFSET_M_R,2742};
	const GEOMETRY_point_t GOAL_DSIDE_BLUE = (GEOMETRY_point_t){1000-OFFSET_M_R,258};

	const GEOMETRY_point_t N1 = (GEOMETRY_point_t){450,1600};
	const GEOMETRY_point_t N2 = (GEOMETRY_point_t){750,2100};
	const GEOMETRY_point_t N3 = (GEOMETRY_point_t){900,2500};
	const GEOMETRY_point_t S1 = (GEOMETRY_point_t){800,1300};
	const GEOMETRY_point_t S2 = (GEOMETRY_point_t){1000,1700};
	const GEOMETRY_point_t S3 = (GEOMETRY_point_t){1400,2400};

	static bool_e my_goal_is_05;
	static bool_e my_goal_is_1234;
	static bool_e my_goal_is_side;

	static enum state_e fail_state;
	static enum state_e success_state;

	error_e ret;
	ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			if(i_am_in_square_color(0,500,1000,1500))
			{
				if(global.color == BLUE)
					state = TURN_TO_SEE_FOES;
				else
					state = COMPUTE;
			}
			else
				state = ERROR;	//impossible de continuer dans cette sub si on est pas au bon endroit !
			break;
		case TURN_TO_SEE_FOES:	//Si on est bleu, la zone adverse est dans l'angle mort de l'hokuyo, on préfère faire une rotation pour y voir plus clair !
			state = try_go_angle(PI4096/4, state, COMPUTE, COMPUTE, FAST,ANY_WAY,END_AT_BRAKE);
			break;
		case COMPUTE:

			my_goal_is_05 = (dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE5_YELLOW_OUTDOOR);
			my_goal_is_1234 = (dzone == DZONE1_BLUE_INDOOR || dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE3_MIDDLE_YELLOW || dzone == DZONE4_YELLOW_INDOOR);
			my_goal_is_side = (dzone == DZONE6_YELLOW_SIDE || DZONE7_BLUE_SIDE);
			if((my_goal_is_05 || my_goal_is_1234 || my_goal_is_side) == FALSE)
				state = ERROR;	//sécurité
			else if(foe_in_square_color(FALSE, 400,800,1600,3000, FOE_TYPE_HOKUYO))	//Adversaire au north !
				state = GOTO_S1;
			else if(foe_in_square_color(FALSE, 800,1600,1600,3000, FOE_TYPE_HOKUYO))	//Adversaire au south !
				state = GOTO_N1;
			else if(dzone == DZONE6_YELLOW_SIDE)	//Point non accessible à cause du cratère
				state = GOTO_N1;
			else
				state = GOTO_GOAL;
			break;
		case GOTO_S1:
			if(entrance)
			{
				success_state = GOTO_S2;
				if(last_state==COMPUTE)
					fail_state = GOTO_N1;
				else if(last_state == GOTO_N1)
					fail_state = GOTO_S2;
				else
					fail_state = GOTO_S2;	//Never happen...
			}
			state = try_going(S1.x, COLOR_Y(S1.y), state, success_state, fail_state, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;
		case GOTO_S2:
			if(entrance)
			{
				if(last_state==GOTO_S1 || last_state == GOTO_N1)
					fail_state = GOTO_N2;
				else if(my_goal_is_1234)
					fail_state = GOTO_GOAL;
				else
					fail_state = GOTO_S3;	//Never happen...

				if(my_goal_is_1234)
					success_state = GOTO_GOAL;
				else
					success_state = GOTO_S3;
			}
			state = try_going(S2.x, COLOR_Y(S2.y), state, success_state, fail_state, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;
		case GOTO_S3:
			if(entrance)
			{
				if(my_goal_is_1234)
					state = ERROR;	//Never happen ! security

				if(my_goal_is_05)
				{
					fail_state = ERROR;	//S3 innaccessible, donc zone KO !
					success_state = GOTO_GOAL;
				}
				else	//goal side !
				{
					fail_state = GOTO_GOAL;
					success_state = GOTO_GOAL;
				}
			}
			else
				state = try_going(S3.x, COLOR_Y(S3.y), state, success_state, fail_state, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;


		case GOTO_N1:
			if(entrance)
			{
				success_state = GOTO_N2;
				if(last_state==COMPUTE)
					fail_state = GOTO_S1;
				else if(last_state == GOTO_S1)
					fail_state = GOTO_N2;
				else
					fail_state = GOTO_N2;	//Never happen...
			}
			state = try_going(N1.x, COLOR_Y(N1.y), state, success_state, fail_state, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);

			break;
		case GOTO_N2:
			if(entrance)
			{
				if(last_state==GOTO_N1 || last_state == GOTO_S1)
					fail_state = GOTO_S2;
				else
					fail_state = GOTO_N3;	//Never happen...

				if(my_goal_is_1234)
					success_state = GOTO_GOAL;
				else
					success_state = GOTO_N3;
			}
			state = try_going(N2.x, COLOR_Y(N2.y), state, success_state, fail_state, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GOTO_N3:
			if(entrance)
			{
				if(my_goal_is_1234)
					state = ERROR;	//Never happen ! security

				if(my_goal_is_05)
				{
					fail_state = GOTO_GOAL;	//N3 innaccessible, donc zone KO !
					success_state = GOTO_GOAL;
				}
				else	//goal side !
				{
					fail_state = GOTO_GOAL;
					success_state = GOTO_GOAL;
				}
			}
			else
				state = try_going(N3.x, COLOR_Y(N3.y), state, success_state, fail_state, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;
		case GOTO_GOAL:
		{
			static GEOMETRY_point_t goal;
			if(entrance)
			{
				if(my_goal_is_1234)
				{
					goal.x = GOAL_DZ1234.x;
					goal.y = COLOR_Y(GOAL_DZ1234.y);
				}
				else if(my_goal_is_05)
				{
					goal.x = GOAL_DZ05.x;
					goal.y = COLOR_Y(GOAL_DZ05.y);
				}
				else
				{
					if(global.color == BLUE)
						goal = GOAL_DSIDE_YELLOW;
					else
						goal = GOAL_DSIDE_BLUE;
				}
			}
			state = try_going(goal.x, goal.y, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;}
		case ERROR:
			//Nous n'avons pas réussi à nous rendre au point demandé !
			ret = NOT_HANDLED;
			RESET_MAE();
			break;
		case DONE:
			//On est content...
			ret = END_OK;
			RESET_MAE();
			break;
		default:
			state = ERROR;
			break;
	}


	return ret;
}



#define PUSH_BEFORE_DISPOSE_DISABLE			0		//Si 1 : on désactive le poussage avant dépose.
#define ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE		(PI4096/18) 	//10 degrés //angle d'erreur en dessous duquel on accèpte la correction !
#define DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE	100
#define R2				1.41				//Racine carré de 2, à 3 queues de vaches près
#define R_TO_CENTER		650					//Rush goal to center (2000;1500)
#define dHB				250					//Point H vers point B
#define	MOONBASE_TO_R	68					//Distance entre le centre de la moonbase et le bord (80/2+28)
#define dRA				(MOONBASE_TO_R+150)	//Distance entre le centre de la moonbase et l'axe de turning
#define dRH				(MOONBASE_TO_R+115)	//Distance entre le centre de la moonbase et l'axe de dépose / poussage
#define FN_FN1			120					//Distance entre deux déposes de modules

#define dHC_OUTDOOR		30			//distance entre le Rush Goal et le premier module posable, côté cratère
#define dHC_INDOOR		180			//distance entre le Rush Goal et le premier module posable, côté intérieur

#define	F0D_LONG		100			//L'extraction du dispose axis se fait en deux temps (D puis E)
#define F0D_LARGE		5
#define F0E_LONG		200
#define F0E_LARGE		(dRA - dRH)

//Cette fonction a pour but d'effectuer la dépose de module sur l'une des 3 zones centrales.
//On doit lui préciser sur quelle zone on veut déposer, et via quel côté...
error_e sub_anne_depose_modules_centre(moduleMoonbaseLocation_e moonbase, ELEMENTS_side_match_e basis_side)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_CENTRE,
			INIT,
			COMPUTE_POINTS,
			GET_IN,
			GET_IN_FROM_OUR_ROCKET_TO_DZONE,
			PATHFIND,
			RUSH_AND_COMPUTE_O_H_A_B_C,
			EXTRACT_FROM_RUSH_TO_A,
			EXTRACT_FROM_RUSH_TO_A_TO_GETOUT,
			//GOTO_B,
			//PUSH_TO_C,
			COMPUTE_F_E_D,
			GOTO_E,
			//GOTO_D_AND_E,
			GOTO_D_FX,
			COMPUTE_DISPOSE_MODULE,
			DISPOSE_MODULE,
			DISPOSE_LAST_MODULE,
			GOTO_NEXT_F,
			//BACK_TO_PREVIOUS_F,
			PUSH_DISPOSED_MODULES_COMPUTE,
			PUSH_DISPOSED_MODULES_GO_PUSH_OUT,
			WAIT_POUSSIX,
			PUSH_DISPOSED_MODULES_GO_PUSH_IN,
			GET_OUT_TO_B,
			EXTRACT_MANAGER,
			ERROR,
			DONE
		);


	static bool_e we_are_disposing_the_last_module;
	static bool_e we_need_to_push_after_dispose;
	static bool_e pusher_is_up = FALSE;
	static dispose_zone_t dzone;
	static color_e color_side;		//Défini si par rapport à une zone on va poser plutôt du côté bleu ou jaune
	static bool_e outdoor;			//Défini si la dépose est au sud (c'est à dire en étant placé près d'un cratère) sinon au nord (entre 2 zones !)


	static GEOMETRY_point_t GetIn;	//Point d'entrée pour continuer 				-> Défini en dur
	static GEOMETRY_point_t G;		//Goal (but du rush... point non atteignable) 	-> Défini en dur
	static GEOMETRY_point_t O;		//Origine du robot à la fin du rush				-> Mesuré lorsqu'on s'y trouve
	static GEOMETRY_vector_t OR;	//vecteur OR (R=centre de la zone de dépose)	->
	static GEOMETRY_vector_t RA;	//vecteur RA (A=projeté de R sur l'axe de dispose ou de push)
	static GEOMETRY_vector_t RH;	//vecteur RH (H=projeté de R sur l'axe de turning)
	static GEOMETRY_point_t A;
	static GEOMETRY_point_t H;

	static GEOMETRY_vector_t HB;	//vecteur HB (B=extraction de bout de ligne pour tourner sur l'axe de dépose)
	static GEOMETRY_vector_t HC;	//vecteur HC (C=point le plus profond du push) -> dépend de la zone choisie (cratère !)
	static GEOMETRY_point_t B;
	static GEOMETRY_point_t C;

	//Le point C pourra être adapté si on push des modules ou si on tombe en erreur en avancant !
	//Le point F0 est calculé selon la réussite à atteindre le point C... et le côté choisi (notamment s'il y a déjà des modules)
	//On en déduit alors les points D et E...
	static GEOMETRY_point_t FX;		//Résultat d'une position atteinte (mesure si échec, théorie si réussite)
	static GEOMETRY_point_t D;
	static GEOMETRY_point_t E;

	static GEOMETRY_vector_t F0D;	//vecteur F0D (D=point pour s'extraire doucement de l'axe de push)
	static GEOMETRY_vector_t F0E;	//vecteur F0E (E=point pour s'extraire carrément de l'axe de push)

	static GEOMETRY_vector_t Fn_to_next;	//Déplacement entre Fn et Fn+1 !
	static GEOMETRY_vector_t vector_cm;	//vecteur unitaire qui indique la direction de la zone !

	static GEOMETRY_point_t Push_out;
	static GEOMETRY_point_t Push_in;

	//Le Getout est le point B !

	static uint8_t remaining_dispose_modules_try;
	//Trajet nominal :

	// GetIn -> G(non atteint) -> A -> B -> C (calcul F0) -> D -> E -> F0 -> F1 -> ... -> F3 -> B



	switch(state){
		case INIT:

			remaining_dispose_modules_try = 5;	//Nb de modules qu'opn essaye de déposer...

			//Color_side
			if(basis_side == OUR_SIDE)
				color_side = global.color;
			else
				color_side = (global.color==BLUE)?YELLOW:BLUE;

			//South (ou north !)
			if((moonbase == MODULE_MOONBASE_OUR_CENTER && basis_side == OUR_SIDE)
					||  (moonbase == MODULE_MOONBASE_ADV_CENTER && basis_side == ADV_SIDE))
				outdoor = TRUE;
			else
				outdoor = FALSE;

			//Dispose Zone !
			switch(moonbase)
			{
				case MODULE_MOONBASE_OUR_CENTER:
					if(global.color == BLUE)
						dzone = (basis_side == OUR_SIDE)?DZONE0_BLUE_OUTDOOR:DZONE1_BLUE_INDOOR;
					else
						dzone = (basis_side == OUR_SIDE)?DZONE5_YELLOW_OUTDOOR:DZONE4_YELLOW_INDOOR;
					break;
				case MODULE_MOONBASE_MIDDLE:
					if(basis_side == OUR_SIDE)
						dzone = (global.color == BLUE)?DZONE2_MIDDLE_BLUE:DZONE3_MIDDLE_YELLOW;
					else
						dzone = (global.color == BLUE)?DZONE3_MIDDLE_YELLOW:DZONE2_MIDDLE_BLUE;

					break;
				case MODULE_MOONBASE_ADV_CENTER:
					if(global.color == BLUE)
						dzone = (basis_side == OUR_SIDE)?DZONE4_YELLOW_INDOOR:DZONE5_YELLOW_OUTDOOR;
					else
						dzone = (basis_side == OUR_SIDE)?DZONE1_BLUE_INDOOR:DZONE0_BLUE_OUTDOOR;
					break;
				default:
					break;
			}

			if(moonbase == MODULE_MOONBASE_OUR_SIDE || moonbase == MODULE_MOONBASE_ADV_SIDE)
				state = ERROR;	//C'est pas cette fonction qui gère la dépose latérale !
			else
				state = COMPUTE_POINTS;
			break;
		case COMPUTE_POINTS:
			//On calcule les points qui seront utilisés dans les trajectoires.
			if(dzone == DZONE0_BLUE_OUTDOOR)
			{
				GetIn = (GEOMETRY_point_t){1725,860};
				if(i_am_in_square(1000, 1800, 200, 650))
					state = GET_IN;
				else if(i_am_in_square(0,500,1500,2000) && global.color == YELLOW)
					state = GET_IN_FROM_OUR_ROCKET_TO_DZONE;
				else
					state = PATHFIND;
			}
			else if(dzone == DZONE5_YELLOW_OUTDOOR)
			{
				GetIn = (GEOMETRY_point_t){1725,2140};
				if(i_am_in_square(1000, 1800, 2350, 2800))
					state = GET_IN;
				else if(i_am_in_square(0,500,1000,1500) && global.color == BLUE)
					state = GET_IN_FROM_OUR_ROCKET_TO_DZONE;
				else
					state = PATHFIND;
			}
			else if(dzone == DZONE1_BLUE_INDOOR || dzone == DZONE2_MIDDLE_BLUE)
			{
				GetIn = (GEOMETRY_point_t){1350,1240};
				if(i_am_in_square(0, 1300, 1000, 1300))
					state = GET_IN;
				else if(i_am_in_square(0,500,1500,2000) && global.color == YELLOW)
					state = GET_IN_FROM_OUR_ROCKET_TO_DZONE;
				else
					state = PATHFIND;
			}
			else	//dzone 3 ou 4
			{
				GetIn = (GEOMETRY_point_t){1350,1760};
				if(i_am_in_square(0, 1300, 1700, 2000))
					state = GET_IN;
				else if(i_am_in_square(0,500,1000,1500) && global.color == BLUE)
					state = GET_IN_FROM_OUR_ROCKET_TO_DZONE;
				else
					state = PATHFIND;
			}

			//Goal (but du rush... point non atteignable) 	-> Défini en dur
			if(dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE1_BLUE_INDOOR)
				G = (GEOMETRY_point_t){2000-R_TO_CENTER/R2,1500-R_TO_CENTER/R2};
			else if(dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE3_MIDDLE_YELLOW)
				G = (GEOMETRY_point_t){2000-R_TO_CENTER,1500};
			else
				G = (GEOMETRY_point_t){2000-R_TO_CENTER/R2,1500+R_TO_CENTER/R2};

			switch(dzone)
			{
				case DZONE0_BLUE_OUTDOOR:
					OR = (GEOMETRY_vector_t){-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){dRA/R2, -dRA/R2};
					RH = (GEOMETRY_vector_t){dRH/R2, -dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, -dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_OUTDOOR/R2, dHC_OUTDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, -FN_FN1/R2};
					vector_cm = (GEOMETRY_vector_t){-10/R2, -10/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG+F0D_LARGE)/R2, (-F0D_LONG-F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG+F0E_LARGE)/R2, (-F0E_LONG-F0E_LARGE)/R2};
					break;
				case DZONE1_BLUE_INDOOR:
					OR = (GEOMETRY_vector_t){(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){-dRA/R2, dRA/R2};
					RH = (GEOMETRY_vector_t){-dRH/R2, dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, -dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_INDOOR/R2, dHC_INDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, -FN_FN1/R2};
					vector_cm = (GEOMETRY_vector_t){-10/R2, -10/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG-F0D_LARGE)/R2, (-F0D_LONG+F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG-F0E_LARGE)/R2, (-F0E_LONG+F0E_LARGE)/R2};
					break;
				case DZONE2_MIDDLE_BLUE:
					OR = (GEOMETRY_vector_t){0,MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
					RA = (GEOMETRY_vector_t){0,-dRA};
					RH = (GEOMETRY_vector_t){0,-dRH};
					HB = (GEOMETRY_vector_t){-dHB, 0};
					HC = (GEOMETRY_vector_t){dHC_INDOOR, 0};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1, 0};
					vector_cm = (GEOMETRY_vector_t){-10, 0};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG), -F0D_LARGE};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG), -F0E_LARGE};
					break;
				case DZONE3_MIDDLE_YELLOW:
					OR = (GEOMETRY_vector_t){0,-MOONBASE_TO_R-SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
					RA = (GEOMETRY_vector_t){0,dRA};
					RH = (GEOMETRY_vector_t){0,dRH};
					HB = (GEOMETRY_vector_t){-dHB, 0};
					HC = (GEOMETRY_vector_t){dHC_INDOOR, 0};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1, 0};
					vector_cm = (GEOMETRY_vector_t){-10, 0};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG), F0D_LARGE};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG), F0E_LARGE};
					break;
				case DZONE4_YELLOW_INDOOR:
					OR = (GEOMETRY_vector_t){(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){-dRA/R2, -dRA/R2};
					RH = (GEOMETRY_vector_t){-dRH/R2, -dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_INDOOR/R2, -dHC_INDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, FN_FN1/R2};
					vector_cm = (GEOMETRY_vector_t){-10/R2, 10/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG-F0D_LARGE)/R2, (F0D_LONG-F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG-F0E_LARGE)/R2, (F0E_LONG-F0E_LARGE)/R2};
					break;
				case DZONE5_YELLOW_OUTDOOR:
					OR = (GEOMETRY_vector_t){-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){dRA/R2, dRA/R2};
					RH = (GEOMETRY_vector_t){dRH/R2, dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_OUTDOOR/R2, -dHC_OUTDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, FN_FN1/R2};
					vector_cm = (GEOMETRY_vector_t){-10/R2, 10/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG+F0D_LARGE)/R2, (F0D_LONG+F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG+F0E_LARGE)/R2, (F0E_LONG+F0E_LARGE)/R2};
					break;
			}


			//Seront calculables à partir du rush :
			//O : Origine du robot à la fin du rush				-> Mesuré lorsqu'on s'y trouve
			//OR;	//vecteur OR (R=centre de la zone de dépose)
			//	A = O + OR + RA;
			//	H = O + OR + RH;
			//	B = H + HB;
			//	C = H + HC;

			//Seront calculables à partir du push :
			//Le point F0 est calculé selon la réussite à atteindre le point C... et le côté choisi (notamment s'il y a déjà des modules)
			//On en déduit alors les points D et E...
			//F0;		//Résultat d'une position atteinte (mesure si échec, théorie si réussite)
			//  D = F0 + F0D;
			//  E = F0 + F0E;
			//  F1 = F0 + Fn_to_next;	//Si existe
			//  F2 = F1 + Fn_to_next;	//Si existe
			//  F3 = F2 + Fn_to_next;	//Si existe

			break;
		case GET_IN_FROM_OUR_ROCKET_TO_DZONE:
			//Cet état n'est accessible que si l'on sort juste de notre fusée color !
			state = check_sub_action_result(sub_from_our_rocket_to_dzone(dzone), state, GET_IN, ERROR);
			break;

		case GET_IN:	//On file direct vers le point de get_in
			state = try_going(GetIn.x, GetIn.y, state, RUSH_AND_COMPUTE_O_H_A_B_C, PATHFIND, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATHFIND:
			state = ASTAR_try_going(GetIn.x, GetIn.y, state, RUSH_AND_COMPUTE_O_H_A_B_C, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case RUSH_AND_COMPUTE_O_H_A_B_C:
			state = try_rush(G.x, G.y, state, EXTRACT_FROM_RUSH_TO_A, EXTRACT_FROM_RUSH_TO_A_TO_GETOUT, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE())
			{
				if(moonbase == MODULE_MOONBASE_MIDDLE)
				{

					if(color_side==BLUE)
					{
						O = (GEOMETRY_point_t){global.pos.x,1500-MOONBASE_TO_R-SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
						PROP_set_position(global.pos.x, O.y,PI4096/2);
					}
					else
					{
						O = (GEOMETRY_point_t){global.pos.x,1500+MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
						PROP_set_position(global.pos.x, O.y,-PI4096/2);
					}
				}
				else
				{
					O = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					if(absolute(global.pos.angle - PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,PI4096/4);
					else if(absolute(global.pos.angle + PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,-PI4096/4);
					else if(absolute(global.pos.angle - 3*PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,3*PI4096/4);
					else if(absolute(global.pos.angle + 3*PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,-3*PI4096/4);
				}

				H = (GEOMETRY_point_t){O.x + OR.x + RH.x, O.y + OR.y + RH.y};
				A = (GEOMETRY_point_t){O.x + OR.x + RA.x, O.y + OR.y + RA.y};
				B = (GEOMETRY_point_t){H.x + HB.x, H.y + HB.y};
				C = (GEOMETRY_point_t){H.x + HC.x, H.y + HC.y};
				FX = (GEOMETRY_point_t){C.x, C.y};	//on fait l'hypothèse que C est atteignable, c'est donc notre F0
					//ce point sera susceptible de bouger si on montre que C n'est pas atteignable !
				error_printf("Points mesurés et calculés :\n");
				error_printf("O :%4d ; %4d\n", O.x, O.y);
				error_printf("H :%4d ; %4d\n", H.x, H.y);
				error_printf("A :%4d ; %4d\n", A.x, A.y);
				error_printf("B :%4d ; %4d\n", B.x, B.y);
				error_printf("C :%4d ; %4d\n", C.x, C.y);

			}
			break;

		case EXTRACT_FROM_RUSH_TO_A:
			state = try_going(A.x, A.y, state, COMPUTE_F_E_D, RUSH_AND_COMPUTE_O_H_A_B_C, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case EXTRACT_FROM_RUSH_TO_A_TO_GETOUT:
			//On recule après un rush qui a échoué... dans l'optique de quitter la sub !
			state = try_going(A.x, A.y, state, GET_OUT_TO_B, RUSH_AND_COMPUTE_O_H_A_B_C, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		/*case GOTO_B:
			//inverser le sens si un servo de poussage est ajouté côté dépose.
			state = try_going(B.x, B.y, state, PUSH_TO_C, COMPUTE_F_E_D, FAST, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PUSH_TO_C:
			if (entrance)
			{
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				pusher_is_up = TRUE;
			}
			//La flemme d'attendre que le pusher soit descendu pour avancer...
			//Si ca pose problème, on pourra toujours ajouter un état intermédiaire avant ce PUSH_TO_C

			//inverser le sens si un servo de poussage est ajouté côté dépose.
			//il peut être nominal d'échouer lors de cette trajectoire (si on bloque contre des modules...)
			state = try_going(C.x, C.y, state, COMPUTE_F_E_D, ERROR, FAST, (color_side==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(state == ERROR)
			{
				state = COMPUTE_F_E_D;
				FX = (GEOMETRY_point_t){global.pos.x , global.pos.y};	//le point C n'est pas atteignable, notre point actuel servira donc de F0.
			}
			break;
			*/
		case COMPUTE_F_E_D:
			FX = (GEOMETRY_point_t){C.x, C.y};
			D = (GEOMETRY_point_t){FX.x + F0D.x, FX.y + F0D.y};
			E = (GEOMETRY_point_t){FX.x + F0E.x, FX.y + F0E.y};

			error_printf("FX (au début) :%4d ; %4d\n", FX.x, FX.y);
			error_printf("D :%4d ; %4d\n", D.x, D.y);
			error_printf("E :%4d ; %4d\n", E.x, E.y);
			state = GOTO_E;
			break;
		case GOTO_E:{
			static way_e way;
			if(entrance)
			{
				if(E.x < global.pos.x)	//On doit monter... vers le north
					way = (color_side==BLUE)?FORWARD:BACKWARD;
				else					//On doit descendre vers le south..
					way = (color_side==BLUE)?BACKWARD:FORWARD;
			}
			state = try_going(E.x, E.y, state, GOTO_D_FX, EXTRACT_MANAGER, FAST, way, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;}


		case GOTO_D_FX:
			//Le sens est important... il faut maintenant qu'on se place pour la pose !
			//state = try_going(FX.x, FX.y, state, COMPUTE_DISPOSE_MODULE, GOTO_D_AND_E, SLOW, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going_multipoint(	(displacement_t []){
											(displacement_t){ (GEOMETRY_point_t) {D.x, D.y}, FAST},
											(displacement_t){ (GEOMETRY_point_t) {FX.x, FX.y}, FAST}
											}, 2, state, COMPUTE_DISPOSE_MODULE, EXTRACT_MANAGER, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;



		case COMPUTE_DISPOSE_MODULE:
			if(remaining_dispose_modules_try)
				remaining_dispose_modules_try--;
			//TODO détection de fin de stock ou de fin de zone ou savoir si c'est le dernier module à poser
			we_are_disposing_the_last_module = (STOCKS_getNbModules(MODULE_STOCK_SMALL) == 1 || remaining_dispose_modules_try == 1)?TRUE:FALSE;
			we_need_to_push_after_dispose =
					(	(global.pos.x < 1650 &&	(dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE5_YELLOW_OUTDOOR))
					|| (global.pos.x < 1500 &&	(dzone == DZONE1_BLUE_INDOOR || dzone == DZONE4_YELLOW_INDOOR))
					|| (global.pos.x < 1350 &&	(dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE3_MIDDLE_YELLOW)))?TRUE:FALSE;

			if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0 || remaining_dispose_modules_try == 0)
				state = GET_OUT_TO_B;			//fini !
			else if(we_are_disposing_the_last_module)
				state = DISPOSE_LAST_MODULE;	//Presque fini !
			else
				state = DISPOSE_MODULE;	//Encore du taff !
			break;
		case DISPOSE_MODULE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER),state,GOTO_NEXT_F,GOTO_NEXT_F);

			//On considère qu'on a posé des modules unicolores de notre couleur... il n'est pas question de venir les retourner plus tard...
			//c'est sans doute perfectible en prenant en compte ce qu'on a réellement déposé.
			if(ON_LEAVE())
			{
				MOONBASES_addModule((global.color==BLUE)?MODULE_BLUE:MODULE_YELLOW, moonbase);

				//C'était pas le dernier, donc j'ai encore au moins un module à poser.
				//Je regarde s'il faut pousser pour lui faire de la place !
				if(we_need_to_push_after_dispose)
					state = PUSH_DISPOSED_MODULES_COMPUTE;
				else
					state = GOTO_NEXT_F;	//pas besoin de pousser, je maintiens le prochain état.

			}
			break;
		case DISPOSE_LAST_MODULE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_AND_FINISH), state, (we_need_to_push_after_dispose)?PUSH_DISPOSED_MODULES_COMPUTE:GET_OUT_TO_B, (we_need_to_push_after_dispose)?PUSH_DISPOSED_MODULES_COMPUTE:GET_OUT_TO_B);

			if(ON_LEAVE())
				MOONBASES_addModule((global.color==BLUE)?MODULE_BLUE:MODULE_YELLOW, moonbase);
			break;
		case GOTO_NEXT_F:
			if(entrance)
				FX = (GEOMETRY_point_t){FX.x + Fn_to_next.x, FX.y + Fn_to_next.y};	//Next point...
			state = try_going(FX.x, FX.y, state, COMPUTE_DISPOSE_MODULE, EXTRACT_MANAGER, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			//Le end at break permettra sans doute de démarrer l'actionneur avant la fin du mouvement, c'est l'éclate !
			break;

		/*case GOTO_D_AND_E:
			if(entrance && pusher_is_up)
			{
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
				pusher_is_up = FALSE;
			}
			state = try_going_multipoint(	(displacement_t []){
											(displacement_t){ (GEOMETRY_point_t) {D.x, D.y}, FAST},
											(displacement_t){ (GEOMETRY_point_t) {E.x, E.y}, FAST}
											}, 2, state, GOTO_D_FX, GOTO_D_FX, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case BACK_TO_PREVIOUS_F:
			if(entrance)
				FX = (GEOMETRY_point_t){FX.x - Fn_to_next.x, FX.y - Fn_to_next.y};	//Previous point...
			state = try_going(FX.x, FX.y, state, GOTO_NEXT_F, (color_side == BLUE)?EXTRACT_RIGHT:EXTRACT_LEFT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;*/
		case PUSH_DISPOSED_MODULES_COMPUTE:
			if(dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE4_YELLOW_INDOOR)
			{
				Push_out = (GEOMETRY_point_t){FX.x + 2*vector_cm.x, FX.y + 2*vector_cm.y};
				Push_in = (GEOMETRY_point_t){FX.x - 15*vector_cm.x, FX.y - 15*vector_cm.y};
				ACT_push_order(ACT_SMALL_CYLINDER_POUSSIX,ACT_SMALL_CYLINDER_POUSSIX_DOWN);	//On le fait dès maintenant...
			}
			else
			{
				Push_out = (GEOMETRY_point_t){FX.x + 12*vector_cm.x, FX.y + 12*vector_cm.y};
				Push_in = (GEOMETRY_point_t){FX.x - 5*vector_cm.x, FX.y - 5*vector_cm.y};
			}

			state = PUSH_DISPOSED_MODULES_GO_PUSH_OUT;
			break;
		case PUSH_DISPOSED_MODULES_GO_PUSH_OUT:

			state = try_going(Push_out.x, Push_out.y, state, WAIT_POUSSIX, EXTRACT_MANAGER, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				if(dzone == DZONE1_BLUE_INDOOR || dzone == DZONE3_MIDDLE_YELLOW || dzone == DZONE5_YELLOW_OUTDOOR)
					ACT_push_order(ACT_SMALL_CYLINDER_POUSSIX,ACT_SMALL_CYLINDER_POUSSIX_DOWN);
			}
			break;
		case WAIT_POUSSIX:
			state = check_act_status(ACT_QUEUE_Small_cylinder_poussix, state, PUSH_DISPOSED_MODULES_GO_PUSH_IN, PUSH_DISPOSED_MODULES_GO_PUSH_IN);
			break;
		case PUSH_DISPOSED_MODULES_GO_PUSH_IN:
			state = try_going(Push_in.x, Push_in.y, state, (we_are_disposing_the_last_module)?GET_OUT_TO_B:GOTO_NEXT_F, EXTRACT_MANAGER, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				ACT_push_order(ACT_SMALL_CYLINDER_POUSSIX,ACT_SMALL_CYLINDER_POUSSIX_UP);
			}
			break;
		case GET_OUT_TO_B:
			state = try_going(B.x, B.y, state, DONE, EXTRACT_MANAGER, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		//Stratégie d'extraction en cas de merdier... demerden Sie sich!
		case EXTRACT_MANAGER:
			if(entrance)
			{
				ACT_push_order(ACT_SMALL_CYLINDER_POUSSIX,ACT_SMALL_CYLINDER_POUSSIX_UP);
			}
			state = check_sub_action_result(sub_ExtractMoonbase(dzone), state, ERROR, ERROR);
			break;

		case ERROR:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IDLE);
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IDLE);
			}
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
			{
				error_printf("default case in sub_anne_depose_modules_centre\n");
			}
			break;
	}
	return IN_PROGRESS;
}



static error_e sub_ExtractMoonbase(dispose_zone_t dzone)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_EXTRACT_MOONBASE,
				INIT,
				COMPUTE_POINTS,
				EXTRACT_TO_COMPUTED_POINT,
				BACK_TO_COMPUTED_INDOOR,
				EXTRACT_LEFT,
				EXTRACT_RIGHT,
				EXTRACT_INDOOR,
				DONE,
				ERROR
				);
	error_e ret;
	ret = IN_PROGRESS;
	static GEOMETRY_point_t projete;
	static GEOMETRY_point_t ExtractPoint;
	static GEOMETRY_point_t ExtractLeft;
	static GEOMETRY_point_t ExtractRight;
	static GEOMETRY_point_t IndoorPoint;
	static GEOMETRY_vector_t vector_cm;
	static GEOMETRY_vector_t vector_extract_cm;
	static Uint8 nb_round_return;
	static Sint16 distance_to_moonbase;
	static bool_e way_getting_oudoor;	//Vrai si on est dans le sens où on va vers la sortie.. Faut si on revient vers l'intérieur avant de retenter de sortir.

	switch(state)
	{
			case INIT:
				//AVOIDANCE_activeSmallAvoidance(TRUE);
				//TODO : ca marche pas top cette histoire !
				way_getting_oudoor = TRUE;
				if(dzone <= DZONE5_YELLOW_OUTDOOR)
					state = COMPUTE_POINTS;
				else
					state = ERROR;
				nb_round_return = 0;
				break;
			case COMPUTE_POINTS:
				//Calcul du projeté du robot au centre de la ligne de dépose.
				if(dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE1_BLUE_INDOOR)
				{
					projete = (GEOMETRY_point_t){		(2000-(1500-global.pos.y) + global.pos.x)/2,
														(global.pos.y + 1500-(2000-global.pos.x))/2};
				}
				else if(dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE3_MIDDLE_YELLOW)
				{
					projete = (GEOMETRY_point_t){global.pos.x, 1500};
				}
				else
				{
					projete = (GEOMETRY_point_t){		(2000-(global.pos.y-1500) + global.pos.x)/2,
														(global.pos.y + 1500+(2000-global.pos.x))/2};

				}

				distance_to_moonbase = GEOMETRY_distance(projete, (GEOMETRY_point_t){global.pos.x, global.pos.y}) - MOONBASE_TO_R;
				if(distance_to_moonbase<0)
					distance_to_moonbase = 0;	//Ecretage !
				if(dzone == DZONE0_BLUE_OUTDOOR)
				{
					ExtractLeft = (GEOMETRY_point_t){1800,750};
					ExtractRight = (GEOMETRY_point_t){1500,750};
					IndoorPoint = (GEOMETRY_point_t){1725,860};
				}
				else if(dzone == DZONE5_YELLOW_OUTDOOR)
				{
					ExtractLeft = (GEOMETRY_point_t){1500,2250};
					ExtractRight = (GEOMETRY_point_t){1800,2250};
					IndoorPoint = (GEOMETRY_point_t){1725,2140};
				}
				else if(dzone == DZONE1_BLUE_INDOOR || dzone == DZONE2_MIDDLE_BLUE)
				{
					ExtractLeft = (GEOMETRY_point_t){1200,1000};
					ExtractRight = (GEOMETRY_point_t){1050,1250};
					IndoorPoint = (GEOMETRY_point_t){1500,1300};
				}
				else	//dzone 3 ou 4
				{
					ExtractLeft = (GEOMETRY_point_t){1050,1750};
					ExtractRight = (GEOMETRY_point_t){1200,2000};
					IndoorPoint = (GEOMETRY_point_t){1500,1700};
				}

				//Le vector_cm nous indique l'orientation de la zone.
				//Le vecteur d'extraction nous indique comment sortir de la zone.
				//La somme des deux donne une trajectoire relative pertinente pour sortir doucement.
				//Une multiplication du vector_extract_cm permet des rotations plus importantes !
				switch(dzone)
				{
					case DZONE0_BLUE_OUTDOOR:	vector_cm = (GEOMETRY_vector_t){-100/R2, -100/R2};	vector_extract_cm = (GEOMETRY_vector_t){0,-10/R2};	break;
					case DZONE1_BLUE_INDOOR:	vector_cm = (GEOMETRY_vector_t){-100/R2, -100/R2};	vector_extract_cm = (GEOMETRY_vector_t){0,10/R2};	break;
					case DZONE2_MIDDLE_BLUE:	vector_cm = (GEOMETRY_vector_t){-100, 0};			vector_extract_cm = (GEOMETRY_vector_t){0,-10};		break;
					case DZONE3_MIDDLE_YELLOW:	vector_cm = (GEOMETRY_vector_t){-100, 0};			vector_extract_cm = (GEOMETRY_vector_t){0,+10};		break;
					case DZONE4_YELLOW_INDOOR:	vector_cm = (GEOMETRY_vector_t){-100/R2, 100/R2};	vector_extract_cm = (GEOMETRY_vector_t){0,-10/R2};	break;
					case DZONE5_YELLOW_OUTDOOR:	vector_cm = (GEOMETRY_vector_t){-100/R2, 100/R2};	vector_extract_cm = (GEOMETRY_vector_t){0,+10/R2};	break;
					default:	break;
				}
				error_printf("Distance to moonbase : %d\n",distance_to_moonbase);
				if(way_getting_oudoor)
					state = EXTRACT_TO_COMPUTED_POINT;	//Hypothèse
				else
					state = BACK_TO_COMPUTED_INDOOR;
				nb_round_return++;
				if(distance_to_moonbase < 110 && nb_round_return < 4)
					vector_extract_cm.y *= 1;			//On s'extrait doucement
				else if(distance_to_moonbase < 120 && nb_round_return < 8)
					vector_extract_cm.y *= 2;			//On s'extrait moyenement
				else if(distance_to_moonbase < 140)
					vector_extract_cm.y *= 4;			//On s'extrait fortement
				else									//Turning point ! :D On vise des points LEFT / RIGHT fixés en dur.
				{
					if(dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE4_YELLOW_INDOOR)
						state = EXTRACT_LEFT;
					else
						state = EXTRACT_RIGHT;
				}



				break;
			case EXTRACT_TO_COMPUTED_POINT:
				if(entrance)
				{
					ExtractPoint = (GEOMETRY_point_t){global.pos.x + vector_cm.x, global.pos.y + vector_cm.y + vector_extract_cm.y};
				}
				state = try_going(ExtractPoint.x, ExtractPoint.y, state, COMPUTE_POINTS, BACK_TO_COMPUTED_INDOOR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				if(state == COMPUTE_POINTS)	//En cas de succès, on considère qu'on est sorti si on atteint un x assez faible !
				{
					if(global.pos.x < 1100)
						state = DONE;		//Fini !
				}
				if(state == BACK_TO_COMPUTED_INDOOR)	//En cas d'échec, on force le passage par COMPUTE_POINTS.
				{
					way_getting_oudoor = FALSE;
					state = COMPUTE_POINTS;
				}
				break;
			case BACK_TO_COMPUTED_INDOOR:
				if(entrance)
				{
					//On s'autorise de reculer de 3cm...
					//Ce point est osé, et un peu arbitraire... wait & see!
					ExtractPoint = (GEOMETRY_point_t){global.pos.x - (3*vector_cm.x)/10, global.pos.y - (3*vector_cm.y)/10 + (3*vector_extract_cm.y)/10};
				}
				state = try_going(ExtractPoint.x, ExtractPoint.y, state, COMPUTE_POINTS, COMPUTE_POINTS, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				if(ON_LEAVE())
					way_getting_oudoor = TRUE;
				break;

			case EXTRACT_LEFT:
				state = try_going(ExtractLeft.x, ExtractLeft.y, state, DONE, EXTRACT_INDOOR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				break;
			case EXTRACT_RIGHT:
				state = try_going(ExtractRight.x, ExtractRight.y, state, DONE, EXTRACT_INDOOR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				break;
			case EXTRACT_INDOOR:	//On retourne vers l'intérieur... pour provoquer des aller-retours !
			{
				static enum state_e success_or_fail_state;
				if(entrance)
				{
					success_or_fail_state = (last_state == EXTRACT_LEFT)?EXTRACT_RIGHT:EXTRACT_LEFT;
				}
				state = try_going(IndoorPoint.x, IndoorPoint.y, state, success_or_fail_state, success_or_fail_state, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);

				break;
			}

			case DONE:
				ret = END_OK;
				state = INIT;
				break;
			case ERROR:
				ret = NOT_HANDLED;
				state = INIT;
				break;
			default:
				break;
	}
	//if(ret != IN_PROGRESS)
	//	AVOIDANCE_activeSmallAvoidance(FALSE);	//TODO : ca marche pas top cette histoire !
	return ret;
}


#if 0
error_e sub_anne_get_in_depose_modules_centre(moduleTypeDominating_e module_type, moduleStockLocation_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_POS_1,
			GET_IN_POS_2,
			GET_IN_POS_3,
			GET_IN_POS_4,
			GET_IN_POS_5,
			GET_IN_POS_6,
			ERROR,
			DONE
		);
//color Y
	switch(state){
		case INIT:
			if((module_type == MODULE_POLY_DOMINATING) && (basis_side == OUR_SIDE)){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_3;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_SIDE)<6){
					state = GET_IN_POS_1;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_SIDE)<6){
					state = GET_IN_POS_5;
				}else{
					state = ERROR;
				}
			}else if(basis_side == OUR_SIDE){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					state = GET_IN_POS_2;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_4;
				}else{
					state = ERROR;
				}
			}else if((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == OUR_SIDE)){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_3;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					state = GET_IN_POS_1;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_CENTER)<6){
					state = GET_IN_POS_5;
				}else{
					state = ERROR;
				}

			}else if(((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == ADV_SIDE))\
					|| ((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == ADV_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_4;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					state = GET_IN_POS_2;
				}else{
					state = ERROR;
				}

			}else if(((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == ADV_SIDE))\
					||((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == ADV_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_SIDE)<6){
					state = GET_IN_POS_5;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_3;
				}else{
					state = ERROR;
				}

			/*}else if((module_type == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_6; //centre depose du cote adv SIM 1
*/

			}else{
				state = ERROR;
			}
			//ce manager me gonfle je met ça pour mon test on verra à après pour le manager.
			state = GET_IN_POS_3;
			break;

		case GET_IN_POS_1:
			if(entrance){
				drop_zone_bis = POS_1;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_1_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_2:
			if(entrance){
				drop_zone_bis = POS_2;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_2_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_3:
			if(entrance){
				drop_zone_bis = POS_3;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_3_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_4:
			if(entrance){
				drop_zone_bis = POS_4;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_4_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_5:
			if(entrance){
				drop_zone_bis = POS_5;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_5_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_6:
			if(entrance){
				drop_zone_bis = POS_6;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_6_depose_module_centre(), state, DONE, ERROR);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}
#endif



/*
 *
	get in
	calage contre la zone
	calcul des points
	get_out_to_turning_axe
	courbe pour approche de la zone (scan faisable en même temps)
	dépose 1
	dépose 2
	dépose 3
	poussage
	dépose 4 ?

	Si on vient de la prise fusée adverse et que le précédent calage contre cette zone est récent
	-> on peut embrayer direct sur le calcul des points et l'entrée en zone...

	Ce cas peut-être géré en faisant la dépose directement dans la sub de prise fusée adverse ?
		en appelant la sub de dépose side à la première position de dépose... !!!
 */


error_e sub_anne_dispose_modules_side(ELEMENTS_side_match_e side)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DISPOSE_MODULES_SIDE,
			INIT,
			GET_IN,
			GET_IN_FROM_BLUE_ROCKET,
			GET_IN_FROM_YELLOW_ROCKET,
			GET_IN_VIA_SOUTH_FROM_BLUE_ROCKET,
			GET_IN_VIA_SOUTH_FROM_YELLOW_ROCKET,
			ASTAR_GET_IN,
			RUSH_TO_GOAL,
			EXTRACT_FROM_RUSH_TO_A,
			EXTRACT_AFTER_FAILING_RUSH,
			EXTRACT_AFTER_FAILING_RUSH_NORTH,
			EXTRACT_AFTER_FAILING_RUSH_SOUTH,
			GOTO_F0_VIA_E0,
			GOTO_F1,
			GOTO_F2,
			GOTO_P0,
			WAIT_POUSSIX,
			GOTO_P1,
			WAIT_POUSSIX_UP,
			GOTO_F3,
			DISPOSE,
			EXTRACT_BEYOND_F0,
			EXTRACT_BEYOND_F3,
			ERROR,
			DONE
		);
	error_e ret;
	ret = IN_PROGRESS;
	static GEOMETRY_point_t G, O, A, E0, F0, F1, F2, F3, P0, P1;
	static color_e color_side;
	static displacement_t output_curve[2];
	static Uint8 nb_try_extract_from_rush_to_a;
	CAN_msg_t can_msg;
	switch(state)
	{
		case INIT:
			nb_try_extract_from_rush_to_a = 0;
			if(side == OUR_SIDE)
				color_side = global.color;
			else
				color_side = (global.color==BLUE)?YELLOW:BLUE;
			if(color_side == BLUE)
			{
				G = (GEOMETRY_point_t){1000-OFFSET_M_R,0};
				O = (GEOMETRY_point_t){1000-OFFSET_M_R,197};
				A = (GEOMETRY_point_t){1000-OFFSET_M_R,258};
				E0 = (GEOMETRY_point_t){900-OFFSET_M_R,220};
				F0 = (GEOMETRY_point_t){800-OFFSET_M_R,208};
				F1 = (GEOMETRY_point_t){925-OFFSET_M_R,208};
				F2 = (GEOMETRY_point_t){1050-OFFSET_M_R,208};
				F3 = (GEOMETRY_point_t){1075-OFFSET_M_R,208};
				P0 = (GEOMETRY_point_t){1100-OFFSET_M_R,208};
				P1 = (GEOMETRY_point_t){950-OFFSET_M_R,208};

				//TODO ajouter les cas spécifiques où l'on est déjà sur place (après prise fusée adverse notamment !)
				if(i_am_in_square(750,1350,200,1400))
					state = GET_IN;
				else if(i_am_in_square(300,500,1500,2000))	//Après la prise de notre fusée jaune !
					state = GET_IN_FROM_YELLOW_ROCKET;
				else
					state = ASTAR_GET_IN;
			}
			else
			{
				G = (GEOMETRY_point_t){850+OFFSET_M_R,3000};
				O = (GEOMETRY_point_t){850+OFFSET_M_R,2903};
				A = (GEOMETRY_point_t){850+OFFSET_M_R,2742};
				E0 = (GEOMETRY_point_t){950+OFFSET_M_R,2780};
				F0 = (GEOMETRY_point_t){1050+OFFSET_M_R,2792};
				F1 = (GEOMETRY_point_t){925+OFFSET_M_R,2792};
				F2 = (GEOMETRY_point_t){800+OFFSET_M_R,2792};
				F3 = (GEOMETRY_point_t){825+OFFSET_M_R,2792};
				P0 = (GEOMETRY_point_t){750+OFFSET_M_R,2792};
				P1 = (GEOMETRY_point_t){900+OFFSET_M_R,2792};

				if(i_am_in_square(750,1350,1600,2800))
					state = GET_IN;
				else if(i_am_in_square(0,500,1000,1500))	//Après la prise de notre fusée jaune !
					state = GET_IN_FROM_BLUE_ROCKET;
				else
					state = ASTAR_GET_IN;
			}
			break;
		case GET_IN:
			state = try_going(A.x, A.y, state, RUSH_TO_GOAL, ASTAR_GET_IN, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case GET_IN_FROM_BLUE_ROCKET:
			//TODO ajouter prise du module polychrome sur cette courbe ?
		/*	state = try_going_multipoint(	(displacement_t []) {
											(displacement_t) { (GEOMETRY_point_t){700, 1850}, FAST},
											(displacement_t) { (GEOMETRY_point_t){800, 2350}, FAST}
										},
										2, state, GET_IN, ASTAR_GET_IN, FORWARD, DODGE_AND_WAIT, END_AT_BRAKE);
			if(state == ASTAR_GET_IN)
			{
				if(i_am_in_square(200,800,900,1700))
					state = GET_IN_VIA_SOUTH_FROM_BLUE_ROCKET;
			}
			*/
			state = check_sub_action_result(sub_from_our_rocket_to_dzone(DZONE6_YELLOW_SIDE), state, GET_IN, ERROR);
			break;
		case GET_IN_VIA_SOUTH_FROM_BLUE_ROCKET:
			state = try_going_multipoint(	(displacement_t []) {
												(displacement_t) { (GEOMETRY_point_t){1200, 1800}, FAST},
												(displacement_t) { (GEOMETRY_point_t){800, 2350}, FAST}
											},
											2, state, GET_IN, ASTAR_GET_IN, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;
		case GET_IN_FROM_YELLOW_ROCKET:
			state = try_going_multipoint(	(displacement_t []) {
											(displacement_t) { (GEOMETRY_point_t){700, 1150}, FAST},
											(displacement_t) { (GEOMETRY_point_t){800, 650}, FAST}
										},
										2, state, GET_IN, ASTAR_GET_IN, FORWARD, DODGE_AND_WAIT, END_AT_BRAKE);
			if(state == ASTAR_GET_IN)
			{
				//On a échoué...
				if(i_am_in_square(200,800,1300,2100))	//Première partie de la courbe
					state = GET_IN_VIA_SOUTH_FROM_YELLOW_ROCKET;
			}
			break;
		case GET_IN_VIA_SOUTH_FROM_YELLOW_ROCKET:
		/*	state = try_going_multipoint(	(displacement_t []) {
											(displacement_t) { (GEOMETRY_point_t){1200, 1200}, FAST},
											(displacement_t) { (GEOMETRY_point_t){800, 650}, FAST}
										},
										2, state, GET_IN, ASTAR_GET_IN, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
										*/
			state = check_sub_action_result(sub_from_our_rocket_to_dzone(DZONE7_BLUE_SIDE), state, GET_IN, ERROR);
			break;
		case ASTAR_GET_IN:
			state = ASTAR_try_going(A.x, A.y, state, RUSH_TO_GOAL, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case RUSH_TO_GOAL:
			if(entrance){
				// Envoi du filtrage de la zone de transparence
				can_msg.sid = PROP_TRANSPARENCY;
				can_msg.size = SIZE_PROP_TRANSPARENCY;
				can_msg.data.prop_transparency.enable = TRUE;
				can_msg.data.prop_transparency.number = FILTER_ZONE_FOR_SMALL;
				CAN_send(&can_msg);
			}
			state = try_rush(G.x, G.y, state, EXTRACT_FROM_RUSH_TO_A, EXTRACT_FROM_RUSH_TO_A, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE())
			{
				if(color_side == BLUE)
				{
						//Si moins de 10cm d'erreur... (dans le bon sens... pas de limite si on est hors terrain)
					if(		(absolute(global.pos.angle + PI4096/2) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						&&	((global.pos.y - O.y) < DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE) )
						PROP_set_position(global.pos.x, O.y,-PI4096/2);
				}
				else
				{
					if(		(absolute(global.pos.angle - PI4096/2) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						&&	((O.y - global.pos.y) < DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE) )
						PROP_set_position(global.pos.x, O.y,PI4096/2);
				}
			}

			//TODO enrichir avec un scan pour permettre le remplissage d'une zone non vide... en rejoignant après un poussage la bonne case directement.
			break;
		case EXTRACT_FROM_RUSH_TO_A:{
			static enum state_e fail_state;
			if(entrance)
			{
				if(nb_try_extract_from_rush_to_a > 0)
					AVOIDANCE_activeSmallAvoidance(TRUE);
			}
			if(nb_try_extract_from_rush_to_a <= 2)
				fail_state = RUSH_TO_GOAL;
			else if(color_side==BLUE && global.pos.y>80+28+130)	//Si on a réussit à sortir suffisamment et que ca fait déjà deux fois, on se casse !
				fail_state = EXTRACT_AFTER_FAILING_RUSH;
			else if (color_side==YELLOW && global.pos.y<3000-80-28-130)
				fail_state = EXTRACT_AFTER_FAILING_RUSH;
			else
				fail_state = RUSH_TO_GOAL;

			state = try_going(A.x, A.y, state, GOTO_F0_VIA_E0, fail_state, SLOW, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				if(nb_try_extract_from_rush_to_a > 0)
					AVOIDANCE_activeSmallAvoidance(FALSE);
				nb_try_extract_from_rush_to_a++;
			}
			break;}
		case EXTRACT_AFTER_FAILING_RUSH:
			//Choix entre un point au SOUTH et un autre au NORTH
			if(color_side == YELLOW && foe_in_square(FALSE,850, 2000, 2000, 3000, FOE_TYPE_ALL))
				state = EXTRACT_AFTER_FAILING_RUSH_NORTH;
			else if(color_side == BLUE && foe_in_square(FALSE,850, 2000, 0, 1000, FOE_TYPE_ALL))
				state = EXTRACT_AFTER_FAILING_RUSH_NORTH;
			else
				state = EXTRACT_AFTER_FAILING_RUSH_SOUTH;
			break;
		case EXTRACT_AFTER_FAILING_RUSH_SOUTH:
			state = try_going(1200, (color_side==BLUE)?270:2730, state, ERROR, EXTRACT_AFTER_FAILING_RUSH_NORTH, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;
		case EXTRACT_AFTER_FAILING_RUSH_NORTH:
			state = try_going(600, (color_side==BLUE)?270:2730, state, ERROR, EXTRACT_AFTER_FAILING_RUSH_SOUTH, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;
		case GOTO_F0_VIA_E0:
			//BACKWARD pour se mettre du bon côté !!!
			state = try_going_multipoint(	(displacement_t []) {
												(displacement_t) { (GEOMETRY_point_t){E0.x, E0.y}, FAST},
												(displacement_t) { (GEOMETRY_point_t){F0.x, F0.y}, FAST}
											},
											2, state, DISPOSE, EXTRACT_BEYOND_F3, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case DISPOSE:{
			static enum state_e success_state;
			static arg_dipose_mae_e last_module;
			if(entrance)
			{
				if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 1)//Il reste plus d'un module à déposer. Il faudra donc une autre dépose.
				{
					if(last_state == GOTO_F0_VIA_E0)
						success_state = GOTO_F1;
					else if(last_state == GOTO_F1)
						success_state = GOTO_F2;
					else if(last_state == GOTO_F2)
						success_state = GOTO_P0;	//Il faut pousser !
					else
						success_state = EXTRACT_BEYOND_F3;

					last_module = ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER;
				}
				else if(STOCKS_getNbModules(MODULE_STOCK_SMALL)==1)//Dernière dépose
				{
					success_state = EXTRACT_BEYOND_F3;
					last_module = ARG_DISPOSE_ONE_CYLINDER_AND_FINISH;
				}
				else	//On a plus rien à déposer
				{
					state = EXTRACT_BEYOND_F3;
				}
			}
			else	//le else est important en cas de sortie effectuée dans l'entrance !
			{
				state = check_sub_action_result(sub_act_anne_mae_dispose_modules(last_module),state,success_state,EXTRACT_BEYOND_F3);
				if(ON_LEAVE())
				{
					if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0)
						nop(); //TODO : Désactiver la sub de dépose, on est vide !
				}
			}

			break;}
		case GOTO_F1:
			state = try_going(F1.x, F1.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case GOTO_F2:
			state = try_going(F2.x, F2.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GOTO_P0:
			state = try_going(P0.x, P0.y, state, WAIT_POUSSIX, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				if(state == WAIT_POUSSIX)
					ACT_push_order(ACT_SMALL_CYLINDER_POUSSIX,ACT_SMALL_CYLINDER_POUSSIX_DOWN);
			}
			break;
		case WAIT_POUSSIX:
			state = check_act_status(ACT_QUEUE_Small_cylinder_poussix, state, GOTO_P1, GOTO_P1);
			break;
		case GOTO_P1:
			state = try_going(P1.x, P1.y, state, WAIT_POUSSIX_UP, EXTRACT_BEYOND_F3, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				ACT_push_order(ACT_SMALL_CYLINDER_POUSSIX,ACT_SMALL_CYLINDER_POUSSIX_UP);
			}
			break;
		case WAIT_POUSSIX_UP:
			state = check_act_status(ACT_QUEUE_Small_cylinder_poussix, state, GOTO_F3, GOTO_F3);
			break;
		case GOTO_F3:
			state = try_going(F3.x, F3.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case EXTRACT_BEYOND_F0:

			if(entrance)
			{
				//On veut sortir vers le côté où se trouve F0
				//On calcule une courbe de sortie...
				if(color_side == BLUE)
				{
					output_curve[0].point.x = global.pos.x - 100;
					output_curve[0].point.y = MIN(global.pos.y + 15, A.y);	//L'écretage est utile si on vient d'un échec d'extraction précédent
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x - 200;
					output_curve[1].point.y = MIN(global.pos.y + 50, A.y);
					output_curve[1].speed = FAST;
				}
				else
				{
					output_curve[0].point.x = global.pos.x + 100;
					output_curve[0].point.y = MAX(global.pos.y - 15, A.y);
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x + 200;
					output_curve[1].point.y = MAX(global.pos.y - 50, A.y);
					output_curve[1].speed = FAST;
				}
				//TODO borner pour ne pas génèrer de point innaccessible (cratère / obstacle.... !)
			}
			state = try_going_multipoint(	output_curve, 2, state, DONE, EXTRACT_BEYOND_F3, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;

		case EXTRACT_BEYOND_F3:
			if(entrance)
			{
				//On veut sortir vers le côté où se trouve F0
				//On calcule une courbe de sortie...
				if(color_side == BLUE)
				{
					output_curve[0].point.x = global.pos.x + 100;
					output_curve[0].point.y = MIN(global.pos.y + 15, A.y);
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x + 200;
					output_curve[1].point.y = MIN(global.pos.y + 50, A.y);
					output_curve[1].speed = FAST;
				}
				else
				{
					output_curve[0].point.x = global.pos.x - 100;
					output_curve[0].point.y = MAX(global.pos.y - 15, A.y);
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x - 200;
					output_curve[1].point.y = MAX(global.pos.y - 50, A.y);
					output_curve[1].speed = FAST;
				}

				//TODO borner pour ne pas génèrer de point innaccessible (cratère / obstacle.... !)

			}
			state = try_going_multipoint(	output_curve, 2, state, DONE, EXTRACT_BEYOND_F0, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case ERROR:
			// Envoi de la libération de la zone de transparence
			can_msg.sid = PROP_TRANSPARENCY;
			can_msg.size = SIZE_PROP_TRANSPARENCY;
			can_msg.data.prop_transparency.enable = FALSE;
			can_msg.data.prop_transparency.number = FILTER_ZONE_FOR_SMALL;
			CAN_send(&can_msg);

			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		case DONE:
			// Envoi de la libération de la zone de transparence
			can_msg.sid = PROP_TRANSPARENCY;
			can_msg.size = SIZE_PROP_TRANSPARENCY;
			can_msg.data.prop_transparency.enable = FALSE;
			can_msg.data.prop_transparency.number = FILTER_ZONE_FOR_SMALL;
			CAN_send(&can_msg);

			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_anne_return_side\n");
			}
			break;
	}


	return ret;
}

#if 0
error_e sub_anne_dispose_modules_side(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DISPOSE_MODULES_SIDE,
			INIT,
			GET_IN,
			COMPUTE,

			GO_TO_BLUE,
			OPEN_MULTIFONCTION_BLUE,
			RUSH_BLUE,
			CLOSE_PUSHER_BLUE,
			AVANCE_BLUE,
			ENABLE_MAGIC_BLUE,
			COMPUTE_LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_ERROR_BLUE,
			MOVE_BACK_BLUE,
			GET_OUT_BLUE,

			GO_TO_YELLOW,
			OPEN_MULTIFONCTION_YELLOW,
			RUSH_YELLOW,
			CLOSE_PUSHER_YELLOW,
			AVANCE_YELLOW,
			ENABLE_MAGIC_YELLOW,
			COMPUTE_LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_ERROR_YELLOW,
			MOVE_BACK_YELLOW,
			GET_OUT_YELLOW,

			ERROR,
			DONE
		);

#define TIMEOUT_COLOR	(4000)  // Temps au dela duquel on arrête de tourner le module, on a échoué a détecté la couleur
	static time32_t time_timeout = 0;
	Uint8 pos_get_in;
	Uint8 passage;

	static Uint8 nb_cylinder_in_basis = 0;
	static Sint16 tryx, tryy;

	switch(state){
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			state = check_sub_action_result(sub_anne_return_modules_side_get_in(modules), state, COMPUTE, ERROR);
			break;

		case COMPUTE:
			if(((modules == OUR_ELEMENT)&&(global.color == BLUE)) || ((modules == ADV_ELEMENT)&&(global.color == YELLOW))){
				state = GO_TO_BLUE;
			}else if(((modules == ADV_ELEMENT)&&(global.color == BLUE)) || ((modules == OUR_ELEMENT)&&(global.color == YELLOW))){
				state = GO_TO_YELLOW;
			}else
				state = ERROR;
			break;

		case GO_TO_BLUE:
			state = try_going(750, 300, state, OPEN_MULTIFONCTION_BLUE, GET_OUT_BLUE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH_BLUE, RUSH_BLUE);
#warning 'je vois pas quoi mettre en cas d\'erreur'
			break;

		case RUSH_BLUE:
			/*if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}*/
			state = try_rush(1450, 300, state, CLOSE_PUSHER_BLUE, MOVE_BACK_BLUE, BACKWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1200 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_BLUE, MOVE_BACK_BLUE);
			break;

		case AVANCE_BLUE:
			state = try_going(1450, 300, state, ENABLE_MAGIC_BLUE, MOVE_BACK_BLUE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_BLUE:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_BLUE, MOVE_BACK_BLUE);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_BLUE:
			if(global.pos.y>(1800 - nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_BLUE;
			else
				state = MOVE_BACK_BLUE;
			break;

		case LITTLE_MOVE_BACK_BLUE:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_BLUE, LITTLE_MOVE_BACK_ERROR_BLUE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_BLUE:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryy = 1200 - 100*nb_module_returned;
			}
			state = try_going(tryx, 300, state, LITTLE_MOVE_BACK_BLUE, LITTLE_MOVE_BACK_BLUE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_BLUE:
			state = try_going(700, 300, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

#warning 'OUPS! j\'ai très mal négocié les get out, faut revoir la fin'

		case GO_TO_YELLOW:
			state = try_going(750, 2700, state, OPEN_MULTIFONCTION_YELLOW, GET_OUT_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH_BLUE, RUSH_BLUE);
#warning 'je vois pas quoi mettre en cas d\'erreur'
			break;

		case RUSH_YELLOW:
/*			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}*/
			state = try_rush(1100, 2700, state, CLOSE_PUSHER_YELLOW, MOVE_BACK_YELLOW, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1200 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_YELLOW, MOVE_BACK_YELLOW);
			break;

		case AVANCE_YELLOW:
			state = try_going(1200, 2700, state, ENABLE_MAGIC_YELLOW, MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_YELLOW:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_YELLOW, MOVE_BACK_YELLOW);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_YELLOW:
			if(global.pos.y>(1200+nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_YELLOW;
			else
				state = MOVE_BACK_YELLOW;
			break;

		case LITTLE_MOVE_BACK_YELLOW:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_YELLOW, LITTLE_MOVE_BACK_ERROR_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_YELLOW:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryx = 1800 - 70*nb_module_returned;
			}
			state = try_going(tryx, 2700, state, LITTLE_MOVE_BACK_YELLOW, LITTLE_MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_YELLOW:
			state = try_going(1000, 2700, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			{
				error_printf("default case in sub_anne_return_side\n");
			}
			break;
	}

	return IN_PROGRESS;
}
#endif


