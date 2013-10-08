/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : <act>.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'assiette contenant les cerises
 *  Auteur : Alexis
 *  Version 20130219
 *  Robot : Krusty
 */

#if 0

#include "<act>.h"
#ifdef I_AM_ROBOT_<robot>

//Pour gérer les messages CAN
#include "../QS/QS_CANmsgList.h"

//Pour asservir un moteur DC
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_adc.h"

//Pour gérer des AX12
#include "../QS/QS_ax12.h"

//Pour afficher des truc sur l'UART suivant le niveau d'importance (debug/info/erreur/...)
#include "../output_log.h"

//Fonction aidant la gestion de la pile d'action des actionneurs
#include "../act_queue_utils.h"

//Fichier de config de l'actionneur: le fait de le mettre ici et pas dans global_config.h permet
//de ne recompiler que ce fichier quand on change la config (compilation bien plus rapide)
#include "<act>_config.h"

//un petit define pour afficher des messages sur l'uart, exemple utilisation:
// COMPONENT_log(LOG_LEVEL_Warning, "Un message indiquant que quelquechose ne c'est pas bien passé, mais que c'est pas trop grave (c'est un warning). Fonctionnalitées de printf dispo: 2+2=%d\n", 2+2)
#define COMPONENT_log(...) OUTPUTLOG_PRETTY("<act_prefix>: ", OUTPUT_LOG_COMPONENT_<act_component>, ## __VA_ARGS__)

//Exemple: #define COMPONENT_log(...) OUTPUTLOG_PRETTY("PL: ", OUTPUT_LOG_COMPONENT_PLATE, ## __VA_ARGS__)

//Des defines dépendant de l'actionneur (en gros on met ce qu'on veux et ce qu'on a besoin)
#define PLATE_NUM_POS           3
	#define PLATE_HORIZONTAL_POS_ID 0
	#define PLATE_PREPARE_POS_ID    1
	#define PLATE_VERTICAL_POS_ID   2

#define PLATE_PLIER_DONT_SEND_RESULT 1

//Vérification des paramètres de l'actionneur: on vérifie que c'est bien cohérent et qu'on demande pas plus de moteur qu'on peut en gérer
#if DCMOTOR_NB_POS < PLATE_NUM_POS
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= PLATE_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

//Init des AX12, réutilisable à souhait
static void PLATE_initAX12();

//Une fonction récupérant l'image de la position d'un moteur DC (utilisé par le module DCMotor). Doit retourner un Sint16 et ne pas avoir d'argument
static Sint16 PLATE_getRotationAngle();

//D'autres fonctions qui ne sont utilisé que dans ce fichier (d'ou le static)
//Elle sont la pour séparer le code un peu et ne pas avoir trop de ligne dans une fonction
static void PLATE_rotation_command_init(queue_id_t queueId);
static void PLATE_rotation_command_run(queue_id_t queueId);
static void PLATE_plier_command_init(queue_id_t queueId);
static void PLATE_plier_command_run(queue_id_t queueId);


