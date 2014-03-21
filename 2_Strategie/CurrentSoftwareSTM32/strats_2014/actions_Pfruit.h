/*
 *	Club Robot ESEO 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_Pfruit.h
 *	Package : Carte S²/strats2014
 *	Description : Tests des actions pour rammasser les fruits
 *	Auteur : Anthony, modifié par .
 *	Version 2014/16/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_PFRUIT_H
#define ACTIONS_PFRUIT_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"
#include "../config/config_pin.h"


typedef enum{
	HORAIRE,
	TRIGO,
	CHOICE // C'est la strat manage qui va choisir elle-même le sens
}tree_way_e;

typedef enum{
	CHOICE_TREE_1,
	CHOICE_TREE_2
}tree_choice_e;

typedef enum{
	TREE_OUR,
	TREE_ADVERSARY
}tree_group_e;

typedef enum{
	NO_ONE,
	TREE_1, // Toujours celui à côté du mammouth
	TREE_2, // Toujours celui situé en face au mammouth
	ALL_TREE
}tree_sucess_e;

static tree_sucess_e strat_fruit_sucess = NO_ONE;

error_e strat_file_fruit();
error_e manage_fruit(tree_group_e group, tree_way_e parmSens);

error_e strat_ramasser_fruit_arbre1_double(tree_way_e sens);
error_e strat_ramasser_fruit_arbre2_double(tree_way_e sens);

error_e strat_ramasser_fruit_arbre1_simple(tree_choice_e tree, tree_way_e sens);
error_e strat_ramasser_fruit_arbre2_simple(tree_choice_e tree, tree_way_e sens);

#endif /* ACTIONS_PFRUIT_H */


