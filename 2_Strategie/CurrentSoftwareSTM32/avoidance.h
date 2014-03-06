/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP
 *
 *	Fichier : avoidance.c
 *	Package : Carte Principale
 *	Description : Actions relatives au déplacement avec evitement
 *	Auteur : Jacen
 *	Version 20110426
 */

#include "QS/QS_all.h"

/*	Toutes les fonctions suivantes ont des machines d'état interne à réinitialiser
 *	entre chaque utilisation. Cette réinitialisation est automatique et s'effectue
 *	chaque fois que la fonction renvoie un signal de fin, c'est à dire NOT_HANDLED
 *	END_OK ou END_WITH_TIMEOUT (à l'écriture de ce commentaire. Veuillez consulter
 *	les commentaires des actions, qui devraient préciser les valeurs de retour des
 *	actions, au cas par cas.
 */

#ifndef AVOIDANCE_H
	#define AVOIDANCE_H

	#include "asser_types.h"
	#include "Geometry.h"
	#include "config_use.h"

	#ifdef USE_POLYGON
		#include "polygon.h"
	#else
		//#include "Pathfind.h"
	#endif

	// Macros permettant de symétriser le terrain
	#define COLOR_Y(y)		((global.env.color == RED) ? (y) : (GAME_ZONE_SIZE_Y - (y)))
	#define COLOR_ANGLE(a)	((global.env.color == RED) ? (a) : (-(a)))

	// Macro permettant d'utiliser les courbes ou pas selon USE_ASSER_MULTI_POINT
	// En effet, on ne fait des courbes que si l'on est en multi-poinrs car sinon
	// il est plus rapide de faire une rotation puis une translation
	#ifdef USE_ASSER_MULTI_POINT
		#define ASSER_CURVES	1
	#else
		#define ASSER_CURVES	0
	#endif

	typedef enum
	{
		END_OK=0,
		IN_PROGRESS,
		END_WITH_TIMEOUT,
		NOT_HANDLED,
		FOE_IN_PATH
	}error_e;

	/* Types d'évitements possibles */
	typedef enum
	{
		NORMAL_WAIT = 0,		// attente normale puis esquive
		DODGE_AND_WAIT,			// tentative d'évitement immédiate, puis attente
		DODGE_AND_NO_WAIT,		// tentative d'évitement immédiate, puis NOT_HANDLED
		NO_DODGE_AND_WAIT,		// attente normale, puis NOT_HANDLED
		NO_DODGE_AND_NO_WAIT,	// aucune attente, aucune esquive, on détecte, on NOT_HANDLED !
		NO_AVOIDANCE,			// désactive l'évitement
	} avoidance_type_e;

	/*Type d'evitement pour construction du message de debug*/
	typedef enum{
		FORCED_BY_USER = 0,
		FOE1,
		FOE2,
	} foe_origin_e;

	/* Définition du type déplacement */
	typedef struct
	{
		GEOMETRY_point_t point;
		ASSER_speed_e speed;
	} displacement_t;

	/* Définition du type déplacement avec courbe */
	typedef struct
	{
		GEOMETRY_point_t point;
		ASSER_speed_e speed;
		bool_e curve;
	} displacement_curve_t;

	bool_e foe_in_path(void);

	/*
	 * Affecture une trajectoire courbe de la position actuelle. Le robot
	 * arrive au point x,y avec angle comme incidence. La courbe est divisée
	 * en plusieurs points (precision = nb de points)
	 * return : IN_PROGRESS, END_OK, END_WITH_TIMEOUT
	 */
	error_e smooth_goto (Sint16 x, Sint16 y, Sint16 angle, Uint8 precision);

	#ifdef USE_POLYGON
		/**
		* Action qui déplace le robot grâce à l'algorithme des polygones en testant avec tous les elements
		* puis seulement avec les notres s'il est impossible de trouver un chemin
		*
		* pre : Etre sur le terrain
		* post : Robot aux coordonnées voulues
		*
		* param x : Abscisse de la destination
		* param y : Ordonnée de la destination
		* param way : sens de déplacement
		* param speed : vitesse de déplacement
		* param curve : utilisation ou non des courbes
		* param element_type : éléments concernés par l'algo de polygones
		* return IN_PROGRESS : En cours
		* return END_OK : Terminé
		* return NOT_HANDLED : Action impossible
		* return END_WITH_TIMEOUT : Timeout
		*/
			error_e goto_polygon_default(Sint16 x, Sint16 y, way_e way, ASSER_speed_e speed, Uint8 curve,polygon_elements_type_e element_type);

		/**
		* Action qui déplace le robot grâce à l'algorithme de polygones
		* Les polygones sont construits à partir des valeurs contenues dans le tableau d'élements
		*
		* param x : Abscisse de la destination
		* param y : Ordonnée de la destination
		* param way : sens de deplacement
		* param speed : vitesse de deplacement
		* param curve : utilisation ou non des courbes
		* param type_elements : type d'éléments utilisés comme polygones
		*
		* return IN_PROGRESS : En cours
		* return END_OK : Terminé
		* return NOT_HANDLED : Action impossible
		* return END_WITH_TIMEOUT : Timeout
		*/
		error_e goto_polygon(Sint16 x, Sint16 y, way_e way, ASSER_speed_e speed, Uint8 curve, polygon_elements_type_e type_elements);

		error_e goto_node(Uint8 node, ...);
	#else
		error_e goto_polygon_default(Sint16 x,...);
		error_e goto_polygon(Sint16 x,...);

		/**
		* Amène le robot au noeud spécifié. renvoie NOT_HANDLED si noeud inaccessible
		*
		* pre  : le robot doit être à la position global.env.pos
		* post : la pile ASSER est vidée.
		*
		* param node : noeud de destination
		* param way : sens de déplacement
		*
		* return IN_PROGRESS : En cours
		* return END_OK : Terminé
		* return NOT_HANDLED : Action impossible
		* return END_WITH_TIMEOUT : Timeout
		*/
		error_e goto_node (Uint8 node, ASSER_speed_e speed, way_e way);
	#endif /* ndef USE_POLYGON */

	/*
	 * Envoie le robot à l'angle spécifiée.
	 * pre  : le robot doit être à la position global.env.pos
	 * post : la pile ASSER est vidée.
	 * return : IN_PROGRESS, END_OK, END_WITH_TIMEOUT, NOT_HANDLED
	 */
	error_e goto_angle (Sint16 angle, ASSER_speed_e speed);

	/*
	 * Envoie le robot à la position spécifiée. renvoie NOT_HANDLED si adversaire en travers du chemin
	 * pre  : le robot doit être à la position global.env.pos
	 * post : la pile ASSER est vidée.
	 * return : IN_PROGRESS, END_OK, END_WITH_TIMEOUT, NOT_HANDLED
	 */
	error_e goto_pos(Sint16 x, Sint16 y, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition);

	/*
	 * Avance d'une distance d à partir de la position actuelle.
	 *
	 * pre    : la position du robot doit être à jour
	 * post   : la pile asser est vidée
	 * param d : Distance à parcourir, valeur positive.
	 * return le state rentré en argument correspondant au resultat du goto_pos_with_scan_foe
	 */
	Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, avoidance_type_e avoidance);
	Uint8 try_going_until_break(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, avoidance_type_e avoidance);
