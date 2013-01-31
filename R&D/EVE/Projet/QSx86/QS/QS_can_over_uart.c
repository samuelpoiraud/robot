/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_can_over_uart.c
 *	Package : QSx86
 *	Description : Fonctions d'encapsulation des messages CAN sur UART
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#include "QS_can_over_uart.h"

typedef enum
{
	HEADER=0,
	SID_MSB,
	SID_LSB,
	DATA0, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7, 
	SIZE_FIELD,
	FOOTER
}can_msg_on_char_array_fields_e;	

#define CAN_MSG_LENGTH	11
#define CreateUnRxToCANmsg(uartId)											\
bool_e u##uartId##rxToCANmsg (CAN_msg_t* dest)								\
{																			\
	/*																		\
	 *	cette fonction lit un octet dans le buffer de reception de l'uart2	\
	 *	et complète le message CAN passé en argument à partir du point où	\
	 *	elle s'est arretée à son précédent appel. Elle renvoie ensuite si	\
	 *	oui ou non elle a fini de complèter le message CAN. Elle vérifie 	\
	 *  aussi si le message est bien conforme au protocole de communication	\
	 *  (cf QS)																\
	 */																		\
	static can_msg_on_char_array_fields_e next_byte_to_read=0;				\
	Uint8 byte_read;														\
	Uint8 tmp_can_msg[CAN_MSG_LENGTH+1];									\
	Uint8 i, start;															\
																			\
	if(UART##uartId##_data_ready())											\
	{																		\
		byte_read = UART##uartId##_get_next_msg();							\
																			\
		switch (next_byte_to_read)											\
		{																	\
			case HEADER:													\
				/*ignore les octets jusqu'à avoir un debut de msg CAN*/		\
				if(byte_read != SOH) 										\
					return FALSE;											\
					break;													\
			case SID_MSB:		/*lecture du MSB du sid */					\
				dest->sid = (Uint16)byte_read <<8;							\
				break;														\
			case SID_LSB:		/*lecture du LSB du sid */					\
				dest->sid |= (Uint16)byte_read;								\
				break;														\
			case SIZE_FIELD:	/*lecture du champs size */					\
				dest->size = byte_read;										\
				break;														\
			case FOOTER:													\
				if(byte_read != EOT)										\
				{															\
					/* on n'a pas un message CAN valide */					\
																			\
					/* on linéarise le message CAN */						\
					tmp_can_msg[0]= dest->sid >> 8;							\
					tmp_can_msg[1]= dest->sid & 0xFF;						\
					for (i=0; i<8;i++)										\
						tmp_can_msg[2+i]=dest->data[i];						\
					tmp_can_msg[10]=dest->size;								\
					tmp_can_msg[CAN_MSG_LENGTH]=byte_read;					\
																			\
					/* on recherche un debut de message dans la suite d'octets */\
					for(start=0; start<(CAN_MSG_LENGTH+1); start++)			\
					{														\
						if (tmp_can_msg[start]==SOH)						\
							break;											\
					}														\
					/* si on en a trouvé un */								\
					if (tmp_can_msg[i]==SOH)								\
					{														\
						/*on recopie ce qu'on a dans le message CAN */		\
						for(i=1; (start+i)<=(CAN_MSG_LENGTH+1); i++)		\
						{													\
							switch (i)										\
							{												\
								case SID_MSB:	/*lecture du MSB du sid */	\
									dest->sid = (Uint16)tmp_can_msg[start+i] <<8;\
									break;									\
								case SID_LSB:	/*lecture du MSB du sid */	\
									dest->sid |= (Uint16)tmp_can_msg[start+i];\
									break;									\
								case SIZE_FIELD:/*lecture du champs size */	\
									dest->size = tmp_can_msg[start+i];		\
									break;									\
								default:	/*lecture d'un octet de data */	\
									dest->data[i-3] = tmp_can_msg[start+i];	\
									break;									\
							}												\
						}													\
					}														\
					/* on reprend la lecture après le dernier octet reçu */	\
					next_byte_to_read = CAN_MSG_LENGTH +1 - start;			\
					return FALSE;											\
				}															\
				else														\
				{															\
					next_byte_to_read = 0;									\
					return TRUE;											\
				}															\
				break;														\
																			\
			default:	/*lecture d'un octet de data */						\
				dest->data[next_byte_to_read - DATA0]=byte_read;			\
				break;														\
		}																	\
		next_byte_to_read++;												\
	}																		\
	return FALSE;															\
}

#define CreateCANmsgToUntx(uartId)											\
void CANmsgToU##uartId##tx (CAN_msg_t* src)									\
{																			\
																			\
}


#ifdef USE_UART1
	#include "../QS/QS_uart.h"
	#ifdef USE_UART1RXINTERRUPT
		CreateUnRxToCANmsg(1)
	#endif /* def USE_UART1RXINTERRUPT */
	#include <stdio.h>
	CreateCANmsgToUntx(1)
#endif /* def USE_UART1 */

#ifdef USE_UART2
	#include "../QS/QS_uart.h"
	#ifdef USE_UART2RXINTERRUPT
		CreateUnRxToCANmsg(2)
	#endif /* def USE_UART1RXINTERRUPT */
	#include <stdio.h>
	CreateCANmsgToUntx(2)
#endif /* def USE_UART2 */
