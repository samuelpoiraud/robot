#include "action_small.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"
#include "../high_level_strat.h"


bool_e dispose_manager_chose_moonbase(moduleMoonbaseLocation_e * moonbase);


error_e sub_anne_initiale(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INITIALE,
			INIT,
			CROSS_ROCKER,
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
	static moduleMoonbaseLocation_e moonbase;
	error_e ret = IN_PROGRESS;

	switch(state){
		case INIT:
			state = CROSS_ROCKER;
			//Les nombres ci dessous permettent de r�gler un "nombre d'essais"... au del�, on rendra la main � la highlevel

			//Prises
			nb_try_our_rocket = IHM_switchs_get(SWITCH_ANNE_TAKE_OUR_ROCKET)?2:0;
			nb_try_adv_rocket = IHM_switchs_get(SWITCH_ANNE_TAKE_ADV_ROCKET)?2:0;
			agressivity = IHM_switchs_get(SWITCH_ANNE_AGRESSIVITY)?1:0;


			//Retournements
			nb_try_return_adv_middle = IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_MIDDLE)?1:0;
			nb_try_return_middle = IHM_switchs_get(SWITCH_ANNE_RETURN_MIDDLE)?1:0;
			nb_try_return_adv_side = IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_SIDE)?1:0;

			//si aucun retournement d'activ� et une seule fus�e d'activ�e... il ne nous restera plus qu'� prot�ger...
			must_protect_after_dispose = IHM_switchs_get(SWITCH_ANNE_PROTECT)?TRUE:FALSE;

			break;
		case CROSS_ROCKER:
			if(sub_cross_rocker() != IN_PROGRESS)
				state = COMPUTE_WHAT_DOING;
			break;
		case COMPUTE_WHAT_DOING:	//Cet �tat est susceptible d'�tre appel� lors de l'�chec d'une sub... pour rer�fl�chir � quoi faire !
			if(nb_try_our_rocket && !(agressivity && nb_try_adv_rocket))
			{
				nb_try_our_rocket--;
				state = TAKE_OUR_ROCKET;	//Notre fus�e si elle est activ�e et si on ne cherche pas � �tre agressif vers la fus�e adverse
			}
			else if(nb_try_adv_rocket)
			{
				nb_try_adv_rocket--;
				if(agressivity && nb_try_our_rocket) //on a fait le choix du module adverse par agressivit�, et non par unique choix
					agressivity--;
				state = TAKE_ADV_ROCKET;
			}
			else if(IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_MIDDLE) || IHM_switchs_get(SWITCH_ANNE_RETURN_MIDDLE) || IHM_switchs_get(SWITCH_ANNE_RETURN_ADV_SIDE) )
				state = TURN_ADV_MODULES;
			else
				state = DONE;	//Plus rien � faire !

			break;

		case TAKE_OUR_ROCKET:
			switch(sub_anne_fusee_color())
			{
				case IN_PROGRESS:
					break;
				default:	//Pour toutes les erreurs (timeout, not_handled...)
					if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 0)
					{
						nb_try_our_rocket = 0;	//On ne retournera pas prendre cette fus�e incompl�te !
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
						nb_try_adv_rocket = 0;		//On ne retournera pas prendre cette fus�e incompl�te !
						state = DISPOSE;
					}
					else
						state = COMPUTE_WHAT_DOING;	//Aucun module pris, on se repose la question de quoi faire !
					//TODO : dans certains cas, il est peut-�tre pr�f�rable de renvoyer la main vers la highlevel...
					//Notamment lorsqu'on a �chou� � cause d'un mauvais positionnement devant la fus�e adverse.
					//Si l'odom�trie est flingu�e, il est sans doute plus pertinent d'attaquer que de revenir vers nos modules !
					break;
			}
			break;
		case DISPOSE:
			//Cette subaction choisi une zone et tente d'aller y d�poser les modules.
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
					else	//Il nous reste des modules en stock, on est coinc� pour une autre prise, on se barre de l'initiale en priant pour que la highlevel fasse son job !
						state = ERROR;	//Echec de la d�pose. On rend la main � la highlevel...
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
		//il faut une fin � tout...
		//on va sortir de l'initiale, et peut �tre le regretter am�rement...
		//C'est donc l'heure de configurer les subactions pour la highlevel.

		//L'initiale � le monopole des prises de modules dans les fus�es... la highlevel n'a pas acc�s � ces prises

		//Sub de retournement

		//Sub de d�pose
		if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 0)	//Il reste des modules � poser.
			set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);	//C'est le manager de d�pose qui choisiera la zone... on se contente d'activer la sub g�n�rique de d�pose.
	}


	return ret;
}


