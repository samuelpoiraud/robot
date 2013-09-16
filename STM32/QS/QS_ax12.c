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

//Pour voir l'ancienne version avant les modifications faites pour le robot 2012-2013, utilisez la version SVN 5340

#include "QS_ax12.h"

#ifdef USE_AX12_SERVO

#include "stm32f4xx_usart.h"
																			
	/**********************************************************************/
	/** Paramètres de configuration du driver AX12                        */
	/**********************************************************************/

	#if !defined(AX12_NUMBER) || AX12_NUMBER > 254
		#error "Vous devez definir un nombre de servo sur le robot (AX12_NUMBER) (254 max)"
	#endif /* ndef AX12_NUMBER */
	
	//#define AX12_DEBUG_PACKETS: si défini, les paquets envoyé sont écrit sur la sortie uart en utilisant debug_printf (attention, debug_printf n'est pas réentrante)

	#ifndef AX12_UART_ID
		#define AX12_UART_ID 2
	#endif

#if AX12_UART_ID == 1
	#define AX12_UART_Ptr USART1
	#define AX12_UART_Interrupt USART1_IRQHandler
	#define AX12_UART_Interrupt_IRQn USART1_IRQn
#elif AX12_UART_ID == 2
	#define AX12_UART_Ptr USART2
	#define AX12_UART_Interrupt USART2_IRQHandler
	#define AX12_UART_Interrupt_IRQn USART2_IRQn
#else
#warning "AX12: Unknown UART ID"
#endif

	#ifndef AX12_UART_BAUDRATE
		#define AX12_UART_BAUDRATE  56700
	#endif

	#ifndef AX12_STATUS_RETURN_TIMEOUT
		#define AX12_STATUS_RETURN_TIMEOUT 50	//Ne pas mettre une valeur trop proche de celle prévue, l'AX12 répond parfois avec un delai plus long
	#endif

	#ifndef AX12_STATUS_SEND_TIMEOUT
		#define AX12_STATUS_SEND_TIMEOUT 10
	#endif

	#ifndef AX12_STATUS_RETURN_MODE
		#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ONREAD
	#endif

	#ifndef AX12_INSTRUCTION_BUFFER_SIZE
		#define AX12_INSTRUCTION_BUFFER_SIZE (5*AX12_NUMBER)
	#endif

	/**********************************************************************/
	/** Configuration du timer pour le timeout de réception du status    **/
	/**********************************************************************/

	#ifdef BUTTONS_TIMER
		#define TIMER_SRC_TIMER_ID BUTTONS_TIMER
	#endif
	#ifdef BUTTONS_TIMER_USE_WATCHDOG
		#define TIMER_SRC_USE_WATCHDOG
	#endif

	#include "QS_setTimerSource.h"

	/*************************************************************************************/

	/* UART2 exclusivement dédié à la gestion des servomoteurs AX-12+ */

	#ifdef USE_UART2
		#error "Vous ne pouvez pas utiliser l'UART2 et l'AX12 en même temps, l'AX12 à besoin de l'UART2 pour communiquer"
	#endif

	#ifndef AX12_DIRECTION_PORT
		#error "Vous devez definir un port de direction (AX12_DIRECTION_PORT) pour gérer l'UART qui est en Half-duplex (cf. Datasheet MAX485)"
	#endif /* ndef AX12_DIRECTION_PORT */

	/* Pour UART half-uplex */
	#define TX_DIRECTION 1
	#define RX_DIRECTION 0


	/*************************************************************************************/
	/** Constantes liées au instruction envoyées a l'AX12                               **/
	/*************************************************************************************/

	/* Instructions */
	
	#define INST_PING 				0x01
	#define INST_READ 				0x02
	#define INST_WRITE 				0x03
	#define INST_REG_WRITE 			0x04
	#define INST_ACTION 			0x05
	//Instructions non utilisées
	//#define INST_RESET 				0x06
	//#define INST_DIGITAL_RESET 		0x07
	//#define INST_SYSTEM_READ 		0x0C
	//#define INST_SYSTEM_WRITE 		0x0D
	//#define INST_SYNC_WRITE 		0x83
	//#define INST_SYNC_REG_WRITE 	0x84

	// Adresses de la mémoire interne de l'AX12
	#define AX12_MEM_SIZE		0x32

	#define AX12_MIN_VALUE_0	0x00
	#define AX12_MIN_VALUE_1	0x01
	#define AX12_MIN_VALUE_34	0x02
	#define AX12_MIN_VALUE_50	0x03

	#define AX12_MAX_VALUE_1	0x70
	#define AX12_MAX_VALUE_2	0x60
	#define AX12_MAX_VALUE_127	0x50
	#define AX12_MAX_VALUE_150	0x40
	#define AX12_MAX_VALUE_250	0x30
	#define AX12_MAX_VALUE_253	0x20
	#define AX12_MAX_VALUE_254	0x10
	#define AX12_MAX_VALUE_1023	0x00

	//Les defines sont globaux, il sont défini en même temps que le tableau des valeurs accepté par l'AX12 pour pouvoir voir à quelle valeur les limites correspondent
	static const Uint8 AX12_values_ranges[0x32] = {
		//EEPROM, utilisé par les fonctions AX12_config_*, les valeurs persiste après une mise hors tension.
		#define AX12_MODEL_NUMBER_L 			0x00
			0,	//non utilisé
		#define AX12_MODEL_NUMBER_H				0x01
			0,	//non utilisé
		#define AX12_VERSION					0x02
			0,	//non utilisé
		#define AX12_ID 						0x03
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_253,
		#define AX12_BAUD_RATE					0x04
			AX12_MIN_VALUE_34 | AX12_MAX_VALUE_254,	//la limite de l'AX12 est 0-254, mais la carte ne supporte pas des vitesses supérieures à 56700bauds.
		#define AX12_RETURN_DELAY_TIME			0x05
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//Maximum de 508 us
		#define AX12_CW_ANGLE_LIMIT_L			0x06
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_CW_ANGLE_LIMIT_H			0x07
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define AX12_CCW_ANGLE_LIMIT_L			0x08
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_CCW_ANGLE_LIMIT_H			0x09
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define AX12_SYSTEM_DATA2				0x0A
			0,	//non utilisé
		#define AX12_LIMIT_TEMPERATURE			0x0B
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_150,
		#define AX12_DOWN_LIMIT_VOLTAGE 		0x0C
			AX12_MIN_VALUE_50 | AX12_MAX_VALUE_250,
		#define AX12_UP_LIMIT_VOLTAGE			0x0D
			AX12_MIN_VALUE_50 | AX12_MAX_VALUE_250,
		#define AX12_MAX_TORQUE_L				0x0E
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_MAX_TORQUE_H				0x0F
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define AX12_RETURN_LEVEL				0x10
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_2,
		#define AX12_ALARM_LED					0x11
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_127,
		#define AX12_ALARM_SHUTDOWN 			0x12
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_127,
		#define AX12_OPERATING_MODE 			0x13
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1,
		#define AX12_DOWN_CALIBRATION_L 		0x14
			0,	//non utilisé
		#define AX12_DOWN_CALIBRATION_H 		0x15
			0,	//non utilisé
		#define AX12_UP_CALIBRATION_L			0x16
			0,	//non utilisé
		#define AX12_UP_CALIBRATION_H			0x17
			0,	//non utilisé

		//RAM de l'AX12, reinitialisé après une mise hors tension
		#define AX12_TORQUE_ENABLE				0x18
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1,
		#define AX12_LED						0x19
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1,
		#define AX12_CW_COMPLIANCE_MARGIN		0x1A
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,
		#define AX12_CCW_COMPLIANCE_MARGIN		0x1B
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,
		#define AX12_CW_COMPLIANCE_SLOPE		0x1C
			AX12_MIN_VALUE_1 | AX12_MAX_VALUE_254,
		#define AX12_CCW_COMPLIANCE_SLOPE		0x1D
			AX12_MIN_VALUE_1 | AX12_MAX_VALUE_254,
		#define AX12_GOAL_POSITION_L			0x1E
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_GOAL_POSITION_H			0x1F
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define AX12_GOAL_SPEED_L				0x20
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_GOAL_SPEED_H				0x21
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define AX12_TORQUE_LIMIT_L				0x22
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_TORQUE_LIMIT_H 			0x23
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas être utilisé
		#define AX12_PRESENT_POSITION_L 		0x24
			0,	//non utilisé
		#define AX12_PRESENT_POSITION_H 		0x25
			0,	//non utilisé
		#define AX12_PRESENT_SPEED_L			0x26
			0,	//non utilisé
		#define AX12_PRESENT_SPEED_H			0x27
			0,	//non utilisé
		#define AX12_PRESENT_LOAD_L 			0x28
			0,	//non utilisé
		#define AX12_PRESENT_LOAD_H 			0x29
			0,	//non utilisé
		#define AX12_PRESENT_VOLTAGE			0x2A
			0,	//non utilisé
		#define AX12_PRESENT_TEMPERATURE		0x2B
			0,	//non utilisé
		#define AX12_REGISTERED_INSTRUCTION		0x2C
			0,	//non utilisé
		#define AX12_PAUSE_TIME 				0x2D
			0,	//non utilisé
		#define AX12_MOVING 					0x2E
			0,	//non utilisé
		#define AX12_LOCK						0x2F
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1,
		#define AX12_PUNCH_L					0x30
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_PUNCH_H					0x31
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254	//ne devrait pas être utilisé
	};

	/***********************************************************************/
	/** Autre définitions internes                                        **/
	/***********************************************************************/

	//Si défini, on teste le checksum des paquets de status, si le resultat est incorrect, le paquet est ignoré
	//#define AX12_STATUS_RETURN_CHECK_CHECKSUM

	//nous avons besoin d'une place en plus pour pouvoir différencier le cas ou le buffer est vide du cas ou il est plein
	#define AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE (AX12_INSTRUCTION_BUFFER_SIZE+1)

	#define MIN_INSTRUCTION_PACKET_SIZE 6
	#define MAX_STATUS_PACKET_SIZE 8

	//Macro pour simplifier l'incrementation d'index (la fonction modulo fait une division assez lourde, donc profitons de super-macro
	//le modulo (%) c'est bien, mais c'est aussi beaucoup d'opération pour les cas utilisés (les valeurs ne doivent jamais dépasser 2*div)
	#define INC_WITH_MOD(val,div) (( (val)+1 < div )? (val)+1 : 0)

	/***********************************************************************/
	
