#include "action_small.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"
#include "../high_level_strat.h"
#include "../actions_both_2017.h"


bool_e dispose_manager_chose_moonbase(moduleMoonbaseLocation_e * moonbase, ELEMENTS_side_match_e * moonbase_side);


error_e sub_anne_initiale(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INITIALE,
			INIT,
			CROSS_ROCKER,
			EXTRACT_FROM_BORDER_IF_NEEDED,
			COMPUTE_WHAT_DOING,
			TAKE_OUR_ROCKET,
			TAKE_ADV_ROCKET,
			TURN_ADV_MODULES,
			DISPOSE,
			ERROR,
			DONE
		);
	static Uint8 nb_try_our_rocket = 0;
	static Uint8 nb_try_adv_rocket = 0;
	static Uint8 agressivity = 0;


	static Uint8 nb_try_return_adv_middle = 0;
	static Uint8 nb_try_return_middle = 0;
	static Uint8 nb_try_return_adv_side = 0;
	static bool_e must_protect_after_dispose = FALSE;
	error_e ret = IN_PROGRESS;

	UNUSED_VAR(nb_try_return_adv_middle);
	UNUSED_VAR(nb_try_return_middle);
	UNUSED_VAR(nb_try_return_adv_side);
	UNUSED_VAR(must_protect_after_dispose);

	switch(state){
		case INIT:
			state = CROSS_ROCKER;
			//Les nombres ci dessous permettent de régler un "nombre d'essais"... au delà, on rendra la main à la highlevel

			//Prises
			nb_try_our_rocket = IHM_switchs_get(SWITCH_ANNE_TAKE_OUR_ROCKET)?2:0;
			nb_try_adv_rocket = IHM_switchs_get(SWITCH_ANNE_TAKE_ADV_ROCKET)?2:0;
			agressivity = IHM_switchs_get(SWITCH_ANNE_AGRESSIVITY)?1:0;


			//Retournements
			nb_try_return_adv_middle = IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_MIDDLE)?1:0;
			nb_try_return_middle = IHM_switchs_get(SWITCH_ANNE_RETURN_MIDDLE)?1:0;
			nb_try_return_adv_side = IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_SIDE)?1:0;

			//si aucun retournement d'activé et une seule fusée d'activée... il ne nous restera plus qu'à protéger...
			must_protect_after_dispose = IHM_switchs_get(SWITCH_ANNE_PROTECT)?TRUE:FALSE;

			break;
		case CROSS_ROCKER:
			if(sub_cross_rocker() != IN_PROGRESS)
				state = EXTRACT_FROM_BORDER_IF_NEEDED;
			break;
		case EXTRACT_FROM_BORDER_IF_NEEDED:
			if(entrance)
			{
				if(global.pos.x > 160)
					state = COMPUTE_WHAT_DOING;
			}
			else	//Le else est très important !
				state = try_going(160, global.pos.y, state, COMPUTE_WHAT_DOING, COMPUTE_WHAT_DOING, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;
		case COMPUTE_WHAT_DOING:	//Cet état est susceptible d'être appelé lors de l'échec d'une sub... pour reréfléchir à quoi faire !
			if(nb_try_our_rocket && !(agressivity && nb_try_adv_rocket))
			{
				nb_try_our_rocket--;
				state = TAKE_OUR_ROCKET;	//Notre fusée si elle est activée et si on ne cherche pas à être agressif vers la fusée adverse
			}
			else if(nb_try_adv_rocket)
			{
				nb_try_adv_rocket--;
				if(agressivity && nb_try_our_rocket) //on a fait le choix du module adverse par agressivité, et non par unique choix
					agressivity--;
				state = TAKE_ADV_ROCKET;
			}
			else if(IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_MIDDLE) || IHM_switchs_get(SWITCH_ANNE_RETURN_MIDDLE) || IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_SIDE) )
				state = TURN_ADV_MODULES;
			else
				state = DONE;	//Plus rien à faire !

			break;

		case TAKE_OUR_ROCKET:
			switch(sub_anne_fusee_color())
			{
				case IN_PROGRESS:
					break;
				default:	//Pour toutes les erreurs (timeout, not_handled...)
					if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 0)
					{
						nb_try_our_rocket = 0;	//On ne retournera pas prendre cette fusée incomplète !
						state = DISPOSE;
					}
					else
						state = COMPUTE_WHAT_DOING;	//Aucun module pris, on se repose la question de quoi faire !
					break;
			}
			break;

		case TAKE_ADV_ROCKET:
			switch(sub_anne_fusee_multicolor(ADV_ELEMENT))
			{
				case IN_PROGRESS:
					break;
				case END_OK:
					nb_try_adv_rocket = 0;
					state = DISPOSE;
					break;
				default:	//Pour toutes les erreurs (timeout, not_handled...)
					if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 0)
					{
						nb_try_adv_rocket = 0;		//On ne retournera pas prendre cette fusée incomplète !
						state = DISPOSE;
					}
					else
						state = COMPUTE_WHAT_DOING;	//Aucun module pris, on se repose la question de quoi faire !
					//TODO : dans certains cas, il est peut-être préférable de renvoyer la main vers la highlevel...
					//Notamment lorsqu'on a échoué à cause d'un mauvais positionnement devant la fusée adverse.
					//Si l'odométrie est flinguée, il est sans doute plus pertinent d'attaquer que de revenir vers nos modules !
					break;
			}
			break;
		case DISPOSE:
			//Cette subaction choisi une zone et tente d'aller y déposer les modules.
			switch(sub_anne_chose_moonbase_and_dispose_modules())
			{
				case END_OK:
					state = COMPUTE_WHAT_DOING;
					break;
				case IN_PROGRESS:
					break;
				default:
					if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0)		//On a aucun module en stock, on se repose la question de quoi faire
						state = COMPUTE_WHAT_DOING;
					else	//Il nous reste des modules en stock, on est coincé pour une autre prise, on se barre de l'initiale en priant pour que la highlevel fasse son job !
						state = ERROR;	//Echec de la dépose. On rend la main à la highlevel...
					break;
			}
			break;

		case TURN_ADV_MODULES:
			if(sub_act_anne_return_module() != IN_PROGRESS)
				state = DONE;
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
				debug_printf("default case in sub_anne_initiale\n");
			break;
	}

	if(ret != IN_PROGRESS)
	{
		//il faut une fin à tout...
		//on va sortir de l'initiale, et peut être le regretter amèrement...
		//C'est donc l'heure de configurer les subactions pour la highlevel.

		//L'initiale à le monopole des prises de modules dans les fusées... la highlevel n'a pas accès à ces prises

		//Sub de retournement

		//Sub de dépose
		if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 0)	//Il reste des modules à poser.
			set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);	//C'est le manager de dépose qui choisiera la zone... on se contente d'activer la sub générique de dépose.
	}


	return ret;
}


