/*
 *	Club Robot ESEO 2010 - 2011
 *	???
 *
 *	Fichier : QS_AX12.h
 *	Package : Qualite Software
 *	Description : Module de gestion des servomoteurs Dynamixel AX12/AX12+
 *	Auteur : Ronan, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20101010
 */

#include "QS_ax12_v2.h"

#ifdef USE_AX12_SERVO
																			
	/**********************************************************************/
	/* Paramètres de configuration du driver AX12                         */
	/**********************************************************************/

	#ifndef AX12_NUMBER
		#error "Vous devez definir un nombre de servo sur le robot (AX12_NUMBER)"
	#endif /* ndef AX12_NUMBER */

	#ifndef AX12_UART_BAUDRATE
		#define AX12_UART_BAUDRATE  56700
		#warning "AX12: No AX12_UART_BAUDRATE defined, using 56700 bauds"
	#endif

	#ifndef AX12_STATUS_RETURN_TIMEOUT
		#define AX12_STATUS_RETURN_TIMEOUT 5
	#endif

	#ifndef AX12_STATUS_RETURN_MODE
		#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ONREAD
	#endif

	#ifndef AX12_INSTRUCTION_BUFFER_SIZE
		#define AX12_INSTRUCTION_BUFFER_SIZE (4*AX12_NUMBER)
	#endif

	/**********************************************************************/
	/* Configuration du timer pour le timeout de réception du status      */
	/**********************************************************************/

	#include "QS_timer.h"
	#if !defined(AX12_TIMER_ID) || AX12_TIMER_ID > 4 || AX12_TIMER_ID < 1
		#error "AX12: AX12_TIMER_ID non défini ou invalide, vous devez choisir le numéro du timer entre 1 et 4 inclus"
	#else
		//Pour plus d'info sur la concaténation de variable dans des macros, voir http://stackoverflow.com/questions/1489932/c-preprocessor-and-concatenation
		#define AX12_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
		#define AX12_TEMP_CONCAT(a,b,c) AX12_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)
		#define AX12_TIMER_interrupt AX12_TEMP_CONCAT(_T, AX12_TIMER_ID, Interrupt)
		#if AX12_TIMER_ID < 4
			#define AX12_TIMER_interrupt_flag AX12_TEMP_CONCAT(IFS0bits.T, AX12_TIMER_ID, IF)
		#else
			#define AX12_TIMER_interrupt_flag AX12_TEMP_CONCAT(IFS1bits.T, AX12_TIMER_ID, IF)
		#endif
		#define AX12_TIMER_start(period_ms) AX12_TEMP_CONCAT(TIMER, AX12_TIMER_ID, _run)(period_ms)
		#define AX12_TIMER_stop() AX12_TEMP_CONCAT(TIMER, AX12_TIMER_ID, _stop)()
	#endif

	/*************************************************************************************/

	/* UART2 exclusivement dédié à la gestion des servomoteurs AX-12+ */

	#include <uart.h>

	#ifdef USE_UART2
		#error "Vous ne pouvez pas utiliser l'UART2 et l'AX12 en même temps, l'AX12 à besoin de l'UART2 pour communiquer"
	#endif

	#ifndef AX12_DIRECTION_PORT
		#error "Vous devez definir un port de direction (AX12_DIRECTION_PORT) pour gérer l'UART qui est en Half-duplex (cf. Datasheet MAX485)"
	#endif /* ndef AX12_DIRECTION_PORT */

	/* Pour UART half-uplex */
	#define TX_DIRECTION 1
	#define RX_DIRECTION 0

	#ifdef FREQ_10MHZ
		#define CLK_FREQ	10000000
	#elif (defined FREQ_20MHZ)
		#define CLK_FREQ	20000000
	#elif (defined FREQ_40MHZ)
		#define CLK_FREQ	40000000
	#elif (defined FREQ_INTERNAL_CLK)
		#define CLK_FREQ	8000000	//fixme: pas sur
	#endif

	#define AX12_UART2_RXInterrupt			_U2RXInterrupt
	#define AX12_UART2_RXInterrupt_flag		IFS1bits.U2RXIF

	#define AX12_UART2_TXInterrupt			_U2TXInterrupt
	#define AX12_UART2_TXInterrupt_flag		IFS1bits.U2TXIF


	/*************************************************************************************/

	/* Instructions */
	
	#define INST_PING 				0x01
	#define INST_READ 				0x02
	#define INST_WRITE 				0x03
	#define INST_REG_WRITE 			0x04
	#define INST_ACTION 			0x05
	//Instructions non utilisée
	//#define INST_RESET 				0x06
	//#define INST_DIGITAL_RESET 		0x07
	//#define INST_SYSTEM_READ 		0x0C
	//#define INST_SYSTEM_WRITE 		0x0D
	//#define INST_SYNC_WRITE 		0x83
	//#define INST_SYNC_REG_WRITE 	0x84

	/***********************************************************************/
	/* Autre définition internes                                           */
	/***********************************************************************/

	//Si défini, on teste le checksum des paquets de status, si le resultat est incorrect, le paquet est ignoré
	//#define AX12_STATUS_RETURN_CHECK_CHECKSUM

	//nous avons besoin d'une place en plus pour pouvoir différencier le cas ou le buffer est vide du cas ou il est plein
	#define AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE (AX12_INSTRUCTION_BUFFER_SIZE+1)

	#define MIN_INSTRUCTION_PACKET_SIZE 6
	#define MAX_INSTRUCTION_PACKET_SIZE 9
	#define MIN_STATUS_PACKET_SIZE 6
	#define MAX_STATUS_PACKET_SIZE 8

	/***********************************************************************/
	
