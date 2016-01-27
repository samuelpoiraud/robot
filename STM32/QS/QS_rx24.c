/*
 *	Club Robot ESEO 2010 - 2013
 *	???
 *
 *	Fichier : QS_rx24.h
 *	Package : Qualite Software
 *	Description : Module de gestion des servomoteurs Dynamixel RX24/RX24+
 *	Auteur : Ronan, Jacen, modifié et complété par Alexis (2012-2013)
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20121110
 */


#include "QS_rx24.h"

#ifdef USE_RX24_SERVO

#include "QS_ports.h"
#include "QS_outputlog.h"
#include "stm32f4xx_usart.h"

#include "../config/config_pin.h"


	/**********************************************************************/
	/** Paramètres de configuration du driver RX24/RX24                        */
	/**********************************************************************/

	#if !defined(RX24_NUMBER) || RX24_NUMBER > 254
		#error "Vous devez definir un nombre de servo sur le robot (RX24_NUMBER) (254 max)"
	#endif /* ndef RX24_NUMBER */

	//#define RX24_DEBUG_PACKETS: si défini, les paquets envoyés sont écrits sur la sortie uart en utilisant debug_printf (attention, debug_printf n'est pas réentrante)

//	#ifndef RX24_UART_ID
//		#define RX24_UART_ID 2
//	#endif



	#if RX24_UART_ID == 1
		#define RX24_UART_Ptr USART1
		#define RX24_UART_Interrupt USART1_IRQHandler
		#define RX24_UART_Interrupt_IRQn USART1_IRQn
	#elif RX24_UART_ID == 2
		#define RX24_UART_Ptr USART2
		#define RX24_UART_Interrupt USART2_IRQHandler
		#define RX24_UART_Interrupt_IRQn USART2_IRQn
	#elif RX24_UART_ID == 3
		#define RX24_UART_Ptr USART3
		#define RX24_UART_Interrupt USART3_IRQHandler
		#define RX24_UART_Interrupt_IRQn USART3_IRQn
	#else
	#warning "RX24: Unknown UART ID"
	#endif

	#ifndef RX24_UART_BAUDRATE
		#define RX24_UART_BAUDRATE  56700
	#endif

	#ifndef RX24_STATUS_RETURN_TIMEOUT
		#define RX24_STATUS_RETURN_TIMEOUT 50	//Ne pas mettre une valeur trop proche de celle prévue, le RX24 répond parfois avec un delai plus long
	#endif

	#ifndef RX24_STATUS_SEND_TIMEOUT
		#define RX24_STATUS_SEND_TIMEOUT 10
	#endif

	#ifndef RX24_STATUS_RETURN_MODE
		#define RX24_STATUS_RETURN_MODE RX24_STATUS_RETURN_ONREAD
	#endif

	#ifndef RX24_INSTRUCTION_BUFFER_SIZE
		#define RX24_INSTRUCTION_BUFFER_SIZE (5*RX24_NUMBER)
	#endif

	/**********************************************************************/
	/** Configuration du timer pour le timeout de réception du status    **/
	/**********************************************************************/

	#ifdef RX24_TIMER_ID
		#define TIMER_SRC_TIMER_ID RX24_TIMER_ID
	#endif
	#ifdef RX24_TIMER_USE_WATCHDOG
		#define TIMER_SRC_USE_WATCHDOG
	#endif

	#include "QS_setTimerSource.h"

	/*************************************************************************************/

	/* UART2 exclusivement dédié à la gestion des servomoteurs AX-12+ */

	#ifdef USE_UART2
		#error "Vous ne pouvez pas utiliser l'UART2 et l'RX24 en même temps, l'RX24 à besoin de l'UART2 pour communiquer"
	#endif

	#if (!defined(RX24_DIRECTION_PORT_RX24))
		#error "Vous devez definir un port de direction (RX24_DIRECTION_PORT_RX24) pour gérer l'UART qui est en Half-duplex (cf. Datasheet MAX485)"
	#endif /* ndef RX24_DIRECTION_PORT */

	/* Pour UART half-uplex */
	#define TX_DIRECTION 1
	#define RX_DIRECTION 0


	/*************************************************************************************/
	/** Constantes liées au instruction envoyées a l'RX24/RX24                               **/
	/*************************************************************************************/

	/* Instructions */

	#define INST_PING 				0x01
	#define INST_READ 				0x02
	#define INST_WRITE 				0x03
	#define INST_REG_WRITE 			0x04
	#define INST_ACTION 			0x05
	//Instructions non utilisées
	//#define INST_RESET 			0x06
	//#define INST_DIGITAL_RESET 	0x07
	//#define INST_SYSTEM_READ 		0x0C
	//#define INST_SYSTEM_WRITE 	0x0D
	//#define INST_SYNC_WRITE 		0x83
	//#define INST_SYNC_REG_WRITE 	0x84

	// Adresses de la mémoire interne de l'RX24
	#define RX24_MEM_SIZE		0x32

	#define RX24_MIN_VALUE_0	0x00
	#define RX24_MIN_VALUE_1	0x01
	#define RX24_MIN_VALUE_34	0x02
	#define RX24_MIN_VALUE_50	0x03

	#define RX24_MAX_VALUE_1	0x70
	#define RX24_MAX_VALUE_2	0x60
	#define RX24_MAX_VALUE_127	0x50
	#define RX24_MAX_VALUE_150	0x40
	#define RX24_MAX_VALUE_250	0x30
	#define RX24_MAX_VALUE_253	0x20
	#define RX24_MAX_VALUE_254	0x10
	#define RX24_MAX_VALUE_1023	0x00

	//Les defines sont globaux, il sont défini en même temps que le tableau des valeurs accepté par l'RX24/RX24 pour pouvoir voir à quelle valeur les limites correspondent
	static const Uint8 RX24_values_ranges[0x32] = {
		//EEPROM, utilisé par les fonctions RX24_config_*, les valeurs persiste après une mise hors tension.
		#define RX24_MODEL_NUMBER_L 			0x00
			0,	//non utilisé
		#define RX24_MODEL_NUMBER_H				0x01
			0,	//non utilisé
		#define RX24_VERSION					0x02
			0,	//non utilisé
		#define RX24_ID 						0x03
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_253,
		#define RX24_BAUD_RATE					0x04
			RX24_MIN_VALUE_34 | RX24_MAX_VALUE_254,	//la limite de l'RX24/RX24 est 0-254, mais la carte ne supporte pas des vitesses supérieures à 56700bauds.
		#define RX24_RETURN_DELAY_TIME			0x05
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//Maximum de 508 us
		#define RX24_CW_ANGLE_LIMIT_L			0x06
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_CW_ANGLE_LIMIT_H			0x07
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define RX24_CCW_ANGLE_LIMIT_L			0x08
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_CCW_ANGLE_LIMIT_H			0x09
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define RX24_SYSTEM_DATA2				0x0A
			0,	//non utilisé
		#define RX24_LIMIT_TEMPERATURE			0x0B
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_150,
		#define RX24_DOWN_LIMIT_VOLTAGE 		0x0C
			RX24_MIN_VALUE_50 | RX24_MAX_VALUE_250,
		#define RX24_UP_LIMIT_VOLTAGE			0x0D
			RX24_MIN_VALUE_50 | RX24_MAX_VALUE_250,
		#define RX24_MAX_TORQUE_L				0x0E
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_MAX_TORQUE_H				0x0F
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define RX24_RETURN_LEVEL				0x10
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_2,
		#define RX24_ALARM_LED					0x11
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_127,
		#define RX24_ALARM_SHUTDOWN 			0x12
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_127,
		#define RX24_OPERATING_MODE 			0x13
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1,
		#define RX24_DOWN_CALIBRATION_L 		0x14
			0,	//non utilisé
		#define RX24_DOWN_CALIBRATION_H 		0x15
			0,	//non utilisé
		#define RX24_UP_CALIBRATION_L			0x16
			0,	//non utilisé
		#define RX24_UP_CALIBRATION_H			0x17
			0,	//non utilisé

		//RAM de l'RX24, reinitialisé après une mise hors tension
		#define RX24_TORQUE_ENABLE				0x18
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1,
		#define RX24_LED						0x19
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1,
		#define RX24_CW_COMPLIANCE_MARGIN		0x1A
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,
		#define RX24_CCW_COMPLIANCE_MARGIN		0x1B
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,
		#define RX24_CW_COMPLIANCE_SLOPE		0x1C
			RX24_MIN_VALUE_1 | RX24_MAX_VALUE_254,
		#define RX24_CCW_COMPLIANCE_SLOPE		0x1D
			RX24_MIN_VALUE_1 | RX24_MAX_VALUE_254,
		#define RX24_GOAL_POSITION_L			0x1E
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_GOAL_POSITION_H			0x1F
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define RX24_GOAL_SPEED_L				0x20
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_GOAL_SPEED_H				0x21
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define RX24_TORQUE_LIMIT_L				0x22
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_TORQUE_LIMIT_H 			0x23
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define RX24_PRESENT_POSITION_L 		0x24
			0,	//non utilisé
		#define RX24_PRESENT_POSITION_H 		0x25
			0,	//non utilisé
		#define RX24_PRESENT_SPEED_L			0x26
			0,	//non utilisé
		#define RX24_PRESENT_SPEED_H			0x27
			0,	//non utilisé
		#define RX24_PRESENT_LOAD_L 			0x28
			0,	//non utilisé
		#define RX24_PRESENT_LOAD_H 			0x29
			0,	//non utilisé
		#define RX24_PRESENT_VOLTAGE			0x2A
			0,	//non utilisé
		#define RX24_PRESENT_TEMPERATURE		0x2B
			0,	//non utilisé
		#define RX24_REGISTERED_INSTRUCTION		0x2C
			0,	//non utilisé
		#define RX24_PAUSE_TIME 				0x2D
			0,	//non utilisé
		#define RX24_MOVING 					0x2E
			0,	//non utilisé
		#define RX24_LOCK						0x2F
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1,
		#define RX24_PUNCH_L					0x30
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_1023,
		#define RX24_PUNCH_H					0x31
			RX24_MIN_VALUE_0 | RX24_MAX_VALUE_254	//ne devrait pas être utilisé
	};

	//Adresse spéciale pour le ping
	#define RX24_PING 0xFF

	/***********************************************************************/
	/** Autre définitions internes                                        **/
	/***********************************************************************/

	//Si défini, on teste le checksum des paquets de status, si le resultat est incorrect, le paquet est ignoré
	//#define RX24_STATUS_RETURN_CHECK_CHECKSUM

	//nous avons besoin d'une place en plus pour pouvoir différencier le cas ou le buffer est vide du cas ou il est plein
	#define RX24_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE (RX24_INSTRUCTION_BUFFER_SIZE+1)

	#define MIN_INSTRUCTION_PACKET_SIZE 6
	#define MAX_STATUS_PACKET_SIZE 8

	//Macro pour simplifier l'incrementation d'index (la fonction modulo fait une division assez lourde, donc profitons de super-macro
	//le modulo (%) c'est bien, mais c'est aussi beaucoup d'opération pour les cas utilisés (les valeurs ne doivent jamais dépasser 2*div)
	#define INC_WITH_MOD(val,div) (( (val)+1 < div )? (val)+1 : 0)

	/***********************************************************************/

