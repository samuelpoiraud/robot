/*
 *	Club Robot ESEO 2015 - 2016
 *
 *
 *	Fichier : astar.c
 *	Package : Stratégie
 *	Description : 	Fonctions de génération des trajectoires
 *					par le biais d'un algo de type A*
 *	Auteurs : Valentin BESNARD
 *	Version 20150701
 */

#include "astar.h"
#include "QS/QS_all.h"
#include "QS/QS_measure.h"
#include "QS/QS_maths.h"
#include <stdarg.h>
#include "state_machine_helper.h"

#ifdef _ASTAR_H_

#define LOG_PREFIX "astar: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ASTAR
#include "QS/QS_outputlog.h"

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Macros ---------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Rayon du polygone d'évitement pour les robots adverses
		#define DEFAULT_FOE_RADIUS  600

		//Nombre d'essais consécutifs avec du DODGE en évitement
		#define NB_TRY_WHEN_DODGE 6

		//Dimension du terrain
		#define PLAYGROUND_WIDTH 2000
		#define PLAYGROUND_HEIGHT 3000

		//Largeur du robot
		#define ROBOT_WIDTH ((QS_WHO_AM_I_get()== BIG_ROBOT)? BIG_ROBOT_WIDTH:SMALL_ROBOT_WIDTH)

		//Marge entre le centre du robot et un obstacle
		#define MARGIN_TO_OBSTACLE (ROBOT_WIDTH/2 + 50)

		//Marge entre le centre du robot et le coin d'un obstacle
		#define CORNER_MARGIN_TO_OBSTACLE  MARGIN_TO_OBSTACLE/1.4

		//Marge entre le centre du robot et la bordure (Les nodes dans cette zone sont supprimés)
		#define MARGIN_TO_BORDER (MARGIN_TO_OBSTACLE - 10)

		//Cout maximal
		#define MAX_COST 65535

		//Pour spécifier qu'un noeud (node) n'a pas de polygonId
		#define NO_ID 255

//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------- Fonctions importantes de l'algo A* (internes au programme)-------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Fonction générant la liste de polygones (adversaires + éléments ou zones de jeu)
		static void ASTAR_generate_polygon_list(Uint8 *currentNodeId, Uint16 foeRadius);

		//Fonction pour créer les polygones correspondant aux zones ou éléments du terrain constituant des obstacles
		static void ASTAR_create_element_polygon(Uint8 *currentId, Uint8 nbSummits, ...);

		//Fonction pour créer les polygones correspondant aux 2 robots adverses
		static void ASTAR_create_foe_polygon(Uint8 *currentId, Uint16 foeRadius);

		//Fonction permettant de générer le graphe de nodes
		static void ASTAR_generate_graph(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint8 *currentNodeId);

		//Fonction qui effectue l'algorithme A* (un algorithme de recherche de chemin dans un graphe)
		static void ASTAR_compute_pathfind(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint16 foeRadius);

		//Fonction qui recherche et affecte des nodes en tant que "voisins" du node courant
		static void ASTAR_link_nodes_on_path(astar_ptr_node_t from, astar_ptr_node_t destination, Uint8 recursivityOrder);

		//Fonction d'optimisation de la trajectoire afin d'éliminer les nodes inutiles pour gagner du temps quand on le peut.
		static void ASTAR_make_the_path(astar_path_t *path);

		//Fonction transformant la trajectoire de nodes en trjaectoire de points.
		static void ASTAR_make_displacements(astar_path_t path, displacement_curve_t displacements[], Uint8 *nbDisplacements, PROP_speed_e speed);

		//Fonction qui recherche si un node est visible à partir d'un autre node. En cas d'échec, cette fonction retourne
		//les nodes consitituant les extrémités du segment le plus proche de lui et qui empêche l'accès au node d'arrivée.
		static bool_e ASTAR_node_is_visible(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);

		//Fonction qui recherche si un node est atteignable à partir d'un autre node. En cas d'échec, cette fonction retourne
		//les nodes consitituant les extrémités du segment le plus proche de lui et qui empêche l'accès au node d'arrivée.
		//Cette  foçnction est très similaire à ASTAR_node_is_visible mais sans "l'aspect voisin". On cherche à savoir si le
		//node est atteignable et non pas si il doit être pris en compte comme point éventuel dans la trajectoire du robot.
		//Cette fonction est utilisé pour optimiser la trajectoire du robot.
		static bool_e ASTAR_node_is_reachable(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);

		//Fonction pour la mise à jour des coûts des nodes qui ont pour parent "parent" (parent = le node current)
		static void ASTAR_update_cost(Uint16 minimal_cost, astar_ptr_node_t from, astar_ptr_node_t destination);


//--------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------- Fonctions annexes (internes au programme) --------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Procédure permettant de nettoyer un liste (RAZ du nombre d'éléments qu'elle contient)
		static void ASTAR_clean_list(astar_list_t *list);

		//Fonction indiquant si une liste est vide
		static bool_e ASTAR_list_is_empty(astar_list_t list);

		//Procédure d'ajout d'un node à une liste
		static void ASTAR_add_node_to_list(astar_ptr_node_t node, astar_list_t *list);

		//Procédure de suppression d'un node dans une liste
		static void ASTAR_delete_node_to_list(astar_ptr_node_t node, astar_list_t *list);

		//Fonction recherchant si un node est présent dans une liste
		static bool_e ASTAR_is_in_list(astar_ptr_node_t node, astar_list_t list);

		//Procédure ajoutant un voisin à un node
		static void ASTAR_add_neighbor_to_node(astar_ptr_node_t node, astar_ptr_node_t neighbor);

		//Fonction retournant l'intersection de deux segements (NOT USED)
		//static GEOMETRY_point_t ASTAR_intersection_is(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2);

		//Fonction retournant si il y a une intersection entre un point et un de ses voisins avec un autre polygone (NOT USED)
		//static bool_e ASTAR_neighbors_intersection(astar_ptr_node_t from, astar_ptr_node_t neighbor);

		//Calcul du cout entre deux points par une distance de manhattan
		static Uint16 ASTAR_pathfind_cost(astar_ptr_node_t start_node, astar_ptr_node_t end_node);




//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Variables globales ---------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//La liste des polygones
		astar_polygon_list_t polygon_list;

		//La liste ouverte pour l'algo A*
		astar_list_t opened_list;

		//La liste fermée pour l'algo A*
		astar_list_t closed_list;


//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- Fonctions importantes de l'algo A* ------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------


/** @brief ASTAR_generate_polygon_list
 *		Fonction générant la liste de polygones (adversaires + éléments ou zones de jeu)
 * @param currentNodeId : le numéro d'identité des nodes afin de ne pas donner le même id à deux nodes
 * @param foeRadius : la distance caractéristique des polygones modélisant les robots  adverses
 */