typedef struct{
	volatile AX12_status_t last_status;
	Uint16 angle_limit[2];	//sauvegarde des angles limites lors d'un passage en mode wheel (rotation complète en continue) Si l'AX12 était déja dans ce mode, les angles limites défini à 0°-300°.
    bool_e is_wheel_enabled;
	// Possibilité d'ajout de membres ...
} AX12_servo_t;
 
//Paquet instruction a envoyer a l'AX12
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
} AX12_instruction_packet_t;


//Paquet renvoyé par l'AX12, 2 paramètres 8bits maximum
typedef struct{
	Uint8 id_servo;
	Uint8 error;
	union {
		Uint16 param;
		struct {
			Uint8 param_1;	//poids fort
			Uint8 param_2;	//poids faible
		};
	};
	Uint8 size;
} AX12_status_packet_t;

typedef struct{
	AX12_instruction_packet_t buffer[AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE];
	Uint8 start_index;
	Uint8 end_index;
} AX12_instruction_buffer;


//Machine à états

// Raison d'appel de la fonction de la machine à états
typedef enum {
	AX12_SME_NoEvent,
	AX12_SME_RxInterrupt,
	AX12_SME_TxInterrupt,
	AX12_SME_Timeout
} AX12_state_machine_event_e;

// Etat de la machine à états
typedef enum {
	AX12_SMS_ReadyToSend = 0,
	AX12_SMS_Sending,
	AX12_SMS_WaitingAnswer
} AX12_state_machine_state_e;


typedef struct {
	volatile AX12_state_machine_state_e state;
	volatile Uint16 timeout_counter;	//compteur décrémenté par le timer lorsque que l'état est AX12_SMS_WaitingAnswer

	AX12_instruction_packet_t current_instruction;

	//Utilisé en mode SEND
	Uint8 sending_index, receive_index;
} AX12_state_machine_t;

/*************************************************************************************/
	
// Variables globales, toutes les variables sont intialisée a zero, il n'y a pas de {0} ou autre pour eviter les warnings (avec -Wall) et c'est plus simple et pratique de rien mettre que des {{{0}},{0,0},0} et pour eviter d'avoir des warnings car les warnings c'est mal (défini dans l'ainsi C)

static AX12_servo_t AX12_on_the_robot[AX12_NUMBER];	//Tableau de structure contenant les informations liées à chaque AX12
static AX12_state_machine_t state_machine = {0};	//Machine à états du driver

//Queue contenant les instructions demandée (ce buffer est géré pas les fonctions/macro AX12_instruction_*, il n'y a qu'un buffer pour tous les AX12)
static volatile AX12_instruction_buffer AX12_special_instruction_buffer;
//Cette variable est à TRUE si le driver est en mode préparation de commandes. Voir doc de AX12_start_command_block dans le .h
static bool_e AX12_prepare_commands = FALSE;

/**************************************************************************/
/** Fonctions internes au driver de l'AX12+ et macros                    **/
/**************************************************************************/

static inline Uint16 AX12_decode_value_range(Uint8 mem_addr, bool_e get_max);	//get_max = 1 pour récupérer la valeur max pour une adresse donnée, get_max = 0 pour récupérer le minimum
static Uint8 AX12_instruction_packet_calc_checksum(AX12_instruction_packet_t* instruction_packet);
static Uint8 AX12_get_instruction_packet(Uint8 byte_offset, AX12_instruction_packet_t* instruction_packet);				//Renvoi l'octet d'une instruction a envoyer
#ifdef AX12_STATUS_RETURN_CHECK_CHECKSUM
static Uint8 AX12_status_packet_calc_checksum(AX12_status_packet_t* status_packet);
#endif
static bool_e AX12_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, AX12_status_packet_t* status_packet);	//retourne FALSE si le paquet est non valide, sinon TRUE
static void AX12_state_machine(AX12_state_machine_event_e event);
static void AX12_UART2_init(Uint32 uart_speed);


/**************************************************************************/
/** Fonctions/macro gérant les instructions et le buffer d'instruction   **/
/**************************************************************************/

/*
 * Les fonctions AX12_instruction_async_* sont asynchrones, utiliser AX12_instruction_wait pour attendre la fin de leurs exécutions avant AX12_instruction_get_last_status
 * Attention, ces fonctions attente que de la place se libère dans le buffer interne, attention lors de l'utilisation des fonctions de l'AX12 dans des interruptions
 * Pour prévenir le problème, vous pouvez utiliser AX12_instruction_buffer_is_full() ou changer la priorité de votre interruption
 * L'interruption de l'UART de l'AX12 a pour priorité 5 (pour Tx et Rx) (Si aucun changement n'est fait, l'IT d'un timer a une priorité de 4 comme la majeure partie des ITs)
 */

//Vous ne pouvez pas utiliser la fonction "AX12_instruction_*read" si le mode de retour d'information est AX12_STATUS_RETURN_NEVER

#if AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER
	static bool_e AX12_instruction_async_read(Uint8 id_servo, Uint8 address, Uint8 length);
	static Uint16 AX12_instruction_read16(Uint8 id_servo, Uint8 address, bool_e *isOk);
	static Uint8  AX12_instruction_read8(Uint8 id_servo, Uint8 address, bool_e *isOk);
#endif

static bool_e AX12_instruction_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e AX12_instruction_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e AX12_instruction_async_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e AX12_instruction_async_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e AX12_instruction_async_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e AX12_instruction_async_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e AX12_instruction_async_execute_write();
static bool_e AX12_instruction_ping(Uint8 id_servo);
static bool_e AX12_instruction_wait(Uint8 id_servo);
static bool_e AX12_instruction_buffer_is_full();
static AX12_status_t AX12_instruction_get_last_status(Uint8 id_servo);
static void AX12_instruction_reset_last_status(Uint8 id_servo);

