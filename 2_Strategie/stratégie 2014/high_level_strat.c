/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : strat_pierre.c
 *	Package : Carte S²/strats2014
 *	Description : Stratégie de base de Pierre
 *	Auteur : Arnaud.
 *	Version 2014/03/08
 */


#include "high_level_strat.h"

#include "../QS/QS_outputlog.h"
#include "../QS/QS_who_am_i.h"
#include "../state_machine_helper.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include "../Supervision/Buzzer.h"
#include "../Supervision/SD/SD.h"
#include "../Supervision/LCD_interface.h"
#include <string.h>

#include "strats_2014/actions_pierre.h"
#include "strats_2014/actions_guy.h"
#include "strats_2014/actions_net.h"
#include "strats_2014/actions_Pfruit.h"
#include "strats_2014/actions_balls.h"
#include "strats_2014/actions_both_2014.h"

typedef struct{
	Uint8 priority;				// Priorité de l'action (faible = forte priorité)
	Uint8 failed;				// Nombre de fois ou l'action à raté
	bool_e done;				// Action terminée ou non
	bool_e enable;				// Action activée ou non
	time32_t t_begin;			// Temps match à partir duquel l'action peut être appelée (en ms)
	time32_t t_end;				// Temps match à partir duquel l'action ne peut plus être appelée (en ms)
	bool_e updated_for_lcd;		// Doit être mis à 1 à chaque mise à jours
	char chaine[10];			// Chaine de 9 caractères + '\0' pour décrir l'action sur le lcd
	bool_e ask_stop_request;	// Peut demander l'arrêt d'une sub_action
	bool_e initialized;			// Vaut TRUE lorsque la subactions a été initialisé sinon FASLE
	bool_e call_again;			// Doit être mis à 1 pour que l'action puisse se répété ssi il ne reste plus rien d'autre
}subaction_t;

static subaction_t subactions[SUB_NB];

static bool_e update_subaction_order = FALSE;

static bool_e sub_action_present(subaction_id_e subaction);