typedef struct{
	volatile RX24_status_t last_status;
	Uint16 angle_limit[2];	//sauvegarde des angles limites lors d'un passage en mode wheel (rotation complète en continue) Si l'RX24/RX24 était déja dans ce mode, les angles limites défini à 0°-300°.
	bool_e is_wheel_enabled;
	// Possibilité d'ajout de membres ...
} RX24_servo_t;

//Paquet instruction a envoyer a l'RX24
typedef struct{
	Uint8 id_servo;
	Uint8 type;
	Uint8 address;
	union {
		Uint16 param;
		struct {
			Uint8 param_1;	//poids fort
			Uint8 param_2;	//poids faible
		};
	};
	Uint8 size;
} RX24_instruction_packet_t;


//Paquet renvoyé par l'RX24, 2 paramètres 8bits maximum
typedef struct{
	Uint8 id_servo;
	Uint16 error;  // l'RX24/RX24 n'utilise que 8 bits
	union {
		Uint16 param;
		struct {
			Uint8 param_1;	//poids fort
			Uint8 param_2;	//poids faible
		};
	};
	Uint8 size;
} RX24_status_packet_t;

typedef struct{
	RX24_instruction_packet_t buffer[RX24_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE];
	Uint8 start_index;
	Uint8 end_index;
} RX24_instruction_buffer;


//Machine à états

// Raison d'appel de la fonction de la machine à états
typedef enum {
	RX24_SME_NoEvent,
	RX24_SME_RxInterrupt,
	RX24_SME_TxInterruptRX24,
	RX24_SME_Timeout
} RX24_state_machine_event_e;

// Etat de la machine à états
typedef enum {
	RX24_SMS_ReadyToSend = 0,
	RX24_SMS_Sending,
	RX24_SMS_WaitingAnswer
} RX24_state_machine_state_e;


typedef struct {
	volatile RX24_state_machine_state_e state;
	volatile Uint16 timeout_counter;	//compteur décrémenté par le timer lorsque que l'état est RX24_SMS_WaitingAnswer

	RX24_instruction_packet_t current_instruction;

	Uint8 receive_index;

	//Utilisé en mode SEND

#ifdef RX24_UART_Ptr
	Uint8 rx24_sending_index;
#endif
} RX24_state_machine_t;

/*************************************************************************************/

// Variables globales, toutes les variables sont intialisée a zero, il n'y a pas de {0} ou autre pour eviter les warnings (avec -Wall) et c'est plus simple et pratique de rien mettre que des {{{0}},{0,0},0} et pour eviter d'avoir des warnings car les warnings c'est mal (défini dans l'ainsi C)

static RX24_servo_t RX24_on_the_robot[RX24_NUMBER];	//Tableau de structure contenant les informations liées à chaque RX24
static RX24_state_machine_t state_machine = {0};	//Machine à états du driver

//Queue contenant les instructions demandée (ce buffer est géré pas les fonctions/macro RX24_instruction_*, il n'y a qu'un buffer pour tous les RX24)
static volatile RX24_instruction_buffer RX24_special_instruction_buffer;
//Cette variable est à TRUE si le driver est en mode préparation de commandes. Voir doc de RX24_start_command_block dans le .h
static bool_e RX24_prepare_commands = FALSE;

/**************************************************************************/
/** Fonctions internes au driver de l'RX24+ et macros                    **/
/**************************************************************************/

static inline Uint16 RX24_decode_value_range(Uint8 mem_addr, bool_e get_max);	//get_max = 1 pour récupérer la valeur max pour une adresse donnée, get_max = 0 pour récupérer le minimum
static Uint8 RX24_instruction_packet_calc_checksum(RX24_instruction_packet_t* instruction_packet);
static Uint8 RX24_get_instruction_packet(Uint8 byte_offset, RX24_instruction_packet_t* instruction_packet);				//Renvoi l'octet d'une instruction a envoyer
#ifdef RX24_STATUS_RETURN_CHECK_CHECKSUM
static Uint8 RX24_status_packet_calc_checksum(RX24_status_packet_t* status_packet);
#endif
static bool_e RX24_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, RX24_status_packet_t* status_packet);	//retourne FALSE si le paquet est non valide, sinon TRUE
static void RX24_state_machine(RX24_state_machine_event_e event);
static void RX24_UART_init_all(Uint32 uart_speed);
static void RX24_UART_init(USART_TypeDef* uartPtr, Uint16 baudrate);
static void RX24_UART_DisableIRQ();
static void RX24_UART_EnableIRQ();
static void RX24_UART_putc(Uint8 c);
static Uint8 RX24_UART_getc();
static bool_e RX24_UART_GetFlagStatus(Uint16 flag);
static void RX24_UART_ITConfig(Uint16 flag, FunctionalState enable);

/**************************************************************************/
/** Fonctions/macro gérant les instructions et le buffer d'instruction   **/
/**************************************************************************/

/*
 * Les fonctions RX24_instruction_async_* sont asynchrones, utiliser RX24_instruction_wait pour attendre la fin de leurs exécutions avant RX24_instruction_get_last_status
 * Attention, ces fonctions attente que de la place se libère dans le buffer interne, attention lors de l'utilisation des fonctions de l'RX24 dans des interruptions
 * Pour prévenir le problème, vous pouvez utiliser RX24_instruction_buffer_is_full() ou changer la priorité de votre interruption
 * L'interruption de l'UART de l'RX24 a pour priorité 5 (pour Tx et Rx) (Si aucun changement n'est fait, l'IT d'un timer a une priorité de 4 comme la majeure partie des ITs)
 */

//Vous ne pouvez pas utiliser la fonction "RX24_instruction_*read" si le mode de retour d'information est RX24_STATUS_RETURN_NEVER

#if RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER
	static bool_e RX24_instruction_async_read(Uint8 id_servo, Uint8 address, Uint8 length);
	static Uint16 RX24_instruction_read16(Uint8 id_servo, Uint8 address, bool_e *isOk);
	static Uint8  RX24_instruction_read8(Uint8 id_servo, Uint8 address, bool_e *isOk);
#endif

static bool_e RX24_instruction_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e RX24_instruction_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e RX24_instruction_async_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e RX24_instruction_async_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e RX24_instruction_async_ping(Uint8 id_servo);
static bool_e RX24_instruction_async_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e RX24_instruction_async_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e RX24_instruction_async_execute_write();
static bool_e RX24_instruction_ping(Uint8 id_servo);
static bool_e RX24_instruction_wait(Uint8 id_servo);
static bool_e RX24_instruction_buffer_is_full();
static void RX24_instruction_next(Uint16 error, Uint16 param);
static RX24_status_t RX24_instruction_get_last_status(Uint8 id_servo);
static void RX24_instruction_reset_last_status(Uint8 id_servo);

static bool_e RX24_instruction_queue_insert(const RX24_instruction_packet_t* inst);
#define RX24_status_packet_is_full(status_packet, bytes_received) (((bytes_received) >= 4) && ((bytes_received) >= status_packet.size))	//on doit avoir lu la taille du paquet (>= 4) et avoir lu tous les octets du paquet
#define RX24_instruction_queue_is_full() (INC_WITH_MOD(RX24_special_instruction_buffer.end_index, RX24_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE) == RX24_special_instruction_buffer.start_index)	//buffer plein si le prochain index de fin est l'index de début du buffer
#define RX24_instruction_queue_is_empty() (RX24_special_instruction_buffer.end_index == RX24_special_instruction_buffer.start_index)
#define RX24_instruction_queue_next() \
	(RX24_special_instruction_buffer.start_index = INC_WITH_MOD(RX24_special_instruction_buffer.start_index, RX24_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE))
