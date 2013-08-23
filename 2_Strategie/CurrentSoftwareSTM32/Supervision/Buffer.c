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


void BUFFER_init()
{
	global.buffer.indiceDebut = 0;
	global.buffer.indiceFin = 0;
	global.compteur_de_secondes = 0;	//On initialise le compteur pour enregistrer le premier message
}
 
void BUFFER_add(CAN_msg_t * m)
{
	switch(m->sid)		//On trie les messages en fonction de leur sid
	{
		case 0:						//ça veut rien dire
			break;
			
		case ASSER_TELL_POSITION:	//On en veut pas
			break;
		
		case BROADCAST_POSITION_ROBOT:
			if(global.compteur_de_secondes-1000>global.compteur_de_secondes_precedent_buffer) //global.compteur_de_secondes est incrémenté toutes les 1ms ...
			{
				(global.buffer.tab[global.buffer.indiceFin]).message = *m;
				(global.buffer.tab[global.buffer.indiceFin]).temps =  (Uint16)(global.current_time_ms/2); //le timer boucle toutes les 250 ms
				global.buffer.indiceFin++;
				global.compteur_de_secondes_precedent_buffer=global.compteur_de_secondes;
			}
			break;
		//case BEACON_ADVERSARY_POSITION_IR:
		//	break;
		
		default:					//Par défaut on enregistre
			(global.buffer.tab[global.buffer.indiceFin]).message = *m;
			(global.buffer.tab[global.buffer.indiceFin]).temps =  (Uint16)(global.current_time_ms/2); //le timer boucle toutes les 250 ms
			global.buffer.indiceFin++;
			break;
		
	}

	if (global.buffer.indiceFin >= BUFFER_SIZE-1)		//Si jamais on dépasse la taille du buffer, on écrase le dernier message reçu, BUFFER_SIZE -1 car on incrémente si la condition est vraie
		global.buffer.indiceFin=BUFFER_SIZE-1;
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
		    
		if(global.buffer.indiceFin < BUFFER_RAM_SIZE)
		{
			//Ecriture en RAM
			(global.buffer.tab[global.buffer.indiceFin]).message = *msg;
			(global.buffer.tab[global.buffer.indiceFin]).temps =  (Uint16)(global.current_time_ms/2);
		}		
		else if(global.buffer.indiceFin < BUFFER_RAM_SIZE + BUFFER_EEPROM_SIZE)
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
//#warning "refaire la fonction BUFFER_flush en utilisant le Verbose_can_msg"
void BUFFER_flush()
{
	Uint16 i;
	Uint16 temps = 0xFFFF;
	Uint16 indice;
	CAN_msg_t pmsg;
	
	indice = global.buffer.indiceDebut;
	while(indice != global.buffer.indiceFin)
	{	
		// Clignotement des LEDS à chaque message !
		LED_USER=!LED_USER;
		LED_CAN=!LED_CAN;
		
		if((global.buffer.tab[indice]).temps != temps)
		{
			temps = (global.buffer.tab[indice]).temps;
			
			UART1_putc(STX);
			UART1_putc((Uint8)(temps>>8));
			UART1_putc((Uint8)temps);
			UART1_putc(ETX);	
		
			UART2_putc(STX);
			UART2_putc((Uint8)(temps>>8));
			UART2_putc((Uint8)temps);
			UART2_putc(ETX);
		}
		//tempo à l'arrache
		for(i=1;i!=0;i++);
		
		pmsg = ((global.buffer.tab[indice]).message);
		CANmsgToU1txAndU2tx (&pmsg);
		//tempo à l'arrache
		for(i=1;i!=0;i++);
		
		indice = (indice + 1) % BUFFER_SIZE;
	}
}
