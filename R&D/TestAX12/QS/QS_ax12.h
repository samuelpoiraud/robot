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

#include "QS_all.h"

#define USE_AX12_SERVO

#ifdef USE_AX12_SERVO
	/*
		Pour utiliser ce module vous devez avoir configuré vos AX-12+ de la manière suivante :

			- Baud Rate de 56700 bauds (Address 0x04)
			- Return Delay Time de 200 µs (temps de transfert d'un packet à 57600 bauds) (Address 0x05)
			- Les ID des AX-12+ doivent être différents et se suivre (ID = 0 ... n)

			(Voir la datasheet pour générer les instructions)
	*/
	
	/******************************************/
	/* Paramètres obligatoires ou recommandé  */
	/******************************************/

	//Pour utiliser l'AX12, vous devez définir USE_AX12_SERVO
	//Et indiquer leur nombre en définissant AX12_NUMBER
	//Exemple pour gérer 4 AX12: #define AX12_NUMBER 4

	//Port du PIC dédié à la gestion de l'UART en half-duplex pour indiquer le sens du bus voulu
	//Exemple: #define AX12_DIRECTION_PORT LATBbits.LATBX

	//Vous devez choisir un timer qui sera utilisé pour définir un timeout lors de l'attente du paquet de status envoyé par l'AX12
	//Exemple pour utiliser le timer 1:
	//#define AX12_TIMER_ID 1

	//Le choix du temps avant timeout est fait avec AX12_STATUS_RETURN_TIMEOUT (recommandé, non obligatoire, valeur par défaut: 5ms)
	//Exemple pour une attente de maximum 50ms:
	//#define AX12_STATUS_RETURN_TIMEOUT 50

	//Pour choisir le baudrate utilisé par les AX12 à commander, définissez AX12_UART_BAUDRATE
	//Exemple pour une vitesse de 56700 bauds:
	//#define AX12_UART_BAUDRATE 56700


	/******************************************/
	/* Paramètres optionnels                  */
	/******************************************/

	//Parametre gérant le status renvoyé par AX12_instruction_get_last_status
	//a utiliser de cette façon (si non défini, le mode AX12_STATUS_RETURN_ONREAD sera utilisé)
	//Exemple: #define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ONREAD
	#define AX12_STATUS_RETURN_NEVER	0	//last_status sera le résultat d'un ping ou AX12_ERROR_OK (non recommandé)
	#define AX12_STATUS_RETURN_ONREAD	1	//last_status sera le résultat d'un ping ou d'une lecture
	#define AX12_STATUS_RETURN_ALWAYS	2	//last_status sera le résultat de la dernière instruction


	//Vous pouvez définir le taille du buffer d'instruction total (TOUT les AX12 confondus) (taille de 4*AX12_NUMBER par defaut si non défini)
	//Exemple: #define AX12_INSTRUCTION_BUFFER_SIZE	4


	typedef struct {
		Uint8 error;
		union {
			Uint16 param;
			struct {
				Uint8 param_1;
				Uint8 param_2;
			};
		};
	} AX12_status_t;

	/* API des servomoteurs Dynamixel AX-12+ */

	void AX12_init();

	/*
	 * Ces fonctions sont asynchrones, utiliser AX12_instruction_wait pour attendre la fin de leurs exécutions avant AX12_instruction_get_last_status
	 * Attention, ces fonctions attente que de la place se libère dans le buffer interne, attention lors de l'utilisation des fonctions de l'AX12 dans des interruptions
	 * Pour prévenir le problème, vous pouvez utiliser AX12_instruction_buffer_is_full() ou changer la priorité de votre interruption
	 * L'interruption de l'UART de l'AX12 a pour priorité 5 (pour Tx et Rx) (Si aucun changement n'est fait, l'IT d'un timer a une priorité de 4 comme la majeure partie des ITs)
	 */

	//Vous ne pouvez pas utiliser la fonction "AX12_instruction_read" si le mode de retour d'information est AX12_STATUS_RETURN_NEVER

#if !defined(AX12_STATUS_RETURN_MODE) || (AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER)
	void AX12_instruction_read(Uint8 id_servo, Uint8 address, Uint8 length);