#define RX24_instruction_queue_get_current() \
	RX24_special_instruction_buffer.buffer[RX24_special_instruction_buffer.start_index]


//retourne TRUE si l'instruction est suivie d'une réponse de l'RX24
//inst est de type RX24_instruction_packet_t
#define RX24_instruction_has_status_packet(inst) (inst.id_servo != RX24_BROADCAST_ID && (RX24_STATUS_RETURN_MODE == RX24_STATUS_RETURN_ALWAYS || inst.type == INST_PING || (RX24_STATUS_RETURN_MODE == RX24_STATUS_RETURN_ONREAD && inst.type == INST_READ)))

/*************************************************************************************/
/** Fonctions gérant l'envoi de paquet instruction par l'intermédiaire du buffer    **/
/*************************************************************************************/

#if RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER
static bool_e RX24_instruction_async_read(Uint8 id_servo, Uint8 address, Uint8 length) {
	RX24_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_READ;
	inst.address = address;
	inst.param_1 = length;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	return RX24_instruction_queue_insert(&inst);
}
#endif

static bool_e RX24_instruction_async_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	RX24_instruction_packet_t inst = {0};

	if((Uint16)value > RX24_decode_value_range(address, TRUE))
		value = RX24_decode_value_range(address, TRUE);

	if((Uint16)value < RX24_decode_value_range(address, FALSE))
		value = RX24_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_WRITE;
	inst.address = address;
	inst.param_1 = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	return RX24_instruction_queue_insert(&inst);
}

static bool_e RX24_instruction_async_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	RX24_instruction_packet_t inst = {0};

	if(value > RX24_decode_value_range(address, TRUE))
		value = RX24_decode_value_range(address, TRUE);

	if(value < RX24_decode_value_range(address, FALSE))
		value = RX24_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_WRITE;
	inst.address = address;
	inst.param = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 3;	//param is 16bits

	return RX24_instruction_queue_insert(&inst);
}

static bool_e RX24_instruction_async_ping(Uint8 id_servo) {
	RX24_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_PING;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	return RX24_instruction_queue_insert(&inst);
}

static bool_e RX24_instruction_async_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	RX24_instruction_packet_t inst = {0};

	if((Uint16)value > RX24_decode_value_range(address, TRUE))
		value = RX24_decode_value_range(address, TRUE);

	if((Uint16)value < RX24_decode_value_range(address, FALSE))
		value = RX24_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_REG_WRITE;
	inst.address = address;
	inst.param_1 = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	return RX24_instruction_queue_insert(&inst);
}

static bool_e RX24_instruction_async_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	RX24_instruction_packet_t inst = {0};

	if(value > RX24_decode_value_range(address, TRUE))
		value = RX24_decode_value_range(address, TRUE);

	if(value < RX24_decode_value_range(address, FALSE))
		value = RX24_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_REG_WRITE;
	inst.address = address;
	inst.param = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 3;	//param is 16bits

	return RX24_instruction_queue_insert(&inst);
}

static bool_e RX24_instruction_async_execute_write() {
	RX24_instruction_packet_t inst = {0};

	inst.id_servo = RX24_BROADCAST_ID;
	inst.type = INST_ACTION;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	return RX24_instruction_queue_insert(&inst);
}

