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

//Pour voir l'ancienne version avant les modifications faites pour le robot 2012-2013, utilisez la version SVN 5340

#include "QS_ax12.h"

#ifdef USE_AX12_SERVO

#include "QS_ports.h"
#include "QS_outputlog.h"
#include "stm32f4xx_usart.h"

#include "../config/config_pin.h"


	/**********************************************************************/
	/** Param�tres de configuration du driver AX12/RX24                        */
	/**********************************************************************/

	#if !defined(AX12_NUMBER) || AX12_NUMBER > 254
		#error "Vous devez definir un nombre de servo sur le robot (AX12_NUMBER) (254 max)"
	#endif /* ndef AX12_NUMBER */

	//#define AX12_DEBUG_PACKETS: si d�fini, les paquets envoy�s sont �crits sur la sortie uart en utilisant debug_printf (attention, debug_printf n'est pas r�entrante)

//	#ifndef AX12_UART_ID
//		#define AX12_UART_ID 2
//	#endif

	#if AX12_UART_ID == 1
		#define AX12_UART_Ptr USART1
		#define AX12_UART_Interrupt USART1_IRQHandler
		#define AX12_UART_Interrupt_IRQn USART1_IRQn
	#elif AX12_UART_ID == 2
		#define AX12_UART_Ptr USART2
		#define AX12_UART_Interrupt USART2_IRQHandler
		#define AX12_UART_Interrupt_IRQn USART2_IRQn
	#elif AX12_UART_ID == 3
		#define AX12_UART_Ptr USART3
		#define AX12_UART_Interrupt USART3_IRQHandler
		#define AX12_UART_Interrupt_IRQn USART3_IRQn
	#else
	#warning "AX12: Unknown UART ID"
	#endif



	#ifndef AX12_UART_BAUDRATE
		#define AX12_UART_BAUDRATE  56700
	#endif

	#ifndef AX12_STATUS_RETURN_TIMEOUT
		#define AX12_STATUS_RETURN_TIMEOUT 50	//Ne pas mettre une valeur trop proche de celle pr�vue, l'AX12/RX24 r�pond parfois avec un delai plus long
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
	/** Configuration du timer pour le timeout de r�ception du status    **/
	/**********************************************************************/

	#ifdef AX12_TIMER_ID
		#define TIMER_SRC_TIMER_ID AX12_TIMER_ID
	#endif
	#ifdef AX12_TIMER_USE_WATCHDOG
		#define TIMER_SRC_USE_WATCHDOG
	#endif

	#include "QS_setTimerSource.h"

	/*************************************************************************************/

	/* UART2 exclusivement d�di� � la gestion des servomoteurs AX-12+ */

	#ifdef USE_UART2
		#error "Vous ne pouvez pas utiliser l'UART2 et l'AX12 en m�me temps, l'AX12 � besoin de l'UART2 pour communiquer"
	#endif

	#if !defined(AX12_DIRECTION_PORT_AX12)
		#error "Vous devez definir un port de direction (AX12_DIRECTION_PORT_AX12) pour g�rer l'UART qui est en Half-duplex (cf. Datasheet MAX485)"
	#endif /* ndef AX12_DIRECTION_PORT */

	/* Pour UART half-uplex */
	#define TX_DIRECTION 1
	#define RX_DIRECTION 0


	/*************************************************************************************/
	/** Constantes li�es au instruction envoy�es a l'AX12/RX24                               **/
	/*************************************************************************************/

	/* Instructions */

	#define INST_PING 				0x01
	#define INST_READ 				0x02
	#define INST_WRITE 				0x03
	#define INST_REG_WRITE 			0x04
	#define INST_ACTION 			0x05
	//Instructions non utilis�es
	//#define INST_RESET 			0x06
	//#define INST_DIGITAL_RESET 	0x07
	//#define INST_SYSTEM_READ 		0x0C
	//#define INST_SYSTEM_WRITE 	0x0D
	//#define INST_SYNC_WRITE 		0x83
	//#define INST_SYNC_REG_WRITE 	0x84

	// Adresses de la m�moire interne de l'AX12
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

	//Les defines sont globaux, il sont d�fini en m�me temps que le tableau des valeurs accept� par l'AX12/RX24 pour pouvoir voir � quelle valeur les limites correspondent
	static const Uint8 AX12_values_ranges[0x32] = {
		//EEPROM, utilis� par les fonctions AX12_config_*, les valeurs persiste apr�s une mise hors tension.
		#define AX12_MODEL_NUMBER_L 			0x00
			0,	//non utilis�
		#define AX12_MODEL_NUMBER_H				0x01
			0,	//non utilis�
		#define AX12_VERSION					0x02
			0,	//non utilis�
		#define AX12_ID 						0x03
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_253,
		#define AX12_BAUD_RATE					0x04
			AX12_MIN_VALUE_34 | AX12_MAX_VALUE_254,	//la limite de l'AX12/RX24 est 0-254, mais la carte ne supporte pas des vitesses sup�rieures � 56700bauds.
		#define AX12_RETURN_DELAY_TIME			0x05
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//Maximum de 508 us
		#define AX12_CW_ANGLE_LIMIT_L			0x06
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_CW_ANGLE_LIMIT_H			0x07
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas �tre utilis�
		#define AX12_CCW_ANGLE_LIMIT_L			0x08
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_CCW_ANGLE_LIMIT_H			0x09
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas �tre utilis�
		#define AX12_SYSTEM_DATA2				0x0A
			0,	//non utilis�
		#define AX12_LIMIT_TEMPERATURE			0x0B
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_150,
		#define AX12_DOWN_LIMIT_VOLTAGE 		0x0C
			AX12_MIN_VALUE_50 | AX12_MAX_VALUE_250,
		#define AX12_UP_LIMIT_VOLTAGE			0x0D
			AX12_MIN_VALUE_50 | AX12_MAX_VALUE_250,
		#define AX12_MAX_TORQUE_L				0x0E
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_MAX_TORQUE_H				0x0F
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas �tre utilis�
		#define AX12_RETURN_LEVEL				0x10
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_2,
		#define AX12_ALARM_LED					0x11
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_127,
		#define AX12_ALARM_SHUTDOWN 			0x12
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_127,
		#define AX12_OPERATING_MODE 			0x13
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1,
		#define AX12_DOWN_CALIBRATION_L 		0x14
			0,	//non utilis�
		#define AX12_DOWN_CALIBRATION_H 		0x15
			0,	//non utilis�
		#define AX12_UP_CALIBRATION_L			0x16
			0,	//non utilis�
		#define AX12_UP_CALIBRATION_H			0x17
			0,	//non utilis�

		//RAM de l'AX12, reinitialis� apr�s une mise hors tension
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
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas �tre utilis�
		#define AX12_GOAL_SPEED_L				0x20
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_GOAL_SPEED_H				0x21
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas �tre utilis�
		#define AX12_TORQUE_LIMIT_L				0x22
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_TORQUE_LIMIT_H 			0x23
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254,	//ne devrait pas �tre utilis�
		#define AX12_PRESENT_POSITION_L 		0x24
			0,	//non utilis�
		#define AX12_PRESENT_POSITION_H 		0x25
			0,	//non utilis�
		#define AX12_PRESENT_SPEED_L			0x26
			0,	//non utilis�
		#define AX12_PRESENT_SPEED_H			0x27
			0,	//non utilis�
		#define AX12_PRESENT_LOAD_L 			0x28
			0,	//non utilis�
		#define AX12_PRESENT_LOAD_H 			0x29
			0,	//non utilis�
		#define AX12_PRESENT_VOLTAGE			0x2A
			0,	//non utilis�
		#define AX12_PRESENT_TEMPERATURE		0x2B
			0,	//non utilis�
		#define AX12_REGISTERED_INSTRUCTION		0x2C
			0,	//non utilis�
		#define AX12_PAUSE_TIME 				0x2D
			0,	//non utilis�
		#define AX12_MOVING 					0x2E
			0,	//non utilis�
		#define AX12_LOCK						0x2F
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1,
		#define AX12_PUNCH_L					0x30
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_1023,
		#define AX12_PUNCH_H					0x31
			AX12_MIN_VALUE_0 | AX12_MAX_VALUE_254	//ne devrait pas �tre utilis�
	};

	//Adresse sp�ciale pour le ping
	#define AX12_PING 0xFF

	/***********************************************************************/
	/** Autre d�finitions internes                                        **/
	/***********************************************************************/

	//Si d�fini, on teste le checksum des paquets de status, si le resultat est incorrect, le paquet est ignor�
	//#define AX12_STATUS_RETURN_CHECK_CHECKSUM

	//nous avons besoin d'une place en plus pour pouvoir diff�rencier le cas ou le buffer est vide du cas ou il est plein
	#define AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE (AX12_INSTRUCTION_BUFFER_SIZE+1)

	#define MIN_INSTRUCTION_PACKET_SIZE 6
	#define MAX_STATUS_PACKET_SIZE 8

	//Macro pour simplifier l'incrementation d'index (la fonction modulo fait une division assez lourde, donc profitons de super-macro
	//le modulo (%) c'est bien, mais c'est aussi beaucoup d'op�ration pour les cas utilis�s (les valeurs ne doivent jamais d�passer 2*div)
	#define INC_WITH_MOD(val,div) (( (val)+1 < div )? (val)+1 : 0)

	/***********************************************************************/

