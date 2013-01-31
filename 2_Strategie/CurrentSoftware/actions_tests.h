/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef ACTIONS_TESTS_H
#define ACTIONS_TESTS_H
	
#include "actions.h"
#include "Asser_functions.h"
#include "act_functions.h"

//Test message can avec carte actionneur
void TEST_STRAT_act();

void TEST_STRAT_go();

error_e TEST_STRAT_go_score();

/**
* permet de sortir et d'aller manuellement sur le noeud 4
*
*/
error_e TEST_STRAT_leave_home();


#ifdef MODE_HOMOLOGATION
/**
* marque les points d'une bouteille et parcours ensuite les noeuds pour l'évitement
*
*/
void TEST_STRAT_homolagation();
#endif

/**
* Fonction permettant de tester l'évitement en faisant le tour du terrain
*
*/
void TEST_STRAT_evitement();

/**
* Simule un match pour les JPO
*
*/
void TEST_STRAT_qualif();

/**
* Permet d'aller taper dans les bouteilles
*
*/
error_e TEST_STRAT_hit_all_bottles();

/**
* Permet d'aller taper dans la bouteille la plus proche de la cale
*
*/
error_e TEST_STRAT_hit_bottle_deck();

/**
* Permet d'aller taper dans la bouteille la plus proche du milieu
*
*/
error_e TEST_STRAT_hit_bottle_middle();

/*permet de marquer les points du lingot à l'entré de la cale
*
*/
error_e TEST_STRAT_bullion();

/**
* Va chercher la carte
*
*/
error_e TEST_STRAT_take_map();

/**
* Récolte les cd autour d'un totem (coef a régler) fonction non testée
*
*/
error_e TEST_STRAT_score_piece_north();

/**
* Prend le lingot du totem le plus proche du départ, coté carte
*
*/
error_e TEST_take_bullion_first_totem_north(void);

/*permet de prendre le lingot dans le totem le plus proche de la case et le plus proche de la carte (pour les deux couleurs)
*
*/
error_e TEST_STRAT_first_totem_north();

/*permet de prendre le lingot dans le totem le plus loin de la case et le plus proche de la carte (pour les deux couleurs)
*
*/
error_e TEST_STRAT_second_totem_north();

/*permet de prendre le lingot dans le totem le plus proche de la case et le plus loin de la carte (pour les deux couleurs)
*
*/
error_e TEST_STRAT_first_totem_south();

/*permet de prendre le lingot dans le totem le plus loin de la case et le plus loin de la carte (pour les deux couleurs)
*
*/
error_e TEST_STRAT_second_totem_south();

/*permet de passer près de tout les totems
*
*/
void TEST_STRAT_all_totem();
/**
* TEST_STRAT_all_node
*
*/
void TEST_STRAT_all_node();

/**
* TEST_STRAT_all_asser_functions
*
*/
void TEST_STRAT_all_asser_functions();

/**
* TEST_STRAT_goto_node
*
*/
void TEST_STRAT_goto_node();

void TEST_test (void);

error_e TEST_STRAT_score_piece_north(void);

error_e TEST_STRAT_score_piece_first_north(void);

void TEST_STRAT_take_point_first_totem(void);

#ifdef ACTIONS_TEST_C
 	#include "telemeter.h"
#endif /* def ACTION_TEST_C */

#endif /* ACTIONS_TESTS_H */