//Cette fonction est utilis�e par la sub initiale d'Anne.
//Elle a pout but de choisir une zone de d�pose selon :
//		- les zones d�j� tent�es
//		- notre position
//		- positions adverses
//		- tout autre id�e g�niale...
//Param nb_try_dispose : tableau de nombre d'essai restant pour chaque zone
//Param moonbase : si on return TRUE, vous trouverez ici le choix de zone !
//TODO : ajouter un argument pointeur qui renverra le meilleur acc�s (c�t� blue ou yellow)
bool_e dispose_manager_chose_moonbase(moduleMoonbaseLocation_e * moonbase)
{
	static bool_e moonbases_enable[NB_MOONBASES];	//nombres de places restantes si zones activ�e (IHM). 0 si zone d�sactiv�e.
	static bool_e nb_tentatives_moonbases[NB_MOONBASES];		//nombre de tentatives o� on a essay� d'aller �...
	static bool_e initialized = FALSE;
	Sint8 modules_in_stock;
	Sint8 moonbases_score[NB_MOONBASES];	//Celui qui obtient le meilleur score va gagner ! A vos marques... pr�t.... partez !
	const Uint8 natural_priority[NB_MOONBASES] = {
			5, //MODULE_MOONBASE_MIDDLE,			//La meilleure zone par d�faut
			3, //MODULE_MOONBASE_OUR_CENTER,		//Si elle est activ� (ce qui n'est pas nominal, cette zone dispose d'une forte priorit�)
			1, //MODULE_MOONBASE_ADV_CENTER,		//Zone non prioritaire...
			4, //MODULE_MOONBASE_OUR_SIDE,			//Si elle est activ� (ce qui n'est pas nominal, cette zone dispose d'une forte priorit�)
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
		return FALSE;	//Rien � d�poser !!!???
	moduleMoonbaseLocation_e mb;
	for(mb = 0; mb<NB_MOONBASES; mb++)
	{
		if(moonbases_enable[mb])
		{
			Sint8 missing_places = modules_in_stock -  MOONBASES_getNbPlace(mb);

			if(missing_places <= 0)	//Assez de place pour poser !
				moonbases_score[mb] = 4;	//4 points !
			else	//Pas assez de place pour d�poser ici !
				moonbases_score[mb] = (missing_places<=4)?(4-missing_places):0;	//C'est moins cool, mais on peut au moins mettre un module !

		}
		else
			moonbases_score[mb] = 0;	//Zone non activ�e, score nul ! On ira pas poser ici !

		//Arriv� ici, on a :
				//4 points pour une zone qui peut acceuillir nos modules
				//3 points pour une zone qui peut tous les acceuillir sauf 1
				//2 points pour une zone qui peut tous les acceuillir sauf 2
				//1 point  pour une zone qui peut tous les acceuillir sauf 3
				//0 point  pour une zone qui peut plus acceuillir grand chose !
				//0 point  pour une zone qui n'est pas autoris�e par les switchs de l'IHM !!! (--> l'humain d'abord... �a fait un bon slogan politique �a, n'est ce pas ?)

		//Calcul de la distance � la zone

		Sint8 bonus = 0;
		if(moonbases_score[mb] > 0)
		{
			//Pour toute zone qui a un score non nul, on donne un bonus � la proximit�.
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

			//Pour toute zone qui a un score non nul, on donne un malus si elle a subit des �checs pr�alablement
			moonbases_score[mb] -= nb_tentatives_moonbases[mb];

			Sint8 malus = 0;

			//Pour toute zone qui a un score non nul, on donne un malus si on y voit des aversaires � l'hokuyo !
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
				//Tout score maluss� ci dessus qui est tomb� � 0 est gracieusement remont� � 1... Ma g�n�rosit� me perdra...
				if(moonbases_score[mb] <= 0)
					moonbases_score[mb] = 1;
				debug_printf("Malus for zone %d : -%d\n",mb, malus);
			}
		}

		if(moonbases_score[mb] < 0)	//Tout score n�gatif est ramen� � 0... faut pas d�conner non plus !
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
			if(natural_priority[mb] > natural_priority[*moonbase])	//On s'en r�f�re aux priorit�s naturelles du tableau constant.
				*moonbase = mb;
		}
		debug_printf("Zone %d - %s : score=%d (%sable;%d places;%d tentatives)\n",mb, MOONBASES_toString(mb), moonbases_score[mb], moonbases_enable[mb]?"en":"dis", MOONBASES_getNbPlace(mb), nb_tentatives_moonbases[mb]);
	}

	if(best_score > 0)	//On a un best_score non nul !
	{
		debug_printf("Zone retenue : %d - %s\n",*moonbase, MOONBASES_toString(*moonbase));
		ret = TRUE;	//On a trouv� une zone !!! champomy !
		nb_tentatives_moonbases[*moonbase]++;	//On incr�mente le nb de tentative de la moonbase choisie
	}

	return ret;
}