typedef struct{
	volatile AX12_status_t last_status;
	Uint16 angle_limit[2];	//sauvegarde des angles limites lors d'un passage en mode wheel (rotation compl�te en continue) Si l'AX12/RX24 �tait d�ja dans ce mode, les angles limites d�fini � 0�-300�.
	bool_e is_wheel_enabled;
	// Possibilit� d'ajout de membres ...
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


//Paquet renvoy� par l'AX12, 2 param�tres 8bits maximum
typedef struct{
	Uint8 id_servo;
	Uint16 error;  // l'AX12/RX24 n'utilise que 8 bits
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


//Machine � �tats

// Raison d'appel de la fonction de la machine � �tats
typedef enum {
	AX12_SME_NoEvent,
	AX12_SME_RxInterrupt,
	AX12_SME_TxInterruptAX12,
	AX12_SME_Timeout
} AX12_state_machine_event_e;

// Etat de la machine � �tats
typedef enum {
	AX12_SMS_ReadyToSend = 0,
	AX12_SMS_Sending,
	AX12_SMS_WaitingAnswer
} AX12_state_machine_state_e;


typedef struct {
	volatile AX12_state_machine_state_e state;
	volatile Uint16 timeout_counter;	//compteur d�cr�ment� par le timer lorsque que l'�tat est AX12_SMS_WaitingAnswer

	AX12_instruction_packet_t current_instruction;

	Uint8 receive_index;

	//Utilis� en mode SEND
#ifdef AX12_UART_Ptr
	Uint8 ax12_sending_index;
#endif
} AX12_state_machine_t;

/*************************************************************************************/

// Variables globales, toutes les variables sont intialis�e a zero, il n'y a pas de {0} ou autre pour eviter les warnings (avec -Wall) et c'est plus simple et pratique de rien mettre que des {{{0}},{0,0},0} et pour eviter d'avoir des warnings car les warnings c'est mal (d�fini dans l'ainsi C)

static AX12_servo_t AX12_on_the_robot[AX12_NUMBER];	//Tableau de structure contenant les informations li�es � chaque AX12
static AX12_state_machine_t state_machine = {0};	//Machine � �tats du driver

//Queue contenant les instructions demand�e (ce buffer est g�r� pas les fonctions/macro AX12_instruction_*, il n'y a qu'un buffer pour tous les AX12)
static volatile AX12_instruction_buffer AX12_special_instruction_buffer;
//Cette variable est � TRUE si le driver est en mode pr�paration de commandes. Voir doc de AX12_start_command_block dans le .h
static bool_e AX12_prepare_commands = FALSE;

/**************************************************************************/
/** Fonctions internes au driver de l'AX12+ et macros                    **/
/**************************************************************************/

static inline Uint16 AX12_decode_value_range(Uint8 mem_addr, bool_e get_max);	//get_max = 1 pour r�cup�rer la valeur max pour une adresse donn�e, get_max = 0 pour r�cup�rer le minimum
static Uint8 AX12_instruction_packet_calc_checksum(AX12_instruction_packet_t* instruction_packet);
static Uint8 AX12_get_instruction_packet(Uint8 byte_offset, AX12_instruction_packet_t* instruction_packet);				//Renvoi l'octet d'une instruction a envoyer
#ifdef AX12_STATUS_RETURN_CHECK_CHECKSUM
static Uint8 AX12_status_packet_calc_checksum(AX12_status_packet_t* status_packet);
#endif
static bool_e AX12_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, AX12_status_packet_t* status_packet);	//retourne FALSE si le paquet est non valide, sinon TRUE
static void AX12_state_machine(AX12_state_machine_event_e event);
static void AX12_UART_init_all(Uint32 uart_speed);
static void AX12_UART_init(USART_TypeDef* uartPtr, Uint16 baudrate);
static void AX12_UART_DisableIRQ();
static void AX12_UART_EnableIRQ();
static void AX12_UART_putc(Uint8 c);
static Uint8 AX12_UART_getc();
static bool_e AX12_UART_GetFlagStatus(Uint16 flag);
static void AX12_UART_ITConfig(Uint16 flag, FunctionalState enable);

/**************************************************************************/
/** Fonctions/macro g�rant les instructions et le buffer d'instruction   **/
/**************************************************************************/

/*
 * Les fonctions AX12_instruction_async_* sont asynchrones, utiliser AX12_instruction_wait pour attendre la fin de leurs ex�cutions avant AX12_instruction_get_last_status
 * Attention, ces fonctions attente que de la place se lib�re dans le buffer interne, attention lors de l'utilisation des fonctions de l'AX12 dans des interruptions
 * Pour pr�venir le probl�me, vous pouvez utiliser AX12_instruction_buffer_is_full() ou changer la priorit� de votre interruption
 * L'interruption de l'UART de l'AX12 a pour priorit� 5 (pour Tx et Rx) (Si aucun changement n'est fait, l'IT d'un timer a une priorit� de 4 comme la majeure partie des ITs)
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
static bool_e AX12_instruction_async_ping(Uint8 id_servo);
static bool_e AX12_instruction_async_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value);
static bool_e AX12_instruction_async_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value);
static bool_e AX12_instruction_async_execute_write();
static bool_e AX12_instruction_ping(Uint8 id_servo);
static bool_e AX12_instruction_wait(Uint8 id_servo);
static bool_e AX12_instruction_buffer_is_full();
static void AX12_instruction_next(Uint16 error, Uint16 param);
static AX12_status_t AX12_instruction_get_last_status(Uint8 id_servo);
static void AX12_instruction_reset_last_status(Uint8 id_servo);

static bool_e AX12_instruction_queue_insert(const AX12_instruction_packet_t* inst);
#define AX12_status_packet_is_full(status_packet, bytes_received) (((bytes_received) >= 4) && ((bytes_received) >= status_packet.size))	//on doit avoir lu la taille du paquet (>= 4) et avoir lu tous les octets du paquet
#define AX12_instruction_queue_is_full() (INC_WITH_MOD(AX12_special_instruction_buffer.end_index, AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE) == AX12_special_instruction_buffer.start_index)	//buffer plein si le prochain index de fin est l'index de d�but du buffer
#define AX12_instruction_queue_is_empty() (AX12_special_instruction_buffer.end_index == AX12_special_instruction_buffer.start_index)
#define AX12_instruction_queue_next() \
	(AX12_special_instruction_buffer.start_index = INC_WITH_MOD(AX12_special_instruction_buffer.start_index, AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE))
#define AX12_instruction_queue_get_current() \
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.start_index]


//retourne TRUE si l'instruction est suivie d'une r�ponse de l'AX12
//inst est de type AX12_instruction_packet_t
#define AX12_instruction_has_status_packet(inst) (inst.id_servo != AX12_BROADCAST_ID && (AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS || inst.type == INST_PING || (AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ONREAD && inst.type == INST_READ)))

/*************************************************************************************/
/** Fonctions g�rant l'envoi de paquet instruction par l'interm�diaire du buffer    **/
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

static bool_e AX12_instruction_async_ping(Uint8 id_servo) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_PING;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

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
		return AX12_instruction_get_last_status(id_servo).error == AX12_ERROR_OK;
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
		return AX12_instruction_get_last_status(id_servo).error == AX12_ERROR_OK;
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

static void AX12_instruction_next(Uint16 error, Uint16 param) {
	AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = error;
	AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.param = param;
	if(state_machine.current_instruction.type == INST_PING)
		AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.last_instruction_address = AX12_PING;
	else
		AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.last_instruction_address = state_machine.current_instruction.address;
	AX12_instruction_queue_next();
}

static bool_e AX12_instruction_wait(Uint8 id_servo) {
	Uint32 i = 0;
	//debug_printf("+1\n");
	while(!AX12_instruction_queue_is_empty() && i < 65000000)	//si i atteint 65000, on stop, on a attendu trop longtemps (au moins 6,5ms � une clock de 10Mhz, mais ce bout de code ne fait qu'une instruction)
		i++;

	if(i < 65000000) return TRUE;

	debug_printf("AX12 Wait too long %d / %d\n", AX12_special_instruction_buffer.start_index, AX12_special_instruction_buffer.end_index);
	AX12_on_the_robot[id_servo].last_status.error = AX12_ERROR_TIMEOUT | AX12_ERROR_RANGE;	//On a attendu trop longtemps, le buffer est toujours plein
	AX12_on_the_robot[id_servo].last_status.param = 0;
	return FALSE;
}

static AX12_status_t AX12_instruction_get_last_status(Uint8 id_servo)
{
	AX12_status_t status;
	AX12_UART_DisableIRQ();
	TIMER_SRC_TIMER_DisableIT();
		status = AX12_on_the_robot[id_servo].last_status;
	TIMER_SRC_TIMER_EnableIT();
	AX12_UART_EnableIRQ();

	return status;
}

static void AX12_instruction_reset_last_status(Uint8 id_servo) {
	AX12_UART_DisableIRQ();
	TIMER_SRC_TIMER_DisableIT();
		AX12_on_the_robot[id_servo].last_status.last_instruction_address = 0;
		AX12_on_the_robot[id_servo].last_status.error = AX12_ERROR_OK;
		AX12_on_the_robot[id_servo].last_status.param = 0;
	TIMER_SRC_TIMER_EnableIT();
	AX12_UART_EnableIRQ();
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
		debug_printf("AX12: Error: in AX12_instruction_packet_calc_checksum(): paquet trop petit\n");

	//packet_length est forc�ment >= 2 (voir datasheet de l'ax12)
	//PAS de break, si il y a 3 param�tres (packet_length = 3 +2), on fait toutes les additions
	switch(packet_length) {
		case 5:	//3 param�tres
			checksum += instruction_packet->param_2;
			//no break;
		case 4:	//2 param�tres
			checksum += instruction_packet->param_1;
			//no break;
		case 3:	//1 param�tre
			checksum += instruction_packet->address;
			//no break;
		case 2:	//pas de param�tre
			checksum += instruction_packet->id_servo + packet_length + instruction_packet->type;
			break;

		default:
			debug_printf("AX12: Error: AX12_instruction_packet_calc_checksum(): paquet trop grand\n");
			break;
	}

	return ~checksum;
}

static Uint8 AX12_get_instruction_packet(Uint8 byte_offset, AX12_instruction_packet_t* instruction_packet) {

	if(byte_offset == instruction_packet->size - 1)	//si c'est le dernier octet, envoyer le checksum
		return AX12_instruction_packet_calc_checksum(instruction_packet);

	switch(byte_offset)
	{
		case 0:	//PAS de break, on retourne 0xFF pour les octets num�ro 0 et 1
		case 1: return 0xFF;
		case 2: return instruction_packet->id_servo;
		case 3: return instruction_packet->size - 4;
		case 4: return instruction_packet->type;
		case 5:	return instruction_packet->address;
		case 6: return instruction_packet->param_1;
		case 7: return instruction_packet->param_2;

		default:
			if(byte_offset > 7)
				debug_printf("AX12: Warning: byte_offset > 7\n");
	}

	//Si on atteint cette ligne, byte_offset > 7, on retourne 0 par d�faut (�a ne devrais jamais arriver)
	return 0;
}

#ifdef AX12_STATUS_RETURN_CHECK_CHECKSUM
static Uint8 AX12_status_packet_calc_checksum(AX12_status_packet_t* status_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = status_packet->size - 4;

	if(status_packet->size < 6)
		debug_printf("AX12: Error: in AX12_status_packet_calc_checksum(): paquet trop petit\n");

	//packet_length est forc�ment >= 2 (voir datasheet de l'ax12)
	//PAS de break, si il y a 2 param�tre, on fait toutes les additions
	switch(packet_length) {
		case 4:	//2 param�tres
			checksum += status_packet->param_2;
			//no break;
		case 3:	//1 param�tre
			checksum += status_packet->param_1;
			//no break;
		case 2:	//pas de param�tre
			checksum += status_packet->id_servo + packet_length + (status_packet->error & 0xFF);
			break;

		default:
			debug_printf("AX12: Error: AX12_status_packet_calc_checksum(): paquet trop grand\n");
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

	//mise a jour de la structure instruction avec les octets re�us
	switch(byte_offset)
	{
		case 0:
			//Initialisation de a structure a des valeurs par d�faut
			status_packet->id_servo = 0;
			status_packet->size = 0;
			//status_packet->error = AX12_ERROR_OK; contient IN_PROGRESS d�j�
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
			//pour �tre sur de ne pas avoir le bit 7 a 1, si l'AX12/RX24 le met a 1, on met tous les bits a 1
			status_packet->error = receive_byte & 0x7F; //On enl�ve le IN_PROGRESS en passant
			break;

		case 5:
			status_packet->param_1 = receive_byte;
			break;

		case 6:
			status_packet->param_2 = receive_byte;
			break;

		default:
			if(byte_offset > 6)
				debug_printf("AX12: Error: AX12_update_status_packet(): trop d'octet re�u > 6\n");
	}

	return TRUE;
}

//La machine a �tats est appel�e par les interruptions de l'UART et par le timer
static void AX12_state_machine(AX12_state_machine_event_e event) {
	static volatile int processing_state = 0;

	#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
		//Pour le deboggage (avec debug_printf � la fin de l'envoi/reception d'un paquet)
		static Uint8 AX12_UART2_reception_buffer[MAX_STATUS_PACKET_SIZE*2] = {0};
		Uint8 i, pos;
	#endif

//s�curisation des appels � cette fonction lorsque event == AX12_SME_NoEvent pour eviter que l'�tat AX12_SMS_ReadyToSend soit ex�cut� plusieurs fois
//si une IT arrive a un certain moment, elle sera execut�e compl�tement, donc processing_state reviendra � son �tat d'origine
	if(event == AX12_SME_NoEvent) {
//1) Si une IT arrive a ce moment, elle s'execute enti�rement et change l'�tat de AX12_SMS_ReadyToSend en AX12_SMS_Sending, cet appel continuera et le switch branchera sur AX12_SMS_Sending, il n'y a rien de pr�vu pour AX12_SME_NoEvent donc cet appel ne fera rien au final
		if(processing_state > 0)
//2) Si une IT arrive a ce moment, elle s'executera pas, elle retournera sans rien faire car processing_state est > 0
			return;
//3) Si une IT arrive a ce moment, elle s'executera enti�rement car processing_state == 0, l'�tat passera de AX12_SMS_ReadyToSend � AX12_SMS_Sending et cet appel de fonction ne fera rien comme au 1)
		processing_state = 1;
//4) Si une IT arrive a ce moment, elle ne s'executera pas car processing_state > 0
	} else if(event == AX12_SME_Timeout) {
		AX12_UART_DisableIRQ();
	} else if(event == AX12_SME_RxInterrupt || event == AX12_SME_TxInterruptAX12) {
		TIMER_SRC_TIMER_DisableIT();
	}

//Si une IT arrive avec event == AX12_SME_TxInterrupt, elle n'aura un effet que dans l'�tat AX12_SMS_Sending (et cette interruption ne devrait �tre lanc�e que apr�s le putcUART2 dans AX12_SMS_ReadyToSend et pas dans AX12_SMS_Sending car une interruption n'est pas prioritaire sur elle-m�me)
//Si une IT arrive avec event == AX12_SME_RxInterrupt, elle n'aura un effet que dans l'�tat AX12_SMS_WaitingAnswer. Elle ne peut �tre lanc�e que si le timer n'a pas eu d'interruption et d�sactive l'interruption du timer

	switch(state_machine.state)
	{
		case AX12_SMS_ReadyToSend:
			if(event == AX12_SME_NoEvent)
			{
				// Choix du paquet � envoyer et d�but d'envoi
				if(!AX12_instruction_queue_is_empty())
					state_machine.current_instruction = AX12_instruction_queue_get_current();
				else {	//s'il n'y a rien a faire, mettre en veille la machine a �tat, l'UART sera donc inactif (et mettre en mode reception pour ne pas forcer la sortie dont on d�fini la tension, celle non reli� a l'AX12)
					while(!AX12_UART_GetFlagStatus(USART_FLAG_TC));   //inifinite loop si uart pas initialis�
					GPIO_WriteBit(AX12_DIRECTION_PORT_AX12, RX_DIRECTION);
					break;
				}

				state_machine.state = AX12_SMS_Sending;
				AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = AX12_ERROR_IN_PROGRESS;
#ifdef AX12_UART_Ptr
				state_machine.ax12_sending_index = 0;
#endif

				state_machine.receive_index = 0;


				#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
					debug_printf("AX12 Tx:");
					for(i = 0; i<state_machine.current_instruction.size; i++)
						debug_printf(" %02x", AX12_get_instruction_packet(i, &state_machine.current_instruction));
					debug_printf("\n");
				#endif

				GPIO_WriteBit(AX12_DIRECTION_PORT_AX12, TX_DIRECTION);


				TIMER_SRC_TIMER_start_ms(AX12_STATUS_SEND_TIMEOUT);	//Pour le timeout d'envoi, ne devrait pas arriver

#ifdef AX12_UART_Ptr
				state_machine.ax12_sending_index++;	//Attention! Nous devons incrementer sending_index AVANT car il y a un risque que l'interuption Tx arrive avant l'incrementation lorsque cette fonction est appell�e par AX12_init() (qui n'est pas dans une interruption)
#endif


				AX12_UART_putc(AX12_get_instruction_packet(0, &state_machine.current_instruction));
				AX12_UART_ITConfig(USART_IT_TXE, ENABLE);
			} /*else if(event == AX12_SME_TxInterrupt) {
				AX12_UART_ITConfig(USART_IT_TXE, DISABLE);
			}*/
		break;

		case AX12_SMS_Sending:
			if(event == AX12_SME_TxInterruptAX12)
			{
				if(event == AX12_SME_TxInterruptAX12) {
#ifdef AX12_UART_Ptr
					// ax12
					if(state_machine.ax12_sending_index < state_machine.current_instruction.size) {
						USART_SendData(AX12_UART_Ptr, AX12_get_instruction_packet(state_machine.ax12_sending_index, &state_machine.current_instruction));
						state_machine.ax12_sending_index++;
					} else
						USART_ITConfig(AX12_UART_Ptr, USART_IT_TXE, DISABLE);
#endif
				}
//				if(state_machine.sending_index < state_machine.current_instruction.size) {
//					AX12_UART_putc(AX12_get_instruction_packet(state_machine.sending_index, &state_machine.current_instruction));
//					state_machine.sending_index++;
//				}


				if(
		#ifdef AX12_UART_Ptr
						state_machine.ax12_sending_index >= state_machine.current_instruction.size
		#endif
					)
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
						//TRMT passe a 1 quand tout est envoy� (bit de stop inclu)
						//plus d'info ici: http://books.google.fr/books?id=ZNngQv_E0_MC&lpg=PA250&ots=_ZTiXKt-8p&hl=fr&pg=PA250
						while(!AX12_UART_GetFlagStatus(USART_FLAG_TC));

						GPIO_WriteBit(AX12_DIRECTION_PORT_AX12, RX_DIRECTION);


						//flush recv buffer
						while(AX12_UART_GetFlagStatus(USART_FLAG_ORE) || AX12_UART_GetFlagStatus(USART_FLAG_FE) || AX12_UART_GetFlagStatus(USART_FLAG_NE))
							AX12_UART_getc();

						state_machine.state = AX12_SMS_WaitingAnswer;

						TIMER_SRC_TIMER_start_ms(AX12_STATUS_RETURN_TIMEOUT);	//Pour le timeout de reception, ne devrait pas arriver
					}
					else
					{
						AX12_instruction_next(AX12_ERROR_OK, 0);
						state_machine.state = AX12_SMS_ReadyToSend;
						AX12_state_machine(AX12_SME_NoEvent);
					}
				}
			} else if(event == AX12_SME_Timeout) {
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();
				debug_printf("AX12[%d]: send timeout !!\n", state_machine.current_instruction.id_servo);

				AX12_instruction_next(AX12_ERROR_TIMEOUT, 1);
				state_machine.state = AX12_SMS_ReadyToSend;
				AX12_state_machine(AX12_SME_NoEvent);
			}

			if(state_machine.state != AX12_SMS_Sending)
				AX12_UART_ITConfig(USART_IT_TXE, DISABLE);
		break;

		case AX12_SMS_WaitingAnswer:
			if(event == AX12_SME_RxInterrupt)
			{
				//AX12_update_status_packet s'occupe de son initialisation et de son remplissage au fur et a mesure que les octets arrivent
				static AX12_status_packet_t status_response_packet;
				// Stockage de la r�ponse dans un buffer, si toute la r�ponse alors mise � jour des variables du driver et state = AX12_SMS_ReadyToSend avec �x�cution

				if(AX12_UART_GetFlagStatus(USART_FLAG_FE) || AX12_UART_GetFlagStatus(USART_FLAG_NE)) { //ignore error bits
					AX12_UART_getc();
					break;
				}

				#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
				AX12_UART2_reception_buffer[pos] = AX12_UART_getc();

				if(!AX12_update_status_packet(AX12_UART2_reception_buffer[pos++], state_machine.receive_index, &status_response_packet))
				#else
				Uint8 data_byte = AX12_UART_getc();
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

						AX12_instruction_next(AX12_ERROR_INSTRUCTION, 1);
					} else {

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

						AX12_instruction_next(status_response_packet.error, status_response_packet.param);
					}
					state_machine.state = AX12_SMS_ReadyToSend;
					AX12_state_machine(AX12_SME_NoEvent);
				}
			} else if(event == AX12_SME_Timeout) {
				TIMER_SRC_TIMER_stop();
				TIMER_SRC_TIMER_resetFlag();

				//debug_printf("AX12[%d] timeout Rx:", state_machine.current_instruction.id_servo);
				#if defined(VERBOSE_MODE) && defined(AX12_DEBUG_PACKETS)
					debug_printf("AX12[%d] timeout Rx:", state_machine.current_instruction.id_servo);
					for(i = 0; i<pos; i++)
						debug_printf(" %02x", AX12_UART2_reception_buffer[i]);
					debug_printf(", recv idx: %d\n", state_machine.receive_index);
					debug_printf(" Original cmd: Id: %02x Cmd: %02x Addr: %02x param: %04x\n", state_machine.current_instruction.id_servo, state_machine.current_instruction.type, state_machine.current_instruction.address, state_machine.current_instruction.param);
				#endif

				if(AX12_UART_GetFlagStatus(USART_FLAG_RXNE))
					debug_printf("AX12 timeout too small\n");

				AX12_instruction_next(AX12_ERROR_TIMEOUT, 0);
				state_machine.state = AX12_SMS_ReadyToSend;
				AX12_state_machine(AX12_SME_NoEvent);
			} /*else if(event == AX12_SME_TxInterrupt) {
				AX12_UART_ITConfig(USART_IT_TXE, DISABLE);
			}*/
		break;
	}

	if(event == AX12_SME_RxInterrupt || event == AX12_SME_TxInterruptAX12) {
		TIMER_SRC_TIMER_EnableIT();
	} else if(event == AX12_SME_Timeout) {
		AX12_UART_EnableIRQ();
	} else if(event == AX12_SME_NoEvent) {
		processing_state = 0;
	}
}

