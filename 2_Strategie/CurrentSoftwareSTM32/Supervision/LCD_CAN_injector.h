/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD__CAN_injector.c
 *	Package : Supervision
 *	Description : Affiche les messages can sur l'�cran selon leur SID
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */

#ifndef LCD_CAN_INJECTOR_H
#define	LCD_CAN_INJECTOR_H

/* Tableau des 4 messages can � afficher */
typedef struct{
	CAN_msg_t msg[4];
	Uint8 start;
}can_t;

can_t message;

/*
 * Filtre les messages qui arrivent.
 */
void LCD_incoming_can(CAN_msg_t* incomming_msg);

/*
 * Affiche sur l'�cran le message can en param�tre � la position donn�e
 */
void display_can(CAN_msg_t msg, Uint8 pos);

#endif	/* LCD_CAN_INJECTOR_H */

