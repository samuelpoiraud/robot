





#include "../QS/QS_all.h"

#ifndef ACTIONS_PFRUIT_H
#define ACTIONS_PFRUIT_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"
#include "../config/config_pin.h"

#define LARGEUR_LABIUM	250

typedef enum{
	HORAIRE,
	TRIGO
}tree_way_e;

static bool_e strat_fruit_sucess = FALSE;

error_e strat_file_fruit();
error_e strat_ramasser_fruit_arbre1_double(tree_way_e sens);
error_e strat_ramasser_fruit_arbre2_double(tree_way_e sens);

#endif /* ACTIONS_PFRUIT_H */


