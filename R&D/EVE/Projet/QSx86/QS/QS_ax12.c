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

#include "../QS/QS_ax12.h"

#ifdef USE_AX12_SERVO
																			
	/*************************************************************************************/
	#ifndef AX12_NUMBER
		#error "Vous devez definir un nombre de servo sur le robot (AX12_NUMBER)"
	#endif /* ndef AX12_NUMBER */
	
	#include "QS_watchdog.h"
	
	#ifndef USE_WATCHDOG
		#error "Vous devez activer le module WATCHDOG pour utiliser le driver AX12 (cf. QS_config_doc)"
	#endif /* ndef USE_WATCHDOG */
	
	#if (WATCHDOG_QUANTUM!=1)
		#error "La granularité du module WATCHDOG doit être de 1 ms pour utiliser le driver AX12 (cf. QS_config_doc)"
	#endif /* WATCHDOG_QUANTUM!=1 */
		
	#define MIN_INSTRUCTION_PACKET_SIZE 6
	#define MAX_INSTRUCTION_PACKET_SIZE 9
	#define MIN_STATUS_PACKET_SIZE 6
	#define MAX_STATUS_PACKET_SIZE 8
	
	#define SPECIAL_INSTRUCTION_NUMBER_MAX 4*AX12_NUMBER
	/*************************************************************************************/
	
	/* Control Table Address AX12+ */
	
	// EEPROM Area
	#define MODEL_NUMBER_L 			0x00
	#define MODOEL_NUMBER_H 		0x01
	#define VERSION 				0x02
	#define ID 						0x03
	#define BAUD_RATE 				0x04
	#define RETURN_DELAY_TIME 		0x05
	#define CW_ANGLE_LIMIT_L 		0x06
	#define CW_ANGLE_LIMIT_H 		0x07
	#define CCW_ANGLE_LIMIT_L 		0x08
	#define CCW_ANGLE_LIMIT_H 		0x09
	#define SYSTEM_DATA2 			0x0A
	#define LIMIT_TEMPERATURE 		0x0B
	#define DOWN_LIMIT_VOLTAGE 		0x0C
	#define UP_LIMIT_VOLTAGE 		0x0D
	#define MAX_TORQUE_L 			0x0E
	#define MAX_TORQUE_H 			0x0F
	#define RETURN_LEVEL 			0x10
	#define ALARM_LED 				0x11
	#define ALARM_SHUTDOWN 			0x12
	#define OPERATING_MODE 			0x13
	#define DOWN_CALIBRATION_L 		0x14
	#define DOWN_CALIBRATION_H 		0x15
	#define UP_CALIBRATION_L 		0x16
	#define UP_CALIBRATION_H 		0x17
	
	// RAM Area
	#define TORQUE_ENABLE 			0x18
	#define LED 					0x19
	#define CW_COMPLIANCE_MARGIN	0x1A
	#define CCW_COMPLIANCE_MARGIN 	0x1B
	#define CW_COMPLIANCE_SLOPE 	0x1C
	#define CCW_COMPLIANCE_SLOPE 	0x1D
	#define GOAL_POSITION_L 		0x1E
	#define GOAL_POSITION_H 		0x1F
	#define GOAL_SPEED_L 			0x20
	#define GOAL_SPEED_H 			0x21
	#define TORQUE_LIMIT_L 			0x22
	#define TORQUE_LIMIT_H 			0x23
	#define PRESENT_POSITION_L 		0x24
	#define PRESENT_POSITION_H 		0x25
	#define PRESENT_SPEED_L 		0x26
	#define PRESENT_SPEED_H 		0x27
	#define PRESENT_LOAD_L 			0x28
	#define PRESENT_LOAD_H 			0x29
	#define PRESENT_VOLTAGE 		0x2A
	#define PRESENT_TEMPERATURE 	0x2B
	#define REGISTERED_INSTRUCTION  0x2C
	#define PAUSE_TIME 				0x2D
	#define MOVING 					0x2E
	#define LOCK 					0x2F
	#define PUNCH_L 				0x30
	#define PUNCH_H 				0x31

	/* Instructions */
	
	#define INST_PING 				0x01
	#define INST_READ 				0x02
	#define INST_WRITE 				0x03
	#define INST_REG_WRITE 			0x04
	#define INST_ACTION 			0x05
	#define INST_RESET 				0x06
	#define INST_DIGITAL_RESET 		0x07
	#define INST_SYSTEM_READ 		0x0C
	#define INST_SYSTEM_WRITE 		0x0D
	#define INST_SYNC_WRITE 		0x83
	#define INST_SYNC_REG_WRITE 	0x84
	
	/*************************************************************************************/
	
	/* UART2 exclusivement dédié à la gestion des servomoteurs AX-12+ */
	
	//#include <uart.h>
	
	#ifndef DIRECTION_PORT
		#error "Vous devez definir un port de direction (DIRECTION_PORT) pour gérer l'UART qui est en Half-duplex (cf. Datasheet MAX485)"
	#endif /* ndef DIRECTION_PORT */
	
	/* Pour UART half-uplex */
	#define TX_DIRECTION 0
	#define RX_DIRECTION 1
	
	//#define AX12_UART2_RXInterrupt			_U2RXInterrupt
	//#define AX12_UART2_RXInterrupt_flag		IFS1bits.U2RXIF
	
	//#define AX12_UART2_TXInterrupt			_U2TXInterrupt
	//#define AX12_UART2_TXInterrupt_flag		IFS1bits.U2TXIF
	
	#ifdef FREQ_10MHZ
		#define UART_SPEED 10 // A 10Mhz : UART_SPEED = 10000000/(16*BAUDRATE)-1 --> 57600 bps --> 9,85
	#elif (defined FREQ_20MHZ)
		#define UART_SPEED 21
	#elif (defined FREQ_40MHZ)
		#define UART_SPEED 42
	#elif (defined FREQ_INTERNAL_CLK)
		#define UART_SPEED 8
	#endif		
	
	/*************************************************************************************/
	