static void ASTAR_generate_polygon_list(Uint8 *currentNodeId, Uint16 foeRadius){
	polygon_list.nbPolygons = 0;  //réinitialisation du nombre de polygones

	//Attention, les nodes doivent être écartés au maximum de 250mm sur un même segment

	//Polygon[0]: Notre zone de départ (Node 0 -> 6)
	ASTAR_create_element_polygon(currentNodeId, 7, (astar_user_node_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(0), TRUE},
									(astar_user_node_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){778-CORNER_MARGIN_TO_OBSTACLE,COLOR_Y(400+CORNER_MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1000,COLOR_Y(400+MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1222+CORNER_MARGIN_TO_OBSTACLE,COLOR_Y(400+CORNER_MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(0), TRUE});

	//Polygon[1]:Zone de départ adverses (Node 7 -> 15)
	ASTAR_create_element_polygon(currentNodeId, 9, (astar_user_node_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(3000), TRUE},
									(astar_user_node_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(3000-MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){778-MARGIN_TO_OBSTACLE,COLOR_Y(2500), TRUE},
									(astar_user_node_t){778-CORNER_MARGIN_TO_OBSTACLE,COLOR_Y(2450-CORNER_MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1000,COLOR_Y(2300-MARGIN_TO_OBSTACLE),TRUE},
									(astar_user_node_t){1222+CORNER_MARGIN_TO_OBSTACLE,COLOR_Y(2450-CORNER_MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(2500), TRUE},
									(astar_user_node_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(3000-MARGIN_TO_OBSTACLE), TRUE},
									(astar_user_node_t){1222+MARGIN_TO_OBSTACLE,COLOR_Y(3000), TRUE});

	//Polygon[2]:Zones des marches (Node 16 -> 26)
	ASTAR_create_element_polygon(currentNodeId, 11, (astar_user_node_t){0, 967-MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){MARGIN_TO_OBSTACLE, 967-MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){580+CORNER_MARGIN_TO_OBSTACLE,967-CORNER_MARGIN_TO_OBSTACLE, FALSE},
									(astar_user_node_t){580+MARGIN_TO_OBSTACLE,1000, FALSE},
									(astar_user_node_t){580+MARGIN_TO_OBSTACLE,1250, TRUE},
									(astar_user_node_t){580+MARGIN_TO_OBSTACLE,1500, TRUE},
									(astar_user_node_t){580+MARGIN_TO_OBSTACLE,1750, TRUE},
									(astar_user_node_t){580+MARGIN_TO_OBSTACLE,2000, FALSE},
									(astar_user_node_t){580+CORNER_MARGIN_TO_OBSTACLE,2033+CORNER_MARGIN_TO_OBSTACLE, FALSE},
									(astar_user_node_t){MARGIN_TO_OBSTACLE, 2033+MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){0,2033+MARGIN_TO_OBSTACLE, TRUE});

	//Polygon[3]:Zones de l'estrade (Node 27 -> 35)
	ASTAR_create_element_polygon(currentNodeId, 9, (astar_user_node_t){1900-CORNER_MARGIN_TO_OBSTACLE, 1200-CORNER_MARGIN_TO_OBSTACLE, FALSE},
									(astar_user_node_t){2000-MARGIN_TO_OBSTACLE,1200-MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){2000,1200-MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){2000,1800+MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){2000-MARGIN_TO_OBSTACLE,1800+MARGIN_TO_OBSTACLE, TRUE},
									(astar_user_node_t){1900-CORNER_MARGIN_TO_OBSTACLE,1800+CORNER_MARGIN_TO_OBSTACLE, FALSE},
									(astar_user_node_t){1900-MARGIN_TO_OBSTACLE,1750, TRUE},
									(astar_user_node_t){1900-MARGIN_TO_OBSTACLE,1500, TRUE},
									(astar_user_node_t){1900-MARGIN_TO_OBSTACLE,1250, TRUE});

	//Polygones des robots adverses
	ASTAR_create_foe_polygon(currentNodeId, foeRadius);
}



/** @brief ASTAR_init
 *		Procédure permettant de réaliser les initialisations nécessaires
 */
void ASTAR_init(void){
	Uint8 i;
	for(i=0; i<NB_MAX_POLYGONS; i++){
		polygon_list.enableByUser[i] = TRUE;
	}
}



/** @brief ASTAR_create_element_polygon
 *		Fonction pour créer les polygones correspondant aux zones ou éléments du terrain constituant des obstacles
 * @param currentId : le numéro d'identité auquel on est rendu
 * @param nbSummits : le nombre de sommets du polygone crée
 */
static void ASTAR_create_element_polygon(Uint8 *currentId, Uint8 nbSummits,...){
	int i;
	int index = polygon_list.nbPolygons;
	va_list listePoints;
	va_start(listePoints, nbSummits);
	astar_user_node_t point;

	for(i=0; i< nbSummits; i++){
		point = va_arg(listePoints, astar_user_node_t);
		polygon_list.polygons[index].summits[i].id = ((*currentId)++);
		polygon_list.polygons[index].summits[i].polygonId = index;
		polygon_list.polygons[index].summits[i].pos = (GEOMETRY_point_t){point.x, point.y};
		polygon_list.polygons[index].summits[i].enable = TRUE; //node enable par défaut. L'affectation de ce champ ne peut se faire que lorsque tous les polygones sont créés.
		polygon_list.polygons[index].summits[i].curve = point.curve;
		polygon_list.polygons[index].summits[i].cost.total = MAX_COST;
		polygon_list.polygons[index].summits[i].cost.heuristic = MAX_COST;
		polygon_list.polygons[index].summits[i].cost.step = MAX_COST;
		polygon_list.polygons[index].summits[i].parent = NULL;  //Le parent est affecté par l'algo A*
		polygon_list.polygons[index].summits[i].nbNeighbors = 0;  //Les voisins sont affectés par la suite, on initialise donc à 0.
	}

	polygon_list.polygons[index].id = index;
	polygon_list.polygons[index].nbSummits = nbSummits;
	polygon_list.polygons[index].enable = polygon_list.enableByUser[index];
	polygon_list.nbPolygons++;
}



/** @brief ASTAR_create_foe_polygon
 *		Fonction pour créer les polygones correspondant aux 2 robots adverses
 * @param currentId : le numéro d'identité auquel on est rendu
 * @param foeRadius : la distance caractéristique du polygone créé (environ égale au rayon)
 */
static void ASTAR_create_foe_polygon(Uint8 *currentId, Uint16 foeRadius){
	Uint8 i, j, nbFoes =0;
	Uint8 index = polygon_list.nbPolygons;
	GEOMETRY_point_t foe;
	Uint16 dist;
	Uint16 dist_sin, dist_cos;

	for(i=0; i<MAX_NB_FOES ; i++){
		if(global.foe[i].enable){
			foe.x = global.foe[i].x;
			foe.y = global.foe[i].y;

			debug_printf("Foe %d in position x=%d   y=%d\n", i, foe.x, foe.y);

			//Si le petit robot adverse est très petit, inutile de surdimensionné l'hexagone
			//Prise en compte dynamique de la taille du robot adverse qaund on se trouve très proche de lui
			dist = MIN(foeRadius, GEOMETRY_distance(foe, (GEOMETRY_point_t){global.pos.x, global.pos.y}) + 50);
			dist_sin = dist*sin4096(PI4096/6);
			dist_cos = dist*cos4096(PI4096/6);
			debug_printf("Polygon Foe  dist= %d  dist_sin= %d   dist_cos= %d\n", dist, dist_sin, dist_cos);

			//Affectation des coordonnées des points du polygone
			polygon_list.polygons[index].summits[0].pos.x = foe.x + dist;
			polygon_list.polygons[index].summits[0].pos.y = foe.y;

			polygon_list.polygons[index].summits[1].pos.x = foe.x + dist_sin;
			polygon_list.polygons[index].summits[1].pos.y = foe.y + dist_cos;

			polygon_list.polygons[index].summits[2].pos.x = foe.x - dist_sin;
			polygon_list.polygons[index].summits[2].pos.y = foe.y + dist_cos;

			polygon_list.polygons[index].summits[3].pos.x = foe.x - dist;
			polygon_list.polygons[index].summits[3].pos.y = foe.y;

			polygon_list.polygons[index].summits[4].pos.x = foe.x - dist_sin;
			polygon_list.polygons[index].summits[4].pos.y = foe.y - dist_cos;

			polygon_list.polygons[index].summits[5].pos.x = foe.x + dist_sin;
			polygon_list.polygons[index].summits[5].pos.y = foe.y - dist_cos;

			//Affectation des autres paramètres du polygone
			polygon_list.polygons[index].id = index;
			polygon_list.polygons[index].nbSummits = 6;
			polygon_list.polygons[index].enable = TRUE;

			//Affectation des données de chaque node
			for(j=0; j<6; j++){
				polygon_list.polygons[index].summits[j].id = ((*currentId)++);
				polygon_list.polygons[index].summits[j].polygonId = index;
				polygon_list.polygons[index].summits[j].enable = TRUE; //node enable par défaut
				polygon_list.polygons[index].summits[j].curve = TRUE;
				polygon_list.polygons[index].summits[j].cost.total = MAX_COST;
				polygon_list.polygons[index].summits[j].cost.heuristic = MAX_COST;
				polygon_list.polygons[index].summits[j].cost.step = MAX_COST;
				polygon_list.polygons[index].summits[j].parent = NULL ; //Le parent est affecté par la suite
				polygon_list.polygons[index].summits[j].nbNeighbors = 0; //initialisation à 0
			}
			nbFoes++;
			index++;
		}
	}

	debug_printf("nbFoes = %d\n\n",index-polygon_list.nbPolygons);
	polygon_list.nbPolygons += nbFoes;
}



/** @brief ASTAR_generate_graph
 *		Fonction permettant de générer le graphe de nodes
 * @param path : la trajectoire du robot contenant le node de départ, le node d'arrivée et les nodes du chemin à emprunter
 * @param from : les coordonnées du point de départ
 * @destination : les coordonnées du point d'arrivée
 * @param currentNodeId : le numéro d'identité des nodes afin de ne pas donner le même id à deux nodes
 */
static void ASTAR_generate_graph(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint8 *currentNodeId){
	Uint8 i, j;

	debug_printf("From x=%d  y=%d\n", from.x, from.y);

	//Création du node de départ
	path->from.id = (*currentNodeId)++;
	path->from.polygonId = NO_ID;
	path->from.pos = from;
	path->from.enable = TRUE; //On considère que la position de départ du robot est valide.
	path->from.cost.total = 0; //Ca ne coute rien d'aller de from vers from
	path->from.cost.heuristic = 0; //de même
	path->from.cost.step = 0; //de même
	path->from.parent = NULL; //Le noeud de départ n'a pas de parent, c'est un noeud racine.
	path->from.nbNeighbors = 0;

	//Création du node d'arrivée
	path->destination.id = (*currentNodeId)++;
	path->destination.polygonId = NO_ID;
	path->destination.pos = destination;
	path->destination.enable = ASTAR_node_enable(&(path->destination), FALSE, FALSE);
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
				polygon_list.polygons[i].summits[j].enable = ASTAR_node_enable(&(polygon_list.polygons[i].summits[j]), TRUE, TRUE);
			}
		}
	}

	//Création des liens entre les noeuds d'un même polygone (cela revient à dessiner les polygones)
	//On affecte les voisins connus de chaque node.
	for(i=0; i<polygon_list.nbPolygons; i++){
		//Affectation du premier sommet (on lui affecte le second sommet et le dermier comme voisins)
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[0]), &(polygon_list.polygons[i].summits[1]));
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[0]), &(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1]));

		//Affectation des sommets "normaux" (on lui affecte le sommet précédent et le sommet suivant comme voisins)
		for(j=1; j<polygon_list.polygons[i].nbSummits-1; j++){
			ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[j]), &(polygon_list.polygons[i].summits[j-1]));
			ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[j]), &(polygon_list.polygons[i].summits[j+1]));
		}

		//Affectation du dernier sommet (on lui affecte l'avant dernier sommet et le premier comme voisins)
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1]), &(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-2]));
		ASTAR_add_neighbor_to_node(&(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1]), &(polygon_list.polygons[i].summits[0]));
	}
}

