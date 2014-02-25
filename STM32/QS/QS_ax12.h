/*
 *	Club Robot ESEO 2010 - 2013
 *	???
 *
 *	Fichier : QS_ax12.h
 *	Package : Qualite Software
 *	Description : Module de gestion des servomoteurs Dynamixel AX12/AX12+
 *	Auteur : Ronan, Jacen, modifié et complété par Alexis (2012-2013)
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20121110
 */

#ifndef QS_AX12_H
#define QS_AX12_H

//Les defines ici ne sont que pour la documentation et ne sont pas effectif (grace au #undef)
//Le premier undef permet de ne pas avoir d'erreur sur le define, ceci perd la définission de la constante, mais QS_all.h est re-inclu après qui inclu Global_config.h qui redéfinira ces constantes

/** Si défini, le driver est compilé et utilisable.
 *
 * Pour utiliser ce module vous devez avoir configuré vos AX-12+ de la manière suivante :
 * <ul>
 * <li> Baud Rate de 56700 bauds (Address 0x04)
 * <li> Return Delay Time de 200 µs (temps de transfert d'un packet à 57600 bauds) (Address 0x05)
 * <li> Les ID des AX-12+ doivent être différents et se suivre (ID = 0 ... n)
 * </ul>
 */
//#ifndef USE_AX12_SERVO
//#define USE_AX12_SERVO
//#undef  USE_AX12_SERVO
//#endif

/** @name Paramètres requis
 * Defines à définir obligatoirement dans Global_config pour utiliser ce driver.
 */
///@{
	/** \def  AX12_NUMBER
	 * Nombre d'AX12 maximum.
	 *
	 * Ce nombre défini la taille des buffers interne. Dans les fonctions, id_servo devra être strictement inférieur à ce nombre.
	 */
//	#ifndef AX12_NUMBER
//	#define AX12_NUMBER
//	#undef  AX12_NUMBER
//	#endif

	/** Port de contrôle de la direction du bus de données.
	 *
	 * Ce port est utilisé pour définir l'état du bus half-duplex, en recpetion, ce port est mis à l'état logique 0,
	 * en transmission il est mis à 1.
	 */
//	#ifndef AX12_DIRECTION_PORT
//	#define AX12_DIRECTION_PORT
//	#undef  AX12_DIRECTION_PORT
//	#endif

	/** Numéro du timer utilisé pour le timeout.
	 *
	 * Ce timer sera utilisé pour avoir un temps d'attente maximum lorsqu'un paquet de status est censé être enovoyé par l'AX12.
	 */
//	#ifndef AX12_TIMER_ID
//	#define AX12_TIMER_ID
//	#undef  AX12_TIMER_ID
//	#endif
///@}

/** @name Paramètres optionnels
 * Defines à définir dans Global_config.
 */
///@{
	/** Vitesse du bus de données.
	 *
	 * Vitesse du port série utilisé pour communiquer avec l'AX12 en bauds.
	 * L'UART2 est utilisé pour la communication, elle ne doit pas être utiliser par un autre module.
	 * Sur le robot de 2012-2013, une vitesse de 56700 bauds est utilisée, cette vitesse est aussi celle par défaut.
	 */
//	#ifndef AX12_UART_BAUDRATE
//	#define AX12_UART_BAUDRATE
//	#undef  AX12_UART_BAUDRATE
//	#endif

	/** Temps d'attente du paquet de status avant timeout.
	 *
	 * Ce temps est en milisecondes, si aucun paquet de status n'est reçu après ce temps après l'envoi du paquet instruction, il y a timeout.
	 * Valeur par défaut: 5ms.
	 * @see AX12_ERROR_TIMEOUT
	 */
//	#ifndef AX12_STATUS_RETURN_TIMEOUT
//	#define AX12_STATUS_RETURN_TIMEOUT
//	#undef  AX12_STATUS_RETURN_TIMEOUT
//	#endif


	/** Politique de retour de paquet status.
	 *
	 * Un paquet status est envoyé par l'AX12 après un paquet instruction dans certains cas défini par ce define.
	 * Le paquet de status informera le driver des erreurs rencontrées et pourra contenir des données demandées.
	 * Si #AX12_STATUS_RETURN_NEVER est utilisé, aucun paquet de status ne sera envoyé par le servo,
	 * et aucune des fonctions AX12_get* et AX12_config_get* ne serons utilisable.
	 * Valeurs possibles:
	 * <ul>
	 *  <li> AX12_STATUS_RETURN_NEVER Le servo ne renverra jamais de paquet de status (sauf dans le cas de #AX12_is_ready). La lecture de données n'est possible dans ce cas.
	 *  <li> AX12_STATUS_RETURN_ONREAD Le servo ne reverra des paquet de status seulement lors de lecture de données et pour #AX12_is_ready.
	 *  <li> AX12_STATUS_RETURN_ALWAYS Le servo renverra un paquet de status après chaque instruction envoyé (sauf lorsque l'instruction est envoyé a tous les servo en utilisant #AX12_BROADCAST_ID).
	 * </ul>
	 * @see AX12_config_set_status_return_mode
	 */
