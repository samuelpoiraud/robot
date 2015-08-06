/*
 *	Club Robot ESEO 2015 - 2016
 *
 *
 *	Fichier : astar.c
 *	Package : Strat�gie
 *	Description : 	Fonctions de g�n�ration des trajectoires
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
//--------------------------------------------------- D�finitions des macros  ------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
		//Rayon du polygone d'�vitement pour les robots adverses
		#define DEFAULT_FOE_RADIUS  600

		//Nombre d'essais cons�cutifs avec du DODGE en �vitement
		#define NB_TRY_WHEN_DODGE 6

		//Indices des polygones
		#define OUR_START_ZONE 0
		#define FOE_START_ZONE 1
		#define STAIRS_ZONE 2
		#define ESTRAD_ZONE 3

		//Nombre maximum de noeud voisins pour un noeud donn�
		#define NB_MAX_NEIGHBORS 20

		//Nombre maximum de sommets par polygones
		#define NB_MAX_SUMMITS_POLYGONS 20

		//Nombre maximum de polyg�nes
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

		//Marge entre le centre du robot et la bordure (Les nodes dans cette zone sont supprim�s)
		#define MARGIN_TO_BORDER (MARGIN_TO_OBSTACLE - 10)

		//Cout maximal
		#define MAX_COST 65535

		//Pour sp�cifier qu'un noeud (node) n'a pas de polygonId
		#define NO_ID 255

		//Nombre de d�placements maximal
		#define NB_MAX_DISPLACEMENTS  20



//-------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- D�finitions des types structr�s ---------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

		//Type structur� du node entr� par l'utilisateur
		typedef struct{
			Uint16 x, y;
			bool_e curve;
		}astar_user_node_t;

		//Type structur� co�t d'un noeud
		typedef struct{
			Uint16 total;  //Cout total = somme du cout (d�part -> node parent) + (step) + (heuristic)
			Uint16 heuristic;  //Cout de d�placement du node courant vers le node d'arriv�e (= heuristic)
			Uint16 step;  //Cout de d�placement du node parent vers le node courant (= step)
		}astar_node_cost_t;

		//Type structur� noeud (ou node)
		struct astar_node{
			//Ses id
			Uint8 id; //Son id personnel
			Uint8 polygonId; //L'id de son polygon

			//Coordonn�es du point
			GEOMETRY_point_t pos;

			//Ses propri�t�s
			bool_e enable;
			bool_e curve;
			astar_node_cost_t cost;
			struct astar_node *parent;
			struct astar_node *neighbors[NB_MAX_NEIGHBORS];
			Uint8 nbNeighbors;
		};

		//Type pointeur sur node
		typedef struct astar_node *astar_ptr_node_t;

		//Type structur� polygone
		typedef struct{
			Uint8 id;
			struct astar_node summits[NB_MAX_SUMMITS_POLYGONS];
			Uint8 nbSummits;
			bool_e enable;
		}astar_polygon_t;

		//Type structur� liste de polygones
		typedef struct{
			astar_polygon_t polygons[NB_MAX_POLYGONS];
			Uint8 nbPolygons;
			bool_e enableByUser[NB_MAX_POLYGONS];
		}astar_polygon_list_t;

		//Type structur� liste de nodes (pour l'opened_list et la closed_list)
		typedef struct{
			astar_ptr_node_t list[NB_MAX_NODES];
			Uint8 nbNodes;
		}astar_list_t;

		//Type structur� path
		typedef struct{
			astar_list_t list;
			struct astar_node from, destination;
		}astar_path_t;



//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- Fonctions importantes de l'algo A* ------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Fonction g�n�rant la liste de polygones (adversaires + �l�ments ou zones de jeu)
		void ASTAR_generate_polygon_list(Uint8 *currentNodeId, Uint16 foeRadius);

		//Proc�dure permettant de r�aliser les initialisations n�cessaires
		void ASTAR_init(void);

		//Fonction pour cr�er les polygones correspondant aux zones ou �l�ments du terrain constituant des obstacles
		void ASTAR_create_element_polygon(Uint8 *currentId, Uint8 nbSummits, ...);

		//Fonction pour cr�er les polygones correspondant aux 2 robots adverses
		void ASTAR_create_foe_polygon(Uint8 *currentId, Uint16 foeRadius);

		//Fonction permettant de g�n�rer le graphe de nodes
		void ASTAR_generate_graph(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint8 *currentNodeId);

		//Fonction qui effectue l'algorithme A* (un algorithme de recherche de chemin dans un graphe)
		void ASTAR_compute_pathfind(astar_path_t *path, GEOMETRY_point_t from, GEOMETRY_point_t destination, Uint16 foeRadius);

		//Fonction qui recherche et affecte des nodes en tant que "voisins" du node courant
		void ASTAR_link_nodes_on_path(astar_ptr_node_t from, astar_ptr_node_t destination, Uint8 recursivityOrder);

		//Fonction d'optimisation de la trajectoire afin d'�liminer les nodes inutiles pour gagner du temps quand on le peut.
		void ASTAR_make_the_path(astar_path_t *path);

		//Fonction transformant la trajectoire de nodes en trjaectoire de points.
		void ASTAR_make_displacements(astar_path_t path, displacement_curve_t displacements[], Uint8 *nbDisplacements, PROP_speed_e speed);

		//Machine � �tat r�alisant le try_going
		Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);



		//Fonction pour v�rifier si un point est pr�sent dans l'aire de jeu et s'il est utilisable suivant les polygones d�finit sur le terrain
		bool_e ASTAR_node_enable(astar_ptr_node_t nodeTested, bool_e withPolygons, bool_e withFoes);

		//Fonction pour v�rifier si un point est � l'ext�rieur d'un polygone
		bool_e ASTAR_point_out_of_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested);

		//Fonction qui recherche si un node est visible � partir d'un autre node. En cas d'�chec, cette fonction retourne
		//les nodes consitituant les extr�mit�s du segment le plus proche de lui et qui emp�che l'acc�s au node d'arriv�e.
		bool_e ASTAR_node_is_visible(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);

		//Fonction qui recherche si un node est atteignable � partir d'un autre node. En cas d'�chec, cette fonction retourne
		//les nodes consitituant les extr�mit�s du segment le plus proche de lui et qui emp�che l'acc�s au node d'arriv�e.
		//Cette  fo�nction est tr�s similaire � ASTAR_node_is_visible mais sans "l'aspect voisin". On cherche � savoir si le
		//node est atteignable et non pas si il doit �tre pris en compte comme point �ventuel dans la trajectoire du robot.
		//Cette fonction est utilis� pour optimiser la trajectoire du robot.
		bool_e ASTAR_node_is_reachable(astar_ptr_node_t *nodeAnswer1, astar_ptr_node_t *nodeAnswer2, astar_ptr_node_t from, astar_ptr_node_t destination);

		//Fonction pour la mise � jour des co�ts des nodes qui ont pour parent "parent" (parent = le node current)
		void ASTAR_update_cost(Uint16 minimal_cost, astar_ptr_node_t from, astar_ptr_node_t destination);



//--------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------- Fonctions annexes --------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Proc�dure permettant de nettoyer un liste (RAZ du nombre d'�l�ments qu'elle contient)
		void ASTAR_clean_list(astar_list_t *list);

		//Fonction indiquant si une liste est vide
		bool_e ASTAR_list_is_empty(astar_list_t list);

		//Proc�dure d'ajout d'un node � une liste
		void ASTAR_add_node_to_list(astar_ptr_node_t node, astar_list_t *list);

		//Proc�dure de suppression d'un node dans une liste
		void ASTAR_delete_node_to_list(astar_ptr_node_t node, astar_list_t *list);

		//Fonction recherchant si un node est pr�sent dans une liste
		bool_e ASTAR_is_in_list(astar_ptr_node_t node, astar_list_t list);

		//Proc�dure ajoutant un voisin � un node
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

		//Proc�dure affichant la liste des polygones et leurs nodes
		void ASTAR_print_polygon_list();

		//Proc�dure affichant la liste des polygones et leurs nodes avec plus de d�tails sur leurs param�tres
		void ASTAR_print_polygon_list_details();

		//Proc�dure affichant une liste de nodes
		void ASTAR_print_list(astar_list_t list);

		//Proc�dure affichant la liste ouverte
		void ASTAR_print_opened_list();

		//Proc�dure affichant la liste ferm�e
		void ASTAR_print_closed_list();

		//Proc�dure affichant les voisins de chaque node
		void ASTAR_print_neighbors();

		//Proc�dure affichant la liste des nodes constituant la trajectoire
		void ASTAR_print_path(astar_path_t path);



//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Accesseurs -----------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

		//Proc�dure permettant d'activer un polygone
		void ASTAR_enable_polygon(Uint8 polygonNumber);

		//Proc�dure permettant de d�sactiv� un polygone
		void ASTAR_disable_polygon(Uint8 polygonNumber);

	#endif /* def USE_ASTAR */
#endif /* ndef ASTAR_H */