static bool_e AX12_instruction_queue_insert(const AX12_instruction_packet_t* inst);
#define AX12_status_packet_is_full(status_packet, bytes_received) (((bytes_received) >= 4) && ((bytes_received) >= status_packet.size))	//on doit avoir lu la taille du paquet (>= 4) et avoir lu tous les octets du paquet
#define AX12_instruction_queue_is_full() (INC_WITH_MOD(AX12_special_instruction_buffer.end_index, AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE) == AX12_special_instruction_buffer.start_index)	//buffer plein si le prochain index de fin est l'index de début du buffer
#define AX12_instruction_queue_is_empty() (AX12_special_instruction_buffer.end_index == AX12_special_instruction_buffer.start_index)
#define AX12_instruction_queue_next() \
	(AX12_special_instruction_buffer.start_index = INC_WITH_MOD(AX12_special_instruction_buffer.start_index, AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE))
#define AX12_instruction_queue_get_current() \
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.start_index]


//retourne TRUE si l'instruction est suivie d'une réponse de l'AX12
//inst est de type AX12_instruction_packet_t
#define AX12_instruction_has_status_packet(inst) (inst.id_servo != AX12_BROADCAST_ID && (AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS || inst.type == INST_PING || (AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ONREAD && inst.type == INST_READ)))

/*************************************************************************************/
/** Fonctions gérant l'envoi de paquet instruction par l'intermédiaire du buffer    **/
/*************************************************************************************/

#if AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER
static bool_e AX12_instruction_async_read(Uint8 id_servo, Uint8 address, Uint8 length) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_READ;
	inst.address = address;
	inst.param_1 = length;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	return AX12_instruction_queue_insert(&inst);
}
#endif

static bool_e AX12_instruction_async_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	AX12_instruction_packet_t inst = {0};

	if((Uint16)value > AX12_decode_value_range(address, TRUE))
		value = AX12_decode_value_range(address, TRUE);

	if((Uint16)value < AX12_decode_value_range(address, FALSE))
		value = AX12_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_WRITE;
	inst.address = address;
	inst.param_1 = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	return AX12_instruction_queue_insert(&inst);
}

static bool_e AX12_instruction_async_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	AX12_instruction_packet_t inst = {0};

	if(value > AX12_decode_value_range(address, TRUE))
		value = AX12_decode_value_range(address, TRUE);

	if(value < AX12_decode_value_range(address, FALSE))
		value = AX12_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_WRITE;
	inst.address = address;
	inst.param = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 3;	//param is 16bits

	return AX12_instruction_queue_insert(&inst);
}

static bool_e AX12_instruction_async_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	AX12_instruction_packet_t inst = {0};

	if((Uint16)value > AX12_decode_value_range(address, TRUE))
		value = AX12_decode_value_range(address, TRUE);

	if((Uint16)value < AX12_decode_value_range(address, FALSE))
		value = AX12_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_REG_WRITE;
	inst.address = address;
	inst.param_1 = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	return AX12_instruction_queue_insert(&inst);
}

static bool_e AX12_instruction_async_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	AX12_instruction_packet_t inst = {0};

	if(value > AX12_decode_value_range(address, TRUE))
		value = AX12_decode_value_range(address, TRUE);

	if(value < AX12_decode_value_range(address, FALSE))
		value = AX12_decode_value_range(address, FALSE);

	inst.id_servo = id_servo;
	inst.type = INST_REG_WRITE;
	inst.address = address;
	inst.param = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 3;	//param is 16bits

	return AX12_instruction_queue_insert(&inst);
}

static bool_e AX12_instruction_async_execute_write() {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = AX12_BROADCAST_ID;
	inst.type = INST_ACTION;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	return AX12_instruction_queue_insert(&inst);
}

#if AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER
static Uint8 AX12_instruction_read8(Uint8 id_servo, Uint8 address, bool_e *isOk) {
	if(!AX12_instruction_async_read(id_servo, address, 1)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(!AX12_instruction_wait(id_servo)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(isOk) *isOk = TRUE;
	return AX12_instruction_get_last_status(id_servo).param_1;
}

static Uint16 AX12_instruction_read16(Uint8 id_servo, Uint8 address, bool_e *isOk) {
	if(!AX12_instruction_async_read(id_servo, address, 2)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(!AX12_instruction_wait(id_servo)) {
		if(isOk) *isOk = FALSE;
		return 0;
	}

	if(isOk) *isOk = TRUE;
	return AX12_instruction_get_last_status(id_servo).param;
}
#endif

static bool_e AX12_instruction_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	if(AX12_prepare_commands) {
		if(!AX12_instruction_async_prepare_write8(id_servo, address, value))
			return FALSE;
	} else {
		if(!AX12_instruction_async_write8(id_servo, address, value))
			return FALSE;
	}

	#if AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS
		if(id_servo == AX12_BROADCAST_ID)
			return TRUE;
		if(!AX12_instruction_wait(id_servo))
			return FALSE;
		return AX12_instruction_get_last_status(id_servo).error == 0;
	#endif

	return TRUE;
}

static bool_e AX12_instruction_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	if(AX12_prepare_commands) {
		if(!AX12_instruction_async_prepare_write16(id_servo, address, value))
			return FALSE;
	} else {
		if(!AX12_instruction_async_write16(id_servo, address, value))
			return FALSE;
	}

	#if AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS
		if(id_servo == AX12_BROADCAST_ID)
			return TRUE;
		if(!AX12_instruction_wait(id_servo))
			return FALSE;
		return AX12_instruction_get_last_status(id_servo).error == 0;
	#endif

	return TRUE;
}

static bool_e AX12_instruction_ping(Uint8 id_servo) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_PING;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	if(!AX12_instruction_queue_insert(&inst))
		return FALSE;

	if(!AX12_instruction_wait(id_servo))
		return FALSE;

	return (AX12_instruction_get_last_status(id_servo).error & AX12_ERROR_TIMEOUT) == 0;
}

static bool_e AX12_instruction_buffer_is_full() {
	return AX12_instruction_queue_is_full();
}

static bool_e AX12_instruction_wait(Uint8 id_servo) {
	Uint16 i = 0;
	//debug_printf("+1\n");
	while(!AX12_instruction_queue_is_empty() && i < 65000)	//si i atteint 65000, on stop, on a attendu trop longtemps (au moins 6,5ms à une clock de 10Mhz, mais ce bout de code ne fait qu'une instruction)
		i++;
		
	if(i < 65000) return TRUE;

	debug_printf("FATAL 2 %d / %d\n", AX12_special_instruction_buffer.start_index, AX12_special_instruction_buffer.end_index);
	AX12_on_the_robot[id_servo].last_status.error = AX12_ERROR_TIMEOUT | AX12_ERROR_RANGE;	//On a attendu trop longtemps, le buffer est toujours plein
	AX12_on_the_robot[id_servo].last_status.param = 0;
	return FALSE;
}

static AX12_status_t AX12_instruction_get_last_status(Uint8 id_servo)
{
	return AX12_on_the_robot[id_servo].last_status;
}

static void AX12_instruction_reset_last_status(Uint8 id_servo) {
	AX12_on_the_robot[id_servo].last_status.error = AX12_ERROR_OK;
	AX12_on_the_robot[id_servo].last_status.param = 0;
}

/*****************************************************************************/
/** Fonctions internes pour la gestion des paquets et les limites de valeur **/
/*****************************************************************************/

static inline Uint16 AX12_decode_value_range(Uint8 mem_addr, bool_e get_max) {
	if(mem_addr >= AX12_MEM_SIZE) return 0;

	if(!get_max) {
		switch(AX12_values_ranges[mem_addr] & 0x0F) {
			case AX12_MIN_VALUE_0:  return 0;
			case AX12_MIN_VALUE_1:  return 1;
			case AX12_MIN_VALUE_34: return 34;
			case AX12_MIN_VALUE_50: return 50;
		}
	} else {
		switch(AX12_values_ranges[mem_addr] & 0xF0) {
			case AX12_MAX_VALUE_1023: return 1023;
			case AX12_MAX_VALUE_254:  return 254;
			case AX12_MAX_VALUE_253:  return 253;
			case AX12_MAX_VALUE_250:  return 250;
			case AX12_MAX_VALUE_150:  return 150;
			case AX12_MAX_VALUE_127:  return 127;
			case AX12_MAX_VALUE_2:    return 2;
			case AX12_MAX_VALUE_1:    return 1;
		}
	}

	return 0;
}

