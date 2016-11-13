/*
 *	Club Robot ESEO 2015 - 2017
 *
 *
 *	Fichier : astar.c
 *	Package : Stratégie
 */

#include "astar.h"
#include <stdlib.h>
#include <math.h>
#include "../QS/QS_all.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../utils/actionChecker.h"
#include "../Supervision/SD/SD.h"
#include "../utils/generic_functions.h"

#define LOG_PREFIX "astar: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ASTAR
#include "../QS/QS_outputlog.h"

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Macros ---------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	// Macros pour les coûts
	#define MAX_COST (65535)		//Coût global maximal
	#define MAX_COST_ANGLE (0) //(200)	//Coût maximal pouvant être généré par un écart d'angle (Je m'autorise à faire une distance de MAX_COST_ANGLE supplémentaire au maximum pour avoir un meilleur angle)
	#define MAX_COST_FOES (600)     //Côût supplémentaire ajouté si on est trop près d'un adversaire

	// Macros pour les polygones
	#define NB_MAX_POLYGONS (9 + MAX_NB_FOES)	// Nombre maximal de polygones
	#define NB_MAX_NODES_IO (10)				// Nombre maximal de noeuds d'entrée/sortie
	#define NB_MAX_SUMMITS (10)					// Nombre maximal de sommets par polygone
	#define OBSTACLE_MARGIN (200)				// Marge du centre du robot avec un obstacle

	// Macros pour les hardlines
	#define NB_MAX_HARDLINES (24)				// Nombre maximale de hardlines (attention si vous ajoutez une protection, ca fait une hardline en plus
	#define HARDLINE_PROTECTION_LENGTH (200)	// Longeur de la protection d'une hardline

	//Distance à laquelle on accepte des noeuds comme voisins
	#define DISTANCE_NEIGHBOURHOOD (500)

	// Macros pour le traitement des listes
	#define ASTAR_IS_NODE_IN(nodeId, nodeList)	((nodeList) & ((1ULL) << nodeId))
	#define ASTAR_ADD_NODE_IN(nodeId, nodeList)	((nodeList) |= ((1ULL) << (nodeId)))
	#define ASTAR_CLR_NODE_IN(nodeId, nodeList)	((nodeList) &= ~((1ULL) << (nodeId)))

	// Rayon du polygone défini pour les robots adverses
	#define FOE_RADIUS (250)

	// Macro renvoyant le noeud symétrique suivant la couleur (tout comme COLOR_Y le fait pour les coordonnées
	#define COLOR_NODE(id) ASTAR_get_symetric_node(id)

	// Nombre de tentatives du pathfind avec dodge en cas d'échec
	#define NB_TRY_WITH_DODGE (3)

	// Distance maximale à laquelle on s'autorise la supression du noeud de départ (non utilisé)
	#define DISTANCE_PROXIMITY_NODE (150)

	// Distance de laquelle on doit tenter de bouger si le pathfind a échouer. On doit bouger pour essayer de se débloquer.
	#define DISTANCE_TO_MOVE_IF_FAIL (600)

	// Activation de l'optimisation
	#define ASTAR_OPTIMISATION_2				// Activation de l'optimisation de trajectoires	(Optimisation par défaut)

	// ASTAR_OPTIMISATION a été testé mais les résultats de sont pas à la hauteur de nos attentes
	// Activer de préférence ASTAR_OPTIMISATION_2, l'optimisation par défaut
	//#define ASTAR_OPTIMISATION				// Activation de l'optimisation de trajectoires
	#define DISTANCE_OF_BRAKING (100)			// Distance de freinage (passage en SLOW)
	#define MAX_DISTANCE_IN_FAST_SPEED (500)	// A partir de cette distance, on redéfini un point pour passer en vitesse SLOW (protection)
	#define MAX_ANGLE_IN_FAST_SPEED (PI4096/6)	// Angle positif (si angle du robot supérieur, on freine)
	#define MAX_ANGLE_IN_SLOW_SPEED (PI4096/2)	// Angle positif (si angle du robot supérieur, on s'arrête sur le noeud)