typedef struct{
	AX12_status_t last_status;
	Uint16 present_position;	//mis a jour automatiquement lorsqu'il n'y a plus d'instruction à executer
	bool_e servo_attribute_outdated;	//VRAI si les attributs (comme present_position) n'ont pas été mis a jour depuis trop longtemps (car le buffer d'instruction était plein par ex)
	
	// Possibilité d'ajout de membres ...	
} AX12_servo_t;

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


//Machine a état

typedef enum {
	NO_EVENT,
	RX_INTERRUPT,
	TX_INTERRUPT,
	TIMEOUT
} event_e;

typedef enum {
	READY_TO_SEND,
	SENDING,
	WAIT_ANSWER
} state_e;


typedef struct {
	state_e state;
	Uint16 timeout_counter;	//compteur décrémenté par le timer lorsque que l'état est WAIT_ANSWER

	AX12_instruction_packet_t current_instruction;

	//Utilisé en mode SEND
	Uint8 sending_index, receive_index;
} state_machine_t;

/*************************************************************************************/
	
// Variables globales, toutes les variables sont intialisée a zero, il n'y a pas de {0} ou autre pour eviter les warnings (avec -Wall) et c'est plus simple et pratique de rien mettre que des {{{0}},{0,0},0} et pour eviter d'avoir des warnings car les warnings c'est mal (défini dans l'ainsi C)

static AX12_servo_t AX12_on_the_robot[AX12_NUMBER];
static state_machine_t state_machine;

//Queue contenant des instructions demandée explicitement (pas d'instruction automatique comme la mise a jour de la variable present_position des servo)
static AX12_instruction_buffer AX12_special_instruction_buffer;

// Fonctions internes au driver de l'AX12+ et macros
static Uint8 AX12_instruction_packet_calc_checksum(AX12_instruction_packet_t* instruction_packet);
static Uint8 AX12_get_instruction_packet(Uint8 byte_offset, AX12_instruction_packet_t* instruction_packet);				//Renvoi l'octet d'une instruction a envoyer
static Uint8 AX12_status_packet_calc_checksum(AX12_status_packet_t* status_packet);
static bool_e AX12_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, AX12_status_packet_t* status_packet);	//retourne FALSE si le paquet est non valide, sinon TRUE
static void AX12_state_machine(event_e event);
static void AX12_UART2_init(Uint32 uart_speed);
#define AX12_status_packet_is_full(status_packet, bytes_received) (((bytes_received) >= 4) && ((bytes_received) >= status_packet.size))	//on doit avoir lu la taille du paquet (>= 4) et avoir lu tous les octets du paquet
#define AX12_instruction_queue_is_full() (((AX12_special_instruction_buffer.end_index + 1) % AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE) == AX12_special_instruction_buffer.start_index)
#define AX12_instruction_queue_is_empty() (AX12_special_instruction_buffer.end_index == AX12_special_instruction_buffer.start_index)
#define AX12_instruction_queue_insert(inst) \
	{ while(AX12_instruction_queue_is_full()) Nop(); AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++] = inst; \
	AX12_special_instruction_buffer.end_index %= AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE; \
	if(state_machine.state == READY_TO_SEND) AX12_state_machine(NO_EVENT); }
