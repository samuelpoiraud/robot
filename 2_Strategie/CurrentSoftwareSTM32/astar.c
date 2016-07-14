/*
 *	Club Robot ESEO 2015 - 2017
 *
 *
 *	Fichier : astar.c
 *	Package : Stratégie
 *	Description : 	Fonctions de génération des trajectoires
 *					par le biais d'un algo de type A*
 *	Auteurs : Valentin BESNARD
 *	Version 2
 */

#include "astar.h"
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include "QS/QS_all.h"
#include "QS/QS_measure.h"
#include "QS/QS_maths.h"
#include "state_machine_helper.h"
#include "Supervision/SD/SD.h"

#ifdef _ASTAR_H_

#define LOG_PREFIX "astar: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ASTAR
#include "QS/QS_outputlog.h"

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Macros ---------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	// Macros pour les coûts
	#define MAX_COST (65535)		//Coût global maximal
	#define MAX_COST_ANGLE (200)	//Coût maximal pouvant être généré par un écart d'angle
	#define MAX_COST_FOES (600)     //Côût supplémentaire ajouté si on est trop près d'un adversaire

	// Macros pour les polygones
	#define NB_MAX_POLYGONS (5 + MAX_NB_FOES)	// Nombre maximal de polygones
	#define NB_MAX_NODES_IO (10)				// Nombre maximal de noeuds d'entrée/sortie
	#define NB_MAX_SUMMITS (10)					// Nombre maximal de sommets par polygone
	#define OBSTACLE_MARGIN (200)				// Marge du centre du robot avec un obstacle

	// Macros pour les hardlines
	#define NB_MAX_HARDLINES (15)				// Nombre maximale de hardlines (attention si vous ajoutez une protection, ca fait une hardline en plus
	#define HARDLINE_PROTECTION_LENGTH (200)	// Longeur de la protection d'une hardline

	//Distance à laquelle on accepte des noeuds comme voisins
	#define DISTANCE_NEIGHBOURHOOD (500)

	// Macros pour le traitement des listes
	#define ASTAR_IS_NODE_IN(nodeId, nodeList)	((nodeList) &  ((1ULL) << nodeId))
	#define ASTAR_ADD_NODE_IN(nodeId, nodeList)	((nodeList) |= ((1ULL) << (nodeId)))
	#define ASTAR_CLR_NODE_IN(nodeId, nodeList)	((nodeList) &= ~((1ULL) << (nodeId)))

	// Rayon du polygone défini pour les robots adverses
	#define FOE_RADIUS (250)

	// Macro renvoyant le noeud symétrique suivant la couleur (tout comme COLOR_Y le fait pour les coordonnées
	#define COLOR_NODE(id) ASTAR_get_symetric_node(id)

	#define NB_TRY_WITH_DODGE (3)

//-------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- Définitions des types structrés ---------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

	//Type énuméré définissant les id des nodes
	typedef enum{
		A1 = 0,
		A2,
		B1,
		B2,
		B3,
		B4,
		B5,
		C1,
		C2,
		C3,
		C4,
		D1,
		D2,
		D3,
		D4,
		D5,
		D6,
		E1,
		E2,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		G1,
		G2,
		G3,
		G4,
		H1,
		H2,
		H3,
		H4,
		H5,
		I1,
		I2,
		FROM_NODE,
		DEST_NODE,
		NB_NODES,
		NO_ID = 255
	}astar_node_id;

	//Type structuré coût d'un noeud
	typedef struct{
		Uint16 total;		//Cout total = somme du cout (départ -> node parent) + (step) + (heuristic)
		Uint16 step;		//Cout de déplacement du node parent vers le node courant (= step)
		Uint16 heuristic;	//Cout de déplacement du node courant vers le node d'arrivée (= heuristic)
	}astar_node_cost_t;

	// Type structuré voisin d'un noeud
	typedef Uint64 astar_neighbor_t;

	//Type structuré noeud (ou node)
	struct astar_node{
		//Son id
		astar_node_id id;

		//Coordonnées du point
		GEOMETRY_point_t pos;

		//Ses voisins
		Uint64 neighbors;

		//Ses propriétés
		bool_e enable;
		astar_node_cost_t cost;
		astar_node_id parent;

	};

	//Type noeud
	typedef struct astar_node astar_node_t;

	//Type structuré liste de nodes (pour l'opened_list et la closed_list)
	typedef Uint64 astar_list_t;

	// Type structuré polygone définissant une zone interdite sous forme d'un polygone
	typedef struct{
		//Son id et son nom
		Uint8 id;
		char *name;

		//Ses caractéristiques
		GEOMETRY_point_t summits[NB_MAX_SUMMITS];
		Uint8 nb_summits;
		Uint8 nodesIO[NB_MAX_NODES_IO];
		Uint8 nb_nodesIO;

		//Ses propriétés
		bool_e enable;
	}astar_polygon_t;

	// Type définissant une "hardline" c'est à dire une ligne physiquement infranchissable par le robot (ex: tasseau de bois)
	typedef struct{
		GEOMETRY_point_t ex1;
		GEOMETRY_point_t ex2;
	}astar_hardline_t;


//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------- Fonctions importantes de l'algo A* (internes au programme)-------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	static void ASTAR_user_define_obstacles();
	static void ASTAR_define_polygon(char *name, GEOMETRY_point_t polygon[], Uint8 nb_summits, bool_e enable_polygon, astar_node_id nodesIO[], Uint8 nb_nodesIO);
	static void ASTAR_define_hardline(bool_e protection_extremity_1, GEOMETRY_point_t extremity_1, GEOMETRY_point_t extremity_2, bool_e protection_extremity_2);
	static void ASTAR_create_foe_polygon(Uint16 foeRadius);
	static void ASTAR_init_nodes();
	static void ASTAR_generate_graph(GEOMETRY_point_t from, GEOMETRY_point_t dest);
	static void ASTAR_search_neighbors(astar_node_id nodeId, bool_e my_neighbors);
	static void ASTAR_link_nodes_on_path(astar_node_id current, bool_e handle_foes);
	static error_e ASTAR_compute(displacement_curve_t *displacements, Uint8 *nb_displacements, GEOMETRY_point_t from, GEOMETRY_point_t dest, PROP_speed_e speed, bool_e handle_foes);
	static error_e ASTAR_make_the_path(displacement_curve_t *displacements, Uint8 *nb_displacements, PROP_speed_e speed, astar_node_id last_node);


