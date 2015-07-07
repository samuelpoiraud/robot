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

#include "astar.h"

//**********************************************  Variables globales  ********************************************
//La liste des polygones
astar_polygon_list_t polygon_list;

//La liste ouverte pour l'algo A*
astar_list_t opened_list;

//La liste fermée pour l'algo A*
astar_list_t closed_list;

//***************************************************  Fonctions  ************************************************
//Fonctions pour créer les polygones correspondant aux robots adverses
void ASTAR_create_foe_polygon(Uint8 *currentId){
	int i = 0;
	int nbFoes = 0;
	GEOMETRY_point_t foe;
	Uint16 dist;
	Uint16 dist_sin, dist_cos;

	for(i=0; i<MAX_NB_FOES ; i++){
		if(global.foe[i].enable){
			foe.x = global.foe[i].x;
			foe.y = global.foe[i].y;

			//Si le petit robot adverse est très petit, inutile de surdimensionné l'hexagone
			dist = MIN(DEFAULT_FOE_RADIUS, GEOMETRY_distance(foe, (GEOMETRY_point_t){global.pos.x, global.pos.y})- ROBOT_WIDTH/2);
			dist_sin = dist*sin(30);
			dist_cos = dist*cos(30);

			polygon_list.polygons[nbFoes].summits[0].pos.x = foe.x + dist;
			polygon_list.polygons[nbFoes].summits[0].pos.y = foe.y;

			polygon_list.polygons[nbFoes].summits[1].pos.x = foe.x + dist_sin;
			polygon_list.polygons[nbFoes].summits[1].pos.y = foe.y + dist_cos;

			polygon_list.polygons[nbFoes].summits[2].pos.x = foe.x - dist_sin;
			polygon_list.polygons[nbFoes].summits[2].pos.y = foe.y + dist_cos;

			polygon_list.polygons[nbFoes].summits[3].pos.x = foe.x - dist;
			polygon_list.polygons[nbFoes].summits[3].pos.y = foe.y;

			polygon_list.polygons[nbFoes].summits[4].pos.x = foe.x - dist_sin;
			polygon_list.polygons[nbFoes].summits[4].pos.y = foe.y - dist_cos;

			polygon_list.polygons[nbFoes].summits[5].pos.x = foe.x + dist_sin;
			polygon_list.polygons[nbFoes].summits[5].pos.y = foe.y - dist_cos;

			polygon_list.polygons[nbFoes].id = nbFoes;
			polygon_list.polygons[nbFoes].nbSummits = 6;
			polygon_list.polygons[nbFoes].enable = TRUE;

			for(i=0; i< 6; i++){
				polygon_list.polygons[nbFoes].summits[i].id = *currentId + i;
				polygon_list.polygons[nbFoes].summits[i].polygonId = polygon_list.polygons[nbFoes].id;
				polygon_list.polygons[nbFoes].summits[i].enable = TRUE;
				polygon_list.polygons[nbFoes].summits[i].cost.total = MAX_COST;
				polygon_list.polygons[nbFoes].summits[i].cost.heuristic = MAX_COST;
				polygon_list.polygons[nbFoes].summits[i].cost.step = MAX_COST;
				polygon_list.polygons[nbFoes].summits[i].parent = NULL ;
				polygon_list.polygons[nbFoes].summits[i].nbNeighbors = 0;
			}
			nbFoes++;
			*currentId = *currentId + 6;
		}
	}
	polygon_list.nbPolygons += nbFoes;
	debug_printf("nbFoes = %d\n", nbFoes);
}


//Fonctions pour créer les polygones correspondant aux zones ou éléments du terrain constituant des obstacles
void ASTAR_create_element_polygon(Uint8 *currentId, Uint8 nbSummits,...){
	int i;
	int index = polygon_list.nbPolygons;
	va_list listePoints;
	va_start(listePoints, nbSummits);
	GEOMETRY_point_t point;
	for(i=0; i< nbSummits; i++){
		point = va_arg(listePoints, GEOMETRY_point_t);
		polygon_list.polygons[index].summits[i].id = ((*currentId)++);
		polygon_list.polygons[index].summits[i].polygonId = index;
		polygon_list.polygons[index].summits[i].pos = point;
		polygon_list.polygons[index].summits[i].enable = TRUE;
		polygon_list.polygons[index].summits[i].cost.total = MAX_COST;
		polygon_list.polygons[index].summits[i].cost.heuristic = MAX_COST;
		polygon_list.polygons[index].summits[i].cost.step = MAX_COST;
		polygon_list.polygons[index].summits[i].parent = NULL;
		polygon_list.polygons[index].summits[i].nbNeighbors = 0;
	}
	polygon_list.polygons[index].id = index;
	polygon_list.polygons[index].nbSummits = nbSummits;
	polygon_list.polygons[index].enable = TRUE;
	polygon_list.nbPolygons++;
}