/*********************************************************************************/
/** Impl�mentation des fonctions g�rant le buffer d'instruction (insertion ici) **/
/*********************************************************************************/

static bool_e AX12_instruction_queue_insert(const AX12_instruction_packet_t* inst) {	//utilisation d'un pointeur pour eviter de provoquer une copie int�grale de la structure, qui pourrait prendre du temps si elle est trop grande
	Uint16 i = 0;
	assert(inst->id_servo < AX12_NUMBER || inst->id_servo == AX12_BROADCAST_ID);
	while(AX12_instruction_queue_is_full() && i < 65000)	//boucle 65000 fois si le buffer reste full, si on atteint 65000, on �choue et retourne FALSE
		i++;

	if(i >= 65000) {
		if(inst->id_servo < AX12_NUMBER) {
			AX12_on_the_robot[inst->id_servo].last_status.error = AX12_ERROR_TIMEOUT | AX12_ERROR_RANGE;
			AX12_on_the_robot[inst->id_servo].last_status.param = 0;
		}
		debug_printf("AX12 Fatal: Instruction buffer full !\n");
		return FALSE;	//return false, on a pas r�ussi a ins�rer l'instruction, probl�me de priorit� d'interruptions ?
	}

	AX12_on_the_robot[inst->id_servo].last_status.error = AX12_ERROR_IN_PROGRESS;
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index] = *inst;
	AX12_special_instruction_buffer.end_index = INC_WITH_MOD(AX12_special_instruction_buffer.end_index, AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE);
	//truc = state_machine.state;
	//if(truc == AX12_SMS_ReadyToSend)
		AX12_state_machine(AX12_SME_NoEvent);
	//else debug_printf("ax12 not ready, in state %d\n", truc);

	return TRUE;
}

