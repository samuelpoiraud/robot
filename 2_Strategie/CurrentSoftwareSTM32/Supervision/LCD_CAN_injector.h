/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD__CAN_injector.c
 *	Package : Supervision
 *	Description : Affiche les messages can sur l'écran selon leur SID
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */

#ifndef LCD_CAN_INJECTOR_H
#define	LCD_CAN_INJECTOR_H


/*
 * Filtre les messages qui arrivent.
 */
void LCD_incoming_can(CAN_msg_t* incomming_msg);

/*
 * Affiche sur l'écran le message can en paramètre à la position donnée
 */
void LCD_display_CAN_msg(CAN_msg_t * msg, Uint8 line);

#endif	/* LCD_CAN_INJECTOR_H */