//--------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------- Fonctions annexes (internes au programme) --------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	static astar_node_id ASTAR_get_symetric_node(astar_node_id id);
	static Uint16 ASTAR_pathfind_cost(astar_node_id start_node, astar_node_id end_node, bool_e with_angle, bool_e with_foes);
	static void ASTAR_update_cost(astar_node_id current_node);
	static Uint16 ASTAR_abs_angle(Sint16 angle);
	static bool_e ASTAR_point_is_in_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested);
	static bool_e ASTAR_is_link_cut_by_hardlines(GEOMETRY_point_t p1, GEOMETRY_point_t p2);
	static bool_e ASTAR_is_node_visible(GEOMETRY_point_t p1, GEOMETRY_point_t p2);

	static void ASTAR_print_list(astar_list_t list);
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Variables globales ---------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	// Le graphe contenant tous les nodes
	static astar_node_t astar_nodes[NB_NODES] =
	{
		//(astar_node_t){ id,  pos, neighbors },

		//Rangée [A]
		(astar_node_t){ A1, {300,  250}, (1ULL<<B1)|(1ULL<<B2)},
		(astar_node_t){ A2, {1400, 250}, (1ULL<<B3)|(1ULL<<B4)|(1<<B5)},

		//Rangée [B]
		(astar_node_t){ B1, {300,  550}, (1ULL<<A1)|(1ULL<<B2)|(1ULL<<C1)},
		(astar_node_t){ B2, {650,  550}, (1ULL<<A1)|(1ULL<<B1)|(1ULL<<B3)|(1ULL<<C1)|(1ULL<<C2)},
		(astar_node_t){ B3, {1000, 550}, (1ULL<<A2)|(1ULL<<B2)|(1ULL<<B4)|(1ULL<<C2)|(1ULL<<C3)},
		(astar_node_t){ B4, {1350, 550}, (1ULL<<A2)|(1ULL<<B3)|(1ULL<<B5)|(1ULL<<C2)|(1ULL<<C3)|(1ULL<<C4)},
		(astar_node_t){ B5, {1700, 550}, (1ULL<<A2)|(1ULL<<B4)|(1ULL<<C3)|(1ULL<<C4)},

		//Rangée [C]
		(astar_node_t){ C1, {475,  800}, (1ULL<<B1)|(1ULL<<B2)|(1ULL<<D1)|(1ULL<<D2)},
		(astar_node_t){ C2, {1050, 750}, (1ULL<<B2)|(1ULL<<B3)|(1ULL<<B4)|(1ULL<<C3)|(1ULL<<D3)|(1ULL<<D4)},
		(astar_node_t){ C3, {1400, 800}, (1ULL<<B3)|(1ULL<<B4)|(1ULL<<B5)|(1ULL<<C2)|(1ULL<<C4)|(1ULL<<D3)|(1ULL<<D4)|(1ULL<<D6)},
		(astar_node_t){ C4, {1700, 800}, (1ULL<<B4)|(1ULL<<B5)|(1ULL<<C3)|(1ULL<<D4)|(1ULL<<D6)},

		//Rangée [D]
		(astar_node_t){ D1, {250,  1100}, (1ULL<<C1)|(1ULL<<D2)|(1ULL<<E1)},
		(astar_node_t){ D2, {475,  1150}, (1ULL<<C1)|(1ULL<<D1)|(1ULL<<E1)},
		(astar_node_t){ D3, {1050, 1150}, (1ULL<<C2)|(1ULL<<C3)|(1ULL<<D4)|(1ULL<<D5)|(1ULL<<D6)},
		(astar_node_t){ D4, {1400, 1000}, (1ULL<<C2)|(1ULL<<C3)|(1ULL<<C4)|(1ULL<<D3)|(1ULL<<D5)|(1ULL<<D6)},
		(astar_node_t){ D5, {1550, 1250}, (1ULL<<D3)|(1ULL<<D4)|(1ULL<<D6)|(1ULL<<E2)},
		(astar_node_t){ D6, {1700, 1100}, (1ULL<<C3)|(1ULL<<C4)|(1ULL<<D4)|(1ULL<<D5)|(1ULL<<E2)},

		//Rangée [E]
		(astar_node_t){ E1, {475,  1500}, (1ULL<<D1)|(1ULL<<D2)|(1ULL<<F1)|(1ULL<<F2)},
		(astar_node_t){ E2, {1700, 1500}, (1ULL<<D5)|(1ULL<<D6)|(1ULL<<F5)|(1ULL<<F6)},

		//Rangée [F]
		(astar_node_t){ F1, {250,  1900}, (1ULL<<G1)|(1ULL<<F2)|(1ULL<<E1)},
		(astar_node_t){ F2, {475,  1850}, (1ULL<<G1)|(1ULL<<F1)|(1ULL<<E1)},
		(astar_node_t){ F3, {1050, 1850}, (1ULL<<G2)|(1ULL<<G3)|(1ULL<<F4)|(1ULL<<F5)|(1ULL<<F6)},
		(astar_node_t){ F4, {1400, 2000}, (1ULL<<G2)|(1ULL<<G3)|(1ULL<<G4)|(1ULL<<F3)|(1ULL<<F5)|(1ULL<<F6)},
		(astar_node_t){ F5, {1550, 1750}, (1ULL<<F3)|(1ULL<<F4)|(1ULL<<F6)|(1ULL<<E2)},
		(astar_node_t){ F6, {1700, 1900}, (1ULL<<G3)|(1ULL<<G4)|(1ULL<<F4)|(1ULL<<F5)|(1ULL<<E2)},


		//Rangée [G]
		(astar_node_t){ G1, {475,  2200}, (1ULL<<H1)|(1ULL<<H2)|(1ULL<<F1)|(1ULL<<F2)},
		(astar_node_t){ G2, {1050, 2250}, (1ULL<<H2)|(1ULL<<H3)|(1ULL<<H4)|(1ULL<<G3)|(1ULL<<F3)|(1ULL<<F4)},
		(astar_node_t){ G3, {1400, 2200}, (1ULL<<H3)|(1ULL<<H4)|(1ULL<<H5)|(1ULL<<G2)|(1ULL<<G4)|(1ULL<<F3)|(1ULL<<F4)|(1ULL<<F6)},
		(astar_node_t){ G4, {1700, 2200}, (1ULL<<H4)|(1ULL<<H5)|(1ULL<<G3)|(1ULL<<F4)|(1ULL<<F6)},

		//Rangée [H]
		(astar_node_t){ H1, {300,  2450}, (1ULL<<I1)|(1ULL<<H2)|(1ULL<<G1)},
		(astar_node_t){ H2, {650,  2450}, (1ULL<<I1)|(1ULL<<H1)|(1ULL<<H3)|(1ULL<<G1)|(1ULL<<G2)},
		(astar_node_t){ H3, {1000, 2450}, (1ULL<<I2)|(1ULL<<H2)|(1ULL<<H4)|(1ULL<<G2)|(1ULL<<G3)},
		(astar_node_t){ H4, {1350, 2450}, (1ULL<<I2)|(1ULL<<H3)|(1ULL<<H5)|(1ULL<<G2)|(1ULL<<G3)|(1ULL<<G4)},
		(astar_node_t){ H5, {1700, 2450}, (1ULL<<I2)|(1ULL<<H4)|(1ULL<<G3)|(1ULL<<G4)},

		//Rangée [I]
		(astar_node_t){ I1, {300,  2750}, (1ULL<<H1)|(1ULL<<H2)},
		(astar_node_t){ I2, {1400, 2750}, (1ULL<<H3)|(1ULL<<H4)|(1ULL<<H5)},

		//Node de départ
		(astar_node_t){ FROM_NODE, {0, 0},  0ULL},

		//Node de destination
		(astar_node_t){ DEST_NODE, {0, 0},  0ULL}
	};


	// Les polygones qui définissent des zones interdites d'accés pendant les trajectoires du pathfind
	static astar_polygon_t astar_polygons[NB_MAX_POLYGONS];
	static Uint8 astar_nb_polygons = 0;   // Le nombre total de polygones (obstacles + robots adverses)
	static Uint8 astar_nb_obstacles = 0;  // Le nombre total d'obstacles

	// Les hardlines qui définissent des lignes physiquement infranchissables par le robot (ex: tasseau de bois)
	static astar_hardline_t astar_hardlines[NB_MAX_HARDLINES];
	static Uint8 astar_nb_hardlines = 0;

	// Les listes
	static astar_list_t opened_list;
	static astar_list_t closed_list;