static Uint8 AX12_instruction_packet_calc_checksum(AX12_instruction_packet_t* instruction_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = instruction_packet->size - 4;

	if(instruction_packet->size < 6)
		debug_printf("AX12: Error: in AX12_instruction_packet_calc_checksum(): paquet trop petit");

	//packet_length est forcément >= 2 (voir datasheet de l'ax12)
	//PAS de break, si il y a 3 paramètres (packet_length = 3 +2), on fait toutes les additions
	switch(packet_length) {
		case 5:	//3 paramètres
			checksum += instruction_packet->param_2;
		case 4:	//2 paramètres
			checksum += instruction_packet->param_1;
		case 3:	//1 paramètre
			checksum += instruction_packet->address;
		case 2:	//pas de paramètre
			checksum += instruction_packet->id_servo + packet_length + instruction_packet->type;
			break;

		default:
			debug_printf("AX12: Error: AX12_instruction_packet_calc_checksum(): paquet trop grand");
	}

	return ~checksum;
}

static Uint8 AX12_get_instruction_packet(Uint8 byte_offset, AX12_instruction_packet_t* instruction_packet) {

	if(byte_offset == instruction_packet->size - 1)	//si c'est le dernier octet, envoyer le checksum
		return AX12_instruction_packet_calc_checksum(instruction_packet);

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
				debug_printf("AX12: Warning: byte_offset > 7\r\n");
	}

	//Si on atteint cette ligne, byte_offset > 7, on retourne 0 par défaut (ça ne devrais jamais arriver)
	return 0;
}

#ifdef AX12_STATUS_RETURN_CHECK_CHECKSUM
static Uint8 AX12_status_packet_calc_checksum(AX12_status_packet_t* status_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = status_packet->size - 4;

	if(status_packet->size < 6)
		debug_printf("AX12: Error: in AX12_status_packet_calc_checksum(): paquet trop petit");

	//packet_length est forcément >= 2 (voir datasheet de l'ax12)
	//PAS de break, si il y a 2 paramètre, on fait toutes les additions
	switch(packet_length) {
		case 4:	//2 paramètres
			checksum += status_packet->param_2;
		case 3:	//1 paramètre
			checksum += status_packet->param_1;
		case 2:	//pas de paramètre
			checksum += status_packet->id_servo + packet_length + status_packet->error;
			break;

		default:
			debug_printf("AX12: Error: AX12_status_packet_calc_checksum(): paquet trop grand");
	}

	return ~checksum;
}
#endif