//Cette fonction est utilisée par la sub initiale d'Anne.
//Elle a pout but de choisir une zone de dépose selon :
//		- les zones déjà tentées
//		- notre position
//		- positions adverses
//		- tout autre idée géniale...
//Param nb_try_dispose : tableau de nombre d'essai restant pour chaque zone
//Param moonbase : si on return TRUE, vous trouverez ici le choix de zone !
//TODO : ajouter un argument pointeur qui renverra le meilleur accès (côté blue ou yellow)
bool_e dispose_manager_chose_moonbase(moduleMoonbaseLocation_e * moonbase, ELEMENTS_side_match_e * moonbase_side)
{
	static bool_e moonbases_enable[NB_MOONBASES];	//nombres de places restantes si zones activée (IHM). 0 si zone désactivée.
	static bool_e nb_tentatives_moonbases[NB_MOONBASES];		//nombre de tentatives où on a essayé d'aller à...
	static bool_e initialized = FALSE;
	Sint8 modules_in_stock;
	Sint8 moonbases_score[NB_MOONBASES];	//Celui qui obtient le meilleur score va gagner ! A vos marques... prêt.... partez !
	const Uint8 natural_priority[NB_MOONBASES] = {
			5, //MODULE_MOONBASE_MIDDLE,			//La meilleure zone par défaut
			3, //MODULE_MOONBASE_OUR_CENTER,		//Si elle est activé (ce qui n'est pas nominal, cette zone dispose d'une forte priorité)
			1, //MODULE_MOONBASE_ADV_CENTER,		//Zone non prioritaire...
			4, //MODULE_MOONBASE_OUR_SIDE,			//Si elle est activé (ce qui n'est pas nominal, cette zone dispose d'une forte priorité)
			2 //MODULE_MOONBASE_ADV_SIDE,
	};
	bool_e ret = FALSE;

	if(!initialized)
	{
		initialized = TRUE;
		//Lecture des switchs
		moonbases_enable[MODULE_MOONBASE_MIDDLE] = IHM_switchs_get(SWITCH_ANNE_DISPOSE_MIDDLE);
		moonbases_enable[MODULE_MOONBASE_ADV_CENTER] = IHM_switchs_get(SWITCH_ANNE_DISPOSE_ADV_MIDDLE);
		moonbases_enable[MODULE_MOONBASE_ADV_SIDE] = IHM_switchs_get(SWITCH_ANNE_DISPOSE_ADV_SIDE);
		moonbases_enable[MODULE_MOONBASE_OUR_CENTER] = IHM_switchs_get(SWITCH_ANNE_DISPOSE_OUR_MIDDLE);
		moonbases_enable[MODULE_MOONBASE_OUR_SIDE] = IHM_switchs_get(SWITCH_ANNE_DISPOSE_OUR_SIDE);
	}

	modules_in_stock = STOCKS_getNbModules(MODULE_STOCK_SMALL);
	if(modules_in_stock == 0)
		return FALSE;	//Rien à déposer !!!???
	moduleMoonbaseLocation_e mb;
	for(mb = 0; mb<NB_MOONBASES; mb++)
	{
		if(moonbases_enable[mb])
		{
			Sint8 missing_places = modules_in_stock -  MOONBASES_getNbPlace(mb);

			if(missing_places <= 0)	//Assez de place pour poser !
				moonbases_score[mb] = 4;	//4 points !
			else	//Pas assez de place pour déposer ici !
				moonbases_score[mb] = (missing_places<=4)?(4-missing_places):0;	//C'est moins cool, mais on peut au moins mettre un module !

		}
		else
			moonbases_score[mb] = 0;	//Zone non activée, score nul ! On ira pas poser ici !

		//Arrivé ici, on a :
				//4 points pour une zone qui peut acceuillir nos modules
				//3 points pour une zone qui peut tous les acceuillir sauf 1
				//2 points pour une zone qui peut tous les acceuillir sauf 2
				//1 point  pour une zone qui peut tous les acceuillir sauf 3
				//0 point  pour une zone qui peut plus acceuillir grand chose !
				//0 point  pour une zone qui n'est pas autorisée par les switchs de l'IHM !!! (--> l'humain d'abord... ça fait un bon slogan politique ça, n'est ce pas ?)

		//Calcul de la distance à la zone

		Sint8 bonus = 0;
		if(moonbases_score[mb] > 0)
		{
			//Pour toute zone qui a un score non nul, on donne un bonus à la proximité.
			switch(mb)
			{
				case MODULE_MOONBASE_MIDDLE:
					//no break
				case MODULE_MOONBASE_OUR_CENTER:
					//no break
				case MODULE_MOONBASE_OUR_SIDE:
					if(i_am_in_square_color(0, 1000, 700, 1930))
						bonus += 1;
					break;
				case MODULE_MOONBASE_ADV_CENTER:
					if(i_am_in_square_color(0, 2000, 1500, 3000))	//On est dans le demi-terrain adverse
						bonus += 1;
					break;
				case MODULE_MOONBASE_ADV_SIDE:
					if(i_am_in_square_color(0, 2000, 2500, 3000))	//On est dans le quart-terrain adverse
						bonus += 2;		//2 points !
					else if(i_am_in_square_color(0, 2000, 1500, 3000))	//On est dans le demi-terrain adverse (mais du coup pas dans le quart...)
						bonus += 1;
					break;
				default:				break;
			}
			if(bonus)
			{
				moonbases_score[mb] += bonus;
				debug_printf("Bonus for zone %d : %d\n", mb, bonus);
			}

			//Pour toute zone qui a un score non nul, on donne un malus si elle a subit des échecs préalablement
			moonbases_score[mb] -= nb_tentatives_moonbases[mb];

			Sint8 malus = 0;

			//Pour toute zone qui a un score non nul, on donne un malus si on y voit des aversaires à l'hokuyo !
			switch(mb)
			{
				case MODULE_MOONBASE_MIDDLE:
					if(foe_in_square_color(FALSE, 1000, 2000, 1150, 1850 ,FOE_TYPE_HOKUYO))
						malus = 2;
					break;
				case MODULE_MOONBASE_OUR_CENTER:
					if(foe_in_square_color(FALSE, 1000, 2000, 500, 1500 ,FOE_TYPE_HOKUYO))
						malus = 2;
					break;
				case MODULE_MOONBASE_OUR_SIDE:
					if(foe_in_square_color(FALSE, 400, 1500, 0, 700 ,FOE_TYPE_HOKUYO))
						malus = 2;
					break;
				case MODULE_MOONBASE_ADV_CENTER:
					if(foe_in_square_color(FALSE, 1000, 2000, 1500, 2500 ,FOE_TYPE_HOKUYO))
						malus = 2;
					break;
				case MODULE_MOONBASE_ADV_SIDE:
					if(foe_in_square_color(FALSE, 400, 1500, 2500, 3000 ,FOE_TYPE_HOKUYO))
						malus = 2;
					break;
				default:
					break;
			}
			if(malus)
			{
				moonbases_score[mb] -= malus;
				//Tout score malussé ci dessus qui est tombé à 0 est gracieusement remonté à 1... Ma générosité me perdra...
				if(moonbases_score[mb] <= 0)
					moonbases_score[mb] = 1;
				debug_printf("Malus for zone %d : -%d\n",mb, malus);
			}
		}

		if(moonbases_score[mb] < 0)	//Tout score négatif est ramené à 0... faut pas déconner non plus !
			moonbases_score[mb] = 0;
	}

	//Recherche du score maximum
	Sint8 best_score = 0;
	for(mb = 0; mb<NB_MOONBASES; mb++)
	{
		if(moonbases_score[mb] > best_score)
		{
			best_score = moonbases_score[mb];
			*moonbase = mb;
		}
		else if(moonbases_score[mb] == best_score)	//Deux scores sont identiques
		{
			if(natural_priority[mb] > natural_priority[*moonbase])	//On s'en réfère aux priorités naturelles du tableau constant.
				*moonbase = mb;
		}
		debug_printf("Zone %d - %s : score=%d (%sable;%d places;%d tentatives)\n",mb, MOONBASES_toString(mb), moonbases_score[mb], moonbases_enable[mb]?"en":"dis", MOONBASES_getNbPlace(mb), nb_tentatives_moonbases[mb]);
	}

	if(best_score > 0)	//On a un best_score non nul !
	{
		debug_printf("Zone retenue : %d - %s\n",*moonbase, MOONBASES_toString(*moonbase));
		ret = TRUE;	//On a trouvé une zone !!! champomy !


		//TODO choix du côté...
		/*A- Si la zone est notre zone du milieu :
		 *  côté privilégié = au milieu (mais si déjà un échec à cet endroit, on acceptera de faire le tour...)
		 *  		Ce switch nous indiquant qu'Harry n'est pas là pour nous emmerder
		 *B- Si la zone est la zone du milieu :
		 *    côté privilégié = côté ADV
		 * 	  On s'autorisera en cas d'échec préalable à viser notre côté (seulement si on a reçu l'autorisation de Harry d'occuper ce GetIn)
		 *C- Si zone adverse du milieu
		 *    on préfère l'accès intérieur... mais en cas d'échec, on peut se rabattre sur l'accès extérieur.
		 */
		switch(*moonbase)
		{
			case MODULE_MOONBASE_OUR_CENTER:
				if(nb_tentatives_moonbases[*moonbase] % 2)	//Si on a aucune tentative ou un nombre pair de tentatives
					*moonbase_side = ADV_SIDE;	//Intérieur
				else
					*moonbase_side = OUR_SIDE;	//On assume qu'Harry n'a pas utilisé la zone... donc on s'y rend en mode "rien à foutre"
				break;
			case MODULE_MOONBASE_MIDDLE:
				if(nb_tentatives_moonbases[*moonbase] % 2)
					*moonbase_side = ADV_SIDE;
				else
				{
					*moonbase_side = OUR_SIDE;
				}
				break;
			case MODULE_MOONBASE_ADV_CENTER:
				if(nb_tentatives_moonbases[*moonbase] % 2)
					*moonbase_side = OUR_SIDE;	//On préfère rester sur l'intérieur... moins gênés par le cratère
				else	//en cas de nombre d'échecs impairs, on se rabat sur l'extérieur...
					*moonbase_side = ADV_SIDE;
				break;
			default:
				//L'argument moonbase_side n'a pas de sens pour les bases latérales...
				break;
		}

		nb_tentatives_moonbases[*moonbase]++;	//On incrémente le nb de tentative de la moonbase choisie
	}

	return ret;
}