#define AX12_instruction_queue_next() \
	{ AX12_special_instruction_buffer.start_index++; \
	AX12_special_instruction_buffer.start_index %= AX12_INSTRUCTION_REAL_NEEDED_BUFFER_SIZE; }
#define AX12_instruction_queue_get_current() \
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.start_index]


//retourne TRUE si l'instruction est suivie d'une réponse de l'AX12
//inst est de type AX12_instruction_packet_t
#define AX12_instruction_has_status_packet(inst) (inst.id_servo != AX12_BROADCAST_ID && (AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ALWAYS || inst.type == INST_PING || (AX12_STATUS_RETURN_MODE == AX12_STATUS_RETURN_ONREAD && inst.type == INST_READ)))

void AX12_init() {
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;	
	AX12_UART2_init(AX12_UART_BAUDRATE);
	TIMER_init();
	AX12_DIRECTION_PORT = RX_DIRECTION;
	AX12_instruction_write8(0, AX12_RETURN_LEVEL, AX12_STATUS_RETURN_MODE);
}

#if !defined(AX12_STATUS_RETURN_MODE) || (AX12_STATUS_RETURN_MODE != AX12_STATUS_RETURN_NEVER)
void AX12_instruction_read(Uint8 id_servo, Uint8 address, Uint8 length) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_READ;
	inst.address = address;
	inst.param_1 = length;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	AX12_instruction_queue_insert(inst);
}
#endif

void AX12_instruction_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_WRITE;
	inst.address = address;
	inst.param_1 = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	AX12_instruction_queue_insert(inst);
}

void AX12_instruction_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_WRITE;
	inst.address = address;
	inst.param = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 3;	//param is 16bits

	AX12_instruction_queue_insert(inst);
}

void AX12_instruction_prepare_write8(Uint8 id_servo, Uint8 address, Uint8 value) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_REG_WRITE;
	inst.address = address;
	inst.param_1 = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 2;

	AX12_instruction_queue_insert(inst);
}

void AX12_instruction_prepare_write16(Uint8 id_servo, Uint8 address, Uint16 value) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_REG_WRITE;
	inst.address = address;
	inst.param = value;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE + 3;	//param is 16bits

	AX12_instruction_queue_insert(inst);
}

void AX12_instruction_execute_write(Uint8 id_servo) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_ACTION;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	AX12_instruction_queue_insert(inst);
}

void AX12_instruction_ping(Uint8 id_servo) {
	AX12_instruction_packet_t inst = {0};

	inst.id_servo = id_servo;
	inst.type = INST_PING;
	inst.size = MIN_INSTRUCTION_PACKET_SIZE;

	AX12_instruction_queue_insert(inst);
}

bool_e AX12_instruction_buffer_is_full() {
	return AX12_instruction_queue_is_full();
}

void AX12_instruction_wait()
{
	while(!AX12_instruction_queue_is_empty());
}

AX12_status_t AX12_instruction_get_last_status(Uint8 id_servo)
{
	return AX12_on_the_robot[id_servo].last_status;
}

void AX12_set_position(Uint8 id_servo, Uint16 position, bool_e now) {
	AX12_instruction_prepare_write16(id_servo, AX12_GOAL_POSITION_L, position);
	
	if(now)
		AX12_instruction_execute_write(AX12_BROADCAST_ID);
}


//Fonctions internes

static Uint8 AX12_instruction_packet_calc_checksum(AX12_instruction_packet_t* instruction_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = instruction_packet->size - 4;

	if(instruction_packet->size < 6)
		debug_printf("AX12: Error: in AX12_instruction_packet_calc_checksum(): instruction_packet->size < 6");

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
			debug_printf("AX12: Error: AX12_instruction_packet_calc_checksum(): packet_length > 5");
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
				debug_printf("AX12: Warning: Instruction packet length > 6\r\n");
	}

	//Si on atteint cette ligne, byte_offset > 7, on retourne 0 par défaut (ça ne devrais jamais arriver)
	return 0;
}