//	#ifndef AX12_STATUS_RETURN_MODE
//	#define AX12_STATUS_RETURN_MODE
//	#undef  AX12_STATUS_RETURN_MODE
//	#endif

	/** Taille du buffer interne d'instruction.
	 *
	 * Ce define défini la taille du buffer d'instruction, cette taille ne prend pas en compte le nombre d'AX12.
	 * Le buffer interne est commun à tous les AX12.
	 * Par defaut, la taille du buffer est de 4*AX12_NUMBER.
	 */
//	#ifndef AX12_INSTRUCTION_BUFFER_SIZE
//	#define AX12_INSTRUCTION_BUFFER_SIZE
//	#undef  AX12_INSTRUCTION_BUFFER_SIZE
//	#endif


	/** Mode tracage des paquets.
	 *
	 * Si ce define est défini, le driver affichera avec debug_printf les paquets envoyés et reçus.
	 * Attention, debug_printf sera utilisé en interruption, cette fonction n'est pas réentrante.
	 */
//	#ifndef AX12_DEBUG_PACKETS
//	#define AX12_DEBUG_PACKETS
//	#undef  AX12_DEBUG_PACKETS
//	#endif
///@}


/** @name Mode de renvois de paquet status */
///@{
	/** Aucun renvois de paquet status.
	 *
	 * Le servo ne renverra jamais de paquet de status (sauf dans le cas de #AX12_is_ready). La lecture de données n'est possible dans ce cas.
	 */
	#define AX12_STATUS_RETURN_NEVER 0

	/** Renvois de paquet status lors de réponse à une lecture de donnée.
	 *
	 * Le servo ne reverra des paquet de status seulement lors de lecture de données et pour #AX12_is_ready.
	 */
	#define AX12_STATUS_RETURN_ONREAD 1	//last_status sera le résultat d'un ping ou d'une lecture

	/** Toujours renvoyer une paquet de status.
	 *
	 * Le servo renverra un paquet de status après chaque instruction envoyé (sauf lorsque l'instruction est envoyé a tous les servo en utilisant #AX12_BROADCAST_ID).
	 */
	#define AX12_STATUS_RETURN_ALWAYS 2	//last_status sera le résultat de la dernière instruction
///@}


#include "QS_all.h"