//Fonction pour vérifier si un point est présent dans l'aire de jeu
bool_e ASTAR_node_enable(astar_ptr_node_t nodeTested, bool_e withPolygons){
	int i;
	if(nodeTested->pos.x < MARGIN_TO_BORDER || nodeTested->pos.x > PLAYGROUND_WIDTH-MARGIN_TO_BORDER || nodeTested->pos.y < MARGIN_TO_BORDER || nodeTested->pos.y > PLAYGROUND_HEIGHT-MARGIN_TO_BORDER)
		return FALSE;

	if(withPolygons){
		for(i=0; i<polygon_list.nbPolygons; i++){
			if(polygon_list.polygons[i].enable && polygon_list.polygons[i].id != nodeTested->polygonId && !ASTAR_point_out_of_polygon(polygon_list.polygons[i], nodeTested->pos))
				return FALSE;
		}
	}
	return TRUE;
}


//Fonction pour vérifier si un point est à l'extérieur d'un polygone
bool_e ASTAR_point_out_of_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested){
	int nbIntersections=0;
	int i;
	GEOMETRY_segment_t seg1 = {nodeTested, (GEOMETRY_point_t){2200, 3200}};
	//debug_printf("x=%d   y=%d\n", nodeTested.x, nodeTested.y);
	for(i=0; i<polygon.nbSummits-1; i++){
		GEOMETRY_segment_t seg2 = {polygon.summits[i].pos, polygon.summits[i+1].pos};
		nbIntersections += GEOMETRY_segments_intersects(seg1, seg2);
		//debug_printf("polygon.summits[%d].pos: x=%d   y=%d \n", i, polygon.summits[i].pos.x, polygon.summits[i].pos.y);
		//debug_printf("polygon.summits[%d].pos: x=%d   y=%d \n", i+1, polygon.summits[i+1].pos.x, polygon.summits[i+1].pos.y);
		//debug_printf("nbIntersections = %d\n", nbIntersections );
	}

	//Test de l'intersection avec le dernier segment
	GEOMETRY_segment_t seg2 = {polygon.summits[polygon.nbSummits-1].pos, polygon.summits[0].pos};
	nbIntersections += GEOMETRY_segments_intersects(seg1, seg2);
	//debug_printf("polygon.summits[%d].pos: x=%d   y=%d \n", polygon.nbSummits-1, polygon.summits[polygon.nbSummits-1].pos.x, polygon.summits[polygon.nbSummits-1].pos.y);
	//debug_printf("polygon.summits[%d].pos: x=%d   y=%d \n", 0, polygon.summits[0].pos.x, polygon.summits[0].pos.y);
	//debug_printf("nbIntersections = %d\n", nbIntersections );
	//debug_printf("Point_out_of_polygon = %d\n", (nbIntersections%2 == 0));
	return  (nbIntersections%2 == 0);
}


