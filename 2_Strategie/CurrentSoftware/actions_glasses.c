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

#define LOG_PREFIX "strat_glasses: "
#define STATECHANGE_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES, log_level, LOG_PREFIX format, ## __VA_ARGS__)

//Défini un nom d'état dans un tableau de string. Utilisé pour afficher le nom d'une valeur d'un enum, par exemple:
//Pour que tableau[UN_ETAT_OU_ENUM] = "UN_ETAT_OU_ENUM", il faut faire STATE_STR_DECLARE(tableau, UN_ETAT_OU_ENUM)
#define STATE_STR_DECLARE(tableau, state)  tableau[state] = #state;

//Le bloc de code est dans une do {} while(0) pour pouvoir être mis dans un if sans accolade du genre if(condition) STATE_STR_INIT_UNDECLARED(tableau, nbstate);
//Initialise les états non défini d'un tableau de string.
//Les noms des états non définis est mis à "Unknown"
//Pour définir un état dans un tableau de nom d'état, veuillez utiliser STATE_STR_DECLARE(tableau, etat)
//Un exemple est dispo dans K_STRAT_sub_glasses_alexis
#define STATE_STR_INIT_UNDECLARED(tableau, nbstate) \
	do {\
		Uint8 i; \
		for(i=0; i<(nbstate); i++) {\
			if(tableau[i] == 0) \
				tableau[i] = "Unknown"; \
		} \
	}while(0)