#ifdef USE_AX12_SERVO

	/** Structure contenant le dernier status reçu d'un AX12.
	 *
	 * Il y a un status par AX12 d'enregistré par le driver,
	 * si le driver reçois un nouveau paquet de status, seul l'ancien status de cet AX12 sera écrasé.
	 * @see AX12_get_last_error
	 */
	typedef struct {
		/** Erreur constituée des flags AX12_ERROR_*. */
		Uint16 error;
		union {
			/** donnée eventuellement reçu sur 16bits, cette valeur partage le même emplacement mémoire que param_1 et param_2. */
			Uint16 param;
			/** 2 donnée sur 8bits. */
			struct {
				Uint8 param_1;
				Uint8 param_2;
			};
		};
	} AX12_status_t;

	/** Id broadcast, il permet d'envoyer des commandes à tous les servo connecté avec un seul appel de fonction.
	 *
	 * Aucun paquet de status ne sera retourné dans ce cas, sauf pour #AX12_is_ready */
	#define AX12_BROADCAST_ID		0xFE

	// Constantes des erreurs possibles
	/** Pas d'erreur.
	 *
	 * Si une variable d'erreur vaut cette constante, alors cette variable indique qu'il n'y a pas d'erreur.*/
	#define AX12_ERROR_OK			0x00

	/** Tension d'alimentation hors des limites.
	 *
	 * Les limites sont définies par #AX12_config_set_lowest_voltage et #AX12_config_set_highest_voltage. */
	#define AX12_ERROR_VOLTAGE		0x01

	/** L'angle demandé est hors des limites.
	 *
	 * Les limites des angles sont définies par #AX12_config_set_minimal_angle et #AX12_config_set_maximal_angle */
	#define AX12_ERROR_ANGLE		0x02

	/** La température interne du servo est trop élevée.
	 *
	 * Si cette erreur est levée, c'est probablement que le moteur force trop.
	 * @see AX12_config_get_temperature_limit
	 */
	#define AX12_ERROR_OVERHEATING	0x04

	/** Plage de valeur non respectée.
	 *
	 * Cette erreur indique qu'un paramètre passé à une instruction était hors de la plage acceptable. Par exemple un pourcentage de 300. */
	#define AX12_ERROR_RANGE		0x08

	/** Checksum du paquet instruction invalide.
	 *
	 * Le driver n'a pas envoyé un paquet instruction valide.
	 * Cette erreur est causé par le driver et non par l'utilisateur. */
	#define AX12_ERROR_CHECKSUM		0x10

	/** Charge trop élevée.
	 *
	 * La charge que subit le servo est trop forte, il ne peut pas garder la position demandée avec le couple maximum défini.
	 * @see AX12_set_torque_limit
	 */
	#define AX12_ERROR_OVERLOAD		0x20

	/** Instruction invalide.
	 *
	 * L'instruction envoyé au servo n'existe pas ou une demande d'exécution de l'action préparée a été faite alors qu'aucune action n'a été préparée.
	 * Cette erreur est causé par le driver et non par l'utilisateur. */
	#define AX12_ERROR_INSTRUCTION	0x40

	/** Paquet status attendu non reçu.
	 *
	 * Un paquet de status était censé être envoyé par le servo, mais le driver n'a rien reçu durant le temps imparti.
	 * Utilisez #AX12_is_ready pour savoir si le servo est connecté et répond correctement.
	 * Si erreur est définie avec AX12_ERROR_RANGE en même temps, c'est que le driver à attendu trop longtemps pour bufferiser la commande et l'a annulé.
	 * Cette erreur est toujours seule autrement (pas d'autre bit défini que celui là)
	 * @see AX12_STATUS_RETURN_TIMEOUT
	 * @see AX12_STATUS_RETURN_MODE
	 */
	#define AX12_ERROR_TIMEOUT		0x80

	/** Des instructions sont en cours de traitement pour l'AX12 considéré
	 *
	 * Une ou plusieurs commande on été demandées de manière asynchrone et le résultat renvoyé par l'AX12 n'a pas encore été reçu.
	 * Ce status n'est donc pas vraiment une erreur.
	 * @see AX12_get_last_error
	 * @see AX12_STATUS_RETURN_MODE
	 */
	#define AX12_ERROR_IN_PROGRESS  0x100

	/*****************************************************************************/
	/** Fonction d'utilisation de l'AX12										**/
	/*****************************************************************************/

	/**
	 * Initialise le driver, cet appel est nescessaire avant d'utiliser ce driver.
	 *
	 * Lors d'appel multiple, seul le premier appel est effectif, les autres sont ignorés.
	 * Lors de l'initialisation, le mode de retour est défini à #AX12_STATUS_RETURN_MODE et les angles limites sont défini à 0°-300°.
	 * Le servo est initialement en mode aservissement de position.
	 */
	void AX12_init();

	/**
	 * Retourne la dernière information de status envoyé par l'AX12.
	 *
	 * L'information s'y trouvant dépend de la dernière fonction utilisée
	 * ansi que du define #AX12_STATUS_RETURN_MODE.
	 * @param id_servo numéro du servo à verifier
	 * @return dernier status du servo indiqué
	 * @see AX12_reset_last_error
	 */
	AX12_status_t AX12_get_last_error(Uint8 id_servo);

	/**
	 * Réinitialise la dernière information de status à #AX12_ERROR_OK.
	 *
	 * Cette fonction ne devrait pas servir, mais aucune vérification du code concernant la remise à zero si une instruction s'est bien executée dans tous les cas n'a été faite
	 * @param id_servo numéro du servo dont il faut reinitialiser le status
	 * @see AX12_get_last_error
	 */
	void AX12_reset_last_error(Uint8 id_servo);

	/**
	 * Permet de savoir si le buffer d'instruction interne du driver est plein.
	 *
	 * S'il est plein et qu'une fonction appelée nécessite l'envoi d'un paquet a l'AX12,
	 * elle renverra FALSE si le buffer est resté plein trop longtemps.
	 * @return TRUE si le buffer est plein, sinon FALSE
	 */
	bool_e AX12_is_buffer_full();

	/**
	 * Envois un ping au servo pour connaitre son état.
	 *
	 * Les informations sur son état sont accessible avec #AX12_get_last_error.
	 * @param id_servo numéro du servo à verifier
	 * @return TRUE si le servo répond au ping, sinon FALSE
	 */
	bool_e AX12_is_ready(Uint8 id_servo);

	bool_e AX12_async_is_ready(Uint8 id_servo); //version asynchrone, resultat à tester avec AX12_get_last_error. Return FALSE que si la file d'action du driver ax12 est pleine (cas grave, buffer trop petit ou flood de demande à l'ax12/rx24 ?)

	/**
	 * Met le driver en mode préparation de commandes.
	 *
	 * Après l'appel à cette fonction, les commmandes ne sont pas exécuté directement par le servo.
	 * Les commandes sont exécuté en même temps après l'appel à #AX12_end_instruction_block.
	 * Les bloc de commande permettent une synchronisation de plusieurs servo lors de l'exécution de commande
	 * comme par exemple le mouvement simultané de plusieurs servos.
	 * Une seule commande peut être préparée par un servo.
	 * Seule les commandes d'écriture AX12_set* serons préparée. Les autres fonctionnerons comme avant l'appel à cette fonction.
	 * @see AX12_end_command_block
	 */
	void AX12_start_command_block();

	/**
	 * Termine le bloc de préparation de commande et exécute-les.
	 *
	 * Les commandes précédement préparée sont exécutée en même temps.
	 * @see AX12_start_command_block
	 */
	void AX12_end_command_block();


	//Configuration de l'AX12, perdure après mise hors tension sauf pour le verouillage de la config (lock).
	//Unités:
	// Angles en degrés
	// Vitesse en degré par seconde
	// Voltage en dixième de volt (50 => 5.0V)
	// Température en degré celsius
	// Pourcentage entre 0 et 100