//Fonction générant la liste de polygones (adversaires + éléments ou zones de jeu)
void ASTAR_generate_polygon_list(){
	Uint8 currentNodeId = 0;
	polygon_list.nbPolygons = 0;  //réinitialisation du nombre de polygones

	//Attention, les nodes doivent être écartés au maximum de 250mm sur un même segment

	//Polygones des robots adverses
	ASTAR_create_foe_polygon(&currentNodeId);

	//Notre zone de départ
	ASTAR_create_element_polygon(&currentNodeId, 7, (GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(0)},
									(GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(MARGIN_TO_OBSTACLE)},
									(GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE/1.4,COLOR_Y(400+MARGIN_TO_OBSTACLE/1.4)},
									(GEOMETRY_point_t){1000,COLOR_Y(400+MARGIN_TO_OBSTACLE)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE/1.4,COLOR_Y(400+MARGIN_TO_OBSTACLE/1.4)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(MARGIN_TO_OBSTACLE)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(0)});

	//Zone de départ adverses
	ASTAR_create_element_polygon(&currentNodeId, 9, (GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(3000)},
									(GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(3000-MARGIN_TO_OBSTACLE)},
									(GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(2500)},
									(GEOMETRY_point_t){778-MARGIN_TO_OBSTACLE/1.4,COLOR_Y(2450-MARGIN_TO_OBSTACLE/1.4)},
									(GEOMETRY_point_t){1000,COLOR_Y(2400-MARGIN_TO_OBSTACLE)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE/1.4,COLOR_Y(2450-MARGIN_TO_OBSTACLE/1.4)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(2500)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(3000-MARGIN_TO_OBSTACLE)},
									(GEOMETRY_point_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(3000)});

	//Zones des marches
	ASTAR_create_element_polygon(&currentNodeId, 11, (GEOMETRY_point_t){0, 967-MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){MARGIN_TO_OBSTACLE, 967-MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE/1.4,967-MARGIN_TO_OBSTACLE/1.4},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE,1000},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE,1250},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE,1500},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE,1750},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE,2000},
									(GEOMETRY_point_t){580+MARGIN_TO_OBSTACLE/1.4,2033+MARGIN_TO_OBSTACLE/1.4},
									(GEOMETRY_point_t){MARGIN_TO_OBSTACLE, 2033+MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){0,2033+MARGIN_TO_OBSTACLE});

	//Zones de l'estrade
	ASTAR_create_element_polygon(&currentNodeId, 9, (GEOMETRY_point_t){1900-MARGIN_TO_OBSTACLE/1.4, 1200-MARGIN_TO_OBSTACLE/1.4},
									(GEOMETRY_point_t){2000-MARGIN_TO_OBSTACLE,1200-MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){2000,1200-MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){2000,1800+MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){2000-MARGIN_TO_OBSTACLE,1800+MARGIN_TO_OBSTACLE},
									(GEOMETRY_point_t){1900-MARGIN_TO_OBSTACLE/1.4,1800+MARGIN_TO_OBSTACLE/1.4},
									(GEOMETRY_point_t){1900-MARGIN_TO_OBSTACLE,1750},
									(GEOMETRY_point_t){1900-MARGIN_TO_OBSTACLE,1500},
									(GEOMETRY_point_t){1900-MARGIN_TO_OBSTACLE,1250});


}


//Fonction permettant de générer le graphe
void ASTAR_generate_graph(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination){
	Uint8 i, j;

	debug_printf("From x=%d  y=%d\n", from.x, from.y);

	//Création du node de départ
	path->from.id = 100;  /// TODO Modifier id
	path->from.polygonId = NO_POLYGON_ID;
	path->from.pos = from;
	path->from.enable = TRUE; //On considère que la position de départ du robot est valide.
	path->from.cost.total = 0; //Ca ne coute rien d'aller de from vers from
	path->from.cost.heuristic = 0; //de même
	path->from.cost.step = 0; //de même
	path->from.parent = NULL; //Le noeud de départ n'a pas de parent, c'est un noeud racine.
	path->from.nbNeighbors = 0;

	//Création du node d'arrivée
	path->destination.id = 101; /// TODO Modifier id
	path->destination.polygonId = NO_POLYGON_ID;
	path->destination.pos = destination;
	path->destination.enable = ASTAR_node_enable(&(path->destination), FALSE);
	path->destination.cost.total = MAX_COST; //On lui affecte un cout maximal
	path->destination.cost.heuristic = MAX_COST; //On lui affecte un cout maximal
	path->destination.cost.step = MAX_COST; //On lui affecte un cout maximal
	path->destination.parent = NULL; //on ne connait pas encore son parent
	path->destination.nbNeighbors = 0;

	//Vérification de la visibilité des noeuds et affectation du champ enable de chaque node
	//Cette action ne peut se faire que lorsque tous les polygones ont été générés
	for(i=0; i< polygon_list.nbPolygons; i++){
		if(polygon_list.polygons[i].enable){
			for(j=0; j< polygon_list.polygons[i].nbSummits; j++){
				//debug_printf("NodeTested  polygon[%d] node[%d] \n", i, j);
				polygon_list.polygons[i].summits[j].enable = ASTAR_node_enable(&(polygon_list.polygons[i].summits[j]), TRUE);
			}
		}
	}

	//Création des liens entre les noeuds d'un même polygone (cela revient à dessiner les polygônes)
	for(i=0; i<polygon_list.nbPolygons; i++){
		//Affectation du premier sommet (on lui affecte le second sommet et le dermier comme voisins)
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[0]), &(polygon_list.polygons[i].summits[1]));
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[0]), &(polygon_list.polygons[i].summits[ polygon_list.polygons[i].nbSummits-1]));

		//Affectation des sommets "normaux" (on lui affecte le sommet précédent et le sommet suivant comme voisins)
		for(j=1; j<polygon_list.polygons[i].nbSummits-1; j++){
			ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[i]), &(polygon_list.polygons[i].summits[i-1]));
			ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[i]), &(polygon_list.polygons[i].summits[i+1]));
		}

		//Affectation du dernier sommet (on lui affecte l'avant dernier sommet et le premier comme voisins)
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[i]), &(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-2]));
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[i]), &(polygon_list.polygons[i].summits[0]));
	}
}


