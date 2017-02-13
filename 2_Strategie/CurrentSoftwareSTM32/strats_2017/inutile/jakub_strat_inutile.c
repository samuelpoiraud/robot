/*
 * jakub_strat_inutile.c
 *
 *  Created on: Jan 5, 2017
 *      Author: Jakub
 */


//MANQUE LES ACTIONNEURS


#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../strats_2017/big/action_big.h"
#include "../../strats_2017/small/action_small.h"
#include "../../strats_2017/actions_both_2017.h"

void jakub_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			START,
			PHASE1,
			PHASE2,
			RETURN1,
			RETURN2,
			DONE,
			ERROR
			);

			switch(state){

			case INIT:
				state = try_going(900,COLOR_Y(2350),INIT,START,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case START:
				state = check_sub_action_result (sub_harry_take_north_little_crater(),state,RETURN1,RETURN1);
				break;

			case RETURN1:
				state = ASTAR_try_going(200,COLOR_Y(950),RETURN1,PHASE1,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case PHASE1:
				state = check_sub_action_result (sub_harry_take_south_little_crater(),state,RETURN2,RETURN2);
				break;

			case RETURN2:
				state = ASTAR_try_going(200,COLOR_Y(950),RETURN2,DONE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case PHASE2:
				break;

			case DONE:
				break;

			case ERROR:
				break;
			}

}
	/*CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			DEFAUT,
			HOME,
			CARRE_HAUT_GAUCHE,
			CARRE_HAUT_DROIT,
			CARRE_BAS_DROIT,
			CARRE_BAS_GAUCHE,
			MISE_EN_PLACE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if (i_am_in_square_color(0,0,1000,1500))
				state = CARRE_BAS_GAUCHE;
			/*{
				if (){
					state = CARRE_BAS_GAUCHE;
				}
				else if (){
					state =
				}
				else if (){
					state =
				}
				else if (){
					state =
				}
				else{
					state = DONE;
				}
			}
			*/
			/*
			 *
			else if (i_am_in_square_color(0,1500,1000,3000))
			{
				state = CARRE_HAUT_GAUCHE;
			}
			else if (i_am_in_square_color(1000,0,1000,1500))
			{
				state = CARRE_BAS_DROIT;
			}
			else if (i_am_in_square_color(1000,1500,2000,3000))
			{
				state = CARRE_HAUT_DROIT;
			}
			else
			{
				state = DEFAUT;
			}
			break;

		case CARRE_HAUT_GAUCHE:
			state = try_going(900,COLOR_Y(2350),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(650,COLOR_Y(2350),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//actionneur avant?
			state = try_going(900,COLOR_Y(2350),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(200,COLOR_Y(950),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = DONE;
			break;

		case CARRE_HAUT_DROIT:
			state = try_going(1500,COLOR_Y(2300),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(1850,COLOR_Y(2000),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//actionneur arriere
			state = try_going(1500,COLOR_Y(2300),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(200,COLOR_Y(950),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = DONE;
			break;

		case CARRE_BAS_DROIT:
			state = try_going(1300,COLOR_Y(700),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(1700,COLOR_Y(750),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(1750,COLOR_Y(920),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//actionneur arriere
			state = try_going(1700,COLOR_Y(750),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(200,COLOR_Y(950),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = DONE;
			break;

		case CARRE_BAS_GAUCHE:
			state = try_going(800,COLOR_Y(1000),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(650,COLOR_Y(750),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//actionneur avant
			state = try_going(800,COLOR_Y(900),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(200,COLOR_Y(950),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = DONE;
			break;

		case MISE_EN_PLACE:
			break;

		case DEFAUT:
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
*/
