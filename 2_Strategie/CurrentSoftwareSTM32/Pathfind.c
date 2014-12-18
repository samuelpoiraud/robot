   /*
 *	Club Robot ESEO 2011 - 2012 - 2015
 *	Shark & Fish / Pierre & Guy / Holly & Wood
 *
 *	Fichier : Pathfind.c
 *	Package : Carte Principale
 *	Description : définition des noeuds et des fonctions de pathfind
 *	Auteur : Louis-Marie, Vincent, Antoine, Nirgal
 *	Version 201312
 */

#include "Pathfind.h"
#include "config_use.h"
#include "../state_machine_helper.h"
#include "../Supervision/SD/SD.h"
#include "avoidance.h"
#include <math.h>

#define LOG_PREFIX ""
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_PATHFIND
#include "QS/QS_outputlog.h"

#define MANHATTAN_DIST_NODE_BLOQUED_BY_ADVERSARY	400		//Distance manhattan entre un advesaire et les noeuds dont il bloque l'accès
#define DISTANCE_CONSIDERE_ADVERSARY				1200	//Distance entre nous et l'adversaire pour qu'il soit pris en compte (s'il est loin, on le néglige.. en espérant qu'il bouge d'ici à notre arrivée)
#define	TIME_CONSIDERE_ADVERSARY					500		//[ms] temps au delà duquel un adversaire mis à jour n'est plus considéré
#define NB_TRY_WHEN_DODGE							3		//Nombre de tentatives d'évitement (recalcul de chemin..)
#define IGNORE_FIRST_POINT_DISTANCE					150		//Distance du noeud en dessous de laquelle on ne s'y rend pas et on attaque directement le voisin
#define DISTANCE_CHANGING_FIRST_NODE_FOE			600		//Distance à laquelle on décide de choisir le node de départ le plus éloigné de tout les adversaires

#ifndef USE_POLYGON

static error_e PATHFIND_compute(displacement_curve_t * displacements, Uint8 * p_nb_displacements, Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to);
static Uint16 Pathfind_cost(pathfind_node_id_t from, pathfind_node_id_t to, bool_e handleOpponent);

// Ensemble des nodes présent sur le terrain avec leurs coordonnées et leurs voisins
static pathfind_node_t nodes[PATHFIND_NODE_NB+1] =
{
	//Colonne 1 coté Rouge [A]
	(pathfind_node_t){ 400, 500,	/* neighbors : */(1<<1)|(1<<4)},															//[A0] 0
	(pathfind_node_t){ 800, 600,	/* neighbors : */(1<<0)|(1<<4)|(1<<5)|(1<<2)},												//[A1] 1
	(pathfind_node_t){ 1200, 600,	/* neighbors : */(1<<1)|(1<<4)|(1<<5)|(1<<6)|(1<<3)},										//[A2] 2
	(pathfind_node_t){ 1600, 500,	/* neighbors : */(1<<2)|(1<<5)|(1<<6)},														//[A3] 3

	//Colonne 2 coté rouge [B]
	(pathfind_node_t){ 700, 800,	/* neighbors : */(1<<0)|(1<<1)|(1<<2)|(1<<5)|(1<<8)|(1<<7)},								//[B1] 4
	(pathfind_node_t){ 1150, 800,	/* neighbors : */(1<<1)|(1<<2)|(1<<3)|(1<<6)|(1<<9)|(1<<8)|(1<<7)|(1<<4)},					//[B2] 5
	(pathfind_node_t){ 1550, 800,	/* neighbors : */(1<<3)|(1<<2)|(1<<5)|(1<<8)|(1<<9)},										//[B3] 6

	//Colonne 3 coté rouge [C]
	(pathfind_node_t){ 850, 1150,	/* neighbors : */(1<<4)|(1<<5)|(1<<8)|(1<<11)|(1<<10)},										//[C1] 7
	(pathfind_node_t){ 1200, 1150,	/* neighbors : */(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<9)|(1<<10)|(1<<11)|(1<<12)|},		//[C2] 8
	(pathfind_node_t){ 1550, 1150,	/* neighbors : */(1<<6)|(1<<5)|(1<<8)|(1<<11)|(1<<12)},										//[C3] 9

	//Colonne 4 milieu [M]
	(pathfind_node_t){ 850, 1500,	/* neighbors : */(1<<7)|(1<<8)|(1<<11)|(1<<14)|(1<<13)},									//[M1] 10
	(pathfind_node_t){ 1200, 1500,	/* neighbors : */(1<<7)|(1<<8)|(1<<9)|(1<<12)|(1<<15)|(1<<14)|(1<<13)|(1<<10)},				//[M2] 11
	(pathfind_node_t){ 1550, 1500,	/* neighbors : */(1<<9)|(1<<8)|(1<<11)|(1<<14)|(1<<15)},									//[M3] 12

	//Colonne 5 coté jaune [W]
	(pathfind_node_t){ 850, 1850,	/* neighbors : */(1<<10)|(1<<11)|(1<<14)|(1<<17)|(1<<16)},									//[W1] 13
	(pathfind_node_t){ 1200, 1850,	/* neighbors : */(1<<10)|(1<<11)|(1<<12)|(1<<15)|(1<<18)|(1<<17)|(1<<16)|(1<<13)|(1<<22)},	//[W2] 14
	(pathfind_node_t){ 1550, 1850,	/* neighbors : */(1<<12)|(1<<11)|(1<<14)|(1<<17)|(1<<18)},									//[W3] 15

	//Colonne 6 coté jaune [Y]
	(pathfind_node_t){ 700, 2200,	/* neighbors : */(1<<13)|(1<<14)|(1<<17)|(1<<21)|(1<<20)|(1<<19)},							//[Y1] 16
	(pathfind_node_t){ 1150, 2200,	/* neighbors : */(1<<13)|(1<<14)|(1<<15)|(1<<18)|(1<<22)|(1<<21)|(1<<20)|(1<<16)},			//[Y2] 17
	(pathfind_node_t){ 1550, 2200,	/* neighbors : */(1<<15)|(1<<14)|(1<<17)|(1<<21)|(1<<22)},									//[Y3] 18

	//Colonne 7 coté Jaune [Z]
	(pathfind_node_t){ 400, 2500,	/* neighbors : */(1<<16)|(1<<20)},															//[Z0] 19
	(pathfind_node_t){ 800, 2400,	/* neighbors : */(1<<19)|(1<<16)|(1<<17)|(1<<21)},											//[Z1] 20
	(pathfind_node_t){ 1200, 2400,	/* neighbors : */(1<<20)|(1<<16)|(1<<17)|(1<<18)|(1<<22)},									//[Z2] 21
	(pathfind_node_t){ 1600, 2500,	/* neighbors : */(1<<21)|(1<<17)|(1<<18)},													//[Z3] 22

	(pathfind_node_t){ 0, 0,		/* neighbors : */0} //[NOT_IN_NODE] 23 (invalid)
};

