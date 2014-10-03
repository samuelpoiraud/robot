   /*
 *	Club Robot ESEO 2011 - 2012 - 2014
 *	Shark & Fish / Pierre & Guy
 *
 *	Fichier : Pathfind.c
 *	Package : Carte Principale
 *	Description : d�finition des noeuds et des fonctions de pathfind
 *	Auteur : Louis-Marie, Vincent, Antoine, Nirgal
 *	Version 201312
 */

#include "Pathfind.h"
#include "QS/QS_outputlog.h"
#include "config_use.h"
#include "../state_machine_helper.h"
#include "../Supervision/SD/SD.h"
#include "avoidance.h"
#include <math.h>


#define pathfind_debug_printf(...)	debug_printf(__VA_ARGS__)
//#define pathfind_debug_printf(...)	void(0)
#define MANHATTAN_DIST_NODE_BLOQUED_BY_ADVERSARY	400		//Distance manhattan entre un advesaire et les noeuds dont il bloque l'acc�s
#define DISTANCE_CONSIDERE_ADVERSARY				1200	//Distance entre nous et l'adversaire pour qu'il soit pris en compte (s'il est loin, on le n�glige.. en esp�rant qu'il bouge d'ici � notre arriv�e)
#define	TIME_CONSIDERE_ADVERSARY					500		//[ms] temps au del� duquel un adversaire mis � jour n'est plus consid�r�
#define NB_TRY_WHEN_DODGE							3		//Nombre de tentatives d'�vitement (recalcul de chemin..)
#define IGNORE_FIRST_POINT_DISTANCE					150		//Distance du noeud en dessous de laquelle on ne s'y rend pas et on attaque directement le voisin

#define FACTEUR_DIRECTIF							10		//Facteur permettant d'avoir un compute plus rapide car priorit� � ce qui est proche de la destination, mais perte d'efficacit�

#ifndef USE_POLYGON

#ifdef OLD_PATHFIND
static pathfind_node_t nodes[PATHFIND_NODE_NB+1] =
{
	//Colonne 1 cot� Rouge [A]
	(pathfind_node_t){ 750, 400, neighbors : (1<<1)|(1<<2)|(1<<3)|(1<<4)},							//[A1] 0
	(pathfind_node_t){ 1350, 400, neighbors : (1<<0)|(1<<3)|(1<<4)|(1<<5)},							//[A2] 1

	//Colonne 2 cot� rouge [B]
	(pathfind_node_t){ 520, 750, neighbors : (1<<0)|(1<<3)|(1<<6)|(1<<7)},							//[B0] 2
	(pathfind_node_t){ 850, 750, neighbors : (1<<0)|(1<<1)|(1<<2)|(1<<4)|(1<<6)|(1<<7)},			//[B1] 3
	(pathfind_node_t){ 1250, 750, neighbors : (1<<0)|(1<<1)|(1<<3)|(1<<5)|(1<<8)|(1<<9)},			//[B2] 4
	(pathfind_node_t){ 1550, 750, neighbors :(1<<1)|(1<<4)|(1<<8)|(1<<9)},							//[B3] 5

	//Colonne 3 cot� rouge [C]
	(pathfind_node_t){ 520, 1100, neighbors : (1<<2)|(1<<3)|(1<<7)|(1<<10)|(1<<11)},				//[C0] 6
	(pathfind_node_t){ 850, 1100, neighbors : (1<<2)|(1<<3)|(1<<6)|(1<<8)|(1<<10)|(1<<11)},			//[C1] 7
	(pathfind_node_t){ 1250, 1100, neighbors : (1<<4)|(1<<5)|(1<<7)|(1<<9)|(1<<12)|(1<<13)},		//[C2] 8
	(pathfind_node_t){ 1600, 1100, neighbors :(1<<4)|(1<<5)|(1<<8)|(1<<12)|(1<<13)},				//[C3] 9

	//Colonne 4 milieu [M]
	(pathfind_node_t){ 400, 1500, neighbors : (1<<6)|(1<<7)|(1<<11)|(1<<14)|(1<<15)},				//[M0] 10
	(pathfind_node_t){ 650, 1500, neighbors : (1<<6)|(1<<7)|(1<<10)|(1<<14)|(1<<15)},				//[M1] 11
	(pathfind_node_t){ 1450, 1500, neighbors : (1<<8)|(1<<9)|(1<<13)|(1<<16)|(1<<17)},				//[M2] 12
	(pathfind_node_t){ 1700, 1500, neighbors :(1<<8)|(1<<9)|(1<<12)|(1<<16)|(1<<17)},				//[M3] 13

	//Colonne 4 cot� jaune [W]
	(pathfind_node_t){ 520, 1900, neighbors : (1<<10)|(1<<11)|(1<<15)|(1<<18)|(1<<19)},				//[W0] 14
	(pathfind_node_t){ 850, 1900, neighbors : (1<<10)|(1<<11)|(1<<14)|(1<<16)|(1<<18)|(1<<19)},		//[W1] 15
	(pathfind_node_t){ 1250, 1900, neighbors : (1<<12)|(1<<13)|(1<<15)|(1<<17)|(1<<20)|(1<<21)},	//[W2] 16
	(pathfind_node_t){ 1600, 1900, neighbors :(1<<12)|(1<<13)|(1<<16)|(1<<20)|(1<<21)},				//[W3] 17

	//Colonne 5 cot� jaune [Y]
	(pathfind_node_t){ 520, 2250, neighbors : (1<<14)|(1<<15)|(1<<19)|(1<<22)},						//[Y0] 18
	(pathfind_node_t){ 850, 2250, neighbors : (1<<14)|(1<<15)|(1<<18)|(1<<20)|(1<<22)|(1<<23)},		//[Y1] 19
	(pathfind_node_t){ 1250, 2250, neighbors : (1<<16)|(1<<17)|(1<<19)|(1<<21)|(1<<22)|(1<<23)},	//[Y2] 20
	(pathfind_node_t){ 1550, 2250, neighbors :(1<<16)|(1<<17)|(1<<20)|(1<<23)},						//[Y3] 21

	//Colonne 6 cot� Jaune [Z]
	(pathfind_node_t){ 750, 2600, neighbors : (1<<18)|(1<<19)|(1<<20)|(1<<23)},				//[Z1] 22
	(pathfind_node_t){ 1350, 2600, neighbors : (1<<19)|(1<<20)|(1<<21)|(1<<22)},					//[Z2] 23

	(pathfind_node_t){ 0, 0, neighbors : 0} //[NOT_IN_NODE] 24 (invalid)
};

