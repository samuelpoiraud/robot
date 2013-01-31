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
	Pour utiliser ce module vous devez avoir configur� vos AX-12+ de la mani�re suivante :
	
		- Baud Rate de 115200 bauds (Address 0x04)
		- Status Return Level en "Respond only to READ_DATA instructions" (Address 0X10)
		- Return Delay Time de 200 �s (temps de transfert d'un packet � 57600 bauds) (Address 0x05)
		- Les ID des AX-12+ doivent �tre diff�rents et se suivre (ID = 0 ... n)
		
		(Voir la datasheet pour g�n�rer les instructions)
	
	Vous devez d�finir d'utilisation du module AX12 (USE_AX12_SERVO).
	Vous devez d�finir une constante donnant le nombre de servos sur le m�me bus UART du robot (AX12_NUMBER).	
	Vous devez d�finir un port du PIC d�di� � la gestion de l'UART Half Duplex (DIRECTION_PORT).
	(cf. QS_config_doc.h)
	! Attention ! Ce module utilise l'UART2 du PIC, il est donc interdit de d�finir USE_UART2 ...
*/

/* API des servomoteurs Dynamixel AX-12+ */

void AX12_init();

void AX12_enable_torque_for_all();

void AX12_disable_torque_for_all();

void AX12_LED_on(Uint8 id_servo);

void AX12_LED_off(Uint8 id_servo);

Uint16 AX12_get_position(Uint8 id_servo);

/* Action qui peut �tre retard�e afin que les servomoteurs r�alisent le d�placement en m�me temps */
void AX12_set_position(Uint8 id_servo, Uint16 position, bool_e now); 

Uint16 AX12_get_speed(Uint8 id_servo);

void AX12_set_speed(Uint8 id_servo, Uint16 speed);

Uint16 AX12_get_load(Uint8 id_servo);

Uint8 AX12_get_voltage(Uint8 id_servo);

Uint8 AX12_get_temperature(Uint8 id_servo);

#endif /* def USE_AX12_SERVO */
#endif /* ndef QS_AX12_H */