static Uint8 AX12_status_packet_calc_checksum(AX12_status_packet_t* status_packet) {
	Uint8 checksum = 0;
	Uint8 packet_length = status_packet->size - 4;

	if(status_packet->size < 6)
		debug_printf("AX12: Error: in AX12_status_packet_calc_checksum(): instruction_packet->size < 6");

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
			debug_printf("AX12: Error: AX12_status_packet_calc_checksum(): packet_length > 5");
	}

	return ~checksum;
}

static bool_e AX12_update_status_packet(Uint8 receive_byte, Uint8 byte_offset, AX12_status_packet_t* status_packet)
{
	//Si c'est le dernier octet, verifier le checksum si AX12_STATUS_RETURN_CHECK_CHECKSUM est defini
	if(byte_offset > 3 && byte_offset == status_packet->size - 1)
	#ifdef AX12_STATUS_RETURN_CHECK_CHECKSUM
		return receive_byte == AX12_status_packet_calc_checksum(status_packet);
	#else
		return TRUE;
	#endif

	//mise a jour de la structure instruction avec les octets reçus
	switch(byte_offset)
	{
		case 0:
			if(receive_byte != 0xFF)
				return FALSE;
			//Initialisation de a structure a des valeurs par défaut
			status_packet->id_servo = 0;
			status_packet->size = 0;
			status_packet->error = 0;
			status_packet->param = 0;
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
				debug_printf("AX12: Error: AX12_update_status_packet(): byte_offset > 6\r\n");
	}

	return TRUE;
}

