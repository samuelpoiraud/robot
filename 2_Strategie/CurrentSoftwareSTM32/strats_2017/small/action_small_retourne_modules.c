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

typedef enum
{
	DZONE0_BLUE_OUTDOOR,
	DZONE1_BLUE_INDOOR,
	DZONE2_MIDDLE_BLUE,
	DZONE3_MIDDLE_YELLOW,
	DZONE4_YELLOW_INDOOR,
	DZONE5_YELLOW_OUTDOOR
}turn_zone_t;


static error_e sub_ExtractMoonbase(turn_zone_t dzone);



#define PUSH_BEFORE_TURN_DISABLE			0		//Si 1 : on d�sactive le poussage avant d�pose.
#define ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE		(PI4096/18) 	//10 degr�s //angle d'erreur en dessous duquel on acc�pte la correction !
#define DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE	100
#define R2				1.41				//Racine carr� de 2, � 3 queues de vaches pr�s
#define R_TO_CENTER		650					//Rush goal to center (2000;1500)
#define dHB				250					//Point H vers point B
#define	MOONBASE_TO_R	68					//Distance entre le centre de la moonbase et le bord (80/2+28)
#define dRA				(MOONBASE_TO_R+150)	//Distance entre le centre de la moonbase et l'axe de turning
#define dRH				(MOONBASE_TO_R+115)	//Distance entre le centre de la moonbase et l'axe de d�pose / poussage
#define FN_FN1			100					//Distance entre deux d�poses de modules

#define dHC_OUTDOOR		30			//distance entre le Rush Goal et le premier module posable, c�t� crat�re
#define dHC_INDOOR		180			//distance entre le Rush Goal et le premier module posable, c�t� int�rieur

#define	F0D_LONG		100			//L'extraction du turn axis se fait en deux temps (D puis E)
#define F0D_LARGE		5
#define F0E_LONG		200
#define F0E_LARGE		(dRA - dRH)