/**************************************************************************/
/** Fonction initialisant l'UART2 utilis� pour communiquer               **/
/**************************************************************************/

/*	vitesse : 57600 bauds (modifiable: uart_speed)
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/

static void AX12_UART_init_all(Uint32 uart_speed)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	PORTS_uarts_init();


	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //inf�rieur aux uarts mais sup�rieur au timers

#ifdef AX12_UART_ID
	AX12_UART_init(AX12_UART_Ptr, uart_speed);
	NVIC_InitStructure.NVIC_IRQChannel = AX12_UART_Interrupt_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	debug_printf("UART %d initialized for AX12\n", AX12_UART_ID);
#endif

}

static void AX12_UART_init(USART_TypeDef* uartPtr, Uint16 baudrate) {
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

static void AX12_UART_DisableIRQ() {
#ifdef AX12_UART_Interrupt_IRQn
	NVIC_DisableIRQ(AX12_UART_Interrupt_IRQn);
#endif

}

static void AX12_UART_EnableIRQ() {
#ifdef AX12_UART_Interrupt_IRQn
	NVIC_EnableIRQ(AX12_UART_Interrupt_IRQn);
#endif

}

static void AX12_UART_putc(Uint8 c) {
#ifdef AX12_UART_Ptr
	USART_SendData(AX12_UART_Ptr, c);
#endif

}

static Uint8 AX12_UART_getc() {
#ifdef AX12_UART_Ptr
	if(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE))
		return USART_ReceiveData(AX12_UART_Ptr);
#endif


	return 0;
}

static bool_e AX12_UART_GetFlagStatus(Uint16 flag) {
	if(flag == USART_FLAG_TC) {
		if(
		#ifdef AX12_UART_Ptr
				USART_GetFlagStatus(AX12_UART_Ptr, flag)
		#endif

				)
			return TRUE;
	} else {
		if(
		#ifdef AX12_UART_Ptr
				USART_GetFlagStatus(AX12_UART_Ptr, flag)
		#endif

				)
			return TRUE;
	}
	return FALSE;
}

static void AX12_UART_ITConfig(Uint16 flag, FunctionalState enable) {
#ifdef AX12_UART_Ptr
	USART_ITConfig(AX12_UART_Ptr, flag, enable);
#endif

}

/*****************************************************************************/
/** Interruptions utilis�es (Reception/Envoi UART2 et timer pour le timeout **/
/*****************************************************************************/