//-------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- Définitions des types structrés ---------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

	//Type énuméré définissant les id des nodes
	typedef enum{
		A1 = 0,
		A2,
		A3,
		B1,
		B2,
		B3,
		C1,
		C2,
		D1,
		D2,
		D3,
		E1,
		E2,
		F1,
		F2,
		F3,
		G1,
		G2,
		H1,
		H2,
		H3,
		I1,
		I2,
		I3,
		FROM_NODE,
		DEST_NODE,
		NB_NODES,
		NO_ID = 255
	}astar_node_id;

	//Type structuré coût d'un noeud
	typedef struct{
		Uint16 total;		//Cout total = lengthPath + heuristic
		Uint16 lengthPath;  //Cout de déplacement du node de départ au node courant
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

	// Définition des coordonnées et des grandeurs des zones interdites modélisées par des polygones et des hardlines
	static void ASTAR_user_define_obstacles();

	// Fonction permettant de définir un polygone modélisant une zone interdite d'accès par le pathfind sauf si le point de départ et le point d'arrivée sont dedans
	static void ASTAR_define_polygon(char *name, GEOMETRY_point_t polygon[], Uint8 nb_summits, bool_e enable_polygon, astar_node_id nodesIO[], Uint8 nb_nodesIO);

	// Fonction permettant de définir une hardline, c'est à dire une ligne physiquement infranchissable par le robot
	static void ASTAR_define_hardline(bool_e protection_extremity_1, GEOMETRY_point_t extremity_1, GEOMETRY_point_t extremity_2, bool_e protection_extremity_2);

	// Fonction pour créer les polygones correspondant aux 2 robots adverses
	static void ASTAR_create_foe_polygon(Uint16 foeRadius);

	// Fonction permettant d'initialiser les propriétés principales des nodes
	static void ASTAR_init_nodes();

	// Fonction permettant de construire le graph de nodes à partir des polygones et des hardlines définies, du point de départ et du point d'arrivée
	static void ASTAR_generate_graph(GEOMETRY_point_t from, GEOMETRY_point_t dest);

	// Fonction permettant de chercher les voisins d'un node (ici utilisé pour le node de départ et le node d'arrivé)
	static void ASTAR_search_neighbors(astar_node_id nodeId, bool_e my_neighbors);

	//Fonction permettant de chercher les noeuds à ajouter à l'opened_list
	static void ASTAR_link_nodes_on_path(astar_node_id current, bool_e handle_foes);

	// Fonction permettant de chercher un chemin du point de départ au point d'arrivée
	static error_e ASTAR_compute(displacement_curve_t *displacements, Uint8 *nb_displacements, GEOMETRY_point_t from, GEOMETRY_point_t dest, PROP_speed_e speed, bool_e handle_foes);

	// Fonction permettant de convertir le chemin trouvé en déplacement réalisable par le robot (+ OPTIMISATION)
	static error_e ASTAR_make_the_path(displacement_curve_t *displacements, Uint8 *nb_displacements, PROP_speed_e speed, astar_node_id last_node);


//--------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------- Fonctions annexes (internes au programme) --------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	// Renvoie l'id du node symétrique au node demandé (fonctionnement analogue à COLOR_Y)
	static astar_node_id ASTAR_get_symetric_node(astar_node_id id);

	// Calcul du cout entre deux nodes
	static Uint16 ASTAR_pathfind_cost(astar_node_id start_node, astar_node_id end_node, bool_e with_angle, bool_e with_foes);

	// Calcul de la valeur absolue de l'angle passé en paramètre
	static Uint16 ASTAR_abs_angle(Sint16 angle);

	// Fonction pour vérifier si un point est à l'intérieur d'un polygone
	static bool_e ASTAR_point_is_in_polygon(astar_polygon_t polygon, GEOMETRY_point_t nodeTested);

	// Fonction pour vérifier si un segment [p1;p2] est coupé par une hardline
	static bool_e ASTAR_is_link_cut_by_hardlines(GEOMETRY_point_t p1, GEOMETRY_point_t p2);

	// Fonction pour vérifier si le point p2 est visible du point p1
	static bool_e ASTAR_is_node_visible(GEOMETRY_point_t p1, GEOMETRY_point_t p2);

	// Procédure affichant une liste de nodes
	static void ASTAR_print_list(astar_list_t list);

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Variables globales ---------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

	// Le graphe contenant tous les nodes
	static astar_node_t astar_nodes[NB_NODES] =
	{
		//(astar_node_t){ id,  pos, neighbors },


		//remove a3, i3 ,d3, f3 ,e2, hardline, (a faire cratère)


		//Rangée [A]
		(astar_node_t){ A1, {600,  300}, (1ULL<<A2)|(1ULL<<A3)|(1ULL<<B1)|(1ULL<<B2)},
		(astar_node_t){ A2, {1000, 400}, (1ULL<<A1)|(1ULL<<A3)|(1ULL<<B1)|(1ULL<<B2)|(1ULL<<B3)},
		(astar_node_t){ A3, {1350, 300}, (1ULL<<A1)|(1ULL<<A2)|(1ULL<<B1)|(1ULL<<B2)|(1ULL<<B3)},

		//Rangée [B]
		(astar_node_t){ B1, {900,  700}, (1ULL<<A1)|(1ULL<<A2)|(1ULL<<A3)|(1ULL<<B2)|(1ULL<<C1)|(1ULL<<C2)|(1ULL<<D2)},
		(astar_node_t){ B2, {1300, 700}, (1ULL<<A1)|(1ULL<<A2)|(1ULL<<A3)|(1ULL<<B1)|(1ULL<<B3)|(1ULL<<C2)|(1ULL<<D3)},
		(astar_node_t){ B3, {1700, 750}, (1ULL<<A2)|(1ULL<<A3)|(1ULL<<B2)},

		//Rangée [C]
		(astar_node_t){ C1, {700, 1000}, (1ULL<<B1)|(1ULL<<C2)|(1ULL<<D1)|(1ULL<<D2)|(1ULL<<D3)|(1ULL<<E1)},
		(astar_node_t){ C2, {1050, 950}, (1ULL<<B1)|(1ULL<<B2)|(1ULL<<C1)|(1ULL<<D2)|(1ULL<<D3)|(1ULL<<E2)},

		//Rangée [D]
		(astar_node_t){ D1, {300,  1300}, (1ULL<<C1)|(1ULL<<D2)|(1ULL<<E1)|(1ULL<<F1)},
		(astar_node_t){ D2, {800,  1300}, (1ULL<<B1)|(1ULL<<C1)|(1ULL<<C2)|(1ULL<<D1)|(1ULL<<D3)|(1ULL<<E1)|(1ULL<<E2)|(1ULL<<F2)},
		(astar_node_t){ D3, {1100, 1100}, (1ULL<<B2)|(1ULL<<C1)|(1ULL<<C2)|(1ULL<<D2)|(1ULL<<E2)},

		//Rangée [E]
		(astar_node_t){ E1, {500,  1500}, (1ULL<<C1)|(1ULL<<D1)|(1ULL<<D2)|(1ULL<<E2)|(1ULL<<F1)|(1ULL<<F2)|(1ULL<<G1)},
		(astar_node_t){ E2, {950, 1500}, (1ULL<<C2)|(1ULL<<D2)|(1ULL<<D3)|(1ULL<<E1)|(1ULL<<F2)|(1ULL<<F3)|(1ULL<<G2)},

		//Rangée [F]
		(astar_node_t){ F1, {300,  1700}, (1ULL<<D1)|(1ULL<<E1)|(1ULL<<F2)|(1ULL<<G1)},
		(astar_node_t){ F2, {800,  1700}, (1ULL<<D2)|(1ULL<<E1)|(1ULL<<E2)|(1ULL<<F1)|(1ULL<<F3)|(1ULL<<G1)|(1ULL<<G2)|(1ULL<<H1)},
		(astar_node_t){ F3, {1100, 1900}, (1ULL<<E2)|(1ULL<<F2)|(1ULL<<G1)|(1ULL<<G2)|(1ULL<<H2)},

		//Rangée [G]
		(astar_node_t){ G1, {700,  2000}, (1ULL<<E1)|(1ULL<<F1)|(1ULL<<F2)|(1ULL<<F3)|(1ULL<<G2)|(1ULL<<H1)},
		(astar_node_t){ G2, {1050, 2050}, (1ULL<<E2)|(1ULL<<F2)|(1ULL<<F3)|(1ULL<<G1)|(1ULL<<H1)|(1ULL<<H2)},

		//Rangée [H]
		(astar_node_t){ H1, {900,  2300}, (1ULL<<F2)|(1ULL<<G1)|(1ULL<<G2)|(1ULL<<H2)|(1ULL<<I1)|(1ULL<<I2)|(1ULL<<I3)},
		(astar_node_t){ H2, {1300, 2300}, (1ULL<<F3)|(1ULL<<G2)|(1ULL<<H1)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I2)|(1ULL<<I3)},
		(astar_node_t){ H3, {1700, 2250}, (1ULL<<H2)|(1ULL<<I2)|(1ULL<<I3)},

		//Rangée [I]
		(astar_node_t){ I1, {600,  2700}, (1ULL<<H1)|(1ULL<<H2)|(1ULL<<I2)|(1ULL<<I3)},
		(astar_node_t){ I2, {1000, 2600}, (1ULL<<H1)|(1ULL<<H2)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I3)},
		(astar_node_t){ I3, {1350, 2700}, (1ULL<<H1)|(1ULL<<H2)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I2)},

		//Node de départ
		(astar_node_t){ FROM_NODE, {0, 0},  0ULL},

		//Node de destination
		(astar_node_t){ DEST_NODE, {0, 0},  0ULL}
	};

	static astar_neighbor_t astar_curves[NB_NODES] =
	{

		//Rangée [A]
		/*A1*/ (0ULL<<A2)|(0ULL<<A3)|(0ULL<<B1)|(0ULL<<B2)|(1ULL<<FROM_NODE),//On est en A1, les courbes sont autorisées lorsque l'on vient de A2 ou de B1
		/*A2*/ (1ULL<<A1)|(1ULL<<A3)|(1ULL<<B1)|(1ULL<<B2)|(1ULL<<B3)|(1ULL<<FROM_NODE),
		/*A3*/ (1ULL<<A1)|(1ULL<<A2)|(1ULL<<B1)|(1ULL<<B2)|(1ULL<<B3)|(1ULL<<FROM_NODE),

		//Rangée [B]
		/*B1*/ (1ULL<<A1)|(1ULL<<A2)|(1ULL<<A3)|(1ULL<<B2)|(1ULL<<C1)|(1ULL<<C2)|(1ULL<<D2)|(1ULL<<FROM_NODE),
		/*B2*/ (1ULL<<A1)|(1ULL<<A2)|(0ULL<<A3)|(1ULL<<B1)|(1ULL<<B3)|(1ULL<<C2)|(0ULL<<D3)|(1ULL<<FROM_NODE),
		/*B3*/ (0ULL<<A2)|(0ULL<<A3)|(0ULL<<B2)|(1ULL<<FROM_NODE),

		//Rangée [C]
		/*C1*/ (1ULL<<B1)|(1ULL<<C2)|(1ULL<<D1)|(1ULL<<D2)|(1ULL<<D3)|(1ULL<<E1)|(1ULL<<FROM_NODE),
		/*C2*/ (1ULL<<B1)|(1ULL<<B2)|(1ULL<<C1)|(1ULL<<D2)|(1ULL<<D3)|(1ULL<<E2)|(1ULL<<FROM_NODE),

		//Rangée [D]
		/*D1*/ (1ULL<<C1)|(1ULL<<D2)|(1ULL<<E1)|(1ULL<<F1)|(1ULL<<FROM_NODE),
		/*D2*/ (1ULL<<B1)|(1ULL<<C1)|(1ULL<<C2)|(1ULL<<D1)|(1ULL<<D3)|(1ULL<<E1)|(1ULL<<E2)|(1ULL<<F2)|(1ULL<<FROM_NODE),
		/*D3*/ (0ULL<<B2)|(1ULL<<C1)|(1ULL<<C2)|(1ULL<<D2)|(0ULL<<E2)|(1ULL<<FROM_NODE),

		//Rangée [E]
		/*E1*/ (1ULL<<C1)|(1ULL<<D1)|(1ULL<<D2)|(1ULL<<E2)|(1ULL<<F1)|(1ULL<<F2)|(1ULL<<G1)|(1ULL<<FROM_NODE),
		/*E2*/ (1ULL<<C2)|(1ULL<<D2)|(0ULL<<D3)|(1ULL<<E1)|(1ULL<<F2)|(0ULL<<F3)|(1ULL<<G2)|(1ULL<<FROM_NODE),

		//Rangée [F]
		/*F1*/ (1ULL<<D1)|(1ULL<<E1)|(1ULL<<F2)|(1ULL<<G1)|(1ULL<<FROM_NODE),
		/*F2*/ (1ULL<<D2)|(1ULL<<E1)|(1ULL<<E2)|(1ULL<<F1)|(1ULL<<F3)|(1ULL<<G1)|(1ULL<<G2)|(1ULL<<H1)|(1ULL<<FROM_NODE),
		/*F3*/ (0ULL<<E2)|(1ULL<<F2)|(1ULL<<G1)|(1ULL<<G2)|(0ULL<<H2)|(1ULL<<FROM_NODE),

		//Rangée [G]
		/*G1*/ (1ULL<<E1)|(1ULL<<F1)|(1ULL<<F2)|(1ULL<<F3)|(1ULL<<G2)|(1ULL<<H1)|(1ULL<<FROM_NODE),
		/*G2*/ (1ULL<<E2)|(1ULL<<F2)|(1ULL<<F3)|(1ULL<<G1)|(1ULL<<H1)|(1ULL<<H2)|(1ULL<<FROM_NODE),

		//Rangée [H]
		/*H1*/ (1ULL<<G1)|(1ULL<<G2)|(1ULL<<H2)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I2)|(1ULL<<I3)|(1ULL<<FROM_NODE),
		/*H2*/ (1ULL<<F3)|(1ULL<<G2)|(1ULL<<H1)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I2)|(1ULL<<I3)|(1ULL<<FROM_NODE),
		/*H3*/ (0ULL<<H2)|(0ULL<<I2)|(0ULL<<I3)|(1ULL<<FROM_NODE),

		//Rangée [I]
		/*I1*/ (0ULL<<H1)|(0ULL<<H2)|(0ULL<<I2)|(0ULL<<I3)|(1ULL<<FROM_NODE),
		/*I2*/ (1ULL<<H1)|(1ULL<<H2)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I3)|(1ULL<<FROM_NODE),
		/*I3*/ (1ULL<<H1)|(1ULL<<H2)|(1ULL<<H3)|(1ULL<<I1)|(1ULL<<I2)|(1ULL<<FROM_NODE),

		//Node de départ (On commence toujours au point de départ donc pas d'importance ici. Par contre, il faut autoriser les courbes en n'importe quel point lorsqu'on vient du point de départ)
		0ULL,

		//Node de destination (c'est le end_condition qui est pris en compte)
		0ULL
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

	/** @brief ASTAR_init
	 *		Initialisation de l'astar : initialisation des obstacles
	 */
	void ASTAR_init(){

		ASTAR_user_define_obstacles();
		ASTAR_print_obstacles();

		/// Affichage COLOR_NODES
		//for(i=0; i<NB_NODES; i++){
		//	debug_printf("COLOR_NODES of %d is %d\n", i, COLOR_NODE(i));
		//}
	}


	/** @brief ASTAR_user_define_obstacles
	 *		Définition des coordonnées et des grandeurs des zones interdites modélisées par des polygones et des hardlines
	 */
	#define POLYGON_OUR_START_ZONE         (0)
	#define POLYGON_DEPOSE_CENTRE_ZONE 	   (1)
	#define POLYGON_OUR_CRATERE_ZONE	   (2)
	#define POLYGON_ADV_CRATERE_ZONE	   (3)
	#define POLYGON_OUR_PETIT1_CRATERE_ZONE (4)
	#define POLYGON_OUR_PETIT2_CRATERE_ZONE (5)
	#define POLYGON_ADV_PETIT1_CRATERE_ZONE (6)
	#define POLYGON_ADV_PETIT2_CRATERE_ZONE (7)
	static void ASTAR_user_define_obstacles(){
		////////////////////////////////////////// DEFINITION DES POLYGONES ///////////////////////////////////////////////////////
		// Notre zone de départ  ?sortie c1?
		GEOMETRY_point_t poly_our_start_zone[4] = {(GEOMETRY_point_t){0, COLOR_Y(0)},
												   (GEOMETRY_point_t){350 + OBSTACLE_MARGIN, COLOR_Y(0)},
												   (GEOMETRY_point_t){350 + OBSTACLE_MARGIN, COLOR_Y(750 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){0, COLOR_Y(750 + OBSTACLE_MARGIN)}};
		astar_node_id nodesIO_our_start_zone[3] = {COLOR_NODE(C1), COLOR_NODE(D1), COLOR_NODE(E1)};
		ASTAR_define_polygon("our_start_zone", poly_our_start_zone, 4, TRUE, nodesIO_our_start_zone, 3);

		// La zone central de depose module
		GEOMETRY_point_t poly_depose_centre_zone[8] = {(GEOMETRY_point_t){1485 - (cos(PI4096/4)*OBSTACLE_MARGIN), COLOR_Y(885 - (cos(PI4096/4)*OBSTACLE_MARGIN))},
												   (GEOMETRY_point_t){1385 - (cos(PI4096/4)*OBSTACLE_MARGIN), COLOR_Y(985 - (cos(PI4096/4)*OBSTACLE_MARGIN))},
												   (GEOMETRY_point_t){1200 + OBSTACLE_MARGIN, COLOR_Y(1430)},
												   (GEOMETRY_point_t){1200 + OBSTACLE_MARGIN, COLOR_Y(1575)},
												   (GEOMETRY_point_t){1385 - cos(PI4096/4)*OBSTACLE_MARGIN, COLOR_Y(2015 + cos(PI4096/4)*OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1485 - cos(PI4096/4)*OBSTACLE_MARGIN, COLOR_Y(2115 + cos(PI4096/4)*OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){2000, COLOR_Y(1600 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){2000, COLOR_Y(1400 - OBSTACLE_MARGIN)}};
		astar_node_id nodesIO_depose_centre_zone[4] = {COLOR_NODE(C2), COLOR_NODE(D3), COLOR_NODE(F3), COLOR_NODE(G2)};
		ASTAR_define_polygon("adv_depose_centre_zone", poly_depose_centre_zone, 8, TRUE, nodesIO_depose_centre_zone, 4);

		// notre grand cratère de balle
		GEOMETRY_point_t poly_our_cratere_zone[6] = {(GEOMETRY_point_t){1490 - OBSTACLE_MARGIN, COLOR_Y(0)},
												   (GEOMETRY_point_t){2000, COLOR_Y(0)},
												   (GEOMETRY_point_t){2000, COLOR_Y(510 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1800 - OBSTACLE_MARGIN, COLOR_Y(470 + OBSTACLE_MARGIN)},
		 	 	 	 	 	 	 	 	 	 	   (GEOMETRY_point_t){1635 - OBSTACLE_MARGIN, COLOR_Y(365 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1530 - OBSTACLE_MARGIN, COLOR_Y(200 + OBSTACLE_MARGIN)}};

		astar_node_id nodesIO_our_cratere_zone[3] = {COLOR_NODE(A3), COLOR_NODE(B2), COLOR_NODE(B3)};
		ASTAR_define_polygon("our_cratère", poly_our_cratere_zone, 6, TRUE, nodesIO_our_cratere_zone, 3);

		// La grand cratère de balle adverse
		GEOMETRY_point_t poly_adv_cratere_zone[6] = {(GEOMETRY_point_t){1490 - OBSTACLE_MARGIN, COLOR_Y(3000)},
												   (GEOMETRY_point_t){2000, COLOR_Y(3000)},
												   (GEOMETRY_point_t){2000, COLOR_Y(2490 - OBSTACLE_MARGIN)},
											       (GEOMETRY_point_t){1800 - OBSTACLE_MARGIN, COLOR_Y(2530 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1635 - OBSTACLE_MARGIN, COLOR_Y(2635 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1530 - OBSTACLE_MARGIN, COLOR_Y(2800 - OBSTACLE_MARGIN)}};

		astar_node_id nodesIO_adv_cratere_zone[3] = {COLOR_NODE(H2), COLOR_NODE(H3), COLOR_NODE(I3)};
		ASTAR_define_polygon("adv_cartère", poly_adv_cratere_zone, 6, TRUE, nodesIO_adv_cratere_zone, 3);

		// notre petit cratere proche
		GEOMETRY_point_t poly_our_petit1_cratere_zone[6] = {(GEOMETRY_point_t){580 + OBSTACLE_MARGIN, COLOR_Y(560 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){480 - OBSTACLE_MARGIN, COLOR_Y(560 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){436 - OBSTACLE_MARGIN, COLOR_Y(650)},
												   (GEOMETRY_point_t){480 - OBSTACLE_MARGIN, COLOR_Y(740 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){580 + OBSTACLE_MARGIN, COLOR_Y(740 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){624 + OBSTACLE_MARGIN, COLOR_Y(650)}};
		astar_node_id nodesIO_our_petit1_cratere_zone[4] = {COLOR_NODE(A1), COLOR_NODE(A2), COLOR_NODE(B1), COLOR_NODE(C1)};
		ASTAR_define_polygon("our_petit1_cratere_zone", poly_our_petit1_cratere_zone, 6, TRUE, nodesIO_our_petit1_cratere_zone, 4);

		// notre petit cratere loin
		GEOMETRY_point_t poly_our_petit2_cratere_zone[6] = {(GEOMETRY_point_t){1820 - OBSTACLE_MARGIN, COLOR_Y(980 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1776 - OBSTACLE_MARGIN, COLOR_Y(1070)},
												   (GEOMETRY_point_t){1820 - OBSTACLE_MARGIN, COLOR_Y(1160 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1920 + OBSTACLE_MARGIN, COLOR_Y(1160 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1964 + OBSTACLE_MARGIN, COLOR_Y(1070)},
												   (GEOMETRY_point_t){1920 + OBSTACLE_MARGIN, COLOR_Y(980 - OBSTACLE_MARGIN)}};
		astar_node_id nodesIO_our_petit2_cratere_zone[1] = {COLOR_NODE(B3)};
		ASTAR_define_polygon("our_petit2_cratere_zone", poly_our_petit2_cratere_zone, 6, TRUE, nodesIO_our_petit2_cratere_zone, 1);

		// petit cratere proche adverse
		GEOMETRY_point_t poly_adv_petit1_cratere_zone[6] = {(GEOMETRY_point_t){580 + OBSTACLE_MARGIN, COLOR_Y(2440 + OBSTACLE_MARGIN)},
													   (GEOMETRY_point_t){480 - OBSTACLE_MARGIN, COLOR_Y(2440 + OBSTACLE_MARGIN)},
													   (GEOMETRY_point_t){436 - OBSTACLE_MARGIN, COLOR_Y(2350)},
													   (GEOMETRY_point_t){480 - OBSTACLE_MARGIN, COLOR_Y(2260 - OBSTACLE_MARGIN)},
													   (GEOMETRY_point_t){580 + OBSTACLE_MARGIN, COLOR_Y(2260 - OBSTACLE_MARGIN)},
													   (GEOMETRY_point_t){624 + OBSTACLE_MARGIN, COLOR_Y(2350)}};
		astar_node_id nodesIO_adv_petit1_cratere_zone[4] = {COLOR_NODE(G1), COLOR_NODE(H1), COLOR_NODE(I1), COLOR_NODE(I2)};
		ASTAR_define_polygon("adv_petit1_cratere_zone", poly_adv_petit1_cratere_zone, 6, TRUE, nodesIO_adv_petit1_cratere_zone, 4);

		// petit cratere loin advserse
		GEOMETRY_point_t poly_adv_petit2_cratere_zone[6] = {(GEOMETRY_point_t){1820 - OBSTACLE_MARGIN, COLOR_Y(2020 + OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1776 - OBSTACLE_MARGIN, COLOR_Y(1930)},
												   (GEOMETRY_point_t){1820 - OBSTACLE_MARGIN, COLOR_Y(1840 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1920 + OBSTACLE_MARGIN, COLOR_Y(1840 - OBSTACLE_MARGIN)},
												   (GEOMETRY_point_t){1964 + OBSTACLE_MARGIN, COLOR_Y(1930)},
												   (GEOMETRY_point_t){1920 + OBSTACLE_MARGIN, COLOR_Y(2020 + OBSTACLE_MARGIN)}};
		astar_node_id nodesIO_adv_petit2_cratere_zone[1] = {COLOR_NODE(H3)};
		ASTAR_define_polygon("adv_petit2_cratere_zone", poly_adv_petit2_cratere_zone, 6, TRUE, nodesIO_adv_petit2_cratere_zone, 1);



		////////////////////////////////////////// DEFINITION DES HARDLINES ///////////////////////////////////////////////////////
		// zone de depose module centre 6cm
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){2000, 1500}, (GEOMETRY_point_t){1200 - HARDLINE_PROTECTION_LENGTH, 1500}, TRUE);
		// zone de depose module cote bleu 45°
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){2000, 1500}, (GEOMETRY_point_t){1430 - HARDLINE_PROTECTION_LENGTH, 2065 + HARDLINE_PROTECTION_LENGTH}, TRUE);
		// zone de depose module cote jaune 45°
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){2000, 1500}, (GEOMETRY_point_t){1430 - HARDLINE_PROTECTION_LENGTH, 935 - HARDLINE_PROTECTION_LENGTH}, TRUE);
		// zone depose module cote bleu
		ASTAR_define_hardline(TRUE,(GEOMETRY_point_t){700, 80}, (GEOMETRY_point_t){1150, 80}, TRUE);
		// zone depose module cote jaune
		ASTAR_define_hardline(TRUE,(GEOMETRY_point_t){700, 2920}, (GEOMETRY_point_t){1150, 2920}, TRUE);
		// la zone de départ adverse
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){360, COLOR_Y(3000)}, (GEOMETRY_point_t){360, COLOR_Y(1930)}, TRUE);
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){0, COLOR_Y(1930)}, (GEOMETRY_point_t){360, COLOR_Y(1930)}, TRUE);
		// tassau notre zone de départ
		ASTAR_define_hardline(FALSE,(GEOMETRY_point_t){360, COLOR_Y(0)}, (GEOMETRY_point_t){360, COLOR_Y(710 + HARDLINE_PROTECTION_LENGTH)}, TRUE);
	}

	/** @brief ASTAR_define_polygon
	 *		Fonction permettant de définir un polygone modélisant une zone interdite d'accès par le pathfind
	 *		sauf si le point de départ et le point d'arrivée sont dedans
	 *	@param name : le nom de la zone interdite
	 *  @param polygon : les coordonnees des sommets du polygone
	 *  @param nb_summits : le nombre de sommets du polygone
	 *  @param enable_polygon : le polygon doit t-il être actif au début du match (TRUE) ou sera-til activé pendant le match (FALSE)
	 *	@param nodesIO : les nodes d'entrées/sorties du polygone
	 *  @param nb_nodesIO : Le nombre de nodes d'entrées/sorties du polygone
	 */
	static void ASTAR_define_polygon(char *name, GEOMETRY_point_t polygon[], Uint8 nb_summits, bool_e enable_polygon, astar_node_id nodesIO[], Uint8 nb_nodesIO){
		int i;

		astar_polygons[astar_nb_polygons].id = astar_nb_polygons;
		astar_polygons[astar_nb_polygons].name = name;
		astar_polygons[astar_nb_polygons].enable = enable_polygon;

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


	/** @brief ASTAR_define_hardline
	 *		Fonction permettant de définir une hardline, c'est à dire une ligne physiquement infranchissable par le robot
	 *	@param protection_extremity_1 : est-il nécéssaire de mettre une protection sur l'extrémité 1 de la hardline définie (par défaut TRUE)
	 *  @param extremity_1 : les coordonnees de l'extrémité 1 du segment modélisant la hardline
	 *  @param extremity_2 : les coordonnees de l'extrémité 2 du segment modélisant la hardline
	 *  @param protection_extremity_2 : lest-il nécéssaire de mettre une protection sur l'extrémité 2 de la hardline définie (par défaut TRUE)
	 */
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
	 *		Fonction permettant d'initialiser les propriétés principales des nodes (couts, voisins, parent etc...)
	 */
	static void ASTAR_init_nodes(){
		Uint8 i;
		Uint16 id = 0;

		for(i=0; i<NB_NODES; i++){
			astar_nodes[i].id = id;
			id++;
			astar_nodes[i].enable = TRUE;
			astar_nodes[i].parent = NO_ID;
			astar_nodes[i].cost.lengthPath = MAX_COST;
			astar_nodes[i].cost.heuristic = MAX_COST;
			astar_nodes[i].cost.total = MAX_COST;

			// Il faut aussi penser à nettoyer le voisin DEST_NODE de tous les noeuds, la position finale demandée change à chaque fois
			ASTAR_CLR_NODE_IN(DEST_NODE, astar_nodes[i].neighbors);
		}

		// On nettoie aussi tous les voisins de FROM_NODE et de DEST_FROM
		astar_nodes[FROM_NODE].neighbors = 0ULL;
		astar_nodes[DEST_NODE].neighbors = 0ULL;
	}


	/** @brief ASTAR_generate_graph
	 *		Fonction permettant de construire le graph de nodes à partir des polygones et des hardlines définies, du point de départ et du point d'arrivée
	 *  @param from : les coordonnées du point de départ
	 *  @param dest : les coordonnées du point d'arrivée ou de destination
	 */
	static void ASTAR_generate_graph(GEOMETRY_point_t from, GEOMETRY_point_t dest){
		Uint8 i, j;

		// On réinitialise tous les noeuds
		ASTAR_init_nodes();

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


	/** @brief ASTAR_search_neighbors
	 *		Fonction permettant de chercher les voisins d'un node (ici utilisé pour le node de départ et le node d'arrivé)
	 *  @param nodeId : l'id du node pour lequel on doit chercher des voisins
	 *  @param my_neighbors : est ce que je cherche mes voisins (TRUE) ou est ce que je cherche des nodes qui doivent m'ajouter comme voisin (FALSE)
	 */
	static void ASTAR_search_neighbors(astar_node_id nodeId, bool_e my_neighbors){
		Uint8 i, j;
		bool_e in_polygon = FALSE;
		Uint32 dist = 0;
		Uint32 dist_ref = 0;
		Uint8 nb_search = 0;
		bool_e finish_search = FALSE;
		Uint8 tmpNodeId = 0;

		debug_printf("\nGENERATE GRAPH\n");
		for(i=0; i<astar_nb_polygons; i++){
			if(astar_polygons[i].enable){
				// Si le node "nodeId" est dans un polygone
				if(ASTAR_point_is_in_polygon(astar_polygons[i], astar_nodes[nodeId].pos)){
					in_polygon = TRUE;
					debug_printf("in_polygon TRUE(%s)\n", astar_polygons[i].name);
					for(j=0; j<astar_polygons[i].nb_nodesIO; j++){
						// Le nodeIO est bien considéré comme voisin si il n'est pas coupé par une hardline
						tmpNodeId = astar_polygons[i].nodesIO[j];
						if(!ASTAR_is_link_cut_by_hardlines(astar_nodes[nodeId].pos, astar_nodes[tmpNodeId].pos)){
							if(my_neighbors){
								// Je cherche mes voisins (voisins de nodeId), donc on ajoute les nodesIO du polygones comme mes voisins
								debug_printf("Node=%d ajoute comme voisin par FROM_NODE\n", astar_polygons[i].nodesIO[j]);
								ASTAR_ADD_NODE_IN(astar_polygons[i].nodesIO[j], astar_nodes[nodeId].neighbors);
							}else{
								// Je cherche de qui je peut être voisin, les nodesIO du polygone m'ajoute comme voisin
								debug_printf("Node=%d ajoute comme voisin à DEST_NODE\n", astar_polygons[i].nodesIO[j]);
								ASTAR_ADD_NODE_IN(nodeId, astar_nodes[astar_polygons[i].nodesIO[j]].neighbors);
							}
						}else{
							debug_printf("NodeIO %d (%d;%d) of polygon %d NOT neighbor (cut by hardline)\n", tmpNodeId, astar_nodes[tmpNodeId].pos.x, astar_nodes[tmpNodeId].pos.y , i);
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
					dist_ref = DISTANCE_NEIGHBOURHOOD*DISTANCE_NEIGHBOURHOOD; // On prend le carré pour éviter de calculer la racine carrée couteuse en temps CPU
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


	/** @brief ASTAR_link_nodes_on_path
	 *		Fonction permettant de chercher les noeuds à ajouter à l'opened_list
	 *  @param current : l'id du node courant
	 *  @param handle_foes : doit-on gérer les adversaires ?
	 */
	static void ASTAR_link_nodes_on_path(astar_node_id current, bool_e handle_foes){
		Uint8 neighbor;
		Uint16 tmp_lengthPath = 0;
		Uint16 tmp_heuristic = 0;
		Uint16 tmp_total = 0;

		for(neighbor=0; neighbor<NB_NODES; neighbor++){
			if(astar_nodes[neighbor].enable && ASTAR_IS_NODE_IN(neighbor, astar_nodes[current].neighbors)){

				// On vérifie que le noeud n'est pas dans la liste fermée
				if(!ASTAR_IS_NODE_IN(neighbor, closed_list)){

					if(!ASTAR_IS_NODE_IN(neighbor, opened_list)){
						// Si le noeud n'est pas dans la liste ouverte, on l'ajoute
						//debug_printf("Le node %d est ajouté à l'opened_list\n", neighbor);
						ASTAR_ADD_NODE_IN(neighbor, opened_list);
						astar_nodes[neighbor].parent = current;
						// Calcul du cout lengthPath = cout du node de départ au node parent + cout du node parent au node final
						astar_nodes[neighbor].cost.lengthPath =  astar_nodes[current].cost.lengthPath + ASTAR_pathfind_cost(current, neighbor, FALSE, handle_foes);
						astar_nodes[neighbor].cost.heuristic =   ASTAR_pathfind_cost(neighbor, DEST_NODE, TRUE, handle_foes);
						astar_nodes[neighbor].cost.total = astar_nodes[neighbor].cost.lengthPath + astar_nodes[neighbor].cost.heuristic;
					}else{
						// Sinon si le cout est inférieur avec le noeud courant en tant que parent,
						// le noeud courant devient le parent du node en question
						tmp_lengthPath = astar_nodes[current].cost.lengthPath + ASTAR_pathfind_cost(current, neighbor, FALSE, handle_foes);
						tmp_heuristic = ASTAR_pathfind_cost(neighbor, DEST_NODE, FALSE, handle_foes);
						tmp_total = tmp_lengthPath + tmp_heuristic;
						if(tmp_total < astar_nodes[neighbor].cost.total){
							//debug_printf("Le node %d a maintenant pour parent %d\n", neighbor, current);
							astar_nodes[neighbor].parent = current;
							astar_nodes[neighbor].cost.lengthPath = tmp_lengthPath;
							astar_nodes[neighbor].cost.heuristic = tmp_heuristic;
							astar_nodes[neighbor].cost.total = tmp_total;
						}
					}

				}

			}
		}
	}

	/** @brief ASTAR_compute
	 *		Fonction permettant de chercher un chemin du point de départ au point d'arrivée
	 *  @param displacements : les déplacements du robot
	 *  @param nb_displacements : le nombre de déplacements du robot
	 *  @param from : les coordonnées du point de départ
	 *  @param dest : les coordonnées du point d'arrivée
	 *  @param speed : la vitesse demandée par l'utilisateur
	 *  @param handle_foes : doit-on gérer les adversaires ?
	 */
	static error_e ASTAR_compute(displacement_curve_t *displacements, Uint8 *nb_displacements, GEOMETRY_point_t from, GEOMETRY_point_t dest, PROP_speed_e speed, bool_e handle_foes){
		Uint16 minimal_cost = MAX_COST;
		Uint16 current_node = NO_ID;
		Uint8 i;
		error_e result;

		debug_printf("global.color = %d\n", global.color);

		//ASTAR_print_nodes(TRUE);

		// On réinitialise les listes avec des 0 sur 64 bits
		opened_list = 0ULL;
		closed_list = 0ULL;

		// On ajoute le noeud de départ (from) à l'opened_list
		ASTAR_ADD_NODE_IN(FROM_NODE, opened_list);
		astar_nodes[FROM_NODE].cost.lengthPath = 0;
		astar_nodes[FROM_NODE].cost.heuristic = ASTAR_pathfind_cost(FROM_NODE, DEST_NODE, FALSE, FALSE);
		astar_nodes[FROM_NODE].cost.total = astar_nodes[FROM_NODE].cost.heuristic; //Le cout total est égal à l'heuristique car le step est nul.

		// Tant que la liste ouverte n'est pas vide et que le noeud d'arrivée n'a pas été ajouté à la liste fermée
		while(opened_list != 0ULL && !ASTAR_IS_NODE_IN(DEST_NODE, closed_list)){

			ASTAR_print_opened_list();
			ASTAR_print_closed_list();

			//Recherche dans la liste ouverte du node avec le cout le plus faible. Ce node devient le node courant (current).
			minimal_cost = MAX_COST;
			current_node = NO_ID;
			for(i=0; i<NB_NODES; i++){
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

				// Mise à jour des coûts des noeuds qui ont pour parent current_node
				//ASTAR_update_cost(current_node);
			}

		}

		// On connait maintenant la trajectoire du robot, on peut en déduire les déplacements à effectuer
		result = ASTAR_make_the_path(displacements, nb_displacements, speed, current_node);

		return result;
	}


	/** @brief ASTAR_make_the_path
	 *		Fonction permettant de convertir le chemin trouvé en déplacement réalisable par le robot
	 *      Si la macro ASTAR_OPTIMISATION est définie, alors une optimisation du chemin est calculée
	 *  @param displacements : les déplacements du robot
	 *  @param nb_displacements : le nombre de déplacements du robot
	 *  @param speed : la vitesse demandée par l'utilisateur
	 *  @param last_node : le dernier id du node courant renvoyé par la boucle de l'astar
	 *  @return l'état de succès ou d'échec (A t-on trouvé un chemin ou pas ?)
	 */
	static error_e ASTAR_make_the_path(displacement_curve_t *displacements, Uint8 *nb_displacements, PROP_speed_e speed, astar_node_id last_node){
		astar_node_id node = NO_ID;
		Uint8 nb_nodes = 0; // Nombre de nodes de la trajectoire (départ et arrivée inclus)
		Sint16 i;
		Uint8 j;
		error_e result;
		Uint8 last_index = 0;
		Uint16 distToStartNode = 0;
		Uint16 wantedDistToStartNode = 0;
		UNUSED_VAR(last_index);

		// On recherche le nombre de noeuds constituant la trajectoire
		node = last_node;
		nb_nodes++;
		do{
			node = astar_nodes[node].parent;
			nb_nodes++;
			if(node == NO_ID)
				debug_printf("Problème un des nodes de la liste de la trajectoire a pour id = NO_ID");
		}while(node != FROM_NODE && node != NO_ID);

		//On regarde si on a un chemin

		if(last_node == DEST_NODE){
			// Le chemin est trouvé jusqu'au point d'arrivée
			result = END_OK;
		}else if(nb_nodes >= 2){ // Noeud de départ + au moins un autre
			// Si un début de chemin est trouvé, alors surement qu'un adversaire nous bloque le chemin
			result = FOE_IN_PATH;
		}else{
			// Pas de chemin trouvé
			*nb_displacements = 0;
			return NOT_HANDLED; // On quitte tout de suite la fonction, il n'y a rien à faire
		}

		astar_node_id path_id[nb_nodes];
		bool_e path_enable[nb_nodes];
		bool_e path_curve[nb_nodes];
		UNUSED_VAR(path_enable);
		UNUSED_VAR(path_curve);

		// On remplit le tableaux du path
		node = last_node;
		for(i=nb_nodes-1; i>=0; i--){
			path_id[i] = astar_nodes[node].id;
			path_enable[i] = TRUE;
			path_curve[i] = TRUE;
			node = astar_nodes[node].parent;
		}
		path_enable[0] = FALSE; //Le point de départ n'est pas dans la trajectoire


		// Affichage de la trajectoire non optimisée(index 0:point de départ, dernier point: point d'arrivé)
		debug_printf("\n PATH \n");
		for(i=0; i<nb_nodes; i++){
			debug_printf("[%d] pos(%d;%d)\n", path_id[i], astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y);
		}
		debug_printf("\n");
		debug_printf("nb_nodes=%d\n", nb_nodes);

		// Si on sait déjà qu'on n'arrivera pas à la position d'arrivée souhaité, on cherche juste à se débloquer
		// On essaie alors de se déplacer un peu pour se débloquer mais ca ne sert à rien d'effectuer tout le début de chemin trouvé pr le pathfind
		if(result == FOE_IN_PATH){
			i=1;
			wantedDistToStartNode = DISTANCE_TO_MOVE_IF_FAIL*DISTANCE_TO_MOVE_IF_FAIL;
			do{
				distToStartNode = GEOMETRY_distance_square(astar_nodes[FROM_NODE].pos, astar_nodes[path_id[i]].pos);
				i++;
			}while(i < nb_nodes && distToStartNode < wantedDistToStartNode);

			nb_nodes = i; // On supprime les derniers points
		}

#ifdef	ASTAR_OPTIMISATION
		double last_angle, angle;
		Sint16 diff_angle;
		Uint16 sum_dist, dist;

		// Elimination des poinst inutiles
		// Lorsqu'un point est visible d'un autre point alors tous les points entre les deux peuvent être supprimés
		last_index = 0;
		j = 1;
		for(i=1; i<nb_nodes-1; i++){
			if(ASTAR_is_node_visible(astar_nodes[path_id[last_index]].pos, astar_nodes[path_id[i+1]].pos)){
				path_enable[i] = FALSE;
				debug_printf("Node %d deleted of path (%d;%d)\n", path_id[i], astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y);
			}else{
				last_index = i;
				path_id[j] = path_id[i];
				j++;
			}
		}
		//On pense à stocker le dernier node
		path_id[j] = path_id[nb_nodes - 1];
		nb_nodes = j + 1;


		// Affichage de la trajectoire optimisée
		debug_printf("\n PATH OPTIMISE\n");
		for(i=0; i<nb_nodes; i++){
			debug_printf("[%d] pos(%d;%d)\n", path_id[i], astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y);
		}
		debug_printf("\n");
		debug_printf("nb_nodes=%d\n", nb_nodes);


		sum_dist = 0;
		j = 0;

		for(i=1; i<nb_nodes-1; i++){

			// On calcule les angles
			last_angle = atan2((double)(astar_nodes[path_id[i]].pos.y - astar_nodes[path_id[i-1]].pos.y),
					(double)(astar_nodes[path_id[i]].pos.x - astar_nodes[path_id[i-1]].pos.x));
			angle = atan2((double)(astar_nodes[path_id[i+1]].pos.y - astar_nodes[path_id[i]].pos.y),
					(double)(astar_nodes[path_id[i+1]].pos.x - astar_nodes[path_id[i]].pos.x));
			diff_angle = (Sint16)(((angle - last_angle)*PI4096)/M_PI);
			diff_angle = ASTAR_abs_angle(diff_angle);

			// On calcule la distance
			dist = GEOMETRY_distance(astar_nodes[path_id[i-1]].pos, astar_nodes[path_id[i]].pos);
			sum_dist += dist;

			debug_printf("Optim : i=%d (%d;%d) last_angle=%d angle=%d diff_angle=%d\n", i, astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y,
					(Sint16)(last_angle*180.0/M_PI), (Sint16)(angle*180.0/M_PI), ((diff_angle*180)/PI4096));

			// On interprète les données
			if(diff_angle >= MAX_ANGLE_IN_SLOW_SPEED){
				//On doit s'arrrêter
				displacements[j].point = astar_nodes[path_id[i]].pos;
				displacements[j].curve = FALSE;
				displacements[j].speed = speed;
				j++;
				sum_dist = 0;
			}else if(diff_angle >= MAX_ANGLE_IN_FAST_SPEED && sum_dist >= MAX_DISTANCE_IN_FAST_SPEED){
				// On doit freiner
				if(dist > DISTANCE_OF_BRAKING + 100 && speed == FAST){
					// On calcule un nouveau point dans le path qui sera un point de freinage
					displacements[j].point.x = astar_nodes[path_id[i]].pos.x + DISTANCE_OF_BRAKING*cos(last_angle+M_PI);
					displacements[j].point.y = astar_nodes[path_id[i]].pos.y + DISTANCE_OF_BRAKING*sin(last_angle+M_PI);
					debug_printf("New point added in path pos(%d;%d)\n", displacements[j].point.x, displacements[j].point.y);
					displacements[j].curve = TRUE;
					displacements[j].speed = speed;
					j++;
					displacements[j].point = astar_nodes[path_id[i]].pos;
					displacements[j].curve = TRUE;
					displacements[j].speed = SLOW;
					j++;
				}else{
					displacements[j].point = astar_nodes[path_id[i]].pos;
					displacements[j].curve = FALSE;
					if(speed == FAST)
						displacements[j].speed = SLOW;
					else
						displacements[j].speed = speed;
					j++;
				}
				sum_dist = 0;
			}else{
				// On continue à la vitesse normale
				displacements[j].point = astar_nodes[path_id[i]].pos;
				displacements[j].curve = TRUE;
				displacements[j].speed = speed;
				j++;
			}

		}

		//On stoke la position finale
		displacements[j].point = astar_nodes[path_id[nb_nodes-1]].pos;
		displacements[j].curve = TRUE;
		displacements[j].speed = speed;
		j++;

		*nb_displacements = j;


#elif defined(ASTAR_OPTIMISATION_2)

		for(i=1; i<nb_nodes-1; i++){
			if(ASTAR_IS_NODE_IN(path_id[i-1], astar_curves[path_id[i]])){
				path_curve[i] = TRUE;
			}else{
				path_curve[i] = FALSE;
			}
		}

		// Affichage de la trajectoire avec curve
		debug_printf("\n PATH CURVE\n");
		for(i=0; i<nb_nodes; i++){
			debug_printf("[%d] pos(%d;%d) curve = %d\n", path_id[i], astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y, path_curve[i]);
		}
		debug_printf("\n");
		debug_printf("nb_nodes=%d\n", nb_nodes);

		// Elimination des poinst inutiles
		// Lorsqu'un point est visible d'un autre point alors tous les points entre les deux peuvent être supprimés
		last_index = 0;
		j = 1;
		for(i=1; i<nb_nodes-1; i++){
			if(ASTAR_is_node_visible(astar_nodes[path_id[last_index]].pos, astar_nodes[path_id[i+1]].pos)){
				path_enable[i] = FALSE;
				debug_printf("Node %d deleted of path (%d;%d)\n", path_id[i], astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y);
			}else{
				last_index = i;
				path_id[j] = path_id[i];
				path_curve[j] = path_curve[i];
				j++;
			}
		}
		//On pense à stocker le dernier node
		path_id[j] = path_id[nb_nodes - 1];
		path_curve[j] = path_curve[nb_nodes - 1];
		nb_nodes = j + 1;


		// Affichage de la trajectoire optimisée
		debug_printf("\n PATH OPTIMISE\n");
		for(i=0; i<nb_nodes; i++){
			debug_printf("[%d] pos(%d;%d)\n", path_id[i], astar_nodes[path_id[i]].pos.x, astar_nodes[path_id[i]].pos.y);
		}
		debug_printf("\n");
		debug_printf("nb_nodes=%d\n", nb_nodes);

		for(i=1; i<nb_nodes; i++){
			displacements[i-1].point = astar_nodes[path_id[i]].pos;
			displacements[i-1].curve = path_curve[i];
			displacements[i-1].speed = speed;
		}
		*nb_displacements = nb_nodes - 1;

#else // SANS OPTIMISATION

		Uint8 curve_index;

		debug_printf("First node at distance = %d\n\n", GEOMETRY_distance(astar_nodes[FROM_NODE].pos, astar_nodes[path_id[1]].pos));
		//Suppression du premier node si on est trop près
		if(nb_nodes >= 3 && ASTAR_is_node_visible(astar_nodes[FROM_NODE].pos, astar_nodes[path_id[2]].pos)
				&& GEOMETRY_squared_distance(astar_nodes[FROM_NODE].pos, astar_nodes[path_id[1]].pos) <  DISTANCE_PROXIMITY_NODE*DISTANCE_PROXIMITY_NODE)
		{
			path_enable[1] = FALSE;
			debug_printf("First node (%d) deleted of path\n", path_id[1]);
		}

		//Suppression du dernier node si on est trop près
		if(nb_nodes >= 3 && ASTAR_is_node_visible(astar_nodes[path_id[nb_nodes-3]].pos, astar_nodes[path_id[nb_nodes-1]].pos)
				&& GEOMETRY_squared_distance(astar_nodes[path_id[nb_nodes-2]].pos, astar_nodes[path_id[nb_nodes-1]].pos) <  DISTANCE_PROXIMITY_NODE*DISTANCE_PROXIMITY_NODE)
		{
			path_enable[nb_nodes-2] = FALSE;
			debug_printf("Last node before dest (%d) deleted of path\n", path_id[nb_nodes-2]);
		}

		// Remplissage du tableau des déplacements
		j = 0;
		curve_index = 0;
		for(i=1; i<nb_nodes - 1; i++){
			if(path_enable[i]){
				displacements[j].point = astar_nodes[path_id[i]].pos;
				if(j == 0){
					displacements[j].curve = TRUE; // Quand on vient du point de départ, on autorise la courbe dans tous les cas.
				}else{
					if(ASTAR_IS_NODE_IN(curve_index, astar_curves[path_id[i]]))
						displacements[j].curve = TRUE;
					else
						displacements[j].curve = FALSE;
					debug_printf("Je suis node (%d, %d) et je viens de (%d, %d) : courbe=%d", displacements[j].point.x, displacements[j].point.y, displacements[j-1].point.x, displacements[j-1].point.y, displacements[j].curve);
				}
				curve_index = path_id[i];
				displacements[j].speed = speed;
				j++;
			}
		}
		displacements[j].point = astar_nodes[path_id[nb_nodes - 1]].pos;
		displacements[j].curve = TRUE;
		displacements[j].speed = speed;
		j++;

		*nb_displacements = j;

#endif /*ASTAR_OPTIMISATION*/

		debug_printf("\nFINAL PATH is (%d displacements)\n", *nb_displacements);
		for(i=0; i<*nb_displacements ; i++){
			debug_printf("pos(%d;%d) curve=%d speed=%d\n", displacements[i].point.x, displacements[i].point.y, displacements[i].curve, displacements[i].speed);
		}

		return result;
	}


	/** @brief ASTAR_try_going
	 *		Machine à état réalisant le try_going  après appel à l'algorithme astar
	 * @param x : l'abscisse u point d'arrivée
	 * @param y : l'ordonnée du point d'arrivée
	 * @param in_progress : l'état courant dans lequel ce ASTAR_try_going est effectué en stratégie
	 * @param success_state : l'état dans lequel on doit aller en cas de succès
	 * @param fail_state : l'état dans lequel on doit aller en cas d'échec
	 * @param speed : la vitesse du robot
	 * @param way : le sens de déplacements du robot
	 * @param avoidance : le paramètre d'évitement
	 * @param end_condition : la condition de fin (END_AT_BREAK ou END_AT_LAST_POINT)
	 * @return l'état en cours ou l'état de succès ou l'état d'échec
	 */
	Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition){
		static error_e result = IN_PROGRESS;
		static displacement_curve_t displacements[NB_NODES];
		static Uint8 nb_displacements;
		static Uint8 nb_try;
		static bool_e success_possible;
		static GEOMETRY_point_t from, dest;
		//static bool_e handles_foes;

		CREATE_MAE_WITH_VERBOSE(SM_ID_ASTAR_TRY_GOING,
								ASTAR_CONFIG,
								ASTAR_INIT,
								ASTAR_GO_DIRECTLY,
								ASTAR_COMPUTE,
								ASTAR_DISPLACEMENT,
								ASTAR_FAIL,
								ASTAR_SUCCESS
								);

		switch(state)
		{
			case ASTAR_CONFIG:
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
				state = ASTAR_INIT;
				break;

			case ASTAR_INIT:{
				from = (GEOMETRY_point_t){global.pos.x, global.pos.y};
				dest = (GEOMETRY_point_t){x, y};

				// On génère le graphe
				ASTAR_generate_graph(from, dest);

				if(ASTAR_is_node_visible((GEOMETRY_point_t){global.pos.x, global.pos.y}, dest)){ // Si le point de destination est directement visible à partir du point de départ
					nb_try--; // On le compte comme une tentative
					state = ASTAR_GO_DIRECTLY;
				}else{
					state = ASTAR_COMPUTE;
				}
			}break;

			case ASTAR_GO_DIRECTLY:
				result = try_going(x, y,  IN_PROGRESS, END_OK, NOT_HANDLED, speed, way, avoidance, end_condition);

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
						//debug_printf("result ASTAR_GO_DIRECTLY is %d\n", result);
						if(nb_try == 0){
							state = ASTAR_FAIL;
						}else{
							from = (GEOMETRY_point_t){global.pos.x, global.pos.y}; // On réinitialise la position de départ
							ASTAR_generate_graph(from, dest);
							state = ASTAR_COMPUTE; // On tente en pathfind
						}
				}
				break;

			case ASTAR_COMPUTE:
				result = ASTAR_compute(displacements, &nb_displacements, from, dest, speed,  FALSE);
				if(result == END_OK){
					success_possible = TRUE;
					state = ASTAR_DISPLACEMENT;
				}else if(result == FOE_IN_PATH){
					success_possible = FALSE;
					state = ASTAR_DISPLACEMENT;
					warn_printf("ASTAR failure but will try to reach x=%d y=%d\n", displacements[nb_displacements-1].point.x, displacements[nb_displacements-1].point.y);
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
								state = ASTAR_INIT; // On retente
						}
						break;
					case FOE_IN_PATH:
					case NOT_HANDLED:
					case END_WITH_TIMEOUT:
					default:
						if(nb_try == 0)
							state = ASTAR_FAIL;
						else
							state = ASTAR_INIT; // On retente
				}
				break;
			case ASTAR_FAIL:
				//debug_printf("Finish with failure\n");
				state = ASTAR_CONFIG;
				return fail_state;
				break;
			case ASTAR_SUCCESS:
				//debug_printf("Finish with success\n");
				state = ASTAR_CONFIG;
				return success_state;
				break;
		}
		return in_progress;
	}



//--------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------- Fonctions annexes ---------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------


	/** @brief ASTAR_get_symetric_node
	 *		Renvoie l'id du node symétrique au node demandé (fonctionnement analogue à COLOR_Y)
	 * @param id : l'id du node
	 * @return l'id du node symétrique si TOP_COLOR ou le node lui même si BOT_COLOR
	 */
	static astar_node_id ASTAR_get_symetric_node(astar_node_id id){
		astar_node_id sym_id = 0;

		if(global.color == BOT_COLOR){
			sym_id = id;
		}else{
			if(id >=A1 && id <=A3)
				sym_id = I1 + id;
			else if(id >=B1 && id <=B3)
				sym_id = H1 - B1 + id;
			else if(id >=C1 && id <=C2)
				sym_id = G1 - C1 + id;
			else if(id >=D1 && id <=D3)
				sym_id = F1 - D1 + id;
			else if(id >=E1 && id <=E2)
				sym_id = id;
			else if(id >=F1 && id <=F3)
				sym_id = D1 - F1 + id ;
			else if(id >=G1 && id <=G2)
				sym_id = C1 - G1 + id;
			else if(id >=H1 && id <=H3)
				sym_id = B1 - H1 + id;
			else if(id >=I1 && id <=I3)
				sym_id = A1 - I1 + id;
			else
				debug_printf("ASTAR_get_symetric_node : symetric node of %d not found\n", id);
		}
		return sym_id;
	}


	/** @brief ASTAR_pathfind_cost
	 *		Calcul du cout entre deux nodes
	 * @param start_node : le node de départ
	 * @param end_node: le node d'arrivée
	 * @param with_angle : doit-on prendre en compte une optimisation de l'angle dans le calcul
	 * @param with_foes : doit-on prendre en compte une optimisation de la distance avec les adversaires dans le calcul
	 * @return le cout entre les deux nodes passés en paramètre
	 */
	static Uint16 ASTAR_pathfind_cost(astar_node_id start_node, astar_node_id end_node, bool_e with_angle, bool_e with_foes){
		assert(start_node < NB_NODES);
		assert(end_node < NB_NODES);


		Uint16 cost = 0;
		Uint16 dist = 0, dist_foe = 0, min_dist_foe = MAX_COST;
		Uint8 parent = 0;
		Uint8 i;
		double new_angle, current_angle = 0;

		// Dans tous les cas, on prend en compte la distance
		dist = GEOMETRY_distance(astar_nodes[start_node].pos, astar_nodes[end_node].pos);
		if(with_foes){
			for(i=0;i<MAX_NB_FOES;i++){
				if(global.foe[i].enable){
					dist_foe = GEOMETRY_distance(astar_nodes[end_node].pos, (GEOMETRY_point_t){global.foe[i].x,  global.foe[i].y});
					if(dist_foe < min_dist_foe){
						min_dist_foe = dist_foe;
					}
				}
			}
			// Si on est très proche d'un adversaire, on alourdit le poids de façon inversement proportionnel
			// à la distance, ainsi le cout minimal se trouve aux alentours de MAX_COST_FOES en distance
			if(min_dist_foe < MAX_COST_FOES){
				cost = dist + MAX_COST_FOES - min_dist_foe;
			}else{
				cost = dist;
			}
		}else{
			cost = dist;
			//debug_printf("cost_dist(%d) = %d\n", end_node, dist);
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
				//debug_printf("cost_angle(%d) = %d   diff=%d\n", end_node, (MAX_COST_ANGLE * ASTAR_abs_angle((Sint16)(new_angle - current_angle))/PI4096), (Sint16)(new_angle - current_angle));
			}
		}

		return cost;
	}

	/** @brief ASTAR_abs_angle
	 *		Calcul de la valeur absolue de l'angle passé en paramètre
	 * @param angle : l'angle dont on veut la valeur absolue
	 * @return la valeur absolue de l'angle
	 */
	static Uint16 ASTAR_abs_angle(Sint16 angle){
		Uint16 result = 0;
		while(angle > PI4096)
			angle -= TWO_PI4096;
		while(angle <= -PI4096)
			angle += TWO_PI4096;

		if(angle >= 0)
			result = (Uint16)angle; // On le cast en unsigned
		else
			result = (Uint16)(-angle);

		return result;
	}

	/** @brief ASTAR_point_is_in_polygon
	 *		Fonction pour vérifier si un point est à l'intérieur d'un polygone
	 * @param polygon : le polygone pris en compte
	 * @param nodeTested : le node testé
	 * @return le booléen indiquant si le node est à l'intérieur (TRUE) ou à l'extérieur (FALSE) du polygone considéré
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

	/** @brief ASTAR_is_link_cut_by_hardlines
	 *		Fonction pour vérifier si le segment [p1;p2] est coupé par une hardline
	 * @param p1 : les coordonnées de l'extrémité 1 du segment
	 * @param p2 : les coordonnées de l'extrémité 1 du segment
	 * @return le booléen indiquant si le segment est coupé par une hardline ou non
	 */
	static bool_e ASTAR_is_link_cut_by_hardlines(GEOMETRY_point_t p1, GEOMETRY_point_t p2){
		Uint8 i = 0;
		GEOMETRY_segment_t seg_tested = {p1, p2};
		GEOMETRY_segment_t seg_hardline;
		bool_e result = FALSE;

		while(i<astar_nb_hardlines && !result){
			seg_hardline.a = astar_hardlines[i].ex1;
			seg_hardline.b = astar_hardlines[i].ex2;
			result = GEOMETRY_segments_intersects(seg_tested, seg_hardline);
			if(result){
				debug_printf("Segment (%d;%d) (%d;%d) is cut by hardline (%d;%d (%d;%d\n",
							 p1.x, p1.y, p2.x, p2.y, seg_hardline.a.x, seg_hardline.a.y, seg_hardline.b.x, seg_hardline.b.y);
			}
			i++;
		}
		return result;
	}


	/** @brief ASTAR_is_node_visible
	 *      Fonction pour vérifier si le point p2 est visible du point p1
	 *		Autrement dit fonction pour vérifier si le segment [p1;p2] est coupé par une hardline ou par un côté d'un polygone
	 * @param p1 : les coordonnées de l'extrémité 1 du segment
	 * @param p2 : les coordonnées de l'extrémité 1 du segment
	 * @return le booléen indiquant si p2 est visible depuis p1
	 */
	static bool_e ASTAR_is_node_visible(GEOMETRY_point_t p1, GEOMETRY_point_t p2){
		Uint8 i = 0, j = 0;
		bool_e result = FALSE;
		GEOMETRY_segment_t seg_tested = {p1, p2};
		GEOMETRY_segment_t seg_polygon;

		result = ASTAR_is_link_cut_by_hardlines(p1, p2);
		if(result)
			debug_printf("Segment is cut by hardline\n");

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
				debug_printf("\t costLengthPath=%d costHeuristic=%d costTotal=%d\n", astar_nodes[i].cost.lengthPath, astar_nodes[i].cost.heuristic,  astar_nodes[i].cost.total);
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

	/** @brief ASTAR_print_nodes
	 *		Procédure affichant la liste des nodes et leurs caractéristiques
	 * @param  with_neighbors : doit-on aussi afficher les voisins ?
	 */
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


	/** @brief ASTAR_print_obstacles
	 *		Procédure affichant la liste des obstacles(zones interdites et hardlines)
	 */
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