static bool_e AX12_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, AX12_status_packet_t* status_packet)
{
	//Si c'est le dernier octet, verifier le checksum si AX12_STATUS_RETURN_CHECK_CHECKSUM est defini
	if(byte_offset > 3 && byte_offset == status_packet->size - 1) {
	#ifdef AX12_STATUS_RETURN_CHECK_CHECKSUM
		if(receive_byte != AX12_status_packet_calc_checksum(status_packet)) {
			debug_printf("AX12[%d]: invalid checksum\n", status_packet->id_servo);
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
			status_packet->error = 0;
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
			status_packet->error = receive_byte;
			break;

		case 5:
			status_packet->param_1 = receive_byte;
			break;
			
		case 6:
			status_packet->param_2 = receive_byte;
			break;

		default:
			if(byte_offset > 6)
				debug_printf("AX12: Error: AX12_update_status_packet(): trop d'octet reçu > 6\n");
	}

	return TRUE;
}

//La machine a états est appelée par les interruptions de l'UART et par le timer
static void AX12_state_machine(AX12_state_machine_event_e event) {
	static volatile int processing_state = 0;

	#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
		//Pour le deboggage (avec debug_printf à la fin de l'envoi/reception d'un paquet)
		static Uint8 AX12_UART2_reception_buffer[MAX_STATUS_PACKET_SIZE*2] = {0};
		Uint8 i, pos;
	#endif

//sécurisation des appels à cette fonction lorsque event == AX12_SME_NoEvent pour eviter que l'état AX12_SMS_ReadyToSend soit exécuté plusieurs fois
//si une IT arrive a un certain moment, elle sera executée complètement, donc processing_state reviendra à son état d'origine
	if(event == AX12_SME_NoEvent) {
//1) Si une IT arrive a ce moment, elle s'execute entièrement et change l'état de AX12_SMS_ReadyToSend en AX12_SMS_Sending, cet appel continuera et le switch branchera sur AX12_SMS_Sending, il n'y a rien de prévu pour AX12_SME_NoEvent donc cet appel ne fera rien au final
		if(processing_state > 0)
//2) Si une IT arrive a ce moment, elle s'executera pas, elle retournera sans rien faire car processing_state est > 0
			return;
//3) Si une IT arrive a ce moment, elle s'executera entièrement car processing_state == 0, l'état passera de AX12_SMS_ReadyToSend à AX12_SMS_Sending et cet appel de fonction ne fera rien comme au 1)
		processing_state = 1;
//4) Si une IT arrive a ce moment, elle ne s'executera pas car processing_state > 0
	} else if(event == AX12_SME_Timeout) {
		NVIC_DisableIRQ(AX12_UART_Interrupt_IRQn);
	} else if(event == AX12_SME_RxInterrupt || event == AX12_SME_TxInterrupt) {
		TIMER_SRC_TIMER_DisableIT();
	}

//Si une IT arrive avec event == AX12_SME_TxInterrupt, elle n'aura un effet que dans l'état AX12_SMS_Sending (et cette interruption ne devrai être lancée que après le putcUART2 dans AX12_SMS_ReadyToSend et pas dans AX12_SMS_Sending car une interruption n'est pas prioritaire sur elle-même)
//Si une IT arrive avec event == AX12_SME_RxInterrupt, elle n'aura un effet que dans l'état AX12_SMS_WaitingAnswer. Elle ne peut être lancée que si le timer n'a pas eu d'interruption et désactive l'interruption du timer

	switch(state_machine.state)
	{
		case AX12_SMS_ReadyToSend:
			if(event == AX12_SME_NoEvent)
			{
				// Choix du paquet à envoyer et début d'envoi
				if(!AX12_instruction_queue_is_empty())
					state_machine.current_instruction = AX12_instruction_queue_get_current();
				else {	//s'il n'y a rien a faire, mettre en veille la machine a état, l'UART sera donc inactif (et mettre en mode reception pour ne pas forcer la sortie dont on défini la tension, celle non relié a l'AX12)
					while(!USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_TC));
					AX12_DIRECTION_PORT = RX_DIRECTION;
					break;
				}

				state_machine.state = AX12_SMS_Sending;
				state_machine.sending_index = 0;
				state_machine.receive_index = 0;


				#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
					debug_printf("AX12 Tx:");
					for(i = 0; i<state_machine.current_instruction.size; i++)
						debug_printf(" %02x", AX12_get_instruction_packet(i, &state_machine.current_instruction));
					debug_printf("\n");
				#endif
 

				AX12_DIRECTION_PORT = TX_DIRECTION;
				
				TIMER_SRC_TIMER_start_ms(AX12_STATUS_SEND_TIMEOUT);	//Pour le timeout d'envoi, ne devrait pas arriver

				state_machine.sending_index++;	//Attention! Nous devons incrementer sending_index AVANT car il y a un risque que l'interuption Tx arrive avant l'incrementation lorsque cette fonction est appellée par AX12_init() (qui n'est pas dans une interruption)

				USART_SendData(AX12_UART_Ptr, AX12_get_instruction_packet(state_machine.sending_index-1, &state_machine.current_instruction));
			}
		break;

		case AX12_SMS_Sending:
			if(event == AX12_SME_TxInterrupt)
			{
				if(state_machine.sending_index < state_machine.current_instruction.size) {
					USART_SendData(AX12_UART_Ptr, AX12_get_instruction_packet(state_machine.sending_index, &state_machine.current_instruction));
					state_machine.sending_index++;
				}
				else	//Le dernier paquet a été envoyé, passage en mode reception et attente de la réponse dans l'état AX12_SMS_WaitingAnswer s'il y a ou enchainement sur le prochain paquet à evnoyer (AX12_SMS_ReadyToSend)
				{
					TIMER_SRC_TIMER_stop();
					TIMER_SRC_TIMER_resetFlag();

					if(AX12_instruction_has_status_packet(state_machine.current_instruction))
					{
						#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
							for(i=0; i<MAX_STATUS_PACKET_SIZE*2; i++)
								AX12_UART2_reception_buffer[i] = 0;
							pos = 0;
						#endif

						//Attente de la fin de la transmition des octets
						//TRMT passe a 1 quand tout est envoyé (bit de stop inclu)
						//plus d'info ici: http://books.google.fr/books?id=ZNngQv_E0_MC&lpg=PA250&ots=_ZTiXKt-8p&hl=fr&pg=PA250
						while(!USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_TC));

						AX12_DIRECTION_PORT = RX_DIRECTION;

						//flush recv buffer
						while(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_ORE) || USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_FE) || USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_NE))
							USART_ReceiveData(AX12_UART_Ptr);

						state_machine.state = AX12_SMS_WaitingAnswer;

						TIMER_SRC_TIMER_start_ms(AX12_STATUS_RETURN_TIMEOUT);	//Pour le timeout de reception, ne devrait pas arriver
					}
					else
					{
						AX12_instruction_queue_next();
						state_machine.state = AX12_SMS_ReadyToSend;
						AX12_state_machine(AX12_SME_NoEvent);
					}
				}
			} else if(event == AX12_SME_Timeout) {
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();
				debug_printf("AX12[%d]: send timeout !!\n", state_machine.current_instruction.id_servo);
				//U2MODEbits.UARTEN = 0;
				//AX12_UART2_init(AX12_BAUD_RATE);

				AX12_instruction_queue_next();
				state_machine.state = AX12_SMS_ReadyToSend;
				AX12_state_machine(AX12_SME_NoEvent);
			}
		break;

		case AX12_SMS_WaitingAnswer:
			if(event == AX12_SME_RxInterrupt)
			{
				//AX12_update_status_packet s'occupe de son initialisation et de son remplissage au fur et a mesure que les octets arrivent
				static AX12_status_packet_t status_response_packet;
				// Stockage de la réponse dans un buffer, si toute la réponse alors mise à jour des variables du driver et state = AX12_SMS_ReadyToSend avec éxécution

				if(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_FE) | USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_NE)) { //ignore error bits
					USART_ReceiveData(AX12_UART_Ptr);
					break;
				}

				#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
				AX12_UART2_reception_buffer[pos] = USART_ReceiveData(AX12_UART_Ptr);

				if(!AX12_update_status_packet(AX12_UART2_reception_buffer[pos++], state_machine.receive_index, &status_response_packet))
				#else
				Uint8 data_byte = USART_ReceiveData(AX12_UART_Ptr);
				if(!AX12_update_status_packet(data_byte, state_machine.receive_index, &status_response_packet))
				#endif
				{
					//debug_printf("AX12: invalid packet, reinit reception\n");
					state_machine.receive_index = 0;	//si le paquet n'est pas valide, on reinitialise la lecture de paquet
					//pos = 0;
					//for(i=0; i<MAX_STATUS_PACKET_SIZE*2; i++)
					//	AX12_UART2_reception_buffer[i] = 0;
					break;
				} else {
					state_machine.receive_index++;
				}


				if(AX12_status_packet_is_full(status_response_packet, state_machine.receive_index))
				{
					TIMER_SRC_TIMER_stop();
					TIMER_SRC_TIMER_resetFlag();

					#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
						debug_printf("AX12 Rx:");
						for(i = 0; i<state_machine.receive_index; i++)
							debug_printf(" %02x", AX12_UART2_reception_buffer[i]);
						debug_printf("\n");
					#endif

					if(status_response_packet.id_servo != state_machine.current_instruction.id_servo || status_response_packet.id_servo >= AX12_NUMBER) {
						debug_printf("Wrong servo ID: %d instead of %d\n", status_response_packet.id_servo, state_machine.current_instruction.id_servo);
					} else {
						//pour être sur de ne pas avoir le bit 7 a 1, si l'AX12 le met a 1, on met tous les bits a 1
						if(status_response_packet.error & 0x80)
							AX12_on_the_robot[status_response_packet.id_servo].last_status.error = 0xFF;
						else AX12_on_the_robot[status_response_packet.id_servo].last_status.error = status_response_packet.error & 0x7F;
						AX12_on_the_robot[status_response_packet.id_servo].last_status.param = status_response_packet.param;

						#ifdef VERBOSE_MODE
							if(status_response_packet.error & AX12_ERROR_VOLTAGE)
								debug_printf("AX12[%d] Fatal: Voltage error\n", status_response_packet.id_servo);
							if(status_response_packet.error & AX12_ERROR_ANGLE)
								debug_printf("AX12[%d] Error: Angle error\n", status_response_packet.id_servo);
							if(status_response_packet.error & AX12_ERROR_OVERHEATING)
								debug_printf("AX12[%d] Fatal: Overheating error\n", status_response_packet.id_servo);
							if(status_response_packet.error & AX12_ERROR_RANGE)
								debug_printf("AX12[%d] Error: Range error\n", status_response_packet.id_servo);
							if(status_response_packet.error & AX12_ERROR_CHECKSUM)
								debug_printf("AX12[%d] Error: Checksum error\n", status_response_packet.id_servo);
							if(status_response_packet.error & AX12_ERROR_OVERLOAD)
								debug_printf("AX12[%d] Fatal: Overload error\n", status_response_packet.id_servo);
							if(status_response_packet.error & AX12_ERROR_INSTRUCTION)
								debug_printf("AX12[%d] Error: Instruction error\n", status_response_packet.id_servo);
							if(status_response_packet.error & 0x80)
								debug_printf("AX12[%d] Fatal: Unknown (0x80) error\n", status_response_packet.id_servo);
							if(status_response_packet.error)
								debug_printf("AX12[%d] Cmd: %d, addr: 0x%x, param: 0x%x\n",
										state_machine.current_instruction.id_servo,
										state_machine.current_instruction.type,
										state_machine.current_instruction.address,
										state_machine.current_instruction.param);
						#endif
					}
					AX12_instruction_queue_next();
					state_machine.state = AX12_SMS_ReadyToSend;
					AX12_state_machine(AX12_SME_NoEvent);
				}
			} else if(event == AX12_SME_Timeout) {
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();

				#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
					debug_printf("AX12[%d] timeout Rx:", state_machine.current_instruction.id_servo);
					for(i = 0; i<pos; i++)
						debug_printf(" %02x", AX12_UART2_reception_buffer[i]);
					debug_printf(", recv idx: %d\n", state_machine.receive_index);
					debug_printf(" Original cmd: Id: %02x Cmd: %02x Addr: %02x param: %04x\n", state_machine.current_instruction.id_servo, state_machine.current_instruction.type, state_machine.current_instruction.address, state_machine.current_instruction.param);
				#endif
				
				if(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE))
					debug_printf("AX12 timeout too small\n");

				AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = AX12_ERROR_TIMEOUT;
				AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.param = 0;
				AX12_instruction_queue_next();
				state_machine.state = AX12_SMS_ReadyToSend;
				AX12_state_machine(AX12_SME_NoEvent);
			}
		break;
	}

	if(event == AX12_SME_RxInterrupt || event == AX12_SME_TxInterrupt) {
		TIMER_SRC_TIMER_EnableIT();
	} else if(event == AX12_SME_Timeout) {
		NVIC_EnableIRQ(AX12_UART_Interrupt_IRQn);
	} else if(event == AX12_SME_NoEvent) {
		processing_state = 0;
	}
}

/*********************************************************************************/
/** Implémentation des fonctions gérant le buffer d'instruction (insertion ici) **/
/*********************************************************************************/

