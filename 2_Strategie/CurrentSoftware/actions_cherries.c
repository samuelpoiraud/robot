/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_cherries.c
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion des cerises et assiettes par Krusty
 *	Auteur : amurzeau
 *	Version 27 avril 2013
 */

#include "actions_cherries.h"
#include "actions_utils.h"
#include "output_log.h"

#define LOG_PREFIX "strat_cherries: "
#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)

error_e K_STRAT_sub_cherries_alexis() {
	//DP comme Do Plate
	enum state_e {
		DP_INIT,			//Initialise la machine � �tat.
		DP_GO_NEXT_PLATE,	//On va devant l'assiette � faire (current_plate)
		DP_PROCESS_PLATE,	//On prend les cerises de l'assiette
		DP_LAUNCH_CHERRIES,	//On lance les cerises dans le gateau
		DP_DROP_PLATE_AND_CHECK_NEXT,		//On lache l'assiette si on l'avait gard� et on passe � la suivante si c'est pas fini
		DP_FAILED,			//G�re les cas d'erreurs
		DP_DONE,			//On a fini de faire les assiettes
		DP_NBSTATE			//Pas un �tat, utilis� pour connaitre le nombre d'�tat
	};
	static enum state_e state = DP_INIT;
	static enum state_e last_state = DP_INIT;
	static enum state_e last_state_for_check_entrance = DP_INIT;


	////////////// Param�tre de la machine � �tat  /////////////////

	//Ce n'est pas mis en define pour garder �a en interne dans la fonction
	//Toutes les positons et angle sont ceux correspondant au cot� rouge

	typedef struct {
		Sint16 x;						//Position en X de l'assiette.
		bool_e keep_plate;				//TRUE si on doit garder l'assiette pendant le lanc� des cerises
		bool_e launch_cherries_after;	//TRUE si on doit lancer les cerises apr�s avoir fait l'assiette
	} CA_plate_data_e;

	//Position en X des assiettes
	static const Uint8 PLATE_NUMBER = 3;		//Nombre d'assiette � faire
	static const CA_plate_data_e PLATE_INFOS[3] = {	//Info sur les assiettes. Il doit y avoir autant de case dans ce tableau que PLATE_NUMBER.
	//  { x   , keep_plate, launch_cherries_after}
		{600  ,   FALSE   ,      FALSE           },
		{1400 ,   TRUE    ,      TRUE            },
		{1750 ,   FALSE   ,      TRUE            }
	};
	//La pince � assiette n'est pas sym�trique ni centr�e sur le robot, on doit avoir un offset en X pour que la pince ne tappe pas l'assiette alors qu'elle est ouverte
	static const Sint16 OFFSET_X_ROBOT = -30;
	//D�calage dans le cas de l'assiette cot� bleu - gateau. Avec l'offset normal on taperait dans le buffet. L'offset est quand m�me celui qu'on aurait du cot� rouge, pour un souci d'avoir un code partout pareil
	static const Sint16 OFFSET_X_ROBOT_FAR_CORNER = -12;
	static const Sint16 POSITION_Y_ROBOT_BEFORE_DOING_PLATE = 550; //Position en Y du robot pr�t � faire une assiette

	////////////////////////////////////////////////////////////////

	//Variables utilis�es par certains �tat:
	static Uint8 current_plate;		//Num�ro de l'assiette en train d'�tre faite, entre 0 et 2 inclus (voir plate_x_positons pour les positions)
	static error_e last_action_result;	//Contient la derni�re erreur retourn�e par un micro strat. Utilis� dans l'�tat DP_FAILED pour savoir quoi faire lors d'un probl�me
	static Sint16 real_x_offset;	//En static pour calculer l'offset qu'une seule fois

	//Si l'�tat � chang�, on rentre dans un nouvel �tat
	bool_e entrance = last_state_for_check_entrance != state;
	last_state_for_check_entrance = state;

	error_e return_value = IN_PROGRESS;

	//On a chang� d'�tat, on l'indique sur l'UART pour d�bugage
	if(entrance) {
		static const char* state_str[DP_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, DP_INIT);
			STATE_STR_DECLARE(state_str, DP_GO_NEXT_PLATE);
			STATE_STR_DECLARE(state_str, DP_PROCESS_PLATE);
			STATE_STR_DECLARE(state_str, DP_LAUNCH_CHERRIES);
			STATE_STR_DECLARE(state_str, DP_DROP_PLATE_AND_CHECK_NEXT);
			STATE_STR_DECLARE(state_str, DP_FAILED);
			STATE_STR_DECLARE(state_str, DP_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, DP_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_sub_cherries_alexis: state changed: %s(%d) -> %s(%d)\n",
			state_str[last_state], last_state,
			state_str[state], state);
	}

	switch(state) {
		//Initialise la machine � �tat.
		case DP_INIT:
			current_plate = 0;
			last_action_result = END_OK;

			state = DP_GO_NEXT_PLATE;
			break;

		//On va devant l'assiette � faire (current_plate)
		case DP_GO_NEXT_PLATE:
			if(entrance) {
				//Si on est rouge, on prend l'offset donn�
				if(global.env.color == RED)
					real_x_offset = OFFSET_X_ROBOT;
				//Si on est bleu et qu'on est � la derniere assiette, on prend l'offset sp�cial, avec - car on veux l'offset pour le cot� bleu
				else if(current_plate == PLATE_NUMBER - 1)
					real_x_offset = -OFFSET_X_ROBOT_FAR_CORNER;
				//Sinon on prend le normal
				else real_x_offset = -OFFSET_X_ROBOT;
			}
			state = try_going(PLATE_INFOS[current_plate].x + real_x_offset, COLOR_Y(POSITION_Y_ROBOT_BEFORE_DOING_PLATE), DP_GO_NEXT_PLATE, DP_PROCESS_PLATE, DP_FAILED, ANY_WAY, NO_DODGE_AND_WAIT);
			break;

		//On prend les cerises de l'assiette
		case DP_PROCESS_PLATE:
			//On peut prendre 2 assiette d'un coup, la deuxi�me on la garde comme paroi donc.

			last_action_result = K_STRAT_micro_grab_plate(PLATE_INFOS[current_plate].keep_plate, PLATE_INFOS[current_plate].x + real_x_offset, COLOR_Y(POSITION_Y_ROBOT_BEFORE_DOING_PLATE));
			state = check_sub_action_result(last_action_result, DP_PROCESS_PLATE, DP_LAUNCH_CHERRIES, DP_FAILED);
			break;

		//On lance les cerises dans le gateau
		case DP_LAUNCH_CHERRIES:
			if(PLATE_INFOS[current_plate].launch_cherries_after) {
				last_action_result = K_STRAT_micro_launch_cherries(STRAT_LC_PositionNear, 8, FALSE);
				state = check_sub_action_result(last_action_result, DP_LAUNCH_CHERRIES, DP_DROP_PLATE_AND_CHECK_NEXT, DP_FAILED);
			} else state = DP_DROP_PLATE_AND_CHECK_NEXT;
			break;

		//On lache l'assiette si on l'avait gard� et on passe � la suivante si c'est pas fini
		case DP_DROP_PLATE_AND_CHECK_NEXT:
			if(PLATE_INFOS[current_plate].keep_plate) {
				switch(K_STRAT_micro_drop_plate(TRUE)) {
					case IN_PROGRESS: break;

					case NOT_HANDLED:
					case FOE_IN_PATH:
					case END_WITH_TIMEOUT: state = DP_FAILED; break;

					case END_OK: state = DP_DONE; break;
				}
			} else state = DP_DONE;

			//Les cerises sont lanc�es et on a lach� l'assiette, on regarde si on a vraiment fini
			if(state == DP_DONE) {
				current_plate++;
				if(current_plate < PLATE_NUMBER)
					state = DP_GO_NEXT_PLATE;		//En fait c'est pas fini, il nous reste d'autre assiette � faire
			}
			break;

		//G�re les cas d'erreurs
		case DP_FAILED:
			switch(last_state) {
				//On � pas pu aller � l'assiette suivante, un ennemi nous bloque ?
				//On tente la suivante, mais �a risque de fail aussi ...
				//On ne peut pas avoir une assiette d'avance car on l'a d�j� drop ... (ou jamais ramass�e)
				case DP_GO_NEXT_PLATE:
					state = DP_DROP_PLATE_AND_CHECK_NEXT;
					break;

				//On a eu un probl�me lors de la prise des cerises
				//Si on a quand m�me potentiellement pris des cerises, on va les lancer, sinon pas la peine d'essayer
				case DP_PROCESS_PLATE:
					if(last_action_result != NOT_HANDLED)
						state = DP_LAUNCH_CHERRIES;
					else state = DP_DROP_PLATE_AND_CHECK_NEXT;
					break;

				//On a pas pu lancer les cerises, tant pis, on prie pour que �a passe pour la prochaine assiette ...
				case DP_LAUNCH_CHERRIES:
					state = DP_DROP_PLATE_AND_CHECK_NEXT;
					break;

				//On a pas pu lacher l'assiette, un adversaire nous bloque ?
				//On passe a la suivante, mais ya des chances que �a foire ...
				#warning "Cas foireux: on a pas lach� l'assiette et on passe � la suivante .. � r�gler prochainement"
				case DP_DROP_PLATE_AND_CHECK_NEXT:
					current_plate++;
					if(current_plate < PLATE_NUMBER)
						state = DP_GO_NEXT_PLATE;		//On fait les autres restante quand m�me
					else state = DP_DONE;
					break;

				//Fausse erreur, mais une vrai dans le code, on retourne END_WITH_TIMEOUT
				default:
					return_value = END_WITH_TIMEOUT;

			}
			if(return_value != IN_PROGRESS)
				state = DP_INIT;
			break;

		//On a fini de faire les assiettes
		case DP_DONE:
			state = DP_INIT;
			return_value = END_OK;

		//Pas un �tat, utilis� pour connaitre le nombre d'�tat
		case DP_NBSTATE:
			break;
	}

	last_state = last_state_for_check_entrance; //last_state contient l'�tat avant de passer dans le switch, pour �tre utilisable dans les �tats quand entrance vaut TRUE

	if(return_value != IN_PROGRESS)
		state = DP_INIT;
	return return_value;
}