//Init de l'actionneur, indispensable
void PLATE_init() {
	DCMotor_config_t plate_rotation_config;
	static bool_e initialized = FALSE;

	//On évite de réinitialiser un actionneur qu'il l'est déjà
	if(initialized)
		return;
	initialized = TRUE;

	//On initialise ce qu'on utilise (sinon bug douteux et bizarre à prévoir)
	DCM_init();
	AX12_init();
	ADC_init();

	//Configuration d'un moteur DC
	plate_rotation_config.sensor_read = &PLATE_getRotationAngle;				//Pointeur vers la fonction qui renvoie une image de la position du moteur (typiquement la valeur d'un potar)
	plate_rotation_config.Kp = PLATE_ASSER_KP;									//Les coefs d'asservissement. C'est mieux de toujours avec des defines pour ça, mis dans le fichier de config
	plate_rotation_config.Ki = PLATE_ASSER_KI;									//KP = Coef proportionnel, influence la force/vitesse du moteur suivant l'écart entre la position demandée et la position réelle.
	plate_rotation_config.Kd = PLATE_ASSER_KD;									//KI = Coef intégral, Tant que le moteur n'est pas à la position voulu, on augmente peu à peu la puissance du moteur. Il finira normalement par atteindre sa position. (A ne pas utiliser avec un asservissement en vitesse)
																				//KP = Coef dérivé, plus le moteur va vite, plus on freine. Utilisé par exemple pour atteindre la position rapidement sans aller trop loin. (Ne pas frapper le sol dans le cas de la pince à assiette 2013)
	
	plate_rotation_config.pos[PLATE_HORIZONTAL_POS_ID] = PLATE_HORIZONTAL_POS;	//Définition de position
	plate_rotation_config.pos[PLATE_PREPARE_POS_ID] = PLATE_PREPARE_POS;		//Ici 3 positions sont définies: rangé/vertical, oblique/prepare, horizontal (pour prendre une assiette)
	plate_rotation_config.pos[PLATE_VERTICAL_POS_ID] = PLATE_VERTICAL_POS;
	plate_rotation_config.pwm_number = PLATE_DCMOTOR_PWM_NUM;					//Le numéro de la PWM (entre 1 et 4 inclus)
	plate_rotation_config.way_latch = &PLATE_DCMOTOR_PORT_WAY;					//Le port d'ou on gère le sens du moteur. en 2013 c'était toujours le port E => way_latch = &PORTE (ou &LATE)
	plate_rotation_config.way_bit_number = PLATE_DCMOTOR_PORT_WAY_BIT;			//Le bit du port E qui gère le sens du moteur. Pour RE0, c'est 0
	plate_rotation_config.way0_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY0;			//Rapport cyclique max dans chaque sens. A calculer sachant que l'alim c'est 24V
	plate_rotation_config.way1_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY1;			//Très souvent le même rapport cyclique dans les 2 sens, le sens 0 c'est le sens positif, le sens 1 négatif
	plate_rotation_config.timeout = PLATE_ASSER_TIMEOUT;						//Si on  a pas atteint la position à epsilon près après ce temps de timeout, le  moteur est arrêté (PWM à 0%) (il n'est pas necessaire d'utiliser DCM_restart pour redémarrer le moteur à une autre position)
	plate_rotation_config.epsilon = PLATE_ASSER_POS_EPSILON;					//Lorsque la position du moteur est la position voulu +/- epsilon, on considère qu'on à atteint la position (DCM_get_state() == DCM_IDLE)
	DCM_config(PLATE_DCMOTOR_ID, &plate_rotation_config);						//Configure le moteur suivant les infos dans la structure
	DCM_stop(PLATE_DCMOTOR_ID);													//Toujours être sur que le moteur va pas démarrer tout seul est plus sûr :)

	//Indication sur l'UART qu'on a initialisé l'actionneur moteur DC. Le message est affiché que si OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL vaut au moins LOG_LEVEL_Info (avec LOG_LEVEL_Warning par exemple, ce message ne sera pas affiché)
	COMPONENT_log(LOG_LEVEL_Info, "Pince à assiette initialisé (DCMotor)\n");

	//Init de l'AX12
	PLATE_initAX12();
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void PLATE_initAX12() {
	static bool_e ax12_is_initialized = FALSE;	//On l'init qu'une fois
	if(ax12_is_initialized == FALSE && AX12_is_ready(PLATE_PLIER_AX12_ID) == TRUE) {	//On init l'AX12 que s'il est la, sinon on tentera une prochaine fois
		ax12_is_initialized = TRUE;
		
		//13,6V max sur l'AX12
		AX12_config_set_highest_voltage(PLATE_PLIER_AX12_ID, 136);
		//7V mini sur l'AX12 (en pratique on utilise du 12V, mais les écarts sont totalement arbitraire, d'autre valeurs aurait pu être choisies, et elle ne sont la que pour allumer la LED)
		AX12_config_set_lowest_voltage(PLATE_PLIER_AX12_ID, 70);
		
		//Le couple max que doit avoir l'AX12. Pour des pinces, c'est mieux de ne pas mettre 100% pour pas qu'il casse. (sur les forums internet, cette possibilité n'est pas négligée, donc c'est bien de ne pas tenter la méthode bourin)
		AX12_config_set_maximum_torque_percentage(PLATE_PLIER_AX12_ID, PLATE_PLIER_AX12_MAX_TORQUE_PERCENT);

		//Ajustement des angles mini et maxi. Si l'AX12 se trouve entre ces valeurs, il va essayer d'atteindre l'angle valide le plus proche dans tous les cas.
		//Si on tente de positioner l'AX12 à un angle en dehors de cette plage, il ignorera la commande
		AX12_config_set_maximal_angle(PLATE_PLIER_AX12_ID, 300);
		AX12_config_set_minimal_angle(PLATE_PLIER_AX12_ID, 0);

		//On choisi quand allumer la LED. Pratique pour detecter visuellement un problème potentiel (comme surchauffe, des instructions invalides, ...)
		AX12_config_set_error_before_led(PLATE_PLIER_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		//Pareil que pour la LED, mais dans ce cas on indique ce qui doit causer l'arrêt de l'AX12 (si une des erreurs indiqué arrive, le couple sera mis à 0)
		AX12_config_set_error_before_shutdown(PLATE_PLIER_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.

		//On met l'AX12 à une position par défaut. (La fin de la fonction n'indique pas que l'AX12 est rendu à la position demandée, juste que l'AX12 à reçu la commande (ou pas si le retour n'est pas TRUE))
		AX12_set_position(PLATE_PLIER_AX12_ID, PLATE_ACT_PLIER_AX12_INIT_POS);
		COMPONENT_log(LOG_LEVEL_Info, "Pince AX12 initialisé\n");
	}
}

//Fonction appelée pour arreter imédiatement l'actionneur.
//En pratique, elle est appelée en fin de match, et de toute façon les moteurs sont stoppés.
//A ce moment, la queue n'est plus fonctionnelle (donc PLATE_run_command n'est plus jamais appelé)
void PLATE_stop() {
	DCM_stop(PLATE_DCMOTOR_ID);
}

//Fonction appelée quand un message CAN à été reçu
//On vérifie s'il est pour cet actionneur et on le traite si c'est le cas.
//Si la fonction renvoie TRUE, le message CAN ne sera pas traité par d'autre module (en gros on renvoie TRUE quand on a traité le message, sinon FALSE)
bool_e PLATE_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;

	if(msg->sid == ACT_PLATE) {
		//Le message est pour nous, on fait un switch sur la commande

		//Initialise l'AX12 de la pince s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		PLATE_initAX12(); 

		switch(msg->data[0]) {
			case ACT_PLATE_ROTATE_HORIZONTALLY:
			case ACT_PLATE_ROTATE_PREPARE:
			case ACT_PLATE_ROTATE_STOP:
				//Cette fonction s'occupe d'empiler les prises de sémaphore pour l'actionneur indiqué (ici QUEUE_ACT_Plate_Rotation).
				//Le message CAN et data[0] sont mis dans les paramètres lié à l'opération mise en queue. (récupérable avec QUEUE_get_arg(queueId))
				//Un paramètre peu être indiqué en plus (ici 0 car non utilisé, pour le lanceur de balle c'est la vitesse voulue par exemple)
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Plate_Rotation, &PLATE_run_command, 0);
				break;

			case ACT_PLATE_ROTATE_VERTICALLY:    //Cas spécial: fermer la pince avant de tourner en vertical (sinon ça ne rentrera pas dans le robot)
				//Ici on gère la queue à la main, car on fait plusieurs action d'un coup
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_PLIER_CLOSE, 0, &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){msg->data[0]         , 0, &ACTQ_finish_SendResult}      , QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_PLATE_PLIER_CLOSE:
			case ACT_PLATE_PLIER_OPEN:
			case ACT_PLATE_PLIER_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Plate_AX12_Plier, &PLATE_run_command, 0);
				break;

			default:
				//Dans le cas d'un message qui nous était destiné, mais dont on ne connait pas la commande, on indique le problème
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

//Récupère la valeur du potar, représentative de la position du moteur DC
static Sint16 PLATE_getRotationAngle() {
	return ADC_getValue(PLATE_ROTATION_POTAR_ADC_ID);
}

//Fonction appelée en boucle pour vérifier l'état d'une action: init, en cours, terminée
void PLATE_run_command(queue_id_t queueId, bool_e init) {
	//Si on est passé a une action suivante et qu'il y a eu une erreur, on évite de faire cette action.
	//On considère que une action dans la pile nécessite que toutes les actions précédentes se soient terminées correctement
	if(QUEUE_has_error(queueId)) {
		//S'il y avait une erreur, on ne fait pas l'action et on passe a la suivante (qui fera la même chose tant que la queue n'est pas vide)
		QUEUE_behead(queueId);
		return;
	}

	//Séparation dans plusieurs fonctions des différents évenements
	if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_Rotation) {    // Gestion des mouvements de rotation de l'assiette
		//Au début de chaque action, on l'initialise
		if(init)
			PLATE_rotation_command_init(queueId);
		else	//Un coup l'init fait, on vérifie si l'action est terminée ou pas
			PLATE_rotation_command_run(queueId);
	} else if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_AX12_Plier) {    // Gestion des mouvements de la pince à assiette
		if(init)
			PLATE_plier_command_init(queueId);
		else
			PLATE_plier_command_run(queueId);
	}
}

