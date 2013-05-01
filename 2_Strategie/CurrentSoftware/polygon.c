/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : polygon.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de génération des trajectoires
 *					par le biais de polygones
 *	Auteurs : Adrien GIRARD et Julien FRANCHINEAU
 *	Version 20110227
 */

#ifdef USE_POLYGON
	
	#define POLYGON_C
	#include "polygon.h"

	/* listes nécessaires au pathfinding */
	static polygon_node_list_t open_list[POLYGON_LIST_WORD_N];
	static polygon_node_list_t closed_list[POLYGON_LIST_WORD_N];
	
	/* Charge dans la pile les goto avec le meilleur chemin */
	Uint16 POLYGON_compute(Sint16 x_from, Sint16 y_from, Sint16 x_destination, Sint16 y_destination, way_e way, ASSER_speed_e speed, bool_e curve, polygon_elements_type_e type_elements, bool_e use_opponent)
	{
		polygon_list_t polygons_list;
		polygon_graph_t minimal;
		polygon_node_id_t current;
		polygon_point_t* point;	
	
		/*test de viabilité du point de destination*/
		if(POLYGON_point_out_of_area(x_destination,y_destination))
		{
			return POLYGON_PATHFIND_OPPONENT_COST;
		}
		
		/* Génération de la liste de polygons */
		POLYGON_generate_list(&polygons_list,type_elements,use_opponent);
	
		/* Génération du graphe minimal */
		POLYGON_generate_graph(&minimal, &polygons_list, x_from, y_from, x_destination, y_destination);
		
		POLYGON_print_graph(&minimal);
	
		/* Pathfinding */
		
		/*Si on le peut on envoie le chemin dans la pile d'Asser */
		if(POLYGON_pathfind(&minimal))
		{
			
			polygon_printf("\n-------\nPath : \n");
			current = POLYGON_LIST_DESTINATION;
			while(current != POLYGON_LIST_FROM)
			{
				polygon_printf("\n node : %d \n",current);
				point = POLYGON_get_point(&minimal,current);
	
				#ifdef USE_ASSER_MULTI_POINT
					ASSER_push_goto_multi_point((*point).x,(*point).y, speed, way, curve, END_OF_BUFFER, END_AT_LAST_POINT, FALSE);
				#else
					ASSER_push_goto((*point).x, (*point).y, speed, way, curve,END_AT_LAST_POINT, FALSE);
				#endif
				minimal.destination.cost -= minimal.destination.cost - (*point).cost;
				current = (*point).parent;	
			}
			minimal.destination.cost -= (*point).cost;
			polygon_printf("\n-------\n");
			STACKS_push(ASSER, &wait_forever, TRUE);
		}
	
		/* on retourne le coût du voyage */
		return minimal.destination.cost;
	}
	
	/*
	*	Pathfinding  : retourne si un chemin est trouvé
	*	cost est le coût de déplacement depuis le point from
	*	heuristic est le coût approximatif (d'où le fait qu'on l'appelle heuristique)
	*	de déplacement vers le point destination.
	*	Si le chemin est trouvé destination aurra le coût réel de déplacement 
	*	ou POLYGON_PATHFIND_OPPONENT_COST
	*/
	static bool_e POLYGON_pathfind(polygon_graph_t* minimal)
	{
		int i;
		bool_e is_in_open_list,is_in_close_list = FALSE;
		polygon_node_id_t current;
		polygon_point_t* point;
		polygon_point_t* parent_point;
		Uint16 minimal_cost,step_cost,heuristic;
		if(minimal == NULL)
		{
			polygon_printf("minimal NULL dans POLYGON_pathfind\r\n");
			return FALSE;
		}
	
		/* Initialisation des variables */	
		(*minimal).from.parent=POLYGON_LIST_FROM;					//le noeud de départ n'a pas de parent ( c'est triste !)
		(*minimal).from.cost=0;										//ça coûte rien d'aller de from vers.....from
		(*minimal).destination.cost = POLYGON_PATHFIND_OPPONENT_COST;	//la destination....vers l'infini et au-delà
		POLYGON_delete_all_neighbors_to_nodelist(open_list);			//une openList toute vierge
		POLYGON_delete_all_neighbors_to_nodelist(closed_list);		// et sa soeur aussi
	
		/* Ajout du point de départ à la liste ouverte*/
		POLYGON_add_neighbor_to_nodelist(open_list, POLYGON_LIST_FROM);	
	
		/* tant que la destination n'est pas dans la closeList ou que l'openList n'est pas vide*/
	
		while(!is_in_close_list && !POLYGON_empty_nodelist(open_list))
		{
			minimal_cost = POLYGON_PATHFIND_OPPONENT_COST;
			current = 0;
	
			/*On cherche le noeud de cout de déplacement le plus faible*/		
			for( i = 0; i <	POLYGON_LIST_MAX; i++)
			{
				if(POLYGON_read_nodelist(open_list, i))
				{
					point = POLYGON_get_point(minimal,i);
					if((*point).cost < minimal_cost)
					{
						minimal_cost = (*point).cost;
						current = i;
					}
				}
			}	
	
			/* On retire le noeud parent de l'open list, l'ajoute à la closedList*/
			POLYGON_delete_neighbor_to_nodelist(open_list, current);
			POLYGON_add_neighbor_to_nodelist(closed_list, current);
	
			is_in_close_list = POLYGON_read_nodelist(closed_list,POLYGON_LIST_DESTINATION);
	
			/*si on a le point de destination on arrête de calculer ici*/
			if(!is_in_close_list)
			{
				/*	On ajoute les noeuds "adjacents" non présent dans la closedList à l'openList
				*	et on calcul l'heuristic
				*/		
				parent_point = POLYGON_get_point(minimal,current);
		
				for( i = 0; i < POLYGON_LIST_MAX; i++)
				{
					if(POLYGON_read_nodelist((*parent_point).neighbors,i) && !POLYGON_read_nodelist(closed_list, i))
					{
						is_in_open_list = POLYGON_read_nodelist(open_list,i);				
						point = POLYGON_get_point(minimal,i);	
						heuristic = POLYGON_pathfind_cost(point,&((*minimal).destination));	
		
						/* si le noeud n'est pas dans l'openLost on l'y ajoute*/
						if(!is_in_open_list)
						{				
							POLYGON_add_neighbor_to_nodelist(open_list, i);
						}
		
						/*si le noeud n'êtait pas dans la liste ouverte
						* ou si le noeud y était mais que l'heuristic est moins grande que celle du noeud
						*	...
						*/
						if(!is_in_open_list || (is_in_open_list && heuristic < (*point).heuristic))
						{	
							/* le noeud courant devient le noeud parent*/	
							(*point).parent = current;
							/* ajout d'heuristic*/	
							(*point).heuristic = heuristic;	
							/*calcul de coût*/	
							step_cost = POLYGON_pathfind_cost(point,parent_point);
							(*point).cost = minimal_cost + step_cost + heuristic;					
						}
					}
				}		
			}
		}
		return is_in_close_list;
	}
	
	/* Retourne le coût de déplacement de startPoint vers endPoint
	*	basé sur un manhattan
	*/
	static Uint16 POLYGON_pathfind_cost(polygon_point_t* start_point, polygon_point_t* end_point)
	{
		if(start_point == NULL)
		{
			polygon_printf("startPoint NULL dans POLYGON_pathfind_cost\r\n");
			return POLYGON_PATHFIND_OPPONENT_COST;
		}
		if(end_point == NULL)
		{
			polygon_printf("endPoint NULL dans POLYGON_pathfind_cost\r\n");
			return POLYGON_PATHFIND_OPPONENT_COST;
		}
		return ((*start_point).x>(*end_point).x ? (*start_point).x-(*end_point).x : (*end_point).x-(*start_point).x) + 
		((*start_point).y>(*end_point).y ? (*start_point).y-(*end_point).y : (*end_point).y-(*start_point).y);
	}
	
	/* Retourne un pointeur vers le point du graphe correspondant au node_id*/
	static polygon_point_t* POLYGON_get_point(polygon_graph_t* graph, polygon_node_id_t id)
	{
		Uint8 polygon, point;
		if(graph == NULL)
		{
			polygon_printf("graph NULL dans POLYGON_get_point\r\n");
			return NULL;
		}
		if(id == POLYGON_LIST_FROM)
		{
				return &((*graph).from);
		}
		else
		{	
			if(id == POLYGON_LIST_DESTINATION)
			{
					return &((*graph).destination);
			}
			else
			{		
					point = id;	
					polygon = 0;	
					while( point >= MAX_SUMMITS_POLYGON)
					{
						point -= MAX_SUMMITS_POLYGON;
						polygon++;
					}
					return &((*(*graph).polygons).polygon[polygon].point[point]);				
			}
		}
	}
	
	/*
	*	Fonctions de création de la liste de polygons
	*/
	
	/* Crée un polygone */
	static void POLYGON_create_polygon(polygon_t* polygon, Sint16 x, Sint16 y)
	{
		if(polygon == NULL)
		{
			polygon_printf("polygon NULL dans POLYGON_create_polygon\r\n");
		}
		(*polygon).point[0].x = x+POINT_A_X;
		(*polygon).point[0].y = y+POINT_A_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*polygon).point[0].neighbors);
		(*polygon).point[0].outOfArea = POLYGON_point_out_of_area((*polygon).point[0].x, (*polygon).point[0].y); 
	
		(*polygon).point[1].x = x+POINT_B_X;
		(*polygon).point[1].y = y+POINT_B_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*polygon).point[1].neighbors);
		(*polygon).point[1].outOfArea = POLYGON_point_out_of_area((*polygon).point[1].x, (*polygon).point[1].y);
	
		(*polygon).point[2].x = x+POINT_C_X;
		(*polygon).point[2].y = y+POINT_C_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*polygon).point[2].neighbors);
		(*polygon).point[2].outOfArea = POLYGON_point_out_of_area((*polygon).point[2].x, (*polygon).point[2].y);
	
		(*polygon).point[3].x = x+POINT_D_X;
		(*polygon).point[3].y = y+POINT_D_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*polygon).point[3].neighbors);
		(*polygon).point[3].outOfArea = POLYGON_point_out_of_area((*polygon).point[3].x, (*polygon).point[3].y);
	
		(*polygon).point[4].x = x+POINT_E_X;
		(*polygon).point[4].y = y+POINT_E_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*polygon).point[4].neighbors);
		(*polygon).point[4].outOfArea = POLYGON_point_out_of_area((*polygon).point[4].x, (*polygon).point[4].y);
	}
	
	/* Crée le polygone de l'adversaire */
	static void POLYGON_create_opponent_polygon(polygon_t* opponent_polygon)
	{
		Sint16 x_opponent, y_opponent;
		if(opponent_polygon == NULL)
		{
			polygon_printf("opponentPolygon NULL dans POLYGON_create_opponent_polygon\r\n");
		}
	
		x_opponent = global.env.foe.x;
		y_opponent = global.env.foe.y;
	
		(*opponent_polygon).point[0].x = x_opponent+OPPONENT_POINT_A_X;
		(*opponent_polygon).point[0].y = y_opponent+OPPONENT_POINT_A_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*opponent_polygon).point[0].neighbors);
		(*opponent_polygon).point[0].outOfArea = POLYGON_point_out_of_area((*opponent_polygon).point[0].x, (*opponent_polygon).point[0].y);
	
		(*opponent_polygon).point[1].x = x_opponent+OPPONENT_POINT_B_X;
		(*opponent_polygon).point[1].y = y_opponent+OPPONENT_POINT_B_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*opponent_polygon).point[1].neighbors);
		(*opponent_polygon).point[1].outOfArea = POLYGON_point_out_of_area((*opponent_polygon).point[1].x, (*opponent_polygon).point[1].y);
	
		(*opponent_polygon).point[2].x = x_opponent+OPPONENT_POINT_C_X;
		(*opponent_polygon).point[2].y = y_opponent+OPPONENT_POINT_C_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*opponent_polygon).point[2].neighbors);
		(*opponent_polygon).point[2].outOfArea = POLYGON_point_out_of_area((*opponent_polygon).point[2].x, (*opponent_polygon).point[2].y);
	
		(*opponent_polygon).point[3].x = x_opponent+OPPONENT_POINT_D_X;
		(*opponent_polygon).point[3].y = y_opponent+OPPONENT_POINT_D_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*opponent_polygon).point[3].neighbors);
		(*opponent_polygon).point[3].outOfArea = POLYGON_point_out_of_area((*opponent_polygon).point[3].x, (*opponent_polygon).point[3].y);
	
		(*opponent_polygon).point[4].x = x_opponent+OPPONENT_POINT_E_X;
		(*opponent_polygon).point[4].y = y_opponent+OPPONENT_POINT_E_Y;
		POLYGON_delete_all_neighbors_to_nodelist((*opponent_polygon).point[4].neighbors);
		(*opponent_polygon).point[4].outOfArea = POLYGON_point_out_of_area((*opponent_polygon).point[4].x, (*opponent_polygon).point[4].y);
	}
	
	/* Génère une liste et la renvoie */
	static void POLYGON_generate_list(polygon_list_t* polygons_list, polygon_elements_type_e type_elements, bool_e use_opponent)
	{
		/* Initialisation des variables */
		Uint16 i;
	
		if(polygons_list == NULL)
		{
			polygon_printf("polygons_list NULL dans POLYGON_generate_list\r\n");
		}
	
		(*polygons_list).polygonsNumber = 0;
	
		if(use_opponent)
		{
			/* On met en premier dans la liste le polygone de l'adversaire */
			POLYGON_create_opponent_polygon(&((*polygons_list).polygon[(*polygons_list).polygonsNumber]));
			(*polygons_list).polygonsNumber++;
		}
		
		/* On regarde quels éléments on doit insérer dans la liste de polygones */
		switch(type_elements)
		{
			case ALL_ELEMENTS:
				for(i=0; i<global.env.nb_elements; i++)
				{
					if(global.env.nb_elements)
					POLYGON_create_polygon(&((*polygons_list).polygon[(*polygons_list).polygonsNumber]),global.env.elements_list[i].x, global.env.elements_list[i].y);	//poly2
					(*polygons_list).polygonsNumber++;
				}
				break;
	
			case OUR_AND_OPPONENT_ELEMENTS:
				for(i=0; i<global.env.nb_elements; i++)
				{
					// On vérifie si l'élement est à nous ou à l'adversaire , si oui, on l'ajoute
					if(ELEMENTS_belonging_type(i) == US || ELEMENTS_belonging_type(i) == ADVERSARY)
					{
						POLYGON_create_polygon(&((*polygons_list).polygon[(*polygons_list).polygonsNumber]),global.env.elements_list[i].x, global.env.elements_list[i].y);	//poly2
						(*polygons_list).polygonsNumber++;
					}		
				}
				break;
				
			case OUR_ELEMENTS:
				for(i=0; i<global.env.nb_elements; i++)
				{
					// On vérifie si l'élement est à nous, si oui, on l'ajoute
					if(ELEMENTS_belonging_type(i) == US)
					{
						POLYGON_create_polygon(&((*polygons_list).polygon[(*polygons_list).polygonsNumber]),global.env.elements_list[i].x, global.env.elements_list[i].y);	//poly2
						(*polygons_list).polygonsNumber++;
					}		
				}
				break;			
	
			default:
				break;
		}
	}

	/*fonction pour générer le chemin*/
	
	/* Génère la graphe minimal de points */
	static void POLYGON_generate_graph(polygon_graph_t* minimal, polygon_list_t* polygons_list, Sint16 x_from, Sint16 y_from, Sint16 x_destination, Sint16 y_destination)
	{
		Uint8 i,j,k;
		bool_e is_visible,from_intersect, dest_intersect, from_accessible, dest_accessible;
		polygon_node_list_t link;
	
		if(minimal == NULL)
		{
			polygon_printf("minimal NULL dans POLYGON_generate_graph\r\n");
		}
		if(polygons_list == NULL)
		{
			polygon_printf("polygonsList NULL dans POLYGON_generate_graph\r\n");
		}
		(*minimal).from.x = x_from;
		(*minimal).from.y = y_from;
		(*minimal).destination.x = x_destination;
		(*minimal).destination.y = y_destination;
		(*minimal).polygons = polygons_list;
		
		POLYGON_delete_all_neighbors_to_nodelist((*minimal).from.neighbors);
		POLYGON_delete_all_neighbors_to_nodelist((*minimal).destination.neighbors);
		POLYGON_add_neighbor_to_nodelist((*minimal).from.neighbors, POLYGON_LIST_DESTINATION);
		POLYGON_add_neighbor_to_nodelist((*minimal).destination.neighbors, POLYGON_LIST_FROM);
	
		/* 
		*	On relie les noeuds de chaque polygone 
		*	autrement dit on "dessine" chaque polygone
		*/
		for(i=0;i<(*polygons_list).polygonsNumber;i++)
		{
			/*sur le polygon i....*/
	
			/*...le noeud 0 est lié au noeud 1 et au dernier noeud*/
			if((*polygons_list).polygon[i].point[0].outOfArea==FALSE)
			{
				POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[0].neighbors, i*MAX_SUMMITS_POLYGON + 1);
				POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[0].neighbors, i*MAX_SUMMITS_POLYGON + MAX_SUMMITS_POLYGON -1);
			}
			for(j=1;j<MAX_SUMMITS_POLYGON-1;j++)
			{
				if((*polygons_list).polygon[i].point[j].outOfArea==FALSE)
				{
					/* chaque point est lié au prochain et au précédent noeud*/
					POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, i*MAX_SUMMITS_POLYGON+j-1);
					POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, i*MAX_SUMMITS_POLYGON+j+1);
				}
			}
			
			if((*polygons_list).polygon[i].point[4].outOfArea==FALSE)
			{
				/*...le dernier noeud est lié au noeud 0 et à l'avant dernier*/
				POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[MAX_SUMMITS_POLYGON - 1].neighbors, i*MAX_SUMMITS_POLYGON);
				POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[MAX_SUMMITS_POLYGON - 1].neighbors, i*MAX_SUMMITS_POLYGON + MAX_SUMMITS_POLYGON- 2);
			}			
		}
		
		/* On relie les noeuds des polygones au point de départ et au point d'arrivée si ils sont vus
		* 	si le point est viable ou on le supprime
		*/
		for(i=0;i<(*polygons_list).polygonsNumber;i++)
		{
			for(j=0;j<MAX_SUMMITS_POLYGON;j++)
			{
				if((*polygons_list).polygon[i].point[j].outOfArea)
				{
					
						/*
						*	On ne supprimer que la liste d'enfant du noeud à supprimer
						*/
						POLYGON_delete_all_neighbors_to_nodelist((*polygons_list).polygon[i].point[j].neighbors);
				}
				else
				{
					from_intersect = POLYGON_intersect_from_dest(minimal, i, j, POLYGON_LIST_FROM, &from_accessible);
					dest_intersect = POLYGON_intersect_from_dest(minimal, i, j, POLYGON_LIST_DESTINATION, &dest_accessible);
					if(from_intersect || dest_intersect)
					{
						POLYGON_delete_neighbor_to_nodelist((*minimal).from.neighbors, POLYGON_LIST_DESTINATION);
						POLYGON_delete_neighbor_to_nodelist((*minimal).destination.neighbors, POLYGON_LIST_FROM);		
					}
					else
					{
						/* Noeud de départ */			
						if(/*si le point est visible : cas des 4 premiers point*/
							from_accessible
							||(j != MAX_SUMMITS_POLYGON - 1  && POLYGON_point_is_visible((*polygons_list).polygon[i].point[j].x, (*polygons_list).polygon[i].point[j].y,
							 (*polygons_list).polygon[i].point[j+1].x, (*polygons_list).polygon[i].point[j+1].y, x_from, y_from))
							/*cas du dernier point*/
							||(j == MAX_SUMMITS_POLYGON - 1  && POLYGON_point_is_visible((*polygons_list).polygon[i].point[j].x, (*polygons_list).polygon[i].point[j].y,
							 (*polygons_list).polygon[i].point[0].x, (*polygons_list).polygon[i].point[0].y, x_from, y_from)))
						{
							POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, POLYGON_LIST_FROM);
							POLYGON_add_neighbor_to_nodelist((*minimal).from.neighbors, i*MAX_SUMMITS_POLYGON+j);
							if(j == MAX_SUMMITS_POLYGON - 1)
							{
								POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[0].neighbors, POLYGON_LIST_FROM);
								POLYGON_add_neighbor_to_nodelist((*minimal).from.neighbors, i*MAX_SUMMITS_POLYGON);				
							}
							else
							{
								POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[j+1].neighbors, POLYGON_LIST_FROM);
								POLYGON_add_neighbor_to_nodelist((*minimal).from.neighbors, i*MAX_SUMMITS_POLYGON+j+1);
				
							}
						}
						/* Noeud d'arrivée */	
						if(	/*si le point est visible : cas des 4 premiers points*/ 
							dest_accessible
							||(j != MAX_SUMMITS_POLYGON - 1  && POLYGON_point_is_visible((*polygons_list).polygon[i].point[j].x, (*polygons_list).polygon[i].point[j].y,
							 (*polygons_list).polygon[i].point[j+1].x, (*polygons_list).polygon[i].point[j+1].y, x_destination, y_destination))
							/*cas du dernier point*/
							||(j == MAX_SUMMITS_POLYGON - 1  && POLYGON_point_is_visible((*polygons_list).polygon[i].point[j].x, (*polygons_list).polygon[i].point[j].y,
							 (*polygons_list).polygon[i].point[0].x, (*polygons_list).polygon[i].point[0].y, x_destination, y_destination)))
						{
							POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, POLYGON_LIST_DESTINATION);
							POLYGON_add_neighbor_to_nodelist((*minimal).destination.neighbors, i*MAX_SUMMITS_POLYGON+j);		
							if(j == MAX_SUMMITS_POLYGON - 1)
							{
								POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[0].neighbors, POLYGON_LIST_DESTINATION);
								POLYGON_add_neighbor_to_nodelist((*minimal).destination.neighbors, i*MAX_SUMMITS_POLYGON);
							}
							else
							{
								POLYGON_add_neighbor_to_nodelist((*polygons_list).polygon[i].point[j+1].neighbors, POLYGON_LIST_DESTINATION);
								POLYGON_add_neighbor_to_nodelist((*minimal).destination.neighbors, i*MAX_SUMMITS_POLYGON+j+1);
				
							}
						}	
					}
				}
			}
		}	
		/*Suppression des points finalement invisibles parce que dans un polygon ou trajectoire infaisable
		*/
		for(i=0;i<(*polygons_list).polygonsNumber;i++)
		{
			for(j=0;j<MAX_SUMMITS_POLYGON;j++)
			{
				is_visible = TRUE;
				for(k=0;(k<(*polygons_list).polygonsNumber)&&(is_visible);k++)
				{
					/* On évite l'autobouclage*/
					if(i!=k)
					{
						link = POLYGON_is_visible(&((*polygons_list).polygon[k]), (*polygons_list).polygon[i].point[j].x, (*polygons_list).polygon[i].point[j].y);
						POLYGON_set_neighbors_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, link, k * MAX_SUMMITS_POLYGON);
						is_visible &= (link!=0);
					}
				}
	
				if(!is_visible)
				{
					/*
					*	On ne supprimer que la liste d'enfant du noeud à supprimer
					*/
					POLYGON_delete_all_neighbors_to_nodelist((*polygons_list).polygon[i].point[j].neighbors);
				}
			}		
		}
		
		/*  nettoyage du graphe*/
		POLYGON_clean_graph(minimal);
	}
	
	
	/*Suppression des arcs, des colisions, des cul-de-sac et des rebouclage dans le graphe*/
	static void POLYGON_clean_graph(polygon_graph_t* graph)
	{
		Uint8 i,j,k,l,m;
		bool_e intersect;
		polygon_node_id_t id,id2;
		if(graph == NULL)
		{
			polygon_printf("polygonsList NULL dans POLYGON_clean_graph\r\n");
		}
		polygon_list_t* polygons_list = (*graph).polygons;
	
		for(i=0;i<(*polygons_list).polygonsNumber;i++)
		{
			for(j=0;j<MAX_SUMMITS_POLYGON;j++)
			{
				id = i * MAX_SUMMITS_POLYGON + j;
				/*Suppression d'un éventuel rebouclage*/
				POLYGON_delete_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, id);
	
				/*Suppressions des arcs avec le point de départ*/
				intersect = POLYGON_intersect_point(graph,i,j,(*graph).from.x,(*graph).from.y);							
				if(!(POLYGON_read_nodelist((*graph).from.neighbors, id)
					&& POLYGON_read_nodelist((*polygons_list).polygon[i].point[j].neighbors, POLYGON_LIST_FROM)
					&& !intersect))
				{
					POLYGON_delete_neighbor_to_nodelist((*graph).from.neighbors, id);
					POLYGON_delete_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, POLYGON_LIST_FROM);	
				}
	
				/*Suppressions des arcs avec le point d'arrivée*/
				intersect = POLYGON_intersect_point(graph,i,j,(*graph).destination.x,(*graph).destination.y);
				if(!(POLYGON_read_nodelist((*graph).destination.neighbors, id)
					&& POLYGON_read_nodelist((*polygons_list).polygon[i].point[j].neighbors, POLYGON_LIST_DESTINATION)
					&& !intersect))
				{
					POLYGON_delete_neighbor_to_nodelist((*graph).destination.neighbors, id);
					POLYGON_delete_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, POLYGON_LIST_DESTINATION);	
					
				}
	
				m = (j==MAX_SUMMITS_POLYGON-1)?i+1:i;
				/*Suppression des arcs avec les autres points des polygons et les colisions*/
				for(k=m; k<(*polygons_list).polygonsNumber; k++)
				{
					m = (j==MAX_SUMMITS_POLYGON-1 || k>i)?0:j+1;
					for(l = m;l<MAX_SUMMITS_POLYGON;l++)
					{
						
						intersect = POLYGON_intersect_polygon(graph,i,j,k,l);
						id2 = k * MAX_SUMMITS_POLYGON + l;	
						if(!(POLYGON_read_nodelist((*polygons_list).polygon[k].point[l].neighbors, id)
							&& POLYGON_read_nodelist((*polygons_list).polygon[i].point[j].neighbors, id2)
							&&!intersect))
						{					
							POLYGON_delete_neighbor_to_nodelist((*polygons_list).polygon[k].point[l].neighbors, id);
							POLYGON_delete_neighbor_to_nodelist((*polygons_list).polygon[i].point[j].neighbors, id2);	
						}
					}
				}
			}
		}
	}
	
	/*
	*	Fonctions d'algorithme du point visible
	*/
	
	/* Algorythme du point visible : 
	*	Cette fonction retourne TRUE si le segment ij est visible depuis p
	*	soit que les point i et j sont accessibles
	*/
	static bool_e POLYGON_point_is_visible(Sint16 xi, Sint16 yi, Sint16 xj, Sint16 yj, Sint16 xp, Sint16 yp)
	{
		Sint32 xpi;
		Sint32 ypi;
		Sint32 xij;
		Sint32 yij;
		Sint32 resultat;
	
		/*si le point est en intersection avec une zone critique 
		*	le point est forcément invisible
		*	C'est un complément à l'algorythme du point visible
		*/
		if(POLYGON_intersect_segment(xi,yi,xp,yp))
		{
			return FALSE;
		}
	
		xpi = xi - xp;
		ypi = yi - yp;
		xij = xj - xi;
		yij = yj - yi;
		
		resultat = xpi * yij - xij * ypi; 
		//polygon_printf("\n Resultat : %ld",resultat);
		return (bool_e)(resultat <= 0)?TRUE:FALSE;
	}
	
	/* teste si le segment [A,B], A(x1,y1), B(x2,y2)
	*	est en intersection avec un segment critique(morceau de bois)
	*/
	
	static bool_e POLYGON_intersect_segment(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
	{
		
		GEOMETRY_segment_t segment;
	
		//Les segments à ne pas traverser au risque de se prendre un morceau de bois
		const GEOMETRY_segment_t red_start = 
				{
					(GEOMETRY_point_t){450,0},
					(GEOMETRY_point_t){450,450+CASE_SIZE_Y/2}
				};
		const GEOMETRY_segment_t blue_start = 
				{
					(GEOMETRY_point_t){450,GAME_ZONE_SIZE_Y},
					(GEOMETRY_point_t){450,GAME_ZONE_SIZE_Y-450-CASE_SIZE_Y/2}
				};
		const GEOMETRY_segment_t red_protected1 = 
				{
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X,450},
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X-3*CASE_SIZE_X/2,450}
				};
		const GEOMETRY_segment_t blue_protected1 = 
				{
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X,GAME_ZONE_SIZE_Y-450},
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X-3*CASE_SIZE_X/2,GAME_ZONE_SIZE_Y-450}
				};
		const GEOMETRY_segment_t red_protected2 = 
				{
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X,450+CASE_SIZE_Y*2},
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X-3*CASE_SIZE_X/2,450+CASE_SIZE_Y*2}
				};
		const GEOMETRY_segment_t blue_protected2 = 
				{
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X,GAME_ZONE_SIZE_Y-450-CASE_SIZE_Y*2},
					(GEOMETRY_point_t){GAME_ZONE_SIZE_X-3*CASE_SIZE_X/2,GAME_ZONE_SIZE_Y-450-CASE_SIZE_Y*2}
				};
	
		segment.a.x = x1;
		segment.a.y = y1;
		segment.b.x = x2;
		segment.b.y = y2;
	
		if(GEOMETRY_segments_intersects(segment,red_start))
		{
			return TRUE;
		}
	
		if(GEOMETRY_segments_intersects(segment,blue_start))
		{
			return TRUE;
		}
	
		if(GEOMETRY_segments_intersects(segment,red_protected1))
		{
			return TRUE;
		}
	
		if(GEOMETRY_segments_intersects(segment,blue_protected1))
		{
			return TRUE;
		}
	
		if(GEOMETRY_segments_intersects(segment,red_protected2))
		{
			return TRUE;
		}
	
		if(GEOMETRY_segments_intersects(segment,blue_protected2))
		{
			return TRUE;
		}
	
		return FALSE;
	}
	
	
	/* Intersection entre un segment et un polygon ou un segment critique 
	*	le segment est toujours défini par un point d'un polygon et un point(x,y);
	*/
	static bool_e POLYGON_intersect_point(polygon_graph_t* graph, Uint8 polygon_id, Uint8 point_id, Sint16 x, Sint16 y)
	{
		Uint8 i,j;	
		GEOMETRY_segment_t segment1,segment2;
	
		if(graph == NULL)
		{
			polygon_printf("graph NULL dans POLYGON_intersect\r\n");
		}
		polygon_list_t* polygons_list = (*graph).polygons;
	
		segment1.a.x = (*polygons_list).polygon[polygon_id].point[point_id].x;
		segment1.a.y = (*polygons_list).polygon[polygon_id].point[point_id].y;
		segment1.b.x = x;
		segment1.b.y = y;
	
		for(i = 0 ; i < (*polygons_list).polygonsNumber ; i++)
		{
			/*il y aura inévitablement intersection entre un point d'un polygon et le même polygon*/
			if(i != polygon_id)
			{
				for(j = 0 ; j < MAX_SUMMITS_POLYGON - 1; j++)
				{
					segment2.a.x = (*polygons_list).polygon[i].point[j].x;
					segment2.a.y = (*polygons_list).polygon[i].point[j].y;
					segment2.b.x = (*polygons_list).polygon[i].point[j + 1].x;
					segment2.b.y = (*polygons_list).polygon[i].point[j + 1].y;
					if(GEOMETRY_segments_intersects(segment1,segment2))
					{
						return TRUE;
					}
				}
				
				segment2.a.x = (*polygons_list).polygon[i].point[MAX_SUMMITS_POLYGON - 1].x;
				segment2.a.y = (*polygons_list).polygon[i].point[MAX_SUMMITS_POLYGON - 1].y;
				segment2.b.x = (*polygons_list).polygon[i].point[0].x;
				segment2.b.y = (*polygons_list).polygon[i].point[0].y;
		
				if(GEOMETRY_segments_intersects(segment1,segment2))
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	
	/* Intersection entre un segment et un polygon  ou un segment critique 
	*	le segment est toujours défini par un point d'un polygon et un point d'un autre polygon
	*/
	static bool_e POLYGON_intersect_polygon(polygon_graph_t* graph, Uint8 polygon_id, Uint8 point_id, Uint8 polygon_id2, Uint8 point_id2)
	{
		Uint8 i,j;	
		GEOMETRY_segment_t segment1,segment2;
	
		if(graph == NULL)
		{
			polygon_printf("graph NULL dans POLYGON_intersect_polygon\r\n");
		}
		polygon_list_t* polygons_list = (*graph).polygons;
	
		segment1.a.x = (*polygons_list).polygon[polygon_id].point[point_id].x;
		segment1.a.y = (*polygons_list).polygon[polygon_id].point[point_id].y;
		segment1.b.x = (*polygons_list).polygon[polygon_id2].point[point_id2].x;
		segment1.b.y = (*polygons_list).polygon[polygon_id2].point[point_id2].y;
	
		for(i = 0 ; i < (*polygons_list).polygonsNumber ; i++)
		{
			/*il y aura inévitablement intersection entre un point d'un polygon et le même polygon*/
			if(i != polygon_id && i != polygon_id2)
			{
				for(j = 0 ; j < MAX_SUMMITS_POLYGON - 1; j++)
				{
					segment2.a.x = (*polygons_list).polygon[i].point[j].x;
					segment2.a.y = (*polygons_list).polygon[i].point[j].y;
					segment2.b.x = (*polygons_list).polygon[i].point[j + 1].x;
					segment2.b.y = (*polygons_list).polygon[i].point[j + 1].y;
					if(GEOMETRY_segments_intersects(segment1,segment2))
					{
						return TRUE;
					}
				}
				
				segment2.a.x = (*polygons_list).polygon[i].point[MAX_SUMMITS_POLYGON - 1].x;
				segment2.a.y = (*polygons_list).polygon[i].point[MAX_SUMMITS_POLYGON - 1].y;
				segment2.b.x = (*polygons_list).polygon[i].point[0].x;
				segment2.b.y = (*polygons_list).polygon[i].point[0].y;
		
				if(GEOMETRY_segments_intersects(segment1,segment2))
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	/* Intersection entre le segment départ-arrivée et celui d'un polygon  ou un segment critique 
	*	le segment est toujours défini par un point d'un polygon et un point d'un autre polygon
	*	on considère l'intersection si le segment traverse le pion
	*/
	static bool_e POLYGON_intersect_from_dest(polygon_graph_t* graph, Uint8 polygon_id, Uint8 point_id, polygon_node_id_t from_or_dest, bool_e* accessible)
	{
		GEOMETRY_segment_t segment1,segment2;
	
		if(graph == NULL)
		{
			polygon_printf("graph NULL dans POLYGON_intersect_polygon\r\n");
		}
		polygon_list_t* polygons_list = (*graph).polygons;
		(*accessible) = TRUE;
	
		segment1.a.x = (*graph).from.x;
		segment1.a.y = (*graph).from.y;
		segment1.b.x = (*graph).destination.x;
		segment1.b.y = (*graph).destination.y;

		if(point_id < MAX_SUMMITS_POLYGON - 1)
		{
			segment2.a.x = (*polygons_list).polygon[polygon_id].point[point_id].x;
			segment2.a.y = (*polygons_list).polygon[polygon_id].point[point_id].y;
			segment2.b.x = (*polygons_list).polygon[polygon_id].point[point_id + 1].x;
			segment2.b.y = (*polygons_list).polygon[polygon_id].point[point_id + 1].y;
		}
		else
		{
			segment2.a.x = (*polygons_list).polygon[polygon_id].point[MAX_SUMMITS_POLYGON - 1].x;
			segment2.a.y = (*polygons_list).polygon[polygon_id].point[MAX_SUMMITS_POLYGON - 1].y;
			segment2.b.x = (*polygons_list).polygon[polygon_id].point[0].x;
			segment2.b.y = (*polygons_list).polygon[polygon_id].point[0].y;
		}

		if(GEOMETRY_segments_intersects(segment1,segment2))
		{
			if(from_or_dest == POLYGON_LIST_FROM)
			{
				(*accessible) = (GEOMETRY_squared_distance(segment1.a,segment2.a)<= POLYGON_CIRCLE_RADIUS
					&&	GEOMETRY_squared_distance(segment1.a,segment2.b)<= POLYGON_CIRCLE_RADIUS);
			}
			else
			{
				(*accessible) = (GEOMETRY_squared_distance(segment1.b,segment2.a)<= POLYGON_CIRCLE_RADIUS
					&&	GEOMETRY_squared_distance(segment1.b,segment2.b)<= POLYGON_CIRCLE_RADIUS);
			}			
			if(!(*accessible))
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	
	/*vérifie si le point est bien dans l'aire de jeux */
	static bool_e POLYGON_point_out_of_area(Sint32 x, Sint32 y)
	{
		Sint32 x_circle_temp = (x-1850)*(x-1850);
		
		/*Point hors du terrain*/
		if(x < POLYGON_AREA_MARGIN || y < POLYGON_AREA_MARGIN 
			|| x > GAME_ZONE_SIZE_X - POLYGON_AREA_MARGIN 
			|| y > GAME_ZONE_SIZE_Y - POLYGON_AREA_MARGIN)
		{
			return TRUE;
		}
	
		/*Point de zone securisée*/
		if(x > 1980 - POLYGON_AREA_MARGIN && ((y > 450 - POLYGON_AREA_MARGIN && y < 1150 + POLYGON_AREA_MARGIN) || (y > 1850 - POLYGON_AREA_MARGIN && y < 2550 + POLYGON_AREA_MARGIN)))
		{
			return TRUE;
		}
		if((x_circle_temp + (y-461)*(y-461) < POLYGON_AREA_MARGIN*POLYGON_AREA_MARGIN) 
			|| (x_circle_temp + (y-1139)*(y-1139) < POLYGON_AREA_MARGIN*POLYGON_AREA_MARGIN) 
			|| (x_circle_temp + (y-1861)* (y-1861) < POLYGON_AREA_MARGIN*POLYGON_AREA_MARGIN) 
			|| (x_circle_temp + (y-2539)*(y-2539) < POLYGON_AREA_MARGIN*POLYGON_AREA_MARGIN))
		{
			return TRUE;
		}	
	
		/*Point de zone de départ*/
		if((x-411)*(Sint32)(x-411)+(y-400)*(y-400) < POLYGON_AREA_MARGIN*POLYGON_AREA_MARGIN
			||(x-411)*(x-411)+(y-GAME_ZONE_SIZE_Y+400)*(y-GAME_ZONE_SIZE_Y+400) < POLYGON_AREA_MARGIN*POLYGON_AREA_MARGIN)
		{
			return TRUE;
		}
		return FALSE;
	}
	
	/*	Vérifie si un point est visible depuis un polygon et retourne les liens avec les noeuds du polygone*/
	static polygon_node_list_t POLYGON_is_visible(polygon_t* polygon, Sint16 x, Sint16 y)
	{
		Uint8 indice;
		bool_e point_is_visible;
		polygon_node_list_t link = 0x00;
		if(polygon == NULL)
		{
			polygon_printf("polygon NULL dans POLYGON_is_visible\r\n");
		}
	
		/*boucle de test pour la visibilité du point*/
		for(indice = 0; indice < MAX_SUMMITS_POLYGON - 1; indice++)
		{
			point_is_visible = POLYGON_point_is_visible((*polygon).point[indice].x, (*polygon).point[indice].y, (*polygon).point[indice+1].x, (*polygon).point[indice+1].y, x, y);
			if(point_is_visible)
			{
				link |= 3<<indice;				
			}
		}
		
		point_is_visible = POLYGON_point_is_visible((*polygon).point[MAX_SUMMITS_POLYGON - 1].x, (*polygon).point[MAX_SUMMITS_POLYGON - 1].y, (*polygon).point[0].x, (*polygon).point[0].y, x, y);
		if(point_is_visible)
		{
					
				link |= 1<<(MAX_SUMMITS_POLYGON-1);
				link |= 1;
		}	
		return link;	
	}
	
	
	
	/*
	*	Fontions de lecture et d'écriture des polygon_node_list_t
	*/
	
	/* Ajouter noeud "newNeighbor" à la liste des noeuds */
	static void POLYGON_add_neighbor_to_nodelist(polygon_node_list_t neighbors[], polygon_node_id_t new_node)
	{	
		Uint32 mask;
		polygon_node_id_t word;
		if(neighbors == NULL)
		{
			polygon_printf("neighbors NULL dans POLYGON_add_neighbor_to_nodelist\r\n");
		}
	
	
	
		if(new_node < POLYGON_LIST_MAX)
		{
			word = 0;
			mask = 0;
	
			word = new_node / POLYGON_NODE_LIST_SIZE;
	
			/* une autre écriture pour le modulo une puissance de 2*/
			new_node &= POLYGON_NODE_LIST_SIZE - 1; 
	
			mask = ((Uint32)1)<<new_node;
			neighbors[word] |= mask;
		}	
	}
	
	/* Définie une polygon list en fonction d'une autre polygon list et d'un point de départ */
	static void POLYGON_set_neighbors_to_nodelist(polygon_node_list_t neighbors[], polygon_node_list_t link, polygon_node_id_t start)
	{	
		Uint32 mask;
		polygon_node_id_t word;
		Uint8 id, offset;
		if(neighbors == NULL)
		{
			polygon_printf("neighbors NULL dans POLYGON_set_neighbors_to_nodelist\r\n");
		}
	
	
		if(start < POLYGON_LIST_MAX)
		{
			word = 0;
	
			word = start / POLYGON_NODE_LIST_SIZE;
	
			/* une autre écriture pour le modulo une puissance de 2*/
			start &= POLYGON_NODE_LIST_SIZE - 1; 
			
			offset = 0;
			
			for(id = 0 ; id<32 && word < POLYGON_LIST_WORD_N ; id++)
			{
				mask = (link>>id);
				mask &= 0x01;
				mask = mask<<(start + offset);
				neighbors[word] |= mask;			
				
				offset++;
				if(start + offset >= 32)
				{
					offset = 0;
					start = 0;	
					word++;
				}
			}
		}	
	}
	
	/* Supprime le noeud voisin d'un noeud */
	static void POLYGON_delete_neighbor_to_nodelist(polygon_node_list_t neighbors[], polygon_node_id_t delete_node)
	{	
		Uint32 mask;
		polygon_node_id_t word;
		if(neighbors == NULL)
		{
			polygon_printf("neighbors NULL dans POLYGON_delete_neighbor_to_nodelist\r\n");
		}
	
	
		if(delete_node < POLYGON_LIST_MAX)
		{
			word = 0;
	
			word = delete_node / POLYGON_NODE_LIST_SIZE;
	
			/* une autre écriture pour le modulo une puissance de 2*/
			delete_node &= POLYGON_NODE_LIST_SIZE - 1; 
		
			mask = ((Uint32)1)<<delete_node;		
			neighbors[word] &= ~mask;
		}	
	}
	
	/* Supprime tous les noeuds voisin */
	static void POLYGON_delete_all_neighbors_to_nodelist(polygon_node_list_t neighbors[])
	{	
		polygon_node_id_t word;
		if(neighbors == NULL)
		{
			polygon_printf("neighbors NULL dans POLYGON_delete_all_neighbors_to_nodelist\r\n");
		}
	
		for(word = 0 ; word < POLYGON_LIST_WORD_N; word++)
		{
			neighbors[word] = 0;
		}	
	}
	
	/* On demande si le noeud a pour voisin le noeud "linkedNode" */
	static bool_e POLYGON_read_nodelist(polygon_node_list_t neighbors[], polygon_node_id_t read_node)
	{
		Uint32 mask;
		polygon_node_id_t word;
		if(neighbors == NULL)
		{
			polygon_printf("neighbors NULL dans POLYGON_read_nodelist\r\n");
		}
	
		if(read_node < POLYGON_LIST_MAX)
		{
			mask = 0;
	
			word = read_node / POLYGON_NODE_LIST_SIZE;
	
			/* une autre écriture pour le modulo une puissance de 2*/
			read_node &= POLYGON_NODE_LIST_SIZE - 1; 
		
			mask = ((Uint32)1)<<read_node;
			mask &= neighbors[word];
	
			return (bool_e)((mask==0)?FALSE:TRUE);
		}	
		else
		{
			return (bool_e)FALSE;
		}
	}
	
	
	
	/* On demande si la nodeListe est vide */
	static bool_e POLYGON_empty_nodelist(polygon_node_list_t neighbors[])
	{
		if(neighbors == NULL)
		{
			polygon_printf("neighbors NULL dans POLYGON_empty_nodelist\r\n");
		}
		polygon_node_id_t word;
		bool_e empty = TRUE;
	
		for(word = 0; word < POLYGON_LIST_WORD_N && empty; word++)
		{
			empty = (neighbors[word] == 0);
		}
		return empty;
	}
	
	
	/*Fonctions d'affichages pour le debug*/
	#ifdef DEBUG_POLYGON
		
		static void POLYGON_print_graph(polygon_graph_t* graph)
		{
			int i;
			polygon_printf("\n-------------\nGraphe : \n");
			polygon_printf("\nFrom : ");
			POLYGON_print_point(&((*graph).from));
			polygon_printf("\n\nDestination :");
			POLYGON_print_point(&((*graph).destination));
			polygon_printf("\n");
	
			for(i=0; i<(*(*graph).polygons).polygonsNumber;i++)
			{
				polygon_printf("\n Polygon %d :\n",i);
				POLYGON_print_polygon(&((*(*graph).polygons).polygon[i]));
				polygon_printf("\n");
			}
			polygon_printf("\n-------------\n");
		}	
	
		static void POLYGON_print_polygon(polygon_t* polygon)
		{
			int i;
			for(i=0; i<MAX_SUMMITS_POLYGON;i++)
			{
				polygon_printf("\n point %d :",i);
				POLYGON_print_point(&((*polygon).point[i]));
				polygon_printf("\n");
			}
		}	
		static void POLYGON_print_point(polygon_point_t* point)
		{
			polygon_printf(" (%d ; %d), ",(*point).x,	(*point).y);
			POLYGON_print_neighbors((*point).neighbors);
		}
	
		static void POLYGON_print_neighbors(polygon_node_list_t neighbors[])
		{
			int i;
			polygon_printf("0x");
			for(i = 1; i<=POLYGON_LIST_WORD_N;i++)
			{
				polygon_printf("%08lX",neighbors[POLYGON_LIST_WORD_N-i]);
			}
		}
	
	#endif

#else	// USE_POLYGON non utilisé
	// USE_POLYGON est commenté, on n'utilise pas ce morceau de code
#endif