//strat de niveau 2 (appelée par un strat de niveau 1 qui est appelée par brain.c)
//Prend tous les verres et les met dans la zone de départ.
error_e K_STRAT_sub_glasses_alexis(void) {
	//GM pour Glass Move
	enum state_e {
		GM_INIT = 0,   //Initialisation: on initialise la sous-strat grab_glass: on a pris 0 verre et l'ascenseur est en bas et ouvert
		GM_FIRST_ROW,  //On va vers le milieu du terrain pour prendre les verres de la première rangée
		GM_SECOND_ROW, //On reviens vers notre zone de départ pour prendre la 2ème rangée
		GM_THIRD_ROW,  //Et on repart vers la zone ennemie pour la dernière rangée. Après ça on a pris tout nos verres (sauf peut être si evitement)
		GM_WAIT_ACT,    //Attent que les actionneurs des ascenseurs aient fini de bouger, l'état suivant est dans la variable what_to_do_after_act
		GM_GO_HOME,    //Ramene les verres a la zone de départ et les laches
		GM_EXTRACT_FROM_GLASSES,  //Recule le robot pour pouvoir faire d'autre mouvement librement sans toucher les verres ramenés
		GM_DONE,       //Les verres on été ramassé
		GM_NBSTATE   //Pas un état mais indique le nombre d'état
	};
	static enum state_e state = GM_INIT;
	static enum state_e last_state = GM_INIT;

	//Pour l'état GL_WAIT_ACT:
	static enum state_e what_to_do_after_act;

	static const bool_e first_row_avoidance_mode = NO_AVOIDANCE;    //Vers l'ennemi
	static const bool_e second_row_avoidance_mode = NO_AVOIDANCE;   //Vers chez nous
	static const bool_e third_row_avoidance_mode = NO_DODGE_AND_WAIT;  //Vers l'ennemi

	//Info sur le robot et les verres:
	//Distance centre robot - centre verre: dx = +/- 56mm (source: mise en plan Krusty sur google drive, 28 janvier 2013)
	//Départ Krusty: x = 1000mm
	// Positions: rangée.num_verre
	//Pos verre 1.1: x = 1050mm y = 900mm
	//Pos verre 1.2: x = 1050mm y = 1200mm
	//Pos verre 2.1: x = 800mm  y = 1050mm
	//Pos verre 2.2: x = 800mm  y = 1350mm
	//Pos verre 3.1: x = 550mm  y = 900mm
	//Pos verre 3.2: x = 550mm  y = 1200mm
	//Pour tourner correctement le robot, il faut avancer de 60mm mini à partir de la position "robot calibré colé au mur"
	// Soit être à y >= 180mm

	//Choix de position 1 (original)
//	const Uint8 row1_nbpos = 2;
//	displacement_t row1_pos[2] =
//				{ {{1000, COLOR_Y(950)} , FAST},
//				  {{1050, COLOR_Y(1500)}, FAST} };
//	const Uint8 row2_nbpos = 3;
//	displacement_t row2_pos[3] =
//				{ {{820, COLOR_Y(1330)}, FAST},
//				  {{750, COLOR_Y(1130)}, FAST},
//				  {{750, COLOR_Y(750)} , FAST} };
//	const Uint8 row3_nbpos = 2;
//	displacement_t row3_pos[2] =
//				{ {{500, COLOR_Y(1134)}, FAST},
//				  {{446, COLOR_Y(1220)}, SLOW} };

	//Choix 2, pas testé
	const Uint8 row1_nbpos = 7;
	displacement_t row1_pos[7] =
				{ {{1000, COLOR_Y(190)} , FAST},    //En face pour prendre verre 1.1 dans lift left
				  {{1215, COLOR_Y(800)} , FAST},
				  {{1000, COLOR_Y(1134)} , FAST},
				  {{800, COLOR_Y(1294)} , FAST},
				  {{650, COLOR_Y(1315)} , SLOW},
				  {{524, COLOR_Y(1256)} , SLOW},
				  {{431, COLOR_Y(1214)} , SLOW}};   //Verre 1.1 pris à gauche
	const Uint8 row2_nbpos = 0;
	displacement_t row2_pos[1] = {{{431, COLOR_Y(1214)} , SLOW}};  //Coord non utilisé
	const Uint8 row3_nbpos = 3;
	displacement_t row3_pos[3] =
				{ {{534, COLOR_Y(957)}, FAST},
				  {{793, COLOR_Y(996)}, FAST},
				  {{890, COLOR_Y(1000)}, SLOW} };

	switch(state) {
		case GM_INIT:
			//On reset l'état des machines à état des prise de verre pour les 2 cotés
			K_STRAT_micro_grab_glass(TRUE, ACT_LIFT_Left);
			K_STRAT_micro_grab_glass(TRUE, ACT_LIFT_Right);
			state = GM_FIRST_ROW;
			break;

		case GM_FIRST_ROW:
			state = try_going_multipoint(row1_pos, row1_nbpos,
					GM_FIRST_ROW, GM_WAIT_ACT, GM_WAIT_ACT,   //Etats suivant: in_progress, success, fail
					FORWARD, first_row_avoidance_mode);

			if(state == GM_WAIT_ACT)
			  what_to_do_after_act = GM_SECOND_ROW; //Quand les ascenseurs auront fini de bouger, on passera a l'etat SECOND_ROW
			else if(state == GM_FIRST_ROW) {
				//On tente de prendre des verres pendant le déplacement ...
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Left);
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Right);
			}
			break;

		case GM_SECOND_ROW:
			state = try_going_multipoint(row2_pos, row2_nbpos,
					GM_SECOND_ROW, GM_WAIT_ACT, GM_WAIT_ACT,   //Etats suivant: in_progress, success, fail
					FORWARD, second_row_avoidance_mode);
			if(state == GM_WAIT_ACT)
			  what_to_do_after_act = GM_THIRD_ROW; //Quand les ascenseurs auront fini de bouger, on passera a l'etat GM_DONE
			else if(state == GM_SECOND_ROW) {
				//On tente de prendre des verres pendant le déplacement ...
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Left);
				K_STRAT_micro_grab_glass(FALSE, ACT_LIFT_Right);
			}
			break;

		case GM_THIRD_ROW:
			state = try_going_multipoint(row3_pos, row3_nbpos,
					GM_THIRD_ROW, GM_WAIT_ACT, GM_WAIT_ACT,   //Etats suivant: in_progress, success, fail
					FORWARD, third_row_avoidance_mode);
			if(state == GM_WAIT_ACT)
			  what_to_do_after_act = GM_GO_HOME; //Quand les ascenseurs auront fini de bouger, on passera a l'etat GM_DONE
			else if(state == GM_THIRD_ROW) {
				//On tente de prendre des verres pendant le déplacement ...
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

		case GM_GO_HOME:
			state = try_going(1000, COLOR_Y(300), GM_GO_HOME, GM_WAIT_ACT, GM_WAIT_ACT, FORWARD, NO_AVOIDANCE);
			if(state == GM_WAIT_ACT) {
				what_to_do_after_act = GM_EXTRACT_FROM_GLASSES;   //Après que les actionneurs auront fini leur mouvement, la micro_strat sera finie
				if(!GLASS_SENSOR_LEFT)  //S'il n'y a pas de verre en bas, on descend l'ascenseur avant
					ACT_lift_translate(ACT_LIFT_Left, ACT_LIFT_TranslateDown);
				ACT_lift_plier(ACT_LIFT_Left, ACT_LIFT_PlierOpen);
				if(!GLASS_SENSOR_RIGHT)  //S'il n'y a pas de verre en bas, on descend l'ascenseur avant
					ACT_lift_translate(ACT_LIFT_Right, ACT_LIFT_TranslateDown);
				ACT_lift_plier(ACT_LIFT_Right, ACT_LIFT_PlierOpen);
			}
			break;

		case GM_EXTRACT_FROM_GLASSES:
			state = try_relative_move(200, FAST, BACKWARD, GM_EXTRACT_FROM_GLASSES, GM_DONE, GM_DONE);
			break;

		case GM_DONE:
			return END_OK;
			break;

		case GM_NBSTATE:
			break;
	}

	//On a changé d'état, on l'indique sur l'UART pour débugage
	if(state != last_state) {
		static const char* state_str[GM_NBSTATE] = {0};
		bool_e state_str_initialized = FALSE;

		if(state_str_initialized == FALSE) {
			STATE_STR_DECLARE(state_str, GM_INIT);
			STATE_STR_DECLARE(state_str, GM_FIRST_ROW);
			STATE_STR_DECLARE(state_str, GM_SECOND_ROW);
			STATE_STR_DECLARE(state_str, GM_THIRD_ROW);
			STATE_STR_DECLARE(state_str, GM_WAIT_ACT);
			STATE_STR_DECLARE(state_str, GM_GO_HOME);
			STATE_STR_DECLARE(state_str, GM_EXTRACT_FROM_GLASSES);
			STATE_STR_DECLARE(state_str, GM_DONE);
			STATE_STR_INIT_UNDECLARED(state_str, GM_NBSTATE);
			state_str_initialized = TRUE;
		}

		STATECHANGE_log(LOG_LEVEL_Debug, "K_STRAT_sub_glasses_alexis: state changed: %s(%d) -> %s(%d)\n",
			state_str[last_state], last_state,
			state_str[state], state);
	}

	return IN_PROGRESS;
}