/** @brief ASTAR_compute_pathfind
 *		Fonction qui effectue l'algorithme A* (un algorithme de recherche de chemin dans un graphe)
 * @param path : la trajectoire du robot contenant le node de départ, le node d'arrivée et les nodes du chemin à emprunter
 * @param from : les coordonnées du point de départ
 * @param destination : les coordonnées du point d'arrivée
 * @param currentNodeId : le numéro d'identité des nodes afin de ne pas donner le même id à deux nodes
 */
static void ASTAR_compute_pathfind(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint16 foeRadius){
	debug_printf("Pathfind Begin\n");

	//Déclaration des variables
	bool_e  destination_is_in_closed_list = FALSE;
	Uint16 minimal_cost; //Cout minimal ie cout entre le node de départ et le node courant
	astar_ptr_node_t current = NULL; //Le node courant
	Uint8 i; // Variable de parcours
	Uint8 currentNodeId = 0; //la variable pour affecter les ID

	//Génération de la liste de polygones
	ASTAR_generate_polygon_list(&currentNodeId, foeRadius);

	//Affichage de la liste de polygones
	ASTAR_print_polygon_list();

	//Génération du graphe
	ASTAR_generate_graph(path, from, destination, &currentNodeId);

	//Affichage de la liste de polygones en détails
	//ASTAR_print_polygon_list_details();

	//if de protection afin que l'algorithme soit lancé uniquement si le point de destination est valide
	debug_printf("\nTest du point de destination \n");
	if(ASTAR_node_enable(&(path->destination), TRUE, FALSE)){

		//Affichage de certains détails
		//ASTAR_print_neighbors();
		//ASTAR_print_polygon_list_details();

		//Nettoyage des différentes listes
		ASTAR_clean_list(&(path->list)); //La liste contenant le chemin
		ASTAR_clean_list(&opened_list); //La liste ouverte
		ASTAR_clean_list(&closed_list); //La liste fermée

		//Ajout du noeud de départ à la liste ouverte
		ASTAR_add_node_to_list(&(path->from), &opened_list);

		//Affichage du contenu de la liste fermée et de la liste ouverte
		debug_printf("BEGIN  état des listes : \n");
		ASTAR_print_closed_list();
		ASTAR_print_opened_list();

		//////// Boucle de l'algorithme A* ////////
		//Tant que la liste ouverte n'est pas vide et que le node de destination n'a pas été ajouté à la liste fermé
		while(!ASTAR_list_is_empty(opened_list) && !destination_is_in_closed_list){

			//Recherche dans la liste ouverte du node avec le cout le plus faible. Ce node devient le node courant (current).
			minimal_cost = opened_list.list[0]->cost.total;
			current = opened_list.list[0];
			for(i=1; i<opened_list.nbNodes; i++){
				if(opened_list.list[i]->cost.total <= minimal_cost){
					minimal_cost = opened_list.list[i]->cost.total;
					current = opened_list.list[i];
				}
			}

			//Ajout du noeud courant dans la closed_list et suppression de celui_ci dans l'opened_list
			ASTAR_add_node_to_list(current, &closed_list);
			ASTAR_delete_node_to_list(current, &opened_list);

			//On regarde si le node courant est le node de destination (ou d'arrivée)
			if(current == &(path->destination)){
				destination_is_in_closed_list = TRUE;
				debug_printf("destination_is_in_closed_list\n");
			}

			if(!destination_is_in_closed_list){
				//Recherche des nodes adjacents (ou voisins) au node current. On ajoute ces nodes à la liste ouverte.
				ASTAR_link_nodes_on_path(current, &(path->destination), 10);

				//Mise à jour des coûts des noeuds qui ont pour parent current
				ASTAR_update_cost(minimal_cost, current, &(path->destination));
			}

			//Affichage des listes
			debug_printf("\n\n\nNode current x=%d  y=%d  -------------------------------------------------------------\n", current->pos.x, current->pos.y);
			ASTAR_print_closed_list();
			ASTAR_print_opened_list();

		}

		//Reconstitution du chemin
		ASTAR_make_the_path(path);
		ASTAR_print_path(*path);

		debug_printf("Pathfind end\n");

	}else{ //Protection si le node destination est dans un polygone fixe ou trop près de la bordure
		SD_printf("ASTAR: Destination node is DISABLE\n");
		debug_printf("ASTAR: Destination node is DISABLE\n");
	}
}