static bool_e AX12_instruction_queue_insert(const AX12_instruction_packet_t* inst) {	//utilisation d'un pointeur pour eviter de provoquer une copie intégrale de la structure, qui pourrait prendre du temps si elle est trop grande
	Uint16 i = 0;
	//Uint16 truc;

	while(AX12_instruction_queue_is_full() && i < 65000)	//boucle 65000 fois si le buffer reste full, si on atteint 65000, on échoue et retourne FALSE
		i++;
		
	if(i >= 65000) {
		AX12_on_the_robot[inst->id_servo].last_status.error = AX12_ERROR_TIMEOUT | AX12_ERROR_RANGE;
		AX12_on_the_robot[inst->id_servo].last_status.param = 0;
		debug_printf("FATAL\n");
		return FALSE;	//return false, on a pas réussi a insérer l'instruction, problème de priorité d'interruptions ?
	}

	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index] = *inst;
	AX12_special_instruction_buffer.end_index = INC_WITH_MOD(AX12_special_instruction_buffer.end_index, AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE);
	//truc = state_machine.state;
	//if(truc == AX12_SMS_ReadyToSend)
		AX12_state_machine(AX12_SME_NoEvent);
	//else debug_printf("ax12 not ready, in state %d\n", truc);
		
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

static void AX12_UART2_init(Uint32 uart_speed)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannel = AX12_UART_Interrupt_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;

	NVIC_Init(&NVIC_InitStructure);


	//PORTS_uarts_init();

#if AX12_UART_ID == 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#elif AX12_UART_ID == 2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif

	USART_OverSampling8Cmd(AX12_UART_Ptr, ENABLE);

	USART_InitStructure.USART_BaudRate = uart_speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(AX12_UART_Ptr, &USART_InitStructure);


	/* Enable USART */
	USART_Cmd(AX12_UART_Ptr, ENABLE);

	USART_ITConfig(AX12_UART_Ptr, USART_IT_RXNE, ENABLE);
	USART_ITConfig(AX12_UART_Ptr, USART_IT_TXE, ENABLE);
}

/*****************************************************************************/
/** Interruptions utilisées (Reception/Envoi UART2 et timer pour le timeout **/
/*****************************************************************************/


//Attention ! Si on met a 0 le flag après avoir executé la machine a état,
//des interruptions peuvent être masquée
//(en gros il se peut qu'une interruption Tx arrive avant d'avoir terminée l'execution de la fonction,
//dans ce cas mettre le flag a 0 fait que l'interruption Tx n'est pas lancée)
//(car le caractère envoyé est envoyé plus vite que le retour de la fonction AX12_state_machine)
void _ISR AX12_UART_Interrupt(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		Uint8 i = 0;
		while(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE)) {		//On a une IT Rx pour chaque caratère reçu, donc on ne devrai pas tomber dans un cas avec 2+ char dans le buffer uart dans une IT
			if(state_machine.state != AX12_SMS_WaitingAnswer) {	//Arrive quand on allume les cartes avant la puissance ou lorsque l'on coupe la puissance avec les cartes alumées (reception d'un octet avec l'erreur FERR car l'entrée RX tombe à 0)
				USART_ReceiveData(AX12_UART_Ptr);
			} else {
				AX12_state_machine(AX12_SME_RxInterrupt);
				if(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE) && i > 5) {
					//debug_printf("Overinterrupt RX !\n");
					break; //force 0, on va perdre des caractères, mais c'est mieux que de boucler ici ...
				}
			}
			i++;
		}
	}
	else if(USART_GetITStatus(USART1, USART_IT_TXE))
	{
		AX12_state_machine(AX12_SME_TxInterrupt);
	}
}

void TIMER_SRC_TIMER_interrupt()
{
	Uint8 i = 0;

	while(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE)) {		//On a une IT Rx pour chaque caratère reçu, donc on ne devrai pas tomber dans un cas avec 2+ char dans le buffer uart dans une IT
		if(state_machine.state != AX12_SMS_WaitingAnswer) {	//Arrive quand on allume les cartes avant la puissance ou lorsque l'on coupe la puissance avec les cartes alumées (reception d'un octet avec l'erreur FERR car l'entrée RX tombe à 0)
			USART_ReceiveData(AX12_UART_Ptr);
		} else {
			AX12_state_machine(AX12_SME_RxInterrupt);
			if(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE) && i > 5) {
				debug_printf("Overinterrupt RX ! while timeout\n");
				break; //force 0, on va perdre des caractères, mais c'est mieux que de boucler ici ...
			}
		}
		i++;
	}
	AX12_state_machine(AX12_SME_Timeout);
	TIMER_SRC_TIMER_resetFlag();
}

/**************************************************************************/
/** Fonctions d'interface utilisé par l'utilisateur du driver            **/
/**************************************************************************/

/* Fonction initialisant ce Driver	                                     */

void AX12_init() {
	static bool_e initialized = FALSE;
	Uint8 i;
	if(initialized)
		return;

	AX12_UART2_init(AX12_UART_BAUDRATE);
	TIMER_SRC_TIMER_init();
	AX12_DIRECTION_PORT = RX_DIRECTION;

	AX12_prepare_commands = FALSE;
	AX12_instruction_write8(AX12_BROADCAST_ID, AX12_RETURN_LEVEL, AX12_STATUS_RETURN_MODE);	//Mettre les AX12 dans le mode indiqué dans Global_config.h

	for(i=0; i<AX12_NUMBER; i++) {
		AX12_on_the_robot[i].angle_limit[0] = 0;
		AX12_on_the_robot[i].angle_limit[1] = 300;

		AX12_on_the_robot[i].is_wheel_enabled = FALSE;
		AX12_instruction_reset_last_status(i);
	}
}

/* Fonction d'utilisation de l'AX12                                        */

//Configuration de l'AX12, perdure après mise hors tension sauf pour le verouillage de la config (lock).
//Unités:
// Angles en degrés
// Vitesse en degrée par seconde
// Voltage en dixième de volt (50 => 5.0V)
// Température en degrée celcius
// Pourcentage entre 0 et 100

//Angle max: 360°
#define AX12_MAX_DEGRE 360
#define AX12_ANGLE_TO_DEGRE(angle) ((((Uint16)(angle))*75) >> 8) // >> 8 <=> / 256, 75/256 = 300/1024 = 0.29296875
//#define AX12_DEGRE_TO_ANGLE(angle) ((((Uint32)(angle)) << 8) / 75)	//L'utilisation d'entier 32bits est nécessaire, l'angle max 360° * 256 donne un nombre supérieur à 65535
//Aproximation pour eviter les entier 32bits (le dspic30F est 16bits), une unité d'angle vaut 0.296875°, perte de précision de 1,3%, AX12_DEGRE_TO_ANGLE(300) donne un angle réel de 296°, soit une perte de 4° pour un angle de 300°
#define AX12_DEGRE_TO_ANGLE(angle) ((((Uint16)(angle)) << 7) / 38)

//Vitesse max: 500°/seconde (83 tours/minute) (maximum supporté par la macro)
//Le max de l'ax12 indiqué par la datasheet est de environ 360°/s (60 tr/min)
#define AX12_MAX_DPS 500
#define AX12_SPEED_TO_DPS(angle_speed) ((((Uint16)(angle_speed))*85) >> 7) // >> 7 <=> / 128, 85/256 = 0.664 degres per second ~= 0.111 rpm (tours par minute)
#define AX12_DPS_TO_SPEED(angle_speed) ((((Uint16)(angle_speed)) << 7) / 85)

//Poucentage max: 100% (tout le monde le savait ça, mais c'est bien de le repréciser :) )
#define AX12_MAX_PERCENTAGE 100
#define AX12_1024_TO_PERCENTAGE(percentage) ((((Uint16)(percentage))*25) >> 8) // >> 8 <=> / 256, 25/256 = 100/1024
#define AX12_PERCENTAGE_TO_1024(percentage) ((((Uint16)(percentage)) << 8) / 25)

//Implementation

bool_e AX12_is_ready(Uint8 id_servo) {
	return AX12_instruction_ping(id_servo);
}

AX12_status_t AX12_get_last_error(Uint8 id_servo) {
	return AX12_instruction_get_last_status(id_servo);
}

void AX12_reset_last_error(Uint8 id_servo) {
	AX12_instruction_reset_last_status(id_servo);
}

bool_e AX12_is_buffer_full() {
	return AX12_instruction_buffer_is_full();
}

void AX12_start_command_block() {
	AX12_prepare_commands = TRUE;
}

void AX12_end_command_block() {
	AX12_instruction_async_execute_write();
	AX12_prepare_commands = FALSE;
}

#if AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER
bool_e AX12_config_is_locked(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_LOCK, NULL) != 0;
}

Uint16 AX12_config_get_model_number(Uint8 id_servo) {
	return AX12_instruction_read16(id_servo, AX12_MODEL_NUMBER_L, NULL);
}