//La machine a états est appelée par les interruptions de l'UART et par le timer
static void AX12_state_machine(event_e event) {

	#ifdef VERBOSE_MODE
		//Pour le deboggage (avec debug_printf à la fin de l'envoi/reception d'un paquet)
		static Uint8 AX12_UART2_sending_buffer[MAX_INSTRUCTION_PACKET_SIZE] = {0};
		static Uint8 AX12_UART2_reception_buffer[MAX_STATUS_PACKET_SIZE] = {0};
		int i;
	#endif
	
	switch(state_machine.state)
	{
		case READY_TO_SEND:
			// Choix du paquet à envoyer et début d'envoi
			if(!AX12_instruction_queue_is_empty())
				state_machine.current_instruction = AX12_instruction_queue_get_current();
			else	//s'il n'y a rien a faire, mettre en veille la machine a état, l'UART sera donc inactif
				return;

			state_machine.sending_index = 0;
			state_machine.receive_index = 0;
			state_machine.state = SENDING;



                        #ifdef VERBOSE_MODE
                            debug_printf("AX12 Tx:");
                            for(i = 0; i<state_machine.current_instruction.size; i++)
                                    debug_printf(" %02x", AX12_get_instruction_packet(i, &state_machine.current_instruction));
                            debug_printf("\r\n");
                        #endif

			AX12_DIRECTION_PORT = TX_DIRECTION;

			state_machine.sending_index++;	//Attention! Nous devons incrementer sending_index AVANT car il y a un risque que l'interuption Tx arrive avant l'incrementation lorsque cette fonction est appellée par AX12_init() (qui n'est pas dans une interruption)
			#ifdef VERBOSE_MODE
				putcUART2(AX12_UART2_sending_buffer[state_machine.sending_index-1] = AX12_get_instruction_packet(state_machine.sending_index-1, &state_machine.current_instruction));
			#else
				putcUART2(AX12_get_instruction_packet(state_machine.sending_index-1, &state_machine.current_instruction));
			#endif
		break;

		case SENDING:
			if(event == TX_INTERRUPT)
			{
				if(state_machine.sending_index < state_machine.current_instruction.size) {
					#ifdef VERBOSE_MODE
						putcUART2(AX12_UART2_sending_buffer[state_machine.sending_index] = AX12_get_instruction_packet(state_machine.sending_index, &state_machine.current_instruction));
					#else
						putcUART2(AX12_get_instruction_packet(state_machine.sending_index, &state_machine.current_instruction));
					#endif
					state_machine.sending_index++;
				}
				else	//Le dernier paquet a été envoyé, passage en mode reception et attente de la réponse dans l'état WAIT_ANSWER s'il y a ou enchainement sur le prochain paquet à evnoyer (READY_TO_SEND)
				{
					if(AX12_instruction_has_status_packet(state_machine.current_instruction))
					{
						//Attente de la fin de la transmition des octets
						//TRMT passe a 1 quand tout est envoyé (bit de stop inclu)
						//plus d'info ici: http://books.google.fr/books?id=ZNngQv_E0_MC&lpg=PA250&ots=_ZTiXKt-8p&hl=fr&pg=PA250
						while(!U2STAbits.TRMT);

						/*
						//L'execution des précedents instruction devrait prendre un temps supérieur a l'envoi du bit de stop
						//pour 57600 bauds, le bit de stop dure 17,4us (arrondi a 18us)
						//AX12_UART2_RXInterrupt();

						#ifdef __dsPIC30F__
							__asm__ __volatile__ (	"MOV #%0,w0\n\t"
													"loop:\n"
													"DEC w0, w0\n\t"
													"BRA NZ, loop\n\t"
													:// pas de sortie
													:"n"((CLK_FREQ/AX12_UART_BAUDRATE)/3+1)	//une boucle dure 3 front montant d'horloge (1 front montant pour DEC et 2 pour BRA)
													:"w0");
						#else
							#warning "L'utilisation de la boucle d'attente en assembleur n'est disponible que pour les processeur dsPIC30F, la version en C sera utilisé"
							//Version C de la boucle, prendra beaucoup plus de temps que la version assembleur (906 front montant d'horloge au lieu de 180, attention au temps de réponse de l'AX12)
							i=CLK_FREQ/AX12_UART_BAUDRATE;
							while(i--);
						#endif*/

						AX12_DIRECTION_PORT = RX_DIRECTION;
						state_machine.state = WAIT_ANSWER;
						AX12_TIMER_start(AX12_STATUS_RETURN_TIMEOUT);	//Pour le timeout de reception, ne devrait pas arriver
					}
					else
					{
						AX12_instruction_queue_next();
						state_machine.state = READY_TO_SEND;
						AX12_state_machine(NO_EVENT);
					}

				}
			}
		break;

		case WAIT_ANSWER:
			if(event == RX_INTERRUPT)
			{
				static AX12_status_packet_t status_response_packet = {0};
				// Stockage de la réponse dans un buffer, si toute la réponse alors mise à jour des variables du driver et state = READY_TO_SEND avec éxécution

				#ifdef VERBOSE_MODE
				AX12_UART2_reception_buffer[state_machine.receive_index] = getcUART2();

				if(!AX12_update_status_packet(AX12_UART2_reception_buffer[state_machine.receive_index], state_machine.receive_index, &status_response_packet))
				#else
				if(!AX12_update_status_packet(getcUART2(), state_machine.receive_index, &status_response_packet))
				#endif
				{
					state_machine.receive_index = 0;	//si le paquet n'est pas valide, on reinitialise la lecture de paquet
					debug_printf("AX12: invalid packet, reinit reception\r\n");
					break;
				}
				state_machine.receive_index++;

					
				if(AX12_status_packet_is_full(status_response_packet, state_machine.receive_index))
				{
					AX12_TIMER_stop();

					#ifdef VERBOSE_MODE
						debug_printf("AX12 Rx:");
						for(i = 0; i<state_machine.receive_index; i++)
							debug_printf(" %02x", AX12_UART2_reception_buffer[i]);
						debug_printf("\r\n");
					#endif

					//pour être sur de ne pas avoir le bit 7 a 1, si l'AX12 le met a 1, on met tous les bits a 1
					if(status_response_packet.error & 0x80)
						AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = 0xFF;
					else AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.error = status_response_packet.error & 0x7F;
					AX12_on_the_robot[state_machine.current_instruction.id_servo].last_status.param = status_response_packet.param;

					#ifdef VERBOSE_MODE
						if(status_response_packet.error & AX12_ERROR_VOLTAGE)
							debug_printf("AX12 Fatal: Voltage error\r\n");
						if(status_response_packet.error & AX12_ERROR_ANGLE)
							debug_printf("AX12 Error: Angle error\r\n");
						if(status_response_packet.error & AX12_ERROR_OVERHEATING)
							debug_printf("AX12 Fatal: Overheating error\r\n");
						if(status_response_packet.error & AX12_ERROR_RANGE)
							debug_printf("AX12 Error: Range error\r\n");
						if(status_response_packet.error & AX12_ERROR_CHECKSUM)
							debug_printf("AX12 Error: Checksum error\r\n");
						if(status_response_packet.error & AX12_ERROR_OVERLOAD)
							debug_printf("AX12 Fatal: Overload error\r\n");
						if(status_response_packet.error & AX12_ERROR_INSTRUCTION)
							debug_printf("AX12 Error: Instruction error\r\n");
						if(status_response_packet.error & 0x80)
							debug_printf("AX12 Fatal: Unknown (0x80) error\r\n");
						if(status_response_packet.error)
							debug_printf("AX12 Info: Errors occured, see datasheet for more informations");
					#endif
					
					AX12_instruction_queue_next();
					state_machine.state = READY_TO_SEND;
					AX12_state_machine(NO_EVENT);
				}
			}	
			if(event == TIMEOUT)
			{
				AX12_TIMER_stop();
				AX12_instruction_queue_next();
				debug_printf("AX12: Warning: read data timed out\r\n");
				state_machine.state = READY_TO_SEND;
				AX12_state_machine(NO_EVENT);
			}
		break;
	}
}

