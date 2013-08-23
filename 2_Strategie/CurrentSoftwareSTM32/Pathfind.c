/*
 *	Club Robot ESEO 2011 - 2012
 *	Shark & Fish
 *
 *	Fichier : Pathfind.c
 *	Package : Carte Principale
 *	Description : définition des noeuds et des fonctions de pathfind
 *	Auteur : Louis-Marie, Vincent, Antoine
 *	Version 2012/03/04
 */

#include "Pathfind.h"


#ifndef USE_POLYGON

//Robot 2011
static pathfind_node_t nodes[PATHFIND_NODE_NB] =
{
	//Milieu
	(pathfind_node_t){ 270, 1500, neighbors : (1<<1)|(1<<3)|(1<<4)|(1<<5)|(1<<8)|(1<<9)|(1<<10)},			//[0]
	(pathfind_node_t){ 625, 1500, neighbors : (1<<0)|(1<<4)|(1<<5)|(1<<8)|(1<<9)|(1<<10)},	       			//[1]
	(pathfind_node_t){ 1375, 1500, neighbors : (1<<6)|(1<<7)|(1<<11)|(1<<12)},		              			//[2]
	
	//Coté Violet
	(pathfind_node_t){ 860, 450, neighbors : (1<<0)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<12)},					//[3]
	(pathfind_node_t){ 270, 742, neighbors : (1<<0)|(1<<1)|(1<<5)|(1<<6)|(1<<9)|(1<<10)},					//[4]
	(pathfind_node_t){ 625, 742, neighbors : (1<<0)|(1<<1)|(1<<3)|(1<<4)|(1<<6)|(1<<9)|(1<<10)},			//[5]
	(pathfind_node_t){ 1375, 742, neighbors :(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<7)|(1<<11)|(1<<12)},			//[6]
	(pathfind_node_t){ 1700, 1117, neighbors :(1<<2)|(1<<3)|(1<<6)|(1<<11)|(1<<12)},						//[7]	
	
	
	//Coté Rouge
	(pathfind_node_t){ 860, 2550, neighbors : (1<<0)|(1<<7)|(1<<9)|(1<<10)|(1<<11)|(1<<12)},				//[8]
	(pathfind_node_t){ 270, 2258, neighbors : (1<<0)|(1<<1)|(1<<4)|(1<<10)|(1<<11)},						//[9]
	(pathfind_node_t){ 625, 2258, neighbors : (1<<0)|(1<<1)|(1<<4)|(1<<5)|(1<<8)|(1<<9)|(1<<11)},			//[10]
	(pathfind_node_t){ 1375, 2258, neighbors :(1<<2)|(1<<6)|(1<<7)|(1<<8)|(1<<9)|(1<<10)|(1<<12)},			//[11]
	(pathfind_node_t){ 1700, 1883, neighbors :(1<<2)|(1<<6)|(1<<7)|(1<<8)|(1<<11)},							//[12]	
	(pathfind_node_t){ 0, 0, neighbors : 0} //[13](invalid)
};


static pathfind_node_list_t openList;
static pathfind_node_list_t closedList;

static pathfind_node_id_t nodeOpponent[NB_FOES];


void PATHFIND_updateOpponentPosition(foe_e foe_id)
{
	//On desactive l'evitement pour trouver le noeud le plus proche de l'adversaire
	pathfind_node_id_t node = PATHFIND_closestNode(global.env.foe[foe_id].x, global.env.foe[foe_id].y, FALSE);
	//si on est trop loin
	int dist = GEOMETRY_squared_distance((GEOMETRY_point_t){nodes[node].x,nodes[node].y},(GEOMETRY_point_t){global.env.foe[foe_id].x,global.env.foe[foe_id].y});
	if(dist>220)
	{
		nodeOpponent[foe_id] = 13;//invalid
		debug_printf("OPPONENT NOT ON A NODE (13) cause dist=%d\n", dist);
	}	
	debug_printf("UPDATE OPPONENT POSITION FOE %d: %d\n", foe_id, nodeOpponent[foe_id]);
}

Uint16 PATHFIND_manhattan_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1>x2 ? x1-x2 : x2-x1) + (y1>y2 ? y1-y2 : y2-y1);
}

Uint32 PATHFIND_squared_dist(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
	return (x1-x2) * (x1-x2) +  (y1-y2) * (y1-y2);
}

