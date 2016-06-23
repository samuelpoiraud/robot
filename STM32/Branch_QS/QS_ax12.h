/*
 *	Club Robot ESEO 2010 - 2013
 *	???
 *
 *	Fichier : QS_ax12.h
 *	Package : Qualite Software
 *	Description : Module de gestion des servomoteurs Dynamixel AX12/AX12+
 *	Auteur : Ronan, Jacen, modifi� et compl�t� par Alexis (2012-2013)
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20121110
 */

/*
	 Servomoteurs AX12/AX12+
		//! Attention ! Ce module utilise l'UART2 du PIC, il est donc interdit de d�finir USE_UART2 ...
		// utilisation du module d�di� � la gestion des servomoteurs AX12/AX12+
		//Pour utiliser l'AX12, vous devez d�finir USE_AX12_SERVO
	#define USE_AX12_SERVO
		//Et indiquer leur nombre en d�finissant AX12_NUMBER
		//Exemple pour g�rer 4 AX12:
	#define AX12_NUMBER 4
		//Port du PIC d�di� � la gestion de l'UART en half-duplex pour indiquer le sens du bus voulu
		//Exemple pour le pour le pin RG8 (Port G, bit 8)
	#define AX12_DIRECTION_PORT PORTGBITS.RG8
		//Vous devez choisir un timer qui sera utilis� pour d�finir un timeout lors de l'attente du paquet de status envoy� par l'AX12
		//Exemple pour utiliser le timer 1:
	#define AX12_TIMER_ID 1
		//Le choix du temps avant timeout est fait avec AX12_STATUS_RETURN_TIMEOUT (recommand�, non obligatoire, valeur par d�faut: 5ms)
		//Exemple pour une attente de maximum 50ms:
	#define AX12_STATUS_RETURN_TIMEOUT 50
		//Pour choisir le baudrate utilis� par les AX12 � commander, d�finissez AX12_UART_BAUDRATE
		//Exemple pour une vitesse de 56700 bauds:
	#define AX12_UART_BAUDRATE 56700
		//Pour d'autre options de l'AX12, voir QS_ax12.h
*/

#ifndef QS_AX12_H
#define QS_AX12_H


//Les defines ici ne sont que pour la documentation et ne sont pas effectif (grace au #undef)
//Le premier undef permet de ne pas avoir d'erreur sur le define, ceci perd la d�finission de la constante, mais QS_all.h est re-inclu apr�s qui inclu Global_config.h qui red�finira ces constantes

/** Si d�fini, le driver est compil� et utilisable.
 *
 * Pour utiliser ce module vous devez avoir configur� vos AX-12+ de la mani�re suivante :
 * <ul>
 * <li> Baud Rate de 56700 bauds (Address 0x04)
 * <li> Return Delay Time de 200 �s (temps de transfert d'un packet � 57600 bauds) (Address 0x05)
 * <li> Les ID des AX-12+ doivent �tre diff�rents et se suivre (ID = 0 ... n)
 * </ul>
 */
//#ifndef USE_AX12_SERVO
//#define USE_AX12_SERVO
//#undef  USE_AX12_SERVO
//#endif

/** @name Param�tres requis
 * Defines � d�finir obligatoirement dans Global_config pour utiliser ce driver.
 */
///@{
	/** \def  AX12_NUMBER
	 * Nombre d'AX12/RX24 maximum.
	 *
	 * Ce nombre d�fini la taille des buffers interne. Dans les fonctions, id_servo devra �tre strictement inf�rieur � ce nombre.
	 */
//	#ifndef AX12_NUMBER
//	#define AX12_NUMBER
//	#undef  AX12_NUMBER
//	#endif

	/** Port de contr�le de la direction du bus de donn�es.
	 *
	 * Ce port est utilis� pour d�finir l'�tat du bus half-duplex, en recpetion, ce port est mis � l'�tat logique 0,
	 * en transmission il est mis � 1.
	 */
//	#ifndef AX12_DIRECTION_PORT
//	#define AX12_DIRECTION_PORT
//	#undef  AX12_DIRECTION_PORT
//	#endif

	/** Num�ro du timer utilis� pour le timeout.
	 *
	 * Ce timer sera utilis� pour avoir un temps d'attente maximum lorsqu'un paquet de status est cens� �tre enovoy� par l'AX12.
	 */
//	#ifndef AX12_TIMER_ID
//	#define AX12_TIMER_ID
//	#undef  AX12_TIMER_ID
//	#endif
///@}

/** @name Param�tres optionnels
 * Defines � d�finir dans Global_config.
 */
///@{
	/** Vitesse du bus de donn�es.
	 *
	 * Vitesse du port s�rie utilis� pour communiquer avec l'AX12/RX24 en bauds.
	 * L'UART2 est utilis� pour la communication, elle ne doit pas �tre utiliser par un autre module.
	 * Sur le robot de 2012-2013, une vitesse de 56700 bauds est utilis�e, cette vitesse est aussi celle par d�faut.
	 */
//	#ifndef AX12_UART_BAUDRATE
//	#define AX12_UART_BAUDRATE
//	#undef  AX12_UART_BAUDRATE
//	#endif

	/** Temps d'attente du paquet de status avant timeout.
	 *
	 * Ce temps est en milisecondes, si aucun paquet de status n'est re�u apr�s ce temps apr�s l'envoi du paquet instruction, il y a timeout.
	 * Valeur par d�faut: 5ms.
	 * @see AX12_ERROR_TIMEOUT
	 */