//Cette fonction a pour but d'effectuer la d�pose de module sur l'une des 3 zones centrales.
//On doit lui pr�ciser sur quelle zone on veut d�poser, et via quel c�t�...
error_e sub_anne_turn_modules_centre(moduleMoonbaseLocation_e moonbase, ELEMENTS_side_match_e basis_side)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_TURN_MODULES_CENTRE,
			INIT,
			COMPUTE_POINTS,
			GET_IN,
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
			COMPUTE_TURN_MODULE,
			TURN_MODULE,
			GOTO_NEXT_F,
			GET_OUT_TO_B,
			EXTRACT_MANAGER,
			ERROR,
			DONE
		);


	static bool_e we_are_disposing_the_last_module;
	static bool_e we_need_to_push_after_turn;
	static bool_e pusher_is_up = FALSE;
	static turn_zone_t dzone;
	static color_e color_side;		//D�fini si par rapport � une zone on va poser plut�t du c�t� bleu ou jaune
	static bool_e outdoor;			//D�fini si la d�pose est au sud (c'est � dire en �tant plac� pr�s d'un crat�re) sinon au nord (entre 2 zones !)


	static GEOMETRY_point_t GetIn;	//Point d'entr�e pour continuer 				-> D�fini en dur
	static GEOMETRY_point_t G;		//Goal (but du rush... point non atteignable) 	-> D�fini en dur
	static GEOMETRY_point_t O;		//Origine du robot � la fin du rush				-> Mesur� lorsqu'on s'y trouve
	static GEOMETRY_vector_t OR;	//vecteur OR (R=centre de la zone de d�pose)	->
	static GEOMETRY_vector_t RA;	//vecteur RA (A=projet� de R sur l'axe de turn ou de push)
	static GEOMETRY_vector_t RH;	//vecteur RH (H=projet� de R sur l'axe de turning)
	static GEOMETRY_point_t A;
	static GEOMETRY_point_t H;

	static GEOMETRY_vector_t HB;	//vecteur HB (B=extraction de bout de ligne pour tourner sur l'axe de d�pose)
	static GEOMETRY_vector_t HC;	//vecteur HC (C=point le plus profond du push) -> d�pend de la zone choisie (crat�re !)
	static GEOMETRY_point_t B;
	static GEOMETRY_point_t C;

	//Le point C pourra �tre adapt� si on push des modules ou si on tombe en erreur en avancant !
	//Le point F0 est calcul� selon la r�ussite � atteindre le point C... et le c�t� choisi (notamment s'il y a d�j� des modules)
	//On en d�duit alors les points D et E...
	static GEOMETRY_point_t FX;		//R�sultat d'une position atteinte (mesure si �chec, th�orie si r�ussite)
	static GEOMETRY_point_t D;
	static GEOMETRY_point_t E;

	static GEOMETRY_vector_t F0D;	//vecteur F0D (D=point pour s'extraire doucement de l'axe de push)
	static GEOMETRY_vector_t F0E;	//vecteur F0E (E=point pour s'extraire carr�ment de l'axe de push)

	static GEOMETRY_vector_t Fn_to_next;	//D�placement entre Fn et Fn+1 !
	static GEOMETRY_vector_t vector_cm;	//vecteur unitaire qui indique la direction de la zone !

	static GEOMETRY_point_t Push_out;
	static GEOMETRY_point_t Push_in;

	//Le Getout est le point B !

	//Trajet nominal :

	// GetIn -> G(non atteint) -> A -> B -> C (calcul F0) -> D -> E -> F0 -> F1 -> ... -> F3 -> B



	switch(state){
		case INIT:

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
				state = ERROR;	//C'est pas cette fonction qui g�re la d�pose lat�rale !
			else
				state = COMPUTE_POINTS;
			break;
		case COMPUTE_POINTS:
			//On calcule les points qui seront utilis�s dans les trajectoires.
			if(dzone == DZONE0_BLUE_OUTDOOR)
			{
				GetIn = (GEOMETRY_point_t){1725,860};
				if(i_am_in_square(1000, 1800, 200, 650))
					state = GET_IN;
				else
					state = PATHFIND;
			}
			else if(dzone == DZONE5_YELLOW_OUTDOOR)
			{
				GetIn = (GEOMETRY_point_t){1725,2140};
				if(i_am_in_square(1000, 1800, 2350, 2800))
					state = GET_IN;
				else
					state = PATHFIND;
			}
			else if(dzone == DZONE1_BLUE_INDOOR || dzone == DZONE2_MIDDLE_BLUE)
			{
				GetIn = (GEOMETRY_point_t){1350,1240};
				if(i_am_in_square(0, 1300, 1000, 1300))
					state = GET_IN;
				else
					state = PATHFIND;
			}
			else	//dzone 3 ou 4
			{
				GetIn = (GEOMETRY_point_t){1350,1760};
				if(i_am_in_square(0, 1300, 1700, 2000))
					state = GET_IN;
				else
					state = PATHFIND;
			}

			//Goal (but du rush... point non atteignable) 	-> D�fini en dur
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


			//Seront calculables � partir du rush :
			//O : Origine du robot � la fin du rush				-> Mesur� lorsqu'on s'y trouve
			//OR;	//vecteur OR (R=centre de la zone de d�pose)
			//	A = O + OR + RA;
			//	H = O + OR + RH;
			//	B = H + HB;
			//	C = H + HC;

			//Seront calculables � partir du push :
			//Le point F0 est calcul� selon la r�ussite � atteindre le point C... et le c�t� choisi (notamment s'il y a d�j� des modules)
			//On en d�duit alors les points D et E...
			//F0;		//R�sultat d'une position atteinte (mesure si �chec, th�orie si r�ussite)
			//  D = F0 + F0D;
			//  E = F0 + F0E;
			//  F1 = F0 + Fn_to_next;	//Si existe
			//  F2 = F1 + Fn_to_next;	//Si existe
			//  F3 = F2 + Fn_to_next;	//Si existe

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
				FX = (GEOMETRY_point_t){C.x, C.y};	//on fait l'hypoth�se que C est atteignable, c'est donc notre F0
					//ce point sera susceptible de bouger si on montre que C n'est pas atteignable !
				error_printf("Points mesur�s et calcul�s :\n");
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
			//On recule apr�s un rush qui a �chou�... dans l'optique de quitter la sub !
			state = try_going(A.x, A.y, state, GET_OUT_TO_B, RUSH_AND_COMPUTE_O_H_A_B_C, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		/*case GOTO_B:
			//inverser le sens si un servo de poussage est ajout� c�t� d�pose.
			state = try_going(B.x, B.y, state, PUSH_TO_C, COMPUTE_F_E_D, FAST, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PUSH_TO_C:
			if (entrance)
			{
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				pusher_is_up = TRUE;
			}
			//La flemme d'attendre que le pusher soit descendu pour avancer...
			//Si ca pose probl�me, on pourra toujours ajouter un �tat interm�diaire avant ce PUSH_TO_C

			//inverser le sens si un servo de poussage est ajout� c�t� d�pose.
			//il peut �tre nominal d'�chouer lors de cette trajectoire (si on bloque contre des modules...)
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

			error_printf("FX (au d�but) :%4d ; %4d\n", FX.x, FX.y);
			error_printf("D :%4d ; %4d\n", D.x, D.y);
			error_printf("E :%4d ; %4d\n", E.x, E.y);
			state = GOTO_E;
			break;

		case GOTO_E:
			if(((A.x < E.x) && (color_side == YELLOW)) || ((A.x > E.x) && (color_side = BLUE)))
				state = try_going(E.x, E.y, state, GOTO_D_FX, EXTRACT_MANAGER, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			else
				state = try_going(E.x, E.y, state, GOTO_D_FX, EXTRACT_MANAGER, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GOTO_D_FX:
			//Le sens est important... il faut maintenant qu'on se place pour la pose !
			//state = try_going(FX.x, FX.y, state, COMPUTE_TURN_MODULE, GOTO_D_AND_E, SLOW, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going_multipoint(	(displacement_t []){
											(displacement_t){ (GEOMETRY_point_t) {D.x, D.y}, FAST},
											(displacement_t){ (GEOMETRY_point_t) {FX.x, FX.y}, FAST}
											}, 2, state, COMPUTE_TURN_MODULE, EXTRACT_MANAGER, (color_side==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;

		case COMPUTE_TURN_MODULE:
			if(global.pos.x < 1150)
				state = GET_OUT_TO_B;			//fini !
			else
				state = TURN_MODULE;	//Encore du taff !
			break;

		case TURN_MODULE:
			state = check_sub_action_result(sub_act_anne_return_module(),state,GOTO_NEXT_F,GOTO_NEXT_F);

			//On consid�re qu'on a pos� des modules unicolores de notre couleur... il n'est pas question de venir les retourner plus tard...
			//c'est sans doute perfectible en prenant en compte ce qu'on a r�ellement d�pos�.
			if(ON_LEAVE())
			{
				MOONBASES_addModule((global.color==BLUE)?MODULE_BLUE:MODULE_YELLOW, moonbase);
				state = GOTO_NEXT_F;	//pas besoin de pousser, je maintiens le prochain �tat.
			}
			break;

		case GOTO_NEXT_F:
			if(entrance)
				FX = (GEOMETRY_point_t){FX.x + Fn_to_next.x, FX.y + Fn_to_next.y};	//Next point...
			state = try_going(FX.x, FX.y, state, COMPUTE_TURN_MODULE, EXTRACT_MANAGER, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			//Le end at break permettra sans doute de d�marrer l'actionneur avant la fin du mouvement, c'est l'�clate !
			break;

		case GET_OUT_TO_B:
			state = try_going(B.x, B.y, state, DONE, EXTRACT_MANAGER, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		//Strat�gie d'extraction en cas de merdier... demerden Sie sich!
		case EXTRACT_MANAGER:
			state = check_sub_action_result(sub_ExtractMoonbase(dzone), state, ERROR, ERROR);
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
				error_printf("default case in sub_anne_turn_modules_centre\n");
			}
			break;
	}
	return IN_PROGRESS;
}



static error_e sub_ExtractMoonbase(turn_zone_t dzone)
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
	static bool_e way_getting_oudoor;	//Vrai si on est dans le sens o� on va vers la sortie.. Faut si on revient vers l'int�rieur avant de retenter de sortir.

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
				//Calcul du projet� du robot au centre de la ligne de d�pose.
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
					state = EXTRACT_TO_COMPUTED_POINT;	//Hypoth�se
				else
					state = BACK_TO_COMPUTED_INDOOR;
				nb_round_return++;
				if(distance_to_moonbase < 110 && nb_round_return < 4)
					vector_extract_cm.y *= 1;			//On s'extrait doucement
				else if(distance_to_moonbase < 120 && nb_round_return < 8)
					vector_extract_cm.y *= 2;			//On s'extrait moyenement
				else if(distance_to_moonbase < 140)
					vector_extract_cm.y *= 4;			//On s'extrait fortement
				else									//Turning point ! :D On vise des points LEFT / RIGHT fix�s en dur.
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
				if(state == COMPUTE_POINTS)	//En cas de succ�s, on consid�re qu'on est sorti si on atteint un x assez faible !
				{
					if(global.pos.x < 1100)
						state = DONE;		//Fini !
				}
				if(state == BACK_TO_COMPUTED_INDOOR)	//En cas d'�chec, on force le passage par COMPUTE_POINTS.
				{
					way_getting_oudoor = FALSE;
					state = COMPUTE_POINTS;
				}
				break;
			case BACK_TO_COMPUTED_INDOOR:
				if(entrance)
				{
					//On s'autorise de reculer de 3cm...
					//Ce point est os�, et un peu arbitraire... wait & see!
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
			case EXTRACT_INDOOR:	//On retourne vers l'int�rieur... pour provoquer des aller-retours !
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



/*
 *
	get in
	calage contre la zone
	calcul des points
	get_out_to_turning_axe
	courbe pour approche de la zone (scan faisable en m�me temps)
	d�pose 1
	d�pose 2
	d�pose 3
	poussage
	d�pose 4 ?

	Si on vient de la prise fus�e adverse et que le pr�c�dent calage contre cette zone est r�cent
	-> on peut embrayer direct sur le calcul des points et l'entr�e en zone...

	Ce cas peut-�tre g�r� en faisant la d�pose directement dans la sub de prise fus�e adverse ?
		en appelant la sub de d�pose side � la premi�re position de d�pose... !!!
 */
#define OFFSET_M_R		25	//distance dans le sens avant-arri�re entre le module stock� dans le robot et le centre du robot.

error_e sub_anne_turn_modules_side(ELEMENTS_side_match_e side)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_TURN_MODULES_SIDE,
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
			WAIT_POUSSIX,
			GOTO_P1,
			WAIT_POUSSIX_UP,
			GOTO_F3,
			TURN,
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
				E0 = (GEOMETRY_point_t){875-OFFSET_M_R,220};
				F0 = (GEOMETRY_point_t){750-OFFSET_M_R,208};
				F1 = (GEOMETRY_point_t){850-OFFSET_M_R,208};
				F2 = (GEOMETRY_point_t){950-OFFSET_M_R,208};
				F3 = (GEOMETRY_point_t){1050-OFFSET_M_R,208};
				P0 = (GEOMETRY_point_t){1100-OFFSET_M_R,208};
				P1 = (GEOMETRY_point_t){950-OFFSET_M_R,208};

				//TODO ajouter les cas sp�cifiques o� l'on est d�j� sur place (apr�s prise fus�e adverse notamment !)
				if(i_am_in_square(750,1350,200,1400))
					state = GET_IN;
				else if(i_am_in_square(300,500,1500,2000))	//Apr�s la prise de notre fus�e jaune !
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
				else if(i_am_in_square(300,500,1000,1500))	//Apr�s la prise de notre fus�e jaune !
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
			state = try_going_multipoint(	(displacement_t []) {
											(displacement_t) { (GEOMETRY_point_t){700, 1850}, FAST},
											(displacement_t) { (GEOMETRY_point_t){800, 2350}, FAST}
										},
										2, state, GET_IN, ASTAR_GET_IN, FORWARD, DODGE_AND_WAIT, END_AT_BRAKE);
			if(state == ASTAR_GET_IN)
			{
				if(i_am_in_square(200,800,900,1700))
					state = GET_IN_VIA_SOUTH_FROM_BLUE_ROCKET;
			}
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
				//On a �chou�...
				if(i_am_in_square(200,800,1300,2100))	//Premi�re partie de la courbe
					state = GET_IN_VIA_SOUTH_FROM_YELLOW_ROCKET;
			}
			break;
		case GET_IN_VIA_SOUTH_FROM_YELLOW_ROCKET:
			state = try_going_multipoint(	(displacement_t []) {
											(displacement_t) { (GEOMETRY_point_t){1200, 1200}, FAST},
											(displacement_t) { (GEOMETRY_point_t){800, 650}, FAST}
										},
										2, state, GET_IN, ASTAR_GET_IN, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;
		case ASTAR_GET_IN:
			state = ASTAR_try_going(A.x, A.y, state, RUSH_TO_GOAL, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case RUSH_TO_GOAL:
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

			//TODO enrichir avec un scan pour permettre le remplissage d'une zone non vide... en rejoignant apr�s un poussage la bonne case directement.
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
			else if(color_side==BLUE && global.pos.y>80+28+130)	//Si on a r�ussit � sortir suffisamment et que ca fait d�j� deux fois, on se casse !
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
			//BACKWARD pour se mettre du bon c�t� !!!
			state = try_going_multipoint(	(displacement_t []) {
												(displacement_t) { (GEOMETRY_point_t){E0.x, E0.y}, FAST},
												(displacement_t) { (GEOMETRY_point_t){F0.x, F0.y}, FAST}
											},
											2, state, TURN, EXTRACT_BEYOND_F3, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TURN:{
			static enum state_e success_state;
			static arg_dipose_mae_e last_module;
			if(entrance)
			{
				if(last_state == GOTO_F0_VIA_E0)
					success_state = GOTO_F1;
				else if(last_state == GOTO_F1)
					success_state = GOTO_F2;
				else if(last_state == GOTO_F2)
					success_state = GOTO_F3;	//Il faut pousser !
				else
					success_state = EXTRACT_BEYOND_F3;

			}
			state = check_sub_action_result(sub_act_anne_return_module(),state,success_state,EXTRACT_BEYOND_F3);
			break;}

		case GOTO_F1:
			state = try_going(F1.x, F1.y, state, TURN, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GOTO_F2:
			state = try_going(F2.x, F2.y, state, TURN, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GOTO_F3:
			state = try_going(F3.x, F3.y, state, TURN, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case EXTRACT_BEYOND_F0:

			if(entrance)
			{
				//On veut sortir vers le c�t� o� se trouve F0
				//On calcule une courbe de sortie...
				if(color_side == BLUE)
				{
					output_curve[0].point.x = global.pos.x - 100;
					output_curve[0].point.y = MIN(global.pos.y + 15, A.y);	//L'�cretage est utile si on vient d'un �chec d'extraction pr�c�dent
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
				//TODO borner pour ne pas g�n�rer de point innaccessible (crat�re / obstacle.... !)
			}
			state = try_going_multipoint(	output_curve, 2, state, DONE, EXTRACT_BEYOND_F3, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;

		case EXTRACT_BEYOND_F3:
			if(entrance)
			{
				//On veut sortir vers le c�t� o� se trouve F0
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

				//TODO borner pour ne pas g�n�rer de point innaccessible (crat�re / obstacle.... !)

			}
			state = try_going_multipoint(	output_curve, 2, state, DONE, EXTRACT_BEYOND_F0, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case ERROR:
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		case DONE:
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