typedef struct{
	Uint16 present_position;
	// Possibilité d'ajout de membres ...	
}AX12_servo_t;

typedef struct{
	Uint8 id_servo;
	Uint8 type;
	Uint8 adress;
	Uint16 param;
	Uint8 size;
}AX12_instruction_t;

typedef struct{
	AX12_instruction_t buffer[SPECIAL_INSTRUCTION_NUMBER_MAX];
	Uint8 start_index;
	Uint8 end_index;
}AX12_instruction_buffer;

typedef enum{
	NO_EVENT,
	RX_INTERRUPT,
	TX_INTERRUPT,
	TIMEOUT
}event_e;

	/*************************************************************************************/
	
// Variables globales
static Uint8 AX12_UART2_sending_buffer[MAX_INSTRUCTION_PACKET_SIZE+4]={0}; // Le +4 correspond à l'octet "poubelle" qui sert à ne pas stopper les interruptions d'envoi UART
static Uint8 AX12_UART2_reception_buffer[MAX_STATUS_PACKET_SIZE]={0};

static AX12_servo_t AX12_on_the_robot[AX12_NUMBER]={{0}};
static Uint8 current_servo=0;

static Uint16 m_speed[AX12_NUMBER]={0};
static Uint16 m_load[AX12_NUMBER]={0};
static Uint8 m_voltage[AX12_NUMBER]={0};
static Uint8 m_temperature[AX12_NUMBER]={0};

static bool_e m_speed_refreshed[AX12_NUMBER]={FALSE};
static bool_e m_load_refreshed[AX12_NUMBER]={FALSE};
static bool_e m_voltage_refreshed[AX12_NUMBER]={FALSE};
static bool_e m_temperature_refreshed[AX12_NUMBER]={FALSE};

static AX12_instruction_buffer AX12_special_instruction_buffer={{{0,0,0,0}},0,0};

// Fonctions internes au driver de l'AX12+
static bool_e checksum_ok(Uint8 status_packet_size);
static void AX12_packet_generate(Uint8 id, Uint8 inst, Uint8 adress, Uint16 param);
static void AX12_state_machine(event_e event);
static void AX12_timeout();
static void AX12_UART2_init(void);

static Uint8 wd_timeout_id=0;

void AX12_init() {
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;	
	AX12_UART2_init();
	WATCHDOG_init();
	DIRECTION_PORT=RX_DIRECTION;
	//AX12_enable_torque_for_all();
	AX12_state_machine(NO_EVENT); // Lancement de la machine d'état appelée en interruption
}

void AX12_enable_torque_for_all() {
	AX12_instruction_t inst={0xFE,INST_WRITE,TORQUE_ENABLE,1,MAX_INSTRUCTION_PACKET_SIZE-1};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
}