#if !defined(AX12_STATUS_RETURN_MODE) || (AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER)

	/**
	 * Verifie si la configuration est protégée.
	 *
	 * @param id_servo numéro du servo à verifier
	 * @return TRUE si la configuration est verrouillée, sinon FALSE
	 * @see AX12_config_lock
	 */
	bool_e AX12_config_is_locked(Uint8 id_servo);

	/**
	 * Retourne le numéro de modèle d'un servo.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return Numéro de modèle
	 * @see AX12_config_get_firmware_version
	 */
	Uint16 AX12_config_get_model_number(Uint8 id_servo);

	/**
	 * Retourne la version du firmware d'un servo.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return Version du firmware
	 * @see AX12_config_get_model_number
	 */
	Uint8  AX12_config_get_firmware_version(Uint8 id_servo);

	/**
	 * Retourne le délai de réponse du servo.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return Temps de délai en microsecondes
	 * @see AX12_config_set_return_delay_time
	 */
	Uint16 AX12_config_get_return_delay_time(Uint8 id_servo);

	/**
	 * Retourne l'angle minimal autorisé (mode asservissement en position seulement).
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return Angle minimal en degrés
	 * @see AX12_config_set_minimal_angle
	 */
	Uint16 AX12_config_get_minimal_angle(Uint8 id_servo);

	/**
	 * Retourne l'angle maximal autorisé (mode asservissement en position seulement).
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return Angle maximal en degrés
	 * @see AX12_config_set_maximal_angle
	 */
	Uint16 AX12_config_get_maximal_angle(Uint8 id_servo);

	/**
	 * Retourne la température maximale de fonctionnement.
	 *
	 * Si le servo dépasse cette température, le bit d'erreur #AX12_ERROR_OVERHEATING
	 * est levé et suivant la configuration, la LED peut être allumée et/ou le servo désactivé.
	 * Il n'est pas conseillé de modifier cette valeur (par défaut: 80°C).
	 * @param id_servo numéro du servo à questionner
	 * @return Limite de température en degrés celsius
	 * @see AX12_config_set_temperature_limit
	 */
	Uint8  AX12_config_get_temperature_limit(Uint8 id_servo);

	/**
	 * Retourne la limite inférieure d'alimentation.
	 *
	 * Si la tension d'alimentation est en dessous de cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera levée. Suivant la configuration, la LED peut être allumée et/ou le servo désactivé.
	 * @param id_servo numéro du servo à questionner
	 * @return Tension minimale de fonctionnement en dixième de Volt (une valeur de 84 indique un tension de 8.4V)
	 * @see AX12_config_set_lowest_voltage
	 */
	Uint8  AX12_config_get_lowest_voltage(Uint8 id_servo);

	/**
	 * Retourne la limite supérieure d'alimentation.
	 *
	 * Si la tension d'alimentation est dépasse cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera levée. Suivant la configuration, la LED peut être allumée et/ou le servo désactivé.
	 * @param id_servo numéro du servo à questionner
	 * @return Tension maximale de fonctionnement en dixième de Volt (une valeur de 114 indique un tension de 11.4V)
	 * @see AX12_config_set_highest_voltage
	 */
	Uint8  AX12_config_get_highest_voltage(Uint8 id_servo);

	/**
	 * Retourne la puissance maximale autorisée par defaut.
	 *
	 * La puissance est un pourcentage de la puissance maximale que permet l'alimentation.
	 * @param id_servo numéro du servo à questionner
	 * @return Puissance maximale en pourcentage (entre 0 et 100)
	 * @see AX12_config_set_maximum_torque
	 * @see AX12_get_torque_limit
	 */
	Uint8  AX12_config_get_maximum_torque(Uint8 id_servo);

	/**
	 * Retourne le mode de retour d'information du servo.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return Mode de retour, peut être une des valeurs ce dessous:
	 * <ul>
	 *  <li> #AX12_STATUS_RETURN_ONREAD Le servo ne renverra un paquet de status lors de lecture de donnée et pour #AX12_is_ready
	 *  <li> #AX12_STATUS_RETURN_ALWAYS Le servo renverra un paquet a tout envoi de paquet d'instruction, sauf si l'ID #AX12_BROADCAST_ID est utilisé
	 * </ul>
	 * @see #AX12_STATUS_RETURN_MODE
	 * @see AX12_config_set_status_return_mode
	 */
	Uint8  AX12_config_get_status_return_mode(Uint8 id_servo);

	/**
	 * Retourne les erreurs qui allumerons la LED.
	 *
	 * Les erreurs sont les flags AX12_ERROR*, utilisez (flags & AX12_ERROR_???) != 0 pour savoir si l'erreur AX12_ERROR_??? est levée.
	 * @param id_servo numéro du servo à questionner
	 * @return Masque d'erreur constitué de AX12_ERROR*
	 * @see AX12_config_set_error_before_led
	 * @see AX12_config_get_error_before_shutdown
	 * @see AX12_set_led_enabled
	 */
	Uint8  AX12_config_get_error_before_led(Uint8 id_servo);

	/**
	 * Retourne les erreurs qui désactiverons le servo.
	 *
	 * Lorsque qu'une erreur est levée et que son bit corespondant est activé dans le masque donné par cette fonction
	 * le servo se désactive en mettant 0% comme couple maximal, utilisez #AX12_set_torque_limit pour autoriser un couple de sortie non nul.
	 * Les erreurs sont les flags AX12_ERROR*, utilisez (flags & AX12_ERROR_???) != 0 pour savoir si l'erreur AX12_ERROR_??? est levée.
	 * @param id_servo numéro du servo à questionner
	 * @return Masque d'erreur constitué de AX12_ERROR*
	 * @see AX12_config_set_error_before_shutdown
	 * @see AX12_config_get_error_before_led
	 * @see AX12_set_torque_limit
	 */
	Uint8  AX12_config_get_error_before_shutdown(Uint8 id_servo);