/*
	 * Comme try_going mais avec le support du multipoint
	 *
	 * pre : Etre sur le terrain
	 * post : Robot aux coordonnées voulues
	 *
	 * param displacements : deplacements de la trajectoire
	 * param nb_displacement : nombre de deplacements de la trajectoire
	 * param way : sens de déplacement
	 * param avoidance: type d'evitement à faire
	 * param in_progress état en cours
	 * param success_state état à retourner si le déplacement s'est terminé correctement
	 * param fail_state état à retourner si le déplacement ne s'est pas terminé correctement
	 */
	Uint8 try_going_multipoint(displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, ASSER_end_condition_e end_condition);
	/*
	 * Avance d'une distance d à partir de la position actuelle.
	 *
	 * pre    : la position du robot doit être à jour
	 * post   : la pile asser est vidée
	 * param d : Angle en PI4096.
	 * return le state rentré en argument correspondant au resultat du goto_pos_with_scan_foe
	 */
	Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed);

	Uint8 try_relative_move(Sint16 distance, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition);

	/*
	 * Avance d'une distance d à partir de la position actuelle.
	 *
	 * pre    : la position du robot doit être à jour
	 * post   : la pile asser est vidée
	 * param d : Distance à parcourir, valeur positive.
	 * return IN_PROGRESS   : le déplacement est en cours.
	 * return END_OK		: le robot s'est déplacé de d.
	 * return NOT_HANDLED   : une des coordonées de destination est négative.
	 */
	error_e relative_move (Sint16 d, ASSER_speed_e speed, way_e way, ASSER_end_condition_e end_condition);

	/*
	 * Envoie un message CAN à l'asser et attend la reponse
	 * return IN_PROGRESS   : requete de position envoyée, attente de réponse.
	 * return END_OK		: la position du robot est à jour
	 */
	error_e ACTION_update_position();

	/*
	 * Envoie un message CAN à l'asser et attend la reponse
	 * return IN_PROGRESS   : requete d'arret envoyée, attente de réponse.
	 * return END_OK		: le robot est arrêté et la pile ASSER est vidée
	 */
	error_e ACTION_asser_stop();


	/***************************** Evitement 2010 **************************/

	/*
	* Fonction et merde ya une erreur d'asser
	* C'est du made in Christian, faut revoir ça au calme
	* l'évitement de l'adversaire est plus prioritaire cette année (2011)
	*/
	error_e move_colision();


	/***************************** Evitement 2011 **************************/

	/*
	* Fonction qui réalise un ASSER_push_goto tout simple avec la gestion de l'évitement (en courbe)
	*
	* pre : Etre sur le terrain
	* post : Robot aux coordonnées voulues
	*
	* param displacements : deplacements de la trajectoire
	* param nb_displacement : nombre de deplacements de la trajectoire
	* param way : sens de déplacement
	* end_condition : doit on finir quand on freine sur le dernier point ou quand on y est ?
	*
	* return IN_PROGRESS : En cours
	* return END_OK : Terminé
	* return NOT_HANDLED : Action impossible
	* return END_WITH_TIMEOUT : Timeout
	* return FOE_IN_PATH : un adversaire nous bloque
	*/
	error_e goto_pos_with_scan_foe(displacement_t displacements[], Uint8 nb_displacements, way_e way,
			avoidance_type_e avoidance_type);

	error_e goto_pos_with_scan_foe_until_break(displacement_t displacements[], Uint8 nb_displacements, way_e way,
			avoidance_type_e avoidance_type);

	//Comme goto_pos_with_scan_foe mais avec choix de la fin, je savais pas quoi mettre comme nom ... si quelqu'un a une idée de nom utile, go changer ça :)
	error_e goto_pos_with_avoidance(displacement_t displacements[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, ASSER_end_condition_e end_condition);

	error_e goto_pos_curve_with_avoidance(displacement_t displacements[], displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, ASSER_end_condition_e end_condition);


	//Défini le temps de timeout d'evitement (pour *AND_WAIT). Ce temps est valide que pour le prochain mouvement, il est réinitialisé après.
	void AVOIDANCE_set_timeout(Uint16 msec);


typedef enum
{
	NORTH_US = 0,
	NORTH_FOE = 1,
	SOUTH_US = 2,
	SOUTH_FOE = 3
} foe_pos_e;

// Vérifie adversaire dans NORTH_BLUE, NORTH_RED...
foe_pos_e AVOIDANCE_where_is_foe(Uint8 foe_id);


/*
* Fonction qui regarde si l'adversaire est devant nous pendant un mouvement, et on l'évite si nécessaire
* Elle doit être appelée à la place de STACKS_wait_end_auto_pull (c'est géré dans cette fonction)
*
* pre : Etre sur le terrain
* post : Pile ASSER vidée
* param : nombre de mouvements chargés dans la pile
*
* return IN_PROGRESS : En cours
* return END_OK : Terminé
* return NOT_HANDLED : Action impossible, ou timeout normal
* return END_WITH_TIMEOUT : Adversaire rencontré, mais on est arrivé à destination
*/
error_e wait_move_and_scan_foe(avoidance_type_e avoidance_type);

/*
 * Envoie un message can lors d'un evitement avec les raisons
 *
 */
void debug_foe_reason(foe_origin_e origin, Sint16 angle, Sint16 distance);

Uint8 try_stop(Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

#endif /* ndef AVOIDANCE_H */