//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- Fonctions importantes de l'algo A* ------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------


	void ASTAR_init(){
		//debug_printf("sizeof = %d\n",sizeof(Uint8));
		//astar_node_t *machin = (Uint8*)malloc(sizeof(Uint8));
		displacement_curve_t displacements[60];
		Uint8 nb_displacements = 0;
		Uint8 i;
		ASTAR_user_define_obstacles();
		ASTAR_print_obstacles();

		for(i=0; i<NB_NODES; i++){
			debug_printf("COLOR_NODES of %d is %d\n", i, COLOR_NODE(i));
		}



		bool_e result = ASTAR_is_node_visible((GEOMETRY_point_t){1500, 500}, (GEOMETRY_point_t){300, 2500});
		debug_printf("astar is node visible = %d\n", result);

		//ASTAR_compute(displacements, &nb_displacements, (GEOMETRY_point_t){200, 1500}, (GEOMETRY_point_t){1200, 1700}, FAST, FALSE);
		debug_printf("\nAFFICHAGE PATH\n");
		for(i=0; i<nb_displacements; i++){
			debug_printf("pos(%d;%d) \n", displacements[i].point.x, displacements[i].point.y);
		}

	}


	#define POLYGON_OUR_DEPOSE_ZONE (0)
	#define POLYGON_ADV_DEPOSE_ZONE (1)
	#define POLYGON_OUR_START_ZONE	(2)
	#define POLYGON_ADV_START_ZONE	(3)
	static void ASTAR_user_define_obstacles(){
		////////////////////////////////////////// DEFINITION DES POLYGONES ///////////////////////////////////////////////////////
		// Notre zone de dépose
		GEOMETRY_point_t poly_our_depose_zone[3] = {(GEOMETRY_point_t){750, COLOR_Y(900 - OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){1350 + OBSTACLE_MARGIN, 1500},
													(GEOMETRY_point_t){750, 1500}};
		astar_node_id nodesIO_our_depose_zone[5] = {COLOR_NODE(C2), COLOR_NODE(C3), COLOR_NODE(D4), COLOR_NODE(D5), COLOR_NODE(D6)};
		ASTAR_define_polygon("our_depose_zone", poly_our_depose_zone, 3, FALSE, nodesIO_our_depose_zone, 5);

		// La zone de dépose adverse
		GEOMETRY_point_t poly_adv_depose_zone[5] = {(GEOMETRY_point_t){750, 1500},
													(GEOMETRY_point_t){1350 + OBSTACLE_MARGIN, 1500},
													(GEOMETRY_point_t){750 + (600 + OBSTACLE_MARGIN)*cos(M_PI/6.0), COLOR_Y(1500 + (600 + OBSTACLE_MARGIN)*sin(M_PI/6.0))},
													(GEOMETRY_point_t){750 + (600 + OBSTACLE_MARGIN)*cos(2*M_PI/6.0), COLOR_Y(1500 + (600 + OBSTACLE_MARGIN)*sin(2*M_PI/6.0))},
													(GEOMETRY_point_t){750, COLOR_Y(2100 + OBSTACLE_MARGIN)}};
		astar_node_id nodesIO_adv_depose_zone[5] = {COLOR_NODE(G2), COLOR_NODE(G3), COLOR_NODE(F4), COLOR_NODE(F5), COLOR_NODE(F6)};
		ASTAR_define_polygon("adv_depose_zone", poly_adv_depose_zone, 5, TRUE, nodesIO_adv_depose_zone, 5);

		// Notre zone de départ
		GEOMETRY_point_t poly_our_start_zone[4] = {(GEOMETRY_point_t){400, COLOR_Y(0)},
												   (GEOMETRY_point_t){400, COLOR_Y(400)},
												   (GEOMETRY_point_t){1300, COLOR_Y(400)},
												   (GEOMETRY_point_t){1300, COLOR_Y(0)}};
		astar_node_id nodesIO_our_start_zone[6] = {COLOR_NODE(A1), COLOR_NODE(A2), COLOR_NODE(B1), COLOR_NODE(B2), COLOR_NODE(B3), COLOR_NODE(B4)};
		ASTAR_define_polygon("our_start_zone", poly_our_start_zone, 4, TRUE, nodesIO_our_start_zone, 6);

		// La zone de départ adverse
		GEOMETRY_point_t poly_adv_start_zone[4] = {(GEOMETRY_point_t){400, COLOR_Y(3000)},
												   (GEOMETRY_point_t){400, COLOR_Y(2600)},
												   (GEOMETRY_point_t){1300, COLOR_Y(2600)},
												   (GEOMETRY_point_t){1300, COLOR_Y(3000)}};
		astar_node_id nodesIO_adv_start_zone[6] = {COLOR_NODE(I1), COLOR_NODE(I2), COLOR_NODE(H1), COLOR_NODE(H2), COLOR_NODE(H3), COLOR_NODE(H4)};
		ASTAR_define_polygon("adv_start_zone", poly_adv_start_zone, 4, TRUE, nodesIO_adv_start_zone, 6);


		////////////////////////////////////////// DEFINITION DES HARDLINES ///////////////////////////////////////////////////////
		// tasseau cube de 8
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){0, 800}, (GEOMETRY_point_t){200 + HARDLINE_PROTECTION_LENGTH, 800}, TRUE);
		// tasseau cube de 8
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){0, 2200}, (GEOMETRY_point_t){200 + HARDLINE_PROTECTION_LENGTH, 2200}, TRUE);
		// tasseau zone de dépose
		ASTAR_define_hardline(TRUE,(GEOMETRY_point_t){750, 900 - HARDLINE_PROTECTION_LENGTH}, (GEOMETRY_point_t){750, 2100 + HARDLINE_PROTECTION_LENGTH}, TRUE);
		// plexi zone de dépose
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){750, 1500}, (GEOMETRY_point_t){1350 + HARDLINE_PROTECTION_LENGTH, 1500}, TRUE);

	}

	/** @brief ASTAR_disable_nodes_in_polygon
	 *		Fonction permettant de définir un polygone dans lequel les noeuds seront désactivés (pas pris en compte dans le compute)
	 *	@param polygon : le polygone définissant la zone souhaité
	 *  @param nbPoints : le nombre de points du polygone
	 */
	static void ASTAR_define_polygon(char *name, GEOMETRY_point_t polygon[], Uint8 nb_summits, bool_e enable_polygon, astar_node_id nodesIO[], Uint8 nb_nodesIO){
		int i;

		astar_polygons[astar_nb_polygons].id = astar_nb_polygons;
		astar_polygons[astar_nb_polygons].name = name;
		astar_polygons[astar_nb_polygons].enable = enable_polygon;

		// Réservation de l'espace mémoire
	/*	if(nb_summits)
			polygons_obstacles[nb_obstacles].summits = (GEOMETRY_point_t *)malloc(sizeof(GEOMETRY_point_t)*nb_summits);
		else
			polygons_obstacles[nb_obstacles].summits = NULL;
		if(nb_nodesIO)
			polygons_obstacles[nb_obstacles].nodesIO = (Uint8 *)malloc(sizeof(Uint8)*nb_nodesIO);
		else
			polygons_obstacles[nb_obstacles].nodesIO = NULL;*/

		// Affetation du polygone
		for(i=0; i<nb_summits; i++){
			astar_polygons[astar_nb_polygons].summits[i] = polygon[i];
		}
		astar_polygons[astar_nb_polygons].nb_summits = nb_summits;

		// Affectation de ses noeuds d'entrée/sortie
		for(i=0; i<nb_nodesIO; i++){
			astar_polygons[astar_nb_polygons].nodesIO[i] = nodesIO[i];
		}
		astar_polygons[astar_nb_polygons].nb_nodesIO = nb_nodesIO;

		astar_nb_polygons++;
		astar_nb_obstacles++;
		assert(astar_nb_polygons < NB_MAX_POLYGONS);
	}

	static void ASTAR_define_hardline(bool_e protection_extremity_1, GEOMETRY_point_t extremity_1, GEOMETRY_point_t extremity_2, bool_e protection_extremity_2){
		double angle;
		if(protection_extremity_1){
			//On ajoute une autre hardline de longueur HARDLINE_PROTECTION_LENGTH/2 qui protegera l'extrémité 1 de la hardline voulue
			angle = atan2(extremity_2.y - extremity_1.y, extremity_2.x - extremity_1.x) + M_PI/2.0;
			astar_hardlines[astar_nb_hardlines].ex1.x = extremity_1.x + HARDLINE_PROTECTION_LENGTH*cos(angle);
			astar_hardlines[astar_nb_hardlines].ex1.y = extremity_1.y + HARDLINE_PROTECTION_LENGTH*sin(angle);
			astar_hardlines[astar_nb_hardlines].ex2.x = extremity_1.x - HARDLINE_PROTECTION_LENGTH*cos(angle);
			astar_hardlines[astar_nb_hardlines].ex2.y = extremity_1.y - HARDLINE_PROTECTION_LENGTH*sin(angle);
			astar_nb_hardlines++;
			assert(astar_nb_hardlines < NB_MAX_HARDLINES);
		}

		//On ajoute la harline voulue
		astar_hardlines[astar_nb_hardlines].ex1 = extremity_1;
		astar_hardlines[astar_nb_hardlines].ex2 = extremity_2;
		astar_nb_hardlines++;
		assert(astar_nb_hardlines < NB_MAX_HARDLINES);

		if(protection_extremity_2){
			//On ajoute une autre hardline de longueur HARDLINE_PROTECTION_LENGTH/2 qui protegera l'extrémité 2 de la hardline voulue
			angle = atan2(extremity_1.y - extremity_2.y, extremity_1.x - extremity_2.x) + M_PI/2.0;
			astar_hardlines[astar_nb_hardlines].ex1.x = extremity_2.x + HARDLINE_PROTECTION_LENGTH*cos(angle);
			astar_hardlines[astar_nb_hardlines].ex1.y = extremity_2.y + HARDLINE_PROTECTION_LENGTH*sin(angle);
			astar_hardlines[astar_nb_hardlines].ex2.x = extremity_2.x - HARDLINE_PROTECTION_LENGTH*cos(angle);
			astar_hardlines[astar_nb_hardlines].ex2.y = extremity_2.y - HARDLINE_PROTECTION_LENGTH*sin(angle);
			astar_nb_hardlines++;
			assert(astar_nb_hardlines < NB_MAX_HARDLINES);
		}
	}



	/** @brief ASTAR_create_foe_polygon
	 *		Fonction pour créer les polygones correspondant aux 2 robots adverses
	 * @param currentId : le numéro d'identité auquel on est rendu
	 * @param foeRadius : la distance caractéristique du polygone créé (environ égale au rayon)
	 */
	static void ASTAR_create_foe_polygon(Uint16 foeRadius){
		Uint8 i, nbFoes =0;
		GEOMETRY_point_t foe;
		Uint16 dist;
		Uint16 dist_sin, dist_cos;

		for(i=0; i<MAX_NB_FOES ; i++){
			if(global.foe[i].enable){
				foe.x = global.foe[i].x;
				foe.y = global.foe[i].y;
				nbFoes++;

				debug_printf("Foe %d in position x=%d   y=%d\n", i, foe.x, foe.y);

				//Si le petit robot adverse est très petit, inutile de surdimensionné l'hexagone
				//Prise en compte dynamique de la taille du robot adverse qaund on se trouve très proche de lui
				dist = MIN(foeRadius, GEOMETRY_distance(foe, (GEOMETRY_point_t){global.pos.x, global.pos.y}));
				dist_sin = dist*sin4096(PI4096/6);
				dist_cos = dist*cos4096(PI4096/6);
				debug_printf("Polygon Foe  dist= %d  dist_sin= %d   dist_cos= %d\n", dist, dist_sin, dist_cos);

				//Affectation des coordonnées des points du polygone
				astar_polygons[astar_nb_polygons].summits[0].x = foe.x + dist;
				astar_polygons[astar_nb_polygons].summits[0].y = foe.y;

				astar_polygons[astar_nb_polygons].summits[1].x = foe.x + dist_sin;
				astar_polygons[astar_nb_polygons].summits[1].y = foe.y + dist_cos;

				astar_polygons[astar_nb_polygons].summits[2].x = foe.x - dist_sin;
				astar_polygons[astar_nb_polygons].summits[2].y = foe.y + dist_cos;

				astar_polygons[astar_nb_polygons].summits[3].x = foe.x - dist;
				astar_polygons[astar_nb_polygons].summits[3].y = foe.y;

				astar_polygons[astar_nb_polygons].summits[4].x = foe.x - dist_sin;
				astar_polygons[astar_nb_polygons].summits[4].y = foe.y - dist_cos;

				astar_polygons[astar_nb_polygons].summits[5].x = foe.x + dist_sin;
				astar_polygons[astar_nb_polygons].summits[5].y = foe.y - dist_cos;



				//Affectation des autres paramètres du polygone
				astar_polygons[astar_nb_polygons].id = i;
				astar_polygons[astar_nb_polygons].nb_summits = 6;
				//La prise en compte de la distance de l'adversaire permet d'éviter à nos robots de se trouver dans des polygones de
				//robots adverses, le nombre de nodesIO peut donc être mis à 0 (voir si ca pose problème dans l'utilisation
				astar_polygons[astar_nb_polygons].nb_nodesIO = 0;
				astar_polygons[astar_nb_polygons].enable = TRUE;

				astar_nb_polygons++;
			}
		}

		debug_printf("nbFoes = %d\n\n",nbFoes);
	}

	/** @brief ASTAR_init_nodes
	 *		Fonction permettant d'initialiser les propriétés principales des nodes
	 */
	static void ASTAR_init_nodes(){
		Uint8 i;
		Uint16 id = 0;

		for(i=0; i<NB_NODES; i++){
			astar_nodes[i].id = id;
			id++;
			astar_nodes[i].enable = TRUE;
			astar_nodes[i].parent = NO_ID;
			astar_nodes[i].cost.heuristic = MAX_COST;
			astar_nodes[i].cost.step = MAX_COST;
			astar_nodes[i].cost.total = MAX_COST;

			// Il faut aussi penser à nettoyer le voisin DEST_NODE de tous les noeuds, la position finale demandée change à chaque fois
			ASTAR_CLR_NODE_IN(DEST_NODE, astar_nodes[i].neighbors);
		}

		// On nettoie aussi tous les voisins de FROM_NODE et de DEST_FROM
		astar_nodes[FROM_NODE].neighbors = 0ULL;
		astar_nodes[DEST_NODE].neighbors = 0ULL;
	}

	static void ASTAR_generate_graph(GEOMETRY_point_t from, GEOMETRY_point_t dest){
		Uint8 i, j;

		// Création des polygones adversaires
		astar_nb_polygons = astar_nb_obstacles;  // Réinitialisation des polygones adversaires correspndant à un ancien appel de l'astar
		ASTAR_create_foe_polygon(FOE_RADIUS);

		// Il faut désactiver tous les noeuds qui sont à l'intérieur d'un polygone valide
		for(i=0; i<astar_nb_polygons; i++){
			if(astar_polygons[i].enable){
				for(j=0; j<NB_NODES - 2; j++){ //On ne traite pas le noeud de départ et le noeud d'arrivée
					//On évite de faire le calcul si le noeud est déjà désactivé
					if(astar_nodes[j].enable && ASTAR_point_is_in_polygon(astar_polygons[i], astar_nodes[j].pos)){
						astar_nodes[j].enable = FALSE;
						debug_printf("Node %d désactivé car dans le polygon %s\n", j, astar_polygons[i].name);
					}
				}
			}
		}

		//Mise à jour du noeud de départ
		astar_nodes[FROM_NODE].pos = from;
		astar_nodes[FROM_NODE].enable = TRUE;

		//Mise à jour du noeud d'arrivée
		astar_nodes[DEST_NODE].pos = dest;
		astar_nodes[DEST_NODE].enable = TRUE;

		ASTAR_search_neighbors(FROM_NODE, TRUE);
		debug_printf("FROM_NODE pos(%d;%d)\n", astar_nodes[FROM_NODE].pos.x, astar_nodes[FROM_NODE].pos.y);
		ASTAR_search_neighbors(DEST_NODE, FALSE);
	}

	static void ASTAR_search_neighbors(astar_node_id nodeId, bool_e my_neighbors){
		Uint8 i, j;
		bool_e in_polygon = FALSE;
		Uint32 dist = 0;
		Uint32 dist_ref = 0;
		Uint8 nb_search = 0;
		bool_e finish_search = FALSE;

		debug_printf("\nGENERATE GRAPH\n");
		for(i=0; i<astar_nb_polygons; i++){
			if(astar_polygons[i].enable){
				// Si le node "nodeId" est dans un polygone
				if(ASTAR_point_is_in_polygon(astar_polygons[i], astar_nodes[nodeId].pos)){
					in_polygon = TRUE;
					debug_printf("in_polygon TRUE(%s)\n", astar_polygons[i].name);
					for(j=0; j<astar_polygons[i].nb_nodesIO; j++){
						if(my_neighbors){
							// Je cherche mes voisins (voisins de nodeId), donc on ajoute les nodesIO du polygones comme mes voisins
							debug_printf("Node=%d ajoute comme voisin par FROM_NODE\n", astar_polygons[i].nodesIO[j]);
							ASTAR_ADD_NODE_IN(astar_polygons[i].nodesIO[j], astar_nodes[nodeId].neighbors);
						}else{
							// Je cherche de qui je peut être voisin, les nodesIO du polygone m'ajoute comme voisin
							debug_printf("Node=%d ajoute comme voisin à DEST_NODE\n", astar_polygons[i].nodesIO[j]);
							ASTAR_ADD_NODE_IN(nodeId, astar_nodes[astar_polygons[i].nodesIO[j]].neighbors);
						}
					}
				}
			}
		}

		// Si le node nodeId n'est pas dans un polygone
		// On effectue un premier passage avec une distance de recherche donné
		// On effectue un second passage avec une distance de recherche plus large si aucun noeud n'a été trouvé au premier passage
		if(!in_polygon){
			do{
				if(nb_search == 0)
					dist_ref = DISTANCE_NEIGHBOURHOOD*DISTANCE_NEIGHBOURHOOD; // On pred le carré pour éviter de calculer la racine carrée couteuse en temps CPU
				else
					dist_ref = 4*DISTANCE_NEIGHBOURHOOD*DISTANCE_NEIGHBOURHOOD; // Doubler la distance, revient à multiplier par 4 le carré de la distance
				for(i=0; i<NB_NODES; i++){
					if(i != nodeId && astar_nodes[i].enable){
						dist = GEOMETRY_squared_distance(astar_nodes[nodeId].pos, astar_nodes[i].pos);
						//Si la distance est acceptable et que ce lien n'est pas coupé par une hardlines
						if(dist < dist_ref && !ASTAR_is_link_cut_by_hardlines(astar_nodes[nodeId].pos, astar_nodes[i].pos)){
							if(my_neighbors){
								// Je cherche mes voisins (voisins de nodeId), donc on ajoute le node trouvé comme mon voisin
								debug_printf("Node=%d ajoute comme voisin par FROM_NODE\n", i);
								ASTAR_ADD_NODE_IN(i, astar_nodes[nodeId].neighbors);
								finish_search = TRUE;
							}else{
								// Je cherche de qui je peut être voisin, les node trouvé m'ajoute comme voisin
								debug_printf("Node=%d ajoute comme voisin à DEST_NODE\n", i);
								ASTAR_ADD_NODE_IN(nodeId, astar_nodes[i].neighbors);
								finish_search = TRUE;
							}
						}
					}
				}
				nb_search++;
			}while(!finish_search && nb_search < 2);
		}
	}

	static void ASTAR_link_nodes_on_path(astar_node_id current, bool_e handle_foes){
		Uint8 neighbor;
		Uint16 test_cost = 0;

		for(neighbor=0; neighbor<NB_NODES; neighbor++){
			if(astar_nodes[neighbor].enable && ASTAR_IS_NODE_IN(neighbor, astar_nodes[current].neighbors)){

				// On vérifie que le noeud n'est pas dans la liste fermée
				if(!ASTAR_IS_NODE_IN(neighbor, closed_list)){

					if(!ASTAR_IS_NODE_IN(neighbor, opened_list)){
						// Si le noeud n'est pas dans la liste ouverte, on l'ajoute
						//debug_printf("Le node %d est ajouté à l'opened_list\n", neighbor);
						ASTAR_ADD_NODE_IN(neighbor, opened_list);
						astar_nodes[neighbor].parent = current;
						astar_nodes[neighbor].cost.step = ASTAR_pathfind_cost(current, neighbor, TRUE, handle_foes);
					}else{
						// Sinon si le cout est inférieur avec le noeud courant en tant que parent,
						// le noeud courant devient le parent du node en question
						test_cost = ASTAR_pathfind_cost(current, neighbor, TRUE, handle_foes);
						if(test_cost < astar_nodes[neighbor].cost.step){
							//debug_printf("Le node %d a maintenant pour parent %d\n", neighbor, current);
							astar_nodes[neighbor].parent = current;
							astar_nodes[neighbor].cost.step = test_cost;
						}
					}

				}

			}
		}
	}

	static error_e ASTAR_compute(displacement_curve_t *displacements, Uint8 *nb_displacements, GEOMETRY_point_t from, GEOMETRY_point_t dest, PROP_speed_e speed, bool_e handle_foes){
		Uint16 minimal_cost = MAX_COST;
		Uint16 current_node = NO_ID;
		Uint8 i;
		error_e result;

		// On réinitialise tous les noeuds
		ASTAR_init_nodes();

		// On génère le graphe
		ASTAR_generate_graph(from, dest);

		ASTAR_print_nodes(TRUE);

		// On réinitialise les listes avec des 0 sur 64 bits
		opened_list = 0ULL;
		closed_list = 0ULL;

		// On ajoute le noeud de départ (from) à l'opened_list
		ASTAR_ADD_NODE_IN(FROM_NODE, opened_list);
		astar_nodes[FROM_NODE].cost.step = 0;
		astar_nodes[FROM_NODE].cost.heuristic = ASTAR_pathfind_cost(FROM_NODE, DEST_NODE, FALSE, FALSE);
		astar_nodes[FROM_NODE].cost.total = astar_nodes[FROM_NODE].cost.heuristic; //Le cout total est égal à l'heuristique car le step est nul.

		// Tant que la liste ouverte n'est pas vide et que le noeud d'arrivée n'a pas été ajouté à la liste fermée
		while(opened_list != 0ULL && !ASTAR_IS_NODE_IN(DEST_NODE, closed_list)){

			ASTAR_print_opened_list();
			ASTAR_print_closed_list();

			//Recherche dans la liste ouverte du node avec le cout le plus faible. Ce node devient le node courant (current).
			minimal_cost = MAX_COST;
			current_node = NO_ID;
			for(i=1; i<NB_NODES; i++){
				if(ASTAR_IS_NODE_IN(i, opened_list) && astar_nodes[i].cost.total <= minimal_cost){
					minimal_cost = astar_nodes[i].cost.total;
					current_node = i;
				}
			}

			debug_printf("Current node selected is %d\n", current_node);
			//Ajout du noeud courant dans la closed_list et suppression de celui_ci dans l'opened_list
			ASTAR_ADD_NODE_IN(current_node, closed_list);
			ASTAR_CLR_NODE_IN(current_node, opened_list);

			// Si le node courant n'est pas le node de destination (ou d'arrivée)
			if(current_node != DEST_NODE){

				// On ajoute les noeuds voisins à la liste ouverte
				ASTAR_link_nodes_on_path(current_node, handle_foes);

				// Mise à jour des coûts des noeuds qui pour parent current_node
				ASTAR_update_cost(current_node);
			}

		}

		// On connait maintenant la trajectoire du robot, on peut en déduire les déplacements à effectuer
		result = ASTAR_make_the_path(displacements, nb_displacements, speed, current_node);

		return result;
	}



	static error_e ASTAR_make_the_path(displacement_curve_t *displacements, Uint8 *nb_displacements, PROP_speed_e speed, astar_node_id last_node){
		astar_node_id id = NO_ID;
		Uint8 nb_nodes = 0;
		Sint16 i;
		error_e result;

		id = last_node;
		// On recherche le nombre de noeuds coinstituant la trajectoire
		do{
			id = astar_nodes[id].parent;
			nb_nodes++;
		}while(id != FROM_NODE);
		*nb_displacements = nb_nodes;

		// On remplit le tableaux des déplacements
		debug_printf("\n PATH \n");
		id = last_node;
		for(i=nb_nodes-1; i>=0; i--){
			displacements[i].point = astar_nodes[id].pos;
			id = astar_nodes[id].parent;
			debug_printf("[%d] pos(%d;%d)\n", id, astar_nodes[id].pos.x, astar_nodes[id].pos.y);
		}

		if(last_node == DEST_NODE){
			// Le chemin est trouvé jusqu'au point d'arrivée
			result = END_OK;
		}else if(nb_nodes >=1){
			// Si un début de chemin est trouvé, alors surement qu'un adversaire nous bloque le chemin
			result = FOE_IN_PATH;
		}else{
			// Pas de chemin trouvé
			result = NOT_HANDLED;
		}
		return result;
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
		static error_e result = IN_PROGRESS;
		static displacement_curve_t displacements[NB_NODES];
		static Uint8 nb_displacements;
		static Uint8 nb_try;
		static bool_e success_possible;
		static GEOMETRY_point_t destination;
		static bool_e handles_foes;

		CREATE_MAE_WITH_VERBOSE(SM_ID_ASTAR_TRY_GOING,
								ASTAR_INIT,
								ASTAR_GO_DIRECTLY,
								ASTAR_COMPUTE,
								ASTAR_DISPLACEMENT,
								ASTAR_FAIL,
								ASTAR_SUCCESS
								);

		switch(state)
		{
			case ASTAR_INIT:{
				switch(avoidance)
				{
					case DODGE_AND_WAIT:
					case DODGE_AND_NO_WAIT:
						nb_try = NB_TRY_WITH_DODGE;
						break;
					default:
						nb_try = 1;
						break;
				}
				destination = (GEOMETRY_point_t){x, y};
				if(ASTAR_is_node_visible((GEOMETRY_point_t){global.pos.x, global.pos.y}, destination)){ // Si le point de destination est directement visible à partir du point de départ
					nb_try--; // On le compte comme une tentative
					state = ASTAR_GO_DIRECTLY;
				}else{
					state = ASTAR_COMPUTE;
				}
			}break;

			case ASTAR_GO_DIRECTLY:
				result = try_going(x, y,  IN_PROGRESS, END_OK, NOT_HANDLED, speed, way, avoidance, end_condition, NO_ZONE);

				switch(result)
				{
					case IN_PROGRESS:
						break;
					case END_OK:
						state = ASTAR_SUCCESS;
						break;
					case FOE_IN_PATH:
					case NOT_HANDLED:
					case END_WITH_TIMEOUT:
					default:
						debug_printf("result ASTAR_GO_DIRECTLY is %d\n", result);
						if(nb_try == 0)
							state = ASTAR_FAIL;
						else
							state = ASTAR_COMPUTE; // On tente en pathfind
				}
				break;

			case ASTAR_COMPUTE:
				result = ASTAR_compute(displacements, &nb_displacements, (GEOMETRY_point_t){global.pos.x, global.pos.y}, destination, speed,  FALSE);

				if(result == END_OK){
					success_possible = TRUE;
					state = ASTAR_DISPLACEMENT;
				}else if(result == FOE_IN_PATH){
					success_possible = FALSE;
					state = ASTAR_DISPLACEMENT;
					info_printf("ASTAR failure but will try to reach x=%d y=%d\n", displacements[nb_displacements-1].point.x, displacements[nb_displacements-1].point.y);
				}else{
					state = ASTAR_FAIL;  // Aucun chemin trouvé
				}
				nb_try--;
				break;

			case ASTAR_DISPLACEMENT:
				result = goto_pos_curve_with_avoidance(NULL, displacements, nb_displacements, way, avoidance, end_condition, PROP_NO_BORDER_MODE);

				switch(result)
				{
					case IN_PROGRESS:
						break;
					case END_OK:
						if(success_possible){
							state = ASTAR_SUCCESS;
						}else{
							if(nb_try == 0)
								state = ASTAR_FAIL;
							else
								state = ASTAR_COMPUTE; // On retente
						}
						break;
					case FOE_IN_PATH:
					case NOT_HANDLED:
					case END_WITH_TIMEOUT:
					default:
						if(nb_try == 0)
							state = ASTAR_FAIL;
						else
							state = ASTAR_COMPUTE; // On retente
				}
				break;
			case ASTAR_FAIL:
				debug_printf("Finish with failure\n");
				state = ASTAR_INIT;
				return fail_state;
				break;
			case ASTAR_SUCCESS:
				debug_printf("Finish with success\n");
				state = ASTAR_INIT;
				return success_state;
				break;
		}
		return in_progress;
	}