//Cette micro strat prend des verres quand il y en a avec les ascensseurs (lift).
//Dans les autres strat, il se peut que la fin de cette sub_action ne soit pas obligatoire ...
//Renvoie END_OK si on a fini de prendre un verre ou qu'il n'y en a pas a prendre (en gros quand les actionneurs ne bouge pas)
error_e K_STRAT_micro_grab_glass(bool_e reset_state, ACT_lift_pos_t lift_pos) {
	//GL pour Glass Lift
	enum state_lift_e {
		GL_WAIT_FIRST_GLASS = 0,   //L'ascensseur est en bas, on attend un premier verre pour le prendre
		GL_WAIT_GLASS_GRABED,      //On attend d'avoir pris le verre et l'avoir monté. Si le nombre de verre pris >= 3, on passe à GL_DONE
		GL_WAIT_NEXT_GLASSES,      //On attend autre verre, on descendra l'ascensseur avant de le prendre contrairement au premier
		GL_DONE,
		GL_NBSTATE   //Pas un état mais indique le nombre d'état
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

	//choix des variables a utiliser, évite d'avoir 2 fonction avec juste des Left remplacé en Right ...
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

	//Reset de l'état, avant de prendre des verres
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
			} else return END_OK;  //Pas de verre à prendre, donc a fini
			break;
		case GL_WAIT_GLASS_GRABED:
			if(ACT_get_last_action_result(current_act) != ACT_FUNCTION_InProgress){   //Si l'ascensseur est en haut avec un verre, on est prêt pour en prendre un autre
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
				if(*current_act_grabed_glasses < 2) //Si on en a pas déjà 2, on monte l'ascensseur au plus haut, sinon on pourra pas tenir 2 verres tout en haut
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

	//On a changé d'état, on l'indique sur l'UART pour débugage
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
