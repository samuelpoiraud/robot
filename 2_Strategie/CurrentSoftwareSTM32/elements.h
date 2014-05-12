/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : elements.h
 *	Package : Carte Principale
 *	Description : Traitement des informations sur les éléments de jeu
 *	Auteur : Ronan, Adrien
 *	Version 20110430
 */

#include "QS/QS_all.h"
#ifndef ELEMENTS_H
	#define ELEMENTS_H

	#include "Geometry.h"
	#include "asser_types.h"
	#include "avoidance.h"

	#define NB_ELEMENTS 				19


//	#warning A modifier/supprimer
//  #MysteryWarning un warning ici, c'est moche !
		/* #define communs à plusieurs modules */
//	#define MATCH_DURATION 				90000UL //90 secondes)
//	#define GAME_ZONE_SIZE_X			2100 //2100 mm
//	#define GAME_ZONE_SIZE_Y			3000 //3000 mm
// C'est défini dans environment, c'est mieux
	#define ELEMENTS_RADIUS 			100 // 100 mm de rayon pour les éléments (2010-2011)
	#define ELEMENTS_DIAMETER 			(2UL*ELEMENTS_RADIUS) // 200 mm de diamètre
	#define ELEMENTS_SQUARED_RADIUS 	(ELEMENTS_RADIUS*ELEMENTS_RADIUS)
	#define ELEMENTS_SQUARED_DIAMETER 	(ELEMENTS_DIAMETER*ELEMENTS_DIAMETER)
	#define CASE_SIZE_X 				350
	#define CASE_SIZE_Y 				350
	/* Taille des differentes zones */
	#define INTER_AREA_CROSS_WIDTH 		100
	#define SCORING_AREA_SIZE_X 		(CASE_SIZE_X-INTER_AREA_CROSS_WIDTH)
	#define SCORING_AREA_SIZE_Y 		(CASE_SIZE_Y-INTER_AREA_CROSS_WIDTH)
	#define GREEN_AREA_SIZE_X 			280
	#define GREEN_AREA_SIZE_Y 			500	// pour prendre les 2 côtés //450
	#define NB_ELEMENTS_MAX_IN_AREA 	3



	/* Nombre de zones */
	#define NB_AREAS 					83 //cf. Tableau des cases (ELEMENTS.png)
	#define NB_SCORING_AREA				36 //cases ou l'on peut marquer des points !
	#define NB_SECURE_SCORING_AREA		4
	#define NB_BONUS_SCORING_AREA		6
	#define NB_BORDER_SCORING_AREA		6
	#define NB_INTER_AREA				35
	#define NB_GREEN_AREA				10
	#define NB_START_AREA				2

	#define NB_CD_MAX_IN_CAPTAIN_CABIN 			4
	#define NB_GOLD_BAR_MAX_IN_CAPTAIN_CABIN 	1
	/* Enumeration des elements possibles */

	typedef enum
	{
		UNDEFINED_ELEMENT, 	//NA
		BLACK_CD,			//0 Pt (4 éléments)
		WHITE_CD, 			//1 Pt (34 éléments)
		GOLD_BAR,			//3 Pts (7 éléments)
		MAP,				//5 Pts (1 élément)
		BOTTLE				//5 Pts (2 éléments)
	}game_element_e;


	/* Enumeration des appartenances de chaque element */
	typedef enum
	{
		UNDEFINED_BELONGING, US, ADVERSARY, NONE
	}belonging_e;

	/* Structure d'un element */
	typedef struct
	{
		Sint16 x;
		Sint16 y;
		game_element_e type;
		time32_t element_add_time;
		Uint8 area_number;
		bool_e taken_already_tried;
		time32_t try_time;
	}element_t;

	/* Enumeration des differentes zones */
	typedef enum
	{
		SCORING_AREA, SECURE_SCORING_AREA, BONUS_SCORING_AREA, INTER_AREA, GREEN_AREA, START_AREA
	}area_type_e;

	/* Accesseur qui retourne l'appartenance de l'element */
	belonging_e ELEMENTS_belonging_type(Uint8 element_number);

	/* Accesseur qui retourne l'appartenance de la zone */
	belonging_e ELEMENTS_get_area_belonging(Uint8 area_number);

	/* Accesseur qui retourne le type de la zone */
	area_type_e ELEMENTS_get_area_type(Uint8 area_number);

	/* Accesseur qui retourne les coordonées du centre de la zone */
	GEOMETRY_point_t ELEMENTS_get_area_center(Uint8 area_number);

	/* Affecte la zone comme étant impossible à déposer */
	void ELEMENTS_set_file_already_tried(Uint8 area_number);

	void ELEMENTS_lock_update();

	void ELEMENTS_unlock_update();

	/* Initialise les zones du terrain */
	void ELEMENTS_init();

	/* Mise a jour du terrain (suppression des informations depassees) */
	void ELEMENTS_update();

	/* Ajoute un element en regardant s'il n'y est pas deja */
	void ELEMENTS_add_or_update(Sint32 x, Sint32 y, game_element_e type, bool_e build, bool_e update_coords);

	void ELEMENTS_delete_with_element_number(Uint8 element_number);

	void ELEMENTS_delete_with_coordinates(Sint32 x, Sint32 y);

	/* Retourne le type d'element selon ses coordonnees */
	game_element_e ELEMENTS_type_element(Sint16 x, Sint16 y);

	/* Renvoie le numero de case a partir des coordonnees */
	Uint8 ELEMENTS_conversion(Sint16 x, Sint16 y);

	/* Ajoute un element a un autre element et retourne le nouvel element */
	game_element_e ELEMENTS_build(game_element_e element_1, game_element_e element_2);

	/* Renvoie le nombre de points de notre equipe */
	Uint8 ELEMENTS_nb_points_us();

	/* Renvoie le nombre de points de l'equipe adverse */
	Uint8 ELEMENTS_nb_points_foe();

	/* Renvoie le numero du pion le plus loin du robot adverse dont le
	type et l'appartenance sont specifies (supprime les cases securisees) */
	Uint8 ELEMENTS_furthest_foe(game_element_e type, belonging_e belonging);

	/* Renvoie un element adverse interressant ... (Voir tres interessant) */
	Uint8 ELEMENTS_interesting_element();

	/* Affichage du tableau des elements */
	void ELEMENTS_display();

	/* Affichage du plateau de jeu */
	void ELEMENTS_display_ascii();

	/**
	* Fonction de tri des numeros des elements
	*
	* elements_list : liste des elements
	* num_elements : liste des numeros des elements
	* size_tab : taille des deux tableaux
	* compare() : fonction de comparaison
	*/
	void sort_elements(element_t elements_list[],Uint8 num_elements[],Uint8 size_tab,bool_e (*compare) (element_t a,element_t b));

	/* Fonction qui indique si l'adversaire est proche du point passé en paramètre
	* TRUE = adversaire trop proche
	* FALSE = adversaire assez loin
	*/
	bool_e ELEMENT_foe_near_of_point(GEOMETRY_point_t target_point, Uint32 minimal_distance);

	/* Fonction qui indique si un nouvel élement a été détecté
	* TRUE = nouvel élement détecté
	* FALSE = pas de nouvel élement détecté
	*/
	bool_e ELEMENT_new_element_detected(Uint8 old_element_number, element_t last_element_saved);

	/* Fonction qui retourne un élément, du plus proche au plus loin, selon la position de l'adversaire */
	Uint8 ELEMENT_near_element_compute_foe();

	/* Fonction qui recherche la case vide la plus proche et assez loin de l'adversaire */
	Uint8 ELEMENT_near_empty_area_compute_foe();

	#define MINIMAL_ELEMENT_FOE_DISTANCE	400
	#define MINIMAL_AREA_FOE_DISTANCE		500


	#ifdef ELEMENTS_C

		/* Taille des differentes zones */
		#define START_AREA_SIZE_X 			500
		#define START_AREA_SIZE_Y 			500 // pour prendre les 2 côtés


		#warning Ne pas oublier d inserer les valeurs de la cale et du pont
		#define GREEN_AREA_SIZE_X 			280
		#define GREEN_AREA_SIZE_Y 			500	// pour prendre les 2 côtés


		#define TIME_BEFORE_EXPIRY_DEFAULT 			20000UL // 20 secondes
		#define TIME_BEFORE_EXPIRY_OUR_PAWN			30000UL // 30 secondes
		#define TIME_BEFORE_EXPIRY_ADVERSARY_PAWN	30000UL // 30 secondes
		#define TIME_BEFORE_EXPIRY_DEFAULT_PAWN		30000UL // 30 secondes
		#define TIME_BEFORE_EXPIRY_OUR_TOWER		40000UL // 40 secondes
		#define TIME_BEFORE_EXPIRY_ADVERSARY_TOWER	40000UL // 40 secondes
		#define TIME_BEFORE_EXPIRY_DEFAULT_TOWER	40000UL // 40 secondes

		#define TIME_BEFORE_RETRY_ACTION_ON_ELEMENT 10000UL // 10 secondes

		#define SQ_DISTANCE_MAX				9000000UL

		#ifdef DEBUG_ELEMENTS
			#define elements_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define elements_printf(...)	(void)0
		#endif

		/* Enumeration des differents modes de zones */
		typedef enum
		{
			NORMAL_AREA, TOP_AREA, BOTTOM_AREA, LEFT_AREA, RIGHT_AREA,
			TOP_LEFT_AREA, TOP_RIGHT_AREA, BOTTOM_LEFT_AREA, BOTTOM_RIGHT_AREA
		}mode_e;

		/* Structure definissant la partie constante des zones */
		typedef struct
		{
			area_type_e	type;
			GEOMETRY_point_t coords;
			mode_e mode;
		}area_mapping_t;

		/* Structure definissant la partie variable des zones */
		typedef struct
		{
			belonging_e belonging;
			Uint8 elements_number[NB_ELEMENTS_MAX_IN_AREA];
			Uint8 nb_elements;
			bool_e file_already_tried;
		}area_t;

		static bool_e ELEMENTS_exist_in_elements_list(Sint16 x, Sint16 y);

		static bool_e ELEMENTS_exist_in_area(Sint16 x, Sint16 y);

		static void ELEMENTS_delete_from_elements_list(Uint8 element_number);

		static void ELEMENTS_delete_from_areas(area_t* area_pt, Uint8 element_number_in_area);

		static void ELEMENTS_clear_area(Uint8 area_number);

		/**
		* Fonction de comparaison de deux elements selon leur distance par rapport au robot adverse
		*
		* current_element : element a
		* next_element : element b
		* return TRUE : l'element a plus loin que l'element b de l'adversaire
		* return FALSE : l'element b plus loin que l'element a de l'adversaire
		*/
		static bool_e compare_foe_position(element_t current_element,element_t next_element);

		static bool_e compare_area(area_mapping_t area_a,area_mapping_t area_b);

		/**
		* Fonction de comparaison de deux elements selon leurs types
		*
		* current_element : element a
		* next_element : element b
		* return TRUE : type de l'element a meilleur que le type de l'element b
		* return FALSE : type de l'element b meilleur que le type de l'element a
		*/
		static bool_e compare_types(element_t current_element,element_t next_element);

		/**
		* Fonction de comparaison de deux elements selon leur distance par rapport a notre robot
		*
		* current_element : element a
		* next_element : element b
		* return TRUE : l'element a plus proche que nous de l'element b
		* return FALSE : l'element b plus proche que nous de l'element a
		*/
		static bool_e compare_distance(element_t current_element,element_t next_element);

		static void sort_empty_area(Uint8 area_number_list[], Uint8* nb_empty_area,bool_e (*compare) (area_mapping_t area_a,area_mapping_t area_b));


		#ifdef USE_ELEMENT_CAN_DEBUG

			typedef enum
			{
				ADD=0x00, UPDATE=0x0F, DELETE=0xF0
			}event_type_e;

			/* Envoi d'un message CAN lors d'une mise a jour en debug */
			static void CAN_send_element_updated(event_type_e event, Uint8 element_number);

		#endif /* def USE_ELEMENT_CAN_DEBUG */