/** @brief ASTAR_link_nodes_on_path
 *		Fonction qui recherche et affecte des nodes en tant que "voisins" du node courant
 * @param path : la trajectoire du robot contenant le node de départ, le node d'arrivée et les nodes du chemin à emprunter
 * @param from : le node de départ
 * @param destination : le node d'arrivée
 * @param recursivityOrder : l'ordre de récursivité afin d'éviter les boucles infinies.
 *		Le premier appel de cet fonction ne peut s'appeller elle-même que recursivityOrder fois.
 */
static void ASTAR_link_nodes_on_path(astar_ptr_node_t from, astar_ptr_node_t destination, Uint8 recursivityOrder){
	debug_printf("\n\n\nLink Begin\n");
	Uint16 test_cost;
	Uint8  k;
	astar_ptr_node_t nodeAnswer1 = NULL, nodeAnswer2 = NULL;
	bool_e is_in_closed_list = FALSE;
	bool_e is_in_opened_list = FALSE;
	//GEOMETRY_segment_t seg;
	//GEOMETRY_point_t middle = GEOMETRY_segment_middle(seg);
	//astar_ptr_node_t nodeMid = NULL;

	//Ajout des voisins de from à la liste ouverte
	for(k=0; k<from->nbNeighbors; k++){

		//portion de code pas au point ((codé pour résoudre un bug)
		/*seg = (GEOMETRY_segment_t){from->pos, from->neighbors[k]->pos};
		debug_printf("Tentative d' ajout\n");
		debug_printf("Middle of (%d , %d)  et (%d , %d)\n", seg.a.x , seg.a.y, seg.b.x, seg.b.y);
		Sint16 xCoord = (seg.a.x + seg.b.x);
		Sint16 yCoord = (seg.a.y + seg.b.y);
		nodeMid->pos.x = xCoord;
		nodeMid->pos.y = yCoord;
		debug_printf("Node middle x=%d, y=%d\n",xCoord, yCoord);
		debug_printf("Node middle x=%d, y=%d\n", nodeMid->pos.x, nodeMid->pos.y);
		nodeMid->id = NO_ID;
		nodeMid->polygonId = NO_ID;
		nodeMid->enable = TRUE;
		nodeMid->nbNeighbors = 0;
		nodeMid->parent = NULL;
		debug_printf("Node ENABLE to add neighbor = %d\n", ASTAR_node_enable(nodeMid, TRUE, TRUE));*/

		if(from->neighbors[k]->enable /*&& ASTAR_node_enable(nodeMid, TRUE, TRUE)*/ &&  !ASTAR_is_in_list(from->neighbors[k], opened_list) && !ASTAR_is_in_list(from->neighbors[k], closed_list)){
			ASTAR_add_node_to_list(from->neighbors[k], &opened_list);
			from->neighbors[k]->parent = from;
			from->neighbors[k]->cost.step = ASTAR_pathfind_cost(from, from->neighbors[k]);
			debug_printf("neighbors added: x=%d  y=%d\n", from->neighbors[k]->pos.x, from->neighbors[k]->pos.y);
		}
	}

	//on regarde si le node de destination est visible par le node de départ
	bool_e answer = ASTAR_node_is_visible(&nodeAnswer1, &nodeAnswer2, from, destination);
	debug_printf("Link nodes for x=%d  y=%d : answer=%d  \n", destination->pos.x, destination->pos.y,answer );


	if(recursivityOrder==0){ //Protection de la récursivité pour éviter les boucles infinies
		//nothing
		nodeAnswer1 = NULL;
		nodeAnswer2 = NULL;
	}else if(answer){ 	//le node destination peut être ajouté à la liste ouverte
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

			//Ajout du node à la liste ouverte si il n'est pas présent dans la liste fermée et dans la liste ouverte
			if(!is_in_opened_list){
				ASTAR_add_node_to_list(destination, &opened_list);
				destination->parent = from;
				destination->cost.step = ASTAR_pathfind_cost(destination->parent, destination);
			}else{
				//Si le node est déjà présent dans la liste ouverte
				//Si le cout global est inférieur avec current en tant que parent, current devient le parent du node destination
				test_cost = ASTAR_pathfind_cost(from, destination);
				if(test_cost < destination->cost.step){
					destination->parent = from;
					destination->cost.step = test_cost;
				}
			}

		}
	}
	else
	{
		//Si le node destination ne peut pas être atteint, on fait de la récursivité pour trouver d'autres voisins et être sur qu'on puisse les atteindre
		debug_printf("Link nodes: Recursivité engagé\n");
		if(nodeAnswer1 != NULL && nodeAnswer1->enable && nodeAnswer1->polygonId != from->polygonId)
			ASTAR_link_nodes_on_path(from, nodeAnswer1, recursivityOrder-1);
		if(nodeAnswer2 != NULL && nodeAnswer2->enable && nodeAnswer2->polygonId != from->polygonId)
			ASTAR_link_nodes_on_path(from, nodeAnswer2, recursivityOrder-1);
	}
	debug_printf("Link End\n");
}


/** @brief ASTAR_make_the_path
 *		Fonction d'optimisation de la trajectoire afin d'éliminer les nodes inutiles pour gagner du temps quand on le peut.
 * @param path : la trajectoire du robot contenant le node de départ, le node d'arrivée et les nodes consituant la trajectoire
 */
static void ASTAR_make_the_path(astar_path_t *path){
	astar_list_t aux; //Liste auxiliaire
	Sint16 i,j;
	astar_ptr_node_t answer1, answer2;

	//Nettoyage de la liste (Vaut mieux deux fois qu'une)
	ASTAR_clean_list(&aux);

	//Quelque soit le résultat de A*, on reconstruit le chemin même si il est incomplet. On verra plus tard ce qu'on décide de faire.
	//Le chemin est reconstruit de la destination vers le point de départ
	ASTAR_add_node_to_list(closed_list.list[closed_list.nbNodes-1], &aux);
	debug_printf("PATH: added node to path x=%d  y=%d\n", closed_list.list[closed_list.nbNodes-1]->pos.x, closed_list.list[closed_list.nbNodes-1]->pos.y);
	while(aux.list[aux.nbNodes-1] != &(path->from)){  //Tant qu'on a pas atteint le point de départ
		ASTAR_add_node_to_list(aux.list[aux.nbNodes-1]->parent, &aux);
	}

	ASTAR_print_list(aux);

	//Vérification de l'utilité de chacun des points. Parfois un des points est inutile,
	//le chemin peut directement aller du point N-1 au point N+1. On supprime donc le point N de la trajectoire.
	//On regarde donc si le second point est visible du point de départ
	//i est l'indice de parcours et j l'indice d'e parcours d'ajout dans la liste du path

	i=aux.nbNodes-1;
	j= i-2;
	while(i>0){
		while(j>=0 && ASTAR_node_is_reachable(&answer1, &answer2, aux.list[i], aux.list[j]) && (aux.list[i]->polygonId != aux.list[j]->polygonId)){
		   j--;
		}
		i = j+1;
		if(i != aux.nbNodes-1){
			ASTAR_add_node_to_list(aux.list[i],  &(path->list));
			j--;
		}
	}

	if(path->list.nbNodes>0 && path->destination.id != path->list.list[path->list.nbNodes-1]->id){

		///arret au point le plus proche de l'arrivée
		/*Uint16 min = GEOMETRY_squared_distance(path->list.list[0]->pos, path->destination.pos);
		Uint16 test;
		Uint8 index=0;
		for(i=1; i<path->list.nbNodes; i++){
			test = GEOMETRY_squared_distance(path->list.list[0]->pos, path->destination.pos);
			if(test < min){
				min = test;
				index = i;
			}
		}
			path->list.nbNodes = index + 1;
		debug_printf("PATHFIND fail, nb Nodes limited to node x=%d  y=%d because nerest node is x=%d  y=%d\n", path->list.list[path->list.nbNodes-1]->pos.x, path->list.list[path->list.nbNodes-1]->pos.y, path->list.list[index]->pos.x, path->list.list[index]->pos.y);
		*/

	   ///solution d'un déplacement
	   path->list.nbNodes = 1;
	}
}

