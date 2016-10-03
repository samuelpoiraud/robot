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



/*	Toutes les fonctions suivantes ont des machines d'état interne à réinitialiser
 *	entre chaque utilisation. Cette réinitialisation est automatique et s'effectue
 *	chaque fois que la fonction renvoie un signal de fin, c'est à dire NOT_HANDLED
 *	END_OK ou END_WITH_TIMEOUT (à l'écriture de ce commentaire. Veuillez consulter
 *	les commentaires des actions, qui devraient préciser les valeurs de retour des
 *	actions, au cas par cas.
 */

#ifndef AVOIDANCE_H
#define AVOIDANCE_H

#include "../QS/QS_all.h"
#include "../QS/QS_maths.h"
#include "../environment.h"
#include "../zones.h"

//-------------------------------------------------------------------Enumerations

/* Types d'évitements possibles */
typedef enum
{
	DODGE_AND_WAIT = 0,// tentative d'évitement immédiate, puis attente
	DODGE_AND_NO_WAIT,// tentative d'évitement immédiate, puis NOT_HANDLED
	NO_DODGE_AND_WAIT,// attente normale, puis NOT_HANDLED
	NO_DODGE_AND_NO_WAIT,// aucune attente, aucune esquive, on détecte, on NOT_HANDLED !
	NO_AVOIDANCE// désactive l'évitement
}avoidance_type_e;

/* Définition du type déplacement */
typedef struct
{
	GEOMETRY_point_t point;
	PROP_speed_e speed;
}displacement_t;

/* Définition du type déplacement avec courbe */
typedef struct
{
	GEOMETRY_point_t point;
	PROP_speed_e speed;
	bool_e curve;
}displacement_curve_t;

typedef enum
{
	END_AT_LAST_POINT = 0,
	END_AT_BRAKE
}STRAT_endCondition_e;



//------------------------------------------------------------------- Machines à états de déplacements

/*
 * Avance d'une distance d à partir de la position actuelle.
 *
 * pre	: la position du robot doit être à jour
 * post   : la pile asser est vidée
 *
 * param x/y			Coordonnée du point cible
 * param way			Sens de déplacement
 * param avoidance	  Type d'evitement à faire
 * param end_condition  Comportement du robot en fin de trajectoire
 * param in_progress	Etat en cours
 * param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * param fail_state	 Etat à retourner si le déplacement ne s'est pas terminé correctement
 *
 * return le state rentré en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_going(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition);

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
Uint8 try_going_multipoint(const displacement_t displacements[], Uint8 nb_displacements, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition);

/*
 * Placement du robot dans l'angle demandé
 *
 * pre	: la position du robot doit être à jour
 * post   : la pile asser est vidée
 * param angle : Angle en PI4096
 * param way			Sens de déplacement
 * param avoidance	  Type d'evitement à faire
 * param in_progress	Etat en cours
 * param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * param fail_state	 Etat à retourner si le déplacement ne s'est pas terminé correctement
 *
 * return le state rentré en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_go_angle(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, STRAT_endCondition_e endCondition);

/*
 * Fait avancer le robot vers un points jusqu'a ce qu'il rencontre un enemie ou un obstacle
 *
 * pre	: la position du robot doit être à jour
 * post   : la pile asser est vidée
 * param x/y : Point cible
 * param way			Sens de déplacement
 * param avoidance	  Type d'evitement à faire
 * param in_progress	Etat en cours
 * param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * param fail_state	 Etat à retourner si le déplacement ne s'est pas terminé correctement
 *
 * return sucess_state si le robot est arrivé au point voulue OU qu'il c'est bloqué dans un obstacle
 * return fail_state si le robot à du arrêter ça trajectoire à cause d'un ennemie
 */
Uint8 try_rush(Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, avoidance_type_e avoidance, bool_e force_rotate);

/*
 * Calcul le point pour une distance dist à partir de la position actuelle et en fonction du sens donné.
 *
 * pre	: la position du robot doit être à jour
 *
 * param dist		   Distance à se déplacer.
 * param way			Sens de déplacement
 * return le point calculé
 */
void compute_advance_point(Uint16 dist, way_e way, GEOMETRY_point_t *point);

/*
 * Avance d'une distance dist à partir de la position actuelle et en fonction du sens donné.
 *
 * pre	: la position du robot doit être à jour
 * post   : la pile asser est vidée
 *
 * param point			Variable option dans le cas d'un calcul externe de la distance
 * param compute		Variable permettant de savoir si l'ont veut compute ou alors prendre le point en paramètre (lors du compute le point est mise à jours)
 * param dist			Distance à se déplacer.
 * param way			Sens de déplacement
 * param avoidance		Type d'evitement à faire
 * param end_condition  Comportement du robot en fin de trajectoire
 * param in_progress	Etat en cours
 * param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * param fail_state		Etat à retourner si le déplacement ne s'est pas terminé correctement
 *
 * return le state rentré en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_advance(GEOMETRY_point_t *point, bool_e compute, Uint16 dist, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, STRAT_endCondition_e end_condition);


/*
 * Arrete le robot
 *
 * post   : la pile asser est vidée
 *
 * param in_progress	Etat en cours
 * param success_state  Etat à retourner si le déplacement s'est terminé correctement
 * param fail_state	 Etat à retourner si le déplacement ne s'est pas terminé correctement
 *
 * return le state rentré en argument correspondant au resultat du goto_pos_with_scan_foe
 */
Uint8 try_stop(Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

Uint8 try_rushInTheWall(Sint16 angle, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, way_e way, bool_e asser_rotate, Uint8 thresholdError, Uint8 acceleration);


//------------------------------------------------------------------- Fonctions relatives à l'évitement

// Envoi un message CAN qui va forcer l'évitement du robot à la propulsion
void AVOIDANCE_forced_foe_dected();

//Défini le temps de timeout d'evitement (pour *AND_WAIT). Ce temps est valide que pour le prochain mouvement, il est réinitialisé après.
void AVOIDANCE_set_timeout(Uint16 msec);



/*
 * Fonction qui réalise un PROP_push_goto tout simple avec la gestion de l'évitement (en courbe)
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
error_e goto_pos_curve_with_avoidance(const displacement_t displacements[], const displacement_curve_t displacements_curve[], Uint8 nb_displacements, way_e way, avoidance_type_e avoidance_type, STRAT_endCondition_e end_condition, prop_border_mode_e border_mode);

//------------------------------------------------------------------- Fonctions autres


/*
 * Envoie un message CAN à l'asser et attend la reponse
 * return IN_PROGRESS   : requete de position envoyée, attente de réponse.
 * return END_OK		: la position du robot est à jour
 */
error_e ACTION_update_position();


//Le point passé en paramètre permet-il les rotations ?
bool_e is_possible_point_for_rotation(GEOMETRY_point_t * p);

//Le point passé en paramètre permet-il une extraction ?
bool_e is_possible_point_for_dodge(GEOMETRY_point_t * p);

#endif /* ndef AVOIDANCE_H */