/*
 * Ensemble des droit de courbe entre les doublets de nodes
 * Notation :
 *		node_curve[x]				->		node de départ
 *		contenue de node_curve[x]	->		listes des voisins (décallage de bit du numéro du node)
 *		valeur de node_curve[x]		->		1 si l'on donne le droit à une courbe après avoir franchi le node voisin
 *
 * Exemple:
 *	numéro : node
 *	x : obstacle
 *
 *		1       2
 *                      3
 * node_curve[1] = (1 << 2);	J'ai le droit de faire une courbe après le point 2 en venant du point 1 parce qu'il n'y a pas d'obstacle
 *
 *					  xxxxxx
 *		1       2     xxxxxx
 *                      3
 * node_curve[1] = (0 << 2);	Je n'ai pas le droit de faire une courbe après le point 2 en venant du point 1 parce qu'il y a un obstacle
 *
 *!!!!!!!!!!!!!Un oublie de complétion de ce tableau engendra une interdiction de faire courbe sur les nodes non remplis!!!!!!!!!!!
 */
static Uint32 node_curve[PATHFIND_NODE_NB+1] =
{
	//Colonne 1 coté Rouge [A]
	(1<<1)|(1<<4),																//[A0] 0
	(0<<0)|(1<<4)|(1<<5)|(1<<2),												//[A1] 1
	(1<<1)|(1<<4)|(1<<5)|(1<<6)|(0<<3),											//[A2] 2
	(1<<2)|(1<<5)|(1<<6),														//[A3] 3

	//Colonne 2 coté rouge [B]
	(0<<0)|(0<<1)|(1<<2)|(1<<5)|(1<<8)|(1<<7),									//[B1] 4
	(1<<1)|(0<<2)|(0<<3)|(0<<6)|(1<<9)|(1<<8)|(1<<7)|(1<<4),					//[B2] 5
	(0<<3)|(1<<2)|(1<<5)|(1<<8)|(1<<9),											//[B3] 6

	//Colonne 3 coté rouge [C]
	(0<<4)|(1<<5)|(1<<8)|(1<<11)|(1<<10),										//[C1] 7
	(0<<3)|(1<<4)|(1<<5)|(1<<6)|(0<<9)|(1<<12)|(1<<11)|(1<<10)|(0<<7),			//[C2] 8
	(1<<6)|(1<<5)|(1<<8)|(1<<11)|(1<<12),										//[C3] 9

	//Colonne 4 milieu [M]
	(1<<7)|(1<<8)|(1<<11)|(1<<14)|(1<<13),										//[M1] 10
	(1<<7)|(1<<8)|(1<<9)|(0<<12)|(1<<15)|(1<<14)|(1<<13)|(0<<10),				//[M2] 11
	(1<<9)|(1<<8)|(1<<11)|(1<<14)|(1<<15),										//[M3] 12

	//Colonne 5 coté jaune [W]
	(1<<10)|(1<<11)|(1<<14)|(1<<17)|(0<<16),									//[W1] 13
	(1<<10)|(1<<11)|(1<<12)|(0<<15)|(1<<18)|(1<<17)|(1<<16)|(0<<13)|(0<<22),	//[W2] 14
	(1<<12)|(1<<11)|(1<<14)|(1<<17)|(1<<18),									//[W3] 15

	//Colonne 6 coté jaune [Y]
	(1<<13)|(1<<14)|(1<<17)|(1<<21)|(0<<20)|(0<<19),							//[Y1] 16
	(1<<13)|(1<<14)|(1<<15)|(0<<18)|(0<<22)|(0<<21)|(1<<20)|(1<<16),			//[Y2] 17
	(1<<15)|(1<<14)|(1<<17)|(1<<21)|(0<<22),									//[Y3] 18

	//Colonne 7 coté Jaune [Z]
	(1<<16)|(1<<20),															//[Z0] 19
	(0<<19)|(0<<16)|(1<<17)|(1<<21),											//[Z1] 20
	(1<<20)|(0<<16)|(1<<17)|(1<<18)|(0<<22),									//[Z2] 21
	(1<<21)|(1<<17)|(1<<18),													//[Z3] 22
	0
};

//Variable de fonctionnement du pathfind
static pathfind_node_list_t openList;
static pathfind_node_list_t closedList;


//////////////////////////////////////////////////////////////////
//------------------Fonctions principales-----------------------//
//////////////////////////////////////////////////////////////////

/**
 * @brief PATHFIND_closestNode
 *		Ancien fonction de sélection du node le plus proche
 *		Pas de prise en compte du point d'arrivé
 *		Sélection un point de départ non optimisé
 *
 * @param x	: notre position en x
 * @param y : notre position en y
 * @param filteredNodes : liste des nodes filtrés (nodes "désactivés")
 *
 * @return l'id du node le plus proche
 */
