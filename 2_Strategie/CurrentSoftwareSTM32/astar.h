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

	#include "config/config_use.h"


	#ifdef USE_ASTAR

	#ifndef _ASTAR_H_
		#define _ASTAR_H_

	#include "QS/QS_who_am_i.h"
	#include "avoidance.h"


//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------- D�finitions des macros  ----------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

		//Nombre maximum de noeud voisins pour un noeud donn�
		#define NB_MAX_NEIGHBORS 20

		//Nombre maximum de sommets par polygones
		#define NB_MAX_SUMMITS_POLYGONS 20

		//Nombre maximum de polyg�nes
		#define NB_MAX_POLYGONS 20

		//Nombre maximum de nodes dans une liste
		#define NB_MAX_NODES 100

		//Nombre de d�placements maximal
		#define NB_MAX_DISPLACEMENTS  20

		//Indices des polygones
		#define OUR_START_ZONE 0
		#define FOE_START_ZONE 1
		#define STAIRS_ZONE 2
		#define ESTRAD_ZONE 3


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

		//Proc�dure permettant de r�aliser les initialisations n�cessaires
		void ASTAR_init(void);

		//Machine � �tat r�alisant le try_going
		Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);

		//Fonction pour v�rifier si un point est pr�sent dans l'aire de jeu et s'il est utilisable suivant les polygones d�finit sur le terrain
		bool_e ASTAR_node_enable(astar_ptr_node_t nodeTested, bool_e withPolygons, bool_e withFoes);

		//Fonction pour v�rifier si un point est � l'ext�rieur d'un polygone
		bool_e ASTAR_point_out_of_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested);





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

	#endif /* ndef _ASTAR_H_ */
#endif /* def USE_ASTAR */