//Fonction qui effectue l'algorithme A*
void ASTAR_pathfind(astar_path_t *path){
	//Déclaration des variables
	bool_e  destination_is_in_closed_list = FALSE;
	Uint16 minimal_cost; //Cout minimal ie cout entre le node de départ et le node courant
	astar_ptr_node_t current = NULL; //Le node courant
	Uint8 i; // Variable de parcours

	//Nettoyage des différentes listes
	ASTAR_clean_list(&(path->list)); //La liste contenant le chemin
	ASTAR_clean_list(&opened_list); //La liste ouverte
	ASTAR_clean_list(&closed_list); //La liste fermée

	//Ajout du noeud de départ à la liste ouverte
	ASTAR_add_node_to_list(&(path->from), &opened_list);

	//////// Boucle de l'algorithme A* ////////
	//Tant que la liste ouverte n'est pas vide et que le node de destination n'a pas été ajouté à la liste fermé
	while(!ASTAR_list_is_empty(opened_list) && !destination_is_in_closed_list){

		//Recherche dans la liste ouverte du node avec le cout le plus faible
		minimal_cost = MAX_COST;
		for(i=0; i<opened_list.nbNodes; i++){
			if(opened_list.list[i]->cost.total < minimal_cost){
				minimal_cost = opened_list.list[i]->cost.total;
				current = opened_list.list[i];
			}
		}

		//Ajout du noeud courant dans la closed_list et suppression de celui_ci dans l'opened_list
		ASTAR_add_node_to_list(current, &closed_list);
		ASTAR_delete_node_to_list(current, &opened_list);



		//On regarde si current est le node de destination (ou d'arrivée)
		if(current == &(path->destination)){
			destination_is_in_closed_list = TRUE;
			debug_printf("destination_is_in_closed_list\n");
		}


		if(!destination_is_in_closed_list){
			//Recherche des nodes adjacents (ou voisins) au node current. On ajoute ces nodes à la liste ouverte.
			ASTAR_link_nodes_on_path(current, &(path->destination));

			//Mise à jour des coûts des noeuds qui ont pour parent current
			ASTAR_update_cost(minimal_cost, current, &(path->destination));
		}

		debug_printf("\n\n\nNode current x=%d  y=%d\n", current->pos.x, current->pos.y);
		print_closed_list();
		print_opened_list();

	}

	//Reconstitution du chemin
	//ASTAR_make_the_path(path);
}