void AX12_disable_torque_for_all() {
	AX12_instruction_t inst={0xFE,INST_WRITE,TORQUE_ENABLE,0,MAX_INSTRUCTION_PACKET_SIZE-1};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
}

void AX12_LED_on(Uint8 id_servo) {
	AX12_instruction_t inst={id_servo,INST_WRITE,LED,1,MAX_INSTRUCTION_PACKET_SIZE-1};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
}

void AX12_LED_off(Uint8 id_servo) {
	AX12_instruction_t inst={id_servo,INST_WRITE,LED,0,MAX_INSTRUCTION_PACKET_SIZE-1};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
}

Uint16 AX12_get_position(Uint8 id_servo) {
	
	return AX12_on_the_robot[id_servo].present_position;
}

void AX12_set_position(Uint8 id_servo, Uint16 position, bool_e now) {	
	AX12_instruction_t inst={id_servo,INST_REG_WRITE,PRESENT_POSITION_L,position,MAX_INSTRUCTION_PACKET_SIZE};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
	
/*	if(now)
	{
		inst.id_servo=0xFE; // Broadcast message !
		inst.type=INST_ACTION; 	
		inst.adress=0xFF;
		inst.param=0;
		inst.size=MIN_INSTRUCTION_PACKET_SIZE;
		AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
		AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;	
	}*/
}

Uint16 AX12_get_speed(Uint8 id_servo) {
	AX12_instruction_t inst={id_servo,INST_READ,PRESENT_SPEED_L,0,MAX_INSTRUCTION_PACKET_SIZE-2};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
	while(!m_speed_refreshed[id_servo]);
	m_speed_refreshed[id_servo]=FALSE;
	
	return m_speed[id_servo];
}

void AX12_set_speed(Uint8 id_servo, Uint16 speed) {
	AX12_instruction_t inst={id_servo,INST_WRITE,PRESENT_SPEED_L,speed,MAX_INSTRUCTION_PACKET_SIZE};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
}

Uint16 AX12_get_load(Uint8 id_servo) {
	AX12_instruction_t inst={id_servo,INST_READ,PRESENT_LOAD_L,0,MAX_INSTRUCTION_PACKET_SIZE-2};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
	while(!m_load_refreshed[id_servo]);
	m_load_refreshed[id_servo]=FALSE;
	
	return m_load[id_servo];
}

Uint8 AX12_get_voltage(Uint8 id_servo) {
	AX12_instruction_t inst={id_servo,INST_READ,PRESENT_VOLTAGE,0,MAX_INSTRUCTION_PACKET_SIZE-2};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
	while(!m_voltage_refreshed[id_servo]);
	m_voltage_refreshed[id_servo]=FALSE;
	
	return m_voltage[id_servo];
}

Uint8 AX12_get_temperature(Uint8 id_servo) {
	AX12_instruction_t inst={id_servo,INST_READ,PRESENT_TEMPERATURE,0,MAX_INSTRUCTION_PACKET_SIZE-2};
	AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.end_index++]=inst;
	AX12_special_instruction_buffer.end_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
	while(!m_temperature_refreshed[id_servo]);
	m_temperature_refreshed[id_servo]=FALSE;
	
	return m_temperature[id_servo];
}