//Attention ! Si on met a 0 le flag apr�s avoir execut� la machine a �tat,
//des interruptions peuvent �tre masqu�e
//(en gros il se peut qu'une interruption Tx arrive avant d'avoir termin�e l'execution de la fonction,
//dans ce cas mettre le flag a 0 fait que l'interruption Tx n'est pas lanc�e)
//(car le caract�re envoy� est envoy� plus vite que le retour de la fonction AX12_state_machine)
#ifdef AX12_UART_Interrupt
void _ISR AX12_UART_Interrupt(void)
{
	if(USART_GetITStatus(AX12_UART_Ptr, USART_IT_RXNE))
	{
		Uint8 i = 0;
		while(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE)) {		//On a une IT Rx pour chaque carat�re re�u, donc on ne devrai pas tomber dans un cas avec 2+ char dans le buffer uart dans une IT
			if(state_machine.state != AX12_SMS_WaitingAnswer) {	//Arrive quand on allume les cartes avant la puissance ou lorsque l'on coupe la puissance avec les cartes alum�es (reception d'un octet avec l'erreur FERR car l'entr�e RX tombe � 0)
				USART_ReceiveData(AX12_UART_Ptr);
			} else {
				AX12_state_machine(AX12_SME_RxInterrupt);
				if(USART_GetFlagStatus(AX12_UART_Ptr, USART_FLAG_RXNE) && i > 5) {
					//debug_printf("Overinterrupt RX !\n");
					break; //force 0, on va perdre des caract�res, mais c'est mieux que de boucler ici ...
				}
			}
			i++;
		}
	}
	else if(USART_GetITStatus(AX12_UART_Ptr, USART_IT_TXE))
	{
		AX12_state_machine(AX12_SME_TxInterruptAX12);
	}
}
#endif



