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

#ifndef _ASTAR_H_
	#define _ASTAR_H_

	#ifdef USE_ASTAR

		#include "QS/QS_all.h"
		#include "avoidance.h"
		#include "QS/QS_measure.h"
		#include "QS/QS_who_am_i.h"
		#include "QS/QS_maths.h"
		#include <stdarg.h>
		#include "state_machine_helper.h"


//-------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------- Définitions des macros  ------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
		//Rayon du polygone d'évitement pour les robots adverses
		#define DEFAULT_FOE_RADIUS  600

		//Nombre d'essais consécutifs avec du DODGE en évitement
		#define NB_TRY_WHEN_DODGE 6

		//Indices des polygones
		#define OUR_START_ZONE 0
		#define FOE_START_ZONE 1
		#define STAIRS_ZONE 2
		#define ESTRAD_ZONE 3

		//Nombre maximum de noeud voisins pour un noeud donné
		#define NB_MAX_NEIGHBORS 20

		//Nombre maximum de sommets par polygones
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

		//Nombre de déplacements maximal
		#define NB_MAX_DISPLACEMENTS  20



//-------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- Définitions des types structrés ---------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

		//Type structuré du node entré par l'utilisateur
		typedef struct{
			Uint16 x, y;
			bool_e curve;
		}astar_user_node_t;

		//Type structuré coût d'un noeud
		typedef struct{
			Uint16 total;  //Cout total = somme du cout (départ -> node parent) + (step) + (heuristic)
			Uint16 heuristic;  //Cout de déplacement du node courant vers le node d'arrivée (= heuristic)
			Uint16 step;  //Cout de déplacement du node parent vers le node courant (= step)
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
			bool_e enableByUser[NB_MAX_POLYGONS];
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



//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- Fonctions importantes de l'algo A* ------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Fonction générant la liste de polygones (adversaires + éléments ou zones de jeu)
		void ASTAR_generate_polygon_list(Uint8 *currentNodeId, Uint16 foeRadius);

		//Procédure permettant de réaliser les initialisations nécessaires
		void ASTAR_init(void);

		//Fonction pour créer les polygones correspondant aux zones ou éléments du terrain constituant des obstacles
		void ASTAR_create_element_polygon(Uint8 *currentId, Uint8 nbSummits, ...);

		//Fonction pour créer les polygones correspondant aux 2 robots adverses
		void ASTAR_create_foe_polygon(Uint8 *currentId, Uint16 foeRadius);

		//Fonction permettant de générer le graphe de nodes
		void ASTAR_generate_graph(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint8 *currentNodeId);

		//Fonction qui effectue l'algorithme A* (un algorithme de recherche de chemin dans un graphe)
		void ASTAR_compute_pathfind(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint16 foeRadius);

		//Fonction qui recherche et affecte des nodes en tant que "voisins" du node courant
		void ASTAR_link_nodes_on_path(astar_ptr_node_t from, astar_ptr_node_t destination, Uint8 recursivityOrder);

		//Fonction d'optimisation de la trajectoire afin d'éliminer les nodes inutiles pour gagner du temps quand on le peut.
		void ASTAR_make_the_path(astar_path_t *path);

		//Fonction transformant la trajectoire de nodes en trjaectoire de points.
		void ASTAR_make_displacements(astar_path_t path, displacement_curve_t displacements[], Uint8 *nbDisplacements, PROP_speed_e speed);

		//Machine à état réalisant le try_going
		Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);



		//Fonction pour vérifier si un point est présent dans l'aire de jeu et s'il est utilisable suivant les polygones définit sur le terrain
		bool_e ASTAR_node_enable(astar_ptr_node_t nodeTested, bool_e withPolygons, bool_e withFoes);

		//Fonction pour vérifier si un point est à l'extérieur d'un polygone
		bool_e ASTAR_point_out_of_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested);

		//Fonction qui recherche si un node est visible à partir d'un autre node. En cas d'échec, cette fonction retourne
		//les nodes consitituant les extrémités du segment le plus proche de lui et qui empêche l'accès au node d'arrivée.
		bool_e ASTAR_node_is_visible(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);

		//Fonction qui recherche si un node est atteignable à partir d'un autre node. En cas d'échec, cette fonction retourne
		//les nodes consitituant les extrémités du segment le plus proche de lui et qui empêche l'accès au node d'arrivée.
		//Cette  foçnction est très similaire à ASTAR_node_is_visible mais sans "l'aspect voisin". On cherche à savoir si le
		//node est atteignable et non pas si il doit être pris en compte comme point éventuel dans la trajectoire du robot.
		//Cette fonction est utilisé pour optimiser la trajectoire du robot.
		bool_e ASTAR_node_is_reachable(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);

		//Fonction pour la mise à jour des coûts des nodes qui ont pour parent "parent" (parent = le node current)
		void ASTAR_update_cost(Uint16 minimal_cost, astar_ptr_node_t from, astar_ptr_node_t destination);



//--------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------- Fonctions annexes --------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Procédure permettant de nettoyer un liste (RAZ du nombre d'éléments qu'elle contient)
		void ASTAR_clean_list(astar_list_t *list);

		//Fonction indiquant si une liste est vide
		bool_e ASTAR_list_is_empty(astar_list_t list);

		//Procédure d'ajout d'un node à une liste
		void ASTAR_add_node_to_list(astar_ptr_node_t node, astar_list_t *list);

		//Procédure de suppression d'un node dans une liste
		void ASTAR_delete_node_to_list(astar_ptr_node_t node, astar_list_t *list);

		//Fonction recherchant si un node est présent dans une liste
		bool_e ASTAR_is_in_list(astar_ptr_node_t node, astar_list_t list);

		//Procédure ajoutant un voisin à un node
		void ASTAR_add_neighbor_to_node(astar_ptr_node_t node, astar_ptr_node_t neighbor);

		//Fonction retournant l'intersection de deux segements
		GEOMETRY_point_t ASTAR_intersection_is(GEOMETRY_segment_t seg1, GEOMETRY_segment_t seg2);

		//Fonction retournant si il y a une intersection entre un point et un de ses voisins avec un autre polygone
		bool_e ASTAR_neighbors_intersection(astar_ptr_node_t from, astar_ptr_node_t neighbor);

		//Calcul du cout entre deux points par une distance de manhattan
		Uint16 ASTAR_pathfind_cost(astar_ptr_node_t start_node, astar_ptr_node_t end_node);



//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Fonctions d'affichage ------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Procédure affichant la liste des polygones et leurs nodes
		void ASTAR_print_polygon_list();

		//Procédure affichant la liste des polygones et leurs nodes avec plus de détails sur leurs paramètres
		void ASTAR_print_polygon_list_details();

		//Procédure affichant une liste de nodes
		void ASTAR_print_list(astar_list_t list);

		//Procédure affichant la liste ouverte
		void ASTAR_print_opened_list();

		//Procédure affichant la liste fermée
		void ASTAR_print_closed_list();

		//Procédure affichant les voisins de chaque node
		void ASTAR_print_neighbors();

		//Procédure affichant la liste des nodes constituant la trajectoire
		void ASTAR_print_path(astar_path_t path);



//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Accesseurs -----------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Procédure permettant d'activer un polygone
		void ASTAR_enable_polygon(Uint8 polygonNumber);

		//Procédure permettant de désactivé un polygone
		void ASTAR_disable_polygon(Uint8 polygonNumber);

	#endif /* def USE_ASTAR */
#endif /* ndef ASTAR_H */