static void AX12_packet_generate(Uint8 id, Uint8 inst, Uint8 adress, Uint16 param) {
 	static Uint8 sending_index;
 	Uint8 length;
 	
 	sending_index=0;
 	AX12_UART2_sending_buffer[sending_index++]=0xFF;
 	AX12_UART2_sending_buffer[sending_index++]=0xFF;
 	AX12_UART2_sending_buffer[sending_index++]=id;
	
 	if(adress==0xFF && param==0)
 	{
	 	length=2;
 		AX12_UART2_sending_buffer[sending_index++]=length;
 		AX12_UART2_sending_buffer[sending_index++]=inst;
	}	
	else if(adress!=0xFF)
	{
		if(inst==INST_READ)
		{
			AX12_UART2_sending_buffer[sending_index++]=length;
 			AX12_UART2_sending_buffer[sending_index++]=inst;
 			AX12_UART2_sending_buffer[sending_index++]=adress; 
		}
		else if(inst!=INST_READ && (param & 0xFF00)==0)
		{
			length=3;
			AX12_UART2_sending_buffer[sending_index++]=length;
 			AX12_UART2_sending_buffer[sending_index++]=inst;
 			AX12_UART2_sending_buffer[sending_index++]=adress; 
 			AX12_UART2_sending_buffer[sending_index++]=(Uint8) param;
		}
		else
		{
			length=4;
 			AX12_UART2_sending_buffer[sending_index++]=length;
 			AX12_UART2_sending_buffer[sending_index++]=inst;
 			AX12_UART2_sending_buffer[sending_index++]=adress;
 			AX12_UART2_sending_buffer[sending_index++]=(Uint8) param >> 8;
 			AX12_UART2_sending_buffer[sending_index++]=(Uint8) param;
 		}	
	}
	 	
 	AX12_UART2_sending_buffer[sending_index++]=(Uint8) ~(id+inst+adress+length+param);
 	AX12_UART2_sending_buffer[sending_index++]=0x00; // Octet perdu à cause de l'UART half-duplex et des interruptions en transmission générées 
	AX12_UART2_sending_buffer[sending_index++]=0x00;
	AX12_UART2_sending_buffer[sending_index++]=0x00;
	AX12_UART2_sending_buffer[sending_index++]=0x00;
}

static bool_e checksum_ok(Uint8 status_packet_size) {
	Uint8 i, sum=0;
	
	for(i=2;i<status_packet_size-1;i++)
	{
		sum+=AX12_UART2_reception_buffer[i];
	}
	
	if(~(sum) == AX12_UART2_reception_buffer[status_packet_size-1])
		return TRUE;
	else
		return FALSE;
}	

static void AX12_state_machine(event_e event) {
	volatile static enum{
		READY_TO_SEND,
		SENDING,
		WAIT_ANSWER
	}state=READY_TO_SEND;
		
	static AX12_instruction_t current_instruction = {0,0,0,0};
	static Uint8 sending_index=0, receive_index=0;
	
	switch(state)
	{
		case READY_TO_SEND:
			sending_index=0;
			receive_index=0;
			state=SENDING;	
			// Choix du paquet à envoyer et début d'envoi
			if(AX12_special_instruction_buffer.start_index!=AX12_special_instruction_buffer.end_index)
			{
				current_instruction = AX12_special_instruction_buffer.buffer[AX12_special_instruction_buffer.start_index];
				if(current_instruction.size==MIN_INSTRUCTION_PACKET_SIZE)
					AX12_packet_generate(current_instruction.id_servo,current_instruction.type,0xFF,0);
				else if(current_instruction.size==MAX_INSTRUCTION_PACKET_SIZE-2)
					AX12_packet_generate(current_instruction.id_servo,current_instruction.type,current_instruction.adress,0);
				else 
					AX12_packet_generate(current_instruction.id_servo,current_instruction.type,current_instruction.adress,current_instruction.param);
				DIRECTION_PORT=TX_DIRECTION;
				putcUART2(AX12_UART2_sending_buffer[sending_index++]);
			}
			// Demande de la position
			else 
			{
				current_instruction.id_servo=current_servo;
				current_instruction.type=INST_READ;
				current_instruction.adress=PRESENT_POSITION_L;
				current_instruction.param=0;
				current_instruction.size=MAX_INSTRUCTION_PACKET_SIZE-2;
				AX12_packet_generate(current_servo,INST_READ,PRESENT_POSITION_L,0);
				DIRECTION_PORT=TX_DIRECTION;
				putcUART2(AX12_UART2_sending_buffer[sending_index++]);
			}
		break;
		case SENDING:
			if(event==TX_INTERRUPT)
			{
				// Gestion envoi paquet puis passage à WAIT_ANSWER ou retour à READY_TO_SEND avec éxécution.
				if(sending_index<current_instruction.size+4)
				{
					putcUART2(AX12_UART2_sending_buffer[sending_index++]);
					if(sending_index==current_instruction.size+1)	
						DIRECTION_PORT=RX_DIRECTION; // L'octet poubelle ne passera peut_être pas ...
				}
				else
				{
					if(current_instruction.type == INST_READ)
					{
						wd_timeout_id=WATCHDOG_create(1000,AX12_timeout);	// 2 ms min à cause de l'implémentaion de QS_watchdog !
						state=WAIT_ANSWER;
					}	
					else
					{
						AX12_special_instruction_buffer.start_index++;
						AX12_special_instruction_buffer.start_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
						state=READY_TO_SEND;
						AX12_state_machine(NO_EVENT);
					}
				}
			}
		break;
		case WAIT_ANSWER:
			if(event==RX_INTERRUPT)
			{
				// Stockage de la réponse dans un buffer, si toute la réponse alors mise à jour des variables du driver et state=READY_TO_SEND avec éxécution
				AX12_UART2_reception_buffer[receive_index++]=getcUART2();
				if(receive_index==current_instruction.size /*&& checksum_ok(current_instruction.size)*/)
				{
					WATCHDOG_stop(wd_timeout_id);
					switch(current_instruction.adress)
					{
						case PRESENT_POSITION_L:
							AX12_on_the_robot[current_servo].present_position=(AX12_UART2_reception_buffer[5]<<2)+AX12_UART2_reception_buffer[6];
							current_servo++;
							current_servo%=AX12_NUMBER;
						break;
						case PRESENT_SPEED_L:
							m_speed[current_instruction.id_servo]=(AX12_UART2_reception_buffer[5]<<2)+AX12_UART2_reception_buffer[6];
							m_speed_refreshed[current_instruction.id_servo]=TRUE;
						break;
						case PRESENT_LOAD_L:
							m_load[current_instruction.id_servo]=(AX12_UART2_reception_buffer[5]<<2)+AX12_UART2_reception_buffer[6];
							m_load_refreshed[current_instruction.id_servo]=TRUE;
						break;
						case PRESENT_VOLTAGE:
							m_voltage[current_instruction.id_servo]=AX12_UART2_reception_buffer[5];
							m_voltage_refreshed[current_instruction.id_servo]=TRUE;
						break;
						case PRESENT_TEMPERATURE:
							m_temperature[current_instruction.id_servo]=AX12_UART2_reception_buffer[5];
							m_temperature_refreshed[current_instruction.id_servo]=TRUE;
						break;
						default:
						break;	
					}
					
					if(current_instruction.adress!=PRESENT_POSITION_L) 
					{
						AX12_special_instruction_buffer.start_index++;
						AX12_special_instruction_buffer.start_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
					}
					state=READY_TO_SEND;
					AX12_state_machine(NO_EVENT);
				}
			}	
			if(event==TIMEOUT)
			{
				if(current_instruction.adress!=PRESENT_POSITION_L)
				{
					AX12_special_instruction_buffer.start_index++;
					AX12_special_instruction_buffer.start_index%=SPECIAL_INSTRUCTION_NUMBER_MAX;
				}	
				state=READY_TO_SEND;
				AX12_state_machine(NO_EVENT);	
			}
		break;
		default:
		break;
	}
}

