/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_glasses.c
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion des verres par Krusty
 *	Auteur : amurzeau
 *	Version 26 avril 2013
 */

#include "actions_glasses.h"
#include "avoidance.h"
#include "act_functions.h"
#include "output_log.h"
#include "zone_mutex.h"

#define LOG_PREFIX "strat_glasses: "
#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)

//D�fini un nom d'�tat dans un tableau de string. Utilis� pour afficher le nom d'une valeur d'un enum, par exemple:
//Pour que tableau[UN_ETAT_OU_ENUM] = "UN_ETAT_OU_ENUM", il faut faire STATE_STR_DECLARE(tableau, UN_ETAT_OU_ENUM)
#define STATE_STR_DECLARE(tableau, state)  tableau[state] = #state;

//Le bloc de code est dans une do {} while(0) pour pouvoir �tre mis dans un if sans accolade du genre if(condition) STATE_STR_INIT_UNDECLARED(tableau, nbstate);
//Initialise les �tats non d�fini d'un tableau de string.
//Les noms des �tats non d�finis est mis � "Unknown"
//Pour d�finir un �tat dans un tableau de nom d'�tat, veuillez utiliser STATE_STR_DECLARE(tableau, etat)
//Un exemple est dispo dans K_STRAT_sub_glasses_alexis
#define STATE_STR_INIT_UNDECLARED(tableau, nbstate) \
	do {\
		Uint8 i; \
		for(i=0; i<(nbstate); i++) {\
			if(tableau[i] == 0) \
				tableau[i] = "Unknown"; \
		} \
	}while(0)