pathfind_node_id_t PATHFIND_closestNode(Sint16 x, Sint16 y, Uint32 filteredNodes)
{
	Uint16 dist, minDist = 65535;
	pathfind_node_id_t n, closestNode = NOT_IN_NODE;

	for (n = 0; n < PATHFIND_NODE_NB; n++)
	{
		if (PATHFIND_TST_NODE_IN(n, filteredNodes) == FALSE)
		{
			dist = PATHFIND_manhattan_dist(x, y, nodes[n].x, nodes[n].y);
			if (dist < minDist)
			{
				minDist = dist;
				closestNode = n;
				/* Si on est a moins de 200mm d'un noeud, c'est le plus proche */
/*	TODO : rétablir les lignes qui suivent
				if (dist <= (PATHFIND_STEP_COST/2)) {
				if (dist <= NODE_PROXIMITY_DISTANCE) {
					break;
				}*/
			}
		}
	}
	return closestNode;
}

/**
 * @brief PATHFIND_closestNodeToEnd
 *		Fonction de sélection du node le plus proche
 *		Sélectionne le node le plus proche qui présente le moins de différence d'angle
 *		entre l'angle à faire et l'angle nous séparant du point d'arrivé
 *
 * @param x	: notre position en x
 * @param y : notre position en y
 * @param filteredNodes : liste des nodes filtrés (nodes "désactivés")
 * @param final_x : destination voulue en x
 * @param final_y : destination voulue en y
 *
 * @return l'id du node le plus proche
 */
pathfind_node_id_t PATHFIND_closestNodeToEnd(Sint16 x, Sint16 y, Uint32 filteredNodes, Sint16 final_x, Sint16 final_y)
{
	Uint8 i;
	Uint16 dist, minDist;
	pathfind_node_id_t n, closestNode, closestNodes[4] = {NOT_IN_NODE, NOT_IN_NODE, NOT_IN_NODE, NOT_IN_NODE};
	GEOMETRY_vector_t vecteur[5];
	Sint16 angle_vector[4], minAngle;


	// On trouve les 4 plus proches node
	for(i=0; i<4; i++){
		closestNode = NOT_IN_NODE;
		minDist = 65535;

		for (n = 0; n < PATHFIND_NODE_NB; n++){
			if (PATHFIND_TST_NODE_IN(n, filteredNodes) == FALSE){
				dist = PATHFIND_manhattan_dist(x, y, nodes[n].x, nodes[n].y);
				if (dist < minDist){
					minDist = dist;
					closestNode = n;
				}
			}
		}

		if(i == 0 && closestNode == NOT_IN_NODE)
			return closestNode;

		closestNodes[i] = closestNode;
		filteredNodes |= ((Uint32)1) << closestNode;
	}

	// On retire les nodes qui ne sont pas voisin avec le node le plus proche
	for(i=1;i<4;i++){
		if(closestNodes[i] == NOT_IN_NODE || ((((Uint32)1) << closestNodes[i]) & nodes[closestNodes[0]].neighbors) == 0)
			closestNodes[i] = NOT_IN_NODE;
	}

	// On calcule le vecteur entre chaque node et nous
	for(i=0;i<4;i++){
		if(closestNodes[i] != NOT_IN_NODE){
			vecteur[i].x = nodes[closestNodes[i]].x-x;
			vecteur[i].y = nodes[closestNodes[i]].y-y;
		}else
			vecteur[i] = (GEOMETRY_vector_t){0,0};
	}

	// On calcule le vecteur entre nous et le node final
	vecteur[4].x = final_x-x;
	vecteur[4].y = final_y-y;

	// On calcule l'angle entre chaque vecteur (node -> nous) et le vecteur final (nous -> fin)
	for(i=0;i<4;i++){
		if(closestNodes[i] != NOT_IN_NODE)
			angle_vector[i] = (Sint16)(acos((float)(vecteur[i].x*vecteur[4].x + vecteur[i].y*vecteur[4].y) /
									(sqrt(SQUARE((float)vecteur[i].x) + SQUARE((float)vecteur[i].y))
										* sqrt(SQUARE((float)vecteur[4].x) + SQUARE((float)vecteur[4].y))))*4096);
	}

	// On cherche qui a le plus petit angle entre chaque vecteur (node -> nous) et le vecteur final (nous -> fin)
	closestNode = closestNodes[0];
	minAngle = absolute(angle_vector[0]);
	for(i=1;i<4;i++){
		if(closestNodes[i] != NOT_IN_NODE && minAngle > absolute(angle_vector[i])){
			minAngle = absolute(angle_vector[i]);
			closestNode = closestNodes[i];
		}
	}

	// On retourne le node le plus optimisé
	return closestNode;
}

/**
 * @brief PATHFIND_closestNodeWithoutAdversary
 *		Fonction de sélection du node le plus proche
 *		En fonction des positions adversaires afin de trouver le node le plus sécurisé
 *
 * @param x	: notre position en x
 * @param y : notre position en y
 * @param filteredNodes : liste des nodes filtrés (nodes "désactivés")
 *
 * @return l'id du node le plus proche
 */
