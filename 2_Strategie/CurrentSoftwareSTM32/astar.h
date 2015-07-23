/*
 *	Club Robot ESEO 2015 - 2016
 *
 *
 *	Fichier : astar.c
 *	Package : Stratégie
 *	Description : 	Fonctions de génération des trajectoires
 *					par le biais d'un algo de type A*
 *	Auteurs : Valentin BESNARD
 *	Version 20150001
 */

#define _ASTAR_H_
#ifdef _ASTAR_H_

#include "avoidance.h"
#include "QS/QS_measure.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include <stdarg.h>
//#include <stdlib.h>
#include "state_machine_helper.h"

//--------------------------------------------------- Définitions des macros ----------------------------------------------------------

	//Nombre maximum de noeud voisins pour un noeud donné
	#define NB_MAX_NEIGHBORS 20

	//Nombre maximum de polygônes
	#define NB_MAX_SUMMITS_POLYGONS 20

	//Nombre maximum de polygônes
	#define NB_MAX_POLYGONS 20

	//Nombre maximum de nodes dans une liste
	#define NB_MAX_NODES 100

	//Dimension du terrain
	#define PLAYGROUND_WIDTH 2000
	#define PLAYGROUND_HEIGHT 3000

	//Largeur du robot
	#define ROBOT_WIDTH ((QS_WHO_AM_I_get()== BIG_ROBOT)? BIG_ROBOT_WIDTH:SMALL_ROBOT_WIDTH)

	//Rayon (ou demie-largeur) du robot adverse par défaut
	//En considérant que c'est un hexagone donc 6 côtés et que le périmètre déployé est de 1500
	#define DEFAULT_FOE_RADIUS  600

	//Marge entre le centre du robot et un obstacle
	#define MARGIN_TO_OBSTACLE (ROBOT_WIDTH/2 + 50)

	//Marge entre le centre du robot et la bordure (Les nodes dans cette zone sont supprimés)
	#define MARGIN_TO_BORDER (MARGIN_TO_OBSTACLE - 10)

	//Cout maximal
	#define MAX_COST 65535

	//Pour spécifier qu'un noeud (node) n'a pas de polygonId
	#define NO_ID 255

	//Nombre de déplacements maximal
	#define NB_MAX_DISPLACEMENTS  20

	//Nombre d'essais consécutifs avec du DODGE en évitement
	#define NB_TRY_WHEN_DODGE 6


//------------------------------------------------- Définitions des types structrés ---------------------------------------------------
//Type structuré du node entré par l'utilisateur
typedef struct{
	Uint16 x, y;
	bool_e curve;
}astar_user_node_t;

//Type structuré coût d'un noeud
typedef struct{
	Uint16 total;  //Cout total = somme du cout (départ -> node parent) + (step) + (heuristic)
	Uint16 heuristic;  //Cout de déplacement du node courant vers le node d'arrivée
	Uint16 step;  //Cout de déplacement du node parent vers le node courant
}astar_node_cost_t;

//Type structuré noeud (ou node)
struct astar_node{
	//Ses id
	Uint8 id; //Son id personnel
	Uint8 polygonId; //L'id de son polygon

	//Coordonnées du point
	GEOMETRY_point_t pos;

	//Ses propriétés
	bool_e enable;
	bool_e curve;
	astar_node_cost_t cost;
	struct astar_node *parent;
	struct astar_node *neighbors[NB_MAX_NEIGHBORS];
	Uint8 nbNeighbors;
};

//Type pointeur sur node
typedef struct astar_node *astar_ptr_node_t;

//Type structuré polygone
typedef struct{
	Uint8 id;
	struct astar_node summits[NB_MAX_SUMMITS_POLYGONS];
	Uint8 nbSummits;
	bool_e enable;
}astar_polygon_t;

//Type structuré liste de polygones
typedef struct{
	astar_polygon_t polygons[NB_MAX_POLYGONS];
	Uint8 nbPolygons;
}astar_polygon_list_t;

//Type structuré liste de nodes (pour l'opened_list et la closed_list)
typedef struct{
	astar_ptr_node_t list[NB_MAX_NODES];
	Uint8 nbNodes;
}astar_list_t;

//Type structuré path
typedef struct{
	astar_list_t list;
	struct astar_node from, destination;
}astar_path_t;

//----------------------------------------------- Fonctions pour l'algo A* ---------------------------------------------
void ASTAR_create_foe_polygon(Uint8 *currentId, Uint16 foeRadius);
void ASTAR_create_element_polygon(Uint8 *currentId, Uint8 nbSummits,...);
bool_e ASTAR_node_enable(astar_ptr_node_t nodeTested, bool_e withPolygons);
bool_e ASTAR_point_out_of_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested);
void ASTAR_generate_polygon_list(Uint8 *currentNodeId, Uint16 foeRadius);
void ASTAR_generate_graph(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint8 *currentNodeId);
void ASTAR_pathfind(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint16 foeRadius);
void ASTAR_link_nodes_on_path(astar_ptr_node_t from, astar_ptr_node_t destination, Uint8 recursivityOrder);
bool_e ASTAR_node_is_visible(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);
bool_e ASTAR_node_is_reachable(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);
void ASTAR_update_cost(Uint16 minimal_cost, astar_ptr_node_t from, astar_ptr_node_t destination);
void ASTAR_make_the_path(astar_path_t *path);
void ASTAR_make_displacements(astar_path_t path, displacement_curve_t displacements[], Uint8 *nbDisplacements, PROP_speed_e speed);
Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);

//------------------------------------------------- Fonctions annexes -------------------------------------------------
void ASTAR_add_neighbor_to_node(astar_ptr_node_t node, astar_ptr_node_t neighbor);
void ASTAR_clean_list(astar_list_t *list);
bool_e ASTAR_list_is_empty(astar_list_t list);
void ASTAR_add_node_to_list(astar_ptr_node_t node, astar_list_t *list);
void ASTAR_delete_node_to_list(astar_ptr_node_t node, astar_list_t *list);
GEOMETRY_point_t ASTAR_intersection_is(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2);
bool_e ASTAR_neighbors_intersection(astar_ptr_node_t from, astar_ptr_node_t neighbor);
Uint16 ASTAR_pathfind_cost(astar_ptr_node_t start_node, astar_ptr_node_t end_node);
bool_e ASTAR_is_in_list(astar_ptr_node_t node, astar_list_t list);


//------------------------------------------------- Fonctions d'affichage ---------------------------------------------
void print_polygon_list();
void print_path(astar_path_t path);
void print_list(astar_list_t list);
void print_opened_list();
void print_closed_list();
void ASTAR_print_neighbors();
void ASTAR_print_polygon_list_details();

#else
	//L'algorithme A* n'est pas activé.
#endif