error_e K_STRAT_sub_glasses_alexis() {
	//Trajectoire 1
	displacement_t MOVE_POINTS_1[11] = {
		//Row 1
		{{1000, COLOR_Y(950 )}, FAST},		// 0
		{{1050, COLOR_Y(1500)}, FAST},		// 1
		//Row 2
		{{820 , COLOR_Y(1330)}, FAST},		// 2
		{{750 , COLOR_Y(1130)}, FAST},		// 3
		{{750 , COLOR_Y(750 )}, FAST},		// 4
		//Row 3
		{{500 , COLOR_Y(1134)}, FAST},		// 5
		{{446 , COLOR_Y(1220)}, SLOW},		// 6

		//Go home
		//Case Krusty
		{{980 , COLOR_Y(220 )}, FAST},		// 7
		{{1000, COLOR_Y(190 )}, FAST},		// 8

		//Case Tiny
		{{250 , COLOR_Y(400 )}, FAST},		// 9
		{{270 , COLOR_Y(190 )}, FAST}		// 10
	};

	static const Uint8 TRAJECTORY_NUMBER_1 = 3;
	static const displacement_block_t TRAJECTORIES_1[3] = {
	//    avoidance_type     , nb_points , move_points_begin_index
		{NO_AVOIDANCE        ,    2      ,           0            },	//Row 1
		{NO_AVOIDANCE        ,    3      ,           2            },	//Row 2
		{NO_DODGE_AND_WAIT   ,    2      ,           5            }		//Row 3
	};
	static const Uint8 TRAJECTORY_TO_HOME_NUMBER_1 = 2;
	static const displacement_block_t TRAJECTORIES_TO_HOME_1[2] = {
		{NO_AVOIDANCE        ,    2      ,           7            },	//Go home Krusty
		{NO_AVOIDANCE        ,    2      ,           9            }		//Go home Tiny
	};

	//Trajectoire 2
	displacement_t MOVE_POINTS_2[20] = {
		//Phase 1: on prend 4 verres
		{{1000, COLOR_Y(190 )}, FAST},	// 0
		{{1140, COLOR_Y(780 )}, FAST},	// 1
		{{1016, COLOR_Y(1155)}, FAST},	// 2
		{{796 , COLOR_Y(1281)}, SLOW},	// 3
		{{557 , COLOR_Y(1258)}, SLOW},	// 4
		{{297 , COLOR_Y(978 )}, SLOW},	// 5

		//Phase 2: 2 verres restants
		{{536 , COLOR_Y(966 )}, FAST},	// 6
		{{782 , COLOR_Y(980 )}, FAST},	// 7
		{{950 , COLOR_Y(898 )}, FAST},	// 8

		//Go home
		//Premier choix de case
		//Case Tiny
		{{308 , COLOR_Y(578 )}, FAST},	// 9
		{{250 , COLOR_Y(400 )}, FAST},	// 10
		{{250 , COLOR_Y(140 )}, FAST},	// 11

		//Case Krusty
		{{1000, COLOR_Y(400 )}, FAST},	// 12
		{{1000, COLOR_Y(140 )}, FAST},	// 13

		//Autres choix: en mode safe pour y  aller (on s'�carte un peut si yavais un ennemi qui nous bloquait)
		//Case Krusty
		{{960 , COLOR_Y(800 )}, FAST},	// 14
		{{1000, COLOR_Y(400 )}, FAST},	// 15
		{{1000, COLOR_Y(140 )}, FAST},	// 16

		//Case Tiny
		{{315 , COLOR_Y(800 )}, FAST},	// 17
		{{250 , COLOR_Y(400 )}, FAST},	// 18
		{{250 , COLOR_Y(140 )}, FAST},	// 19
	};

	static const Uint8 TRAJECTORY_NUMBER_2 = 2;
	static const displacement_block_t TRAJECTORIES_2[2] = {
	//    avoidance_type     , nb_points , move_points_begin_index
		{NO_DODGE_AND_NO_WAIT,    6      ,           0            },	//Phase 1
		{NO_AVOIDANCE        ,    3      ,           6            }		//Phase 2
	};
	static const Uint8 TRAJECTORY_TO_HOME_NUMBER_2 = 4;
	static const displacement_block_t TRAJECTORIES_TO_HOME_2[4] = {
		{NO_DODGE_AND_NO_WAIT,    3      ,           9            },	//Go home Tiny
		{NO_DODGE_AND_NO_WAIT,    2      ,           12           },	//Go home Krusty
		{NO_DODGE_AND_NO_WAIT,    3      ,           14           },	//Go home Krusty safe
		{NO_DODGE_AND_NO_WAIT,    3      ,           17           }		//Go home Tiny safe
	};

//	return K_STRAT_micro_do_glasses(TRAJECTORY_TO_HOME_NUMBER_1, TRAJECTORIES_TO_HOME_1, TRAJECTORY_NUMBER_1, TRAJECTORIES_1, MOVE_POINTS_1);
	return K_STRAT_micro_do_glasses(TRAJECTORY_TO_HOME_NUMBER_2, TRAJECTORIES_TO_HOME_2, TRAJECTORY_NUMBER_2, TRAJECTORIES_2, MOVE_POINTS_2);
}