static void AX12_timeout() {
	AX12_state_machine(TIMEOUT);
}

/* Toutes les fonctions suivantes utilisent l'UART2 et sont dédiées aux servomoteurs AX-12+ */

/*	Fonction initialisant l'UART2 
	vitesse : 57600 bauds
	bits de donnees : 8
	parite : aucune
	bit de stop : 1
	pas de controle de flux
*/

static void AX12_UART2_init(void)
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	Uint16 U2MODEvalue;
	Uint16 U2STAvalue;
	U2MODEvalue = UART_EN & UART_IDLE_CON &
		UART_DIS_WAKE & UART_DIS_LOOPBACK &
		UART_DIS_ABAUD & UART_NO_PAR_8BIT &
		UART_1STOPBIT;
	U2STAvalue = UART_INT_TX &
		UART_TX_PIN_NORMAL &
		UART_TX_ENABLE &
		UART_INT_RX_CHAR &
		UART_ADR_DETECT_DIS &
		UART_RX_OVERRUN_CLEAR ;
	
	OpenUART2(U2MODEvalue, U2STAvalue, UART_SPEED);
	ConfigIntUART2(UART_RX_INT_DIS & UART_RX_INT_PR4 &
		UART_TX_INT_DIS & UART_TX_INT_PR3);
	EnableIntU2RX;
	EnableIntU2TX;
}

void _ISR AX12_UART2_RXInterrupt(void)
{
	AX12_state_machine(RX_INTERRUPT);
	AX12_UART2_RXInterrupt_flag = 0;
}

void _ISR AX12_UART2_TXInterrupt(void)
{
	AX12_state_machine(TX_INTERRUPT);
	AX12_UART2_TXInterrupt_flag = 0;
}

#endif /* def USE_AX12_SERVO */
