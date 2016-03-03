#ifndef ZONES
#define	ZONES

#include "QS/QS_all.h"
#include "QS/QS_who_am_i.h"

#define TIMEOUT_3s		3000
#define TIMEOUT_5s		5000
#define TIMEOUT_10s		10000
#define TIMEOUT_20s		20000

typedef Uint16 zones_list_t;

#define NO_ZONE				0x0000
#define Z_OUR_DOORS			0x0002
#define Z_OUR_DUNE			0x0004
#define Z_ADV_DUNE			0x0008
#define Z_ADV_DOORS			0x0010
#define Z_OUR_START_ZONE	0x0020
#define Z_OUR_START			0x0040
#define Z_OUR_DEPOSE		0x0080
#define Z_ADV_DEPOSE		0x0100
#define Z_ADV_START			0x0200
#define Z_ADV_START_ZONE	0x0400
#define Z_OUR_ROCK			0x0800
#define Z_OUR_FISH			0x1000
#define Z_ADV_FISH			0x2000
#define Z_ADV_ROCK			0x4000

typedef enum {
	ZIS_Free,		//La zone est initialement libre
	ZIS_BIG,		//La zone appartient au BIG_ROBOT
	ZIS_SMALL		//La zone appartient au SMALL_ROBOT
} zone_owner_e;

typedef enum {
	ZS_Free,			//La zone est libre, on ne se base pas sur cette info, on demandera toujours à l'autre robot si on peut y aller dans la zone
	ZS_OwnedByMe,		//La zone est occupée par nous-même quand j'occupe une zone libre ou qui ne apprtient pas
	ZS_OwnedByOther,	//La zone est occupée par l'autre robot
	ZS_Acquiring,		//On a demandé le verrouillage de la zone, on attend une réponse de l'autre robot
	ZS_OtherTryLock		//L'autre robot a fait une demande pour aller dans une de nos zones
} zone_state_e;

typedef struct {
	Sint16 x1;
	Sint16 x2;
	Sint16 y1;
	Sint16 y2;
} rectangle_t;

typedef struct {
	zone_id_e id;
	rectangle_t rectangle;
	zone_owner_e owner;
	zone_state_e state;
	time32_t lastTime;			// Date du dernier changement d'état de la zone
	time32_t timeout;			// Timeout avant libération de la zone
	bool_e accept_donate;
} zone_meta_t;

/*
 * Initialisation des zones suivant la couleur de départ
 * Cette fonction doit être appelée obligatoirement juste avant le début du match ou à chaque changement de couleur
 */
void initZones();

/*
 * Fonction qui scrute l'état des zones
 * A chaque changement de la variable state d'une zone la variable lastTime est mise à jour
 * On va rendre les zones libres si lastTime + timeout > tempsDuMatch
 * Cela permet de libérer des zones qui sont restées trop longtemps occupées
 */
void ZONES_process_main();

/*
 * Fonction qui permet de demander la prise d'une zone
 * - timeout_msec : durée maximum de la demande de prise de zone
 * - in_progress_state : etat en cours
 * - success_state : etat en cas de succès
 * - cant_lock_state : etat si on a attendu plus que timeout_ms et la zone n'était toujours pas dispo. Mais l'autre robot répond à nos requetes
 * - no_response_state : etat si aucune réponse de l'autre robot
 */
Uint8 ZONES_try_lock_zone(zone_id_e id, Uint16 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state);

/*
 * Fonction qui gère la réception des messages CAN
 */
void ZONE_CAN_process_msg(CAN_msg_t *msg);

/*
 * Fonction qui modifie la durée du timeout associé à une zone
 * Cela permet de modifier le temps après lequel la zone va passer libre s'il n'y a pas eu de changement d'état de la zone
 */
void setZoneTimeout(zone_id_e id, time32_t timeout);

/*
 * Fonction à appeler dans le try_going ou autre part si l'on veut vérifier les zones libres
 * On regarde dans la liste des zones fournies si elles sont libres ou si elles nous appartiennent : renvoi TRUE sinon FALSE
 * - list : faire une liste des zones sous la forme suivante : (Z_OUR_DUNE | Z_OUR_DEPOSE)
 */
bool_e checkZones(zones_list_t list);

/*
 * Fonction à appeler dans le try_going ou autre part si l'on veut libérer des zones
 * On regarde dans la liste des zones fournies et on les libèrent si le robot n'est pas dans la zone
 * - list : faire une liste des zones sous la forme suivante : (Z_OUR_DUNE | Z_OUR_DEPOSE)
 */
void releaseZones(zones_list_t list);

/*
 * Renvoi le nom de la zone associé à l'id d'une zone
 */
char* stringNameZone(zone_id_e id);

#endif	/* ZONES_H */