pathfind_node_id_t PATHFIND_closestNodeWithoutAdversary(Sint16 x, Sint16 y, Uint32 filteredNodes){
	Uint8 i, indexMinValue;
	Uint16 dist, minValue, minDist;
	pathfind_node_id_t n, closestNode, closestNodes[4] = {NOT_IN_NODE, NOT_IN_NODE, NOT_IN_NODE, NOT_IN_NODE};
	Uint16 nodeValue[4] = {0, 0, 0, 0};

	// On trouve les 4 plus proches node
	for(i=0; i<4; i++){
		closestNode = NOT_IN_NODE;
		minDist = 65535;

		for (n = 0; n < PATHFIND_NODE_NB; n++){
			if (PATHFIND_TST_NODE_IN(n, filteredNodes) == FALSE){
				dist = PATHFIND_manhattan_dist(x, y, nodes[n].x, nodes[n].y);
				if (dist < minDist){
					minDist = dist;
					closestNode = n;
				}
			}
		}

		if(i == 0 && closestNode == NOT_IN_NODE)
			return closestNode;

		closestNodes[i] = closestNode;
		filteredNodes |= ((Uint32)1) << closestNode;
	}

	// On retire les nodes qui ne sont pas voisin avec le node le plus proche
	for(i=1;i<4;i++){
		if(closestNodes[i] == NOT_IN_NODE || ((((Uint32)1) << closestNodes[i]) & nodes[closestNodes[0]].neighbors) == 0)
			closestNodes[i] = NOT_IN_NODE;
	}

	// On calcul la valeur des nodes choisis avec la distance des adversaires
	for(i=0;i<MAX_NB_FOES;i++){
		for(n = 0; n < 4; n++){
			if(closestNodes[i] != NOT_IN_NODE){
				dist = dist_point_to_point(nodes[n].x, nodes[n].y, global.env.foe[i].x, global.env.foe[i].y);
				if(dist < DISTANCE_CONSIDERE_ADVERSARY)
					nodeValue[n] = dist_point_to_point(nodes[n].x, nodes[n].y, global.env.foe[i].x, global.env.foe[i].y);
			}
		}
	}

	// On choisie le node le plus éloignés des adversaires
	indexMinValue = NOT_IN_NODE;
	minValue = 0xFFFF;
	for(i=0;i<MAX_NB_FOES;i++){
		for(n = 0; n < 4; n++){
			if(closestNodes[i] != NOT_IN_NODE){
				if(nodeValue[n] < minValue){
					minValue = dist;
					indexMinValue = n;
				}
			}
		}
	}

	// On retourne le node le plus sécurisé
	return indexMinValue;
}



/**
 * @brief Pathfind_cost
 *		Estimation de la distance necessaire pour aller d'un noeud a l'autre,
 *		plus cette distance est proche de la realite, plus l'algorithme sera
 *		rapide.
 *
 * @todo précalculer toutes les possibilités pour avoir un heuristic exact ?
 *
 * @param from : id du node de départ
 * @param to : id du node d'arrivée
 * @param handleOpponent : booléen activant la prise en compte des ennemies dans le calcul (le node est plus "chère" si un adversaire est à côté)
 *
 * @return le coût du noeud
 */
static Uint16 Pathfind_cost(pathfind_node_id_t from, pathfind_node_id_t to, bool_e handleOpponent)
{
	Uint8 i;
	Uint32 dist;
	/* On se base sur un manhattan */
	Uint16 cost;
	cost = dist_point_to_point(nodes[from].x, nodes[from].y, nodes[to].x, nodes[to].y);

	if (handleOpponent)
	{
	/*
	 * Si l'adversaire est à moins de 90 cm du noeud, on
	 * ajoute une pénalité importante, inversement proportionnelle à la distance adversaire-noeud
	 */

		for(i=0; i<MAX_NB_FOES; i++)
		{
			if(global.env.foe[i].enable	&& global.env.foe[i].dist < DISTANCE_CONSIDERE_ADVERSARY)	//Si l'adversaire est proche de nous...
			{
				dist = dist_point_to_point(nodes[from].x, nodes[from].y, global.env.foe[i].x, global.env.foe[i].y);
				if (dist < ((Uint32)900))	//Si l'adversaire proche de nous est proche du noeud en question... on allourdi sérieusement le coût de ce noeud.
					cost += (((Uint32)900) - dist);
			}
		}
	}
	return cost;
}

/**
 * @brief PATHFIND_compute
 *		Fonction principale du pathfind qui génére la trajectoire
 *		Utilise l'algorithme A*
 *
 * @param displacements : tableau de displacement_curve_t à compléter avec les coordonnées des nodes sélectionnés
 * @param p_nb_displacements : nombre de déplacement ajouté dans le tableau, ou 0 si pas de chemin possible
 * @param xFrom : notre position en x
 * @param yFrom : notre position en y
 * @param to : id du node de destination
 *
 * @return une erreur de type error_e
 */