#endif

	void AX12_instruction_write8(Uint8 id_servo, Uint8 address, Uint8 value);
	void AX12_instruction_write16(Uint8 id_servo, Uint8 address, Uint16 value);
	void AX12_instruction_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value);
	void AX12_instruction_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value);
	void AX12_instruction_execute_write(Uint8 id_servo);
	void AX12_instruction_ping(Uint8 id_servo);
	void AX12_instruction_wait();
	bool_e AX12_instruction_buffer_is_full();
	AX12_status_t AX12_instruction_get_last_status(Uint8 id_servo);

	#define AX12_enable_torque_for_all() AX12_instruction_write8(AX12_BROADCAST_ID, AX12_TORQUE_ENABLE, 1)
	#define AX12_disable_torque_for_all() AX12_instruction_write8(AX12_BROADCAST_ID, AX12_TORQUE_ENABLE, 0)

	#define AX12_LED_on(id_servo) AX12_instruction_write8((id_servo), AX12_LED, 1)
	#define AX12_LED_off(id_servo) AX12_instruction_write8((id_servo), AX12_LED, 0)

	/* Action qui peut être retardée afin que les servomoteurs réalisent le déplacement en même temps */
	void AX12_set_position(Uint8 id_servo, Uint16 position, bool_e now);
	#define AX12_get_position(id_servo) (AX12_instruction_read(id_servo, AX12_PRESENT_POSITION_L, 2), AX12_instruction_wait(), AX12_instruction_get_last_status(id_servo).param)


	#define AX12_get_speed(id_servo) (AX12_instruction_read((id_servo), AX12_PRESENT_SPEED_L, 2), AX12_instruction_wait(), AX12_instruction_get_last_status(id_servo).param)
	#define AX12_set_speed(id_servo, speed) AX12_instruction_write16((id_servo), AX12_GOAL_SPEED_L, speed)

	#define AX12_get_load(id_servo) (AX12_instruction_read((id_servo), AX12_PRESENT_LOAD_L, 2), AX12_instruction_wait(), AX12_instruction_get_last_status(id_servo).param)
	#define AX12_get_voltage(id_servo) (AX12_instruction_read((id_servo), AX12_PRESENT_VOLTAGE, 1), AX12_instruction_wait(), AX12_instruction_get_last_status(id_servo).param_1)
	#define AX12_get_temperature(id_servo) (AX12_instruction_read((id_servo), AX12_PRESENT_TEMPERATURE, 1), AX12_instruction_wait(), AX12_instruction_get_last_status(id_servo).param_1)

	/* Control Table Address AX12+ */
	// A utiliser avec AX12_instruction_*

	//Broadcast id
	#define AX12_BROADCAST_ID				0xFE

	//Error bits
	#define AX12_ERROR_OK			0x00	//erreur = 0 <=> pas d'erreur
	#define AX12_ERROR_VOLTAGE		0x01
	#define AX12_ERROR_ANGLE		0x02
	#define AX12_ERROR_OVERHEATING	0x04
	#define AX12_ERROR_RANGE		0x08
	#define AX12_ERROR_CHECKSUM		0x10
	#define AX12_ERROR_OVERLOAD		0x20
	#define AX12_ERROR_INSTRUCTION	0x40
	#define AX12_ERROR_TIMEOUT		0x80	//erreur ajoutée par la QS, le bit 7 n'est pas utilisé par l'AX12

	// EEPROM Area
	#define AX12_MODEL_NUMBER_L 			0x00
	#define AX12_MODOEL_NUMBER_H			0x01
	#define AX12_VERSION					0x02
	#define AX12_ID 						0x03
	#define AX12_BAUD_RATE					0x04
	#define AX12_RETURN_DELAY_TIME			0x05
	#define AX12_CW_ANGLE_LIMIT_L			0x06
	#define AX12_CW_ANGLE_LIMIT_H			0x07
	#define AX12_CCW_ANGLE_LIMIT_L			0x08
	#define AX12_CCW_ANGLE_LIMIT_H			0x09
	#define AX12_SYSTEM_DATA2				0x0A
	#define AX12_LIMIT_TEMPERATURE			0x0B
	#define AX12_DOWN_LIMIT_VOLTAGE 		0x0C
	#define AX12_UP_LIMIT_VOLTAGE			0x0D
	#define AX12_MAX_TORQUE_L				0x0E
	#define AX12_MAX_TORQUE_H				0x0F
	#define AX12_RETURN_LEVEL				0x10
	#define AX12_ALARM_LED					0x11
	#define AX12_ALARM_SHUTDOWN 			0x12
	#define AX12_OPERATING_MODE 			0x13
	#define AX12_DOWN_CALIBRATION_L 		0x14
	#define AX12_DOWN_CALIBRATION_H 		0x15
	#define AX12_UP_CALIBRATION_L			0x16
	#define AX12_UP_CALIBRATION_H			0x17

	// RAM Area
	#define AX12_TORQUE_ENABLE				0x18
	#define AX12_LED						0x19
	#define AX12_CW_COMPLIANCE_MARGIN		0x1A
	#define AX12_CCW_COMPLIANCE_MARGIN		0x1B
	#define AX12_CW_COMPLIANCE_SLOPE		0x1C
	#define AX12_CCW_COMPLIANCE_SLOPE		0x1D
	#define AX12_GOAL_POSITION_L			0x1E
	#define AX12_GOAL_POSITION_H			0x1F
	#define AX12_GOAL_SPEED_L				0x20
	#define AX12_GOAL_SPEED_H				0x21
	#define AX12_TORQUE_LIMIT_L				0x22
	#define AX12_TORQUE_LIMIT_H 			0x23
	#define AX12_PRESENT_POSITION_L 		0x24
	#define AX12_PRESENT_POSITION_H 		0x25
	#define AX12_PRESENT_SPEED_L			0x26
	#define AX12_PRESENT_SPEED_H			0x27
	#define AX12_PRESENT_LOAD_L 			0x28
	#define AX12_PRESENT_LOAD_H 			0x29
	#define AX12_PRESENT_VOLTAGE			0x2A
	#define AX12_PRESENT_TEMPERATURE		0x2B
	#define AX12_REGISTERED_INSTRUCTION		0x2C
	#define AX12_PAUSE_TIME 				0x2D
	#define AX12_MOVING 					0x2E
	#define AX12_LOCK						0x2F
	#define AX12_PUNCH_L					0x30
	#define AX12_PUNCH_H					0x31

#endif /* def USE_AX12_SERVO */
#endif /* ndef QS_AX12_H */