void TIMER_SRC_TIMER_interrupt()
{
	Uint8 i = 0;

	while(AX12_UART_GetFlagStatus(USART_FLAG_RXNE)) {		//On a une IT Rx pour chaque carat�re re�u, donc on ne devrai pas tomber dans un cas avec 2+ char dans le buffer uart dans une IT
		if(state_machine.state != AX12_SMS_WaitingAnswer) {	//Arrive quand on allume les cartes avant la puissance ou lorsque l'on coupe la puissance avec les cartes alum�es (reception d'un octet avec l'erreur FERR car l'entr�e RX tombe � 0)
			AX12_UART_getc();
		} else {
			AX12_state_machine(AX12_SME_RxInterrupt);
			if(AX12_UART_GetFlagStatus(USART_FLAG_RXNE) && i > 5) {
				debug_printf("Overinterrupt RX ! while timeout\n");
				break; //force 0, on va perdre des caract�res, mais c'est mieux que de boucler ici ...
			}
		}
		i++;
	}
	AX12_state_machine(AX12_SME_Timeout);
	TIMER_SRC_TIMER_resetFlag();
}

/**************************************************************************/
/** Fonctions d'interface utilis� par l'utilisateur du driver            **/
/**************************************************************************/

/* Fonction initialisant ce Driver	                                     */