//Prend tous les verres en suivant une trajectoire donn�es. A la fin les verres sont pos� (apr�s la derni�re position)
error_e K_STRAT_micro_do_glasses(Uint8 trajectory_to_home_number, const displacement_block_t trajectories_to_home[], Uint8 trajectory_number, const displacement_block_t trajectories[], displacement_t move_points[]) {
	//GM pour Glass Move
	enum state_e {
		GM_INIT = 0,				//Initialisation: on initialise la sous-strat grab_glass: on a pris 0 verre et l'ascenseur est en bas et ouvert
		GM_CATCH_GLASSES,			//Avance et prend des verres en m�me temps
		GM_WAIT_ACT,				//Attent que les actionneurs des ascenseurs aient fini de bouger, l'�tat suivant est dans la variable what_to_do_after_act
		GM_CHECK_TINY_ZONE,			//V�rifie que la zone de Tiny est dispo. Si c'est pas le cas, on va sur la zone de Krusty.
		GM_GO_HOME,					//Va � une case d�part pour d�poser les verres
		GM_ROTATE,					//Se tourne pour �tre bien droit, pour en sortant, ne pas pousser les verres en tournant ...
		GM_PUT_DOWN_GLASSES,		//D�pose les verres
		GM_EXTRACT_FROM_GLASSES,	//Recule le robot pour pouvoir faire d'autre mouvement librement sans toucher les verres ramen�s
		GM_FAILED,					//G�re les cas d'erreur / �vitement
		GM_DONE,					//Les verres on �t� ramass�
		GM_NBSTATE					//Pas un �tat mais indique le nombre d'�tat
	};
	static enum state_e state = GM_INIT;
	static enum state_e last_state_for_check_entrance = GM_INIT;
	static enum state_e last_state = GM_INIT;


	////// Param�tres /////
	static const Uint8 FIRST_SAFE_HOME = 2;
	static const Sint16 SAFE_Y_ZONE = 450;
	static const avoidance_type_e AVOIDANCE_TYPE_ON_EXTRACT = NO_DODGE_AND_WAIT;
	static const Uint16 AVOIDANCE_MAX_WAIT_ON_EXTRACT = 1000;

	//Pour l'�tat GL_WAIT_ACT:
	static enum state_e what_to_do_after_act;
	static Uint8 current_displacement_block;
	static Uint8 force_first_point;				//Si != 0, on force le premier point � force_first_point. Utilis� pour reprendre une trajectoire a moiti� faite
	static Uint8 current_dest_home;				//Numero de la trajectoire pour aller � une case d�part pour poser les verres. Si on ne peut pas, on en tentera une autre.

	static error_e zone_starttiny_lock_state;

	//Si l'�tat � chang�, on rentre dans un nouvel �tat
	bool_e entrance = last_state_for_check_entrance != state;
	last_state_for_check_entrance = state;

	error_e return_value = IN_PROGRESS;


	//On a chang� d'�tat, on l'indique sur l'UART pour d�bugage
	if(entrance) {
		static const char* state_str[GM_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, GM_INIT);
			STATE_STR_DECLARE(state_str, GM_CATCH_GLASSES);
			STATE_STR_DECLARE(state_str, GM_WAIT_ACT);
			STATE_STR_DECLARE(state_str, GM_GO_HOME);
			STATE_STR_DECLARE(state_str, GM_ROTATE);
			STATE_STR_DECLARE(state_str, GM_PUT_DOWN_GLASSES);
			STATE_STR_DECLARE(state_str, GM_EXTRACT_FROM_GLASSES);
			STATE_STR_DECLARE(state_str, GM_FAILED);
			STATE_STR_DECLARE(state_str, GM_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, GM_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_sub_glasses_alexis: state changed: %s(%d) -> %s(%d)\n",
			state_str[last_state], last_state,
			state_str[state], state);
	}

	switch(state) {
		case GM_INIT:
			//On reset l'�tat des machines � �tat des prise de verre pour les 2 cot�s
			K_STRAT_micro_grab_glass(TRUE, ACT_LIFT_Left);
			K_STRAT_micro_grab_glass(TRUE, ACT_LIFT_Right);
			current_displacement_block = 0;
			current_dest_home = 0;
			force_first_point = 0;
			zone_starttiny_lock_state = IN_PROGRESS;
			global.env.must_drop_glasses_at_end = FALSE;
			state = GM_CATCH_GLASSES;
			break;

		case GM_CATCH_GLASSES:
			state = try_going_multipoint(&(move_points[trajectories[current_displacement_block].move_points_begin_index + force_first_point]), trajectories[current_displacement_block].nb_points - force_first_point,
					FORWARD, trajectories[current_displacement_block].avoidance_type, END_AT_LAST_POINT,
					GM_CATCH_GLASSES, GM_WAIT_ACT, GM_FAILED);   //Etats suivant: in_progress, success, fail

			//Temps qu'on a pas la zone, on tente de la prendre ... pendant le deplacmenet
			if(zone_starttiny_lock_state != END_OK)
				zone_starttiny_lock_state = ZONE_try_lock(MZ_StartTiny, 0);

			if(state == GM_WAIT_ACT) {
				force_first_point = 0;
				current_displacement_block++;
				if(current_displacement_block < trajectory_number)
					what_to_do_after_act = GM_CATCH_GLASSES; //Quand les ascenseurs auront fini de bouger, on continuera les autres d�placements
				else what_to_do_after_act = GM_CHECK_TINY_ZONE;	//Si aucun autre d�placement apr�s �a, on va d�poser les verres
			} else if(state == GM_CATCH_GLASSES) {
				//On tente de prendre des verres pendant le d�placement ...
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Left);
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Right);
			}
			break;

		case GM_WAIT_ACT:
			//On attend que les actionneurs aient fini de prendre les verres avant de faire autre chose
			if(ACT_get_last_action_result(ACT_QUEUE_LiftLeft)  != ACT_FUNCTION_InProgress &&
			   ACT_get_last_action_result(ACT_QUEUE_LiftRight) != ACT_FUNCTION_InProgress)
			{
				state = what_to_do_after_act;
			}
			break;

		case GM_CHECK_TINY_ZONE:
			if(zone_starttiny_lock_state != IN_PROGRESS && zone_starttiny_lock_state != END_OK)
				zone_starttiny_lock_state = ZONE_try_lock(MZ_StartTiny, 0);
			switch(zone_starttiny_lock_state) {
				case IN_PROGRESS: break;
				case END_OK: state = GM_GO_HOME;

				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
				default:
					current_dest_home = 1;	//On passe a la zone de Krusty
					if(trajectory_to_home_number > 1)
						state = GM_GO_HOME;
					else {		//Pas d'autre choix, donc on fera a la fin
						global.env.must_drop_glasses_at_end = TRUE;
						state = GM_DONE;
					}
					break;
			}
			break;

		case GM_GO_HOME:
			state = try_going_multipoint(&(move_points[trajectories_to_home[current_dest_home].move_points_begin_index]), trajectories_to_home[current_dest_home].nb_points,
					FORWARD, trajectories_to_home[current_dest_home].avoidance_type, END_AT_LAST_POINT,
					GM_GO_HOME, GM_ROTATE, GM_FAILED);   //Etats suivant: in_progress, success, fail
			if(state == GM_GO_HOME) {
				//On tente de prendre des verres pendant le d�placement ...
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Left);
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Right);
			}
			break;

		case GM_ROTATE:
			state = try_go_angle(COLOR_ANGLE(-PI4096/2), GM_ROTATE, GM_PUT_DOWN_GLASSES, GM_FAILED, FAST);
			break;

		case GM_PUT_DOWN_GLASSES:
			if(!GLASS_SENSOR_LEFT)  //S'il n'y a pas de verre en bas, on descend l'ascenseur avant
				ACT_lift_translate(ACT_LIFT_Left, ACT_LIFT_TranslateDown);
			ACT_lift_plier(ACT_LIFT_Left, ACT_LIFT_PlierOpen);
			if(!GLASS_SENSOR_RIGHT)  //S'il n'y a pas de verre en bas, on descend l'ascenseur avant
				ACT_lift_translate(ACT_LIFT_Right, ACT_LIFT_TranslateDown);
			ACT_lift_plier(ACT_LIFT_Right, ACT_LIFT_PlierOpen);
			state = GM_WAIT_ACT;
			what_to_do_after_act = GM_EXTRACT_FROM_GLASSES;   //Apr�s que les actionneurs auront fini leur mouvement, la micro_strat sera finie
			break;

		case GM_EXTRACT_FROM_GLASSES:
			if(entrance)
				AVOIDANCE_set_timeout(AVOIDANCE_MAX_WAIT_ON_EXTRACT);
			state = try_going_multipoint((displacement_t[]){{{global.env.pos.x, COLOR_Y(SAFE_Y_ZONE)}, FAST}}, 1, BACKWARD, AVOIDANCE_TYPE_ON_EXTRACT, END_AT_BREAK, GM_EXTRACT_FROM_GLASSES, GM_DONE, GM_FAILED);
			break;

		case GM_FAILED:
			state = GM_INIT;
			switch(last_state) {

				//Quelqu'un nous g�ne ... on va tenter de se d�placer au point suivant en sautant celui qu'on a essay� d'atteindre ...
				case GM_CATCH_GLASSES:
					force_first_point = global.env.pos.nb_points_reached + 1;	//On va au suivant apr�s celui qu'on voulait atteindre
					if(force_first_point > trajectories[current_displacement_block].nb_points) {
						//Numero du point incoh�rent sup�rieur au nombre de point -> on fait comme si on avait fait la trajectoire entiere,
						//on s'arrange pour que first_point = nb_points == 0 points � faire.
						//On passera a la trajectoire suivant directement apr�s
						force_first_point = trajectories[current_displacement_block].nb_points;
					}
					state = GM_CATCH_GLASSES;
					break;

				//On a un probl�me: evitement (ou on s'est plant� dans le mur en arrivant la ou on d�pose les verres)
				case GM_GO_HOME:
					//On regarde si on est d�j� dans la zone de d�part
					if(COLOR_Y(global.env.pos.y) < SAFE_Y_ZONE) {
						//On est proche, on a du taper dans le mur ... Donc pas grave on continue la strat normalement
						state = GM_ROTATE;
					} else {
						//On est loin, on tente un autre choix d'arriv�e en mode safe
						if(current_dest_home == FIRST_SAFE_HOME)
							current_dest_home++;
						else current_dest_home = FIRST_SAFE_HOME;
						if(current_dest_home < trajectory_to_home_number) {
							state = GM_GO_HOME;
						} else {
							//En fait on a d�j� fait tous les choix ! => on ne peut pas revenir chez nous, on le fera a la fin
							global.env.must_drop_glasses_at_end = TRUE;
							state = GM_DONE;
						}
					}
					break;

				//Si on fail, bin tantpis, cet �tat pour tourner le robot c'est de l'est�tique :o
				case GM_ROTATE:
					state = GM_PUT_DOWN_GLASSES;
					break;

				//On a eu un probl�me en reculant, on a fonc� dans quelque chose ?
				case GM_EXTRACT_FROM_GLASSES:
					state = GM_DONE;
					break;

				//Fausse erreur, mais une vrai dans le code, on retourne le cas fatal: END_WITH_TIMEOUT
				default:
					state = GM_INIT;
					return_value = END_WITH_TIMEOUT;
					break;
			}
			if(state == GM_INIT && return_value == IN_PROGRESS)
				return_value = NOT_HANDLED;
			else if(return_value != IN_PROGRESS)
				state = GM_INIT;
			break;

		case GM_DONE:
			state = GM_INIT;
			return_value = END_OK;
			break;

		case GM_NBSTATE:
			break;
	}

	last_state = last_state_for_check_entrance; //last_state contient l'�tat avant de passer dans le switch, pour �tre utilisable dans les �tats quand entrance vaut TRUE

	if(return_value != IN_PROGRESS)
		state = GM_INIT;
	return return_value;
}