void ASTAR_link_nodes_on_path(astar_ptr_node_t from, astar_ptr_node_t destination){
	Uint16 minimal_dist = MAX_COST;
	Uint16 test_dist, test_cost;
	Uint8 i, j, k;
	GEOMETRY_segment_t reference = {from->pos, destination->pos};
	GEOMETRY_segment_t test;
	GEOMETRY_point_t intersection;
	astar_ptr_node_t nodeAnswer1 = NULL, nodeAnswer2 = NULL;
	bool_e is_in_closed_list = FALSE;
	bool_e is_in_opened_list = FALSE;
	for(i=0; i<polygon_list.nbPolygons; i++){
		if(polygon_list.polygons[i].enable && polygon_list.polygons[i].id != destination->polygonId){ //Le test n'est réalisé que si le polygone est "enable"
			for(j=0; j<polygon_list.polygons[i].nbSummits-1; j++){
				//On vérifie  si il y a une intersection avec chaque segment de polygone excepté le polygone auquel le node appartient
				//Le test n'est réalisé que si au moins une des deux extrémités est "enable".
				if(polygon_list.polygons[i].summits[j].enable || polygon_list.polygons[i].summits[j+1].enable){
					//Vérification de l'intersection
					test.a = polygon_list.polygons[i].summits[j].pos;
					test.b = polygon_list.polygons[i].summits[j+1].pos;
					if(GEOMETRY_segments_intersects(reference, test)){
						//Si il y a intersection, on calcule le point d'intersecsection
						debug_printf("Calcul intersection ....");
						intersection = ASTAR_intersection_is(reference, test);
						debug_printf("Finish\n");
						test_dist = GEOMETRY_distance(from->pos, intersection);
						if(test_dist < minimal_dist){
							debug_printf("minimal_dist affected\n");
							minimal_dist = test_dist;
							nodeAnswer1 = &(polygon_list.polygons[i].summits[j]);
							nodeAnswer2 = &(polygon_list.polygons[i].summits[j+1]);
						}
					}
				}
			}
			//Vérification du segment entre le premier et le dernier node de chaque polygone
			if(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].enable || polygon_list.polygons[i].summits[0].enable){
				//Vérification de l'intersection
				test.a = polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].pos;
				test.b = polygon_list.polygons[i].summits[0].pos;
				if(GEOMETRY_segments_intersects(reference, test)){
					//Si il y a intersection, on calcule le point d'intersecsection
					debug_printf("Calcul intersection ....");
					intersection = ASTAR_intersection_is(reference, test);
					debug_printf("Finish\n");
					test_dist = GEOMETRY_distance(from->pos, intersection);
					if(test_dist < minimal_dist){
						debug_printf("minimal_dist affected\n");
						minimal_dist = test_dist;
						*nodeAnswer1 = polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1];
						*nodeAnswer2 = polygon_list.polygons[i].summits[0];
					}
				}
			}
		}
	}

	debug_printf("Link nodes for x=%d  y=%d : nodeAnswer1  NULL=%d, nodeAnswer2 NULL=%d\n", destination->pos.x, destination->pos.y,((nodeAnswer1==NULL)? 1:0), ((nodeAnswer2==NULL)? 1:0) );

	//On regarde ensuite le résultat obtenu
	//Si les deux nodes answers sont NULL, alors le node destination peut être ajouté à la liste ouverte
	if(nodeAnswer1 == NULL && nodeAnswer2 == NULL){
		k = 0;
		is_in_closed_list = FALSE;
		while(k<closed_list.nbNodes && !is_in_closed_list){  //On vérifie qu'il n'est pas déjà dans la liste fermée
			if(closed_list.list[k]->id == destination->id)
				is_in_closed_list = TRUE;
			else
				k++;
		}

		if(!is_in_closed_list){
			k=0;
			is_in_opened_list = FALSE;
			while(k<opened_list.nbNodes && !is_in_opened_list){  //On vérifie qu'il n'est pas déjà dans la liste ouverte
				if(opened_list.list[k]->id == destination->id)
					is_in_opened_list = TRUE;
				else
					k++;
			}

			//Ajout du node à la liste ouverte
			if(!is_in_opened_list){
				ASTAR_add_node_to_list(destination, &opened_list);
				destination->parent = from;
				destination->cost.step = ASTAR_pathfind_cost(destination->parent, destination);
			}else{
				//Si le cout est inférieur, current devient le parent du node destination
				test_cost = ASTAR_pathfind_cost(from, destination);
				if(test_cost < destination->cost.step){
					destination->parent = from;
					destination->cost.step = test_cost;
				}
			}
		}
	}
	else  //On fait de la récursivité pour trouver d'autres voisins et être sur qu'on puisse les atteindre
	{
		debug_printf("Link nodes: Recursivité engagé");
		if(nodeAnswer1 != NULL && nodeAnswer1->enable)
			ASTAR_link_nodes_on_path(from, nodeAnswer1);
		if(nodeAnswer2 != NULL && nodeAnswer2->enable)
			ASTAR_link_nodes_on_path(from, nodeAnswer2);
	}
}