//On est devant une assiette, cette fonction prend l'assiette et les cerises qu'il y a dedans.
//Mettre keep_plate � TRUE pour garder l'assiette verticalement lors du lanc� de cerise.
//initial_y_position est absolu, il faut utiliser COLOR_Y, et dans cette fonction il n'en faut pas.
error_e K_STRAT_micro_grab_plate(bool_e keep_plate, Sint16 initial_x_position, Sint16 initial_y_position) {
	//GP comme grab plate
	enum state_e {
		GP_INIT,			//Initialise la machine � �tat
		GP_ADJUST_ANGLE,	//Ajuste l'angle, on doit �tre bien droit avant de prendre l'assiette
		GP_PREPARE_PLIER,	//Descend la pince a la hauteur de l'assiette
		GP_CATCH_PLATE,		//Avance et serre l'assiette 2 fois pour �tre sur de bien la prendre
		GP_TAKING_CHERRIES,	//Leve l'assiette et prend les cerises
		GP_PULL_OUT,		//Sort de la zone � assiette � la position d'origine (initial_y_position)
		GP_DROP_PLATE,		//Lache l'assiette et remonte la pince verticallement, pour �tre pr�s pour d'autre actions
		GP_FAILED,			//On a eu un probl�me, cet �tat analyse la situation et g�re les cas
		GP_TURN_90_AND_DROP,//Dans le cas ou on a pas pu lacher l'assiette, on se tourne de 90� et on retente
		GP_WAIT_PLATE_MVT,	//Attend les derniers mouvement de la pince � assiette avant de terminer l'op�ration. error_e � retourner dans what_to_return_after_end
		GP_DONE,			//Fin de l'action, on retourne END_OK
		GP_NBSTATE			//Pas un �tat, utilis� pour connaitre le nombre d'�tat
	};
	static enum state_e state = GP_INIT;
	static enum state_e last_state = GP_INIT;
	static enum state_e last_state_for_check_entrance = GP_INIT;


	////////////// Param�tre de la machine � �tat  /////////////////

	//Ce n'est pas mis en define pour garder �a en interne dans la fonction

	//En vitesse CUSTOM de 8 [mm/32/5ms]: 1024 [mm/4096/5ms] soit 50mm/s
	//Temps pour serrer une assiette: < 0.4s
	//Distance parcourue pendant ce temps: 20mm
	//On ferme l'ax12 sur une distance de 20mm pour fermer pendant le mouvement, entre y = 500 et y = 520 sachant qu'on fait 5mm sans avoir l'assiette au bout

	static const Sint16 Y_POS_AX12_CLOSING   = 520;   //D�but du serrage de l'assiette
	static const Sint16 Y_POS_CATCHING_PLATE = 520;   //D�but de la vitesse lente
	static const Sint16 Y_POS_CATCHED_PLATE  = 500;   //Fin de la vitesse lente, apr�s on soul�ve l'assiette pour prendre les cerises
	static const Uint8 CATCHING_PLATE_SPEED = 8 + 8; //vitesse de 8 [mm/32/5ms] == 50mm/s, le premier 8 c'est un offset n�cessaire pour indiquer � la prop que la vitesse est une vitesse "analogique" (voir pilot.c, PILOT_set_speed)

	static const bool_e USE_DOUBLE_CLOSE_AX12 = TRUE; //Si TRUE, on serre 2 fois l'assiette pour mieux la prendre
	static const time32_t TIME_BEFORE_DROP_DURATION = 1000;  //Temps d'attente pendant que les cerises tombe de l'assiette en position verticale dans la tremie
	static const Uint8 MAX_DROP_RETRY_COUNT = 3;	//Nombre d'essai max de lacher l'assiette avant de fail

	////////////////////////////////////////////////////////////////

	//Variable utilis�es par certains �tat:
	//Celles utilis�es en interne dans un seul �tat sont d�clar�s dans le case (pour avoir la variable au plus proche de son utilisation et prot�ge du copier coller � outrance
	static time32_t plate_vertical_begin_time;  //Moment ou l'assiette est arriv�e a la position verticale
	static error_e what_to_return_after_end;

	//Si l'�tat � chang�, on rentre dans un nouvel �tat
	bool_e entrance = last_state_for_check_entrance != state;
	last_state_for_check_entrance = state;

	error_e return_value = IN_PROGRESS;

	//On a chang� d'�tat, on l'indique sur l'UART pour d�bugage
	if(entrance) {
		static const char* state_str[GP_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, GP_INIT);
			STATE_STR_DECLARE(state_str, GP_ADJUST_ANGLE);
			STATE_STR_DECLARE(state_str, GP_PREPARE_PLIER);
			STATE_STR_DECLARE(state_str, GP_CATCH_PLATE);
			STATE_STR_DECLARE(state_str, GP_TAKING_CHERRIES);
			STATE_STR_DECLARE(state_str, GP_PULL_OUT);
			STATE_STR_DECLARE(state_str, GP_DROP_PLATE);
			STATE_STR_DECLARE(state_str, GP_FAILED);
			STATE_STR_DECLARE(state_str, GP_TURN_90_AND_DROP);
			STATE_STR_DECLARE(state_str, GP_WAIT_PLATE_MVT);
			STATE_STR_DECLARE(state_str, GP_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, GP_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_micro_grab_plate: state changed: %s(%d) -> %s(%d)\n",
			state_str[last_state], last_state,
			state_str[state], state);
	}

	switch(state) {
		//Initialise la machine � �tat
		case GP_INIT:
			plate_vertical_begin_time = 0;
			what_to_return_after_end = END_OK;
			state = GP_ADJUST_ANGLE;
			break;

		//Ajuste l'angle, on doit �tre bien droit avant de prendre l'assiette
		case GP_ADJUST_ANGLE:
			state = try_go_angle(COLOR_ANGLE(PI4096/2), GP_ADJUST_ANGLE, GP_PREPARE_PLIER, GP_FAILED, FAST);
			break;

		//Descend la pince a la hauteur de l'assiette
		case GP_PREPARE_PLIER:
			if(entrance) {
				ACT_plate_rotate(ACT_PLATE_RotateMid);
				ACT_plate_plier(ACT_PLATE_PlierOpen);
				ACT_plate_rotate(ACT_PLATE_RotateDown);
			}
			state = check_act_status(ACT_QUEUE_Plate, GP_PREPARE_PLIER, GP_CATCH_PLATE, GP_FAILED);
			break;

		//Avance et serre l'assiette 2 fois pour �tre sur de bien la prendre
		case GP_CATCH_PLATE:
		{
			static bool_e catching_plate;			//TRUE quand on serre l'assiette

			if(entrance) {
				//On arme le warner � la position Y_POS_AX12_CLOSING pour serrer l'assiette, � Y_POS_CATCHING_PLATE on commence � la pousser
				catching_plate = FALSE;
				if(Y_POS_AX12_CLOSING != 0)
					ASSER_WARNER_arm_y(COLOR_Y(Y_POS_AX12_CLOSING));
			}
			state = try_going_multipoint((displacement_t[]){
				{{initial_x_position, COLOR_Y(Y_POS_CATCHING_PLATE)}, FAST},					//On avance rapidement sur l'assiette
				{{initial_x_position, COLOR_Y(Y_POS_CATCHED_PLATE)} , CATCHING_PLATE_SPEED}},	//Puis on la pousse (en la serrant) doucement
				2, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT,
				GP_CATCH_PLATE, GP_TAKING_CHERRIES, GP_FAILED);

			//Gestion du serrage en parall�le quand on atteint la position Y_POS_AX12_CLOSING ou quand on change d'�tat sans que le warner n'ait �t� d�clench�
			//si Y_POS_AX12_CLOSING vaut 0, on n'utilise pas le warner et on fait le serrage syst�matiquement � la fin du mouvement
			if((Y_POS_AX12_CLOSING != 0 && global.env.asser.reach_y) || (state != GP_CATCH_PLATE && catching_plate == FALSE)) {
				if(USE_DOUBLE_CLOSE_AX12) {					//Si on fait un double serrage, on serre juste
					ACT_plate_plier(ACT_PLATE_PlierClose);
				} else {									//Sinon on serre & monte l'assiette directement
					//ACT_plate_plier(ACT_PLATE_PlierClose); //Non utile, le fait de remonter l'assiette implique que la pince se serre
					ACT_plate_rotate(ACT_PLATE_RotateUp);
				}
				catching_plate = TRUE;
			}
			break;
		}

		//Leve l'assiette et prend les cerises
		case GP_TAKING_CHERRIES:
			if(entrance) {
				//On prend les cerises, si on fait double serrage, il faut ajouter les commandes pour ouvrir et refermer la pince puis monter l'assiette.
				//On le fait que maintenant quand le robot ne bouge plus.
				if(USE_DOUBLE_CLOSE_AX12) {
					ACT_plate_plier(ACT_PLATE_PlierOpen);
					//ACT_plate_plier(ACT_PLATE_PlierClose); //Non utile, le fait de remonter l'assiette implique que la pince se serre
					ACT_plate_rotate(ACT_PLATE_RotateUp);
				}
				//Si on ne fait pas un double serrage, on a d�ja demand� de fermer la pince puis de monter l'assiette
			}
			state = check_act_status(ACT_QUEUE_Plate, GP_TAKING_CHERRIES, GP_PULL_OUT, GP_FAILED);
			if(state != GP_TAKING_CHERRIES)
				plate_vertical_begin_time = global.env.match_time;
			break;

		//Sort de la zone � assiette � la position d'origine (initial_y_position, position absolue, pas de COLOR_Y !!)
		case GP_PULL_OUT:
			//Si �vitement: l'adversaire est dans notre zone !!!!
			//state = try_going(initial_x_position, initial_y_position, GP_PULL_OUT, GP_DROP_PLATE, GP_FAILED, FORWARD, NO_DODGE_AND_NO_WAIT);
			state = try_going_multipoint((displacement_t[]){{{initial_x_position, initial_y_position}, FAST}}, 1, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BREAK, GP_PULL_OUT, GP_DROP_PLATE, GP_FAILED);
			break;

		//Lache l'assiette et remonte la pince verticallement, pour �tre pr�s pour d'autre actions
		case GP_DROP_PLATE:
			//On doit garder l'assiette, donc rien � faire on a fini
			if(keep_plate == TRUE) {
				state = GP_DONE;
			} else {
				//On ne redescend pas l'assiette avant un certain temps (qui peut �tre nul, dans ce cas aucune attente)
				if(plate_vertical_begin_time == 0 || global.env.match_time > plate_vertical_begin_time + TIME_BEFORE_DROP_DURATION) {
					switch(K_STRAT_micro_drop_plate(FALSE)) {
						case IN_PROGRESS: break;

						case NOT_HANDLED:
						case FOE_IN_PATH:
						case END_WITH_TIMEOUT: state = GP_FAILED; break;

						case END_OK: state = GP_DONE; break;
					}
				}
			}
			break;

		//On a eu un probl�me, cet �tat analyse la situation et g�re les cas
		case GP_FAILED:
			switch(last_state) {
				//On a pas pu faire notre angle WTF?? Quelqu'un nous bloque ? On peut pas faire l'assiette    //pas impl�ment� car tir� par les cheveux: si notre angle est trop loin de 90�, sinon on fait comme si rien �tait et on avant vers l'assiette
				case GP_ADJUST_ANGLE:
					return_value = NOT_HANDLED;

				//On a pas pu bouger la pince correctement,
				//un adversaire est chez nous � la place de l'assiette et bloque son mouvement ??
				//Ou on est perdu niveau odom�trie ?
				//=> on referme la pince et retourne NOT_HANDLED (assiette pas faite)
				case GP_PREPARE_PLIER:
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					what_to_return_after_end = NOT_HANDLED;
					state = GP_WAIT_PLATE_MVT;
					break;

				//On a pas pu prendre l'assiette ou la soulever, peut �tre qu'on la mal prise ou on est pas bien en face ...
				//On annule et repasse � l'�tat GP_PULL_OUT et on se barre (en gros on fait le cas normal)
				case GP_TAKING_CHERRIES:
				case GP_CATCH_PLATE:
					state = GP_PULL_OUT;
					break;

				//On peut pas se barrer !!! Du monde est derri�re nous avec un strat comme notre Tiny ?
				//Tant pis, on peu se d�placer autre part sans taper dans nos verres vu qu'on est d�j� asser loin du bord
				//Donc on continue en lachant l'assiette si on doit le faire ...
				//   //On indique quand m�me � la strat sup�rieure qu'on a vu un ennemi ... (�a pourrait aussi �tre une erreur prop, mais en quel honneur ? m�me si on a d�j� eu des probl�mes, ce mvt est relativement simple et pas long normalement ...)
				case GP_PULL_OUT:
					//return_value = FOE_IN_PATH;
					state = GP_DROP_PLATE;
					break;

				//On a pas pu jeter l'assiette ... qqun est sur l'emplacement de l'assiette ?
				//Un robot ? Bon bin on a potentiellement plus nos verre empil�, on retente 3 fois d'ouvrir la pince histoire de le g�ner un peu et esperer le faire planter ...
				case GP_DROP_PLATE:
				{
					static Uint8 retry_count = 0; //Compteur du nombre de fois qu'on a ressay� d�j�, faudrait pas retenter en boucle
					if(retry_count < MAX_DROP_RETRY_COUNT) {
						retry_count++;
						//On remonte la pince, pour mieux la redescendre apr�s
						ACT_plate_rotate(ACT_PLATE_RotateUp);
						state = GP_DROP_PLATE;
					} else {   //On a vraiment pas pu, on espere avoir bien fait chier la chose qui nous g�ne et on remonte l'assiette pour la lacher 90� plus loin
						state = GP_TURN_90_AND_DROP;
					}
					break;
				}

				//On refail ... on informe la strat sup�rieure, ne devrait jamais arriver � moins d'avoir un bug en code
				case GP_TURN_90_AND_DROP:
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					what_to_return_after_end = END_WITH_TIMEOUT;
					state = GP_WAIT_PLATE_MVT;
					break;

				//Fausse erreur, mais une vrai dans le code, on retourne le cas fatal: END_WITH_TIMEOUT apr�s avoir remont� l'assiette dans le doute. (Faudrait pas tout d�foncer ...)
				default:
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					what_to_return_after_end = END_WITH_TIMEOUT;
					state = GP_WAIT_PLATE_MVT;
					break;
			}
			//Si on return direct, on repasse � l'init pour les actions suivantes
			if(return_value != IN_PROGRESS)
				state = GP_INIT;
			break;

		case GP_TURN_90_AND_DROP:
			//On attend le RotateUp fait dans GP_FAILED, un coup fini on fait le mouvement
			if(ACT_get_last_action_result(ACT_QUEUE_Plate) != ACT_FUNCTION_InProgress) {
				state = try_go_angle(COLOR_ANGLE(PI4096/2), GP_TURN_90_AND_DROP, GP_DROP_PLATE, GP_FAILED, FAST);
			}
			break;

		//Attend les derniers mouvement de la pince � assiette avant de terminer l'op�ration. error_e � retourner dans what_to_return_after_end
		case GP_WAIT_PLATE_MVT:
			if(ACT_get_last_action_result(ACT_QUEUE_Plate) != ACT_FUNCTION_InProgress) {
				state = GP_INIT;
				return_value = what_to_return_after_end;
			}
			break;

		//Fin de l'action, on retourne END_OK
		case GP_DONE:
			state = GP_INIT;
			return_value = END_OK;

		//Pas un �tat, utilis� pour connaitre le nombre d'�tat
		case GP_NBSTATE:
			break;

	}

	last_state = last_state_for_check_entrance; //last_state contient l'�tat avant de passer dans le switch, pour �tre utilisable dans les �tats quand entrance vaut TRUE

	if(return_value != IN_PROGRESS)
		state = GP_INIT;
	return return_value;
}