/** @brief ASTAR_make_displacements
 *		Fonction transformant la trajectoire de nodes en trjaectoire de points.
 * @param path : la trajectoire de nodes
 * @param displacements : la trajectoire de points
 * @param nbDisplacements : le nombre de déplacements
 * @speed : la vitesse du robot
 */
static void ASTAR_make_displacements(astar_path_t path, displacement_curve_t displacements[], Uint8 *nbDisplacements, PROP_speed_e speed){
	Uint8 i;
	*nbDisplacements = path.list.nbNodes;
	debug_printf("MAKE DISPLACEMENTS\n nbDisplacements = %d\n", *nbDisplacements);
	for(i=0; i<path.list.nbNodes; i++){
		displacements[i].point = path.list.list[i]->pos;
		debug_printf("displacement x=%d  y=%d\n", displacements[i].point.x, displacements[i].point.y);
		displacements[i].speed = speed;

		//Affectation du paramètre de courbe
		if(i!=0 && path.list.list[i]->curve == FALSE && GEOMETRY_distance(path.list.list[i-1]->pos, path.list.list[i]->pos) < 300)
			displacements[i].curve = TRUE; //Si on est vraiment proche d'un point et qu on devrait s'arreter, on autorise quand même une courbe
		else
			displacements[i].curve = path.list.list[i]->curve;
	}
}



/** @brief ASTAR_try_going
 *		Machine à état réalisant le try_going
 *
 * @param x : l'abscisse u point d'arrivée
 * @param y : l'ordonnée du point d'arrivée
 * @param success_state : l'état courant dans lequel ce ASTAR_try_going est effectué en stratégie
 * @param success_state : l'état dans lequel on doit aller en cas de succès
 * @param fail_state : l'état dans lequel on doit aller en cas d'échec
 * @param speed la vitesse du robot
 * @param way : le sens de déplacements du robot
 * @param avoidance : le paramètre d'évitement
 * @param end_condition : la condition de fin (END_AT_BREAK ou END_AT_LAST_POINT)
 * @return l'état en cours ou l'état de succès ou l'état d'échec
 */
Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition){
	error_e sub_action;
	static displacement_curve_t displacements[NB_MAX_DISPLACEMENTS];
	static Uint8 nbDisplacements;
	static astar_path_t path;
	static Uint8 nbTry;
	static bool_e successPossible;
	static Uint16 foeRadius;

	CREATE_MAE_WITH_VERBOSE(SM_ID_ASTAR_TRY_GOING,
							INIT_PARAMETERS,
							INIT,
							COMPUTE,
							DISPLACEMENT,
							FAIL,
							SUCCESS
							);

	switch(state)
	{
		case INIT_PARAMETERS:{
			switch(avoidance)
			{
				case DODGE_AND_WAIT:
				case DODGE_AND_NO_WAIT:
					nbTry = NB_TRY_WHEN_DODGE;
					break;
				default:
					nbTry = 1;
					break;
			}
			foeRadius = DEFAULT_FOE_RADIUS;
			state = INIT;
		}break;

		case INIT:
			if(nbTry == 4 ||  nbTry==1)  //&& nbDisplacements < 3
				foeRadius = DEFAULT_FOE_RADIUS - 150;
			else if(nbTry == 5 ||  nbTry==2)
				foeRadius = DEFAULT_FOE_RADIUS - 100;
			else
				foeRadius = DEFAULT_FOE_RADIUS;
			debug_printf("\n\n\nASTAR_try_going with nbTry = %d ------------------------------------------------------------\n", nbTry);
			debug_printf("foeRadius = %d\n", foeRadius);
			ASTAR_compute_pathfind(&path, (GEOMETRY_point_t){global.pos.x, global.pos.y}, (GEOMETRY_point_t){x, y}, foeRadius);
			state = COMPUTE;
			break;

		case COMPUTE:
			debug_printf("Compute for try_going\n");
			ASTAR_make_displacements(path, displacements, &nbDisplacements, speed);
			debug_printf("%d displacements announced\n", nbDisplacements);
			if(displacements[nbDisplacements-1].point.x == x && displacements[nbDisplacements-1].point.y == y)
				successPossible = TRUE;
			else
				successPossible = FALSE;
			state = DISPLACEMENT;
			break;

		case DISPLACEMENT:
			if(successPossible || nbTry==4 || nbTry==1)
				sub_action = goto_pos_curve_with_avoidance(NULL, displacements, nbDisplacements, way, avoidance, end_condition, PROP_NO_BORDER_MODE);
			else
				sub_action = NOT_HANDLED;

			switch(sub_action)
			{
				case IN_PROGRESS:
					break;
				case END_OK:
					debug_printf("Displacement for try_going....success\n");
					if(successPossible){
						state = SUCCESS;
					}else{
						nbTry--;
						if(nbTry == 0)
							state = FAIL;
						else
							state = INIT;
					}
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
				default:
					debug_printf("Displacement for try_going....fail\n");
					nbTry--;
					if(nbTry == 0)
						state = FAIL;
					else
						state = INIT;
					break;
			}
			break;
		case FAIL:
			debug_printf("Finish by Fail nbTry=%d\n", nbTry);
			state = INIT;
			return fail_state;
			break;
		case SUCCESS:
			debug_printf("Finish by Success nbTry=%d\n", nbTry);
			state = INIT;
			return success_state;
			break;
	}
	return in_progress;
}



/** @brief ASTAR_node_enable
 *		Fonction pour vérifier si un point est présent dans l'aire de jeu et s'il est utilisable suivant les polygones définit sur le terrain
 * @param nodeTested : le node testé
 * @param withPolygons : booléen indiquant si on doit aussi prendre en compte les polygones correspondant aux éléments de jeu
 *		Node à l'intérieur d'un polygone ->  return FALSE
 * @param withFoes : booléen indiquant si on doit aussi prendre en compte les polygones correspondant aux robots adverses
 *		Node à l'intérieur d'un polygone ->  return FALSE
 * @return le booléen indiquant si le node est utilisable ou inutilisable
 */