#if RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER
static Uint8 RX24_instruction_read8(Uint8 id_servo, Uint8 address, bool_e *isOk) {
	if(!RX24_instruction_async_read(id_servo, address, 1)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(!RX24_instruction_wait(id_servo)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(isOk) *isOk = TRUE;
	return RX24_instruction_get_last_status(id_servo).param_1;
}

static Uint16 RX24_instruction_read16(Uint8 id_servo, Uint8 address, bool_e *isOk) {
	if(!RX24_instruction_async_read(id_servo, address, 2)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(!RX24_instruction_wait(id_servo)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(isOk) *isOk = TRUE;
	return RX24_instruction_get_last_status(id_servo).param;
}
#endif

static bool_e RX24_instruction_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	if(RX24_prepare_commands) {
		if(!RX24_instruction_async_prepare_write8(id_servo, address, value))
			return FALSE;
	} else {
		if(!RX24_instruction_async_write8(id_servo, address, value))
			return FALSE;
	}

	#if RX24_STATUS_RETURN_MODE == RX24_STATUS_RETURN_ALWAYS
		if(id_servo == RX24_BROADCAST_ID)
			return TRUE;
		if(!RX24_instruction_wait(id_servo))
			return FALSE;
		return RX24_instruction_get_last_status(id_servo).error == RX24_ERROR_OK;
	#endif

	return TRUE;
}

static bool_e RX24_instruction_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	if(RX24_prepare_commands) {
		if(!RX24_instruction_async_prepare_write16(id_servo, address, value))
			return FALSE;
	} else {
		if(!RX24_instruction_async_write16(id_servo, address, value))
			return FALSE;
	}

	#if RX24_STATUS_RETURN_MODE == RX24_STATUS_RETURN_ALWAYS
		if(id_servo == RX24_BROADCAST_ID)
			return TRUE;
		if(!RX24_instruction_wait(id_servo))
			return FALSE;
		return RX24_instruction_get_last_status(id_servo).error == RX24_ERROR_OK;
	#endif

	return TRUE;
}

static bool_e RX24_instruction_ping(Uint8 id_servo) {
	RX24_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_PING;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	if(!RX24_instruction_queue_insert(&inst))
		return FALSE;

	if(!RX24_instruction_wait(id_servo))
		return FALSE;

	return (RX24_instruction_get_last_status(id_servo).error & RX24_ERROR_TIMEOUT) == 0;
}

static bool_e RX24_instruction_buffer_is_full() {
	return RX24_instruction_queue_is_full();
}

static void RX24_instruction_next(Uint16 error, Uint16 param) {
	RX24_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = error;
	RX24_on_the_robot[state_machine.current_instruction.id_servo].last_status.param = param;
	if(state_machine.current_instruction.type == INST_PING)
		RX24_on_the_robot[state_machine.current_instruction.id_servo].last_status.last_instruction_address = RX24_PING;
	else
		RX24_on_the_robot[state_machine.current_instruction.id_servo].last_status.last_instruction_address = state_machine.current_instruction.address;
	RX24_instruction_queue_next();
}

static bool_e RX24_instruction_wait(Uint8 id_servo) {
	Uint32 i = 0;
	time32_t local_t = global.absolute_time;
	//debug_printf("+1\n");
	while(!RX24_instruction_queue_is_empty() && global.absolute_time - local_t < 7)	//si on a attendu 7 ms, on stop, on a attendu trop longtemps (au moins 6,5ms, mais ce bout de code ne fait qu'une instruction)
		i++;

	if(global.absolute_time - local_t >= 7) return TRUE;

	debug_printf("RX24 Wait too long %d / %d\n", RX24_special_instruction_buffer.start_index, RX24_special_instruction_buffer.end_index);
	RX24_on_the_robot[id_servo].last_status.error = RX24_ERROR_TIMEOUT | RX24_ERROR_RANGE;	//On a attendu trop longtemps, le buffer est toujours plein
	RX24_on_the_robot[id_servo].last_status.param = 0;
	return FALSE;
}

static RX24_status_t RX24_instruction_get_last_status(Uint8 id_servo)
{
	RX24_status_t status;
	RX24_UART_DisableIRQ();
	TIMER_SRC_TIMER_DisableIT();
		status = RX24_on_the_robot[id_servo].last_status;
	TIMER_SRC_TIMER_EnableIT();
	RX24_UART_EnableIRQ();

	return status;
}

static void RX24_instruction_reset_last_status(Uint8 id_servo) {
	RX24_UART_DisableIRQ();
	TIMER_SRC_TIMER_DisableIT();
		RX24_on_the_robot[id_servo].last_status.last_instruction_address = 0;
		RX24_on_the_robot[id_servo].last_status.error = RX24_ERROR_OK;
		RX24_on_the_robot[id_servo].last_status.param = 0;
	TIMER_SRC_TIMER_EnableIT();
	RX24_UART_EnableIRQ();
}

/*****************************************************************************/
/** Fonctions internes pour la gestion des paquets et les limites de valeur **/
/*****************************************************************************/

static inline Uint16 RX24_decode_value_range(Uint8 mem_addr, bool_e get_max) {
	if(mem_addr >= RX24_MEM_SIZE) return 0;

	if(!get_max) {
		switch(RX24_values_ranges[mem_addr] & 0x0F) {
			case RX24_MIN_VALUE_0:  return 0;
			case RX24_MIN_VALUE_1:  return 1;
			case RX24_MIN_VALUE_34: return 34;
			case RX24_MIN_VALUE_50: return 50;
		}
	} else {
		switch(RX24_values_ranges[mem_addr] & 0xF0) {
			case RX24_MAX_VALUE_1023: return 1023;
			case RX24_MAX_VALUE_254:  return 254;
			case RX24_MAX_VALUE_253:  return 253;
			case RX24_MAX_VALUE_250:  return 250;
			case RX24_MAX_VALUE_150:  return 150;
			case RX24_MAX_VALUE_127:  return 127;
			case RX24_MAX_VALUE_2:    return 2;
			case RX24_MAX_VALUE_1:    return 1;
		}
	}

	return 0;
}

static Uint8 RX24_instruction_packet_calc_checksum(RX24_instruction_packet_t* instruction_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = instruction_packet->size - 4;

	if(instruction_packet->size < 6)
		debug_printf("RX24: Error: in RX24_instruction_packet_calc_checksum(): paquet trop petit\n");

	//packet_length est forcément >= 2 (voir datasheet de l'RX24)
	//PAS de break, si il y a 3 paramètres (packet_length = 3 +2), on fait toutes les additions
	switch(packet_length) {
		case 5:	//3 paramètres
			checksum += instruction_packet->param_2;
			//no break;
		case 4:	//2 paramètres
			checksum += instruction_packet->param_1;
			//no break;
		case 3:	//1 paramètre
			checksum += instruction_packet->address;
			//no break;
		case 2:	//pas de paramètre
			checksum += instruction_packet->id_servo + packet_length + instruction_packet->type;
			break;

		default:
			debug_printf("RX24: Error: RX24_instruction_packet_calc_checksum(): paquet trop grand\n");
			break;
	}

	return ~checksum;
}

static Uint8 RX24_get_instruction_packet(Uint8 byte_offset, RX24_instruction_packet_t* instruction_packet) {

	if(byte_offset == instruction_packet->size - 1)	//si c'est le dernier octet, envoyer le checksum
		return RX24_instruction_packet_calc_checksum(instruction_packet);

	switch(byte_offset)
	{
		case 0:	//PAS de break, on retourne 0xFF pour les octets numéro 0 et 1
		case 1: return 0xFF;
		case 2: return instruction_packet->id_servo;
		case 3: return instruction_packet->size - 4;
		case 4: return instruction_packet->type;
		case 5:	return instruction_packet->address;
		case 6: return instruction_packet->param_1;
		case 7: return instruction_packet->param_2;

		default:
			if(byte_offset > 7)
				debug_printf("RX24: Warning: byte_offset > 7\n");
	}

	//Si on atteint cette ligne, byte_offset > 7, on retourne 0 par défaut (ça ne devrais jamais arriver)
	return 0;
}

#ifdef RX24_STATUS_RETURN_CHECK_CHECKSUM
static Uint8 RX24_status_packet_calc_checksum(RX24_status_packet_t* status_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = status_packet->size - 4;

	if(status_packet->size < 6)
		debug_printf("RX24: Error: in RX24_status_packet_calc_checksum(): paquet trop petit\n");

	//packet_length est forcément >= 2 (voir datasheet de l'RX24)
	//PAS de break, si il y a 2 paramètre, on fait toutes les additions
	switch(packet_length) {
		case 4:	//2 paramètres
			checksum += status_packet->param_2;
			//no break;
		case 3:	//1 paramètre
			checksum += status_packet->param_1;
			//no break;
		case 2:	//pas de paramètre
			checksum += status_packet->id_servo + packet_length + (status_packet->error & 0xFF);
			break;

		default:
			debug_printf("RX24: Error: RX24_status_packet_calc_checksum(): paquet trop grand\n");
	}

	return ~checksum;
}
#endif

static bool_e RX24_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, RX24_status_packet_t* status_packet)
{
	//Si c'est le dernier octet, verifier le checksum si RX24_STATUS_RETURN_CHECK_CHECKSUM est defini
	if(byte_offset > 3 && byte_offset == status_packet->size - 1) {
	#ifdef RX24_STATUS_RETURN_CHECK_CHECKSUM
		if(receive_byte != RX24_status_packet_calc_checksum(status_packet)) {
			debug_printf("RX24[%d]: invalid checksum\n", status_packet->id_servo);
			return FALSE;
		} else return TRUE;
	#else
		return TRUE;
	#endif
	}

	//mise a jour de la structure instruction avec les octets reçus
	switch(byte_offset)
	{
		case 0:
			//Initialisation de a structure a des valeurs par défaut
			status_packet->id_servo = 0;
			status_packet->size = 0;
			//status_packet->error = RX24_ERROR_OK; contient IN_PROGRESS déjà
			status_packet->param = 0;

			if(receive_byte != 0xFF)
				return FALSE;
			break;

		case 1:
			if(receive_byte != 0xFF)
				return FALSE;
			break;

		case 2:
			status_packet->id_servo = receive_byte;
			break;

		case 3:
			status_packet->size = receive_byte + 4;
			break;

		case 4:
			//pour être sur de ne pas avoir le bit 7 a 1, si l'RX24/RX24 le met a 1, on met tous les bits a 1
			status_packet->error = receive_byte & 0x7F; //On enlève le IN_PROGRESS en passant
			break;

		case 5:
			status_packet->param_1 = receive_byte;
			break;

		case 6:
			status_packet->param_2 = receive_byte;
			break;

		default:
			if(byte_offset > 6)
				debug_printf("RX24: Error: RX24_update_status_packet(): trop d'octet reçu > 6\n");
	}

	return TRUE;
}

//La machine a états est appelée par les interruptions de l'UART et par le timer
static void RX24_state_machine(RX24_state_machine_event_e event) {
	static volatile int processing_state = 0;

	#if defined(VERBOSE_MODE) && defined(RX24_DEBUG_PACKETS)
		//Pour le deboggage (avec debug_printf à la fin de l'envoi/reception d'un paquet)
		static Uint8 RX24_UART2_reception_buffer[MAX_STATUS_PACKET_SIZE*2] = {0};
		Uint8 i, pos;
	#endif

//sécurisation des appels à cette fonction lorsque event == RX24_SME_NoEvent pour eviter que l'état RX24_SMS_ReadyToSend soit exécuté plusieurs fois
//si une IT arrive a un certain moment, elle sera executée complètement, donc processing_state reviendra à son état d'origine
	if(event == RX24_SME_NoEvent) {
//1) Si une IT arrive a ce moment, elle s'execute entièrement et change l'état de RX24_SMS_ReadyToSend en RX24_SMS_Sending, cet appel continuera et le switch branchera sur RX24_SMS_Sending, il n'y a rien de prévu pour RX24_SME_NoEvent donc cet appel ne fera rien au final
		if(processing_state > 0)
//2) Si une IT arrive a ce moment, elle s'executera pas, elle retournera sans rien faire car processing_state est > 0
			return;
//3) Si une IT arrive a ce moment, elle s'executera entièrement car processing_state == 0, l'état passera de RX24_SMS_ReadyToSend à RX24_SMS_Sending et cet appel de fonction ne fera rien comme au 1)
		processing_state = 1;
//4) Si une IT arrive a ce moment, elle ne s'executera pas car processing_state > 0
	} else if(event == RX24_SME_Timeout) {
		RX24_UART_DisableIRQ();
	} else if(event == RX24_SME_RxInterrupt || event == RX24_SME_TxInterruptRX24) {
		TIMER_SRC_TIMER_DisableIT();
	}

//Si une IT arrive avec event == RX24_SME_TxInterrupt, elle n'aura un effet que dans l'état RX24_SMS_Sending (et cette interruption ne devrait être lancée que après le putcUART2 dans RX24_SMS_ReadyToSend et pas dans RX24_SMS_Sending car une interruption n'est pas prioritaire sur elle-même)
//Si une IT arrive avec event == RX24_SME_RxInterrupt, elle n'aura un effet que dans l'état RX24_SMS_WaitingAnswer. Elle ne peut être lancée que si le timer n'a pas eu d'interruption et désactive l'interruption du timer

	switch(state_machine.state)
	{
		case RX24_SMS_ReadyToSend:
			if(event == RX24_SME_NoEvent)
			{
				// Choix du paquet à envoyer et début d'envoi
				if(!RX24_instruction_queue_is_empty())
					state_machine.current_instruction = RX24_instruction_queue_get_current();
				else {	//s'il n'y a rien a faire, mettre en veille la machine a état, l'UART sera donc inactif (et mettre en mode reception pour ne pas forcer la sortie dont on défini la tension, celle non relié a l'RX24)
					while(!RX24_UART_GetFlagStatus(USART_FLAG_TC));   //inifinite loop si uart pas initialisé

					GPIO_WriteBit(RX24_DIRECTION_PORT_RX24, RX_DIRECTION);
					break;
				}

				state_machine.state = RX24_SMS_Sending;
				RX24_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = RX24_ERROR_IN_PROGRESS;

#ifdef RX24_UART_Ptr
				state_machine.rx24_sending_index = 0;
#endif
				state_machine.receive_index = 0;


				#if defined(VERBOSE_MODE) && defined(RX24_DEBUG_PACKETS)
					debug_printf("RX24 Tx:");
					for(i = 0; i<state_machine.current_instruction.size; i++)
						debug_printf(" %02x", RX24_get_instruction_packet(i, &state_machine.current_instruction));
					debug_printf("\n");
				#endif


				GPIO_WriteBit(RX24_DIRECTION_PORT_RX24, TX_DIRECTION);

				TIMER_SRC_TIMER_start_ms(RX24_STATUS_SEND_TIMEOUT);	//Pour le timeout d'envoi, ne devrait pas arriver

#ifdef RX24_UART_Ptr
				state_machine.rx24_sending_index++;
#endif

				RX24_UART_putc(RX24_get_instruction_packet(0, &state_machine.current_instruction));
				RX24_UART_ITConfig(USART_IT_TXE, ENABLE);
			} /*else if(event == RX24_SME_TxInterrupt) {
				RX24_UART_ITConfig(USART_IT_TXE, DISABLE);
			}*/
		break;

		case RX24_SMS_Sending:
			if(event == RX24_SME_TxInterruptRX24)
			{
#ifdef RX24_UART_Ptr
				// rx24
				if(state_machine.rx24_sending_index < state_machine.current_instruction.size) {
					USART_SendData(RX24_UART_Ptr, RX24_get_instruction_packet(state_machine.rx24_sending_index, &state_machine.current_instruction));
					state_machine.rx24_sending_index++;
				} else
					USART_ITConfig(RX24_UART_Ptr, USART_IT_TXE, DISABLE);
#endif

//				if(state_machine.sending_index < state_machine.current_instruction.size) {
//					RX24_UART_putc(RX24_get_instruction_packet(state_machine.sending_index, &state_machine.current_instruction));
//					state_machine.sending_index++;
//				}


			if(

		#ifdef RX24_UART_Ptr
					state_machine.rx24_sending_index >= state_machine.current_instruction.size	//Le dernier paquet a été envoyé, passage en mode reception et attente de la réponse dans l'état RX24_SMS_WaitingAnswer s'il y a ou enchainement sur le prochain paquet à evnoyer (RX24_SMS_ReadyToSend)
		#endif
			)
			{
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();

				if(RX24_instruction_has_status_packet(state_machine.current_instruction))
				{
					#if defined(VERBOSE_MODE) && defined(RX24_DEBUG_PACKETS)
						for(i=0; i<MAX_STATUS_PACKET_SIZE*2; i++)
							RX24_UART2_reception_buffer[i] = 0;
						pos = 0;
					#endif

						//Attente de la fin de la transmition des octets
						//TRMT passe a 1 quand tout est envoyé (bit de stop inclu)
						//plus d'info ici: http://books.google.fr/books?id=ZNngQv_E0_MC&lpg=PA250&ots=_ZTiXKt-8p&hl=fr&pg=PA250
						while(!RX24_UART_GetFlagStatus(USART_FLAG_TC));


					GPIO_WriteBit(RX24_DIRECTION_PORT_RX24, RX_DIRECTION);

						//flush recv buffer
					while(RX24_UART_GetFlagStatus(USART_FLAG_ORE) || RX24_UART_GetFlagStatus(USART_FLAG_FE) || RX24_UART_GetFlagStatus(USART_FLAG_NE))
						RX24_UART_getc();

					state_machine.state = RX24_SMS_WaitingAnswer;

					TIMER_SRC_TIMER_start_ms(RX24_STATUS_RETURN_TIMEOUT);	//Pour le timeout de reception, ne devrait pas arriver
				}
				else
				{
					RX24_instruction_next(RX24_ERROR_OK, 0);
					state_machine.state = RX24_SMS_ReadyToSend;
					RX24_state_machine(RX24_SME_NoEvent);
				}
			}
			}else if(event == RX24_SME_Timeout) {
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();
				debug_printf("RX24[%d]: send timeout !!\n", state_machine.current_instruction.id_servo);

				RX24_instruction_next(RX24_ERROR_TIMEOUT, 1);
				state_machine.state = RX24_SMS_ReadyToSend;
				RX24_state_machine(RX24_SME_NoEvent);
			}

			if(state_machine.state != RX24_SMS_Sending)
				RX24_UART_ITConfig(USART_IT_TXE, DISABLE);
		break;

		case RX24_SMS_WaitingAnswer:
			if(event == RX24_SME_RxInterrupt)
			{
				//RX24_update_status_packet s'occupe de son initialisation et de son remplissage au fur et a mesure que les octets arrivent
				static RX24_status_packet_t status_response_packet;
				// Stockage de la réponse dans un buffer, si toute la réponse alors mise à jour des variables du driver et state = RX24_SMS_ReadyToSend avec éxécution

				if(RX24_UART_GetFlagStatus(USART_FLAG_FE) || RX24_UART_GetFlagStatus(USART_FLAG_NE)) { //ignore error bits
					RX24_UART_getc();
					break;
				}

				#if defined(VERBOSE_MODE) && defined(RX24_DEBUG_PACKETS)
				RX24_UART2_reception_buffer[pos] = RX24_UART_getc();

				if(!RX24_update_status_packet(RX24_UART2_reception_buffer[pos++], state_machine.receive_index, &status_response_packet))
				#else
				Uint8 data_byte = RX24_UART_getc();
				if(!RX24_update_status_packet(data_byte, state_machine.receive_index, &status_response_packet))
				#endif
				{
					//debug_printf("RX24: invalid packet, reinit reception\n");
					state_machine.receive_index = 0;	//si le paquet n'est pas valide, on reinitialise la lecture de paquet
					//pos = 0;
					//for(i=0; i<MAX_STATUS_PACKET_SIZE*2; i++)
					//	RX24_UART2_reception_buffer[i] = 0;
					break;
				} else {
					state_machine.receive_index++;
				}


				if(RX24_status_packet_is_full(status_response_packet, state_machine.receive_index))
				{
					TIMER_SRC_TIMER_stop();
					TIMER_SRC_TIMER_resetFlag();

					#if defined(VERBOSE_MODE) && defined(RX24_DEBUG_PACKETS)
						debug_printf("RX24 Rx:");
						for(i = 0; i<state_machine.receive_index; i++)
							debug_printf(" %02x", RX24_UART2_reception_buffer[i]);
						debug_printf("\n");
					#endif

					if(status_response_packet.id_servo != state_machine.current_instruction.id_servo || status_response_packet.id_servo >= RX24_NUMBER) {
						debug_printf("Wrong servo ID: %d instead of %d\n", status_response_packet.id_servo, state_machine.current_instruction.id_servo);

						RX24_instruction_next(RX24_ERROR_INSTRUCTION, 1);
					} else {

						#ifdef VERBOSE_MODE
							if(status_response_packet.error & RX24_ERROR_VOLTAGE)
								debug_printf("RX24[%d] Fatal: Voltage error\n", status_response_packet.id_servo);
							if(status_response_packet.error & RX24_ERROR_ANGLE)
								debug_printf("RX24[%d] Error: Angle error\n", status_response_packet.id_servo);
							if(status_response_packet.error & RX24_ERROR_OVERHEATING)
								debug_printf("RX24[%d] Fatal: Overheating error\n", status_response_packet.id_servo);
							if(status_response_packet.error & RX24_ERROR_RANGE)
								debug_printf("RX24[%d] Error: Range error\n", status_response_packet.id_servo);
							if(status_response_packet.error & RX24_ERROR_CHECKSUM)
								debug_printf("RX24[%d] Error: Checksum error\n", status_response_packet.id_servo);
							if(status_response_packet.error & RX24_ERROR_OVERLOAD)
								debug_printf("RX24[%d] Fatal: Overload error\n", status_response_packet.id_servo);
							if(status_response_packet.error & RX24_ERROR_INSTRUCTION)
								debug_printf("RX24[%d] Error: Instruction error\n", status_response_packet.id_servo);
							if(status_response_packet.error & 0x80)
								debug_printf("RX24[%d] Fatal: Unknown (0x80) error\n", status_response_packet.id_servo);
							if(status_response_packet.error)
								debug_printf("RX24[%d] Cmd: %d, addr: 0x%x, param: 0x%x\n",
										state_machine.current_instruction.id_servo,
										state_machine.current_instruction.type,
										state_machine.current_instruction.address,
										state_machine.current_instruction.param);
						#endif

						RX24_instruction_next(status_response_packet.error, status_response_packet.param);
					}
					state_machine.state = RX24_SMS_ReadyToSend;
					RX24_state_machine(RX24_SME_NoEvent);
				}
			} else if(event == RX24_SME_Timeout) {
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();

				//debug_printf("RX24[%d] timeout Rx:", state_machine.current_instruction.id_servo);
				#if defined(VERBOSE_MODE) && defined(RX24_DEBUG_PACKETS)
					debug_printf("RX24[%d] timeout Rx:", state_machine.current_instruction.id_servo);
					for(i = 0; i<pos; i++)
						debug_printf(" %02x", RX24_UART2_reception_buffer[i]);
					debug_printf(", recv idx: %d\n", state_machine.receive_index);
					debug_printf(" Original cmd: Id: %02x Cmd: %02x Addr: %02x param: %04x\n", state_machine.current_instruction.id_servo, state_machine.current_instruction.type, state_machine.current_instruction.address, state_machine.current_instruction.param);
				#endif

				if(RX24_UART_GetFlagStatus(USART_FLAG_RXNE))
					debug_printf("RX24 timeout too small\n");

				RX24_instruction_next(RX24_ERROR_TIMEOUT, 0);
				state_machine.state = RX24_SMS_ReadyToSend;
				RX24_state_machine(RX24_SME_NoEvent);
			} /*else if(event == RX24_SME_TxInterrupt) {
				RX24_UART_ITConfig(USART_IT_TXE, DISABLE);
			}*/
		break;
	}

	if(event == RX24_SME_RxInterrupt || event == RX24_SME_TxInterruptRX24) {
		TIMER_SRC_TIMER_EnableIT();
	} else if(event == RX24_SME_Timeout) {
		RX24_UART_EnableIRQ();
	} else if(event == RX24_SME_NoEvent) {
		processing_state = 0;
	}
}

/*********************************************************************************/
/** Implémentation des fonctions gérant le buffer d'instruction (insertion ici) **/
/*********************************************************************************/

static bool_e RX24_instruction_queue_insert(const RX24_instruction_packet_t* inst) {	//utilisation d'un pointeur pour eviter de provoquer une copie intégrale de la structure, qui pourrait prendre du temps si elle est trop grande
	Uint16 i = 0;
	assert(inst->id_servo < RX24_NUMBER || inst->id_servo == RX24_BROADCAST_ID);
	while(RX24_instruction_queue_is_full() && i < 65000)	//boucle 65000 fois si le buffer reste full, si on atteint 65000, on échoue et retourne FALSE
		i++;

	if(i >= 65000) {
		if(inst->id_servo < RX24_NUMBER) {
			RX24_on_the_robot[inst->id_servo].last_status.error = RX24_ERROR_TIMEOUT | RX24_ERROR_RANGE;
			RX24_on_the_robot[inst->id_servo].last_status.param = 0;
		}
		debug_printf("RX24 Fatal: Instruction buffer full !\n");
		return FALSE;	//return false, on a pas réussi a insérer l'instruction, problème de priorité d'interruptions ?
	}

	RX24_on_the_robot[inst->id_servo].last_status.error = RX24_ERROR_IN_PROGRESS;
	RX24_special_instruction_buffer.buffer[RX24_special_instruction_buffer.end_index] = *inst;
	RX24_special_instruction_buffer.end_index = INC_WITH_MOD(RX24_special_instruction_buffer.end_index, RX24_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE);
	//truc = state_machine.state;
	//if(truc == RX24_SMS_ReadyToSend)
		RX24_state_machine(RX24_SME_NoEvent);
	//else debug_printf("RX24 not ready, in state %d\n", truc);

	return TRUE;
}

/**************************************************************************/
/** Fonction initialisant l'UART2 utilisé pour communiquer               **/
/**************************************************************************/

/*	vitesse : 57600 bauds (modifiable: uart_speed)
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/

static void RX24_UART_init_all(Uint32 uart_speed)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	PORTS_uarts_init();


	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //inférieur aux uarts mais supérieur au timers


#ifdef RX24_UART_ID
	RX24_UART_init(RX24_UART_Ptr, uart_speed);
	NVIC_InitStructure.NVIC_IRQChannel = RX24_UART_Interrupt_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	debug_printf("UART %d initialized for RX24\n", RX24_UART_ID);
#endif
}

static void RX24_UART_init(USART_TypeDef* uartPtr, Uint16 baudrate) {
	USART_InitTypeDef USART_InitStructure;

	if(uartPtr == USART1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	else if(uartPtr == USART2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	else if(uartPtr == USART3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	else {
		fatal_printf("UART inconnu !!! %p\n", uartPtr);
		assert(0 && "Uart inconnu");
	}

	USART_OverSampling8Cmd(uartPtr, ENABLE);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(uartPtr, &USART_InitStructure);


	/* Enable USART */
	USART_Cmd(uartPtr, ENABLE);

	USART_ITConfig(uartPtr, USART_IT_RXNE, ENABLE);
	USART_ITConfig(uartPtr, USART_IT_TXE, DISABLE);
}