//	#ifndef AX12_STATUS_RETURN_TIMEOUT
//	#define AX12_STATUS_RETURN_TIMEOUT
//	#undef  AX12_STATUS_RETURN_TIMEOUT
//	#endif


	/** Politique de retour de paquet status.
	 *
	 * Un paquet status est envoy� par l'AX12/RX24 apr�s un paquet instruction dans certains cas d�fini par ce define.
	 * Le paquet de status informera le driver des erreurs rencontr�es et pourra contenir des donn�es demand�es.
	 * Si #AX12_STATUS_RETURN_NEVER est utilis�, aucun paquet de status ne sera envoy� par le servo,
	 * et aucune des fonctions AX12_get* et AX12_config_get* ne serons utilisable.
	 * Valeurs possibles:
	 * <ul>
	 *  <li> AX12_STATUS_RETURN_NEVER Le servo ne renverra jamais de paquet de status (sauf dans le cas de #AX12_is_ready). La lecture de donn�es n'est possible dans ce cas.
	 *  <li> AX12_STATUS_RETURN_ONREAD Le servo ne reverra des paquet de status seulement lors de lecture de donn�es et pour #AX12_is_ready.
	 *  <li> AX12_STATUS_RETURN_ALWAYS Le servo renverra un paquet de status apr�s chaque instruction envoy� (sauf lorsque l'instruction est envoy� a tous les servo en utilisant #AX12_BROADCAST_ID).
	 * </ul>
	 * @see AX12_config_set_status_return_mode
	 */
//	#ifndef AX12_STATUS_RETURN_MODE
//	#define AX12_STATUS_RETURN_MODE
//	#undef  AX12_STATUS_RETURN_MODE
//	#endif

	/** Taille du buffer interne d'instruction.
	 *
	 * Ce define d�fini la taille du buffer d'instruction, cette taille ne prend pas en compte le nombre d'AX12.
	 * Le buffer interne est commun � tous les AX12.
	 * Par defaut, la taille du buffer est de 4*AX12_NUMBER.
	 */
//	#ifndef AX12_INSTRUCTION_BUFFER_SIZE
//	#define AX12_INSTRUCTION_BUFFER_SIZE
//	#undef  AX12_INSTRUCTION_BUFFER_SIZE
//	#endif


	/** Mode tracage des paquets.
	 *
	 * Si ce define est d�fini, le driver affichera avec debug_printf les paquets envoy�s et re�us.
	 * Attention, debug_printf sera utilis� en interruption, cette fonction n'est pas r�entrante.
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
	 * Le servo ne renverra jamais de paquet de status (sauf dans le cas de #AX12_is_ready). La lecture de donn�es n'est possible dans ce cas.
	 */
	#define AX12_STATUS_RETURN_NEVER 0

	/** Renvois de paquet status lors de r�ponse � une lecture de donn�e.
	 *
	 * Le servo ne reverra des paquet de status seulement lors de lecture de donn�es et pour #AX12_is_ready.
	 */
	#define AX12_STATUS_RETURN_ONREAD 1	//last_status sera le r�sultat d'un ping ou d'une lecture

	/** Toujours renvoyer une paquet de status.
	 *
	 * Le servo renverra un paquet de status apr�s chaque instruction envoy� (sauf lorsque l'instruction est envoy� a tous les servo en utilisant #AX12_BROADCAST_ID).
	 */
	#define AX12_STATUS_RETURN_ALWAYS 2	//last_status sera le r�sultat de la derni�re instruction
///@}


#include "QS_all.h"