static error_e PATHFIND_compute(displacement_curve_t * displacements, Uint8 * p_nb_displacements, Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to)
{
	pathfind_node_id_t from, n, current, from_without_adversaries, suivant;
	pathfind_node_list_t adversaries_nodes;
	Uint16 lengthPath, distEnd, closestAdv = 0xFFFF, dist;
	Uint32 minTotalCost;
	Uint8 i;
	Uint8 nb_displacements = 0;
	bool_e first_node_deleted = FALSE;
	//On identifie les noeuds placés à moins de 500mm manhattan d'un adversaire.
	adversaries_nodes = 0;
	for(i=0;i<MAX_NB_FOES;i++)
	{
		if(global.env.foe[i].enable && global.env.foe[i].dist < DISTANCE_CONSIDERE_ADVERSARY)	//Pour chaque adversaire situé proche de nous...
		{
			for(n = 0; n < PATHFIND_NODE_NB; n++)	//Pour chaque noeud
			{	//Si l'adversaire en question est proche du noeud : on ajoute le noeud dans la liste des noeuds innaccessibles.

				if(PATHFIND_manhattan_dist(nodes[n].x, nodes[n].y, global.env.foe[i].x, global.env.foe[i].y)<MANHATTAN_DIST_NODE_BLOQUED_BY_ADVERSARY)
				{
					PATHFIND_SET_NODE_IN(n,adversaries_nodes);
					SD_printf("Adv%d in node %d\n",i,n);
				}
			}
			dist = dist_point_to_point(nodes[n].x, nodes[n].y, global.env.foe[i].x, global.env.foe[i].y);
			if(dist < closestAdv)
				closestAdv = dist;
		}
	}

	if(closestAdv < DISTANCE_CHANGING_FIRST_NODE_FOE){
		from_without_adversaries = 	PATHFIND_closestNode(xFrom, yFrom, 0);
		from =						PATHFIND_closestNodeWithoutAdversary(xFrom, yFrom, adversaries_nodes);
	}else{
		from_without_adversaries = 	PATHFIND_closestNode(xFrom, yFrom, 0);
		//from = 						PATHFIND_closestNode(xFrom, yFrom, adversaries_nodes);	//On cherche le noeud le plus proche (en enlevant les noeuds occupés par l'adversaire)
		from =						PATHFIND_closestNodeToEnd(xFrom, yFrom, adversaries_nodes, PATHFIND_get_node_x(to), PATHFIND_get_node_y(to));
	}

	if(from == NOT_IN_NODE)
		return NOT_HANDLED;	//Pas de chemin possible... c'est d'ailleurs très étrange...

	//si le noeud le plus proche est un noeud situé de l'autre coté d'un obstacle car les adversaires empêchent l'accès aux autres noeuds !!!!
	//Le noeud le plus proche sans filtrage adverse... correspond à notre position, doit permettre d'accéder par la logique des voisinages au noeud le plus proche avec filtrage adverse/
	//Sinon : pas de chemin !
	//AUTREMENT DIT.... (la même chose en différent)
	//Si le noeud 'from' (le plus proche parmi ceux qui ne sont pas recouvert par des adversaires)
	//		n'est pas dans la liste des voisins du noeud réel le plus proche (from_without_adversaries)..
	//Alors, on ne peut pas considérer que l'on se trouve sur le noeud from...
	if((from != from_without_adversaries) && !(PATHFIND_TST_NODE_IN(from, nodes[from_without_adversaries].neighbors)))
		return NOT_HANDLED;

	//Une meilleure piste devrait être dans ce cas de choisir un point calculé permettant de s'extraire de l'adversaire.


	debug_printf ("x:%d | y:%d | from:%d | to:%d\n", xFrom, yFrom, from, to);

	/* On reinitialise les listes et penalites */
	openList = 0;
	closedList = 0;

	if(global.env.color == TOP_COLOR)
		closedList = closedList | (1<<1)|(1<<2); // Supprime les nodes au près du spawn Jaune
	else
		closedList = closedList | (1<<20)|(1<<21); // Supprime les nodes au près du spawn Vert

	/* On ajoute le point de depart dans la liste ouverte */
	PATHFIND_SET_NODE_IN(from, openList);
	/* TODO mettre le cout a la distance que le robot doit parcourir pour atteindre le noeud */
	nodes[from].total_cost = 0;
	nodes[from].nb_nodes = 1;


	/* Tant que la destination n'est pas dans la liste fermee
	 * et que la liste ouverte n'est pas vide
	 */
	while ((!PATHFIND_TST_NODE_IN(to, closedList)) && (openList != 0))
	{

		/*
		 * On cherche la case ayant le cout F le plus faible dans la
		 * liste ouverte. Elle devient la case en cours.
		 */
		minTotalCost = 0xFFFFFFFF;	//On suppose que le cout est max.
		current = 0;
		for (n = 0; n < PATHFIND_NODE_NB; n++) {
			if (PATHFIND_TST_NODE_IN(n, openList) && nodes[n].total_cost < minTotalCost) {
				minTotalCost = nodes[n].total_cost;
				current = n;
			}
		}

		/* On passe la case en cours dans la liste fermee */
		PATHFIND_CLR_NODE_IN(current, openList);
		PATHFIND_SET_NODE_IN(current, closedList);

		/* Pour toutes les cases adjacentes n'etant pas dans la liste fermee */
		for (n = 0; n < PATHFIND_NODE_NB; n++) {

			if ( (PATHFIND_TST_NODE_IN(n, nodes[current].neighbors)) &&
				 !(PATHFIND_TST_NODE_IN(n, closedList)) &&
				 !(PATHFIND_TST_NODE_IN(n,adversaries_nodes)))
			{
				lengthPath = Pathfind_cost(current, n, FALSE);
				distEnd = Pathfind_cost(n, to, TRUE);

				//if(n == Z3)
					debug_printf("focus %d, current %d, total %d, just %d\n", n, current, nodes[current].length_path + lengthPath, lengthPath);

				/*
				 * Si elle n'est pas dans la liste ouverte, on l'y ajoute.
				 * La case en cours devient le parent de cette case.
				 * On calcule les couts F, G et H de cette case.
				 */
				if (!PATHFIND_TST_NODE_IN(n, openList))
				{
					PATHFIND_SET_NODE_IN(n, openList);
					nodes[n].parent = current;
					nodes[n].length_path = nodes[current].length_path + lengthPath;
					nodes[n].dist_end = distEnd;
					nodes[n].total_cost = nodes[n].length_path + distEnd;
					nodes[n].nb_nodes = nodes[current].nb_nodes + 1;
				}
				/*
				 * Si elle est deja  dans la liste ouverte, on teste si le
				 * chemin passant par la case en cours est meilleur en
				 * comparant les couts G. Un cout G inferieur signifie un
				 * meilleur chemin. Si c'est le cas, on change le parent de
				 * la case pour devenir la case en cours, en on recalcule les
				 * couts F et G.
				 */
				else {

					if (nodes[current].length_path + lengthPath < nodes[n].length_path) {
						nodes[n].length_path = nodes[current].length_path + lengthPath;
						nodes[n].dist_end = distEnd;
						nodes[n].total_cost = nodes[n].length_path + distEnd;
						nodes[n].parent = current;
						nodes[n].nb_nodes = nodes[current].nb_nodes + 1;
					}
				}
			}
		}
	}

	/* Si le chemin n'a pas été trouvé */
	if (!PATHFIND_TST_NODE_IN(to, closedList))
		return NOT_HANDLED;


	/*le noeud de départ ne doit pas avoir de parent*/
	nodes[from].parent=from;

	/* On a le chemin inverse (to->from) */

	/* On regarde si la distance avec le premier node peut être négligeable */
	nb_displacements = nodes[to].nb_nodes;
	if(nb_displacements>1 && PATHFIND_manhattan_dist(xFrom, yFrom, nodes[from].x, nodes[from].y) < IGNORE_FIRST_POINT_DISTANCE)
	{	//Si le premier noeud est trop proche de nous pour qu'il faille le rejoindre, on considère qu'on y est déjà
		nb_displacements--;	//Le premier déplacement ne compte pas...
		first_node_deleted = TRUE;
		//Comme ce serait le dernier à être enregistré... il ne sera pas enregistré.
		debug_printf("Le node %d est proche de nous. On ne va pas s'y rendre, on va sur le point suivant.\n",from);
	}

	/* On remplie le tableau de déplacements */
	n = to;
	suivant = to;
	debug_printf("Nodes : ");
	for(i=0;i<nb_displacements;i++)
	{
		displacements[nb_displacements-i-1].point.x = nodes[n].x;
		displacements[nb_displacements-i-1].point.y = nodes[n].y;
		if(suivant != to)
			displacements[nb_displacements-i-1+2].curve = (node_curve[n] & (1<<suivant))?TRUE:FALSE;
		// On attribue le droit à la trajectoire de faire une courbe si :
		// une trajectoire du node courant (n) vers le node suivant (suivant)
		//		autorise une trajectoire pour le déplacement d'après (nb_displacements-i-1+2)

		debug_printf("%d <- ",n);
		suivant = n;
		n = nodes[n].parent;

		// Gestion de la courbe dans le cas ou l'on supprime le premier node par optimisation
		if(i == nb_displacements-1 && first_node_deleted)
			displacements[nb_displacements-i-1+2-1].curve = (node_curve[n] & (1<<suivant))?TRUE:FALSE;
	}

	if(nb_displacements > 0)
		displacements[0].curve = FALSE;

	if(nb_displacements > 1 && first_node_deleted == FALSE)
		displacements[1].curve = FALSE;

	debug_printf(" = %d displacements\n", nb_displacements);
	*p_nb_displacements = nb_displacements;
	return END_OK;
}