static void RX24_UART_DisableIRQ() {

#ifdef RX24_UART_Interrupt_IRQn
	NVIC_DisableIRQ(RX24_UART_Interrupt_IRQn);
#endif
}

static void RX24_UART_EnableIRQ() {

#ifdef RX24_UART_Interrupt_IRQn
	NVIC_EnableIRQ(RX24_UART_Interrupt_IRQn);
#endif
}

static void RX24_UART_putc(Uint8 c) {

#ifdef RX24_UART_Ptr
	USART_SendData(RX24_UART_Ptr, c);
#endif
}

static Uint8 RX24_UART_getc() {

#ifdef RX24_UART_Ptr
		if(USART_GetFlagStatus(RX24_UART_Ptr, USART_FLAG_RXNE))
			return USART_ReceiveData(RX24_UART_Ptr);
#endif

	return 0;
}

static bool_e RX24_UART_GetFlagStatus(Uint16 flag) {
	if(flag == USART_FLAG_TC) {
		if(

		#ifdef RX24_UART_Ptr
				USART_GetFlagStatus(RX24_UART_Ptr, flag)
		#endif
				)
			return TRUE;
	} else {
		if(

		#ifdef RX24_UART_Ptr
				USART_GetFlagStatus(RX24_UART_Ptr, flag)
		#endif
				)
			return TRUE;
	}
	return FALSE;
}