#ifdef USE_AX12_SERVO

	/** Structure contenant le dernier status re�u d'un AX12.
	 *
	 * Il y a un status par AX12/RX24 d'enregistr� par le driver,
	 * si le driver re�ois un nouveau paquet de status, seul l'ancien status de cet AX12/RX24 sera �cras�.
	 * @see AX12_get_last_error
	 */
	typedef struct {
		Uint8 last_instruction_address;
		/** Erreur constitu�e des flags AX12_ERROR_*. */
		Uint16 error;
		union {
			/** donn�e eventuellement re�u sur 16bits, cette valeur partage le m�me emplacement m�moire que param_1 et param_2. */
			Uint16 param;
			/** 2 donn�e sur 8bits. */
			struct {
				Uint8 param_1;
				Uint8 param_2;
			};
		};
	} AX12_status_t;

	/** Id broadcast, il permet d'envoyer des commandes � tous les servo connect� avec un seul appel de fonction.
	 *
	 * Aucun paquet de status ne sera retourn� dans ce cas, sauf pour #AX12_is_ready */
	#define AX12_BROADCAST_ID		0xFE

	// Constantes des erreurs possibles
	/** Pas d'erreur.
	 *
	 * Si une variable d'erreur vaut cette constante, alors cette variable indique qu'il n'y a pas d'erreur.*/
	#define AX12_ERROR_OK			0x00

	/** Tension d'alimentation hors des limites.
	 *
	 * Les limites sont d�finies par #AX12_config_set_lowest_voltage et #AX12_config_set_highest_voltage. */
	#define AX12_ERROR_VOLTAGE		0x01

	/** L'angle demand� est hors des limites.
	 *
	 * Les limites des angles sont d�finies par #AX12_config_set_minimal_angle et #AX12_config_set_maximal_angle */
	#define AX12_ERROR_ANGLE		0x02

	/** La temp�rature interne du servo est trop �lev�e.
	 *
	 * Si cette erreur est lev�e, c'est probablement que le moteur force trop.
	 * @see AX12_config_get_temperature_limit
	 */
	#define AX12_ERROR_OVERHEATING	0x04

	/** Plage de valeur non respect�e.
	 *
	 * Cette erreur indique qu'un param�tre pass� � une instruction �tait hors de la plage acceptable. Par exemple un pourcentage de 300. */
	#define AX12_ERROR_RANGE		0x08

	/** Checksum du paquet instruction invalide.
	 *
	 * Le driver n'a pas envoy� un paquet instruction valide.
	 * Cette erreur est caus� par le driver et non par l'utilisateur. */
	#define AX12_ERROR_CHECKSUM		0x10

	/** Charge trop �lev�e.
	 *
	 * La charge que subit le servo est trop forte, il ne peut pas garder la position demand�e avec le couple maximum d�fini.
	 * @see AX12_set_torque_limit
	 */
	#define AX12_ERROR_OVERLOAD		0x20

	/** Instruction invalide.
	 *
	 * L'instruction envoy� au servo n'existe pas ou une demande d'ex�cution de l'action pr�par�e a �t� faite alors qu'aucune action n'a �t� pr�par�e.
	 * Cette erreur est caus� par le driver et non par l'utilisateur. */
	#define AX12_ERROR_INSTRUCTION	0x40

	/** Paquet status attendu non re�u.
	 *
	 * Un paquet de status �tait cens� �tre envoy� par le servo, mais le driver n'a rien re�u durant le temps imparti.
	 * Utilisez #AX12_is_ready pour savoir si le servo est connect� et r�pond correctement.
	 * Si erreur est d�finie avec AX12_ERROR_RANGE en m�me temps, c'est que le driver � attendu trop longtemps pour bufferiser la commande et l'a annul�.
	 * Cette erreur est toujours seule autrement (pas d'autre bit d�fini que celui l�)
	 * @see AX12_STATUS_RETURN_TIMEOUT
	 * @see AX12_STATUS_RETURN_MODE
	 */
	#define AX12_ERROR_TIMEOUT		0x80

	/** Des instructions sont en cours de traitement pour l'AX12/RX24 consid�r�
	 *
	 * Une ou plusieurs commande on �t� demand�es de mani�re asynchrone et le r�sultat renvoy� par l'AX12/RX24 n'a pas encore �t� re�u.
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
	 * Lors d'appel multiple, seul le premier appel est effectif, les autres sont ignor�s.
	 * Lors de l'initialisation, le mode de retour est d�fini � #AX12_STATUS_RETURN_MODE et les angles limites sont d�fini � 0�-300�.
	 * Le servo est initialement en mode aservissement de position.
	 */
	void AX12_init();

	/**
	 * Retourne la derni�re information de status envoy� par l'AX12.
	 *
	 * L'information s'y trouvant d�pend de la derni�re fonction utilis�e
	 * ansi que du define #AX12_STATUS_RETURN_MODE.
	 * @param id_servo num�ro du servo � verifier
	 * @return dernier status du servo indiqu�
	 * @see AX12_reset_last_error
	 */
	AX12_status_t AX12_get_last_error(Uint8 id_servo);

	/**
	 * R�initialise la derni�re information de status � #AX12_ERROR_OK.
	 *
	 * Cette fonction ne devrait pas servir, mais aucune v�rification du code concernant la remise � zero si une instruction s'est bien execut�e dans tous les cas n'a �t� faite
	 * @param id_servo num�ro du servo dont il faut reinitialiser le status
	 * @see AX12_get_last_error
	 */
	void AX12_reset_last_error(Uint8 id_servo);

	/**
	 * Permet de savoir si le buffer d'instruction interne du driver est plein.
	 *
	 * S'il est plein et qu'une fonction appel�e n�cessite l'envoi d'un paquet a l'AX12,
	 * elle renverra FALSE si le buffer est rest� plein trop longtemps.
	 * @return TRUE si le buffer est plein, sinon FALSE
	 */
	bool_e AX12_is_buffer_full();

	/**
	 * Envois un ping au servo pour connaitre son �tat.
	 *
	 * Les informations sur son �tat sont accessible avec #AX12_get_last_error.
	 * @param id_servo num�ro du servo � verifier
	 * @return TRUE si le servo r�pond au ping, sinon FALSE
	 */
	bool_e AX12_is_ready(Uint8 id_servo);

	bool_e AX12_async_is_ready(Uint8 id_servo, bool_e *isReady); //version asynchrone, resultat � tester avec AX12_get_last_error. Return FALSE que si la file d'action du driver ax12 est pleine (cas grave, buffer trop petit ou flood de demande � l'ax12/rx24 ?)

	/**
	 * Met le driver en mode pr�paration de commandes.
	 *
	 * Apr�s l'appel � cette fonction, les commmandes ne sont pas ex�cut� directement par le servo.
	 * Les commandes sont ex�cut� en m�me temps apr�s l'appel � #AX12_end_instruction_block.
	 * Les bloc de commande permettent une synchronisation de plusieurs servo lors de l'ex�cution de commande
	 * comme par exemple le mouvement simultan� de plusieurs servos.
	 * Une seule commande peut �tre pr�par�e par un servo.
	 * Seule les commandes d'�criture AX12_set* seront pr�par�es. Les autres fonctionneront comme avant l'appel � cette fonction.
	 * @see AX12_end_command_block
	 */
	void AX12_start_command_block();

	/**
	 * Termine le bloc de pr�paration de commande et les ex�cute.
	 *
	 * Les commandes pr�c�dement pr�par�e sont ex�cut�e en m�me temps.
	 * @see AX12_start_command_block
	 */
	void AX12_end_command_block();


	//Configuration de l'AX12, perdure apr�s mise hors tension sauf pour le verouillage de la config (lock).
	//Unit�s:
	// Angles en degr�s
	// Vitesse en degr� par seconde
	// Voltage en dixi�me de volt (50 => 5.0V)
	// Temp�rature en degr� celsius
	// Pourcentage entre 0 et 100