/**
 * @brief PATHFIND_try_going
 *		Fonction a appelée si l'on veut se déplacer sur un node
 *
 * @param node_wanted : l'id du node voulu
 *
 * @param in_progress : Etat en cours
 * @param success_state : Etat à retourner si le déplacement s'est terminé correctement
 * @param fail_state : Etat à retourner si le déplacement ne s'est pas terminé correctement
 *
 * @param way : Sens de déplacement
 * @param speed : Vitesse de la  trajectoire selon l'enum PROP_speed_e ou la fonction PROP_custom_speed_convertor(Uint16 speed);
 * @param avoidance : Type d'evitement à faire
 *			DODGE_AND_WAIT / DODGE_AND_NO_WAIT : lorsqu'un adversaire est détecté, l'évittement est tenté en recalculant une alternative.
 * @param end_condition : Comportement du robot en fin de trajectoire
 *
 * @return le state rentré en argument
 */
Uint8 PATHFIND_try_going(pathfind_node_id_t node_wanted, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, PROP_speed_e speed, avoidance_type_e avoidance, PROP_end_condition_e end_condition)
{
	error_e sub_action;
	static displacement_curve_t displacements[PATHFIND_NODE_NB];
	static Uint8 nb_displacements;
	//static avoidance_type_e no_dodge_avoidance;
	static bool_e dodge_nb_try;
	Uint8 i;
	CREATE_MAE_WITH_VERBOSE(SM_ID_PATHFIND_TRY_GOING,
							INIT,
							COMPUTE,
							DISPLACEMENT,
							FAIL,
							SUCCESS
							);

	switch(state)
	{
		case INIT:
			switch(avoidance)
			{
				case DODGE_AND_WAIT:
					dodge_nb_try = NB_TRY_WHEN_DODGE;
					//no_dodge_avoidance = NO_DODGE_AND_WAIT;
					break;
				case DODGE_AND_NO_WAIT:
					dodge_nb_try = NB_TRY_WHEN_DODGE;
					//no_dodge_avoidance = NO_DODGE_AND_NO_WAIT;
					break;
				default:
					dodge_nb_try = 0;
					//no_dodge_avoidance = avoidance;
					break;
			}
			state = COMPUTE;
			break;
		case COMPUTE:
			debug_printf("Compute\n");
			//Calcul d'un chemin pour atteindre l'objectif.
			if(PATHFIND_compute(displacements, &nb_displacements, global.env.pos.x, global.env.pos.y, node_wanted) != END_OK)
			{
				SD_printf("I cannot find any path. x=%d y=%d to node %d\n",global.env.pos.x, global.env.pos.y, node_wanted);
				state = FAIL;
			}
			else
			{
				if(nb_displacements)	//on a trouvé des déplacements à faire
				{
					for(i=0;i<nb_displacements;i++)
					{
						debug_printf("[%d,%d]->\n",displacements[i].point.x, displacements[i].point.y);
						displacements[i].speed = speed;
					}
					state = DISPLACEMENT;
				}
				else
				{
					debug_printf("No displacements, we are already on the point\n");
					state = SUCCESS;	//aucun déplacement n'est nécessaire, on est déjà sur le point...
				}
			}
			break;
		case DISPLACEMENT:
			sub_action = goto_pos_curve_with_avoidance(NULL, displacements, nb_displacements, way, avoidance, end_condition);
			switch(sub_action)
			{
				case IN_PROGRESS:
					break;
				case END_OK:
					state = SUCCESS;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
				default:
					if(dodge_nb_try)
					{
						dodge_nb_try--;
						state = COMPUTE;
					}
					else
						state = FAIL;
					break;
			}
			break;
		case FAIL:
			state = INIT;
			return fail_state;
			break;
		case SUCCESS:
			state = INIT;
			return success_state;
			break;
	}
	return in_progress;
}