static void RX24_UART_ITConfig(Uint16 flag, FunctionalState enable) {

#ifdef RX24_UART_Ptr
	USART_ITConfig(RX24_UART_Ptr, flag, enable);
#endif
}

/*****************************************************************************/
/** Interruptions utilisées (Reception/Envoi UART2 et timer pour le timeout **/
/*****************************************************************************/


//Attention ! Si on met a 0 le flag après avoir executé la machine a état,
//des interruptions peuvent être masquée
//(en gros il se peut qu'une interruption Tx arrive avant d'avoir terminée l'execution de la fonction,
//dans ce cas mettre le flag a 0 fait que l'interruption Tx n'est pas lancée)
//(car le caractère envoyé est envoyé plus vite que le retour de la fonction RX24_state_machine)


#ifdef RX24_UART_Interrupt
void _ISR RX24_UART_Interrupt(void)
{
	if(USART_GetITStatus(RX24_UART_Ptr, USART_IT_RXNE))
	{
		Uint8 i = 0;
		while(USART_GetFlagStatus(RX24_UART_Ptr, USART_FLAG_RXNE)) {		//On a une IT Rx pour chaque caratère reçu, donc on ne devrai pas tomber dans un cas avec 2+ char dans le buffer uart dans une IT
			if(state_machine.state != RX24_SMS_WaitingAnswer) {	//Arrive quand on allume les cartes avant la puissance ou lorsque l'on coupe la puissance avec les cartes alumées (reception d'un octet avec l'erreur FERR car l'entrée RX tombe à 0)
				USART_ReceiveData(RX24_UART_Ptr);
			} else {
				RX24_state_machine(RX24_SME_RxInterrupt);
				if(USART_GetFlagStatus(RX24_UART_Ptr, USART_FLAG_RXNE) && i > 5) {
					//debug_printf("Overinterrupt RX !\n");
					break; //force 0, on va perdre des caractères, mais c'est mieux que de boucler ici ...
				}
			}
			i++;
		}
	}
	else if(USART_GetITStatus(RX24_UART_Ptr, USART_IT_TXE))
	{
		RX24_state_machine(RX24_SME_TxInterruptRX24);
	}
}
#endif

void TIMER_SRC_TIMER_interrupt()
{
	Uint8 i = 0;

	while(RX24_UART_GetFlagStatus(USART_FLAG_RXNE)) {		//On a une IT Rx pour chaque caratère reçu, donc on ne devrai pas tomber dans un cas avec 2+ char dans le buffer uart dans une IT
		if(state_machine.state != RX24_SMS_WaitingAnswer) {	//Arrive quand on allume les cartes avant la puissance ou lorsque l'on coupe la puissance avec les cartes alumées (reception d'un octet avec l'erreur FERR car l'entrée RX tombe à 0)
			RX24_UART_getc();
		} else {
			RX24_state_machine(RX24_SME_RxInterrupt);
			if(RX24_UART_GetFlagStatus(USART_FLAG_RXNE) && i > 5) {
				debug_printf("Overinterrupt RX ! while timeout\n");
				break; //force 0, on va perdre des caractères, mais c'est mieux que de boucler ici ...
			}
		}
		i++;
	}
	RX24_state_machine(RX24_SME_Timeout);
	TIMER_SRC_TIMER_resetFlag();
}

/**************************************************************************/
/** Fonctions d'interface utilisé par l'utilisateur du driver            **/
/**************************************************************************/

/* Fonction initialisant ce Driver	                                     */

void RX24_init() {
	static bool_e initialized = FALSE;
	Uint8 i;
	if(initialized)
		return;

	initialized = TRUE;

	RX24_UART_init_all(RX24_UART_BAUDRATE);
	TIMER_SRC_TIMER_init();

	GPIO_WriteBit(RX24_DIRECTION_PORT_RX24, RX_DIRECTION);

	RX24_prepare_commands = FALSE;
	RX24_instruction_write8(RX24_BROADCAST_ID, RX24_RETURN_LEVEL, RX24_STATUS_RETURN_MODE);	//Mettre les RX24/RX24 dans le mode indiqué dans Global_config.h

	for(i=0; i<RX24_NUMBER; i++) {
		RX24_on_the_robot[i].angle_limit[0] = 0;
		RX24_on_the_robot[i].angle_limit[1] = 1023;

		RX24_on_the_robot[i].is_wheel_enabled = FALSE;
		RX24_instruction_reset_last_status(i);
	}
}

/* Fonction d'utilisation de l'RX24/RX24                                        */

//Configuration de l'RX24, perdure après mise hors tension sauf pour le verouillage de la config (lock).
//Unités:
// Angles sur 300° de 0 à 1023
// Vitesse en pourcentage de la vitesse max
// Voltage en dixième de volt (50 => 5.0V)
// Température en degrée celcius
// Pourcentage entre 0 et 100

#define RX24_MAX_ANGLE 1023