void high_level_strat(void){
	static subaction_id_e previous_state = SUB_INIT, state = SUB_INIT;
	static subaction_id_e previous_subaction = SUB_INIT, current_subaction = SUB_INIT;
	static bool_e init = FALSE;
	static bool_e previous_switch[4];

	bool_e current_switch[4];
	error_e sub_action;
	subaction_id_e sub;

	bool_e entrance = previous_state != state;

	if(entrance)
	{
		SD_printf	("# %s (%d) -> %s (%d)\n", subaction_name[previous_state], previous_state, subaction_name[state], state);
	}

	previous_state = state;
	previous_subaction = (state < SUB_NB )? state : previous_subaction;

	if(init == FALSE){
		// Initialisation de la structure
		for(sub=0;sub<SUB_NB;sub++){
			subactions[sub].priority = 0xFF;
			subactions[sub].failed = 0;
			subactions[sub].done = FALSE;
			subactions[sub].enable = FALSE;
			subactions[sub].t_begin = 0;
			subactions[sub].t_end = 90000;
			subactions[sub].updated_for_lcd = FALSE;
			subactions[sub].ask_stop_request = FALSE;
			subactions[sub].call_again = FALSE;
			subactions[sub].initialized = FALSE;
		}

		set_sub_act(SUB_INIT,		0,			TRUE,		0,			90000,		FALSE,		FALSE,			"INIT");

		if(QS_WHO_AM_I_get()==PIERRE)
		{
			//			sub action				priority	enable		t_begin			t_end		stop_request	call_again		chaine (max 8 caractères)
			set_sub_act(SUB_GETOUT,				0,			TRUE,		0,				90000,		FALSE,			FALSE,			"GetOut");
			set_sub_act(SUB_ACTION_INIT,		0,			TRUE,		0,				90000,		FALSE,			FALSE,			"Initiale");
			set_sub_act(SUB_LANCE,				1,			TRUE,		0,				90000,		FALSE,			FALSE,			"Lance");
			set_sub_act(SUB_FRESCO,				2,			TRUE,		0,				90000,		FALSE,			FALSE,			"Fresco");
			set_sub_act(SUB_FRUITS,				3,			TRUE,		0,				60000,		FALSE,			FALSE,			"Fruits");
			set_sub_act(SUB_TRIANGLE_START,		4,			TRUE,		0,				90000,		FALSE,			FALSE,			"Triangle Start");
			set_sub_act(SUB_DROP_FRUITS,		3,			FALSE,		0,				90000,		FALSE,			FALSE,			"DropFrui");
			set_sub_act(SUB_LANCE_ADV,			4,			TRUE,		0,				90000,		FALSE,			FALSE,			"LancAdv"); // Activer seulement en qualification
			set_sub_act(SUB_LANCE_ADV_FAIL,		4,			FALSE,		0,				90000,		FALSE,			FALSE,			"LancAdvF"); // Si le tire sur la lance adverse a échoué, on va tiré sur notre mammouth
			set_sub_act(SUB_FRUITS_ADV,			5,			FALSE,		40000,			70000,		FALSE,			FALSE,			"FruitAdv");
			set_sub_act(SUB_BALAYAGE,			6,			FALSE,		40000,			90000,		FALSE,			FALSE,			"Balayage");
			set_sub_act(SUB_PROTECTED_FIRE,		6,			FALSE,		0,				90000,		FALSE,			FALSE,			"Protect2");
			//set_sub_act(SUB_PROTECTED_FIRE_2,	6,			FALSE,		0,				90000,		FALSE,			FALSE,			"Protect");
			set_sub_act(SUB_PROTECTED_FIRE_ADV,	7,			FALSE,		0,				90000,		FALSE,			FALSE,			"ProtectA");
			set_sub_act(SUB_PROTECT_NORTH_WAY,	7,			TRUE,		0,				90000,		FALSE,			TRUE,			"ProtNord");
			//set_sub_act(SUB_PROTECT_NORTH_WAY2,	7,			FALSE,		0,				90000,		FALSE,			FALSE,			"ProtNor2");
			set_sub_act(SUB_FILET,				0,			TRUE,		TIME_TO_NET - 3000,	90000,	TRUE,			FALSE,			"Net");	//3000 : marge de sureté
		}else{
			//			sub action							priority	enable		t_begin		t_end		stop_request	call_again		chaine (max 8 caractères)
			set_sub_act(SUB_ACTION_INIT_GUY,				0,			TRUE,		0,			90000,		FALSE,			FALSE,			"Initiale");
			set_sub_act(SUB_ACTION_TRIANGLE_BETWEEN_TREE,	4,			FALSE,		0,			90000,		FALSE,			FALSE,			"TreeFire");
			set_sub_act(SUB_ACTION_TRIANGLE_VERTICALE_2,	4,			TRUE,		0,			90000,		FALSE,			FALSE,			"FeuFixe2");
			set_sub_act(SUB_ACTION_TRIANGLE_VERTICALE_3,	4,			TRUE,		0,			90000,		FALSE,			FALSE,			"FeuFixe3");
			set_sub_act(SUB_ACTION_TRIANGLE_VERTICALE_ADV,	4,			TRUE,		0,			90000,		FALSE,			FALSE,			"FeuFixeA");
			//set_sub_act(SUB_ACTION_TRIANGLE_START_ADVERSARY,4,			FALSE,		0,			90000,		FALSE,			FALSE,			"StartFiA");
			set_sub_act(SUB_ACTION_SCAN_HEARTH_CENTRALE,	4,			FALSE,		45000,		90000,		FALSE,			FALSE,			"ScanCent");
			set_sub_act(SUB_ACTION_SCAN_HEARTH_ADV,			4,			FALSE,		45000,		90000,		FALSE,			FALSE,			"ScanAdHe");
			set_sub_act(SUB_PROTECTED_FIRE_ADV,				15,			TRUE,		0,			90000,		FALSE,			TRUE,			"ProtectA");
			set_sub_act(SUB_SCAN,							5,			FALSE,		45000,		90000,		FALSE,			FALSE,			"Scan");
			set_sub_act(SUB_STEAL_SPACE_CRACKERS,			1,			FALSE,		20000,		90000,		FALSE,			FALSE,			"Steal");
		}

		// Calcul du nombre de subaction initialisée
		number_of_sub_action = 0;
		for(sub=0;sub<SUB_NB;sub++){
			if(subactions[sub].initialized == TRUE)
				number_of_sub_action++;
		}
	}

	// Sauvegarde de l'état des switchs
	current_switch[0] = SWITCH_STRAT_1;
	current_switch[1] = SWITCH_STRAT_2;
	current_switch[2] = SWITCH_STRAT_3;
	current_switch[3] = SWITCH_SAVE;

	if(		current_switch[0] != previous_switch[0] ||
			current_switch[1] != previous_switch[1] ||
			current_switch[2] != previous_switch[2] ||
			current_switch[3] != previous_switch[3]	||
			init == FALSE){
		// Si les switchs ont changés d'état ou si les subs actions n'ont pas été initialisées

		// Sub action à régler en fonction des switchs
		if(QS_WHO_AM_I_get()==PIERRE)
		{
			//SWITCH_STRAT_1 : On active le lancé d'une balle sur le mammouth adverse
			if(SWITCH_STRAT_3)	//TORCHE Activée
			{
				if(SWITCH_STRAT_2)	//Torche avant fresque
					LCD_printf(0,TRUE,TRUE,"Bal>Torch>Fre>HL Cp%c",(SWITCH_STRAT_1)?'1':'0');
				else
					LCD_printf(0,TRUE,TRUE,"Bal>Fre>Torch>HL Cp%c",(SWITCH_STRAT_1)?'1':'0');
			}
			else
			{	//TORCHE Désactivée
				if(SWITCH_STRAT_2)	//Torche avant fresque
					LCD_printf(0,TRUE,TRUE,"Bal>HL (NoTorch) Cp%c",(SWITCH_STRAT_1)?'1':'0');
				else
					LCD_printf(0,TRUE,TRUE,"Bal>Fre>HL(NoTor)Cp%c",(SWITCH_STRAT_1)?'1':'0');
			}

		}else{	//GUY

			if(SWITCH_SAVE)
				LCD_printf(0,TRUE,TRUE,"Our_Torch AdvTorch:%c",(SWITCH_STRAT_1)?'1':'0');	//Sud extérieur : proche des arbres
			else
			{
				if(SWITCH_STRAT_2)	//NORTH ou SOUTH
				{
					if(SWITCH_STRAT_3)	//intérieur ou extérieur
						LCD_printf(0,TRUE,TRUE,"South_Ext Torch:%c",(SWITCH_STRAT_1)?'1':'0');	//Sud extérieur : proche des arbres
					else
						LCD_printf(0,TRUE,TRUE,"South_Int Torch:%c",(SWITCH_STRAT_1)?'1':'0');	//Sud intérieur : proche du foyer central
				}
				else
				{
					if(SWITCH_STRAT_3)	//intérieur ou extérieur
						LCD_printf(0,TRUE,TRUE,"North_Ext Torch:%c",(SWITCH_STRAT_1)?'1':'0');	//Nord extérieur : proche des mammouths
					else
						LCD_printf(0,TRUE,TRUE,"North_Int Torch:%c",(SWITCH_STRAT_1)?'1':'0');	//Nord intérieur : proche du foyer central
				}
			}

		}

		previous_switch[0] = current_switch[0];
		previous_switch[1] = current_switch[1];
		previous_switch[2] = current_switch[2];
		previous_switch[3] = current_switch[3];
	}


	// Mise à jours de l'ordre des stratégies pour l'affichage
	if(update_subaction_order == TRUE){
		Uint8 temp_prio, index = 0;
		subaction_id_e sub_min;
		time32_t temp_t_begin;
		sub_action_order[0] = -1;

		while(index != number_of_sub_action){
			sub_min = 0xFF;
			temp_prio = 0xFF;
			temp_t_begin = 90000;
			for(sub=0;sub<SUB_NB;sub++){
				if(sub_action_present(sub) == FALSE
						&& subactions[sub].initialized == TRUE
						&&(subactions[sub].priority < temp_prio
						   || (subactions[sub].priority == temp_prio && subactions[sub].t_begin < temp_t_begin))){
					sub_min = sub;
					temp_prio = subactions[sub].priority;
					temp_t_begin = subactions[sub].t_begin;
				}
			}
			if(sub_min != 0xFF){
				subactions[index].updated_for_lcd = TRUE;
				sub_action_order[index++] = sub_min;
				sub_action_order[index] = -1;
			}
		}
		update_subaction_order = FALSE;
	}


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------En match
//----------------------------------------------------------------------------------------------------------------------
	if(global.env.match_started){ // On ne lance la machine à état que si le match est lancé

		// Lève le flag stop_request si une action prioritaire est disponible et urgente (ask_stop_request)
		if(stop_request == FALSE){
			for(sub=0;sub<SUB_NB;sub++){
				if(subactions[sub].initialized == TRUE
						&& subactions[sub].ask_stop_request == TRUE
						&& subactions[current_subaction].priority + subactions[current_subaction].failed > subactions[sub].priority + subactions[sub].failed
						&& subactions[sub].enable == TRUE
						&& previous_subaction != sub
						&& current_subaction != sub
						&& subactions[sub].t_begin <= global.env.match_time
						&& subactions[sub].t_end >= global.env.match_time){
					stop_request = TRUE;
					sub_action_broken = FALSE;
					BUZZER_play(2000, DEFAULT_NOTE, 1); // Avertisseur sonnore utile dans un premier temps pour vérifier le bon fonctionnement des actions urgentes
					break;
				}
			}
		}

//----------------------------------------------------------------------------------------------------------------------Cas particuliers
		if(QS_WHO_AM_I_get()==PIERRE)
		{
			if(global.env.match_time >= TIME_MATCH_TO_NET_ROTATE && state != SUB_FILET
					&& subactions[SUB_FILET].enable == TRUE && subactions[SUB_FILET].initialized == TRUE
					&& subactions[SUB_FILET].done == FALSE)
				state = SUB_FILET;
		}else{

		}


//----------------------------------------------------------------------------------------------------------------------Machine à état
		switch(state){

			case SUB_INIT :
				assert(subactions[SUB_INIT].initialized);
				subactions[SUB_INIT].done = TRUE;
				stop_request = FALSE;
				sub_action_broken = FALSE;
				main_strategie_used = TRUE;
				state = TAKE_DECISION;
				break;

			case SUB_GETOUT :
				assert(subactions[SUB_GETOUT].initialized);
				if(global.env.prop.calibrated){
					set_sub_act_done(SUB_GETOUT, TRUE);
					state = TAKE_DECISION;
				}else
					state = GET_OUT_IF_NO_CALIBRATION;
				break;

			//--------------------------------------------------------------------------------------------------------------Sub action Pierre

			case SUB_ACTION_INIT :
				assert(subactions[SUB_ACTION_INIT].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_INIT, TRUE);
				sub_action = sub_action_initiale(); // Configuration phase de qualification
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_INIT, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_INIT); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_LANCE :
				assert(subactions[SUB_LANCE].initialized);
				sub_action = sub_lance_launcher(FALSE,global.env.color); // Configuration phase de qualification
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_LANCE, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_LANCE); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_PROTECTED_FIRE :	//No break;	//Les deux sub_actions permettent d'alterner en high_level
			case SUB_PROTECTED_FIRE_2 :
				assert(subactions[SUB_PROTECTED_FIRE].initialized);
				sub_action = protected_fires(OUR_FIRES);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action)
					{
						case IN_PROGRESS :
							break;
						default:
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_PROTECTED_FIRE_ADV :
				assert(subactions[SUB_PROTECTED_FIRE_ADV].initialized);
				sub_action = protected_fires(ADVERSARY_FIRES);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action)
					{
						case IN_PROGRESS :
							break;
						default:
							inc_sub_act_failed(SUB_PROTECTED_FIRE_ADV);
							state = TAKE_DECISION;
							break;
					}
				break;
			case SUB_PROTECT_NORTH_WAY:	//No break;
			case SUB_PROTECT_NORTH_WAY2:
				assert(subactions[SUB_PROTECT_NORTH_WAY].initialized);
				sub_action = protect_north_way();
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action)
					{
						case IN_PROGRESS :
							break;
						default:
							state = TAKE_DECISION;
							break;
					}
				break;
			case SUB_LANCE_ADV :
				assert(subactions[SUB_LANCE_ADV].initialized);
				sub_action = sub_lance_launcher(FALSE,(global.env.color==RED)?YELLOW:RED);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_LANCE_ADV, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :

							// On active le tire sur notre mammouth si nous avons échoué sur celui-là de l'adversaire
							set_sub_act_enable(SUB_LANCE_ADV_FAIL, TRUE);
							set_sub_act_done(SUB_LANCE_ADV_FAIL, FALSE);

							set_sub_act_done(SUB_LANCE_ADV, TRUE);
							inc_sub_act_failed(SUB_LANCE_ADV);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_LANCE_ADV_FAIL :
				assert(subactions[SUB_LANCE_ADV_FAIL].initialized);
				sub_action = sub_lance_launcher(FALSE,global.env.color);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_LANCE_ADV_FAIL, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_LANCE_ADV_FAIL);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_FRUITS :
				assert(subactions[SUB_FRUITS].initialized);
				sub_action = manage_fruit(TREE_OUR, CHOICE_ALL_TREE, WAY_CHOICE);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_FRUITS, TRUE);
							set_sub_act_done(SUB_DROP_FRUITS, FALSE);
							set_sub_act_enable(SUB_DROP_FRUITS, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_FRUITS);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_DROP_FRUITS :
				assert(subactions[SUB_DROP_FRUITS].initialized);
				sub_action = strat_file_fruit();
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_DROP_FRUITS, TRUE);
							set_sub_act_enable(SUB_DROP_FRUITS, FALSE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_DROP_FRUITS);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_FRUITS_ADV :
				assert(subactions[SUB_FRUITS_ADV].initialized);
				sub_action = manage_fruit(TREE_ADVERSARY, CHOICE_ALL_TREE, WAY_CHOICE);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_FRUITS, TRUE);
							set_sub_act_done(SUB_DROP_FRUITS, FALSE);
							set_sub_act_enable(SUB_DROP_FRUITS, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_FRUITS);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_FILET :
				assert(subactions[SUB_FILET].initialized);
				strat_placement_net();
				break;

			case SUB_FRESCO :
				assert(subactions[SUB_FRESCO].initialized);
				sub_action = strat_manage_fresco();
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_FRESCO, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_FRESCO);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_TRIANGLE_START :
				assert(subactions[SUB_TRIANGLE_START].initialized);
				sub_action = do_triangle_start();
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_TRIANGLE_START, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_TRIANGLE_START);
							set_sub_act_enable(SUB_TRIANGLE_START, FALSE);

							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_BALAYAGE:
				assert(subactions[SUB_BALAYAGE].initialized);
				sub_action = sub_action_balayage();
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION;
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_BALAYAGE, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_BALAYAGE);
							state = TAKE_DECISION;
							break;
					}
				break;


	//-----------------------------------------------------------------------------------------------------------------------Sub action Guy

			case SUB_SCAN :
				assert(subactions[SUB_SCAN].initialized);
				break;

			case SUB_ACTION_INIT_GUY :
				assert(subactions[SUB_ACTION_INIT_GUY].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_INIT_GUY, TRUE);
				sub_action = sub_action_initiale_guy(); // Configuration phase de qualification
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_INIT_GUY, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_INIT_GUY); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							// On indique a high_strat_level que la sub_action_initiale a deja été faite pour pas la refaire une prochaine fois
							set_sub_act_done(SUB_ACTION_INIT_GUY,TRUE);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_ACTION_SCAN_HEARTH_CENTRALE :
				assert(subactions[SUB_ACTION_SCAN_HEARTH_CENTRALE].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_SCAN_HEARTH_CENTRALE, TRUE);
				sub_action = scan_and_back(SCAN_CENTRAL_HEARTH); // Configuration phase de qualification
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_SCAN_HEARTH_CENTRALE, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_SCAN_HEARTH_CENTRALE); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème

							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_ACTION_TRIANGLE_VERTICALE_2 :
				assert(subactions[SUB_ACTION_TRIANGLE_VERTICALE_2].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_TRIANGLE_VERTICALE_2, TRUE);
				sub_action = sub_action_triangle_on_edge(V_TRIANGLE_2);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_TRIANGLE_VERTICALE_2, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_TRIANGLE_VERTICALE_2); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							set_sub_act_enable(SUB_ACTION_TRIANGLE_VERTICALE_2, FALSE);

							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_ACTION_TRIANGLE_VERTICALE_3 :
				assert(subactions[SUB_ACTION_TRIANGLE_VERTICALE_3].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_TRIANGLE_VERTICALE_3, TRUE);
				sub_action = sub_action_triangle_on_edge(V_TRIANGLE_3);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_TRIANGLE_VERTICALE_3, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_TRIANGLE_VERTICALE_3); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							set_sub_act_enable(SUB_ACTION_TRIANGLE_VERTICALE_3, FALSE);

							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_ACTION_TRIANGLE_VERTICALE_ADV :
				assert(subactions[SUB_ACTION_TRIANGLE_VERTICALE_ADV].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_TRIANGLE_VERTICALE_ADV, TRUE);
				sub_action = sub_action_triangle_on_edge((global.env.color == RED)? V_TRIANGLE_4:V_TRIANGLE_1);
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_TRIANGLE_VERTICALE_ADV, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_TRIANGLE_VERTICALE_ADV); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							set_sub_act_enable(SUB_ACTION_TRIANGLE_VERTICALE_ADV, FALSE);

							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_ACTION_SCAN_HEARTH_ADV :
				assert(subactions[SUB_ACTION_SCAN_HEARTH_ADV].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_SCAN_HEARTH_ADV, TRUE);
				sub_action = scan_and_back(SCAN_ADV_HEARTH); // Configuration phase de qualification
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_SCAN_HEARTH_ADV, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_SCAN_HEARTH_ADV); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							// On indique a high_strat_level que la sub_action_initiale a deja été faite pour pas la refaire une prochaine fois
							set_sub_act_done(SUB_ACTION_SCAN_HEARTH_ADV,TRUE);
							state = TAKE_DECISION;
							break;
					}
				break;

			case SUB_ACTION_TRIANGLE_BETWEEN_TREE :
				assert(subactions[SUB_ACTION_TRIANGLE_BETWEEN_TREE].initialized);
				if(entrance)
					set_sub_act_enable(SUB_ACTION_TRIANGLE_BETWEEN_TREE, TRUE);
				sub_action = do_triangles_between_trees(); // Configuration phase de qualification
				if(sub_action_broken == TRUE)
					state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
				else
					switch(sub_action){
						case END_OK :
							set_sub_act_done(SUB_ACTION_TRIANGLE_BETWEEN_TREE, TRUE);
							state = TAKE_DECISION;
							break;

						case IN_PROGRESS :
							break;

						case END_WITH_TIMEOUT :
						case NOT_HANDLED :
						case FOE_IN_PATH :
							inc_sub_act_failed(SUB_ACTION_TRIANGLE_BETWEEN_TREE); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
							state = TAKE_DECISION;
							break;
					}
				break;

				case SUB_ACTION_TRIANGLE_START_ADVERSARY :
					assert(subactions[SUB_ACTION_TRIANGLE_START_ADVERSARY].initialized);
					if(entrance)
						set_sub_act_enable(SUB_ACTION_TRIANGLE_START_ADVERSARY, TRUE);
					sub_action = do_triangle_start_adversary(); // Configuration phase de qualification
					if(sub_action_broken == TRUE)
						state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
					else
						switch(sub_action){
							case END_OK :
								set_sub_act_done(SUB_ACTION_TRIANGLE_START_ADVERSARY, TRUE);
								state = TAKE_DECISION;
								break;

							case IN_PROGRESS :
								break;

							case END_WITH_TIMEOUT :
							case NOT_HANDLED :
							case FOE_IN_PATH :
								inc_sub_act_failed(SUB_ACTION_TRIANGLE_START_ADVERSARY); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
								state = TAKE_DECISION;
								break;
						}
					break;
				case SUB_STEAL_SPACE_CRACKERS :
					assert(subactions[SUB_STEAL_SPACE_CRACKERS].initialized);
					if(entrance)
						set_sub_act_enable(SUB_STEAL_SPACE_CRACKERS, TRUE);
					sub_action = sub_steal_space_crackers(); // Configuration phase de qualification
					if(sub_action_broken == TRUE)
						state = TAKE_DECISION; // Ne pas décompter le fail, ni rien si c'est juste une interruption d'action
					else
						switch(sub_action){
							case END_OK :
								set_sub_act_done(SUB_STEAL_SPACE_CRACKERS, TRUE);
								state = TAKE_DECISION;
								break;

							case IN_PROGRESS :
								break;

							case END_WITH_TIMEOUT :
							case NOT_HANDLED :
							case FOE_IN_PATH :
								//inc_sub_act_failed(SUB_ACTION_TRIANGLE_START_ADVERSARY); // Il vaut mieux passer par un mutateur qui va tester et avertir en cas de problème
								state = TAKE_DECISION;
								break;
						}
					break;



	//-----------------------------------------------------------------------------------------------------------------------Autre état

			case GET_OUT_IF_NO_CALIBRATION :
				if(QS_WHO_AM_I_get()==PIERRE)
					state = try_going_until_break(587, COLOR_Y(400), GET_OUT_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, FAST, ANY_WAY, NO_AVOIDANCE);
				else
					state = TURN_IF_NO_CALIBRATION;//try_going_until_break(960, COLOR_Y(400), GET_OUT_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, FAST, ANY_WAY, NO_AVOIDANCE);

				break;

			case TURN_IF_NO_CALIBRATION :

				if(QS_WHO_AM_I_get()==PIERRE){
					if(global.env.color == YELLOW)
						state = try_go_angle(PI4096/2, TURN_IF_NO_CALIBRATION, TAKE_DECISION, TAKE_DECISION, FAST);
					else
						state = TAKE_DECISION;
				}else
					state = TAKE_DECISION;

				if(ON_LEAVING(TURN_IF_NO_CALIBRATION))
					set_sub_act_done(SUB_GETOUT, TRUE);
				break;

	//----------------------------------------------------------------------------------------------------------------------- Cerveau

			case TAKE_DECISION :{
				// Init des variables
				Uint8 min_failed = 0xFF, priority = 0xFF;

				// Init des variables pour la call_again
				Uint8 min_failed_ca = 0xFF, priority_ca = 0xFF;
				subaction_id_e sub_with_call_again = TAKE_DECISION;

				// Détermination de la sub action la plus important à faire
				for(sub=0;sub<SUB_NB;sub++){
					if(subactions[sub].initialized == TRUE
							&& subactions[sub].enable == TRUE
							&& subactions[sub].done == FALSE
							&& subactions[sub].t_begin <= global.env.match_time
							&& subactions[sub].t_end >= global.env.match_time){
						if(previous_subaction != sub && subactions[sub].failed + subactions[sub].priority < min_failed + priority){
							min_failed = subactions[sub].failed;
							priority = subactions[sub].priority;
							state = sub;
							current_subaction = sub;
						}else if(subactions[sub].call_again == TRUE && subactions[sub].failed + subactions[sub].priority < min_failed_ca + priority_ca){
							min_failed_ca = subactions[sub].failed;
							priority_ca = subactions[sub].priority;
							sub_with_call_again = sub;
						}
					}
				}

				// Si l'on a pas trouvé de subaction standart on met la subaction call_again
				if(state == TAKE_DECISION){
					state = sub_with_call_again;
					current_subaction = sub_with_call_again;
				}



				stop_request = FALSE; // La subaction levant ce flag a été séléctionné, il faut donc le remetre à FALSE
				sub_action_broken = FALSE; // On réinitialise le flag

			}break;

			case SUB_NB : // Impossible state just for no warning
				if(entrance)
					error_printf("state = SUB_NB ! Rien à faire ici !\n");
				break;

			// No default for warning 'missing statement in switch'
		}
	}
	init = TRUE;
}