//Lance les cerises qui sont dans le robot et compte le nombre de cerise lanc�e.
//Cette microstrat peut donc soit lancer un nombre fixe de fois (sur�valu� pour �tre sur de toutes les envoyer) ou de compter le nombre de cerise envoy�es et comparer avec expected_cherry_number
error_e K_STRAT_micro_launch_cherries(STRAT_launch_cherries_positions_e position, Uint8 expected_cherry_number, bool_e smooth_shake) {
	enum state_e {
		LC_INIT,			//Initialise un nouveau lanc� de cerise
		LC_PREPARE_POS,		//Va � la position demand�e (dans position) ou une autre si on a fail sur une des positions
		LC_AIM,				//Vise le gateau en tournant le robot
		LC_FIRE,			//Lance une cerise (si elle n'est pas blanche elle ne va pas sur le gateau)
		LC_CHECK_CHERRY,	//Regarde ce qu'on a lanc�: une cerise blanche, pourie ou rien et agit en cons�quence
		LC_SHAKE_CHERRIES,	//Quand les cerises sont bloqu�es et qu'aucune ne vient dans le lanceur de balle, on secoue le robot. Si smooth_shake vaut TRUE, on bouge la pince � la position interm�diaire et on remonte la pince pour faire un choc, sinon on la descend tout en bas.
		LC_FAILED,			//G�re les cas d'erreur
		LC_DONE,			//Termin�
		LC_NBSTATE			//Pas un �tat, utilis� pour connaitre le nombre d'�tat
	};
	static enum state_e state = LC_INIT;
	static enum state_e last_state = LC_INIT;
	static enum state_e last_state_for_check_entrance = LC_INIT;

	////////////// Param�tres de la machine � �tat  /////////////////

	//Ce n'est pas mis en define pour garder �a en interne dans la fonction
	//Toutes les positons et angle sont ceux correspondant au cot� rouge

	typedef struct {
		Sint16 x;			//en mm
		Sint16 y;			//en mm, cot� rouge
		Sint16 angle_red;	//en rad/4096
		Sint16 angle_blue;	//en rad/4096
		Uint16 ball_launcher_speed;
	} LC_position_t;

	static const bool_e BLIND_LAUNCH = FALSE;	//Si TRUE, on lance un nombre fixe de fois, peut importe ce qu'on lance m�me si on lance rien
	static const Uint8 MAX_FAILED_LAUNCH_BEFORE_SHAKE = 2;	//Nombre de lanc� rat� avant de secouer les cerises. Le compteur est reinitialis� a chaque secouage pour pas secouer apr�s chaque fail
	static const Uint8 MAX_FAILED_LAUNCH = 3;	//Nombre maximum de lanc� rat� possible (rat� dans le sens aucune cerise n'a �t� prise)
	static const Uint8 MAX_BLOCKED_AX12 = 2;	//Maximum de fois que le lanc� de balle peut fail. Apr�s �a on arrete de secouer le robot et on retourne a la strat sup�rieur qu'on a fail.
	static const LC_position_t POSITIONS[STRAT_LC_NumberOfPosition] = {	//Param�tres li� � chaque position. Y est indiqu� pour le cot� rouge, il faut donc utiliser COLOR_Y.
	//  { x  ,  y  , angle_red,angle_blue,ball_launcher_speed}
		{1015, 700 ,   -4150  ,  -9400   ,    6300           },		//STRAT_LC_PositionMid
		{1400, 620 ,   -2900  , -10549   ,    5850           }		//STRAT_LC_PositionNear
	};
	static const ASSER_speed_e AIM_ROTATION_SPEED = FAST;	//Vitesse de rotation pour viser le gateau

	///////////////////////////////////////////////////////////////

	//Variables utilis�es par certains �tat:
	static STRAT_launch_cherries_positions_e current_position;	//Position choisie actuellement pour lancer les cerises
	static STRAT_launch_cherries_positions_e original_position;	//Premi�re position qu'on a tent�. Pour �viter de boucler si aucune position n'est possible pour lancer les cerises
	static Uint8 ball_launched;					//Nombre de cerises lanc�es
	static Uint8 ball_continuous_fail;			//Nombre de cerises qui aurait du �tre lanc� mais on a rien capt� devant le capteur ... Peut �tre d� a des cerises bloqu�es
	static Uint8 blocked_ax12_situation_count;	//Nombre de fois ou on a pas pu lancer une cerise car l'ax12 ou le lanceur de balle s'est bloqu�
	static Uint8 ball_fail_count_before_shake;	//Nombre de fail encore possible avant de secouer les cerises

	//Si l'�tat � chang�, on rentre dans un nouvel �tat
	bool_e entrance = last_state_for_check_entrance != state;
	last_state_for_check_entrance = state;

	error_e return_value = IN_PROGRESS;

	//On a chang� d'�tat, on l'indique sur l'UART pour d�bugage
	if(entrance) {
		static const char* state_str[LC_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, LC_INIT);
			STATE_STR_DECLARE(state_str, LC_PREPARE_POS);
			STATE_STR_DECLARE(state_str, LC_AIM);
			STATE_STR_DECLARE(state_str, LC_FIRE);
			STATE_STR_DECLARE(state_str, LC_CHECK_CHERRY);
			STATE_STR_DECLARE(state_str, LC_SHAKE_CHERRIES);
			STATE_STR_DECLARE(state_str, LC_FAILED);
			STATE_STR_DECLARE(state_str, LC_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, LC_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_micro_launch_cherries: state changed: %s(%d) -> %s(%d)\n",
			state_str[last_state], last_state,
			state_str[state], state);
	}

	switch(state) {
		//Initialise un nouveau lanc� de cerise
		case LC_INIT:
			original_position = position;
			current_position = original_position;
			ball_launched = 0;
			ball_continuous_fail = 0;
			blocked_ax12_situation_count = 0;
			ball_fail_count_before_shake = MAX_FAILED_LAUNCH_BEFORE_SHAKE;
			state = LC_PREPARE_POS;
			break;

		//Va � la position demand�e (dans position) ou une autre si on a fail sur une des positions
		case LC_PREPARE_POS:
			state = try_going(POSITIONS[current_position].x, COLOR_Y(POSITIONS[current_position].y), LC_PREPARE_POS, LC_AIM, LC_FAILED, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			break;

		//Vise le gateau en tournant le robot
		case LC_AIM:
		{
			static Sint16 aim_angle;
			if(entrance) {
				if(global.env.color == RED)
					aim_angle = POSITIONS[current_position].angle_red;
				else aim_angle = POSITIONS[current_position].angle_blue;
			}
			state = try_go_angle(aim_angle, LC_AIM, LC_FIRE, LC_FAILED, AIM_ROTATION_SPEED);
			break;
		}

		//Lance une cerise (si elle n'est pas blanche elle ne va pas sur le gateau)
		case LC_FIRE:
			if(entrance)
				ACT_ball_sorter_next_autoset_speed(POSITIONS[current_position].ball_launcher_speed);
			state = check_act_status(ACT_QUEUE_BallSorter, LC_FIRE, LC_CHECK_CHERRY, LC_FAILED);
			break;

		//Regarde ce qu'on a lanc�: une cerise blanche, pourie ou rien et agit en cons�quence
		case LC_CHECK_CHERRY:
			//Si BLIND_LAUNCH == TRUE, on verifie pas si on a bien lanc� une cerise
			if(BLIND_LAUNCH == FALSE) {
				switch(global.env.color_ball) {
					case ACT_BALLSORTER_WHITE_CHERRY:
					case ACT_BALLSORTER_BAD_CHERRY:
						ball_launched++;
						ball_continuous_fail = 0;
						break;

					case ACT_BALLSORTER_NO_CHERRY:
						ball_continuous_fail++;
						ball_fail_count_before_shake--;
						break;

					default:
						OUTPUTLOG_printf(TRUE, LOG_LEVEL_Debug, LOG_PREFIX"global.env.color_ball: invalid value %d\n", global.env.color_ball);
				}
				//On a fini de lancer des cerises quand on a lanc� le nombre pr�vu de cerise ou si on a eu MAX_FAILED_LAUNCH � la suite
				if(ball_launched >= expected_cherry_number || ball_continuous_fail >= MAX_FAILED_LAUNCH)
					state = LC_DONE;
				//Si on n'a rien pris avec l'ax12 un certain nombre de fois de suite, on secoue les cerises
				else if(ball_fail_count_before_shake == 0) {
					ball_fail_count_before_shake = MAX_FAILED_LAUNCH_BEFORE_SHAKE;
					state = LC_SHAKE_CHERRIES;
				} else state = LC_FIRE;
			} else {
				ball_launched++;
				//On a fini de lancer des cerises quand on a lanc� le nombre pr�vu de cerise + une s�curit�
				if(ball_launched >= expected_cherry_number + MAX_FAILED_LAUNCH)
					state = LC_DONE;
				else state = LC_FIRE;
			}
			break;

		//Quand les cerises sont bloqu�es et qu'aucune ne vient dans le lanceur de balle, on secoue le robot.
		//Si smooth_shake vaut TRUE, on bouge la pince � la position interm�diaire et on remonte la pince pour faire un choc, sinon on la descend tout en bas.
		case LC_SHAKE_CHERRIES:
			if(entrance) {
				if(smooth_shake)
					ACT_plate_rotate(ACT_PLATE_RotateMid);
				else ACT_plate_rotate(ACT_PLATE_RotateDown);
				ACT_plate_rotate(ACT_PLATE_RotateUp);
			}
			state = check_act_status(ACT_QUEUE_BallSorter, LC_SHAKE_CHERRIES, LC_FIRE, LC_FAILED);
			break;

		//G�re les cas d'erreur
		case LC_FAILED:
			switch(last_state) {
				//On a pas pu se mettre � une position pour lancer les cerises, on en tente une autre.
				//Si on a d�j� essay� toutes les positions, on retourne erreur (NOT_HANDLED)
				case LC_PREPARE_POS:
				//On a pas pu se tourner ... �a ne devrait pas arriver, mais on tente une autre position
				case LC_AIM:
					current_position = (current_position+1) % STRAT_LC_NumberOfPosition;
					if(current_position == original_position)
						return_value = NOT_HANDLED;
					else state = LC_PREPARE_POS;
					break;

				//On a pas pu prendre une cerise pour la lancer, c'est bloqu� dans le robot ??
				//Dans ce cas on secoue un coup et c'est reparti. On compte le nombre de fail pour pas boucler en cas de probl�me persistant ...
				case LC_FIRE:
					blocked_ax12_situation_count++;
					if(blocked_ax12_situation_count > MAX_BLOCKED_AX12)
						return_value = END_WITH_TIMEOUT;
					else state = LC_SHAKE_CHERRIES;
					break;

				//On a pas pu bouger la pince pour secouer les cerises ...
				//Tant pis on continu � lancer les cerises
				case LC_SHAKE_CHERRIES:
					state = LC_FIRE;
					break;

				//Fausse erreur, mais une vrai dans le code, on retourne le cas fatal: END_WITH_TIMEOUT.
				default:
					return_value = END_WITH_TIMEOUT;
			}
			if(return_value != IN_PROGRESS)
				state = LC_INIT;
			break;

		//Lanc� termin�
		case LC_DONE:
			state = LC_INIT;
			return_value = END_OK;

		//Pas un �tat, utilis� pour connaitre le nombre d'�tat
		case LC_NBSTATE:
			break;
	}

	last_state = last_state_for_check_entrance; //last_state contient l'�tat avant de passer dans le switch, pour �tre utilisable dans les �tats quand entrance vaut TRUE

	if(return_value != IN_PROGRESS)
		state = LC_INIT;
	return return_value;
}

//Lache un assiette du cot� de notre zone de d�part (mais ne se rapproche pas)
error_e K_STRAT_micro_drop_plate(bool_e turn_before_drop) {
	enum state_e {
		DP_TURN,	//Se tourne vers la zone de d�part
		DP_DROP,	//Lache l'assiette
		DP_FAILED,	//G�re les erreurs
		DP_DONE,	//On a fini
		DP_NBSTATE	//Pas un �tat, utilis� pour connaitre le nombre d'�tat
	};
	static enum state_e state = DP_TURN;
	static enum state_e last_state = DP_TURN;
	static enum state_e last_state_for_check_entrance = DP_TURN;

	//Si l'�tat � chang�, on rentre dans un nouvel �tat
	bool_e entrance = last_state_for_check_entrance != state;
	last_state_for_check_entrance = state;

	error_e return_value = IN_PROGRESS;

	//On a chang� d'�tat, on l'indique sur l'UART pour d�bugage
	if(entrance) {
		static const char* state_str[DP_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, DP_TURN);
			STATE_STR_DECLARE(state_str, DP_DROP);
			STATE_STR_DECLARE(state_str, DP_FAILED);
			STATE_STR_DECLARE(state_str, DP_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, DP_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_micro_drop_plate: state changed: %s(%d) -> %s(%d)\n",
			state_str[last_state], last_state,
			state_str[state], state);
	}

	switch(state) {
		case DP_TURN:
			if(turn_before_drop)
				state = try_go_angle(COLOR_ANGLE(PI4096/2), DP_TURN, DP_DROP, DP_FAILED, FAST);
			else state = DP_DROP;
			break;

		case DP_DROP:
			if(entrance) {
				ACT_plate_rotate(ACT_PLATE_RotateMid);
				ACT_plate_plier(ACT_PLATE_PlierOpen);
				//ACT_plate_plier(ACT_PLATE_PlierClose); //Non utile, le fait de remonter l'assiette implique que la pince se serre
				ACT_plate_rotate(ACT_PLATE_RotateUp);
			}
			state = check_act_status(ACT_QUEUE_Plate, DP_DROP, DP_DONE, DP_FAILED);  //Fail possible si une cerise s'est coinc�e entre la pince et le robot ...
			break;

		case DP_FAILED:
			switch(last_state) {
				//On a pas pu tourner, on retourne NOT_HANDLED
				case DP_TURN:
					return_value = NOT_HANDLED;

				//On a pas pu lacher l'assiette, on retourne END_WITH_TIMEOUT
				case DP_DROP:
					return_value = END_WITH_TIMEOUT;

				//Fausse erreur, mais une vrai dans le code, on retourne  END_WITH_TIMEOUT apr�s avoir remont� l'assiette dans le doute. (Faudrait pas tout d�foncer ...)
				default:
					return_value = END_WITH_TIMEOUT;
			}
			if(return_value != IN_PROGRESS)
				state = DP_TURN;
			break;

		case DP_DONE:
			state = DP_TURN;
			return_value = END_OK;

		//Pas un �tat, utilis� pour connaitre le nombre d'�tat
		case DP_NBSTATE:
			break;
	}

	last_state = last_state_for_check_entrance; //last_state contient l'�tat avant de passer dans le switch, pour �tre utilisable dans les �tats quand entrance vaut TRUE

	if(return_value != IN_PROGRESS)
		state = DP_TURN;
	return return_value;
}
