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
#include "Stacks.h"
#include "Asser_functions.h"

/* 
 * La formule pour changer le numéro du noeud (parallélisme)
 */
#define COLOR_NODE(n)	((global.env.color == BLUE || n<=2) ? (n) : (n<=7 ? (n+5) : (n-5)))

#define PATHFIND_NODE_NB 14


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
    Uint16 cost;
    Uint16 heuristic;
    pathfind_node_id_t parent;
} pathfind_node_t;

/*typedef struct {
	Sint8 length;
	GEOMETRY_point_t * points;
} PATHFIND_microb_path_t;*/

/*
 * Met la position de l'adversaire à jour.
 *
 * TODO Prise en compte de la direction et de la vitesse de l'adversaire
 */
void PATHFIND_updateOpponentPosition();
/*
 * Calcule la distance approximatique entre deux points : abs(dx) + abs(dy).
 */
Uint16 PATHFIND_manhattan_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

/*
 * Calcule la distance au carré entre deux points x1,y1 et x2,y2
 */
Uint32 PATHFIND_squared_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

/*
 * Renvoie l'identifiant du noeud le plus proche du point (x,y)
 */
pathfind_node_id_t PATHFIND_closestNode(Sint16 x, Sint16 y, bool_e handleOpponent);

/*
 * Renvoie le noeud le plus proche de l'adversaire
 */
pathfind_node_id_t PATHFIND_opponent_node();

/*
 * Renvoie si l'adversaire est sur le chemin
 */
bool_e PATHFIND_is_opponent_in_path();

/*
 * Renvoie un noeud du neighbor du noeud of 
 */
pathfind_node_id_t  PATHFIND_random_neighbor(pathfind_node_id_t of, bool_e handleOpponent[NB_FOES]);

Sint16 PATHFIND_get_node_x (pathfind_node_id_t n);
Sint16 PATHFIND_get_node_y (pathfind_node_id_t n);

/*
 * Optimise les deplacements en supprimant les 
 * noeuds inutile. (si 3 noeuds 1,2 et 3 sont alignes,
 * le noeud n°2 sera supprimé de la liste des noeuds à
 * parcourir).
 */
void PATHFIND_delete_useless_node(pathfind_node_id_t from, pathfind_node_id_t to);

/*
 * Calcule le chemin le plus court pour aller au noeud to.
 * Utilise l'algorithme A*.
 *
 * TODO Si on est dans la zone verte et l'adversaire en 9, regarder si on peut commencer en 6 ou 11
 */
Uint16 PATHFIND_compute(Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to, ASSER_speed_e speed, ASSER_way_e way, bool_e handleOpponent);


#endif /* PATHFIND_H */