bool_e ASTAR_node_enable(astar_ptr_node_t nodeTested, bool_e withPolygons, bool_e withFoes){
	Uint8 i;
	if(nodeTested->pos.x < MARGIN_TO_BORDER || nodeTested->pos.x > PLAYGROUND_WIDTH-MARGIN_TO_BORDER || nodeTested->pos.y < MARGIN_TO_BORDER || nodeTested->pos.y > PLAYGROUND_HEIGHT-MARGIN_TO_BORDER){
		//debug_printf("Node x=%d y=%d return FALSE because out of area\n", nodeTested->pos.x, nodeTested->pos.y);
		return FALSE;
	}

	if(withPolygons){
		Uint8 nbPolygons;
		if(withFoes)
			nbPolygons = polygon_list.nbPolygons;
		else
			nbPolygons = polygon_list.nbPolygons - 2;
		for(i=0; i<nbPolygons; i++){
			if(polygon_list.polygons[i].enable && polygon_list.polygons[i].id != nodeTested->polygonId && !ASTAR_point_out_of_polygon(polygon_list.polygons[i], nodeTested->pos)){
				//debug_printf("Node x=%d y=%d return FALSE because in polygon id=%d\n", nodeTested->pos.x, nodeTested->pos.y, i);
				return FALSE;
			}
		}
	}
	return TRUE;
}



/** @brief ASTAR_point_out_of_polygon
 *		Fonction pour vérifier si un point est à l'extérieur d'un polygone
 * @param polygon : le polygone pris en compte
 * @param nodeTested : le node testé
 * @return le booléen indiquant si le node est à l'extérieur (TRUE) ou à l'intérieur (FALSE) du polygone considéré
 */
bool_e ASTAR_point_out_of_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested){
	int nbIntersections=0;
	int i;
	//segment de référence dont une extrémité est à l'extérieur du polygone
	GEOMETRY_segment_t seg1 = {nodeTested, (GEOMETRY_point_t){3000, 4000}};

	//on compte le nombre d'intersection avec chaque coté du polygone
	for(i=0; i<polygon.nbSummits-1; i++){
		GEOMETRY_segment_t seg2 = {polygon.summits[i].pos, polygon.summits[i+1].pos};
		nbIntersections += GEOMETRY_segments_intersects(seg1, seg2);
		//if(GEOMETRY_segments_intersects(seg1, seg2)){
		//	debug_printf("Intersection of node (%d, %d) with seg (%d, %d)   (%d, %d)\n", nodeTested.x, nodeTested.y, seg2.a.x, seg2.a.y, seg2.b.x, seg2.b.y);
		//}
	}

	//Test de l'intersection avec le dernier segment
	GEOMETRY_segment_t seg2 = {polygon.summits[polygon.nbSummits-1].pos, polygon.summits[0].pos};
	nbIntersections += GEOMETRY_segments_intersects(seg1, seg2);
	//if(GEOMETRY_segments_intersects(seg1, seg2)){
	//	debug_printf("Intersection of node (%d, %d) with seg (%d, %d)   (%d, %d)\n", nodeTested.x, nodeTested.y, seg2.a.x, seg2.a.y, seg2.b.x, seg2.b.y);
	//}

	//Le point est à l'extérieur du polygone si le nombre d'intersections avec chacun des côté du polygone est un nombre pair.
	//Si le nombre d'intersection est un nombre impair, le node est donc à l'intérieur du polygone.
	return  (nbIntersections%2 == 0);
}



/** @brief ASTAR_node_is_visible
 *		Fonction qui recherche si un node est visible à partir d'un autre node. En cas d'échec, cette fonction retourne
 *		les nodes constituant les extrémités du segment le plus proche de lui et qui empêche l'accès au node d'arrivée.
 * @param nodeAnswer1 : premier node atteignable par le node de départ (node réponse)
 * @param nodeAnswer2 : second node atteignable par le node de départ (node réponse)
 * @param from : le node de départ
 * @param destination : le node d'arrivée
 * @return le booléen indiquant si le node destination est visible par le node from
 */
static bool_e ASTAR_node_is_visible(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination){
	debug_printf("Visible Begin\n");
	Uint16 minimal_dist = MAX_COST;
	Uint16 test_dist;
	Uint8 i, j;
	GEOMETRY_segment_t reference = {from->pos, destination->pos};
	GEOMETRY_segment_t test;

	*nodeAnswer1 = NULL;
	*nodeAnswer2 = NULL;
	for(i=0; i<polygon_list.nbPolygons; i++){
		if(polygon_list.polygons[i].enable && polygon_list.polygons[i].id != destination->polygonId){ //Le test n'est réalisé que si le polygone est "enable"
			for(j=0; j<polygon_list.polygons[i].nbSummits-1; j++){
				//On vérifie  si il y a une intersection avec chaque segment de polygone excepté le polygone auquel le node appartient
				if(polygon_list.polygons[i].summits[j].id != from->id && polygon_list.polygons[i].summits[j+1].id != from->id){
					//Vérification de l'intersection
					test.a = polygon_list.polygons[i].summits[j].pos;
					test.b = polygon_list.polygons[i].summits[j+1].pos;
					//debug_printf("Test intersection (%d , %d ) et (%d , %d)\n", test.a.x, test.a.y, test.b.x, test.b.y);
					if(GEOMETRY_segments_intersects(reference, test)){
						//Si il y a intersection, on calcule le point d'intersecsection
						debug_printf("Calcul dist ....with (%d , %d) et (%d , %d) ", test.a.x, test.a.y, test.b.x, test.b.y);
						test_dist = (GEOMETRY_distance(from->pos, test.a) + GEOMETRY_distance(from->pos, test.b))/2; //Moyenne des distances
						debug_printf("Finish with dist = %d\n", test_dist);
						if(test_dist < minimal_dist){ // Si la distance est inférieur, on sauvegarde les extrémités du segment
							debug_printf("minimal_dist affected to %d\n", test_dist);
							minimal_dist = test_dist;
							*nodeAnswer1 = &(polygon_list.polygons[i].summits[j]);
							*nodeAnswer2 = &(polygon_list.polygons[i].summits[j+1]);
						}
					}
				}
			}
			//Vérification du segment entre le premier et le dernier node de chaque polygone
			if(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].id != from->id && polygon_list.polygons[i].summits[0].id != from->id){
				//Vérification de l'intersection
				test.a = polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].pos;
				test.b = polygon_list.polygons[i].summits[0].pos;
				if(GEOMETRY_segments_intersects(reference, test)){
					//Si il y a intersection, on calcule le point d'intersecsection;
					debug_printf("Calcul dist ....with (%d , %d) et (%d , %d) ", test.a.x, test.a.y, test.b.x, test.b.y);
					test_dist = (GEOMETRY_distance(from->pos, test.a) + GEOMETRY_distance(from->pos, test.b))/2; //Moyenne des distances
					debug_printf("Finish with dist = %d\n", test_dist);
					if(test_dist < minimal_dist){ // Si la distance est inférieur, on sauvegarde les extrémités du segment
						debug_printf("minimal_dist affected\n");
						minimal_dist = test_dist;
						*nodeAnswer1 = &(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1]);
						*nodeAnswer2 = &(polygon_list.polygons[i].summits[0]);
					}
				}
			}
		}
	}
	debug_printf("Visible End\n");
	if(*nodeAnswer1 == NULL && *nodeAnswer2 == NULL)
		return TRUE;
	else
		return FALSE;
}



/** @brief ASTAR_node_is_reachable
 *		Fonction qui recherche si un node est atteignable à partir d'un autre node. En cas d'échec, cette fonction retourne
 *		les nodes consitituant les extrémités du segment le plus proche de lui et qui empêche l'accès au node d'arrivée.
 *		Cette  fonction est très similaire à ASTAR_node_is_visible mais sans "l'aspect voisin". On cherche à savoir si le
 *		node est atteignable et non pas si il doit être pris en compte comme point éventuel dans la trajectoire du robot.
 *		Cette fonction est utilisé pour optimiser la trajectoire du robot.
 * @param nodeAnswer1 : premier node atteignable par le node de départ (node réponse)
 * @param nodeAnswer2 : second node atteignable par le node de départ (node réponse)
 * @param from : le node de départ
 * @param destination : le node d'arrivée
 * @return le booléen indiquant si le node destination est atteignable par le node from
 */
