#ifndef SD_H
#define SD_H

#include "../../QS/QS_all.h"

void SD_process_1ms(void);
void SD_process_main(void);
void SD_init(void);
void SD_print_previous_match(void);
void SD_print_match(Uint16 nb_match);

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


void SD_new_event(source_e source, CAN_msg_t * can_msg, char * user_string, bool_e insert_time);
int printf_SD_verbose(bool_e verbose, const char *format, ...);
Uint16 SD_get_match_id();

// Gestion de l'affichage des SD_printf dans le terminal selon les rêgles d'affichage du fichier
#if LOG_COMPONENT == LOG_PRINT_Off && !defined(OUTPUTLOG_PRINT_ALL_COMPONENTS)
	#define SD_printf(...) printf_SD_verbose(FALSE, __VA_ARGS__);
#else
	#define SD_printf(...) printf_SD_verbose(TRUE, __VA_ARGS__);
#endif

#endif /* SD_H */