static Uint32 node_curve[PATHFIND_NODE_NB+1] =
{
	0 | (1<<1)|(0<<2)|(1<<3)|(1<<4),						//[A1] 0
	0 | (1<<0)|(1<<3)|(1<<4)|(0<<5),						//[A2] 1
	0 | (1<<0)|(1<<3)|(1<<6)|(0<<7),						//[B0] 2
	0 | (1<<0)|(1<<1)|(0<<2)|(1<<4)|(1<<6)|(0<<7),			//[B1] 3
	0 | (1<<0)|(0<<1)|(1<<3)|(0<<5)|(0<<8)|(1<<9),			//[B2] 4
	0 | (1<<1)|(1<<4)|(0<<8)|(1<<9),						//[B3] 5
	0 | (1<<2)|(1<<3)|(0<<7)|(1<<10)|(1<<11),				//[C0] 6
	0 | (0<<2)|(1<<3)|(0<<6)|(1<<8)|(1<<10)|(1<<11),		//[C1] 7
	0 | (1<<4)|(1<<5)|(1<<7)|(0<<9)|(1<<12)|(1<<13),		//[C2] 8
	0 | (1<<4)|(1<<5)|(0<<8)|(1<<12)|(1<<13),				//[C3] 9
	0 | (1<<6)|(1<<7)|(0<<11)|(1<<14)|(1<<15),				//[M0] 10
	0 | (1<<6)|(1<<7)|(0<<10)|(1<<14)|(1<<15),				//[M1] 11
	0 | (1<<8)|(0<<9)|(0<<13)|(1<<16)|(0<<17),				//[M2] 12
	0 | (1<<8)|(1<<9)|(0<<12)|(1<<16)|(1<<17),				//[M3] 13
	0 | (1<<10)|(1<<11)|(0<<15)|(1<<18)|(1<<19),			//[W0] 14
	0 | (1<<10)|(1<<11)|(0<<14)|(1<<16)|(0<<18)|(1<<19),	//[W1] 15
	0 | (1<<12)|(0<<13)|(1<<15)|(0<<17)|(1<<20)|(1<<21),	//[W2] 16
	0 | (1<<12)|(1<<13)|(0<<16)|(1<<20)|(1<<21),			//[W3] 17
	0 | (1<<14)|(0<<15)|(1<<19)|(0<<22),					//[Y0] 18
	0 | (1<<14)|(0<<15)|(0<<18)|(1<<20)|(0<<22)|(1<<23),	//[Y1] 19
	0 | (0<<16)|(1<<17)|(1<<19)|(0<<21)|(0<<22)|(0<<23),	//[Y2] 20
	0 | (0<<16)|(1<<17)|(1<<20)|(0<<23),					//[Y3] 21
	0 | (0<<18)|(1<<19)|(1<<20)|(1<<23),					//[Z0] 22
	0 | (1<<19)|(1<<20)|(1<<21)|(1<<22),					//[Z1] 23
	0
};


#else