static bool_e ASTAR_node_is_reachable(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination){
	debug_printf("Reachable between x=%d  y=%d  et  x=%d y=%d\n", from->pos.x, from->pos.y, destination->pos.x, destination->pos.y );
	Uint16 minimal_dist = MAX_COST;
	Uint16 test_dist;
	Uint8 i, j;
	GEOMETRY_segment_t reference = {from->pos, destination->pos};
	GEOMETRY_segment_t test;

	*nodeAnswer1 = NULL;
	*nodeAnswer2 = NULL;
	for(i=0; i<polygon_list.nbPolygons; i++){
		if(polygon_list.polygons[i].enable){ //Le test n'est réalisé que si le polygone est "enable"
			for(j=0; j<polygon_list.polygons[i].nbSummits-1; j++){
				//On vérifie  si il y a une intersection avec chaque segment de polygone excepté le polygone auquel le node appartient
				if(polygon_list.polygons[i].summits[j].id != from->id && polygon_list.polygons[i].summits[j+1].id != from->id
					   && polygon_list.polygons[i].summits[j].id != destination->id && polygon_list.polygons[i].summits[j+1].id != destination->id){
					//Vérification de l'intersection
					test.a = polygon_list.polygons[i].summits[j].pos;
					test.b = polygon_list.polygons[i].summits[j+1].pos;
					if(GEOMETRY_segments_intersects(reference, test)){
						//Si il y a intersection, on calcule le point d'intersecsection
						debug_printf("Calcul dist ....with (%d , %d) et (%d , %d) ", test.a.x, test.a.y, test.b.x, test.b.y);
						test_dist = (GEOMETRY_distance(from->pos, test.a) + GEOMETRY_distance(from->pos, test.b))/2; //Moyenne des distances
						debug_printf("Finish with dist = %d\n", test_dist);
						if(test_dist < minimal_dist){
							debug_printf("minimal_dist affected to %d\n", test_dist);
							minimal_dist = test_dist;
							*nodeAnswer1 = &(polygon_list.polygons[i].summits[j]);
							*nodeAnswer2 = &(polygon_list.polygons[i].summits[j+1]);
						}
					}
				}
			}
			//Vérification du segment entre le premier et le dernier node de chaque polygone
			if(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].id != from->id && polygon_list.polygons[i].summits[0].id != from->id
				   && polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].id != destination->id && polygon_list.polygons[i].summits[0].id != destination->id){
				//Vérification de l'intersection
				test.a = polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1].pos;
				test.b = polygon_list.polygons[i].summits[0].pos;
				if(GEOMETRY_segments_intersects(reference, test)){
					//Si il y a intersection, on calcule le point d'intersecsection
					debug_printf("Calcul dist ....with (%d , %d) et (%d , %d) ", test.a.x, test.a.y, test.b.x, test.b.y);
					test_dist = (GEOMETRY_distance(from->pos, test.a) + GEOMETRY_distance(from->pos, test.b))/2; //Moyenne des distances
					debug_printf("Finish with dist = %d\n", test_dist);
					if(test_dist < minimal_dist){
						debug_printf("minimal_dist affected\n");
						minimal_dist = test_dist;
						*nodeAnswer1 = &(polygon_list.polygons[i].summits[polygon_list.polygons[i].nbSummits-1]);
						*nodeAnswer2 = &(polygon_list.polygons[i].summits[0]);
					}
				}
			}
		}
	}
	if(*nodeAnswer1 == NULL && *nodeAnswer2 == NULL){
		debug_printf("Reachable x=%d  y=%d  et  x=%d y=%d  is TRUE\n\n", from->pos.x, from->pos.y, destination->pos.x, destination->pos.y );
		return TRUE;
	}else{
		debug_printf("Reachable x=%d  y=%d  et  x=%d y=%d  is FALSE\n\n", from->pos.x, from->pos.y, destination->pos.x, destination->pos.y );
		return FALSE;
	}
}



/** @brief ASTAR_update_cost
 *		Fonction pour la mise à jour des coûts des nodes qui ont pour parent "parent" (parent = le node current dans ASTAR_compute_pathfind)
 * @param minimal_cost : le cout_minimal du point de départ au node courant
 * @param parent : le node parent
 * @param destination : le node d'arrivée
 */
static void ASTAR_update_cost(Uint16 minimal_cost, astar_ptr_node_t parent, astar_ptr_node_t destination){
	Uint8 i;
	for(i=0; i<opened_list.nbNodes; i++){
		opened_list.list[i]->cost.heuristic = ASTAR_pathfind_cost(opened_list.list[i], destination);
		opened_list.list[i]->cost.total = minimal_cost + opened_list.list[i]->cost.step + opened_list.list[i]->cost.heuristic;
	}
}





//--------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------- Fonctions annexes --------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

/** @brief ASTAR_clean_list
 *		Procédure permettant de nettoyer un liste (RAZ du nombre d'éléments qu'elle contient)
 * @param list : la liste concernée
 */
static void ASTAR_clean_list(astar_list_t *list){
	list->nbNodes = 0;
}



/** @brief ASTAR_list_is_empty
 *		Fonction indiquant si une liste est vide
 * @param list : la liste concernée
 * @return le booléen indiquant si la liste est vide
 */
static bool_e ASTAR_list_is_empty(astar_list_t list){
	return (list.nbNodes == 0);
}



/** @brief ASTAR_add_node_to_list
 *		Procédure d'ajout d'un node à une liste
 * @param node : le node à ajouter
 * @param list : la liste concernée
 */
static void ASTAR_add_node_to_list(astar_ptr_node_t node, astar_list_t *list){
	list->list[list->nbNodes] = node;
	list->nbNodes = list->nbNodes + 1;
}



/** @brief ASTAR_delete_node_to_list
 *		Procédure de suppression d'un node dans une liste
 * @param node : le node à supprimer
 * @param list : la liste concernée
 */