/* Toutes les fonctions suivantes utilisent l'UART2 et sont dédiées aux servomoteurs AX-12+ */

/*	Fonction initialisant l'UART2 
	vitesse : 57600 bauds (modifiable: uart_speed)
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/

static void AX12_UART2_init(Uint32 uart_speed)
{
	static bool_e initialized = FALSE;
	Uint16 uart_brg_speed = (CLK_FREQ-8*uart_speed)/(16*uart_speed);	//equivalent a CLK_FREQ/(16*AX12_UART_BAUDRATE)-1 + 0.5 sans nombre floatant (+0.5 pour que la troncation du nombre réel donne l'arrondi
	Uint32 uart_real_speed = CLK_FREQ/(16*(uart_brg_speed+1));
	if(initialized)
		return;
	initialized = TRUE;

	Uint16 U2MODEvalue;
	Uint16 U2STAvalue;
	U2MODEvalue = UART_EN & UART_IDLE_CON &
		UART_DIS_WAKE & UART_DIS_LOOPBACK &
		UART_DIS_ABAUD & UART_NO_PAR_8BIT &
		UART_1STOPBIT;
	U2STAvalue = UART_INT_TX_BUF_EMPTY &	//l'interruption Tx est lancée quand le buffer de sortie est completement vide, ie quand toutes les données ont été envoyé, de cette façon on peut changer la direction du l'uart après une interruption Tx et un minimum d'attente de la fin de transmission
		UART_TX_PIN_NORMAL &
		UART_TX_ENABLE &
		UART_INT_RX_CHAR &
		UART_ADR_DETECT_DIS &
		UART_RX_OVERRUN_CLEAR ;

	if((uart_real_speed > uart_speed && (uart_real_speed - uart_speed) > uart_speed/34) || (uart_real_speed < uart_speed && (uart_speed - uart_real_speed) > uart_speed/34))
		debug_printf("AX12: Warning: Le taux d'erreur du baudrate dépasse 3%%\r\n");

	//debug_printf("AX12: real baudrate = %ld, goal baudrate = %ld, brg divisor value = %d\r\n", uart_real_speed, uart_speed, uart_brg_speed);

	OpenUART2(U2MODEvalue, U2STAvalue, uart_brg_speed);
	ConfigIntUART2(UART_RX_INT_DIS & UART_RX_INT_PR5 &
		UART_TX_INT_DIS & UART_TX_INT_PR5);
	EnableIntU2RX;
	EnableIntU2TX;
}


//Attention ! Si on met a 0 le flag après avoir executé la machine a état,
//des interruptions peuvent être masquée
//(en gros il se peut qu'une interruption Tx arrive avant d'avoir terminée l'execution de la fonction,
//dans ce cas mettre le flag a 0 fait que l'interruption Tx n'est pas lancée)
//(car le caractère envoyé est envoyé plus vite que le retour de la fonction AX12_state_machine)
void _ISR AX12_UART2_RXInterrupt(void)
{
	AX12_UART2_RXInterrupt_flag = 0;
	AX12_state_machine(RX_INTERRUPT);
}

void _ISR AX12_UART2_TXInterrupt(void)
{
	AX12_UART2_TXInterrupt_flag = 0;
	AX12_state_machine(TX_INTERRUPT);
}

void _ISR AX12_TIMER_interrupt()
{
	AX12_state_machine(TIMEOUT);
	AX12_TIMER_interrupt_flag = 0;
}

#endif /* def USE_AX12_SERVO */