//////////////////////////////////////////////////////////////////
//--------------Accesseurs et fonctions calculs-----------------//
//////////////////////////////////////////////////////////////////

/**
 * @brief PATHFIND_get_node_x
 * @param n	: id du node
 * @return la coordonnée x du node voulu
 */
Sint16 PATHFIND_get_node_x (pathfind_node_id_t n) {
	return nodes[n].x;
}

/**
 * @brief PATHFIND_get_node_y
 * @param n	: id du node
 * @return la coordonnée y du node voulu
 */
Sint16 PATHFIND_get_node_y (pathfind_node_id_t n) {
	return nodes[n].y;
}

/**
 * @brief PATHFING_get_symetric
 * @param n : id du node
 * @return le node symétrique à ce node
 */
Uint16 PATHFING_get_symetric(pathfind_node_id_t n){
	if(global.env.color == TOP_COLOR){
		if(n<4)
			return n+19;
		if(n<7 && n>3)
			return n +12;
		if(n<10 && n>6)
			return n+6;
		if(n>12 && n<16)
			return n-6;
		if(n>15 && n<19)
			return n-12;
		if(n>18 && n<23)
			return n-19;
	}
	return n;
}


/**
 * @brief min_node_dist
 *		Renvoie l'id du node le plus proche entre 2 choix
 *
 * @param n1 : id du premier node
 * @param n2 : id du second node
 * @return l'id du node le plus proche de notre position
 */
pathfind_node_id_t min_node_dist(pathfind_node_id_t n1,pathfind_node_id_t n2){
	if(PATHFIND_manhattan_dist(PATHFIND_get_node_x(n1),PATHFIND_get_node_y(n1),global.env.pos.x,global.env.pos.y)
			< PATHFIND_manhattan_dist(PATHFIND_get_node_x(n2),PATHFIND_get_node_y(n2),global.env.pos.x,global.env.pos.y))
		return n1;
	else
		return n2;
}

/**
 * @brief PATHFIND_dist_node_to_node
 * @param n1 : id du premier node
 * @param n2 : id du second node
 * @return la distance entre les deux nodes
 */
Sint16 PATHFIND_dist_node_to_node(pathfind_node_id_t n1, pathfind_node_id_t n2){
	return dist_point_to_point(nodes[n1].x, nodes[n1].y, nodes[n2].x, nodes[n2].y);
}

/**
 * @brief path_length
 * @param tab_node : tableau de node
 * @param nb_node : nombre de node dans le tableau
 * @return la longueur du chemin des nodes présent dans le tableau
 */
Sint16 path_length(pathfind_node_id_t* tab_node, Uint8 nb_node){
	Uint8 i;
	Sint16 length = 0;
	for(i=0;i<nb_node-1;i++){
		length += PATHFIND_dist_node_to_node(tab_node[i], tab_node[i+1]);
	}
	return length;
}

/**
 * @brief PATHFIND_manhattan_dist
 *		Calcul de la distance manhattan entre les deux points (addition de la différence en x avec la différence en y)
 * @param x1 : x du point 1
 * @param y1 : y du point 1
 * @param x2 : x du point 2
 * @param y2 : y du point 2
 * @return distance manhattan entre les deux points
 */
Uint16 PATHFIND_manhattan_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1>x2 ? x1-x2 : x2-x1) + (y1>y2 ? y1-y2 : y2-y1);
}

/**
 * @brief PATHFIND_squared_dist
 *		Calcul de la distance algébrique entre les deux points sans faire la racine carré (gain de performance)
 * @param x1 : x du point 1
 * @param y1 : y du point 1
 * @param x2 : x du point 2
 * @param y2 : y du point 2
 * @return distance algébrique sans racine carré du résultat
 */
Uint32 PATHFIND_squared_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1-x2) * (x1-x2) +  (y1-y2) * (y1-y2);
}

// Ebauche et ancienne fonction de pathfind
#if VIEILLE_FONCTION_OBSOLETE

static pathfind_node_id_t nodeOpponent[MAX_NB_FOES];

void PATHFIND_updateOpponentPosition(Uint8 foe_id)
{
	assert(foe_id < MAX_NB_FOES);
	//On desactive l'evitement pour trouver le noeud le plus proche de l'adversaire
	pathfind_node_id_t node = PATHFIND_closestNode(global.env.foe[foe_id].x, global.env.foe[foe_id].y, 0);
	//si on est trop loin
	int dist = GEOMETRY_squared_distance((GEOMETRY_point_t){nodes[node].x,nodes[node].y},(GEOMETRY_point_t){global.env.foe[foe_id].x,global.env.foe[foe_id].y});
	if(dist>220)
	{
		nodeOpponent[foe_id] = NOT_IN_NODE;//invalid
		debug_printf("OPPONENT NOT ON A NODE (%d) cause dist=%d\n", NOT_IN_NODE, dist);
	}
	debug_printf("UPDATE OPPONENT POSITION FOE %d: %d\n", foe_id, nodeOpponent[foe_id]);
}

/*
pathfind_node_id_t  PATHFIND_random_neighbor(pathfind_node_id_t of, bool_e handleOpponent[MAX_NB_FOES])
{
	static pathfind_node_id_t n = 0;
	while (1)
	{
		n = (n + 1) % PATHFIND_NODE_NB;
		if (PATHFIND_TST_NODE_IN(n, nodes[of].neighbors)) {
			if ( ! ((n == nodeOpponent[FOE_1] && handleOpponent[FOE_1]) ||
				(n == nodeOpponent[FOE_2] && handleOpponent[FOE_2])))
			{
				return n;
			}
		}
	}
	return n;
}
*/

/*
 * Cree par Alexis
 */