static void ASTAR_delete_node_to_list(astar_ptr_node_t node, astar_list_t *list){
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



/** @brief ASTAR_is_in_list
 *		Fonction recherchant si un node est présent dans une liste
 * @param start_node : le node testé
 * @param list : la liste de recherche
 * @return un booléen indiquant si le node est présent (TRUE) dans la liste ou non (FALSE)
 */
static bool_e ASTAR_is_in_list(astar_ptr_node_t node, astar_list_t list){
	bool_e is_in_list = FALSE;
	Uint8 i=0;
	while(i<list.nbNodes && !is_in_list){
		if(list.list[i]->id == node->id)
			is_in_list = TRUE;
		else
			i++;
	}
	return is_in_list;
}



/** @brief ASTAR_add_neighbor_to_node
 *		Procédure ajoutant un voisin à un node
 * @param node : le node auquel on doit ajouter un voisin
 * @param neighbor : le node voisin à ajouter
 */
static void ASTAR_add_neighbor_to_node(astar_ptr_node_t node, astar_ptr_node_t neighbor){
	node->neighbors[node->nbNeighbors] = neighbor;
	node->nbNeighbors = node->nbNeighbors + 1;
}



/** @brief ASTAR_intersection_is  (Pas opérationnelle)
 *		Fonction retournant l'intersection de deux segements
 * @param seg1 : les coordonnées des extrémités du premier segment
 * @param seg2 : les coordonnées des extrémités du second segment
 * @return les coordonnées du point d'intersection
 */
/*static GEOMETRY_point_t ASTAR_intersection_is(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2){
	double a=1, b=1, c=1, d=1;
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

	//Equation de la droite du segment 2: Y = cX + d
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
		intersection.x = d;
		intersection.y = a*d + b;
	}else if(verticale1 && !verticale2){
		intersection.x = b;
		intersection.y = c*b + d;
	}else{
		debug_printf("Problème dans ASTAR_intersection\n");
	}
	return intersection;
}*/



/** @brief ASTAR_neighbors_intersection
 *		Fonction retournant si il y a une intersection entre un point et un de ses voisins avec un autre polygone
 * @param from: le node de départ
 * @param neighbor : le node voisin
 * @return le booléen indiquant si il y a intersection (TRUE) ou non (FALSE)
 */
/*static bool_e ASTAR_neighbors_intersection(astar_ptr_node_t from, astar_ptr_node_t neighbor){
	Uint8 i=0, j=0;
	bool_e intersection = FALSE;
	GEOMETRY_segment_t reference = {from->pos, neighbor->pos};
	GEOMETRY_segment_t test;
	while(i < polygon_list.nbPolygons && !intersection){
		if(polygon_list.polygons[i].enable){
			j=0;
			while(j < polygon_list.polygons[i].nbSummits && !intersection ){
				test = (GEOMETRY_segment_t){polygon_list.polygons[i].summits[i].pos, polygon_list.polygons[i].summits[(i+1)%polygon_list.polygons[i].nbSummits].pos};
				if(GEOMETRY_segments_intersects(reference, test) && !GEOMETRY_segments_parallel(reference, test)){
					//debug_printf("Neighbor Intersection from (%d , %d) neighbor(%d , %d)  with: (%d , %d)  (%d, %d)", from->pos.x, from->pos.y, neighbor->pos.x, neighbor->pos.y, test.a.x, test.a.y, test.b.x, test.b.y);
					intersection = TRUE;
				}
				j++;
			}
		}
		i++;
	}
	return intersection;
}*/



/** @brief ASTAR_pathfind_cost
 *		Calcul du cout entre deux points par une distance de manhattan
 * @param start_node : le node de départ
 * @param end_node: le node d'arrivée
 * @return le cout entre les deux nodes passés en paramètre
 */
static Uint16 ASTAR_pathfind_cost(astar_ptr_node_t start_node, astar_ptr_node_t end_node)
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



//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Fonctions d'affichage ------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

/** @brief ASTAR_print_polygon_list
 *		Procédure affichant la liste des polygones et leurs nodes
 */
void ASTAR_print_polygon_list(){
	int i, j;
	int nb=0;
	debug_printf("Liste des nodes \n\n");
	for(i=0; i< polygon_list.nbPolygons; i++){
		if(polygon_list.polygons[i].enable){
			for(j=0; j< polygon_list.polygons[i].nbSummits; j++){
				if(polygon_list.polygons[i].summits[j].enable){
					debug_printf("%d       %d\n", polygon_list.polygons[i].summits[j].pos.x, polygon_list.polygons[i].summits[j].pos.y);
					nb++;
				}
			}
		}
	}
	debug_printf("%d nodes written \n\n", nb);
}



/** @brief ASTAR_print_polygon_list_details
 *		Procédure affichant la liste des polygones et leurs nodes avec plus de détails sur leurs paramètres
 */
void ASTAR_print_polygon_list_details(){
	int i, j;
	int nb=0;
	debug_printf("Liste des nodes \n\n");
	for(i=0; i< polygon_list.nbPolygons; i++){
		debug_printf("polygone[%d]: polygonId=%d enable=%d\n", i, polygon_list.polygons[i].id, polygon_list.polygons[i].enable);
		if(polygon_list.polygons[i].enable){
			for(j=0; j< polygon_list.polygons[i].nbSummits; j++){
				debug_printf("\tnode id=%d   x=%d   y=%d  polygonId=%d enable=%d\n", polygon_list.polygons[i].summits[j].id, polygon_list.polygons[i].summits[j].pos.x, polygon_list.polygons[i].summits[j].pos.y, polygon_list.polygons[i].summits[j].polygonId, polygon_list.polygons[i].summits[j].enable);
				nb++;
			}
		}
	}
	debug_printf("%d nodes written \n\n", nb);
}



/** @brief ASTAR_print_list
 *		Procédure affichant une liste de nodes
 * @param list : la liste concernée
 */
void ASTAR_print_list(astar_list_t list){
	Uint8 i;
	for(i=0; i<list.nbNodes; i++){
		debug_printf("%d) Node:  pos x=%d  y=%d    parent: x=%d  y=%d\n", i, list.list[i]->pos.x, list.list[i]->pos.y, list.list[i]->parent->pos.x, list.list[i]->parent->pos.y);
		debug_printf("\t costHeuristic=%d  costStep=%d  costTotal=%d\n",list.list[i]->cost.heuristic, list.list[i]->cost.step, list.list[i]->cost.total);
	}
	debug_printf("\n\n");
}



/** @brief ASTAR_print_opened_list
 *		Procédure affichant la liste ouverte
 */
void ASTAR_print_opened_list(){
	debug_printf("\nOPENED LIST:\n");
	ASTAR_print_list(opened_list);
}



/** @brief ASTAR_print_closed_list
 *		Procédure affichant la liste fermée
 */
void ASTAR_print_closed_list(){
	debug_printf("\nCLOSED LIST:\n");
	ASTAR_print_list(closed_list);
}



/** @brief ASTAR_print_neighbors
 *		Procédure affichant les voisins de chaque node
 */
void ASTAR_print_neighbors(){
	Uint8 i, j, k;
	debug_printf("\nPRINT NEIGHBORS :");
	for(i=0; i<polygon_list.nbPolygons; i++){
		debug_printf("polygon %d \n", i);
		for(j=0; j<polygon_list.polygons[i].nbSummits; j++){
			debug_printf("\t node id=%d x=%d y=%d\n",polygon_list.polygons[i].summits[j].id, polygon_list.polygons[i].summits[j].pos.x, polygon_list.polygons[i].summits[j].pos.y);
			for(k=0; k<polygon_list.polygons[i].summits[j].nbNeighbors; k++){
				debug_printf("\t\tneighbor  id=%d x=%d y=%d\n",polygon_list.polygons[i].summits[j].neighbors[k]->id, polygon_list.polygons[i].summits[j].neighbors[k]->pos.x, polygon_list.polygons[i].summits[j].neighbors[k]->pos.y );
			}
		}
	}
	debug_printf("\n\n");
}



/** @brief ASTAR_print_path
 *		Procédure affichant la liste des nodes constituant la trajectoire
 * @param path : la trajectoire contenant la liste des nodes de la trajectoire
 */
void ASTAR_print_path(astar_path_t path){
	Uint8 i;
	debug_printf("\nPath is:\n");
	for(i=0; i<path.list.nbNodes; i++){
		debug_printf("Node: pos x=%d  y=%d curve=%d\n", path.list.list[i]->pos.x, path.list.list[i]->pos.y, path.list.list[i]->curve);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Accesseurs -----------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

/** @brief ASTAR_enable_polygon
 *		Procédure permettant d'activer un polygone
 *  @param polygonNumber : le numéro du polygon concerné
 */
void ASTAR_enable_polygon(Uint8 polygonNumber){
	polygon_list.enableByUser[polygonNumber] = TRUE;
}



/** @brief ASTAR_disnable_polygon
 *		Procédure permettant de désactiver un polygone
 *  @param polygonNumber : le numéro du polygon concerné
 */
void ASTAR_disable_polygon(Uint8 polygonNumber){
	debug_printf("ASTAR_disable_polygon :  polygonNumber = %d    polygon_list.nbPolygons = %d\n", polygonNumber, polygon_list.nbPolygons);
	polygon_list.enableByUser[polygonNumber] = FALSE;
}

#endif //_ASTAR_H_