Uint8  AX12_config_get_firmware_version(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_VERSION, NULL);
}

Uint16 AX12_config_get_return_delay_time(Uint8 id_servo) {
	return ((Uint16)AX12_instruction_read8(id_servo, AX12_RETURN_DELAY_TIME, NULL)) * 2;
}

Uint16 AX12_config_get_minimal_angle(Uint8 id_servo) {
	return AX12_ANGLE_TO_DEGRE(AX12_instruction_read16(id_servo, AX12_CW_ANGLE_LIMIT_L, NULL));
}

Uint16 AX12_config_get_maximal_angle(Uint8 id_servo) {
	return AX12_ANGLE_TO_DEGRE(AX12_instruction_read16(id_servo, AX12_CCW_ANGLE_LIMIT_L, NULL));
}

Uint8  AX12_config_get_temperature_limit(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_LIMIT_TEMPERATURE, NULL);
}

Uint8  AX12_config_get_lowest_voltage(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_DOWN_LIMIT_VOLTAGE, NULL);
}

Uint8  AX12_config_get_highest_voltage(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_UP_LIMIT_VOLTAGE, NULL);
}

Uint8 AX12_config_get_maximum_torque(Uint8 id_servo) {
	Uint16 value;    //On ne peut pas utiliser la lecture 16bits a cause d'un bug de l'AX12 (voir https://sites.google.com/site/robotsaustralia/ax-12dynamixelinformation)
	value = AX12_instruction_read8(id_servo, AX12_MAX_TORQUE_L, NULL) | (AX12_instruction_read8(id_servo, AX12_MAX_TORQUE_H, NULL) << 8);
	return (Uint8)AX12_1024_TO_PERCENTAGE(value);
}

Uint8  AX12_config_get_status_return_mode(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_RETURN_LEVEL, NULL);
}

Uint8  AX12_config_get_error_before_led(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_ALARM_LED, NULL);
}

Uint8  AX12_config_get_error_before_shutdown(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_ALARM_SHUTDOWN, NULL);
}
#endif //AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER

bool_e AX12_config_lock(Uint8 id_servo) {
	return AX12_instruction_write8(id_servo, AX12_LOCK, 1);
}

bool_e AX12_config_set_return_delay_time(Uint8 id_servo, Uint16 delay_us) {
	return AX12_instruction_write8(id_servo, AX12_RETURN_DELAY_TIME, (Uint8)(delay_us >> 1)); // >> 1 <=> /2
}

bool_e AX12_config_set_minimal_angle(Uint8 id_servo, Uint16 degre) {
	if(degre > AX12_MAX_DEGRE) degre = AX12_MAX_DEGRE;
	return AX12_instruction_write16(id_servo, AX12_CW_ANGLE_LIMIT_L, AX12_DEGRE_TO_ANGLE(degre));
}

bool_e AX12_config_set_maximal_angle(Uint8 id_servo, Uint16 degre) {
	if(degre > AX12_MAX_DEGRE) degre = AX12_MAX_DEGRE;
	if(degre == 0) degre = 1;	//Si l'utilisateur met un angle mini et maxi à 0, l'AX12 passera en mode rotation en continue, ce mode ne doit être activé que par AX12_set_wheel_mode_enabled (en passant TRUE)
	return AX12_instruction_write16(id_servo, AX12_CCW_ANGLE_LIMIT_L, AX12_DEGRE_TO_ANGLE(degre));
}

bool_e AX12_config_set_temperature_limit(Uint8 id_servo, Uint8 temperature) {
	return AX12_instruction_write8(id_servo, AX12_LIMIT_TEMPERATURE, temperature);
}

bool_e AX12_config_set_lowest_voltage(Uint8 id_servo, Uint8 voltage) {
	return AX12_instruction_write8(id_servo, AX12_DOWN_LIMIT_VOLTAGE, voltage);
}

bool_e AX12_config_set_highest_voltage(Uint8 id_servo, Uint8 voltage) {
	return AX12_instruction_write8(id_servo, AX12_UP_LIMIT_VOLTAGE, voltage);
}

bool_e AX12_config_set_maximum_torque_percentage(Uint8 id_servo, Uint8 percentage) {
	Uint16 value = AX12_PERCENTAGE_TO_1024(percentage);
	return AX12_instruction_write16(id_servo, AX12_MAX_TORQUE_L, value);
}

bool_e AX12_config_set_status_return_mode(Uint8 id_servo, Uint8 mode) {
	return AX12_instruction_write8(id_servo, AX12_RETURN_LEVEL, mode);
}

bool_e AX12_config_set_error_before_led(Uint8 id_servo, Uint8 error_type) {
	return AX12_instruction_write8(id_servo, AX12_ALARM_LED, error_type);
}

bool_e AX12_config_set_error_before_shutdown(Uint8 id_servo, Uint8 error_type) {
	return AX12_instruction_write8(id_servo, AX12_ALARM_SHUTDOWN, error_type);
}


//RAM

bool_e AX12_is_wheel_mode_enabled(Uint8 id_servo) {
	return AX12_on_the_robot[id_servo].is_wheel_enabled;
}

#if AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER
bool_e AX12_is_torque_enabled(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_TORQUE_ENABLE, NULL) != 0;
}

bool_e AX12_is_led_enabled(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_LED, NULL) != 0;
}

bool_e AX12_get_torque_response(Uint8 id_servo, Uint16* A, Uint16* B, Uint16* C, Uint16* D) {
	bool_e isOk;
	Uint8 CcwComplianceSlope;
	Uint8 CcwComplianceMargin;
	Uint8 CwComplianceMargin;
	Uint8 CwComplianceSlope;

	CcwComplianceSlope = AX12_instruction_read8(id_servo, AX12_CCW_COMPLIANCE_SLOPE, &isOk);
	if(!isOk) return FALSE;
	CcwComplianceMargin = AX12_instruction_read8(id_servo, AX12_CCW_COMPLIANCE_MARGIN, &isOk);
	if(!isOk) return FALSE;
	CwComplianceMargin = AX12_instruction_read8(id_servo, AX12_CW_COMPLIANCE_MARGIN, &isOk);
	if(!isOk) return FALSE;
	CwComplianceSlope = AX12_instruction_read8(id_servo, AX12_CW_COMPLIANCE_SLOPE, &isOk);
	if(!isOk) return FALSE;

	if(A) *A = AX12_ANGLE_TO_DEGRE(CcwComplianceSlope);
	if(B) *B = AX12_ANGLE_TO_DEGRE(CcwComplianceMargin);
	if(C) *C = AX12_ANGLE_TO_DEGRE(CwComplianceMargin);
	if(D) *D = AX12_ANGLE_TO_DEGRE(CwComplianceSlope);

	return TRUE;
}

Uint16 AX12_get_position(Uint8 id_servo) {
	return AX12_ANGLE_TO_DEGRE(AX12_instruction_read16(id_servo, AX12_PRESENT_POSITION_L, NULL));
}

Sint16 AX12_get_move_to_position_speed(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 speed;

	if(AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_get_move_to_position_speed while in wheel mode, use AX12_get_speed_percentage instead.\n");
		return 0;
	}

	speed = AX12_instruction_read16(id_servo, AX12_PRESENT_SPEED_L, NULL);

	isBackward = (speed & 0x400) != 0;
	speed = AX12_SPEED_TO_DPS(speed & 0x3FF);
	if(isBackward)
		return -(Sint16)speed;
	return (Sint16)speed;
}

Sint8 AX12_get_speed_percentage(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 speedPercentage;

	if(!AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_get_speed_percentage while not in wheel mode, use AX12_get_move_to_position_speed instead.\n");
		return 0;
	}

	speedPercentage = AX12_instruction_read16(id_servo, AX12_PRESENT_SPEED_L, NULL);

	isBackward = (speedPercentage & 0x400) != 0;
	speedPercentage = AX12_1024_TO_PERCENTAGE(speedPercentage & 0x3FF);
	if(isBackward)
		return -(Sint8)speedPercentage;
	return (Sint8)speedPercentage;
}

Uint8 AX12_get_torque_limit(Uint8 id_servo) {
	return (Uint8)AX12_1024_TO_PERCENTAGE(AX12_instruction_read16(id_servo, AX12_TORQUE_LIMIT_L, NULL));
}