//Fonction pour la mise à jour des coûts des nodes qui ont pour parent from (le point de départ qui est en fait le node current)
void ASTAR_update_cost(Uint16 minimal_cost, astar_ptr_node_t from, astar_ptr_node_t destination){
	Uint8 i;
	for(i=0; i<opened_list.nbNodes; i++){
		if(opened_list.list[i]->parent == from){
			opened_list.list[i]->cost.heuristic = ASTAR_pathfind_cost(opened_list.list[i], destination);
			opened_list.list[i]->cost.total = minimal_cost + opened_list.list[i]->cost.step + opened_list.list[i]->cost.heuristic;
		}
	}
}

void ASTAR_make_the_path(astar_path_t *path){
	//Liste auxiliaire
	astar_list_t aux;
	Sint16 i;

	debug_printf("Make the path: ..........");

	//Nettoyage de la liste (Vaut mieux deux fois qu'une)
	ASTAR_clean_list(&(path->list));
	ASTAR_clean_list(&aux);

	debug_printf("Lists clean\n");

	//Quelque soit le résultat de A*, on reconstruit le chemin même si il est incomplet. On verra plus tard ce qu'on décide de faire.
	//Le chemin est reconstruit de la destination vers le point de départ
	ASTAR_add_node_to_list(closed_list.list[closed_list.nbNodes-1], &aux);
	while(aux.list[aux.nbNodes-1] != &(path->from)){  //Tant qu'on a pas atteint le point de départ
		ASTAR_add_node_to_list(aux.list[aux.nbNodes-1]->parent, &aux);
		debug_printf("Node added 1\n");
	}

	debug_printf("\n\nReconstruction à l'endroit\n");
	debug_printf("aux.nbNodes =%d\n", aux.nbNodes);

	//On reconstruit de chemin à l'endroit: du départ vers la destination voulue
	for(i=aux.nbNodes-1; i>=0; i--){
		ASTAR_add_node_to_list(aux.list[i],  &(path->list));
		debug_printf("i=%d  aux.nbNodes=%d Node added 2\n", aux.nbNodes, i);
	}
	debug_printf("Finish\n");
}

//-------------------------------------------------- Fonctions Annexes  ------------------------------------------------------

void ASTAR_add_neighbor_to_node(astar_ptr_node_t node, astar_ptr_node_t neighbor){
	node->neighbors[node->nbNeighbors] = neighbor;
	node->nbNeighbors = node->nbNeighbors + 1;
}

void ASTAR_clean_list(astar_list_t *list){
	list->nbNodes = 0;
}

bool_e ASTAR_list_is_empty(astar_list_t list){
	return (list.nbNodes == 0);
}

void ASTAR_add_node_to_list(astar_ptr_node_t node, astar_list_t *list){
	list->list[list->nbNodes] = node;
	list->nbNodes = list->nbNodes + 1;
}

void ASTAR_delete_node_to_list(astar_ptr_node_t node, astar_list_t *list){
	if(list == NULL){
		debug_printf("list = NULL in ASTAR_delete_node_to_list\n");
	}else if(node == NULL){
		debug_printf("node = NULL in ASTAR_delete_node_to_list\n");
	}else{
		Uint8 i=0, j;
		bool_e index_found = FALSE;
		//recherche de la position
		while(i < list->nbNodes && !index_found){
			if(list->list[i]->id == node->id){
				index_found = TRUE;
			}else{
				i++;
			}
		}

		//Décalage des nodes
		for(j=i ; j<list->nbNodes - 1; j++){
			list->list[j] = list->list[j+1];
		}

		//Dinimution du nombre de nodes
		list->nbNodes = list->nbNodes - 1;
	}
}