static pathfind_node_t nodes[PATHFIND_NODE_NB+1] =
{
	//Colonne 1 cot� Rouge [A]
	(pathfind_node_t){ 400, 500, neighbors : (1<<1)|(1<<4)},											//[A0] 0
	(pathfind_node_t){ 800, 600, neighbors : (1<<0)|(1<<4)|(1<<5)|(1<<2)},								//[A1] 1
	(pathfind_node_t){ 1200, 600, neighbors : (1<<1)|(1<<4)|(1<<5)|(1<<6)|(1<<3)},						//[A2] 2
	(pathfind_node_t){ 1600, 500, neighbors : (1<<2)|(1<<5)|(1<<6)},									//[A3] 3

	//Colonne 2 cot� rouge [B]
	(pathfind_node_t){ 700, 800, neighbors : (1<<0)|(1<<1)|(1<<2)|(1<<5)|(1<<8)|(1<<7)},				//[B1] 4
	(pathfind_node_t){ 1150, 800, neighbors : (1<<1)|(1<<2)|(1<<3)|(1<<6)|(1<<9)|(1<<8)|(1<<7)|(1<<4)},	//[B2] 5
	(pathfind_node_t){ 1550, 800, neighbors : (1<<3)|(1<<2)|(1<<5)|(1<<8)|(1<<9)},						//[B3] 6

	//Colonne 3 cot� rouge [C]
	(pathfind_node_t){ 850, 1150, neighbors : (1<<4)|(1<<5)|(1<<8)|(1<<11)|(1<<10)},					//[C1] 7
	(pathfind_node_t){ 1200, 1150, neighbors : (1<<4)|(1<<5)|(1<<6)|(1<<9)|(1<<12)|(1<<11)|(1<<10)|(1<<7)},		//[C2] 8
	(pathfind_node_t){ 1550, 1150, neighbors : (1<<6)|(1<<5)|(1<<8)|(1<<11)|(1<<12)},					//[C3] 9

	//Colonne 4 milieu [M]
	(pathfind_node_t){ 850, 1500, neighbors : (1<<7)|(1<<8)|(1<<11)|(1<<14)|(1<<13)},					//[M1] 10
	(pathfind_node_t){ 1200, 1500, neighbors : (1<<7)|(1<<8)|(1<<9)|(1<<12)|(1<<15)|(1<<14)|(1<<13)|(1<<10)},	//[M2] 11
	(pathfind_node_t){ 1550, 1500, neighbors : (1<<9)|(1<<8)|(1<<11)|(1<<14)|(1<<15)},					//[M3] 12

	//Colonne 4 cot� jaune [W]
	(pathfind_node_t){ 850, 1850, neighbors : (1<<10)|(1<<11)|(1<<14)|(1<<17)|(1<<16)},					//[W1] 13
	(pathfind_node_t){ 1200, 1850, neighbors : (1<<10)|(1<<11)|(1<<12)|(1<<15)|(1<<18)|(1<<17)|(1<<16)|(1<<13)},//[W2] 14
	(pathfind_node_t){ 1550, 1850, neighbors : (1<<12)|(1<<11)|(1<<14)|(1<<17)|(1<<18)},				//[W3] 15

	//Colonne 5 cot� jaune [Y]
	(pathfind_node_t){ 700, 2200, neighbors : (1<<13)|(1<<14)|(1<<17)|(1<<21)|(1<<20)|(1<<19)},			//[Y1] 16
	(pathfind_node_t){ 1150, 2200, neighbors : (1<<13)|(1<<14)|(1<<15)|(1<<18)|(1<<22)|(1<<21)|(1<<20)|(1<<16)},//[Y2] 17
	(pathfind_node_t){ 1550, 2200, neighbors : (1<<15)|(1<<14)|(1<<17)|(1<<21)|(1<<22)},				//[Y3] 18

	//Colonne 6 cot� Jaune [Z]
	(pathfind_node_t){ 400, 2500, neighbors : (1<<16)|(1<<20)},											//[Z0] 19
	(pathfind_node_t){ 800, 2400, neighbors : (1<<19)|(1<<16)|(1<<17)|(1<<21)},							//[Z1] 20
	(pathfind_node_t){ 1200, 2400, neighbors : (1<<20)|(1<<16)|(1<<17)|(1<<18)|(1<<22)},				//[Z2] 21
	(pathfind_node_t){ 1600, 2500, neighbors : (1<<21)|(1<<17)|(1<<18)},								//[Z3] 22

	(pathfind_node_t){ 0, 0, neighbors : 0} //[NOT_IN_NODE] 23 (invalid)
};



// Pas � jour
static Uint32 node_curve[PATHFIND_NODE_NB+1] =
{
	0 | (1<<1)|(0<<2)|(1<<3)|(1<<4),						//[A1] 0
	0 | (1<<0)|(1<<3)|(1<<4)|(0<<5),						//[A2] 1
	0 | (1<<0)|(1<<3)|(1<<6)|(0<<7),						//[B0] 2
	0 | (1<<0)|(1<<1)|(0<<2)|(1<<4)|(1<<6)|(0<<7),			//[B1] 3
	0 | (1<<0)|(0<<1)|(1<<3)|(0<<5)|(0<<8)|(1<<9),			//[B2] 4
	0 | (1<<1)|(1<<4)|(0<<8)|(1<<9),						//[B3] 5
	0 | (1<<2)|(1<<3)|(0<<7)|(1<<10)|(1<<11),				//[C0] 6
	0 | (0<<2)|(1<<3)|(0<<6)|(1<<8)|(1<<10)|(1<<11),		//[C1] 7
	0 | (1<<4)|(1<<5)|(1<<7)|(0<<9)|(1<<12)|(1<<13),		//[C2] 8
	0 | (1<<4)|(1<<5)|(0<<8)|(1<<12)|(1<<13),				//[C3] 9
	0 | (1<<6)|(1<<7)|(0<<11)|(1<<14)|(1<<15),				//[M0] 10
	0 | (1<<6)|(1<<7)|(0<<10)|(1<<14)|(1<<15),				//[M1] 11
	0 | (1<<8)|(0<<9)|(0<<13)|(1<<16)|(0<<17),				//[M2] 12
	0 | (1<<8)|(1<<9)|(0<<12)|(1<<16)|(1<<17),				//[M3] 13
	0 | (1<<10)|(1<<11)|(0<<15)|(1<<18)|(1<<19),			//[W0] 14
	0 | (1<<10)|(1<<11)|(0<<14)|(1<<16)|(0<<18)|(1<<19),	//[W1] 15
	0 | (1<<12)|(0<<13)|(1<<15)|(0<<17)|(1<<20)|(1<<21),	//[W2] 16
	0 | (1<<12)|(1<<13)|(0<<16)|(1<<20)|(1<<21),			//[W3] 17
	0 | (1<<14)|(0<<15)|(1<<19)|(0<<22),					//[Y0] 18
	0 | (1<<14)|(0<<15)|(0<<18)|(1<<20)|(0<<22)|(1<<23),	//[Y1] 19
	0 | (0<<16)|(1<<17)|(1<<19)|(0<<21)|(0<<22)|(0<<23),	//[Y2] 20
	0 | (0<<16)|(1<<17)|(1<<20)|(0<<23),					//[Y3] 21
	0 | (0<<18)|(1<<19)|(1<<20)|(1<<23),					//[Z0] 22
	0
};
#endif