void set_update_subaction_order(bool_e update){
	update_subaction_order = update;
}

void set_sub_act_priority(subaction_id_e sub_action, Uint8 priority){
	assert(sub_action < SUB_NB);
	subactions[sub_action].priority = priority;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_enable(subaction_id_e sub_action, bool_e enable){
	assert(sub_action < SUB_NB);
	subactions[sub_action].enable = enable;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_done(subaction_id_e sub_action, bool_e done){
	assert(sub_action < SUB_NB);
	subactions[sub_action].done = done;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_call_again(subaction_id_e sub_action, bool_e call_again){
	assert(sub_action < SUB_NB);
	subactions[sub_action].call_again = call_again;
}

void set_sub_act_t_begin(subaction_id_e sub_action, time32_t t_begin){
	assert(sub_action < SUB_NB);
	subactions[sub_action].t_begin = t_begin;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_t_end(subaction_id_e sub_action, time32_t t_end){
	assert(sub_action < SUB_NB);
	subactions[sub_action].t_end = t_end;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_chaine(subaction_id_e sub_action, char* chaine){
	assert(sub_action < SUB_NB);
	strncpy(subactions[sub_action].chaine, chaine, 9); \
	subactions[sub_action].chaine[strlen(chaine)] = '\0';
	subactions[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_ask_stoop_resquest(subaction_id_e sub_action, bool_e ask_stop_request){
	assert(sub_action < SUB_NB);
	subactions[sub_action].ask_stop_request = ask_stop_request;
}

void inc_sub_act_failed(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].failed == 0xFF){
		debug_printf("Trying to incremente %s (%d) subaction but is overflow !\n", subaction_name[sub_action], sub_action);
		return;
	}
	subactions[sub_action].failed++;
}

void inc_sub_act_priority(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].priority == 0x99)
		subactions[sub_action].priority = 0;
	else
		subactions[sub_action].priority++;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_priority(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].priority == 0x00)
		subactions[sub_action].priority = 0x99;
	else
		subactions[sub_action].priority--;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_t_begin(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].t_begin >= subactions[sub_action].t_end - 1000)
		subactions[sub_action].t_begin = 0;
	else
		subactions[sub_action].t_begin+=1000;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_t_begin(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].t_begin <= 0)
		subactions[sub_action].t_begin = subactions[sub_action].t_end - 1000;
	else
		subactions[sub_action].t_begin-=1000;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_t_end(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].t_end >= 90000)
		subactions[sub_action].t_end = subactions[sub_action].t_begin + 1000;
	else
		subactions[sub_action].t_end+=1000;
	subactions[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_t_end(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subactions[sub_action].t_end <= subactions[sub_action].t_begin + 1000)
		subactions[sub_action].t_end = 90000;
	else
		subactions[sub_action].t_end-=1000;
	subactions[sub_action].updated_for_lcd = TRUE;
}