#if !defined(AX12_STATUS_RETURN_MODE) || (AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER)

	/**
	 * Verifie si la configuration est prot�g�e.
	 *
	 * @param id_servo num�ro du servo � verifier
	 * @return TRUE si la configuration est verrouill�e, sinon FALSE
	 * @see AX12_config_lock
	 */
	bool_e AX12_config_is_locked(Uint8 id_servo);

	/**
	 * Retourne le num�ro de mod�le d'un servo.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return Num�ro de mod�le
	 * @see AX12_config_get_firmware_version
	 */
	Uint16 AX12_config_get_model_number(Uint8 id_servo);

	/**
	 * Retourne la version du firmware d'un servo.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return Version du firmware
	 * @see AX12_config_get_model_number
	 */
	Uint8  AX12_config_get_firmware_version(Uint8 id_servo);

	/**
	 * Retourne le d�lai de r�ponse du servo.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return Temps de d�lai en microsecondes
	 * @see AX12_config_set_return_delay_time
	 */
	Uint16 AX12_config_get_return_delay_time(Uint8 id_servo);

	/**
	 * Retourne l'angle minimal autoris� (mode asservissement en position seulement).
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return Angle minimal sur 300� de 0 � 1023
	 * @see AX12_config_set_minimal_angle
	 */
	Uint16 AX12_config_get_minimal_angle(Uint8 id_servo);

	/**
	 * Retourne l'angle maximal autoris� (mode asservissement en position seulement).
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return Angle maximal sur 300� de 0 � 1023
	 * @see AX12_config_set_maximal_angle
	 */
	Uint16 AX12_config_get_maximal_angle(Uint8 id_servo);

	/**
	 * Retourne la temp�rature maximale de fonctionnement.
	 *
	 * Si le servo d�passe cette temp�rature, le bit d'erreur #AX12_ERROR_OVERHEATING
	 * est lev� et suivant la configuration, la LED peut �tre allum�e et/ou le servo d�sactiv�.
	 * Il n'est pas conseill� de modifier cette valeur (par d�faut: 80�C).
	 * @param id_servo num�ro du servo � questionner
	 * @return Limite de temp�rature en degr�s celsius
	 * @see AX12_config_set_temperature_limit
	 */
	Uint8  AX12_config_get_temperature_limit(Uint8 id_servo);

	/**
	 * Retourne la limite inf�rieure d'alimentation.
	 *
	 * Si la tension d'alimentation est en dessous de cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera lev�e. Suivant la configuration, la LED peut �tre allum�e et/ou le servo d�sactiv�.
	 * @param id_servo num�ro du servo � questionner
	 * @return Tension minimale de fonctionnement en dixi�me de Volt (une valeur de 84 indique un tension de 8.4V)
	 * @see AX12_config_set_lowest_voltage
	 */
	Uint8  AX12_config_get_lowest_voltage(Uint8 id_servo);

	/**
	 * Retourne la limite sup�rieure d'alimentation.
	 *
	 * Si la tension d'alimentation est d�passe cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera lev�e. Suivant la configuration, la LED peut �tre allum�e et/ou le servo d�sactiv�.
	 * @param id_servo num�ro du servo � questionner
	 * @return Tension maximale de fonctionnement en dixi�me de Volt (une valeur de 114 indique un tension de 11.4V)
	 * @see AX12_config_set_highest_voltage
	 */
	Uint8  AX12_config_get_highest_voltage(Uint8 id_servo);

	/**
	 * Retourne la puissance maximale autoris�e par defaut.
	 *
	 * La puissance est un pourcentage de la puissance maximale que permet l'alimentation.
	 * @param id_servo num�ro du servo � questionner
	 * @return Puissance maximale en pourcentage (entre 0 et 100)
	 * @see AX12_config_set_maximum_torque
	 * @see AX12_get_torque_limit
	 */
	Uint8  AX12_config_get_maximum_torque(Uint8 id_servo);

	/**
	 * Retourne le mode de retour d'information du servo.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return Mode de retour, peut �tre une des valeurs ce dessous:
	 * <ul>
	 *  <li> #AX12_STATUS_RETURN_ONREAD Le servo ne renverra un paquet de status lors de lecture de donn�e et pour #AX12_is_ready
	 *  <li> #AX12_STATUS_RETURN_ALWAYS Le servo renverra un paquet a tout envoi de paquet d'instruction, sauf si l'ID #AX12_BROADCAST_ID est utilis�
	 * </ul>
	 * @see #AX12_STATUS_RETURN_MODE
	 * @see AX12_config_set_status_return_mode
	 */
	Uint8  AX12_config_get_status_return_mode(Uint8 id_servo);

	/**
	 * Retourne les erreurs qui allumerons la LED.
	 *
	 * Les erreurs sont les flags AX12_ERROR*, utilisez (flags & AX12_ERROR_???) != 0 pour savoir si l'erreur AX12_ERROR_??? est lev�e.
	 * @param id_servo num�ro du servo � questionner
	 * @return Masque d'erreur constitu� de AX12_ERROR*
	 * @see AX12_config_set_error_before_led
	 * @see AX12_config_get_error_before_shutdown
	 * @see AX12_set_led_enabled
	 */
	Uint8  AX12_config_get_error_before_led(Uint8 id_servo);

	/**
	 * Retourne les erreurs qui d�sactiverons le servo.
	 *
	 * Lorsque qu'une erreur est lev�e et que son bit corespondant est activ� dans le masque donn� par cette fonction
	 * le servo se d�sactive en mettant 0% comme couple maximal, utilisez #AX12_set_torque_limit pour autoriser un couple de sortie non nul.
	 * Les erreurs sont les flags AX12_ERROR*, utilisez (flags & AX12_ERROR_???) != 0 pour savoir si l'erreur AX12_ERROR_??? est lev�e.
	 * @param id_servo num�ro du servo � questionner
	 * @return Masque d'erreur constitu� de AX12_ERROR*
	 * @see AX12_config_set_error_before_shutdown
	 * @see AX12_config_get_error_before_led
	 * @see AX12_set_torque_limit
	 */
	Uint8  AX12_config_get_error_before_shutdown(Uint8 id_servo);