#endif /* def ELEMENTS_C */

	// Enumération

	typedef enum{
		FRUIT_VERIN_OPEN,
		FRUIT_VERIN_CLOSE,
		UNKNOWN
	}fruit_verin_state_e;

	typedef enum{
		OBJECT_IN_ZONE,
		ZONE_EMPTY,
		NO_ANSWER
	}scan_anything_e;

	typedef enum{
		ADVERSARY_TORCH,
		OUR_TORCH
	}torch_choice_e;

	typedef enum{
		NO_DISPOSE = 0,			//l'action do_torch se termine dès qu'on a la torche (sans la dépiler)
		HEARTH_OUR,			//Dépilage de la torche sur notre foyer
		HEARTH_ADVERSARY,	//Dépilage sur foyer adverse
		HEARTH_CENTRAL,		//Dépilage sur foyer central
		FILED_FRESCO,		//Dépilage devant les fresques
		ANYWHERE			//Dépilage n'importe où (la fonction choisit elle même le plus pertinent)
	}torch_dispose_zone_e;

	typedef enum{
		RED_CAVERN,
		YELLOW_CAVERN,
		RED_TREE_MAMOU,
		RED_TREE,
		YELLOW_TREE_MAMOU,
		YELLOW_TREE
	}pos_drop_e;

	typedef struct{
		Sint16 x;
		Sint16 y;
		Sint16 teta;
	}objet_t;

	// Va poser un triangle à l'endriot demandé
	error_e ELEMENT_go_and_drop(pos_drop_e choice);

	// Fonction de réception de message CAN
	void ELEMENT_triangle_add_to_list(CAN_msg_t* msg);	// Ajoute le triangle du message can dans la liste
	void ELEMENT_triangle_warner(CAN_msg_t* msg);		// Récupère le résultat du warner
	void ELEMENT_update_fruit_verin_state(CAN_msg_t* msg);	// Mets à jours l'état du labium
	void ELEMENT_answer_scan_anything(CAN_msg_t* msg);	// Récupère le résultat du scan
	void ELEMENT_answer_pump(CAN_msg_t *msg);			// Récupère la réponse de la pompe

	// Fonction utilisateur
	void ELEMENT_get_nb_object(Uint8 nb_obj[3]);	// Retourne le nb d'objet
	void ELEMENT_get_object(objet_t obj[3][20]);	// Retourne le tableau d'objet
	void ELEMENT_afficher_triangle();			// Affiche tout les triangles que le scan a trouver
	bool_e ELEMENT_triangle_present();			// Retourne la présence d'un triangle après demande de la warner
	bool_e ELEMENT_torche_present();			// Retounre la présence d'une torche après un scan
	scan_anything_e ELEMENT_get_result_scan();	// Retourne l'état du dernier scan

	// Fonction de lancement / subaction

	// Lance une warner pour la détection des triangles contre bordure de 0 à 3 en commençant par le triangle contre la bordure côté rouge
	void ELEMENT_launch_triangle_warner(Uint8 number_triangle);

	// subaction qui donne l'ordre puis attend que le labium soit dans la position 'labium_order'
	Uint8 ELEMENT_do_and_wait_end_fruit_verin_order(fruit_verin_state_e labium_order, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	// subaction qui dit d'aller en telle position puis d'effectuer un scan
	Uint8 ELEMENT_try_going_and_rotate_scan(Sint16 startTeta, Sint16 endTeta, Uint8 nb_points, Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, avoidance_type_e avoidance);

	// subaction qui effectue un scan
	Uint8 rotate_scan(Sint16 startTeta, Sint16 endTeta, Uint8 nb_points, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	// Nouvelle position de la torche apres une poussée
	void TORCH_new_position(torch_choice_e choice);

	// Renvois les position de la torche
	GEOMETRY_point_t TORCH_get_position(torch_choice_e choice);

	// Gere le message can de la nouvelle position de la torche
	void TORCH_CAN_process_msg(CAN_msg_t *msg);

	// Envois la nouvelle position de la torche par XBEE
	void TORCH_XBee_send_msg(torch_choice_e choice, GEOMETRY_point_t pos);

	// Attends que la pompe renvoie si elle détecte la une prise d'objet
	Uint8 ELEMENT_wait_pump_capture_object(Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

	Uint8 ELEMENT_wait_time(time32_t time, Uint8 in_progress, Uint8 success_state);

#endif /* ndef ELEMENTS_H */
