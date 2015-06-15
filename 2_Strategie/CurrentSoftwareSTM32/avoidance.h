/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP
 *
 *	Fichier : avoidance.c
 *	Package : Carte Principale
 *	Description : Actions relatives au d�placement avec evitement
 *	Auteur : Jacen
 *	Version 20110426
 */

#include "QS/QS_all.h"

/*	Toutes les fonctions suivantes ont des machines d'�tat interne � r�initialiser
 *	entre chaque utilisation. Cette r�initialisation est automatique et s'effectue
 *	chaque fois que la fonction renvoie un signal de fin, c'est � dire NOT_HANDLED
 *	END_OK ou END_WITH_TIMEOUT (� l'�criture de ce commentaire. Veuillez consulter
 *	les commentaires des actions, qui devraient pr�ciser les valeurs de retour des
 *	actions, au cas par cas.
 */

#ifndef AVOIDANCE_H
#define AVOIDANCE_H

#include "prop_types.h"
#include "QS/QS_maths.h"
#include "environment.h"

#ifdef USE_POLYGON
	#include "polygon.h"
#endif


// Macro permettant d'utiliser les courbes ou pas selon USE_PROP_MULTI_POINT
// En effet, on ne fait des courbes que si l'on est en multi-poinrs car sinon
// il est plus rapide de faire une rotation puis une translation
#define PROP_CURVES	1

//-------------------------------------------------------------------Enumerations

typedef enum
{
	END_OK=0,
	IN_PROGRESS,
	END_WITH_TIMEOUT,
	NOT_HANDLED,
	FOE_IN_PATH
}error_e;

/* Types d'�vitements possibles */
typedef enum
{
	NORMAL_WAIT = 0,		// attente normale puis esquive
	DODGE_AND_WAIT,// tentative d'�vitement imm�diate, puis attente
	DODGE_AND_NO_WAIT,// tentative d'�vitement imm�diate, puis NOT_HANDLED
	NO_DODGE_AND_WAIT,// attente normale, puis NOT_HANDLED
	NO_DODGE_AND_NO_WAIT,// aucune attente, aucune esquive, on d�tecte, on NOT_HANDLED !
	NO_AVOIDANCE// d�sactive l'�vitement
}avoidance_type_e;

/*Type d'evitement pour construction du message de debug*/
typedef enum {
	FORCED_BY_USER = 0,
	FOE1,
	FOE2
}foe_origin_e;

/* D�finition du type d�placement */
typedef struct
{
	GEOMETRY_point_t point;
	PROP_speed_e speed;
}displacement_t;

/* D�finition du type d�placement avec courbe */
typedef struct
{
	GEOMETRY_point_t point;
	PROP_speed_e speed;
	bool_e curve;
}displacement_curve_t;

/* D�finition du type mode de bordure qui permet la gestion de bordure fictive en propulsion */
typedef enum{
	PROP_NO_BORDER_MODE,
	PROP_BORDER_MODE
}prop_border_mode_e;

//------------------------------------------------------------------- Machines � �tats de d�placements

/*
 * Avance d'une distance d � partir de la position actuelle.
 *
 * pre	: la position du robot doit �tre � jour
 * post   : la pile asser est vid�e
 *
 * param x/y			Coordonn�e du point cible
 * param way			Sens de d�placement
 * param avoidance	  Type d'evitement � faire
 * param end_condition  Comportement du robot en fin de trajectoire
 * param in_progress	Etat en cours
 * param success_state  Etat � retourner si le d�placement s'est termin� correctement
 * param fail_state	 Etat � retourner si le d�placement ne s'est pas termin� correctement
 *
 * return le state rentr� en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);

/*
 * Comme try_going mais avec le support du multipoint
 *
 * pre : Etre sur le terrain
 * post : Robot aux coordonn�es voulues
 *
 * param displacements : deplacements de la trajectoire
 * param nb_displacement : nombre de deplacements de la trajectoire
 * param way : sens de d�placement
 * param avoidance: type d'evitement � faire
 * param in_progress �tat en cours
 * param success_state �tat � retourner si le d�placement s'est termin� correctement
 * param fail_state �tat � retourner si le d�placement ne s'est pas termin� correctement
 */
Uint8 try_going_multipoint(const displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);

/*
 * Placement du robot dans l'angle demand�
 *
 * pre	: la position du robot doit �tre � jour
 * post   : la pile asser est vid�e
 * param angle : Angle en PI4096
 * param way			Sens de d�placement
 * param avoidance	  Type d'evitement � faire
 * param in_progress	Etat en cours
 * param success_state  Etat � retourner si le d�placement s'est termin� correctement
 * param fail_state	 Etat � retourner si le d�placement ne s'est pas termin� correctement
 *
 * return le state rentr� en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed);

/*
 * Fait avancer le robot vers un points jusqu'a ce qu'il rencontre un enemie ou un obstacle
 *
 * pre	: la position du robot doit �tre � jour
 * post   : la pile asser est vid�e
 * param x/y : Point cible
 * param way			Sens de d�placement
 * param avoidance	  Type d'evitement � faire
 * param in_progress	Etat en cours
 * param success_state  Etat � retourner si le d�placement s'est termin� correctement
 * param fail_state	 Etat � retourner si le d�placement ne s'est pas termin� correctement
 *
 * return sucess_state si le robot est arriv� au point voulue OU qu'il c'est bloqu� dans un obstacle
 * return fail_state si le robot � du arr�ter �a trajectoire � cause d'un ennemie
 */