#endif

	/**
	 * Verrouille la configuration du servo.
	 *
	 * Apr�s cet appel, il n'est plus possible d'utiliser les fonctions AX12_config_set* ansi que #AX12_set_punch_torque_percentage
	 * Le rallumage du servo est le seul moyen de le d�verrouiller, lors de la mise sous tension, le servo est toujours d�verrouill�.
	 * @param id_servo num�ro du servo � verrouiller
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_is_locked
	 */
	bool_e AX12_config_lock(Uint8 id_servo);

	/**
	 * Change l'ID d'un servo AX12/RX24 branch� (� utiliser temporairement, de pr�f�rence avec un seul servo de branch� sur le bus pour �viter les ambigu�t� !)
	 *
	 * @param id_servo_current	id du servo � changer
	 * @param id_servo_new		id du servo une fois chang�
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 */
	bool_e AX12_config_set_id(Uint8 id_servo_current, Uint8 id_servo_new);

	/**
	 * D�fini le d�lai de r�ponse que doit avoir le servo.
	 *
	 * Utilisez cette fonction si le driver n'a pas le temps de se placer en mode reception pour recevoir le paquet de status.
	 * @param id_servo num�ro du servo � controler
	 * @param delay_us d�lai en microsecondes
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_set_return_delay_time
	 */
	bool_e AX12_config_set_return_delay_time(Uint8 id_servo, Uint16 delay_us);

	/**
	 * D�fini l'angle minimal autoris� (mode asservissement en position seulement).
	 *
	 * L'angle doit �tre entre 0 et 300� soit 0 � 1023. Si la valeur degre est en dehors de ces limites, la valeur prise en compte sera la limite correspondante.
	 * @param id_servo num�ro du servo � r�gler
	 * @param degre angle sur 300� de 0 � 1023
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_minimal_angle
	 */
	bool_e AX12_config_set_minimal_angle(Uint8 id_servo, Uint16 degre);

	/**
	 * D�fini l'angle maximal autoris� (mode asservissement en position seulement).
	 *
	 * @param id_servo num�ro du servo � r�gler
	 * @param degre angle sur 300� de 0 � 1023
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_maximal_angle
	 */
	bool_e AX12_config_set_maximal_angle(Uint8 id_servo, Uint16 degre);

	/**
	 * D�fini la temp�rature maximale avant de lever l'erreur #AX12_ERROR_OVERHEATING.
	 *
	 * @param id_servo num�ro du servo � r�gler
	 * @param temperature temp�rature en degr� celsius
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_temperature_limit
	 */
	bool_e AX12_config_set_temperature_limit(Uint8 id_servo, Uint8 temperature);

	/**
	 * D�fini la tension minimale de fonctionnement.
	 *
	 * Si la tension d'alimentation est en dessous de cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera lev�e. Suivant la configuration, la LED peut �tre allum�e et/ou le servo d�sactiv�.
	 * @param id_servo num�ro du servo � r�gler
	 * @param voltage tension en dixi�me de Volts (Une valeur de 76 correspond � 7.6V)
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_lowest_voltage
	 */
	bool_e AX12_config_set_lowest_voltage(Uint8 id_servo, Uint8 voltage);

	/**
	 * D�fini la tension maximale de fonctionnement.
	 *
	 * Si la tension d'alimentation d�passe cette valeur,
	 * l'erreur #AX12_ERROR_VOLTAGE sera lev�e. Suivant la configuration, la LED peut �tre allum�e et/ou le servo d�sactiv�.
	 * @param id_servo num�ro du servo � r�gler
	 * @param voltage tension en dixi�me de Volts (Une valeur de 76 correspond � 7.6V)
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_highest_voltage
	 */
	bool_e AX12_config_set_highest_voltage(Uint8 id_servo, Uint8 voltage);

	/**
	 * D�fini la valeur initiale du couple maximum utilisable par le servo.
	 *
	 * La puissance est un pourcentage de la puissance maximale que permet l'alimentation.
	 * @param id_servo num�ro du servo � r�gler
	 * @param percentage Pourcentage de couple utilisable (entre 0 et 100)
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_maximum_torque_percentage
	 */
	bool_e AX12_config_set_maximum_torque_percentage(Uint8 id_servo, Uint8 percentage);

	/**
	 * D�fini le mode de retour de paquet de status.
	 *
	 * Les paquets de status permette de recevoir des informations d'erreur et des donn�es du servo.
	 * ATTENTION ! Cette fonction ne devrait pas �tre utilis�e, changez la valeur de #AX12_STATUS_RETURN_MODE � la place.
	 * Cette fonction ne change pas le fonctionnement interne du driver vis-�-vis des retours d'informations.
	 * @param id_servo num�ro du servo � r�gler
	 * @param mode mode de fonctionnement, peut �tre une des valeurs suivantes:
	 * <ul>
	 *  <li> #AX12_STATUS_RETURN_NEVER Le servo ne renverra jamais de paquet de status (sauf dans le cas de #AX12_is_ready). La lecture de donn�es n'est possible dans ce cas.
	 *  <li> #AX12_STATUS_RETURN_ONREAD Le servo ne reverra des paquet de status seulement lors de lecture de donn�es et pour #AX12_is_ready.
	 *  <li> #AX12_STATUS_RETURN_ALWAYS Le servo renverra un paquet de status apr�s chaque instruction envoy� (sauf lorsque l'instruction est envoy� a tous les servo en utilisant #AX12_BROADCAST_ID).
	 * </ul>
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_status_return_mode
	 */
	bool_e AX12_config_set_status_return_mode(Uint8 id_servo, Uint8 mode);

	/**
	 * D�fini les erreurs qui allumerons la LED.
	 *
	 * Les erreurs sont les flags AX12_ERROR*, utilisez l'operateur OU binaire (a | b par exemple) pour d�finir plusieurs erreurs
	 * qui devrons allumer la LED.
	 * @param id_servo num�ro du servo � r�gler
	 * @param error_type Masque d'erreurs constitu� de AX12_ERROR*
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_config_get_error_before_led
	 * @see AX12_config_set_error_before_shutdown
	 * @see AX12_set_led_enabled
	 */
	bool_e AX12_config_set_error_before_led(Uint8 id_servo, Uint8 error_type);

	/**
	 * D�fini les erreurs qui d�sactiverons le servo.
	 *
	 * Les erreurs sont les flags AX12_ERROR*, utilisez l'operateur OU binaire (a | b par exemple) pour d�finir plusieurs erreurs.
	 * Lorsque qu'une erreur est lev�e et que son bit corespondant est activ� dans le masque donn� par cette fonction le servo
	 * se d�sactive en mettant 0% comme couple maximal, utilisez #AX12_set_torque_limit pour autoriser un couple de sortie
	 * non nul.
	 * @param id_servo num�ro du servo � r�gler
	 * @param error_type Masque d'erreurs constitu� de AX12_ERROR*
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
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
	 * @param id_servo num�ro du servo � questionner
	 * @return <ul>
	 *  <li> TRUE si le servo est asservi en couple, l'arbre moteur n'a pas de limite d'angle et tourne en continu.
	 *  <li> FALSE si le servo est asservi en position (angle). Les limites d'angle sont d�finies avec #AX12_config_set_minimal_angle et #AX12_config_set_maximal_angle
	 * </ul>
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_is_wheel_mode_enabled(Uint8 id_servo);

#if !defined(AX12_STATUS_RETURN_MODE) || (AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER)
	/**
	 * V�rifie la sortie du servo est control�e ou non.
	 *
	 * Si la sortie n'est pas control�e, le moteur du servo n'est pas aliment�.
	 * @param id_servo num�ro du servo � questionner
	 * @return TRUE si la sortie du servo est control�e en position ou couple, FLASE si la sortie est libre
	 * @see AX12_set_torque_enabled
	 */
	bool_e AX12_is_torque_enabled(Uint8 id_servo);

	/**
	 * V�rifie l'�tat de la LED.
	 *
	 * Cette LED est allum�e soit par l'utilisateur, soit par le servo lui-m�me lorsque une erreur est lev�e
	 * selon la configuration de la LED avec #AX12_config_set_error_before_led.
	 * @param id_servo num�ro du servo � questionner
	 * @return TRUE si la LED est allum�e, sinon FALSE
	 * @see AX12_set_led_enabled
	 * @see AX12_config_set_error_before_led
	 */
	bool_e AX12_is_led_enabled(Uint8 id_servo);

	/**
	 * Retourne les param�tres de la r�ponse de l'asservissement en position.
	 *
	 * Graphique repr�sentant le couple de sortie en fonction de l'erreur de position:
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
	 * @param id_servo num�ro du servo � questionner
	 * @param A repr�sente l'intervale d'erreur dans laquelle le couple est proportionnel � l'erreur angulaire dans le sens anti-horaire. L'intervale d'erreur est [A+B, B]
	 * @param B repr�sente l'intervale d'erreur dans laquelle le couple reste nul dans le sens anti-horaire. L'intervale d'erreur est [B, 0]
	 * @param C repr�sente l'intervale d'erreur dans laquelle le couple reste nul dans le sens horaire. L'intervale d'erreur est [0, C]
	 * @param D repr�sente l'intervale d'erreur dans laquelle le couple est proportionnel � l'erreur angulaire dans le sens horaire. L'intervale d'erreur est [C, C+D]
	 * @return TRUE si la commande s'est bien ex�cut�e et que les param�tres A, B, C et D ont une valeur valide, FALSE sinon
	 * @see AX12_set_torque_response
	 */
	bool_e AX12_get_torque_response(Uint8 id_servo, Uint16* A, Uint16* B, Uint16* C, Uint16* D);

	/**
	 * Retourne la position actuelle du servo.
	 *
	 * Position des angles et vue de devant:
	 * <pre>\verbatim
	 *              | 511(150�)
	 *              |
	 *              +
	 *             / \
	 *  1023(300�)/   \ 0(0�)
	 * \endverbatim
	 * </pre>
	 * @param id_servo num�ro du servo � questionner
	 * @return position actuelle sur 300� de 0 � 1023
	 * @see AX12_set_position
	 */
	Uint16 AX12_get_position(Uint8 id_servo);

	/**
	 * Retourne la vitesse que le servo doit avoir lors d'un changement de position.
	 *
	 * Cette valeur n'est utilis� qu'en mode d'asservissement en position.
	 * Position des angles et vue de devant:
	 * <pre>\verbatim
	 *         |    | 511
	 *    Sens |    |        |
	 * positif |    +        | Sens n�gatif
	 *         v   / \       |
	 *       1023 /   \ 0    v
	 * \endverbatim
	 * </pre>
	 * @param id_servo num�ro du servo � questionner
	 * @return pourcentage de couple par rapport au couple maximal possible d�pendant de l'alimentation
	 * @see AX12_set_move_to_position_speed
	 * @see AX12_set_wheel_mode_enabled
	 */
	Sint8 AX12_get_move_to_position_speed(Uint8 id_servo);

	/**
	 * Retourne le pourcentage de couple appliqu�e a la sortie du servo en mode asservissement en couple.
	 *
	 * Cette valeur n'est utilis� qu'en mode d'asservissement de couple.
	 * @param id_servo num�ro du servo � questionner
	 * @return pourcentage de couple par rapport au couple maximal possible d�pendant de l'alimentation
	 * @see AX12_set_speed_percentage
	 * @see AX12_set_wheel_mode_enabled
	 */
	Sint8  AX12_get_speed_percentage(Uint8 id_servo);

	/**
	 * Retourne le couple maximal utilisable par le servo.
	 *
	 * Le couple donn� est en pourcentage du couple maximal d�pendant de l'alimentation.
	 * S'il y a eu une erreur, selon la configuration, le servo peut se d�sactiver en mettant 0% comme couple maximal.
	 * @param id_servo num�ro du servo � questionner
	 * @return pourcentage de couple maximum
	 * @see AX12_set_torque_limit
	 * @see AX12_config_set_error_before_shutdown
	 */
	Uint8  AX12_get_torque_limit(Uint8 id_servo);

	/**
	 * Retourne le pourcentage de couple minimal � exercer pour faire bouger le servo.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return pourcentage de couple
	 * @see AX12_set_punch_torque_percentage
	 */
	Uint8  AX12_get_punch_torque_percentage(Uint8 id_servo);

	/**
	 * Retourne une estimation du couple r�sistant.
	 *
	 * Cette valeur est d�duite du couple apliqu� en sortie, et ne devrais pas �tre utilis�e
	 * pour calculer d'autre information sur la charge tel que son poids, mais seulement pour savoir dans quel sens la charge pousse.
	 * @param id_servo num�ro du servo � questionner
	 * @return pourcentage du couple resistant par rapport au couple maximal
	 */
	Sint8  AX12_get_load_percentage(Uint8 id_servo);

	/**
	 * Retourne la tension d'alimentation.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return tension en dixi�me de Volts (Une valeur 87 correspond � 8.7V)
	 * @see AX12_config_set_lowest_voltage
	 * @see AX12_config_set_highest_voltage
	 */
	Uint8  AX12_get_voltage(Uint8 id_servo);

	/**
	 * Retourne la temp�rature interne du servo.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return temp�rature en degr� celsius
	 * @see AX12_config_set_temperature_limit
	 */
	Uint8  AX12_get_temperature(Uint8 id_servo);

	/**
	 * V�rifie si une instruction a �t� pr�par�e et est pr�te � �tre ex�cut�.
	 *
	 * @param id_servo num�ro du servo � questionner
	 * @return TRUE si une instruction est pr�te � �tre ex�cut�, sinon FALSE
	 */
	bool_e AX12_is_instruction_prepared(Uint8 id_servo);

	/**
	 * V�rifie si le servo � atteint sa nouvelle position.
	 *
	 * Lorsqu'une nouvelle position est demand�e avec #AX12_set_position,
	 * cette fonction retournera FALSE tant que le servo n'a pas atteint la nouvelle position.
	 * Si le servo apr�s avoir atteint sa position change de position par une force ext�rieure,
	 * la valeur retourn�e par cette fonction sera toujours de TRUE.
	 * Cette fonction n'est effective qu'en mode d'asservissement en position.
	 * @param id_servo num�ro du servo � questionner
	 * @return TRUE si le servo � atteint sa nouvelle position, FALSE sinon
	 * @see AX12_set_position
	 */
	bool_e AX12_is_moving(Uint8 id_servo);


	/**
	 * Change le mode de fonctionnement du servo.
	 *
	 * Le servo peut �tre asservi en position ou en couple.
	 * Si le mode d�fini est le m�me que le mode actuel, la fonction ne fait rien et retourne TRUE.
	 * @param id_servo
	 * @param enabled TRUE pour utiliser le mode d'asservissement en couple, ou FALSE pour l'asservissement en position
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_is_wheel_mode_enabled
	 * @see AX12_set_position
	 * @see AX12_set_speed_percentage
	 */
	bool_e AX12_set_wheel_mode_enabled(Uint8 id_servo, bool_e enabled);