static pathfind_node_list_t openList;
static pathfind_node_list_t closedList;

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
		pathfind_debug_printf("OPPONENT NOT ON A NODE (%d) cause dist=%d\n", NOT_IN_NODE, dist);
	}
	pathfind_debug_printf("UPDATE OPPONENT POSITION FOE %d: %d\n", foe_id, nodeOpponent[foe_id]);
}

Uint16 PATHFIND_manhattan_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1>x2 ? x1-x2 : x2-x1) + (y1>y2 ? y1-y2 : y2-y1);
}

Uint32 PATHFIND_squared_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1-x2) * (x1-x2) +  (y1-y2) * (y1-y2);
}

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
/*	TODO : r�tablir les lignes qui suivent
				if (dist <= (PATHFIND_STEP_COST/2)) {
				if (dist <= NODE_PROXIMITY_DISTANCE) {
					break;
				}*/
			}
		}
	}
	return closestNode;
}

/*
 * Estimation de la distance necessaire pour aller d'un noeud a l'autre,
 * plus cette distance est proche de la realite, plus l'algorithme sera
 * rapide.
 *
 * TODO pr�calculer toutes les possibilit�s pour avoir un heuristic exact ?
 */
Uint16 Pathfind_cost(pathfind_node_id_t from, pathfind_node_id_t to, bool_e handleOpponent)
{
	Uint8 i;
	Uint32 dist;
	/* On se base sur un manhattan */
	Uint16 cost;
	cost = PATHFIND_manhattan_dist(nodes[from].x, nodes[from].y, nodes[to].x, nodes[to].y);

	if (handleOpponent)
	{
	/*
	 * Si l'adversaire est � moins de 90 cm du noeud, on
	 * ajoute une p�nalit� importante, inversement proportionnelle � la distance adversaire-noeud
	 */

		for(i=0; i<MAX_NB_FOES; i++)
		{
			if(global.env.foe[i].enable	&& global.env.foe[i].dist < DISTANCE_CONSIDERE_ADVERSARY)	//Si l'adversaire est proche de nous...
			{
				dist = PATHFIND_manhattan_dist(nodes[from].x, nodes[from].y, global.env.foe[i].x, global.env.foe[i].y);
				if (dist < ((Uint32)900))	//Si l'adversaire proche de nous est proche du noeud en question... on allourdi s�rieusement le co�t de ce noeud.
					cost += (((Uint32)900) - dist);
			}
		}
	}
	return cost;
}


Uint16 Pathfind_cost_new(pathfind_node_id_t from, pathfind_node_id_t to, bool_e handleOpponent)
{
	Uint8 i;
	Uint32 dist;
	/* On se base sur un manhattan */
	Uint16 cost;
	Uint16 already_compute[PATHFIND_NODE_NB][PATHFIND_NODE_NB] = {{0}};

	if(already_compute[from][to] != 0)
		cost = already_compute[from][to];
	else
		already_compute[from][to] = cost = GEOMETRY_distance((GEOMETRY_point_t){nodes[from].x, nodes[from].y}, (GEOMETRY_point_t){nodes[to].x, nodes[to].y});

	/*if (handleOpponent)
	{
	/*
	 * Si l'adversaire est � moins de 90 cm du noeud, on
	 * ajoute une p�nalit� importante, inversement proportionnelle � la distance adversaire-noeud


		for(i=0; i<MAX_NB_FOES; i++)
		{
			if(global.env.foe[i].enable	&& global.env.foe[i].dist < DISTANCE_CONSIDERE_ADVERSARY)	//Si l'adversaire est proche de nous...
			{
				dist = PATHFIND_manhattan_dist(nodes[from].x, nodes[from].y, global.env.foe[i].x, global.env.foe[i].y);
				if (dist < ((Uint32)900))	//Si l'adversaire proche de nous est proche du noeud en question... on allourdi s�rieusement le co�t de ce noeud.
					cost += (((Uint32)900) - dist);
			}
		}
	}*/
	return cost;
}


Sint16 PATHFIND_get_node_x (pathfind_node_id_t n) {
	return nodes[n].x;
}