Uint8 AX12_get_punch_torque_percentage(Uint8 id_servo) {
	return (Uint8)AX12_1024_TO_PERCENTAGE(AX12_instruction_read16(id_servo, AX12_PUNCH_L, NULL));
}

Sint8 AX12_get_load_percentage(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 loadPercentage;

	loadPercentage = AX12_instruction_read16(id_servo, AX12_PRESENT_LOAD_L, NULL);

	isBackward = (loadPercentage & 0x400) != 0;
	loadPercentage = AX12_1024_TO_PERCENTAGE(loadPercentage & 0x3FF);
	if(isBackward)
		return -(Sint8)loadPercentage;
	return (Sint8)loadPercentage;
}

Uint8 AX12_get_voltage(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_PRESENT_VOLTAGE, NULL);
}

Uint8 AX12_get_temperature(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_PRESENT_TEMPERATURE, NULL);
}

bool_e AX12_is_instruction_prepared(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_REGISTERED_INSTRUCTION, NULL) != 0;
}

bool_e AX12_is_moving(Uint8 id_servo) {
	return AX12_instruction_read8(id_servo, AX12_MOVING, NULL) != 0;
}

#endif //AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER


bool_e AX12_set_wheel_mode_enabled(Uint8 id_servo, bool_e enabled) {
	if(enabled == AX12_on_the_robot[id_servo].is_wheel_enabled)	//mode déja défini
		return TRUE;

	if(enabled) {	//sauvegarde les angles limite défini et passe en mode wheel
		Uint16 min_angle, max_angle;

		AX12_on_the_robot[id_servo].is_wheel_enabled = TRUE;
		
		min_angle = AX12_config_get_minimal_angle(id_servo);
		if(AX12_get_last_error(id_servo).error)
			return FALSE;
		
		max_angle = AX12_config_get_maximal_angle(id_servo);
		if(AX12_get_last_error(id_servo).error)
			return FALSE;
		if(max_angle == 0) max_angle = 1;	//évite les bugs si les 2 angles limites sont à 0 (et donc qu'on était déja en mode wheel mais que le servo ne le savait pas.

		if(!AX12_config_set_minimal_angle(id_servo, 0))
			return FALSE;
		if(!AX12_config_set_maximal_angle(id_servo, 0))
			return FALSE;

		AX12_on_the_robot[id_servo].angle_limit[0] = min_angle;
		AX12_on_the_robot[id_servo].angle_limit[1] = max_angle;
		return TRUE;
	} else {
		AX12_on_the_robot[id_servo].is_wheel_enabled = FALSE;
		if(!AX12_config_set_minimal_angle(id_servo, AX12_on_the_robot[id_servo].angle_limit[0]))
			return FALSE;
		if(!AX12_config_set_maximal_angle(id_servo, AX12_on_the_robot[id_servo].angle_limit[1]))
			return FALSE;
		return TRUE;
	}
}

bool_e AX12_set_torque_enabled(Uint8 id_servo, bool_e enabled) {
	return AX12_instruction_write8(id_servo, AX12_TORQUE_ENABLE, enabled != 0);
}

bool_e AX12_set_led_enabled(Uint8 id_servo, bool_e enabled) {
	return AX12_instruction_write8(id_servo, AX12_LED, enabled != 0);
}

	/*
	Paramétrage de l'asservissement avec AX12_set_torque_response(A, B, C, D)
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
	Si la valeur absolue de l'erreur est entre A et B ou C et D, le couple est proportionnel à la valeur absolue de l'erreur (le courant minimal pour donner du couple est défini par les fonctions AX12_set_punch_current/AX12_get_punch_current)
	Si la valeur absolue de l'erreur est plus petite que B ou C, l'AX12 n'exerce aucun couple

	*/
bool_e AX12_set_torque_response(Uint8 id_servo, Uint16 A, Uint16 B, Uint16 C, Uint16 D) {
	Uint16 CcwComplianceSlope;
	Uint16 CcwComplianceMargin;
	Uint16 CwComplianceMargin;
	Uint16 CwComplianceSlope;

	//Limite d'angle max sinon si l'angle est trop élevé, AX12_DEGRE_TO_ANGLE pourrait renvoyer une valeur erronée a cause d'un overflow
	if(A > AX12_MAX_DEGRE)
		A = AX12_MAX_DEGRE;
	if(B > AX12_MAX_DEGRE)
		B = AX12_MAX_DEGRE;
	if(C > AX12_MAX_DEGRE)
		C = AX12_MAX_DEGRE;
	if(D > AX12_MAX_DEGRE)
		D = AX12_MAX_DEGRE;

	CcwComplianceSlope = AX12_DEGRE_TO_ANGLE(A);
	CcwComplianceMargin = AX12_DEGRE_TO_ANGLE(B);
	CwComplianceMargin = AX12_DEGRE_TO_ANGLE(C);
	CwComplianceSlope = AX12_DEGRE_TO_ANGLE(D);

	if(!AX12_instruction_async_write8(id_servo, AX12_CCW_COMPLIANCE_SLOPE, CcwComplianceSlope))
		return FALSE;
	if(!AX12_instruction_async_write8(id_servo, AX12_CCW_COMPLIANCE_MARGIN, CcwComplianceMargin))
		return FALSE;
	if(!AX12_instruction_async_write8(id_servo, AX12_CW_COMPLIANCE_MARGIN, CwComplianceMargin))
		return FALSE;
	if(!AX12_instruction_async_write8(id_servo, AX12_CW_COMPLIANCE_SLOPE, CwComplianceSlope))
		return FALSE;

	#if AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS
		if(!AX12_instruction_wait(id_servo) || AX12_instruction_get_last_status(id_servo).error == 0)
			return FALSE;
	#endif

	return TRUE;
}

bool_e AX12_set_position(Uint8 id_servo, Uint16 degre) {
	if(degre > AX12_MAX_DEGRE) degre = AX12_MAX_DEGRE;
	return AX12_instruction_write16(id_servo, AX12_GOAL_POSITION_L, AX12_DEGRE_TO_ANGLE(degre));
}

bool_e AX12_set_move_to_position_speed(Uint8 id_servo, Uint16 degre_per_sec) {
	if(AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_set_move_to_position_speed while in wheel mode, use AX12_set_speed_percentage instead.\n");
		return FALSE;
	}

	if(degre_per_sec > AX12_MAX_DPS) degre_per_sec = AX12_MAX_DPS;
	return AX12_instruction_write16(id_servo, AX12_GOAL_SPEED_L, AX12_DPS_TO_SPEED(degre_per_sec));
}

bool_e AX12_set_speed_percentage(Uint8 id_servo, Sint8 percentage) {
	bool_e isBackward;

	if(!AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_set_speed_percentage while not in wheel mode, use AX12_set_move_to_position_speed instead.\n");
		return FALSE;
	}

	isBackward = percentage < 0;
	//percentage & 0x7F: on enlève le bit de signe, isBackward << 10: ajoute 1024 si on tourne a l'envers
	percentage = percentage & 0x7F;
	if(percentage > AX12_MAX_PERCENTAGE) percentage = AX12_MAX_PERCENTAGE;
	Uint16 realValue = AX12_PERCENTAGE_TO_1024(percentage) | (((Uint16)isBackward) << 10);

	return AX12_instruction_write16(id_servo, AX12_GOAL_SPEED_L, realValue);
}

bool_e AX12_set_torque_limit(Uint8 id_servo, Uint8 percentage) {
	if(percentage > AX12_MAX_PERCENTAGE) percentage = AX12_MAX_PERCENTAGE;

	return AX12_instruction_write16(id_servo, AX12_TORQUE_LIMIT_L, AX12_PERCENTAGE_TO_1024(percentage));
}

bool_e AX12_set_punch_torque_percentage(Uint8 id_servo, Uint8 percentage) {
	if(percentage > AX12_MAX_PERCENTAGE) percentage = AX12_MAX_PERCENTAGE;

	return AX12_instruction_write16(id_servo, AX12_TORQUE_LIMIT_L, AX12_PERCENTAGE_TO_1024(percentage));
}

#endif /* def USE_AX12_SERVO */