/*
void PATHFIND_delete_useless_node(pathfind_node_id_t from, pathfind_node_id_t to)
{
	pathfind_node_id_t before, current, after;
	pathfind_node_id_t xb, yb, xc, yc, xa, ya;

	before = to;
	current=nodes[to].parent;

	//On parcours le chemin calcule

	while(current!=from)
	{
		after=nodes[current].parent;

		xb=nodes[before].x; yb=nodes[before].y;
		xc=nodes[current].x; yc=nodes[current].y;
		xa=nodes[after].x; ya=nodes[after].y;


		if ( ((xb==xc) && (xc==xa)) || ((yb==yc) && (yc==ya)) )
		{
			// Si trois noeuds sont strictement alignes,
			// celui du milieu est supprime

			nodes[before].parent=after;

			after=nodes[(nodes[after].parent)].parent;
			current=nodes[after].parent;
			before=current;
		}
		else
		{
			// sinon on decalle simplement les noeuds a etudier
			after=nodes[after].parent;
			current=after;
			before=current;
		}
	}
}
*/

Uint16 PATHFIND_compute(Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to, PROP_speed_e speed, way_e way, bool_e handleOpponent)
{
	pathfind_node_id_t from, n, current;

	Uint16 minCost, heuristic;

	from = PATHFIND_closestNode(xFrom, yFrom, handleOpponent);

	debug_printf ("Noeud le plus proche : %d", from);

	/* On reinitialise les listes et penalites */
	openList = 0;
	closedList = 0;

	/* On ajoute le point de depart dans la liste ouverte */
	PATHFIND_SET_NODE_IN(from, openList);
	/* TODO mettre le cout a la distance que le robot doit parcourir pour atteindre le noeud */
	nodes[from].cost = 0;

	/* Tant que la destination n'est pas dans la liste fermee
	 * et que la liste ouverte n'est pas vide
	 */
	while ((!PATHFIND_TST_NODE_IN(to, closedList)) && (openList != 0))
	{

		/*
		 * On cherche la case ayant le cout F le plus faible dans la
		 * liste ouverte. Elle devient la case en cours.
		 */
		minCost = 65535;
		current = 0;
		for (n = 0; n < PATHFIND_NODE_NB; n++) {
			if (PATHFIND_TST_NODE_IN(n, openList) && nodes[n].cost < minCost) {
				minCost = nodes[n].cost;
				current = n;
			}
		}

		/* On passe la case en cours dans la liste fermee */
		PATHFIND_CLR_NODE_IN(current, openList);
		PATHFIND_SET_NODE_IN(current, closedList);

		/* Pour toutes les cases adjacentes n'etant pas dans la liste fermee */
		for (n = 0; n < PATHFIND_NODE_NB; n++) {

			if ( (PATHFIND_TST_NODE_IN(n, nodes[current].neighbors)) &&
				 !(PATHFIND_TST_NODE_IN(n, closedList)) &&
				 !(handleOpponent && ((n == nodeOpponent[FOE_1])||(n == nodeOpponent[FOE_2]))))
			{

				heuristic = Pathfind_heuristic(n, to, handleOpponent);
				/*
				 * Si elle n'est pas dans la liste ouverte, on l'y ajoute.
				 * La case en cours devient le parent de cette case.
				 * On calcule les couts F, G et H de cette case.
				 */
				if (!PATHFIND_TST_NODE_IN(n, openList))
				{
					PATHFIND_SET_NODE_IN(n, openList);
					nodes[n].parent = current;
					nodes[n].heuristic = heuristic;
					nodes[n].cost = nodes[current].cost + PATHFIND_STEP_COST + heuristic;
				}
				/*
				 * Si elle est deja  dans la liste ouverte, on teste si le
				 * chemin passant par la case en cours est meilleur en
				 * comparant les couts G. Un cout G inferieur signifie un
				 * meilleur chemin. Si c'est le cas, on change le parent de
				 * la case pour devenir la case en cours, en on recalcule les
				 * couts F et G.
				 */
				else {

					if (heuristic < nodes[n].heuristic) {
						nodes[n].heuristic = heuristic;
						nodes[n].cost = nodes[current].cost + PATHFIND_STEP_COST + heuristic;
						nodes[n].parent = current;
					}
				}
			}
		}
	}

	/* Si le chemin n'a pas été trouvé */
	if (!PATHFIND_TST_NODE_IN(to, closedList))
	{
		STACKS_push(PROP, &wait_forever, TRUE);
		return PATHFIND_OPPONENT_COST;
	}


	/*le noeud de départ ne doit pas avoir de parent*/
	nodes[from].parent=from;

	/* Permet d'optimiser les deplacements*/
	//PATHFIND_delete_useless_node(from, to);

	/* On a le chemin inverse (to->from) */
	for(n = to; n != from; n = nodes[n].parent)
	{
		// on ne renvoie que le coût supplémentaire, on enlève donc le coût normal
		// pour passer d'un noeud à l'autre
/*

		PROP_push_goto_multi_point(nodes[n].x, nodes[n].y, speed, way, 0, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);

		nodes[to].cost -= PATHFIND_STEP_COST;
*/
		debug_printf("goto %d\n",n);
	}


	if ((to == from) || /** Le noeud de destination est le plus proche */
		(PATHFIND_squared_dist(nodes[n].x, nodes[n].y, xFrom, yFrom) > ((Uint32)NODE_PROXIMITY_DISTANCE*NODE_PROXIMITY_DISTANCE))
	) {
/*
			PROP_push_goto_multi_point(nodes[from].x, nodes[from].y, speed, way, 1, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);

*/
		debug_printf("goto %d\n",from);
		// on ne renvoie que le coût supplémentaire, on enlève donc le coût normal
		// pour passer d'un noeud à l'autre
		if (nodes[to].parent != from) {
			// Permet de passer par un noeud intermédiaire dans la trajectoire (le robot ne sait pas encore passer à travers les maïs)
			nodes[to].cost -= PATHFIND_STEP_COST;
		}
	}

	STACKS_push(PROP, &wait_forever, TRUE);

	return nodes[to].cost;
}
#endif

#endif
