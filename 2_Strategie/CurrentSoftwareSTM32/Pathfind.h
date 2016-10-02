/*
 * Pathfind.h
 *
 *  Created on: 6 avr. 2009
 *      Author: cyril, modified
 */

#include "QS/QS_all.h"

#ifndef PATHFIND_H
#define PATHFIND_H

#include "Generic_functions.h"
#include "prop_functions.h"
#include "avoidance.h"

/*
 * La formule pour changer le numéro du noeud (parallélisme)
 */
#define COLOR_NODE( n)	PATHFING_get_symetric(n)



/*
 * Defines des noms des nodes
 */
#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define B1 4
#define B2 5
#define B3 6
#define C1 7
#define C2 8
#define C3 9
#define M1 10
#define M2 11
#define M3 12
#define W1 13
#define W2 14
#define W3 15
#define Y1 16
#define Y2 17
#define Y3 18
#define Z0 19
#define Z1 20
#define Z2 21
#define Z3 22
#define NOT_IN_NODE	23

#define PATHFIND_NODE_NB NOT_IN_NODE

/*
 * Modifie par Alexis
 * Distance a laquelle on s'estime proche du noeud de depart
 * Si on est proche du noeud de départ, le Pathfinder ne nous
 * fait pas passer par le noeud de départ (sauf si il s'agit
 * egalement du noeud de destination) (unité : mm)
 */
#define NODE_PROXIMITY_DISTANCE		175

#define PATHFIND_OPPONENT_COST 			65535
#define PATHFIND_NEAR_OPPONENT_COST 	2000

/*
 * Cout pour passer d'un noeud a l'autre (distance en mm)
 */
#define PATHFIND_STEP_COST 300

/*
 * Gestion des listes de noeuds. Une liste est représentée par un Uint32,
 * tous les bits à 1 correspondent aux noeuds présents dans la liste.
 * Utilisé pour la liste des voisins, la liste ouverte et la liste fermée
 */
#define PATHFIND_TST_NODE_IN(nodeId, nodeList)	((nodeList) &  (((Uint32)1) << nodeId))
#define PATHFIND_SET_NODE_IN(nodeId, nodeList)	((nodeList) |= (((Uint32)1) << (nodeId)))//BIT_SET(nodeList, nodeId)
#define PATHFIND_CLR_NODE_IN(nodeId, nodeList)	((nodeList) &= ~(((Uint32)1) << (nodeId)))//BIT_CLR(nodeList, nodeId)

typedef Uint8  pathfind_node_id_t;
/*
 * Modifie par Alexis (codé a present sur 32 bits)
 * Représente une liste de noeuds sous forme de masque.
 * ex. 0b100000000110 => les noeuds 1, 2 et 12 sont dans la liste.
 */
typedef Uint32 pathfind_node_list_t;

typedef struct {

	/*
	 * Localisation et voisins du noeud
	 */
	Sint16 x;
	Sint16 y;
	pathfind_node_list_t neighbors;

	/*
	 * Variables utilisées dans l'algo.
	 */
	Uint16 length_path; // Soit G la distance parcourue depuis le point de départ pour arriver au node courant
	Uint16 dist_end;	// Soit H la distance à vol d'oiseau entre le node courant et le node d'arrivée
	Uint32 total_cost;	// Soit F la somme de G et H

	pathfind_node_id_t parent;
	Uint16 nb_nodes;

	// Variable ancienne
	//Uint16 cost;
} pathfind_node_t;

// Fonction a appelée au début de match afin de faire la configuration de pathfind spécifique à la couleur
void PATHFIND_MAJ_COLOR();

// Fonction a appelée si l'on veut se déplacer sur un node
Uint8 PATHFIND_try_going(pathfind_node_id_t node_wanted, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition);

/*	Fonction de sélection du node le plus proche
 *	Sélectionne le node le plus proche qui présente le moins de différence d'angle
 *	entre l'angle à faire et l'angle nous séparant du point d'arrivé
 */
pathfind_node_id_t PATHFIND_closestNodeToEnd(Sint16 x, Sint16 y, Uint32 filteredNodes, Sint16 final_x, Sint16 final_y);

/*	Ancien fonction de sélection du node le plus proche
 *	Pas de prise en compte du point d'arrivé
 *	Sélection un point de départ non optimisé
 */
pathfind_node_id_t PATHFIND_closestNode(Sint16 x, Sint16 y, Uint32 filteredNodes);


/*	Fonction de sélection du node le plus proche
*	En fonction des positions adversaires afin de trouver le node le plus sécurisé
*/
pathfind_node_id_t PATHFIND_closestNodeWithoutAdversary(Sint16 x, Sint16 y, Uint32 filteredNodes);

// Compare deux node et revoit la plus pres
pathfind_node_id_t min_node_dist(pathfind_node_id_t n1,pathfind_node_id_t n2);

// Retourne la longueur du chemin effectué par le pathfind
Sint16 path_length(pathfind_node_id_t* tab_node, Uint8 nb_node);

// Calcule la distance approximatique entre deux points : absolute(dx) + absolute(dy).
Uint16 PATHFIND_manhattan_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

// Calcule la distance au carré entre deux points x1,y1 et x2,y2
Uint32 PATHFIND_squared_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

// Retourne le x d'un node
Sint16 PATHFIND_get_node_x (pathfind_node_id_t n);

// Retourne le y d'un node
Sint16 PATHFIND_get_node_y (pathfind_node_id_t n);

// Retourne le distance entre deux node
Sint16 PATHFIND_dist_node_to_node(pathfind_node_id_t n1, pathfind_node_id_t n2);

// Calcule le point symétrique au point rentré n
Uint16 PATHFING_get_symetric(pathfind_node_id_t n);

#endif /* PATHFIND_H */
