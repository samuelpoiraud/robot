/*
 *	Club Robot ESEO 2010 - 2011
 *	???
 *
 *	Fichier : QS_ax12.h
 *	Package : Qualite Software
 *	Description : Module de gestion des servomoteurs Dynamixel AX12/AX12+
 *	Auteur : Ronan, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20101010
 */

#ifndef QS_AX12_H
#define QS_AX12_H

#include "QS_all.h"

#ifdef USE_AX12_SERVO
/*
	Pour utiliser ce module vous devez avoir configuré vos AX-12+ de la manière suivante :
	
		- Baud Rate de 115200 bauds (Address 0x04)
		- Status Return Level en "Respond only to READ_DATA instructions" (Address 0X10)
		- Return Delay Time de 200 µs (temps de transfert d'un packet à 57600 bauds) (Address 0x05)
		- Les ID des AX-12+ doivent être différents et se suivre (ID = 0 ... n)
		
		(Voir la datasheet pour générer les instructions)
	
	Vous devez définir d'utilisation du module AX12 (USE_AX12_SERVO).
	Vous devez définir une constante donnant le nombre de servos sur le même bus UART du robot (AX12_NUMBER).	
	Vous devez définir un port du PIC dédié à la gestion de l'UART Half Duplex (DIRECTION_PORT).
	(cf. QS_config_doc.h)
	! Attention ! Ce module utilise l'UART2 du PIC, il est donc interdit de définir USE_UART2 ...
*/

/* API des servomoteurs Dynamixel AX-12+ */

void AX12_init();

void AX12_enable_torque_for_all();

void AX12_disable_torque_for_all();

void AX12_LED_on(Uint8 id_servo);

void AX12_LED_off(Uint8 id_servo);

Uint16 AX12_get_position(Uint8 id_servo);

/* Action qui peut être retardée afin que les servomoteurs réalisent le déplacement en même temps */
void AX12_set_position(Uint8 id_servo, Uint16 position, bool_e now); 

Uint16 AX12_get_speed(Uint8 id_servo);

void AX12_set_speed(Uint8 id_servo, Uint16 speed);

Uint16 AX12_get_load(Uint8 id_servo);

Uint8 AX12_get_voltage(Uint8 id_servo);

Uint8 AX12_get_temperature(Uint8 id_servo);

#endif /* def USE_AX12_SERVO */
#endif /* ndef QS_AX12_H */