Uint8 try_rush(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, bool_e force_rotate);

/*
 * Calcul le point pour une distance dist � partir de la position actuelle et en fonction du sens donn�.
 *
 * pre	: la position du robot doit �tre � jour
 *
 * param dist		   Distance � se d�placer.
 * param way			Sens de d�placement
 * return le point calcul�
 */
void compute_advance_point(Uint16 dist, way_e way, GEOMETRY_point_t *point);

/*
 * Avance d'une distance dist � partir de la position actuelle et en fonction du sens donn�.
 *
 * pre	: la position du robot doit �tre � jour
 * post   : la pile asser est vid�e
 *
 * param point			Variable option dans le cas d'un calcul externe de la distance
 * param dist			Distance � se d�placer.
 * param way			Sens de d�placement
 * param avoidance		Type d'evitement � faire
 * param end_condition  Comportement du robot en fin de trajectoire
 * param in_progress	Etat en cours
 * param success_state  Etat � retourner si le d�placement s'est termin� correctement
 * param fail_state		Etat � retourner si le d�placement ne s'est pas termin� correctement
 *
 * return le state rentr� en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_advance(GEOMETRY_point_t *point, Uint16 dist, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);


/*
 * Arrete le robot
 *
 * post   : la pile asser est vid�e
 *
 * param in_progress	Etat en cours
 * param success_state  Etat � retourner si le d�placement s'est termin� correctement
 * param fail_state	 Etat � retourner si le d�placement ne s'est pas termin� correctement
 *
 * return le state rentr� en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_stop(Uint8 in_progress, Uint8 success_state, Uint8 fail_state);


//------------------------------------------------------------------- Fonctions relatives � l'�vitement

// Envoi un message CAN qui va forcer l'�vitement du robot � la propulsion
void AVOIDANCE_forced_foe_dected();

//D�fini le temps de timeout d'evitement (pour *AND_WAIT). Ce temps est valide que pour le prochain mouvement, il est r�initialis� apr�s.
void AVOIDANCE_set_timeout(Uint16 msec);

typedef enum
{
	NORTH_US = 0,
	NORTH_FOE = 1,
	SOUTH_US = 2,
	SOUTH_FOE = 3
}foe_pos_e;

// V�rifie adversaire dans NORTH_BLUE, NORTH_BOT_COLOR...
foe_pos_e AVOIDANCE_where_is_foe(Uint8 foe_id);

/*
 * Envoie un message can lors d'un evitement avec les raisons
 *
 */
void debug_foe_reason(foe_origin_e origin, Sint16 angle, Sint16 distance);

bool_e foe_in_square(bool_e verbose, Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2);


void clear_prop_detected_foe();

void set_prop_detected_foe(CAN_msg_t *msg);

/*
 * Fonction qui r�alise un PROP_push_goto tout simple avec la gestion de l'�vitement (en courbe)
 *
 * pre : Etre sur le terrain
 * post : Robot aux coordonn�es voulues
 *
 * param displacements : deplacements de la trajectoire
 * param nb_displacement : nombre de deplacements de la trajectoire
 * param way : sens de d�placement
 * end_condition : doit on finir quand on freine sur le dernier point ou quand on y est ?
 *
 * return IN_PROGRESS : En cours
 * return END_OK : Termin�
 * return NOT_HANDLED : Action impossible
 * return END_WITH_TIMEOUT : Timeout
 * return FOE_IN_PATH : un adversaire nous bloque
 */
error_e goto_pos_curve_with_avoidance(const displacement_t displacements[], const displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, PROP_end_condition_e end_condition, prop_border_mode_e border_mode);

//------------------------------------------------------------------- Fonctions autres


/*
 * Envoie un message CAN � l'asser et attend la reponse
 * return IN_PROGRESS   : requete de position envoy�e, attente de r�ponse.
 * return END_OK		: la position du robot est � jour
 */
error_e ACTION_update_position();

/*
 * Envoie un message CAN � l'asser et attend la reponse
 * return IN_PROGRESS   : requete d'arret envoy�e, attente de r�ponse.
 * return END_OK		: le robot est arr�t� et la pile PROP est vid�e
 */
error_e ACTION_prop_stop();

//Le point pass� en param�tre permet-il les rotations ?
bool_e is_possible_point_for_rotation(GEOMETRY_point_t * p);

/**
 * @brief i_am_in_square_color permet de tester si le robot est dans un carr� donn�e avec une gestion automatique en miroir avec la couleur
 * @return vrai si nous somme dans le carr�
 */
bool_e i_am_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2);



#endif /* ndef AVOIDANCE_H */