Sint16 PATHFIND_get_node_y (pathfind_node_id_t n) {
	return nodes[n].y;
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

//Retourne le nombre de d�placements, ou 0 si pas de chemin possible
error_e PATHFIND_compute(displacement_curve_t * displacements, Uint8 * p_nb_displacements, Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to)
{
	pathfind_node_id_t from, n, current, from_without_adversaries, suivant;
	pathfind_node_list_t adversaries_nodes;
	Uint16 minCost, cost;
	Uint8 i;
	Uint8 nb_displacements = 0;
	//On identifie les noeuds plac�s � moins de 500mm manhattan d'un adversaire.
	adversaries_nodes = 0;
	for(i=0;i<MAX_NB_FOES;i++)
	{
		if(global.env.foe[i].enable && global.env.foe[i].dist < DISTANCE_CONSIDERE_ADVERSARY)	//Pour chaque adversaire situ� proche de nous...
		{
			for(n = 0; n < PATHFIND_NODE_NB; n++)	//Pour chaque noeud
			{	//Si l'adversaire en question est proche du noeud : on ajoute le noeud dans la liste des noeuds innaccessibles.

				if(PATHFIND_manhattan_dist(nodes[n].x, nodes[n].y, global.env.foe[i].x, global.env.foe[i].y)<MANHATTAN_DIST_NODE_BLOQUED_BY_ADVERSARY)
				{
					PATHFIND_SET_NODE_IN(n,adversaries_nodes);
					SD_printf("Adv%d in node %d\n",i,n);
				}
			}
		}
	}

	from_without_adversaries = 	PATHFIND_closestNode(xFrom, yFrom, 0);
	from = 						PATHFIND_closestNode(xFrom, yFrom, adversaries_nodes);	//On cherche le noeud le plus proche (en enlevant les noeuds occup�s par l'adversaire)
	//from =						PATHFIND_closestNodeToEnd(xFrom, yFrom, adversaries_nodes, PATHFIND_get_node_x(to), PATHFIND_get_node_y(to));

	if(from == NOT_IN_NODE)
		return NOT_HANDLED;	//Pas de chemin possible... c'est d'ailleurs tr�s �trange...

	//si le noeud le plus proche est un noeud situ� de l'autre cot� d'un obstacle car les adversaires emp�chent l'acc�s aux autres noeuds !!!!
	//Le noeud le plus proche sans filtrage adverse... correspond � notre position, doit permettre d'acc�der par la logique des voisinages au noeud le plus proche avec filtrage adverse/
	//Sinon : pas de chemin !
	//AUTREMENT DIT.... (la m�me chose en diff�rent)
	//Si le noeud 'from' (le plus proche parmi ceux qui ne sont pas recouvert par des adversaires)
	//		n'est pas dans la liste des voisins du noeud r�el le plus proche (from_without_adversaries)..
	//Alors, on ne peut pas consid�rer que l'on se trouve sur le noeud from...
	if((from != from_without_adversaries) && !(PATHFIND_TST_NODE_IN(from, nodes[from_without_adversaries].neighbors)))
		return NOT_HANDLED;

	//Une meilleure piste devrait �tre dans ce cas de choisir un point calcul� permettant de s'extraire de l'adversaire.


	pathfind_debug_printf ("x:%d | y:%d | from:%d | to:%d\n", xFrom, yFrom, from, to);

	/* On reinitialise les listes et penalites */
	openList = 0;
	closedList = 0;

#ifndef OLD_PATHFIND
	if(global.env.color == GREEN)
		closedList = closedList | (1<<1)|(1<<2); // Supprime les nodes au pr�s du spawn Jaune
	else
		closedList = closedList | (1<<20)|(1<<21); // Supprime les nodes au pr�s du spawn Vert
#endif

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
		minCost = 0xFFFF;	//On suppose que le cout est max.
		current = 0;
		for (n = 0; n < PATHFIND_NODE_NB; n++) {
			if (PATHFIND_TST_NODE_IN(n, openList) && nodes[n].total_cost < minCost) {
				minCost = nodes[n].total_cost;
				current = n;
			}
		}

		/* On passe la case en cours dans la liste fermee */
		PATHFIND_CLR_NODE_IN(current, openList);
		PATHFIND_SET_NODE_IN(current, closedList);
		//pathfind_debug_printf("current open->close %d\n", current);

		/* Pour toutes les cases adjacentes n'etant pas dans la liste fermee */
		for (n = 0; n < PATHFIND_NODE_NB; n++) {

			if ( (PATHFIND_TST_NODE_IN(n, nodes[current].neighbors)) &&
				!(PATHFIND_TST_NODE_IN(n, closedList)) &&
				!(PATHFIND_TST_NODE_IN(n,adversaries_nodes)) )
			{
				cost = Pathfind_cost(n, to, TRUE);
				/*
				 * Si elle n'est pas dans la liste ouverte, on l'y ajoute.
				 * La case en cours devient le parent de cette case.
				 * On calcule les couts F, G et H de cette case.
				 */
				if (!PATHFIND_TST_NODE_IN(n, openList))
				{
					PATHFIND_SET_NODE_IN(n, openList);
					nodes[n].parent = current;
					nodes[n].cost = cost;
					nodes[n].total_cost = nodes[current].total_cost + cost;
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

					if (cost < nodes[n].cost) {
						nodes[n].cost = cost;
						nodes[n].total_cost = nodes[current].total_cost + cost;
						nodes[n].parent = current;
						nodes[n].nb_nodes = nodes[current].nb_nodes + 1;
					}
				}
			}
		}
	}

	/* Si le chemin n'a pas �t� trouv� */
	if (!PATHFIND_TST_NODE_IN(to, closedList))
		return NOT_HANDLED;


	/*le noeud de d�part ne doit pas avoir de parent*/
	nodes[from].parent=from;

	/* Permet d'optimiser les deplacements*/
	//PATHFIND_delete_useless_node(from, to);
	//for (n = 0; n < PATHFIND_NODE_NB; n++) {
	//	if (PATHFIND_TST_NODE_IN(n, closedList))
	//		pathfind_debug_printf(" Node %d : cost = %d | total_cost = %d | parent = %d\n", n, nodes[n].cost, nodes[n].total_cost, nodes[n].parent);
	//}
	/* On a le chemin inverse (to->from) */
	nb_displacements = nodes[to].nb_nodes;
	n = to;
	suivant = to;

	if(nb_displacements>1 && PATHFIND_manhattan_dist(xFrom, yFrom, nodes[from].x, nodes[from].y) < IGNORE_FIRST_POINT_DISTANCE)
	{	//Si le premier noeud est trop proche de nous pour qu'il faille le rejoindre, on consid�re qu'on y est d�j�
		nb_displacements--;	//Le premier d�placement ne compte pas...
		//Comme ce serait le dernier � �tre enregistr�... il ne sera pas enregistr�.
		pathfind_debug_printf("Le node %d est proche de nous. On ne va pas s'y rendre, on va sur le point suivant.\n",from);
	}
	pathfind_debug_printf("Nodes : ");
	for(i=0;i<nb_displacements;i++)
	{
		displacements[nb_displacements-i-1].point.x = nodes[n].x;
		displacements[nb_displacements-i-1].point.y = nodes[n].y;
		if(suivant != to)
			displacements[nb_displacements-i-1+2].curve = (node_curve[n] & (1<<suivant))?TRUE:FALSE;
		// On attribue le droit � la trajectoire de faire une courbe si :
		// une trajectoire du node courant (n) vers le node suivant (suivant)
		//		autorise une trajectoire pour le d�placement d'apr�s (nb_displacements-i-1+2)

		pathfind_debug_printf("%d <- ",n);
		suivant = n;
		n = nodes[n].parent;
	}
	if(nb_displacements > 0)
		displacements[0].curve = FALSE;
	if(nb_displacements > 1)
		displacements[1].curve = FALSE;
	pathfind_debug_printf(" = %d displacements\n", nb_displacements);
	*p_nb_displacements = nb_displacements;
	return END_OK;
}

