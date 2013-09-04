#ifndef SD_H
#define SD_H

#include "../../QS/QS_all.h"
void SD_process_1ms(void);
void SD_process_main(void);
void SD_init(void);
void SD_print_previous_match(void);

typedef enum
{
	FROM_SOFT = 0,	//Evenement envoyé par le code... (changement d'état... information...)
	FROM_BUS_CAN,
	FROM_UART1,
	FROM_UART2,
	FROM_XBEE,
	TO_BUSCAN,	//Envoyé par nous, sur le bus can
	TO_UART1,	//Envoyé par nous, sur l'uart1
	TO_UART2,	//Envoyé par nous, sur l'uart2
	TO_XBEE_BROADCAST,		//Envoyé par nous, sur le XBee, en Broadcast
	TO_XBEE_DESTINATION		//Envoyé par nous, sur le XBee, à la destination par défaut
}source_e;

void SD_new_event(source_e source, CAN_msg_t * can_msg, char * user_string);

void SD_puts(char * string);	//Enregistre à la suite du match une chaine de caractère... Utilisable par exemple pour les changements d'états.


#endif /* SD_H */