void AX12_init() {
	static bool_e initialized = FALSE;
	Uint8 i;
	if(initialized)
		return;

	initialized = TRUE;

	AX12_UART_init_all(AX12_UART_BAUDRATE);
	TIMER_SRC_TIMER_init();

	GPIO_WriteBit(AX12_DIRECTION_PORT_AX12, RX_DIRECTION);


	AX12_prepare_commands = FALSE;
	AX12_instruction_write8(AX12_BROADCAST_ID, AX12_RETURN_LEVEL, AX12_STATUS_RETURN_MODE);	//Mettre les AX12/RX24 dans le mode indiqu� dans Global_config.h

	for(i=0; i<AX12_NUMBER; i++) {
		AX12_on_the_robot[i].angle_limit[0] = 0;
		AX12_on_the_robot[i].angle_limit[1] = 1023;

		AX12_on_the_robot[i].is_wheel_enabled = FALSE;
		AX12_instruction_reset_last_status(i);
	}
}

/* Fonction d'utilisation de l'AX12/RX24                                        */

//Configuration de l'AX12, perdure apr�s mise hors tension sauf pour le verouillage de la config (lock).
//Unit�s:
// Angles sur 300� de 0 � 1023
// Vitesse en pourcentage de la vitesse max
// Voltage en dixi�me de volt (50 => 5.0V)
// Temp�rature en degr�e celcius
// Pourcentage entre 0 et 100

#define AX12_MAX_ANGLE 1023

//Poucentage max: 100% (tout le monde le savait �a, mais c'est bien de le repr�ciser :) )
#define AX12_MAX_PERCENTAGE 100
#define AX12_1024_TO_PERCENTAGE(percentage) ((((Uint16)(percentage))*25) >> 8) // >> 8 <=> / 256, 25/256 = 100/1024
#define AX12_PERCENTAGE_TO_1024(percentage) ((((Uint16)(percentage)) << 8) / 25)

//Implementation

bool_e AX12_is_ready(Uint8 id_servo) {
	return AX12_instruction_ping(id_servo);
}

bool_e AX12_async_is_ready(Uint8 id_servo, bool_e *isReady) {
	AX12_status_t status = AX12_get_last_error(id_servo);
	if(status.error != AX12_ERROR_IN_PROGRESS) {
		if(status.last_instruction_address == AX12_PING) {
			AX12_reset_last_error(id_servo);
			if(status.error == AX12_ERROR_OK)
				*isReady = TRUE;
			else
				*isReady = FALSE;
			return TRUE;
		} else {
			AX12_instruction_async_ping(id_servo);
		}
	}


	return FALSE;
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
	return AX12_instruction_read16(id_servo, AX12_CW_ANGLE_LIMIT_L, NULL);
}