pathfind_node_id_t PATHFIND_closestNode(Sint16 x, Sint16 y, bool_e handleOpponent)
{
	Uint16 dist, minDist = 65535;
	pathfind_node_id_t n, closestNode = 0;

	for (n = 0; n < PATHFIND_NODE_NB; n++)
	{
		if (!(handleOpponent && ((n == nodeOpponent[FOE_1]) || (n == nodeOpponent[FOE_2])))) {
			dist = PATHFIND_manhattan_dist(x, y, nodes[n].x, nodes[n].y);
			if (dist < minDist) {
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

/*
 * Estimation de la distance necessaire pour aller d'un noeud a l'autre,
 * plus cette distance est proche de la realite, plus l'algorithme sera
 * rapide.
 *
 * TODO précalculer toutes les possibilités pour avoir un heuristic exact ?
 */
Uint16 Pathfind_heuristic(pathfind_node_id_t from, pathfind_node_id_t to, bool_e handleOpponent)
{
	Uint8 i;
	Uint32 dist;
	/* On se base sur un manhattan */
	Uint16 heuristic = PATHFIND_manhattan_dist(nodes[from].x, nodes[from].y, nodes[to].x, nodes[to].y);

	if (handleOpponent)
	{
	/*
	 * Si l'adversaire est à moins de 90 cm du noeud, on
	 * ajoute une pénalité importante, inversement proportionnelle à la distance adversaire-noeud
	 */

		for(i=0; i<NB_FOES; i++)
		{
			dist = PATHFIND_squared_dist(nodes[from].x, nodes[from].y, global.env.foe[i].x, global.env.foe[i].y);
			if (dist < ((Uint32)900*900))
			{
				heuristic += (((Uint32)900*900) - dist) / 20;
			}
		}	
	}
	return heuristic;
}

Sint16 PATHFIND_get_node_x (pathfind_node_id_t n) {
	return nodes[n].x;
}

Sint16 PATHFIND_get_node_y (pathfind_node_id_t n) {
	return nodes[n].y;
}

pathfind_node_id_t PATHFIND_opponent_node(foe_e foe_id)
{
	return nodeOpponent[foe_id];
}

bool_e PATHFIND_is_opponent_in_path(foe_e foe_id)
{
	return ASSER_has_goto(nodes[nodeOpponent[foe_id]].x, nodes[nodeOpponent[foe_id]].y);
}

pathfind_node_id_t  PATHFIND_random_neighbor(pathfind_node_id_t of, bool_e handleOpponent[NB_FOES])
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

/*
 * Cree par Alexis
 */
void PATHFIND_delete_useless_node(pathfind_node_id_t from, pathfind_node_id_t to)
{
	pathfind_node_id_t before, current, after;
	pathfind_node_id_t xb, yb, xc, yc, xa, ya;

	before = to;
	current=nodes[to].parent;

	/*
	 * On parcours le chemin calcule
	 */
	while(current!=from)
	{
		after=nodes[current].parent;

		xb=nodes[before].x; yb=nodes[before].y;
		xc=nodes[current].x; yc=nodes[current].y;
		xa=nodes[after].x; ya=nodes[after].y;


		if ( ((xb==xc) && (xc==xa)) || ((yb==yc) && (yc==ya)) )
		{
			/*
		 	* Si trois noeuds sont strictement alignes,
		 	* celui du milieu est supprime
		 	*/
		 	nodes[before].parent=after;

			after=nodes[(nodes[after].parent)].parent;
			current=nodes[after].parent;
			before=current;
		}
		else
		{
			/*
			 * sinon on decalle simplement les noeuds a etudier
		 	 */
			after=nodes[after].parent;
			current=after;
			before=current;
		}
	}
}


Uint16 PATHFIND_compute(Sint16 xFrom, Sint16 yFrom, pathfind_node_id_t to, ASSER_speed_e speed, way_e way, bool_e handleOpponent)
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
				 * Si elle est deja  dans la liste ouverte, on teste si le
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
		STACKS_push(ASSER, &wait_forever, TRUE);
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

		#ifdef USE_ASSER_MULTI_POINT
			ASSER_push_goto_multi_point(nodes[n].x, nodes[n].y, speed, way, 0, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);
		#else
			ASSER_push_goto(nodes[n].x, nodes[n].y, speed, way, 0, END_AT_LAST_POINT, FALSE);
		#endif
		nodes[to].cost -= PATHFIND_STEP_COST;
	}


	if ((to == from) || /** Le noeud de destination est le plus proche */
		(PATHFIND_squared_dist(nodes[n].x, nodes[n].y, xFrom, yFrom) > ((Uint32)NODE_PROXIMITY_DISTANCE*NODE_PROXIMITY_DISTANCE))
	) {

			#ifdef USE_ASSER_MULTI_POINT
				ASSER_push_goto_multi_point(nodes[from].x, nodes[from].y, speed, way, 1, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);
			#else
				ASSER_push_goto(nodes[from].x, nodes[from].y, speed, way, 1, END_AT_LAST_POINT, FALSE);
			#endif
		// on ne renvoie que le coût supplémentaire, on enlève donc le coût normal
		// pour passer d'un noeud à l'autre
		if (nodes[to].parent != from) {
			// Permet de passer par un noeud intermédiaire dans la trajectoire (le robot ne sait pas encore passer à travers les maïs)
			nodes[to].cost -= PATHFIND_STEP_COST;
		}
	}

	STACKS_push(ASSER, &wait_forever, TRUE);

	return nodes[to].cost;
}

#endif