//Retourne le nombre de d�placements, ou 0 si pas de chemin possible
error_e PATHFIND_compute_new(displacement_curve_t * displacements, Uint8 * p_nb_displacements, Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to)
{
	pathfind_node_id_t from, n, current, from_without_adversaries, suivant;
	pathfind_node_list_t adversaries_nodes;
	Uint16 minLengthPath, lengthPath, distEnd;
	Uint8 i;
	Uint8 nb_displacements = 0;
	//On identifie les noeuds plac�s � moins de 500mm manhattan d'un adversaire.
	adversaries_nodes = 0;
	for(i=0;i<MAX_NB_FOES;i++)
	{
		if(global.env.foe[i].enable && global.env.foe[i].dist < DISTANCE_CONSIDERE_ADVERSARY)	//Pour chaque adversaire situ� proche de nous...
		{
			for(n = 0; n < PATHFIND_NODE_NB; n++)	//Pour chaque noeud
			{	//Si l'adversaire en question est proche du noeud : on ajoute le noeud dans la liste des noeuds innaccessibles.

				if(PATHFIND_manhattan_dist(nodes[n].x, nodes[n].y, global.env.foe[i].x, global.env.foe[i].y)<MANHATTAN_DIST_NODE_BLOQUED_BY_ADVERSARY)
				{
					PATHFIND_SET_NODE_IN(n,adversaries_nodes);
					SD_printf("Adv%d in node %d\n",i,n);
				}
			}
		}
	}

	from_without_adversaries = 	PATHFIND_closestNode(xFrom, yFrom, 0);
	from = 						PATHFIND_closestNode(xFrom, yFrom, adversaries_nodes);	//On cherche le noeud le plus proche (en enlevant les noeuds occup�s par l'adversaire)
	//from =						PATHFIND_closestNodeToEnd(xFrom, yFrom, adversaries_nodes, PATHFIND_get_node_x(to), PATHFIND_get_node_y(to));

	if(from == NOT_IN_NODE)
		return NOT_HANDLED;	//Pas de chemin possible... c'est d'ailleurs tr�s �trange...

	//si le noeud le plus proche est un noeud situ� de l'autre cot� d'un obstacle car les adversaires emp�chent l'acc�s aux autres noeuds !!!!
	//Le noeud le plus proche sans filtrage adverse... correspond � notre position, doit permettre d'acc�der par la logique des voisinages au noeud le plus proche avec filtrage adverse/
	//Sinon : pas de chemin !
	//AUTREMENT DIT.... (la m�me chose en diff�rent)
	//Si le noeud 'from' (le plus proche parmi ceux qui ne sont pas recouvert par des adversaires)
	//		n'est pas dans la liste des voisins du noeud r�el le plus proche (from_without_adversaries)..
	//Alors, on ne peut pas consid�rer que l'on se trouve sur le noeud from...
	if((from != from_without_adversaries) && !(PATHFIND_TST_NODE_IN(from, nodes[from_without_adversaries].neighbors)))
		return NOT_HANDLED;

	//Une meilleure piste devrait �tre dans ce cas de choisir un point calcul� permettant de s'extraire de l'adversaire.


	pathfind_debug_printf ("x:%d | y:%d | from:%d | to:%d\n", xFrom, yFrom, from, to);

	/* On reinitialise les listes et penalites */
	openList = 0;
	closedList = 0;

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
		minLengthPath = 0xFFFF;	//On suppose que le cout est max.
		current = 0;
		for (n = 0; n < PATHFIND_NODE_NB; n++) {
			if (PATHFIND_TST_NODE_IN(n, openList) && nodes[n].total_cost < minLengthPath) {
				minLengthPath = nodes[n].total_cost;
				current = n;
			}
		}

		/* On passe la case en cours dans la liste fermee */
		PATHFIND_CLR_NODE_IN(current, openList);
		PATHFIND_SET_NODE_IN(current, closedList);
		//pathfind_debug_printf("current open->close %d\n", current);

		/* Pour toutes les cases adjacentes n'etant pas dans la liste fermee */
		for (n = 0; n < PATHFIND_NODE_NB; n++) {

			if ( (PATHFIND_TST_NODE_IN(n, nodes[current].neighbors)) &&
				!(PATHFIND_TST_NODE_IN(n, closedList)) &&
				!(PATHFIND_TST_NODE_IN(n,adversaries_nodes)) )
			{
				lengthPath = Pathfind_cost_new(current, n, FALSE);
				distEnd = Pathfind_cost_new(n, to, FALSE) * FACTEUR_DIRECTIF;

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

					if (lengthPath < nodes[n].length_path) {
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

	/* Si le chemin n'a pas �t� trouv� */
	if (!PATHFIND_TST_NODE_IN(to, closedList))
		return NOT_HANDLED;


	/*le noeud de d�part ne doit pas avoir de parent*/
	nodes[from].parent=from;

	/* Permet d'optimiser les deplacements*/
	//PATHFIND_delete_useless_node(from, to);
	//for (n = 0; n < PATHFIND_NODE_NB; n++) {
	//	if (PATHFIND_TST_NODE_IN(n, closedList))
	//		pathfind_debug_printf(" Node %d : cost = %d | total_cost = %d | parent = %d\n", n, nodes[n].cost, nodes[n].total_cost, nodes[n].parent);
	//}
	/* On a le chemin inverse (to->from) */
	nb_displacements = nodes[to].nb_nodes;
	n = to;
	suivant = to;

	if(nb_displacements>1 && PATHFIND_manhattan_dist(xFrom, yFrom, nodes[from].x, nodes[from].y) < IGNORE_FIRST_POINT_DISTANCE)
	{	//Si le premier noeud est trop proche de nous pour qu'il faille le rejoindre, on consid�re qu'on y est d�j�
		nb_displacements--;	//Le premier d�placement ne compte pas...
		//Comme ce serait le dernier � �tre enregistr�... il ne sera pas enregistr�.
		pathfind_debug_printf("Le node %d est proche de nous. On ne va pas s'y rendre, on va sur le point suivant.\n",from);
	}
	pathfind_debug_printf("Nodes : ");
	for(i=0;i<nb_displacements;i++)
	{
		displacements[nb_displacements-i-1].point.x = nodes[n].x;
		displacements[nb_displacements-i-1].point.y = nodes[n].y;
		if(suivant != to)
			displacements[nb_displacements-i-1+2].curve = (node_curve[n] & (1<<suivant))?TRUE:FALSE;
		// On attribue le droit � la trajectoire de faire une courbe si :
		// une trajectoire du node courant (n) vers le node suivant (suivant)
		//		autorise une trajectoire pour le d�placement d'apr�s (nb_displacements-i-1+2)

		pathfind_debug_printf("%d <- ",n);
		suivant = n;
		n = nodes[n].parent;
	}
	if(nb_displacements > 0)
		displacements[0].curve = FALSE;
	if(nb_displacements > 1)
		displacements[1].curve = FALSE;
	pathfind_debug_printf(" = %d displacements\n", nb_displacements);
	*p_nb_displacements = nb_displacements;
	return END_OK;
}

/*
 * DODGE_AND_WAIT / DODGE_AND_NO_WAIT : lorsqu'un adversaire est d�tect�, l'�vittement est tent� en recalculant une alternative.
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
			pathfind_debug_printf("Compute\n");
			//Calcul d'un chemin pour atteindre l'objectif.
			if(PATHFIND_compute(displacements, &nb_displacements, global.env.pos.x, global.env.pos.y, node_wanted) != END_OK)
			{
				SD_printf("I cannot find any path. x=%d y=%d to node %d\n",global.env.pos.x, global.env.pos.y, node_wanted);
				state = FAIL;
			}
			else
			{
				if(nb_displacements)	//on a trouv� des d�placements � faire
				{
					for(i=0;i<nb_displacements;i++)
					{
						pathfind_debug_printf("[%d,%d]->\n",displacements[i].point.x, displacements[i].point.y);
						displacements[i].speed = speed;
					}
					state = DISPLACEMENT;
				}
				else
				{
					pathfind_debug_printf("No displacements, we are already on the point\n");
					state = SUCCESS;	//aucun d�placement n'est n�cessaire, on est d�j� sur le point...
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


#ifdef OLD_PATHFIND
Uint16 PATHFING_get_symetric(Uint8 n){
	if(global.env.color == BLUE){
		if(n<2)
			return n+22;
		if(n<6 && n>1)
			return n +16;
		if(n<10 && n>5)
			return n+8;
		if(n>13 && n<18)
			return n-8;
		if(n>17 && n<22)
			return n-16;
		if(n>21 && n<24)
			return n-22;
	}
	return n;
}
#else
Uint16 PATHFING_get_symetric(Uint8 n){
	if(global.env.color == BLUE){
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
#endif


// Retourne le node le plus proche de notre position
pathfind_node_id_t min_node_dist(pathfind_node_id_t n1,pathfind_node_id_t n2){
	if(PATHFIND_manhattan_dist(PATHFIND_get_node_x(n1),PATHFIND_get_node_y(n1),global.env.pos.x,global.env.pos.y)
			< PATHFIND_manhattan_dist(PATHFIND_get_node_x(n2),PATHFIND_get_node_y(n2),global.env.pos.x,global.env.pos.y))
		return n1;
	else
		return n2;
}


/* Fonction de s�lection du point le plus proche en fonction du point final voulue
 * TODO : Finir et tester cette fonction
*/
pathfind_node_id_t PATHFIND_closestNodeToEnd(Sint16 x, Sint16 y, Uint32 filteredNodes, Sint16 final_x, Sint16 final_y)
{
	typedef struct{
		Uint16 x;
		Uint16 y;
	}vecteur_s;
	Uint8 i;
	Uint16 dist, minDist = 65535, minAngle;
	pathfind_node_id_t n, closestNode = NOT_IN_NODE, closestNodes[4] = {NOT_IN_NODE, NOT_IN_NODE, NOT_IN_NODE, NOT_IN_NODE};
	vecteur_s vecteur[5];
	float angle_vector[4];

	// On trouve les 4 plus proches node
	for(i=0; i<4; i++){
		for (n = 0; n < PATHFIND_NODE_NB; n++)
		{
			if (PATHFIND_TST_NODE_IN(n, filteredNodes) == FALSE)
			{
				dist = PATHFIND_manhattan_dist(x, y, nodes[n].x, nodes[n].y);
				if (dist < minDist)
				{
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
			vecteur[i].x = x-PATHFIND_get_node_x(closestNodes[i]);
			vecteur[i].y = y-PATHFIND_get_node_y(closestNodes[i]);
		}else
			vecteur[i] = (vecteur_s){0,0};
	}

	// On calcule le vecteur entre nous et le node final
	vecteur[4].x = final_x-x;
	vecteur[4].y = final_y-y;

	// On calcule l'angle entre chaque vecteur (node -> nous) et le vecteur final (nous -> fin)
	for(i=0;i<4;i++){
		if(closestNodes[i] != NOT_IN_NODE)
			angle_vector[i] = acos(vecteur[i].x*final_x + vecteur[i].y*final_y /
									(sqrt(SQUARE((float)vecteur[i].x) + SQUARE((float)vecteur[i].y))
										* sqrt(SQUARE((float)final_x) + SQUARE((float)final_y))));
	}

	// On cherche qui a le plus petit angle entre chaque vecteur (node -> nous) et le vecteur final (nous -> fin)
	closestNode = closestNodes[0];
	minAngle = angle_vector[0];
	for(i=1;i<4;i++){
		if(minAngle > angle_vector[i]){
			minAngle = angle_vector[i];
			closestNode = closestNodes[i];
		}
	}

	// On retourne le node le plus optimis�
	return closestNode;
}

Sint16 PATHFIND_dist_node_to_node(pathfind_node_id_t n1, pathfind_node_id_t n2){
	return dist_point_to_point(nodes[n1].x, nodes[n1].y, nodes[n2].x, nodes[n2].y);
}

Sint16 path_length(pathfind_node_id_t* tab_node, Uint8 nb_node){
	Uint8 i;
	Sint16 length = 0;
	for(i=0;i<nb_node-1;i++){
		length += PATHFIND_dist_node_to_node(tab_node[i], tab_node[i+1]);
	}
	return length;
}

#if VIEILLE_FONCTION_OBSOLETE


Uint16 PATHFIND_compute(Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to, PROP_speed_e speed, way_e way, bool_e handleOpponent)
{
	pathfind_node_id_t from, n, current;

	Uint16 minCost, heuristic;

	from = PATHFIND_closestNode(xFrom, yFrom, handleOpponent);

	pathfind_debug_printf ("Noeud le plus proche : %d", from);

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

	/* Si le chemin n'a pas �t� trouv� */
	if (!PATHFIND_TST_NODE_IN(to, closedList))
	{
		STACKS_push(PROP, &wait_forever, TRUE);
		return PATHFIND_OPPONENT_COST;
	}


	/*le noeud de d�part ne doit pas avoir de parent*/
	nodes[from].parent=from;

	/* Permet d'optimiser les deplacements*/
	//PATHFIND_delete_useless_node(from, to);

	/* On a le chemin inverse (to->from) */
	for(n = to; n != from; n = nodes[n].parent)
	{
		// on ne renvoie que le co�t suppl�mentaire, on enl�ve donc le co�t normal
		// pour passer d'un noeud � l'autre
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
		// on ne renvoie que le co�t suppl�mentaire, on enl�ve donc le co�t normal
		// pour passer d'un noeud � l'autre
		if (nodes[to].parent != from) {
			// Permet de passer par un noeud interm�diaire dans la trajectoire (le robot ne sait pas encore passer � travers les ma�s)
			nodes[to].cost -= PATHFIND_STEP_COST;
		}
	}

	STACKS_push(PROP, &wait_forever, TRUE);

	return nodes[to].cost;
}
#endif

#endif