Uint8 get_sub_act_priority(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].priority;
}

bool_e get_sub_act_enable(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].enable;
}

bool_e get_sub_act_done(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].done;
}

bool_e get_sub_act_call_again(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].call_again;
}

Uint32 get_sub_act_t_begin(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].t_begin;
}

Uint32 get_sub_act_t_end(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].t_end;
}

char * get_sub_act_chaine(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].chaine;
}

bool_e get_sub_act_updated_for_lcd(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].updated_for_lcd;
}

void reset_sub_act_updated_for_lcd(subaction_id_e sub_action)
{
	assert(sub_action < SUB_NB);
	subactions[sub_action].updated_for_lcd = FALSE;
}

bool_e ask_stop_request(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subactions[sub_action].ask_stop_request;
}

void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, time32_t t_begin, time32_t t_end, bool_e ask_stop_request, bool_e call_again, char * chaine){
	assert(sub_action < SUB_NB);
	subactions[sub_action].priority = priority;
	subactions[sub_action].enable = enable;
	subactions[sub_action].t_begin = t_begin;
	subactions[sub_action].t_end = t_end;
	subactions[sub_action].ask_stop_request = ask_stop_request;
	strncpy(subactions[sub_action].chaine, chaine, 9);
	subactions[sub_action].chaine[strlen(chaine)] = '\0';
	subactions[sub_action].updated_for_lcd = TRUE;
	subactions[sub_action].call_again = call_again;
	subactions[sub_action].initialized = TRUE;
	update_subaction_order = TRUE;
}

static bool_e sub_action_present(subaction_id_e subaction){
	Uint8 i = 0;
	while(sub_action_order[i] != -1 && i < SUB_NB){
		if(sub_action_order[i++] == subaction)
			return TRUE;
	}
	return FALSE;
}


void STOP_REQUEST_IF_CHANGE(bool_e condition, Uint8 *state, Uint8 taille, Uint8 state_tab[]){
	Uint8 i;
	if(main_strategie_used == FALSE || stop_request == FALSE || condition == FALSE)
		return;
	for(i=0;i<taille;i++)
		if(*state == state_tab[i])
			return;
	*state = state_tab[0];
	sub_action_broken = TRUE; // Si on arrive jusqu'ici c'est qu'on a dû au minimum oublié un état de la machine
	// donc on signale que la sub_actions à été cassée
}
