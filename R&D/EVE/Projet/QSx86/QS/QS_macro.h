/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_macro.h
 *	Package : QSx86
 *	Description : Définitions de macros
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_MACRO_H
	#define QS_MACRO_H

	#define HIGHINT(x)				(((x) >> 8) & 0xFF)
	#define LOWINT(x)				((x) & 0xFF)
	#define U16FROMU8(high,low)		((((Uint16)(high))<<8)|(Uint16)(low))
	#define U32FROMU16(high,low)		((((Uint32)(high))<<16)|(Uint32)(low))
	#define U32FROMU8(higher,high,low,lower)		((((Uint32)(higher))<<24)|(((Uint32)(high))<<16)|(((Uint32)(low))<<8)|(Uint32)(lower))
	#define BITS_ON(var, mask)		((var) |= (mask))
	/* ~0 est le complement à 1 de 0, donc pour 16 bits OxFFFF) */
	/* ~0 ^ mask permet d'etre indépendant de la taille (en bits) de ~mask */
	#define BITS_OFF(var, mask)		((var) &= ~0 ^ (mask))
	#define BIT_SET(var, bitno)		((var) |= (1 << (bitno)))
	#define BIT_CLR(var, bitno)		((var) &= ~(1 << (bitno)))
	#define BIT_TEST(data, bitno)	(((data) >> (bitno)) & 0x01)
	#define MIN(a, b)				(((a) > (b)) ? (b) : (a))
	#define MAX(a, b)				(((a) > (b)) ? (a) : (b))

	#define nop()					__asm__("nop")
	/* la fonction valeur absolue pour des entiers */
	#define abs(x)					(((x) >= 0) ? (x) : (-(x)))
	#ifdef VERBOSE_MODE
		// Nouvelle définition du debug_printf
		// On envoie un message vers l'IHM par la BAL messages UART
		// C'est simple...
		#define debug_printf(...) 	\
			if(eve_global.bal_id_card_to_ihm_uart!=-1 && eve_global.ihm_use_uart1 == TRUE)													\
			{																																\
				EVE_UART_msg_t debug_printf_to_uart_msg;																					\
				Uint16 debug_printf_size_counter, debug_printf_msg_sent = 0;																\
				/* On définit la taille du tampon en fonction des arguments */ 																\
				char uart_msg_content_buffer[MAX_SIZE_DEBUG_PRINTF];																		\
																																			\
				/* Chargement du contenu du printf dans la variable msg_content	*/															\
				sprintf(uart_msg_content_buffer,__VA_ARGS__);																				\
				/* On indique le type du message */																							\
				debug_printf_to_uart_msg.mtype = QUEUE_UART_MTYPE;																			\
				debug_printf_to_uart_msg.uart_number = 1; /* Pour UART 1 */																		\
				debug_printf_to_uart_msg.end_data = 0;																						\
				/* On envoie des messages de la taille de MAX_UART_CHARACTER pour éviter de surcharger la mémoire de la BAL */				\
				for(debug_printf_msg_sent=0;																								\
					debug_printf_msg_sent<(strlen(uart_msg_content_buffer)/MAX_UART_CHARACTER+(strlen(uart_msg_content_buffer)%MAX_UART_CHARACTER>0?1:0));	\
					debug_printf_msg_sent++)																								\
				{																															\
					/* On charge le message sur MAX_UART_CHARACTER caractères */															\
					for(debug_printf_size_counter=0;																						\
						debug_printf_size_counter<MAX_UART_CHARACTER;																		\
						debug_printf_size_counter++)																						\
					{																														\
						debug_printf_to_uart_msg.data[debug_printf_size_counter] = uart_msg_content_buffer[debug_printf_size_counter+MAX_UART_CHARACTER*debug_printf_msg_sent];		\
					}																														\
					/* On envoie le message à l'IHM via la BAL spécifiée */																	\
					EVE_write_new_msg_UART(eve_global.bal_id_card_to_ihm_uart,debug_printf_to_uart_msg);									\
				}																															\
			}																		

		#define assert(condition) \
			if(!(condition)) {debug_printf("assert failed file " __FILE__ " line %d : %s", __LINE__ , #condition );}
	#else
		#define debug_printf(...) (void)0
		#define assert(condition) (void)0
	#endif
	#define PI4096	12868
	
	#define LED_RUN  LATDbits.LATD8
	#define LED_CAN  LATDbits.LATD9
	#define LED_UART LATDbits.LATD10
	#define LED_USER LATDbits.LATD11
	#define LED_USER2 LATDbits.LATD12
	#define LED_ERROR LATDbits.LATD13
	
	// Boutons des cartes génériques (pull_up)
	#define BUTTON1_PORT	PORTDbits.RD4
	#define BUTTON2_PORT	PORTDbits.RD5
	#define BUTTON3_PORT	PORTDbits.RD6
	#define BUTTON4_PORT	PORTDbits.RD7

#endif /* ndef QS_MACRO_H */