#endif

	/**
	 * D�fini si la sortie du servo doit �tre control�e ou non.
	 *
	 * Si la sortie n'est pas control�e, le moteur du servo n'est pas aliment�.
	 * @param id_servo num�ro du servo � contr�ler
	 * @param enabled FALSE pour laisser libre l'arbre de sortie, sinon TRUE
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_torque_enabled
	 */
	bool_e AX12_set_torque_enabled(Uint8 id_servo, bool_e enabled);

	/**
	 * Allume ou �teint la LED du servo.
	 *
	 * @param id_servo num�ro du servo � contr�ler
	 * @param enabled TRUE pour l'allumer, sinon FALSE
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_led_enabled
	 */
	bool_e AX12_set_led_enabled(Uint8 id_servo, bool_e enabled);

	/**
	 * D�fini les param�tres de r�ponse de l'asservissement en position.
	 *
	 * Graphique repr�sentant le couple de sortie en fonction de l'erreur de position:
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
	 * @param id_servo num�ro du servo � contr�ler
	 * @param A repr�sente l'intervale d'erreur dans laquelle le couple est proportionnel � l'erreur angulaire dans le sens anti-horaire. L'intervale d'erreur est [A+B, B]
	 * @param B repr�sente l'intervale d'erreur dans laquelle le couple reste nul dans le sens anti-horaire. L'intervale d'erreur est [B, 0]
	 * @param C repr�sente l'intervale d'erreur dans laquelle le couple reste nul dans le sens horaire. L'intervale d'erreur est [0, C]
	 * @param D repr�sente l'intervale d'erreur dans laquelle le couple est proportionnel � l'erreur angulaire dans le sens horaire. L'intervale d'erreur est [C, C+D]
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il ce peut qu'elle soit quand m�me compl�tement ou partiellement ex�cut�e.
	 * @see AX12_get_torque_response
	 */
	bool_e AX12_set_torque_response(Uint8 id_servo, Uint16 A, Uint16 B, Uint16 C, Uint16 D);

	/**
	 * Change la position du servo.
	 *
	 * Cette fonction n'est effective qu'en mode d'asservissement en position.
	 * @param id_servo num�ro du servo � contr�ler
	 * @param degre angle demand� sur 300� de 0 � 1023
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il ce peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_position
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_set_position(Uint8 id_servo, Uint16 angle);

	/**
	 * D�fini la vitesse que le servo doit avoir lors d'un changement de position.
	 *
	 * Ne fait rien si le servo n'est pas en mode asservissement en position.
	 * @param id_servo num�ro du servo � contr�ler
	 * @param percentage pourcentage de vitesse
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il ce peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_move_to_position_speed
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_set_move_to_position_speed(Uint8 id_servo, Uint8 percentage);

	/**
	 * D�fini le pourcentage de couple � appliquer � la sortie pour d�finir sa vitesse.
	 *
	 * La vitesse effective du servo d�pendra de la charge appliqu�e et de la tension d'alimentation.
	 * Ne fait rien si le servo n'est pas en mode asservissement en couple.
	 * @param id_servo num�ro du servo � contr�ler
	 * @param percentage pourcentage de couple
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il ce peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_speed_percentage
	 * @see AX12_set_wheel_mode_enabled
	 */
	bool_e AX12_set_speed_percentage(Uint8 id_servo, Sint8 percentage);

	/**
	 * D�fini le couple maximal utilisable par le servo.
	 *
	 * Le couple donn� est en pourcentage du couple maximal d�pendant de l'alimentation.
	 * S'il y a eu une erreur, selon la configuration, le servo peut se d�sactiver en mettant 0% comme couple maximal,
	 * ce sera a vous d'examiner la cause de l'erreur et de reactiver le servo en utilisant cette fonction.
	 * La valeur initiale � la mise sous tension est celle retourn�e par #AX12_config_set_maximum_torque_percentage.
	 * @param id_servo num�ro du servo � contr�ler
	 * @param percentage pourcentage du couple maximum
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il se peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_torque_limit
	 * @see AX12_config_set_error_before_shutdown
	 * @see AX12_config_set_torque_limit
	 */
	bool_e AX12_set_torque_limit(Uint8 id_servo, Uint8 percentage);

	/**
	 * D�fini le pourcentage de couple minimal � exercer pour faire bouger le servo.
	 *
	 * Cette valeur doit �tre dans la plage 4%-100%, sinon la valeur limite correspondante est utilis�e.
	 * @param id_servo num�ro du servo � contr�ler
	 * @param percentage pourcentage de couple
	 * @return TRUE si l'op�ration s'est bien d�roul�, sinon FALSE.
	 * Si la fonction retourne FALSE, aucune conclusion n'est possible quand �
	 * l'�tat de l'op�ration demand�e, il ce peut qu'elle soit quand m�me ex�cut�e.
	 * @see AX12_get_punch_torque_percentage
	 */
	bool_e AX12_set_punch_torque_percentage(Uint8 id_servo, Uint8 percentage);


	#define AX12_BEFORE_SHUTDOWN	(AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD)
	#define AX12_BEFORE_LED			(AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE | AX12_ERROR_VOLTAGE)
	#define AX12_MIN_VOLTAGE		70
	#define AX12_MAX_VOLTAGE		146


#endif /* def USE_AX12_SERVO */
#endif /* ndef QS_AX12_H */