GEOMETRY_point_t ASTAR_intersection_is(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2){
	Uint8 a=1, b=1, c=1, d=1;
	bool_e verticale1= FALSE, verticale2 = FALSE;
	//Equation de la droite du segment 1: Y = aX + b
	if(seg1.b.x != seg1.a.x){
		a = (seg1.b.y - seg1.a.y)/(seg1.b.x - seg1.a.x);
		b = a * seg1.a.x - seg1.a.y;
	}else{
		a = 0;
		b = seg1.a.x;
		verticale1 = TRUE;
	}


	//Equation de la droite du segment 2: Y = cX + c
	if(seg2.b.x != seg2.a.x){
		c = (seg2.b.y - seg2.a.y)/(seg2.b.x - seg2.a.x);
		d = c * seg2.a.x - seg2.a.y;
	}else{
		c = 0;
		d = seg1.a.x;
		verticale2 = TRUE;
	}



	GEOMETRY_point_t intersection;
	if(!verticale1 && !verticale2 && c != a){
		intersection.x = (b - d)/(c - a);
		intersection.y = (b*c - a*d)/(c - a);
	}else if(!verticale1 && verticale2){
		intersection.x = b;
		intersection.y = c*b + d;
	}else if(verticale1 && !verticale2){
		intersection.x = d;
		intersection.y = a*d + b;
	}else{
		debug_printf("Problème dans ASTAR_intersection\n");
	}

	return intersection;
}



//Calcul du cout entre deux points par une distance de manhattan
Uint16 ASTAR_pathfind_cost(astar_ptr_node_t start_node, astar_ptr_node_t end_node)
{
	if(start_node == NULL)
	{
		debug_printf("start_node NULL dans ASTAR_pathfind_cost\n");
		return MAX_COST;
	}
	if(end_node == NULL)
	{
		debug_printf("end_node NULL dans ASTAR_pathfind_cost\n");
		return MAX_COST;
	}
	return ((start_node->pos.x > end_node->pos.x) ? start_node->pos.x-end_node->pos.x : end_node->pos.x-start_node->pos.x) +
			((start_node->pos.y >end_node->pos.y) ? start_node->pos.y-end_node->pos.y : end_node->pos.y-start_node->pos.y);
}

//-------------------------------------------------- Fonctions d'affichage ---------------------------------------------------

void print_polygon_list(){
	int i, j;
	int nb=0;
	debug_printf("MARGIN_TO_OBSTACLE = %d\n", MARGIN_TO_OBSTACLE);
	debug_printf("MARGIN_TO_BORDER = %d\n", MARGIN_TO_BORDER);
	debug_printf("Liste des nodes \n\n");
	ASTAR_generate_polygon_list();
	for(i=0; i< polygon_list.nbPolygons; i++){
		//debug_printf("polygone[%d]: \n", i);
		if(polygon_list.polygons[i].enable){
			for(j=0; j< polygon_list.polygons[i].nbSummits; j++){
				if(polygon_list.polygons[i].summits[j].enable){
					debug_printf("%d       %d\n", polygon_list.polygons[i].summits[j].pos.x, polygon_list.polygons[i].summits[j].pos.y);
					nb++;
				}
			}
		}
	}
	debug_printf("%d nodes written \n", nb);
}

void print_path(astar_path_t path){
	Uint8 i;
	debug_printf("\nPath is:\n");
	for(i=0; i<path.list.nbNodes; i++){
		debug_printf("Node: pos x=%d  y=%d \n", path.list.list[i]->pos.x, path.list.list[i]->pos.y);
	}
}

void print_list(astar_list_t list){
	Uint8 i;
	//debug_printf("List is:\n");
	for(i=0; i<list.nbNodes; i++){
		debug_printf("%d) Node: pos x=%d  y=%d \n", i, list.list[i]->pos.x, list.list[i]->pos.y);
		debug_printf("\t costHeuristic=%d  costStep=%d  costTotal=%d\n",list.list[i]->cost.heuristic, list.list[i]->cost.step, list.list[i]->cost.total);
	}
}

void print_opened_list(){
	debug_printf("\nOPENED LIST:\n");
	print_list(opened_list);
}

void print_closed_list(){
	debug_printf("\nCLOSED LIST:\n");
	print_list(closed_list);
}













