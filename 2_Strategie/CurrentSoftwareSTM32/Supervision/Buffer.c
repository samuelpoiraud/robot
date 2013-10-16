/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech
 *
 *	Fichier : Buffer.c
 *	Package : Supervision
 *	Description : implémentation du buffer circulaire qui sauvegarde les messages can échangés durant le match ainsi que la base de temps
 *	Auteur : Jixi
 *	Version 20090305
 */

#define BUFFER_C
#include "Buffer.h"
#include "Verbose_can_msg.h"
#include "config_use.h"

#include "../QS/QS_outputlog.h"
#include "../QS/QS_can_over_uart.h"


typedef struct			//définition de la structure qui sera stockée dans le buffer circulaire
{
	CAN_msg_t message;
	Uint16 temps;
} CANtime_t;

typedef struct		//définition de la structure du buffer
{
	CANtime_t tab[BUFFER_SIZE];
	Uint16 indiceDebut;
	Uint16 indiceFin;
} bufferCirculaire_t;

bufferCirculaire_t buffer;

void BUFFER_init()
{
	buffer.indiceDebut = 0;
	buffer.indiceFin = 0;
}

void BUFFER_add(CAN_msg_t * m)
{
	//Enregistrement du message dans le buffer...
	(buffer.tab[buffer.indiceFin]).message = *m;
	(buffer.tab[buffer.indiceFin]).temps =  (Uint16)(global.env.match_time/2); //le timer boucle toutes les 250 ms
	buffer.indiceFin++;

	if (buffer.indiceFin >= BUFFER_SIZE-1)		//Si jamais on dépasse la taille du buffer, on écrase le dernier message reçu, BUFFER_SIZE -1 car on incrémente si la condition est vraie
		buffer.indiceFin=BUFFER_SIZE-1;
}

#ifdef USE_EEPROM_FOR_BUFFER
	#warning "module EEPROM_FOR_BUFFER activé mais non fonctionnel !"

	/*
	Plus d'infos sur l'utilisation de l'EEPROM :
	http://ww1.microchip.com/downloads/en/DeviceDoc/CE017_DataEEPROM_write_erase_functions_110907.zip
	*/
	Uint16 array_to_eeprom = {0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0xABCD, 0xBCDE,
						   0xCDEF, 0xDEFA, 0x0000, 0x1111, 0x2222, 0x3333, 0x0000, 0x0000};

	void BUFFER_write(CAN_msg_t &msg)
	{
		_prog_addressT EE_addr;

		if(buffer.indiceFin < BUFFER_RAM_SIZE)
		{
			//Ecriture en RAM
			(buffer.tab[buffer.indiceFin]).message = *msg;
			(buffer.tab[buffer.indiceFin]).temps =  (Uint16)(global.match_time/2);
		}
		else if(buffer.indiceFin < BUFFER_RAM_SIZE + BUFFER_EEPROM_SIZE)
		{
			//Ecriture en EEPROM

/*		SEULEMENT POUR INFO... instructions d'exploitation du module EEPROM
				_init_prog_address(EE_addr, fooArrayInDataEE);			  // initialize a variable to represent the Data EEPROM address
				_memcpy_p2d16(fooArray2inRAM, EE_addr, _EE_ROW);			    //Copy array "fooArrayinDataEE" from DataEEPROM to "fooArray2inRAM" in RAM
				_erase_eedata(EE_addr, _EE_ROW);//Erase a row in Data EEPROM at array "fooArrayinDataEE"
				_wait_eedata();
*/
				/*
				TODO: écrire dans les 16 octets du tableau array_to_eeprom l'ensemble des données à inscrire ensuite dans l'EEPROM
					array_to_eeprom[0] = U18FROMU8(...)

				_write_eedata_row(EE_addr, array_to_eeprom);	//Cette ligne permet l'écriture en EEPROM de 16 octets (on en a besoin de 14 seulement, mais tant pis !)
				_wait_eedata();									//attente obligatoire !
				*/
		}
	}
#endif

void BUFFER_flush()
{
	Uint16 index;
	CAN_msg_t * pmsg;
	for(index = buffer.indiceDebut; index < buffer.indiceFin; index++)
	{
		debug_printf("t=%.2d.%03ds ",buffer.tab[index].temps/500, ((buffer.tab[index].temps)%500)*2);
		pmsg = &(buffer.tab[index].message);
		VERBOSE_CAN_MSG_print(pmsg);
	}
}