#endif

	/**
	 * Verrouille la configuration du servo.
	 *
	 * Après cet appel, il n'est plus possible d'utiliser les fonctions AX12_config_set* ansi que #AX12_set_punch_torque_percentage
	 * Le rallumage du servo est le seul moyen de le déverrouiller, lors de la mise sous tension, le servo est toujours déverrouillé.
	 * @param id_servo numéro du servo à verrouiller
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_is_locked
	 */
	bool_e AX12_config_lock(Uint8 id_servo);

	/**
	 * Change l'ID de TOUT les AX12 branché (à utiliser temporairement pour configurer un AX12 seulement !!!!)
	 *
	 * @param id_servo nouveau id
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 */
	bool_e AX12_config_set_id(Uint8 id_servo);

	/**
	 * Défini le délai de réponse que doit avoir le servo.
	 *
	 * Utilisez cette fonction si le driver n'a pas le temps de se placer en mode reception pour recevoir le paquet de status.
	 * @param id_servo numéro du servo à controler
	 * @param delay_us délai en microsecondes
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_set_return_delay_time
	 */
	bool_e AX12_config_set_return_delay_time(Uint8 id_servo, Uint16 delay_us);

	/**
	 * Défini l'angle minimal autorisé (mode asservissement en position seulement).
	 *
	 * L'angle doit être entre 0 et 300°. Si la valeur degre est en dehors de ces limites, la valeur prise en compte sera la limite correspondante.
	 * (Par exemple, AX12_config_set_minimal_angle(500) sera équivalent à AX12_config_set_minimal_angle(300))
	 * L'angle effectif peut avoir 1,3% d'erreur, l'angle défini réel de AX12_config_set_minimal_angle(300) est un angle limite de 296°.
	 * @param id_servo numéro du servo à régler
	 * @param degre angle en degrés
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_minimal_angle
	 */
	bool_e AX12_config_set_minimal_angle(Uint8 id_servo, Uint16 degre);

	/**
	 * Défini l'angle maximal autorisé (mode asservissement en position seulement).
	 *
	 * L'angle effectif peut avoir 1,3% d'erreur, l'angle défini réel de AX12_config_set_minimal_angle(300) est un angle limite de 296°.
	 * @param id_servo numéro du servo à régler
	 * @param degre angle en degrés
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_maximal_angle
	 */
	bool_e AX12_config_set_maximal_angle(Uint8 id_servo, Uint16 degre);

	/**
	 * Défini la température maximale avant de lever l'erreur #AX12_ERROR_OVERHEATING.
	 *
	 * @param id_servo numéro du servo à régler
	 * @param temperature température en degré celsius
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_temperature_limit
	 */
	bool_e AX12_config_set_temperature_limit(Uint8 id_servo, Uint8 temperature);

	/**
	 * Défini la tension minimale de fonctionnement.
	 *
	 * Si la tension d'alimentation est en dessous de cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera levée. Suivant la configuration, la LED peut être allumée et/ou le servo désactivé.
	 * @param id_servo numéro du servo à régler
	 * @param voltage tension en dixième de Volts (Une valeur de 76 correspond à 7.6V)
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_lowest_voltage
	 */
	bool_e AX12_config_set_lowest_voltage(Uint8 id_servo, Uint8 voltage);

	/**
	 * Défini la tension maximale de fonctionnement.
	 *
	 * Si la tension d'alimentation dépasse cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera levée. Suivant la configuration, la LED peut être allumée et/ou le servo désactivé.
	 * @param id_servo numéro du servo à régler
	 * @param voltage tension en dixième de Volts (Une valeur de 76 correspond à 7.6V)
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_highest_voltage
	 */
	bool_e AX12_config_set_highest_voltage(Uint8 id_servo, Uint8 voltage);

	/**
	 * Défini la valeur initiale du couple maximum utilisable par le servo.
	 *
	 * La puissance est un pourcentage de la puissance maximale que permet l'alimentation.
	 * @param id_servo numéro du servo à régler
	 * @param percentage Pourcentage de couple utilisable (entre 0 et 100)
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_maximum_torque_percentage
	 */
	bool_e AX12_config_set_maximum_torque_percentage(Uint8 id_servo, Uint8 percentage);

	/**
	 * Défini le mode de retour de paquet de status.
	 *
	 * Les paquets de status permette de recevoir des informations d'erreur et des données du servo.
	 * ATTENTION ! Cette fonction ne devrait pas être utilisée, changez la valeur de #AX12_STATUS_RETURN_MODE à la place.
	 * Cette fonction ne change pas le fonctionnement interne du driver vis-à-vis des retours d'informations.
	 * @param id_servo numéro du servo à régler
	 * @param mode mode de fonctionnement, peut être une des valeurs suivantes:
	 * <ul>
	 *  <li> #AX12_STATUS_RETURN_NEVER Le servo ne renverra jamais de paquet de status (sauf dans le cas de #AX12_is_ready). La lecture de données n'est possible dans ce cas.
	 *  <li> #AX12_STATUS_RETURN_ONREAD Le servo ne reverra des paquet de status seulement lors de lecture de données et pour #AX12_is_ready.
	 *  <li> #AX12_STATUS_RETURN_ALWAYS Le servo renverra un paquet de status après chaque instruction envoyé (sauf lorsque l'instruction est envoyé a tous les servo en utilisant #AX12_BROADCAST_ID).
	 * </ul>
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_status_return_mode
	 */
	bool_e AX12_config_set_status_return_mode(Uint8 id_servo, Uint8 mode);

	/**
	 * Défini les erreurs qui allumerons la LED.
	 *
	 * Les erreurs sont les flags AX12_ERROR*, utilisez l'operateur OU binaire (a | b par exemple) pour définir plusieurs erreurs
	 * qui devrons allumer la LED.
	 * @param id_servo numéro du servo à régler
	 * @param error_type Masque d'erreurs constitué de AX12_ERROR*
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_error_before_led
	 * @see AX12_config_set_error_before_shutdown
	 * @see AX12_set_led_enabled
	 */
	bool_e AX12_config_set_error_before_led(Uint8 id_servo, Uint8 error_type);

	/**
	 * Défini les erreurs qui désactiverons le servo.
	 *
	 * Les erreurs sont les flags AX12_ERROR*, utilisez l'operateur OU binaire (a | b par exemple) pour définir plusieurs erreurs.
	 * Lorsque qu'une erreur est levée et que son bit corespondant est activé dans le masque donné par cette fonction le servo
	 * se désactive en mettant 0% comme couple maximal, utilisez #AX12_set_torque_limit pour autoriser un couple de sortie
	 * non nul.
	 * @param id_servo numéro du servo à régler
	 * @param error_type Masque d'erreurs constitué de AX12_ERROR*
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_config_get_error_before_shutdown
	 * @see AX12_config_set_error_before_led
	 * @see AX12_set_torque_limit
	 */
	bool_e AX12_config_set_error_before_shutdown(Uint8 id_servo, Uint8 error_type); //AX12_ERROR_* (voir QS_ax12.h)


	//RAM

	/**
	 * Retourne le mode de fonctionnement du servo.
	 *
	 * Les mode possibles sont: asservissement en position ou asservissement en couple.
	 * @param id_servo numéro du servo à questionner
	 * @return <ul>
	 *  <li> TRUE si le servo est asservi en couple, l'arbre moteur n'a pas de limite d'angle et tourne en continu.
	 *  <li> FALSE si le servo est asservi en position (angle). Les limites d'angle sont définies avec #AX12_config_set_minimal_angle et #AX12_config_set_maximal_angle
	 * </ul>
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_is_wheel_mode_enabled(Uint8 id_servo);

#if !defined(AX12_STATUS_RETURN_MODE) || (AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER)
	/**
	 * Vérifie la sortie du servo est controlée ou non.
	 *
	 * Si la sortie n'est pas controlée, le moteur du servo n'est pas alimenté.
	 * @param id_servo numéro du servo à questionner
	 * @return TRUE si la sortie du servo est controlée en position ou couple, FLASE si la sortie est libre
	 * @see AX12_set_torque_enabled
	 */
	bool_e AX12_is_torque_enabled(Uint8 id_servo);

	/**
	 * Vérifie l'état de la LED.
	 *
	 * Cette LED est allumée soit par l'utilisateur, soit par le servo lui-même lorsque une erreur est levée
	 * selon la configuration de la LED avec #AX12_config_set_error_before_led.
	 * @param id_servo numéro du servo à questionner
	 * @return TRUE si la LED est allumée, sinon FALSE
	 * @see AX12_set_led_enabled
	 * @see AX12_config_set_error_before_led
	 */
	bool_e AX12_is_led_enabled(Uint8 id_servo);

	/**
	 * Retourne les paramètres de la réponse de l'asservissement en position.
	 *
	 * Graphique représentant le couple de sortie en fonction de l'erreur de position:
	 *
	 * <pre>\verbatim
	 *         Couple          Consigne de position
	 *              ^                   |
	 *              |=============\     |
	 *              |             ^\    |
	 * Sens anti-   |             | |   v
	 *   horaire  <-+-------------|-+=======+------------------->  Sens horaire
	 *              |             | ^   ^   |             X: Erreur en position
	 *              |        A--->| |<  |    \
	 *              |             | |<->|<->^ \===============
	 *              v             | | B | C | ^
	 *                            | |   |  >| |<---D
	 * \endverbatim
	 * </pre>
	 *
	 * === : Courbe du couple en fonction de l'erreur en position
	 *
	 * @param id_servo numéro du servo à questionner
	 * @param A représente l'intervale d'erreur dans laquelle le couple est proportionnel à l'erreur angulaire dans le sens anti-horaire. L'intervale d'erreur est [A+B, B]
	 * @param B représente l'intervale d'erreur dans laquelle le couple reste nul dans le sens anti-horaire. L'intervale d'erreur est [B, 0]
	 * @param C représente l'intervale d'erreur dans laquelle le couple reste nul dans le sens horaire. L'intervale d'erreur est [0, C]
	 * @param D représente l'intervale d'erreur dans laquelle le couple est proportionnel à l'erreur angulaire dans le sens horaire. L'intervale d'erreur est [C, C+D]
	 * @return TRUE si la commande s'est bien exécutée et que les paramètres A, B, C et D ont une valeur valide, FALSE sinon
	 * @see AX12_set_torque_response
	 */
	bool_e AX12_get_torque_response(Uint8 id_servo, Uint16* A, Uint16* B, Uint16* C, Uint16* D);

	/**
	 * Retourne la position actuelle du servo.
	 *
	 * Position des angles et vue de devant:
	 * <pre>\verbatim
	 *           | 150°
	 *           |
	 *           +
	 *          / \
	 *    300° /   \ 0°
	 * \endverbatim
	 * </pre>
	 * @param id_servo numéro du servo à questionner
	 * @return position actuelle en degrés
	 * @see AX12_set_position
	 */
	Uint16 AX12_get_position(Uint8 id_servo);

	/**
	 * Retourne la vitesse que le servo doit avoir lors d'un changement de position.
	 *
	 * Cette valeur n'est utilisé qu'en mode d'asservissement en position.
	 * Position des angles et vue de devant:
	 * <pre>\verbatim
	 *         |    | 150°
	 *    Sens |    |        |
	 * positif |    +        | Sens négatif
	 *         v   / \       |
	 *       300° /   \ 0°   v
	 * \endverbatim
	 * </pre>
	 * @param id_servo numéro du servo à questionner
	 * @return vitesse en degrés par seconde, positif dans le sens trigonométrique, négatif sinon
	 * @see AX12_set_move_to_position_speed
	 * @see AX12_set_wheel_mode_enabled
	 */
	Sint16 AX12_get_move_to_position_speed(Uint8 id_servo);

	/**
	 * Retourne le pourcentage de couple appliquée a la sortie du servo en mode asservissement en couple.
	 *
	 * Cette valeur n'est utilisé qu'en mode d'asservissement de couple.
	 * @param id_servo numéro du servo à questionner
	 * @return pourcentage de couple par rapport au couple maximal possible dépendant de l'alimentation
	 * @see AX12_set_speed_percentage
	 * @see AX12_set_wheel_mode_enabled
	 */
	Sint8  AX12_get_speed_percentage(Uint8 id_servo);

	/**
	 * Retourne le couple maximal utilisable par le servo.
	 *
	 * Le couple donné est en pourcentage du couple maximal dépendant de l'alimentation.
	 * S'il y a eu une erreur, selon la configuration, le servo peut se désactiver en mettant 0% comme couple maximal.
	 * @param id_servo numéro du servo à questionner
	 * @return pourcentage de couple maximum
	 * @see AX12_set_torque_limit
	 * @see AX12_config_set_error_before_shutdown
	 */
	Uint8  AX12_get_torque_limit(Uint8 id_servo);

	/**
	 * Retourne le pourcentage de couple minimal à exercer pour faire bouger le servo.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return pourcentage de couple
	 * @see AX12_set_punch_torque_percentage
	 */
	Uint8  AX12_get_punch_torque_percentage(Uint8 id_servo);

	/**
	 * Retourne une estimation du couple résistant.
	 *
	 * Cette valeur est déduite du couple apliqué en sortie, et ne devrais pas être utilisée
	 * pour calculer d'autre information sur la charge tel que son poids, mais seulement pour savoir dans quel sens la charge pousse.
	 * @param id_servo numéro du servo à questionner
	 * @return pourcentage du couple resistant par rapport au couple maximal
	 */
	Sint8  AX12_get_load_percentage(Uint8 id_servo);

	/**
	 * Retourne la tension d'alimentation.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return tension en dixième de Volts (Une valeur 87 correspond à 8.7V)
	 * @see AX12_config_set_lowest_voltage
	 * @see AX12_config_set_highest_voltage
	 */
	Uint8  AX12_get_voltage(Uint8 id_servo);

	/**
	 * Retourne la température interne du servo.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return température en degré celsius
	 * @see AX12_config_set_temperature_limit
	 */
	Uint8  AX12_get_temperature(Uint8 id_servo);

	/**
	 * Vérifie si une instruction a été préparée et est prête à être exécuté.
	 *
	 * @param id_servo numéro du servo à questionner
	 * @return TRUE si une instruction est prête à être exécuté, sinon FALSE
	 */
	bool_e AX12_is_instruction_prepared(Uint8 id_servo);

	/**
	 * Vérifie si le servo à atteint sa nouvelle position.
	 *
	 * Lorsqu'une nouvelle position est demandée avec #AX12_set_position,
	 * cette fonction retournera FALSE tant que le servo n'a pas atteint la nouvelle position.
	 * Si le servo après avoir atteint sa position change de position par une force extérieure,
	 * la valeur retournée par cette fonction sera toujours de TRUE.
	 * Cette fonction n'est effective qu'en mode d'asservissement en position.
	 * @param id_servo numéro du servo à questionner
	 * @return TRUE si le servo à atteint sa nouvelle position, FALSE sinon
	 * @see AX12_set_position
	 */
	bool_e AX12_is_moving(Uint8 id_servo);


	/**
	 * Change le mode de fonctionnement du servo.
	 *
	 * Le servo peut être asservi en position ou en couple.
	 * Si le mode défini est le même que le mode actuel, la fonction ne fait rien et retourne TRUE.
	 * @param id_servo
	 * @param enabled TRUE pour utiliser le mode d'asservissement en couple, ou FALSE pour l'asservissement en position
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_is_wheel_mode_enabled
	 * @see AX12_set_position
	 * @see AX12_set_speed_percentage
	 */
	bool_e AX12_set_wheel_mode_enabled(Uint8 id_servo, bool_e enabled);