//Poucentage max: 100% (tout le monde le savait ça, mais c'est bien de le repréciser :) )
#define RX24_MAX_PERCENTAGE 100
#define RX24_1024_TO_PERCENTAGE(percentage) ((((Uint16)(percentage))*25) >> 8) // >> 8 <=> / 256, 25/256 = 100/1024
#define RX24_PERCENTAGE_TO_1024(percentage) ((((Uint16)(percentage)) << 8) / 25)

//Implementation

bool_e RX24_is_ready(Uint8 id_servo) {
	return RX24_instruction_ping(id_servo);
}

bool_e RX24_async_is_ready(Uint8 id_servo, bool_e *isReady) {
	RX24_status_t status = RX24_get_last_error(id_servo);
	if(status.error != RX24_ERROR_IN_PROGRESS) {
		if(status.last_instruction_address == RX24_PING) {
			RX24_reset_last_error(id_servo);
			if(status.error == RX24_ERROR_OK)
				*isReady = TRUE;
			else
				*isReady = FALSE;
			return TRUE;
		} else {
			RX24_instruction_async_ping(id_servo);
		}
	}


	return FALSE;
}

RX24_status_t RX24_get_last_error(Uint8 id_servo) {
	return RX24_instruction_get_last_status(id_servo);
}

void RX24_reset_last_error(Uint8 id_servo) {
	RX24_instruction_reset_last_status(id_servo);
}

bool_e RX24_is_buffer_full() {
	return RX24_instruction_buffer_is_full();
}

void RX24_start_command_block() {
	RX24_prepare_commands = TRUE;
}

void RX24_end_command_block() {
	RX24_instruction_async_execute_write();
	RX24_prepare_commands = FALSE;
}

#if RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER
bool_e RX24_config_is_locked(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_LOCK, NULL) != 0;
}

Uint16 RX24_config_get_model_number(Uint8 id_servo) {
	return RX24_instruction_read16(id_servo, RX24_MODEL_NUMBER_L, NULL);
}

Uint8  RX24_config_get_firmware_version(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_VERSION, NULL);
}

Uint16 RX24_config_get_return_delay_time(Uint8 id_servo) {
	return ((Uint16)RX24_instruction_read8(id_servo, RX24_RETURN_DELAY_TIME, NULL)) * 2;
}

Uint16 RX24_config_get_minimal_angle(Uint8 id_servo) {
	return RX24_instruction_read16(id_servo, RX24_CW_ANGLE_LIMIT_L, NULL);
}

Uint16 RX24_config_get_maximal_angle(Uint8 id_servo) {
	return RX24_instruction_read16(id_servo, RX24_CCW_ANGLE_LIMIT_L, NULL);
}

Uint8  RX24_config_get_temperature_limit(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_LIMIT_TEMPERATURE, NULL);
}

Uint8  RX24_config_get_lowest_voltage(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_DOWN_LIMIT_VOLTAGE, NULL);
}

Uint8  RX24_config_get_highest_voltage(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_UP_LIMIT_VOLTAGE, NULL);
}

Uint8 RX24_config_get_maximum_torque(Uint8 id_servo) {
	Uint16 value;    //On ne peut pas utiliser la lecture 16bits a cause d'un bug de l'RX24 (voir https://sites.google.com/site/robotsaustralia/ax-12dynamixelinformation)
	value = RX24_instruction_read8(id_servo, RX24_MAX_TORQUE_L, NULL) | (RX24_instruction_read8(id_servo, RX24_MAX_TORQUE_H, NULL) << 8);
	return (Uint8)RX24_1024_TO_PERCENTAGE(value);
}

Uint8  RX24_config_get_status_return_mode(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_RETURN_LEVEL, NULL);
}

Uint8  RX24_config_get_error_before_led(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_ALARM_LED, NULL);
}

Uint8  RX24_config_get_error_before_shutdown(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_ALARM_SHUTDOWN, NULL);
}
#endif //RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER

bool_e RX24_config_lock(Uint8 id_servo) {
	return RX24_instruction_write8(id_servo, RX24_LOCK, 1);
}

bool_e RX24_config_set_id(Uint8 id_servo_current, Uint8 id_servo_new){
	return RX24_instruction_write8(id_servo_current, RX24_ID, id_servo_new);
}

bool_e RX24_config_set_return_delay_time(Uint8 id_servo, Uint16 delay_us) {
	return RX24_instruction_write8(id_servo, RX24_RETURN_DELAY_TIME, (Uint8)(delay_us >> 1)); // >> 1 <=> /2
}

bool_e RX24_config_set_minimal_angle(Uint8 id_servo, Uint16 degre) {
	if(degre > RX24_MAX_ANGLE) degre = RX24_MAX_ANGLE;
	return RX24_instruction_write16(id_servo, RX24_CW_ANGLE_LIMIT_L, degre);
}

bool_e RX24_config_set_maximal_angle(Uint8 id_servo, Uint16 degre) {
	if(degre > RX24_MAX_ANGLE) degre = RX24_MAX_ANGLE;
	if(degre == 0) degre = 1;	//Si l'utilisateur met un angle mini et maxi à 0, l'RX24/RX24 passera en mode rotation en continue, ce mode ne doit être activé que par RX24_set_wheel_mode_enabled (en passant TRUE)
	return RX24_instruction_write16(id_servo, RX24_CCW_ANGLE_LIMIT_L, degre);
}

bool_e RX24_config_set_temperature_limit(Uint8 id_servo, Uint8 temperature) {
	return RX24_instruction_write8(id_servo, RX24_LIMIT_TEMPERATURE, temperature);
}

bool_e RX24_config_set_lowest_voltage(Uint8 id_servo, Uint8 voltage) {
	return RX24_instruction_write8(id_servo, RX24_DOWN_LIMIT_VOLTAGE, voltage);
}

bool_e RX24_config_set_highest_voltage(Uint8 id_servo, Uint8 voltage) {
	return RX24_instruction_write8(id_servo, RX24_UP_LIMIT_VOLTAGE, voltage);
}

bool_e RX24_config_set_maximum_torque_percentage(Uint8 id_servo, Uint8 percentage) {
	Uint16 value = RX24_PERCENTAGE_TO_1024(percentage);
	return RX24_instruction_write16(id_servo, RX24_MAX_TORQUE_L, value);
}

bool_e RX24_config_set_status_return_mode(Uint8 id_servo, Uint8 mode) {
	return RX24_instruction_write8(id_servo, RX24_RETURN_LEVEL, mode);
}

bool_e RX24_config_set_error_before_led(Uint8 id_servo, Uint8 error_type) {
	return RX24_instruction_write8(id_servo, RX24_ALARM_LED, error_type);
}

bool_e RX24_config_set_error_before_shutdown(Uint8 id_servo, Uint8 error_type) {
	return RX24_instruction_write8(id_servo, RX24_ALARM_SHUTDOWN, error_type);
}


//RAM

bool_e RX24_is_wheel_mode_enabled(Uint8 id_servo) {
	return RX24_on_the_robot[id_servo].is_wheel_enabled;
}

#if RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER
bool_e RX24_is_torque_enabled(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_TORQUE_ENABLE, NULL) != 0;
}

bool_e RX24_is_led_enabled(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_LED, NULL) != 0;
}

bool_e RX24_get_torque_response(Uint8 id_servo, Uint16* A, Uint16* B, Uint16* C, Uint16* D) {
	bool_e isOk;
	Uint8 CcwComplianceSlope;
	Uint8 CcwComplianceMargin;
	Uint8 CwComplianceMargin;
	Uint8 CwComplianceSlope;

	CcwComplianceSlope = RX24_instruction_read8(id_servo, RX24_CCW_COMPLIANCE_SLOPE, &isOk);
	if(!isOk) return FALSE;
	CcwComplianceMargin = RX24_instruction_read8(id_servo, RX24_CCW_COMPLIANCE_MARGIN, &isOk);
	if(!isOk) return FALSE;
	CwComplianceMargin = RX24_instruction_read8(id_servo, RX24_CW_COMPLIANCE_MARGIN, &isOk);
	if(!isOk) return FALSE;
	CwComplianceSlope = RX24_instruction_read8(id_servo, RX24_CW_COMPLIANCE_SLOPE, &isOk);
	if(!isOk) return FALSE;

	if(A) *A = CcwComplianceSlope;
	if(B) *B = CcwComplianceMargin;
	if(C) *C = CwComplianceMargin;
	if(D) *D = CwComplianceSlope;

	return TRUE;
}

Uint16 RX24_get_position(Uint8 id_servo) {
	return RX24_instruction_read16(id_servo, RX24_PRESENT_POSITION_L, NULL);
}

Sint8 RX24_get_move_to_position_speed(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 speedPercentage;

	if(RX24_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("RX24: RX24_get_move_to_position_speed while in wheel mode, use RX24_get_speed_percentage instead.\n");
		return 0;
	}

	speedPercentage = RX24_instruction_read16(id_servo, RX24_PRESENT_SPEED_L, NULL);

	isBackward = (speedPercentage & 0x400) != 0;
	speedPercentage = RX24_1024_TO_PERCENTAGE(speedPercentage & 0x3FF);
	if(isBackward)
		return -(Sint8)speedPercentage;
	return (Sint8)speedPercentage;
}

Sint8 RX24_get_speed_percentage(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 speedPercentage;

	if(!RX24_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("RX24: RX24_get_speed_percentage while not in wheel mode, use RX24_get_move_to_position_speed instead.\n");
		return 0;
	}

	speedPercentage = RX24_instruction_read16(id_servo, RX24_PRESENT_SPEED_L, NULL);

	isBackward = (speedPercentage & 0x400) != 0;
	speedPercentage = RX24_1024_TO_PERCENTAGE(speedPercentage & 0x3FF);
	if(isBackward)
		return -(Sint8)speedPercentage;
	return (Sint8)speedPercentage;
}