//Cette micro strat prend des verres quand il y en a avec les ascensseurs (lift).
//Dans les autres strat, il se peut que la fin de cette sub_action ne soit pas obligatoire ...
//Renvoie END_OK si on a fini de prendre un verre ou qu'il n'y en a pas a prendre (en gros quand les actionneurs ne bouge pas)
error_e K_STRAT_micro_grab_glass(bool_e reset_state, ACT_lift_pos_t lift_pos) {
	//GL pour Glass Lift
	enum state_lift_e {
		GL_WAIT_FIRST_GLASS = 0,   //L'ascensseur est en bas, on attend un premier verre pour le prendre
		GL_WAIT_GLASS_GRABED,      //On attend d'avoir pris le verre et l'avoir mont�. Si le nombre de verre pris >= 3, on passe � GL_DONE
		GL_WAIT_NEXT_GLASSES,      //On attend autre verre, on descendra l'ascensseur avant de le prendre contrairement au premier
		GL_DONE,
		GL_NBSTATE   //Pas un �tat mais indique le nombre d'�tat
	};

	static enum state_lift_e state_act_left = GL_WAIT_FIRST_GLASS;
	static Uint8 number_of_grabed_glass_left = 0;

	static enum state_lift_e state_act_right = GL_WAIT_FIRST_GLASS;
	static Uint8 number_of_grabed_glass_right = 0;

	enum state_lift_e* current_act_state;
	enum state_lift_e current_act_last_state;
	Uint8* current_act_grabed_glasses;
	queue_id_e current_act;
	bool_e current_act_is_glass_detected;

	//choix des variables a utiliser, �vite d'avoir 2 fonction avec juste des Left remplac� en Right ...
	if(lift_pos == ACT_LIFT_Left) {
		current_act_is_glass_detected = GLASS_SENSOR_LEFT;
		current_act = ACT_QUEUE_LiftLeft;
		current_act_state = &state_act_left;
		current_act_grabed_glasses = &number_of_grabed_glass_left;
	} else {
		current_act_is_glass_detected = GLASS_SENSOR_RIGHT;
		current_act = ACT_QUEUE_LiftRight;
		current_act_state = &state_act_right;
		current_act_grabed_glasses = &number_of_grabed_glass_right;
	}

	current_act_last_state = *current_act_state;

	//Reset de l'�tat, avant de prendre des verres
	if(reset_state) {
		*current_act_state = GL_WAIT_FIRST_GLASS;
		*current_act_grabed_glasses = 0;
		return END_OK;
	}

	switch(*current_act_state){
		case GL_WAIT_FIRST_GLASS:
			if(current_act_is_glass_detected) {
				ACT_lift_plier(lift_pos, ACT_LIFT_PlierClose);
				ACT_lift_translate(lift_pos, ACT_LIFT_TranslateUp);  //Mise en queue
				*current_act_state = GL_WAIT_GLASS_GRABED;
			} else return END_OK;  //Pas de verre � prendre, donc a fini
			break;
		case GL_WAIT_GLASS_GRABED:
			if(ACT_get_last_action_result(current_act) != ACT_FUNCTION_InProgress){   //Si l'ascensseur est en haut avec un verre, on est pr�t pour en prendre un autre
				(*current_act_grabed_glasses)++;   //Un verre de pris de plus
				if(*current_act_grabed_glasses < 3) //On en a pas pris 3, donc on peut en prendre d'autre
					*current_act_state = GL_WAIT_NEXT_GLASSES;
				else *current_act_state = GL_DONE;  //Sinon on peut pas en prendre d'autre, donc fin
			}
			break;
		case GL_WAIT_NEXT_GLASSES:
			if(current_act_is_glass_detected) {
				ACT_lift_plier(lift_pos, ACT_LIFT_PlierOpen);
				ACT_lift_translate(lift_pos, ACT_LIFT_TranslateDown);
				ACT_lift_plier(lift_pos, ACT_LIFT_PlierClose);
				if(*current_act_grabed_glasses < 2) //Si on en a pas d�j� 2, on monte l'ascensseur au plus haut, sinon on pourra pas tenir 2 verres tout en haut
					ACT_lift_translate(lift_pos, ACT_LIFT_TranslateUp);  //Mise en queue
				else ACT_lift_translate(lift_pos, ACT_LIFT_TranslateMid);
				*current_act_state = GL_WAIT_GLASS_GRABED;
			} else return END_OK;  //On a fini de prendre un verre si on en prend pas d'autre
			break;
		case GL_DONE:
			return END_OK;
			break;

		default:
			break;
	}

	//On a chang� d'�tat, on l'indique sur l'UART pour d�bugage
	if(*current_act_state != current_act_last_state) {
		static const char* state_str[GL_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, GL_WAIT_FIRST_GLASS);
			STATE_STR_DECLARE(state_str, GL_WAIT_GLASS_GRABED);
			STATE_STR_DECLARE(state_str, GL_WAIT_NEXT_GLASSES);
			STATE_STR_DECLARE(state_str, GL_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, GL_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_micro_grab_glass: state changed: %s(%d) -> %s(%d)\n",
			state_str[current_act_last_state], current_act_last_state,
			state_str[*current_act_state], *current_act_state);
	}

	return IN_PROGRESS;
}