error_e sub_anne_chose_moonbase_and_dispose_modules(void)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_CHOSE_MOONBASE_AND_DISPOSE,
				INIT,
				COMPUTE_DISPOSE_ZONE,
				DISPOSE_MOONBASE_MIDDLE,
				DISPOSE_MOONBASE_OUR_CENTER,
				DISPOSE_MOONBASE_OUR_SIDE,
				DISPOSE_MOONBASE_ADV_SIDE,
				DISPOSE_MOONBASE_ADV_CENTER,
				COMPUTE_WHAT_DOING,
				ERROR,
				DONE
			);
	static moduleMoonbaseLocation_e moonbase;
	static Uint8 nb_fail = 0;
	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			nb_fail = 0;
			state = COMPUTE_DISPOSE_ZONE;
			break;

		case COMPUTE_DISPOSE_ZONE:
			if(dispose_manager_chose_moonbase(&moonbase) == TRUE)	//Une zone est choisie
			{
				switch(moonbase)
				{
					case MODULE_MOONBASE_MIDDLE:		state = DISPOSE_MOONBASE_MIDDLE;		break;
					case MODULE_MOONBASE_OUR_CENTER:	state = DISPOSE_MOONBASE_OUR_CENTER;	break;
					case MODULE_MOONBASE_OUR_SIDE:		state = DISPOSE_MOONBASE_OUR_SIDE;		break;
					case MODULE_MOONBASE_ADV_SIDE:		state = DISPOSE_MOONBASE_ADV_SIDE;		break;
					case MODULE_MOONBASE_ADV_CENTER:	state = DISPOSE_MOONBASE_ADV_CENTER;	break;
					default:							state = ERROR;							break;
				}
			}
			else
				state = ERROR;	//Aucune zone choisie par le dispose manager.

			break;
		case DISPOSE_MOONBASE_MIDDLE:
			if(sub_anne_depose_modules_centre(MODULE_MOONBASE_MIDDLE, ADV_SIDE) != IN_PROGRESS)
				state = COMPUTE_WHAT_DOING;
			break;
		case DISPOSE_MOONBASE_OUR_CENTER:	//TODO mieux choisir le basis side
			if(sub_anne_depose_modules_centre(MODULE_MOONBASE_OUR_CENTER, ADV_SIDE) != IN_PROGRESS)
				state = COMPUTE_WHAT_DOING;
			break;
		case DISPOSE_MOONBASE_OUR_SIDE:	//TODO
			debug_printf("Fonction non impl�ment�e � ce jour : d�pose de notre c�t�\n");
			state = COMPUTE_WHAT_DOING;
			break;
		case DISPOSE_MOONBASE_ADV_SIDE: //TODO
			debug_printf("Fonction non impl�ment�e � ce jour : d�pose au c�t� adverse\n");
			state = COMPUTE_WHAT_DOING;
			break;
		case DISPOSE_MOONBASE_ADV_CENTER:	//TODO
			if(sub_anne_depose_modules_centre(MODULE_MOONBASE_ADV_CENTER, OUR_SIDE) != IN_PROGRESS)
				state = COMPUTE_WHAT_DOING;
			break;
		case COMPUTE_WHAT_DOING:
			//Si on a r�ussi la d�pose, alors c'est la fin.
			//Si on a �chou�, on se doit de retenter une autre d�pose.
			if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0)
				state = DONE; //g�nial !
			else
			{
				nb_fail++;
				if(nb_fail < 2)	//On autorise une seconde tentative (au premier passage ici : nb_fail == 1)
					state = COMPUTE_DISPOSE_ZONE;
				else
					state = ERROR;	//Bon bah, on a tent� 2 fois, et on a perdu... demerden sie sich
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