//--------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------- Fonctions annexes ---------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	static astar_node_id ASTAR_get_symetric_node(astar_node_id id){
		astar_node_id sym_id = 0;

		if(global.color == BOT_COLOR){
			sym_id = id;
		}else{
			if(id >=A1 && id <=A2)
				sym_id = I1 + id;
			else if(id >=B1 && id <=B5)
				sym_id = H1 - B1 + id;
			else if(id >=C1 && id <=C4)
				sym_id = G1 - C1 + id;
			else if(id >=D1 && id <=D6)
				sym_id = F1 - D1 + id;
			else if(id >=E1 && id <=E2)
				sym_id = id;
			else if(id >=F1 && id <=F6)
				sym_id = D1 - F1 + id ;
			else if(id >=G1 && id <=G4)
				sym_id = C1 - G1 + id;
			else if(id >=H1 && id <=H5)
				sym_id = B1 - H1 + id;
			else if(id >=I1 && id <=I2)
				sym_id = A1 - I1 + id;
			else
				debug_printf("ASTAR_get_symetric_node : symetric node of %d not found\n", id);
		}
		return sym_id;
	}


	/** @brief ASTAR_pathfind_cost
	 *		Calcul du cout entre deux points
	 * @param start_node : le node de départ
	 * @param end_node: le node d'arrivée
	 * @return le cout entre les deux nodes passés en paramètre
	 */
	static Uint16 ASTAR_pathfind_cost(astar_node_id start_node, astar_node_id end_node, bool_e with_angle, bool_e with_foes){
		assert(start_node < NB_NODES);
		assert(end_node < NB_NODES);


		Uint16 cost = 0;
		Uint16 dist = 0;
		Uint8 parent = 0;
		double new_angle, current_angle = 0;

		// Dans tous les cas, on prend en compte la distance
		dist = GEOMETRY_distance(astar_nodes[start_node].pos, astar_nodes[end_node].pos);
		if(with_foes && dist < MAX_COST_FOES){
			// Si on est très proche d'un adversaire, on alourdit le poids de façon inversement proportionnel
			// à la distance, ainsi le cout minimal se toruve aux alentours de MAX_COST_FOES en distance
			cost = MAX_COST_FOES - dist;
		}else{
			cost = dist;
			debug_printf("cost_dist(%d) = %d\n", end_node, dist);
		}


		if(with_angle){
			if(start_node == FROM_NODE){
				//new_angle = atan2(astar_nodes[end_node].pos.y - astar_nodes[start_node].pos.y, astar_nodes[end_node].pos.x - astar_nodes[start_node].pos.x);
				//new_angle = new_angle*PI4096/M_PI;  // transformation de la valeur en unité PI4096
				// Si c'est le noeud de départ, current_angle est égal à l'angle actuel du robot
				//cost += (MAX_COST_ANGLE * ASTAR_abs_angle((Sint16)((Sint16)new_angle - global.pos.angle)))/TWO_PI4096;
				//debug_printf("FROM cost_angle(%d) = %d   diff=%d\n", end_node, (MAX_COST_ANGLE * ASTAR_abs_angle((Sint16)((Sint16)new_angle - global.pos.angle)))/PI4096, (Sint16)(new_angle - global.pos.angle));

			}else{
				parent = astar_nodes[start_node].parent;
			//	debug_printf("Node %d a pour parent %d\n", start_node, parent);
				assert(parent != NO_ID);
				new_angle = atan2(astar_nodes[end_node].pos.y - astar_nodes[start_node].pos.y, astar_nodes[end_node].pos.x - astar_nodes[start_node].pos.x);
				new_angle = new_angle*PI4096/M_PI;  // transformation de la valeur en unité PI4096
				current_angle = atan2(astar_nodes[start_node].pos.y - astar_nodes[parent].pos.y, astar_nodes[start_node].pos.x - astar_nodes[parent].pos.x);
				current_angle = current_angle*PI4096/M_PI;  // transformation de la valeur en unité PI4096
				cost += (MAX_COST_ANGLE * ASTAR_abs_angle((Sint16)(new_angle - current_angle)))/TWO_PI4096;
			//	debug_printf("current_angle=%lf new_angle=%lf\n", current_angle, new_angle);
				debug_printf("cost_angle(%d) = %d   diff=%d\n", end_node, (MAX_COST_ANGLE * ASTAR_abs_angle((Sint16)(new_angle - current_angle))/PI4096), (Sint16)(new_angle - current_angle));
			}
		}

		return cost;
	}

	static void ASTAR_update_cost(astar_node_id current_node){
		Uint8 i;

		for(i=0; i<NB_NODES; i++){
			if(astar_nodes[i].parent == current_node){
				astar_nodes[i].cost.heuristic = ASTAR_pathfind_cost(i, DEST_NODE, FALSE, FALSE);
				// Le cout total est juste la somme su step et de l'heuristique.
				// On veut choisir le meilleur point pour aller à la destination sans s'occuper du chemin parcouru avant
				astar_nodes[i].cost.total = astar_nodes[i].cost.step + astar_nodes[i].cost.heuristic;
			}
		}
	}

	static Uint16 ASTAR_abs_angle(Sint16 angle){
		Uint16 result = 0;
		while(angle >= TWO_PI4096)
			angle -= TWO_PI4096;
		while(angle < 0)
			angle += TWO_PI4096;

		result = (Uint16)angle; // On le cast en unsigned

		return result;
	}

	/** @brief ASTAR_point_out_of_polygon
	 *		Fonction pour vérifier si un point est à l'extérieur d'un polygone
	 * @param polygon : le polygone pris en compte
	 * @param nodeTested : le node testé
	 * @return le booléen indiquant si le node est à l'extérieur (TRUE) ou à l'intérieur (FALSE) du polygone considéré
	 */
	static bool_e ASTAR_point_is_in_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested){
		Uint8 nbIntersections=0;
		Uint8 i;
		//segment de référence dont une extrémité (x=3000, y=4000) est à l'extérieur du polygone
		GEOMETRY_segment_t seg1 = {nodeTested, (GEOMETRY_point_t){3000, 4000}};

		//on compte le nombre d'intersection avec chaque coté du polygone
		for(i=0; i<polygon.nb_summits-1; i++){
			GEOMETRY_segment_t seg2 = {polygon.summits[i], polygon.summits[i+1]};
			nbIntersections += GEOMETRY_segments_intersects(seg1, seg2);
		}

		//Test de l'intersection avec le dernier segment
		GEOMETRY_segment_t seg2 = {polygon.summits[polygon.nb_summits-1], polygon.summits[0]};
		nbIntersections += GEOMETRY_segments_intersects(seg1, seg2);

		//Le point est à l'intérieur du polygone si le nombre d'intersections avec chacun des côté du polygone est un nombre impair.
		//Si le nombre d'intersection est un nombre pair, le node est donc à l'extérieur du polygone.
		return  (nbIntersections%2 == 1);
	}

	static bool_e ASTAR_is_link_cut_by_hardlines(GEOMETRY_point_t p1, GEOMETRY_point_t p2){
		Uint8 i = 0;
		GEOMETRY_segment_t seg_tested = {p1, p2};
		GEOMETRY_segment_t seg_hardline;
		bool_e result = FALSE;

		while(i<astar_nb_hardlines && !result){
			seg_hardline.a = astar_hardlines[i].ex1;
			seg_hardline.b = astar_hardlines[i].ex2;
			result = GEOMETRY_segments_intersects(seg_tested, seg_hardline);
			i++;
		}
		return result;
	}

	static bool_e ASTAR_is_node_visible(GEOMETRY_point_t p1, GEOMETRY_point_t p2){
		Uint8 i = 0, j = 0;
		bool_e result = FALSE;
		GEOMETRY_segment_t seg_tested = {p1, p2};
		GEOMETRY_segment_t seg_polygon;

		result = ASTAR_is_link_cut_by_hardlines(p1, p2);
		if(result)
			debug_printf("Segement is cut by hardline\n");

		if(!result){ //Si ce n'est pas coupé par une hardline, on continue les vérifications
			while(i < astar_nb_polygons && !result){
				j = 0;
				while(j < astar_polygons[i].nb_summits - 1 && !result){
					seg_polygon.a = astar_polygons[i].summits[j];
					seg_polygon.b = astar_polygons[i].summits[j+1];
					result = GEOMETRY_segments_intersects(seg_tested, seg_polygon);
					if(result)
						debug_printf("Segement is cut by polygon[%d] summit[%d] and [%d]\n", i, j, j+1);
					j++;
				}

				// Test du dernier segment
				if(!result){
					seg_polygon.a = astar_polygons[i].summits[0];
					seg_polygon.b = astar_polygons[i].summits[astar_polygons[i].nb_summits - 1];
					result = GEOMETRY_segments_intersects(seg_tested, seg_polygon);
					if(result)
						debug_printf("Segement is cut by polygon[%d] summit[%d] and [%d]\n", i, j, j+1);
				}
				i++;
			}
		}

		return !result;
	}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Fonctions d'affichage ------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	/** @brief ASTAR_print_list
	 *		Procédure affichant une liste de nodes
	 * @param list : la liste concernée
	 */
	static void ASTAR_print_list(astar_list_t list){
		Uint8 i;
		for(i=0; i<NB_NODES; i++){
			if(ASTAR_IS_NODE_IN(i, list)){
				debug_printf("%d) Node:  pos x=%d  y=%d    parent: id=%d\n", i, astar_nodes[i].pos.x, astar_nodes[i].pos.y, astar_nodes[i].parent);
				debug_printf("\t costHeuristic=%d  costStep=%d  costTotal=%d\n",astar_nodes[i].cost.heuristic, astar_nodes[i].cost.step, astar_nodes[i].cost.total);
			}
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


	void ASTAR_print_nodes(bool_e with_neighbors){
		Uint8 i, j;
		debug_printf("\nAFFICHAGE DES NODES:\n");
		for(i=0; i<NB_NODES; i++){
			if(!with_neighbors){
				debug_printf("%3d; %4d; %4d\n", astar_nodes[i].id, astar_nodes[i].pos.x,  astar_nodes[i].pos.y);
			}else{
				debug_printf("id = %3d; pos_x = %4d; pos_y = %4d;  enable = %d\n", astar_nodes[i].id, astar_nodes[i].pos.x,  astar_nodes[i].pos.y, astar_nodes[i].enable);
				for(j=0; j<NB_NODES; j++){
					if(astar_nodes[i].neighbors & (1ULL<<j)){
						debug_printf("neighbor : %d\n", j);
					}
				}
				debug_printf("\n");
			}
		}
	}

	void ASTAR_print_obstacles(){
		Uint8 i, j;
		debug_printf("\nAFFICHAGE DES OBSTACLES:\n");
		for(i=0; i<astar_nb_polygons; i++){
				debug_printf("%3d; %s\n", astar_polygons[i].id, astar_polygons[i].name);
				for(j=0; j<astar_polygons[i].nb_summits; j++){
					debug_printf("%2d; %4d; %4d\n", j, astar_polygons[i].summits[j].x, astar_polygons[i].summits[j].y);
				}
				debug_printf("NodesIO : ");
				for(j=0; j<astar_polygons[i].nb_nodesIO; j++){
					debug_printf("%d, ", astar_polygons[i].nodesIO[j]);
				}
				debug_printf("\n");
		}
		debug_printf("\nAFFICHAGE DES HARDLINES:\n");
		for(i=0; i<astar_nb_hardlines; i++){
			debug_printf("%2d  (%4d;%4d) -> (%4d;%4d)\n", i, astar_hardlines[i].ex1.x, astar_hardlines[i].ex1.y, astar_hardlines[i].ex2.x, astar_hardlines[i].ex2.y);
		}
	}


//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Accesseurs -----------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------


	/** @brief ASTAR_enable_polygon
	 *		Procédure permettant d'activer un polygone
	 *  @param polygon_number : le numéro du polygone concerné
	 */
	void ASTAR_enable_polygon(Uint8 polygon_number){
		astar_polygons[polygon_number].enable = TRUE;
	}



	/** @brief ASTAR_disnable_polygon
	 *		Procédure permettant de désactiver un polygone
	 *  @param polygon_number : le numéro du polygone concerné
	 */
	void ASTAR_disable_polygon(Uint8 polygon_number){
		astar_polygons[polygon_number].enable = FALSE;
	}


#endif //_ASTAR_H_




/*
	static void ASTAR_user_define_polygons(){
		////////////////////////////////////////// DEFINITION DES POLYGONES ///////////////////////////////////////////////////////
		// La zone de dépose
		GEOMETRY_point_t poly_depose_zone[9] = {(GEOMETRY_point_t){750, 900 - OBSTACLE_MARGIN},
												(GEOMETRY_point_t){750 + (600 + OBSTACLE_MARGIN)*cos(-2*M_PI/6.0), 1500 + (600 + OBSTACLE_MARGIN)*sin(-2*M_PI/6.0)},
												(GEOMETRY_point_t){750 + (600 + OBSTACLE_MARGIN)*cos(-M_PI/6.0), 1500 + (600 + OBSTACLE_MARGIN)*sin(-M_PI/6.0)},
												(GEOMETRY_point_t){1350 + OBSTACLE_MARGIN, 1500},
												(GEOMETRY_point_t){750 + (600 + OBSTACLE_MARGIN)*cos(M_PI/6.0), 1500 + (600 + OBSTACLE_MARGIN)*sin(M_PI/6.0)},
												(GEOMETRY_point_t){750 + (600 + OBSTACLE_MARGIN)*cos(2*M_PI/6.0), 1500 + (600 + OBSTACLE_MARGIN)*sin(2*M_PI/6.0)},
												(GEOMETRY_point_t){750, 2100 + OBSTACLE_MARGIN},
												(GEOMETRY_point_t){750 - OBSTACLE_MARGIN, 2100 + OBSTACLE_MARGIN},
												(GEOMETRY_point_t){750 - OBSTACLE_MARGIN, 900 - OBSTACLE_MARGIN}};
		astar_node_id nodesIO_depose_zone[4];
		ASTAR_define_polygon("depose_zone", poly_depose_zone, 9, TRUE, nodesIO_depose_zone, 0);

		// Notre zone de départ
		GEOMETRY_point_t poly_our_start_zone[4] = {(GEOMETRY_point_t){600 - OBSTACLE_MARGIN, COLOR_Y(0)},
												   (GEOMETRY_point_t){600 - OBSTACLE_MARGIN, COLOR_Y(300 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1100 + OBSTACLE_MARGIN, COLOR_Y(300 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1100 + OBSTACLE_MARGIN, COLOR_Y(0)}};
		char *nodesIO_our_start_zone[0];
		ASTAR_define_polygon("our_start_zone", poly_our_start_zone, 4, TRUE, nodesIO_our_start_zone, 0);

		// La zone de départ adverse
		GEOMETRY_point_t poly_adv_start_zone[4] = {(GEOMETRY_point_t){600 - OBSTACLE_MARGIN, COLOR_Y(3000)},
												  (GEOMETRY_point_t){600 - OBSTACLE_MARGIN, COLOR_Y(2700 - OBSTACLE_MARGIN)},
												  (GEOMETRY_point_t){1100 + OBSTACLE_MARGIN, COLOR_Y(2700 - OBSTACLE_MARGIN)},
												  (GEOMETRY_point_t){1100 + OBSTACLE_MARGIN, COLOR_Y(3000)}};
		char *nodesIO_adv_start_zone[0];
		ASTAR_define_polygon("adv_start_zone", poly_adv_start_zone, 4, TRUE, nodesIO_adv_start_zone, 0);

		// Le rocher de notre zone
		GEOMETRY_point_t poly_our_rocher_zone[4] = {(GEOMETRY_point_t){1750 - OBSTACLE_MARGIN, COLOR_Y(0)},
												  (GEOMETRY_point_t){2000, COLOR_Y(0)},
												  (GEOMETRY_point_t){2000, COLOR_Y(250 + OBSTACLE_MARGIN)},
												  (GEOMETRY_point_t){1800 - OBSTACLE_MARGIN, COLOR_Y(200 + OBSTACLE_MARGIN)}};
		char *nodesIO_our_rocher_zone[0];
		ASTAR_define_polygon("our_rocher_zone", poly_our_rocher_zone, 4, TRUE, nodesIO_our_rocher_zone, 0);

		// Le rocher de la zone adverse
		GEOMETRY_point_t poly_adv_rocher_zone[4] = {(GEOMETRY_point_t){1750 - OBSTACLE_MARGIN, COLOR_Y(3000)},
												  (GEOMETRY_point_t){2000, COLOR_Y(3000)},
												  (GEOMETRY_point_t){2000, COLOR_Y(2750 - OBSTACLE_MARGIN)},
												  (GEOMETRY_point_t){1800 - OBSTACLE_MARGIN, COLOR_Y(2800 - OBSTACLE_MARGIN)} };
		char *nodesIO_adv_rocher_zone[0];
		ASTAR_define_polygon("adv_rocher_zone", poly_adv_rocher_zone, 4, TRUE, nodesIO_adv_rocher_zone, 0);

		// Le cube de 8 de notre zone
		GEOMETRY_point_t poly_our_cube_8_zone[4] = {(GEOMETRY_point_t){0, COLOR_Y(800 - OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){200 + OBSTACLE_MARGIN, COLOR_Y(800 - OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){200 + OBSTACLE_MARGIN, COLOR_Y(960 + OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){0, COLOR_Y(960 + OBSTACLE_MARGIN)}};
		char *nodesIO_our_cube_8_zone[0];
		ASTAR_define_polygon("our_cube_8_zone", poly_our_cube_8_zone, 4, TRUE, nodesIO_our_cube_8_zone, 0);

		// Le cube de 8 de la zone adverse
		GEOMETRY_point_t poly_adv_cube_8_zone[4] = {(GEOMETRY_point_t){0, COLOR_Y(2200 + OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){200 + OBSTACLE_MARGIN, COLOR_Y(2200 + OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){200 + OBSTACLE_MARGIN, COLOR_Y(2040 - OBSTACLE_MARGIN)},
													(GEOMETRY_point_t){0, COLOR_Y(2040 - OBSTACLE_MARGIN)}};
		char *nodesIO_adv_cube_8_zone[0];
		ASTAR_define_polygon("adv_cube_8_zone", poly_adv_cube_8_zone, 4, TRUE, nodesIO_adv_cube_8_zone, 0);

		// La zone de la dune
		GEOMETRY_point_t poly_dune_zone[4] = {(GEOMETRY_point_t){0, 1200 - OBSTACLE_MARGIN},
											  (GEOMETRY_point_t){180 + OBSTACLE_MARGIN,  1200 - OBSTACLE_MARGIN},
											  (GEOMETRY_point_t){180 + OBSTACLE_MARGIN, 1800 + OBSTACLE_MARGIN},
											  (GEOMETRY_point_t){0, 1800 + OBSTACLE_MARGIN}};
		char *nodesIO_dune_zone[0];
		ASTAR_define_polygon("dune_zone", poly_dune_zone, 4, TRUE, nodesIO_dune_zone, 0);

		////////////////////////////////////////// DEFINITION DES HARDLINES ///////////////////////////////////////////////////////
		ASTAR_define_hardlines(4, (astar_hardline_t){(GEOMETRY_point_t){0, 800}, (GEOMETRY_point_t){200 + OBSTACLE_MARGIN, 800}},                       // tasseau cube de 8
								  (astar_hardline_t){(GEOMETRY_point_t){0, 2200}, (GEOMETRY_point_t){200 + OBSTACLE_MARGIN, 2200}},						// tasseau cube de 8
								  (astar_hardline_t){(GEOMETRY_point_t){750, 900 - OBSTACLE_MARGIN}, (GEOMETRY_point_t){750, 2100 + OBSTACLE_MARGIN}},	// tasseau zone de dépose
								  (astar_hardline_t){(GEOMETRY_point_t){750, 1500}, (GEOMETRY_point_t){1350 + OBSTACLE_MARGIN, 1500}});					// plexi zone de dépose
	}
*/