//Initialisation d'une commande de rotation du bras
static void PLATE_rotation_command_init(queue_id_t queueId) {
	Uint8 wantedPosition;
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	
	//On regarde quelle commande doit on gérer et on lance le mouvement demandé
	switch(command) {
		case ACT_PLATE_ROTATE_HORIZONTALLY:
			wantedPosition = PLATE_HORIZONTAL_POS_ID;
			//Coefs différents pour la descente car on sinon on tape trop fort le sol a cause du poids de l'assiette entre autres
			DCM_setCoefs(PLATE_DCMOTOR_ID, PLATE_ASSER_AMORTIZED_KP, PLATE_ASSER_AMORTIZED_KI, PLATE_ASSER_AMORTIZED_KD);
			break;

		case ACT_PLATE_ROTATE_PREPARE:
			wantedPosition = PLATE_PREPARE_POS_ID;
			DCM_setCoefs(PLATE_DCMOTOR_ID, PLATE_ASSER_KP, PLATE_ASSER_KI, PLATE_ASSER_KD);
			break;

		case ACT_PLATE_ROTATE_VERTICALLY:
			wantedPosition = PLATE_VERTICAL_POS_ID;
			DCM_setCoefs(PLATE_DCMOTOR_ID, PLATE_ASSER_KP, PLATE_ASSER_KI, PLATE_ASSER_KD);
			break;

		case ACT_PLATE_ROTATE_STOP:
			DCM_stop(PLATE_DCMOTOR_ID);
			//Le moteur atteindra l'état IDLE automatiquement et le message de retour sera envoyé, voir PLATE_rotation_command_run
			return;

		default: {
				COMPONENT_log(LOG_LEVEL_Error, "Invalid rotation command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	COMPONENT_log(LOG_LEVEL_Debug, "Rotation at position %d\n", DCM_getPosValue(PLATE_DCMOTOR_ID, wantedPosition));
	DCM_goToPos(PLATE_DCMOTOR_ID, wantedPosition);
	DCM_restart(PLATE_DCMOTOR_ID);
	//Ici, si tout c'est bien passé, le moteur est asservi à sa nouvelle position (mais ne la surement pas encore atteinte, il va faloir vérifier en boucle sa position avant de passer à l'action suivante)
}

//Gestion des états en cours d'exécution de la commande de rotation
static void PLATE_rotation_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	COMPONENT_log(LOG_LEVEL_Trace, "At position %d\n", PLATE_getRotationAngle());

	//Les états des moteurs DC sont géré dans cette fonction, elle renvoie TRUE si l'action est terminée (erreur ou pas) et des infos sur le résultat de l'action dans result, errorCode et line (utilisé pour du débuggage par exemple)
	//Un message de niveau Info est affiché sur l'UART (si activé) indiquant le résultat de l'action et un message CAN est envoyé à la strat pour lui dire que l'action est terminée (et s'il y a eu des erreurs ou pas)
	if(ACTQ_check_status_dcmotor(PLATE_DCMOTOR_ID, FALSE, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_PLATE, result, errorCode, line);
}

//Initialise une commande de la pince à assiette
static void PLATE_plier_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_PLATE_PLIER_OPEN:  *ax12_goalPosition = PLATE_PLIER_AX12_OPEN_POS; break;
		case ACT_PLATE_PLIER_CLOSE: *ax12_goalPosition = PLATE_PLIER_AX12_CLOSED_POS; break;
		case ACT_PLATE_PLIER_STOP:
			AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère la pince
			QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				COMPONENT_log(LOG_LEVEL_Error, "Invalid plier command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		COMPONENT_log(LOG_LEVEL_Error, "Invalid plier position: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	COMPONENT_log(LOG_LEVEL_Debug, "Move plier ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(PLATE_PLIER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(PLATE_PLIER_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		COMPONENT_log(LOG_LEVEL_Error, "AX12_set_position error: 0x%x\n", AX12_get_last_error(PLATE_PLIER_AX12_ID).error);
		QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de la pince.
static void PLATE_plier_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	//360° of large_epsilon, quand on a un timeout, on a forcément un resultat Ok (et pas d'erreur, on considère qu'on serre l'assiette)
	if(ACTQ_check_status_ax12(queueId, PLATE_PLIER_AX12_ID, QUEUE_get_arg(queueId)->param, PLATE_PLIER_AX12_ASSER_POS_EPSILON, PLATE_PLIER_AX12_ASSER_TIMEOUT, 360, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_PLATE, result, errorCode, line);
}


#endif  /* I_AM_ROBOT_KRUSTY */

#endif