error_e sub_anne_chose_moonbase_and_dispose_modules(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_CHOSE_MOONBASE_AND_DISPOSE,
				INIT,
				COMPUTE_DISPOSE_ZONE,
				DISPOSE_ON_CENTRAL_MOONBASE,
				DISPOSE_ON_LATERAL_MOONBASE,
				COMPUTE_WHAT_DOING,
				ERROR,
				DONE
			);
	static moduleMoonbaseLocation_e moonbase;
	static ELEMENTS_side_match_e moonbase_side;
	static Uint8 nb_fail = 0;
	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			nb_fail = 0;
			state = COMPUTE_DISPOSE_ZONE;
			break;

		case COMPUTE_DISPOSE_ZONE:
			if(dispose_manager_chose_moonbase(&moonbase, &moonbase_side) == TRUE)	//Une zone est choisie (avec un côté d'accès s'il s'agit d'une zone centrale)
			{
				if(moonbase == MODULE_MOONBASE_OUR_SIDE || moonbase == MODULE_MOONBASE_ADV_SIDE)
					state = DISPOSE_ON_LATERAL_MOONBASE;
				else
					state = DISPOSE_ON_CENTRAL_MOONBASE;
			}
			else
				state = ERROR;	//Aucune zone choisie par le dispose manager.

			break;
		case DISPOSE_ON_CENTRAL_MOONBASE:
			if(sub_anne_depose_modules_centre(moonbase, moonbase_side) != IN_PROGRESS)
				state = COMPUTE_WHAT_DOING;
			break;

		case DISPOSE_ON_LATERAL_MOONBASE:
			if(sub_anne_dispose_modules_side((moonbase==MODULE_MOONBASE_OUR_SIDE)?OUR_SIDE:ADV_SIDE) != IN_PROGRESS)
				state = COMPUTE_WHAT_DOING;
			break;

		case COMPUTE_WHAT_DOING:
			//Si on a réussi la dépose, alors c'est la fin.
			//Si on a échoué, on se doit de retenter une autre dépose.
			if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0)
				state = DONE; //génial !
			else
			{
				nb_fail++;
				if(nb_fail < 2)	//On autorise une seconde tentative (au premier passage ici : nb_fail == 1)
					state = COMPUTE_DISPOSE_ZONE;
				else
					state = ERROR;	//Bon bah, on a tenté 2 fois, et on a perdu... demerden sie sich
			}
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

	}
	return ret;
}

//TODO !
error_e sub_anne_end_of_match(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_END_OF_MATCH,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
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
				debug_printf("default case in sub_anne_end_of_match\n");
			break;
	}

	return IN_PROGRESS;
}
