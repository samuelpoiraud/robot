 /*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : polygon.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de génération des trajectoires
 *					par le biais de polygones
 *					On donne les coordonnées d'arrivées
 *					On envoie les coordonnées des noeuds selon le modèle de pathfind
 *	Auteur : Adrien GIRARD et Julien FRANCHINEAU
 *	Version 20110227
 */

#ifndef POLYGON_H_
#define POLYGON_H_

	
	#include "QS/QS_all.h"
	#include "prop_functions.h"
		
		/* Caractéristiques des polygones */
		/* Nombres de polygones maximal sur le terrain
		 * 1 robot adverse
		 * 2 rois, 2 dames
		 * 6 pions dans les zones vertes + 9 pions sur le damier
		 * ce qui nous fait un total de 9+6+2+2+1=20
		 */
	
		#define MAX_POLYGONS	20
		
		/* On utilise des pentagones comme polygones */
		#define MAX_SUMMITS_POLYGON		5
	
		/*nombre max de Node liés*/
		#define POLYGON_LIST_MAX 	102
	
		/*nombre de mot pour la liste de noeud*/
		#define POLYGON_LIST_WORD_N	4
	
		/*Index du noeud de départ dans la node list*/
		#define POLYGON_LIST_FROM 				POLYGON_LIST_MAX - 1
		
		/*Index du noeud d'arrivée dans la node list*/
		#define POLYGON_LIST_DESTINATION		POLYGON_LIST_MAX - 2
	
		/* Coût opposé (donc erreur)*/
		#define POLYGON_PATHFIND_OPPONENT_COST 65535
	
		/*dimension de l'aire de jeux et marge */
		#define POLYGON_AREA_MARGIN 310UL
	
		/*rayon du cercle circonscrit au polygon */
		#define POLYGON_CIRCLE_RADIUS 260UL
		
		/* Coordonnées des sommets du pentagone
		 * Les sommets sont numérotés dans le sens trigo
		 * Le sommet A est positionné sur un axe parallèle à l'axe X du terrain
		 */
		#define POINT_A_X	321
		#define POINT_A_Y	0
		
		#define POINT_B_X	99
		#define POINT_B_Y	306
		
		#define POINT_C_X	-260
		#define POINT_C_Y	189
		
		#define POINT_D_X	-260
		#define POINT_D_Y	-189
		
		#define POINT_E_X	99
		#define POINT_E_Y	-306
		
		/* Coordonnées du pentagone utilisé pour représenter l'adversaire */
		#define OPPONENT_POINT_A_X	618
		#define OPPONENT_POINT_A_Y	0
		
		#define OPPONENT_POINT_B_X	190
		#define OPPONENT_POINT_B_Y	588
		
		#define OPPONENT_POINT_C_X	-500
		#define OPPONENT_POINT_C_Y	363
		
		#define OPPONENT_POINT_D_X	-500
		#define OPPONENT_POINT_D_Y	-363
		
		#define OPPONENT_POINT_E_X	190
		#define OPPONENT_POINT_E_Y	-588

	
	/* Utilisation : en fonction du nombre de bits, 
	*	donc du nombre de points, considérer un tableau 
	*	de polygon_node_list_t. L'indexe 0 désignera 
	*	alors les 32 bits de poids faible. En 2010-2011 
	*	nous avons besoin de 107 points, donc de 107 bits 
	*	soit 4 mots de 32 bits = 128 bits
	*/

	/*nombre de bit pour un node list
	*	obligatoirement une puissance de 2
	*/
	#define POLYGON_NODE_LIST_SIZE 32

	typedef Uint32 polygon_node_list_t;



	/* Défini le type d'élements qui seront recherchés dans le tableau d'élements pour éviter
	 * soit tous les éléments, soit juste les notres, soit... à définir
	 */
	typedef enum
	{
		ALL_ELEMENTS = 0, OUR_AND_OPPONENT_ELEMENTS, OUR_ELEMENTS
	} polygon_elements_type_e;

	typedef Uint8  polygon_node_id_t;

	typedef struct {
		/* Coordonnées du point */
		Sint16 x;
		Sint16 y;

		/* Variables pour l'algo de point visible et l'algo A* */
		bool_e outOfArea;
		polygon_node_list_t neighbors[POLYGON_LIST_WORD_N];			
    	Uint16 cost;
    	Uint16 heuristic;
    	polygon_node_id_t parent;

	} polygon_point_t;
	
	typedef struct {
		/* Points du polygone */
		polygon_point_t point[MAX_SUMMITS_POLYGON];
	} polygon_t;
	
	typedef struct {
		/* Liste des polygones */
		/* On ajoute une case de plus pour pouvoir récupérer le nombre de polygones dans la liste */
		polygon_t polygon[MAX_POLYGONS];
	
		/* Nombre de polygon dans la liste */
		Uint8 polygonsNumber;
	} polygon_list_t;

	
	typedef struct {
		polygon_point_t from,destination;
		polygon_list_t* polygons;

	} polygon_graph_t;
	

	/* Charge dans la pile les goto avec le meilleur chemin */
	Uint16 POLYGON_compute(Sint16 xFrom, Sint16 yFrom, Sint16 xDestination, Sint16 yDestination, way_e way, PROP_speed_e speed, bool_e curve, polygon_elements_type_e typeElements, bool_e use_opponent);

	#ifdef POLYGON_C

		#include "Generic_functions.h"
		#include "Stacks.h"
		#include "Geometry.h"
		/* Fonctions de pathfinding */
		
		/* Pathfind */
		static bool_e POLYGON_pathfind(polygon_graph_t* minimal);
	
		/* calcul du coût relatif au pathfinding */
		static Uint16 POLYGON_pathfind_cost(polygon_point_t* start_point, polygon_point_t* end_point);
	
		/* Retourne un pointeur vers le point du graphe d'id correspondant*/
		static polygon_point_t* POLYGON_get_point(polygon_graph_t* graph, polygon_node_id_t id);
	
		
		static bool_e POLYGON_point_out_of_area(Sint32 x, Sint32 y);
	
		/* Fonctions de création de la liste de polygones */
	
		/* Crée un polygone */
		static void POLYGON_create_polygon(polygon_t* polygon, Sint16 x, Sint16 y);
	
		/* Crée le polygone de l'adversaire */
		static void POLYGON_create_opponent_polygon(polygon_t* opponent_polygon);
	
		/* Génère une liste et la renvoie */
		static void POLYGON_generate_list(polygon_list_t* polygons_list, polygon_elements_type_e type_elements, bool_e use_opponent);
	
		/* Fonction de génération du graphe */
	
		/* Génère le graphe minimal des points */
		static void POLYGON_generate_graph(polygon_graph_t* minimal, polygon_list_t* polygons_list, Sint16 x_from, Sint16 y_from, Sint16 x_destination, Sint16 y_destination);
	
		/* suppression des arcs, des cul-de-sac et des rebouclage dans le graphe */
		static void POLYGON_clean_graph(polygon_graph_t* graph);
			
	
		/*
		*	Foncrions d'algorythme du point visible
		*/
		
		/* Algorythme du point visible*/
		static bool_e POLYGON_point_is_visible(Sint16 xi, Sint16 yi, Sint16 xj, Sint16 yj, Sint16 xp, Sint16 yp);
	
		/* teste si le segment [A,B], A(x1,y1), B(x2,y2)
		*	est en intersection avec un segment critique(morceau de bois)
		*/	
		static bool_e POLYGON_intersect_segment(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);
	
		/* Intersection entre un segment et un polygon ou un segment critique 
		*	le segment est toujours défini par un point d'un polygon et un point(x,y);
		*/
		static bool_e POLYGON_intersect_point(polygon_graph_t* graph, Uint8 polygon_id, Uint8 point_id, Sint16 x, Sint16 y);
		
		/* Intersection entre un segment et un polygon  ou un segment critique 
		*	le segment est toujours défini par un point d'un polygon et un point d'un autre polygon
		*/
		static 	bool_e POLYGON_intersect_polygon(polygon_graph_t* graph, Uint8 polygon_id, Uint8 point_id, Uint8 polygon_id2, Uint8 point_id2);
		
		/* Intersection entre le segment départ-arrivée et un polygon  ou un segment critique 
		*	le segment est toujours défini par un point d'un polygon et un point d'un autre polygon
		*/
		static bool_e POLYGON_intersect_from_dest(polygon_graph_t* graph, Uint8 polygon_id, Uint8 point_id, polygon_node_id_t from_or_dest, bool_e* accessible);
	
		/*	Vérifie si un point est visible depuis un polygon*/
		static polygon_node_list_t POLYGON_is_visible(polygon_t* polygon, Sint16 x, Sint16 y);
	
		/*
		*	Fonctions de lecture et d'écriture des polygon_node_list_t
		*/
	
		/* On lit si le noeud readNode fait parti de la nodeliste */
		static bool_e POLYGON_read_nodelist(polygon_node_list_t neighbors[],polygon_node_id_t read_node);
	
		/* On ajoute le noeud newNode à la nodeliste */
		static void POLYGON_add_neighbor_to_nodelist(polygon_node_list_t neighbors[], polygon_node_id_t new_node);
	
		/* Définie une nodelist en fonction d'une autre nodelist et d'un offset */
		static void POLYGON_set_neighbors_to_nodelist(polygon_node_list_t neighbors[], polygon_node_list_t link, polygon_node_id_t start);
	
		/* Supprime le noeud deleteNode */
		static void POLYGON_delete_neighbor_to_nodelist(polygon_node_list_t neighbors[], polygon_node_id_t delete_node);
	
		/* Supprime tous les nodes */
		static void POLYGON_delete_all_neighbors_to_nodelist(polygon_node_list_t neighbors[]);
		
		/* On demande si la nodeListe est vide */
		static bool_e POLYGON_empty_nodelist(polygon_node_list_t neighbors[]);
	
	
		/*Fonction d'affichage pour le debug*/
		
		#ifdef DEBUG_POLYGON
			#define polygon_printf(...)	debug_printf(__VA_ARGS__)
			
			static void POLYGON_print_graph(polygon_graph_t* graph);
		
			static void POLYGON_print_polygon(polygon_t* polygon);
		
			static void POLYGON_print_point(polygon_point_t* point);
		
			static void POLYGON_print_neighbors(polygon_node_list_t neighbors[]);
		#else
			#define polygon_printf(...)	 
			#define POLYGON_print_graph(...)  
		#endif
	#endif

#endif /* POLYGON_H_ */