Uint8 RX24_get_torque_limit(Uint8 id_servo) {
	return (Uint8)RX24_1024_TO_PERCENTAGE(RX24_instruction_read16(id_servo, RX24_TORQUE_LIMIT_L, NULL));
}

Uint8 RX24_get_punch_torque_percentage(Uint8 id_servo) {
	return (Uint8)RX24_1024_TO_PERCENTAGE(RX24_instruction_read16(id_servo, RX24_PUNCH_L, NULL));
}

Sint8 RX24_get_load_percentage(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 loadPercentage;

	loadPercentage = RX24_instruction_read16(id_servo, RX24_PRESENT_LOAD_L, NULL);

	isBackward = (loadPercentage & 0x400) != 0;
	loadPercentage = RX24_1024_TO_PERCENTAGE(loadPercentage & 0x3FF);
	if(isBackward)
		return -(Sint8)loadPercentage;
	return (Sint8)loadPercentage;
}

Uint8 RX24_get_voltage(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_PRESENT_VOLTAGE, NULL);
}

Uint8 RX24_get_temperature(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_PRESENT_TEMPERATURE, NULL);
}

bool_e RX24_is_instruction_prepared(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_REGISTERED_INSTRUCTION, NULL) != 0;
}

bool_e RX24_is_moving(Uint8 id_servo) {
	return RX24_instruction_read8(id_servo, RX24_MOVING, NULL) != 0;
}


bool_e RX24_set_wheel_mode_enabled(Uint8 id_servo, bool_e enabled) {
	if(enabled == RX24_on_the_robot[id_servo].is_wheel_enabled)	//mode déja défini
		return TRUE;

	if(enabled) {	//sauvegarde les angles limite défini et passe en mode wheel
		Uint16 min_angle, max_angle;

		RX24_on_the_robot[id_servo].is_wheel_enabled = TRUE;

		min_angle = RX24_config_get_minimal_angle(id_servo);
		if(RX24_get_last_error(id_servo).error)
			return FALSE;

		max_angle = RX24_config_get_maximal_angle(id_servo);
		if(RX24_get_last_error(id_servo).error)
			return FALSE;
		if(max_angle == 0) max_angle = 1;	//évite les bugs si les 2 angles limites sont à 0 (et donc qu'on était déja en mode wheel mais que le servo ne le savait pas.

		if(!RX24_config_set_minimal_angle(id_servo, 0))
			return FALSE;
		if(!RX24_config_set_maximal_angle(id_servo, 0))
			return FALSE;

		RX24_on_the_robot[id_servo].angle_limit[0] = min_angle;
		RX24_on_the_robot[id_servo].angle_limit[1] = max_angle;

		return TRUE;
	} else {
		RX24_on_the_robot[id_servo].is_wheel_enabled = FALSE;
		if(!RX24_config_set_minimal_angle(id_servo, RX24_on_the_robot[id_servo].angle_limit[0]))
			return FALSE;
		if(!RX24_config_set_maximal_angle(id_servo, RX24_on_the_robot[id_servo].angle_limit[1]))
			return FALSE;
		return TRUE;
	}
}
#endif //RX24_STATUS_RETURN_MODE != RX24_STATUS_RETURN_NEVER

bool_e RX24_set_torque_enabled(Uint8 id_servo, bool_e enabled) {
	return RX24_instruction_write8(id_servo, RX24_TORQUE_ENABLE, enabled != 0);
}

bool_e RX24_set_led_enabled(Uint8 id_servo, bool_e enabled) {
	return RX24_instruction_write8(id_servo, RX24_LED, enabled != 0);
}

	/*
	Paramétrage de l'asservissement avec RX24_set_torque_response(A, B, C, D)
				Couple          Consigne de position
					^                   |
					|=============\     |
					|             ^\    |
	Sens anti-      |             | |   v
	  horaire     <-+-------------|-+=======+------------------->  Sens horaire
					|             | ^   ^   |             X: Erreur en position
					|             | |   |    \
					|             | |   |   ^ \===============
					v             | |   |   | ^
								  A B   0   C D

	=== : Courbe du couple en fonction de l'erreur en position
	Les points A, B, C, D sont la valeur absolue de la différence de position entre le point et la position consigne (en minute d'angle)
	Si la valeur absolue de l'erreur est plus grande que A (sens anti-horaire) ou D (sens horaire), le couple est constant quelque soit l'erreur
	Si la valeur absolue de l'erreur est entre A et B ou C et D, le couple est proportionnel à la valeur absolue de l'erreur (le courant minimal pour donner du couple est défini par les fonctions RX24_set_punch_current/RX24_get_punch_current)
	Si la valeur absolue de l'erreur est plus petite que B ou C, l'RX24/RX24 n'exerce aucun couple

	*/
bool_e RX24_set_torque_response(Uint8 id_servo, Uint16 A, Uint16 B, Uint16 C, Uint16 D) {
	Uint16 CcwComplianceSlope;
	Uint16 CcwComplianceMargin;
	Uint16 CwComplianceMargin;
	Uint16 CwComplianceSlope;

	//Limite d'angle max sinon si l'angle est trop élevé, RX24_DEGRE_TO_ANGLE pourrait renvoyer une valeur erronée a cause d'un overflow
	if(A > RX24_MAX_ANGLE)
		A = RX24_MAX_ANGLE-1;
	if(B > RX24_MAX_ANGLE)
		B = RX24_MAX_ANGLE-1;
	if(C > RX24_MAX_ANGLE)
		C = RX24_MAX_ANGLE-1;
	if(D > RX24_MAX_ANGLE)
		D = RX24_MAX_ANGLE-1;

	CcwComplianceSlope = A;
	CcwComplianceMargin = B;
	CwComplianceMargin = C;
	CwComplianceSlope = D;

	if(!RX24_instruction_async_write8(id_servo, RX24_CCW_COMPLIANCE_SLOPE, CcwComplianceSlope))   // A verifier value 16bits envoie dans une fonction 8 bits
		return FALSE;
	if(!RX24_instruction_async_write8(id_servo, RX24_CCW_COMPLIANCE_MARGIN, CcwComplianceMargin))
		return FALSE;
	if(!RX24_instruction_async_write8(id_servo, RX24_CW_COMPLIANCE_MARGIN, CwComplianceMargin))
		return FALSE;
	if(!RX24_instruction_async_write8(id_servo, RX24_CW_COMPLIANCE_SLOPE, CwComplianceSlope))
		return FALSE;

	#if RX24_STATUS_RETURN_MODE == RX24_STATUS_RETURN_ALWAYS
		if(!RX24_instruction_wait(id_servo) || RX24_instruction_get_last_status(id_servo).error == RX24_ERROR_OK)
			return FALSE;
	#endif

	return TRUE;
}

bool_e RX24_set_position(Uint8 id_servo, Uint16 angle) {
	if(angle > RX24_MAX_ANGLE) angle = RX24_MAX_ANGLE-1;
	return RX24_instruction_write16(id_servo, RX24_GOAL_POSITION_L, angle);
}

bool_e RX24_set_move_to_position_speed(Uint8 id_servo, Uint8 percentage) {
	if(RX24_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("RX24: RX24_set_move_to_position_speed while in wheel mode, use RX24_set_speed_percentage instead.\n");
		return FALSE;
	}

	if(percentage > RX24_MAX_PERCENTAGE) percentage = RX24_MAX_PERCENTAGE;
	Uint16 realValue = RX24_PERCENTAGE_TO_1024(percentage);

	return RX24_instruction_write16(id_servo, RX24_GOAL_SPEED_L, realValue);
}

bool_e RX24_set_speed_percentage(Uint8 id_servo, Sint8 percentage) {
	bool_e isBackward;

	if(!RX24_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("RX24: RX24_set_speed_percentage while not in wheel mode, use RX24_set_move_to_position_speed instead.\n");
		return FALSE;
	}

	isBackward = percentage < 0;
	//percentage & 0x7F: on enlève le bit de signe, isBackward << 10: ajoute 1024 si on tourne a l'envers
	percentage = percentage & 0x7F;
	if(percentage > RX24_MAX_PERCENTAGE) percentage = RX24_MAX_PERCENTAGE;
	Uint16 realValue = RX24_PERCENTAGE_TO_1024(percentage) | (((Uint16)isBackward) << 10);

	return RX24_instruction_write16(id_servo, RX24_GOAL_SPEED_L, realValue);
}

bool_e RX24_set_torque_limit(Uint8 id_servo, Uint8 percentage) {
	if(percentage > RX24_MAX_PERCENTAGE) percentage = RX24_MAX_PERCENTAGE;

	return RX24_instruction_write16(id_servo, RX24_TORQUE_LIMIT_L, RX24_PERCENTAGE_TO_1024(percentage));
}

bool_e RX24_set_punch_torque_percentage(Uint8 id_servo, Uint8 percentage) {
	if(percentage > RX24_MAX_PERCENTAGE) percentage = RX24_MAX_PERCENTAGE;
	return RX24_instruction_write16(id_servo, RX24_PUNCH_L, RX24_PERCENTAGE_TO_1024(percentage));
}

#endif /* def USE_RX24_SERVO */