Uint16 AX12_config_get_maximal_angle(Uint8 id_servo) {
	return AX12_instruction_read16(id_servo, AX12_CCW_ANGLE_LIMIT_L, NULL);
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

bool_e AX12_config_set_id(Uint8 id_servo_current, Uint8 id_servo_new){
	return AX12_instruction_write8(id_servo_current, AX12_ID, id_servo_new);
}

bool_e AX12_config_set_return_delay_time(Uint8 id_servo, Uint16 delay_us) {
	return AX12_instruction_write8(id_servo, AX12_RETURN_DELAY_TIME, (Uint8)(delay_us >> 1)); // >> 1 <=> /2
}

bool_e AX12_config_set_minimal_angle(Uint8 id_servo, Uint16 degre) {
	if(degre > AX12_MAX_ANGLE) degre = AX12_MAX_ANGLE;
	return AX12_instruction_write16(id_servo, AX12_CW_ANGLE_LIMIT_L, degre);
}

bool_e AX12_config_set_maximal_angle(Uint8 id_servo, Uint16 degre) {
	if(degre > AX12_MAX_ANGLE) degre = AX12_MAX_ANGLE;
	if(degre == 0) degre = 1;	//Si l'utilisateur met un angle mini et maxi � 0, l'AX12/RX24 passera en mode rotation en continue, ce mode ne doit �tre activ� que par AX12_set_wheel_mode_enabled (en passant TRUE)
	return AX12_instruction_write16(id_servo, AX12_CCW_ANGLE_LIMIT_L, degre);
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

	if(A) *A = CcwComplianceSlope;
	if(B) *B = CcwComplianceMargin;
	if(C) *C = CwComplianceMargin;
	if(D) *D = CwComplianceSlope;

	return TRUE;
}

Uint16 AX12_get_position(Uint8 id_servo) {
	return AX12_instruction_read16(id_servo, AX12_PRESENT_POSITION_L, NULL);
}

Sint8 AX12_get_move_to_position_speed(Uint8 id_servo) {
	bool_e isBackward;
	Uint16 speedPercentage;

	if(AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_get_move_to_position_speed while in wheel mode, use AX12_get_speed_percentage instead.\n");
		return 0;
	}

	speedPercentage = AX12_instruction_read16(id_servo, AX12_PRESENT_SPEED_L, NULL);

	isBackward = (speedPercentage & 0x400) != 0;
	speedPercentage = AX12_1024_TO_PERCENTAGE(speedPercentage & 0x3FF);
	if(isBackward)
		return -(Sint8)speedPercentage;
	return (Sint8)speedPercentage;
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


bool_e AX12_set_wheel_mode_enabled(Uint8 id_servo, bool_e enabled) {
	if(enabled == AX12_on_the_robot[id_servo].is_wheel_enabled)	//mode d�ja d�fini
		return TRUE;

	if(enabled) {	//sauvegarde les angles limite d�fini et passe en mode wheel
		Uint16 min_angle, max_angle;

		AX12_on_the_robot[id_servo].is_wheel_enabled = TRUE;

		min_angle = AX12_config_get_minimal_angle(id_servo);
		if(AX12_get_last_error(id_servo).error)
			return FALSE;

		max_angle = AX12_config_get_maximal_angle(id_servo);
		if(AX12_get_last_error(id_servo).error)
			return FALSE;
		if(max_angle == 0) max_angle = 1;	//�vite les bugs si les 2 angles limites sont � 0 (et donc qu'on �tait d�ja en mode wheel mais que le servo ne le savait pas.

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
#endif //AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER

bool_e AX12_set_torque_enabled(Uint8 id_servo, bool_e enabled) {
	return AX12_instruction_write8(id_servo, AX12_TORQUE_ENABLE, enabled != 0);
}

bool_e AX12_set_led_enabled(Uint8 id_servo, bool_e enabled) {
	return AX12_instruction_write8(id_servo, AX12_LED, enabled != 0);
}

	/*
	Param�trage de l'asservissement avec AX12_set_torque_response(A, B, C, D)
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
	Les points A, B, C, D sont la valeur absolue de la diff�rence de position entre le point et la position consigne (en minute d'angle)
	Si la valeur absolue de l'erreur est plus grande que A (sens anti-horaire) ou D (sens horaire), le couple est constant quelque soit l'erreur
	Si la valeur absolue de l'erreur est entre A et B ou C et D, le couple est proportionnel � la valeur absolue de l'erreur (le courant minimal pour donner du couple est d�fini par les fonctions AX12_set_punch_current/AX12_get_punch_current)
	Si la valeur absolue de l'erreur est plus petite que B ou C, l'AX12/RX24 n'exerce aucun couple

	*/
bool_e AX12_set_torque_response(Uint8 id_servo, Uint16 A, Uint16 B, Uint16 C, Uint16 D) {
	Uint16 CcwComplianceSlope;
	Uint16 CcwComplianceMargin;
	Uint16 CwComplianceMargin;
	Uint16 CwComplianceSlope;

	//Limite d'angle max sinon si l'angle est trop �lev�, AX12_DEGRE_TO_ANGLE pourrait renvoyer une valeur erron�e a cause d'un overflow
	if(A > AX12_MAX_ANGLE)
		A = AX12_MAX_ANGLE-1;
	if(B > AX12_MAX_ANGLE)
		B = AX12_MAX_ANGLE-1;
	if(C > AX12_MAX_ANGLE)
		C = AX12_MAX_ANGLE-1;
	if(D > AX12_MAX_ANGLE)
		D = AX12_MAX_ANGLE-1;

	CcwComplianceSlope = A;
	CcwComplianceMargin = B;
	CwComplianceMargin = C;
	CwComplianceSlope = D;

	if(!AX12_instruction_async_write8(id_servo, AX12_CCW_COMPLIANCE_SLOPE, CcwComplianceSlope))   // A verifier value 16bits envoie dans une fonction 8 bits
		return FALSE;
	if(!AX12_instruction_async_write8(id_servo, AX12_CCW_COMPLIANCE_MARGIN, CcwComplianceMargin))
		return FALSE;
	if(!AX12_instruction_async_write8(id_servo, AX12_CW_COMPLIANCE_MARGIN, CwComplianceMargin))
		return FALSE;
	if(!AX12_instruction_async_write8(id_servo, AX12_CW_COMPLIANCE_SLOPE, CwComplianceSlope))
		return FALSE;

	#if AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS
		if(!AX12_instruction_wait(id_servo) || AX12_instruction_get_last_status(id_servo).error == AX12_ERROR_OK)
			return FALSE;
	#endif

	return TRUE;
}

bool_e AX12_set_position(Uint8 id_servo, Uint16 angle) {
	if(angle > AX12_MAX_ANGLE) angle = AX12_MAX_ANGLE-1;
	return AX12_instruction_write16(id_servo, AX12_GOAL_POSITION_L, angle);
}

bool_e AX12_set_move_to_position_speed(Uint8 id_servo, Uint8 percentage) {
	if(AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_set_move_to_position_speed while in wheel mode, use AX12_set_speed_percentage instead.\n");
		return FALSE;
	}

	if(percentage > AX12_MAX_PERCENTAGE) percentage = AX12_MAX_PERCENTAGE;
	Uint16 realValue = AX12_PERCENTAGE_TO_1024(percentage);

	return AX12_instruction_write16(id_servo, AX12_GOAL_SPEED_L, realValue);
}

bool_e AX12_set_speed_percentage(Uint8 id_servo, Sint8 percentage) {
	bool_e isBackward;

	if(!AX12_on_the_robot[id_servo].is_wheel_enabled) {
		debug_printf("AX12: AX12_set_speed_percentage while not in wheel mode, use AX12_set_move_to_position_speed instead.\n");
		return FALSE;
	}

	isBackward = percentage < 0;
	//percentage & 0x7F: on enl�ve le bit de signe, isBackward << 10: ajoute 1024 si on tourne a l'envers
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
	return AX12_instruction_write16(id_servo, AX12_PUNCH_L, AX12_PERCENTAGE_TO_1024(percentage));
}

#endif /* def USE_AX12_SERVO */