#endif

	/**
	 * Défini si la sortie du servo doit être controlée ou non.
	 *
	 * Si la sortie n'est pas controlée, le moteur du servo n'est pas alimenté.
	 * @param id_servo numéro du servo à contrôler
	 * @param enabled FALSE pour laisser libre l'arbre de sortie, sinon TRUE
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_torque_enabled
	 */
	bool_e AX12_set_torque_enabled(Uint8 id_servo, bool_e enabled);

	/**
	 * Allume ou éteint la LED du servo.
	 *
	 * @param id_servo numéro du servo à contrôler
	 * @param enabled TRUE pour l'allumer, sinon FALSE
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_led_enabled
	 */
	bool_e AX12_set_led_enabled(Uint8 id_servo, bool_e enabled);

	/**
	 * Défini les paramètres de réponse de l'asservissement en position.
	 *
	 * Graphique représentant le couple de sortie en fonction de l'erreur de position:
	 *
	 * <pre>\verbatim
	 *         Couple          Consigne de position
	 *              ^                   |
	 *              |=============\     |
	 *              |             ^\    |
	 * Sens anti-   |             | |   v
	 *   horaire  <-+-------------|-+=======+------------------->  Sens horaire
	 *              |             | ^   ^   |             X: Erreur en position
	 *              |        A--->| |<  |    \
	 *              |             | |<->|<->^ \===============
	 *              v             | | B | C | ^
	 *                            | |   |  >| |<---D
	 * \endverbatim
	 * </pre>
	 *
	 * === : Courbe du couple en fonction de l'erreur en position
	 *
	 * @param id_servo numéro du servo à contrôler
	 * @param A représente l'intervale d'erreur dans laquelle le couple est proportionnel à l'erreur angulaire dans le sens anti-horaire. L'intervale d'erreur est [A+B, B]
	 * @param B représente l'intervale d'erreur dans laquelle le couple reste nul dans le sens anti-horaire. L'intervale d'erreur est [B, 0]
	 * @param C représente l'intervale d'erreur dans laquelle le couple reste nul dans le sens horaire. L'intervale d'erreur est [0, C]
	 * @param D représente l'intervale d'erreur dans laquelle le couple est proportionnel à l'erreur angulaire dans le sens horaire. L'intervale d'erreur est [C, C+D]
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même complétement ou partiellement exécutée.
	 * @see AX12_get_torque_response
	 */
	bool_e AX12_set_torque_response(Uint8 id_servo, Uint16 A, Uint16 B, Uint16 C, Uint16 D);

	/**
	 * Change la position du servo.
	 *
	 * Cette fonction n'est effective qu'en mode d'asservissement en position.
	 * L'angle effectif peut avoir 1,3% d'erreur, l'angle défini réel de AX12_config_set_minimal_angle(300) est un angle limite de 296°.
	 * @param id_servo numéro du servo à contrôler
	 * @param degre angle demandé en degrés
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_position
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_set_position(Uint8 id_servo, Uint16 degre);

	/**
	 * Défini la vitesse que le servo doit avoir lors d'un changement de position.
	 *
	 * Ne fait rien si le servo n'est pas en mode asservissement en position.
	 * @param id_servo numéro du servo à contrôler
	 * @param degre_per_sec vitesse en degrés par seconde, maximum: 500
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_move_to_position_speed
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_set_move_to_position_speed(Uint8 id_servo, Uint16 degre_per_sec);

	/**
	 * Défini le pourcentage de couple à appliquer à la sortie pour définir sa vitesse.
	 *
	 * La vitesse effective du servo dépendra de la charge appliquée et de la tension d'alimentation.
	 * Ne fait rien si le servo n'est pas en mode asservissement en couple.
	 * @param id_servo numéro du servo à contrôler
	 * @param percentage pourcentage de couple
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_speed_percentage
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_set_speed_percentage(Uint8 id_servo, Sint8 percentage);

	/**
	 * Défini le couple maximal utilisable par le servo.
	 *
	 * Le couple donné est en pourcentage du couple maximal dépendant de l'alimentation.
	 * S'il y a eu une erreur, selon la configuration, le servo peut se désactiver en mettant 0% comme couple maximal,
	 * ce sera a vous d'examiner la cause de l'erreur et de reactiver le servo en utilisant cette fonction.
	 * La valeur initiale à la mise sous tension est celle retournée par #AX12_config_set_maximum_torque_percentage.
	 * @param id_servo numéro du servo à contrôler
	 * @param percentage pourcentage du couple maximum
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_torque_limit
	 * @see AX12_config_set_error_before_shutdown
	 * @see AX12_config_set_torque_limit
	 */
	bool_e AX12_set_torque_limit(Uint8 id_servo, Uint8 percentage);

	/**
	 * Défini le pourcentage de couple minimal à exercer pour faire bouger le servo.
	 *
	 * Cette valeur doit être dans la plage 4%-100%, sinon la valeur limite correspondante est utilisée.
	 * @param id_servo numéro du servo à contrôler
	 * @param percentage pourcentage de couple
	 * @return TRUE si l'opération s'est bien déroulé, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand à
	 * l'état de l'opération demandée, il ce peut qu'elle soit quand même exécutée.
	 * @see AX12_get_punch_torque_percentage
	 */
	bool_e AX12_set_punch_torque_percentage(Uint8 id_servo, Uint8 percentage);

#endif /* def USE_AX12_SERVO */
#endif /* ndef QS_AX12_H */
